#pragma once

namespace bc
{
	class StateData
	{
	public:
		StateData();
		virtual ~StateData();
		virtual int		GetCurrentState();
		virtual int		GetLastState();
		virtual void	SetCurrentState(const int& nState);
		virtual void	SetLastState(const int& nState);
		virtual void	SetMachineState(const MachineState_e& eState);
		virtual bool	TestMachineState(const MachineState_e& eState);
		virtual void	ResetMachineState();
	protected:
		int				m_nCurrentState;
		int				m_nLastState;
	private:
		int				m_eMachineState;
	};

}

