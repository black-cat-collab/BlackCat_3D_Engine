#pragma once

namespace bc
{
	class ProjectBasedClient;
	class IFunc;
	class LabelDetailFactory;
	class ClusterFunc;
	class TweenAnimateForNode;
	class ListenElementParam;
	class SceneElementLoader;
	class PROJECT_BASED_API CommonLabelFactory
	{
	public:
		CommonLabelFactory();
		virtual ~CommonLabelFactory();

		virtual EventReturnType_e iProcessEvent(const BCEvent& tEvent);
		virtual void iDoWebCommond(WebCommandData* pWebData);

		void SetType(int nType) { m_nType = nType; }
		int GetType() { return m_nType; }
		//添加数据(创建失败会返回null，所以处理数据需要自行判断)
		virtual std::vector<FactoryAssemble*> AddData(std::vector<ProjectBasedSceneElement*>& vecData);
		virtual void UpdateData(std::vector<FactoryAssemble*>& vecData);
		virtual void FrameUpdate();
		virtual void AddFunc(ELabelFunc eFunc,IFunc* pFuncObject = nullptr);
		virtual void RemoveFunc(ELabelFunc eFunc,bool bDelete = true);
		IFunc* GetFunc(ELabelFunc eFunc);
		std::map<ELabelFunc, IFunc*>& GetAllFunc() { return m_mapFuncObject; }
		virtual bool IsInstanceFactory() { return false; }
		virtual void RefreshData(std::vector<ProjectBasedSceneElement*>* pVecData);		//刷新数据
		virtual void RefreshZero();
		virtual void SetAllVisible(bool bVisible);
		virtual void SetTypeVisible(std::vector<int>& vecType);
		std::vector<int> GetCurVisibleType() { return m_vecVisibleType; }
		bool IsAllVisible() { return m_bAllVisible; }
		bool IsNoTypeVisible();
		std::vector<int> GetVisibleType();
		void GetCurData(std::vector<ProjectBasedSceneElement*>& vecData);
		ProjectBasedSceneElement* RemoveByAssemble(FactoryAssemble *pAssemble);	//移除某个(数据自行销毁)
		FactoryAssemble* FindAssembleByData(ProjectBasedSceneElement* pData);
		FactoryAssemble* FindAssembleByNode(INode* pNode);
		FactoryAssemble* FindAssembleByID(const std::string& strID);
		void SetContainerGroup(INode* pGroupNode) { m_pContainerGroupNode = pGroupNode; }
		INode* GetContainerGroup();
		bool IsContainerNode(INode* pNode, bool bIncludeDetail = true);		//判断节点是否是这个Factory的
		void SetExtraData(const std::string& strKey, INT_PTR pExtraData);
		INT_PTR GetExtraData(const std::string& strKey);
		void SetServerHost(const std::string& strHost) { m_strServerHost = strHost; }
		LabelDetailFactory* GetDetailFactory() { return m_pDetailFactory; }
		//点击是否置反（若为true,打开的状态下，再次点击关闭）
		virtual void SetRevertClick(bool bRevertClick) { m_bRevertClick = bRevertClick; }
		bool IsRevertClick() { return m_bRevertClick; }
		void GetCurAssemble(std::vector<FactoryAssemble*>& vecAssemble);
		void RefreshAllVisible();		//刷新显隐
		void RefreshVisible(std::vector<FactoryAssemble*> &vecAssemble);		//刷新显隐，隐藏时会把pNode设置为null
		void SetPreCreateNodeCount(int nCount) { m_nPreCreateNodeCount = nCount; }  //预创建的个数
		virtual void SetAssembleVisible(std::vector<FactoryAssemble*> &vecAssemble,bool bVisible);
		virtual void SetAssembleVisible(FactoryAssemble* pAssemble, bool bVisible);
		void SetDefaultNoTifZ(float fZ) { m_fDefaultNoTifZ = fZ; }
		float GetDefaultNoTifZ() { return m_fDefaultNoTifZ; }
		void SetDefaultClusterNoTifZ(float fZ);
		float GetDefaultClusterNoTifZ();
		BoardParam* GetLabelBoardParam(int nType) { return &m_mapLabelBoardParam[nType]; }
		virtual void RefreshClusterVisible(std::vector<FactoryAssemble*>& vecAssemble);		//刷新聚合的显隐
		virtual bool CheckAssembleVisible(FactoryAssemble* pAssemble);
		void HideLabelTemporaryByID(std::vector<std::string> &vecID);		//临时隐藏标签（不从数据源里移除）,切换图层显隐会重新全部显示|隐藏
		void OnlyShowLabelTemporaryByID(std::vector<std::string>& vecID);		//临时显示标签，其他都隐藏，切换图层显隐会重新全部显示|隐藏
		void SetChangeShowByLoader(bool Change) { m_bChangeShowByLoader = Change; }
		bool IsChangeShowByLoader() { return m_bChangeShowByLoader; }
		virtual bool IsAllStop();				//是否都停止了，用于删除delete自身检测用，因为有线程得保证线程停止了才delete
		SceneElementLoader* GetSceneElementLoader();
		void SetSceneElementLoader(SceneElementLoader* pLoader) { m_pSceneElementLoader = pLoader; }

	public:
		//详情面板方法
		
		//返回数据的Type，用于多类型
		virtual int iGetTypeByData(ProjectBasedSceneElement* pInfo) { return pInfo->nTypeID; }
		//创建节点
		virtual INode* iCreateNodeForDetail(int nType,ProjectBasedSceneElement *pInfo) { return nullptr; }
		//节点打开时
		virtual void iOpenForDetail(FactotryAssembleDetail* pAssemble) {}
		//更新节点
		virtual void iUpdateDataForDetail(FactotryAssembleDetail* pAssemble) {}
		//节点关闭时
		virtual void iCloseForDetail(FactotryAssembleDetail* pAssemble) {}
		// 更新详细信息面板位置
		virtual void iFrameUpdateForDetail(FactotryAssembleDetail* pAssemble) {}
		//标签是否选中
		virtual void iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect) {}
		//详情面板按钮选中
		virtual void iOnSetDetailSelect(FactotryAssembleDetail* pAssemble, std::string strFlagName,bool bSelect) {}

		//聚合面板
		virtual INode* iCreateClusterLabelNode(int nType) { return nullptr; }
		virtual void iUpdateClusterLabelNode(INode* pClusterNode,const int nCount) {}
		virtual void iSetClusterLabelSelect(INode* pClusterNode, bool bSelect) {}
		virtual void iSetClusterParam(CommonLabelFactory* pFactory, ClusterFunc* pClusterFunc);

		//重叠标签
		virtual INode* iCreateOverTapLabelNode(int nType) { return nullptr; }
		virtual void iUpdateOverTapLabelNode(FactoryAssemble* pAssemble) {}
		virtual void iSetOverTapLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect) {}

		//平滑移动
		virtual void iSetSmoothMoveParam(TweenAnimateForNode* pTween);
		virtual Vector3d iTweenCalcAngle(int nType, Vector3d vCurOrigin, Vector3d vPreOrigin);
		virtual void iTweenMove(int nType, FactoryAssemble* pAssemble,  Vector3d vPos, Vector3d vAngle, bool bPerEndPoint);

		virtual void SetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect);
		virtual void SetDetailSelect(FactotryAssembleDetail* pAssemble,std::string strFlagName, bool bSelect);

	protected:
		//标签方法

		//创建节点
		virtual INode* iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo) = 0;
		//更新数据
		virtual void iUpdateDataForLabel(FactoryAssemble* pAssemble) {}
		//帧更新前
		virtual void iPreFrameUpdateForLabel() {}
		//帧更新中
		virtual void iFrameUpdateForLabel(FactoryAssemble* pAssemble) {}
		//帧更新后
		virtual void iPostFrameUpdateForLabel() {}
		//显示
		virtual void iShowAssembleForLabel(FactoryAssemble* pAssemble) {}
		//隐藏
		virtual void iHideAssembleForLabel(FactoryAssemble* pAssemble) {}

	protected:
		//处理数据
		virtual FactoryAssemble* handleData(ProjectBasedSceneElement* pData);

		INode* checkNeedCreateNode(int nType, ProjectBasedSceneElement* pData,bool bForceCreate);
		int getAllNodeCount(int nType);
		virtual void setAssembleVisible(FactoryAssemble *pAssemble,bool bVisible,bool bDynamicNode = true);
		virtual INode* bindIdleNode(ProjectBasedSceneElement* pData,bool bForceCreate);		//绑定空闲或者创建Node
		virtual void releaseUsedNode(int nType,INode *pNode);		//释放使用的Node
		virtual void updateDataForLabel(FactoryAssemble* pAssemble);
		virtual void showAssembleForLabel(FactoryAssemble* pAssemble);
		virtual void hideAssembleForLabel(FactoryAssemble* pAssemble);
		virtual void frameUpdateForLabel(FactoryAssemble* pAssemble);
		bool isTypeVisible(int nType);

	private:
		void sendToWebListenElementRealTime(ListenElementParam* pParam);
		virtual bool isInRangeToCamera(FactoryAssemble* pAssemble);

	protected:
		ProjectBasedClient*					m_pClient;
		std::string m_strServerHost;		//服务器地址（主要用于下载图片）
		int m_nType;						//类型，同后台图层id
		bool m_bNodeCanReuse;					//节点是否复用
		std::vector<FactoryAssemble*> m_vecIdleAssemble;	//空闲着的Assemble
		std::vector<FactoryAssemble*> m_vecCurAssemble;		//当前使用的Assemble
		std::vector<ProjectBasedSceneElement*> m_vecCurData;	//当前的数据

		std::map<int, std::vector<INode*>> m_mapUsedNode;	//已经使用着的Node
		std::map<int, std::vector<INode*>> m_mapIdleNode;	//空闲着的Node
		std::map<ELabelFunc, IFunc*> m_mapFuncObject;	//功能模块（聚合，重叠等）

		int m_nPreCreateNodeCount;		//预先创建的Node数量（聚合模式下生效）
		bool m_bAllVisible;						//是否全显示
		std::vector<int> m_vecVisibleType;	//当前显示的Type
		INode* m_pContainerGroupNode;		//节点的容器
		std::map<std::string, INT_PTR> m_mapExtraData;
		LabelDetailFactory* m_pDetailFactory;
		bool m_bRevertClick;				//点击是否置反（若为true,打开的状态下，再次点击关闭）
		float m_fDefaultNoTifZ;			//默认未取高地图的高度，默认=0.0f，0.0f则标识未生效需要设置不为0后才生效
		std::map<int,BoardParam> m_mapLabelBoardParam;
		std::vector<std::string> m_vecLastDetailUUID;	//刷新前打开详情面板的id
		std::vector<std::string> m_vecHideOrShowLabelTempID;	//临时隐藏
		int m_nTemporaryState;	//当前临时显隐状态    0：无   1：隐藏    2：仅显示
		bool m_bChangeShowByLoader;		//显隐是否受sceneLoader控制
		bool m_bCalcDisHide;					//是否距离显隐判断
		int m_nCurFrameUpdateIndex;		//用于每帧分摊（对于大数据使用）
		BCMutexLock							m_CurAssembleLock;
		SceneElementLoader* m_pSceneElementLoader;
	};
}


