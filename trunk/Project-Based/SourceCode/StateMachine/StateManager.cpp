#include "ProjectBasedHeader.h"
#include "StateManager.h"

using namespace bc;

StateManager* StateManager::m_pThis = nullptr;

bool StateMachineCompFunc(IStateMachine* pMachine1, IStateMachine* pMachine2)
{
	return (pMachine1->iGetSort() < pMachine2->iGetSort());
}

StateManager::StateManager() :
	m_pStateData(new StateData),
	m_pClientParam(nullptr)
{
	
}

StateManager::~StateManager()
{
	for (size_t i = 0; i < m_vStateMachine.size(); ++i)
	{
		DELETE_PTR(m_vStateMachine[i]);
	}
	m_vStateMachine.clear();

	DELETE_PTR(m_pStateData);
}

StateManager* StateManager::GetInstance()
{
	if (m_pThis == nullptr)
	{
		m_pThis = new StateManager;
	}

	return m_pThis;
}

void StateManager::Destroy()
{
	DELETE_PTR(m_pThis);
}

void StateManager::iAddStateMachine(IStateMachine* pStateMachine)
{
	if (std::find(m_vStateMachine.begin(), m_vStateMachine.end(), pStateMachine) != m_vStateMachine.end())
	{
		return;
	}

	m_vStateMachine.push_back(pStateMachine);

	std::stable_sort(m_vStateMachine.begin(), m_vStateMachine.end(), StateMachineCompFunc);
}

void StateManager::iRemoveStateMachine(IStateMachine* pStateMachine)
{
	std::vector<IStateMachine*>::iterator it = std::find(m_vStateMachine.begin(), m_vStateMachine.end(), pStateMachine);
	if (it != m_vStateMachine.end())
	{
		IStateMachine* pStateMachine = (*it);
		DELETE_PTR(pStateMachine);
		m_vStateMachine.erase(it);
	}
}

void StateManager::iClearStateMachine()
{
	for (size_t i = 0; i < m_vStateMachine.size(); ++i)
	{
		DELETE_PTR(m_vStateMachine[i]);
	}
	m_vStateMachine.clear();
	m_pStateData->ResetMachineState();
}

void StateManager::iSetCurrentState(const int& nState)
{
	m_pStateData->SetCurrentState(nState);
	m_pStateData->SetMachineState(MACHINE_STATE_SWITCH_STATE);
}

int StateManager::iGetCurrentState()
{
	return m_pStateData->GetCurrentState();
}

int	StateManager::iGetLastState()
{
	return m_pStateData->GetLastState();
}

void StateManager::iSetMachineState(const MachineState_e& eState)
{
	m_pStateData->SetMachineState(eState);
}

void StateManager::InitializeScene()
{
	for (size_t i = 0; i < m_vStateMachine.size(); ++i)
	{
		m_vStateMachine[i]->iInitializeScene(this);
	}
}

void StateManager::Clear()
{
	iClearStateMachine();
}

void StateManager::FrameUpdate()
{
	for (size_t i = 0; i < m_vStateMachine.size(); ++i)
	{
		// 状态切换
		if (m_pStateData->TestMachineState(MACHINE_STATE_SWITCH_STATE))
		{
			m_vStateMachine[i]->iStateChangeUpdate(this);
		}
		// 数据变化
		if (m_pStateData->TestMachineState(MACHINE_STATE_DATA_CHANGE))
		{
			m_vStateMachine[i]->iDataChangeUpdate(this);
		}
		// 无任何变化
		//if (m_pStateData->TestMachineState(MACHINE_STATE_NONE))
		{
			m_vStateMachine[i]->iFrameUpdate(this);
		}
	}
	m_pStateData->SetLastState(m_pStateData->GetCurrentState());
	m_pStateData->ResetMachineState();
}

void StateManager::SceneLoad()
{
	for (size_t i = 0; i < m_vStateMachine.size(); ++i)
	{
		m_vStateMachine[i]->iSceneLoad(this);
	}
}

EventReturnType_e StateManager::ProcessEvent(const BCEvent& tEvent)
{
	for (size_t i = 0; i < m_vStateMachine.size(); ++i)
	{
		if (m_vStateMachine[i]->iProcessEvent(this, tEvent) == EventReturnType_e::BREAK)
		{
			return EventReturnType_e::BREAK;
		}
	}

	return EventReturnType_e::NONE;
}

EventReturnType_e StateManager::iExecuteEvent(const BCEvent& tEvent)
{
	for (size_t i = 0; i < m_vStateMachine.size(); ++i)
	{
		if (m_vStateMachine[i]->iExecuteEvent(this, tEvent) == EventReturnType_e::BREAK)
		{
			return EventReturnType_e::BREAK;
		}
	}

	return EventReturnType_e::NONE;
}

