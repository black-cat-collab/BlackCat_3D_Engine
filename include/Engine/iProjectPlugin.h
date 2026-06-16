#pragma once

namespace bc
{
	// Qt和VS的交互,strCommond 命令，pInData输入参数，pOutData输出参数
	typedef void(*QtVsConnectFunc)(const std::string& strCommond, std::map<std::string, INT_PTR> mapExtraParam, void* pInData, void* pOutData);

	class IProjectPluginAPI
	{
	public:
		virtual void				iInitialize(ISystemAPI* pSystemAPI) = 0;
		virtual void				iInitializeScene() = 0;
		virtual void				iSceneLoad() = 0;
		virtual void				iFrameUpdate() = 0;
		/*
		* 场景每帧全部渲染后
		*/
		virtual void				iPostFrameUpdate() = 0;
		/*
		* 场景全部加载完成
		*/
		virtual void				iPostSceneLoad() = 0;
                virtual EventReturnType_e	iExecuteEvent(const BCEvent& tEvent) = 0;
                virtual EventReturnType_e	iProcessEvent(const BCEvent& tEvent) = 0;
		/*
		 *	 加载网页前，可对地址进行修改，返回修改地址
		 *	strOriUrl		[in]	最原始的url（appconfig.tw4里的web_url）
		 *	strConcatUrl	[in]	进行拼接了的的url
		 *	return	[out]	处理的url
		 */
		virtual std::string			iPreLoadWebPage(const std::string& strOriUrl, const std::string& strConcatUrl) { return strConcatUrl; }
		/*
		* 添加Qt和VS交互的方法
		*/
		virtual void				iAddQtVsConnectFunc(QtVsConnectFunc fn) {};
		/*
		* 移除Qt和VS交互的方法
		*/
		virtual void				iRemoveQtVsConnectFunc(QtVsConnectFunc fn) {};
		/*
		* 执行Qt和VS交互的方法
		*/
		virtual void				iExecuteQtVsCommond(const std::string& strCommond, void* pInData, void* pOutData) {};
		/*
		* 通过屏幕xy判断是否可以操作三维
		*/
		virtual bool				iCanHandleScene(const Vector2& vScreenPos) { return true; }
		/*
		* 判断此时三维是否可响应键盘事件
		*/
		virtual bool				iCanRespondKey() { return true; }
	};
}
