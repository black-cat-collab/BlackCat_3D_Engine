#pragma once

#include <queue>

namespace bc
{
	enum ThreadRequestType_e		//请求类型
	{
		THREAD_REQUEST_CUSTOM,		//自定义
		THREAD_REQUEST_API_GET,		//api get请求
		THREAD_REQUEST_API_POST,		//api post请求
		THREAD_REQUEST_UPLOAD,		//api 上传
		THREAD_REQUEST_DOWNLOAD,		//下载功能
	};

	enum ThreadReturnFormat_e			//返回数据类型
	{
		RETURN_FORMAT_JSON				//json格式
	};

	struct ThreadCallbackParam_s;
	struct ThreadRequestParam_s;

	// 线程完成回调  pParam 为返回的参数类  pTimer只有当时循环的时候不为NULL，其他的都为NULL
	typedef void(*ThreadCompleteFunc)(ThreadCallbackParam_s* pParam);
	// 线程执行函数	return false;不往下执行  return true;继续执行
	typedef bool(*ThreadRuningFunc)(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam);
	// 解密
	typedef int(*DecodeFunc)(const char* pKey, const char* pDataIn, const int& nLengthIn, char*& pDataOut, int& nLengthOut);
	// 请求前
	typedef void(*PreDoRequestFunc)(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam);

	//请求参数
	typedef struct ThreadRequestParam_s
	{
	public:
		std::string						strID;						//ID
		std::string						strUrl;						//请求地址
		std::string						strPostData;				//post请求参数
		std::string						strDownloadPath;			//下载的路径
		int								nTimeoutTime;				//超时时间
		int								nRetryCount;				//错误尝试次数
		long							lInterval;					//间隔时间(毫秒)
		long							lDelay;						//延迟时间(毫秒)
		bool							bRepeat;					//是否循环
		bool							bThread;					//是否使用线程
		bool							bDecode;					//是否需要解密
		bool							bRunCallbackInMainThread;	//是否在主线程运行完成回调
		HttpPostParam					vecHttpPostParam;
		ThreadCompleteFunc				pThreadCompleteFunc;		//回调方法
		ThreadRuningFunc				pThreadRuningFunc;			//执行方法（自定义的模式），当为null的时候直接走回调
		DecodeFunc						pDecodeFunc;				//解密
		ThreadRequestType_e				eRequestType;				//请求类型
		ThreadReturnFormat_e			eReturnType;				//数据返回格式
		std::map<std::string, INT_PTR>	mapParams;					//参数
		bool							bStop;						//是否停止(接着将自动销毁)
		std::vector<std::pair<std::string, Json::Value>> vecHttpCustomHeaderParam;		//自定义请求头参数
		PreDoRequestFunc				pPreDoRequestFunc;			//请求前的回调
		std::map<std::string, Json::Value>	mapJValue;		//变量存储区
		bool							bInQueue;					//是否放置于队列里
		std::string						strQueueID;					//队列id
		std::string						strUploadFilePath;			//上传的文件路径(全路径)
		std::string						strUploadFileFieldName;			//服务器上传文件的fieldName(服务器定义)

	private:
		bool			bInnerLoopTask;			//是否是内部循环的一次任务
		std::string		strInnerUUID;			//内部用来区别的uuid

	public:
		ThreadRequestParam_s() :
			lDelay(0),
			lInterval(0),
			bThread(true),
			bRepeat(false),
			bDecode(false),
			nRetryCount(-1),
			nTimeoutTime(-1),
			bRunCallbackInMainThread(false),
			pDecodeFunc(nullptr),
			pThreadRuningFunc(nullptr),
			pThreadCompleteFunc(nullptr),
			eReturnType(RETURN_FORMAT_JSON),
			eRequestType(THREAD_REQUEST_CUSTOM),
			bStop(false),
			bInnerLoopTask(false),
			pPreDoRequestFunc(nullptr),
			bInQueue(false)
		{
			 Guid::CreateGuId(strInnerUUID);
		}

		~ThreadRequestParam_s()
		{
		}

		struct ThreadRequestParam_s& operator = (const struct ThreadRequestParam_s& src)
		{
			strID = src.strID;
			strUrl = src.strUrl;
			strPostData = src.strPostData;
			strDownloadPath = src.strDownloadPath;
			nTimeoutTime = src.nTimeoutTime;
			nRetryCount = src.nRetryCount;
			lInterval = src.lInterval;
			lDelay = src.lDelay;
			bRepeat = src.bRepeat;
			bThread = src.bThread;
			bDecode = src.bDecode;
			bRunCallbackInMainThread = src.bRunCallbackInMainThread;
			vecHttpPostParam = src.vecHttpPostParam;
			pThreadCompleteFunc = src.pThreadCompleteFunc;
			pThreadRuningFunc = src.pThreadRuningFunc;
			pDecodeFunc = src.pDecodeFunc;
			eRequestType = src.eRequestType;
			eReturnType = src.eReturnType;
			mapParams = src.mapParams;
			bStop = src.bStop;
			vecHttpCustomHeaderParam = src.vecHttpCustomHeaderParam;
			bInnerLoopTask = src.bInnerLoopTask;
			pPreDoRequestFunc = src.pPreDoRequestFunc;
			mapJValue = src.mapJValue;
			strInnerUUID = src.strInnerUUID;
			bInQueue = src.bInQueue;
			strQueueID = src.strQueueID;
			strUploadFilePath = src.strUploadFilePath;
			strUploadFileFieldName = src.strUploadFileFieldName;

			return *this;
		}

		void AddParam(const std::string strKey, INT_PTR nParam)
		{
			//添加执行参数
			mapParams[strKey] = nParam;
		}

		INT_PTR GetParam(const std::string strKey)
		{
			if (mapParams.find(strKey) != mapParams.end())
			{
				return mapParams[strKey];
			}
			return NULL;
		}

		void AddExtraJValue(const std::string strKey, Json::Value jValue)
		{
			//添加执行参数
			mapJValue[strKey] = jValue;
		}

		Json::Value GetExtraJValue(const std::string strKey)
		{
			if (mapJValue.find(strKey) != mapJValue.end())
			{
				return mapJValue[strKey];
			}
			return Json::nullValue;
		}

		void SetInnerTask(bool bInner)
		{
			bInnerLoopTask = bInner;
		}

		bool IsInnerTask()
		{
			return bInnerLoopTask;
		}

		std::string GetUUID()
		{
			return strInnerUUID;
		}

		void SetUUID(std::string strUUID)
		{
			strInnerUUID = strUUID;
		}
	}ThreadRequestParam;

	//回调参数
	typedef struct ThreadCallbackParam_s
	{
		ThreadRequestParam		sRequestParam;
		std::map<std::string, INT_PTR>	mapParams;

		int						nSelfID;
		int						nResult;
		std::string				strUrl;
		std::string				strResponse;
		std::map<std::string, std::string> mapResponseHeader;
		Json::Value				jRoot;

		ThreadCallbackParam_s() :
			nSelfID(0),
			nResult(STATUS_ERROR)
		{

		}

		~ThreadCallbackParam_s()
		{
		}

		//添加回调参数
		void AddParam(std::string strKey, INT_PTR nParam)
		{
			mapParams[strKey] = nParam;
		}

	}ThreadCallbackParam;

	class PROJECT_BASED_API ThreadWrapper
	{
	public:
		~ThreadWrapper();

		static ThreadWrapper* GetInstance();
		static void Destroy();
		virtual void Clear();
		//添加任务
		virtual int AddTask(ThreadRequestParam &pParam);		
		//执行回调任务(在update类似的方法中执行)
		virtual void RunCallback();	
		//进行网络请求
		virtual int DoNetRequest(ThreadRequestParam &sRequestParam, ThreadCallbackParam *pCallbackParam);			
		virtual ThreadRequestParam* GetNetTask(const std::string &strID);
		//bFuzzy 是否模糊
		virtual void StopNetTask(const std::string& strID, const bool& bFuzzy = false);
		//添加主线程运行的任务
		virtual void AddMainRunCallback(ThreadCallbackParam* pCallbackParam);

		virtual void SetAesKey(const std::string& strKey);
		virtual void ClearQueueTask(std::string strQueueID);

	protected:
		ThreadWrapper();

	private:	
		static void	threadCallbackFunc(std::map<std::string, INT_PTR>& mapParam);
		static int	decodeCallbackFunc(const char* pKey, const char* pDataIn, const int& nLengthIn, char*& pDataOut, int& nLengthOut);

		//处理自定义
		bool		handleCustomFunc(ThreadCallbackParam *pParam);	
		void		removeNetTask(const std::string& strID);
		void		checkToNextTask(std::string strID,std::string strUUID);

	protected:
		static ThreadWrapper*				m_pThis;
		std::string								m_strAesKey;
		BCMutexLock							m_MutexLock;
		std::queue<ThreadCallbackParam*>	m_queThreadCallbackParam;
		std::vector<ThreadRequestParam*>	m_vecNetTask;
		//https证书文件名称
		std::string							m_strCertFile;
		std::map<std::string, std::queue<ThreadCallbackParam*>> m_mapQueThreadRequest;//请求数队列（仅线程才添加）
		std::map<std::string, int> m_mapCurRunningRequestCount;
	};
}


