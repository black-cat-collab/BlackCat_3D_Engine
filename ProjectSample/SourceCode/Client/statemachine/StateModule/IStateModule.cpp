#include "UserHeader.h"
#include "../../ProjectClientAPI.h"
#include "IStateModule.h"

using namespace hc;

IStateModule::IStateModule(const std::string& strModuleName)
{
	m_strModuleName = strModuleName;
	m_pClient = static_cast<ProjectClientAPI*>(ProjectClientAPI::GetProjectClientAPI());
}

IStateModule::~IStateModule()
{
	
}

std::string IStateModule::iGetModuleName()
{
	return m_strModuleName;
}

INode* IStateModule::iGetRootNode()
{
	return APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode();
}

EventReturnType_e IStateModule::iProcessEvent(IStateManager* pStateManager, const HCEvent& tEvent)
{
	return EventReturnType_e::NONE;
}

EventReturnType_e IStateModule::iExecuteEvent(IStateManager* pStateManager, const HCEvent& tEvent)
{
	return EventReturnType_e::NONE;
}

void IStateModule::iRefreshData()
{
}

void IStateModule::iDoWebCommond(WebCommandData* pWebData)
{
}

void IStateModule::iDoUdpData(int nCommand, char* pData)
{
}


