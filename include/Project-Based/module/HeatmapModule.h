#pragma once

namespace bc
{
	class HeatmapItemData;
	class PROJECT_BASED_API HeatmapModule : public BaseModule
	{
	public:
		HeatmapModule(const std::string& strModuleName);
		virtual ~HeatmapModule();

	public:
		virtual void				iFrameUpdate(IStateManager* pStateManager);
		virtual void				iSceneLoad(IStateManager* pStateManager);

		virtual void				iRefreshData();
		virtual void				iDoWebCommond(WebCommandData* pWebData);	//处理网页数据
		virtual EventReturnType_e	iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent);
		virtual void				iOnRefreshLogin(bool bIsAccountChanged);
		virtual void				iOnLayerLoaded();

	protected:
		virtual void iSetHeatmapItem(HeatmapItemData& pItemData) {}

		void requestLbsList();

	private:
		static void static_request_lbslist(ThreadCallbackParam* pParam);

	private:
		bool  m_bSceneLoaderComplete;
	};
}


