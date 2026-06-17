#pragma once

namespace bc
{
	class StateManager : public IStateManager
	{
	public:
		StateManager();
		virtual ~StateManager();
		virtual void				iAddStateMachine(IStateMachine* pStateMachine);
		virtual void				iRemoveStateMachine(IStateMachine* pStateMachine);
		virtual void				iClearStateMachine();
		virtual void				iAttachClientParam(void* pClientParam) { m_pClientParam = pClientParam; }
		virtual void*				iGetClientParam() { return m_pClientParam; }
		virtual void				iSetCurrentState(const int& nState);
		virtual int					iGetCurrentState();
		virtual int					iGetLastState();
		virtual void				iSetMachineState(const MachineState_e& eState);
		virtual EventReturnType_e	iExecuteEvent(const BCEvent& tEvent);

		virtual void				InitializeScene();
		virtual void				Clear();
		virtual void				SceneLoad();
		virtual void				FrameUpdate();
		virtual EventReturnType_e	ProcessEvent(const BCEvent& tEvent);

		static StateManager*		GetInstance();
		static void					Destroy();
	protected:
		static StateManager*		m_pThis;
		StateData*					m_pStateData;
		void*						m_pClientParam;
		std::vector<IStateMachine*>	m_vStateMachine;
	};
}

