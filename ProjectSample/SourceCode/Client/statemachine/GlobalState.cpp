#include "UserHeader.h"
#include "./StateModule/SampleModule/SampleModule.h"

using namespace bc;

GlobalState::GlobalState() :
	HCBaseState()
{
	//m_vecStateModule.push_back(new SampleModule("SampleModule"));
}

GlobalState::~GlobalState()
{
	for (size_t i = 0; i < m_vecStateModule.size(); i++)
	{
		DELETE_PTR(m_vecStateModule[i]);
	}
	m_vecStateModule.clear();
}

int GlobalState::iGetType()
{
	return 0;
}

int GlobalState::iGetSort()
{
	return 0;
}

void GlobalState::iStateChangeUpdate(IStateManager* pStateManager)
{
	HCBaseState::iStateChangeUpdate(pStateManager);

	if (!m_bSceneLoaded)
	{
		return;
	}
}

void GlobalState::iDataChangeUpdate(IStateManager* pStateManager)
{
	HCBaseState::iDataChangeUpdate(pStateManager);

	if (!m_bSceneLoaded)
	{
		return;
	}

	for (size_t i = 0; i < m_vecStateModule.size(); i++)
	{
		m_vecStateModule[i]->iDataChangeUpdate(pStateManager);
	}
}

void GlobalState::iFrameUpdate(IStateManager* pStateManager)
{
	HCBaseState::iFrameUpdate(pStateManager);

	if (!m_bSceneLoaded)
	{
		return;
	}

	for (size_t i = 0; i < m_vecStateModule.size(); i++)
	{
		m_vecStateModule[i]->iFrameUpdate(pStateManager);
	}
}

void GlobalState::iInitializeScene(IStateManager* pStateManager)
{
	HCBaseState::iInitializeScene(pStateManager);

	for (size_t i = 0; i < m_vecStateModule.size(); i++)
	{
		m_vecStateModule[i]->iInitializeScene(pStateManager);
	}
}

void GlobalState::iSceneLoad(IStateManager* pStateManager)
{
	HCBaseState::iSceneLoad(pStateManager);

	for (size_t i = 0; i < m_vecStateModule.size(); i++)
	{
		m_vecStateModule[i]->iSceneLoad(pStateManager);
	}
}

EventReturnType_e GlobalState::iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent)
{
	HCBaseState::iProcessEvent(pStateManager,tEvent);

	if (!m_bSceneLoaded)
	{
		return EventReturnType_e::NONE;
	}

	for (size_t i = 0; i < m_vecStateModule.size(); i++)
	{
		m_vecStateModule[i]->iProcessEvent(pStateManager, tEvent);
	}

	return EventReturnType_e::NONE;
}

EventReturnType_e GlobalState::iExecuteEvent(IStateManager* pStateManager, const BCEvent& tEvent)
{
	HCBaseState::iExecuteEvent(pStateManager, tEvent);

	if (!m_bSceneLoaded)
	{
		return EventReturnType_e::NONE;
	}

	for (size_t i = 0; i < m_vecStateModule.size(); i++)
	{
		m_vecStateModule[i]->iExecuteEvent(pStateManager,tEvent);
	}

	return EventReturnType_e::NONE;
}

void GlobalState::iRefreshData()
{
	HCBaseState::iRefreshData();

	for (size_t i = 0; i < m_vecStateModule.size(); i++)
	{
		m_vecStateModule[i]->iRefreshData();
	}
}

void GlobalState::iDoWebCommond(WebCommandData* pWebData)
{
	HCBaseState::iDoWebCommond(pWebData);

	for (size_t i = 0; i < m_vecStateModule.size(); i++)
	{
		m_vecStateModule[i]->iDoWebCommond(pWebData);
	}
}

void GlobalState::iDoUdpData(int nCommand, char* pData)
{
	HCBaseState::iDoUdpData(nCommand,pData);

	for (size_t i = 0; i < m_vecStateModule.size(); i++)
	{
		m_vecStateModule[i]->iDoUdpData(nCommand, pData);
	}
}
