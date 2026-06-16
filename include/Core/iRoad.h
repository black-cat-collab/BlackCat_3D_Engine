#pragma once

namespace bc
{
	enum RoadType
	{
		Road_Image,
		Road_Line,
		Road_Point
	};

		class RoadParam : public NodeParamBase
	{
	public:
		enum CalcHeightType
		{
			FIXED_HEIGHT = 0,		// 固定高度
			ORIGINAL_HEIGHT,		// 顶点原始高度
			CALC_WITH_MAP,			// 根据图计算
			COLL_WITH_NODE			// 根据物体碰撞计算
		};

		std::vector<std::vector<Vector3>>	vecVertex;			// 输入顶点
		std::string					strImage;			// 道路贴图
		std::string                   strurl;
		std::vector<float>			vecWidth;			// 道路宽度
		std::vector<float>			vecStep;			// 道路步长

		int						nCalcHeightType;	// 计算高度类型

		float					fFixedHeight;		// 固定高度

		std::string					strHeightFilePath;	// 场景高度图路径

		std::vector<INode*>			vecCollisionNode;	// 道路高度相交模型

		float					fAdjustHeight;		// 高度计算后的调整

		bool					bGlow;				// 是否开启辉光
		float					fGlowIntensity;		// 辉光强度
		bool					bGlowFlash;			//是否辉光闪烁
		float					fGlowFlashSpeed;	//辉光闪烁速度
		bool					bAnimation;			// 贴图是否流动
		float					fAnimationSpeed;	// 贴图流动速度	
		bool					bDepthTest;			// 是否开启深度测试
		bool					bIntersectEnable;	// 是否计算相交
		bool                 bIsTextureFromNet;
		std::string					strModelName;		// Msh文件名

		Vector4           vLineColor;
		Vector4           vPointColor;
		float                fPointSize;
		float                fLineWidth;
		RoadType       eType;
		int                   nNumArc;
		RoadParam() : NodeParamBase()
		{
			nVersion = 1;			//标识版本为1，解析上跟老的做区分
			nNodeType = NODE_ROAD;
			bDepthTest = true;
			vecVertex.clear();
			strImage = "";
			vecWidth.clear();
			vecStep.clear();
			fGlowIntensity = 5.0f;
			nCalcHeightType = ORIGINAL_HEIGHT;

			fFixedHeight = 0.0f;

			strHeightFilePath = "";	
			fAdjustHeight = 0.0f;

			vecCollisionNode.clear();

			bGlow = false;
			bGlowFlash = false;
			fGlowFlashSpeed = 0.0f;

			bAnimation = false;
			fAnimationSpeed = 0.0f;

			bIntersectEnable = false;

			vLineColor = Vector4(1.0);
			vPointColor = Vector4(1.0);
			fPointSize = 1.0f;
			fLineWidth = 1.0f;
			eType = RoadType::Road_Image;
			bIsTextureFromNet = false;
			nNumArc = 4;
		}

		RoadParam& operator=(const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		RoadParam& operator = (const RoadParam& src)
		{
			NodeParamBase::operator=(src);

			vecVertex = src.vecVertex;
			strImage = src.strImage;
			vecWidth = src.vecWidth;
			vecStep = src.vecStep;
			bDepthTest = src.bDepthTest;
			nCalcHeightType = src.nCalcHeightType;

			fFixedHeight = src.fFixedHeight;

			strHeightFilePath = src.strHeightFilePath;
			fAdjustHeight = src.fAdjustHeight;

			vecCollisionNode = src.vecCollisionNode;

			bGlow = src.bGlow;
			fGlowIntensity = src.fGlowIntensity;
			bGlowFlash = src.bGlowFlash;
			fGlowFlashSpeed = src.fGlowFlashSpeed;
			bAnimation = src.bAnimation;
			fAnimationSpeed = src.fAnimationSpeed;
			strModelName = src.strModelName;
			bIntersectEnable = src.bIntersectEnable;

			vLineColor = src.vLineColor;
			vPointColor = src.vPointColor;
			fPointSize = src.fPointSize;
			fLineWidth = src.fLineWidth;
			eType = src.eType;
			bIsTextureFromNet = src.bIsTextureFromNet;
			strurl = src.strurl;
			nNumArc = src.nNumArc;
			return *this;
		}
	};

	class IRoad : public IComponent
	{

	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_ROAD; }

		virtual void		iSetMaterialTexturePath(const std::string& strImage, bool isFromNet = false, const std::string& savePath = "") = 0;

		virtual void iAddPoint(const Vector3& point) = 0;
		virtual void iRemovePoint(const int32_t& index) = 0;
		virtual void iUpdatePoint(const int32_t& index, const Vector3& point) = 0;
		virtual void iGetControlPoints(std::vector<std::vector<Vector3>>& outputs) = 0;
		virtual void iSetRoadType(RoadType type) = 0;
		virtual void iSetEditor(bool b) = 0;
		virtual void iGenerate() = 0;
	};
} // namespace bc


