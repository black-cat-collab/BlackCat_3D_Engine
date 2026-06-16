#pragma once

#ifdef __GNUC__
#define MAX_PATH          PATH_MAX
#endif

#define DEFAULT_REMOTERENDER_PORT        (8090)
namespace bc
{
	//远程渲染websocket主机信息
	typedef struct _CloudServer_t
	{
		int        nID;
		int        nUseConfig;          //1: user config file
		int        nVerify;             //1: verify by szUser/szPass,or use server's info to verify ,2: verify from server
		int        nEncodeType;         //default:1,soft encode,2:hard encode
		int        nCommonMode;         //communication:1:webrtc,2:native,3:websocket
		char       szIP[40];
		char       szAppName[40];
		char       szClient[40];

		char       szProcess[MAX_PATH]; //process name
		int        nWebPort;

		char       szUser[60];
		char       szPass[80];
		short      nOriginWidth;
		short      nOriginHeight;


		_CloudServer_t()
		{
			nID = 0;
			memset(szIP, 0, sizeof(szIP));
			memset(szAppName, 0, sizeof(szAppName));
			memset(szProcess, 0, sizeof(szProcess));

			memset(szUser, 0, sizeof(szUser));
			memset(szPass, 0, sizeof(szPass));

			memset(szClient, 0, sizeof(szClient));
 

			nCommonMode = 1;
			nUseConfig = 0;
			nVerify = 0;
			nWebPort = 0;
		 
			nOriginWidth = 0;
			nOriginHeight = 0;
			nEncodeType = 1;

		}


		struct _CloudServer_t& operator = (const struct _CloudServer_t& src)
		{
			nID = src.nID;
			nWebPort = src.nWebPort;
			 

			nOriginWidth = src.nOriginWidth;
			nOriginHeight = src.nOriginHeight;

			nUseConfig = src.nUseConfig;
			nVerify = src.nVerify;
			nEncodeType = src.nEncodeType;
			nCommonMode = src.nCommonMode;

			memcpy(szIP, src.szIP, sizeof(szIP));
			memcpy(szClient, src.szClient, sizeof(szClient));

			memcpy(szAppName, src.szAppName, sizeof(szAppName));
			memcpy(szProcess, src.szProcess, sizeof(szProcess));

			memcpy(szUser, src.szUser, sizeof(szUser));
			memcpy(szPass, src.szPass, sizeof(szPass));
 

			return *this;
		}

	}ICloudServer;

	class ICloudRenderManager
	{
	public:
		
		/*
		* 获取云渲染host的状态
		*/
		virtual int		iGetRenderHost() = 0;

		/*
		* 获取clound参数
		*/
		virtual ICloudServer* iGetCloudParam() = 0;
	};
}
