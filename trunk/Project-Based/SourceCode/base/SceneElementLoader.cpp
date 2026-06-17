#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ParamParseStream.h"
#include "Project-Based/utils/ProjectLogger.h"
#include "Project-Based/base/SceneElementLoader.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/redis/BCRedis.h"
#include "../label/HeightTifReader.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/label/InstanceLabelFactory.h"
#include "Project-Based/Factory/SubLabelFactory.h"
#include "Project-Based/Factory/SubInstanceLabelFactory.h"
#include "Project-Based/label/func/SmoothMoveFunc.h"
#include "Project-Based/label/func/OverTapFunc.h"
#include "Project-Based/utils/tools/BaseStatisticsTool.h"
#include "Project-Based/label/LabelDetailFactory.h"

using namespace bc;

#define PAGE_SIZE  5000
#define THREAD_QUEUE_ID  std::string("Queue_SceneElementLoader")

SceneElementLoader::SceneElementLoader() :
	m_bWriteWebSocketLog(true),
	m_bConnectedRedis(false),
	m_nPerFrameHandleLabelCount(1000),
	m_bThreadRefreshInstanceRunning(true)
{
	m_pBCRedis = new BCRedis();
	m_nSupportRequestType = REQUEST_TCP;
	m_pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;

	doThreadRefreshInstanceFactory();
}

SceneElementLoader::~SceneElementLoader()
{
	m_bThreadRefreshInstanceRunning = false;
	//清空解析的队列数据
	while (!m_queueSubscribeBuffer.empty())
	{
		SubscribeBufferData* pBuffer = m_queueSubscribeBuffer.front();
		while (!pBuffer->queueElement.empty())
		{
			ProjectBasedSceneElement* pInfo = pBuffer->queueElement.front();
			DELETE_PTR(pInfo);
			pBuffer->queueElement.pop();
		}
		DELETE_PTR(pBuffer);
		m_queueSubscribeBuffer.pop();
	}
	DELETE_PTR(m_pBCRedis);
}

void SceneElementLoader::requestElementTypeInfo(RequestEndData* pRequestEndData)
{
	try
	{
		char szBuffer[1024] = { 0 };
		sprintf_s(szBuffer, sizeof(szBuffer), "%s/api/v10/getElementStyle?projectName=%s&userToken=%s", m_strServerHost.c_str(),
			APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strToolProjectName.c_str(), m_pClient->m_sUserInfo.strUserToken.c_str());
		string strUrl = string(szBuffer);

		ThreadCallbackParam stCallback;
		ThreadRequestParam stParam;
		stParam.eRequestType = THREAD_REQUEST_API_GET;
		stParam.strUrl = strUrl;
		m_pClient->AddHttpCommonParam(&stParam);
		ThreadWrapper::GetInstance()->DoNetRequest(stParam, &stCallback);
		if (stCallback.nResult == STATUS_SUCCESS)
		{
			Json::Value root = stCallback.jRoot["data"];
			for (int i = 0; i < root.size(); i++)
			{
				ProjectBasedSceneElementTypeInfo* pInfo = new ProjectBasedSceneElementTypeInfo;
				parseElementTypeInfo(pInfo, root[i]);

				pRequestEndData->vecTempSceneTypeInfo.push_back(pInfo);
			}
		}

		for (size_t j = 0; j < pRequestEndData->vecTempSceneTypeInfo.size(); j++)
		{
			ProjectBasedSceneElementTypeInfo* pTemp = pRequestEndData->vecTempSceneTypeInfo[j];
			std::string strInnerName = pTemp->strInnerName;
			std::vector<std::string> vecNames;
			SplitStringBySpecial(strInnerName, vecNames, "_");
			if (vecNames.size() >= 2)
			{
				std::string strBaseTypeInnerName = vecNames[0];
				ProjectBasedSceneElementTypeInfo* pBaseTypeInfo = getElementTypeInfoByName(strBaseTypeInnerName, &pRequestEndData->vecTempSceneTypeInfo);
				pTemp->pBaseParent = pBaseTypeInfo;
				if (!pBaseTypeInfo)
				{
					continue;
				}

				//替换属性
				{
					Json::Value jBaseStyle;
					ParseJsonByString(pBaseTypeInfo->strOverTapStyle, jBaseStyle);
					Json::Value& jParamObject = jBaseStyle["param"];
					Json::Value jSelfStyle;
					ParseJsonByString(pTemp->strOverTapStyle, jSelfStyle);
					if (jSelfStyle.isMember("isSubStyle"))
					{
						replaceBaseSceneTypeStyle(jSelfStyle, jParamObject);
						pTemp->strOverTapStyle = jBaseStyle.toStyledString();
					}
					else
					{
						pTemp->strOverTapStyle = pBaseTypeInfo->strOverTapStyle;
					}
				}

				{
					Json::Value jBaseStyle;
					ParseJsonByString(pBaseTypeInfo->vecStrLabelStyle[0], jBaseStyle);
					Json::Value& jParamObject = jBaseStyle["param"];
					Json::Value jSelfStyle;
					ParseJsonByString(pTemp->vecStrLabelStyle[0], jSelfStyle);
					if (jSelfStyle.isMember("isSubStyle"))
					{
						replaceBaseSceneTypeStyle(jSelfStyle, jParamObject);
						pTemp->vecStrLabelStyle = pBaseTypeInfo->vecStrLabelStyle;
						pTemp->vecStrLabelStyle[0] = jBaseStyle.toStyledString();
					}
					else
					{
						pTemp->vecStrLabelStyle = pBaseTypeInfo->vecStrLabelStyle;
					}
				}

				{
					Json::Value jBaseStyle;
					ParseJsonByString(pBaseTypeInfo->strDetailStyle, jBaseStyle);
					Json::Value& jParamObject = jBaseStyle["param"];
					Json::Value jSelfStyle;
					ParseJsonByString(pTemp->strDetailStyle, jSelfStyle);
					if (jSelfStyle.isMember("isSubStyle"))
					{
						replaceBaseSceneTypeStyle(jSelfStyle, jParamObject);
						pTemp->strDetailStyle = jBaseStyle.toStyledString();
					}
					else
					{
						pTemp->strDetailStyle = pBaseTypeInfo->strDetailStyle;
					}
				}

				{
					Json::Value jBaseStyle;
					ParseJsonByString(pBaseTypeInfo->strClusterStyle, jBaseStyle);
					Json::Value& jParamObject = jBaseStyle["param"];
					Json::Value jSelfStyle;
					ParseJsonByString(pTemp->strClusterStyle, jSelfStyle);
					if (jSelfStyle.isMember("isSubStyle"))
					{
						replaceBaseSceneTypeStyle(jSelfStyle, jParamObject);
						pTemp->strClusterStyle = jBaseStyle.toStyledString();
					}
					else
					{
						pTemp->strClusterStyle = pBaseTypeInfo->strClusterStyle;
					}
				}

				{
					if (!pTemp->j3dOption.isObject())
					{
						pTemp->j3dOption = pBaseTypeInfo->j3dOption;
						std::string strOffset;
						if (pTemp->j3dOption.isObject())
						{
							strOffset = pTemp->Get3DOption_detailOffset();
						}
						if (!strOffset.empty())
						{
							sscanf_s(strOffset.c_str(), "%f,%f", &pTemp->fOffsetDetailX, &pTemp->fOffsetDetailY);
						}
					}
					if (pTemp->mapLabelDataMatch.empty())
					{
						pTemp->mapLabelDataMatch = pBaseTypeInfo->mapLabelDataMatch;
					}
					if (pTemp->mapDetailDataMatch.empty())
					{
						pTemp->mapDetailDataMatch = pBaseTypeInfo->mapDetailDataMatch;
					}
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		ProjectLogger::GetInstance()->ErrorMessage("requestElementTypeInfo," + std::string(e.what()));
	}
}

void SceneElementLoader::parseElementTypeInfo(ProjectBasedSceneElementTypeInfo* pInfo, Json::Value& jValue)
{
	if (!pInfo)
	{
		return;
	}
	try
	{
		pInfo->vecStrLabelStyle.clear();
		pInfo->mapLabelDataMatch.clear();
		pInfo->mapDetailDataMatch.clear();

		pInfo->nTypeID = jValue["id"].asInt();
		pInfo->strName = jValue["name"].asString();
		char szBuffer[1024*8] = { 0 };
		Utf8ToAscii(pInfo->strName.c_str(), sizeof(szBuffer), szBuffer);
		pInfo->strName = szBuffer;
		pInfo->nUseType = jValue["use_type"].asInt();
		pInfo->nLayerID = jValue["layer_id"].asInt();
		pInfo->strInnerName = jValue["inner_name"].asString();
		pInfo->mapLabelDataMatch.insert({ "", jValue["label_data_match"].asString() });
		pInfo->mapDetailDataMatch.insert({ "", jValue["detail_data_match"].asString() });
		if (jValue.isMember("data_map"))
		{
			std::string strDataMap = jValue["data_map"].asString();
			Json::Value jDataMap;
			ParseJsonByString(strDataMap, jDataMap);
			auto all_member = jDataMap.getMemberNames();
			for (auto member : all_member) {
				Json::Value &jItem = jDataMap[member];
				if (jItem.isObject())
				{
					if (jItem.isMember("detail_map"))
					{
						pInfo->mapDetailDataMatch.insert({ member ,(jItem["detail_map"].asString())});
					}
					if (jItem.isMember("label_map"))
					{
						pInfo->mapLabelDataMatch.insert({ member ,(jItem["label_map"].asString()) });
					}
				}
			}
		}
		if (!jValue["style"].isString())
		{
			jValue["style"] = jValue["style"].toStyledString();
		}
		string strStyle = jValue["style"].asString();
		pInfo->strDetailStyle = jValue["style_detail"].asString();
		if (jValue.isMember("style_overtap"))
		{
			std::string strTemp = jValue["style_overtap"].asString();
			Json::Value jTemp;
			ParseJsonByString(strTemp, jTemp);
			if (jTemp.isArray() && jTemp.size() > 0)
			{
				pInfo->strOverTapStyle = jTemp[0].toStyledString();
			}
			else
			{
				pInfo->strOverTapStyle = jTemp.toStyledString();
			}
			if (pInfo->strOverTapStyle.find("null") != std::string::npos)
			{
				pInfo->strOverTapStyle.clear();
			}
		}
		if (jValue.isMember("style_group"))
		{
			pInfo->strClusterStyle = jValue["style_group"].asString();
		}
		if (jValue.isMember("indoor"))
		{
			pInfo->nIndoor = jValue["indoor"].asInt();
		}
		if (jValue.isMember("option_3d"))
		{
			std::string strOption3d = jValue["option_3d"].asString();
			ParseJsonByString(strOption3d, pInfo->j3dOption);
		}
		std::string strOffset;
		if (pInfo->j3dOption.isObject())
		{
			strOffset = pInfo->Get3DOption_detailOffset();
		}
		else
		{
			strOffset = jValue["offset"].asString();
		}
		if (strOffset.empty())
		{
			if (jValue.isMember("pos_offset"))
			{
				strOffset = jValue["pos_offset"].asString();
			}
		}
		sscanf_s(strOffset.c_str(), "%f,%f", &pInfo->fOffsetDetailX, &pInfo->fOffsetDetailY);
		//解析style
		{
			Json::CharReaderBuilder jsonBuilder;
			std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
			Json::Value styleRoot;
			std::string jsError;
			if (reader->parse(strStyle.c_str(), strStyle.c_str() + strStyle.length(), &styleRoot, &jsError))
			{
				if (styleRoot.isObject() && styleRoot.isMember("isSubStyle"))
				{
					pInfo->vecStrLabelStyle.push_back(strStyle);
				}
				else
				{
					bool bArray = false;
					bArray = styleRoot.isArray() && styleRoot.size() > 1;
					if (bArray)
					{
						vector<string> vecBoardStrStyle;
						vector<string> vecOtherStrStyle;
						for (int n = 0; n < styleRoot.size(); n++)
						{
							string strSty = styleRoot[n].toStyledString();
							ReadParam stParam;
							ParamParseStream::GetInstance()->ReadJsonToParam(stParam, strSty);
							BoardParam* pParam = dynamic_cast<BoardParam*>(stParam.pParam);
							if (pParam)
							{
								vecBoardStrStyle.push_back(strSty);
							}
							else
							{
								vecOtherStrStyle.push_back(strSty);
							}
						}
						pInfo->vecStrLabelStyle.insert(pInfo->vecStrLabelStyle.end(), vecBoardStrStyle.begin(), vecBoardStrStyle.end());
						pInfo->vecStrLabelStyle.insert(pInfo->vecStrLabelStyle.end(), vecOtherStrStyle.begin(), vecOtherStrStyle.end());
					}
					else
					{
						Json::Value item;
						if (styleRoot.isObject())
						{
							strStyle = styleRoot.toStyledString();
						}
						else
						{
							strStyle = styleRoot[0].toStyledString();
						}
						ReadParam stParam;
						ParamParseStream::GetInstance()->ReadJsonToParam(stParam, strStyle);
						BoardParam* pParam = dynamic_cast<BoardParam*>(stParam.pParam);
						if (!pParam)
						{
							//不是board，不可复用
							BoardParam sBoardParam;
							sBoardParam.fWidth = 0;
							sBoardParam.fHeight = 0;
							Json::Value value;
							string strTypeName = ParamParseStream::GetInstance()->ConvertParamToJson(&sBoardParam, value);
							Json::Value jsonData;
							int nNodeType = ParamParseStream::GetInstance()->GetNodeTypeByName(strTypeName);
							jsonData["nodeTypeName"] = strTypeName;
							jsonData["nodeType"] = nNodeType;
							jsonData["param"] = value;
							pInfo->vecStrLabelStyle.push_back(jsonData.toStyledString());
							pInfo->vecStrLabelStyle.push_back(strStyle);
						}
						else
						{
							pInfo->vecStrLabelStyle.push_back(strStyle);
						}
					}
				}
			}
			else
			{
				pInfo->vecStrLabelStyle.push_back(strStyle);
			}
		}
	}
	catch (const std::exception&)
	{

	}
}

void SceneElementLoader::requestMapInfo(RequestEndData* pRequestEndData)
{
	try
	{
		char szBuffer[1024] = { 0 };
		EServerServion eType = APIProvider::GetSystemAPI()->iProtocolAPI->iGetServerVersion();
		sprintf_s(szBuffer, sizeof(szBuffer), "%s/api/v10/mapList?projectName=%s&userToken=%s", m_strServerHost.c_str(),
			APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strApplicationName.c_str(), m_pClient->m_sUserInfo.strUserToken.c_str());
		string strUrl = string(szBuffer);

		ThreadCallbackParam stCallback;
		ThreadRequestParam stParam;
		stParam.eRequestType = THREAD_REQUEST_API_GET;
		stParam.strUrl = strUrl;
		m_pClient->AddHttpCommonParam(&stParam);
		ThreadWrapper::GetInstance()->DoNetRequest(stParam, &stCallback);
		if (stCallback.nResult == STATUS_SUCCESS)
		{
			Json::Value root = stCallback.jRoot["data"];
			for (int i = 0; i < root.size(); i++)
			{
				ProjectBasedSceneElementMapInfo* pInfo = new ProjectBasedSceneElementMapInfo;
				pInfo->nMapID = root[i]["id"].asInt();
				pInfo->nPid = root[i]["pid"].asInt();
				pInfo->strUrl = root[i]["url"].asString();
				pInfo->strNodeName = root[i]["node_name"].asString();
				pInfo->nStatus = root[i]["status"].asInt();
				if (root[i].isMember("map_type"))
				{
					string strCoorType = root[i]["map_type"].asString();
					if (pInfo->strNodeName.empty())
					{
						if (strCoorType == "EGSG:4326")
						{
							pInfo->eCoorType = COOR_WGS84;
						}
					}
					else
					{
						pInfo->eCoorType = COOR_SCENE_LOCAL;
					}
				}

				pRequestEndData->vecTempMapInfo.push_back(pInfo);
			}
		}
	}
	catch (const std::exception&)
	{
	}
}

void SceneElementLoader::iRequestData(bool bThread)
{
	requestBaseInfo(bThread, true);
}

void SceneElementLoader::iRefreshData(bool bThread)
{
	requestBaseInfo(bThread,true);
}

void SceneElementLoader::requestBaseInfo(bool bThread, bool bToWebCommand)
{
	std::string strID = "requestElementBaseInfo";
	ThreadWrapper::GetInstance()->StopNetTask(strID);

	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_CUSTOM;
	stParam.pThreadRuningFunc = static_request_BaseInfo;
	stParam.pThreadCompleteFunc = static_callback_BaseInfo;
	stParam.bRunCallbackInMainThread = true;
	stParam.bThread = bThread;
	stParam.strID = strID;
	m_pClient->AddHttpCommonParam(&stParam);
	stParam.AddParam("this", (INT_PTR)this);
	stParam.AddParam("bThread", (INT_PTR)bThread);
	stParam.AddParam("bToWebCommand", (INT_PTR)bToWebCommand);

	ThreadWrapper::GetInstance()->AddTask(stParam);
}

bool SceneElementLoader::static_request_BaseInfo(map<string, INT_PTR>& mapParams, ThreadCallbackParam* stCallbackParam)
{
	SceneElementLoader* pThis = (SceneElementLoader*)mapParams["this"];
	RequestEndData* pEndData = new RequestEndData;
	mapParams["end_data"] = (INT_PTR)pEndData;

	pThis->requestElementTypeInfo(pEndData);
	pThis->requestMapInfo(pEndData);

	return true;
}

void SceneElementLoader::static_callback_BaseInfo(ThreadCallbackParam* pParam)
{
	SceneElementLoader* pThis = (SceneElementLoader*)pParam->sRequestParam.mapParams["this"];
	RequestEndData* pEndData = (RequestEndData*)pParam->sRequestParam.mapParams["end_data"];
	bool bThread = (bool)pParam->sRequestParam.mapParams["bThread"];
	bool bToRequestElementData = (bool)pParam->sRequestParam.mapParams["bToRequestElementData"];
	bool bToWebCommand = (bool)pParam->sRequestParam.mapParams["bToWebCommand"];

	std::vector<ProjectBasedSceneElementMapInfo*> vecLastMapInfo = pThis->m_vecMapInfo;
	std::vector<ProjectBasedSceneElementTypeInfo*> vecLastSceneTypeInfo = pThis->m_vecSceneTypeInfo;
	pThis->m_vecMapInfo = pEndData->vecTempMapInfo;
	pThis->m_vecSceneTypeInfo = pEndData->vecTempSceneTypeInfo;

	for (size_t i = 0; i < vecLastMapInfo.size(); ++i)
	{
		DELETE_PTR(vecLastMapInfo[i]);
	}
	vecLastMapInfo.clear();

	for (size_t i = 0; i < vecLastSceneTypeInfo.size(); ++i)
	{
		DELETE_PTR(vecLastSceneTypeInfo[i]);
	}
	vecLastSceneTypeInfo.clear();

	DELETE_PTR(pEndData);

	pThis->loadDataComplete(true, bToWebCommand);
}

void SceneElementLoader::OnlyRequestBaseInfo(bool bThread, bool bToWebCommand)
{
	requestBaseInfo(bThread, bToWebCommand);
}

void SceneElementLoader::loadDataComplete(bool bAllComplete,bool bToWebCommond)
{
	if (bAllComplete)
	{
		//预先创建Factory
		for (size_t i = 0; i < m_vecSceneTypeInfo.size(); ++i)
		{
			doCreateFactory(&m_vecSceneTypeInfo, 0, m_vecSceneTypeInfo[i]->nTypeID);
		}
	}

	if(bToWebCommond)
	{
		iOnLoadedData(bAllComplete);
	}
	else if(bAllComplete)
	{
		bc::BCEvent _tEvent(PROJECTBASED_EVENT_ONLY_LAYER_LOADED, 0, 0, 0);
		APIProvider::GetSystemAPI()->iEngineAPI->iExecuteEvent(_tEvent);
	}
}

void SceneElementLoader::addPageTaskToMain(PageData* pPageData, ThreadCallbackParam* stCallbackParam)
{
	ThreadCallbackParam* pCallbackParam = new ThreadCallbackParam;
	pCallbackParam->sRequestParam = stCallbackParam->sRequestParam;
	pCallbackParam->sRequestParam.bRepeat = false;
	Guid::CreateGuId(pCallbackParam->sRequestParam.strID, "Thread_");
	pCallbackParam->sRequestParam.AddParam("pageData", (INT_PTR)pPageData);
	ThreadWrapper::GetInstance()->AddMainRunCallback(pCallbackParam);
}

void SceneElementLoader::iOnSetDetailData(INode* pNode, ProjectBasedSceneElementTypeInfo* pTypeInfo, ProjectBasedSceneElement* pInfo)
{
	setDataMatch(pNode, pTypeInfo->mapDetailDataMatch, pInfo,false);
}

void SceneElementLoader::iOnSetLabelData(INode* pNode, ProjectBasedSceneElementTypeInfo* pTypeInfo, ProjectBasedSceneElement* pInfo)
{
	setDataMatch(pNode, pTypeInfo->mapLabelDataMatch, pInfo,false);
}

void SceneElementLoader::iOnCloseDetail(INode* pNode, ProjectBasedSceneElementTypeInfo* pTypeInfo, ProjectBasedSceneElement* pInfo)
{
	IBoard* pBoard = pNode->GetDynamicComponent<IBoard>();
	if (pBoard)
	{
		std::vector<INode*> vecNode;
		pBoard->iGetVideoBoard(vecNode);
		for (size_t i = 0; i < vecNode.size(); i++)
		{
			IVideo* pVideo = vecNode[i]->GetDynamicComponent<IVideo>();
			pVideo->iStopVideo();
		}
	}
}

void SceneElementLoader::iOnOpenDetail(INode* pNode, ProjectBasedSceneElementTypeInfo* pTypeInfo, ProjectBasedSceneElement* pInfo)
{
	setDataMatch(pNode, pTypeInfo->mapDetailDataMatch, pInfo,true);
}

void SceneElementLoader::setDataMatch(INode* pNode, std::map<std::string, std::string>& mapDataMatch, ProjectBasedSceneElement* pInfo, bool bPlayVideo)
{
	try
	{
		std::vector<std::string> vecItem;
		std::string strDataMatch = "";
		if (mapDataMatch.find(pInfo->strDataType) != mapDataMatch.end())
		{
			strDataMatch = mapDataMatch[pInfo->strDataType];
		}
		else
		{
			strDataMatch = mapDataMatch[""];
		}
		if (!strDataMatch.empty())
		{
			SplitStringBySpecial(strDataMatch, vecItem, ";");
			for (size_t i = 0; i < vecItem.size(); i++)
			{
				std::vector<std::string> vec;
				SplitStringBySpecial(vecItem[i], vec, "=");
				if (vec.size() >= 2)
				{
					std::string str3DAttr = vec[0];
					std::string strTemp = vec[1];
					std::vector<std::string> vecDataParam;
					SplitStringBySpecial(strTemp, vecDataParam, ":");
					if (vecDataParam.size() >= 1)
					{
						std::string strDataAttr = vecDataParam[0];
						std::string strDataType = "string";
						if (vecDataParam.size() >= 2)
						{
							strDataType = vecDataParam[1];
						}
						std::vector<std::string> vecTemp;
						SplitStringBySpecial(strDataAttr, vecTemp, ",");
						Json::Value value = Json::nullValue;
						if (vecTemp.size() == 1)
						{
							value = pInfo->GetExtraAttrValue(vecTemp[0]);
						}
						else
						{
							Json::Value jExtra;
							ParseJsonByString(pInfo->strExtra, jExtra);
							value = GetJsonItemRecursion(jExtra, strDataAttr);
						}
						std::string strText = "";
						if (strDataType == "string" && value.isString())
						{
							strText = value.asString();
						}
						else if (strDataType == "int" && value.isInt())
						{
							strText = to_string(value.asInt());
						}
						else if (strDataType == "float" && value.isDouble())
						{
							strText = to_string(value.asFloat());
						}
						else if (strDataType == "json" && value.isObject())
						{
							strText = value.toStyledString();
						}
						char sz[1024] = { 0 };
						Utf8ToAscii(strText.data(), sizeof(sz), sz);
						strText = sz;
						SetTextShowByStrKey(pNode, str3DAttr, strText);

						if (value.isNull() && strDataAttr == "title")
						{
							SetTextShowByStrKey(pNode, str3DAttr, pInfo->strTitle);
						}

						CommonLabelFactory* pFactory = GetOutElementFactoryByType(pInfo->nTypeID);
						if (!pFactory)
						{
							pFactory = GetInElementFactoryByType(pInfo->nTypeID);
						}
						if (bPlayVideo)
						{
							std::map<std::string, void*> mapExtraVoid;
							mapExtraVoid["this"] = this;
							mapExtraVoid["info"] = pInfo;
							std::map<std::string, Json::Value> mapExtraJson;

							//视频监控
							if (StartWith(pInfo->strDataType, "livegbs_camera") && str3DAttr == "camera_id")
							{
								mapExtraJson["isLivegbs"] = true;
								std::string strCameraID = strText;
								if (!strCameraID.empty())
								{
									IBoard* pBoard = pNode->GetDynamicComponent<IBoard>();
									if (pBoard)
									{
										std::vector<INode*> vecNode;
										pBoard->iGetVideoBoard(vecNode);
										if (vecNode.size() > 0)
										{
											IVideo* pVideo = vecNode[0]->GetDynamicComponent<IVideo>();
											pVideo->iStopVideo();
											pVideo->iSetVideoStream(m_pClient->m_strLivegbsHost, strCameraID, m_pClient->m_strLivegbsChannel);
											pVideo->iSetPlayErrorCallback(static_playerror_callback, mapExtraVoid, mapExtraJson);
											pVideo->iPlayVideo();

										}
									}
								}
							}
							if (StartWith(pInfo->strDataType, "url_camera") && str3DAttr == "url")
							{
								mapExtraJson["isLivegbs"] = false;
								std::string strUrl = strText;
								if (!strUrl.empty())
								{
									IBoard* pBoard = pNode->GetDynamicComponent<IBoard>();
									if (pBoard)
									{
										std::vector<INode*> vecNode;
										pBoard->iGetVideoBoard(vecNode);
										if (vecNode.size() > 0)
										{
											VideoBoardParam* pParam = (VideoBoardParam*)vecNode[0]->GetDynamicComponent<IBoard>()->iGetBoardParam();
											pParam->bTimeStamp = true;
											IVideo* pVideo = vecNode[0]->GetDynamicComponent<IVideo>();
											pVideo->iStopVideo();
											pVideo->iSetVideoStream(strUrl);
											pVideo->iSetPlayErrorCallback(static_playerror_callback, mapExtraVoid, mapExtraJson);
											pVideo->iPlayVideo();
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	catch (const std::exception&)
	{
	}
}

void SceneElementLoader::ParseSubscribeData(const std::string* pStrData)
{
	doThreadParseSubscribeTask(1, pStrData);
}

int SceneElementLoader::comParseSubscribeData(int nParseType, std::string& strData, std::vector<ProjectBasedSceneElement*>&
	vecData,bool bUseHeightTif)
{
	int nCode = 0;
	std::map<int, CommonLabelFactory*> mapFactory;
	Json::Value root;
	ParseJsonByString(strData, root);
	if (!root.isNull())
	{
		try
		{
			std::string strDataType = root["dataType"].asString();
			if (strDataType == "#clear#")
			{
				return -1;
			}
			if (strDataType == "#end#")
			{
				return -2;
			}
			if (StartWith(strDataType, "udp_") && !(nParseType == 2))
			{
				//订阅的udp的模式，不是真正的udp推送
				nCode = 3;
			}
			std::string strSub = root["subId"].asString();
			std::string strCoorType = root["xyz"].asString();
			Json::Value& dataArray = root["data"];
			EElementCoordinateType eCoorType;
			if (strCoorType == "xyz")
			{
				eCoorType = EElementCoordinateType::COOR_SCENE_LOCAL;
			}
			else if (strCoorType == "wgs84")
			{
				eCoorType = EElementCoordinateType::COOR_WGS84;
			}
			else if (strCoorType == "hc_84")
			{
				eCoorType = EElementCoordinateType::COOR_WGS84_SELF;
			}
			else if (strCoorType == "CGCS2000")
			{
				eCoorType = EElementCoordinateType::COOR_CGCS2000;
			}
			else if (strCoorType == "BD_09")
			{
				eCoorType = EElementCoordinateType::COOR_BD09;
			}
			else if (strCoorType == "GCJ_02")
			{
				eCoorType = EElementCoordinateType::COOR_GCJ02;
			}
			if (dataArray.isArray())
			{
				for (int i = 0; i < dataArray.size(); i++)
				{
					int nType = dataArray[i]["usage_type"].asInt();
					if (mapFactory.find(nType) == mapFactory.end())
					{
						CommonLabelFactory* pFactory = GetOutElementFactoryByType(nType);
						if (!pFactory)
						{
							pFactory = GetInElementFactoryByType(nType);
						}
						mapFactory[nType] = pFactory;
					}
					CommonLabelFactory* pFactory = mapFactory[nType];
					if (!pFactory)
					{
						continue;
					}
					bool bCanSmooth = pFactory->GetFunc(FUNC_SMOOTH_MOVE) ? true : false;
					ProjectBasedSceneElementTypeInfo* pType = nullptr;
					//先不查找，减少查找次数
					//ProjectBasedSceneElementTypeInfo* pType = GetElementTypeInfoByID(nType);
					//if (!pType)
					//{
					//	continue;
					//}
					ProjectBasedSceneElement* pInfo = iCreateSceneElementInfo(nType);
					std::string strUUID = dataArray[i]["id"].asString();
					if (!strUUID.empty())
					{
						pInfo->strUuid = dataArray[i]["id"].asString();
					}
					pInfo->nTypeID = nType;
					pInfo->bCanSmooth = bCanSmooth;
					pInfo->strDataType = strDataType;
					pInfo->strTitle = dataArray[i]["title"].asString();
					pInfo->strTitle = Utf8ToAscii(pInfo->strTitle.c_str());
					pInfo->strExtra = dataArray[i]["extend"].toStyledString();
					if (dataArray[i].isMember("map_id"))
					{
						pInfo->nMapID = dataArray[i]["map_id"].asInt();
					}
					pInfo->nElemType = dataArray[i]["elem_type"].asInt();
					std::string strLabel_coord, strCoords;
					double dX = 0.0f;
					double dY = 0.0f;
					if (dataArray[i]["label_coord"].isString())
					{
						strLabel_coord = dataArray[i]["label_coord"].asString();
					}
					if (dataArray[i]["coords"].isString())
					{
						strCoords = dataArray[i]["coords"].asString();
					}
					if (dataArray[i]["x"].isDouble())
					{
						dX = dataArray[i]["x"].asDouble();
					}
					if (dataArray[i]["y"].isDouble())
					{
						dY = dataArray[i]["y"].asDouble();
					}
					if (pInfo->nElemType == 1)
					{
						//点
						pInfo->dLng = dX;
						pInfo->dLat = dY;
						m_pClient->ConvertToVector3WithCoorSystem(pInfo->dLng, pInfo->dLat, &pInfo->vLabelPos, eCoorType, false);
					}
					else if (pInfo->nElemType == 2 || pInfo->nElemType == 3)
					{
						std::vector<std::string> vecLabelCoord;
						if (!strLabel_coord.empty())
						{
							SplitStringBySpecial(strLabel_coord, vecLabelCoord, ",");
						}
						std::vector<std::string> vecCoord;
						if (!strCoords.empty())
						{
							SplitStringBySpecial(strCoords, vecCoord, ":");
						}
						if (vecLabelCoord.size() >= 2)
						{
							pInfo->dLng = dX;
							pInfo->dLat = dY;
							m_pClient->ConvertToVector3WithCoorSystem(atof(vecLabelCoord[0].c_str()), atof(vecLabelCoord[1].c_str()), &pInfo->vLabelPos, eCoorType, false);
						}
						for (size_t j = 0; j < vecCoord.size(); j++)
						{
							std::vector<std::string> vecTemp;
							SplitStringBySpecial(vecCoord[j], vecTemp, ",");
							if (vecTemp.size() >= 2)
							{
								pInfo->vecOriginLngLat.push_back(Vector2(atof(vecTemp[0].c_str()), atof(vecTemp[1].c_str())));
								Vector3d vPos = Vector3d::ZERO;
								m_pClient->ConvertToVector3WithCoorSystem(atof(vecTemp[0].c_str()), atof(vecTemp[1].c_str()), &vPos, eCoorType, false);
								pInfo->vecPoint.push_back(vPos);
							}
						}
						if (pInfo->vLabelPos.Empty())
						{
							for (size_t i = 0; i < pInfo->vecPoint.size(); i++)
							{
								pInfo->vLabelPos += pInfo->vecPoint[i];
							}
							if (pInfo->vecPoint.size() > 0)
							{
								pInfo->vLabelPos /= pInfo->vecPoint.size();
							}
						}
					}

					//判断是否超出聚合范围，超出的则忽略不要
					bool bDelete = false;
					ClusterFunc* pClusterFun = (ClusterFunc*)pFactory->GetFunc(FUNC_CLUSTER);
					if (pClusterFun)
					{
						Vector3d vPos = pInfo->vLabelPos;
						bDelete = !(pClusterFun->IsInRange(vPos));
					}
					if (!bDelete)
					{
						bDelete = !(iFilterElementInfo(eCoorType, dataArray[i], pType, pInfo));
					}
					if (bDelete)
					{
						DELETE_PTR(pInfo);
					}
					else
					{
						checkIndoor(pInfo);
						vecData.push_back(pInfo);
					}
				}

				root.clear();
				strData.clear();
				std::map<int, std::vector<Vector3d*>> mapVecPoints;
				for (size_t j = 0; j < vecData.size(); j++)
				{
					std::vector<Vector3d*>& vec = mapVecPoints[vecData[j]->nTypeID];
					CommonLabelFactory* pFactory = mapFactory[vecData[j]->nTypeID];
					bool bReadHeightTif = pFactory->GetFunc(FUNC_READ_HRIGHT_TIF);
					//判断是否获取高地图
					if (!vecData[j]->bIndoor)
					{
						if ((!bReadHeightTif || bUseHeightTif))
						{
							vec.push_back(&vecData[j]->vLabelPos);
							for (size_t n = 0; n < vecData[j]->vecPoint.size(); n++)
							{
								vec.push_back(&vecData[j]->vecPoint[n]);
							}
							vecData[j]->eReadTifStatus = READ_TIF_NEED_DONE; //无需读取高地图
						}
						else
						{
							vecData[j]->eReadTifStatus = READ_TIF_NEED_NOT_YET_ADD;	//需要加载高地图，还未加载
						}
					}
				}
				for (std::map<int, std::vector<Vector3d*>>::iterator it = mapVecPoints.begin(); it != mapVecPoints.end(); ++it)
				{
					std::vector<double> vecHeight;
					std::vector<Vector3d> vecTemp;
					for (size_t i = 0; i < it->second.size(); ++i)
					{
						Vector3d v = *(it->second[i]);
						vecTemp.push_back(v);
					}
					m_pClient->GetZFromTif(vecTemp, vecHeight);
					for (size_t i = 0; i < vecHeight.size(); ++i)
					{
						Vector3d vPos = *it->second[i];
						m_pClient->AdjustZ(vPos,vecHeight[i],false);
						*it->second[i] = vPos;
					}
				}
				for (size_t j = 0; j < vecData.size(); j++)
				{
					vecData[j]->vSmoothPos = vecData[j]->vLabelPos;
					vecData[j]->vSmoothAngle = vecData[j]->vAngle;
				}
			}
		}
		catch (const std::exception&)
		{
		}
	}

	return nCode;
}

void SceneElementLoader::iDoUdpData(std::string strData)
{
	std::string* pStrData = new std::string(strData);
	doThreadParseSubscribeTask(2, pStrData);
}

void SceneElementLoader::iDoUdpData(std::vector<ProjectBasedSceneElement*>& vecData,bool bNeedReadTif)
{
	//处理高地图的逻辑
	ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
	for (size_t i = 0; i < vecData.size(); i++)
	{
		vecData[i]->vSmoothPos = vecData[i]->vLabelPos;
		vecData[i]->vSmoothAngle = vecData[i]->vAngle;
		checkIndoor(vecData[i]);
		if (bNeedReadTif && !vecData[i]->bIndoor)
		{
			vecData[i]->vInitNodePos = vecData[i]->vLabelPos;
			vecData[i]->eReadTifStatus = READ_TIF_NEED_NOT_YET_ADD;
		}
	}

	SubscribeBufferData* pInfo = new SubscribeBufferData;
	pInfo->nType = 2;
	for (auto pElementInfo : vecData)
	{
		pInfo->queueElement.push(pElementInfo);
	}
	std::vector<int> vecType;
	for (size_t i = 0; i < vecData.size(); i++)
	{
		if (find(vecType.begin(), vecType.end(), vecData[i]->nTypeID) == vecType.end())
		{
			vecType.push_back(vecData[i]->nTypeID);
		}
	}
	pInfo->vecLayers = vecType;
	BCAutoLock lock(&m_MutexSubcribeLock);
	m_queueSubscribeBuffer.push(pInfo);

	std::string strMsg = "SceneElementLoader iDoUdpData直接ProjectBasedSceneElement,";
	strMsg += "数量=" + to_string(vecData.size());
	std::string strTypes = "";
	for (size_t i = 0; i < vecType.size(); ++i)
	{
		if (i != 0)
		{
			strTypes += ",";
		}
		strTypes += to_string(vecType[i]);
	}
	strMsg += ",类型:" + strTypes;
	ProjectLogger::GetInstance()->InfoMessage(strMsg);
}

void SceneElementLoader::iFrameUpdate()
{
	std::vector<int> vecTypes;
	bool bUpdate = false;
	bool bUdpData = false;
	bool bAllRefresh = false;
	bool bClear = false;
	std::vector<ProjectBasedSceneElement*> vecAddElement;		//直接添加的
	std::vector<ProjectBasedSceneElement*> vecUdpUpdateElement;	//upd的更新数据
	std::vector<ProjectBasedSceneElement*> vecUdpAddElement;		//upd的添加数据
	int nHandleCount = 0;			//每一帧已经处理的队列数量
	int nHandleMax = 1000;
	while (!m_queueSubscribeBuffer.empty())
	{
		bUpdate = true;
		SubscribeBufferData* pBuffer = m_queueSubscribeBuffer.front();
		if (pBuffer->nType == 1)
		{
			//静态数据
			while (!pBuffer->queueElement.empty())
			{
				ProjectBasedSceneElement* pEle = pBuffer->queueElement.front();

				ProjectBasedSceneElement* pInfo = FindSceneElementByID(m_vecUdpSceneElement, pEle->strUuid, pEle->nTypeID, false);
				if (!pInfo)
				{
					m_vecResidentElement.emplace_back(pEle);
					vecAddElement.emplace_back(pEle);
				}
				else
				{
					DELETE_PTR(pEle);
				}
				pBuffer->queueElement.pop();
				nHandleCount++;
				if (nHandleCount >= nHandleMax)
				{
					break;
				}
			}
		}
		else if (pBuffer->nType == 2)
		{
			//udp
			while (!pBuffer->queueElement.empty())
			{
				ProjectBasedSceneElement* pEle = pBuffer->queueElement.front();
				ProjectBasedSceneElement* pInfo = FindSceneElementByID(m_vecUdpSceneElement, pEle->strUuid, pEle->nTypeID, false);
				bool bFindInUdp = pInfo ? true : false;
				if (!pInfo)
				{
					//在extra里面找
					for (map<int, std::vector<ProjectBasedSceneElement*>>::iterator it = m_mapExtraSceneElement.begin(); it != m_mapExtraSceneElement.end(); it++)
					{
						if (it->first != pEle->nTypeID)
						{
							continue;
						}
						bool bFind = false;
						for (std::vector<ProjectBasedSceneElement*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
						{
							if ((*it2)->strUuid == pEle->strUuid)
							{
								pInfo = (*it2);
								it->second.erase(it2);
								bFind = true;
								break;
							}
						}
						if (bFind)
						{
							break;
						}
					}
				}
				if (!pInfo)
				{
					//在订阅里m_vecResidentElement找
					pInfo = FindSceneElementByID(m_vecResidentElement, pEle->strUuid, pEle->nTypeID, true);
				}
				if (pInfo)
				{
					pEle->jProjectExtra = pInfo->jProjectExtra;
					if (pInfo->bCanSmooth)
					{
						//假如是平滑移动
						pEle->vLabelPos = pInfo->vLabelPos;
					}
					*pInfo = *pEle;
					DELETE_PTR(pEle);

					vecUdpUpdateElement.push_back(pInfo);
					if (!bFindInUdp)
					{
						m_vecUdpSceneElement.push_back(pInfo);
					}
				}
				else
				{
					m_vecUdpSceneElement.push_back(pEle);
					vecUdpAddElement.push_back(pEle);
				}

				pBuffer->queueElement.pop();
				nHandleCount++;
				if (nHandleCount >= nHandleMax)
				{
					break;
				}
			}
			
			bUdpData = true;
		}
		else if (pBuffer->nType == -1)
		{
			//清空
			ClearElementShow();
			bAllRefresh = true;
			bClear = true;
		}
		else if (pBuffer->nType == -2)
		{
			//断开连接
			APIProvider::GetSystemAPI()->iProjectClientAPI->iExecuteQtVsCommond("close_subscribe_connect", nullptr, nullptr);
		}
		else if (pBuffer->nType == 3)
		{
			//udp的订阅类型
			while (!pBuffer->queueElement.empty())
			{
				ProjectBasedSceneElement* pEle = pBuffer->queueElement.front();
				bool bFind = false;
				for (size_t i = 0; i < m_vecUdpSceneElement.size(); ++i)
				{
					if ((m_vecUdpSceneElement[i]->strUuid == pEle->strUuid) &&
						m_vecUdpSceneElement[i]->nTypeID == pEle->nTypeID)
					{
						bFind = true;
						break;
					}
				}
				if (!bFind)
				{
					m_vecUdpSceneElement.push_back(pEle);
					vecUdpAddElement.push_back(pEle);
				}
				pBuffer->queueElement.pop();
				nHandleCount++;
				if (nHandleCount >= nHandleMax)
				{
					break;
				}
			}
		}
		vecTypes.insert(vecTypes.end(), pBuffer->vecLayers.begin(), pBuffer->vecLayers.end());
		if (nHandleCount >= nHandleMax)
		{
			break;
		}
		m_queueSubscribeBuffer.pop();

		DELETE_PTR(pBuffer);
	}
	if (bUpdate)
	{
		if (bAllRefresh)
		{
			if (bClear)
			{
				vecTypes.clear();
			}
			createFactory(&m_vecSceneTypeInfo, vecTypes);
		}
		else
		{
			if (vecAddElement.size() > 0)
			{
				appendRefreshFactoryElementData(vecAddElement,true);
			}
			if (vecUdpAddElement.size() > 0)
			{
				appendRefreshFactoryElementData(vecUdpAddElement,true);
			}
			if (vecUdpUpdateElement.size() > 0)
			{		
				updateRefreshFactoryElementData(vecUdpUpdateElement);
			}
		}
	}

	//删除需要删除的Factory
	for (std::vector<CommonLabelFactory*>::iterator it = m_vecWillDeleteFactory.begin(); it != m_vecWillDeleteFactory.end();)
	{
		CommonLabelFactory* pFactory = *it;
		if (pFactory->IsAllStop())
		{
			DELETE_PTR(pFactory);
			m_vecWillDeleteFactory.erase(it);
		}
		else
		{
			it++;
		}
	}
}

bool SceneElementLoader::static_parse_subscribe(map<string, INT_PTR>& mapParams, ThreadCallbackParam* stCallbackParam)
{
	SceneElementLoader* pThis = (SceneElementLoader*)mapParams["this"];
	int nType = mapParams["type"];
	std::string* pStrData = (std::string*)mapParams["pStrData"];
	std::string strData(*pStrData);
	DELETE_PTR(pStrData);

	std::vector<ProjectBasedSceneElement*> vecElement;
	if (pThis->m_bWriteWebSocketLog && nType == 1)
	{
		//只打印订阅的，udp的在udp目录找
		int nLength = strData.length() + 1;
		char* szBuffer = new char[nLength];
		Utf8ToAscii(strData.data(), nLength, szBuffer);
		ProjectLogger::GetInstance()->InfoMessage(szBuffer);
		DELETE_ARRAY(szBuffer);
	}
	bool bUseHeightTif = false;
	if (nType == 2)
	{
		//udp来的则直接读取高度提
		bUseHeightTif = true;
	}
	int nCode = pThis->comParseSubscribeData(nType, strData, vecElement, bUseHeightTif);
	SubscribeBufferData* pInfo = new SubscribeBufferData;
	pInfo->nType = nType;
	for (auto pEle : vecElement)
	{
		pInfo->queueElement.push(pEle);
	}
	std::vector<int> vecType;
	for (size_t i = 0; i < vecElement.size(); i++)
	{
		if (find(vecType.begin(), vecType.end(), vecElement[i]->nTypeID) == vecType.end())
		{
			vecType.push_back(vecElement[i]->nTypeID);
		}
	}
	if (nType == 1)
	{
		std::string strTypes = "";
		for (size_t i = 0; i < vecType.size(); ++i)
		{
			if (i > 0)
			{
				strTypes = ",";
			}
			strTypes += to_string(vecType[i]);
		}
		//静态
		std::string str = "订阅收到静态元素个数：";
		str += to_string(vecElement.size());
		str += ", 类型: " + strTypes;
		ProjectLogger::GetInstance()->DebugMessage(str);
	}
	else if (nType == 2)
	{
		//udp,会有很多打印
		//std::string strTypes = "";
		//for (size_t i = 0; i < vecType.size(); ++i)
		//{
		//	if (i > 0)
		//	{
		//		strTypes = ",";
		//	}
		//	strTypes += to_string(vecType[i]);
		//}
		////
		//std::string str = "订阅收到udp元素个数：";
		//str += to_string(vecElement.size());
		//str += "类型有:" + strTypes;
		//ProjectLogger::GetInstance()->InfoMessage(str);
	}
	if (nCode != 0)
	{
		pInfo->nType = nCode;
	}
	mapParams["result"] = (INT_PTR)pInfo;

	return true;
}

void SceneElementLoader::static_callback_subscribe(ThreadCallbackParam* pParam)
{
	SceneElementLoader* pThis = (SceneElementLoader*)pParam->sRequestParam.mapParams["this"];
	int nType = pParam->sRequestParam.mapParams["type"];
	SubscribeBufferData* pInfo = (SubscribeBufferData*)pParam->sRequestParam.mapParams["result"];

	pThis->m_queueSubscribeBuffer.push(pInfo);

}

void SceneElementLoader::doThreadParseSubscribeTask(int nType, const std::string* pStrData)
{
	std::string strID;
	Guid::CreateGuId(strID, "Thread_");
	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_CUSTOM;
	stParam.pThreadRuningFunc = static_parse_subscribe;
	stParam.pThreadCompleteFunc = static_callback_subscribe;
	stParam.bRunCallbackInMainThread = true;
	if (nType == 2)
	{
		//udp
		stParam.bThread = false;
	}
	else
	{
		stParam.bThread = true;
	}
	stParam.bInQueue = true;
	stParam.strQueueID = THREAD_QUEUE_ID;
	stParam.strID = strID;
	stParam.AddParam("this", (INT_PTR)this);
	stParam.AddParam("type", (INT_PTR)nType);
	stParam.AddParam("pStrData", (INT_PTR)pStrData);		
	ThreadWrapper::GetInstance()->AddTask(stParam);
}

void SceneElementLoader::replaceBaseSceneTypeStyle(Json::Value& jSrc, Json::Value& jParamObject)
{
	Json::Value::Members mem = jSrc.getMemberNames();
	for (auto iter = mem.begin(); iter != mem.end(); iter++)
	{
		Json::Value& jItem = jSrc[(*iter)];
		if (!jItem.isNull() && !jItem.isObject())
		{
			//当前的key
			for (int n = 0; n < jParamObject.size(); n++)
			{
				if (jParamObject[n]["key"].asString() == (*iter))
				{
					jParamObject[n]["value"] = jItem;
					break;
				}
			}
		}
		else if (jItem.isObject())
		{
			for (int n = 0; n < jParamObject.size(); n++)
			{
				if (jParamObject[n]["key"].asString() == (*iter))
				{
					Json::Value& jTempOutParam = jParamObject[n];
					replaceBaseSceneTypeStyle(jItem, jTempOutParam["children"]);
					break;
				}
			}
		}
	}
}

void SceneElementLoader::iDoWebCommond(WebCommandData* pWebData)
{
	try
	{
		if (pWebData->strCommand == "Web_ShowLabelType" || pWebData->strCommand == "Web_ShowIndoorLabelType"
			|| pWebData->strCommand == "Web_ChangeCurShowLabelType" || pWebData->strCommand == "Web_ChangeIndoorCurShowLabelType")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strTypes = pWebData->vecValues[0].asString();
				std::vector<std::string> vecType;
				SplitStringBySpecial(strTypes, vecType, ",");
				std::vector<CommonLabelFactory*> vecFactory;
				if (pWebData->strCommand == "Web_ShowLabelType")
				{
					GetAllOutElementFacotory(vecFactory);
					m_vecCurOutShowType = vecType;

					std::string strMsg = "SceneElementLoader m_vecCurOutShowType:" + strTypes;
					ProjectLogger::GetInstance()->InfoMessage(strMsg);
				}
				if (pWebData->strCommand == "Web_ShowIndoorLabelType")
				{
					GetAllInElementFacotory(vecFactory);
					m_vecCurInShowType = vecType;

					std::string strMsg = "SceneElementLoader m_vecCurInShowType:" + strTypes;
					ProjectLogger::GetInstance()->InfoMessage(strMsg);
				}
				if (pWebData->strCommand == "Web_ChangeCurShowLabelType")
				{
					bool bShow = pWebData->vecValues[1].asBool();
					for (size_t i = 0; i < vecType.size(); i++)
					{
						std::vector<std::string>::iterator it = find(m_vecCurOutShowType.begin(), m_vecCurOutShowType.end(), vecType[i]);
						if (bShow)
						{
							if (it == m_vecCurOutShowType.end())
							{
								m_vecCurOutShowType.push_back(vecType[i]);
							}
						}
						else
						{
							if (it != m_vecCurOutShowType.end())
							{
								m_vecCurOutShowType.erase(it);
							}
						}
					}

					std::string strMsg = "SceneElementLoader m_vecCurOutShowType:" + strTypes;
					ProjectLogger::GetInstance()->InfoMessage(strMsg);
				}
				if (pWebData->strCommand == "Web_ChangeIndoorCurShowLabelType")
				{
					bool bShow = pWebData->vecValues[1].asBool();
					for (size_t i = 0; i < vecType.size(); i++)
					{
						std::vector<std::string>::iterator it = find(m_vecCurInShowType.begin(), m_vecCurInShowType.end(), vecType[i]);
						if (bShow)
						{
							if (it == m_vecCurInShowType.end())
							{
								m_vecCurInShowType.push_back(vecType[i]);
							}
						}
						else
						{
							if (it != m_vecCurInShowType.end())
							{
								m_vecCurInShowType.erase(it);
							}
						}
					}

					std::string strMsg = "SceneElementLoader m_vecCurInShowType:" + strTypes;
					ProjectLogger::GetInstance()->InfoMessage(strMsg);
				}
				ChangeShowLabel();
			}
		}
		else if (pWebData->strCommand == "Web_SubscribeHCElement")
		{
			ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
			bool bUserChanged = !pClient->m_sUserInfo.IsSame();
			if (bUserChanged || m_vecResidentElement.empty())
			{
				HeightTifReader::GetInstance()->Clear();
				std::string strMsg = "清空SceneElementLoader高度图读取队列";
				ProjectLogger::GetInstance()->InfoMessage(strMsg);

				//清空线程队列
				ThreadWrapper::GetInstance()->ClearQueueTask(THREAD_QUEUE_ID);
				strMsg = "清空SceneElementLoader线程队列";
				ProjectLogger::GetInstance()->InfoMessage(strMsg);

				//清空解析的队列数据
				while (!m_queueSubscribeBuffer.empty())
				{
					SubscribeBufferData* pBuffer = m_queueSubscribeBuffer.front();
					while (!pBuffer->queueElement.empty())
					{
						ProjectBasedSceneElement* pInfo = pBuffer->queueElement.front();
						DELETE_PTR(pInfo);
						pBuffer->queueElement.pop();
					}
					DELETE_PTR(pBuffer);
					m_queueSubscribeBuffer.pop();
				}
				strMsg = "清空SceneElementLoader数据队列";
				ProjectLogger::GetInstance()->InfoMessage(strMsg);
				//清空订阅数据
				ClearElementShow();
				strMsg = "清空SceneElementLoader静态数据和m_queueAddElement,m_queueUpdateElement";
				ProjectLogger::GetInstance()->InfoMessage(strMsg);
				ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
				if ((pClient->m_nAppMode & APP_MODE_EXHIBITION) || (pClient->m_nAppMode & APP_MODE_LAYER_EDIT))
				{
					//从redis读取数据
					if (m_nSupportRequestType & REQUEST_REDIS)
					{
						RequestDoSubscribeFromRedis();
						std::string strMsg = "SceneElementLoader读取redis";
						ProjectLogger::GetInstance()->InfoMessage(strMsg);
					}
					if (m_nSupportRequestType & REQUEST_TCP)
					{
						RequestDoSubscribe();
						std::string strMsg = "SceneElementLoader启动订阅";
						ProjectLogger::GetInstance()->InfoMessage(strMsg);
					}
				}
			}
			//bUserChanged:用户或者应用id改变了
			bc::BCEvent _tEvent(PROJECTBASED_EVENT_SCENE_ELEMENT_DATA_REFRESHED, bUserChanged ? 1 : 0, 0, 0);

			APIProvider::GetSystemAPI()->iEngineAPI->iExecuteEvent(_tEvent);
		}
		else if (pWebData->strCommand == "Web_LayerDebug_ChangeTypeStyle")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				Json::Value& jObject = pWebData->vecValues[0];
				int nTypeID = jObject["id"].asInt();
				ProjectBasedSceneElementTypeInfo* pTypeInfo = GetElementTypeInfoByID(nTypeID);
				if (pTypeInfo)
				{
					parseElementTypeInfo(pTypeInfo, jObject);
				}
				else
				{
					pTypeInfo = new ProjectBasedSceneElementTypeInfo;
					parseElementTypeInfo(pTypeInfo, jObject);
					m_vecSceneTypeInfo.push_back(pTypeInfo);
				}
				//清空，然后重新生成数据
				ParamParseStream::GetInstance()->Clear();
				CommonLabelFactory* pFactory = GetOutElementFactoryByType(nTypeID);
				bool bLastVisible = true;
				if (pFactory)
				{
					std::vector<FactotryAssembleDetail*> vecDetailAssemble;
					pFactory->GetDetailFactory()->GetCurAssemble(vecDetailAssemble);
					for (auto* pDetailAssemble : vecDetailAssemble)
					{
						m_vecModifyStyleSelectPoint.push_back(pDetailAssemble->pData->strUuid);
					}

					bLastVisible = pFactory->IsAllVisible();
					pFactory->SetAllVisible(false);
					//pFactory->RefreshZero();
					vector<CommonLabelFactory*>::iterator it = find(m_vecElementFactory.begin(), m_vecElementFactory.end(), pFactory);
					if (it != m_vecElementFactory.end())
					{
						m_vecElementFactory.erase(it);
					}
					std::map<int, CommonLabelFactory*>::iterator it2 = m_mapOutElementFactory.find(nTypeID);
					if (it2 != m_mapOutElementFactory.end())
					{
						m_mapOutElementFactory.erase(it2);
					}
					if (find(m_vecWillDeleteFactory.begin(), m_vecWillDeleteFactory.end(), pFactory) == m_vecWillDeleteFactory.end())
					{
						m_vecWillDeleteFactory.push_back(pFactory);
					}
				}
				std::vector<int> vecType;
				vecType.push_back(nTypeID);
				CommonLabelFactory* pNewFactory = doCreateFactory(&m_vecSceneTypeInfo, 0, nTypeID);
				createFactory(&m_vecSceneTypeInfo, vecType);
				if (pNewFactory)
				{
					pNewFactory->SetAllVisible(bLastVisible);
				}
				if (bLastVisible)
				{
					//打开详情面板
					for (auto strUUID : m_vecModifyStyleSelectPoint)
					{
						FactoryAssemble* pTemp = pNewFactory->FindAssembleByID(strUUID);
						if (pTemp)
						{
							pNewFactory->GetDetailFactory()->AutoSwitchState(pTemp, pNewFactory->IsRevertClick());
						}
					}
				}
				m_vecModifyStyleSelectPoint.clear();
			}
		}
		else if (pWebData->strCommand == "Web_GetLayerTextShow")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				int nLayerID = pWebData->vecValues[0].asInt();
				ProjectBasedSceneElementTypeInfo *pType = GetElementTypeInfoByID(nLayerID);
				if (pType)
				{
					Json::Value jObject;
					{
						std::vector<std::string> vecStyle;
						vecStyle.insert(vecStyle.end(), pType->vecStrLabelStyle.begin(), pType->vecStrLabelStyle.end());
						Json::Value jArray = Json::arrayValue;
						for (auto strType : vecStyle)
						{
							ReadParam tReadParam;
							ParamParseStream::GetInstance()->ReadJsonToParam(tReadParam, strType);
							if (tReadParam.pParam)
							{
								INode* pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(*tReadParam.pParam);
								std::vector<TextShow*> vecTextShow;
								FindAllTextShow(pNode, vecTextShow);
								for (auto pTextShow : vecTextShow)
								{
									if (!pTextShow->strID.empty())
									{
										jArray.append(pTextShow->strID);
									}
								}
								DELETE_PTR(pNode);
							}
							jObject["label"] = jArray;
						}
					}
					{
						std::vector<std::string> vecStyle;
						vecStyle.push_back(pType->strDetailStyle);
						Json::Value jArray = Json::arrayValue;
						for (auto strType : vecStyle)
						{
							ReadParam tReadParam;
							ParamParseStream::GetInstance()->ReadJsonToParam(tReadParam, strType);
							if (tReadParam.pParam)
							{
								INode* pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(*tReadParam.pParam);
								std::vector<TextShow*> vecTextShow;
								FindAllTextShow(pNode, vecTextShow);
								for (auto pTextShow : vecTextShow)
								{
									if (!pTextShow->strID.empty())
									{
										jArray.append(pTextShow->strID);
									}
								}
								DELETE_PTR(pNode);
							}
							jObject["detail"] = jArray;
						}
					}
					std::string strData = jObject.toStyledString();
					m_pClient->ToSendWebCommond("VS_GetLayerTextShow", pWebData->strID, strData);
				}
			}
		}
	}
	catch (const std::exception&)
	{

	}

	for (size_t i = 0; i < m_vecElementFactory.size(); ++i)
	{
		m_vecElementFactory[i]->iDoWebCommond(pWebData);
	}
}

void SceneElementLoader::RequestDoSubscribeFromRedis()
{
	char szBuffer[1024] = { 0 };
	sprintf_s(szBuffer, sizeof(szBuffer), "%s/api/tz/getDataRulesByCache", m_strServerHost.c_str());
	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_API_GET;
	stParam.AddParam("this", (INT_PTR)this);
	stParam.pThreadCompleteFunc = static_callback_subscribe_cache;
	stParam.strID = "RequestDoSubscribeFromCache";
	stParam.strUrl = szBuffer;
	stParam.bRepeat = false;
	stParam.bThread = true;
	stParam.bRunCallbackInMainThread = false;
	ThreadWrapper::GetInstance()->AddTask(stParam);
}

void SceneElementLoader::static_callback_subscribe_cache(ThreadCallbackParam_s* pParam)
{
	SceneElementLoader* pThis = (SceneElementLoader*)pParam->sRequestParam.mapParams["this"];
	if (pParam->nResult == STATUS_SUCCESS)
	{
		try
		{
			//redis 服务 端口 auth ds
			std::string strCache = pParam->jRoot["data"]["redis_link"].toStyledString();
			std::string strData = pParam->jRoot["data"]["data_rules"].toStyledString();
			Json::Value jCache, jData;
			ParseJsonByString(strCache, jCache);
			ParseJsonByString(strData, jData);
			if (!jCache.isNull())
			{
				std::string strCacheHost = jCache["Host"].asString();
				int nCachePort = jCache["Port"].asInt();
				std::string strCacheAuth = jCache["Pass"].asString();
				int nCacheDbIndex = jCache["DbIndex"].asInt();
				pThis->m_bConnectedRedis = pThis->m_pBCRedis->FConnect(strCacheHost, nCachePort, strCacheAuth);
				if (pThis->m_bConnectedRedis)
				{
					pThis->m_pBCRedis->FSetDataBase(nCacheDbIndex);
				}
			}
			if (!jData.isNull() && jData.isArray())
			{
				std::vector<std::string> vecValue;
				int nCount = jData.size();
				for (int i = 0; i < nCount; i++)
				{
					if (pThis->m_bConnectedRedis)
					{
						vecValue.clear();
						std::string strSetKey = jData[i]["value"].asString();
						pThis->m_pBCRedis->FGetSetValue(strSetKey, vecValue);
						std::string str;
						str += "redis收到的类型: ";
						str += strSetKey;
						ProjectLogger::GetInstance()->DebugMessage(str);
						for (size_t j = 0; j < vecValue.size(); j++)
						{
							std::string* pStrData = new std::string(vecValue[j]);
							pThis->ParseSubscribeData(pStrData);
						}
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			e.what();
		}
	}
}

void SceneElementLoader::RequestDoSubscribe()
{
	char szBuffer[1024] = { 0 };
	bool bServer = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bDataSubscribeServer;
	sprintf_s(szBuffer, sizeof(szBuffer), "%s/api/v10/subscribe?pageSize=2000&dataType=all&userToken=%s&passive=%d", m_strServerHost.c_str(),
		m_pClient->m_sUserInfo.strUserToken.c_str(), bServer ? 0 : 1);

	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_API_GET;
	stParam.AddParam("this", (INT_PTR)this);
	stParam.pThreadCompleteFunc = static_callback_dosubscribe;
	stParam.strID = "RequestDoSubscribe";
	stParam.strUrl = szBuffer;
	stParam.bRepeat = false;
	stParam.bThread = true;
	stParam.bRunCallbackInMainThread = true;
	ThreadWrapper::GetInstance()->AddTask(stParam);
}

void SceneElementLoader::static_callback_dosubscribe(ThreadCallbackParam_s* pParam)
{
	SceneElementLoader* pThis = (SceneElementLoader*)pParam->sRequestParam.mapParams["this"];
	if (pParam->nResult == STATUS_SUCCESS)
	{
		try
		{
			bool bServer = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bDataSubscribeServer;
			if (!bServer)
			{
				std::string strInfo = pParam->jRoot["data"]["info"].asString();
				Json::Value info;
				ParseJsonByString(strInfo, info);
				Json::Value jData = pParam->jRoot["data"];
				std::string strSubId = "";
				if (!jData.isNull())
				{
					strSubId = jData["subId"].asString();
				}
				if (!info.isNull())
				{
					std::string strDataHost = info["data"]["dataHost"].asString();
					strDataHost += "/ws";
					if (info["data"].isMember("cacheMode") && info["data"]["cacheMode"].asInt() > 0)
					{
						int nPageCount = info["data"]["pageCount"].asInt();
						std::string strCacheHost = info["data"]["cacheHost"].asString();
						ThreadRequestParam stParam;
						stParam.eRequestType = THREAD_REQUEST_CUSTOM;
						stParam.strID = "SubscribeCache";
						stParam.pThreadRuningFunc = static_request_subscribe_cache;
						stParam.bThread = true;
						stParam.bRepeat = false;
						stParam.AddParam("this", (INT_PTR)pThis);
						stParam.AddExtraJValue("nPageCount", nPageCount);
						stParam.AddExtraJValue("strCacheHost", strCacheHost);
						ThreadWrapper::GetInstance()->AddTask(stParam);

					}
					else
					{
						std::vector<std::string> vecInParam;
						vecInParam.push_back(strDataHost);
						vecInParam.push_back(pThis->m_pClient->m_sUserInfo.strUserToken);
						vecInParam.push_back(strSubId);
						//启动QWebsocket客户端
						ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
						pClient->iExecuteQtVsCommond("do_subscribe_client", &vecInParam, nullptr);
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			e.what();
		}
	}
}

bool SceneElementLoader::static_request_subscribe_cache(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam)
{
	SceneElementLoader* pThis = (SceneElementLoader*)pParam->sRequestParam.mapParams["this"];
	int nPageCount = pParam->sRequestParam.GetExtraJValue("nPageCount").asInt();
	std::string strCacheHost = pParam->sRequestParam.GetExtraJValue("strCacheHost").asString();
	pThis->getSubscribeCache(pParam, 1, nPageCount, strCacheHost);
	return true;
}

void SceneElementLoader::getSubscribeCache(ThreadCallbackParam* stCallbackParam, int nPageNo, int nPageSize, std::string strCacheHost)
{
	try
	{
		SceneElementLoader* pThis = (SceneElementLoader*)stCallbackParam->sRequestParam.mapParams["this"];
		char szBuffer[1024] = { 0 };
		std::string strProtocal = "http://";
		if (StartWith(m_strServerHost, "https"))
		{
			strProtocal = "https://";
		}
		sprintf_s(szBuffer, sizeof(szBuffer), "%s%s/tasker/api/subscribeData?pageNo=%d&userToken=%s", strProtocal.c_str(), strCacheHost.c_str(), nPageNo,
			m_pClient->m_sUserInfo.strUserToken.c_str());
		ThreadCallbackParam stCallback;
		ThreadRequestParam stParam;
		stParam.eRequestType = THREAD_REQUEST_API_GET;
		stParam.strUrl = szBuffer;
		ThreadWrapper::GetInstance()->DoNetRequest(stParam, &stCallback);
		if (stCallback.nResult == STATUS_SUCCESS)
		{
			std::string strData = stCallback.jRoot["data"].asString();
			std::string* pStrData = new std::string(strData);
			ParseSubscribeData(pStrData);
			if (nPageNo < nPageSize)
			{
				int nTempPage = nPageNo;
				nTempPage++;
				getSubscribeCache(stCallbackParam, nTempPage, nPageSize, strCacheHost);
			}
		}
	}
	catch (const std::exception&)
	{
	}
}

void SceneElementLoader::static_playerror_callback(IVideo* pIVideo,
	std::map<std::string, void*>& mapPlayExtraVoid,
	std::map<std::string, Json::Value>& mapPlayExtraJson)
{
	SceneElementLoader* pThis = (SceneElementLoader*)mapPlayExtraVoid["this"];
	ProjectBasedSceneElement* pInfo = (ProjectBasedSceneElement*)mapPlayExtraVoid["info"];
	bool bLivegbs = mapPlayExtraJson["isLivegbs"].asBool();
	pThis->iOnPlayingError(pIVideo, pInfo, bLivegbs);
}

void SceneElementLoader::iOnPlayingError(IVideo* pIVideo, ProjectBasedSceneElement* pInfo, bool isLivegbs)
{
	CommonLabelFactory* pFactory = nullptr;
	if (pInfo->strFloorNodeID.empty())
	{
		//室外
		pFactory = GetOutElementFactoryByType(pInfo->nTypeID);
	}
	else
	{
		pFactory = GetInElementFactoryByType(pInfo->nTypeID);
	}
	if (!pFactory)
	{
		return;
	}
	INode* SelfNode = (INode*)(pIVideo->iGetOriginNode()->iGetExternParam());
	if (!SelfNode)
	{
		return;
	}
	ProjectBasedSceneElementTypeInfo* pTypeInfo = GetElementTypeInfoByID(pInfo->nTypeID);
	if (!pTypeInfo)
	{
		return;
	}
	setDataMatch(SelfNode,pTypeInfo->mapDetailDataMatch,pInfo,true);
}


ProjectBasedSceneElementTypeInfo* SceneElementLoader::getElementTypeInfoByID(int nType, std::vector<ProjectBasedSceneElementTypeInfo*>* pVector)
{
	if (!pVector)
	{
		pVector = &m_vecSceneTypeInfo;
	}

	for (int i = 0; i < pVector->size(); i++)
	{
		if (pVector->at(i)->nTypeID == nType)
		{
			return pVector->at(i);
		}
	}

	return nullptr;
}

ProjectBasedSceneElementTypeInfo* SceneElementLoader::getElementTypeInfoByName(std::string strInnerName, std::vector<ProjectBasedSceneElementTypeInfo*>* pVector)
{
	if (!pVector)
	{
		pVector = &m_vecSceneTypeInfo;
	}
	for (int i = 0; i < pVector->size(); i++)
	{
		if (pVector->at(i)->strInnerName == strInnerName)
		{
			return pVector->at(i);
		}
	}

	return nullptr;
}

ProjectBasedSceneElementMapInfo* SceneElementLoader::getElementMapInfoByID(int nMapID, std::vector<ProjectBasedSceneElementMapInfo*>* pVector)
{
	if (!pVector)
	{
		pVector = &m_vecMapInfo;
	}
	for (int i = 0; i < pVector->size(); i++)
	{
		if (pVector->at(i)->nMapID == nMapID)
		{
			return pVector->at(i);
		}
	}

	return nullptr;
}

CommonLabelFactory* SceneElementLoader::doCreateFactory(std::vector<ProjectBasedSceneElementTypeInfo*>* pVector,
	int nPosition, int nType)
{
	ProjectBasedSceneElementTypeInfo* pTypeInfo = getElementTypeInfoByID(nType, pVector);
	if (!pTypeInfo)
	{
		std::string strMsg = "SceneElementLoader 未查找图层字典:" + to_string(nType);
		ProjectLogger::GetInstance()->InfoMessage(strMsg);
		return nullptr;
	}

	CommonLabelFactory* pFactory = NULL;
	bool bNew = false;
	if (nPosition == 0)
	{
		pFactory = (CommonLabelFactory*)GetOutElementFactoryByType(nType);
	}
	else if (nPosition == 1)
	{
		pFactory = (CommonLabelFactory*)GetInElementFactoryByType(nType);
	}

	if (!pFactory)
	{
		bNew = true;
		ReadParam stTempDetail;
		ParamParseStream::GetInstance()->ReadJsonToParam(stTempDetail, pTypeInfo->strDetailStyle);
		bool bDetail = stTempDetail.pParam ? true : false;
		if (pTypeInfo->vecStrLabelStyle.size() == 0)
		{
			return nullptr;
		}
		ReadParam stParam;
		ParamParseStream::GetInstance()->ReadJsonToParam(stParam, pTypeInfo->vecStrLabelStyle[0]);
		if (!stParam.pParam)
		{
			return nullptr;
		}
		pFactory = (CommonLabelFactory*)iCreateElementFactory(nType, stParam.nNodeType);
		pFactory->SetSceneElementLoader(this); 
		if (pFactory->IsInstanceFactory())
		{
			AddRefreshInstanceDataFactory((InstanceLabelFactory*)pFactory);
		}
		std::string strMsg = "SceneElementLoader 创建factory类型=:" + to_string(nType);
		ProjectLogger::GetInstance()->InfoMessage(strMsg);
		pFactory->SetServerHost(m_pClient->m_strApiHost);
		pFactory->SetType(nType);
		pFactory->SetRevertClick(pTypeInfo->Is3DOption_Revert_Click());
		bool bSmooth = pTypeInfo->Get3DOption_Smooth_Move();
		if (bSmooth)
		{
			pFactory->AddFunc(FUNC_SMOOTH_MOVE);
		}
		//重叠
		ReadParam strOverTap;
		ParamParseStream::GetInstance()->ReadJsonToParam(strOverTap, pTypeInfo->strOverTapStyle);
		bool bOverTap = strOverTap.pParam ? true : false;
		bool bOverTapHasEnable = pTypeInfo->Is3DOption_OverTap_Enable();
		if (bOverTap && bOverTapHasEnable)
		{
			pFactory->AddFunc(FUNC_OVERTAP);
			OverTapFunc *pFunc = (OverTapFunc*)(pFactory->GetFunc(FUNC_OVERTAP));
			pFunc->SetJudgeDis(pTypeInfo->Get3DOption_OverTap_JudgeDis());
		}
		//聚合
		ReadParam stClusterParam;
		ParamParseStream::GetInstance()->ReadJsonToParam(stClusterParam, pTypeInfo->strClusterStyle);
		bool bCluster = stClusterParam.pParam ? true : false;
		bool bClusterHasEnable = pTypeInfo->Is3DOption_Cluster_Enable();
		if (bCluster && !pFactory->GetFunc(FUNC_CLUSTER) && bClusterHasEnable)
		{
			pFactory->AddFunc(FUNC_CLUSTER);
		}
		bool bReadTifFrame = pTypeInfo->Is3DOption_ReadHeightTif_Frame();
		if (bReadTifFrame)
		{
			pFactory->AddFunc(FUNC_READ_HRIGHT_TIF);
		}
		m_vecElementFactory.push_back(pFactory);

		if (nPosition == 0)
		{
			m_mapOutElementFactory[nType] = pFactory;
		}
		else if (nPosition == 1)
		{
			m_mapInElementFactory[nType] = pFactory;
		}
	}

	iOnCreateFactory(pFactory);
	return pFactory;
}

CommonLabelFactory* SceneElementLoader::iCreateElementFactory(int nType,  int nNodeType)
{
	if (nNodeType == NODE_BOARD_INSTANCE)
	{
		return new SubInstanceLabelFactory(this);
	}
	else
	{
		return new SubLabelFactory(this);
	}
}

bool SceneElementLoader::checkIndoor(ProjectBasedSceneElement* pInfo)
{
	int nType = pInfo->nTypeID;
	int nMapID = pInfo->nMapID;
	ProjectBasedSceneElementMapInfo* pMapInfo = GetElementMapInfoByID(nMapID);
	if ((pMapInfo && !pMapInfo->strNodeName.empty()) || !pInfo->strFloorNodeID.empty())
	{
		//室内
		string strFloorNodeID = pInfo->strFloorNodeID;
		if (pMapInfo && !pMapInfo->strNodeName.empty())
		{
			strFloorNodeID = pMapInfo->strNodeName;
		}
		INode* pFloorNode = NULL;
		if (m_mapFloorNode.find(strFloorNodeID) != m_mapFloorNode.end())
		{
			pFloorNode = m_mapFloorNode[strFloorNodeID];
		}
		if (!pFloorNode)
		{
			pFloorNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(strFloorNodeID);
			if (pFloorNode)
			{
				m_mapFloorNode[strFloorNodeID] = pFloorNode;
			}
		}
		pInfo->strFloorNodeID = strFloorNodeID;
		pInfo->pFloorNode = pFloorNode;
		pInfo->bIndoor = true;
		return true;
	}
	pInfo->bIndoor = false;
	return false;
}

ProjectBasedSceneElement* SceneElementLoader::FindSceneElementByID(std::vector<ProjectBasedSceneElement*>& vecData, std::string strID, int nType, bool bErase)
{
	for (std::vector<ProjectBasedSceneElement*>::iterator it = vecData.begin(); it != vecData.end(); ++it)
	{
		if (nType == -1 || (*it)->nTypeID == nType)
		{
			if ((*it)->strUuid == strID)
			{
				ProjectBasedSceneElement* pInfo = (*it);
				if (bErase)
				{
					vecData.erase(it);
				}
				return pInfo;
			}
		}
	}
	return nullptr;
}

void SceneElementLoader::ClearElementShow()
{
	resetFactory();
	//清空队列里的
	std::queue<ProjectBasedSceneElement*> queueTemp1;
	m_queueAddElement.swap(queueTemp1);
	std::queue<ProjectBasedSceneElement*> queueTemp2;
	m_queueUpdateElement.swap(queueTemp2);

	for (int i = 0; i < m_vecResidentElement.size(); i++)
	{
		delete m_vecResidentElement[i];
		m_vecResidentElement[i] = NULL;
	}
	m_vecResidentElement.clear();
	createFactory(&m_vecSceneTypeInfo);
}

void SceneElementLoader::resetFactory()
{
	for (size_t i = 0; i < m_vecElementFactory.size(); i++)
	{
		m_vecElementFactory[i]->RefreshZero();
	}
}

void SceneElementLoader::createFactory(std::vector<ProjectBasedSceneElementTypeInfo*>* pVector, std::vector<int> vecNeedRefreshType)
{
	std::map<int, vector<ProjectBasedSceneElement*>> mapInElement;
	std::map<int, vector<ProjectBasedSceneElement*>> mapOutElement;
	classfyElement(mapInElement, mapOutElement);

	std::vector<std::map<int, std::vector<ProjectBasedSceneElement*>>> vec;
	vec.push_back(mapOutElement);
	vec.push_back(mapInElement);
	for (int i = 0; i < vec.size(); i++)
	{
		for (std::map<int, vector<ProjectBasedSceneElement*>>::iterator it = vec[i].begin(); it != vec[i].end(); it++)
		{
			int nType = it->first;
			if (!vecNeedRefreshType.empty() && find(vecNeedRefreshType.begin(), vecNeedRefreshType.end(), nType) == vecNeedRefreshType.end())
			{
				continue;
			}
			CommonLabelFactory* pFactory = doCreateFactory(pVector, i, nType);
			if (!pFactory)
			{
				continue;
			}
			std::string strMsg = "SceneElementLoader Factory 类型Type=" + to_string(nType);
			strMsg += ",刷新数量=" + to_string(it->second.size());
			strMsg += "," + (i == 0) ? "室外" : "室内";
			ProjectLogger::GetInstance()->InfoMessage(strMsg);
			iRefreshFactoryData(nType, pFactory, it->second);
		}
	}

	ChangeShowLabel();

	std::string str = "";
	str += "Factory_室外数量：";
	str += to_string(m_mapOutElementFactory.size());
	str += ",";
	str += "Factory_室内数量：";
	str += to_string(m_mapInElementFactory.size());
	ProjectLogger::GetInstance()->InfoMessage(str);
}

void SceneElementLoader::classfyElement(std::map<int, std::vector<ProjectBasedSceneElement*>>& mapInElement,
	std::map<int, vector<ProjectBasedSceneElement*>>& mapOutElement)
{
	std::vector<ProjectBasedSceneElement*> vecData;
	vecData.insert(vecData.end(), m_vecResidentElement.begin(), m_vecResidentElement.end());
	vecData.insert(vecData.end(), m_vecUdpSceneElement.begin(), m_vecUdpSceneElement.end());
	std::vector<ProjectBasedSceneElement*> vecTemp;
	GetExtraElement(vecTemp);
	vecData.insert(vecData.end(), vecTemp.begin(), vecTemp.end());
	//分类现在的
	for (int i = 0; i < vecData.size(); i++)
	{
		bool bIndoor = checkIndoor(vecData[i]);
		if (bIndoor)
		{
			//室内
			mapInElement[vecData[i]->nTypeID].push_back(vecData[i]);
		}
		else
		{
			mapOutElement[vecData[i]->nTypeID].push_back(vecData[i]);
		}
		bool bDoPostionPost = false;
		bDoPostionPost = iDoPositionPost(vecData[i], vecData[i]->vLabelPos, vecData[i]->pFloorNode);
		for (size_t j = 0; j < vecData[i]->vecPoint.size(); j++)
		{
			bDoPostionPost = iDoPositionPost(vecData[i], vecData[i]->vecPoint[j], vecData[i]->pFloorNode);
		}
		vecData[i]->bDoPositonPost = bDoPostionPost;
	}
}

void SceneElementLoader::GetExtraElement(std::vector<ProjectBasedSceneElement*>& vecExtraData, int nType)
{
	for (std::map<int, std::vector<ProjectBasedSceneElement*>>::iterator it = m_mapExtraSceneElement.begin(); it != m_mapExtraSceneElement.end(); it++)
	{
		if (it->first == nType || nType == -1)
		{
			vecExtraData.insert(vecExtraData.end(), it->second.begin(), it->second.end());
		}
	}
}

void SceneElementLoader::appendRefreshFactoryElementData(std::vector<ProjectBasedSceneElement*> vecSceneElement, bool bQueue)
{
	if (bQueue)
	{
		for (size_t i = 0; i < vecSceneElement.size(); ++i)
		{
			m_queueAddElement.push(vecSceneElement[i]);
		}
	}
	else
	{
		doingAppendElementData(vecSceneElement);
	}
}

void SceneElementLoader::updateRefreshFactoryElementData(std::vector<ProjectBasedSceneElement*> vecSceneElement)
{
	doingUpdateElementData(vecSceneElement);
}

void SceneElementLoader::doQueueAppendElementData()
{
	int nHandleCount = 0;
	std::vector<ProjectBasedSceneElement*> vecSceneElement;
	while (!m_queueAddElement.empty() && (nHandleCount < m_nPerFrameHandleLabelCount))
	{
		++nHandleCount;
		ProjectBasedSceneElement* pElement = m_queueAddElement.front();
		vecSceneElement.push_back(pElement);

		m_queueAddElement.pop();
	}

	if (vecSceneElement.size() > 0)
	{
		doingAppendElementData(vecSceneElement);
	}
}

void SceneElementLoader::doQueueUpdateElementData()
{
	int nHandleCount = 0;
	std::vector<ProjectBasedSceneElement*> vecSceneElement;
	while (!m_queueUpdateElement.empty() && (nHandleCount < m_nPerFrameHandleLabelCount))
	{
		++nHandleCount;
		ProjectBasedSceneElement* pElement = m_queueUpdateElement.front();
		vecSceneElement.push_back(pElement);

		m_queueUpdateElement.pop();
	}

	if (vecSceneElement.size() > 0)
	{
		doingUpdateElementData(vecSceneElement);
	}
}

void SceneElementLoader::doingAppendElementData(std::vector<ProjectBasedSceneElement*> vecSceneElement)
{
	std::map<int, std::vector<ProjectBasedSceneElement*>> mapOutData;
	std::map<int, std::vector<ProjectBasedSceneElement*>> mapInData;
	std::vector<int> vecTyps;
	for (size_t i = 0; i < vecSceneElement.size(); i++)
	{
		bool bIndoor = vecSceneElement[i]->bIndoor;
		if (bIndoor)
		{
			mapInData[vecSceneElement[i]->nTypeID].push_back(vecSceneElement[i]);
		}
		else
		{
			mapOutData[vecSceneElement[i]->nTypeID].push_back(vecSceneElement[i]);
		}
		if (find(vecTyps.begin(), vecTyps.end(), vecSceneElement[i]->nTypeID) == vecTyps.end())
		{
			vecTyps.push_back(vecSceneElement[i]->nTypeID);
		}

	}
	std::vector<std::map<int, std::vector<ProjectBasedSceneElement*>>> vec;
	vec.push_back(mapOutData);
	vec.push_back(mapInData);
	for (size_t i = 0; i < vec.size(); ++i)
	{
		for (std::map<int, std::vector<ProjectBasedSceneElement*>>::iterator it = vec[i].begin(); it != vec[i].end(); it++)
		{
			CommonLabelFactory* pFactory = nullptr;
			if (i == 0)
			{
				pFactory = GetOutElementFactoryByType(it->first);
			}
			else
			{
				pFactory = GetInElementFactoryByType(it->first);
			}
			if (!pFactory)
			{
				pFactory = doCreateFactory(&m_vecSceneTypeInfo, i, it->first);
			}
			if (!pFactory)
			{
				continue;
			}
			pFactory->AddData(it->second);
		}
	}
	if (vecSceneElement.size() > 0)
	{
		std::string strMsg = "SceneElementLoader doingAppendElementData,";
		strMsg += "数量=" + to_string(vecSceneElement.size());
		std::string strTypes = "";
		for (size_t i = 0; i < vecTyps.size(); ++i)
		{
			if (i > 0)
			{
				strTypes += ",";
			}
			strTypes += to_string(vecTyps[i]);
		}
		strMsg += ",类型=" + strTypes;
		ProjectLogger::GetInstance()->InfoMessage(strMsg);
	}
}

void SceneElementLoader::doingUpdateElementData(std::vector<ProjectBasedSceneElement*> vecSceneElement)
{
	std::vector<int> vecTyps;
	for (size_t i = 0; i < vecSceneElement.size(); i++)
	{
		bool bIndoor = vecSceneElement[i]->bIndoor;
		CommonLabelFactory* pFactory = nullptr;
		if (bIndoor)
		{
			pFactory = GetInElementFactoryByType(vecSceneElement[i]->nTypeID);
		}
		else
		{
			pFactory = GetOutElementFactoryByType(vecSceneElement[i]->nTypeID);
		}
		if (pFactory)
		{
			FactoryAssemble* pAssemble = pFactory->FindAssembleByData(vecSceneElement[i]);
			if (pAssemble)
			{
				std::vector<FactoryAssemble*> vec;
				vec.push_back(pAssemble);
				pFactory->UpdateData(vec);
			}
		}
		if (find(vecTyps.begin(), vecTyps.end(), vecSceneElement[i]->nTypeID) == vecTyps.end())
		{
			vecTyps.push_back(vecSceneElement[i]->nTypeID);
		}
	}
	if (vecSceneElement.size() > 0)
	{
		std::string strMsg = "SceneElementLoader doingUpdateElementData,";
		strMsg += "数量=" + to_string(vecSceneElement.size());
		std::string strTypes = "";
		for (size_t i = 0; i < vecTyps.size(); ++i)
		{
			if (i > 0)
			{
				strTypes += ",";
			}
			strTypes += to_string(vecTyps[i]);
		}
		strMsg += ",类型=" + strTypes;
		ProjectLogger::GetInstance()->InfoMessage(strMsg);
	}
}

void SceneElementLoader::GetAllOutElementFacotory(std::vector<CommonLabelFactory*>& vecFactory)
{
	for (std::map<int, CommonLabelFactory*>::iterator it = m_mapOutElementFactory.begin(); it != m_mapOutElementFactory.end(); it++)
	{
		vecFactory.push_back(it->second);
	}
}

void SceneElementLoader::GetAllInElementFacotory(std::vector<CommonLabelFactory*>& vecFactory)
{
	for (std::map<int, CommonLabelFactory*>::iterator it = m_mapInElementFactory.begin(); it != m_mapInElementFactory.end(); it++)
	{
		vecFactory.push_back(it->second);
	}
}

void SceneElementLoader::ChangeShowLabel()
{
	{
		std::vector<CommonLabelFactory*> vecFactory;
		GetAllOutElementFacotory(vecFactory);
		for (size_t i = 0; i < vecFactory.size(); i++)
		{
			if (!vecFactory[i]->IsChangeShowByLoader())
			{
				continue;
			}
			if (find(m_vecCurOutShowType.begin(), m_vecCurOutShowType.end(), to_string(vecFactory[i]->GetType())) != m_vecCurOutShowType.end())
			{
				vecFactory[i]->SetAllVisible(true);
			}
			else
			{
				vecFactory[i]->SetAllVisible(false);
			}
		}
	}

	{
		std::vector<CommonLabelFactory*> vecFactory;
		GetAllInElementFacotory(vecFactory);
		for (size_t i = 0; i < vecFactory.size(); i++)
		{
			if (!vecFactory[i]->IsChangeShowByLoader())
			{
				continue;
			}
			if (find(m_vecCurInShowType.begin(), m_vecCurInShowType.end(), to_string(vecFactory[i]->GetType())) != m_vecCurInShowType.end())
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

void SceneElementLoader::iRefreshFactoryData(int nType, CommonLabelFactory* pFactory, std::vector<ProjectBasedSceneElement*>& vecData)
{
	pFactory->RefreshData(&vecData);
}

ProjectBasedSceneElementTypeInfo* SceneElementLoader::GetElementTypeInfoByID(int nType)
{
	return getElementTypeInfoByID(nType, nullptr);
}

ProjectBasedSceneElementTypeInfo* SceneElementLoader::GetElementTypeInfoByName(std::string strInnerName)
{
	return getElementTypeInfoByName(strInnerName, nullptr);
}

ProjectBasedSceneElementMapInfo* SceneElementLoader::GetElementMapInfoByID(int nMapID)
{
	return getElementMapInfoByID(nMapID, nullptr);
}

vector<ProjectBasedSceneElementTypeInfo*> SceneElementLoader::GetAllSceneElementTypeInfo()
{
	return m_vecSceneTypeInfo;
}

void SceneElementLoader::GetOutSceneElementByType(int nType, vector<ProjectBasedSceneElement*>& vecData)
{
	std::vector<ProjectBasedSceneElement*> vecOrigin;
	vecOrigin.insert(vecOrigin.end(), m_vecResidentElement.begin(), m_vecResidentElement.end());
	vecOrigin.insert(vecOrigin.end(), m_vecUdpSceneElement.begin(), m_vecUdpSceneElement.end());
	std::vector<ProjectBasedSceneElement*> vecTemp;
	GetExtraElement(vecTemp);
	vecOrigin.insert(vecOrigin.end(), vecTemp.begin(), vecTemp.end());
	for (size_t i = 0; i < vecOrigin.size(); i++)
	{
		if (vecOrigin[i]->strFloorNodeID.empty() && vecOrigin[i]->nTypeID == nType)
		{
			vecData.push_back(vecOrigin[i]);
		}
	}
}

void SceneElementLoader::GetInSceneElementByType(int nType, std::vector<ProjectBasedSceneElement*>& vecData)
{
	std::vector<ProjectBasedSceneElement*> vecOrigin;
	vecOrigin.insert(vecOrigin.end(), m_vecResidentElement.begin(), m_vecResidentElement.end());
	vecOrigin.insert(vecOrigin.end(), m_vecUdpSceneElement.begin(), m_vecUdpSceneElement.end());
	std::vector<ProjectBasedSceneElement*> vecTemp;
	GetExtraElement(vecTemp);
	vecOrigin.insert(vecOrigin.end(), vecTemp.begin(), vecTemp.end());
	for (size_t i = 0; i < vecOrigin.size(); i++)
	{
		if (!vecOrigin[i]->strFloorNodeID.empty() && vecOrigin[i]->nTypeID == nType)
		{
			vecData.push_back(vecOrigin[i]);
		}
	}
}

void SceneElementLoader::GetOutAllSceneElement(std::vector<ProjectBasedSceneElement*>& vecData)
{
	std::vector<ProjectBasedSceneElement*> vecOrigin;
	vecOrigin.insert(vecOrigin.end(), m_vecResidentElement.begin(), m_vecResidentElement.end());
	vecOrigin.insert(vecOrigin.end(), m_vecUdpSceneElement.begin(), m_vecUdpSceneElement.end());
	std::vector<ProjectBasedSceneElement*> vecTemp;
	GetExtraElement(vecData);
	vecOrigin.insert(vecOrigin.end(), vecTemp.begin(), vecTemp.end());
	for (size_t i = 0; i < vecOrigin.size(); i++)
	{
		if (vecOrigin[i]->strFloorNodeID.empty())
		{
			vecData.push_back(vecOrigin[i]);
		}
	}
}

void SceneElementLoader::GetInAllSceneElement(std::vector<ProjectBasedSceneElement*>& vecData)
{
	std::vector<ProjectBasedSceneElement*> vecOrigin;
	vecOrigin.insert(vecOrigin.end(), m_vecResidentElement.begin(), m_vecResidentElement.end());
	vecOrigin.insert(vecOrigin.end(), m_vecUdpSceneElement.begin(), m_vecUdpSceneElement.end());
	std::vector<ProjectBasedSceneElement*> vecTemp;
	GetExtraElement(vecData);
	vecOrigin.insert(vecOrigin.end(), vecTemp.begin(), vecTemp.end());
	for (size_t i = 0; i < vecOrigin.size(); i++)
	{
		if (!vecOrigin[i]->strFloorNodeID.empty())
		{
			vecData.push_back(vecOrigin[i]);
		}
	}
}

CommonLabelFactory* SceneElementLoader::GetOutElementFactoryByType(int nType)
{

	if (m_mapOutElementFactory.find(nType) != m_mapOutElementFactory.end())
	{
		return m_mapOutElementFactory[nType];
	}
	return nullptr;
}

CommonLabelFactory* SceneElementLoader::GetInElementFactoryByType(int nType)
{
	if (m_mapInElementFactory.find(nType) != m_mapInElementFactory.end())
	{
		return m_mapInElementFactory[nType];
	}
	return nullptr;
}

void SceneElementLoader::FrameUpdate()
{
	for (size_t i = 0; i < m_vecElementFactory.size(); i++)
	{
		m_vecElementFactory[i]->FrameUpdate();
	}
	//交替添加或者更新元素
	static int s_LastReadType = 2;		//交替处理
	if (s_LastReadType == 2)
	{
		doQueueAppendElementData();
		s_LastReadType = 1;
	}
	else if (s_LastReadType == 1)
	{
		doQueueUpdateElementData();
		s_LastReadType = 2;
	}

	iFrameUpdate();

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
		//订阅上图的，只统计室外的
		std::vector<CommonLabelFactory*> vecFactory;
		GetAllOutElementFacotory(vecFactory);
		for (size_t i = 0; i < vecFactory.size(); i++)
		{
			if (m_pClient->m_vecStatisticLayer.empty() 
				|| find(m_pClient->m_vecStatisticLayer.begin(), m_pClient->m_vecStatisticLayer.end(), to_string(vecFactory[i]->GetType())) != m_pClient->m_vecStatisticLayer.end())
			{
				std::vector<ProjectBasedSceneElement*> vecData;
				vecFactory[i]->GetCurData(vecData);
				for (size_t j = 0; j < vecData.size(); j++)
				{
					ProjectBasedSceneElement* pInfo = (ProjectBasedSceneElement*)vecData[j];
					std::string strKey = to_string(vecFactory[i]->GetType());
					Json::Value jExtra = {};
					jExtra["extra"] = pInfo->strExtra;
					jExtra["id"] = pInfo->strUuid;
					char sz[1024] = { 0 };
					AsciiToUtf8(pInfo->strTitle.c_str(), sizeof(sz), sz);
					jExtra["name"] = sz;
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
}

EventReturnType_e	SceneElementLoader::iProcessEvent(const BCEvent& tEvent)
{
	for (size_t i = 0; i < m_vecElementFactory.size(); i++)
	{
		m_vecElementFactory[i]->iProcessEvent(tEvent);
	}
	return EventReturnType_e::NONE;
}

void SceneElementLoader::AddElement(const std::vector<ProjectBasedSceneElement*>& vecSceneElement, bool bNeedReadTif, bool bQueue /*= false*/)
{
	std::vector<int> vecType;
	std::map<int, CommonLabelFactory*> mapFactory;
	ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
	for (size_t i = 0; i < vecSceneElement.size(); i++)
	{
		vecSceneElement[i]->vSmoothPos = vecSceneElement[i]->vLabelPos;
		vecSceneElement[i]->vSmoothAngle = vecSceneElement[i]->vAngle;
		checkIndoor(vecSceneElement[i]);
		if (bNeedReadTif && !vecSceneElement[i]->bIndoor)
		{
			vecSceneElement[i]->vInitNodePos = vecSceneElement[i]->vLabelPos;
			vecSceneElement[i]->eReadTifStatus = READ_TIF_NEED_NOT_YET_ADD;
		}
		if (find(vecType.begin(), vecType.end(), vecSceneElement[i]->nTypeID) == vecType.end())
		{
			vecType.push_back(vecSceneElement[i]->nTypeID);
		}
		m_mapExtraSceneElement[vecSceneElement[i]->nTypeID].push_back(vecSceneElement[i]);
	}
	std::string strMsg = "SceneElementLoader AddElement直接ProjectBasedSceneElement,";
	strMsg += "数量=" + to_string(vecSceneElement.size());
	std::string strTypes = "";
	for (size_t i = 0; i < vecType.size(); ++i)
	{
		if (i != 0)
		{
			strTypes += ",";
		}
		strTypes += to_string(vecType[i]);
	}
	strMsg += ",类型:" + strTypes;
	ProjectLogger::GetInstance()->InfoMessage(strMsg);

	//额外的则自行管理,不使用队列方式,可外部做队列添加
	appendRefreshFactoryElementData(vecSceneElement, bQueue);
	//createFactory(&m_vecSceneTypeInfo, vecType);
}

void SceneElementLoader::ClearExtraData(int nType, bool bRefreshFactory /*= true*/)
{
	m_mapExtraSceneElement[nType].clear();
	CommonLabelFactory* pFactory = GetOutElementFactoryByType(nType);
	if (!pFactory)
	{
		pFactory = GetInElementFactoryByType(nType);
	}
	if (bRefreshFactory)
	{
		if (pFactory)
		{
			pFactory->RefreshZero();
		}
		std::vector<int> vecType;
		vecType.push_back(nType);
		createFactory(&m_vecSceneTypeInfo, vecType);
	}
}

void SceneElementLoader::ClearUdpData(int nType, bool bRefreshFactory /*= true*/)
{
	CommonLabelFactory* pFactory = GetOutElementFactoryByType(nType);
	if (!pFactory)
	{
		pFactory = GetInElementFactoryByType(nType);
	}
	//if (bRefreshFactory)
	{
		if (pFactory)
		{
			pFactory->RefreshZero();
		}
	}
	//删除数据
	for (std::vector<ProjectBasedSceneElement*>::iterator it = m_vecUdpSceneElement.begin(); it != m_vecUdpSceneElement.end();)
	{
		if ((*it)->nTypeID == nType)
		{
			ProjectBasedSceneElement* pInfo = (*it);
			m_vecUdpSceneElement.erase(it);
			DELETE_PTR(pInfo);
		}
		else
		{
			it++;
		}
	}
	//if (bRefreshFactory)
	{
		std::vector<int> vecType;
		vecType.push_back(nType);
		createFactory(&m_vecSceneTypeInfo, vecType);
	}
}

void SceneElementLoader::GetUdpElement(std::vector<ProjectBasedSceneElement*>& vecUdpData, int nType /*= -1*/)
{
	for (size_t i = 0; i < m_vecUdpSceneElement.size(); ++i)
	{
		if (m_vecUdpSceneElement[i]->nTypeID == nType || nType == -1)
		{
			vecUdpData.emplace_back(m_vecUdpSceneElement[i]);
		}
	}
}

void SceneElementLoader::AddExclusiveFactory(std::string strGroupKey, int nType)
{
	if (find(m_mapExclusiveFactoryType[strGroupKey].begin(), m_mapExclusiveFactoryType[strGroupKey].end(), nType) == m_mapExclusiveFactoryType[strGroupKey].end())
	{
		m_mapExclusiveFactoryType[strGroupKey].push_back(nType);
	}
}

void SceneElementLoader::AddDefaultExclusiveFactory(int nType)
{
	AddExclusiveFactory(DEFAULT_EXCLUSIVE_KEY,nType);
}

void SceneElementLoader::RemoveExclusiveFactory(std::string strGroupKey, int nType)
{
	std::vector<int>& vec = m_mapExclusiveFactoryType[strGroupKey];
	for (std::vector<int>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		vec.erase(it);
		break;
	}
}

void SceneElementLoader::RemoveDefaultExclusiveFactory(int nType)
{
	RemoveExclusiveFactory(DEFAULT_EXCLUSIVE_KEY,nType);
}

void SceneElementLoader::ClearExclusiveFactory(const std::string& strGroupKey)
{
	m_mapExclusiveFactoryType[strGroupKey].clear();
}

void SceneElementLoader::ClearDefaultExclusiveFactory()
{
	ClearExclusiveFactory(DEFAULT_EXCLUSIVE_KEY);
}

void SceneElementLoader::ClearResidentElement(int nType)
{
	CommonLabelFactory* pFactory = GetOutElementFactoryByType(nType);
	if (pFactory)
	{
		pFactory->RefreshZero();
	}
	for (std::vector<ProjectBasedSceneElement*>::iterator it = m_vecResidentElement.begin(); it != m_vecResidentElement.end(); )
	{
		if ((*it)->nTypeID == nType)
		{
			//移除
			ProjectBasedSceneElement* pInfo = (*it);
			m_vecResidentElement.erase(it);
			DELETE_PTR(pInfo);
		}
		else
		{
			it++;
		}
	}
}

void SceneElementLoader::AddRefreshInstanceDataFactory(InstanceLabelFactory* pFactory)
{
	m_RefreshInstanceFactoryLock.Lock();
	if (find(m_vecRefreshInstanceFactory.begin(), m_vecRefreshInstanceFactory.end(), pFactory) == m_vecRefreshInstanceFactory.end())
	{
		m_vecRefreshInstanceFactory.push_back(pFactory);
	}
	m_RefreshInstanceFactoryLock.UnLock();
}

void SceneElementLoader::RemoveRefreshInstanceDataFactory(InstanceLabelFactory* pFactory)
{
	m_RefreshInstanceFactoryLock.Lock();
	std::vector<InstanceLabelFactory*>::iterator it = find(m_vecRefreshInstanceFactory.begin(), m_vecRefreshInstanceFactory.end(), pFactory);
	if (it != m_vecRefreshInstanceFactory.end())
	{
		m_vecRefreshInstanceFactory.erase(it);
	}
	m_RefreshInstanceFactoryLock.UnLock();
}

bool SceneElementLoader::IsRefreshInstanceDataFactory(InstanceLabelFactory* pFactory)
{
	bool bRet = true;
	m_RefreshInstanceFactoryLock.Lock();
	std::vector<InstanceLabelFactory*>::iterator it = find(m_vecRefreshInstanceFactory.begin(), m_vecRefreshInstanceFactory.end(), pFactory);
	if (it == m_vecRefreshInstanceFactory.end())
	{
		bRet = false;
	}
	m_RefreshInstanceFactoryLock.UnLock();
	return bRet;
}


void SceneElementLoader::doThreadRefreshInstanceFactory()
{
	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_CUSTOM;
	stParam.AddParam("this", (INT_PTR)this);
	stParam.pThreadRuningFunc = static_request_refreshInstanceFactory;
	stParam.pThreadCompleteFunc = static_callback_refreshInstanceFactory;
	stParam.strID = "doThreadRefreshInstanceFactory";
	stParam.bRepeat = false;
	stParam.bThread = true;
	stParam.bRunCallbackInMainThread = true;
	ThreadWrapper::GetInstance()->AddTask(stParam);
}

bool SceneElementLoader::static_request_refreshInstanceFactory(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam* stCallbackParam)
{
	SceneElementLoader* pThis = (SceneElementLoader*)mapParams["this"];
	while (pThis->m_bThreadRefreshInstanceRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		pThis->m_RefreshInstanceFactoryLock.Lock();
		std::vector<InstanceLabelFactory*> vecInstanceFactory = pThis->m_vecRefreshInstanceFactory;
		pThis->m_RefreshInstanceFactoryLock.UnLock();
		for (auto pInstanceFactory : vecInstanceFactory)
		{
			pInstanceFactory->ThreadFilterShowAssemble();
		}

		//给到主线程执行
		ThreadCallbackParam* pCallbackParam = new ThreadCallbackParam;
		pCallbackParam->sRequestParam = stCallbackParam->sRequestParam;
		pCallbackParam->sRequestParam.bRepeat = false;
		Guid::CreateGuId(pCallbackParam->sRequestParam.strID, "Thread_");
		ThreadWrapper::GetInstance()->AddMainRunCallback(pCallbackParam);
	}

	return true;
}

void SceneElementLoader::static_callback_refreshInstanceFactory(ThreadCallbackParam* pParam)
{
	SceneElementLoader* pThis = (SceneElementLoader*)pParam->sRequestParam.GetParam("this");
	pThis->m_RefreshInstanceFactoryLock.Lock();
	std::vector<InstanceLabelFactory*> vecInstanceFactory = pThis->m_vecRefreshInstanceFactory;
	pThis->m_RefreshInstanceFactoryLock.UnLock();
	for (auto pInstanceFactory : vecInstanceFactory)
	{
		if (pThis->IsRefreshInstanceDataFactory(pInstanceFactory))
		{
			pInstanceFactory->HandleRefreshInstanceData();
		}
	}
}

