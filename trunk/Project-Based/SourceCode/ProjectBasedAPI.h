#pragma once

namespace bc
{
	class IsolatedTool;
	class PROJECT_BASED_API ProjectBasedAPI : public IProjectBasedAPI
	{
	public:
		ProjectBasedAPI();
		virtual	~ProjectBasedAPI();

		// API
		virtual void				iInitialize(ISystemAPI* pSystemAPI);
		virtual void				iInitializeScene();
		virtual void				iSceneLoad();
		virtual void				iFrameUpdate();
		virtual EventReturnType_e	iProcessEvent(const BCEvent& tEvent);
		virtual IStateManager*		iGetStateManager();
		virtual void				iShutDown();
		virtual void				iSetExtraParam(const std::string& strKey, INT_PTR pValue);
		virtual INT_PTR				iGetExtraParam(const std::string& strKey);

		// public
		static ProjectBasedAPI*	GetProjectBasedAPI();
		static void				Destroy();
	protected:
		static ProjectBasedAPI*	m_pProjectBasedAPI;
		APIProvider*			m_pAPIProvider;

		std::map<std::string, INT_PTR> m_mapExtraParam;
	};
}

