#include "ProjectBasedHeader.h"
#include "StateData.h"

using namespace bc;

StateData::StateData():
	m_nCurrentState(-1),
	m_nLastState(-1),
	m_eMachineState(MACHINE_STATE_NONE)
{

}

StateData::~StateData()
{

}

int StateData::GetCurrentState()
{
	return m_nCurrentState;
}

int StateData::GetLastState()
{
	return m_nLastState;
}

void StateData::SetCurrentState(const int& nState)
{
	m_nCurrentState = nState;
}

void StateData::SetLastState(const int& nState)
{
	m_nLastState = nState;
}

void StateData::SetMachineState(const MachineState_e& eState)
{
	m_eMachineState = m_eMachineState | eState;
}

bool StateData::TestMachineState(const MachineState_e& eState)
{
	if (eState == MACHINE_STATE_NONE)
	{
		return m_eMachineState == MACHINE_STATE_NONE;
	}
	return (m_eMachineState & eState) != 0;
}

void StateData::ResetMachineState()
{
	m_eMachineState = MACHINE_STATE_NONE;
}

