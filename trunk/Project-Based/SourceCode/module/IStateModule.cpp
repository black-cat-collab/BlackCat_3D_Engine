#include "ProjectBasedHeader.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/module/IStateModule.h"

using namespace bc;

IStateModule::IStateModule(const std::string& strModuleName):
	m_pModuleGroupNode(nullptr)
{
	m_strModuleName = strModuleName;
	m_pClient = (ProjectBasedClient*)(APIProvider::GetSystemAPI()->iProjectClientAPI);
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

EventReturnType_e IStateModule::iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent)
{
	return EventReturnType_e::NONE;
}

EventReturnType_e IStateModule::iExecuteEvent(IStateManager* pStateManager, const BCEvent& tEvent)
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

INode* IStateModule::iGetModuleGroupNode()
{
	return m_pModuleGroupNode;
}

INode* IStateModule::createModuleGroupNode()
{
	if (!m_pModuleGroupNode)
	{
		NodeParamBase stParam;
		stParam.nNodeType = NODE_GROUP;
		stParam.strName = m_strModuleName;
		stParam.strID = m_strModuleName;
		m_pModuleGroupNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(stParam);
		m_pModuleGroupNode->iSetVisible(true);
		m_pModuleGroupNode->iLoadModel();
		m_pModuleGroupNode->iSetSaveMyself(false);
		iGetRootNode()->iAddChild(m_pModuleGroupNode);
	}
	return m_pModuleGroupNode;
}


