#pragma once

namespace bc
{
	class InstanceData
	{
	public:
		INT_PTR		externParam;
		BoundingBoxd tBBox;
		Matrix4     mMatrix;
		bool		bVisible;
		float		fHideGreaterDistance;	// 大于于此距离隐藏，若不为0则启用，小于该距离内显示
		float		fHideLessDistance;	// 小于此距离隐藏，若不为0则启用，大于该距离内显示

	private:
		bool		bSelect;				//是否选中，如果启动选择效果则有效,true则显示选中的节点，false则显示自己

	public:
		InstanceData() :
			externParam(NULL),
			bVisible(true),
			mMatrix(Matrix4::IDENTITY),
			fHideGreaterDistance(0.0f),
			fHideLessDistance(0.0f),
			bSelect(false)
		{

		}

		InstanceData& operator = (const InstanceData& src)
		{
			externParam = src.externParam;
			bVisible = src.bVisible;
			mMatrix = src.mMatrix;
			fHideGreaterDistance = src.fHideGreaterDistance;
			fHideLessDistance = src.fHideLessDistance;
			bSelect = src.bSelect;

			return *this;
		}


		void SetSelect(bool bSel)
		{
			bSelect = bSel;
		}

		bool IsSelect()
		{
			return bSelect;
		}
	};

	class BoardInstanceData : public InstanceData
	{
	public:
		Vector3		vOrigin;
		Vector3		vTargetPosition;	// 目标物体位置
		Vector3		vAngle;
		float		fLength;
		float		fHeight;
		float		fWidth;
		float		fSize;
		float		fNormalSize;
		float		fSelectSize;
		std::string strID;
		std::string strUUID;
		bool		bCluster;		//是否被聚合（包括内部和外部）
		bool		bWithLine;
		bool		bRotateAroundTarget;
		Vector2		vDistanceToTarget;	// 与目标的距离
		bool		bOverTapHide;		//是否被重叠隐藏了
		Vector3		vClusterOrigin;		//聚合判断位置
		BoardInstanceData() : InstanceData(),
			fHeight(0), fWidth(0), fLength(0), fNormalSize(1.0),
			fSize(1.0f), fSelectSize(1.5f), vTargetPosition(Vector3(0, 0, 0)),
			bCluster(false), bWithLine(false), bRotateAroundTarget(true), vDistanceToTarget(Vector2(0, 0)),
			bOverTapHide(false), vClusterOrigin(Vector3(0,0,0))
		{

		}

		BoardInstanceData& operator = (const BoardInstanceData& src)
		{
			InstanceData::operator=(src);

			vOrigin = src.vOrigin;
			vAngle = src.vAngle;
			fLength = src.fLength;
			fHeight = src.fHeight;
			fWidth = src.fWidth;
			fNormalSize = src.fNormalSize;
			fSize = src.fSize;
			fSelectSize = src.fSelectSize;
			vTargetPosition = src.vTargetPosition;
			bCluster = src.bCluster;
			bWithLine = src.bWithLine;
			bRotateAroundTarget = src.bRotateAroundTarget;
			vDistanceToTarget = src.vDistanceToTarget;
			bOverTapHide = src.bOverTapHide;
			vClusterOrigin = src.vClusterOrigin;
			strID = src.strID;
			strUUID = src.strUUID;

			return *this;
		}
	};

	class BoardInstanceParam : public BoardParam
	{
	public:
		BoardInstanceParam() : BoardParam()
		{
			nNodeType = NODE_BOARD_INSTANCE;
		}

		BoardInstanceParam& operator = (const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		BoardInstanceParam& operator = (const BoardInstanceParam& src)
		{
			BoardParam::operator=(src);

			return *this;
		}
	};

	class UIBoardInstanceData : public InstanceData
	{
	public:
		Vector3		vOrigin;
		Vector3		vTargetPosition;	// 目标物体位置
		Vector3		vAngle;
		float		fHeight;
		float		fWidth;
		float		fLength;
		float		fSize;
		std::string strID;
		std::string strUUID;
		UIBoardInstanceData() : InstanceData(),
			fHeight(0), fWidth(0), fLength(1.0f), fSize(1.0f),
			vAngle(Vector3(0.0f)), vOrigin(Vector3(0.0f)),
			vTargetPosition(Vector3(0.0f))
		{

		}
	};

	class UIBoardInstanceParam : public UIBoardParam
	{
	public:
		UIBoardInstanceParam() : UIBoardParam()
		{
			nNodeType = NODE_UI_BOARD_INSTANCE;
		}

		UIBoardInstanceParam& operator = (const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		UIBoardInstanceParam& operator = (const UIBoardInstanceParam& src)
		{
			UIBoardParam::operator=(src);

			return *this;
		}
	};

	class RiseParticleInstanceData : public InstanceData
	{
	public:
		Vector3		vOrigin;
		Vector3		vAngle;
		float		fHeight;
		float		fWidth;
		float		fSize;
		RiseParticleInstanceData() : InstanceData(),
			vOrigin(Vector3(0.0)), vAngle(Vector3(0.0)),
			fHeight(0), fWidth(0), fSize(1.0f)
		{

		}
	};

	class RiseParticleInstanceParam : public BoardParam
	{
	public:
		int		nCount;
		Vector2	vDistanceRange;
		float	fFlyHeight;
		float	fBottomOffset;
		float	fFlySpeedMax;
		float	fFlySpeedMin;
		Vector3 vFlyDirection;
		RiseParticleInstanceParam() : BoardParam()
		{
			nNodeType = NODE_RISE_PARTICLE_INSTANCE;
			direction = BoardDirect_e::BillBoard;
			vDistanceRange = Vector2(1000.0, 1000.0);
			fFlyHeight = 1000.0;
			fFlySpeedMax = 120.0;
			fFlySpeedMin = 60.0;
			nCount = 500;
			fBottomOffset = 20;
			vFlyDirection = Vector3(0, 0, 1);
		}

		RiseParticleInstanceParam& operator = (const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		RiseParticleInstanceParam& operator = (const RiseParticleInstanceParam& src)
		{
			BoardParam::operator=(src);
			nCount = src.nCount;
			vDistanceRange = src.vDistanceRange;
			fFlyHeight = src.fFlyHeight;
			fFlySpeedMax = src.fFlySpeedMax;
			fFlySpeedMin = src.fFlySpeedMin;
			vFlyDirection = src.vFlyDirection;
			fBottomOffset = src.fBottomOffset;
			return *this;
		}
	};

	class IInstance : public IComponent
	{
	public:
		virtual int					iGetComponentType() { return COMPONENT_TYPE_INSTANCE; }

		/*
		*	添加实例
		*	vPos    [in]  实例位置
		*	vAngle  [in]  实例角度
		*/
		virtual void				iAddInstance(const Vector3& vPos, const Vector3& vAngle) = 0;

		/*
		*	添加实例
		*	mMat  [in]  移动矩阵
		*/
		virtual void				iAddInstance(const Matrix4& mMat) = 0;
		/*
		*	添加实例
		*	tData    [in]  实例数据
		*/
		virtual void				iAddInstance(const InstanceData& tData) = 0;

		/*
		*	添加board
		*	tData  [in]  board实例数据
		*/
		virtual void				iAddBoard(const BoardInstanceData& tData) = 0;

		/*
		*	添加实例
		*	tData    [in]  实例数据
		*/
		virtual void				iRefreshInstancedData(const std::vector<BoardInstanceData*> &vecInstanceData) = 0;

		/*
		*	获取board参数
		*	return  [out]  board参数
		*/
		virtual BoardInstanceParam*	iGetBoardParam() = 0;

		/*
		*	获取选择索引
		*	return  [out]  实例索引值
		*/
		virtual int					iGetSelectIndex() = 0;

		/*
		*	清空选择索引
		*/
		virtual void				iClearSelectIndex() = 0;

		/*
		*	设置选择索引
		*	return  [void]  
		*/
		virtual void				iSetSelectIndex(const int& nIndex) = 0;

		/*
		*	获取选择数据
		*	return  [out]  实例数据
		*/
		virtual InstanceData*		iGetSelectInstanceData() = 0;

		/*
		*	清空数据
		*	return  [void]  
		*/
		virtual void				iClearInstanceData() = 0;

		/*
		*	获取实例数量
		*	return  [out]  获取实例数量
		*/
		virtual size_t				iGetInstanceCount() = 0;

		/*
		*	获取实例数据
		*	return  [in]  用vector容器存储所有的实例数据
		*/
		virtual std::vector<InstanceData*>& iGetInstancedData() = 0;

		/*
		*	设置位置
		*	vOrigin  [in]  位置
		* 	nIndex  [in]  索引
		*/
		virtual void				iSetOrigin(const Vector3d& vOrigin, const int& nIndex) = 0;

		/*
		*	设置角度
		*	vAngles  [in]  角度
		* 	nIndex  [in]  索引
		*/
		virtual void				iSetAngles(const Vector3d& vAngles, const int& nIndex) = 0;


		/*
		*	设置面板材质
		*	strImage  [in]  背景图片
		*/
		virtual void				iSetMaterial(const std::string& strImage) = 0;;

		
	};
}

