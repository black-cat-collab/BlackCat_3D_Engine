#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/webApi/WebCommonApi.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/base/HeatmapFactory.h"
#include "Project-Based/utils/ProjectLogger.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/utils/tools/BaseStatisticsTool.h"
#include "Project-Based/utils/tools/CircleStatisticsTool.h"
#include "Project-Based/utils/tools/LineStatisticsTool.h"
#include "Project-Based/utils/tools/PolygonStatisticsTool.h"
#include "Project-Based/utils/tools/AreaMeasureTool.h"
#include "Project-Based/utils/tools/DistanceMeasureTool.h"
#include "Project-Based/utils/tools/ViewshedAnalysisTool.h"
#include "Project-Based/utils/TweenAnimateForNode.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/module/IStateModule.h"
#include "Project-Based/module/BaseModule.h"
#include "Project-Based/module/YBSSModule.h"
#include "Project-Based/webApi/WebLabelParser.h"
#include "Project-Based/base/SceneElementLoader.h"
#include "Project-Based/webApi/WebPoiLabelFactory.h"
#include "Project-Based/base/BuildIndoorStateFactory.h"
#include "Project-Based/Factory/BuildIndoorFactory.h"

#ifdef _MSC_VER
#include <process.h>
#include <tchar.h>
#endif



using namespace bc;

WebCommonApi::WebCommonApi(ISystemAPI* pSystemAPI, ProjectBasedClient* pClient) :
	m_bRotateAroundCenter(false),
	m_fAroundCenterSpeed(0.01f),
	m_fCurAroundCenterAngle(0.0f),
	m_bRoamLoop(false)
{
	this->m_pClient = pClient;
}

WebCommonApi::~WebCommonApi()
{
}

void WebCommonApi::iDoWebCommond(WebCommandData* pWebData)
{
	try
	{
		if (pWebData->strCommand == "Web_GoToViewport")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
				std::string strViewPort = pWebData->vecValues[0].asString();
				bool bSky = pWebData->vecValues[1].asInt();
				GoViewPoint(pCameraNode, strViewPort, bSky);
			}
		}
		else if (pWebData->strCommand == "Web_GoLookAtTargetPos")
		{
			if (pWebData->vecValues.size() >= 3)
			{
				INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
				Json::Value& jParam = pWebData->vecValues[0];
				bool bWgs84 = pWebData->vecValues[1].asBool();
				bool bSky = pWebData->vecValues[2].asBool();
				Vector3d vPos = m_pClient->m_pWebLabelParser->ParseVector3(jParam["pos"]);
				Vector2 vAngle = m_pClient->m_pWebLabelParser->ParseVector2(jParam["angle"]);
				Vector3d vCameraAngle = Vector3d(vAngle.x, vAngle.y, 0);
				if (vCameraAngle.Empty())
				{
					vCameraAngle = Vector3d(45, 45, 0);
				}
				float fDis = jParam["distance"].asFloat();
				if (bWgs84)
				{
					m_pClient->ConvertLngToVector3(vPos.x,vPos.y,&vPos,false);
				}
				float fOffsetX = jParam["offsetX"].asFloat();
				float fOffsetY = jParam["offsetY"].asFloat();
				Vector2 vOffset(fOffsetX, fOffsetY);
				GoViewPoint(pCameraNode, vPos, vCameraAngle, fDis, vOffset, bSky, false);
			}
		}
		else if (pWebData->strCommand == "Web_GoToViewportWithLngLat")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
				Json::Value& jParam = pWebData->vecValues[0];
				bool bSky = pWebData->vecValues[1].asBool();
				Vector3d vPos = m_pClient->m_pWebLabelParser->ParseVector3(jParam["pos"]);
				Vector2 vAngle = m_pClient->m_pWebLabelParser->ParseVector2(jParam["angle"]);
				m_pClient->ConvertLngToVector3(vPos.x, vPos.y, &vPos, false);
				Vector3d vCameraAngle = Vector3d(vAngle.x, vAngle.y, 0);
				GoViewPoint(pCameraNode, vPos, vCameraAngle, bSky);
			}
		}
		else if (pWebData->strCommand == "Web_AddHeatmap" || pWebData->strCommand == "Web_UpdateHeatmap")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				std::string strKey = pWebData->vecValues[0].asString();
				Json::Value& jData = pWebData->vecValues[1];
				HeatmapItemData* pHeatmapItem = m_pClient->m_pHeatMapFactory->GetHeatmapItem(strKey);
				if (!pHeatmapItem)
				{
					HeatmapItemData stItem;
					stItem.strKey = strKey;
					m_pClient->m_pHeatMapFactory->AddHeatmap(stItem);
					pHeatmapItem = m_pClient->m_pHeatMapFactory->GetHeatmapItem(strKey);
				}
				if (!pHeatmapItem)
				{
					return;
				}
				pHeatmapItem->vecExtraHeatmapData.clear();
				pHeatmapItem->vecBoundaryData.clear();
				Json::Value& jPointArray = jData["pointData"];
				for (int i = 0; i < jPointArray.size(); i++)
				{
					ExtraHeatmapData sPoint;
					sPoint.fSize = jPointArray[i]["fSize"].asFloat();
					sPoint.nWeight = jPointArray[i]["nWeight"].asInt();
					sPoint.strKey = strKey;
					bool bWGS84 = jPointArray[i]["bWGS84"].asBool();
					bool bUseTif = jPointArray[i]["bUseHeightTif"].asBool();
					Vector3d vPos = m_pClient->m_pWebLabelParser->ParseVector3(jPointArray[i]["vPos"]);
					if (bWGS84)
					{
						m_pClient->ConvertLngToVector3(vPos.x, vPos.y, &vPos, bUseTif);
					}
					sPoint.vPos = vPos;

					pHeatmapItem->vecExtraHeatmapData.push_back(sPoint);
				}
				Json::Value& jboundaryArray = jData["boundaryData"];
				for (int i = 0; i < jboundaryArray.size(); i++)
				{
					bool bWGS84 = jboundaryArray[i]["bWGS84"].asBool();
					bool bUseTif = jboundaryArray[i]["bUseHeightTif"].asBool();
					Vector3d vPos = m_pClient->m_pWebLabelParser->ParseVector3(jboundaryArray[i]["vPos"]);
					if (bWGS84)
					{
						m_pClient->ConvertLngToVector3(vPos.x, vPos.y, &vPos, bUseTif);
					}
					pHeatmapItem->vecBoundaryData.push_back(vPos);
				}
				Vector4 vColor = m_pClient->m_pWebLabelParser->ParseColor(jData["boundaryColor"]);
				pHeatmapItem->vBoundaryColor = Vector3d(vColor.x, vColor.y, vColor.z);
				pHeatmapItem->fBoundaryWidth = jData["fboundaryLineWidth"].asFloat();

				m_pClient->m_pHeatMapFactory->RefreshHeatmapItem();
			}
		}
		else if (pWebData->strCommand == "Web_DeleteHeatmap")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strKey = pWebData->vecValues[0].asString();
				m_pClient->m_pHeatMapFactory->RemoveHeatmap(strKey);
			}
		}
		else if (pWebData->strCommand == "Web_DeleteAllHeatmap")
		{
			m_pClient->m_pHeatMapFactory->RemoveAllHeatmap();
		}
		else if (pWebData->strCommand == "Web_ShowHeatmap")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				std::string strShowKey = pWebData->vecValues[0].asString();
				std::vector<std::string> vecKeys;
				bool bIsShow = pWebData->vecValues[1].asBool();
				SplitStringBySpecial(strShowKey, vecKeys, ",");
				m_pClient->m_pHeatMapFactory->SetAreaVisible(vecKeys, bIsShow);
			}
		}
		else if (pWebData->strCommand == "Web_ShowHeatmapOnlyArea")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strShowKey = pWebData->vecValues[0].asString();
				std::vector<std::string> vecKeys;
				SplitStringBySpecial(strShowKey, vecKeys, ",");
				m_pClient->m_pHeatMapFactory->ShowOnlyAreas(vecKeys);
			}
		}
		else if (pWebData->strCommand == "Web_SetHeatmapMaxIntensity")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				std::string strKey = pWebData->vecValues[0].asString();
				float fValue = pWebData->vecValues[1].asFloat();
				std::vector<HeatmapItemData*> vec;
				m_pClient->m_pHeatMapFactory->GetAllHeatmapItems(vec);
				for (size_t i = 0; i < vec.size(); ++i)
				{
					vec[i]->fIntensityMax = fValue;
				}
				m_pClient->m_pHeatMapFactory->UpdateAndSetDataChanged();

				//HeatmapItemData* pHeatmapItem = m_pClient->m_pHeatMapFactory->GetHeatmapItem(strKey);
				//if (pHeatmapItem)
				//{
				//	pHeatmapItem->fIntensityMax = fValue;
				//	m_pClient->m_pHeatMapFactory->RefreshHeatmapItem();
				//}
			}
		}
		else if (pWebData->strCommand == "Web_PlayRoam")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				std::string strName = pWebData->vecValues[0].asString();
				bool bLoop = pWebData->vecValues[1].asBool();
				char szName[512] = { 0 };
				Utf8ToAscii(strName.c_str(), sizeof(szName), szName);
				INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
				ICamera* pCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
				pCamera->iPlayAnimationPath(szName, bLoop ? PATH_PLAY_LOOP_SINGLE : PATH_PLAY_NOLOOP_SINGLE, true);

				m_strCurRoamPath = szName;
				m_bRoamLoop = bLoop;
			}
		}
		else if (pWebData->strCommand == "Web_PauseRoam")
		{
			INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
			ICamera* pCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
			pCamera->iStopAnimationPath();
		}
		else if (pWebData->strCommand == "Web_ResumeRoam")
		{
			if (!m_strCurRoamPath.empty())
			{
				INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
				ICamera* pCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
				pCamera->iPlayAnimationPath(m_strCurRoamPath.c_str(), m_bRoamLoop ? PATH_PLAY_LOOP_SINGLE : PATH_PLAY_NOLOOP_SINGLE, false);
			}
		}
		else if (pWebData->strCommand == "Web_StopRoam")
		{
			INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
			ICamera* pCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
			pCamera->iStopAnimationPath();
			m_strCurRoamPath = "";
			m_bRoamLoop = false;
		}
		else if (pWebData->strCommand == "Web_Login3D")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				m_pClient->m_sUserInfo.strLastAccount = m_pClient->m_sUserInfo.strAccount;
				std::string strUserToken = pWebData->vecValues[0].asString();
				m_pClient->m_sUserInfo.strUserToken = strUserToken;
				if (m_pClient->m_sUserInfo.strUserToken.empty())
				{
					m_pClient->m_sUserInfo.strUserToken = "default";
				}
				if (pWebData->vecValues.size() >= 2)
				{
					m_pClient->m_sUserInfo.strAccount = pWebData->vecValues[1].asString();
				}
				if (m_pClient->m_sUserInfo.strAccount.empty())
				{
					m_pClient->m_sUserInfo.strAccount = "default";
				}
			}
			SceneElementLoader* pElementLoader = static_cast<SceneElementLoader*>(m_pClient->m_pSceneElementLoader);
			if (pElementLoader)
			{
				pElementLoader->OnlyRequestBaseInfo(true, true);
			}
		}
		else if (pWebData->strCommand == "Web_POIAreaResourcePicking")
		{
			if (pWebData->vecValues.size() >= 3)
			{
				std::string strNodeId = pWebData->vecValues[0].asString();
				std::string strLayer = pWebData->vecValues[1].asString();
				float fRadius = pWebData->vecValues[2].asFloat();
				std::vector<std::string> vecLayer;
				SplitStringBySpecial(strLayer, vecLayer, ",");
				m_pClient->m_vecStatisticLayer = vecLayer;
				m_pClient->m_jStatisticTemplate.clear();
				for (size_t i = 0; i < vecLayer.size(); i++)
				{
					m_pClient->m_jStatisticTemplate[vecLayer[i]] = Json::arrayValue;
				}

				INode* pNode = nullptr;

				if (m_pClient->m_pWebLabelParser)
				{
					WebPoiLabelFactory* pPoifactory = m_pClient->m_pWebLabelParser->GetPoiFactory();
					if (pPoifactory)
					{
						std::vector<FactoryAssemble*> vecAssemble;
						pPoifactory->GetCurAssemble(vecAssemble);
						for (size_t m = 0; m < vecAssemble.size(); m++)
						{
							ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)vecAssemble[m]->pData;
							if (pInfo->strUuid == strNodeId)
							{
								pNode = vecAssemble[m]->pLabelNode;
								break;
							}
						}
					}
				}
				if (pNode)
				{
					m_pClient->m_pPOICircleStatisticsTool->AddJsonExtra("web_commond_id", pWebData->strID);
					m_pClient->m_pPOICircleStatisticsTool->SetStatisticsTemplet(m_pClient->m_jStatisticTemplate);
					m_pClient->m_pPOICircleStatisticsTool->SetEnable(true);
					m_pClient->m_pPOICircleStatisticsTool->SetData(pNode->iGetOrigin(), fRadius);
				}
			}
		}
		else if (pWebData->strCommand == "Web_DoStatistics")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				int nType = pWebData->vecValues[0].asInt();
				std::string strLayer = pWebData->vecValues[1].asString();
				std::vector<std::string> vecLayer;
				SplitStringBySpecial(strLayer, vecLayer, ",");
				m_pClient->m_vecStatisticLayer = vecLayer;
				m_pClient->m_jStatisticTemplate.clear();
				for (size_t i = 0; i < vecLayer.size(); i++)
				{
					//m_jStatisticTemplate[vecLayer[i]] = 0;
					m_pClient->m_jStatisticTemplate[vecLayer[i]] = Json::arrayValue;
				}
				if (nType == 1)
				{
					m_pClient->m_pCircleStatisticsTool->AddJsonExtra("web_commond_id", pWebData->strID);
					m_pClient->m_pCircleStatisticsTool->SetStatisticsTemplet(m_pClient->m_jStatisticTemplate);
					m_pClient->m_pCircleStatisticsTool->SetEnable(true);
				}
				else if (nType == 2)
				{
					float fLineWidth = 50.0f;
					if (pWebData->vecValues.size() >= 3)
					{
						fLineWidth = pWebData->vecValues[2].asFloat();
					}
					m_pClient->m_pLineStatisticsTool->AddJsonExtra("web_commond_id", pWebData->strID);
					m_pClient->m_pLineStatisticsTool->SetRoadWidth(fLineWidth);
					m_pClient->m_pLineStatisticsTool->SetStatisticsTemplet(m_pClient->m_jStatisticTemplate);
					m_pClient->m_pLineStatisticsTool->SetEnable(true);
				}
				else if (nType == 3)
				{
					m_pClient->m_pPolygonStatisticsTool->AddJsonExtra("web_commond_id", pWebData->strID);
					m_pClient->m_pPolygonStatisticsTool->SetStatisticsTemplet(m_pClient->m_jStatisticTemplate);
					m_pClient->m_pPolygonStatisticsTool->SetEnable(true);
				}
				else if (nType == 4)
				{
					m_pClient->m_pPolygonStatisticsRectangularTool->AddJsonExtra("web_commond_id", pWebData->strID);
					m_pClient->m_pPolygonStatisticsRectangularTool->SetStatisticsTemplet(m_pClient->m_jStatisticTemplate);
					m_pClient->m_pPolygonStatisticsRectangularTool->SetEnable(true);
				}
				else
				{
					return;
				}

				INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
				ICamera* pCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
				Vector3d vAngle = pCameraNode->iGetAngles();
				Vector3d vOrigin = pCameraNode->iGetOrigin();
				vAngle.x = 89;
				pCameraNode->iSetOrigin(vOrigin);
				pCameraNode->iSetAngles(vAngle);

				IManipulator* pManipulator = GetComponent<IManipulator>(pCameraNode);
				pManipulator->iSetMouseButtonEnable(MouseButton_e::MOUSE_BUTTON_L, false);
				if (!pManipulator->iIsSwitchRBMB())
				{
					pManipulator->iSetMouseButtonEnable(MouseButton_e::MOUSE_BUTTON_R, false);
				}
				pManipulator->iSetKeyActionEnable(ACTION_ROTATE_LEFT, false);
				pManipulator->iSetKeyActionEnable(ACTION_ROTATE_RIGHT, false);
				pManipulator->iSetKeyActionEnable(ACTION_APPLY_VIEWPOINT, false);
			}
		}
		else if (pWebData->strCommand == "Web_CloseStatistics")
		{
			INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
			if (!pCameraNode)
			{
				return;
			}
			ICamera* pCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
			IManipulator* pManipulator = GetComponent<IManipulator>(pCameraNode);
			pManipulator->iSetMouseButtonEnable(MouseButton_e::MOUSE_BUTTON_L, true);
			pManipulator->iSetMouseButtonEnable(MouseButton_e::MOUSE_BUTTON_R, true);
			pManipulator->iSetKeyActionEnable(ACTION_ROTATE_LEFT, true);
			pManipulator->iSetKeyActionEnable(ACTION_ROTATE_RIGHT, true);
			pManipulator->iSetKeyActionEnable(ACTION_APPLY_VIEWPOINT, true);

			if (m_pClient->m_pCircleStatisticsTool)
			{
				m_pClient->m_pCircleStatisticsTool->SetEnable(false);
			}
			if (m_pClient->m_pLineStatisticsTool)
			{
				m_pClient->m_pLineStatisticsTool->SetEnable(false);
			}
			if (m_pClient->m_pPolygonStatisticsTool)
			{
				m_pClient->m_pPolygonStatisticsTool->SetEnable(false);
			}
			if (m_pClient->m_pPOICircleStatisticsTool)
			{
				m_pClient->m_pPOICircleStatisticsTool->SetEnable(false);
			}
			if (m_pClient->m_pPolygonStatisticsRectangularTool)
			{
				m_pClient->m_pPolygonStatisticsRectangularTool->SetEnable(false);
			}
			m_pClient->m_vecStatisticLayer.clear();
		}
		else if (pWebData->strCommand == "Web_SetEnableAreaMeasure")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bEnable = pWebData->vecValues[0].asBool();
				m_pClient->m_pAreaMeasureTool->SetEnable(bEnable);
				INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
				ICamera* pCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
				IManipulator* pManipulator = GetComponent<IManipulator>(pCameraNode);
				if (bEnable)
				{
					Vector3d vAngle = pCameraNode->iGetAngles();
					vAngle.x = 89;
					pCameraNode->iSetAngles(vAngle);
					
					WebCommandData* pCommond = (WebCommandData*)m_pClient->m_pAreaMeasureTool->GetINTPTR("web_commond");
					pCommond->strID = pWebData->strID;
				}
				pManipulator->iSetMouseButtonEnable(MouseButton_e::MOUSE_BUTTON_L, !bEnable);
				if (!pManipulator->iIsSwitchRBMB())
				{
					pManipulator->iSetMouseButtonEnable(MouseButton_e::MOUSE_BUTTON_R, !bEnable);
				}
				pManipulator->iSetKeyActionEnable(ACTION_ROTATE_LEFT, !bEnable);
				pManipulator->iSetKeyActionEnable(ACTION_ROTATE_RIGHT, !bEnable);
				pManipulator->iSetKeyActionEnable(ACTION_APPLY_VIEWPOINT, !bEnable);
			}
		}
		else if (pWebData->strCommand == "Web_SetEnableDistanceMeasure")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bEnable = pWebData->vecValues[0].asBool();
				m_pClient->m_pDistanceMeasureTool->SetEnable(bEnable);
				if (bEnable)
				{
					WebCommandData* pCommond = (WebCommandData*)m_pClient->m_pDistanceMeasureTool->GetINTPTR("web_commond");
					pCommond->strID = pWebData->strID;
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetViewshedColor")
		{
			if (pWebData->vecValues.size() >= 3)
			{
				Vector4 vVisibleColor = m_pClient->m_pWebLabelParser->ParseColor(pWebData->vecValues[0]);
				Vector4 vInVisibleColor = m_pClient->m_pWebLabelParser->ParseColor(pWebData->vecValues[1]);
				Vector4 vLineColor = m_pClient->m_pWebLabelParser->ParseColor(pWebData->vecValues[2]);
				m_pClient->m_pViewshedAnalysisTool->SetVisibleColor(Vector3d(vVisibleColor[0], vVisibleColor[1], vVisibleColor[2]));
				m_pClient->m_pViewshedAnalysisTool->SetInvisibleColor(Vector3d(vInVisibleColor[0], vInVisibleColor[1], vInVisibleColor[2]));
				m_pClient->m_pViewshedAnalysisTool->SetLineColor(Vector4(vLineColor[0], vLineColor[1], vLineColor[2]));
			}
		}
		else if (pWebData->strCommand == "Web_SetViewshedType")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				int nType = pWebData->vecValues[0].asInt();
				m_pClient->m_pViewshedAnalysisTool->SetViewshedType((EViewshedType)nType);
			}
		}
		else if (pWebData->strCommand == "Web_SetViewshedAngle")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				Vector2 vAngle = m_pClient->m_pWebLabelParser->ParseVector2(pWebData->vecValues[0]);
				if (m_pClient->m_pViewshedAnalysisTool->GetViewshedAnalysis())
				{
					Vector3d v(vAngle.x, vAngle.y, 0.0f);
					m_pClient->m_pViewshedAnalysisTool->SetAngle(v);
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetViewshedFov")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				float fAngle = pWebData->vecValues[0].asFloat();
				if (m_pClient->m_pViewshedAnalysisTool->GetViewshedAnalysis())
				{
					Vector3d vAngle = m_pClient->m_pViewshedAnalysisTool->GetViewshedAnalysis()->iGetAngles();
					vAngle.y = fAngle;
					m_pClient->m_pViewshedAnalysisTool->SetAngle(vAngle);
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetViewshedYaw")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				float fAngle = pWebData->vecValues[0].asFloat();
				if (m_pClient->m_pViewshedAnalysisTool->GetViewshedAnalysis())
				{
					Vector3d vAngle = m_pClient->m_pViewshedAnalysisTool->GetViewshedAnalysis()->iGetAngles();
					vAngle.x = -fAngle;
					m_pClient->m_pViewshedAnalysisTool->SetAngle(vAngle);
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetViewshedRotate")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				float fValueX = pWebData->vecValues[0].asFloat();
				float fValueY = pWebData->vecValues[1].asFloat();
				if (m_pClient->m_pViewshedAnalysisTool->GetViewshedAnalysis())
				{
					Vector3d vAngle = m_pClient->m_pViewshedAnalysisTool->GetViewshedAnalysis()->iGetAngles();
					vAngle.x += fValueX;
					vAngle.y += fValueY;
					m_pClient->m_pViewshedAnalysisTool->SetAngle(vAngle);
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetViewshedHRangeAngle" || pWebData->strCommand == "Web_SetViewshedRangeAngle")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				float fAngle = pWebData->vecValues[0].asFloat();
				if (m_pClient->m_pViewshedAnalysisTool->GetViewshedAnalysis())
				{
					m_pClient->m_pViewshedAnalysisTool->SetHFov(fAngle);
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetViewshedVRangeAngle")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				float fAngle = pWebData->vecValues[0].asFloat();
				if (m_pClient->m_pViewshedAnalysisTool->GetViewshedAnalysis())
				{
					m_pClient->m_pViewshedAnalysisTool->SetVFov(fAngle);
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetViewshedFarClip")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				float fValue = pWebData->vecValues[0].asFloat();
				m_pClient->m_pViewshedAnalysisTool->SetFarClip(fValue);
			}
		}
		else if (pWebData->strCommand == "Web_SetViewshedOffsetZ")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				float fValue = pWebData->vecValues[0].asFloat();
				m_pClient->m_pViewshedAnalysisTool->SetOffsetZ(fValue);
			}
		}
		else if (pWebData->strCommand == "Web_SetViewshedMove")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				float fX = pWebData->vecValues[0].asFloat();
				float fY = pWebData->vecValues[1].asFloat();
				IViewshedAnalysis* pView = m_pClient->m_pViewshedAnalysisTool->GetViewshedAnalysis();
				if (pView)
				{
					INode* pCameraNode = pView->iGetCamera();
					Vector3d vFront = pCameraNode->iGetAxis()[0];
					Vector3d vLeft = pCameraNode->iGetAxis()[1];
					vFront.z = 0;
					vLeft.z = 0;
					vFront.Normalise();
					vLeft.Normalise();
					Vector3d vPos = pView->iGetOrigin();
					vPos += vFront * fX;
					vPos += vLeft * fY;
					pView->iSetOrigin(vPos);
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetViewshedOrigin")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bWgs84 = true;
				if (pWebData->vecValues.size() >= 2)
				{
					bWgs84 = pWebData->vecValues[1].asBool();
				}
				Vector3d vPos = m_pClient->m_pWebLabelParser->ParseVector3(pWebData->vecValues[0]);
				Vector3d vScenePos;
				if (bWgs84)
				{
					m_pClient->ConvertLngToVector3(vPos[0], vPos[1], &vScenePos, true);
				}
				else
				{
					vScenePos = vPos;
					float fHeight = m_pClient->GetZFromTif(vScenePos);
					m_pClient->AdjustZ(vScenePos, fHeight,false);
				}
				m_pClient->m_pViewshedAnalysisTool->SetOrigin(vScenePos);
			}
		}
		else if (pWebData->strCommand == "Web_StartViewshed")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bGrapScenePoint = pWebData->vecValues[0].asBool();
				m_pClient->m_pViewshedAnalysisTool->Start(bGrapScenePoint);
			}
		}
		else if (pWebData->strCommand == "Web_StopViewshed")
		{
			m_pClient->m_pViewshedAnalysisTool->Stop();
		}
		else if (pWebData->strCommand == "Web_EnterIndoorBuilding")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				m_pClient->m_pBuildingIndoorFactory->SetEnterAreaScene(m_pClient->m_bEnterAreaScene);
				std::string strBuildingID = pWebData->vecValues[0].asString();
				bool bDynamicLoad = pWebData->vecValues[1].asBool();
				BuildingData* pBuildingData = m_pClient->m_pBuildingIndoorFactory->GetBuildingData(strBuildingID);
				if (pBuildingData)
				{
					m_pClient->m_pBuildingIndoorFactory->ChangeAreaScene(pBuildingData->pBuildingGroupData->pAreaSceneData->strAreaSceneNodeID, bDynamicLoad);
					m_pClient->m_pBuildingIndoorFactory->ChangeBuilding(pBuildingData->strBuildingNodeID, bDynamicLoad);
				}
			}
		}
		else if (pWebData->strCommand == "Web_LeavelIndoorBuilding")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bDynamicLoad = pWebData->vecValues[0].asBool();
				m_pClient->m_pBuildingIndoorFactory->CloseAreaScene(m_pClient->m_bEnterAreaScene, bDynamicLoad);
			}
		}
		else if (pWebData->strCommand == "Web_ChangeIndoorFloor")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strFloorID = pWebData->vecValues[0].asString();
				m_pClient->m_pBuildingIndoorFactory->ChangeFloor(strFloorID);
			}
		}
		else if (pWebData->strCommand == "Web_SetIndoorAlpha")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				float fAlpha = pWebData->vecValues[0].asFloat();
				m_pClient->m_pBuildingIndoorFactory->ChangeAlpha(fAlpha);
			}
		}
		else if (pWebData->strCommand == "Web_LauchBrowser")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strUrl = pWebData->vecValues[0].asString();
#ifdef _MSC_VER				
				ShellExecute(NULL, "open", strUrl.c_str(), NULL, NULL, SW_RESTORE);
#endif
				
			}
		}
		else if (pWebData->strCommand == "Web_Enter3DScene")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				//m_pClient->m_nMapType = pWebData->vecValues[0].asInt();
				//Json::Value jInfo = pWebData->vecValues[1];
				//Vector3d vViewPointPos(0), vViewPointAngle(89, 89, 0);
				//double dLng, dLat, fZoom, fAngleX, fAngleY, fHeight = 0.0f;
				//if (jInfo.isMember("lng"))
				//{
				//	dLng = jInfo["lng"].asDouble();
				//}
				//if (jInfo.isMember("lat"))
				//{
				//	dLat = jInfo["lat"].asDouble();
				//}
				//if (jInfo.isMember("zoom"))
				//{
				//	fZoom = jInfo["zoom"].asDouble();
				//}
				//if (jInfo.isMember("angleX"))
				//{
				//	fAngleX = jInfo["angleX"].asDouble();
				//}
				//if (jInfo.isMember("angleY"))
				//{
				//	fAngleY = jInfo["angleY"].asDouble();
				//}
				//if (jInfo.isMember("height"))
				//{
				//	fHeight = jInfo["height"].asDouble();
				//}
				//vViewPointAngle = Vector3d(fAngleX, fAngleY, 0.0f);
				//if (vViewPointAngle.Empty())
				//{
				//	vViewPointAngle = Vector3d(89.0f, 89.0f, 0);
				//}
				//switch (m_pClient->m_nMapType)
				//{
				//case 1:
				//{
				//	double dOutLng, dOutlat;
				//	m_pClient->GCJ02_to_WGS(dLng, dLat, dOutLng, dOutlat);
				//	m_pClient->ConvertLngToVector3(dOutLng, dOutlat, &vViewPointPos, false);
				//}
				//break;
				//case 2:
				//{
				//	m_pClient->ConvertLngToVector3(dLng, dLat, &vViewPointPos, false);
				//}
				//break;
				//default:
				//	break;
				//}
				//if (fZoom <= 0)
				//{
				//	m_pClient->AdjustZ(vViewPointPos, fHeight,false);
				//}
				//else
				//{
				//	int nBaseZoom = 16;
				//	m_pClient->AdjustZ(vViewPointPos, ((1.0f * nBaseZoom) / fZoom)* m_pClient->m_fMapScene16LevelHeight,false);
				//}
				//INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
				//GoViewPoint(pCameraNode, vViewPointPos, vViewPointAngle, false);
			}
		}
		else if (pWebData->strCommand == "Web_Leave3DScene")
		{
			//INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
			//Vector3d vCameraOrigin = pCameraNode->iGetOrigin();
			//Vector3d vCameraAngle = pCameraNode->iGetAngles();
			//double dLng, dlat, dOutLng, dOutlat;
			//m_pClient->ScenePosToWGS(vCameraOrigin, dLng, dlat);
			//switch (m_pClient->m_nMapType)
			//{
			//case 1:
			//{
			//	m_pClient->WGS_TO_GCJ02(dLng, dlat, dOutLng, dOutlat);
			//	dLng = dOutLng;
			//	dlat = dOutlat;
			//}
			//break;
			//default:
			//	break;
			//}
			//int nBaseZoom = 16;
			//int nZoom = nBaseZoom;
			//if (vCameraOrigin.z > m_pClient->m_fMapScene16LevelHeight)
			//{
			//	nZoom = nBaseZoom - (int)(vCameraOrigin.z / m_pClient->m_fMapScene16LevelHeight);
			//	if (nZoom < 1)
			//	{
			//		nZoom = 1;
			//	}
			//}
			//else
			//{
			//	nZoom = nBaseZoom + (int)(m_pClient->m_fMapScene16LevelHeight / vCameraOrigin.z);
			//	if (nZoom > 18)
			//	{
			//		nZoom = 18;
			//	}
			//}
			//Json::Value jInfo;
			//jInfo["lng"] = dLng;
			//jInfo["lat"] = dlat;
			//jInfo["zoom"] = nZoom;
			//jInfo["angleX"] = vCameraAngle.x;
			//jInfo["angleY"] = vCameraAngle.y;
			//jInfo["height"] = vCameraOrigin.z;

			//std::string strWeb = "";
			//strWeb += jInfo.toStyledString();
			//m_pClient->ToSendWebCommond("VS_Leave3DScene", pWebData->strID, strWeb);
		}
		else if (pWebData->strCommand == "Web_PlayVideoInspection")
		{
			std::string strKey = pWebData->vecValues[0].asString();
			int nLoop = pWebData->vecValues[1].asInt();
			m_pClient->m_pVideoInspectionFactory->Play(strKey, nLoop);
		}
		else if (pWebData->strCommand == "Web_PauseVideoInspection")
		{
			m_pClient->m_pVideoInspectionFactory->Pause();
		}
		else if (pWebData->strCommand == "Web_ResumeVideoInspection")
		{
			m_pClient->m_pVideoInspectionFactory->Resume();
		}
		else if (pWebData->strCommand == "Web_StopVideoInspection")
		{
			m_pClient->m_pVideoInspectionFactory->Stop();
		}
		else if (pWebData->strCommand == "Web_GetLabelList" || pWebData->strCommand == "Web_GetElementList")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strTypes = pWebData->vecValues[0].asString();
				std::vector<std::string> vecType;
				SplitStringBySpecial(strTypes, vecType, ",");
				Json::Value jArray = Json::arrayValue;

				std::vector<CommonLabelFactory*> vecFactory;
				m_pClient->m_pSceneElementLoader->GetAllOutElementFacotory(vecFactory);
				m_pClient->m_pSceneElementLoader->GetAllInElementFacotory(vecFactory);
				for (size_t i = 0; i < vecFactory.size(); i++)
				{
					if (find(vecType.begin(), vecType.end(), to_string(vecFactory[i]->GetType())) != vecType.end() || vecType.empty())
					{
						std::vector<ProjectBasedSceneElement*> vecData;
						vecFactory[i]->GetCurData(vecData);
						for (size_t j = 0; j < vecData.size(); j++)
						{
							ProjectBasedSceneElement* pInfo =vecData[j];

							Json::Value jParam = {};
							jParam["extra"] = pInfo->strExtra;
							jParam["id"] = pInfo->strUuid;
							char sz[1024] = { 0 };
							AsciiToUtf8(pInfo->strTitle.c_str(), sizeof(sz), sz);
							jParam["name"] = sz;
							jParam["type"] = pInfo->nTypeID;

							jArray.append(jParam);
						}
					}
				}

				{
					std::vector<CommonLabelFactory*> vecFactory;
					m_pClient->m_pWebLabelParser->GetAllFactory(vecFactory);
					vecFactory.push_back(m_pClient->m_pWebLabelParser->GetPoiFactory());
					for (size_t i = 0; i < vecFactory.size(); i++)
					{
						if (find(vecType.begin(), vecType.end(), to_string(vecFactory[i]->GetType())) != vecType.end() || vecType.empty())
						{
							std::vector<ProjectBasedSceneElement*> vecData;
							vecFactory[i]->GetCurData(vecData);
							for (size_t j = 0; j < vecData.size(); j++)
							{
								ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)vecData[j];

								Json::Value jParam = {};
								jParam["extra"] = pInfo->strExtra;
								jParam["id"] = pInfo->strUuid;
								jParam["name"] = pInfo->strTitle;
								if (vecFactory[i] == m_pClient->m_pWebLabelParser->GetPoiFactory())
								{
									jParam["type"] = -1;
								}
								else
								{
									jParam["type"] = pInfo->nTypeID;
								}

								jArray.append(jParam);
							}
						}
					}
				}

				std::string str = "";
				str += jArray.toStyledString();
				m_pClient->ToSendWebCommond("VS_ToWebLabelList", pWebData->strID, str);
			}
		}
		else if (pWebData->strCommand == "Web_ConvertLnglatToScenePos")
		{
			if (pWebData->vecValues.size() >= 3)
			{
				std::string strType = pWebData->vecValues[0].asString();
				double dLng = pWebData->vecValues[1].asDouble();
				double dLat = pWebData->vecValues[2].asDouble();

				Vector3d vPos;
				EElementCoordinateType eType = ElementCoordinateType_e::COOR_WGS84;
				if (strType == "WGS84")
				{
					eType = ElementCoordinateType_e::COOR_WGS84;
				}
				else if (strType == "GCJ02")
				{
					eType = ElementCoordinateType_e::COOR_GCJ02;
				}
				else if (strType == "BD09")
				{
					eType = ElementCoordinateType_e::COOR_BD09;
				}
				else if (strType.compare("CGS2000") == 0)
				{
					eType = ElementCoordinateType_e::COOR_CGCS2000;
				}
				else if (strType == "CGCS2000")
				{
					eType = ElementCoordinateType_e::COOR_CGCS2000;
				}
				m_pClient->ConvertToVector3WithCoorSystem(dLng, dLat, &vPos, eType, true);

				std::string str = "";
				str += to_string(vPos.x);
				str += ",";
				str += to_string(vPos.y);
				str += ",";
				str += to_string(vPos.z);

				m_pClient->ToSendWebCommond("VS_ConvertLnglatToScenePos", pWebData->strID, str);
			}
		}

		else if (pWebData->strCommand == "Web_LoadSceneStart")
		{
			if (pWebData->vecValues.size() >= 2)
			{
			}
		}
		else if (pWebData->strCommand == "Web_SwitchScene")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				SwitchViewParam tParam;

				//tParam.strSceneViewID = pWebData->vecValues[0].asString();
				//tParam.strEntityManagerID = pWebData->vecValues[1].asString();
				tParam.strSceneFile = pWebData->vecValues[0].asString();
				tParam.bReloadScene = pWebData->vecValues[1].asBool();

				APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iSwitchView(tParam);
			}
		}
		else if (pWebData->strCommand == "Web_SceneBasicSettings")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				float fLimitHeight = atof(pWebData->vecValues[0].asCString());
				float fBaseHeight = atof(pWebData->vecValues[1].asCString());

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fSceneLimitHeight = fLimitHeight;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fSceneBaseHeight = fBaseHeight;
			}
		}
		else if (pWebData->strCommand == "Web_SceneSizeSetting")
		{
			if (pWebData->vecValues.size() >= 4)
			{
				int nSceneWidth = atoi(pWebData->vecValues[0].asCString());
				int nSceneHeight = atoi(pWebData->vecValues[1].asCString());
				int nOutputWidth = atoi(pWebData->vecValues[2].asCString());
				int nOutputHeight = atoi(pWebData->vecValues[3].asCString());

				APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iSetSceneSize(nSceneWidth, nSceneHeight);
				APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iSetOutputSize(nOutputWidth, nOutputHeight);
			}
		}
		else if (pWebData->strCommand == "Web_FixedFrameRateSetting")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				int nUpdateFPS = atoi(pWebData->vecValues[0].asCString());

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nUpdateFPS = nUpdateFPS;
			}
		}
		else if (pWebData->strCommand == "Web_EnableBackGroundRender")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bBackgroundRender = pWebData->vecValues[0].asBool();

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender = bBackgroundRender;
			}
		}
		else if (pWebData->strCommand == "Web_CompileTiemLimitPerFrame")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				float fCompileTimePerFrame = atof(pWebData->vecValues[0].asCString());

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fCompileTimePerFrame = fCompileTimePerFrame;
			}
		}
		else if (pWebData->strCommand == "Web_ReleaseTimeLimitPerFrame")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				float fReleaseTimePerFrame = atof(pWebData->vecValues[0].asCString());

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fReleaseTimePerFrame = fReleaseTimePerFrame;
			}
		}
		else if (pWebData->strCommand == "Web_ShowFarthestDistance")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				float fPagedNodeMaxVisible = atof(pWebData->vecValues[0].asCString());

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fPagedNodeMaxVisible = fPagedNodeMaxVisible;
			}
		}
		else if (pWebData->strCommand == "Web_LoadFarthesDistance")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				float fPagedNodeMaxRange = atof(pWebData->vecValues[0].asCString());

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fPagedNodeMaxRange = fPagedNodeMaxRange;
			}
		}
		else if (pWebData->strCommand == "Web_LoadAccuracyScale")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				float fPagedNodeRangeScale = atof(pWebData->vecValues[0].asCString());

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fPagedNodeRangeScale = fPagedNodeRangeScale;
			}
		}
		else if (pWebData->strCommand == "Web_EnableReceiveLight")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bPagedNodeAcceptLight = pWebData->vecValues[0].asBool();

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.bPagedNodeAcceptLight = bPagedNodeAcceptLight;
			}
		}
		else if (pWebData->strCommand == "Web_SetMapColor")
		{
			if (pWebData->vecValues.size() >= 4)
			{
				Vector4 vPagedNodeToneColor;
				vPagedNodeToneColor.x = atof(pWebData->vecValues[0].asCString());
				vPagedNodeToneColor.y = atof(pWebData->vecValues[1].asCString());
				vPagedNodeToneColor.z = atof(pWebData->vecValues[2].asCString());
				vPagedNodeToneColor.w = atof(pWebData->vecValues[3].asCString());

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.vPagedNodeToneColor = vPagedNodeToneColor;
			}
		}
		else if (pWebData->strCommand == "Web_EnableDynamicLoadSceneFile")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bPagedNodeLoadSceneDynamic = pWebData->vecValues[0].asBool();

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.bPagedNodeLoadSceneDynamic = bPagedNodeLoadSceneDynamic;
			}
		}
		else if (pWebData->strCommand == "Web_EnableDynamicUnloadSceneFile")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bPagedNodeFreeSceneDynamic = pWebData->vecValues[0].asBool();

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.bPagedNodeFreeSceneDynamic = bPagedNodeFreeSceneDynamic;
			}
		}
		else if (pWebData->strCommand == "Web_EnableDepthFirstLoading")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bPagedNodeLoadDepthFirst = pWebData->vecValues[0].asBool();

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.bPagedNodeLoadDepthFirst = bPagedNodeLoadDepthFirst;
			}
		}
		else if (pWebData->strCommand == "Web_SceneWeatherSelection")
		{
			if (pWebData->vecValues.size() >= 1)
			{
			}
		}
		else if (pWebData->strCommand == "Web_SetLightAndShadow")
		{
			if (pWebData->vecValues.size() >= 4)
			{
				int nCSMShadowCount = atoi(pWebData->vecValues[0].asCString());
				float fCSMShadowDistanceMax = atof(pWebData->vecValues[1].asCString());
				int nDynamicLightMax = atoi(pWebData->vecValues[2].asCString());
				int nStationaryLightMax = atof(pWebData->vecValues[3].asCString());

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.nCSMShadowCount = nCSMShadowCount;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fCSMShadowDistanceMax = fCSMShadowDistanceMax;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.nDynamicLightMax = nDynamicLightMax;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.nStationaryLightMax = nStationaryLightMax;
			}
		}
		else if (pWebData->strCommand == "Web_SetFloodlight")
		{
			if (pWebData->vecValues.size() >= 3)
			{
				float fBloomThreshold = atof(pWebData->vecValues[0].asCString());
				float fBloomIntensity = atof(pWebData->vecValues[1].asCString());
				float fCrossFlareThreshold = atof(pWebData->vecValues[2].asCString());

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fBloomThreshold = fBloomThreshold;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fBloomIntensity = fBloomIntensity;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fCrossFlareThreshold = fCrossFlareThreshold;
			}
		}
		else if (pWebData->strCommand == "Web_SetAmbientOucclusion")
		{
			if (pWebData->vecValues.size() >= 3)
			{
				int nSSAOKernelCount = atoi(pWebData->vecValues[0].asCString());
				float fSSAORadius = atof(pWebData->vecValues[1].asCString());
				float fSSAOIntensity = atof(pWebData->vecValues[2].asCString());

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.nSSAOKernelCount = nSSAOKernelCount;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fSSAORadius = fSSAORadius;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fSSAOIntensity = fSSAOIntensity;
			}
		}
		else if (pWebData->strCommand == "Web_SetAntiAliasing")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bTXAAAntiFlicker = pWebData->vecValues[0].asBool();

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.bTXAAAntiFlicker = bTXAAAntiFlicker;
			}
		}
		else if (pWebData->strCommand == "Web_SetToneMapping")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bUseToneMappingLUT = pWebData->vecValues[0].asBool();
				string strToneMappingLUTFile = pWebData->vecValues[1].asString();
				bool bUseColorGradingLUT = pWebData->vecValues[2].asBool();
				string strColorGradingLUTFile = pWebData->vecValues[3].asString();
				bool bAutoExposure = pWebData->vecValues[4].asBool();
				float fExposureScale = atof(pWebData->vecValues[5].asCString());
				float fLuminanceThresholdMax = atof(pWebData->vecValues[6].asCString());
				float fLuminanceThresholdMin = atof(pWebData->vecValues[7].asCString());
				float fColorHue = atof(pWebData->vecValues[8].asCString());
				float fColorSaturation = atof(pWebData->vecValues[9].asCString());
				float fColorValue = atof(pWebData->vecValues[10].asCString());
				//float fColorContrast			= atof(pWebData->vecValues[11].asCString());
				//float fColorBrightness			= atof(pWebData->vecValues[12].asCString());

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.bUseToneMappingLUT = bUseToneMappingLUT;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.strToneMappingLUTFile = strToneMappingLUTFile;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.bUseColorGradingLUT = bUseColorGradingLUT;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.strColorGradingLUTFile = strColorGradingLUTFile;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.bAutoExposure = bAutoExposure;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fExposureScale = fExposureScale;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fLuminanceThresholdMax = fLuminanceThresholdMax;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fLuminanceThresholdMin = fLuminanceThresholdMin;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fColorHue = fColorHue;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fColorSaturation = fColorSaturation;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fColorValue = fColorValue;
				//APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fColorContrast = fColorContrast;
				//APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fColorBrightness = fColorBrightness;
			}
		}
		else if (pWebData->strCommand == "Web_SetAmbientLight")
		{
			if (pWebData->vecValues.size() >= 4)
			{
				float fSkyExposureScale = atof(pWebData->vecValues[0].asCString());
				float fSkyOcclusionIntensity = atof(pWebData->vecValues[1].asCString());
				string strSkyOcclusionColor = pWebData->vecValues[2].asString();
				string strSkyOcclusionGroundColor = pWebData->vecValues[3].asString();

				Vector3d vSkyOcclusionColor;
				Vector3d vSkyOcclusionGroundColor;

				vector<string> vecPos;
				SplitStringBySpecial(strSkyOcclusionColor, vecPos, ",");
				if (vecPos.size() > 0)
				{
					vSkyOcclusionColor = Vector3d(atof(vecPos[0].c_str()), atof(vecPos[1].c_str()), atof(vecPos[2].c_str()));
				}

				vector<string> vecPos2;
				SplitStringBySpecial(strSkyOcclusionGroundColor, vecPos2, ",");
				if (vecPos2.size() > 0)
				{
					vSkyOcclusionGroundColor = Vector3d(atof(vecPos2[0].c_str()), atof(vecPos2[1].c_str()), atof(vecPos2[2].c_str()));
				}

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fSkyExposureScale = fSkyExposureScale;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fSkyOcclusionIntensity = fSkyOcclusionIntensity;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.vSkyOcclusionColor = vSkyOcclusionColor;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.vSkyOcclusionGroundColor = vSkyOcclusionGroundColor;
			}
		}
		else if (pWebData->strCommand == "Web_SetDepthOfField")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				float fDepthOfFiledStart = atof(pWebData->vecValues[0].asCString());
				float fDepthOfFiledFallOff = atof(pWebData->vecValues[1].asCString());

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fDepthOfFiledStart = fDepthOfFiledStart;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fDepthOfFiledFallOff = fDepthOfFiledFallOff;
			}
		}
		else if (pWebData->strCommand == "Web_SetFogEffect")
		{
			if (pWebData->vecValues.size() >= 3)
			{
				int nType = atoi(pWebData->vecValues[0].asCString());
				float fHeightFallOff = atof(pWebData->vecValues[1].asCString());
				float fDistanceFallOff = atof(pWebData->vecValues[2].asCString());

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.sSceneFog.nType = nType;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.sSceneFog.fHeightFallOff = fHeightFallOff;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.sSceneFog.fDistanceFallOff = fDistanceFallOff;
			}
		}
		else if (pWebData->strCommand == "Web_ReflexType")
		{
			if (pWebData->vecValues.size() >= 3)
			{
				bool bPlaneReflectShader = pWebData->vecValues[0].asBool();
				bool bCubeReflectShader = pWebData->vecValues[1].asBool();
				bool bSSRShader = pWebData->vecValues[2].asBool();

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sShaderConfig.bPlaneReflectShader = bPlaneReflectShader;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sShaderConfig.bCubeReflectShader = bCubeReflectShader;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sShaderConfig.bSSRShader = bSSRShader;
			}
		}
		else if (pWebData->strCommand == "Web_SetGlow")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bGlowShader = pWebData->vecValues[0].asBool();

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sShaderConfig.bGlowShader = bGlowShader;
			}
		}
		else if (pWebData->strCommand == "Web_ViewAngleHighlight")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bViewSpecularShader = pWebData->vecValues[0].asBool();

				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sShaderConfig.bViewSpecularShader = bViewSpecularShader;
			}
		}
		else if (pWebData->strCommand == "Web_ViewpointJump")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				string strPos = pWebData->vecValues[0].asString();

				vector<string> vecPos;
				Vector3d vOrigin;
				Vector3d vAngle;

				SplitStringBySpecial(strPos, vecPos, ",");
				if (vecPos.size() > 0)
				{
					vOrigin = Vector3d(atof(vecPos[0].c_str()), atof(vecPos[1].c_str()), atof(vecPos[2].c_str()));
					vAngle = Vector3d(atof(vecPos[3].c_str()), atof(vecPos[4].c_str()), atof(vecPos[5].c_str()));

					INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
					ICamera* pCamera = pCameraNode->GetDynamicComponent<ICamera>();
					pCamera->iGoViewPoint(vOrigin, vAngle, false);
				}
			}
		}
		else if (pWebData->strCommand == "Web_SceneRoaming")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				bool bEnable = pWebData->vecValues[0].asBool();
				string strRoamName = pWebData->vecValues[1].asString();

				INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
				ICamera* pCamera = pCameraNode->GetDynamicComponent<ICamera>();

				char szName[512] = { 0 };
				Utf8ToAscii(strRoamName.c_str(), strlen(strRoamName.c_str()), szName);
				if (bEnable)
				{
					pCamera->iPlayAnimationPath(szName, PATH_PLAY_LOOP_SINGLE, true);
				}
				else
				{
					pCamera->iStopAnimationPath();
					Vector3d vAngle = pCameraNode->iGetAngles();
					if (vAngle.x <= 0)
					{
						vAngle.x = 2.0f;
						pCameraNode->iSetAngles(vAngle);
					}
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetSceneTime")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				float nTimeHour = pWebData->vecValues[0].asFloat();
				float nTimeMinutes = pWebData->vecValues[1].asFloat() / 60.0f;
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.fSceneTime = nTimeHour + nTimeMinutes;
				BCEvent tEvent(EVENT_UPDATE_SHADOW);
				APIProvider::GetSystemAPI()->iEngineAPI->iExecuteEvent(tEvent);
			}
		}
		else if (pWebData->strCommand == "Web_SetRotateAroundCenter")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				m_bRotateAroundCenter = pWebData->vecValues[0].asBool();
				m_fAroundCenterSpeed = pWebData->vecValues[1].asFloat();
				if (m_bRotateAroundCenter)
				{
					m_vAroundCenterStartCameraPos = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->iGetOrigin();
					m_vAroundCenterStartCameraAngle = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->iGetAngles();
					IManipulator* pManupulator = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->GetDynamicComponent<IManipulator>();
					m_vAroundCenterPos = pManupulator->iGetViewCenterPoint();

					Axisd tAxis;
					tAxis.LookAt(m_vAroundCenterStartCameraPos, m_vAroundCenterPos, Vector3d(0, 0, 1));
					m_fCurAroundCenterAngle = tAxis.ToAngles().y;
				}
				ICamera* pCamera = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->GetDynamicComponent<ICamera>();
				pCamera->iSetOperateType(m_bRotateAroundCenter ? OPERATE_TYPE_NO_OPERATE : OPERATE_TYPE_SELECT);
			}
		}
		else if (pWebData->strCommand == "Web_AddCustomRoamPath")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				std::string strKey = pWebData->vecValues[0].asString();
				Json::Value jPointArray = pWebData->vecValues[1];
				bool bWgs84 = false;
				if (pWebData->vecValues.size() >= 3)
				{
					bWgs84 = pWebData->vecValues[2].asBool();
				}
				TweenAnimateForNode* pTween = nullptr;
				if (m_mapTween.find(strKey) != m_mapTween.end())
				{
					pTween = m_mapTween[strKey];
				}
				else
				{
					pTween = new TweenAnimateForNode(APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera());
					pTween->SetMoveCallback(
						[](TweenAnimateForNode* pTween, INode* pTarget, Vector3d vPos, Vector3d vAngle, bool bPerEndPoint)
						{
							Vector3d vAng = vAngle;
							vAng.y += 180.0f;
							Vector3d vOffsetOrigin(0, 0, 0), vOffsetAngle(0, 0, 0);
							Json::Value jOption = pTween->GetExtraJValue("option");
							if (!jOption.isNull())
							{
								if (jOption.isMember("camera_offset_origin"))
								{
									Json::Value& value = jOption["camera_offset_origin"]["values"];
									vOffsetOrigin = Vector3d(value[0].asFloat(), value[1].asFloat(), value[2].asFloat());
								}
								if (jOption.isMember("camera_offset_angle"))
								{
									Json::Value& value = jOption["camera_offset_angle"]["values"];
									vOffsetAngle = Vector3d(value[0].asFloat(), value[1].asFloat(), 0);
								}
							}
							pTarget->iSetOrigin(vPos + vOffsetOrigin);
							pTarget->iSetAngles(vAng + vOffsetAngle);
						});
					pTween->SetCalcAngleCallback(
						[](TweenAnimateForNode* pTween, Vector3d vCurOrigin, Vector3d vPreOrigin)
						{
							Vector3d vAngle;
							vAngle = CalcAngle(vPreOrigin,vCurOrigin); 
							vAngle.x = 10.0f;
							vAngle.y += 180.0f;
							vAngle.z = 0;
							return vAngle;
						}
					);
					pTween->SetSpeedUniform(true);
					m_mapTween[strKey] = pTween;
				}
				pTween->Stop();
				pTween->ClearAllPoint();
				std::vector<PointData> vecPointData;
				for (int i = 0; i < jPointArray.size(); i++)
				{
					std::string strPoint = jPointArray[i].asString();
					Vector3d vOrigin, vAngle;
					sscanf_s(strPoint.c_str(), "%lf,%lf,%lf,%lf,%lf,%lf", &vOrigin.x, &vOrigin.y, &vOrigin.z, &vAngle.x, &vAngle.y, &vAngle.z);
					if (bWgs84)
					{
						m_pClient->ConvertLngToVector3(vOrigin.x, vOrigin.y, &vOrigin,false);
					}
					PointData stPoint;
					stPoint.vPos = vOrigin;
					//stPoint.vAngle = vAngle;
					vecPointData.push_back(stPoint);
				}
				pTween->SetPoints(vecPointData);
			}
		}
		else if (pWebData->strCommand == "Web_PlayCustomRoamPath")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				std::string strKey = pWebData->vecValues[0].asString();
				bool bLoop = pWebData->vecValues[1].asBool();
				if (m_mapTween.find(strKey) != m_mapTween.end())
				{
					m_mapTween[strKey]->SetLoop(bLoop);
					float fSpeed = 1000.0f;
					if (pWebData->vecValues.size() >= 3)
					{
						Json::Value& jItem = pWebData->vecValues[2];
						if (jItem.isMember("speed"))
						{
							fSpeed = jItem["speed"].asFloat();
						}
						m_mapTween[strKey]->SetExtraJValue("option", jItem);
					}
					m_mapTween[strKey]->ResetPosition();
					m_mapTween[strKey]->SetSpeed(fSpeed);
					m_mapTween[strKey]->Start();
					m_strCurRoamKey = strKey;
				}
			}
		}
		else if (pWebData->strCommand == "Web_PauseCustomRoamPath")
		{
			if (m_mapTween.find(m_strCurRoamKey) != m_mapTween.end())
			{
				m_mapTween[m_strCurRoamKey]->Pause();
			}
		}
		else if (pWebData->strCommand == "Web_ResumeCustomRoamPath")
		{
			if (m_mapTween.find(m_strCurRoamKey) != m_mapTween.end())
			{
				m_mapTween[m_strCurRoamKey]->Start();
			}
		}
		else if (pWebData->strCommand == "Web_StopCustomRoamPath")
		{
			if (m_mapTween.find(m_strCurRoamKey) != m_mapTween.end())
			{
				m_mapTween[m_strCurRoamKey]->Stop();

				m_strCurRoamKey = "";
			}
		}
		else if (pWebData->strCommand == "Web_GoToCSViewport")
		{
			ICamera* pCamera = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->GetDynamicComponent<ICamera>();
			pCamera->iGoViewPoint("CS", false);
		}
		else if (pWebData->strCommand == "Web_GetSceneMapScale")
		{
			//std::string strWeb = "";
			//strWeb += to_string(m_pClient->m_fMapScale);
			//m_pClient->ToSendWebCommond("VS_ToSceneMapScale", pWebData->strID, strWeb);
		}
		else if (pWebData->strCommand == "Web_SetSceneAttr")
		{
			if (pWebData->vecValues.size() > 0)
			{
				IManipulator* pManipulator = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->GetDynamicComponent<IManipulator>();
				RenderConfig& pConfig = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig;

				Json::Value jValue = pWebData->vecValues[0];
				if (jValue.isMember("limit_height"))
				{
					float fValue = jValue["limit_height"].asFloat();
					pConfig.fSceneLimitHeight = fValue;
					pManipulator->iSetSceneLimitHeight(pConfig.fSceneLimitHeight);
				}
				if (jValue.isMember("base_height"))
				{
					float fValue = jValue["base_height"].asFloat();
					pConfig.fSceneBaseHeight = fValue;
					pManipulator->iSetSceneBaseHeight(pConfig.fSceneBaseHeight);
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetShellLoadDistance")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				float fDistance = pWebData->vecValues[0].asFloat();
				std::vector<INode*> vecNode;
				APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetTypeNode(vecNode,NODE_SHELL);
				for (size_t i = 0; i < vecNode.size(); ++i)
				{
					ShellParam stParam;
					vecNode[i]->GetDynamicComponent<IShell>()->iGetShellParam(stParam);
					stParam.fLoadDistance = fDistance;
					vecNode[i]->GetDynamicComponent<IShell>()->iSetShellParam(stParam);
					if (vecNode[i]->iGetParent())
					{
						vecNode[i]->iGetParent()->GetDynamicComponent<IRenderNode>()->iGenerateChildMesh();
					}
					vecNode[i]->iSetVisible(true);
					if (vecNode[i]->iGetParent())
					{
						vecNode[i]->iGetParent()->iSetVisible(true);
					}
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetShellAttr")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				std::string strId = pWebData->vecValues[0].asString();
				std::vector<INode*> vecNode;
				APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetTypeNode(vecNode, NODE_SHELL);
				for (size_t i = 0; i < vecNode.size(); ++i)
				{
					bool bFind = strId.empty();
					if (!bFind)
					{
						bFind = bFind && (vecNode[i]->iGetID() == strId);
					}
					if (!bFind)
					{
						YBSSInfo* pInfo = (YBSSInfo*)vecNode[i]->iGetExternParam();
						if (pInfo)
						{
							bFind = bFind && (pInfo->strBindId == strId);
						}
					}
					if (bFind)
					{
						ShellParam stParam;
						vecNode[i]->GetDynamicComponent<IShell>()->iGetShellParam(stParam);
						Json::Value jObject = pWebData->vecValues[1];
						if (jObject.isMember("base_color"))
						{
							Vector4 vColor = m_pClient->m_pWebLabelParser->ParseColor(jObject["base_color"]);
							stParam.vBaseColor = vColor;
						}
						if (jObject.isMember("hover_color"))
						{
							Vector4 vColor = m_pClient->m_pWebLabelParser->ParseColor(jObject["hover_color"]);
							stParam.vHoverColor = vColor;
						}
						if (jObject.isMember("select_color"))
						{
							Vector4 vColor = m_pClient->m_pWebLabelParser->ParseColor(jObject["select_color"]);
							stParam.vSelectColor = vColor;
						}
						if (jObject.isMember("hover_enable"))
						{
							bool bEnable = jObject["hover_enable"].asBool();
							stParam.bHoverEnable = bEnable;
						}
						if (jObject.isMember("select_enable"))
						{
							bool bEnable = jObject["select_enable"].asBool();
							vecNode[i]->GetDynamicComponent<IRenderNode>()->iSetSelectEnable(bEnable);
						}
						vecNode[i]->GetDynamicComponent<IShell>()->iSetShellParam(stParam);

						if (vecNode[i]->iGetParent())
						{
							vecNode[i]->iGetParent()->GetDynamicComponent<IRenderNode>()->iGenerateChildMesh();
						}
					}
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetAppId")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strAppId = pWebData->vecValues[0].asString();
				m_pClient->m_sUserInfo.strLastAppId = m_pClient->m_sUserInfo.strAppId;
				m_pClient->m_sUserInfo.strAppId = strAppId;
			}
		}
		else if (pWebData->strCommand == "Web_PlanningRoadPath")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				Vector2 vStartLngLat = m_pClient->m_pWebLabelParser->ParseVector2(pWebData->vecValues[0]);
				Vector2 vEndLngLat = m_pClient->m_pWebLabelParser->ParseVector2(pWebData->vecValues[1]);
				std::string strStart = to_string(vStartLngLat[0]) + "," + to_string(vStartLngLat[1]);
				std::string strEnd = to_string(vEndLngLat[0]) + "," + to_string(vEndLngLat[1]);
				//调用后台接口
				ThreadRequestParam stParam;
				char szBuffer[1024] = { 0 };
				sprintf_s(szBuffer, sizeof(szBuffer), "%s/api/v10/pathPlan?from=%s&to=%s", m_pClient->m_strApiHost.c_str(),
					strStart.c_str(), strEnd.c_str());
				stParam.strUrl = szBuffer;
				stParam.eRequestType = THREAD_REQUEST_API_GET;
				stParam.pThreadCompleteFunc = static_callback_PlanningRoadPath;
				stParam.AddParam("this", (INT_PTR)this);
				stParam.AddExtraJValue("web_id", pWebData->strID);
				stParam.bThread = true;
				stParam.bRunCallbackInMainThread = false;
				ProjectBasedClient* pClient = (ProjectBasedClient*)(APIProvider::GetSystemAPI()->iProjectClientAPI);
				if (pClient)
				{
					pClient->AddHttpCommonParam(&stParam);
				}
				ThreadWrapper::GetInstance()->AddTask(stParam);
			}
		}
		else if (pWebData->strCommand == "Web_GetHeatmapIntervalTime")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strID = pWebData->vecValues[0].asString();
				HeatmapItemData* pItem = m_pClient->m_pHeatMapFactory->GetHeatmapItem(strID);
				if (pItem)
				{
					std::string strData = to_string(pItem->lHeatmapIntervalMillisecond);
					m_pClient->ToSendWebCommond("VS_GetHeatmapIntervalTime",pWebData->strID,strData);
				}
			}
		}
	}
	catch (const std::exception&)
	{
	}
}

bc::EventReturnType_e WebCommonApi::iProcessEvent(const BCEvent& tEvent)
{
	return EventReturnType_e::NONE;
}

void WebCommonApi::AddINTPTR(std::string strKey, INT_PTR pTr)
{
	m_mapINTPTR[strKey] = pTr;
}

INT_PTR WebCommonApi::GetINTPTR(std::string strKey)
{
	return m_mapINTPTR[strKey];
}

void WebCommonApi::iFrameUpdate()
{
	if (m_bRotateAroundCenter)
	{
		float fRadius = m_vAroundCenterStartCameraPos.Distance(m_vAroundCenterPos);
		m_fCurAroundCenterAngle += m_fAroundCenterSpeed;
		if (m_fCurAroundCenterAngle > 360.0f)
		{
			m_fCurAroundCenterAngle -= 360.0f;
		}
		Vector3d vCurCameraPos;
		vCurCameraPos.x = fRadius * sin(m_fCurAroundCenterAngle) + m_vAroundCenterPos.x;
		vCurCameraPos.y = fRadius * cos(m_fCurAroundCenterAngle) + m_vAroundCenterPos.y;
		vCurCameraPos.z = m_vAroundCenterStartCameraPos.z;
		Vector3d vCurCameraAngle = m_vAroundCenterStartCameraAngle;
		Axisd tAxis;
		tAxis.LookAt(vCurCameraPos, m_vAroundCenterPos, Vector3d(0, 0, 1));
		vCurCameraAngle.y = tAxis.ToAngles().y;
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->iSetOrigin(vCurCameraPos);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->iSetAngles(vCurCameraAngle);
		ICamera* pCamera = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->
			iGetCurrentView()->iGetMainCamera()->GetDynamicComponent<ICamera>();
	}
	for (std::map<std::string, TweenAnimateForNode*>::iterator it = m_mapTween.begin(); it != m_mapTween.end(); it++)
	{
		it->second->Update();
	}
}

void WebCommonApi::iPostFrameUpdate()
{
}

void WebCommonApi::static_callback_PlanningRoadPath(ThreadCallbackParam_s* pParam)
{
	WebCommonApi* pThis = (WebCommonApi*)pParam->sRequestParam.mapParams["this"];
	std::string strWebId = pParam->sRequestParam.GetExtraJValue("web_id").asString();

	Json::Value jArray = Json::arrayValue;
	Json::Value jItem = {};
	jItem["distance"] = 0.0f;
	jItem["path"] = "";
	jArray.append(jItem);
	std::string strData = jArray.toStyledString();
	if (pParam->nResult == STATUS_SUCCESS)
	{
		try
		{
			Json::Value& jData = pParam->jRoot["data"];
			strData = jData.toStyledString();
		}
		catch (const std::exception&)
		{

		}
	}
	pThis->m_pClient->ToSendWebCommond("VS_PlanningRoadPath", strWebId, strData);
}

