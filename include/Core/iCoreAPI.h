#pragma once

namespace bc
{
	class ICoreAPI
	{
	public:
		/*
		*	Core接口初始化
		*	pSystemAPI  [in]  系统接口
		*/
		virtual void			iInitialize(ISystemAPI* pSystemAPI) = 0;

		/*
		*	注册视图管理器
		*	pViewManager  [in] 
		*/
		virtual void			iRegistViewManager(IViewManager* pViewManager = nullptr) = 0;

		/*
		*	获取视图管理器
		*	return  [out]  
		*/
		virtual IViewManager*	iGetViewManager() = 0;
	};
}
