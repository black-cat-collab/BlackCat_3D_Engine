#include "UserHeader.h"
#include "HCBaseState.h"

using namespace bc;

HCBaseState::HCBaseState()
{
	m_bSceneLoaded = false;
	m_pClient = ProjectClientAPI::GetProjectClientAPI();
}

HCBaseState::~HCBaseState()
{

}

int HCBaseState::iGetType()
{
	return 0;
}

int HCBaseState::iGetSort()
{
	return 0;
}

void HCBaseState::iStateChangeUpdate(IStateManager* pStateManager)
{
	if (!m_bSceneLoaded)
	{
		return;
	}
}

void HCBaseState::iDataChangeUpdate(IStateManager* pStateManager)
{
	if (!m_bSceneLoaded)
	{
		return;
	}
}

void HCBaseState::iFrameUpdate(IStateManager* pStateManager)
{
	if (!m_bSceneLoaded)
	{
		return;
	}
}

void HCBaseState::iInitializeScene(IStateManager* pStateManager)
{
}

void HCBaseState::iSceneLoad(IStateManager* pStateManager)
{
	m_bSceneLoaded = true;
}

EventReturnType_e HCBaseState::iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent)
{
	if (!m_bSceneLoaded)
	{
		return EventReturnType_e::NONE;
	}

	return EventReturnType_e::NONE;
}

EventReturnType_e HCBaseState::iExecuteEvent(IStateManager* pStateManager, const BCEvent& tEvent)
{
	if (!m_bSceneLoaded)
	{
		return EventReturnType_e::NONE;
	}

	if (tEvent.eMessage == PROJECTBASED_EVENT_PROJECT_WEB_COMMOND)
	{
		WebCommandData *pWebData = (WebCommandData*)tEvent.nParam1;
		iDoWebCommond(pWebData);
		if (pWebData->strCommand == "Web_Refesh3D")
		{
			iRefreshData();
		}
	}

	return EventReturnType_e::NONE;
}

void HCBaseState::iRefreshData()
{
}

void HCBaseState::iDoWebCommond(WebCommandData *pWebData)
{
}

void HCBaseState::iDoUdpData(int nCommand, char* pData)
{
}
