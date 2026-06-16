#pragma once

namespace bc
{	
	class CommonLabelFactory;
	class IStateModule;
	class PROJECT_BASED_API BaseModule : public IStateModule
	{
	public:
		BaseModule(const std::string& strModuleName);
		virtual ~BaseModule();

	public:
		virtual void				iStateChangeUpdate(IStateManager* pStateManager) {};
		virtual void				iDataChangeUpdate(IStateManager* pStateManager) {};
		virtual void				iFrameUpdate(IStateManager* pStateManager) {};
		virtual void				iInitializeScene(IStateManager* pStateManager) {};

		virtual EventReturnType_e	iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent);
		virtual EventReturnType_e	iExecuteEvent(IStateManager* pStateManager, const BCEvent& tEvent);
		virtual void				iOnAppModeChanged(int nAppMode) {}		//app模式更改
		virtual void				iOnAppIdChanged(std::string  strAppId) {}		//appid更改
		virtual void				iOnRefreshLogin(bool bIsAccountChanged) {}	//刷新登录并且图层刷新后加载
		virtual void				iOnLayerLoaded() {}	//启动三维默认token登录，并且图层数据已加载完成
		virtual void				iOnHCLabelSelect(FactotryAssembleDetail* pAssemble, CommonLabelFactory *pFactory, bool bSelect) {}	//标签点击
		virtual void				iOnHCDetailSelect(FactotryAssembleDetail* pAssemble, CommonLabelFactory* pFactory, std::string strFlagName, bool bSelect) {}  //标签详情面板按钮点击
		virtual void				iOnSwitchToSceneMode(SceneMode_e eMode) {}	//GIS球和平面切换
		virtual void				iSceneLoad(IStateManager* pStateManager);
	protected:
		bool						m_bEditMode;		//编辑模式
	};
}


