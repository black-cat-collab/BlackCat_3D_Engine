#pragma once

namespace bc
{
	enum MachineState_e
	{
		MACHINE_STATE_NONE = 0,
		MACHINE_STATE_SWITCH_STATE,
		MACHINE_STATE_DATA_CHANGE
	};

	class IStateMachine;
	class IStateManager
	{
	public:
		virtual void				iAddStateMachine(IStateMachine* pStateMachine) = 0;
		virtual void				iRemoveStateMachine(IStateMachine* pStateMachine) = 0;
		virtual void				iClearStateMachine() = 0;
		virtual void				iAttachClientParam(void* pClientParam) = 0;
		virtual void*				iGetClientParam() = 0;

                virtual EventReturnType_e	iExecuteEvent(const BCEvent& tEvent) = 0;

		virtual void				iSetCurrentState(const int& nState) = 0;
		virtual int					iGetCurrentState() = 0;
		virtual int					iGetLastState() = 0;
		virtual void				iSetMachineState(const MachineState_e& eState) = 0;
	};
}
