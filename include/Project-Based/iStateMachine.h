#pragma once

namespace bc
{
	class IStateManager;
	class IStateMachine
	{
	public:
		virtual ~IStateMachine() {}
		virtual int					iGetType() = 0;
		virtual int					iGetSort() = 0;
		
		virtual void				iStateChangeUpdate(IStateManager* pStateManager) = 0;
		virtual void				iDataChangeUpdate(IStateManager* pStateManager) = 0;
		virtual void				iFrameUpdate(IStateManager* pStateManager) = 0;
		virtual void				iInitializeScene(IStateManager* pStateManager) = 0;
		virtual void				iSceneLoad(IStateManager* pStateManager) = 0;
                virtual EventReturnType_e	iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent) = 0;
                virtual EventReturnType_e	iExecuteEvent(IStateManager* pStateManager, const BCEvent& tEvent) = 0;
	};
}

