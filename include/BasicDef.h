#pragma once

#include <stdio.h>

#define	LOGI(...)

// 数据长度1KB
#ifndef LENGTH_1K
#define LENGTH_1K               (1024)
#endif 

//当指针为空时返回
#ifndef RETURN_IF_PTR_IS_NULL
#define RETURN_IF_PTR_IS_NULL(ptr)	do { if(ptr == nullptr) { return; } } while(0)
#endif

//当指针为空时返回false
#ifndef RETURN_FALSE_IF_PTR_IS_NULL
#define RETURN_FALSE_IF_PTR_IS_NULL(ptr)	do { if(ptr == nullptr) { return false; } } while(0)
#endif 

// 当指针为空时返回空
#ifndef RETURN_NULL_IF_PTR_IS_NULL
#define RETURN_NULL_IF_PTR_IS_NULL(ptr)	do { if(ptr == nullptr) { return nullptr; } } while(0)
#endif

// 当变量不相等时返回false，否则不做操作
#ifndef RETURN_FALSE_IF_NOT_EQUAL
#define RETURN_FALSE_IF_NOT_EQUAL(b1, b2) do { if((b1) != (b2)) return false; } while(0)
#endif

// 当为false时返回false，否则不做操作
#ifndef RETURN_FALSE_IF_FALSE
#define RETURN_FALSE_IF_FALSE(b) do { if(!(b)) return false; } while(0)
#endif

// 删除指针并置为nullptr
#ifndef DELETE_PTR
#define DELETE_PTR(ptr) do { if(ptr) { delete ptr; ptr = nullptr; } } while(0)
#endif

// 删除数组
#ifndef DELETE_ARRAY
#define DELETE_ARRAY(ptr) do { if(ptr) { delete[]ptr; ptr = nullptr; } } while(0)
#endif

typedef unsigned short	ushort;
typedef unsigned int	uint;
typedef unsigned long	ulong;
typedef unsigned char	byte;

namespace bc
{
	typedef enum StatusCode_e
	{
		STATUS_SUCCESS = 1,
		STATUS_ERROR = 0,                                   ///< 未知错误

		STATUS_MSG_VERSION_UPDATE = 9099,      ///< 消息框：文件版本更新

		STATUS_BAD_PARAMS = 8001,                     ///< 参数错误

		STATUS_AUTH,

		STATUS_BAD_DATA,                            ///< 数据错误
		STATUS_NET_INIT,                            ///<  网络初始化错误
		STATUS_OPEN,                                ///< 文件打开错误
		STATUS_DIR_OPEN,                            ///< 目录错误
		STATUS_NOT_FOUND,                           ///< 没有发现
		STATUS_DOWNLOAD,                            ///< 下载失败
		STATUS_FOUND,


		STATUS_DLL_LOAD,                           ///< 加载动态库出错
		STATUS_OBJECT_CREATE,                  ///< 创建对象出错
		STATUS_GET_FUNCTION,                  ///< 获取函数地址出错
		STATUS_INTERFACE_REGIST,             ///< 注册接口失败
		STATUS_APP_INIT,                             ///< APP初始化失败	
		STATUS_HAS_EXIST,                                     ///< 已经存在   
		STATUS_VERSION,                       	   ///< 检查版本
		STATUS_CLASS_CREATE,                    ///< 创建class失败

		STATUS_LOGIN,                               ///< 登录失败
		STATUS_UPLOAD,                            ///< 上传失败
		STATUS_NEW,                              ///< 内存分配不成功
		STATUS_DELETE,
		STATUS_DATA_INVALID,                          ///< 数据无效
		STATUS_DATA_DELETED,                                  ///< 数据已被删除

		STATUS_NETWORK_DISCONNECT,                ///< 网络已断开
		STATUS_LOCAL_DENY,                                  ///< 本机拒绝
		STATUS_PARSE,                                 ///< 解析出错
		STATUS_ZIP_CREATE,                       ///< 创建Zip文件时出错 

		STATUS_USE_LOCAL,                               ///< 使用本地缓存信息
		STATUS_TOO_MUCH,                               ///< 数据太多
		STATUS_FUNC_NO_OPEN,                             ///< 功能没有打开 
		STATUS_TIME_OUT,                             ///< 超时
		STATUS_GAME_INVALID,                              ///< 游戏无效
		STATUS_GAME_OVER,                              ///< 游戏结束
		STATUS_DEL_FILE,                            ///< 删除文件出错

		STATUS_SQLLITE_OPEN,                     ///< SqlLite打开数据库错误
		STATUS_SQLLITE_EXEC,                     ///< SqlLite打开数据库错误
		STATUS_OPEN_THUMB,                      ///< 缩略图不存在
		STATUS_TASK_STOP,                                           ///< 

		STATUS_SOCKET_CREATE,                              ///< 创建socket出错
		STATUS_SOCKET_BIND,                              ///< socket绑定端口出错
		STATUS_SOCKET_RECV,                              ///< socket接收出错
		STATUS_SOCKET_SEND,                              ///< socket发送出错
		STATUS_SOCKET_LISTEN,
		STATUS_SOCKET_DISCONNECT,
		STATUS_SOCKET_CONNECT,
		STATUS_SOCKET_SETOPT,

		STATUS_PLAY,
		STATUS_REINIT,                                          ////< 重复初始化    
		STATUS_NOT_READY,                                       ///< 还没准备好 
		STATUS_LOAD,
		STATUS_SAVE,

		STATUS_RUN,

		STATUS_INIT,
		STATUS_RECORD,                                  //录音
		STATUS_CLOSE,                                   //关闭

		STATUS_REDIS_CONNECT,
		STATUS_REDIS_WRITE,
		STATUS_REDIS_READ,
		STATUS_REDIS_SELECT,
		STATUS_REDIS_CLEAR,

		STATUS_MYSQL_CONNECT,
		STATUS_MYSQL_EXECUTE,


		STATUS_OPENSSL_ENCRPT,
		STATUS_OPENSSL_DECRPT,

		STATUS_SUPPORT,

		STATUS_MAX_VALUE
	}StatusCode;

	static ushort DecToHex(const ushort& wDec)
	{
		return (wDec / 1000) * 4096 + ((wDec % 1000) / 100) * 256 + ((wDec % 100) / 10) * 16 + (wDec % 10);
	}

	static ushort HexToDec(const ushort& wHex)
	{
		return (wHex / 4096) * 1000 + ((wHex % 4096) / 256) * 100 + ((wHex % 256) / 16) * 10 + (wHex % 16);
	}
}
