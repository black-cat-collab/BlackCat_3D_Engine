#pragma once

#define SGL_BS_NONE					0x00000000
#define SGL_BS_ZERO					0x00000001	/* blend source: GL_ZERO */
#define SGL_BS_ONE					0x00000002	/* blend source: GL_ONE */
#define SGL_BS_DST_COLOR			0x00000003	/* blend source: GL_DST_COLOR */
#define SGL_BS_ONE_MINUS_DST_COLOR	0x00000004	/* blend source: GL_ONE_MINUS_DST_COLOR */
#define SGL_BS_SRC_ALPHA			0x00000005	/* blend source: GL_SRC_ALPHA */
#define SGL_BS_ONE_MINUS_SRC_ALPHA	0x00000006	/* blend source: GL_ONE_MINUS_SRC_ALPHA */
#define SGL_BS_DST_ALPHA			0x00000007	/* blend source: GL_DST_ALPHA */
#define SGL_BS_ONE_MINUS_DST_ALPHA	0x00000008	/* blend source: GL_ONE_MINUS_DST_ALPHA */
#define SGL_BS_SRC_ALPHA_SATURATE	0x00000009	/* blend source: GL_ALPHA_SATURATE */

#define SGL_BD_NONE					0x00000000
#define SGL_BD_ZERO					0x00000010	/* blend dest:   GL_ZERO */
#define SGL_BD_ONE					0x00000020	/* blend dest:   GL_ONE */
#define SGL_BD_SRC_COLOR			0x00000030	/* blend dest:   GL_SRC_COLOR */
#define SGL_BD_ONE_MINUS_SRC_COLOR	0x00000040	/* blend dest:   GL_ONE_MINUS_SRC_COLOR */
#define SGL_BD_SRC_ALPHA			0x00000050	/* blend dest:   GL_SRC_ALPHA */
#define SGL_BD_ONE_MINUS_SRC_ALPHA	0x00000060	/* blend dest:   GL_ONE_MINUS_SRC_ALPHA */
#define SGL_BD_DST_ALPHA			0x00000070	/* blend dest:   GL_DST_ALPHA */
#define SGL_BD_ONE_MINUS_DST_ALPHA	0x00000080	/* blend dest:   GL_ONE_MINUS_DST_ALPHA */

#define SGL_DEPTH_WRITE				0x00000100	/* depth mask:   enabled (if set) or disabled (else) */
#define SGL_PM_LINE					0x00001000	/* polygon mode: line (if set) or fill (else) */
#define SGL_DEPTH_CLAMP				0x00100000

#define SGL_DF_NONE					0x00000000	/* depth func:	 disabled */
#define SGL_DF_LESS					0x00010000	/* depth func:   GL_LESS */
#define SGL_DF_LEQUAL				0x00020000	/* depth func:   GL_LEQUAL */
#define SGL_DF_EQUAL				0x00040000	/* depth func:   GL_EQUAL */
#define SGL_DF_GEQUAL               0x00080000	/* depth func:   GL_GEQUAL */

#define SGL_AF_NONE					0x00000000	/* alpha func:   disable */
#define SGL_AF_GT0					0x10000000	/* alpha func:   GL_GREATER 0 */
#define SGL_AF_LT128				0x20000000	/* alpha func:   GL_LESS 128 */
#define SGL_AF_GE128				0x30000000	/* alpha func:   GL_GEQUAL 128 */

#define SGL_AF_GT					0x40000000	/* alpha func:   GL_GREATER */
#define SGL_AF_LT   				0x50000000	/* alpha func:   GL_LESS   */
#define SGL_AF_GE					0x60000000	/* alpha func:   GL_GEQUAL */
#define SGL_AF_LE					0x70000000	/* alpha func:   GL_LEQUAL */
#define SGL_AF_EQ					0x80000000	/* alpha func:   GL_EQUAL  */
#define SGL_AF_NE					0x90000000	/* alpha func:   GL_NEQUAL */

#define SGL_AF_GE76					0xa0000000	/* alpha func:   GL_NEQUAL */

#define SGL_MASK_BS					0x0000000F	/* blend source mask */
#define SGL_MASK_BD					0x000000F0	/* blend dest mask */
#define SGL_MASK_BF					0x000000FF	/* blend func mask */
#define SGL_MASK_DF					0x000F0000	/* depth func mask */
#define SGL_MASK_AF					0xF0000000	/* alpha func mask */
#define SGL_MASK_AF_VALUE			0x0FF00000	/* alpha func mask */

#define SGL_CULL_NONE				0
#define SGL_CULL_FRONT				1
#define SGL_CULL_BACK				2

#define MAX_STATIC_LIGHT_COUNT		4

#define MAX_CSM_SPLITS				10

#define MAX_VIEWSHED_ANALYSIS		6

#define MAX_GAUSS_BLUR_COUNT		100

#define MAX_SSAO_KERNEL_COUNT		128

#define MAX_VIDEO_FUSION_COUNT		20

#define MAX_HEAT_MAP_LAYER			5

#define MAX_HIZ_MAP					6

namespace bc
{
	class IMaterial;
	typedef void(*MaterialFrameUpdateFunc)(ISystemAPI* pSystemAPI, IMaterial* pMaterial, void* pUser);

	/************* 枚举定义开始 **************/
	enum EffectStage_e
	{
		EFFECT_STAGE_UNKNOWN = -1,						// 无效
		EFFECT_STAGE_DIFFUSE = 0,						// 漫反射贴图
		EFFECT_STAGE_SECOND_DIFFUSE_MAP,				// 第二套漫反射贴图
		EFFECT_STAGE_ALPHA,								// 透明贴图
		EFFECT_STAGE_LIGHTMAP,							// 灯光贴图
		EFFECT_STAGE_SPECULAR,							// 高光贴图
		EFFECT_STAGE_NORMAL,							// 法线贴图
		EFFECT_STAGE_HEIGHT,							// 高度贴图
		EFFECT_STAGE_DEPTH_MAP,							// 深度图
		EFFECT_STAGE_BLEND_TARGET_MAP_1,				// 混合目标贴图
		EFFECT_STAGE_BLEND_TARGET_MAP_2,				// 混合目标贴图
		EFFECT_STAGE_BLEND_TARGET_MAP_3,				// 混合目标贴图
		EFFECT_STAGE_BLEND_RATE_MAP,					// 混合贴图因子 r:target1 g:target2 b:target3
		EFFECT_STAGE_MASK_MAP,							// MASK贴图
		EFFECT_STAGE_GBUFFER_FRAG_COLOR,				// GBuffer片段颜色
		EFFECT_STAGE_GBUFFER_EMISSIVE_COLOR,			// GBuffer自发光颜色
		EFFECT_STAGE_GBUFFER_VERTEX_WS,					// GBuffer顶点信息
		EFFECT_STAGE_GBUFFER_NORMAL_WS,					// GBuffer世界空间法线信息
		EFFECT_STAGE_GBUFFER_NORMAL_VS,					// GBuffer视图空间法线信息
		EFFECT_STAGE_GBUFFER_MATERIAL,					// GBuffer材质信息
		EFFECT_STAGE_GBUFFER_STATIC_SHADOW,				// GBuffer静态阴影
		EFFECT_STAGE_GBUFFER_VELOCITY,					// GBuffer场景速度
		EFFECT_STAGE_PREVIOUS_VELOCITY,					// 上一帧场景速度
		EFFECT_STAGE_PREVIOUS_COLOR,					// 上一帧颜色
		EFFECT_STAGE_PREVIOUS_DEPTH,					// 上一帧深度
		EFFECT_STAGE_DEFFERED_LIGHTING,					// 延迟光照灯光图
		EFFECT_STAGE_BLOOM,								// 泛光贴图
		EFFECT_STAGE_CROSS_FLARE_H,						// 泛光贴图
		EFFECT_STAGE_CROSS_FLARE_V,						// 泛光贴图
		EFFECT_STAGE_PLANE_REFLECT,						// 平面反射贴图
		EFFECT_STAGE_CUBE_REFLECT,						// 球型反射贴图
		EFFECT_STAGE_CUBE_REFLECT_IRRADIANCE,			// 球型反射辐照度
		EFFECT_STAGE_CUBE_REFLECT_PREFILTER,			// 球型反射预处理
		EFFECT_STAGE_SHADOW,							// 阴影贴图
		EFFECT_STAGE_SSSHADOW_MAP,						// 屏幕空间阴影
		EFFECT_STAGE_YUV_Y,								// YUV贴图
		EFFECT_STAGE_YUV_U,								// YUV贴图
		EFFECT_STAGE_YUV_V,								// YUV贴图
		EFFECT_STAGE_HEAT_MAP,							// 热力图强度图
		EFFECT_STAGE_POLYGON_MESH,
		EFFECT_STAGE_AVERAGE_TRANSPARENT_COUNTER_MAP,	// 加权平均透明 计数
		EFFECT_STAGE_AVERAGE_TRANSPARENT_COLOR_MAP,		// 加权平均透明 颜色
		EFFECT_STAGE_NOISE,								// 噪声贴图
		EFFECT_STAGE_AO,								// 环境遮蔽贴图
		EFFECT_STAGE_METALLIC,							// 金属度贴图
		EFFECT_STAGE_ROUGHNESS,							// 粗糙度贴图
		EFFECT_STAGE_GLOSS,								// 光滑度贴图
		EFFECT_STAGE_REFLECT,							// 反射度贴图
		EFFECT_STAGE_SRM,								// r:specular g:roughness b:metalic
		EFFECT_STAGE_EMISSIVE,							// 自发光贴图
		EFFECT_STAGE_IRRADIANCE,						// 辐照度贴图
		EFFECT_STAGE_SKY_IRRADIANCE,					// 天空辐照度贴图	
		EFFECT_STAGE_SKY_PREFILTER,						// 天空预处理贴图
		EFFECT_STAGE_ENVIRONMENT_MAP,					// 环境反射贴图
		EFFECT_STAGE_BRDF_MAP,							// BRDF贴图
		EFFECT_STAGE_SSAO_NOISE_MAP,					// 屏幕空间环境遮蔽 扰动	
		EFFECT_STAGE_SSAO_SHADOW_MAP,					// 屏幕空间环境遮蔽 阴影	
		EFFECT_STAGE_HIZ_1,								// 深度mipmap 1
		EFFECT_STAGE_HIZ_2,								// 深度mipmap 2
		EFFECT_STAGE_HIZ_3,								// 深度mipmap 3
		EFFECT_STAGE_HIZ_4,								// 深度mipmap 4
		EFFECT_STAGE_HIZ_5,								// 深度mipmap 5
		EFFECT_STAGE_HIZ_6,								// 深度mipmap 6
		EFFECT_STAGE_SSR_REFLECT_MAP,					// 屏幕空间反射 反射
		EFFECT_STAGE_VIEWSHED_ANALYSIS_MAP,				// 视域分析贴图
		EFFECT_STAGE_VIDEO_FUSION_MAP,					// 视频融合贴图
		EFFECT_STAGE_VIDEO_FUSION_DEPTH_MAP,			// 视频融合深度贴图
		EFFECT_STAGE_SUN_GOD_RAY,						// 太阳体积光
		EFFECT_STAGE_LUMINANCE,							// 亮度图
		EFFECT_STAGE_LUMINANCE_PREVIOUS,				// 上一帧亮度						
		EFFECT_STAGE_STATIC_LIGHT_MAP,					// 静态光照图
		EFFECT_STAGE_STATIC_SHADOW_MAP,					// 静态阴影图
		EFFECT_STAGE_PERLIN_WORLEY_NOISE_MAP,			// Perlin-Worly噪声图
		EFFECT_STAGE_WORLEY_NOISE_MAP,					// Worly噪声图
		EFFECT_STAGE_CURL_NOISE_MAP,					// Curl噪声图
		EFFECT_STAGE_SH_COEFFS_MAP_1,					// 球谐系数体素图
		EFFECT_STAGE_SH_COEFFS_MAP_2,					// 球谐系数体素图
		EFFECT_STAGE_SH_COEFFS_MAP_3,					// 球谐系数体素图
		EFFECT_STAGE_SH_COEFFS_MAP_4,					// 球谐系数体素图
		EFFECT_STAGE_SH_COEFFS_MAP_5,					// 球谐系数体素图
		EFFECT_STAGE_SH_COEFFS_MAP_6,					// 球谐系数体素图
		EFFECT_STAGE_SH_COEFFS_MAP_7,					// 球谐系数体素图
		EFFECT_STAGE_SH_COEFFS_MAP_8,					// 球谐系数体素图
		EFFECT_STAGE_SH_COEFFS_MAP_9,					// 球谐系数体素图
		EFFECT_STAGE_TONE_MAPPING_LUT_MAP,				// 色调映射图
		EFFECT_STAGE_COLOR_GRADING_LUT_MAP,				// LUT
		EFFECT_STAGE_SDF_FONT_DISTANCE_MAP,				// SDF字体距离
		EFFECT_STAGE_SDF_FONT_COLOR_MAP,				// SDF字体颜色
		EFFECT_STAGE_SDF_FONT_OUTLINE_MAP,				// SDF字体轮廓线
		EFFECT_STAGE_SDF_FONT_SHADOW_MAP,				// SDF字体阴影
		EFFECT_STAGE_ENVIRONMENT_REFLECT_MAP,			// 环境反射
		EFFECT_STAGE_ENVIRONMENT_REFLECT_HDR_MAP,		// 环境反射
		EFFECT_STAGE_NUM
	};

	enum TexAnimationType_e
	{
		TRANSLATE_STRAIGHT_Y,
		TRANSLATE_STRAIGHT_X,
		TRANSLATE_STRAIGHT_XY,
		ROTATION_CENTER
	};

	enum ShaderLightingMode_e
	{
		LIGHTING_MODE_NONE = 0,
		LIGHTING_MODE_PBR,
		LIGHTING_MODE_EMISSIVE
	};

	enum LightType_e
	{
		LIGHT_UNKOWN = 0,
		LIGHT_SPOT,			//射灯
		LIGHT_POINT,		//点光源
		LIGHT_DIRECTION,	// 方向光
		LIGHT_SQUARE,		// 矩形光源
		LIGHT_AMBIENT,		// 氛围光
		LIGHT_SUN,			// 日光
		LIGHT_MOON,			// 月光
		LIGHT_SKY			// 天光
	};

	enum ShadowFilterType_e
	{
		SHADOW_FAST_FILTER = 0,	// 快速采样
		SHADOW_PCF_POISSON_FILTER,	// 泊松 PCF
		SHADOW_PCF_AVERAGE_FILTER,	// 平均 PCF
		SHADOW_PCSS_POISSON_FILTER	// 泊松 PSCC
	};

	enum LightShadowType_e
	{
		CAST_NONE_SHADOW = 0,
		CAST_DYNAMIC_SHADOW,	// 发射动态阴影
		CAST_STATIC_SHADOW		// 发射静态阴影
	};

	enum ShadowMapQuality_e
	{
		SHADOW_MAP_QUALITY_LOW = 0,			// 贴图大小 512
		SHADOW_MAP_QUALITY_MEDIUM,			// 贴图大小 1024
		SHADOW_MAP_QUALITY_HIGH,			// 贴图大小 2048
		SHADOW_MAP_QUALITY_EXTREMELY_HIGH	// 贴图大小 4096
	};

	enum TransparentType_e
	{
		TRANSPARENT_NONE = 0,	// 不混合
		TRANSPARENT_BLEND,		// 基础混合
		TRANSPARENT_AVERAGE		// 平均透明算法
	};

	enum BlendFunc_e
	{
		BLEND_FUNC_BLEND = 0,			// 混合
		BLEND_FUNC_ADD,					// 叠加
		BLEND_FUNC_FILTER,				// 过滤
		BLEND_FUNC_CUSTOM				// 自定义
	};

	enum SkyType_e
	{
		SKY_NO_CLOUD = 0,			//没有云的天空
		SKY_NORMAL_CLOUD,			//带普通云的天空
		SKY_TYPE_VOLUMETRIC_CLOUD	//带体积云的天空
	};

	enum HeatMapColorMode_e
	{
		HEATMAP_COLOR_MODE_NORMAL = 0,		//普通模式
		HEATMAP_COLOR_MODE_TRAFFIC			//交通模式
	};

	enum DistanceClipMode_e
	{
		DISTANCE_CLIP_MODE_KEEP_MIDDLE = 0,			//保留中间
		DISTANCE_CLIP_MODE_DISCARD_MIDDLE			//丢弃中间
	};
	/************* 枚举定义结束 **************/

	/************* 材质参数定义开始 **************/
	class ICamera;
	class MaterialLight
	{
	public:
		std::string	strID;				// 灯光ID
		int		nType;					// 光源类型 参数：LightType_e
		Vector3 vDirection;				// 方向
		Vector3 vPosition;				// 位置

		float fIntensity;				// 强度
		float fSpecularIntensity;		// 高光强度

		Vector3 vDiffuse;				// 漫反射颜色

		float cutOff;					// 聚光灯夹角
		float outerCutOff;				// 聚光灯外夹角

		float fLengthX;					// 矩形光X长度
		float fLengthY;					// 矩形光Y长度
		float fLengthZ;					// 矩形光Z长度
		Vector3 vAxisX;					// 矩形光X轴
		Vector3 vAxisY;					// 矩形光Y轴
		Vector3 vAxisZ;					// 矩形光Z轴
		float	fSquareAttenuation;		// 矩形光衰减

		int nShadowType;				// 阴影类型 参数：LightShadowType_e
		int nDynamicIndex;				// 对应动态阴影贴图索引值
		Vector4 vDynamicShadowChannel;	// 动态阴影通道
		Vector4 vStaticShadowChannel;	// 静态阴影通道

		float fDiscardThreshold;		// 丢弃阈值

		int nBoxEnable;					// 是否启用Box限制
		Matrix4 mBoxMatrix;				// Box矩阵
		Vector3 vBoxSize;				// Box大小

		MaterialLight()
		{
			nType = LIGHT_UNKOWN;
			strID = "";

			vDirection = Vector3(0, 0, 0);
			vPosition = Vector3(0, 0, 0);

			vDiffuse = Vector3(0, 0, 0);

			fLengthX = 1.0;
			fLengthY = 1.0;
			fLengthZ = 1.0;

			vAxisX = Vector3(0, 0, 0);
			vAxisY = Vector3(0, 0, 0);
			vAxisZ = Vector3(0, 0, 0);

			fSquareAttenuation = 0.5;

			cutOff = 0;
			outerCutOff = 0;
			fIntensity = 1;
			fSpecularIntensity = 1;

			nDynamicIndex = -1;

			vStaticShadowChannel = Vector4(0, 0, 0, 0);
			vDynamicShadowChannel = Vector4(0, 0, 0, 0);

			nShadowType = CAST_NONE_SHADOW;

			fDiscardThreshold = 0.01;

			nBoxEnable = 0;
			mBoxMatrix = Matrix4::IDENTITY;
			vBoxSize = Vector3(0, 0, 0);
		}

		MaterialLight& operator = (const MaterialLight& rhs)
		{
			nType = rhs.nType;
			strID = rhs.strID;
			vPosition = rhs.vPosition;
			vDirection = rhs.vDirection;

			fIntensity = rhs.fIntensity;
			fSpecularIntensity = rhs.fSpecularIntensity;
			vDiffuse = rhs.vDiffuse;

			vAxisX = rhs.vAxisX;
			vAxisY = rhs.vAxisY;
			vAxisZ = rhs.vAxisZ;
			fLengthX = rhs.fLengthX;
			fLengthY = rhs.fLengthY;
			fLengthZ = rhs.fLengthZ;
			fSquareAttenuation = rhs.fSquareAttenuation;

			cutOff = rhs.cutOff;
			outerCutOff = rhs.outerCutOff;

			nShadowType = rhs.nShadowType;
			nDynamicIndex = rhs.nDynamicIndex;
			vDynamicShadowChannel = rhs.vDynamicShadowChannel;
			vStaticShadowChannel = rhs.vStaticShadowChannel;

			fDiscardThreshold = rhs.fDiscardThreshold;

			nBoxEnable = rhs.nBoxEnable;
			mBoxMatrix = rhs.mBoxMatrix;
			vBoxSize = rhs.vBoxSize;

			return *this;
		}
	};

	class GBufferShadowLight
	{
	public:
		GBufferShadowLight() :
			nIndex(-1),
			nShadowType(CAST_DYNAMIC_SHADOW)
		{

		}
	public:
		int nIndex;
		int nShadowType;
	};

	// 球型反射
	class MaterialCubeReflect
	{
	public:
		std::string	strID;				// 灯光ID
		int		nEnable;				// 是否启用
		int		nBoxProjection;			// 是否盒体矫正
		float   fRange;					// 范围
		Vector3 vPosition;				// 位置
		Vector3 vBoxSize;				// 盒体大小
		int		nIrradiance;			// 是否启用辐照度
		int		nLighting;				// 是否计算光照

		MaterialCubeReflect()
		{
			nEnable = 1;
			nBoxProjection = 0;
			nIrradiance = 0;
			fRange = 3000.0f;
			vPosition = Vector3(0, 0, 0);
			vBoxSize = Vector3(10, 10, 10);
			nLighting = 0;
		}

		MaterialCubeReflect& operator = (const MaterialCubeReflect& rhs)
		{
			strID = rhs.strID;
			nEnable = rhs.nEnable;
			nIrradiance = rhs.nIrradiance;
			nBoxProjection = rhs.nBoxProjection;
			vPosition = rhs.vPosition;
			fRange = rhs.fRange;
			vBoxSize = rhs.vBoxSize;
			nLighting = rhs.nLighting;

			return *this;
		}
	};

	// 阴影贴图参数
	class ShadowParam
	{
	public:
		int			nMapSize;							// 贴图大小
		Matrix4		mShadowMatrix[MAX_CSM_SPLITS];		// 投影矩阵
		float		fCSMFarClip[MAX_CSM_SPLITS];		// CSM远平面
		float		fBiasBase[MAX_CSM_SPLITS];			// 阴影偏移最小值
		float		fBiasScale[MAX_CSM_SPLITS];			// 阴影偏移系数
		Vector3		vCameraOrigin[MAX_CSM_SPLITS];		// 投影相机位置
		Vector3		vCameraDirection[MAX_CSM_SPLITS];	// 投影相机前轴
		LightType_e eLightType;							// 灯光类型
		int			nCSMCurrentSplits;					// 当前使用CSM分割数量
		float		fShadowIntensity;					// 阴影强度
		Vector4		vSSShadowChannel;					// 屏幕空间阴影通道
		float		fLightWidth;						// 光源宽度 用于PCSS
		ShadowMapQuality_e eQuality;					// 贴图精度
		ShadowFilterType_e eFilterType;					// 阴影滤波方式
		ShadowParam()
		{
			fShadowIntensity = 0.8f;
			nMapSize = 512;
			eFilterType = SHADOW_PCF_AVERAGE_FILTER;
			eQuality = SHADOW_MAP_QUALITY_MEDIUM;
			eLightType = LIGHT_UNKOWN;
			for (int i = 0; i < MAX_CSM_SPLITS; ++i)
			{
				mShadowMatrix[i] = Matrix4::IDENTITY;
				fCSMFarClip[i] = -1.0;
				vCameraOrigin[i] = Vector3(0, 0, 0);
				vCameraDirection[i] = Vector3(0, 0, 0);
				fBiasScale[i] = 0.0;
				fBiasBase[i] = 0.0;
			}
			nCSMCurrentSplits = MAX_CSM_SPLITS;
			vSSShadowChannel = Vector4(0, 0, 0, 0);
			fLightWidth = 1.0f;
		}
	};

	class ViewshedAnalysisParam
	{
	public:
		int		nEnable[MAX_VIEWSHED_ANALYSIS];					// 是否启用
		Matrix4	mCameraMatrix[MAX_VIEWSHED_ANALYSIS];			// 投影矩阵
		Vector3 vCameraOrigin[MAX_VIEWSHED_ANALYSIS];			// 投影相机位置
		Vector3 vCameraDirection[MAX_VIEWSHED_ANALYSIS];		// 投影相机前轴
		Vector3 vVisibleColor;									// 可视区域颜色
		Vector3 vInvisibleColor;								// 不可视区域颜色

		ViewshedAnalysisParam()
		{
			for (int i = 0; i < MAX_VIEWSHED_ANALYSIS; ++i)
			{
				nEnable[i] = 0;
				mCameraMatrix[i] = Matrix4::IDENTITY;
				vCameraOrigin[i] = Vector3(0, 0, 0);
				vCameraDirection[i] = Vector3(0, 0, 0);
			}

			vVisibleColor = Vector3(0, 1, 0);
			vInvisibleColor = Vector3(1, 0, 0);
		}
	};

	// 视频融合参数
	class VideoFusionParam
	{
	public:
		int			nCount;
		int			nEnable[MAX_VIDEO_FUSION_COUNT];
		Vector3		vCameraOrigin[MAX_VIDEO_FUSION_COUNT];		// 投影相机位置
		Vector3		vCameraDirection[MAX_VIDEO_FUSION_COUNT];	// 投影相机前轴
		Matrix4		mVideoMatrix[MAX_VIDEO_FUSION_COUNT];		// 投影矩阵
		int			nEdgeFade[MAX_VIDEO_FUSION_COUNT];			// 边缘淡出
		float		fEdgeFadeWidth[MAX_VIDEO_FUSION_COUNT];		// 边缘淡出宽度
		VideoFusionParam()
		{
			nCount = 0;
			for (int i = 0; i < MAX_VIDEO_FUSION_COUNT; ++i)
			{
				nEnable[i] = 1;
				nEdgeFade[i] = 1;
				fEdgeFadeWidth[i] = 0.05;
				mVideoMatrix[i] = Matrix4::IDENTITY;
				vCameraOrigin[i] = Vector3(0, 0, 0);
				vCameraDirection[i] = Vector3(0, 0, 0);
			}
		}
	};

	class MaterialParamBase
	{
	public:
		int	nEnable;	// 是否启用

		MaterialParamBase()
		{
			nEnable = 0;
		}

		virtual ~MaterialParamBase()
		{

		}

		MaterialParamBase& operator = (const MaterialParamBase& rhs)
		{
			nEnable = rhs.nEnable;

			return *this;
		}
	};

	// Box参数
	class BoxParam : public MaterialParamBase
	{
	public:
		Vector3		vModelBoxMax;			// 模型Box
		Vector3		vModelBoxMin;			// 模型Box
		Vector3		vMeshBoxMax;			// 面Box
		Vector3		vMeshBoxMin;			// 面Box
		Vector3		vSHCoeffsBoxMax;		// 球谐系数Box
		Vector3		vSHCoeffsBoxMin;		// 球谐系数Box
		BoxParam() : MaterialParamBase()
		{
			vModelBoxMax = Vector3(1.0f);
			vModelBoxMin = Vector3(0.0f);

			vMeshBoxMax = Vector3(1.0f);
			vMeshBoxMin = Vector3(0.0f);

			vSHCoeffsBoxMax = Vector3(0.0f);
			vSHCoeffsBoxMin = Vector3(0.0f);
		}

		BoxParam& operator = (const BoxParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			vModelBoxMax = rhs.vModelBoxMax;
			vModelBoxMin = rhs.vModelBoxMin;

			vMeshBoxMax = rhs.vMeshBoxMax;
			vMeshBoxMin = rhs.vMeshBoxMin;

			vSHCoeffsBoxMax = rhs.vSHCoeffsBoxMax;
			vSHCoeffsBoxMin = rhs.vSHCoeffsBoxMin;

			return *this;
		}
	};

	// 剔除参数
	class CullParam : public MaterialParamBase
	{
	public:
		int		nCullType;			// 剔除方法

		CullParam() : MaterialParamBase()
		{
			nCullType = SGL_CULL_BACK;
		}

		CullParam& operator = (const CullParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			nCullType = rhs.nCullType;

			return *this;
		}
	};

	// 深度 / 透明 参数
	class StateParam : public MaterialParamBase
	{
	public:
		uint	uState;				// OpenGL状态值
		bool	bDepthClamp;		// 是否关闭远近平面裁剪
		int		nTransparentType;	// 透明算法 参数为 TransparentType_e
		int		nBlendFunc;			// 透明算法为blend时混合公式 参数为 BlendFunc_e
		float	fAlphaThreshold;	// 透明剔除

		StateParam() : MaterialParamBase()
		{
			nTransparentType = TRANSPARENT_NONE;
			nBlendFunc = BLEND_FUNC_BLEND;
			fAlphaThreshold = 0.2;
			bDepthClamp = false;
			uState = SGL_DEPTH_WRITE | SGL_DF_LEQUAL;
		}

		StateParam& operator = (const StateParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			uState = rhs.uState;
			nTransparentType = rhs.nTransparentType;
			nBlendFunc = rhs.nBlendFunc;
			fAlphaThreshold = rhs.fAlphaThreshold;
			bDepthClamp = rhs.bDepthClamp;
			return *this;
		}
	};

	// 视角高光
	class ViewSpecularParam : public MaterialParamBase
	{
	public:
		float fIntensity;		// 高光强度
		float fBaseIntensity;	// 底色强度

		ViewSpecularParam() : MaterialParamBase()
		{
			fIntensity = 0.5;
			fBaseIntensity = 1.0;
		}

		ViewSpecularParam& operator = (const ViewSpecularParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			fIntensity = rhs.fIntensity;
			fBaseIntensity = rhs.fBaseIntensity;

			return *this;
		}
	};

	// PBR 材质
	class PBRMaterialParam : public MaterialParamBase
	{
	public:
		float	fAO;					// 环境遮蔽
		float	fRoughness;				// 粗糙度
		float	fMetallic;				// 金属度
		float	fSpecular;				// 高光度
		float	fReflect;				// 反射强度
		PBRMaterialParam() : MaterialParamBase()
		{
			fAO = 1.0f;
			fRoughness = 0.8f;
			fMetallic = 0.0f;
			fSpecular = 1.0;
			fReflect = 0.5;
		}

		PBRMaterialParam& operator = (const PBRMaterialParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			fAO = rhs.fAO;
			fRoughness = rhs.fRoughness;
			fMetallic = rhs.fMetallic;
			fSpecular = rhs.fSpecular;
			fReflect = rhs.fReflect;
			return *this;
		}
	};

	// 阴影
	/*class ShadowParam : public MaterialParamBase
	{
	public:
		bool	bShadowReceive;
		ShadowMapParam tShadowParam;	// 4个通道阴影参数

		ShadowParam() : MaterialParamBase()
		{
			bShadowReceive = false;
			tShadowParam = ShadowMapParam();
		}

		ShadowParam& operator = (const ShadowParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			bShadowReceive = rhs.bShadowReceive;
			tShadowParam = rhs.tShadowParam;

			return *this;
		}
	};*/

	// 静态光照
	class StaticLightParam : public MaterialParamBase
	{
	public:
		int						nLightCount;	// 静态光源数量
		std::vector<MaterialLight>	vecLight;		// 静态光源数据
		std::map<std::string, Vector4>	mShadowChannel;	// 静态光阴影通道

		StaticLightParam() : MaterialParamBase()
		{
			nLightCount = 0;

			for (int i = 0; i < MAX_STATIC_LIGHT_COUNT; ++i)
			{
				MaterialLight tLight;
				vecLight.push_back(tLight);
			}
		}

		StaticLightParam& operator = (const StaticLightParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			nLightCount = rhs.nLightCount;
			mShadowChannel = rhs.mShadowChannel;
			for (int i = 0; i < MAX_STATIC_LIGHT_COUNT; ++i)
			{
				vecLight[i] = rhs.vecLight[i];
			}

			return *this;
		}
	};

	// 贴图流动参数
	class TexAnimationParam : public MaterialParamBase
	{
	public:
		int		nType;		// 贴图流动方式 参数为：TexAnimationType_e
		float   fSpeed;		// 流动速度

		Matrix4 mMatrix;	// 贴图变换矩阵
		float	fCounter;	// 计数器

		TexAnimationParam() : MaterialParamBase()
		{

			nType = TRANSLATE_STRAIGHT_Y;
			fSpeed = 0.5;
			mMatrix = Matrix4(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1);
			fCounter = 0;
		}

		TexAnimationParam& operator = (const TexAnimationParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			nType = rhs.nType;
			fSpeed = rhs.fSpeed;
			mMatrix = rhs.mMatrix;
			fCounter = rhs.fCounter;

			return *this;
		}
	};

	//两套贴图切换参数
	class TexSwitchAnimationParam : public MaterialParamBase
	{
	public:
		float	fSpeed;				// 贴图切换速度

		float	fCounter;			// 计数器

		TexSwitchAnimationParam() : MaterialParamBase()
		{
			fSpeed = 0.5;
			fCounter = 0;
		}

		TexSwitchAnimationParam& operator = (const TexSwitchAnimationParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			fSpeed = rhs.fSpeed;
			fCounter = rhs.fCounter;

			return *this;
		}
	};

	// 雾化参数
	class FogParam : public MaterialParamBase
	{
	public:
		FogParam() : MaterialParamBase()
		{
			nEnable = 1;			// 默认打开雾化
		}

		FogParam& operator = (const FogParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			return *this;
		}
	};

	// 高斯参数
	class GaussParam
	{
	public:
		GaussParam()
		{
			fSigmaFactor = 10.0;
			SetGaussCount(20);
		}

		void SetSigma(const float& fSigma)
		{
			fSigmaFactor = fSigma;
		}

		// 设置高斯密度
		void SetGaussCount(const int& nCount)
		{
			nGaussCount = nCount;

			float fSigma = fSigmaFactor;
			float fSum = 0.0f;

			vecGauss.clear();
			// 由于是对称图像，只计算半边的数据，在shader中同时向两边计算
			for (int i = 0; i < nGaussCount; ++i)
			{
				float fValue = GetGaussValue(i, fSigma);
				vecGauss.push_back(fValue);
				// 只算了半边，所以要乘以2
				//fSum += 2.0 * fValue;
				fSum += (i == 0 ? fValue : 2.0 * fValue);
			}

			for (int i = 0; i < nGaussCount; ++i)
			{
				vecGauss[i] = vecGauss[i] / fSum;
			}
		}

		int& GetGaussCount()
		{
			return nGaussCount;
		}

		std::vector<float>& GetGaussData()
		{
			return vecGauss;
		}

		GaussParam& operator = (const GaussParam& rhs)
		{
			fSigmaFactor = rhs.fSigmaFactor;
			nGaussCount = rhs.nGaussCount;
			vecGauss.clear();
			vecGauss.assign(rhs.vecGauss.begin(), rhs.vecGauss.end());

			return *this;
		}

		static double GetGaussValue(int index, float fSigma)
		{
			//const double dPI = 3.141592653;
			//double A = 1.0 / (fSigma * sqrt(2 * dPI));
			//double dValue = -1.0 * (double)(index * index) / (2.0 * fSigma * fSigma);
			//double ex = exp(dValue);
			//double fResult = A * ex;
			double dValue = -16.7 * (double)(index * index) / (fSigma * fSigma);
			double fResult = exp(dValue);
			return fResult;
		}
	protected:
		float			fSigmaFactor;	// Sigma系数
		int				nGaussCount;	// 高斯参数个数
		std::vector<float>	vecGauss;	// 高斯参数
	};

	// 模糊参数
	class GaussBlurParam : public MaterialParamBase
	{
	public:
		Vector2		vBlurStep;			// 高斯模糊 x:横向步长 y:纵向步长
		GaussParam	tGaussParam;		// 高斯参数
		Vector3		vTintColor;
		float		fQuality;
		GaussBlurParam() : MaterialParamBase()
		{
			vBlurStep = Vector2(1.0f / 1920.0f, 1.0f / 1080.0f);
			vTintColor = Vector3(1.0);
			fQuality = 1.0;
		}

		GaussBlurParam& operator = (const GaussBlurParam& rhs)
		{
			MaterialParamBase::operator=(rhs);
			vTintColor = rhs.vTintColor;
			fQuality = rhs.fQuality;
			tGaussParam = rhs.tGaussParam;
			vBlurStep = rhs.vBlurStep;

			return *this;
		}
	};

	// 亮度计算参数
	class LuminanceParam : public MaterialParamBase
	{
	public:
		float fLuminanceThresholdMax;		// 自动曝光亮度最大阈值
		float fLuminanceThresholdMin;		// 自动曝光亮度最小阈值

		LuminanceParam() : MaterialParamBase()
		{
			fLuminanceThresholdMax = 10.0;
			fLuminanceThresholdMin = 0.0;
		}

		LuminanceParam& operator = (const LuminanceParam& rhs)
		{
			MaterialParamBase::operator=(rhs);
			fLuminanceThresholdMax = rhs.fLuminanceThresholdMax;
			fLuminanceThresholdMin = rhs.fLuminanceThresholdMin;

			return *this;
		}
	};

	// 色调映射参数
	class ToneMappingParam : public MaterialParamBase
	{
	public:
		float fLuminanceMax;		// 自动曝光亮度最大阈值
		float fLuminanceMin;		// 自动曝光亮度最小阈值
		float fExposureScale;		// 缩放比例 -15.0 至 15.0
		float fExposureCurve;		// 曲线 0.0 至 1.0
		ToneMappingParam() : MaterialParamBase()
		{
			fLuminanceMax = 10.0;
			fLuminanceMin = 0.0;
			fExposureScale = 1.0;
			fExposureCurve = 1.0;
		}

		ToneMappingParam& operator = (const ToneMappingParam& rhs)
		{
			MaterialParamBase::operator=(rhs);
			fLuminanceMax = rhs.fLuminanceMax;
			fLuminanceMin = rhs.fLuminanceMin;
			fExposureScale = rhs.fExposureScale;
			fExposureCurve = rhs.fExposureCurve;

			return *this;
		}
	};

	// 色彩滤镜
	class ColorFilterParam : public MaterialParamBase
	{
	public:
		float fHue;				// 色调
		float fSaturation;		// 饱和度
		float fValue;			// 明度
		float fContrast;		// 对比度
		float fBrightness;		// 亮度
		ColorFilterParam() : MaterialParamBase()
		{
			fHue = 0.0;
			fSaturation = 0.0;
			fValue = 0.0;
			fContrast = 0.0;
			fBrightness = 0.0;
		}

		ColorFilterParam& operator = (const ColorFilterParam& rhs)
		{
			MaterialParamBase::operator=(rhs);
			fHue = rhs.fHue;
			fSaturation = rhs.fSaturation;
			fValue = rhs.fValue;
			fContrast = rhs.fContrast;
			fBrightness = rhs.fBrightness;
			return *this;
		}
	};

	// 降采样参数
	class DownSampleParam : public MaterialParamBase
	{
	public:
		Vector2		vStep;			//  x:横向步长 y:纵向步长
		int			nCount;			// 降低比例
		DownSampleParam() : MaterialParamBase()
		{
			vStep = Vector2(1.0f / 1920.0f, 1.0f / 1080.0f);
			nCount = 2;
		}

		DownSampleParam& operator = (const DownSampleParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			vStep = rhs.vStep;
			nCount = rhs.nCount;

			return *this;
		}
	};

	// 辉光参数
	class GlowParam : public MaterialParamBase
	{
	public:
		float	fGlowScale;			// 辉光缩放比例
		float	fGlowColorScale;	// 辉光颜色比例
		int		nGlowColorEnable;	// 是否启用自定义辉光颜色
		Vector4 vGlowColor;			// 辉光颜色
		int		nGlowFlashEnable;	// 是否启用辉光闪烁
		float	fGlowFlashSpeed;	// 辉光闪烁速度

		float fGlowFlashFactor;		// 闪烁速度参数

		GlowParam() : MaterialParamBase()
		{
			fGlowScale = 0;
			fGlowColorScale = 1.0;
			nGlowColorEnable = 0;
			vGlowColor = Vector4(0, 0, 0, 1);
			nGlowFlashEnable = 0;
			fGlowFlashSpeed = 1.0;
			fGlowFlashFactor = 1.0;
		}

		GlowParam& operator = (const GlowParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			fGlowScale = rhs.fGlowScale;
			fGlowColorScale = rhs.fGlowColorScale;
			nGlowColorEnable = rhs.nGlowColorEnable;
			vGlowColor = rhs.vGlowColor;
			nGlowFlashEnable = rhs.nGlowFlashEnable;
			fGlowFlashSpeed = rhs.fGlowFlashSpeed;
			fGlowFlashFactor = rhs.fGlowFlashFactor;


			return *this;
		}
	};

	// 泛光参数
	class BloomParam : public MaterialParamBase
	{
	public:
		float		fThreshold;			// HDR下的亮度阈值	
		float		fBlendAlpha;		// 混合时Bloom的透明度

		BloomParam() : MaterialParamBase()
		{
			fThreshold = 1.2f;
			fBlendAlpha = 1.0f;
		}

		BloomParam& operator = (const BloomParam& rhs)
		{
			MaterialParamBase::operator=(rhs);
			fThreshold = rhs.fThreshold;
			fBlendAlpha = rhs.fBlendAlpha;

			return *this;
		}
	};

	// 十字泛光参数
	class CrossFlareParam : public MaterialParamBase
	{
	public:
		float		fThreshold;			// HDR下的亮度阈值	
		float		fBlendAlpha;		// 混合时Bloom的透明度

		CrossFlareParam() : MaterialParamBase()
		{
			fThreshold = 1.2f;
			fBlendAlpha = 1.0f;
		}

		CrossFlareParam& operator = (const CrossFlareParam& rhs)
		{
			MaterialParamBase::operator=(rhs);
			fThreshold = rhs.fThreshold;
			fBlendAlpha = rhs.fBlendAlpha;

			return *this;
		}
	};

	// 扩散光
	class LightSpreadParam : public MaterialParamBase
	{
	public:
		Vector2 vPostion;	// 在水平面上的坐标
		float 	fRadius;	// 半径
		float   fMinRadius;	// 最小半径
		float   fMaxRadius; // 最大半径 当半径大于最大半径时将重置为最小半径
		float   fSpeed;		// 速度
		Vector3 vColor;		// 颜色
		float	fWidth;		// 色带宽度

		LightSpreadParam() : MaterialParamBase()
		{
			vPostion = Vector2(0.0f, 0.0f);
			fMinRadius = 0.1f;
			fMaxRadius = 5000.0f;
			fRadius = 0.0f;
			fSpeed = 100.0f;
			fWidth = 50.0f;
			vColor = Vector3(0.8f, 0.8f, 0.8f);
		}

		LightSpreadParam& operator = (const LightSpreadParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			vPostion = rhs.vPostion;
			fMinRadius = rhs.fMinRadius;
			fMaxRadius = rhs.fMaxRadius;
			fRadius = rhs.fRadius;
			fSpeed = rhs.fSpeed;
			vColor = rhs.vColor;
			fWidth = rhs.fWidth;
			return *this;
		}
	};

	// 平面反射参数
	class PlaneReflectParam : public MaterialParamBase
	{
	public:
		Matrix4		mReflectMatrix;	// 投影矩阵
		float		fReflectRate;	// 反射率
		bool		bBlur;			// 是否启用模糊


		PlaneReflectParam() : MaterialParamBase()
		{
			mReflectMatrix = Matrix4(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1);
			fReflectRate = 0.5;
			bBlur = false;
		}

		PlaneReflectParam& operator = (const PlaneReflectParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			mReflectMatrix = rhs.mReflectMatrix;
			fReflectRate = rhs.fReflectRate;
			bBlur = rhs.bBlur;

			return *this;
		}
	};

	// YUV参数
	class YUVParam : public MaterialParamBase
	{
	public:
		YUVParam() : MaterialParamBase()
		{

		}

		YUVParam& operator = (const YUVParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			return *this;
		}
	};

	// 法线流动参数
	class NormalFlowParam : public MaterialParamBase
	{
	public:
		float fFlowSpeed;		// 流动速度
		Vector4 vFlowBase;		// 基础流动参数
		Vector4 vFlowAddition;	// 叠加流动参数
		Vector4 vFlowSmall;		// 细节流动参数

		NormalFlowParam() : MaterialParamBase()
		{
			fFlowSpeed = 1.0f;
			vFlowBase = Vector4(0.05f, 0.08f, 0.0f, 0.007f);
			vFlowAddition = Vector4(0.18f, 0.15f, 0.0f, 0.01f);
			vFlowSmall = Vector4(0.55f, 0.55f, 0.0f, 0.02f);
		}

		NormalFlowParam& operator = (const NormalFlowParam& rhs)
		{
			MaterialParamBase::operator=(rhs);
			fFlowSpeed = rhs.fFlowSpeed;
			vFlowBase = rhs.vFlowBase;
			vFlowAddition = rhs.vFlowAddition;
			vFlowSmall = rhs.vFlowSmall;

			return *this;
		}
	};

	// 高度贴图参数
	class HeightMapParam : public MaterialParamBase
	{
	public:
		float fHeightScale;		// 高度缩放参数

		HeightMapParam() : MaterialParamBase()
		{
			fHeightScale = 0.1;
		}

		HeightMapParam& operator = (const HeightMapParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			fHeightScale = rhs.fHeightScale;

			return *this;
		}
	};

	// 延迟光照参数
	class DefferedLightingParam : public MaterialParamBase
	{
	public:
		int nDynamicLightCount;
		MaterialLight	mLight;

		DefferedLightingParam() : MaterialParamBase()
		{
			nDynamicLightCount = 1;
		}

		DefferedLightingParam& operator = (const DefferedLightingParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			nDynamicLightCount = rhs.nDynamicLightCount;
			mLight = rhs.mLight;

			return *this;
		}

	};

	//动态天空参数
	class DynamicSkyParam : public MaterialParamBase
	{
	public:
		SkyType_e eSkyType;
		int	nToneEnable;
		int nSunVisible;
		float fExposure;
		Vector3 vSkyColor;
		Vector3 vSunLightColor;
		Vector3 vGroundColor;
		DynamicSkyParam() : MaterialParamBase()
		{
			eSkyType = SKY_NO_CLOUD;
			nToneEnable = 1;
			nSunVisible = 1;
			fExposure = 1.0;
			vSkyColor = Vector3(1.0);
			vSunLightColor = Vector3(1.0);
			vGroundColor = Vector3(0.0);
		}
		DynamicSkyParam& operator = (const DynamicSkyParam& rhs)
		{
			MaterialParamBase::operator=(rhs);
			eSkyType = rhs.eSkyType;
			nToneEnable = rhs.nToneEnable;
			nSunVisible = rhs.nSunVisible;
			fExposure = rhs.fExposure;
			vSkyColor = rhs.vSkyColor;
			vSunLightColor = rhs.vSunLightColor;
			vGroundColor = rhs.vGroundColor;
			return *this;
		}
	};

	//径向模糊参数
	class RadialBlurParam : public MaterialParamBase
	{
	public:
		int nSampleCount;
		Vector2 vTextureSize;
		Vector2 vBlurCenter;
		RadialBlurParam() : MaterialParamBase()
		{
			nSampleCount = 6;
			vTextureSize = Vector2(1920, 1080);
			vBlurCenter = Vector2(0, 0);
		}
		RadialBlurParam& operator = (const RadialBlurParam& rhs)
		{
			MaterialParamBase::operator=(rhs);
			nSampleCount = rhs.nSampleCount;
			vTextureSize = rhs.vTextureSize;
			vBlurCenter = rhs.vBlurCenter;
			return *this;
		}
	};

	class SunGodRayParam : public MaterialParamBase
	{
	public:
		Vector2 vSunPosition;
		float	fSunLuminanceThreshold;
		Vector3 vColorTint;
		SunGodRayParam() : MaterialParamBase()
		{
			vSunPosition = Vector2(1, 1);
			fSunLuminanceThreshold = 10.0f;
			vColorTint = Vector3(1.0);
		}
		SunGodRayParam& operator = (const SunGodRayParam& rhs)
		{
			MaterialParamBase::operator=(rhs);
			vSunPosition = rhs.vSunPosition;
			fSunLuminanceThreshold = rhs.fSunLuminanceThreshold;
			vColorTint = rhs.vColorTint;
			return *this;
		}
	};

	// 静态光照图参数
	class StaticLightMapParam : public MaterialParamBase
	{
	public:
		//在UV 大图中起点位置   针对烘培
		Vector2 vMapCoordinateBias;
		//比例尺， 针对烘培
		Vector2 vMapCoordinateScale;

		Vector4 vAdds[2];
		Vector4 vScale[2];

		StaticLightMapParam() : MaterialParamBase()
		{
			vMapCoordinateBias = Vector2(0, 0);
			vMapCoordinateScale = Vector2(0, 0);

			vAdds[0] = Vector4(0, 0, 0, 0);
			vAdds[1] = Vector4(0, 0, 0, 0);

			vScale[0] = Vector4(0, 0, 0, 0);
			vScale[1] = Vector4(0, 0, 0, 0);
		}

		StaticLightMapParam& operator = (const StaticLightMapParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			vMapCoordinateBias = rhs.vMapCoordinateBias;
			vMapCoordinateScale = rhs.vMapCoordinateScale;

			vAdds[0] = rhs.vAdds[0];
			vAdds[1] = rhs.vAdds[1];

			vScale[0] = rhs.vScale[0];
			vScale[1] = rhs.vScale[1];

			return *this;
		}

		void SetAdd(Vector4* pAdd)
		{
			for (int i = 0; i < 2; ++i)
			{
				vAdds[0] = pAdd[0];
				vAdds[1] = pAdd[1];
			}
		}

		void SetScale(Vector4* pScale)
		{
			for (int i = 0; i < 2; ++i)
			{
				vScale[0] = pScale[0];
				vScale[1] = pScale[1];
			}
		}
	};

	// 静态阴影图参数
	class StaticShadowMapParam : public MaterialParamBase
	{
	public:
		//在UV 大图中起点位置   针对烘培
		Vector2 vMapCoordinateBias;
		//比例尺， 针对烘培
		Vector2 vMapCoordinateScale;

		Vector4 vAdds[2];
		Vector4 vScale[2];

		StaticShadowMapParam() : MaterialParamBase()
		{
			vMapCoordinateBias = Vector2(0, 0);
			vMapCoordinateScale = Vector2(0, 0);

			vAdds[0] = Vector4(0, 0, 0, 0);
			vAdds[1] = Vector4(0, 0, 0, 0);

			vScale[0] = Vector4(0, 0, 0, 0);
			vScale[1] = Vector4(0, 0, 0, 0);
		}

		StaticShadowMapParam& operator = (const StaticShadowMapParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			vMapCoordinateBias = rhs.vMapCoordinateBias;
			vMapCoordinateScale = rhs.vMapCoordinateScale;

			vAdds[0] = rhs.vAdds[0];
			vAdds[1] = rhs.vAdds[1];

			vScale[0] = rhs.vScale[0];
			vScale[1] = rhs.vScale[1];

			return *this;
		}

		void SetAdd(Vector4* pAdd)
		{
			for (int i = 0; i < 2; ++i)
			{
				vAdds[0] = pAdd[0];
				vAdds[1] = pAdd[1];
			}
		}

		void SetScale(Vector4* pScale)
		{
			for (int i = 0; i < 2; ++i)
			{
				vScale[0] = pScale[0];
				vScale[1] = pScale[1];
			}
		}
	};

	// 景深参数
	class DepthOfFiledParam : public MaterialParamBase
	{
	public:
		float	fStart;			// 起始距离
		float	fFallOff;		// 衰减系数

		DepthOfFiledParam() : MaterialParamBase()
		{
			fStart = 0;
			fFallOff = 0.001;
		}

		DepthOfFiledParam& operator = (const DepthOfFiledParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			fStart = rhs.fStart;
			fFallOff = rhs.fFallOff;

			return *this;
		}
	};

	// 抖动参数
	class ProjectionJitterParam : public MaterialParamBase
	{
	public:
		int		nCurrentFrameCount;			// 当前帧数
		int		nTotalFrameCount;			// 总帧数
		Vector2	vJitter;					// 抖动
		Vector2 vPreviousJitter;			// 上一帧抖动
		ProjectionJitterParam() : MaterialParamBase()
		{
			nCurrentFrameCount = 0;
			nTotalFrameCount = 8;
			vJitter = Vector2(0, 0);
			vPreviousJitter = Vector2(0, 0);
		}

		ProjectionJitterParam& operator = (const ProjectionJitterParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			nCurrentFrameCount = rhs.nCurrentFrameCount;
			nTotalFrameCount = rhs.nTotalFrameCount;
			vJitter = rhs.vJitter;
			vPreviousJitter = rhs.vPreviousJitter;
			return *this;
		}
	};

	// 时间性抗锯齿参数
	class TXAAParam : public MaterialParamBase
	{
	public:
		float	fNearClip;					// 相机近平面
		float	fFarClip;					// 相机远平面
		int		nAntiFlicker;				// 是否抗抖动
		int		nMixToneMap;				// 是否使用混合色调映射
		float	fSharpness;					// 锐利程度
		TXAAParam() : MaterialParamBase()
		{
			fNearClip = 0;
			fFarClip = 10000;
			nAntiFlicker = 0;
			nMixToneMap = 0;
			fSharpness = 0.5;
		}

		TXAAParam& operator = (const TXAAParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			fNearClip = rhs.fNearClip;
			fFarClip = rhs.fFarClip;
			nAntiFlicker = rhs.nAntiFlicker;
			nMixToneMap = rhs.nMixToneMap;
			fSharpness = rhs.fSharpness;
			return *this;
		}
	};

	// 边框参数
	class BorderParam : public MaterialParamBase
	{
	public:
		Vector4	vBorderColor;
		float	fBorderWidth;
		BorderParam() : MaterialParamBase()
		{
			vBorderColor = Vector4(1.0);
			fBorderWidth = 1.0;
		}

		BorderParam& operator = (const BorderParam& rhs)
		{
			MaterialParamBase::operator=(rhs);
			vBorderColor = rhs.vBorderColor;
			fBorderWidth = rhs.fBorderWidth;
			return *this;
		}
	};

	// 蒙版参数
	class BlurMaskParam : public MaterialParamBase
	{
	public:
		int		nMode;
		float	fRadius;			// Circle模式下 半径 0-1
		float	fFactorBase;		// 比例因子
		float	fMarginLeft;		// Square模式下边距 0-1
		float	fMarginRight;
		float	fMarginTop;
		float	fMarginBottom;
		BlurMaskParam() : MaterialParamBase()
		{
			nMode = BLUR_MASK_MODE_CIRCLE;
			fRadius = 0.646;
			fFactorBase = 0.0;
			fMarginLeft = 0.646;
			fMarginRight = 0.646;
			fMarginTop = 0.646;
			fMarginBottom = 0.0;
		}

		BlurMaskParam& operator = (const BlurMaskParam& rhs)
		{
			MaterialParamBase::operator=(rhs);
			nMode = rhs.nMode;
			fRadius = rhs.fRadius;
			fFactorBase = rhs.fFactorBase;
			fMarginLeft = rhs.fMarginLeft;
			fMarginRight = rhs.fMarginRight;
			fMarginTop = rhs.fMarginTop;
			fMarginBottom = rhs.fMarginBottom;
			return *this;
		}
	};

	// 粒子参数
	class ParticleParam : public MaterialParamBase
	{
	public:
		float	fSize;
		float	fRadius;
		float	fMaxLife;
		float	fMinLife;
		Vector3	vMaxVelocity;
		Vector3	vMinVelocity;
		int		nRotation;
		Vector3	vAccelerate;
		ParticleParam() :
			MaterialParamBase(),
			fSize(50.0),
			fRadius(0.7),
			fMaxLife(2000.0f),
			fMinLife(1000.0f),
			nRotation(0),
			vAccelerate(Vector3(0.0)),
			vMaxVelocity(Vector3(0.0, 5.0, 0.0)),
			vMinVelocity(Vector3(0.0, 3.0, 0.0))
		{

		}

		ParticleParam& operator = (const ParticleParam& rhs)
		{
			MaterialParamBase::operator=(rhs);
			nRotation = rhs.nRotation;
			vAccelerate = rhs.vAccelerate;
			fSize = rhs.fSize;
			fRadius = rhs.fRadius;
			fMaxLife = rhs.fMaxLife;
			fMinLife = rhs.fMinLife;
			vMaxVelocity = rhs.vMaxVelocity;
			vMinVelocity = rhs.vMinVelocity;
			return *this;
		}
	};

#define MAX_EFFECT_BONE		4
#define MAX_BONES_NUM		128
	// 骨骼动画参数
	class BoneParam : public MaterialParamBase
	{
	public:
		int nFrameCount;
		int nBoneCount;
		int nFps;
		Matrix4 mCurBoneMatrix[MAX_BONES_NUM];
		Matrix4* pAllBoneMatrix;

		BoneParam()
		{
			nFrameCount = 0;
			nBoneCount = 0;
			nFps = 30;
			pAllBoneMatrix = nullptr;
		}

		virtual ~BoneParam()
		{
			DELETE_ARRAY(pAllBoneMatrix);
		}

		BoneParam& operator = (const BoneParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			nFrameCount = rhs.nFrameCount;
			nBoneCount = rhs.nBoneCount;
			for (int i = 0; i < nBoneCount; i++)
			{
				mCurBoneMatrix[i] = rhs.mCurBoneMatrix[i];
			}

			return *this;
		}
	};

	class HeatMapParam : public MaterialParamBase
	{
	public:
		int					nLayer;
		int					nColorMode;
		float				fLowAlphaThreshold;
		float				fHighAlphaThreshold;

		HeatMapParam()
		{
			nLayer = 0;
			nColorMode = HEATMAP_COLOR_MODE_NORMAL;
			fLowAlphaThreshold = 0.15;
			fHighAlphaThreshold = 0.4;
		}

		HeatMapParam& operator = (const HeatMapParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			nLayer = rhs.nLayer;
			nColorMode = rhs.nColorMode;
			fLowAlphaThreshold = rhs.fLowAlphaThreshold;
			fHighAlphaThreshold = rhs.fHighAlphaThreshold;

			return *this;
		}
	};


	class PolygonMeshParam : public MaterialParamBase
	{

	};

	class GridRenderParam : public MaterialParamBase
	{
	public:
		float				fFadeDistance;

		GridRenderParam()
		{
			fFadeDistance = 10.0;
		}

		GridRenderParam& operator = (const GridRenderParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			fFadeDistance = rhs.fFadeDistance;

			return *this;
		}
	};

	class SDFFontRenderParam : public MaterialParamBase
	{
	public:
		int				nFontEnable;
		int				nFontWeight;
		Vector2			vTextPos;
		Vector2			vTextRange;

		int				nFontColorType;		// 文字颜色类型
		float			fFontGradientDegree;// 文字渐变色角度
		Vector4			vFontBaseColor;		// 基础文字颜色
		Vector4			vFontGradientColor;	// 渐变文字颜色

		int				nOutLine;			// 是否有文字外轮廓
		float			fOutLineWidth;		// 文字外轮廓宽度
		Vector4			vOutLineColor;		// 文字外轮廓颜色

		int				nShadow;			// 是否有文字阴影
		Vector2			vShadowOffset;		// 文字阴影偏移
		Vector4			vShadowColor;		// 文字阴影颜色

		SDFFontRenderParam()
		{
			nFontEnable = 0;
			nFontWeight = 0;
			vTextPos = Vector2(0.0);
			vTextRange = Vector2(0.0);

			nFontColorType = 0;
			fFontGradientDegree = 0.0f;
			vFontBaseColor = Vector4(1.0);
			vFontGradientColor = Vector4(1.0);

			nOutLine = 0;
			fOutLineWidth = 1.0f;
			vOutLineColor = Vector4(1.0);

			nShadow = 0;
			vShadowOffset = Vector2(0.0);
			vShadowColor = Vector4(1.0);
		}

		SDFFontRenderParam& operator = (const SDFFontRenderParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			nFontEnable = rhs.nFontEnable;
			nFontWeight = rhs.nFontWeight;
			vTextPos = rhs.vTextPos;
			vTextRange = rhs.vTextRange;

			nFontColorType = rhs.nFontColorType;
			fFontGradientDegree = rhs.fFontGradientDegree;
			vFontBaseColor = rhs.vFontBaseColor;
			vFontGradientColor = rhs.vFontGradientColor;

			nOutLine = rhs.nOutLine;
			fOutLineWidth = rhs.fOutLineWidth;
			vOutLineColor = rhs.vOutLineColor;

			nShadow = rhs.nShadow;
			vShadowOffset = rhs.vShadowOffset;
			vShadowColor = rhs.vShadowColor;

			return *this;
		}
	};

	class DistanceClipParam : public MaterialParamBase
	{
	public:
		float				fNearDistance;
		float				fFarDistance;
		int					nMode;

		DistanceClipParam()
		{
			fNearDistance = 0.0;
			fFarDistance = FLT_MAX;
			nMode = DISTANCE_CLIP_MODE_KEEP_MIDDLE;
		}

		DistanceClipParam& operator = (const DistanceClipParam& rhs)
		{
			MaterialParamBase::operator=(rhs);

			fNearDistance = rhs.fNearDistance;
			fFarDistance = rhs.fFarDistance;
			nMode = rhs.nMode;

			return *this;
		}
	};

	class MaterialParam
	{
	public:
		std::string		strMatName;			// 材质名称
		bool			bBatchBind;			// 是否进行批处理绑定
		std::string		strEffectStageID;	// 贴图参数ID
		std::string		strMaterialParamID;	// 材质参数ID
		int				nChannelCount;		// 通道总数
		bool			bInstance;			// 是否是实例
		float			fAlphaThreshold;	// 透明阈值
		bool			bCompileIM;			// 是否立即编译
		bool			bCompileTexture;	// 一起编译贴图
		bool			bCompileShader;		// 一起编译shader
		TextureParam	textureParam;		// 贴图参数
		XMLNodePtr		pScriptNode;		// Script节点
		MaterialParam() :
			MaterialParam(DEFAULT_TEXTURE_NAME)
		{

		}
		MaterialParam(const std::string& _strName, const int& _nChannelCount = 1) :
			strMatName(_strName), nChannelCount(_nChannelCount), bInstance(false),
			bCompileIM(false), bCompileTexture(false), bCompileShader(true),
			pScriptNode(nullptr), fAlphaThreshold(0.2f), bBatchBind(false)
		{

		}

		MaterialParam& operator=(const MaterialParam& rhs)
		{
			strEffectStageID = rhs.strEffectStageID;
			strMaterialParamID = rhs.strMaterialParamID;
			textureParam = rhs.textureParam;
			strMatName = rhs.strMatName;
			nChannelCount = rhs.nChannelCount;
			bCompileIM = rhs.bCompileIM;
			bInstance = rhs.bInstance;
			bCompileTexture = rhs.bCompileTexture;
			bCompileShader = rhs.bCompileShader;
			fAlphaThreshold = rhs.fAlphaThreshold;
			pScriptNode = rhs.pScriptNode;
			bBatchBind = rhs.bBatchBind;
		}
	};
	/************* 材质参数定义结束 **************/


	/************* IMaterial **************/
	class IMaterial
	{
	public:
		virtual void			iSetName(const std::string& strName) = 0;
		// 材质名称
		virtual std::string		iGetName() = 0;
		// 帧更新
		virtual void			iFrameUpdate(ICamera* pCamera) = 0;
		// 设置编译shader
		virtual void			iSetCompileType(const std::vector<ShaderType_e>& vecShaderType) = 0;
		// 设置编译shader
		virtual void			iAddCompileType(const ShaderType_e eShaderType) = 0;
		// 编译
		virtual bool			iCompile(const std::vector<ShaderType_e>& vecShaderType) = 0;
		// 编译
		virtual bool			iCompile() = 0;
		// 是否修改
		virtual bool			iIsChanged() = 0;

		// EffectStage
		virtual IEffectStage* iGetEffectStage(const EffectStage_e& eStage) = 0;
		virtual IEffectStage* iSetEffectStageEnable(const EffectStage_e& eStage, bool bEnable) = 0;
		virtual bool			iIsEffectStageEnable(const EffectStage_e& eStage) = 0;
		virtual void			iSetTextureStage(ITexture* pTexture, const EffectStage_e& eStage) = 0;

		// 设置贴图通道
		virtual int				iGetChannelCount() = 0;
		virtual void			iSetChannelCount(const int& nCount) = 0;

		// 设置深度/透明参数
		virtual uint			iGetState() = 0;
		virtual void			iSetState(const uint& uState) = 0;
		virtual void			iSetAlphaThreshold(const float& fValue) = 0;
		virtual float			iGetAlphaThreshold() = 0;
		virtual bool			iTestTransparentType(const TransparentType_e& eType) = 0;

		// 设置剔除
		virtual void			iSetCull(const int& nCull) = 0;
		virtual int				iGetCull() = 0;
		virtual bool			iGetCullNone() = 0;
		virtual void			iSetCullNone(const bool& bNone) = 0;

		// 设置颜色
		virtual void			iSetColor(const Vector4& vColor) = 0;
		virtual Vector4			iGetColor() = 0;

		// 设置实例
		virtual void			iSetInstance(const bool& bIsInstance) = 0;
		virtual bool			iIsInstance() = 0;

		//设置骨骼动画
		virtual void			iSetBone(const bool& bIsBone) = 0;
		virtual bool			iIsBone() = 0;

		// 静态光照
		virtual void			iClearStaticLight() = 0;
		virtual void			iAddStaticLight(MaterialLight& tMaterialLight) = 0;

		// 光照类型
		virtual ShaderLightingMode_e iGetLightingMode() = 0;
		virtual void			iSetLightingMode(const ShaderLightingMode_e& eMode) = 0;

		// 设置透明度
		virtual void			iSetAlpha(const float& fAlpha) = 0;
		virtual float			iGetAlpha() = 0;

		virtual bool			iIsMSAA() = 0;
		virtual void			iSetMSAA(const bool& bMSAA) = 0;

		virtual void			iSetParamEnable(MaterialParamBase& tParam, const bool& bEnable) = 0;
		virtual bool			iIsParamEnable(MaterialParamBase& tParam) = 0;
		virtual void			iSetParam(MaterialParamBase& tParam) = 0;
		virtual void			iGetParam(MaterialParamBase& tParam) = 0;

		// 设置更新回调
		virtual void			iSetFrameUpdateFunc(MaterialFrameUpdateFunc pFunc, void* pUser) = 0;
		// 生成xml
		virtual bool			iGenerateScript(XMLDoc* pXmlDoc) = 0;
		// 保存
		virtual bool			iSave(const std::string& strFilePath) = 0;

		virtual void			iSetBatchBind(const bool& bBatch) = 0;
		virtual bool			iIsBatchBind() = 0;

		virtual void			iSetEffectStageID(const std::string& strID) = 0;
		virtual std::string		iGetEffectStageID() = 0;

		virtual void			iSetMaterialParamID(const std::string& strID) = 0;
		virtual std::string		iGetMaterialParamID() = 0;

		/// <summary>
		/// </summary>
		/// <returns></returns>
		virtual int iGetDownloadTextureState(EffectStage_e eStage) = 0;
		virtual int   iGetDownloadTextureFialedCount(EffectStage_e eStage) = 0;
		virtual bool   iIsDownloadTextureFailed() = 0;


		// 材质参数设置
		template<typename T>
		void iSetParamEnable(bool bEnable)
		{
			T t;
			iSetParamEnable(t, bEnable);
		}
		template<typename T>
		bool iIsParamEnable()
		{
			T t;
			return iIsParamEnable(t);
		}

	};
}

