#pragma once


/*
样例
*/
namespace bc
{
	class SampleState : public HCBaseState
	{
	public:
		SampleState();
		virtual ~SampleState();

		virtual int					iGetType();
		virtual int					iGetSort();

		virtual void				iStateChangeUpdate(IStateManager* pStateManager);
		virtual void				iDataChangeUpdate(IStateManager* pStateManager);
		virtual void				iFrameUpdate(IStateManager* pStateManager);

		virtual void				iInitializeScene(IStateManager* pStateManager);
		virtual void				iSceneLoad(IStateManager* pStateManager);
		virtual EventReturnType_e	iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent);
		virtual EventReturnType_e	iExecuteEvent(IStateManager* pStateManager, const BCEvent& tEvent);

		virtual void				iRefreshData();

	protected:
		virtual void				iDoWebCommond(WebCommandData* pWebData);	//处理网页数据
		virtual void				iDoUdpData(int nCommand, char* pData);		//处理udp数据

	};
}
/////////////////
