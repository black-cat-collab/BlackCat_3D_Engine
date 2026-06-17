#include "ProjectBasedHeader.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/module/IStateModule.h"
#include "Project-Based/module/BaseModule.h"
#include "Project-Based/module/HeatmapModule.h"
#include "Project-Based/base/HeatmapFactory.h"

using namespace bc;

HeatmapModule::HeatmapModule(const std::string& strModuleName)
	:BaseModule(strModuleName),
	m_bSceneLoaderComplete(false)
{

}

HeatmapModule::~HeatmapModule()
{
	
}

void HeatmapModule::iFrameUpdate(IStateManager* pStateManager)
{
	
}

void HeatmapModule::iSceneLoad(IStateManager* pStateManager)
{
	BaseModule::iSceneLoad(pStateManager);
}

void HeatmapModule::iRefreshData()
{

}

void HeatmapModule::iDoWebCommond(WebCommandData* pWebData)
{
	try
	{
		if (pWebData->strCommand == "Web_SelectHeatmap")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strID = pWebData->vecValues[0].asString();
				HeatmapItemData* pItem = m_pClient->m_pHeatMapFactory->GetHeatmapItem(strID);
				if (pItem)
				{

				}
			}
		}
	}
	catch (const std::exception&)
	{

	}
}

EventReturnType_e HeatmapModule::iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent)
{
	BaseModule::iProcessEvent(pStateManager,tEvent);
	//if (tEvent.eMessage == PROJECTBASED_EVENT_SCENE_ELEMENT_DATA_REFRESHED)
	//{
	//	requestLbsList();
	//}

	return EventReturnType_e::NONE;
}

void HeatmapModule::iOnRefreshLogin(bool bIsAccountChanged)
{
	requestLbsList();
}

void HeatmapModule::iOnLayerLoaded()
{

}

void HeatmapModule::requestLbsList()
{
	char szBuffer[1024] = { 0 };
	sprintf_s(szBuffer, sizeof(szBuffer), "%s/api/v10/lbsList", m_pClient->m_strApiHost.c_str());
	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_API_GET;
	stParam.pThreadCompleteFunc = static_request_lbslist;
	stParam.strID = "requestLbsList";
	stParam.strUrl = szBuffer;
	stParam.bRepeat = false;
	stParam.bRunCallbackInMainThread = true;
	stParam.AddParam("this", (INT_PTR)this);
	m_pClient->AddHttpCommonParam(&stParam);
	ThreadWrapper::GetInstance()->AddTask(stParam);
}

void HeatmapModule::static_request_lbslist(ThreadCallbackParam* pParam)
{
	HeatmapModule* pThis = (HeatmapModule*)pParam->sRequestParam.mapParams["this"];
	if (pParam->nResult == STATUS_SUCCESS)
	{
		try
		{
			pThis->m_pClient->m_pHeatMapFactory->RemoveAllHeatmap();

			Json::Value& jArray = pParam->jRoot["data"];
			Json::Value jWebList = Json::arrayValue;
			for (int i = 0; i < jArray.size(); ++i)
			{
				bool bShow = (jArray[i]["is_scene_show"].asInt() == 1) ? true : false;
				if (!bShow)
				{
					continue;
				}
				HeatmapItemData stItem;
				stItem.strKey = jArray[i]["area_id"].asString();
				stItem.nClusterCount = jArray[i]["cluster_count"].asInt();
				stItem.nClusterLevel = jArray[i]["cluster_level"].asInt();
				stItem.bCluster = (jArray[i]["is_cluster"].asInt() == 1)?true:false;
				stItem.strName = jArray[i]["name"].asString();
				char sz[1024] = { 0 };
				//热力图
				sprintf_s(sz, sizeof(sz), "%s/api/v10/lbsHeatmap?areaId=%s&typeId=4",
					pThis->m_pClient->m_strApiHost.c_str(),stItem.strKey.c_str());
				stItem.strHeatmapUrl = sz;
				//边界
				sprintf_s(sz, sizeof(sz), "%s/api/v10/lbsHeatmap?areaId=%s&typeId=1",
					pThis->m_pClient->m_strApiHost.c_str(), stItem.strKey.c_str());
				stItem.strBoundaryUrl = sz;
				pThis->iSetHeatmapItem(stItem);
				pThis->m_pClient->m_pHeatMapFactory->AddHeatmap(stItem);

				Json::Value jParam = {};
				jParam["id"] = stItem.strKey;
				jParam["name"] = stItem.strName;
				jParam["viewport"] = stItem.strViewport;
				jWebList.append(jParam);
			}
			pThis->m_pClient->m_pHeatMapFactory->StartRequest();
			//先隐藏
			std::vector<std::string> vec;
			pThis->m_pClient->m_pHeatMapFactory->ShowOnlyAreas(vec);

			//发网页
			std::string strData = jWebList.toStyledString();
			pThis->m_pClient->ToSendWebCommond("VS_ToWebHeatmapList","", strData);
		}
		catch (const std::exception&)
		{

		}
	}
}

