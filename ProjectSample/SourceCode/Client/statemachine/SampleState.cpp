#include "UserHeader.h"
#include "SampleState.h"

using namespace bc;

SampleState::SampleState()
	:HCBaseState()
{

}

SampleState::~SampleState()
{

}

int SampleState::iGetType()
{
	return 0;
}

int SampleState::iGetSort()
{
	return 0;
}

void SampleState::iStateChangeUpdate(IStateManager* pStateManager)
{
	if (!m_bSceneLoaded)
	{
		return;
	}
}

void SampleState::iDataChangeUpdate(IStateManager* pStateManager)
{
	if (!m_bSceneLoaded)
	{
		return;
	}
}

void SampleState::iFrameUpdate(IStateManager* pStateManager)
{
	if (!m_bSceneLoaded)
	{
		return;
	}
}

void SampleState::iInitializeScene(IStateManager* pStateManager)
{
}

void SampleState::iSceneLoad(IStateManager* pStateManager)
{
	m_bSceneLoaded = true;
}

EventReturnType_e SampleState::iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent)
{
	HCBaseState::iProcessEvent(pStateManager,tEvent);

	if (!m_bSceneLoaded)
	{
		return EventReturnType_e::NONE;
	}

	return EventReturnType_e::NONE;
}

EventReturnType_e SampleState::iExecuteEvent(IStateManager* pStateManager, const BCEvent& tEvent)
{
	HCBaseState::iExecuteEvent(pStateManager, tEvent);

	if (!m_bSceneLoaded)
	{
		return EventReturnType_e::NONE;
	}

	return EventReturnType_e::NONE;
}

void SampleState::iRefreshData()
{
}

void SampleState::iDoWebCommond(WebCommandData *pWebData)
{
}

void SampleState::iDoUdpData(int nCommand, char* pData)
{
}
