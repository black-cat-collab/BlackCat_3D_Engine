#pragma once
namespace bc
{
	class IModel;
	class Triangle;
	typedef struct _sModelData_t
	{
		std::vector<IMesh*>		vecMesh;
		std::vector<BoundingBox> vecBoundingBox;
		std::string				strModelFile;
		std::string				strMaterialFile;
	}BCModelData;

	typedef void (*ModelCompileCallBack)(INT_PTR, IModel*);
	// 模型数据加载函数指针
	typedef bool(*ModelLoadFunc)(BCModelData& tModelData, IModel* pModel);
	// 模型数据保存函数指针
	typedef bool(*ModelSaveFunc)(const std::string& strModelFile, IModel* pModel);

	enum class ModelType_e
	{
		GeoModel,
		HcmModel,
		PolygonModel,
		FlyLineModel,
		MultiFlyLineModel,
		RoadModel,
		ShellModel,
		PlottingModel,
		ShellRootModel,
		BufferNodeModel,
		BIMRootModel,
		TIFRootModel,
		BlankModel,
	};

	enum class ModelLoadState_e
	{
		Initialize,
		WaitForRead,
		Reading,
		WaitForCompile,
		Loaded,
		FailForRead
	};

	class ModelParam
	{
	public:
		ModelType_e			nModelType;			// 模型类型
		std::string			strName;			// 模型文件名称
		std::string			strFile;			// 模型文件名称
		std::string			strMaterialFile;	// 材质文件名称
		int					nCompileSort;		// 编译排序 升序
		int					nCompileLevel;		// 编译级别
		bool                bCompileSlice;        // 分片编译
		std::vector<IMesh*>	vecMesh;			// 已有的Mesh
		int                 nCompileIndex;
		bool				bMerge;				// 是否合并
		IMesh*				pMergeMesh;			// 合并mesh
		ModelLoadFunc		pFunc;				// 模型生成函数
		bool				bWaitTask;			// 是否进入线程等待区域
		bool				bSaveFile;			// 是否需要保存文件
		bool            bExternalModel;
		void*		pParam;             //传输参数
		ModelParam() :
			nModelType(ModelType_e::GeoModel),
			nCompileSort(1),
			nCompileLevel(1),
			nCompileIndex(0),
			pFunc(nullptr),
			bMerge(false),
			bWaitTask(false),
			bSaveFile(false),
			bCompileSlice(false),
			pMergeMesh(nullptr),
			pParam(nullptr),
			bExternalModel(false)
		{

		}

		ModelParam& operator=(const ModelParam& rhs)
		{
			strName = rhs.strName;
			strFile = rhs.strFile;
			strMaterialFile = rhs.strMaterialFile;
			nCompileSort = rhs.nCompileSort;
			nCompileLevel = rhs.nCompileLevel;
			bCompileSlice = rhs.bCompileSlice;
			pFunc = rhs.pFunc;
			bMerge = rhs.bMerge;
			pMergeMesh = rhs.pMergeMesh;
			bWaitTask = rhs.bWaitTask;
			bSaveFile = rhs.bSaveFile;
			nCompileIndex = 0;
			pParam = rhs.pParam;
			bExternalModel = rhs.bExternalModel;
			for (size_t i = 0; i < rhs.vecMesh.size(); ++i)
			{
				vecMesh.push_back(rhs.vecMesh[i]);
			}

			return *this;
		}
	};
	
	class HcmModelParam : public ModelParam
	{
	public:
		int			nMshOffset;
		int			nMshZipLength;
		int			nMshLength;
		int			nTexOffset;
		int			nTexLength;

		HcmModelParam() : ModelParam()
		{
			nModelType = ModelType_e::HcmModel;

			nMshOffset = -1;
			nMshLength = -1;
			nTexOffset = -1;
			nTexLength = -1;
		}

		HcmModelParam& operator =(const HcmModelParam& rParam)
		{
			ModelParam::operator= (rParam);

			nMshOffset = rParam.nMshOffset;
			nMshLength = rParam.nMshLength;
			nTexOffset = rParam.nTexOffset;
			nTexLength = rParam.nTexLength;
			nMshZipLength = rParam.nMshZipLength;
			return *this;
		}
	};

	class PolygonModelParam : public ModelParam
	{
	public:
		std::vector<Vector3>	vecVertex;
		float					fUVRepeat;
		bool					bWithFence;
		float					fBaseHeight;
		float					fFenceHeight;
		bool                 bCalculateCenter;
		PolygonModelParam() : ModelParam()
		{
			nModelType = ModelType_e::PolygonModel;
			vecVertex.clear();
			fUVRepeat = -1.0;
			bWithFence = false;
			fBaseHeight = 0.0;
			fFenceHeight = 1.0;
			bCalculateCenter = false;
		}

		PolygonModelParam& operator =(const PolygonModelParam& rParam)
		{
			ModelParam::operator= (rParam);

			vecVertex.assign(rParam.vecVertex.begin(), rParam.vecVertex.end());
			fUVRepeat = rParam.fUVRepeat;
			fBaseHeight = rParam.fBaseHeight;
			bWithFence = rParam.bWithFence;
			fFenceHeight = rParam.fFenceHeight;
			bCalculateCenter = rParam.bCalculateCenter;
			return *this;
		}
	};

	class ShellModelParam : public ModelParam
	{
	public:

		std::vector<Triangle>   vecTriangle;
		Vector3 color;
		Vector3 origin;
		BoundingBox box;
		ShellModelParam() : ModelParam()
		{
			nModelType = ModelType_e::ShellModel;
			vecTriangle.clear();
		}

		ShellModelParam& operator =(const ShellModelParam& rParam)
		{
			ModelParam::operator= (rParam);
			color = rParam.color;
			origin = rParam.origin;
			box = rParam.box;
			vecTriangle.assign(rParam.vecTriangle.begin(), rParam.vecTriangle.end());
			return *this;
		}
	};

	class TIFRootModelParam : public ModelParam
	{
	public:

		float          fRadius;
		int             nMinHeight;
		Vector4d   vRect;
		std::vector<Vector3d> vecKeyPoint;
		ITIFNode* pNode;
		bool          bGenerateByPoints;
		TIFRootModelParam() : ModelParam()
		{
			nModelType = ModelType_e::TIFRootModel;
			fRadius = 1.0f;
			nMinHeight = 0;
			vRect = Vector4d(0.0f);
			vecKeyPoint.clear();
			pNode = nullptr;
			bGenerateByPoints = false;
		}

		TIFRootModelParam& operator =(const TIFRootModelParam& rParam)
		{
			ModelParam::operator= (rParam);
			fRadius = rParam.fRadius;
			nMinHeight = rParam.nMinHeight;
			vRect = rParam.vRect;
			pNode = rParam.pNode;
			vecKeyPoint = rParam.vecKeyPoint;
			bGenerateByPoints = rParam.bGenerateByPoints;
			return *this;
		}
	};

	class ShellRootModelParam : public ModelParam
	{
	public:

		struct ShellMeshParam
		{
			int32_t childIndex;
			int32_t meshIndex;
			int32_t offset;
			int32_t size;
			bool     update;
		};

		std::vector<ShellMeshParam> mVecShellMeshParam;
		std::vector<std::vector<Triangle>> mVecChildTriangles;
		std::vector<int2> mVecShellParam;
		std::vector<IShellNode*> mVecNodes;
		std::vector<INode*> mVecIsolNodes;
		BoundingBoxd box;
		bool  mIsolocation = false;
		bool  mIsOld = false;
		bool  mIsFromModel = false;
		std::vector<Vector3d> mVecIsolocationPoint;
		std::string strTexturePath;
		std::string strSaveTexturePath;
		ShellRootModelParam() : ModelParam()
		{
			nModelType = ModelType_e::ShellRootModel;
			mVecShellMeshParam.clear();
			mVecChildTriangles.clear();
			mVecShellParam.clear();
			mVecIsolNodes.clear();
			box.Clear();
			mIsolocation = false;
			mIsOld = false;
			mVecIsolocationPoint.clear();
			strTexturePath = "";
			strSaveTexturePath = "";
		}

		ShellRootModelParam& operator =(const ShellRootModelParam& rParam)
		{
			ModelParam::operator= (rParam);

			mVecNodes.assign(rParam.mVecNodes.begin(), rParam.mVecNodes.end());
			mVecShellMeshParam.assign(rParam.mVecShellMeshParam.begin(), rParam.mVecShellMeshParam.end());
			mVecChildTriangles.assign(rParam.mVecChildTriangles.begin(), rParam.mVecChildTriangles.end());
			mVecShellParam.assign(rParam.mVecShellParam.begin(), rParam.mVecShellParam.end());
			box = rParam.box;
			mIsolocation = rParam.mIsolocation;
			mVecIsolocationPoint.assign(rParam.mVecIsolocationPoint.begin(), rParam.mVecIsolocationPoint.end());
			mVecIsolNodes.assign(rParam.mVecIsolNodes.begin(), rParam.mVecIsolNodes.end());
			strTexturePath = rParam.strTexturePath;
			strSaveTexturePath = rParam.strSaveTexturePath;
			mIsOld = rParam.mIsOld;
			mIsFromModel = rParam.mIsFromModel;
			return *this;
		}
	};

	class BIMRootModelParam : public ModelParam
	{
	public:

		struct BIMMeshParam
		{
			int32_t childIndex;
			int32_t meshIndex;
			int32_t offset;
			int32_t size;
			bool     update;
		};

		std::string   strModelPath;
		IBIMNode* m_pBIMNodeRoot;
		std::vector<BIMMeshParam> mVecBIMMeshParam;
		std::vector<BoundingBox> mVecChildBoundingBox;
		std::vector<int2> mVecBIMParam;
		BoundingBox box;
		bool  mIsolocation = false;
		std::vector<Vector3> mVecIsolocationPoint;
		std::string strTexturePath;
		BIMRootModelParam() : ModelParam()
		{
			nModelType = ModelType_e::BIMRootModel;
			//mColorList.clear();
			//mOriginList.clear();
			mVecBIMMeshParam.clear();
			mVecChildBoundingBox.clear();
			mVecBIMParam.clear();
			box.Clear();
			mIsolocation = false;
			mVecIsolocationPoint.clear();
			strTexturePath = "";
		}

		BIMRootModelParam& operator =(const BIMRootModelParam& rParam)
		{
			ModelParam::operator= (rParam);

			//mColorList.assign(rParam.mColorList.begin(), rParam.mColorList.end());
			//mOriginList.assign(rParam.mOriginList.begin(), rParam.mOriginList.end());
			mVecBIMMeshParam.assign(rParam.mVecBIMMeshParam.begin(), rParam.mVecBIMMeshParam.end());
			mVecChildBoundingBox.assign(rParam.mVecChildBoundingBox.begin(), rParam.mVecChildBoundingBox.end());
			mVecBIMParam.assign(rParam.mVecBIMParam.begin(), rParam.mVecBIMParam.end());
			box = rParam.box;
			mIsolocation = rParam.mIsolocation;
			mVecIsolocationPoint.assign(rParam.mVecIsolocationPoint.begin(), rParam.mVecIsolocationPoint.end());
			strTexturePath = rParam.strTexturePath;
			strModelPath   = rParam.strModelPath;
			//mvecBIMNode.assign(rParam.begin(),)
			m_pBIMNodeRoot = rParam.m_pBIMNodeRoot;
			return *this;
		}
	};

	class PlottingModelParam : public ModelParam
	{
	public:
		std::vector<Triangle>   vecTriangle;

		PlottingModelParam() : ModelParam()
		{
			nModelType = ModelType_e::PlottingModel;
			vecTriangle.clear();
		}

		PlottingModelParam& operator =(const PlottingModelParam& rParam)
		{
			ModelParam::operator= (rParam);

			vecTriangle.assign(rParam.vecTriangle.begin(), rParam.vecTriangle.end());
			return *this;
		}
	};

	class FlyLineModelParam : public ModelParam
	{
	public:
		std::vector<Vector3>	vecVertex;
		int						nTubularSegment;
		int						nRadialSegment;
		float					fRadius;

		FlyLineModelParam() : ModelParam()
		{
			nModelType = ModelType_e::FlyLineModel;

			vecVertex.clear();
			nTubularSegment = 2;
			nRadialSegment = 4;
			fRadius = 1.0;
		}

		FlyLineModelParam& operator =(const FlyLineModelParam& rParam)
		{
			ModelParam::operator= (rParam);

			vecVertex.assign(rParam.vecVertex.begin(), rParam.vecVertex.end());
			nTubularSegment = rParam.nTubularSegment;
			nRadialSegment = rParam.nRadialSegment;
			fRadius = rParam.fRadius;

			return *this;
		}
	};

	class MultiFlyLineModelParam : public ModelParam
	{
	public:
		std::vector<Vector3>	vecVertex;
		int						nTubularSegment;
		int						nRadialSegment;
		float					fRadius;

		MultiFlyLineModelParam() : ModelParam()
		{
			nModelType = ModelType_e::FlyLineModel;

			vecVertex.clear();
			nTubularSegment = 2;
			nRadialSegment = 4;
			fRadius = 1.0;
		}

		MultiFlyLineModelParam& operator =(const MultiFlyLineModelParam& rParam)
		{
			ModelParam::operator= (rParam);

			vecVertex.assign(rParam.vecVertex.begin(), rParam.vecVertex.end());
			nTubularSegment = rParam.nTubularSegment;
			nRadialSegment = rParam.nRadialSegment;
			fRadius = rParam.fRadius;

			return *this;
		}
	};

	class BlankModelParam : public ModelParam
	{
	public:
		BoundingBox bbox;
		BlankModelParam()
		{
			nModelType = ModelType_e::BlankModel;
		}

		BlankModelParam& operator =(const BlankModelParam& rParam)
		{
			ModelParam::operator= (rParam);
			return *this;
		}
	};

	class RoadModelParam : public ModelParam
	{
	public:
		std::vector<std::vector<Vector3>>	vecVertex;
		std::vector<float>			vecWidth;
		std::vector<float>			vecStep;
		int						nCalcHeightType;
		float					fFixedHeight;
		std::string					strHeightFilePath;
		std::vector<INode*>		vecCollisionNode;
		float					fAdjustHeight;
		bool					bGlowFlash;
		float					fGlowFlashSpeed;
		int                    nNumArc;
		RoadModelParam()
		{
			nModelType = ModelType_e::RoadModel;

			vecVertex.clear();
			vecWidth.clear();
			vecStep.clear();
			nCalcHeightType = RoadParam::FIXED_HEIGHT;
			fFixedHeight = 0.0f;
			strHeightFilePath = "";
			vecCollisionNode.clear();
			fAdjustHeight = 0.0f;
			bGlowFlash = false;
			fGlowFlashSpeed = 0.0f;
			nNumArc = 4;
		}

		RoadModelParam& operator =(const RoadModelParam& rParam)
		{
			ModelParam::operator= (rParam);

			vecVertex = rParam.vecVertex;
			vecWidth = rParam.vecWidth;
			vecStep = rParam.vecStep;
			nCalcHeightType = rParam.nCalcHeightType;
			fFixedHeight = rParam.fFixedHeight;
			strHeightFilePath = rParam.strHeightFilePath;
			vecCollisionNode = rParam.vecCollisionNode;
			fAdjustHeight = rParam.fAdjustHeight;
			bGlowFlash = rParam.bGlowFlash;
			fGlowFlashSpeed = rParam.fGlowFlashSpeed;
			nNumArc = rParam.nNumArc;
			return *this;
		}
	};

	class BufferNodeModelParam : public ModelParam
	{
	public:
		BufferType                  eType;
		ClipType                     eClipType;
		RuleType                    eRuleType;
		std::vector<Vector3> vecPoint;
		float                           fRadius;
		Vector3 vOrigin;

		BufferNodeModelParam()
		{
			nModelType = ModelType_e::BufferNodeModel;
			eType = BufferType::Buffer_Point;
			eRuleType = RuleType::NonZero;
			eClipType = ClipType::Union;
			vOrigin = Vector3(0.0f);
			fRadius = 1.0f;
		}

		BufferNodeModelParam& operator =(const BufferNodeModelParam& rParam)
		{
			ModelParam::operator= (rParam);

			vecPoint = rParam.vecPoint;
			eType = rParam.eType;
			eClipType = rParam.eClipType;
			eRuleType = rParam.eRuleType;
			vOrigin = rParam.vOrigin;
			fRadius = rParam.fRadius;
			return *this;
		}
	};

	class VoxelModelParam : public ModelParam
	{
	public:
		IPagedNode* mpPagedNode{ nullptr };

		VoxelModelParam& operator =(const VoxelModelParam& rParam)
		{
			ModelParam::operator= (rParam);
		}
	};

	class IModel
	{
	public:
		/*
		*	初始化模型
		*	pParam  [in]  模型参数
		*/
		virtual void				iInit(ModelParam * pParam) = 0;
		
		/*
		*	获取模型名字
		*	return  [out]  模型名字
		*/
		virtual std::string			iGetName() = 0;
		
		/*
		*	获取模型参数
		*	return  [out]  模型参数
		*/
		virtual const ModelParam*	iGetModelParam() = 0;

		/*
		*	获取模型加载状态
		*	return  [out]  模型加载状态
		*/
		virtual uint				iGetLoadState() const = 0;
		virtual void		        iSetLoadState(const uint& nState) = 0;
		/*
		*	获取加载任务ID
		*	return  [out]  ID
		*/
		virtual size_t				iGetLoadTaskID() = 0;
		
		/*
		*	获取Mesh缓冲
		*	nCount  [in]	Mesh的个数
		*	return  [out]	首个Mesh
		*/
		virtual IMesh**				iGetMeshBuffer(int& nCount) = 0;

		/*
		*	获取合并Mesh缓冲
		*	return  [out]	Mesh
		*/
		virtual IMesh**				iGetMergeMeshBuffer() = 0;
		
		/*
		*	获取模型包围盒
		*	return  [out]  包围盒
		*/
		virtual const BoundingBox&	iGetBBox() = 0;
		
		/*
		*	添加编译回调函数
		*	pPtr	 [in]  节点指针
		*	pBefore  [in]  模型编译前的操作
		*	pAfter   [in]  模型编译后的操作
		*/
		virtual void				iAddCompileCallBack(INT_PTR pPtr, ModelCompileCallBack pBefore, ModelCompileCallBack pAfter) = 0;
		
		/*
		*	移除编译回调函数
		*	pPtr  [in]  节点指针
		*/
		virtual void				iRemoveCompileCallBack(INT_PTR pPtr) = 0;

		/*
		*	设置是否需要保存文件
		*	bSave	 [in]  是否需要保存文件
		*/
		virtual void				iSetSaveFile(const bool& bSave) = 0;

		/*
		*	是否需要保存文件
		*	return  [out]  是否需要保存文件
		*/
		virtual bool				iIsSaveFile() = 0;

		/*
		*	保存模型文件
		*	strFilePath  [in]  文件绝对路径
		*/
		virtual bool				iSaveModel(const std::string& strFilePath) = 0;

		/*
		*	保存材质文件
		*	strFilePath  [in]  文件绝对路径
		*/
		virtual bool				iSaveMaterial(const std::string& strFilePath) = 0;

		/*
			重新追加IMesh
		*/
		virtual void		        iLoadMesh(BCModelData& tModelData)= 0;

		/*
			清除 Model 中的Mesh
		*/
		virtual void                iClearMesh() = 0;

		/*
			IMesh重新加载脚本，载入材质，需要把 两个路径设置好。
		*/
		virtual bool				iLoadScript(BCModelData& tModelData) = 0;

		virtual void              iUpdateBBox() = 0;
	};

	enum MeshType_e
	{
		LUMP_UNKOWN = -1,
		LUMP_MESH,
		LUMP_ANI_MESH
	};

	class TW4MeshHeader
	{
	public:
		short		sVersion;
		short		sMeshCount;
		char		szName[64];
		TW4MeshHeader()
		{
			sVersion = 43;
			sMeshCount = 0;
			memset(szName, 0, sizeof(szName));
		}
	};

	class TW4MeshInfo
	{
	public:
		MeshType_e eType;
		int  nPosition;
		int  nZipLen;
		int  nUnZipLen;
		TW4MeshInfo& operator=(const TW4MeshInfo& tMesh)
		{
			nPosition = tMesh.nPosition;
			nZipLen = tMesh.nZipLen;
			nUnZipLen = tMesh.nUnZipLen;

			return *this;
		}

		TW4MeshInfo() :
			eType(MeshType_e::LUMP_UNKOWN), 
			nPosition(0), nZipLen(0) , nUnZipLen(0)
		{

		}

		int GetDataPos() 
		{ 
			return (nPosition + sizeof(TW4MeshInfo));
		}
	};
}
