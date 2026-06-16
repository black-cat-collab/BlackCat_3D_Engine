#include "UserHeader.h"
#include "SampleModule.h"

using namespace bc;

SampleModule::SampleModule(const std::string& strModuleName)
	:IStateModule(strModuleName)
{
	m_strModuleName = strModuleName;
	m_pClient = static_cast<ProjectClientAPI*>(ProjectClientAPI::GetProjectClientAPI());
}

SampleModule::~SampleModule()
{
	
}

std::string SampleModule::iGetModuleName()
{
	return m_strModuleName;
}

INode* SampleModule::iGetRootNode()
{
	return APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode();
}

void SampleModule::iStateChangeUpdate(IStateManager* pStateManager)
{
}

void SampleModule::iDataChangeUpdate(IStateManager* pStateManager)
{
}

void SampleModule::iFrameUpdate(IStateManager* pStateManager)
{
}

void SampleModule::iInitializeScene(IStateManager* pStateManager)
{
}

void SampleModule::iSceneLoad(IStateManager* pStateManager)
{
}

EventReturnType_e SampleModule::iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent)
{
	return EventReturnType_e::NONE;
}

EventReturnType_e SampleModule::iExecuteEvent(IStateManager* pStateManager, const BCEvent& tEvent)
{
	return EventReturnType_e::NONE;
}

void SampleModule::iRefreshData()
{
}

void SampleModule::iDoWebCommond(WebCommandData* pWebData)
{
}


