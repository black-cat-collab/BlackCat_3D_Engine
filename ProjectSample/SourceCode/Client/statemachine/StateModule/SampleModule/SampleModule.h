#pragma once

namespace bc
{	
	class SampleModule : public IStateModule
	{
	public:
		SampleModule(const std::string& strModuleName);
		virtual ~SampleModule();

	public:
		virtual std::string			iGetModuleName();
		virtual INode*				iGetRootNode();

		virtual void				iStateChangeUpdate(IStateManager* pStateManager);
		virtual void				iDataChangeUpdate(IStateManager* pStateManager);
		virtual void				iFrameUpdate(IStateManager* pStateManager);
		virtual void				iInitializeScene(IStateManager* pStateManager);
		virtual void				iSceneLoad(IStateManager* pStateManager);
		virtual EventReturnType_e	iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent);
		virtual EventReturnType_e	iExecuteEvent(IStateManager* pStateManager, const BCEvent& tEvent);

		virtual void				iRefreshData();
		virtual void				iDoWebCommond(WebCommandData* pWebData);	//处理网页数据
		
	protected:

	};
}

