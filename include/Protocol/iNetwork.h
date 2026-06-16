#pragma once

#include <string>
#include <vector>

#define DEFAULT_SERVER_DATA_PORT		(9897)      //数据服务器端口
#define DEFAULT_CLIENT_DATA_PORT		(9896)      //数据客户端端口

#define DEFAULT_SERVER_HEART_BEAT_PORT	(9899)      //心跳服务器端口
#define DEFAULT_CLIENT_HEART_BEAT_PORT	(9898)      //心跳客户端端口

namespace bc
{
	typedef void(*SocketReceiveFunc)(INT_PTR wParam, INT_PTR lParam);
	typedef void(*NetworkProgressFunc)(INT_PTR wParam, INT_PTR lParam);
	typedef std::vector<std::pair<std::string, Json::Value>> HttpPostParam;
	typedef std::vector<std::pair<std::string, Json::Value>> HttpCustomHeaderParam;

	enum SocketConnectorType_e
	{
		SOCKET_CONNECTOR_TYPE_UDP,
		SOCKET_CONNECTOR_TYPE_TCP
	};

	enum HttpHeaderType_e
	{
		HTTP_HEADER_TYPE_NONE,
		HTTP_HEADER_TYPE_JSON,
		HTTP_HEADER_TYPE_EXPECT,
	};

	enum PacketType_e
	{
		PACKET_TYPE_SERVICE_CHECK = 1,              //查找服务位置,广播包
		PACKET_TYPE_SERVICE_FIND,                   //查找服务位置,广播包
		PACKET_TYPE_SERVICE_FIND_ACK,               //查找服务位置反馈

		PACKET_TYPE_CLIENT_HEARTBEAT,               //服务心跳
		PACKET_TYPE_CLIENT_HEARTBEAT_ACK,           //服务心跳反馈 

		PACKET_TYPE_CLIENT_COMMAND,                 //客户端指令 
		PACKET_TYPE_CLIENT_COMMAND_ACK,             //指令反馈 

		PACKET_TYPE_CLIENT_REPORT,                  //信息上报

		PACKET_TYPE_CAMERA_REPORT,                  //摄像头信息上报

		PACKET_TYPE_CLIENT_LOGOUT,                  //客户端登出

		PACKET_TYPE_MOBILE_ONLINE,                  //移动终端上线
		PACKET_TYPE_MOBILE_OFFLINE,                 //移动终端下线

		PACKET_TYPE_AUDIO_DATA,                     //音频数据
		PACKET_TYPE_VIDEO_DATA,                     //视频数据

		PACKET_TYPE_TALK_APPLY,                     //申请通话
		PACKET_TYPE_TALK_APPLY_ACK,                 //应答


		PACKET_TYPE_TALK_KICKOFF,                   //关闭通话

		PACKET_TYPE_REMOTE_CONTROL,                 //远程控制

		PACKET_TYPE_SERVICE_MAX
	};

	typedef struct NetworkAdapter_s
	{
		std::string					strName;                      ///< 网卡名称
		std::string					strDescription;               ///< 网卡描述
		std::string					strMac;                       ///< mac地址
		std::string					strIP;
		std::string					strMask;

		NetworkAdapter_s()
		{

		}
	}NetworkAdapter;

	typedef struct SocketConnectorParam_s
	{
		std::string					strID;
		SocketConnectorType_e		eType;
		int							nDataServerPort;
		int							nDataClientPort;
		std::string					strHeartBeatFindIP;
		int							nHeartBeatServerPort;
		int							nHeartBeatClientPort;
		SocketReceiveFunc			pReceiveCallback;
		void*						pReceiveCallbackUser;
		SocketReceiveFunc			pFindServerCallBack;
		void*						pFindServerCallBackUser;
		std::vector<NetworkAdapter>	vecAdapter;

		SocketConnectorParam_s() :
			pReceiveCallback(nullptr),
			pReceiveCallbackUser(nullptr),
			pFindServerCallBack(nullptr),
			pFindServerCallBackUser(nullptr),
			eType(SOCKET_CONNECTOR_TYPE_UDP),
			nDataServerPort(DEFAULT_SERVER_DATA_PORT),
			nDataClientPort(DEFAULT_CLIENT_DATA_PORT),
			nHeartBeatServerPort(DEFAULT_SERVER_HEART_BEAT_PORT),
			nHeartBeatClientPort(DEFAULT_CLIENT_HEART_BEAT_PORT)
		{

		}
	}SocketConnectorParam;

	typedef struct UserMessage_s
	{
		int								nMessage;      //消息号
		int								nTimeout;      //同步等待时间
		bool							bSync;         //是否是同步数据
		std::map<std::string, INT_PTR>	mapParam;

		UserMessage_s() :
			bSync(true),
			nMessage(0),
			nTimeout(60 * 1000)
		{

		}
	}UserMessage;

	typedef struct NetworkTask_s
	{
		int				nID;
		UserMessage		sMsg;
		int				nPageNumber;
		int				nPageSize;
		int				nPageCount;
		int				nDataSize;
		char*			pData;

		NetworkTask_s() :
			nID(0),
			nPageNumber(1),
			nPageCount(0),
			nPageSize(200),
			nDataSize(0),
			pData(nullptr)
		{

		}

		~NetworkTask_s()
		{
			DELETE_ARRAY(pData);
		}

		int SetParam(char* pParam, const int& nLength)
		{
			int nSize = 0;
			if ((!pParam) || (nLength < 1))
			{
				return STATUS_BAD_PARAMS;
			}
			if (nLength > nDataSize)
			{
				DELETE_ARRAY(pData);
				nSize = nLength + nLength % 4;
				nDataSize = nLength;
				pData = new char[nSize];
			}
			else
			{
				nSize = nDataSize;
			}
			memset(pData, 0, nSize);
			if (pParam)
			{
				memcpy(pData, pParam, nLength);
			}
			return STATUS_SUCCESS;
		}

		struct NetworkTask_s& operator = (const struct NetworkTask_s& rhs)
		{
			int nSize = 0;
			nID = rhs.nID;
			sMsg = rhs.sMsg;
			nPageNumber = rhs.nPageNumber;
			nPageSize = rhs.nPageSize;
			nPageCount = rhs.nPageCount;
			if (rhs.nDataSize > nDataSize)
			{
				DELETE_ARRAY(pData);
				nSize = rhs.nDataSize + rhs.nDataSize % 4;
				nDataSize = rhs.nDataSize;
				pData = new char[nSize];
			}
			else
			{
				nSize = nDataSize;
			}
			memset(pData, 0, nSize);
			if (rhs.pData)
			{
				memcpy(pData, rhs.pData, rhs.nDataSize);
			}

			return *this;
		}
	}NetworkTask;

	class INetworkCommand
	{
	public:
		virtual int					iSetPointer(char* pMsg, const int& nLength) = 0;
		virtual int					iSet(char* pMsg, const int& nLen) = 0;
		virtual int					iSet(const int& nCommand, char* pMsg, const int& nLen) = 0;
		virtual int					iSetVector(const int& nCommand, const int& nPageCount, std::vector<void*>* pVector, const int& nBlockSize) = 0;    //nBlockSize为vector对应结构体的长度
		virtual uint				iGetBlockType() = 0;     //块类型：普通结构体，vector 
		virtual uint				iGetBlockSize() = 0;     //每个结构体的size
		virtual uint				iGetBlockCount() = 0;    //vector长度
		virtual uint				iGetPageCount() = 0;     //返回页数,如果多页，则填入此项
		virtual uint				iGetCommand() = 0;       //命令类型
		virtual char*				iGetData() = 0;          //数据指针
		virtual uint				iGetDataLength() = 0;    //数据长度
		virtual uint				iGetLength() = 0;
		virtual char*				iGet() = 0;
	};

	typedef struct HttpParam_s
	{
		std::string					strID;
		std::string					strCert;

		HttpParam_s()
		{

		}
	}HttpParam;

	class IPacketBuffer
	{
	public:
		//包ID
		virtual uint				iGetID() = 0;
		//包类型:1 字节 可容纳255个指令
		virtual uint				iGetType() = 0;
		//分组： 1 字节 可容纳255个分组 
		virtual uint				iGetGroup() = 0;
		//目的地：  4字节 可存放IP 
		virtual uint				iGetDestination() = 0;
		//数据长度  4字节
		virtual uint				iGetDataLength() = 0;
		//数据长度    （包含包头）   
		virtual uint				iGetLength() = 0;
		//所有数据（不包含包头）   
		virtual char*				iGetData() = 0;
		//所有数据（包含包头）    
		virtual char*				iGet() = 0;
		//设置所有数据
		virtual int					iSet(const char* pData, const int& nLength) = 0;
		//设置包头和所有数据
		virtual int					iSet(const short& nID, byte nType, const char* pData, const int& nDataLength, const int& nDestination = 0, byte nGroup = 0) = 0;
		//设置包头
		virtual int					iSetHeader(const char* pData, const int& nDataLength) = 0;
		//设置数据
		virtual int					iSetData(const char* pData, const int& nDataLength) = 0;
	};

	class ISocketConnector
	{
	public:
		virtual SocketConnectorParam*	iGetParam() = 0;
		//开启本地检测服务器 
		virtual void				iListen() = 0;
		virtual void				iStop() = 0;
		virtual int					iAddTask(NetworkTask* pTask, const bool& bMultiple = false) = 0;
	};

	class IHttp
	{
	public:
		virtual HttpParam*			iGetParam() = 0;
		virtual int					iGenerateHead(const std::string& strUrl, const int& nTimeout = 10) = 0;

		virtual	int					iDownloadFile(const std::string& strUrl, const std::string& strPath,
			NetworkProgressFunc pProgressFunc, const int& nTimeout = 10, const int& nRetryTimes = 3,
			HttpCustomHeaderParam* pHeaderParam = nullptr) = 0;

		virtual int					iDownloadUrl(const std::string& strUrl, std::string& strResponse, std::map<std::string, std::string>& mapResponseHeader,
			const HttpHeaderType_e& eHeaderType = HTTP_HEADER_TYPE_NONE, const int& nTimeout = 10, const int& nRetryTimes = 3,
			HttpCustomHeaderParam* pHeaderParam = nullptr) = 0;

		virtual int					iPostUrl(const std::string& strUrl, std::string& strResponse, std::map<std::string, std::string>& mapResponseHeader,
			const std::string& strPostData, const HttpHeaderType_e& eHeaderType = HTTP_HEADER_TYPE_JSON,
			HttpPostParam* pParam = nullptr, const int& nTimeout = 10, const int& nRetryTimes = 3,
			HttpCustomHeaderParam* pHeaderParam = nullptr) = 0;

		virtual int					iUploadFile(const std::string& strUrl, std::string& strResponse, const std::string& strFilePath,
			const std::string& strFileFieldName, const std::string& strPostData, const HttpHeaderType_e& eHeaderType = HTTP_HEADER_TYPE_EXPECT,
			HttpPostParam* pParam = nullptr, const int& nTimeout = 10, const int& nRetryTimes = 3,
			HttpCustomHeaderParam* pHeaderParam = nullptr) = 0;
	};
	class INetworkManager
	{
	public:
		virtual bool				iGetNetworkAdapterList(std::vector<NetworkAdapter>& vecAdapter) = 0;
		virtual ISocketConnector*	iRegistSocketConnector(const SocketConnectorParam& tParam) = 0;
		virtual void				iFreeSocketConnector(const std::string& strID) = 0;
		virtual void				iFreeSocketConnector(ISocketConnector** ppSocketConnector) = 0;

		virtual IHttp*				iRegistHttp(const HttpParam& tParam) = 0;
		virtual void				iFreeHttp(const std::string& strID) = 0;
		virtual void				iFreeHttp(IHttp** ppHttp) = 0;

		virtual IPacketBuffer*		iCreatePacketBuffer() = 0;
		virtual void				iFreePacketBuffer(IPacketBuffer** ppPacketBuffer) = 0;

		virtual INetworkCommand*	iCreateNetworkCommand() = 0;
		virtual void				iFreeNetworkCommand(INetworkCommand** ppNetworkCommand) = 0;

		//加密通信数据，返回base64
		virtual int					iEncryptData(const char* szKey, const char* pDataIn, const int& nLengthIn, char*& pDataOut, int& nLengthOut) = 0;
		//解密通讯数据，传入base64
		virtual int					iDecryptData(const char* szKey, const char* pDataIn, const int& nLengthIn, char*& pDataOut, int& nLengthOut) = 0;
	};
}
