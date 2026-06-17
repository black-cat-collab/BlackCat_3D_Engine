#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/base/BuildIndoorStateFactory.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/base/SceneElementLoader.h"

using namespace bc;



BuildIndoorStateFactory::BuildIndoorStateFactory()
{
	m_pISystemAPI = APIProvider::GetSystemAPI();
	m_pSceneElementLoader = ((ProjectBasedClient*)m_pISystemAPI->iProjectClientAPI)->m_pSceneElementLoader;

	m_pCurAreaSceneData = nullptr;
	m_pCurBuildingGroupData = nullptr;
	m_pCurBuildingData = nullptr;
	m_pCurFloorData = nullptr;
	m_pCurRoomData = nullptr;


	m_strFCSuffix = "_FC";
	m_strSNSuffix = "_SN";
	m_strSWSuffix = "_SW";
	m_strWKSuffix = "_WK";
	m_strLABELSuffix = "_LABEL_";
	m_strBIGScene = "BIGSCENE";

	m_eServerVersion = EVersion::VERSION_V10;
}


BuildIndoorStateFactory::~BuildIndoorStateFactory()
{
	clearData();
}

void BuildIndoorStateFactory::Create()
{
	{
		vector<AreaSceneData*> vec;
		GetAllAreaSceneData(vec);
		for (int i = 0; i < vec.size(); i++)
		{
			if (vec[i]->pNode)
			{
				vec[i]->vecBuildingGroup = iGetBuildingGroupPerAreaScene(vec[i]->strAreaSceneNodeID, vec[i]);
				for (int j = 0; j < vec[i]->vecBuildingGroup.size(); j++)
				{
					vec[i]->vecBuildingGroup[j]->pAreaSceneData = vec[i];
				}
			}
		}
	}
	{
		vector<BuildingGroupData*> vec;
		GetAllBuildingGroupData(vec);
		for (int i = 0; i < vec.size(); i++)
		{
			if (vec[i]->pNode)
			{
				vec[i]->vecBuilding = iGetBuildingPerBuildingGroup(vec[i]->strNodeID, vec[i]);
				for (int j = 0; j < vec[i]->vecBuilding.size(); j++)
				{
					vec[i]->vecBuilding[j]->pBuildingGroupData = vec[i];
				}
			}
		}
	}
	{
		vector<BuildingData*> vec;
		GetAllBuildingData(vec);
		for (int i = 0; i < vec.size(); i++)
		{
			if (vec[i]->pNode)
			{
				vec[i]->vecFloorData = iGetFloorPerBuilding(vec[i]->strBuildingNodeID, vec[i]);
				////添加全关闭和全打开
				//{
				//	FloorData* pData = new FloorData;
				//	pData->nFloorIndex = FLOOR_ALL_CLOSE_INDEX;
				//	pData->strFloorNodeID = FLOOR_ALL_CLOSE_ID;
				//	vec[i]->vecFloorData.push_back(pData);
				//}
				//{
				//	FloorData* pData = new FloorData;
				//	pData->nFloorIndex = FLOOR_ALL_OPEN_INDEX;
				//	pData->strFloorNodeID = FLOOR_ALL_OPEN_ID;
				//	vec[i]->vecFloorData.push_back(pData);
				//}

				for (int j = 0; j < vec[i]->vecFloorData.size(); j++)
				{
					vec[i]->vecFloorData[j]->pBuildingData = vec[i];
				}
			}
		}
	}
	{
		vector<FloorData*> vec;
		GetAllFloorData(vec);
		for (int i = 0; i < vec.size(); i++)
		{
			if (vec[i]->pNode)
			{
				vec[i]->vInitOrigin = vec[i]->pNode->iGetOrigin();
				vec[i]->vecRoomData = iGetRoomPerFloor(vec[i]->strFloorNodeID, vec[i]);

				for (int j = 0; j < vec[i]->vecRoomData.size(); j++)
				{
					vec[i]->vecRoomData[j]->pFloorData = vec[i];
				}
			}
		}
	}
}

AreaSceneData* BuildIndoorStateFactory::ChangeAreaScene(string strID, bool bDynamicLoaded)
{
	bool bSame = false;
	if (m_pCurAreaSceneData)
	{
		bSame = (m_pCurAreaSceneData->strAreaSceneNodeID == strID);
	}
	//floorMove(FLOOR_ALL_CLOSE_ID);
	ChangeFloor(FLOOR_ALL_CLOSE_ID);
	if (m_pCurAreaSceneData)
	{
		m_pCurAreaSceneData->pNode->iSetVisible(false);
		if (!bSame && bDynamicLoaded)
		{
			m_pCurAreaSceneData->pNode->iFreeModel();
		}
	}
	m_pCurAreaSceneData = GetAreaSceneData(strID);
	if (!m_pCurAreaSceneData)
	{		
		return NULL;
	}
	m_pCurAreaSceneData->pNode->iSetVisible(true);
	if (!bSame && bDynamicLoaded)
	{
		m_pCurAreaSceneData->pNode->iLoadModel();
	}
	vector<BuildingData*> vecAllBuilding;
	GetAllBuildingData(vecAllBuilding, nullptr);
	for (int i = 0; i < vecAllBuilding.size(); i++)
	{
		INode* pWkNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(vecAllBuilding[i]->strBuildingNodeID + m_strWKSuffix);
		if (pWkNode)
		{
			pWkNode->iSetVisible(true);
		}
		INode* pFcNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(vecAllBuilding[i]->strBuildingNodeID + m_strFCSuffix);
		if (pFcNode)
		{
			pFcNode->iSetVisible(false);
		}
	}
	iEnterAreaScene(m_pCurAreaSceneData);
	return m_pCurAreaSceneData;
}

BuildingData* BuildIndoorStateFactory::ChangeBuilding(string strID, bool bDynamicLoaded)
{
	if (!m_pCurAreaSceneData)
	{
		return NULL;
	}
	//动态加载
	bool bSame = false;
	if (m_pCurBuildingData)
	{
		bSame = (m_pCurBuildingData->strBuildingNodeID == strID);
	}
	if (m_pCurBuildingData)
	{
		INode* pWkNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(m_pCurBuildingData->strBuildingNodeID + m_strWKSuffix);
		if (pWkNode)
		{
			pWkNode->iSetVisible(true);
		}
		INode* pFcNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(m_pCurBuildingData->strBuildingNodeID + m_strFCSuffix);
		if (pFcNode)
		{
			if (!bSame && bDynamicLoaded)
			{
				pFcNode->iFreeModel();
			}
			pFcNode->iSetVisible(false);
		}
	}
	//当前的
	m_pCurBuildingData = GetBuildingData(strID);
	//加载模型
	if (m_pCurBuildingData)
	{
		INode* pWkNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(m_pCurBuildingData->strBuildingNodeID + m_strWKSuffix);
		if (pWkNode)
		{
			pWkNode->iSetVisible(false);
		}
		INode* pFcNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(m_pCurBuildingData->strBuildingNodeID + m_strFCSuffix);
		if (pFcNode)
		{
			if (!bSame && bDynamicLoaded)
			{
				pFcNode->iLoadModel();
			}
			pFcNode->iSetVisible(true);
		}
	}	
	if (!m_pCurBuildingData)
	{
		ChangeFloor(FLOOR_ALL_CLOSE_ID);
		//floorMove(FLOOR_ALL_CLOSE_ID);
		return NULL;
	}
	if (!m_pCurBuildingData)
	{
		return nullptr;
	}

	iChangeBuilding(m_pCurBuildingData);
	ChangeFloor(FLOOR_ALL_CLOSE_ID);
	//floorMove(FLOOR_ALL_CLOSE_ID);
	//floorMove(FLOOR_ALL_OPEN_ID);
	return m_pCurBuildingData;
}

FloorData* BuildIndoorStateFactory::ChangeFloor(string strID)
{
	if (!m_pCurBuildingData)
	{
		return NULL;
	}
	if(strID != FLOOR_ALL_CLOSE_ID && strID != FLOOR_ALL_OPEN_ID)
	{
		m_pCurFloorData = GetFloorData(strID, m_pCurBuildingData);
		if (!m_pCurFloorData)
		{
			return nullptr;
		}
	}
	else
	{
		m_pCurFloorData = nullptr;
	}
	floorMove(strID);
	//调整上图元素数据
	RefreshSceneElement(strID);
	iChangeFloor(strID,m_pCurFloorData);

	m_strCurFloorID = strID;
	return m_pCurFloorData;
}

void BuildIndoorStateFactory::ChangeAlpha(float fAlpha)
{
	if (m_pCurBuildingData)
	{
		vector<INode*> vecAlphaNode = iGetAlphaNode(m_pCurBuildingData);
		if (vecAlphaNode.size() == 0)
		{
			vector<INode*> vecGeo;
			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetTypeNode(vecGeo,NODE_GEO, m_pCurBuildingData->pNode);
			for (INode *pNode : vecGeo)
			{
				vecAlphaNode.push_back(pNode);
			}
		}

		for (int i = 0; i < vecAlphaNode.size(); ++i)
		{
			IRenderNode* pRenderNode = static_cast<IRenderNode*>(vecAlphaNode[i]->iGetComponent(COMPONENT_TYPE_RENDER_NODE));
			if (!pRenderNode)
			{
				continue;
			}
			vector<IMaterial*> vecMaterial;
			pRenderNode->iGetMaterial(vecMaterial);
			for (int j = 0; j < vecMaterial.size(); ++j)
			{
				IMaterial *pMaterial = vecMaterial[j];
				if (pMaterial->iGetLightingMode() != LIGHTING_MODE_NONE)
				{
					pMaterial->iSetAlpha(fAlpha);
					StateParam tParam;
					pMaterial->iGetParam(tParam);
					tParam.nTransparentType = fAlpha<1.0f ? TRANSPARENT_AVERAGE : TRANSPARENT_NONE;
					pMaterial->iSetParam(tParam);
				}
			}
		}
	}
}

void BuildIndoorStateFactory::OpenEffectShader(bool bOpen)
{
	//if (APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.eRenderQuality >= NormalType)
	//{
		APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sShaderConfig.bSSAOShader = bOpen;
	//	APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sShaderConfig.bSunGodRayShader = bOpen;
	//	APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sShaderConfig.bDynamicShadowMapShader = bOpen;
	//}
	//APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sShaderConfig.bToneMappingShader = bOpen;
}


void BuildIndoorStateFactory::ChangeFog(bool bIndoor)
{
	if (bIndoor)
	{
		SceneFogParam tFogParam;
		tFogParam.vFogColor = Vector4(28.0f / 255.0f, 34.0f / 255.0f, 48.0f / 255.0f);
		//tFogParam.vFogColor = Vector3d(1, 1, 1);
		tFogParam.bDynamicSky = true;
		tFogParam.fHeightFallOff = 0.01;
		tFogParam.fDistanceFallOff = 0.0001;
		tFogParam.fLightInscatter = 2.0;
		tFogParam.fIntensity = 1.0;
		tFogParam.fStart = 500;
		tFogParam.fEnd = 5000;
		tFogParam.nType = FOG_EXPONENT;
		APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.sSceneFog = tFogParam;
		APIProvider::GetSystemAPI()->iRenderAPI->iUpdateSceneFog();
	}
	else
	{
		SceneFogParam tFogParam;
		tFogParam.bDynamicSky = false;
		tFogParam.fHeightFallOff = 0.0001;
		tFogParam.fDistanceFallOff = 0.00001;
		tFogParam.fLightInscatter = 2.0;
		tFogParam.fIntensity = 1.0;
		tFogParam.fStart = 5000;
		tFogParam.fEnd = 20000;
		tFogParam.nType = FOG_EXPONENT;
		APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.sSceneFog = tFogParam;
		APIProvider::GetSystemAPI()->iRenderAPI->iUpdateSceneFog();
	}
}


void BuildIndoorStateFactory::CloseAreaScene(bool bEnterAreaScene, bool bDynamicLoaded)
{
	if (!m_pCurAreaSceneData)
	{
		return;
	}
	//floorMove(FLOOR_ALL_CLOSE_ID);
	ChangeFloor(FLOOR_ALL_CLOSE_ID);
	iLeaveAreaScene(m_pCurAreaSceneData);

	if (bEnterAreaScene)
	{
		m_pCurAreaSceneData->pNode->iSetVisible(false);
		if (bDynamicLoaded)
		{
			m_pCurAreaSceneData->pNode->iFreeModel();
		}
	}
	m_pCurAreaSceneData = NULL;
	m_pCurBuildingGroupData = NULL;
	m_pCurBuildingData = NULL;
	m_pCurFloorData = NULL;
	m_strCurFloorID = "";
}

void BuildIndoorStateFactory::FrameUpdate()
{
	//添加楼层模型加载完成的回调
	std::vector<FloorData*> vecAllFloorData;
	GetAllFloorData(vecAllFloorData);
	for (size_t i = 0; i < vecAllFloorData.size(); i++)
	{
		FloorData* pFloor = vecAllFloorData[i];
		INode* pNode = pFloor->pNode;
		if (pNode && !pFloor->bNodeLoaded)
		{
			if (pNode->iIsLoad())
			{
				if (pNode->iGetNodeType() == NODE_GROUP)
				{
					//pFloor->vInitOrigin = Vector3(0, 0, 0);
					pFloor->vInitOrigin = pNode->iGetOrigin();
					//pFloor->vInitOrigin = pNode->iGetBBox().GetCenter().Getf();
				}
				else
				{
					pFloor->vInitOrigin = pNode->iGetOrigin();
				}

				pFloor->bNodeLoaded = true;
			}
		}
	}
	//调整上图元素位置信息
	changeSceneElementPos();

	iFrameUpdate();
}

void BuildIndoorStateFactory::GetAllAreaSceneData(vector<AreaSceneData*> &vecData)
{
	for (int i = 0; i < m_vecAreaSceneData.size(); i++)
	{
		vecData.push_back(m_vecAreaSceneData[i]);
	}
}

AreaSceneData* BuildIndoorStateFactory::GetAreaSceneData(string strID)
{
	for (int i = 0; i < m_vecAreaSceneData.size(); i++)
	{
		if (m_vecAreaSceneData[i]->strAreaSceneNodeID == strID)
		{
			return m_vecAreaSceneData[i];
		}
	}
	return NULL;
}

void BuildIndoorStateFactory::GetAllBuildingGroupData(vector<BuildingGroupData*>& vecData, AreaSceneData* pData)
{
	vector<AreaSceneData*> vec;
	GetAllAreaSceneData(vec);
	for (int i = 0; i < vec.size(); i++)
	{
		if (!pData || (vec[i] == pData))
		{
			for (int j = 0; j < vec[i]->vecBuildingGroup.size(); j++)
			{
				vecData.push_back(vec[i]->vecBuildingGroup[j]);
			}
		}
	}
}

BuildingGroupData* BuildIndoorStateFactory::GetBuildingGroupDataByNodeID(string strID, AreaSceneData* pData)
{
	vector<BuildingGroupData*> vecData;
	GetAllBuildingGroupData(vecData, pData);
	for (int i = 0; i < vecData.size(); i++)
	{
		if (vecData[i]->strNodeID == strID)
		{
			return vecData[i];
		}
	}
	return nullptr;
}

BuildingGroupData* BuildIndoorStateFactory::GetBuildingGroupDataByRealID(string strID, AreaSceneData* pData)
{
	vector<BuildingGroupData*> vecData;
	GetAllBuildingGroupData(vecData, pData);
	for (int i = 0; i < vecData.size(); i++)
	{
		if (vecData[i]->strRealID == strID)
		{
			return vecData[i];
		}
	}
	return nullptr;
}

void BuildIndoorStateFactory::GetAllBuildingData(vector<BuildingData*> &vecData, BuildingGroupData*pData)
{
	vector<BuildingGroupData*> vec;
	GetAllBuildingGroupData(vec);
	for (int i = 0; i < vec.size(); i++)
	{
		if (!pData || (vec[i] == pData))
		{
			for (int j = 0; j < vec[i]->vecBuilding.size(); j++)
			{
				vecData.push_back(vec[i]->vecBuilding[j]);
			}
		}
	}
}

BuildingData* BuildIndoorStateFactory::GetBuildingData(string strID, BuildingGroupData*pData)
{
	vector<BuildingData*> vecData;
	GetAllBuildingData(vecData, pData);
	for (int i = 0; i < vecData.size(); i++)
	{
		if (vecData[i]->strBuildingNodeID == strID)
		{
			return vecData[i];
		}
	}
	return NULL;
}

void BuildIndoorStateFactory::GetAllFloorData(vector<FloorData*> &vecData, BuildingData *pData)
{
	vector<BuildingData*> vec;
	GetAllBuildingData(vec);
	for (int i = 0; i < vec.size(); i++)
	{
		if (!pData || (vec[i] == pData))
		{
			for (int j = 0; j < vec[i]->vecFloorData.size(); j++)
			{
				vecData.push_back(vec[i]->vecFloorData[j]);
			}
		}
	}
}

FloorData* BuildIndoorStateFactory::GetFloorData(string strID, BuildingData *pData)
{
	vector<FloorData*> vecData;
	GetAllFloorData(vecData, pData);
	for (int i = 0; i < vecData.size(); i++)
	{
		if (vecData[i]->strFloorNodeID == strID)
		{
			return vecData[i];
		}
	}
	return NULL;
}

void BuildIndoorStateFactory::GetAllRoomData(vector<RoomData*> &vecData, FloorData *pData)
{
	vector<FloorData*> vec;
	GetAllFloorData(vec);
	for (int i = 0; i < vec.size(); i++)
	{
		if (!pData || (vec[i] == pData))
		{
			for (int j = 0; j < vec[i]->vecRoomData.size(); j++)
			{
				vecData.push_back(vec[i]->vecRoomData[j]);
			}
		}
	}
}

RoomData* BuildIndoorStateFactory::GetRoomData(string strID, FloorData *pData)
{
	vector<RoomData*> vecData;
	GetAllRoomData(vecData, pData);
	for (int i = 0; i < vecData.size(); i++)
	{
		if (vecData[i]->strRoomNodeID == strID)
		{
			return vecData[i];
		}
	}
	return NULL;
}

void BuildIndoorStateFactory::floorMove(string strNodeID)
{
	if (!m_pCurBuildingData)
	{
		return;
	}
	ITimeLine *pTimeLine = NULL;
	int nTimeLineIndex = 0;
	while (1)
	{
		string strTimeLine = "BuildingFloorTimeLine" + to_string(nTimeLineIndex);
		pTimeLine = dynamic_cast<ITimeLine*>(APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindObject(strTimeLine));
		if (pTimeLine)
		{
			if (pTimeLine->iGetState() != TIME_LINE_STOP)
			{
				++nTimeLineIndex;
				continue;
			}

			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteObject(strTimeLine);
		}

		ObjectParamBase stParam;
		stParam.nObjectType = OBJECT_TYPE_TIME_LINE;
		stParam.strID = strTimeLine;
		pTimeLine = dynamic_cast<ITimeLine*>(APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeObject(stParam));
		pTimeLine->iAddKeyFrame(600.0f);
		break;
	}

	bool bAllClose = true;
	for (int i = 0; i < m_pCurBuildingData->vecFloorData.size(); ++i)
	{
		FloorData* pInfo = m_pCurBuildingData->vecFloorData[i];
		if (!pInfo || pInfo->pNode == NULL)
		{
			continue;
		}
		TimeLineObjectTransform* pObject = new TimeLineObjectTransform(m_pISystemAPI, pInfo->pNode);
		pObject->SetStartValue(TimeLineObjectValue(pInfo->pNode->iGetOrigin()));
		Vector3d vBaseInitPos = Vector3d(0,0,0);
		if (pInfo->pNode->iGetNodeType() != NODE_GROUP)
		{
			vBaseInitPos = pInfo->vInitOrigin;
		}
		else
		{
			//vBaseInitPos.z = pInfo->vInitOrigin.z;
			vBaseInitPos = pInfo->vInitOrigin;
		}
		pTimeLine->iBindObject(pObject);
		ProjectBasedClient* pClient = ((ProjectBasedClient*)m_pISystemAPI->iProjectClientAPI);
		if (strNodeID == FLOOR_ALL_OPEN_ID)
		{
			Vector3d vTargetPos = vBaseInitPos;
			pClient->AdjustZ(vTargetPos, i * pInfo->fAllOpenSpace, true);
			pObject->AddKeyFrameValue(TimeLineObjectValue(vTargetPos));
			bAllClose = false;
		}
		else if (strNodeID == FLOOR_ALL_CLOSE_ID)
		{
			pObject->AddKeyFrameValue(TimeLineObjectValue(Vector3d(vBaseInitPos)));
		}
		else
		{
			if (m_pCurFloorData == NULL)
			{
				return;
			}
			if (pInfo->nFloorIndex > m_pCurFloorData->nFloorIndex)
			{
				bAllClose = false;
				Vector3d vTargetPos = vBaseInitPos;
				pClient->AdjustZ(vTargetPos, pInfo->fFlyHeight, true);
				pObject->AddKeyFrameValue(TimeLineObjectValue(vTargetPos));
			}
			else
			{
				pObject->AddKeyFrameValue(TimeLineObjectValue(Vector3d(vBaseInitPos)));
			}
		}

	}

	pTimeLine->iPlay(TIME_LINE_PLAY_FROM_START);
}

vector<INode*> BuildIndoorStateFactory::iGetAlphaNode(BuildingData* pData)
{
	vector<INode*> vecNode;
	return vecNode;
}

void BuildIndoorStateFactory::clearData()
{
	for (size_t i = 0; i < m_vecAreaSceneData.size(); i++)
	{
		DELETE_PTR(m_vecAreaSceneData[i]);
	}
	m_vecAreaSceneData.resize(0);
}

void BuildIndoorStateFactory::RefreshSceneElement(std::string& strFloorID)
{
	if (!m_pCurBuildingData || !m_pSceneElementLoader)
	{
		return;
	}
	//vector<CommonLabelFactory*> vecFactory;
	//m_pSceneElementLoader->GetAllInElementFacotory(vecFactory);
	//for (int i = 0; i < vecFactory.size(); i++)
	//{
	//	vector<ProjectBasedSceneElement*> vecSceneElement;
	//	m_pSceneElementLoader->GetInSceneElementByType(vecFactory[i]->GetFlagType(), vecSceneElement);
	//	vector<ProjectBasedSceneElement*> vecResult;
	//	for (int n = 0; n < vecSceneElement.size(); n++)
	//	{
	//		bool bFind = false;
	//		for (int j = 0; j < m_pCurBuildingData->vecFloorData.size(); j++)
	//		{
	//			if (m_pCurBuildingData->vecFloorData[j]->strFloorNodeID == vecSceneElement[n]->strFloorNodeID)
	//			{
	//				bFind = true;
	//				break;
	//			}
	//		}
	//		if (bFind)
	//		{
	//			if (strFloorID == FLOOR_ALL_OPEN_ID || strFloorID == vecSceneElement[n]->strFloorNodeID)
	//			{
	//				vecResult.push_back(vecSceneElement[n]);
	//			}
	//		}
	//	}

	//	vecFactory[i]->RefreshData((vector<INT_PTR>*) & vecResult);
	//}
}

void BuildIndoorStateFactory::changeSceneElementPos()
{
	if (!m_pCurBuildingData || !m_pSceneElementLoader)
	{
		return;
	}

	//vector<CommonLabelFactory*> vecFactory;
	//m_pSceneElementLoader->GetAllInElementFacotory(vecFactory);
	//for (int m = 0; m < vecFactory.size(); m++)
	//{
	//	vector<AssembleLabelDataAndNode*> vecAssemble;
	//	vecFactory[m]->GetCurAssembles(vecAssemble);
	//	for (int i = 0; i < m_pCurBuildingData->vecFloorData.size(); i++)
	//	{
	//		if (m_pCurBuildingData->vecFloorData[i]->pNode == NULL)
	//		{
	//			continue;
	//		}

	//		FloorData* pFloor = m_pCurBuildingData->vecFloorData[i];
	//		if (pFloor)
	//		{
	//			Vector3d vFloorPos = pFloor->pNode->iGetBBox().GetCenter();
	//			float fPosZ = vFloorPos.z + pFloor->fLabelOffsetZ;
	//			float foffsetZ = vFloorPos.z - pFloor->vInitOrigin.z;

	//			for (int j = 0; j < vecAssemble.size(); j++)
	//			{
	//				AssembleLabelDataAndNode* pDataAndNode = vecAssemble[j];
	//				if (pDataAndNode == NULL || !pDataAndNode->pNode)
	//				{
	//					continue;
	//				}
	//				ProjectBasedSceneElement* pInfo = (ProjectBasedSceneElement*)pDataAndNode->pData;
	//				if (pInfo->strFloorNodeID == pFloor->strFloorNodeID)
	//				{
	//					if (pInfo->strNodePath.empty())
	//					{
	//						Vector3d vTargetPosition = pInfo->vLabelPos;
	//						if (pInfo->bDoPositonPost)
	//						{
	//							vTargetPosition.z += foffsetZ;
	//						}
	//						else
	//						{
	//							vTargetPosition.z = fPosZ;
	//						}
	//						pDataAndNode->pNode->iSetOrigin(vTargetPosition);
	//					}
	//					else
	//					{
	//						Vector3d vOrigin = pInfo->vInitNodePos;
	//						vOrigin.z += foffsetZ;
	//						pDataAndNode->pNode->iSetOrigin(vOrigin);
	//					}

	//					for (int n1 = 0; n1 < pInfo->vecExtraNode.size(); n1++)
	//					{
	//						INode* pExtraNode = pInfo->vecExtraNode[n1];
	//						if (pExtraNode->iGetNodeType() != NODE_LOCUS_LINE)
	//						{
	//							Vector3d vOrigin = pInfo->vecExtraNode[n1]->iGetOrigin();
	//							if (pInfo->bDoPositonPost)
	//							{
	//								vOrigin.z += foffsetZ;
	//							}
	//							else
	//							{
	//								vOrigin.z = fPosZ;
	//							}
	//							pInfo->vecExtraNode[n1]->iSetOrigin(vOrigin);
	//						}
	//						else
	//						{
	//							std::vector<Vector3d> vecPoints = pInfo->vecPoint;
	//							for (int mm = 0; mm < vecPoints.size(); mm++)
	//							{
	//								if (pInfo->bDoPositonPost)
	//								{
	//									vecPoints[mm].z = foffsetZ;
	//								}
	//								else
	//								{
	//									vecPoints[mm].z = fPosZ;
	//								}
	//							}
	//							ILocusLine* pLocusLine = GetComponent<ILocusLine>(pExtraNode);
	//							LocusLineParam* pLineParam = (LocusLineParam*)pExtraNode->iGetNodeParam();
	//							vector<LocusLineSegment> vecSeg;
	//							LocusLineSegment sSegment = pLineParam->stDefaultSegment;

	//							sSegment.vecVertex = vecPoints;
	//							vecSeg.push_back(sSegment);
	//							pLocusLine->iUpdateLocusLineSegment(vecSeg);
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
}

AreaSceneData* BuildIndoorStateFactory::iCreateAreaSceneData()
{
	return new AreaSceneData();
}

BuildingGroupData* BuildIndoorStateFactory::iCreateBuildingGroupData()
{
	return new BuildingGroupData();
}

BuildingData* BuildIndoorStateFactory::iCreateBuildingData()
{
	return new BuildingData();
}

FloorData* BuildIndoorStateFactory::iCreateFloorData()
{
	return new FloorData();
}

RoomData* BuildIndoorStateFactory::iCreateRoomData()
{
	return new RoomData();
}

