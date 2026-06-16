#pragma once

#define DEFAULT_EXCLUSIVE_KEY  std::string("default_exclusive_key")  //默认互斥组

namespace bc 
{
	class CommonLabelFactory;
	class InstanceLabelFactory;
	class ProjectBasedClient;
	class BCRedis;

	//支持的请求类型
	enum ESupportRequestType
	{
		REQUEST_TCP =  1 << 1,	
		REQUEST_REDIS = 1 << 2,
	};

	class PROJECT_BASED_API SceneElementLoader
	{

		class RequestEndData
		{
		public:
			std::vector<ProjectBasedSceneElementTypeInfo*> vecTempSceneTypeInfo;
			std::vector<ProjectBasedSceneElementMapInfo*> vecTempMapInfo;
			std::vector<ProjectBasedSceneElement*> vecSceneElement;
			std::vector<std::string> vecDataType;
		};

		class PageData
		{
		public:
			bool	bClear;
			bool	bComplete;
			std::vector<ProjectBasedSceneElement*> vecData;

			PageData()
			{
				bClear = false;
				bComplete = false;
			}
		};

		class SubscribeBufferData {
		public:
			std::queue<ProjectBasedSceneElement*> queueElement;
			int nType;		//1：静态数据    2：udp    -1:清空  -2:websocket断开连接
			std::vector<int> vecLayers;	//需要更新的图层，为空则表示全部更新

			SubscribeBufferData()
			{
				nType = -1;
			}
		};

	public:
		SceneElementLoader();
		~SceneElementLoader();

		void SetServerIp(const std::string& strServerIp) { m_strServerHost = strServerIp; }

		virtual void iRequestData(bool bThread);
		virtual void iRefreshData(bool bThread);
		virtual void OnlyRequestBaseInfo(bool bThread, bool bToWebCommand);
		virtual void ParseSubscribeData(const std::string* pStrData);	//解析后台订阅发送的消息,使用指针效率高
		void iDoUdpData(std::string strData);
		void iDoUdpData(std::vector<ProjectBasedSceneElement*> &vecData,bool bNeedReadTif);
		void SetWriteWebSocketLog(bool bWrite) { m_bWriteWebSocketLog = bWrite; }
		virtual void iDoWebCommond(WebCommandData* pWebData);
		void RequestDoSubscribeFromRedis();	//订阅 从缓存获取数据 (redis)
		void RequestDoSubscribe();			//开始订阅
		void FrameUpdate();
		virtual EventReturnType_e	iProcessEvent(const BCEvent& tEvent);
		void AddExclusiveFactory(std::string strGroupKey, int nType);		//添加互斥
		void AddDefaultExclusiveFactory(int nType);		//添加互斥到默认互斥组
		void RemoveExclusiveFactory(std::string strGroupKey, int nType);		
		void RemoveDefaultExclusiveFactory(int nType);		
		void ClearExclusiveFactory(const std::string& strGroupKey);		
		void ClearDefaultExclusiveFactory();	
		std::map<std::string, std::vector<int>> GetExclusiveFactory() { return m_mapExclusiveFactoryType; }

	public:
		//设置详情面板数据
		virtual void iOnSetDetailData(INode* pNode, ProjectBasedSceneElementTypeInfo* pTypeInfo, ProjectBasedSceneElement* pInfo);
		virtual void iOnCloseDetail(INode* pNode, ProjectBasedSceneElementTypeInfo* pTypeInfo, ProjectBasedSceneElement* pInfo);
		virtual void iOnOpenDetail(INode* pNode, ProjectBasedSceneElementTypeInfo* pTypeInfo, ProjectBasedSceneElement* pInfo);
		//设置标签信息数据
		virtual void iOnSetLabelData(INode* pNode, ProjectBasedSceneElementTypeInfo* pTypeInfo, ProjectBasedSceneElement* pInfo);

	protected:
		//获取基础数据（类型和地图关联）
		void requestBaseInfo(bool bThread,bool bToWebCommand);
		//网络请求的拦截处理
		void requestElementTypeInfo(RequestEndData* pRequestEndData);		//获取元素类型基础信息(同步)
		void requestMapInfo(RequestEndData* pRequestEndData);				//获取图片映射关系(同步)

		void addPageTaskToMain(PageData* pPageData, ThreadCallbackParam* stCallbackParam);
		virtual void parseElementTypeInfo(ProjectBasedSceneElementTypeInfo *pTypeInfo,Json::Value &jValue);

		//是否过滤掉元素,false则忽略此元素
		virtual bool iFilterElementInfo(EElementCoordinateType eType,Json::Value &jData, ProjectBasedSceneElementTypeInfo* pTypeInfo, ProjectBasedSceneElement* pInfo) { return true; }
		virtual void iFrameUpdate();
		virtual void iOnPlayingError(IVideo* pIVideo,ProjectBasedSceneElement *pInfo,bool isLivegbs);
		virtual ProjectBasedSceneElement* iCreateSceneElementInfo(int nType) { return new ProjectBasedSceneElement; }

	private:
		static bool static_request_BaseInfo(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam* stCallbackParam);	//线程执行函数	return false;不往下执行  return true;继续执行
		static void static_callback_BaseInfo(ThreadCallbackParam* pParam);
		static bool static_parse_subscribe(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam* stCallbackParam);	//线程执行函数	return false;不往下执行  return true;继续执行
		static void static_callback_subscribe(ThreadCallbackParam* pParam);
		static void	static_callback_dosubscribe(ThreadCallbackParam_s* pParam);
		static bool	static_request_subscribe_cache(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam* stCallbackParam);//订阅之后 去取缓存 
		static void	static_callback_subscribe_cache(ThreadCallbackParam_s* pParam);
		static void	static_playerror_callback(IVideo* pIVideo,
			std::map<std::string, void*>& mapPlayExtraVoid, 
			std::map<std::string, Json::Value>& m_mapPlayExtraJson);

		virtual void loadDataComplete(bool bAllComplete,bool bToWebCommond);
		void setDataMatch(INode* pNode, std::map<std::string, std::string> &mapDataMatch, ProjectBasedSceneElement* pInfo,bool bPlayVideo);
		int comParseSubscribeData(int nParseType,std::string &strData,std::vector<ProjectBasedSceneElement*> &vecData,bool bUseHeightTif);
		void doThreadParseSubscribeTask(int nType, const std::string* pStrData);
		void replaceBaseSceneTypeStyle(Json::Value &jSrc,Json::Value &jParamObject);	//覆盖基础样式的参数
		void getSubscribeCache(ThreadCallbackParam* stCallbackParam, int nPageNo, int nPageSize, std::string strCacheHost);

		void doThreadRefreshInstanceFactory();
		static bool static_request_refreshInstanceFactory(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam* stCallbackParam);	//线程执行函数	return false;不往下执行  return true;继续执行
		static void static_callback_refreshInstanceFactory(ThreadCallbackParam* pParam);

	public:
		BCRedis* m_pBCRedis;						//redis
		bool	m_bConnectedRedis;				//是否连接成功redis服务器

	protected:
		bool	m_bWriteWebSocketLog;				//是否打印日志
		BCMutexLock		m_MutexSubcribeLock;
		std::queue<SubscribeBufferData*> m_queueSubscribeBuffer;	//缓冲区
		int m_nSupportRequestType;		//当前支持的请求类型，ESupportRequestType的位运算

		std::string		m_strServerHost;
		std::vector<ProjectBasedSceneElementTypeInfo*> m_vecSceneTypeInfo;		//上图元素基础类型
		std::vector<ProjectBasedSceneElementMapInfo*> m_vecMapInfo;				//上图元素地图信息
		std::vector<CommonLabelFactory*> m_vecElementFactory;
		std::vector<ProjectBasedSceneElement*> m_vecResidentElement;					//常驻要素
		std::map<int, CommonLabelFactory*> m_mapInElementFactory;					//室内
		std::map<int, CommonLabelFactory*> m_mapOutElementFactory;					//室外
		std::vector<ProjectBasedSceneElement*> m_vecUdpSceneElement;		//udp数据过来
		std::map<int, std::vector<ProjectBasedSceneElement*>> m_mapExtraSceneElement;		//手动额外添加的数据
		std::queue<ProjectBasedSceneElement*>					m_queueAddElement;
		std::queue<ProjectBasedSceneElement*>					m_queueUpdateElement;
		std::vector<std::string>		 m_vecCurOutShowType;
		std::vector<std::string>		 m_vecCurInShowType;				//室内
		std::map<std::string, INode*>			 m_mapFloorNode;

	public:
		ProjectBasedSceneElementTypeInfo* GetElementTypeInfoByID(int nType);
		ProjectBasedSceneElementTypeInfo* GetElementTypeInfoByName(std::string strInnerName);
		ProjectBasedSceneElementMapInfo* GetElementMapInfoByID(int nMapID);
		std::vector<ProjectBasedSceneElementTypeInfo*> GetAllSceneElementTypeInfo();
		void GetOutSceneElementByType(int nType, std::vector<ProjectBasedSceneElement*>& vecData);
		void GetInSceneElementByType(int nType, std::vector<ProjectBasedSceneElement*>& vecData);
		void GetOutAllSceneElement(std::vector<ProjectBasedSceneElement*>& vecData);
		void GetInAllSceneElement(std::vector<ProjectBasedSceneElement*>& vecData);
		CommonLabelFactory* GetOutElementFactoryByType(int nType);
		CommonLabelFactory* GetInElementFactoryByType(int nType);
		ProjectBasedSceneElement* FindSceneElementByID(std::vector<ProjectBasedSceneElement*>& vecData, std::string strID, int nType, bool bErase = false);
		void ClearElementShow();
		void ClearResidentElement(int nType);
		//手动数据
		void AddElement(const std::vector<ProjectBasedSceneElement*>& vecSceneElement, bool bNeedReadTif, bool bQueue = false);
		//清除Factory的数据
		void ClearExtraData(int nType, bool bRefreshFactory = true);
		void ClearUdpData(int nType, bool bRefreshFactory = true);
		void GetUdpElement(std::vector<ProjectBasedSceneElement*>& vecUdpData, int nType = -1);
		void GetExtraElement(std::vector<ProjectBasedSceneElement*>& vecExtraData, int nType = -1);
		float GetPerFrameHandleLabelCount() { return m_nPerFrameHandleLabelCount; }
		void SetPerFrameHandleLabelCount(int nCount) { m_nPerFrameHandleLabelCount = nCount; }
		void GetAllOutElementFacotory(std::vector<CommonLabelFactory*>& vecFactory);
		void GetAllInElementFacotory(std::vector<CommonLabelFactory*>& vecFactory);
		virtual void ChangeShowLabel();
		void AddRefreshInstanceDataFactory(InstanceLabelFactory* pFactory);
		void RemoveRefreshInstanceDataFactory(InstanceLabelFactory* pFactory);
		bool IsRefreshInstanceDataFactory(InstanceLabelFactory* pFactory);

	public:
		virtual void iPreCreateSetParam(int nType, NodeParamBase* pParam) {} //创建之前的额外设置

	protected:
		ProjectBasedSceneElementTypeInfo* getElementTypeInfoByID(int nType, std::vector<ProjectBasedSceneElementTypeInfo*>* pVector);
		ProjectBasedSceneElementTypeInfo* getElementTypeInfoByName(std::string strInnerName, std::vector<ProjectBasedSceneElementTypeInfo*>* pVector);
		ProjectBasedSceneElementMapInfo* getElementMapInfoByID(int nMapID, std::vector<ProjectBasedSceneElementMapInfo*>* pVector);
		virtual CommonLabelFactory* doCreateFactory(std::vector<ProjectBasedSceneElementTypeInfo*>* pVector, int nPosition, int nType);
		bool checkIndoor(ProjectBasedSceneElement* pInfo);
		virtual void createFactory(std::vector<ProjectBasedSceneElementTypeInfo*>* pVector, std::vector<int> vecNeedRefreshType = std::vector<int>());
		virtual void resetFactory();
		void classfyElement(std::map<int, std::vector<ProjectBasedSceneElement*>>& mapInElement,
			std::map<int, std::vector<ProjectBasedSceneElement*>>& mapOutElement);
		void appendRefreshFactoryElementData(std::vector<ProjectBasedSceneElement*> vecSceneElement, bool bQueue);	//追加Factory元素,元素已添加到数组中不会再添加
		void updateRefreshFactoryElementData(std::vector<ProjectBasedSceneElement*> vecSceneElement);	//更新Factory元素,元素已添加到数组中不会再添加
		void doQueueAppendElementData();
		void doQueueUpdateElementData();
		void doingAppendElementData(std::vector<ProjectBasedSceneElement*> vecSceneElement);
		void doingUpdateElementData(std::vector<ProjectBasedSceneElement*> vecSceneElement);

	protected:
		//创建Factory
		virtual CommonLabelFactory* iCreateElementFactory(int nType,int nNodeType);
		virtual void iOnLoadedData(bool bAllComplete) {}		//数据加载完成
		virtual bool iDoPositionPost(ProjectBasedSceneElement* pInfo, Vector3d& vPos, INode* pNode) { return false; }
		virtual void iRefreshFactoryData(int nType, CommonLabelFactory* pFactory, std::vector<ProjectBasedSceneElement*>& vecData);
		virtual void iOnCreateFactory(CommonLabelFactory *pFactory) {};		//创建回调

	protected:
		ProjectBasedClient* m_pClient;
		int	m_nPerFrameHandleLabelCount;	//每帧处理标签的个数
		std::vector<CommonLabelFactory*> m_vecWillDeleteFactory;	//需要删除的Factory
		std::vector<std::string> m_vecModifyStyleSelectPoint;		//修改样式选中了的

	private:
		std::map<std::string,std::vector<int>> m_mapExclusiveFactoryType;		//相互互斥
		BCMutexLock		m_RefreshInstanceFactoryLock;
		bool m_bThreadRefreshInstanceRunning;
		std::vector<InstanceLabelFactory*> m_vecRefreshInstanceFactory;		//线程更新Instance里的数据
	};
}


