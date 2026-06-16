#pragma once

/*
标签类详细面板类
*/

namespace bc
{
	class PROJECT_BASED_API LabelDetailFactory
	{
	public:
		LabelDetailFactory();
		virtual ~LabelDetailFactory();

		virtual EventReturnType_e iProcessEvent(const BCEvent& tEvent);

		// 点击的时候调用(返回详情面板Node)，内部自动判断是显示还是隐藏
		virtual FactotryAssembleDetail* AutoSwitchState(FactoryAssemble* pLabelAssemble, bool bRevertClick);
		//更新数据
		virtual void UpdateData(ProjectBasedSceneElement* pData);
		virtual void FrameUpdate();

		void SetServerHost(const std::string& strHost) { m_strServerHost = strHost; }
		FactotryAssembleDetail* FindAssembleByData(ProjectBasedSceneElement* pData);
		FactotryAssembleDetail* FindAssembleBySelf(INode* pNode);
		void CloseByData(ProjectBasedSceneElement* pData);
		void CloseByAssemble(FactotryAssembleDetail* pAssemble);
		void CloseAll();
		void SetMaxShowCount(int nCount) { m_nMaxShowCount = nCount; }
		int GetMaxShowCount() { return m_nMaxShowCount; }
		void SetExtraData(const std::string& strKey, INT_PTR pExtraData);
		INT_PTR GetExtraData(const std::string& strKey);
		void GetCurAssemble(std::vector<FactotryAssembleDetail*>& vecAssemble);
		BoardParam* GetDetailBoardParam(int nType) { return &m_mapLabelBoardParam[nType]; }
		void DeleteByAssemble(FactotryAssembleDetail* pAssemble);
		void SetNodeCanReuse(bool bCan) { m_bNodeCanReuse = bCan; }
		bool CheckNodeHasSelectEffect(INode* pLabelNode);

	protected:
		//创建节点
		virtual INode* iCreateNode(int nType, ProjectBasedSceneElement *pData) = 0;
		virtual int iGetTypeByData(ProjectBasedSceneElement* pInfo) { return pInfo->nTypeID; }
		//节点打开时
		virtual void iOpenWithNode(FactotryAssembleDetail* pAssemble) {}
		//更新节点
		virtual void iUpdateWithNode(FactotryAssembleDetail* pAssemble) {};
		//节点关闭时
		virtual void iCloseWithNode(FactotryAssembleDetail* pAssemble) {};
		// 更新详细信息面板位置
		virtual void iFrameUpdateWithNode(FactotryAssembleDetail* pAssemble) {};

	protected:
		FactotryAssembleDetail* handleData(FactoryAssemble* pLabelAssemble, bool bRevertClick);
		void toWebNoSelectEffect();

	protected:
		ProjectBasedClient*					m_pClient;

		bool m_bRevertClick;	//点击是否置反
		std::string m_strServerHost;		//服务器地址（主要用于下载图片）

		std::vector<FactotryAssembleDetail*> m_vecIdleAssemble;	//空闲着的Assemble
		std::vector<FactotryAssembleDetail*> m_vecCurAssemble;		//当前使用的Assemble

		std::map<int, std::vector<INode*>> m_mapUsedNode;	//已经使用着的Node
		std::map<int, std::vector<INode*>> m_mapIdleNode;	//空闲着的Node
		std::map<std::string, INT_PTR> m_mapExtraData;

		int m_nMaxShowCount;			//同时显示的最大数量（0：表示不限制）
		std::map<int, BoardParam> m_mapLabelBoardParam;
		bool m_bNodeCanReuse;		//是否复用
		ProjectBasedSceneElement* m_pNoSelectEffectLastData;
	};
}


