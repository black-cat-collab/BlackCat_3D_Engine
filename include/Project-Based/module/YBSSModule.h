#pragma once

/***************/
//一标三实地址关联
/***************/

namespace bc
{
	class IStateModule;
	class BaseModule;
	class IsolatedTool;

	class YBSSInfo
	{
	public:
		std::string strAddress;		//地址名称
		std::string strBindId;		//关联户室id
		std::string strBindShell;	//关联分片shell
		int nBindType;				//关联状态，1表示未关联，2表示已关联
		std::string strBuildingId;	//楼栋id
		std::string strNodeID;		//ShellId
		std::string strExtra;		//额外信息

		bool		bRequested;		//是否已经过绑定接口
		INT_PTR		externParam;	//存放项目自定义的结构体

		YBSSInfo()
		{
			nBindType = 1;
			Json::Value item = {};
			item["address"] = "";
			item["bind_id"] = "";
			item["bind_shell"] = "";
			item["bind_type"] = 1;
			item["building_id"] = "";
			strExtra = item.toStyledString();
			bRequested = false;
			externParam = NULL;
		}

		class YBSSInfo& operator = (const class YBSSInfo& src)
		{
			strAddress = src.strAddress;
			strBindId = src.strBindId;
			strBindShell = src.strBindShell;
			nBindType = src.nBindType;
			strBuildingId = src.strBuildingId;
			strNodeID = src.strNodeID;
			strExtra = src.strExtra;
			bRequested = src.bRequested;
			externParam = src.externParam;
			return *this;
		}
	};

	class PROJECT_BASED_API YBSSModule :public BaseModule
	{
	protected:
		class YBSSNetTask
		{
		public:
			std::vector<std::string> vecNode;
		};

		class NodeLoad : public NodeLoadCallback 
		{
		public:
			virtual void Loading(INode* pNode, float progress) override;
			virtual void Loaded(INode* pNode) override;
			virtual void Preloading(INode* pNode) override;
		};

	public:
		YBSSModule(const std::string& strModuleName);
		virtual ~YBSSModule();

		void SetShellLoadDis(float fDis);

	public:
		virtual void				iFrameUpdate(IStateManager* pStateManager);
		virtual void				iSceneLoad(IStateManager* pStateManager);
		virtual EventReturnType_e	iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent);
		virtual void				iRefreshData();
		virtual void				iDoWebCommond(WebCommandData* pWebData);				//处理网页数据
		virtual void				iOnAppModeChanged(int nAppMode);
		virtual void				iOnAppIdChanged(std::string strAppId);

		virtual IShellNode*			iGetShellNodeById(std::string strShellId, std::string strShellRootId = "");
		void						AssembleShellToWeb(INode* pShellNode, const std::string& strWebId);
		void						SaveChange();											//保存
		void						CancelChange();											//取消
		void						LoadYBSSXml(std::string& strAppId);

		virtual ShellNodeParam		SetShellGenerateParam();			//设置切割默认颜色

	protected:
		static YBSSModule* s_pThis;
		void						clear();
		void						refreshAllShell();
		void						parseYBSSInfo(YBSSInfo* pInfo, Json::Value& jObject);
		void						setColor(IShellNode* pNode);//设置节点颜色
		IShellNode*					getSelectShellNode();
		void						setShellHoverEnable(INode* pNode, bool bEnable);
		void						setShellShow();
		void						frameUpdateLoadXml();
		Vector3						getNodeOrigin(INode* pNode);
		void						checkReLoadXml();
		virtual YBSSInfo*			iCreateYBSSInfo();
		virtual void				iSetShellAttr(IShellNode* pShellNode);
		virtual void				iOnXmlLoaded() {}		//xml加载完回调
		virtual void				doRequest();
		void						doCheckBackup();		//检测是否需要备份
		void						saveBackup(const std::vector<std::string>& vecNodeId);	//编辑端备份操作的单体化楼栋的Tw4文件
		static bool					static_BackuppingTw4(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam);

		static bool					static_Backupping(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam);
		static void					static_Backupped(ThreadCallbackParam* pParam);				
	public:
		void						SetVisible(bool bVisible);

		//切割回调函数
		static void					Static_CreateShellRootNode(INode* pSelector);
		static void					Static_CreateShellNodesSplitVerticalPlanel(INode* pSelector);
		static void					Static_CreateShellNodesSplitVerticalRect(INode* pSelector);
		static void					Static_CreateShellNodesSplitVerticalSingleCut(INode* pSelector);
		static void					Static_CreateShellNodesSplitSingleHorizontal(INode* pSelector);
		static void					Static_CreateShellNodesSplitAllHorizontal(INode* pSelector);

		//复制文件
		static bool					Copy(const std::string& src, const std::string& dst, bool bCover = false);

		static void					static_pre_request_BindList(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam);
		static void					static_callback_BindList(ThreadCallbackParam* pParam);

		void						loadShellRootNode();
		//线程加载单体化数据
		static bool					static_load_shellnode(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam);
	public:
		IsolatedTool*				m_pIsolatedTool;					//单体化工具类
		bool						m_bIsolatedEnable;					//编辑模式
		bool						m_bDoingSplite;						//进行切割
		bool						m_bIndividualBuilding;				//单体化楼栋
		bool						m_bVisible;
		bool						m_bReLoadXml;						//是否重新加载xml
		bool						m_bAutoBackup;						//是否自动备份
		bool						m_bContinuousCutting;				//是否连续切割（除了单体楼栋）
		bool						m_bBackupDone;						//备份是否完成 (表示线程执行完成)
		long long 					m_lLastTime;						//加载XML 上一次时间戳
		long long 					m_lLastTimeCheckBackup;				//备份  上一次时间戳
		float						m_dRatio;
		float						m_fShellLoadDis;
		BCMutexLock					m_stMutex;
		Vector3						m_vLastCameraPos;
		ThreadCallbackParam*		m_pCurThreadBindListCallback;
		INode*						m_pCurIsolateNode;					//当前操作的Node				
		std::string					m_strIndividualBuildingNodeId;
		std::string					m_strAppID;
		std::string					m_strAccount;
		std::string					m_strSaveDirPath;
		std::string					m_strTw4Path;
		std::queue<std::string>		m_queueLoadXmlFile;					//分摊加载xml文件，不然一次性加载会卡住很久
		std::vector<std::string>	m_vecNeedGenerateMeshShellGroup;
		std::stack<IShellNode*>		m_stackRequestShellNode;
		std::vector<INode*>			m_vecNewBuilding;					//每次 完成 前 单体化的楼栋
		std::vector<INode*>			m_vecSplitBuilding;					//每次 完成 前 水平 垂直切的楼栋

		int							m_nNeedLoadCount;					//需要加载的XML个数
		int							m_nLoadedCount;						//已经加载的XML个数

		std::map<std::string, std::string> m_mapShellAndShellRootId;	//shell 映射 shellRoot 的 id
	};
}

