#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ProjectLogger.h"
#include "Project-Based/utils/ParamParseStream.h"
#include "Project-Based/webApi/WebLabelParser.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/label/InstanceLabelFactory.h"
#include "Project-Based/webApi/WebPoiLabelFactory.h"
#include "Project-Based/webApi/WebLayerLabelFactory.h"
#include "Project-Based/webApi/WebLayerInsLabelFactory.h"
#include "Project-Based/base/SceneElementLoader.h"
#include "Project-Based/label/LabelDetailFactory.h"
#include "Project-Based/webApi/IWebLabelFactory.h"
#include "Project-Based/utils/tools/BaseStatisticsTool.h"
#include "Project-Based/utils/TweenAnimateForNode.h"
#include "Project-Based/label/func/SmoothMoveFunc.h"
#include "Project-Based/utils/AnimateWithNodeMg.h"
#include "Project-Based/utils/ParamNetImage.h"

using namespace bc;

WebLabelParser::WebLabelParser() :
	m_nCurPoiIndex(0)
{
	m_bEnable = true;
	m_pWebSinglePoiFactory = new WebPoiLabelFactory();
	m_pWebSinglePoiFactory->SetAllVisible(true);
	m_pWebSinglePoiFactory->AddFunc(FUNC_SMOOTH_MOVE);
	m_pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
	m_pWebSinglePoiFactory->SetContainerGroup(m_pClient->iGetApiGroupNode());
}

WebLabelParser::~WebLabelParser()
{
	for (std::map<int, IWebLabelFactory*>::iterator it = m_mapLabelFactory.begin(); it != m_mapLabelFactory.end(); it++)
	{
		clearFactory(it->second->GetFactory());
	}
	m_mapLabelFactory.clear();
}

void WebLabelParser::iFrameUpdate()
{
	for (std::map<int, IWebLabelFactory*>::iterator it = m_mapLabelFactory.begin(); it != m_mapLabelFactory.end(); it++)
	{
		it->second->GetFactory()->FrameUpdate();
	}
	m_pWebSinglePoiFactory->FrameUpdate();

	//统计
	bool bStatistics = false;
	for (size_t i = 0; i < m_pClient->m_vecStatisticsTool.size(); i++)
	{
		if (m_pClient->m_vecStatisticsTool[i])
		{
			bStatistics = bStatistics || m_pClient->m_vecStatisticsTool[i]->IsCanStatistics();
		}
	}
	if (bStatistics)
	{
		//图层
		{
			std::vector<CommonLabelFactory*> vecFactory;
			GetAllFactory(vecFactory);
			for (size_t i = 0; i < vecFactory.size(); i++)
			{
				if (m_pClient->m_vecStatisticLayer.empty()
					|| find(m_pClient->m_vecStatisticLayer.begin(), m_pClient->m_vecStatisticLayer.end(), to_string(vecFactory[i]->GetType())) != m_pClient->m_vecStatisticLayer.end())
				{
					std::vector<ProjectBasedSceneElement*> vecData;
					vecFactory[i]->GetCurData(vecData);
					for (size_t j = 0; j < vecData.size(); j++)
					{
						ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)vecData[j];
						std::string strKey = to_string(vecFactory[i]->GetType());
						Json::Value jExtra = {};
						jExtra["extra"] = pInfo->strExtra;
						jExtra["id"] = pInfo->strUuid;
						jExtra["name"] = pInfo->strTitle;
						for (size_t i = 0; i < m_pClient->m_vecStatisticsTool.size(); i++)
						{
							if (m_pClient->m_vecStatisticsTool[i] && m_pClient->m_vecStatisticsTool[i]->IsCanStatistics())
							{
								m_pClient->m_vecStatisticsTool[i]->AddStatisticsTask(pInfo->vLabelPos, strKey, jExtra, (INT_PTR)pInfo);
							}
						}
					}
				}
			}
		}

		//POI
		{
			if (m_pClient->m_vecStatisticLayer.empty())
			{
				CommonLabelFactory* pFactory = GetPoiFactory();
				std::vector<ProjectBasedSceneElement*> vecData;
				pFactory->GetCurData(vecData);
				for (size_t j = 0; j < vecData.size(); j++)
				{
					ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)vecData[j];
					std::string strKey = "-100";
					Json::Value jExtra = {};
					jExtra["extra"] = pInfo->strExtra;
					jExtra["id"] = pInfo->strUuid;
					jExtra["name"] = pInfo->strTitle;
					for (size_t i = 0; i < m_pClient->m_vecStatisticsTool.size(); i++)
					{
						if (m_pClient->m_vecStatisticsTool[i] && m_pClient->m_vecStatisticsTool[i]->IsCanStatistics())
						{
							m_pClient->m_vecStatisticsTool[i]->AddStatisticsTask(pInfo->vLabelPos, strKey, jExtra, (INT_PTR)pInfo);
						}
					}
				}
			}
		}
	}

	//做分摊处理，放置数据量大卡住主线程,每帧处理1000个
	int nCount = 0;
	int nMaxCount = 1000;	
	while (!m_queueTaskInfo.empty())
	{
		TaskInfo* pTask = m_queueTaskInfo.front();
		bool bContinue = false;
		if (pTask->eTaskType == WebLabelParser::TASK_POI_ADD)
		{
			bContinue = addPoiElement(pTask, &pTask->tWebCommandData, nCount, nMaxCount);
		}
		else if (pTask->eTaskType == WebLabelParser::TASK_POI_UPDATE)
		{
			bContinue = updatePoiElement(pTask, &pTask->tWebCommandData, nCount, nMaxCount);
		}
		else if (pTask->eTaskType == WebLabelParser::TASK_LAYER_ADD)
		{
			bContinue = addLayerElement(pTask, &pTask->tWebCommandData, nCount, nMaxCount);
		}
		else if (pTask->eTaskType == WebLabelParser::TASK_LAYER_UPDATE)
		{
			bContinue = updateLayerElement(pTask, &pTask->tWebCommandData, nCount, nMaxCount);
		}
		if (!bContinue)
		{
			break;
		}
		m_queueTaskInfo.pop();
		DELETE_PTR(pTask);
	}
}

EventReturnType_e WebLabelParser::iProcessEvent(const BCEvent& tEvent)
{
	for (std::map<int, IWebLabelFactory*>::iterator it = m_mapLabelFactory.begin(); it != m_mapLabelFactory.end(); it++)
	{
		it->second->GetFactory()->iProcessEvent(tEvent);
	}
	m_pWebSinglePoiFactory->iProcessEvent(tEvent);

	return EventReturnType_e::NONE;
}

NodeParamBase* WebLabelParser::ParseNodeParam(std::string strJson)
{
	NodeParamBase* pNodeParam = nullptr;
	Json::Value root;
	ParseJsonByString(strJson, root);
	if (!root.isNull())
	{
		try
		{
			pNodeParam = ParseNodeParam(root);
		}
		catch (const std::exception&)
		{
		}
	}
	return pNodeParam;
}

NodeParamBase* WebLabelParser::ParseNodeParam(Json::Value& jValue)
{
	NodeParamBase* pParam = nullptr;
	std::string strNodeType = jValue["strNodeType"].asString();
	if (strNodeType == "effect_type")
	{
		pParam = new BoardParam;
		parseEffectParam(jValue, (BoardParam*)pParam);

		return pParam;
	}

	int nNodeType = ParamParseStream::GetInstance()->GetNodeTypeByName(strNodeType.c_str());
	if (nNodeType == NODE_BOARD)
	{
		pParam = new BoardParam;
		parseBoardParam(jValue, (BoardParam*)pParam);
	}
	else if (nNodeType == NODE_FONTBOARD)
	{
		pParam = new FontBoardParam;
		parseFontBoardParam(jValue, (FontBoardParam*)pParam);
	}
	else if (nNodeType == NODE_VIDEO_BOARD)
	{
		pParam = new VideoBoardParam;
		parseVideoBoardParam(jValue, (VideoBoardParam*)pParam);
	}
	else if (nNodeType == NODE_FONTBOARD_WITH_BUTTON)
	{
		pParam = new FontBoardWithButtonParam;
		parseFontBoardWithButtonParam(jValue, (FontBoardWithButtonParam*)pParam);
	}
	else if (nNodeType == NODE_VIDEO_BOARD_WITH_BUTTON)
	{
		pParam = new VideoBoardWithButtonParam;
		parseVideoBoardWithButtonParam(jValue, (VideoBoardWithButtonParam*)pParam);
	}
	else if (nNodeType == NODE_BOARD_WITH_FONTBOARD)
	{
		pParam = new BoardWithFontBoardParam;
		parseBoardWithFontBoardParam(jValue, (BoardWithFontBoardParam*)pParam);
	}
	else if (nNodeType == NODE_POLYGON)
	{
		pParam = new PolygonParam;
		parsePolygonParam(jValue, (PolygonParam*)pParam);
	}
	else if (nNodeType == NODE_ROAD)
	{
		pParam = new RoadParam;
		parseRoadParam(jValue, (RoadParam*)pParam);
	}
	else if (nNodeType == NODE_BOARD_INSTANCE)
	{
		pParam = new BoardInstanceParam;
		parseBoardInstanceParam(jValue, (BoardInstanceParam*)pParam);
	}
	else if (nNodeType == NODE_LOCUS_LINE)
	{
		pParam = new LocusLineParam;
		parseLocusLineParam(jValue, (LocusLineParam*)pParam);
	}
	else if (nNodeType == NODE_FLYLINE)
	{
		pParam = new FlyLineParam;
		parseFlyLineParam(jValue, (FlyLineParam*)pParam);
	}

	return pParam;
}

Vector2 WebLabelParser::ParseVector2(Json::Value& jValue)
{
	Json::Value& value = jValue["values"];
	Vector2 v = Vector2(value[0].asFloat(), value[1].asFloat());
	return v;
}

Vector3d WebLabelParser::ParseVector3(Json::Value& jValue)
{
	Json::Value& value = jValue["values"];
	Vector3d v = Vector3d(value[0].asFloat(), value[1].asFloat(), value[2].asFloat());
	return v;
}

Vector4 WebLabelParser::ParseColor(Json::Value& jValue)
{
	Json::Value& value = jValue["values"];
	Vector4 v = Vector4(value[0].asFloat() / 255.0f, value[1].asFloat() / 255.0f, value[2].asFloat() / 255.0f, value[3].asFloat());
	return v;
}

void WebLabelParser::parseBaseParam(Json::Value& jValue, NodeParamBase* pParam)
{
	if (!pParam)
	{
		return;
	}
	pParam->strFlagName = jValue["strFlagName"].asString();
}

void WebLabelParser::parseBoardParam(Json::Value& jValue, BoardParam* pParam)
{
	if (!pParam)
	{
		return;
	}
	parseBaseParam(jValue, pParam);

	pParam->bDepthTest = jValue["bDepthTest"].asBool();
	pParam->vBoardCenter = ParseVector2(jValue["vBoardCenter"]);
	pParam->bDepthClamp = jValue["bDepthClamp"].asBool();
	pParam->fWidth = jValue["fWidth"].asFloat();
	pParam->fHeight = jValue["fHeight"].asFloat();
	pParam->strImage = jValue["strImage"].asString();
	pParam->strHoverImage = jValue["strHoverImage"].asString();
	pParam->strCheckedImage = jValue["strCheckedImage"].asString();
	pParam->direction = jValue["direction"].asInt();
	pParam->bClickEnable = jValue["bClickEnable"].asBool();
	pParam->bHoverEnable = jValue["bHoverEnable"].asBool();
	pParam->bCheckEnable = jValue["bCheckEnable"].asBool();
	pParam->bCheckEnable = false;	//不使用自带的状态切换，业务来处理
	pParam->bDynamicScaleEnable = jValue["bDynamicScaleEnable"].asBool();
	pParam->fScale = jValue["fScale"].asFloat();
	pParam->fDynamicScale = pParam->fScale;
	pParam->fDynamicMaxScale = jValue["fDynamicMaxScale"].asFloat();
	pParam->fDynamicMinScale = jValue["fDynamicMinScale"].asFloat();
	pParam->fVisibleDistance = jValue["fVisibleDistance"].asFloat();
	pParam->fHideLessDistance = jValue["fHideLessDistance"].asFloat();
	pParam->bGlowEnable = jValue["bGlowEnable"].asBool();
	pParam->fGlowIntensity = jValue["fGlowIntensity"].asFloat();
	pParam->bRotateAroundTarget = jValue["bRotateAroundTarget"].asBool();
	pParam->vDistanceToTarget = ParseVector2(jValue["vDistanceToTarget"]);
	pParam->bWithLine = jValue["bWithLine"].asBool();
	pParam->vLineColor = ParseColor(jValue["vLineColor"]);
	pParam->vHoverLineColor = ParseColor(jValue["vHoverLineColor"]);
	pParam->vCheckedLineColor = ParseColor(jValue["vCheckedLineColor"]);
	pParam->fLineWidth = jValue["fLineWidth"].asFloat();
	pParam->bLinePoint = jValue["bLinePoint"].asBool();
	pParam->vTopLeftOffset = ParseVector2(jValue["vTopLeftOffset"]);

	if (jValue.isMember("strModelUrl"))
	{
		pParam->strModelName = jValue["strModelUrl"].asString();
	}
	if (jValue.isMember("vModelAngle"))
	{
		pParam->vModelAngle = ParseVector3(jValue["vModelAngle"]);
	}
	if (jValue.isMember("vModelScale"))
	{
		pParam->vModelScale = ParseVector3(jValue["vModelScale"]);
	}
	if (!pParam->strModelName.empty())
	{
		pParam->bCreateModel = true;
	}

	if (jValue.isMember("eTransparentType"))
	{
		pParam->eTransparentType = (TransparentType_e)jValue["eTransparentType"].asInt();
	}
	if (jValue.isMember("bNeedClarity"))
	{
		pParam->bNeedClarity = jValue["bNeedClarity"].asBool();
	}
	if (jValue.isMember("fAlphaThreshold"))
	{
		pParam->fAlphaThreshold = jValue["fAlphaThreshold"].asFloat();
	}
}

void WebLabelParser::parseTextShow(Json::Value& jValue, FontBoardParam* pParam)
{
	if (!pParam)
	{
		return;
	}
	for (int i = 0; i < jValue.size(); i++)
	{
		TextShow sTextShow;
		sTextShow.vTextPos = ParseVector2(jValue[i]["vTextPos"]);
		sTextShow.strText = jValue[i]["strText"].asString();
		sTextShow.vTextSize = ParseVector2(jValue[i]["vTextSize"]);
		sTextShow.vColor = ParseColor(jValue[i]["vColor"]);
		sTextShow.strID = jValue[i]["strID"].asString();
		sTextShow.bAdjustWidth = jValue[i]["bAdjustWidth"].asBool();
		sTextShow.bWidthCenter = jValue[i]["bWidthCenter"].asBool();
		sTextShow.bHeightCenter = jValue[i]["bHeightCenter"].asBool();
		sTextShow.strDescription = jValue[i]["strDescription"].asString();
		sTextShow.nLineMaxLength = jValue[i]["nLineMaxLength"].asInt();
		sTextShow.nMaxLine = jValue[i]["nMaxLine"].asInt();

		pParam->vecText.push_back(sTextShow);
	}
}

void WebLabelParser::parseFontBoardParam(Json::Value& jValue, FontBoardParam* pParam)
{
	if (!pParam)
	{
		return;
	}

	parseBoardParam(jValue, pParam);
	parseTextShow(jValue["vecText"], pParam);
}

void WebLabelParser::parseVideoBoardParam(Json::Value& jValue, VideoBoardParam* pParam)
{
	if (!pParam)
	{
		return;
	}
	parseBoardParam(jValue, pParam);

	pParam->bPlayVideo = jValue["bPlayVideo"].asBool();
	pParam->bTimeStamp = jValue["bTimeStamp"].asBool();
	pParam->strLiveUrl = jValue["strLiveUrl"].asString();
	pParam->strDeviceID = jValue["strDeviceID"].asString();
	pParam->strChannelID = jValue["strChannelID"].asString();
	pParam->bLoop = jValue["bLoop"].asBool();
	pParam->bAutoReconnect = jValue["bAutoReconnect"].asBool();
	pParam->fReconnectTime = jValue["fReconnectTime"].asFloat();
}

void WebLabelParser::parseButtonBoardParam(Json::Value& jValue, ButtonBoardParam* pParam)
{
	if (!pParam)
	{
		return;
	}
	parseFontBoardParam(jValue, pParam);
}

void WebLabelParser::parseFontBoardWithButtonParam(Json::Value& jValue, FontBoardWithButtonParam* pParam)
{
	if (!pParam)
	{
		return;
	}
	parseFontBoardParam(jValue, pParam);

	Json::Value& items = jValue["vecBoardParam"];
	for (int i = 0; i < items.size(); i++)
	{
		ButtonBoardParam sParam;
		parseButtonBoardParam(items[i], &sParam);

		pParam->vecBoardParam.push_back(sParam);
	}
}

void WebLabelParser::parseVideoBoardWithButtonParam(Json::Value& jValue, VideoBoardWithButtonParam* pParam)
{
	if (!pParam)
	{
		return;
	}
	parseFontBoardWithButtonParam(jValue, pParam);

	Json::Value& items = jValue["vecVideoParam"];
	for (int i = 0; i < items.size(); i++)
	{
		VideoBoardParam sParam;
		parseVideoBoardParam(items[i], &sParam);

		pParam->vecVideoParam.push_back(sParam);
	}
}

void WebLabelParser::parseBoardWithFontBoardParam(Json::Value& jValue, BoardWithFontBoardParam* pParam)
{
	if (!pParam)
	{
		return;
	}

	parseBoardParam(jValue, pParam);

	pParam->fFontBoardOffset = jValue["fFontBoardOffset"].asFloat();
	pParam->nFontLocation = (BoardWithFontBoardParam::BoardWithFontBoardLocation)jValue["nFontLocation"].asInt();
	parseFontBoardParam(jValue["tFontBoardParam"], &pParam->tFontBoardParam);
}

void WebLabelParser::parsePolygonParam(Json::Value& jValue, PolygonParam* pParam)
{
	if (!pParam)
	{
		return;
	}
	parseBaseParam(jValue, pParam);

	pParam->strBaseImage = jValue["strBaseImage"].asString();
	pParam->strHoverBaseImage = jValue["strHoverBaseImage"].asString();
	pParam->bBaseGlowEnable = jValue["bBaseGlowEnable"].asBool();
	pParam->fBaseGlowIntensity = jValue["fBaseGlowIntensity"].asFloat();
	pParam->fUVRepeat = jValue["fUVRepeat"].asFloat();
	pParam->vBaseColor = ParseColor(jValue["vBaseColor"]);
	pParam->vSelectColor = ParseColor(jValue["vSelectColor"]);
	pParam->vHoverColor = ParseColor(jValue["vHoverColor"]);
	pParam->bClickEnable = jValue["bClickEnable"].asBool();
	pParam->bHoverEnable = jValue["bHoverEnable"].asBool();
	pParam->fBaseHeight = jValue["fBaseHeight"].asFloat();
	pParam->bWithLine = jValue["bWithLine"].asBool();
	pParam->fLineWidth = jValue["fLineWidth"].asFloat();
	pParam->vLineColor = ParseColor(jValue["vLineColor"]);
	pParam->vHoverLineColor = ParseColor(jValue["vHoverLineColor"]);
	pParam->bDepthTest = jValue["bDepthTest"].asBool();
	pParam->bWithFence = jValue["bWithFence"].asBool();
	pParam->fFenceHeight = jValue["fFenceHeight"].asFloat();
	pParam->strFenceImage = jValue["strFenceImage"].asString();
	pParam->strHoverFenceImage = jValue["strHoverFenceImage"].asString();
	pParam->bFenceGlowEnable = jValue["bFenceGlowEnable"].asBool();
	pParam->fFenceGlowIntensity = jValue["fFenceGlowIntensity"].asFloat();
	pParam->vFenceColor = ParseColor(jValue["vFenceColor"]);
	pParam->vHoverFenceColor = ParseColor(jValue["vHoverFenceColor"]);
	if (pParam->vBaseColor.a > 0 || (pParam->bHoverEnable && pParam->vHoverColor.a > 0) ||
		!pParam->strBaseImage.empty() || (pParam->bHoverEnable && !pParam->strHoverBaseImage.empty()))
	{
		pParam->bCalculateCenter = true;
	}
}

void WebLabelParser::parseRoadParam(Json::Value& jValue, RoadParam* pParam)
{
	if (!pParam)
	{
		return;
	}
	parseBaseParam(jValue, pParam);

	pParam->strImage = jValue["strImage"].asString();
	pParam->vecWidth.push_back(jValue["fWidth"].asFloat());
	pParam->vecStep.push_back(jValue["fStep"].asFloat());
	pParam->nCalcHeightType = jValue["nCalcHeightType"].asInt();
	pParam->bGlow = jValue["bGlow"].asBool();
	pParam->fGlowIntensity = jValue["fGlowIntensity"].asFloat();
	pParam->bGlowFlash = jValue["bGlowFlash"].asBool();
	pParam->fGlowFlashSpeed = jValue["fGlowFlashSpeed"].asFloat();
	pParam->bAnimation = jValue["bAnimation"].asBool();
	pParam->fAnimationSpeed = jValue["fAnimationSpeed"].asFloat();
	pParam->bDepthTest = jValue["bDepthTest"].asBool();
	pParam->bIntersectEnable = jValue["bIntersectEnable"].asBool();
}

void WebLabelParser::parseBoardInstanceParam(Json::Value& jValue, BoardInstanceParam* pParam)
{
	if (!pParam)
	{
		return;
	}

	parseBoardParam(jValue, pParam);
}

void WebLabelParser::parseLocusLineParam(Json::Value& jValue, LocusLineParam* pParam)
{
	if (!pParam)
	{
		return;
	}

	parseBaseParam(jValue, pParam);

	Json::Value jSegmentArray = jValue["vecSegment"];
	for (int i = 0; i < jSegmentArray.size(); i++)
	{
		LocusLineSegment sSegment;
		sSegment.vColor = ParseColor(jSegmentArray[i]["vColor"]);
		sSegment.fWidth = jSegmentArray[i]["fWidth"].asFloat();
		sSegment.strID = jSegmentArray[i]["strID"].asString();
		pParam->vecSegment.push_back(sSegment);
	}

	pParam->bDepthTest = jValue["bDepthTest"].asBool();
	pParam->bGlowEnable = jValue["bGlowEnable"].asBool();
}

void WebLabelParser::parseFlyLineParam(Json::Value& jValue, FlyLineParam* pParam)
{
	if (!pParam)
	{
		return;
	}

	parseBaseParam(jValue, pParam);

	pParam->strMaterial = jValue["strMaterial"].asString();
	pParam->nTubularSegment = jValue["nTubularSegment"].asInt();
	pParam->nRadialSegment = jValue["nRadialSegment"].asInt();
	pParam->fRadius = jValue["fRadius"].asFloat();
	pParam->bAnimation = jValue["bAnimation"].asBool();
	pParam->fAnimationSpeed = jValue["fAnimationSpeed"].asFloat();
	pParam->bGlowEnable = jValue["bGlowEnable"].asBool();
	pParam->fGlowIntensity = jValue["fGlowIntensity"].asFloat();
}

void WebLabelParser::parseEffectParam(Json::Value& jValue, BoardParam* pParam)
{
	std::string strKey = "";
	strKey = "effectType";
	if (jValue.isMember(strKey))
	{
		std::string strEffectType = jValue[strKey].asString();
		std::string strStyleFilePath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strResourceDir;
		strStyleFilePath += "/extra/api";
		if (strEffectType == EFFECT_TYPE_CONE_TRIANGLE)
		{
			//三角锥形
			strStyleFilePath += "/style/style_effect_cone_triangle.json";
			pParam->vBoardCenter = Vector2(0, 0.5f);
		}
		else if (strEffectType == EFFECT_TYPE_CONE_ROUND)
		{
			//园锥形
			strStyleFilePath += "/style/style_effect_cone_round.json";
			pParam->vBoardCenter = Vector2(0, 0.5f);
		}

		if (!ParamParseStream::GetInstance()->IsContainKey(strEffectType))
		{
			std::string strData = ReadFile(strStyleFilePath);
			ParamParseStream::GetInstance()->ReadParamFromString(strData, strEffectType);
		}
		if (ParamParseStream::GetInstance()->IsContainKey(strEffectType))
		{
			ReadParam* pReadParam = ParamParseStream::GetInstance()->GetReadParamByKey(strEffectType);
			BoardParam* pTemp = (BoardParam*)pReadParam->pParam;
			*pParam = *pTemp;
		}
		pParam->strFlagName = strEffectType;

		if (pParam->strImage.empty())
		{
			pParam->bUseModelMaterial = true;
		}
		pParam->bCreateModel = true;
		pParam->direction = BoardDirect::NormalDirect;
	}
	strKey = "fScale";
	if (jValue.isMember(strKey))
	{
		pParam->fScale *= jValue[strKey].asFloat();
		pParam->fDynamicScale *= jValue[strKey].asFloat();

		pParam->tBaseParam.fScale *= jValue[strKey].asFloat();
	}
}

IWebLabelFactory* WebLabelParser::iCreateFactory(int nLabelType, NodeType_e eNodeType, bool bDetail)
{
	IWebLabelFactory* pFactory = nullptr;
	if (eNodeType == NODE_BOARD_INSTANCE)
	{
		pFactory = new WebLayerInsLabelFactory();
	}
	else
	{
		pFactory = new WebLayerLabelFactory();
	}
	return pFactory;
}

void WebLabelParser::parseData(int nType, Json::Value& jValue, ProjectBasedWebLabelData* pInfo, NodeParamBase* pParam)
{
	try
	{
		pInfo->nTypeID = nType;
		std::string strDataType = jValue["strDataType"].asString();
		pInfo->strUuid = jValue["strID"].asString();
		if (pInfo->strUuid.empty())
		{
			Guid::CreateGuId(pInfo->strUuid, "web_");
		}
		pInfo->strTitle = jValue["strName"].asString();
		pInfo->strExtra = jValue["strExtraData"].asString();
		pInfo->bWGS84 = jValue["bWGS84"].asBool();
		pInfo->bUseHeightTif = jValue["bUseHeightTif"].asBool();
		pInfo->j3DTextMatchObject = jValue["j3DTextMatchObject"];
		if (pInfo->j3DTextMatchObject.isObject() && !pInfo->j3DTextMatchObject.isMember("title"))
		{
			pInfo->j3DTextMatchObject["title"] = pInfo->strTitle;
		}
		std::string strTemp = pInfo->j3DTextMatchObject.toStyledString();
		int nLength = strTemp.length();
		char* szBuffer = new char[nLength + 1];
		Utf8ToAscii(strTemp.c_str(), (nLength + 1), szBuffer);
		std::string str = szBuffer;
		ParseJsonByString(str, pInfo->j3DTextMatchObject);

		if (strDataType.find("SingleData") != std::string::npos)
		{
			Vector3d vPos = ParseVector3(jValue["vPos"]);
			if (pInfo->bWGS84)
			{
				m_pClient->ConvertLngToVector3(vPos.x, vPos.y, &vPos, pInfo->bUseHeightTif);
			}
			pInfo->vLabelPos = vPos;
			pInfo->vAngle = ParseVector3(jValue["vAngle"]);
			pInfo->fSize = jValue["fSize"].asFloat();
		}
		if (strDataType.find("MultiData") != std::string::npos)
		{
			Vector3d vLabelPos;
			for (int j = 0; j < jValue["vecPos"].size(); j++)
			{
				Vector3d vPos = ParseVector3(jValue["vecPos"][j]);
				if (pInfo->bWGS84)
				{
					m_pClient->ConvertLngToVector3(vPos.x, vPos.y, &vPos, pInfo->bUseHeightTif);
				}
				vLabelPos += vPos;

				pInfo->vecPoint.push_back(vPos);
			}
			if (pInfo->vecPoint.size() > 0)
			{
				pInfo->vLabelPos = vLabelPos / pInfo->vecPoint.size();
			}
		}
		if (strDataType.find("CircleData") != std::string::npos)
		{
			float fRadius = jValue["fRadius"].asFloat();
			Vector3d vPos = ParseVector3(jValue["vRadiusPos"]);
			if (pInfo->bWGS84)
			{
				m_pClient->ConvertLngToVector3(vPos.x, vPos.y, &vPos, pInfo->bUseHeightTif);
			}
			std::vector<Vector3d> vecPoint;
			SplitToCircle(vPos, fRadius, vecPoint);
			pInfo->vecPoint = vecPoint;
			pInfo->vLabelPos = vPos;
		}
		if (strDataType.find("VideoData") != std::string::npos)
		{
			pInfo->bLivegbs = jValue["bLivegbs"].asBool();
			pInfo->strLiveUrl = jValue["strLiveUrl"].asString();
			pInfo->strDeviceID = jValue["strDeviceID"].asString();
			pInfo->strChannelID = jValue["strChannelID"].asString();
		}
		if (strDataType.find("LocusLineData") != std::string::npos)
		{
			if (pParam && pParam->nNodeType == NODE_LOCUS_LINE)
			{
				Json::Value jSegmentPointObject = jValue["mapPerSegmentPoint"];
				Json::Value::Members keys = jSegmentPointObject.getMemberNames();
				LocusLineParam* pLineParam = (LocusLineParam*)pParam;
				Vector3d vLabelPos;
				int nCount = 0;
				for (auto iter = keys.begin(); iter != keys.end(); iter++)
				{
					std::string strId = *iter;
					LocusLineSegment* pSegment = nullptr;
					for (size_t i = 0; i < pLineParam->vecSegment.size(); i++)
					{
						if (pLineParam->vecSegment[i].strID == strId)
						{
							pSegment = &pLineParam->vecSegment[i];
							break;
						}
					}
					if (pSegment)
					{
						std::string strrr = jSegmentPointObject.toStyledString();
						std::string strrr2 = jSegmentPointObject[strId].toStyledString();
						Json::Value jArray = jSegmentPointObject[strId];
						std::vector<Vector3d> vecPoints;
						for (int j = 0; j < jArray.size(); j++)
						{
							Vector3d vPos = ParseVector3(jArray[j]);
							if (pInfo->bWGS84)
							{
								m_pClient->ConvertLngToVector3(vPos.x, vPos.y, &vPos, pInfo->bUseHeightTif);
							}
							vecPoints.push_back(vPos);
							vLabelPos += vPos;
							nCount++;
						}
						pSegment->vecVertex = ConvertVecVector3dTo(vecPoints);
					}
				}
				if (nCount > 0)
				{
					pInfo->vLabelPos = vLabelPos / nCount;
				}
			}
		}
		pInfo->vSmoothPos = pInfo->vLabelPos;
		pInfo->vSmoothAngle = pInfo->vAngle;
		DELETE_ARRAY(szBuffer);
	}
	catch (const std::exception&)
	{
	}
}

void WebLabelParser::iDoWebCommond(WebCommandData* pWebData)
{
	try
	{
		if (pWebData->strCommand == "Web_CreateLayer")
		{
			if (pWebData->vecValues.size() >= 4)
			{
				int nType = pWebData->vecValues[0].asInt();
				Json::Value& labelStyle = pWebData->vecValues[1];
				Json::Value& labelDetailStyle = pWebData->vecValues[2];
				Json::Value& labelClusterStyle = pWebData->vecValues[3];
				std::string strLabel = labelStyle.isNull() ? "" : labelStyle.toStyledString().c_str();
				std::string strDetail = labelDetailStyle.isNull() ? "" : labelDetailStyle.toStyledString().c_str();
				std::string strCluster = labelClusterStyle.isNull() ? "" : labelClusterStyle.toStyledString().c_str();
				MD5 tMD5Label(strLabel.c_str());
				MD5 tMD5Detail(strDetail.c_str());
				MD5 tMD5Cluster(strCluster.c_str());
				tMD5Label.GenerateMD5((unsigned char*)strLabel.c_str(), strLabel.length());
				tMD5Detail.GenerateMD5((unsigned char*)strDetail.c_str(), strDetail.length());
				tMD5Cluster.GenerateMD5((unsigned char*)strCluster.c_str(), strCluster.length());

				if (m_mapLabelFactory.find(nType) != m_mapLabelFactory.end())
				{
					deleteLayer(nType, tMD5Label.ToString(), tMD5Detail.ToString(), tMD5Cluster.ToString());
				}
				IWebLabelFactory* pFactory = nullptr;
				if (m_mapLabelFactory.find(nType) == m_mapLabelFactory.end())
				{
					bool bHCNativeStyle = false;
					ProjectBasedSceneElementTypeInfo* pTypeInfo = nullptr;
					if (labelStyle.isNull())
					{
						bHCNativeStyle = true;
						if (m_pClient->m_pSceneElementLoader)
						{
							pTypeInfo = m_pClient->m_pSceneElementLoader->GetElementTypeInfoByID(nType);
							if (!pTypeInfo)
							{
								std::string strMsg = "WebLabelParser 未查找图层字典:" + to_string(nType);
								ProjectLogger::GetInstance()->InfoMessage(strMsg);
								return;
							}
						}
					}
					//面板风格
					NodeParamBase* pLabelParam = nullptr;
					if (bHCNativeStyle && pTypeInfo)
					{
						std::vector<std::string> vecStrLabelStyle = pTypeInfo->vecStrLabelStyle;
						if (!vecStrLabelStyle.empty())
						{
							std::string strLabelKey = "WebLabelParser_type_label_" + to_string(nType);
							std::string strStyle = vecStrLabelStyle[0];
							if (vecStrLabelStyle.size() > 1)
							{
								strStyle = vecStrLabelStyle[vecStrLabelStyle.size() - 1];
							}
							ParamParseStream::GetInstance()->ReadParamFromString(strStyle, strLabelKey);
							ReadParam* pReadParam = ParamParseStream::GetInstance()->GetReadParamByKey(strLabelKey);
							if (!pReadParam)
							{
								return;
							}
							pLabelParam = dynamic_cast<NodeParamBase*>(pReadParam->pParam);
						}
					}
					else
					{
						pLabelParam = ParseNodeParam(labelStyle);
					}
					if (!pLabelParam)
					{
						ProjectLogger::GetInstance()->DebugMessage("Web_CreateLabel type=" + to_string(nType) +
							",label nodeParam is null,parse error!");
						return;
					}
					//详情面板风格
					NodeParamBase* pDetailParam = nullptr;
					if (!labelDetailStyle.isNull())
					{
						pDetailParam = ParseNodeParam(labelDetailStyle);
					}
					else if (pTypeInfo)
					{
						std::string strDetailStyle = pTypeInfo->strDetailStyle;
						if (!strDetailStyle.empty())
						{
							std::string strLabelKey = "WebLabelParser_type_detail_" + to_string(nType);
							ParamParseStream::GetInstance()->ReadParamFromString(strDetailStyle, strLabelKey);
							ReadParam* pReadParam = ParamParseStream::GetInstance()->GetReadParamByKey(strLabelKey);
							pDetailParam = dynamic_cast<NodeParamBase*>(pReadParam->pParam);
						}
					}
					bool bShowDetail = pDetailParam ? true : false;
					if (labelDetailStyle.isNull())
					{
						bShowDetail = false;
					}
					if (bShowDetail && pWebData->vecValues.size() >= 5)
					{
						Json::Value& labelOption = pWebData->vecValues[4];
						if (labelOption.isMember("bShowDetail"))
						{
							bShowDetail = labelOption["bShowDetail"].asBool();
						}
						else
						{
							bShowDetail = false;
						}
					}
					pFactory = iCreateFactory(nType, (NodeType_e)pLabelParam->nNodeType, bShowDetail);
					pFactory->SetLabelParam(pLabelParam);
					pFactory->SetLabelNodeType((NodeType_e)pLabelParam->nNodeType);
					pFactory->SetLabelType(nType);
					pFactory->GetFactory()->SetType(nType);
					pFactory->SetStyleMD5(tMD5Label.ToString(), tMD5Detail.ToString(), tMD5Cluster.ToString());
					if (!pDetailParam)
					{
						ProjectLogger::GetInstance()->DebugMessage("Web_CreateLabel type=" + to_string(nType) +
							",label detail nodeParam is null,parse error!");
					}
					pFactory->SetLabelDetailParam(pDetailParam);
					//聚合风格
					NodeParamBase* pClusterParam = nullptr;
					if (!labelClusterStyle.isNull())
					{
						pClusterParam = ParseNodeParam(labelClusterStyle);
					}
					else if (pTypeInfo)
					{
						if (!pTypeInfo->strClusterStyle.empty())
						{
							std::string strLabelKey = "WebLabelParser_type_cluster_" + to_string(nType);
							ParamParseStream::GetInstance()->ReadParamFromString(pTypeInfo->strClusterStyle, strLabelKey);
							ReadParam* pReadParam = ParamParseStream::GetInstance()->GetReadParamByKey(strLabelKey);
							pClusterParam = dynamic_cast<NodeParamBase*>(pReadParam->pParam);
						}
					}
					if (!pClusterParam)
					{
						ProjectLogger::GetInstance()->DebugMessage("Web_CreateLabel type=" + to_string(nType) +
							",label cluster nodeParam is null,parse error!");
					}
					pFactory->SetLabelClusterParam(pClusterParam);
					//是否平滑移动
					bool bSmoothMove = false;
					float nSecondToIgnorePoint = 2;
					float fSpeed = 60.0f;
					if (bHCNativeStyle && pTypeInfo)
					{
						bSmoothMove = pTypeInfo->Get3DOption_Smooth_Move();
						nSecondToIgnorePoint = pTypeInfo->Get3DOption_SeondToIgnorePoint();
						fSpeed = pTypeInfo->Get3DOption_Smooth_Speed();
					}
					if (pWebData->vecValues.size() >= 5)
					{
						Json::Value& labelOption = pWebData->vecValues[4];
						pFactory->SetLabelOption(labelOption);
						if (labelOption.isMember("bSmoothMove"))
						{
							bSmoothMove = labelOption["bSmoothMove"].asBool();
						}
					}
					if (bSmoothMove)
					{
						pFactory->GetFactory()->AddFunc(FUNC_SMOOTH_MOVE);
					}

					m_mapLabelFactory[nType] = pFactory;
				}
				else
				{
					pFactory = m_mapLabelFactory[nType];
					if (pFactory->GetLabelNodeType() == NODE_POLYGON ||
						pFactory->GetLabelNodeType() == NODE_FLYLINE)
					{
						pFactory->ClearData();
					}
				}
				if (pWebData->vecValues.size() >= 5)
				{
					Json::Value& labelOption = pWebData->vecValues[4];
					pFactory->GetFactory()->SetRevertClick(labelOption["bRevertClick"].asBool());
					pFactory->GetFactory()->GetDetailFactory()->SetMaxShowCount(labelOption["nDetailCount"].asInt());
				}
				else
				{
					pFactory->GetFactory()->SetRevertClick(true);
					pFactory->GetFactory()->GetDetailFactory()->SetMaxShowCount(1);
				}

				ChangeShowLabel();
			}
		}
		else if (pWebData->strCommand == "Web_DeleteLayer")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				int nType = pWebData->vecValues[0].asInt();
				deleteLayer(nType,"","","");
			}
		}
		else if (pWebData->strCommand == "Web_ClearLayerAllElement")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				int nType = pWebData->vecValues[0].asInt();
				clearAllLayerElement(nType);
			}
		}
		else if (pWebData->strCommand == "Web_ShowLabelType")
		{
			if (!m_bEnable)
			{
				return;
			}
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strTypes = pWebData->vecValues[0].asString();
				std::vector<std::string> vecType;
				SplitStringBySpecial(strTypes, vecType, ",");
				m_vecCurShowType = vecType;

				ChangeShowLabel();

				std::string strMsg = "WebLabelParser m_vecCurShowType:" + strTypes;
				ProjectLogger::GetInstance()->InfoMessage(strMsg);
			}
		}
		else if (pWebData->strCommand == "Web_AddLayerElement")
		{
			TaskInfo* pTask = new TaskInfo;
			pTask->eTaskType = WebLabelParser::TASK_LAYER_ADD;
			pTask->tWebCommandData = *pWebData;
			m_queueTaskInfo.push(pTask);
		}
		else if (pWebData->strCommand == "Web_UpdateLayerElement")
		{
			TaskInfo* pTask = new TaskInfo;
			pTask->eTaskType = WebLabelParser::TASK_LAYER_UPDATE;
			pTask->tWebCommandData = *pWebData;
			m_queueTaskInfo.push(pTask);
		}
		else if (pWebData->strCommand == "Web_CreateElement")
		{
			TaskInfo* pTask = new TaskInfo;
			pTask->eTaskType = WebLabelParser::TASK_POI_ADD;
			pTask->tWebCommandData = *pWebData;
			m_queueTaskInfo.push(pTask);
		}
		else if (pWebData->strCommand == "Web_UpdateElement")
		{
			TaskInfo* pTask = new TaskInfo;
			pTask->eTaskType = WebLabelParser::TASK_POI_UPDATE;
			pTask->tWebCommandData = *pWebData;
			m_queueTaskInfo.push(pTask);
		}
		else if (pWebData->strCommand == "Web_ChangeCurShowLabelType")
		{
			std::string strTypes = pWebData->vecValues[0].asString();
			std::vector<std::string> vecType;
			SplitStringBySpecial(strTypes, vecType, ",");
			bool bShow = pWebData->vecValues[1].asBool();
			for (size_t i = 0; i < vecType.size(); i++)
			{
				std::vector<std::string>::iterator it = find(m_vecCurShowType.begin(), m_vecCurShowType.end(), vecType[i]);
				if (bShow)
				{
					if (it == m_vecCurShowType.end())
					{
						m_vecCurShowType.push_back(vecType[i]);
					}
				}
				else
				{
					if (it != m_vecCurShowType.end())
					{
						m_vecCurShowType.erase(it);
					}
				}
			}
			ChangeShowLabel();
		}
		else if (pWebData->strCommand == "Web_AddShpArea")
		{
			if (pWebData->vecValues.size() >= 3)
			{
				std::string strId = pWebData->vecValues[0].asString();
				std::string strShpUrl = pWebData->vecValues[1].asString();
				Json::Value labelStyle = pWebData->vecValues[2];
				NodeParamBase* pLabelParam = ParseNodeParam(labelStyle);
				if (!pLabelParam || pLabelParam->nNodeType != NODE_POLYGON)
				{
					return;
				}
				bool bLocalFile = StartWith(strShpUrl, "resource");
				ThreadRequestParam stParam;
				if (bLocalFile)
				{
					stParam.eRequestType = THREAD_REQUEST_CUSTOM;
					stParam.pThreadRuningFunc = nullptr;
					std::string strDownPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir;
					strDownPath += "/" + strShpUrl;
					stParam.strDownloadPath = strDownPath;
				}
				else
				{
					stParam.eRequestType = THREAD_REQUEST_DOWNLOAD;
					stParam.strUrl = strShpUrl;
					int nPos1 = stParam.strUrl.find_last_of("/");
					int nPos2 = stParam.strUrl.find_last_of(".");
					if (nPos2 == string::npos)
					{
						nPos2 = stParam.strUrl.length();
					}
					string strFileName = stParam.strUrl.substr(nPos1 + 1, nPos2 - nPos1 - 1);
					if (strFileName.empty())
					{
						DELETE_PTR(pLabelParam);
						return;
					}
					strFileName += ".zip";
					std::string strDownPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strResourceDir
						+ string("/shp/");
					Mkdirs(strDownPath.c_str());
					strDownPath += strFileName;
					strDownPath = Replace_all(strDownPath, "/", "\\");
					stParam.strDownloadPath = strDownPath;
				}
				stParam.AddParam("this", (INT_PTR)this);
				stParam.AddParam("nodeParam", (INT_PTR)pLabelParam);
				stParam.AddExtraJValue("id", strId);
				stParam.AddExtraJValue("web_id", pWebData->strID);
				stParam.pThreadCompleteFunc = static_callback_downshpFile;
				stParam.bThread = true;
				stParam.bRunCallbackInMainThread = true;
				ProjectBasedClient* pClient = (ProjectBasedClient*)(APIProvider::GetSystemAPI()->iProjectClientAPI);
				if (pClient)
				{
					pClient->AddHttpCommonParam(&stParam);
				}
				ThreadWrapper::GetInstance()->AddTask(stParam);
			}
		}
		else if (pWebData->strCommand == "Web_UpdateShpArea")
		{
			if (pWebData->vecValues.size() >= 3)
			{
				std::string strId = pWebData->vecValues[0].asString();
				//清除之前的
				if (m_mapShp.find(strId) != m_mapShp.end())
				{
					for (size_t i = 0; i < m_mapShp[strId].size(); ++i)
					{
						m_pWebSinglePoiFactory->DeletePoi(m_mapShp[strId][i]);
					}
				}
				m_mapShp.clear();
				//再添加
				WebCommandData stData;
				stData = *pWebData;
				stData.strCommand = "Web_AddShpArea";
				iDoWebCommond(&stData);
			}
		}
		else if (pWebData->strCommand == "Web_DeleteAllElement")
		{
			m_mapShp.clear();

			while (!m_queueTaskInfo.empty())
			{
				TaskInfo* pTaskInfo = m_queueTaskInfo.front();
				DELETE_PTR(pTaskInfo);
				m_queueTaskInfo.pop();
			}
		}
		else if (pWebData->strCommand == "Web_BufferAreaAnalysis")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				std::string strID = pWebData->vecValues[0].asString();
				m_pWebSinglePoiFactory->DeletePoi(strID);
				float fRadius = pWebData->vecValues[1].asFloat();
				Vector4 vColor = ParseColor(pWebData->vecValues[2]);
				Json::Value& jArray = pWebData->vecValues[3];
				if (!jArray.isArray())
				{
					return;
				}
				BufferParam tParam;
				tParam.eClipType = ClipType::Union;
				tParam.fRadius = fRadius;
				tParam.vFaceColor = vColor;
				for (int i = 0; i < jArray.size(); ++i)
				{
					Vector3d vPos = ParseVector3(jArray[i]);
					tParam.vecPoint.push_back(vPos);
				}
				INode* pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(tParam);
				pNode->iLoadModel();
				pNode->iSetID(strID);
				pNode->iSetVisible(true);
				m_pClient->iGetApiGroupNode()->iAddChild(pNode);
				m_pWebSinglePoiFactory->AddExtraNode(strID, pNode);
			}
		}
		else if (pWebData->strCommand == "Web_OverlayAnalysis")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				std::string strID = pWebData->vecValues[0].asString();
				m_pWebSinglePoiFactory->DeletePoi(strID);
				float fRadius = pWebData->vecValues[1].asFloat();
				Vector4 vColor = ParseColor(pWebData->vecValues[2]);
				Json::Value& jArray = pWebData->vecValues[3];
				if (!jArray.isArray())
				{
					return;
				}
				BufferParam tParam;
				tParam.eClipType = ClipType::Intersect;
				tParam.fRadius = fRadius;
				tParam.vFaceColor = vColor;
				for (int i = 0; i < jArray.size(); ++i)
				{
					Vector3d vPos = ParseVector3(jArray[i]);
					tParam.vecPoint.push_back(vPos);
				}
				INode* pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(tParam);
				pNode->iLoadModel();
				pNode->iSetID(strID);
				pNode->iSetVisible(true);
				m_pClient->iGetApiGroupNode()->iAddChild(pNode);
				m_pWebSinglePoiFactory->AddExtraNode(strID, pNode);
			}
		}
		else if (pWebData->strCommand == "Web_CartographicOutput")
		{
			ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
			std::string strScreenPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir;
			strScreenPath += "/screen";
			Mkdirs(strScreenPath.c_str());
			strScreenPath += "/CartographicOutput.png";
			remove(strScreenPath.c_str());
			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iSaveScreen(strScreenPath);

			WebCommandData tParam;
			tParam.strCommand = "inner_UploadCartographicOutput";
			tParam.vecValues.push_back(strScreenPath);
			tParam.vecValues.push_back(pWebData->strID);
			pClient->AddWebCommond(tParam);
		}
		else if (pWebData->strCommand == "inner_UploadCartographicOutput")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
				std::string strScreenPath = pWebData->vecValues[0].asString();
				if (!IsFileExist(strScreenPath))
				{
					WebCommandData tData;
					tData = *pWebData;
					pClient->AddWebCommond(tData);
					return;
				}
				//上传文件
				char szBuffer[1024] = { 0 };
				sprintf_s(szBuffer, sizeof(szBuffer), "%s/uploadFile", pClient->m_strApiHost.c_str());
				//std::string strFilePath = "C:/Users/Administrator/Desktop/a.png";
				std::string strFilePath = strScreenPath;
				ThreadRequestParam stParam;
				stParam.eRequestType = THREAD_REQUEST_UPLOAD;
				stParam.AddParam("this", (INT_PTR)this);
				stParam.AddExtraJValue("web_commond_id", pWebData->vecValues[1].asString());
				stParam.AddExtraJValue("filePath", strScreenPath);
				stParam.pThreadCompleteFunc = static_callback_cartographicOutput;
				stParam.strID = "net_CartographicOutput";
				stParam.strUrl = szBuffer;
				stParam.strUploadFilePath = strFilePath;
				stParam.strUploadFileFieldName = "file";
				stParam.bRunCallbackInMainThread = true;
				ThreadWrapper::GetInstance()->AddTask(stParam);

				////内网上传不了，先直接存放地址,拷贝文件,初验要求
				//std::string strDesPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir;
				//strDesPath += "/../../server/uploads/";
				//Mkdirs(strDesPath.c_str());
				//strDesPath += "CartographicOutput.png";
				//CopyFile(strScreenPath.c_str(), strDesPath.c_str(), false);
				//std::string strUrlPath = pClient->m_strApiHost + "/uploads/CartographicOutput.png";
				//std::string strData = "'" + strUrlPath + "'";
				//pClient->ToSendWebCommond("VS_Uploads", pWebData->vecValues[1].asString(), strData);
			}
		}
	}
	catch (const std::exception& e)
	{
		ProjectLogger::GetInstance()->DebugMessage("WebLabelParser error!");
		ProjectLogger::GetInstance()->DebugMessage(e.what());
	}

	std::vector<CommonLabelFactory*> vecFactory;
	GetAllFactory(vecFactory);
	vecFactory.push_back(m_pWebSinglePoiFactory);
	for (size_t i = 0; i < vecFactory.size(); i++)
	{
		vecFactory[i]->iDoWebCommond(pWebData);
	}
}

void WebLabelParser::clearFactory(CommonLabelFactory* pFactory)
{
	if (!pFactory)
	{
		return;
	}
	std::vector<ProjectBasedSceneElement*> vecData;
	pFactory->GetCurData(vecData);
	pFactory->RefreshZero();
	for (int i = 0; i < vecData.size(); i++)
	{
		ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)vecData[i];
		DELETE_PTR(pInfo);
	}
}

void WebLabelParser::GetAllFactory(std::vector<CommonLabelFactory*>& vecFactory)
{
	for (std::map<int, IWebLabelFactory*>::iterator it = m_mapLabelFactory.begin(); it != m_mapLabelFactory.end(); it++)
	{
		vecFactory.push_back(it->second->GetFactory());
	}
}

CommonLabelFactory* WebLabelParser::GetFactoryByType(int nLabelType)
{
	if (m_mapLabelFactory.find(nLabelType) != m_mapLabelFactory.end())
	{
		return m_mapLabelFactory[nLabelType]->GetFactory();
	}
	return nullptr;
}

void WebLabelParser::SetEnable(bool bEnable)
{
	m_bEnable = bEnable;
	ChangeShowLabel();
}

void WebLabelParser::ChangeShowLabel()
{
	{
		std::vector<CommonLabelFactory*> vecFactory;
		GetAllFactory(vecFactory);
		for (size_t i = 0; i < vecFactory.size(); i++)
		{
			if (!vecFactory[i])
			{
				continue;
			}
			if (find(m_vecCurShowType.begin(), m_vecCurShowType.end(), to_string(vecFactory[i]->GetType())) != m_vecCurShowType.end()
				&& m_bEnable)
			{
				vecFactory[i]->SetAllVisible(true);
			}
			else
			{
				vecFactory[i]->SetAllVisible(false);
			}
		}
	}
}

void WebLabelParser::deleteLayer(int nType,const std::string& strLabelStyleMD5, const std::string& strDetailStyleMD5, const std::string& strClusterStyleMD5)
{
	//删除图层
	std::map<int, IWebLabelFactory*>::iterator it = m_mapLabelFactory.find(nType);
	if (it != m_mapLabelFactory.end())
	{
		std::vector<ProjectBasedSceneElement*> vecData;
		it->second->GetFactory()->GetCurData(vecData);
		it->second->GetFactory()->RefreshZero();
		for (size_t i = 0; i < vecData.size(); i++)
		{
			ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)vecData[i];
			DELETE_PTR(pInfo);
		}
		vecData.clear();
		
		if (it->second->CheckStyleChanged(strLabelStyleMD5, strDetailStyleMD5, strClusterStyleMD5))
		{
			DELETE_PTR(it->second);
			m_mapLabelFactory.erase(it);
		}
	}
}

void WebLabelParser::clearAllLayerElement(int nType)
{
	//清空图层的所有元素
	std::map<int, IWebLabelFactory*>::iterator it = m_mapLabelFactory.find(nType);
	if (it != m_mapLabelFactory.end())
	{
		clearFactory(it->second->GetFactory());
	}
}

void WebLabelParser::static_callback_downshpFile(ThreadCallbackParam_s* pParam)
{
	WebLabelParser* pThis = (WebLabelParser*)pParam->sRequestParam.mapParams["this"];
	NodeParamBase* pNodeParam = (NodeParamBase*)pParam->sRequestParam.mapParams["nodeParam"];
	std::string strId = pParam->sRequestParam.GetExtraJValue("id").asString();
	std::string strWebId = pParam->sRequestParam.GetExtraJValue("web_id").asString();
	ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
	do
	{
		if (pParam->nResult == STATUS_SUCCESS)
		{
			std::string strShpFile;
			std::string strFilePath = pParam->sRequestParam.strDownloadPath;
			if (strFilePath.find(".zip") != std::string::npos)
			{
				std::string strUnZipPath = strFilePath + "\\..\\";
				//zip包解压
				UnZipFile(strFilePath.c_str(), strUnZipPath.c_str());
				remove(strFilePath.c_str());
				if (strShpFile.empty())
				{
					break;
				}
				strShpFile = strUnZipPath + strShpFile;
			}
			else if (strFilePath.find(".shp") != std::string::npos)
			{
				strShpFile = strFilePath;
			}
		}
		break;
	} while (1);
	DELETE_PTR(pNodeParam);
}

void WebLabelParser::static_callback_cartographicOutput(ThreadCallbackParam_s* pParam)
{
	WebLabelParser* pThis = (WebLabelParser*)pParam->sRequestParam.mapParams["this"];
	std::string strWebID = pParam->sRequestParam.GetExtraJValue("web_commond_id").asString();
	std::string strFilePath = pParam->sRequestParam.GetExtraJValue("filePath").asString();
	ProjectBasedClient* pClient = (ProjectBasedClient*)(APIProvider::GetSystemAPI()->iProjectClientAPI);
	if (pParam->nResult == STATUS_SUCCESS)
	{
		try
		{
			std::string strUrlPath = pParam->jRoot["data"].asString();
			strUrlPath = pClient->m_strApiHost + "/" + strUrlPath;
			std::string strData = "'" + strUrlPath + "'";
			pClient->ToSendWebCommond("VS_Uploads", strWebID, strData);
		}
		catch (const std::exception&)
		{
			pClient->ToSendWebCommond("VS_Uploads", strWebID, "");
		}
	}
	else
	{
		pClient->ToSendWebCommond("VS_Uploads", strWebID, "");
	}
}

void WebLabelParser::setSmoothMoveParam(ProjectBasedWebLabelData* pInfo)
{
	try
	{
		if (pInfo)
		{
			if (pInfo->jPoiOption.isNull())
			{
				return;
			}
			int nSecondCount = 2;
			float fSpeed = 50.0f;
			//持续时间
			if (pInfo->jPoiOption.isMember("nSecondToIgnorePoint")) 
			{
				nSecondCount = pInfo->jPoiOption["nSecondToIgnorePoint"].asInt();
			}
			//平移速度
			if (pInfo->jPoiOption.isMember("fSmoothSpeed"))
			{
				fSpeed = pInfo->jPoiOption["fSmoothSpeed"].asFloat();
			}
			SmoothMoveFunc* pMoveFunc = (SmoothMoveFunc*)m_pWebSinglePoiFactory->GetFunc(FUNC_SMOOTH_MOVE);
			if (pMoveFunc)
			{
				TweenAnimateForNode* pTween = pMoveFunc->GetTweenByID(pInfo->strUuid, false);
				if (pTween) 
				{
					pTween->SetMin2PointTimeSecond(nSecondCount);
					pTween->SetSpeed(fSpeed);
				}
			}
		}
	}
	catch (const std::exception&)
	{

	}
}

bool WebLabelParser::addPoiElement(TaskInfo* pTask, WebCommandData* pWebData, int& nCount, const int& nMaxCount)
{
	if (pWebData->vecValues.size() >= 3)
	{
		Json::Value labelStyle = pWebData->vecValues[0];
		Json::Value detailStyle = pWebData->vecValues[1];
		Json::Value labelData = pWebData->vecValues[2];
		NodeParamBase* pLabelParam = ParseNodeParam(labelStyle);
		if (!pLabelParam)
		{
			return true;
		}
		NodeParamBase* pDetailParam = ParseNodeParam(detailStyle);
		ProjectBasedWebLabelData* pInfo = new ProjectBasedWebLabelData;
		if (pWebData->vecValues.size() >= 4)
		{
			pInfo->jPoiOption = pWebData->vecValues[3];
			if (pInfo->jPoiOption.isMember("bSmoothMove") && pInfo->jPoiOption["bSmoothMove"].isBool())
			{
				pInfo->bCanSmooth = pInfo->jPoiOption["bSmoothMove"].asBool();
			}
		}
		parseData(-1, labelData, pInfo, pLabelParam);
		pInfo->pLabelParam = pLabelParam;
		pInfo->pDetailParam = pDetailParam;
		m_nCurPoiIndex++;
		m_pWebSinglePoiFactory->CreatePoi(pInfo);
		setSmoothMoveParam(pInfo);

		nCount++;
		if (nCount >= nMaxCount)
		{
			return false;
		}
	}
	
	return true;
}

bool WebLabelParser::updatePoiElement(TaskInfo* pTask, WebCommandData* pWebData, int& nCount, const  int& nMaxCount)
{
	if (pWebData->vecValues.size() >= 3)
	{
		Json::Value labelStyle = pWebData->vecValues[0];
		Json::Value detailStyle = pWebData->vecValues[1];
		Json::Value labelData = pWebData->vecValues[2];
		MD5 tMD5Label(labelStyle.toStyledString().c_str());
		MD5 tMD5Detail(detailStyle.toStyledString().c_str());
		tMD5Label.GenerateMD5((unsigned char*)labelStyle.toStyledString().c_str(), labelStyle.toStyledString().length());
		tMD5Detail.GenerateMD5((unsigned char*)detailStyle.toStyledString().c_str(), detailStyle.toStyledString().length());
		std::string strNodeId = labelData["strID"].asString();
		FactoryAssemble* pAssemble = m_pWebSinglePoiFactory->FindAssembleByID(strNodeId);
		ProjectBasedWebLabelData* pInfoData = nullptr;
		bool bCreate = true;
		if (pAssemble)
		{
			pInfoData = (ProjectBasedWebLabelData*)pAssemble->pData;
			if (pInfoData)
			{
				if (pInfoData->strUuid == strNodeId &&
					pInfoData->strLabelStyleMD5 == tMD5Label.ToString() &&
					pInfoData->strDetailStylrMD5 == tMD5Detail.ToString())
				{
					IBoard* pBoard = pAssemble->pLabelNode->GetDynamicComponent<IBoard>();
					bool bDelete = false;
					if (!pBoard)
					{
						bDelete = true;
					}
					if (bDelete)
					{
						m_pWebSinglePoiFactory->DeletePoi(pInfoData->strUuid);
						bCreate = true;
					}
					else
					{
						bCreate = false;
					}
				}
			}
		}
		if (bCreate)
		{
			NodeParamBase* pLabelParam = ParseNodeParam(labelStyle);
			NodeParamBase* pDetailParam = ParseNodeParam(detailStyle);
			ProjectBasedWebLabelData* pInfo = new ProjectBasedWebLabelData;
			if (pWebData->vecValues.size() >= 4)
			{
				pInfo->jPoiOption = pWebData->vecValues[3];
				if (pInfo->jPoiOption.isMember("bSmoothMove") && pInfo->jPoiOption["bSmoothMove"].isBool())
				{
					pInfo->bCanSmooth = pInfo->jPoiOption["bSmoothMove"].asBool();
				}
			}
			parseData(-1, labelData, pInfo, pLabelParam);
			pInfo->pLabelParam = pLabelParam;
			pInfo->pDetailParam = pDetailParam;
			pInfo->strLabelStyleMD5 = tMD5Label.ToString();
			pInfo->strDetailStylrMD5 = tMD5Detail.ToString();
			m_nCurPoiIndex++;
			m_pWebSinglePoiFactory->CreatePoi(pInfo);
			setSmoothMoveParam(pInfo);
		}
		else
		{
			if (pInfoData)
			{
				NodeParamBase* pLabelParam = pInfoData->pLabelParam;
				parseData(-1, labelData, pInfoData, pLabelParam);
				if (pWebData->vecValues.size() >= 4)
				{
					pInfoData->jPoiOption = pWebData->vecValues[3];
				}
				m_pWebSinglePoiFactory->UpdatePoi(pInfoData);
				setSmoothMoveParam(pInfoData);
			}
		}

		nCount++;
		if (nCount >= nMaxCount)
		{
			return false;
		}
	}

	return true;
}

bool WebLabelParser::addLayerElement(TaskInfo* pTask, WebCommandData* pWebData, int& nCount, const int& nMaxCount)
{
	if (pWebData->vecValues.size() >= 2)
	{
		int nType = pWebData->vecValues[0].asInt();
		Json::Value& dataArray = pWebData->vecValues[1];
		if (m_mapLabelFactory.find(nType) != m_mapLabelFactory.end())
		{
			IWebLabelFactory* pFactory = m_mapLabelFactory[nType];
			std::vector<ProjectBasedSceneElement*> vecAddData;
			for (int i = pTask->nCurDataIndex; i < dataArray.size(); i++)
			{
				ProjectBasedWebLabelData* pInfo = new ProjectBasedWebLabelData;
				parseData(nType, dataArray[i], pInfo, pFactory->GetLabelParam());
				vecAddData.push_back(pInfo);

				nCount++;
				if (nCount >= nMaxCount)
				{
					pTask->nCurDataIndex = i + 1;
					break;
				}
			}
			pFactory->GetFactory()->AddData(vecAddData);

			std::string strMsg = "Web_AddLayerElement add count:";
			strMsg += to_string(nType);
			strMsg += ",数量=" + to_string(vecAddData.size());
			ProjectLogger::GetInstance()->InfoMessage(strMsg);
		}

		if (nCount >= nMaxCount)
		{
			return false;
		}
	}

	return true;
}

bool WebLabelParser::updateLayerElement(TaskInfo* pTask, WebCommandData* pWebData, int& nCount, const int& nMaxCount)
{
	if (pWebData->vecValues.size() >= 2)
	{
		int nType = pWebData->vecValues[0].asInt();
		Json::Value& dataArray = pWebData->vecValues[1];
		if (m_mapLabelFactory.find(nType) != m_mapLabelFactory.end())
		{
			IWebLabelFactory* pFactory = m_mapLabelFactory[nType];
			std::vector<ProjectBasedSceneElement*> vecData;
			pFactory->GetFactory()->GetCurData(vecData);
			std::vector<ProjectBasedSceneElement*> vecAdd;
			std::vector<ProjectBasedSceneElement*> vecUpdate;
			for (int i = pTask->nCurDataIndex; i < dataArray.size(); i++)
			{
				ProjectBasedWebLabelData stInfo;
				parseData(nType, dataArray[i], &stInfo, pFactory->GetLabelParam());
				bool bFind = false;
				ProjectBasedWebLabelData* pInfo = nullptr;
				for (size_t j = 0; j < vecData.size(); j++)
				{
					pInfo = (ProjectBasedWebLabelData*)vecData[j];
					if (pInfo->strUuid == stInfo.strUuid)
					{
						if (pFactory->GetLabelNodeType() == NODE_FLYLINE
							|| pFactory->GetLabelNodeType() == NODE_POLYGON
							|| pFactory->GetLabelNodeType() == NODE_ROAD)
						{
							FactoryAssemble* pAssemble = pFactory->GetFactory()->FindAssembleByData(pInfo);
							if (pAssemble && pAssemble->pLabelNode)
							{
								ParamNetImage::GetInstance()->RemoveNetTask(pAssemble->pLabelNode);
								AnimateWithNodeMg::GetInstance()->RemoveAnimate(pAssemble->pLabelNode);
								APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&pAssemble->pLabelNode, false);
								pAssemble->pLabelNode = nullptr;
							}
						}
						*pInfo = stInfo;
						bFind = true;
						break;
					}
				}
				if (!bFind)
				{
					ProjectBasedWebLabelData* pTemp = new ProjectBasedWebLabelData;
					*pTemp = stInfo;
					vecAdd.push_back(pTemp);
				}
				else
				{
					vecUpdate.push_back(pInfo);
				}

				nCount++;
				if (nCount >= nMaxCount)
				{
					pTask->nCurDataIndex = i + 1;
					break;
				}
			}
			pFactory->GetFactory()->AddData(vecAdd);
			std::vector<FactoryAssemble*> vecAssemble;
			for (size_t i = 0; i < vecUpdate.size(); ++i)
			{
				FactoryAssemble* pAssemble = pFactory->GetFactory()->FindAssembleByData(vecUpdate[i]);
				if (pAssemble)
				{
					vecAssemble.push_back(pAssemble);
				}
			}
			pFactory->GetFactory()->UpdateData(vecAssemble);
		}

		if (nCount >= nMaxCount)
		{
			return false;
		}
	}

	return true;
}

