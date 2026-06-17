#include "ProjectBasedHeader.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/utils/ProjectLogger.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/module/IStateModule.h"
#include "Project-Based/module/BaseModule.h"
#include "Project-Based/module/RoamPathModule.h"
#include "Project-Based/utils/TweenAnimateForNode.h"

using namespace bc;

RoamPathModule::RoamPathModule(const std::string& strModuleName)
	: BaseModule(strModuleName),
	m_bEnable(false),
	m_eState(State_e::STATE_NONE),
	m_pLocusLine(nullptr),
	m_pLine(nullptr)
{
	m_tLocusLineParam.stDefaultSegment.vColor = Vector4(0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1);
	m_tLocusLineParam.stDefaultSegment.fWidth = 3.0f;
}

RoamPathModule::~RoamPathModule()
{
}

std::string RoamPathModule::iGetModuleName()
{
	return m_strModuleName;
}

INode* RoamPathModule::iGetRootNode()
{
	return APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode();
}

void RoamPathModule::iStateChangeUpdate(IStateManager* pStateManager)
{
}

void RoamPathModule::iDataChangeUpdate(IStateManager* pStateManager)
{
}

void RoamPathModule::iFrameUpdate(IStateManager* pStateManager)
{
	for (auto pTween : m_mapTween)
	{
		pTween.second->Update();
	}
}

void RoamPathModule::iInitializeScene(IStateManager* pStateManager)
{
}

void RoamPathModule::iSceneLoad(IStateManager* pStateManager)
{
	BaseModule::iSceneLoad(pStateManager);
}

bc::EventReturnType_e RoamPathModule::iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent)
{
	BaseModule::iProcessEvent(pStateManager, tEvent);
	if (!m_bEditMode)
	{
		return EventReturnType_e::NONE;
	}
	if (!m_bEnable)
	{
		return EventReturnType_e::NONE;
	}
	ISystemAPI* pSystemAPI = APIProvider::GetSystemAPI();
	INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
	ICamera* pMainCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
	IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));
	if (tEvent.eMessage == EVENT_MOUSE)
	{
		if (!APIProvider::GetSystemAPI()->iProjectClientAPI->iCanHandleScene(Vector2(tEvent.nParam2, tEvent.nParam3)))
		{
			return EventReturnType_e::NONE;
		}
		switch (tEvent.nParam1)
		{
		case MOUSE_LBUTTONDOWN:
		{
			m_vLastMousePos = Vector2(tEvent.nParam2, tEvent.nParam3);
		}
		break;
		case MOUSE_LBUTTONUP:
		{
			if (m_eState != STATE_END)
			{
				Vector2 vCurMousePos = Vector2(tEvent.nParam2, tEvent.nParam3);
				if (m_vLastMousePos.Distance(vCurMousePos) > 3)
				{
					return EventReturnType_e::NONE;
				}
				if (m_vecLinePoint.size() >= 1)
				{
					//处理云渲染最后一个重复点
					Vector3d vPos = pManipulator->iGetMousePoint();
					if (vPos.Distance(m_vecLinePoint.back()) > 0.5f)
					{
						m_vecLinePoint.push_back(pManipulator->iGetMousePoint());
					}
				}
				else
				{
					m_vecLinePoint.push_back(pManipulator->iGetMousePoint());
				}
				if (m_vecLinePoint.size() == 1)
				{
					//设置开始
					m_eState = STATE_MOVE;
				}
			}
		}
		break;
		case MOUSE_LBUTTONDBLCLK:
		{
			if (m_eState == STATE_END)
			{
				return EventReturnType_e::NONE;
			}
			m_eState = STATE_END;
			if (m_pLocusLine)
			{
				//m_vecLinePoint.push_back(pManipulator->iGetMousePoint());
				if (m_vecLinePoint.size() > 1)
				{
					std::string strMsg = "";
					for (size_t i = 0; i < m_vecLinePoint.size(); i++)
					{
						std::string strTemp = "\'" +
							std::to_string(m_vecLinePoint[i].x) + "," +
							std::to_string(m_vecLinePoint[i].y) + "," +
							std::to_string(m_vecLinePoint[i].z) + "\'";
						if (i != (m_vecLinePoint.size() - 1))
						{
							strTemp += ",";
						}
						strMsg += strTemp;
					}
					strMsg.insert(strMsg.begin(), '[');
					strMsg.push_back(']');
					m_pClient->ToSendWebCommond("VS_DrawRoute", m_pLocusLine->iGetName(), strMsg.c_str());
				}
			}
		}break;
		case MOUSE_RBUTTONDOWN:
		{
			if (!pManipulator->iIsSwitchRBMB())
			{
				m_eState = STATE_NONE;
				Cancel();
			}
		}
		break;
		case MOUSE_MOVE:
		{
			if (m_eState == STATE_MOVE)
			{
				std::vector<Vector3d> vecPoint = m_vecLinePoint;
				vecPoint.push_back(pManipulator->iGetMousePoint());
				//更新线
				ILocusLine* pLocus = GetComponent<ILocusLine>(m_pLocusLine);
				if (pLocus)
				{
					m_pLocusLine->iSetVisible(true);
					vector<LocusLineSegment> vecSegment;
					LocusLineSegment sSeg = m_tLocusLineParam.stDefaultSegment;
					sSeg.vecVertex = ConvertVecVector3dTo(vecPoint);
					vecSegment.push_back(sSeg);
					pLocus->iUpdateLocusLineSegment(vecSegment);
					m_pLocusLine->iSetVisible(true);
				}
			}
		}
		break;
		default:
			break;
		}
	}
	else if (tEvent.eMessage == EVENT_KEY)
	{
		if (APIProvider::GetSystemAPI()->iProjectClientAPI
			&& APIProvider::GetSystemAPI()->iProjectClientAPI->iCanRespondKey())
		{
			int nKeyCode = tEvent.nParam1;
			if (nKeyCode == 27)
			{
				//ESC 取消
				m_eState = STATE_NONE;
				Cancel();
			}
		}
	}
	return EventReturnType_e::NONE;
}

bc::EventReturnType_e RoamPathModule::iExecuteEvent(IStateManager* pStateManager, const BCEvent& tEvent)
{
	BaseModule::iExecuteEvent(pStateManager, tEvent);
	return EventReturnType_e::NONE;
}

void RoamPathModule::iRefreshData()
{
	getAllRoamPaths(true);
}

void RoamPathModule::iDoWebCommond(WebCommandData* pWebData)
{
	if (!m_bEditMode)
	{
		return;
	}
	try
	{
		if (pWebData->strCommand == "Web_DrawRoute")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				if (pWebData->vecValues[0].asInt() == 1)
				{
					SetLineEnable(true);
					if (m_pLocusLine)
					{
						m_pLocusLine->iSetName(pWebData->strID);
						m_pLocusLine->iSetVisible(true);
					}
					if (m_pLine)
					{
						m_pLine->iSetVisible(false);
					}
				}
			}
		}
		else if (pWebData->strCommand == "Web_ClearRoute")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				if (pWebData->vecValues[0].asInt() == 1)
				{
					ILocusLine* pLocus = GetComponent<ILocusLine>(m_pLine);
					if (pLocus)
					{
						vector<LocusLineSegment> vecSegment;
						LocusLineSegment sSeg = m_tLocusLineParam.stDefaultSegment;
						sSeg.vecVertex.clear();
						vecSegment.push_back(sSeg);
						pLocus->iUpdateLocusLineSegment(vecSegment);
						m_pLine->iSetVisible(false);
					}
					ILocusLine* pLocusLine = GetComponent<ILocusLine>(m_pLocusLine);
					if (pLocusLine)
					{
						vector<LocusLineSegment> vecSegment;
						LocusLineSegment sSeg = m_tLocusLineParam.stDefaultSegment;
						sSeg.vecVertex.clear();
						vecSegment.push_back(sSeg);
						pLocusLine->iUpdateLocusLineSegment(vecSegment);
						m_pLocusLine->iSetVisible(false);
					}
					SetLineEnable(false);
				}
			}
		}
		else if (pWebData->strCommand == "Web_PlayRoute")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				if (pWebData->vecValues[0].asInt() == 1)
				{
					if (m_pLocusLine)
					{
						m_pLocusLine->iSetVisible(false);
					}
					std::string strRoamId = pWebData->vecValues[1].asString();
					m_strCurrentRoamId = strRoamId;
					if (m_mapTween.find(strRoamId) != m_mapTween.end() && m_mapPathParam.find(strRoamId) != m_mapPathParam.end())
					{
						if (m_mapPathParam[strRoamId]->m_nLoop == -1)
						{
							m_mapTween[strRoamId]->SetLoop(true);
						}
						else
						{
							m_mapTween[strRoamId]->SetLoop(false, m_mapPathParam[strRoamId]->m_nLoop);
						}
						m_mapTween[strRoamId]->ResetPosition();
						m_mapTween[strRoamId]->SetSpeed(m_mapPathParam[strRoamId]->m_fSpeed);
						m_mapTween[strRoamId]->Start();
						ICamera* pCamera = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->GetDynamicComponent<ICamera>();
						pCamera->iSetOperateType(OPERATE_TYPE_NO_OPERATE);
					}
				}
			}
		}
		else if (pWebData->strCommand == "Web_PauseRoute")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				if (pWebData->vecValues[0].asInt() == 1)
				{
					if (m_mapTween.find(m_strCurrentRoamId) != m_mapTween.end())
					{
						m_mapTween[m_strCurrentRoamId]->Pause();
						ICamera* pCamera = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->GetDynamicComponent<ICamera>();
						pCamera->iSetOperateType(OPERATE_TYPE_SELECT);
					}
				}
			}
		}
		else if (pWebData->strCommand == "Web_ResumeRoute")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				if (pWebData->vecValues[0].asInt() == 1)
				{
					if (m_mapTween.find(m_strCurrentRoamId) != m_mapTween.end())
					{
						m_mapTween[m_strCurrentRoamId]->Start();
						ICamera* pCamera = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->GetDynamicComponent<ICamera>();
						pCamera->iSetOperateType(OPERATE_TYPE_NO_OPERATE);
					}
				}
			}
		}
		else if (pWebData->strCommand == "Web_StopRoute")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				if (pWebData->vecValues[0].asInt() == 1)
				{
					if (m_mapTween.find(m_strCurrentRoamId) != m_mapTween.end())
					{
						m_mapTween[m_strCurrentRoamId]->Stop();
						ICamera* pCamera = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->GetDynamicComponent<ICamera>();
						pCamera->iSetOperateType(OPERATE_TYPE_SELECT);
					}
				}
			}
		}
		else if (pWebData->strCommand == "Web_UpdateRoute")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				if (pWebData->vecValues[0].asInt() == 1)
				{
					getAllRoamPaths(true);
				}
			}
		}
		else if (pWebData->strCommand == "Web_StartEditRoute")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				if (pWebData->vecValues[0].asInt() == 1)
				{
					std::string strId = pWebData->vecValues[1].asString();
					if (m_mapPathParam.find(strId) != m_mapPathParam.end())
					{
						std::vector<Vector3d> vecPoint = m_mapPathParam[strId]->m_vecPoints;
						if (!m_pLine)
						{
							m_tLocusLineParam.bDepthTest = true;
							m_pLine = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(m_tLocusLineParam);
							m_pLine->iLoadModel();
							m_pLine->iSetVisible(false);
							m_pLine->iSetSaveMyself(false);
							m_pModuleGroupNode->iAddChild(m_pLine);
						}
						ILocusLine* pLocus = GetComponent<ILocusLine>(m_pLine);
						if (pLocus)
						{
							vector<LocusLineSegment> vecSegment;
							LocusLineSegment sSeg = m_tLocusLineParam.stDefaultSegment;
							sSeg.vecVertex = ConvertVecVector3dTo(vecPoint);
							vecSegment.push_back(sSeg);
							pLocus->iUpdateLocusLineSegment(vecSegment);
							m_pLine->iSetVisible(true);
							if (vecPoint.size() >= 2)
							{
								INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
								Vector3d vOrigin = (vecPoint.front() + vecPoint.back()) / 2;
								Vector3d vAngle = pCameraNode->iGetAngles();
								ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
								pClient->AdjustZ(vOrigin, vecPoint.front().Distance(vecPoint.back()) / 2 * tan(3.1415926 / 180 * 70),true);
								vAngle.x = 84.0f;
								GoViewPoint(pCameraNode, vOrigin, vAngle, false);
							}
						}
					}
				}
			}
		}
		else if (pWebData->strCommand == "Web_EndEditRoute")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				if (pWebData->vecValues[0].asInt() == 1)
				{
					ILocusLine* pLocus = GetComponent<ILocusLine>(m_pLine);
					if (pLocus)
					{
						vector<LocusLineSegment> vecSegment;
						LocusLineSegment sSeg = m_tLocusLineParam.stDefaultSegment;
						sSeg.vecVertex.clear();
						vecSegment.push_back(sSeg);
						pLocus->iUpdateLocusLineSegment(vecSegment);
						m_pLine->iSetVisible(false);
					}
					ILocusLine* pLocusLine = GetComponent<ILocusLine>(m_pLocusLine);
					if (pLocusLine)
					{
						vector<LocusLineSegment> vecSegment;
						LocusLineSegment sSeg = m_tLocusLineParam.stDefaultSegment;
						sSeg.vecVertex.clear();
						vecSegment.push_back(sSeg);
						pLocusLine->iUpdateLocusLineSegment(vecSegment);
						m_pLocusLine->iSetVisible(false);
					}
					SetLineEnable(false);
				}
			}
		}
	}
	catch (const std::exception&)
	{
	}
}

void RoamPathModule::getAllRoamPaths(bool bThread, ThreadCallbackParam* pCallback)
{
	ThreadRequestParam requestParam;
	requestParam.strID = "getAllRoamPaths";
	ThreadWrapper::GetInstance()->StopNetTask(requestParam.strID);
	requestParam.bThread = bThread;
	requestParam.bRepeat = false;
	requestParam.eRequestType = ThreadRequestType_e::THREAD_REQUEST_API_GET;
	char szBuffer[1024] = { 0 };
	sprintf_s(szBuffer, sizeof(szBuffer), "%s/api/tz/roam/list?pageNo=1&pageSize=2000", m_pClient->m_strApiHost.c_str());
	requestParam.strUrl = szBuffer;
	requestParam.pThreadCompleteFunc = static_CallbackAllRoamPaths;
	requestParam.bRunCallbackInMainThread = true;
	m_pClient->AddHttpCommonParam(&requestParam);
	requestParam.AddParam("this", (INT_PTR)this);
	m_pClient->AddHttpCommonParam(&requestParam);
	ThreadWrapper::GetInstance()->AddTask(requestParam);
}

void RoamPathModule::static_CallbackAllRoamPaths(ThreadCallbackParam* pParam)
{
	if (pParam->nResult == StatusCode::STATUS_SUCCESS)
	{
		RoamPathModule* pThis = (RoamPathModule*)pParam->sRequestParam.mapParams["this"];
		pThis->m_mapPathParam.clear();
		try
		{
			Json::Value jData = pParam->jRoot["data"];
			if (!jData.isNull() && jData.isArray() && jData.size() > 0)
			{
				//TweenAnimateForNode
				for (int i = 0; i < jData.size(); i++)
				{
					RoamPathParam* pPathParam = new RoamPathParam;
					pPathParam->m_fAngle = jData[i]["angle"].asFloat();
					pPathParam->m_fHeight = jData[i]["height"].asFloat();
					pPathParam->m_strId = jData[i]["id"].asString();
					pPathParam->m_nLoop = jData[i]["loop"].asInt();
					pPathParam->m_strName = jData[i]["name"].asString();
					pPathParam->m_fSpeed = jData[i]["speed"].asFloat();
					pPathParam->m_fTransitionTime = jData[i]["time"].asFloat();
					Json::Value jPoints = jData[i]["path"];
					if (!jPoints.isNull() && jPoints.isArray() && jPoints.size() > 0)
					{
						for (int j = 0; j < jPoints.size(); j++)
						{
							std::string strPoint = jPoints[j].asString();
							Vector3d vPos;
							sscanf_s(strPoint.c_str(), "%lf,%lf,%lf", &vPos.x, &vPos.y, &vPos.z);
							pThis->m_pClient->AdjustZ(vPos, pPathParam->m_fHeight,true);
							pPathParam->m_vecPoints.push_back(vPos);
						}
					}
					pThis->m_mapPathParam[pPathParam->m_strId] = pPathParam;
					TweenAnimateForNode* pTween = new TweenAnimateForNode(APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera());
					std::vector<PointData> vecPointData;
					for (size_t i = 0; i < pPathParam->m_vecPoints.size(); i++)
					{
						PointData pointData;
						pointData.fSpeed = pPathParam->m_fSpeed;
						pointData.vPos = pPathParam->m_vecPoints[i];
						pointData.vAngle = Vector3d(0, 0, 0);
						vecPointData.push_back(pointData);
					}
					pTween->SetMoveCallback(static_move);
					pTween->SetCalcAngleCallback(static_caclAngleCallbake);
					pTween->SetPoints(vecPointData);
					Json::Value jAngle;
					jAngle["angle"] = pPathParam->m_fAngle;
					pTween->SetExtraJValue("angle", jAngle);
					pThis->m_mapTween[pPathParam->m_strId] = pTween;
				}
			}
		}
		catch (const std::exception&)
		{
		}
	}
}

void RoamPathModule::SetLineEnable(bool bEnable)
{
	m_bEnable = bEnable;
	if (!m_bEnable)
	{
		Cancel();
	}
	if (!m_pLocusLine)
	{
		//创建线的节点
		m_tLocusLineParam.bDepthTest = true;
		m_pLocusLine = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(m_tLocusLineParam);
		m_pLocusLine->iLoadModel();
		m_pLocusLine->iSetVisible(false);
		m_pLocusLine->iSetSaveMyself(false);
		m_pModuleGroupNode->iAddChild(m_pLocusLine);
	}
}

void RoamPathModule::Cancel()
{
	if (m_pLocusLine)
	{
		m_pLocusLine->iSetVisible(false);
	}
	m_vecLinePoint.clear();
	m_vecLinePoint.resize(0);
	m_eState = STATE_NONE;
}

bc::INode* RoamPathModule::iGetModuleGroupNode()
{
	return m_pModuleGroupNode;
}

void RoamPathModule::static_move(TweenAnimateForNode* pTween, INode* pTarget, Vector3d vPos, Vector3d vAngle, bool bPerEndPoint)
{
	pTarget->iSetOrigin(vPos);
	float fAngle = pTween->GetExtraJValue("angle")["angle"].asFloat();
	vAngle.x = -fAngle;
	pTarget->iSetAngles(vAngle);
}
Vector3d RoamPathModule::static_caclAngleCallbake(TweenAnimateForNode* pTween, Vector3d vCurOrigin, Vector3d vPreOrigin)
{
	Vector3d vAngle;
	vAngle = CalcAngle(vPreOrigin, vCurOrigin);
	vAngle.x = 10.0f;
	vAngle.z = 0;
	return vAngle;
}

void RoamPathModule::iOnAppModeChanged(int nAppMode)
{
	INode* pGroup = iGetModuleGroupNode();
	if (pGroup)
	{
		if (nAppMode & APP_MODE_ROUTE_EDIT)
		{
			pGroup->iSetVisible(true);
			m_bEditMode = true;
		}
		else
		{
			pGroup->iSetVisible(false);
			m_bEditMode = false;
		}
		//清除绘制的线
		ILocusLine* pLocus = GetComponent<ILocusLine>(m_pLine);
		if (pLocus)
		{
			vector<LocusLineSegment> vecSegment;
			LocusLineSegment sSeg = m_tLocusLineParam.stDefaultSegment;
			sSeg.vecVertex.clear();
			vecSegment.push_back(sSeg);
			pLocus->iUpdateLocusLineSegment(vecSegment);
			m_pLine->iSetVisible(false);
		}
		ILocusLine* pLocusLine = GetComponent<ILocusLine>(m_pLocusLine);
		if (pLocusLine)
		{
			vector<LocusLineSegment> vecSegment;
			LocusLineSegment sSeg = m_tLocusLineParam.stDefaultSegment;
			sSeg.vecVertex.clear();
			vecSegment.push_back(sSeg);
			pLocusLine->iUpdateLocusLineSegment(vecSegment);
			m_pLocusLine->iSetVisible(false);
		}
		m_bEnable = false;
	}
}

