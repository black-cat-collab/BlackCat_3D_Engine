#include "ProjectBasedHeader.h"

#include "Utility/LinuxOS/LinuxOS.h"
#include "Utility/WatchTimer/WatchTimer.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/module/IStateModule.h"
#include "Project-Based/module/BaseModule.h"
#include "Project-Based/module/LayerEditModule.h"

using namespace bc;

LayerEditModule::LayerEditModule(const std::string& strModuleName)
	: BaseModule(strModuleName)
{

}

LayerEditModule::~LayerEditModule()
{

}

void LayerEditModule::iFrameUpdate(IStateManager* pStateManager)
{
	BaseModule::iFrameUpdate(pStateManager);
	m_UdpSubscribeMutexLock.Lock();
	while (!m_queueUdpSubscribe.empty())
	{
		UdpScribeInfo* pInfo = m_queueUdpSubscribe.front();
		if (pInfo->nType != -1)
		{
			m_pClient->m_pSceneElementLoader->ClearResidentElement(pInfo->nType);
			for (auto& strData : pInfo->vecData)
			{
				std::string* pStrData = new std::string(strData);
				m_pClient->m_pSceneElementLoader->ParseSubscribeData(pStrData);
			}
		}
		m_queueUdpSubscribe.pop();

		DELETE_PTR(pInfo);
	}
	m_UdpSubscribeMutexLock.UnLock();
}

void LayerEditModule::iDoWebCommond(WebCommandData* pWebData)
{

}

void LayerEditModule::iOnAppModeChanged(int nAppMode)
{
	APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iSetVisible(m_pClient->m_nAppMode & APP_MODE_LAYER_EDIT);
}

void LayerEditModule::iDoUdpData(int nCommand, char* pData)
{
	if (nCommand == 10000)
	{
		//udp图层更改数据
		Json::Value jRoot;
		std::string str = pData;
		ParseJsonByString(str, jRoot);
		try
		{
			if (jRoot.isObject())
			{
				UdpScribeInfo* pInfo = new UdpScribeInfo;
				pInfo->nType = jRoot["type"].asInt();
				Json::Value& jArray = jRoot["data"];
				for (int i = 0; i < jArray.size(); ++i)
				{
					std::string strData = jArray[i].toStyledString();
					pInfo->vecData.push_back(strData);
				}
				m_UdpSubscribeMutexLock.Lock();
				m_queueUdpSubscribe.push(pInfo);
				m_UdpSubscribeMutexLock.UnLock();
			}
		}
		catch (const std::exception&)
		{

		}
	}
}
