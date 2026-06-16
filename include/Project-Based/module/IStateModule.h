#pragma once

namespace bc
{	
	class ProjectBasedClient;
	class PROJECT_BASED_API IStateModule
	{
	public:
		IStateModule(const std::string& strModuleName);
		virtual ~IStateModule();

	public:
		virtual std::string			iGetModuleName();
		virtual INode*				iGetRootNode();

		virtual void				iStateChangeUpdate(IStateManager* pStateManager) = 0;
		virtual void				iDataChangeUpdate(IStateManager* pStateManager) = 0;
		virtual void				iFrameUpdate(IStateManager* pStateManager) = 0;
		virtual void				iInitializeScene(IStateManager* pStateManager) = 0;
		virtual void				iSceneLoad(IStateManager* pStateManager) = 0;
		virtual EventReturnType_e	iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent);
		virtual EventReturnType_e	iExecuteEvent(IStateManager* pStateManager, const BCEvent& tEvent);

		virtual void				iRefreshData();
		virtual void				iDoWebCommond(WebCommandData* pWebData);	//处理网页数据
		virtual void				iDoUdpData(int nCommand, char* pData);		//处理udp数据
		
		virtual INode*				iGetModuleGroupNode();

	protected:
		virtual INode*				createModuleGroupNode();

	protected:
		ProjectBasedClient*			m_pClient;
		std::string					m_strModuleName;
		INode*						m_pModuleGroupNode;
	};
}


