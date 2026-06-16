#pragma once

namespace bc
{
	typedef struct PathPoint_s
	{
		Vector3		vPos;
		Quaternion	qRot;
		byte		nFlag;
		float		fMSecond;
		PathPoint_s() : 
			vPos(),
			qRot(),
			nFlag(0),
			fMSecond(33.33f)
		{

		}
	}PathPoint;

	typedef struct PathLine_s
	{
		std::string strID;
		std::vector<PathPoint> vecPathPoint;
		PathLine_s()
		{

		}
	}PathLine;

	typedef struct SwitchViewParam_s
	{
		std::string strSceneViewID;
		std::string strEntityManagerID;
		std::string strSceneFile;
		bool		bReloadScene;
		Vector2		vViewSize;
		Vector2		vOutputSize;
		SwitchViewParam_s() :
			bReloadScene(false)
		{
			vViewSize = Vector2(1920, 1080);
			vOutputSize = Vector2(1920, 1080);
		}
	}SwitchViewParam;

	class IViewManager
	{
	public:

		/*
		*	视图管理器初始化
		*/
		virtual void				iInitialize() = 0;

		/*
		*	是否在加载场景
		*	return  [out]  
		*/
		virtual bool				iIsSceneLoad() = 0;

		/*
		*	设置场景大小
		*	nWidth   [in]  场景宽度
		*	nHeight  [in]  场景高度
		*/
		virtual void				iSetSceneSize(const int& nWidth, const int& nHeight) = 0;
		virtual int					iGetSceneWidth() = 0;
		virtual int					iGetSceneHeight() = 0;

		/*
		*	设置输出大小
		*	nWidth   [in]  宽度
		*	nHeight  [in]  高度
		*/
		virtual void				iSetOutputSize(const int& nWidth, const int& nHeight) = 0;
		virtual int					iGetOutputWidth() = 0;
		virtual int					iGetOutputHeight() = 0;

		/*
		*	保存场景
		*	strSceneFile	[in]  场景文件名
		*	return			[out]  是否成功
		*/
		virtual bool				iSaveScene(const std::string& strSceneFile, const std::string& strEntityID = "") = 0;

		/*
		*	获得场景根节点
		*	return  [out]  根节点
		*/
		virtual INode*				iGetRootNode() = 0;

		/*
		*	通过节点ID查找节点
		*	strID   [in]  节点ID
		*	return  [out]  查找到的节点
		*/
		virtual INode*				iFindNodeByID(const std::string& strID) = 0;

		/*
		*	通过XML文件创建并且初始化节点
		*	pXmlNode  [in]   XML文件
		*	return    [out]  节点
		*/
		virtual INode*				iCreateAndInitializeNode(XMLNodePtr pXmlNode) = 0;

		/*
		*	通过节点参数库创建并且初始化节点
		*	tParam  [in]   节点参数
		*	return  [out]  节点
		*/
		virtual INode*				iCreateAndInitializeNode(NodeParamBase& tParam) = 0;
		
		/*
			创建动画控制器
		*/
		//virtual IAnimation*      iCreateAnimation() = 0;
		virtual IAnimation*		iCreateAnimation(AnimationParam& tParam) = 0;

		/*
			删除动画
		*/
		virtual void                 iDeleteAnimation(IAnimation* pAnimation) = 0;

		/*
		*	是否删除节点(多个节点)
		*	vecNode       [in]   所有需要删除的节点
		*	bUndoCommand  [in]   是否撤销删除命令(默认false)
		*	return        [out]  删除成功/删除失败
		*/
		virtual bool				iDeleteNode(std::vector<INode*>& vecNode, const bool& bUndoCommand = false) = 0;

		/*
		*	是否删除节点
		*	ppNode        [in]   需要删除的节点
		*	bUndoCommand  [in]   是否撤销删除命令(默认false)
		*	return        [out]  删除成功/删除失败
		*/
		virtual bool				iDeleteNode(INode** ppNode, const bool& bUndoCommand = false) = 0;

		virtual bool				iMergeGroup(const std::vector<INode*>, const std::string strGroupID, bool bSubScene) = 0;

		/*
		*	创建和初始化对象
		*	tParam  [in]   对象基本参数
		*	return  [out]  对象
		*/
		virtual IObject*			iCreateAndInitializeObject(ObjectParamBase& tParam) = 0;

		/*
		*	查找对象
		*	strID   [int]  对象ID
		*	return  [out]  对象
		*/
		virtual IObject*			iFindObject(const std::string& strID) = 0;

		/*
		*	是否删除对象
		*	strID   [in]   对象ID
		*	return  [out]  删除成功/删除失败
		*/
		virtual bool				iDeleteObject(const std::string& strID) = 0;

		/*
		*	获取场景视图
		*	strSceneViewID  [in]   场景视图ID
		*	return          [out]  场景视图 
		*/
		virtual IView*				iGetSceneView(const std::string& strSceneViewID) = 0;

		/*
		*	获取效果视图
		*	strSceneViewID  [in]   效果视图ID
		*	nViewType       [in]   效果视图类型
		*	return          [out]  效果视图
		*/
		virtual IView*				iGetEffectView(const std::string& strSceneViewID, const int& nViewType) = 0;

		/*
		*	获取当前视图
		*	return  [out]  当前视图
		*/
		virtual IView*				iGetCurrentView() = 0;

		/*
		*	获取当前效果视图
		*	nViewType  [in]   视图类型
		*	return     [out]  当前效果视图
		*/
		virtual IView*				iGetCurrentEffectView(const int& nViewType) = 0;

		/*
		*	设置当前效果视图可视
		*	nViewType  [in]  视图类型
		*	bVisible   [in]  视图是否可视
		*/
		virtual void				iSetCurrentEffectViewVisible(const int& nViewType, const bool& bVisible) = 0;

		/*
		*	切换场景
		*	SwitchViewParam  [in]  切换参数
		*/
		virtual void				iSwitchView(const SwitchViewParam& tParam) = 0;
		
		/*
		*	获取天空
		*	return  [out]  天空
		*/
		virtual ISky*				iGetSky() = 0;

		/*
		*	切换瓦片地图
		*	TiledMapProvider_e  [in]  切换参数
		*/
		virtual void				iSwitchTiledMapProvider(TiledMapProvider_e& eMapProvider) = 0;

		/*
		*	获取路径
		*	return  [out]  路径
		*/
		virtual PathLine*			iGetPathLine(const std::string& strID) = 0;
		virtual PathLine*			iGetPathLine(const int& nIndex) = 0;

		virtual int					iGetNodeType(const char* strType) = 0;

		virtual void				iSetUndoEnable(const bool& bEnable) = 0;
		virtual bool				iIsUndoEnable() = 0;

		virtual void				iMoveNode(const std::vector<INode*>& vecNode, const Vector3d& vAdd) = 0;
		virtual void				iRotateNode(const std::vector<INode*>& vecNode, const Vector3d& vAddAngle, const Vector3d& vCenter) = 0;
		virtual void				iScaleNode(const std::vector<INode*>& vecNode, const Vector3d& vAddScale) = 0;
		virtual void				iSpacingNode(const std::vector<INode*>& vecNode, const bool& bVertical) = 0;
		virtual void				iAlignNode(const Vector3d& vDir, const std::vector<INode*>& vecNode) = 0;

		virtual bool				iAbsorbNode(INode* pNode) = 0;
		virtual bool				iAbsorbNode(INode* pNode, Vector3d& vMoveOrigin) = 0;
		virtual void				iSetAbsorbScope(const float& fScope) = 0;
		virtual float				iGetAbsorbScope() = 0;

		virtual INode*			iGetCurrentSelectNode() = 0;
		virtual void				iGetSelectedNodeBySort(std::vector<INode*>& vecNode) = 0;
		virtual void				iClearSelect() = 0;

		virtual INode*			iGetCurrentHoverNode() = 0;
		virtual void				iClearHover() = 0;

		virtual void				iPushUndoCommand(IUndoCommand* pCommand) = 0;
		virtual void				iUndo() = 0;
		virtual void				iRedo() = 0;

		virtual void				iBuildStaticMergeMesh() = 0;
		/*
		*	根据Node 类型获取vector
		*/
		virtual void				iGetTypeNode(std::vector<INode*>& vecNode, NodeType_e eNodeType,INode* pParentNode = nullptr) = 0;

		virtual SceneMode_e iGetSceneMode() = 0;
	};
}
