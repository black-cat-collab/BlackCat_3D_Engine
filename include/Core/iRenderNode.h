#pragma once

namespace bc
{
	typedef void(*CompileCompleteFunc)(ISystemAPI* pSystemAPI, INode* pNode);
	typedef void(*ClickFunc)(ISystemAPI* pSystemAPI, INode* pNode, const MouseButton_e& eButton, INT_PTR pExterParam, void* pUser);
	typedef void(*HoverFunc)(ISystemAPI* pSystemAPI, INode* pNode, const bool& bHover, INT_PTR pExterParam, void* pUser);

	typedef enum NodeMovability_e
	{
		NODE_MOVABILITY_DYNAMIC,
		NODE_MOVABILITY_STATIC,
		NODE_MOVABILITY_STATIONARY
	}NodeMovability;

	typedef enum NodeLoadType_e
	{
		NODE_LOAD_DYNAMIC,
		NODE_LOAD_STATIC
	}NodeLoadType;

	class IntersectSet
	{
	public:
		IntersectSet() :
			pNode(nullptr),
			pMesh(nullptr),
			pObject(nullptr),
			fDistance(FLT_MAX),
			vCrossNormal(Vector3d(0.0f)),
			vCrossPoint(Vector3d(0.0f))
		{

		}

		IntersectSet& operator = (const IntersectSet& rhs)
		{
			fDistance = rhs.fDistance;
			vCrossPoint = rhs.vCrossPoint;
			vCrossNormal = rhs.vCrossNormal;
			pNode = rhs.pNode;
			pMesh = rhs.pMesh;
			pObject = rhs.pObject;
			return  *this;
		}

	public:
		double		fDistance;
		Vector3d		vCrossNormal;
		Vector3d     vCrossPoint;
		INode*		pNode;
		IMesh*		pMesh;
	    void*	 pObject;
	};


	//class IntersectSetd
	//{
	//public:
	//	IntersectSetd() :
	//		pNode(nullptr),
	//		pMesh(nullptr),
	//		pObject(nullptr),
	//		fDistance(FLT_MAX),
	//		vCrossNormal(Vector3(0.0f)),
	//		vCrossPoint(Vector3(0.0f))
	//	{

	//	}

	//	IntersectSetd& operator = (const IntersectSet& rhs)
	//	{
	//		fDistance = rhs.fDistance;
	//		vCrossPoint = rhs.vCrossPoint;
	//		vCrossNormal = rhs.vCrossNormal;
	//		pNode = rhs.pNode;
	//		pMesh = rhs.pMesh;
	//		pObject = rhs.pObject;
	//		return  *this;
	//	}

	//public:
	//	double		fDistance;
	//	Vector3d		vCrossNormal;
	//	Vector3d     vCrossPoint;
	//	INode* pNode;
	//	IMesh* pMesh;
	//	void* pObject;
	//};

	class IRenderNode : public IComponent
	{
	public:
		/*
		 *	获取部件类型
		 *	return		[out]	渲染节点
		 */
		virtual int					iGetComponentType() { return COMPONENT_TYPE_RENDER_NODE; }

		/*
		 *	获取模型文件名
		 *	return		[out]	文件名
		 */
		virtual std::string			iGetModelFile() = 0;

		/*
		 *	设置模型文件名
		 *	strFileName	[in]	文件名
		 */
		virtual void				iSetModelFile(const std::string& strFileName) = 0;

		virtual void              iSetExternalModel(bool b) = 0;
		/*
		 *	获取材质文件名
		 *	return		[out]	文件名
		 */
		virtual std::string			iGetMaterialFile() = 0;

		/*
		 *	设置材质文件名
		 *	strFileName	[in]	文件名
		 */
		virtual void				iSetMaterialFile(const std::string& strFileName) = 0;

		/*
		 *	是否添加到相机渲染队列
		 *	return		[out]	是否添加
		 */
		virtual bool				iIsAddCameraRenderQueue() = 0;

		/*
		 *	获取当前选中面
		 *	return		[out]	面
		 */
		virtual IMesh*				iGetSelectMesh() = 0;

		/*
		 *	设置当前选中面
		 *	pMesh		[in]	面
		 */
		virtual void				iSetSelectMesh(IMesh* pMesh) = 0;

		/*
		 *	获取模型
		 *	return		[out]	模型
		 */
		virtual IModel*				iGetModel() = 0;

		/*
		 *	获取网格
		 *	vecMesh		[in]	网格组
		 */
		virtual void				iGetMesh(std::vector<IMesh*>& vecMesh) = 0;

		/*
		 *	获取材质
		 *	vecMaterial	[in]	材质组
		 */
		virtual void				iGetMaterial(std::vector<IMaterial*>& vecMaterial) = 0;

		/*
		 *	设置能否点击
		 *	bEnable		[in]	能否点击
		 */
		virtual void				iSetClickEnable(const bool& bEnable) = 0;

		/*
		 *	能否点击
		 *	return		[out]	能否点击
		 */
		virtual bool				iIsClickEnable() = 0;

		/*
		 *	是否被悬停
		 *	return		[out]	是否被悬停
		 */
		virtual bool				iIsHovered() = 0;

		/*
		 *	设置能否悬停
		 *	bEnable		[in]	能否悬浮
		 */
		virtual void				iSetHoverEnable(const bool& bEnable) = 0;

		/*
		 *	能否悬停
		 *	return		[out]	能否悬浮
		 */
		virtual bool				iIsHoverEnable() = 0;

		/*
		 *	设置悬停颜色
		 *	vColor		[in]	悬停颜色
		 */
		virtual void				iSetHoverColor(const Vector4& vColor) = 0;

		/*
		 *	是否被选中
		 *	return		[out]	是否被选中
		 */
		virtual bool				iIsSelected() = 0;

		/*
		 *	设置能否选中
		 *	bEnable		[in]	能否选中
		 */
		virtual void				iSetSelectEnable(const bool& bEnable) = 0;

		/*
		 *	能否被选中
		 *	return		[out]	能否被选中
		 */
		virtual bool				iIsSelectEnable() = 0;

		/*
		 *	设置选中颜色
		 *	vColor		[in]	选中颜色
		 */
		virtual void				iSetSelectColor(const Vector4& vColor) = 0;

		/*
		 *	是否被check
		 *	return		[out]	是否被check
		 */
		virtual bool				iIsChecked() = 0;


		/*
		 *	设置能否check
		 *	bEnable		[in]	能否check
		 */
		virtual void				iSetCheckEnable(const bool& bEnable) = 0;

		/*
		 *	能否被check
		 *	return		[out]	能否被check
		 */
		virtual bool				iIsCheckEnable() = 0;

		/*
		 *	设置只显示颜色，不显示原本材质
		 *  bChange		[in]	是否启用
		 *	vColor		[in]	颜色
		 */
		virtual void				iChangeColor(const bool& bChange, const Vector4& vColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f)) = 0;

		/*
		 *	是否设置颜色
		 *  return		[out]	是否启用
		 */
		virtual bool				iIsChangeColor() = 0;

		/*
		 *	设置材质颜色
		 *  bEnable		[in]	是否启用
		 *	vColor		[in]	材质颜色
		 */
		virtual void				iSetMaterialColor(const bool& bEnable, const Vector4& vColor) = 0;

		/*
		 *	是否相交
		 *	si			[in]	坐标
		 *	vecSet		[in]	存储相交对象
		 *	bPick		[in]	是否选中
		 */
		virtual void				iIntersect(const SegmentInstrect& si, std::vector<IntersectSet>& vecSet, const bool& bPick = false) = 0;
		
		

		/*
		 *	是否相交
		 *	si			[in]	坐标
		 *	vecSet		[in]	存储相交对象
		 *	bPick		[in]	是否选中 双精度
		 */
	//	virtual void				iIntersect(const SegmentInstrect& si, std::vector<IntersectSetd>& vecSet, const bool& bPick = false) = 0;
		
		/*
		 *	获取渲染排序
		 *	return		[out]	渲染排序
		 */
		virtual int					iGetRenderSort() const = 0;

		/*
		 *	设置渲染排序
		 *	nSort		[in]	渲染排序
		 */
		virtual void				iSetRenderSort(const int& nSort) = 0;

		/*
		 *	获取离相机的距离
		 *	return		[out]	距离
		 */
		virtual float				iGetDistanceToCamera() const = 0;

		/*
		 *	注册点击函数
		 *	pFunc		[in]	点击功能
		 *  pUser		[in]	使用者
		 */
		virtual void				iRegistClickFunc(ClickFunc pFunc, void* pUser) = 0;

		virtual void				iClearClickFunc() = 0;

		/*
		 *	设置参与视频融合
		 *	bFuse		[in]	是否融合
		 */
		virtual void				iSetVideoFusion(const bool& bFuse) = 0;

		/*
		 *	是否参与视频融合
		 *  return		[out]	是否融合
		 */
		virtual bool				iIsVideoFusion() = 0;

		/*
		 *	设置参与平面反射
		 *	bRender		[in]	是否反射
		 */
		virtual void				iSetRenderInPlaneReflect(const bool& bRender) = 0;

		/*
		 *	是否参与视频融合
		 *  return		[out]	是否反射
		 */
		virtual bool				iIsRenderInPlaneReflect() = 0;

		/*
		 *	设置编译比重
		 *	nWeight		[in]		比重
		 */
		virtual void				iSetCompileWeight(const int& nWeight) = 0;

		/*
		 *	获取编译比重
		 *	return		[out]	比重
		 */
		virtual int					iGetCompileWeight() = 0;

		/*
		 *	设置编译完成功能
		 *	pFunc		[in]		功能
		 */
		virtual void				iSetCompileCompleteFunc(CompileCompleteFunc pFunc) = 0;

		/*
		 *	是否产生辉光
		 *	return		[out]	是否产生辉光
		 */
		virtual bool				iIsGlow() = 0;

		virtual void				iSetMovability(const NodeMovability& eType) = 0;

		virtual NodeMovability		iGetMovability() = 0;

		virtual void				iSetLoadType(const NodeLoadType& eType) = 0;

		virtual NodeLoadType		iGetLoadType() = 0;

		virtual void				iSetHeatMapLayer(const int& nLayer) = 0;

		virtual void				iSetHeatMapMode(const HeatMapColorMode_e& eMode) = 0;

		virtual void				iSetHeatMapAlphaThreshold(const float& fLow, const float& fHigh) = 0;

		virtual bool				iSaveModel(const std::string& strModelFileName = "", 
			const std::string& strMaterialFileName = "") = 0;

		virtual void				iGetTriangle(std::vector<Triangle>& vecTriangle, bool hasuv1 = false) = 0;

		virtual void				iReplaceMesh(std::vector<IMesh*>& vecMeshSrc, std::vector<IMesh*>& vecMeshDst) = 0;

		virtual void              iGenerateChildMesh() = 0;
	
		virtual bool                iUpdateStaticMesh(const char* filePath) = 0;

		virtual IRenderNode*		iCopy() = 0;
	};
}

