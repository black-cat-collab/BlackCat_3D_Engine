#include "ProjectBasedHeader.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/webApi/WebPoiLabelFactory.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/utils/animate/EasingAnimate.h"
#include "Project-Based/utils/animate/BaseAnimateObject.h"
#include "Project-Based/utils/animate/TranslateAnimateObject.h"
#include "Project-Based/utils/AnimateWithNodeMg.h"
#include "Project-Based/webApi/WebLabelParser.h"
#include "Project-Based/label/LabelDetailFactory.h"
#include "Project-Based/utils/ParamNetImage.h"
#include "Project-Based/utils/tools/RoadAuxiliaryTool.h"
#include "Project-Based/utils/TweenAnimateForNode.h"
#include "Project-Based/label/func/SmoothMoveFunc.h"

using namespace bc;

WebPoiLabelFactory::WebPoiLabelFactory() :
	CommonLabelFactory()
{
	m_bNodeCanReuse = false;
	GetDetailFactory()->SetNodeCanReuse(false);
	m_pRoadAuxiliaryTool = new RoadAuxiliaryTool(m_pClient); 
	SetContainerGroup(m_pClient->iGetApiGroupNode());
}

WebPoiLabelFactory::~WebPoiLabelFactory()
{
	DeleteAllPoi();
}

INode* WebPoiLabelFactory::iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pData)
{
	INode* pNode = nullptr;
	ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)pData;
	if (!pInfo->pLabelParam)
	{
		return nullptr;
	}
	if (pInfo->pLabelParam->nNodeType == NODE_POLYGON)
	{
		PolygonParam* pParam = (PolygonParam*)pInfo->pLabelParam;
		//多指取最大的z值
		float fMaxz = 0;
		for (int i = 0; i < pInfo->vecPoint.size(); i++)
		{
			if (i == 0)
			{
				fMaxz = pInfo->vecPoint[i].z;
			}
			if (pInfo->vecPoint[i].z > fMaxz)
			{
				fMaxz = pInfo->vecPoint[i].z;
			}
		}
		ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
		for (int i = 0; i < pInfo->vecPoint.size(); i++)
		{
			Vector3d &vPos = pInfo->vecPoint[i];
			pParam->vecVertex.push_back(vPos);
		}
	}
	else if (pInfo->pLabelParam->nNodeType == NODE_ROAD)
	{
		RoadParam* pParam = (RoadParam*)pInfo->pLabelParam;
		pParam->vecVertex.push_back(ConvertVecVector3dTo(pInfo->vecPoint));
	}
	else if (pInfo->pLabelParam->nNodeType == NODE_LOCUS_LINE)
	{
		//LocusLineParam* pParam = (LocusLineParam*)pInfo->pLabelParam;
		//float fMaxz = 0;
		//for (int i = 0; i < pInfo->vecPoints.size(); i++)
		//{
		//	if (i == 0)
		//	{
		//		fMaxz = pInfo->vecPoints[i].z;
		//	}
		//	if (pInfo->vecPoints[i].z > fMaxz)
		//	{
		//		fMaxz = pInfo->vecPoints[i].z;
		//	}
		//}
		//vector<Vector3d> vecPos;
		//for (int i = 0; i < pInfo->vecPoints.size(); i++)
		//{
		//	Vector3d vPos = pInfo->vecPoints[i];
		//	vPos.z = fMaxz;
		//	vecPos.push_back(vPos);
		//}
		//if (pParam->vecSegment.size() > 0)
		//{
		//	pParam->vecSegment[0].vecVertex = vecPos;
		//}

		//if (pParam->vecSegment.size() > 0)
		//{
		//	pParam->vecSegment[0].vecVertex = pInfo->vecPoints;
		//}
	}
	else if (pInfo->pLabelParam->nNodeType == NODE_FLYLINE)
	{
		FlyLineParam* pParam = (FlyLineParam*)pInfo->pLabelParam;
		pParam->vecVertex.clear();
		if (pInfo->vecPoint.size() >= 2)
		{
			Vector3d vCenter = (pInfo->vecPoint[0] + pInfo->vecPoint[1]) / 2.0f;
			ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
			pClient->AdjustZ(vCenter, pInfo->vecPoint[0].Distance(pInfo->vecPoint[1]) / 2, true);
			pParam->vecVertex.push_back(pInfo->vecPoint[1]);
			pParam->vecVertex.push_back(vCenter);
			pParam->vecVertex.push_back(pInfo->vecPoint[0]);
		}
	}

	pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(*pInfo->pLabelParam);
	pNode->iSetVisible(false);
	bool bNetLoadModel = ParamNetImage::GetInstance()->CheckNetLoadModel(pNode);
	if (!bNetLoadModel)
	{
		pNode->iLoadModel();
	}
	else
	{
		//模型则添加到队列进行下载检测，所以暂不加载Model
		ParamNetImage::GetInstance()->AddModelTask(pNode, m_strServerHost);
	}
	pNode->iSetExternParam((INT_PTR)pInfo);
	pNode->iSetSaveMyself(false);
	GetContainerGroup()->iAddChild(pNode);
	if (pNode->iGetNodeType() == NODE_POLYGON)
	{
		PolygonParam* pParam = (PolygonParam*)pInfo->pLabelParam;
		pNode->GetDynamicComponent<IRenderNode>()->iSetHoverEnable(pParam->bHoverEnable);
		pNode->GetDynamicComponent<IRenderNode>()->iSetClickEnable(pParam->bClickEnable);
	}
	if (pNode->iGetFlagName() == EFFECT_TYPE_CONE_TRIANGLE
		|| pNode->iGetFlagName() == EFFECT_TYPE_CONE_ROUND)
	{
		//三角锥，圆锥动画效果
		TranslateAnimateParam sAnimateParam;
		//上
		Vector3ProValue sValue;
		sValue.nEasingType = EasingType_CircularLinearInterpolation;
		sValue.vStart = Vector3d(0, 0, -1.0f);
		sValue.vEnd = Vector3d(0, 0, 1.0f);
		sValue.nAllTimeMill = 700;
		sAnimateParam.vecValue.push_back(sValue);
		//下
		sValue.vStart = sValue.vEnd;
		sValue.vEnd = Vector3d(0, 0, -1.0f);
		sValue.nAllTimeMill = 1300;
		sAnimateParam.vecValue.push_back(sValue);

		BaseAnimateObject* pAnimateObject = AnimateWithNodeMg::GetInstance()->AddAnimate(pNode, &sAnimateParam, "effect_type_cone");
		pAnimateObject->GetEasingAnimate()->SetLoop(true, -1);
	}

	return pNode;
}

void WebPoiLabelFactory::iUpdateDataForLabel(FactoryAssemble* pAssemble)
{
	ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)pAssemble->pData;
	bool bCanSmooth = GetFunc(FUNC_SMOOTH_MOVE) && pInfo->bCanSmooth;
	if (pAssemble->pLabelNode && !bCanSmooth)
	{
		IBoard* pIBoard = pAssemble->pLabelNode->GetDynamicComponent<IBoard>();
		if (pIBoard && !pInfo->vAngle.Empty())
		{
			Vector3d vAngle;
			vAngle = Vector3d(pIBoard->iGetBoardParam()->vModelAngle) + pInfo->vAngle;
			pAssemble->pLabelNode->iSetAngles(vAngle);
		}
	}
	SetTextShowByStrKey(pAssemble->pLabelNode, "title", pInfo->strTitle);
	setText(pAssemble->pLabelNode, pInfo->j3DTextMatchObject);
}

INode* WebPoiLabelFactory::iCreateNodeForDetail(int nType, ProjectBasedSceneElement* pData)
{
	INode* pNode = nullptr;
	ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)pData;
	if (pInfo->pDetailParam)
	{
		pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(*pInfo->pDetailParam);
		pNode->iSetVisible(false);
		pNode->iLoadModel();
		pNode->iSetExternParam((INT_PTR)pInfo);
		pNode->iSetSaveMyself(false);
		GetContainerGroup()->iAddChild(pNode);
	}

	return pNode;
}

void WebPoiLabelFactory::iOpenForDetail(FactotryAssembleDetail* pAssemble)
{
	if (!pAssemble->pSelfNode)
	{
		return;
	}
	ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)pAssemble->pData;
	IBoard* pIBoard = pAssemble->pSelfNode->GetDynamicComponent<IBoard>();
	if (pIBoard)
	{
		std::vector<INode*> vecVideo;
		pIBoard->iGetVideoBoard(vecVideo);
		if (vecVideo.size() > 0)
		{
			IVideo* pVideo = vecVideo[0]->GetDynamicComponent<IVideo>();
			if (pVideo)
			{
				if (pInfo->bLivegbs)
				{
					pVideo->iStopVideo();
					pVideo->iSetVideoStream(pInfo->strLiveUrl, pInfo->strDeviceID, pInfo->strChannelID);
					pVideo->iPlayVideo();
				}
				else
				{
					if (StartWith(pInfo->strLiveUrl, "resource"))
					{
						//使用本地的地址
						pInfo->strLiveUrl = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir
							+ "/" + pInfo->strLiveUrl;

					}
					pVideo->iSetVideoStream(pInfo->strLiveUrl);
					pVideo->iPlayVideo();
				}
			}
		}
	}
}

void WebPoiLabelFactory::iUpdateDataForDetail(FactotryAssembleDetail* pAssemble)
{
	if (!pAssemble->pSelfNode)
	{
		return;
	}
	ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)pAssemble->pData;
	IBoard* pIBoard = pAssemble->pSelfNode->GetDynamicComponent<IBoard>();
	if (pIBoard)
	{
		setText(pAssemble->pSelfNode, pInfo->j3DTextMatchObject);
	}
}

void WebPoiLabelFactory::iCloseForDetail(FactotryAssembleDetail* pAssemble)
{
	if (!pAssemble->pSelfNode)
	{
		return;
	}
	IBoard* pIBoard = pAssemble->pSelfNode->GetDynamicComponent<IBoard>();
	if (pIBoard)
	{
		std::vector<INode*> vecVideo;
		pIBoard->iGetVideoBoard(vecVideo);
		if (vecVideo.size() > 0)
		{
			IVideo* pVideo = vecVideo[0]->GetDynamicComponent<IVideo>();
			if (pVideo)
			{
				pVideo->iStopVideo();
			}
		}
	}
}

void WebPoiLabelFactory::DeletePoi(std::string strID)
{
	std::vector<FactoryAssemble*> vecAssemble;
	GetCurAssemble(vecAssemble);
	for (size_t i = 0; i < vecAssemble.size(); ++i)
	{
		if (vecAssemble[i]->pData->strUuid == strID)
		{
			ProjectBasedWebLabelData *pInfo = (ProjectBasedWebLabelData*)RemoveByAssemble(vecAssemble[i]);
			DELETE_PTR(pInfo);
			break;
		}
	}
	for (std::map<std::string, INode*>::iterator it = m_mapExtraAddNode.begin(); it != m_mapExtraAddNode.end(); ++it)
	{
		if (it->first == strID)
		{
			ParamNetImage::GetInstance()->RemoveNetTask(it->second);
			AnimateWithNodeMg::GetInstance()->RemoveAnimate(it->second);
			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&it->second, false);
			m_mapExtraAddNode.erase(it);
			break;
		}
	}
}

void WebPoiLabelFactory::DeleteAllPoi()
{	
	std::vector<ProjectBasedSceneElement*> vecData = m_vecCurData;
	RefreshZero();
	for (size_t i = 0; i < vecData.size(); ++i)
	{
		//平滑移动的删除
		SmoothMoveFunc* pFunc = (SmoothMoveFunc*)GetFunc(FUNC_SMOOTH_MOVE);
		if (pFunc)
		{
			pFunc->RemoveTweenByID(vecData[i]->strUuid);
		}
		DELETE_PTR(vecData[i]);
	}
	vecData.clear();

	for (std::map<std::string, INode*>::iterator it = m_mapExtraAddNode.begin(); it != m_mapExtraAddNode.end();)
	{
		ParamNetImage::GetInstance()->RemoveNetTask(it->second);
		AnimateWithNodeMg::GetInstance()->RemoveAnimate(it->second);
		m_queueDeleteNode.push(it->second);
		it++;
	}
	m_mapExtraAddNode.clear();
}

void WebPoiLabelFactory::CreatePoi(ProjectBasedWebLabelData* pInfo)
{
	DeletePoi(pInfo->strUuid);
	std::vector<ProjectBasedSceneElement*> vecData;
	vecData.push_back(pInfo);
	AddData(vecData);
}

void WebPoiLabelFactory::UpdatePoi(ProjectBasedWebLabelData* pInfo)
{
	FactoryAssemble* pAssemble = nullptr;
	for (size_t i = 0; i < m_vecCurAssemble.size(); i++)
	{
		ProjectBasedWebLabelData* pTemp = (ProjectBasedWebLabelData*)m_vecCurAssemble[i]->pData;
		if (pTemp->strUuid == pInfo->strUuid)
		{
			pAssemble = m_vecCurAssemble[i];
			break;
		}
	}
	if (!pAssemble || !pAssemble->pLabelNode)
	{
		return;
	}
	std::vector<FactoryAssemble*> vecAssemble;
	vecAssemble.push_back(pAssemble);
	UpdateData(vecAssemble);
}

void WebPoiLabelFactory::AddExtraNode(std::string strID, INode* pNode)
{
	if (!pNode)
	{
		return;
	}
	if (m_mapExtraAddNode.find(strID) == m_mapExtraAddNode.end())
	{
		m_mapExtraAddNode[strID] = pNode;
	}
	else
	{
		ParamNetImage::GetInstance()->RemoveNetTask(m_mapExtraAddNode[strID]);
		AnimateWithNodeMg::GetInstance()->RemoveAnimate(m_mapExtraAddNode[strID]);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_mapExtraAddNode[strID], false);
		m_mapExtraAddNode.erase(strID);
		m_mapExtraAddNode[strID] = pNode;
	}
}

void WebPoiLabelFactory::iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{
	m_pClient->iOnWebLabelClickLabel(pAssemble,this,bSelect);
}

void WebPoiLabelFactory::iOnSetDetailSelect(FactotryAssembleDetail* pAssemble, std::string strFlagName, bool bSelect)
{
	m_pClient->iOnWebDetailClickLabel(pAssemble, this, strFlagName,bSelect);
}

void WebPoiLabelFactory::iDoWebCommond(WebCommandData* pWebData)
{
	CommonLabelFactory::iDoWebCommond(pWebData);
	if (pWebData->strCommand == "Web_DeleteAllElement")
	{
		DeleteAllPoi();
	}
	else if (pWebData->strCommand == "Web_DeleteAnyElement")
	{
		if (pWebData->vecValues.size() >= 1)
		{
			std::string strId = pWebData->vecValues[0].asString();
			std::vector<FactoryAssemble*> vecAssemble;
			GetCurAssemble(vecAssemble);
			for (size_t m = 0; m < vecAssemble.size(); m++)
			{
				std::string& strTempID = vecAssemble[m]->pData->strUuid;
				if (strTempID == strId)
				{
					RemoveByAssemble(vecAssemble[m]);

					return;
				}
			}
		}
	}
	//路线绘制
	else if (pWebData->strCommand == "Web_StartDrawRoad")
	{
		if (pWebData->vecValues.size() >= 1)
		{
			Json::Value jParam = pWebData->vecValues[0];
			if (!jParam.isNull() && jParam.isObject())
			{
				m_pRoadAuxiliaryTool->SetLineEnable(false);
				RoadParam* pRoadParam = (RoadParam*) m_pClient->m_pWebLabelParser->ParseNodeParam(jParam);
				m_pRoadAuxiliaryTool->SetRoadParam(*pRoadParam);
				m_pRoadAuxiliaryTool->SetLineEnable(true);
			}
		}
	}
	else if (pWebData->strCommand == "Web_EndDrawRoad")
	{
		if (pWebData->vecValues.size() >= 2)
		{
			std::string strId = pWebData->vecValues[0].asString();
			bool bCreate = pWebData->vecValues[1].asBool();
			m_pRoadAuxiliaryTool->SetLineEnable(false);
			if (!bCreate)
			{
				m_pClient->ToSendWebCommond("VS_EndDrawRoad", pWebData->strID, "\'Cancel\'");
				return;
			}
			std::vector<Vector3d> vecPos = m_pRoadAuxiliaryTool->GetPosForRoad();
			if (vecPos.size() == 0)
			{
				m_pClient->ToSendWebCommond("VS_EndDrawRoad", pWebData->strID, "\'Error\'");
				return;
			}
			RoadParam tParam;
			m_pRoadAuxiliaryTool->GetRoadParam(tParam);
			tParam.vecVertex.push_back(ConvertVecVector3dTo(vecPos));
			Json::Value jPos = Json::arrayValue;
			for (int i = 0; i < vecPos.size(); i++)
			{
				double dLng = 0.0, dLat = 0.0;
				Vector3d vPos(vecPos[i]);
				m_pClient->ScenePosToWGS(vPos, dLng, dLat);
				Json::Value jValue;
				jValue = std::to_string(vecPos[i].x) + "," +
					std::to_string(vecPos[i].y) + "," +
					std::to_string(vecPos[i].z) + "," +
					std::to_string(dLng) + "," +
					std::to_string(dLat) + "," +
					std::to_string(vecPos[i].z);
				jPos.append(jValue);
			}
			vecPos.clear();
			m_pRoadAuxiliaryTool->CreateRoadNode(&tParam);
			INode* pNode = m_pRoadAuxiliaryTool->GetCurRoadNode();
			AddExtraNode(strId, pNode);
			Json::Value jResult = Json::objectValue;
			jResult["ID"] = strId;
			jResult["Points"] = jPos;
			m_pClient->ToSendWebCommond("VS_EndDrawRoad", pWebData->strID, jResult.toStyledString());
		}
	}
	//创建路线
	else if (pWebData->strCommand == "Web_CreateRoad")
	{
		if (pWebData->vecValues.size() >= 2)
		{
			Json::Value jParam = pWebData->vecValues[0];
			if (jParam.isNull() || !jParam.isObject())
			{
				return;
			}
			m_pRoadAuxiliaryTool->GetPosForRoad().clear();
			RoadParam* pRoadParam = (RoadParam*) m_pClient->m_pWebLabelParser->ParseNodeParam(jParam);
			m_pRoadAuxiliaryTool->SetRoadParam(*pRoadParam);
			Json::Value jPointData = pWebData->vecValues[1];
			std::string strID = jPointData["strID"].asString();
			std::string strName = jPointData["strName"].asString();
			bool bWGS84 = jPointData["bWGS84"].asBool();
			bool bUseHeightTif = jPointData["bUseHeightTif"].asBool();
			std::vector<Vector3> vecPos;
			for (int i = 0; i < jPointData["vecPos"].size(); i++)
			{
				Vector3d vPos = m_pClient->m_pWebLabelParser->ParseVector3(jPointData["vecPos"][i]);
				if (bWGS84)
				{
					m_pClient->ConvertToVector3WithCoorSystem(vPos.x, vPos.y, &vPos, EElementCoordinateType::COOR_WGS84, bUseHeightTif);
				}
				vecPos.push_back(vPos);
			}
			RoadParam tParam;
			m_pRoadAuxiliaryTool->GetRoadParam(tParam);
			tParam.vecVertex.push_back(vecPos);
			m_pRoadAuxiliaryTool->CreateRoadNode(&tParam);
			INode* pNode = m_pRoadAuxiliaryTool->GetCurRoadNode();
			AddExtraNode(strID, pNode);
			Json::Value jResult = Json::objectValue;
			jResult["ID"] = strID;
			std::vector<std::vector<Vector3>> vecControllPos;
			pNode->GetDynamicComponent<IRoad>()->iGetControlPoints(vecControllPos);
			Json::Value jPos = Json::arrayValue;
			for (int i = 0; i < vecControllPos[0].size(); i++)
			{
				double dLng = 0.0, dLat = 0.0;
				Vector3d vPos(vecPos[i]);
				m_pClient->ScenePosToWGS(vPos, dLng, dLat);
				Json::Value jValue;
				jValue = std::to_string(vecPos[i].x) + "," +
					std::to_string(vecPos[i].y) + "," +
					std::to_string(vecPos[i].z) + "," +
					std::to_string(dLng) + "," +
					std::to_string(dLat) + "," +
					std::to_string(vecPos[i].z);
				jPos.append(jValue);
			}
			jResult["Points"] = jPos;
			m_pClient->ToSendWebCommond("VS_CreateRoad", pWebData->strID, jResult.toStyledString());
		}
	}
	else if (pWebData->strCommand == "Web_UpdateRoad")
	{
		if (pWebData->vecValues.size() >= 2)
		{
			std::string strId = pWebData->vecValues[0].asString();
			bool bEditor = pWebData->vecValues[1].asBool();
			if (m_mapExtraAddNode.find(strId) != m_mapExtraAddNode.end())
			{
				INode* pNode = m_mapExtraAddNode[strId];
				Json::Value jPos = Json::arrayValue;
				Json::Value jResult = Json::objectValue;
				if (pNode && pNode->GetDynamicComponent<IRoad>())
				{
					RoadParam* pParam = (RoadParam*)pNode->iGetNodeParam();
					pNode->GetDynamicComponent<IRoad>()->iSetEditor(bEditor);
					std::vector<std::vector<Vector3>> vecPoints;
					pNode->GetDynamicComponent<IRoad>()->iGetControlPoints(vecPoints);
					if (vecPoints.size() > 0)
					{
						for (int i = 0; i < vecPoints[0].size(); i++)
						{
							double dLng = 0.0, dLat = 0.0;
							Vector3d vPos(vecPoints[0][i]);
							m_pClient->ScenePosToWGS(vPos, dLng, dLat);
							Json::Value jValue;
							jValue = std::to_string(vecPoints[0][i].x) + "," +
								std::to_string(vecPoints[0][i].y) + "," +
								std::to_string(vecPoints[0][i].z) + "," +
								std::to_string(dLng) + "," +
								std::to_string(dLat) + "," +
								std::to_string(vecPoints[0][i].z);
							jPos.append(jValue);

						}
					}
					jResult["strId"] = strId;
					jResult["State"] = true;
					jResult["Points"] = jPos;
					m_pClient->ToSendWebCommond("VS_UpdateRoad", pWebData->strID, jResult.toStyledString());
				}
				else
				{
					jResult["strId"] = strId;
					jResult["State"] = false;
					jResult["Points"] = jPos;
					m_pClient->ToSendWebCommond("VS_UpdateRoad", pWebData->strID, jResult.toStyledString());
				}
			}
		}
	}
}

void WebPoiLabelFactory::FrameUpdate()
{
	CommonLabelFactory::FrameUpdate();

	int nCount = 0;
	while (!m_queueDeleteNode.empty() && nCount < 100)
	{
		INode* pNode = m_queueDeleteNode.front();
		ParamNetImage::GetInstance()->RemoveNetTask(pNode);
		AnimateWithNodeMg::GetInstance()->RemoveAnimate(pNode);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&pNode);
		m_queueDeleteNode.pop();

		nCount++;
	}
}

EventReturnType_e WebPoiLabelFactory::iProcessEvent(const BCEvent& tEvent)
{
	CommonLabelFactory::iProcessEvent(tEvent);

	m_pRoadAuxiliaryTool->iProcessEvent(tEvent);
	return EventReturnType_e::NONE;
}

void WebPoiLabelFactory::RefreshZero()
{
	for (size_t i = 0; i < m_vecCurAssemble.size(); ++i)
	{
		setAssembleVisible(m_vecCurAssemble[i], false);
		m_vecCurAssemble[i]->pData = nullptr;
		m_vecCurAssemble[i]->pLabelNode = nullptr;
		m_vecCurAssemble[i]->nType = -1;
		m_vecCurAssemble[i]->bModify.store(true);
	};
	//移到空闲中
	m_vecIdleAssemble.insert(m_vecIdleAssemble.end(), m_vecCurAssemble.begin(), m_vecCurAssemble.end());
	m_vecCurAssemble.clear();
	for (auto& it : m_mapIdleNode)
	{
		std::vector<INode*>& vecUsedNode = m_mapUsedNode[it.first];
		if (m_bNodeCanReuse)
		{
			it.second.insert(it.second.end(), vecUsedNode.begin(), vecUsedNode.end());
		}
		else
		{
			//直接删除
			for (size_t i = 0; i < vecUsedNode.size(); ++i)
			{
				ParamNetImage::GetInstance()->RemoveNetTask(vecUsedNode[i]);
				AnimateWithNodeMg::GetInstance()->RemoveAnimate(vecUsedNode[i]);
				m_queueDeleteNode.push(vecUsedNode[i]);
			}
		}
		vecUsedNode.clear();
	}

	m_vecCurData.clear();

	//临时显隐的去除
	m_nTemporaryState = 0;
	m_vecHideOrShowLabelTempID.clear();
}

