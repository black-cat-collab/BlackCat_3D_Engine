#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/base/SceneElementLoader.h"
#include "LayerDebugSceneLoader.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/utils/ParamParseStream.h"
#include "Project-Based/label/LabelDetailFactory.h"
#include "Project-Based/label/func/OverTapFunc.h"

LayerDebugSceneLoader::LayerDebugSceneLoader():
	SceneElementLoader(),
	m_bShow(true),
	m_bToolsSceneFactoryShow(true)
{

}

LayerDebugSceneLoader::~LayerDebugSceneLoader()
{

}

void LayerDebugSceneLoader::loadDataComplete(bool bAllComplete, bool bToWebCommond)
{
	if (bAllComplete)
	{
		clearFactory();
		//预先创建Factory
		for (size_t i = 0; i < m_vecSceneTypeInfo.size(); ++i)
		{
			doCreateFactory(&m_vecSceneTypeInfo, 0, m_vecSceneTypeInfo[i]->nTypeID);
		}
		//请求元素数据
		requestElementShow();
	}
}

void LayerDebugSceneLoader::requestElementShow()
{
	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_CUSTOM;
	stParam.pThreadRuningFunc = static_request_ElementShow;
	stParam.pThreadCompleteFunc = static_callback_ElementShow;
	stParam.bRunCallbackInMainThread = true;
	stParam.bThread = true;
	m_pClient->AddHttpCommonParam(&stParam);
	InnerRequestEndData* pEndData = new InnerRequestEndData;
	copyMapAndTypeInfo(pEndData);

	stParam.AddParam("end_data", (INT_PTR)pEndData);
	stParam.AddParam("this", (INT_PTR)this);

	ThreadWrapper::GetInstance()->AddTask(stParam);
}

bool LayerDebugSceneLoader::static_request_ElementShow(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam* stCallbackParam)
{
	LayerDebugSceneLoader* pThis = (LayerDebugSceneLoader*)mapParams["this"];
	BCAutoLock lock(&pThis->m_ParseElementShowLock);
	{
		InnerRequestEndData* pEndData = (InnerRequestEndData*)mapParams["end_data"];
		pThis->requestSceneElement(pEndData, stCallbackParam);
	}

	return true;
}

void LayerDebugSceneLoader::static_callback_ElementShow(ThreadCallbackParam* pParam)
{
	LayerDebugSceneLoader* pThis = (LayerDebugSceneLoader*)pParam->sRequestParam.mapParams["this"];
	InnerRequestEndData* pEndData = (InnerRequestEndData*)pParam->sRequestParam.mapParams["end_data"];
	InnerPageData* pPageData = (InnerPageData*)pParam->sRequestParam.mapParams["pageData"];
	if (pPageData)
	{
		if (pPageData->bClear)
		{
			pThis->ClearElementShow();
		}
		pThis->m_vecResidentElement = pPageData->vecData;
		pThis->createFactory(&pEndData->vecTempSceneTypeInfo);
		if (pPageData->bComplete)
		{
			for (size_t i = 0; i < pEndData->vecTempMapInfo.size(); i++)
			{
				DELETE_PTR(pEndData->vecTempMapInfo[i]);
			}
			pEndData->vecTempMapInfo.resize(0);
			for (size_t i = 0; i < pEndData->vecTempSceneTypeInfo.size(); i++)
			{
				DELETE_PTR(pEndData->vecTempSceneTypeInfo[i]);
			}
			pEndData->vecTempSceneTypeInfo.resize(0);
			DELETE_PTR(pEndData);
		}
	}
	DELETE_PTR(pPageData);
}

void LayerDebugSceneLoader::copyMapAndTypeInfo(InnerRequestEndData* pData)
{
	//copy 一份type和map数据
	for (size_t i = 0; i < m_vecSceneTypeInfo.size(); i++)
	{
		ProjectBasedSceneElementTypeInfo* pInfo = new ProjectBasedSceneElementTypeInfo;
		*pInfo = *m_vecSceneTypeInfo[i];
		pData->vecTempSceneTypeInfo.push_back(pInfo);
	}
	for (size_t i = 0; i < m_vecMapInfo.size(); i++)
	{
		ProjectBasedSceneElementMapInfo* pInfo = new ProjectBasedSceneElementMapInfo;
		*pInfo = *m_vecMapInfo[i];
		pData->vecTempMapInfo.push_back(pInfo);
	}
}

void LayerDebugSceneLoader::requestSceneElement(InnerRequestEndData* pRequestEndData, ThreadCallbackParam* stCallbackParam)
{
	try
	{
		char szBuffer[1024] = { 0 };
		EServerServion eType = APIProvider::GetSystemAPI()->iProtocolAPI->iGetServerVersion();
		std::string strAppName = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strToolProjectName;
		ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
		sprintf_s(szBuffer, sizeof(szBuffer), "%s/api/hcd/element/detailList?planId=static&userToken=%s&project_name=%s", m_strServerHost.c_str(),
			m_strLayerDebuggToken.c_str(),strAppName.c_str());
		string strUrl = string(szBuffer);

		ThreadCallbackParam stCallback;
		ThreadRequestParam stParam;
		stParam.eRequestType = THREAD_REQUEST_API_GET;
		stParam.strUrl = strUrl;
		m_pClient->AddHttpCommonParam(&stParam);
		ThreadWrapper::GetInstance()->DoNetRequest(stParam, &stCallback);
		if (stCallback.nResult == STATUS_SUCCESS)
		{
			//重新显示数据
			{
				InnerPageData* pPageData = new InnerPageData;
				pPageData->vecData = pRequestEndData->vecSceneElement;
				pPageData->bClear = true;
				addPageTaskToMain(pPageData, stCallbackParam);
			}

			Json::Value root = stCallback.jRoot["data"];
			for (int i = 0; i < root.size(); i++)
			{
				ProjectBasedSceneElement* pInfo = new ProjectBasedSceneElement;
				parseElement(pInfo, root[i], pRequestEndData);
				pRequestEndData->vecSceneElement.push_back(pInfo);

				if (pRequestEndData->vecSceneElement.size() % 5000 == 0)
				{
					//给到主线程进行渲染
					{
						InnerPageData* pPageData = new InnerPageData;
						pPageData->vecData = pRequestEndData->vecSceneElement;
						pPageData->bClear = false;
						addPageTaskToMain(pPageData, stCallbackParam);
					}
				}
			}

			{
				InnerPageData* pPageData = new InnerPageData;
				pPageData->vecData = pRequestEndData->vecSceneElement;
				pPageData->bClear = false;
				pPageData->bComplete = true;
				addPageTaskToMain(pPageData, stCallbackParam);
			}
		}
	}
	catch (const std::exception&)
	{

	}
}

void LayerDebugSceneLoader::addPageTaskToMain(InnerPageData* pPageData, ThreadCallbackParam* stCallbackParam)
{
	ThreadCallbackParam* pCallbackParam = new ThreadCallbackParam;
	pCallbackParam->sRequestParam = stCallbackParam->sRequestParam;
	pCallbackParam->sRequestParam.bRepeat = false;
	Guid::CreateGuId(pCallbackParam->sRequestParam.strID, "Thread_");
	pCallbackParam->sRequestParam.AddParam("pageData", (INT_PTR)pPageData);
	ThreadWrapper::GetInstance()->AddMainRunCallback(pCallbackParam);
}

void LayerDebugSceneLoader::parseElement(ProjectBasedSceneElement* pInfo, Json::Value root, InnerRequestEndData* pEndData)
{
	if (!pInfo)
	{
		return;
	}

	try
	{
		char sz[8 * 1024] = { 0 };
		if (root.isMember("plan_id"))
		{
			pInfo->strPlanID = root["plan_id"].asString();
		}
		pInfo->nID = root["id"].asInt();
		pInfo->strUuid = root["uuid"].asString();
		pInfo->nTypeID = root["type"].asInt();
		if (root.isMember("usage_type"))
		{
			pInfo->nTypeID = root["usage_type"].asInt();
		}
		pInfo->nMapID = root["map_id"].asInt();
		if (root.isMember("site_id"))
		{
			pInfo->nSiteID = root["site_id"].asInt();
		}
		if (root.isMember("layer_id"))
		{
			pInfo->nLayerID = root["layer_id"].asInt();
		}
		if (root.isMember("userid"))
		{
			pInfo->nUserid = root["userid"].asInt();
		}
		if (root.isMember("elem_type"))
		{
			pInfo->nElemType = root["elem_type"].asInt();
		}
		if (!root["title"].isNull())
		{
			Utf8ToAscii(root["title"].asCString(), sizeof(sz), sz);
			pInfo->strTitle = string(sz);
		}
		//utf8_to_ascii(root["description"].asCString(), sizeof(sz), sz);
		//pInfo->strDescription = string(sz);
		pInfo->strDescription = root["description"].asString();
		string strStyle = root["style"].asString();
		pInfo->strStyle = strStyle;
		pInfo->strPos = root["pos"].asString();
		//解析位置
		if (pInfo->strPos.find(":") != string::npos)
		{
			vector<string> vec;
			SplitStringBySpecial(pInfo->strPos, vec, ":");
			for (int j = 0; j < vec.size(); j++)
			{
				Vector3d vPos;
				sscanf_s(vec[j].c_str(), "%lf,%lf,%lf", &vPos.x, &vPos.y, &vPos.z);
				pInfo->vecPoint.push_back(vPos);

			}
		}
		else
		{
			sscanf_s(pInfo->strPos.c_str(), "%lf,%lf,%lf", &pInfo->vLabelPos.x, &pInfo->vLabelPos.y, &pInfo->vLabelPos.z);
		}
		ProjectBasedSceneElementMapInfo* pTempMapInfo = NULL;
		std::vector<ProjectBasedSceneElementMapInfo*>* pVector = nullptr;
		if (pEndData)
		{
			pVector = &pEndData->vecTempMapInfo;
		}
		ProjectBasedSceneElementMapInfo* pMapInfo = getElementMapInfoByID(pInfo->nMapID, pVector);
		if (!pMapInfo)
		{
			pTempMapInfo = new ProjectBasedSceneElementMapInfo;
			pMapInfo = pTempMapInfo;
		}

		if (root.isMember("label_lnglat"))
		{
			string strPos = root["label_lnglat"].asString();
			if (strPos != "")
			{
				float fLng, fLat;
				sscanf_s(strPos.c_str(), "%f,%f", &fLng, &fLat);
				m_pClient->ConvertToVector3WithCoorSystem(fLng, fLat, &pInfo->vLabelPos, pMapInfo->eCoorType, true);
			}
		}

		string strLngLats = root["lnglats"].asString();
		pInfo->strLngLats = strLngLats;
		if (pInfo->vecPoint.size() == 0 && strLngLats.find(":") != string::npos)
		{
			//多值
			vector<string> vec;
			SplitStringBySpecial(strLngLats, vec, ":");
			if (pInfo->nElemType == 4)
			{
				if (vec.size() < 2)
				{
					return;
				}
				//圆
				string strLngLatPos = vec[0];
				Vector2 vCenterLngLatPos;
				Vector3d vCenterScenePos;
				sscanf_s(strLngLatPos.c_str(), "%f,%f", &vCenterLngLatPos.x, &vCenterLngLatPos.y);
				m_pClient->ConvertToVector3WithCoorSystem(vCenterLngLatPos.x, vCenterLngLatPos.y, &vCenterScenePos, pMapInfo->eCoorType, true);
				float fRadius = atof(vec[1].c_str());
				//分割
				int nCount = 100;
				float fDeltaAngle = 360.0f / nCount;
				for (int zz = 0; zz < nCount; zz++)
				{
					Vector3d vScenePos;
					float fAngle = zz * fDeltaAngle;
					double fX = vCenterScenePos.x + fRadius * sin(DEG_TO_RAD(fAngle));
					double fY = vCenterScenePos.y + fRadius * cos(DEG_TO_RAD(fAngle));
					m_pClient->ConvertToVector3WithCoorSystem(fX, fY, &vScenePos, COOR_SCENE_LOCAL, true);
					pInfo->vecPoint.push_back(vScenePos);
				}
			}
			else
			{
				for (int j = 0; j < vec.size(); j++)
				{
					if (vec[j].find("zIndex") != string::npos)
					{
						continue;
					}
					Vector3d vPos;
					sscanf_s(vec[j].c_str(), "%lf,%lf,%lf", &vPos.x, &vPos.y, &vPos.z);
					if (!pMapInfo->strNodeName.empty())
					{
						Vector3d vScenePos = Vector3d(vPos.x, vPos.y, 0);
						pInfo->vecPoint.push_back(vScenePos);
					}
					else
					{
						Vector3d vScenePos;
						m_pClient->ConvertToVector3WithCoorSystem(vPos.x, vPos.y, &vScenePos, pMapInfo->eCoorType, true);
						if (pMapInfo->eCoorType == COOR_SCENE_LOCAL)
						{
							vScenePos.z = vPos.z;
						}
						pInfo->vecPoint.push_back(vScenePos);
					}
				}
			}
		}
		else if (pInfo->vLabelPos.Empty() && strLngLats.find(":") == string::npos)
		{
			sscanf_s(strLngLats.c_str(), "%lf,%lf,%lf", &pInfo->dLng, &pInfo->dLat, &pInfo->dZ);
			if (!pMapInfo->strNodeName.empty())
			{
				Vector3d vScenePos = Vector3d(pInfo->dLng, pInfo->dLat, pInfo->dZ);
				pInfo->vLabelPos = vScenePos;
			}
			else
			{
				Vector3d vScenePos;
				m_pClient->ConvertToVector3WithCoorSystem(pInfo->dLng, pInfo->dLat, &vScenePos, pMapInfo->eCoorType, true);
				if (pMapInfo->eCoorType == COOR_SCENE_LOCAL)
				{
					vScenePos.z = pInfo->dZ;
				}
				pInfo->vLabelPos = vScenePos;
			}
		}

		pInfo->strNodePath = root["node_path"].asString();
		string strNodePos = root["node_pos"].asString();
		{
			vector<string> vec;
			SplitStringBySpecial(strNodePos, vec, ",");
			if (vec.size() >= 3)
			{
				sscanf_s(strNodePos.c_str(), "%lf,%lf,%lf", &pInfo->vInitNodePos.x, &pInfo->vInitNodePos.y, &pInfo->vInitNodePos.z);
			}
			else if (vec.size() >= 2)
			{
				double dLng = atof(vec[0].c_str());
				double dLat = atof(vec[1].c_str());
				m_pClient->ConvertToVector3WithCoorSystem(dLng, dLat, &pInfo->vInitNodePos, pMapInfo->eCoorType, true);
			}
		}
		////多指取最大的z值
		//float fMaxz = 0;
		//for (int i = 0; i < pInfo->vecPos.size(); i++)
		//{
		//	if (pInfo->vecPos[i].vPos.z > fMaxz)
		//	{
		//		fMaxz = pInfo->vecPos[i].vPos.z;
		//	}
		//}
		//标签位置
		if (pInfo->vLabelPos.Empty())
		{
			Vector3d vPos;
			if (pInfo->vecPoint.size() > 0)
			{
				for (int i = 0; i < pInfo->vecPoint.size(); i++)
				{
					vPos += pInfo->vecPoint[i];
				}
				vPos /= pInfo->vecPoint.size();
			}
			else
			{
				vPos = pInfo->vInitNodePos;
			}
			pInfo->vLabelPos = vPos;
		}

		if (pTempMapInfo)
		{
			delete pTempMapInfo;
			pTempMapInfo = NULL;
		}
	}
	catch (const std::exception&)
	{

	}
}

void LayerDebugSceneLoader::iDoWebCommond(WebCommandData* pWebData)
{
	try
	{
		if (pWebData->strCommand == "Web_LayerDebug_ChangeTypeStyle")
		{
			SceneElementLoader::iDoWebCommond(pWebData);
		}

		if (pWebData->strCommand == "Web_SetLayerDebugToken")
		{
			if (pWebData->vecValues.size() > 0)
			{
				std::string strToken = pWebData->vecValues[0].asString();
				m_strLayerDebuggToken = strToken;
			}
		}
		else if (pWebData->strCommand == "Web_ShowToolLabelType")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strTypes = pWebData->vecValues[0].asString();
				std::vector<std::string> vecType;
				SplitStringBySpecial(strTypes, vecType, ",");
				m_vecCurToolShowType = vecType;
				ChangeShowLabel();
			}
		}
		else if (pWebData->strCommand == "Web_Refresh")
		{
			iRefreshData(true);
			if (m_stLayerDebugParam.pLocusLine)
			{
				m_stLayerDebugParam.pLocusLine->iSetVisible(false);
			}
		}
		else if (pWebData->strCommand == "Web_GrapPos")
		{
			if (pWebData->vecValues.size() > 0)
			{
				m_stLayerDebugParam.nType = pWebData->vecValues[0].asInt();
				m_stLayerDebugParam.nPointID = pWebData->vecValues[1].asInt();
				m_stLayerDebugParam.strWebCommondID = pWebData->strID;
				m_stLayerDebugParam.bGetPos = true;
				m_vecPos.clear();
				if (m_stLayerDebugParam.pLocusLine)
				{
					m_stLayerDebugParam.pLocusLine->iSetVisible(false);
				}
			}
		}
		else if (pWebData->strCommand == "Web_Jump")
		{
			if (pWebData->vecValues.size() > 0)
			{
				vector<string> vecStrPos;
				vector<Vector3d> vecPos;
				string strPos = pWebData->vecValues[0].asString();
				SplitStringBySpecial(strPos, vecStrPos, ":");
				for (int i = 0; i < vecStrPos.size(); i++)
				{
					Vector3d vOrigin;
					sscanf_s(vecStrPos[i].c_str(), "%lf, %lf, %lf", &vOrigin.x, &vOrigin.y, &vOrigin.z);
					vecPos.push_back(vOrigin);
				}
				INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
				ICamera* pCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
				if (vecPos.size() > 1)
				{
					int n = vecPos.size();
					GoViewPoint(pCameraNode, vecPos[n / 2], 100, 300);
				}
				else if (vecPos.size() == 1)
				{
					GoViewPoint(pCameraNode, vecPos[0], 30, 100);
				}
			}
		}
		else if (pWebData->strCommand == "Web_ToolWebShow")
		{
			//m_bToolsSceneFactoryShow = pWebData->vecValues[0].asBool();
			//std::vector<CommonLabelFactory*> vecFactory;
			//GetAllOutElementFacotory(vecFactory);
			//GetAllInElementFacotory(vecFactory);
			//for (size_t i = 0; i < vecFactory.size(); i++)
			//{
			//	vecFactory[i]->SetAllVisible(m_bToolsSceneFactoryShow);
			//}
			//SetShow(m_bToolsSceneFactoryShow);
		}
		else if (pWebData->strCommand == "Web_LayerDebug_ChangePos")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				if (m_stLayerDebugParam.pLocusLine)
				{
					m_vecPos.clear();
					m_stLayerDebugParam.pLocusLine->iSetVisible(false);
				}

				Json::Value& jObject = pWebData->vecValues[0];
				int nType = pWebData->vecValues[1].asInt();     //1:添加     2：修改     3：删除
				ProjectBasedSceneElement* pInfo = new ProjectBasedSceneElement;
				parseElement(pInfo, jObject, nullptr);

				CommonLabelFactory* pFactory = GetOutElementFactoryByType(pInfo->nTypeID);
				if (pFactory)
				{
					FactoryAssemble *pAssemble = pFactory->FindAssembleByID(pInfo->strUuid);
					if (nType == 1)
					{
						if (!pAssemble)
						{
							std::vector<ProjectBasedSceneElement*> vec;
							vec.push_back(pInfo);
							pFactory->AddData(vec);
							//添加到所有数据数组中
							m_vecResidentElement.push_back(pInfo);
						}
					}
					else if (nType == 2)
					{
						if (pAssemble)
						{
							ProjectBasedSceneElement* pLastInfo = pAssemble->pData;
							pInfo->jProjectExtra = pLastInfo->jProjectExtra;

							*pLastInfo = *pInfo;

							std::vector<FactoryAssemble*> vec;
							vec.push_back(pAssemble);
							pFactory->UpdateData(vec);

							DELETE_PTR(pInfo);
						}
					}
					else if (nType == 3)
					{
						if (pAssemble)
						{
							//从所有数据数组中移除
							vector<ProjectBasedSceneElement*>::iterator it = find(m_vecResidentElement.begin(), m_vecResidentElement.end(), pAssemble->pData);
							if (it != m_vecResidentElement.end())
							{
								m_vecResidentElement.erase(it);
							}
							ProjectBasedSceneElement *pInfo = pFactory->RemoveByAssemble(pAssemble);
							DELETE_PTR(pInfo);
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

void LayerDebugSceneLoader::clearFactory()
{
	ParamParseStream::GetInstance()->Clear(SCENE_ELEMENT_PARAM_KEY);
	m_mapOutElementFactory.clear();
	m_mapInElementFactory.clear();

	for (auto pFactory : m_vecElementFactory)
	{
		pFactory->SetAllVisible(false);
		pFactory->RefreshZero();
		if (find(m_vecWillDeleteFactory.begin(), m_vecWillDeleteFactory.end(), pFactory) == m_vecWillDeleteFactory.end())
		{
			m_vecWillDeleteFactory.push_back(pFactory);
		}
	}
	m_vecElementFactory.clear();
}

void LayerDebugSceneLoader::ChangeShowLabel()
{
	vector<CommonLabelFactory*> vecFactory;
	GetAllOutElementFacotory(vecFactory);
	GetAllInElementFacotory(vecFactory);
	for (int i = 0; i < vecFactory.size(); i++)
	{
		bool bShow = m_bShow;
		if (find(m_vecCurToolShowType.begin(), m_vecCurToolShowType.end(), to_string(vecFactory[i]->GetType())) == m_vecCurToolShowType.end())
		{
			bShow = false;
		}
		vecFactory[i]->SetAllVisible(bShow);
	}
}

EventReturnType_e LayerDebugSceneLoader::iProcessEvent(const BCEvent& tEvent)
{
	SceneElementLoader::iProcessEvent(tEvent);

	if (tEvent.eMessage == EVENT_MOUSE)
	{
		switch (tEvent.nParam1)
		{
		case MOUSE_RBUTTONUP:
		{
			if (m_stLayerDebugParam.bGetPos)
			{
				if (1 == m_stLayerDebugParam.nType)
				{
					// 获取单个三维点位坐标
					getPointPos();
				}
				else if (2 == m_stLayerDebugParam.nType)
				{
					// 获取多个点位三维坐标
					addPointPos(false);
				}
			}
		}
		break;
		case MOUSE_LBUTTONUP:
		{
			if (m_stLayerDebugParam.bGetManyPos)
			{
				addPointPos(true);
			}
		}
		break;
		}
	}

	return EventReturnType_e::NONE;
}

void LayerDebugSceneLoader::getPointPos()
{
	INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
	ICamera* pCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
	char szPos[LENGTH_1K] = { 0 };

	IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));
	Vector3d vPos = pManipulator->iGetMousePoint();
	sprintf_s(szPos, sizeof(szPos), "%f, %f, %f", vPos.x, vPos.y, vPos.z);
	vPos.z += 2.0f;

	// 获取鼠标三维坐标，发送给网页
	char szBuffer[LENGTH_1K] = { 0 };
	sprintf_s(szBuffer, sizeof(szBuffer), "%d, %d, '%s'", m_stLayerDebugParam.nType, m_stLayerDebugParam.nPointID, szPos);
	m_pClient->ToSendWebCommond("VS_SetPointPos", m_stLayerDebugParam.strWebCommondID, szBuffer);

	m_stLayerDebugParam.bGetPos = false;
}

void LayerDebugSceneLoader::addPointPos(bool bIsOver)
{
	// 判断是否结束取点
	if (bIsOver)
	{
		char szPos[LENGTH_1K] = { 0 };
		std::string strPos = "";
		for (int i = 0; i < m_vecPos.size(); i++)
		{
			sprintf_s(szPos, sizeof(szPos), "%f, %f, %f, %s", m_vecPos[i].x, m_vecPos[i].y, m_vecPos[i].z, ":");
			strPos += szPos;
		}

		strPos = strPos.substr(0, strPos.length() - 3);

		// 获取鼠标三维坐标，发送给网页
		char szBuffer[LENGTH_1K] = { 0 };
		sprintf_s(szBuffer, sizeof(szBuffer), "%d, %d, '%s'", m_stLayerDebugParam.nType, m_stLayerDebugParam.nPointID, strPos.c_str());
		m_pClient->ToSendWebCommond("VS_SetPointPos", m_stLayerDebugParam.strWebCommondID, szBuffer);

		m_stLayerDebugParam.bGetPos = false;
		m_stLayerDebugParam.bGetManyPos = false;
	}
	else
	{
		INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
		IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));
		Vector3 vPos = pManipulator->iGetMousePoint();
		vPos.z += 2.0f;
		m_vecPos.push_back(vPos);

		m_stLayerDebugParam.bGetManyPos = true;
	}

	// 画轨迹
	if (!m_stLayerDebugParam.pLocusLine)
	{
		LocusLineParam stParam;
		stParam.nNodeType = NODE_LOCUS_LINE;
		m_stLayerDebugParam.pLocusLine = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(stParam);
		m_stLayerDebugParam.pLocusLine->iLoadModel();
		m_stLayerDebugParam.pLocusLine->iSetSaveMyself(false);
		APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(m_stLayerDebugParam.pLocusLine);
	}
	else
	{
		vector<Vector3> vecPos = m_vecPos;
		if (bIsOver)
		{
			m_vecPos.clear();
		}

		vector<LocusLineSegment> vecData;
		LocusLineSegment stSegment;
		stSegment.vColor = Vector4(137 / 255.0f, 255 / 255.0f, 72 / 255.0f);
		stSegment.fWidth = 4.0f;
		stSegment.vecVertex = vecPos;
		vecData.push_back(stSegment);
		GetComponent<ILocusLine>(m_stLayerDebugParam.pLocusLine)->iUpdateLocusLineSegment(vecData);
		m_stLayerDebugParam.pLocusLine->iSetVisible(true);
	}
}

void LayerDebugSceneLoader::iOnCreateFactory(CommonLabelFactory* pFactory)
{
	pFactory->RemoveFunc(FUNC_READ_HRIGHT_TIF);
}

void LayerDebugSceneLoader::FrameUpdate()
{
	SceneElementLoader::FrameUpdate(); 
}

