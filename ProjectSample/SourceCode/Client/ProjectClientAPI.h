#pragma once

namespace bc
{
	class ProjectBasedClient;
	class ProjectClientAPI : public ProjectBasedClient
	{
	public:
		ProjectClientAPI();
		virtual	~ProjectClientAPI();

		// API
		virtual void				iInitialize(ISystemAPI* pSystemAPI);
		virtual void				iInitializeScene();
		virtual void				iSceneLoad();
		virtual void				iFrameUpdate();
		virtual EventReturnType_e	iExecuteEvent(const BCEvent& tEvent);
		virtual EventReturnType_e	iProcessEvent(const BCEvent& tEvent);
		virtual void				iPostFrameUpdate();
		virtual void				iPostSceneLoad();
		virtual std::string			iPreLoadWebPage(const std::string& strOriUrl, const std::string& strConcatUrl);
		virtual void				iOnHCLabelClickLabel(FactotryAssembleDetail* pAssemble, CommonLabelFactory* pFactory, bool bClick);

		// public
		static ProjectClientAPI*	GetProjectClientAPI();
		static void					Destroy();

	protected:
		//处理udp数据
		virtual void				iDoUdpData(int nCommond, char* pData);

		virtual void				iDoWebCommond(WebCommandData* pWebData);
		virtual void				iOnSceneElementLoadedData(bool bAllComplete);
		virtual SceneElementLoader* iCreateSceneElementLoader();

		virtual void				iRefreshData();

	protected:
		APIProvider*				m_pAPIProvider;
		static ProjectClientAPI*	m_pProjectClientAPI;

	private:
		INode*									m_pLocationBoard;
	};
}
