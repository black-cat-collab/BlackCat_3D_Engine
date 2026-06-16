#pragma once

#include <string>
#include "InterfaceDef.h"

#ifdef _MSC_VER

#ifdef BC_EXPORT
#define PROJECT_BASED_API __declspec(dllexport) 
#else
#define PROJECT_BASED_API __declspec(dllimport) 
#endif
#endif


#define IOCTL_PROJECTBASED_PARSE_COMMAND (-100000)		//特定数值，用来协定和Qt的交互，不要更改此值
#define IOCTL_PROJECTBASED_SUBSCRIBE_WEBSOCKET_MESSAGE (-100001)		//订阅websocket来消息

namespace bc
{
	class IStateManager;
	class IProjectBasedAPI
	{
	public:
		/*
		 *	初始化
		 *  pSystemAPI	[in] 系统接口指针
		 */
		virtual void				iInitialize(ISystemAPI* pSystemAPI) = 0;
		virtual void				iInitializeScene() = 0;
		virtual void				iSceneLoad() = 0;
		virtual void				iFrameUpdate() = 0;
                virtual EventReturnType_e	iProcessEvent(const BCEvent& tEvent) = 0;
		virtual void				iShutDown() = 0;
		virtual IStateManager*		iGetStateManager() = 0;

		virtual void				iSetExtraParam(const std::string& strKey, INT_PTR pValue) = 0;
		virtual INT_PTR				iGetExtraParam(const std::string& strKey) = 0;
	};
}
