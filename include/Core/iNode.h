#pragma once

#include <string>

#define	RENDER_SORT_DEFAULT		10
#define RENDER_SORT_BLEND		20
#define RENDER_SORT_SKY			20
#define	RENDER_SORT_BOARD		200

namespace bc
{
	typedef void(*FrameUpdateFunc)(ISystemAPI* pSystemAPI, INode* pNode, void* pUser);

	enum NodeType_e
	{
		NODE_UNKNOWN = 0,						// 未知
		NODE_GROUP,								// 群组
		NODE_GEO,
		NODE_INSTANCE,							// 实例
		NODE_CAMERA,							// 相机
		NODE_BASE_CAMERA,						// 基础相机
		NODE_MANIPULATOR,						// 操作者
		NODE_ORTHO_MANIPULATOR,					// 正射投影操作者
		NODE_INERTIA_MANIPULATOR,				// 惯性移动操作者
		NODE_PROJECT_INERTIA_MANIPULATOR,		// 自定义惯性操作者
		NODE_PROGRESS_BAR,						// 进度条
		NODE_POLYGON,							// 多边形
		NODE_FLYLINE,							// 飞线
		NODE_MULTI_FLYLINE,						// 多段飞线
		NODE_ROAD,								// 道路
		NODE_MEASURE_DISTANCE,					// 测量距离
		NODE_MEASURE_AREA,						// 测量面积
		NODE_LOCUS_LINE,
		NODE_LIGHT,								// 光
		NODE_PAGED_ROOT,						// 倾斜摄影根节点
		NODE_PAGED_NODE,						// 倾斜摄影节点
		NODE_TILED_ROOT,						// 瓦片根节点
		NODE_TILED_NODE,						// 瓦片节点
		NODE_RENDER_FRUSTUM,					// 渲染视锥体
		NODE_VIDEO_FUSION,						// 视频融合节点
		NODE_BOARD,								// 面板
		NODE_FONTBOARD,							// 字体板
		NODE_FONTBOARD_WITH_BUTTON,				// 字体板带按钮
		NODE_BOARD_WITH_FONTBOARD,				// 面板带字体板
		NODE_VIDEO_BOARD,						// 视频面板
		NODE_VIDEO_BOARD_WITH_BUTTON,			// 视频面板带按钮
		NODE_VIDEO_ROUTE_BOARD,					// 视频路径面板
		NODE_VIDEO_ROUTE,						// 视频路径
		NODE_BOARD_ROUTE,
		NODE_BOARD_INSTANCE,					// 面板实例
		NODE_RISE_PARTICLE_INSTANCE,			// 上升粒子
		NODE_SPOT_LIGHT_BOARD,
		NODE_LIGHT_BEAM,						// 光束
		NODE_DYNAMIC_LOAD,						// 动态加载
		NODE_CUBE_REFLECT,						// 球型反射
		NODE_IRRADIANCE,						// 辐照度
		NODE_SH_VOLUME,							// 球谐光照
		NODE_PARTICLE,							// 粒子
		NODE_MOVE_AXIS,							// 移动轴
		NODE_MOVE_PLANE,						// 移动平面
		NODE_MOVE_TOOL,							// 移动工具
		NODE_ROTATE_PLANE,						// 旋转平面
		NODE_ROTATE_TOOL,						// 旋转工具
		NODE_SCALE_AXIS,						// 缩放轴
		NODE_SCALE_PLANE,						// 缩放平面
		NODE_SCALE_TOOL,						// 缩放工具
		NODE_COORDINATE_AXIS,					// 场景坐标示意
		NODE_GRID,								// 网格
		NODE_USER_INTERFACE,					// UI
		NODE_UI_WIDGET,							// UI窗口
		NODE_UI_BUTTON,							// UI按钮
		NODE_UI_VIDEO,							// UI视频
		NODE_UI_BOARD,							// UI面板
		NODE_UI_BOARD_INSTANCE,					// UI面板实例
		NODE_ISOLATED,					  // 单体化
		NODE_BUILDING_SHELL,
		NODE_SHELL,								// 外框
		NODE_FUSION,                              //视频融合
		NODE_FUSION_CAMERA,                       //视频融合调整阶段Node
		NODE_DRAW_LINE,
		NODE_GEO_VIDEO,								// 视频模型对象
		NODE_PLOTTING,                      // 标绘
		NODE_SHELL_GROUP,
		NODE_BUFFER,
		NODE_ACGIS_INFO,
		NODE_ENERGY,
		NODE_POLYGON_SELECTOR,
		NODE_BIM_NODE,
		NODE_BIM_ROOT,
		NODE_TIF_ROOT,
		NODE_BLANK_MODEL,
		NODE_VOXEL_ROOT,
		NODE_TYPE_END					  // 结束
	};

	// 多细节层次范围模式
	enum LODRangeMode
	{
		HC_DISTANCE_FROM_EYE_POINT = 0,				// 离视点的距离
		HC_PIXEL_SIZE_ON_SCREEN						// 屏幕上的像素大小
	};

	// 坐标平面
	typedef enum CoordPlane_e
	{
		COORD_PLANE_XOY,						// XOY平面
		COORD_PLANE_XOZ,						// XOZ平面
		COORD_PLANE_YOZ							// YOZ平面
	}CoordPlane;

	// 面板正面
	typedef enum BoardDirect_e
	{
		NormalDirect = 0,						// 标准正面
		BillBoard,								// 公告板
		FaceToCamera,							// 面向相机
		Rotation								// 旋转
	}BoardDirect;

	typedef enum LineStyle_e
	{
		LINE_STYLE_LINES,
		LINE_STYLE_STRIP,
		LINE_STYLE_LOOP,
		LINE_STYLE_DOTTED
	}LineStyle;

	typedef enum SDFTextFontWeight_e
	{
		SDF_FONT_WEIGHT_REGULAR = 0,
		SDF_FONT_WEIGHT_LIGHT,
		SDF_FONT_WEIGHT_MEDIUM,
		SDF_FONT_WEIGHT_BOLD,
		SDF_FONT_WEIGHT_HEAVY
	}SDFTextFontWeight;

	typedef enum SDFTextFontColor_e
	{
		SDF_FONT_COLOR_PURE = 0,
		SDF_FONT_COLOR_GRADIENT
	}SDFTextFontColor;

	class AddCameraRenderQueueParam
	{
	public:
		AddCameraRenderQueueParam()
		{
			bCullBox = true;
			bFrameUpdate = false;
			bRecursion = true;
			nCullFrustumFace = 5;
		}

		virtual ~AddCameraRenderQueueParam() {}

	public:
		bool	bFrameUpdate;			// 是否进行更新数据
		bool	bCullBox;				// 是否计算视锥裁剪
		int		nCullFrustumFace;		// 视锥剔除时视锥的面数
		bool	bRecursion;				// 是否递归子节点
	};

	// TextShow 用于FontBoard
	class TextShow
	{
	public:
		int				nLineMaxLength; // 每行最大长度
		int				nMaxLine;		// 做大行数
		Vector2			vTextPos;		// 文字位置 
		std::string		strText;		// 文字
		Vector2			vTextSize;		// 文字大小
		Vector2			vTextRange;		// 文字范围
		Vector4			vColor;			// 文字颜色 
		float			fColorScale;	// 颜色缩放
		std::string		strID;			// ID，用于查找
		std::string		strFlagName;	// 标记，用于查找
		bool			bAdjustWidth;	//是否宽度根据文字进行缩放
		bool			bWidthCenter;	//是否宽度居中
		bool			bHeightCenter;	//是否居中
		std::string		strDescription;	//描述

		TextShow()
		{
			strText = "";
			vTextRange = Vector2(0);
			vTextPos = Vector2(0);
			vTextSize = Vector2(20.0);
			vColor = Vector4(1.0);
			fColorScale = 1.0;
			bAdjustWidth = false;
			bWidthCenter = false;
			nLineMaxLength = 0;
			nMaxLine = 0;
			bHeightCenter = false;
		}

		inline TextShow& operator = (const TextShow& rhs)
		{
			strText = rhs.strText;
			vTextPos = rhs.vTextPos;
			vTextSize = rhs.vTextSize;
			vTextRange = rhs.vTextRange;
			vColor = rhs.vColor;
			fColorScale = rhs.fColorScale;
			strID = rhs.strID;
			strFlagName = rhs.strFlagName;
			bAdjustWidth = rhs.bAdjustWidth;
			bWidthCenter = rhs.bWidthCenter;
			nLineMaxLength = rhs.nLineMaxLength;
			nMaxLine = rhs.nMaxLine;
			bHeightCenter = rhs.bHeightCenter;
			strDescription = rhs.strDescription;

			return *this;
		}

		inline bool operator ==(const TextShow& rhs) const
		{
			if (strText != rhs.strText ||
				vTextPos != rhs.vTextPos ||
				vTextSize != rhs.vTextSize ||
				vTextRange != rhs.vTextRange ||
				vColor != rhs.vColor ||
				fColorScale != rhs.fColorScale)
			{
				return false;
			}

			return true;
		}

		inline bool operator !=(const TextShow& rhs) const
		{
			return !(*this == rhs);
		}

		float GetTextWidth()
		{
			if (strText.empty())
			{
				return 0.0;
			}

			std::vector<std::string> vecLineText;
			SplitStringBySpecial(strText, vecLineText, "\n");
			float fMaxLength = 0.0;
			for (size_t i = 0; i < vecLineText.size(); ++i)
			{
				float fTextLength = AsciiNum(vecLineText[i].c_str()) / 2.0;
				if (fTextLength > fMaxLength)
				{
					fMaxLength = fTextLength;
				}
			}

			return fMaxLength * vTextSize.x;
		}

		float GetTextHeight()
		{
			if (strText.empty())
			{
				return 0.0;
			}

			std::vector<std::string> vecLineText;
			SplitStringBySpecial(strText, vecLineText, "\n");
			return vecLineText.size() * vTextSize.y;
		}

	};

	class SDFTextShape
	{
	public:
		std::string		strText;			// 文字

		Vector2			vTextPos;			// 文字起始位置
		Vector2			vTextRange;			// 文字宽高范围
		float			fBaseLine;			// 文字基准线

		Vector2			vFontSize;			// 文字大小
		float			fFontSpace;			// 文字间隔

		int				nFontWeight;		// 文字重量

		SDFTextShape()
		{
			strText = "";

			vTextPos = Vector2(0.0);
			vTextRange = Vector2(0.0);
			fBaseLine = 0.825f;

			vFontSize = Vector2(16.0);
			fFontSpace = 0.0f;

			nFontWeight = SDF_FONT_WEIGHT_MEDIUM;
		}
	};

	class SDFTextStyle
	{
	public:
		int				nFontColorType;		// 文字颜色类型
		float			fFontGradientDegree;// 文字渐变色角度
		Vector4			vFontBaseColor;		// 基础文字颜色
		float			fFontBaseColorScale;// 颜色缩放
		Vector4			vFontGradientColor;	// 渐变文字颜色

		bool			bOutLine;			// 是否有文字外轮廓
		float			fOutLineWidth;		// 文字外轮廓宽度
		Vector4			vOutLineColor;		// 文字外轮廓颜色

		bool			bShadow;			// 是否有文字阴影
		Vector2			vShadowOffset;		// 文字阴影偏移
		Vector4			vShadowColor;		// 文字阴影颜色

		SDFTextStyle()
		{
			nFontColorType = SDF_FONT_COLOR_PURE;
			fFontGradientDegree = 0.0f;
			vFontBaseColor = Vector4(1.0);
			vFontGradientColor = Vector4(1.0);
			fFontBaseColorScale = 1.0f;
			bOutLine = false;
			fOutLineWidth = 1.0f;
			vOutLineColor = Vector4(1.0);

			bShadow = false;
			vShadowOffset = Vector2(0.0);
			vShadowColor = Vector4(1.0);
		}
	};

	class SDFTextShow
	{
	public:
		std::string		strID;				// ID，用于查找
		std::string		strFlagName;		// 标记，用于查找
		SDFTextShape	tTextShape;			// 文字形状信息
		SDFTextStyle	tTextStyle;			// 文字样式信息

		SDFTextShow()
		{
			tTextShape = SDFTextShape();
			tTextStyle = SDFTextStyle();
		}
	};

	// 节点参数库
	class NodeParamBase
	{
	public:
		int					nNodeType;			// 节点类型
		std::string			strName;			// 节点名字
		std::string			strID;				// 节点ID
		std::string			strFlagName;		// 业务的分类名称
		INT_PTR				externParam;		// 外部参数
		int					nVersion;			//版本，当字段或者逻辑变动较大时可用此做区分，主要用于js的解析，可通过版本进行不同的解析操作
		NodeParamBase() :
			externParam(NULL),
			nNodeType(NODE_UNKNOWN),
			nVersion(-1)
		{

		}

		NodeParamBase(const int& _nType) :
			NodeParamBase()
		{
			nNodeType = _nType;
		}

		NodeParamBase(const int& _nType, const std::string& _strName) :
			NodeParamBase()
		{
			nNodeType = _nType;
			strName = _strName;
		}

		NodeParamBase(const int& _nType, const std::string& _strName, const std::string& _strID) :
			NodeParamBase()
		{
			nNodeType = _nType;
			strName = _strName;
			strID = _strID;
		}

		NodeParamBase& operator= (const NodeParamBase& rParam)
		{
			nNodeType = rParam.nNodeType;
			strName = rParam.strName;
			strID = rParam.strID;
			strFlagName = rParam.strFlagName;
			externParam = rParam.externParam;
			nVersion = rParam.nVersion;

			return *this;
		}

		virtual ~NodeParamBase() {}
	};
	
	//节点参数倾斜摄影节点
	class NodeParamPagedNode : public NodeParamBase
	{
	public:
		BoundingSphered	tSphere;								// 包围球
		int				nRangeMode;								// 范围模式
		int				nRangeSize;								// 范围大小
		std::vector<std::pair<float, float>> vecRangeList;		// 范围清单
		char			szParent[64];							// 父母节点
		char			szSelf[64];								// 自己
		int				nDeep;									// 深度
		int				nMshOffset;								// msh文件偏移量
		int				nMshZipLength;								// msh压缩文件长度
		int				nMshLength;								// msh文件长度
		int				nTexOffset;								// tex文件偏移量
		int				nTexLength;								// tex文件长度
		char			szHcmName[128];							// hcm文件名

		NodeParamPagedNode() : NodeParamBase()
		{
			nNodeType = NODE_PAGED_NODE;
			
			tSphere._vCenter = Vector3(0.0f);
			tSphere._fRadius = 0.0f;
			nRangeMode = HC_PIXEL_SIZE_ON_SCREEN;
			nRangeSize = 0;
			vecRangeList.clear();
			memset(szParent, 0, sizeof(szParent));
			memset(szSelf, 0, sizeof(szSelf));
			nDeep = -1;
			nMshOffset = -1;
			nMshLength = -1;
			nTexOffset = -1;
			nTexLength = -1;
			nMshZipLength = -1;
			memset(szHcmName, 0, sizeof(szHcmName));
		}

		NodeParamPagedNode& operator = (const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);
			return *this;
		}

		NodeParamPagedNode& operator= (const NodeParamPagedNode& rParam)
		{
			NodeParamBase::operator=(rParam);

			tSphere = rParam.tSphere;
			nRangeMode = rParam.nRangeMode;
			nRangeSize = rParam.nRangeSize;
			vecRangeList = rParam.vecRangeList;
			memcpy(szParent, rParam.szParent, sizeof(szParent));
			memcpy(szSelf, rParam.szSelf, sizeof(szSelf));
			nDeep = rParam.nDeep;
			nMshOffset = rParam.nMshOffset;
			nMshLength = rParam.nMshLength;
			nTexOffset = rParam.nTexOffset;
			nTexLength = rParam.nTexLength;
			nMshZipLength = rParam.nMshZipLength;
			memcpy(szHcmName, rParam.szHcmName, sizeof(szHcmName));

			return *this;

		}

		void Clear()
		{
			externParam = NULL;
			nNodeType = NODE_PAGED_NODE;

			tSphere._vCenter = Vector3(0.0f);
			tSphere._fRadius = 0.0f;
			nRangeMode = HC_PIXEL_SIZE_ON_SCREEN;
			nRangeSize = 0;
			vecRangeList.clear();
			memset(szParent, 0, sizeof(szParent));
			memset(szSelf, 0, sizeof(szSelf));
			nDeep = -1;
			nMshOffset = -1;
			nMshLength = -1;
			nTexOffset = -1;
			nTexLength = -1;
			memset(szHcmName, 0, sizeof(szHcmName));
		}
	};

	//节点参数 瓦片
	class NodeParamTiledNode : public NodeParamBase
	{
	public:
		BoundingBox		tBox;									// 包围盒
		int				nRangeMode;								// 范围模式
		int				nRangeSize;								// 范围大小
		std::vector<std::pair<float, float>> vecRangeList;		// 范围清单
		Matrix4			mMatrix;								// 矩阵
		int				nDeep;									// 深度
		int				nColumn;								// 列
		int				nRow;									// 行
		std::string		strUrl;									// 瓦片地址
		NodeParamTiledNode() : NodeParamBase()
		{
			tBox.Clear();
			nNodeType = NODE_TILED_NODE;
			nRangeMode = HC_DISTANCE_FROM_EYE_POINT;
			nRangeSize = 0;
			vecRangeList.clear();
			mMatrix = Matrix4::IDENTITY;
			nDeep = -1;
			nColumn = 0;
			nRow = 0;
		}

		NodeParamTiledNode& operator = (const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		NodeParamTiledNode& operator= (const NodeParamTiledNode& rParam)
		{
			NodeParamBase::operator=(rParam);
			tBox = rParam.tBox;
			nRangeMode = rParam.nRangeMode;
			nRangeSize = rParam.nRangeSize;
			vecRangeList = rParam.vecRangeList;
			mMatrix = rParam.mMatrix;
			nDeep = rParam.nDeep;
			strUrl = rParam.strUrl;
			nColumn = rParam.nColumn;
			nRow = rParam.nRow;
			return *this;

		}

		void Clear()
		{
			externParam = NULL;
			nNodeType = NODE_TILED_NODE;
			nRangeMode = HC_DISTANCE_FROM_EYE_POINT;
			nRangeSize = 0;
			vecRangeList.clear();
			mMatrix = Matrix4::IDENTITY;
			nDeep = -1;
			tBox.Clear();
			nColumn = 0;
			nRow = 0;
			strUrl = "";
		}
	};


	class PolygonParam : public NodeParamBase
	{
	public:
		std::string				strBaseImage;			// 底面图片
		std::string				strHoverBaseImage;		// 底面悬浮图片
		bool					bBaseGlowEnable;		// 底面辉光
		float					fBaseGlowIntensity;		// 底面辉光强度
		float					fUVRepeat;				// 图片重复范围
		Vector4					vBaseColor;				// 底面颜色 有图片优先使用图片
		Vector4					vSelectColor;			// 底面被选择颜色
		Vector4					vHoverColor;			// 底面鼠标悬浮颜色
		bool					bClickEnable;			// 是否可以点击
		bool					bHoverEnable;			// 是否可以悬浮
		float					fBaseHeight;			// 底面高度
		std::vector<Vector3>	vecVertex;				// 顶点

		bool                 bCalculateCenter;    //是否计算重心
		bool					bWithLine;				// 是否带边框线
		float					fLineWidth;				// 边框线宽度
		Vector4					vLineColor;				// 边框线颜色
		Vector4					vHoverLineColor;		// 边框线悬浮颜色
		bool					bDepthTest;				// 深度测试
		bool					bWithFence;				// 是否带围栏
		float					fFenceHeight;			// 围栏高度
		std::string				strFenceImage;			// 围栏贴图
		std::string				strHoverFenceImage;		// 围栏悬浮贴图
		bool					bFenceGlowEnable;		// 围栏辉光
		float					fFenceGlowIntensity;	// 围栏辉光强度
		Vector4					vFenceColor;			// 围栏颜色 有图片优先使用图片
		Vector4					vHoverFenceColor;		// 围栏悬浮颜色 有图片优先使用图片
		bool					bCircle;							//是否圆形

		PolygonParam() : NodeParamBase()
		{
			
			nNodeType = NODE_POLYGON;
			strBaseImage = "";
			bBaseGlowEnable = false;
			fBaseGlowIntensity = 1.0;
			fUVRepeat = -1.0;
			vBaseColor = Vector4(1.0);
			vSelectColor = Vector4(1.0);
			vHoverColor = Vector4(1.0);
			bClickEnable = false;
			bHoverEnable = false;
			fBaseHeight = 0.0;
			vecVertex.clear();

			bDepthTest = true;
			bWithLine = false;
			fLineWidth = 1.0;
			vLineColor = Vector4(1.0);
			vHoverLineColor = Vector4(1.0);

			bWithFence = false;
			fFenceHeight = 1.0;
			strFenceImage = "";
			bFenceGlowEnable = false;
			fFenceGlowIntensity = 1.0;
			vFenceColor = Vector4(1.0);
			vHoverFenceColor = Vector4(1.0);

			bCircle = false;
			bCalculateCenter = false;
		}

		PolygonParam& operator=(const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		PolygonParam& operator=(const PolygonParam& src)
		{
			NodeParamBase::operator=(src);

			strBaseImage = src.strBaseImage;
			strHoverBaseImage = src.strHoverBaseImage;
			bBaseGlowEnable = src.bBaseGlowEnable;
			fBaseGlowIntensity = src.fBaseGlowIntensity;
			fUVRepeat = src.fUVRepeat;
			vBaseColor = src.vBaseColor;
			vSelectColor = src.vSelectColor;
			vHoverColor = src.vHoverColor;
			bClickEnable = src.bClickEnable;
			bHoverEnable = src.bHoverEnable;
			fBaseHeight = src.fBaseHeight;
			vecVertex.assign(src.vecVertex.begin(), src.vecVertex.end());

			bWithLine = src.bWithLine;
			fLineWidth = src.fLineWidth;
			vLineColor = src.vLineColor;
			vHoverLineColor = src.vHoverLineColor;
			bDepthTest = src.bDepthTest;
			bWithFence = src.bWithFence;
			fFenceHeight = src.fFenceHeight;
			strFenceImage = src.strFenceImage;
			strHoverFenceImage = src.strHoverFenceImage;
			bFenceGlowEnable = src.bFenceGlowEnable;
			fFenceGlowIntensity = src.fFenceGlowIntensity;
			vFenceColor = src.vFenceColor;
			vHoverFenceColor = src.vHoverFenceColor;
			bCalculateCenter = src.bCalculateCenter;
			bCircle = src.bCircle;
			return *this;
		}
	};

	//动态加载节点参数
	typedef float(*GetCameraDistance)(ISystemAPI* pSystemAPI, INode* pNode, INode* pCamera);
	class DynamicLoadNodeParam : public NodeParamBase
	{
	public:
		DynamicLoadNodeParam() : NodeParamBase()
		{
			nNodeType = NODE_DYNAMIC_LOAD;
			bDynamicEnable = true;
			fLoadDistance = 1000.0f;
			pGetCameraDistance = nullptr;
			nCompileLevel = 0.0f;
		}

		DynamicLoadNodeParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		DynamicLoadNodeParam& operator =(const DynamicLoadNodeParam& rhs)
		{
			NodeParamBase::operator=(rhs);
			bDynamicEnable = rhs.bDynamicEnable;
			fLoadDistance = rhs.fLoadDistance;
			pGetCameraDistance = rhs.pGetCameraDistance;
			nCompileLevel = rhs.nCompileLevel;

			return *this;
		}
	public:
		GetCameraDistance pGetCameraDistance;
		bool	bDynamicEnable;	// 是否启用
		float	fLoadDistance;	// 加载距离
		int		nCompileLevel;  //加载级别，越大则越后加载
	};

	// 视频融合节点
	class VideoFusionNodeParam : public NodeParamBase
	{
	public:
		VideoFusionNodeParam() : NodeParamBase()
		{
			bScaleSize = false;
			nScaleWidth = 1920;
			nScaleHeight = 1080;
			bUFront = false;
			nNodeType = NODE_VIDEO_FUSION;
			nSort = 0;
		}

		VideoFusionNodeParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		VideoFusionNodeParam& operator =(const VideoFusionNodeParam& rhs)
		{
			NodeParamBase::operator=(rhs);
			bScaleSize = rhs.bScaleSize;
			nScaleWidth = rhs.nScaleWidth;
			nScaleHeight = rhs.nScaleHeight;
			bUFront = rhs.bUFront;
			nSort = rhs.nSort;
			return *this;
		}
	public:
		bool	bScaleSize;		// 缩放大小
		int		nScaleWidth;	// 视频宽
		int		nScaleHeight;	// 视频高
		bool	bUFront;
		int		nSort;			// 排序
	};

	// 粒子
	class ParticleNodeParam : public NodeParamBase
	{
	public:		
		int		nCount;
		float	fMaxLife;
		float	fMinLife;
		float	fRadius;
		float	fSize;
		bool	bRotation;
		Vector3	vAccelerate;
		Vector3 vMaxVelocity;
		Vector3 vMinVelocity;

		ParticleNodeParam() : NodeParamBase()
		{
			nNodeType = NODE_PARTICLE;
			fSize = 50;
			nCount = 100;
			fMaxLife = 2.0;
			fMinLife = 1.0;
			fRadius = 0.0;
			bRotation = false;
			vAccelerate = Vector3(0.0);
			vMaxVelocity = Vector3(1.0, 1.0, 10.0);
			vMinVelocity = Vector3(-1.0, -1.0, 5.0);
		}

		ParticleNodeParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		ParticleNodeParam& operator=(const ParticleNodeParam& rhs)
		{
			NodeParamBase::operator=(rhs);
			vMaxVelocity = rhs.vMaxVelocity;
			vMinVelocity = rhs.vMinVelocity;
			bRotation = rhs.bRotation;
			vAccelerate = rhs.vAccelerate;
			nCount = rhs.nCount;
			fMaxLife = rhs.fMaxLife;
			fMinLife = rhs.fMinLife;
			fRadius = rhs.fRadius;
			fSize = rhs.fSize;
			return *this;
		}
	};

	enum MouseToolType
	{
		MOVE_AXIS_X = 0,
		MOVE_AXIS_Y,
		MOVE_AXIS_Z,
		MOVE_PLANE_XY,
		MOVE_PLANE_YZ,
		MOVE_PLANE_ZX,
		ROTATE_PLANE_XY,
		ROTATE_PLANE_YZ,
		ROTATE_PLANE_ZX,
		SCALE_AXIS_X,
		SCALE_AXIS_Y,
		SCALE_AXIS_Z,
		SCALE_PLANE_XY,
		SCALE_PLANE_YZ,
		SCALE_PLANE_ZX,
		SCALE_PLANE_XYZ
	};

	class MoveAxisParam :public NodeParamBase
	{
	public:
		int		nAxisType;
		float	fScale;
		std::string	strImage;
		std::string	strHoverImage;
		std::string	strModelFile;
		MoveAxisParam() : NodeParamBase()
		{
			fScale = 0.2f;
			nAxisType = MOVE_AXIS_X;
		}

		MoveAxisParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		MoveAxisParam& operator = (const MoveAxisParam& rhs)
		{
			NodeParamBase::operator=(rhs);
			strImage = rhs.strImage;
			strHoverImage = rhs.strHoverImage;
			strModelFile = rhs.strModelFile;
			nAxisType = rhs.nAxisType;
			fScale = rhs.fScale;
			return *this;
		}
	};

	class MoveToolParam : public NodeParamBase
	{
	public:
		std::vector<MoveAxisParam> vecAxis;
		MoveToolParam() : NodeParamBase()
		{

		}

		MoveToolParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		MoveToolParam& operator = (const MoveToolParam& rhs)
		{
			NodeParamBase::operator=(rhs);

			vecAxis = rhs.vecAxis;

			return *this;
		}
	};

	enum CoordinateAxisType
	{
		COORDINATE_AXIS_X = 0,
		COORDINATE_AXIS_Y,
		COORDINATE_AXIS_Z
	};

	class CoordinateAxisParam :public NodeParamBase
	{
	public:
		float		fScale;
		int			nAxisType;
		std::string	strImage;
		std::string	strModelFile;
		
		CoordinateAxisParam() : NodeParamBase()
		{
			fScale = 1.0;
			nNodeType = NODE_COORDINATE_AXIS;
			nAxisType = COORDINATE_AXIS_X;
		}

		CoordinateAxisParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		CoordinateAxisParam& operator = (const CoordinateAxisParam& rhs)
		{
			NodeParamBase::operator=(rhs);
			fScale = rhs.fScale;
			strImage = rhs.strImage;
			strModelFile = rhs.strModelFile;
			nAxisType = rhs.nAxisType;

			return *this;
		}
	};

	class AcgisInfoParam :public NodeParamBase
	{
	public:
		std::string strScaleRulerPath;
		std::string strCompassBackgroundPath;
		std::string strCompassFrontPath;
		Vector4    vRulePos; // x, y ->(start xy)。z,w -> )width, height)
		Vector4    vCompassPos; // x, y ->(start xy)。z,w -> )width, height)

		AcgisInfoParam() : NodeParamBase()
		{
			nNodeType = NODE_ACGIS_INFO;
		}

		AcgisInfoParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		AcgisInfoParam& operator = (const AcgisInfoParam& rhs)
		{
			NodeParamBase::operator=(rhs);
			strScaleRulerPath = rhs.strScaleRulerPath;
			strCompassBackgroundPath = rhs.strCompassBackgroundPath;
			strCompassFrontPath = rhs.strCompassFrontPath;
			vRulePos = rhs.vRulePos;
			vCompassPos = rhs.vCompassPos;
			return *this;
		}
	};

#define LINE_COUNT_MAX	(1025)
#define LINE_COUNT_MIN	21

	class GridParam :public NodeParamBase
	{
	public:
		float	fHeight;
		float	fLineWidth;
		int		nLineCount;
		float	fDynamicSpace;
		float	fSpace;
		Vector4 vColor;
		Vector4	vCenterColor;
		bool	bGammaCorrect;
		bool	bDynamicScale;
		float	fFadeDistance;
		GridParam() : NodeParamBase()
		{
			nNodeType = NODE_GRID;
			fHeight = -0.1f;
			fLineWidth = 1.0f;
			nLineCount = 201;
			fSpace = 1.0f;
			fDynamicSpace = 1.0f;
			vColor = Vector4(0.3, 0.3, 0.3, 0.8);
			vCenterColor = Vector4(0.1, 0.1, 0.1, 0.8);
			bDynamicScale = false;
			fFadeDistance = 10.0f;
			bGammaCorrect = true;
		}

		GridParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		GridParam& operator = (const GridParam& rhs)
		{
			NodeParamBase::operator=(rhs);
			fDynamicSpace = rhs.fDynamicSpace;
			fHeight = rhs.fHeight;
			fLineWidth = rhs.fLineWidth;
			nLineCount = rhs.nLineCount;
			fSpace = rhs.fSpace;
			vColor = rhs.vColor;
			vCenterColor = rhs.vCenterColor;
			bDynamicScale = rhs.bDynamicScale;
			fFadeDistance = rhs.fFadeDistance;
			bGammaCorrect = rhs.bGammaCorrect;
			return *this;
		}
	};

	// 节点访问器
	class INodeVisitor
	{
	public:
		/*
		 *	申请
		 *	pNode	[in]	节点
		 */
		virtual void iApply(INode* pNode) = 0;

		/*
		 *	清空
		 */
		virtual void iClear() = 0;

		/*
		 *	获取节点
		 *	return	[out]	节点
		 */
		virtual std::vector<INode*>& iGetNode() = 0;
	};
	class INode;

	class NodeLoadCallback
	{
	public:
		virtual void Loading(INode* pNode, float progress) = 0;
		virtual void Loaded(INode* pNode) = 0;
		virtual void Preloading(INode* pNode) = 0;
	};

	class IModel;
	class IComponent;
	class ICamera;
	class IManipulator;
	class IInertiaManipulator;
	class IOrthoManipulator;
	class IRenderNode;
	class IGroup;
	class IGeo;
	class IInstance;
	class IParticle;
	class ICubeReflect;
	class ILight;
	class IBoard;
	class ISpotLightBoard;
	class IVideo;
	class IBoardRoute;
	class IVideoRoute;
	class ILightBeam;
	class IMeasureDistance;
	class IMeasureArea;
	class ILocusLine;
	class IVideoFusionNode;
	class IUserInterface;
	class IUIWidget;
	class IUIVideo;
	class IUIBoard;
	class IUIBoardInstance;
	class IFlyLine;
	class IDrawLine;
	class IShell;
	class IPolygon;
	class IRoad;
	class IPlotting;
	class IFusion;
	class IFusionCamera;
	class IAnimation;
	class IPolygonSelector;
	class IBIMRootNode;
	class IMultiFlyLine;
	class SpatialReferenceParam;
	class INode
	{
	public:
		/*
		 *	获取名字
		 *	return		[out]	节点名字
		 */
		virtual std::string			iGetName() const = 0;

		/*
		 *	设置名字
		 *	strName		[in]	节点名字
		 */
		virtual void				iSetName(const std::string& strName) = 0;

		/*
		 *	获取ID
		 *	return		[out]	节点ID
		 */
		virtual std::string			iGetID() const = 0;

		/*
		 *	设置ID
		 *	strID		[in]	节点ID
		 */
		virtual void				iSetID(const std::string& strName) = 0;

		/*
		 *	获取业务分类名称
		 *	return		[out]	分类名称
		 */
		virtual std::string			iGetFlagName() const = 0;

		/*
		 *	设置业务分类名称
		 *	strID		[in]	分类名称
		 */
		virtual void				iSetFlagName(const std::string& strID) = 0;

		/*
		 *	获取节点参数
		 *	return		[out]	节点参数
		 */
		virtual NodeParamBase*		iGetNodeParam() = 0;

		/*
		 *	通过ID找节点
		 *	strID		[in]	节点ID
		 *	return		[out]	节点
		 */
		virtual INode*				iFindNodeByID(const std::string& strID) = 0;

		/*
		 *	获取外部参数
		 *	return		[out]	外部参数
		 */
		virtual INT_PTR				iGetExternParam() = 0;

		/*
		 *	设置外部参数
		 *	ptr			[in]	外部参数
		 */
		virtual void				iSetExternParam(const INT_PTR& ptr) = 0;

		/*
		 *	获取节点类型
		 *	return		[out]	节点类型
		 */
		virtual int					iGetNodeType() const = 0;

		/*
		 *	获取部件
		 *	nType		[in]	类型
		 *	return		[out]	部件
		 */
		virtual IComponent*			iGetComponent(const int& nType) = 0;

		/*
		 *	是否启用
		 *	return		[out]	是否启用
		 */
		virtual bool				iIsEnable() const = 0;

		/*
		 *	设置启用
		 *	bEnable		[in]	是否启用
		 */
		virtual void				iSetEnable(const bool& bEnable) = 0;

		/*
		 *	获取原点
		 *	return		[out]	原点
		 */
		virtual Vector3d				iGetOrigin() = 0;

		/*
 *	获取原点
 *	return		[out]	原点
 */
	//	virtual Vector3d				iGetOrigind() = 0;

		/*
		 *	设置原点
		 *	vPos		[in]	原点
		 */
		virtual void				iSetOrigin(const Vector3d& vPos) = 0;

		/*
 *	设置原点
 *	vPos		[in]	原点 双精度
 */
	//	virtual void				iSetOrigind(const Vector3d& vPos) = 0;

		/*
		 *	获取轴
		 *	return		[out]	轴
		 */
		virtual Axisd				iGetAxis() = 0;


		/*
		 *	获取轴
		 *	return		[out]	轴 双精度
		 */
	//	virtual Axisd				iGetAxisd() = 0;

		/*
		 *	设置轴
		 *	vAxis		[in]	轴 
		 */
		virtual void				iSetAxis(const Axisd& vAxis) = 0;

		/*
 *	设置轴
 *	vAxis		[in]	轴  双精度
 */
		//virtual void				iSetAxisd(const Axisd& vAxis) = 0;

		/*
		 *	获取角度
		 *	return		[out]	角度
		 */
		virtual Vector3d				iGetAngles() = 0;

		/*
 *	获取角度
 *	return		[out]	角度   双精度
 */
	//	virtual Vector3d				iGetAnglesd() = 0;


		/*
		 *	设置角度
		 *	vAngles		[in]	角度
		 */
		virtual void				iSetAngles(const Vector3d& vAngles) = 0;

		/*
 *	设置角度
 *	vAngles		[in]	角度   双精度
 */
	//	virtual void				iSetAnglesd(const Vector3d& vAngles) = 0;

		/*
		 *	获取缩放
		 *	return		[out]	缩放
		 */
		virtual Vector3d				iGetScale() = 0;

		/*
 *	获取缩放
 *	return		[out]	缩放    双精度
 */
	//	virtual Vector3d				iGetScaled() = 0;

		/*
		 *	设置缩放
		 *	vScale		[in]	缩放
		 */
		virtual void				iSetScale(const Vector3d& vScale) = 0;


		/*
 *	设置缩放
 *	vScale		[in]	缩放     双精度
 */
//		virtual void				iSetScaled(const Vector3d& vScale) = 0;

		/*
		 *	转换
		 *	vOrigin		[in]	新坐标
		 *	bRecord		[in]	是否记录
		 */
		virtual void				iTransform(const Vector3d& vOrigin, const bool& bRecord = true) = 0;

		/*
		 *	转换
		 *	vAxis		[in]	新轴
		 *	bRecord		[in]	是否记录
		 */
		virtual void				iTransform(const Axisd& vAxis, const bool& bRecord = true) = 0;

		/*
		 *	转换
		 *	mMat		[in]	新的模型矩阵
		 *	bRecord		[in]	是否记录
		 */
		virtual void				iTransform(const Matrix4d& mMat, const bool& bRecord = true) = 0;

		/*
		 *	转换
		 *	vAngles		[in]	新角度
		 *	vCenter		[in]	围绕中心点
		 *	bRecord		[in]	是否记录
		 */
		virtual void				iTransformFromAngle(const Vector3d& vAngles, const Vector3d& vCenter, const bool& bRecord = true) = 0;

		/*
		 *	转换
		 *	vAngles		[in]	新角度
		 *	bRecord		[in]	是否记录
		 */
		virtual void				iTransformFromAngle(const Vector3d& vAngles, const bool& bRecord = true) = 0;

		/*
		 *	转换
		 *	vScale		[in]	新缩放
		 *	bRecord		[in]	是否记录
		 */
		virtual void				iTransformFromScale(const Vector3d& vScale, const bool& bRecord = true) = 0;

		/*
		 *	记录矩阵
		 */
		virtual void				iRecordMatrix() = 0;

		/*
		 *	获取记录矩阵
		 */
		virtual const Matrix4d&				iGetRecordMatrix() const = 0;

		/*
		 *	获取记录位置
		 */
		virtual Vector3d				iGetRecordOrigin() = 0;

		/*
		 *	获取记录角度
		 */
		virtual Vector3d				iGetRecordAngles() = 0;

		/*
		 *	获取记录缩放
		 */
		virtual Vector3d			iGetRecordScale() = 0;

		/*
		 *	获取矩阵
		 *	return		[out]	矩阵
		 */
		virtual const Matrix4d&				iGetMatrix() const = 0;

		/*
		 *	接受节点访问器
		 *	tVisitor	[in]	节点访问器
		 */
		virtual void				iAcceptNodeVisitor(INodeVisitor& tVisitor) = 0;

		/*
		 *	获取父节点
		 *	return		[out]	父节点
		 */
		virtual INode*				iGetParent() = 0;

		/*
		 *	增加子节点
		 *	pNode		[in]	子节点
		 */
		virtual void				iAddChild(INode* pNode) = 0;

		/*
		 *	移除子节点
		 *	pNode		[in]	子节点
		 */
		virtual void				iRemoveChild(INode* pNode) = 0;

		/*
		 *	获取子节点
		 *	return		[out]	子节点
		 */
		virtual INode*				iGetChild(const size_t& i) = 0;

		/*
		 *	获取所有子节点
		 *	vecChildren		[out]	所有子节点
		 */
		virtual void				iGetAllChildren(std::vector<INode*>& vecChildren) = 0;

		/*
		 *	获取节点数
		 *	return		[out]	节点数
		 */
		virtual size_t				iGetChildCount() = 0;

		/*
		 *	设置是否能访问
		 *	bEnable		[in]	能否访问
		 */
		virtual void				iSetVisible(const bool& bEnable) = 0;

		/*
		 *	能否访问
		 *	return		[out]	能否访问
		 */
		virtual bool				iIsVisible() = 0;

		/*
		 *	加载模型
		 *	pCamera		[in]	相机
		 */
		virtual void				iLoadModel(INode* pCameraNode = nullptr) = 0;

		/*
		 *	释放模型
		 *	pCamera		[in]	相机
		 */
		virtual void				iFreeModel(INode* pCameraNode = nullptr) = 0;

		/*
		 *	是否加载
		 *	return		[out]	是否加载
		 */
		virtual bool				iIsLoad() = 0;

		/*
		 *	选中节点
		 *	bSelect		[in]	是否选中
		 */
		virtual void				iSetSelect(const bool& bSelect) = 0;

		/*
		 *	悬停节点
		 *	bHover		[in]	是否悬停
		 */
		virtual void				iSetHover(const bool& bHover) = 0;

		/*
		 *	清空选择
		 */
		virtual void				iClearSelect() = 0;
		/*
		 *	清空悬停
		 */
		virtual void				iClearHover() = 0;
		/*
		 *	设置帧更新功能
		 *	pFunc		[in]	帧更新功能
		 *  pUser		[in]	回调使用者
		 */
		virtual void				iSetFrameUpdateFunc(FrameUpdateFunc pFunc, void* pUser) = 0;

		/*
		 *	更新包围盒
		 *	return		[out]	包围盒
		 */
		virtual const BoundingBoxd&	iUpdateBBox() = 0;

		/*
		 *	获取包围盒
		 *	return		[out]	包围盒
		 */
		virtual const BoundingBoxd&	iGetBBox() = 0;

		/*
		 *	获取渲染包围盒
		 *	return		[out]	包围盒
		 */
		virtual const BoundingBoxd&	iGetRenderBox() = 0;

		/*
		 *	设置显示移动工具
		 *	bEnable		[int]	是否显示
		 */
		virtual void				iSetMoveToolEnable(const bool& bEnable) = 0;

		/*
		 *	是否显示移动工具
		 *	return		[out]	是否显示
		 */
		virtual bool				iIsMoveToolEnable() = 0;

		/*
		 *	获取事件接收排序 采用降序排列
		 *	return		[out]	排序
		 */
		virtual int					iGetProcessEventSort() const = 0;

		/*
		 *	设置事件接收排序 采用降序排列
		 *	nSort		[in]	排序
		 */
		virtual void				iSetProcessEventSort(const int& nSort) = 0;

		/*
		 *	设置是否保存自己
		 *	bSave		[in]	是否保存
		 */
		virtual void				iSetSaveMyself(bool bSave) = 0;

		/*
		 *	设置是否保存自己的子节点
		 *	bSave		[in]	是否保存
		 */
		virtual void				iSetSaveChild(bool bSave) = 0;

		/*
			设置场景模式
		*/
		virtual void              iUpdateSceneMode(SceneMode_e mode, const SpatialReferenceParam& param) = 0;

		/*
			设置动画
		*/
		virtual void           iSetAnimation(const std::string& name, IAnimation* pAnimation) = 0;

		/*
			设置额外指针参数（扩展）
		*/
		virtual void           iSetExtraINTPTR(std::string strKey,INT_PTR pTr) = 0;

		/*
			获取额外指针参数（扩展）
		*/
		virtual INT_PTR        iGetExtraINTPTR(std::string strKey) = 0;

		/*
			设置额Json针参数（扩展）
		*/
		virtual void           iSetExtraJson(std::string strKey, const Json::Value& jValue) = 0;

		/*
			获取额外Json参数（扩展）
		*/
		virtual Json::Value    iGetExtraJson(std::string strKey) = 0;

		virtual void iSetLoadingCallback(NodeLoadCallback* pCallback) = 0;

		/*
		 *	获取组件
		 *	return		[out]	组件
		 */
		template <typename T>
		T* GetDynamicComponent()
		{
			if (typeid(T) == typeid(bc::ICamera))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_CAMERA));
			}
			else if (typeid(T) == typeid(bc::IManipulator))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_MANIPULATOR));
			}
			else if (typeid(T) == typeid(bc::IInertiaManipulator))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_INERTIA_MANIPULATOR));
			}
			else if (typeid(T) == typeid(bc::IOrthoManipulator))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_ORTHO_MANIPULATOR));
			}
			else if (typeid(T) == typeid(bc::IRenderNode))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_RENDER_NODE));
			}
			else if (typeid(T) == typeid(bc::IGroup))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_GROUP));
			}
			else if (typeid(T) == typeid(bc::IGeo))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_GEO));
			}
			else if (typeid(T) == typeid(bc::IInstance))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_INSTANCE));
			}
			else if (typeid(T) == typeid(bc::IParticle))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_PARTICLE));
			}
			else if (typeid(T) == typeid(bc::ICubeReflect))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_CUBE_REFLECT));
			}
			else if (typeid(T) == typeid(bc::ILight))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_LIGHT));
			}
			else if (typeid(T) == typeid(bc::IBoard))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_BOARD));
			}
			else if (typeid(T) == typeid(bc::ISpotLightBoard))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_SPOT_LIGHT_BOARD));
			}
			else if (typeid(T) == typeid(bc::IVideo))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_VIDEO));
			}
			else if (typeid(T) == typeid(bc::IBoardRoute))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_BOARD_ROUTE));
			}
			else if (typeid(T) == typeid(bc::IVideoRoute))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_VIDEO_ROUTE));
			}
			else if (typeid(T) == typeid(bc::ILightBeam))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_LIGHTBEAM));
			}
			else if (typeid(T) == typeid(bc::IMeasureDistance))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_MEASURE_DISTANCE));
			}
			else if (typeid(T) == typeid(bc::IMeasureArea))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_MEASURE_AREA));
			}
			else if (typeid(T) == typeid(bc::ILocusLine))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_LOCUS_LINE));
			}
			else if (typeid(T) == typeid(bc::IVideoFusionNode))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_VIDEO_FUSION_NODE));
			}
			else if (typeid(T) == typeid(bc::IUserInterface))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_USER_INTERFACE));
			}
			else if (typeid(T) == typeid(bc::IUIWidget))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_UI_WIDGET));
			}
			else if (typeid(T) == typeid(bc::IUIVideo))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_UI_VIDEO));
			}
			else if (typeid(T) == typeid(bc::IUIBoard))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_UI_BOARD));
			}
			else if (typeid(T) == typeid(bc::IUIBoardInstance))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_UI_BOARD_INSTANCE));
			}
			else if (typeid(T) == typeid(bc::IFlyLine))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_FLY_LINE));
			}
			else if (typeid(T) == typeid(bc::IDrawLine))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_DRAW_LINE));
			}
			else if (typeid(T) == typeid(bc::IShell))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_SHELL));
			}
			else if (typeid(T) == typeid(bc::IPolygon))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_POLYGON));
			}
			else if (typeid(T) == typeid(bc::IRoad))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_ROAD));
			}
			else if (typeid(T) == typeid(bc::IPlotting))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_PLOTTING));
			}
			else if (typeid(T) == typeid(bc::IFusion))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_FUSION));
			}
			else if (typeid(T) == typeid(bc::IFusionCamera))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_FUSION_CAMERA));
			}
			else if (typeid(T) == typeid(bc::IPolygonSelector))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_POLYGON_SELECTOR));
			}
			else if (typeid(T) == typeid(bc::IBIMRootNode))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_BIM_ROOT));
			}
			else if (typeid(T) == typeid(bc::IMultiFlyLine))
			{
				return static_cast<T*>(iGetComponent(bc::COMPONENT_TYPE_MULTI_FLY_LINE));
			}
			return nullptr;
		}
	};
}
