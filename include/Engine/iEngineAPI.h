#pragma once

#include <limits>
#include <unordered_map>

#define THREAD_LOAD_TEXTURE_WEIGHT		1
#define THREAD_LOAD_MODEL_WEIGHT		4
#define THREAD_LOAD_PAGED_ROOT_WEIGHT	5
#define THREAD_LOAD_PAGED_NODE_WEIGHT	6

#define THREAD_LOAD_TEXTURE_TYPE		7
#define	THREAD_LOAD_MODEL_TYPE			10
#define THREAD_LOAD_PAGED_ROOT_TYPE		11
#define THREAD_LOAD_PAGED_NODE_TYPE		12

namespace bc
{
	enum MouseButton_e
	{
		MOUSE_BUTTON_L,
		MOUSE_BUTTON_M,
		MOUSE_BUTTON_R,
		MOUSE_BUTTON_S_M,
		MOUSE_BUTTON_S_L,
		MOUSE_BUTTON_S_R,
		MOUSE_BUTTON_C_M,
		MOUSE_BUTTON_C_L,
		MOUSE_BUTTON_C_R,
	};

	enum MouseState_e
	{
		MOUSE_NONE,
		MOUSE_LBUTTONDOWN,
		MOUSE_LBUTTONUP,
		MOUSE_LBUTTONDBLCLK,
		MOUSE_RBUTTONDOWN,
		MOUSE_RBUTTONUP,
		MOUSE_RBUTTONDBLCLK,
		MOUSE_MBUTTONDOWN,
		MOUSE_MBUTTONUP,
		MOUSE_MBUTTONDBLCLK,
		MOUSE_WHEEL,
		MOUSE_MOVE,
	};

	enum KeyAction_e
	{
		ACTION_MOVE_FORWARD = 0,
		ACTION_MOVE_BACK,
		ACTION_MOVE_LEFT,
		ACTION_MOVE_RIGHT,
		ACTION_ROTATE_LEFT,
		ACTION_ROTATE_RIGHT,
		ACTION_APPLY_VIEWPOINT,
		ACTION_SCENE_TIME_ADD,
		ACTION_SCENE_TIME_SUBTRACT,
		ACTION_BAKE_LIGHT,
		ACTION_RENDER_CUBE_REFLECT,
		ACTION_DEVELOP_TEST,
		ACTION_MULTIPLE_SELECT,
		ACTION_SHIFT,
		ACTION_CUSTOM_START,
		ACTION_PAGED_DELETE,
		ACTION_LEFT_SHIFT,
		ACTION_RIGHT_SHIFT,
		ACTION_LEFT_CONTROL,
		ACTION_RIGHT_CONTROL,
		ACTION_LEFT_MENU,
		ACTION_RIGHT_MENU
	};

	enum MouseToolType_e
	{
		MOUSE_TOOL_SELECT = 0,
		MOUSE_TOOL_MOVE,
		MOUSE_TOOL_ROTATE,
		MOUSE_TOOL_SCALE,

		MOUSE_TOOL_ISOLATED,
		MOUSE_TOOL_H_SPLIT_SINGLE,
		MOUSE_TOOL_H_SPLIT_ALL,

		MOUSE_TOOL_V_SPLIT_SINGLE,
		MOUSE_TOOL_V_SPLIT_ALL,
		MOUSE_TOOL_POLYGON_SELECTOR,
	};

	enum class KeyBoardType
	{
		CONTROL,
		SHIFT,
		LSHIFT,
		RSHIFT,
		LCONTROL,
		RCONTROL,
		LMENU,
		RMENU,
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	enum RenderQualityType_e
	{
		CompatibilityType = 0,
		NormalType,
		ProfessionalType
	};

	enum DynamicShadowType_e
	{
		SHADOW_NONE = 0,
		SHADOW_SINGLE,		// 只产生一个阴影
		SHADOW_MULTIPLE		// 产生4个阴影
	};

	enum SkyMapQuality_e
	{
		SKY_MAP_QUALITY_EXTREMELY_LOW = 0,		// 贴图大小 64
		SKY_MAP_QUALITY_LOW,					// 贴图大小 128
		SKY_MAP_QUALITY_MEDIUM,					// 贴图大小 256
		SKY_MAP_QUALITY_HIGH,					// 贴图大小 512
		SKY_MAP_QUALITY_EXTREMELY_HIGH			// 贴图大小 1024
	};

	enum CubeMapQuality_e
	{
		CUBE_MAP_QUALITY_EXTREMELY_LOW = 0,		// 贴图大小 64
		CUBE_MAP_QUALITY_LOW,					// 贴图大小 128
		CUBE_MAP_QUALITY_MEDIUM,				// 贴图大小 256
		CUBE_MAP_QUALITY_HIGH,					// 贴图大小 512
		CUBE_MAP_QUALITY_EXTREMELY_HIGH			// 贴图大小 1024
	};

	enum SelectedType_e
	{
		SelectNone = 0,
		SelectBorder,
		SelectBox,
		SelectMesh,
		SelectObject
	};

	enum HoverType_e
	{
		HoverNone = 0,
		HoverBorder,
		HoverBox,
		HoverMesh,
		HoverObject
	};

	enum FogType_e
	{
		FOG_NONE = 0,			// 不雾化
		FOG_LINEAR,				// 线性雾化
		FOG_EXPONENT			// 指数雾化
	};

	enum BlurMaskMode_e
	{
		BLUR_MASK_MODE_CIRCLE = 0,			// 圆
		BLUR_MASK_MODE_SQUARE				// 方
	};

	enum TiledMapProvider_e
	{
		TILED_MAP_PROVIDER_TENCENT = 0,
		TILED_MAP_PROVIDER_GAODE,
		TILED_MAP_PROVIDER_BAIDU,
		TILED_MAP_PROVIDER_OPENSTREET,
		TILED_MAP_PROVIDER_GOOGLE,
		TILED_MAP_PROVIDER_TIANDITU
	};

	enum TiledMapType_e
	{
		TILED_MAP_TYPE_NORMAL = 0,
		TILED_MAP_TYPE_SATELLITE,
	};

	enum EventType_e
	{
		EVENT_NONE,
		EVENT_KEY,
		EVENT_MOUSE,
		EVENT_MOUSE_CLICK,
		EVENT_MOUSE_HOVER,
		EVENT_SCENE_OPEN,
		EVENT_SCENE_LOAD,
		EVENT_SCENE_SAVE,
		EVENT_SCENE_SHUTDOWN,
		EVENT_SCENE_FREE,
		EVENT_SCENE_RELOAD,
		EVENT_SCENE_VIEW_SWITCHED,
		EVENT_SCENE_VIEW_SIZE_CHANGED,
		EVENT_CAMERA_OPERATE_CHANGED,
		EVENT_NODE_HOVER,
		EVENT_NODE_CLICK,
		EVENT_NODE_SELECT_CHANGE,
		EVENT_NODE_MATRIX_CHANGED,
		EVENT_NODE_DELETE,
		EVENT_SHOW_MOUSE_TOOL,
		EVENT_UPDATE_SHADOW,
		EVENT_CASTER_SHADOW_CHANGED,
		EVENT_UPDATE_SKY_OCCLUSION,
		EVENT_UPDATE_PLANE_REFLECT,
		EVENT_UPDATE_CUBE_REFLECT,
		EVENT_UPDATE_CUBE_REFLECT_ONE_DONE,
		EVENT_UPDATE_CUBE_REFLECT_START,
		EVENT_UPDATE_CUBE_REFLECT_END,
		EVENT_UPDATE_IRRADIANCE_VIEW,
		EVENT_UPDATE_IRRADIANCE_VIEW_ONE_DONE,
		EVENT_UPDATE_IRRADIANCE_VIEW_START,
		EVENT_UPDATE_IRRADIANCE_VIEW_END,
		EVENT_UPDATE_STATIC_MERGE_MESH,
		EVENT_UPDATE_NODE_CONTAINER,
		EVENT_LOAD_MODEL,
		EVENT_FREE_MODEL,
		EVENT_RELOAD_UI_FILE,
		EVENT_DRAW_LINE_COMPLETE,
		EVENT_RUN_WEB_JS,
		EVENT_PAGED_DELETE,
		EVENT_TYPE_END
	};

	enum class EventReturnType_e
	{
		NONE = 0,
		BREAK,
	};

    enum CodecType_e
    {
            CODEC_TYPE_H264,
            CODEC_TYPE_H265,
            CODEC_TYPE_VP9,

            CODEC_TYPE_AUTO
    };

	typedef INT_PTR EVENT_PARAM;
	class BCEvent
	{
	public:
		int			eMessage;
		EVENT_PARAM nParam1;
		EVENT_PARAM nParam2;
		EVENT_PARAM nParam3;
		EVENT_PARAM nParam4;
		std::string strWebJsParam;
	
		BCEvent() :
			eMessage((int)EventType_e::EVENT_NONE), 
			nParam1(0), 
			nParam2(0), 
			nParam3(0), 
			nParam4(0)
		{
		}

		BCEvent(const int& e, EVENT_PARAM n1, EVENT_PARAM n2, EVENT_PARAM n3, EVENT_PARAM n4) :
			eMessage(e),
			nParam1(n1),
			nParam2(n2),
			nParam3(n3),
			nParam4(n4)
		{
		}
		
		BCEvent(const int& e) :
			BCEvent(e, 0, 0, 0, 0)
		{
		}

		BCEvent(const int& e, EVENT_PARAM n1) :
			BCEvent(e, n1, 0, 0, 0)
		{
		}

		BCEvent(const int& e, EVENT_PARAM n1, EVENT_PARAM n2) :
			BCEvent(e, n1, n2, 0, 0)
		{
		}

		BCEvent(const int& e, EVENT_PARAM n1, EVENT_PARAM n2, EVENT_PARAM n3) :
			BCEvent(e, n1, n2, n3, 0)
		{
		}

		void SetParams(const int& e, EVENT_PARAM n1, EVENT_PARAM n2, EVENT_PARAM n3, EVENT_PARAM n4)
		{
			eMessage = e;
			nParam1 = n1;
			nParam2 = n2;
			nParam3 = n3;
			nParam4 = n4;
		}
	};

	enum class ProcessEventSort_e
	{
		RenderAPISort = 10,
		CoreAPISort = 20
	};



	// 事件处理器
	class IEventProcessor
	{
	public:
		/*
		 *	执行事件
		 *  tEvent	[in]	执行的事件
		 */
		virtual EventReturnType_e iProcessEvent(const BCEvent& tEvent) = 0;

		/*
		 *	获取排序
		 *  return	[out]	排序值 由小到大升序排列
		 */
		virtual int	iGetProcessEventSort() = 0;
	};

	enum class FrameUpdateObjectSort_e
	{
		RenderAPISort = 10,
		ModelAPISort,
		CoreAPISort
	};

	// 帧更新模块
	class IFrameUpdateModule
	{
	public:
		/*
		 *	启动
		 */
		virtual void iStartUpdate() = 0;

		/*
		 *	帧更新
		 */
		virtual void iFrameUpdate() = 0;

		/*
		 *	获取排序
		 *  return	[out]	排序值 由小到大升序排列
		 */
		virtual int	iGetFrameUpdateSort() = 0;
	};

	typedef struct s_ShaderConfig
	{
		// 生成PBR光照
		bool bPBRShader;		
		// 生成辐照度光照
		bool bIrradianceShader;	
		// 生成平面反射
		bool bPlaneReflectShader;		
		// 生成球面反射
		bool bCubeReflectShader;	
		// 生成动态阴影
		bool bDynamicShadowMapShader;	
		// 生成多层级阴影
		bool bCSMShadowShader;	
		// 生成屏幕空间阴影
		bool bSSShadowMapShader;
		// 生成雾化
		bool bFogShader;	
		// 生成静态光照贴图
		bool bStaticLightingMapShader;	
		// 生成光扩散
		bool bLightSpreadShader;	
		// 生成泛光
		bool bBloomShader;			
		// 生成十字泛光
		bool bCrossFlareShader;		
		// 生成辉光
		bool bGlowShader;	
		// 生成rgb分离故障处理
		bool bRGBSplitGlitchShader;
		// 生成视角高光
		bool bViewSpecularShader;	
		// 生成透明加权平均
		bool bAverageTransparentShader;	
		// 生成后处理的
		bool bPostProcessShader;	
		// 生成延迟光照
		bool bDefferedLightingShader;		
		// 生成延迟渲染
		bool bDefferedShadingShader;	
		// 快速采样
		bool bFXAAShader;	
		// 时间性抗锯齿
		bool bTXAAShader;		
		// 生成动态天空与云
		bool bDynamicSkyShader;
		// 生成天空反射光
		bool bSkyOcclusionShader;		
		// 生成日光光晕
		bool bSunLensFlareShader;	
		// 生成屏幕空间环境光遮蔽
		bool bSSAOShader;		
		// 生成GroundTruth环境光遮蔽
		bool bGTAOShader;
		// 生成模型热力图
		bool bModelHeatMapShader;	
		// 生成路网图
		bool bPolygonMeshShader;
		// 生成屏幕空间反射
		bool bSSRShader;		
		// 生成视域分析
		bool bViewshedAnalysisShader;	
		// 生成太阳体积光
		bool bSunGodRayShader;		
		// 生成色调映射
		bool bToneMappingShader;		
		// 生成色彩滤镜
		bool bColorFilterShader;
		// 生成景深
		bool bDepthOfFiledShader;		
		// 是否允许使用Box限制光源
		bool bLightBoxLimitShader;		
		// 是否允许遮罩剔除
		bool bOcclusionCullingShader;	
		// 动态网格LOD
		bool bDynamicProgressiveMesh;	
		// 视频融合
		bool bVideoFusionShader;
		// 模糊蒙版
		bool bBlurMaskShader;
		// 图片蒙版
		bool bImageMaskShader;
		// 生成颜色转场
		bool bColorTransitionShader;
		// 生成POI
		bool bPOIShader;
		// 生成裁剪
		bool bDistanceClipShader;
		s_ShaderConfig()
		{
			memset(this, 0, sizeof(s_ShaderConfig));
		}
	}ShaderConfig;

	typedef struct s_BootConfig
	{
		// 引擎版本
		std::string		strEngineVersion;
		// 场景视图宽度
		int				nSceneWidth;
		// 场景视图高度
		int				nSceneHeight;
		// 是否全屏
		bool			bFullScreen;
		// 是否缩放输出
		bool			bScaleOutputSize;
		// 输出宽度
		int				nOutputWidth;
		// 输出高度
		int				nOutputHeight;
		// 是否显示登录窗口
		bool			bShowLogin;
		// 是否显示调试信息
		bool			bShowFrameTime;
		// 是否显示球谐光照的系数球
		bool			bShowIrradianceNode;
		// 是否后台渲染
		bool			bBackgroundRender;
		// 后台渲染场景加载时帧率
		int				nBackgroundLoadFPS;
		// 是否固定帧率
		bool			bFixedFPS;
		// 固定帧率值
		int				nUpdateFPS;
		// Http请求超时
		int				nHttpTimeOut;
		// Http请求重试次数
		int				nHttpRetryCount;
		// 程序名称
		std::string		strApplicationName;
		// 程序版本
		std::string		strApplicationVersion;
		// 加载的业务dll名称
		std::string		strClientDLL;
		// 场景用户名称
		std::string		strUserName;
		// 场景文件名称
		std::string		strSceneFile;
		// 默认加载字体
		std::string		strFontName;
		// 默认加载SDF字体
		std::string		strSDFFontName;
		// 默认加载的Script文件
		std::string		strScriptFile;
		// 证书文件
		std::string		strCertFile;
		// WebRTC信令服务器
		std::string		strWebRTCServerHost;
		// Qt websocket端口
		int	nQtCmdPort;
		// HardEncode
		bool			bHardEncode;
		// 硬件编码选择GPU序号
		int				nEncodeVideoGPU;
		// codec type [0-1] 0:h264,1:h265
		int				nCodecType;
		//codec compress level [0-6]
		int				nCodecLevel;
		// 服务器ip（包括端口）
		std::string		strServerIp;
		// 网页地址（全地址）
		std::string		strWebUrl;
		// 调试工具服务器ip（包括端口）
		std::string		strToolIp;
		// 项目自己的配置信息
		std::string		strProjectCustomConfig;
		// token验证服务器IP
		std::string		strTokenAccessIP;
		// 是否显示右下角托盘图标(只在云渲染有效)
		bool			bShowServerTray;
		// 是否显示程序窗口(云渲染模式才生效)
		bool			bShowMainWindow;
		// 是否显示水印
		bool			bWaterMark;
		// 是否是第一次安装DEMO
		bool			bDemo;
		// 是否允许休眠
		bool			bAutoSleep;
		// 多长事件进入休眠/秒
		long 			lAutoSleepWaitTime;
		// 是否是数据订阅的服务端
		bool 			bDataSubscribeServer;
		// 调试工具用到的项目名称
		std::string		strToolProjectName;
		/*同一设备同时运行的程序个数，对每个client进行cpu线程数分配参数*/
		//同时运行的client数量
		int nRunningClientCount;
		//自身client的索引，通过application_name 里配置的来解析，比如TZSJPT:1
		int nSelfClientIndex;	
		// 主线程开始核
	//	int nCpuMainThreadProcessorsBegin;
		//主线程占用核数
		int nCpuMainProcessorsCount;
		//自身client cpu开始线程索引
		int nCpuProcessorsBegin;
		//自身client cpu结束线程索引
		int nCpuProcessorsEnd;
		//cpu总的线程数
		int nCpuAllProcessorsNum;

		s_BootConfig() :	
			strEngineVersion(""),
			nUpdateFPS(30),
			bFixedFPS(true),
			bShowLogin(false),
			nHttpTimeOut(15),
			nHttpRetryCount(1),
			nSceneWidth(1920),
			nSceneHeight(1078),
			bScaleOutputSize(false),
			nOutputWidth(1920),
			nOutputHeight(1078),
			bFullScreen(false),
			bShowFrameTime(false),
			bShowIrradianceNode(false),
			bBackgroundRender(false),
			nBackgroundLoadFPS(120),
			strApplicationName("Project"),
			strApplicationVersion("0.0.0"),
			strUserName("default"),
			strSceneFile("default.tw4"),
			strFontName("black_72"),
			strSDFFontName(""),
            bHardEncode(true),
			strWebRTCServerHost("127.0.0.1:6001"),
			nQtCmdPort(8090),
			strScriptFile("resource/system.script"),
			strServerIp("127.0.0.1"),
			strToolIp(""),
			strWebUrl(""),
			strTokenAccessIP("127.0.0.1"),
			bShowServerTray(true),
			bShowMainWindow(false),
			nEncodeVideoGPU(0),
			bWaterMark(true),
			bDemo(false),
			bAutoSleep(false),
			lAutoSleepWaitTime(300),
			bDataSubscribeServer(true),
			nCpuAllProcessorsNum(1),
			nCpuProcessorsBegin(0)
		{

		}	
	}BootConfig;

	typedef struct s_PathConfig
	{
	public:
		std::string	strAppDir;
		std::string	strDataDir;
		std::string strUserDir;
		std::string strProjectDir;
		std::string strProjectDirName;
		std::string strScenesDir;
		std::string strScenesDirName;
		std::string strModelsDir;
		std::string strModelsDirName;
		std::string	strTexturesDir;
		std::string strTexturesDirName;
		std::string	strResourceDir;
		std::string	strResourceDirName;
		std::string strFontsDirName;
		std::string strTempDir;
		std::string strTempDirName;

		s_PathConfig() :
			strFontsDirName("fonts"),
			strModelsDirName("models"),
			strScenesDirName("scenes"),
			strTexturesDirName("textures"),
			strResourceDirName("resource"),
			strTempDirName("temp")
		{

		}	
	}PathConfig;

	typedef struct s_CubeImageSixFace
	{
		std::string strTop;
		std::string strFront;
		std::string strRight;
		std::string strBack;
		std::string strLeft;
		std::string strBottom;
	}CubeImageSixFace;

	typedef struct s_SceneFogParam
	{
		// 0 不雾化  1 线性雾化  2 指数雾化
		int		nType;
		// 基础颜色
		Vector3 vBaseColor;
		// 颜色
		Vector4 vFogColor;
		// 强度
		float	fIntensity;
		// 起始高度
		float	fHeight;
		// 高度衰减值
		float	fHeightFallOff;
		// 指数级雾距离衰减
		float	fDistanceFallOff;
		// 起始距离
		float	fStart;
		// 结束距离
		float	fEnd;
		// 最大半径
		float	fMaxRad;
		// 太阳光方向
		Vector3 vLightDirection;
		// 太阳光颜色
		Vector3 vLightColor;
		// 太阳散射率
		float	fLightInscatter;
		// 是否受光照影响
		bool	bLightEnable;
		// 是否有光
		int		nLightEnable;
		// 是否根据动态天空变化
		bool	bDynamicSky;
		
		s_SceneFogParam() :
			nType(FOG_EXPONENT),
			fMaxRad(-1),
			fHeight(0.0f),
			fStart(500.0f),
			fEnd(2000.0f),
			fIntensity(1.0f),
			bDynamicSky(true),
			bLightEnable(true),
			nLightEnable(1),
			fLightInscatter(3.0f),
			fHeightFallOff(0.0002f),
			fDistanceFallOff(0.00002),
			vBaseColor(Vector3(0.314, 0.498, 1.000)),
			vFogColor(Vector4(1.0f)),
			vLightColor(Vector3(1.0f, 1.0f, 1.0f)),
			vLightDirection(Vector3(0.0f, 1.0f, -1.0f))
		{

		}
	}SceneFogParam;

	typedef struct s_RenderConfig
	{
		// 天空图
		CubeImageSixFace	sSkyImageFile;
		// 场景雾
		SceneFogParam		sSceneFog;
		// 场景时间
		float				fSceneTime;
		// 太阳偏转角
		float				fSunLightAngle;
		// 每帧编译时间限制 /毫秒
		float				fCompileTimePerFrame;

		int             iCompileMaxTriangleCount;
		int             iCompileMaxNodeCount;
		int             iShellNodeLoadDistance;
		// 每帧释放时间限制 /毫秒
		float				fReleaseTimePerFrame;
		// 渲染质量
		RenderQualityType_e	eRenderQuality;
		// 球型反射数量
		int					nCubeReflectCount;
		// 球型反射质量
		CubeMapQuality_e	eCubeMapQuality;
		// 天空反射质量
		SkyMapQuality_e		eSkyMapQuality;
		// 阴影数量
		DynamicShadowType_e eDynamicShadowType;
		// 多层级阴影数量
		int					nCSMShadowCount;
		// 多层级阴影最远距离
		float				fCSMShadowDistanceMax;
		// 是否使用固定CSM远近平面
		bool				bCSMStableClip;
		// 天空遮罩反射地面颜色
		Vector3				vSkyOcclusionGroundColor;
		// 天空颜色
		Vector3				vSkyColor;
		// 间接光颜色
		Vector3				vIndirectColor;
		// 辉光深度剔除
		bool				bGlowDepthEnable;
		// 是否每一帧都更新阴影
		bool				bFrameUpdateShadow;
		// 动态光源最大值
		int					nDynamicLightMax;
		// 固定光源最大值
		int					nStationaryLightMax;
		// 泛光阈值
		float				fBloomThreshold;
		// 泛光强度
		float				fBloomIntensity;
		// 十字泛光阈值
		float				fCrossFlareThreshold;
		// SSAO采样核数量
		int					nSSAOKernelCount;
		// SSAO半径
		float				fSSAORadius;
		// SSAO强度
		float				fSSAOIntensity;
		// 景深起始距离
		float				fDepthOfFiledStart;
		// 景深衰减系数
		float				fDepthOfFiledFallOff;
		// 倾斜摄影模型是否开启mipmap
		bool				bPagedNodeMipmap;
		// 倾斜摄影模型显示距离是否根据高度动态变化
		bool				bPagedNodeVisibleDynamic;
		// 倾斜摄影模型高度动态变化比值
		float				fPagedNodeVisibleDynamicScale;
		// 倾斜摄影模型显示最近距离
		float				fPagedNodeMinVisible;
		// 倾斜摄影模型显示最远距离
		float				fPagedNodeMaxVisible;
		// 倾斜摄影模型加载层级最远距离
		float				fPagedNodeMaxRange;
		// 倾斜摄影模型加载精度比例 基础比例为1 值越高显示精度越低
		float				fPagedNodeRangeScale;
		// 倾斜摄影模型是否接受光照
		bool				bPagedNodeAcceptLight;
		// 倾斜摄影模型是否启用后期处理
		bool				bPagedNodePostProcess;
		// 是否动态加载倾斜摄影场景文件
		bool				bPagedNodeLoadSceneDynamic;
		// 是否动态释放倾斜摄影场景文件
		bool				bPagedNodeFreeSceneDynamic;
		// 是否深度优先加载倾斜摄影
		bool				bPagedNodeLoadDepthFirst;
		// 倾斜摄影模型贴图颜色调整 x、y、z为增加值，w为次幂
		Vector4				vPagedNodeToneColor;
		// 每帧释放倾斜摄影时间 毫秒
		float				fPagedNodeReleaseTimePerFrame;
		// 倾斜摄影池大小
		int					nPagedNodePoolCount;
		// 瓦片模型是否接受光照
		bool				bTiledNodeAcceptLight;
		// 瓦片模型是否启用后期处理
		bool				bTiledNodePostProcess;
		// 是否启用瓦片地图
		bool				bTiledNodeEnable;
		// 瓦片地图配置
		XMLNodePtr			pTiledXmlNode;
		// 初始化Model池大小
		int					nModelPoolCount;
		// 场景选中模式
		SelectedType_e		eSelectType;
		// 选中线粗细
		float				fSelectLineWidth;
		// 选中物体的颜色
		Vector4				vSelectColor;
		// 场景悬停模式
		HoverType_e			eHoverType;
		// 悬停线粗细
		float				fHoverLineWidth;
		// 悬停物体颜色
		Vector4				vHoverColor;
		// 相机按键移动速度
		float				fCameraKeyMoveSpeed;
		// 相机按键旋转速度
		float				fCameraKeyRotateSpeed;
		// 相机限制区域
		BoundingSphered		cCameraBoundSphere;
		// 相机高度限制
		float				fSceneLimitHeight;
		// 场景基准高度
		float				fSceneBaseHeight;
		// 视频融合宽高
		Vector2				vVideoFusionSize;
		// 是否启用自动曝光
		bool				bAutoExposure;
		// 曝光系数
		float				fExposureScale;
		// 自动曝光亮度最大阈值
		float				fLuminanceThresholdMax;
		// 自动曝光亮度最小阈值
		float				fLuminanceThresholdMin;
		// 天光曝光系数
		float				fSkyExposureScale;
		// 天光颜色
		Vector3				vSkyOcclusionColor;
		// 天空反射强度
		float				fSkyOcclusionIntensity;
		// 是否使用LUT色调映射
		bool				bUseToneMappingLUT;
		// LUT图片路径
		std::string			strToneMappingLUTFile;
		// 是否使用LUT调色
		bool				bUseColorGradingLUT;
		// LUT图片路径
		std::string			strColorGradingLUTFile;
		// 模糊蒙版
		BlurMaskMode_e		eBlurMaskMode;
		bool				bMaskBlur;
		float				fBlurMaskRadius;
		float				fBlurMaskBase;
		// Square模式下边距 左 上 右 下
		Vector4				vBlurMaskMargin;
		// 蒙版颜色
		Vector4				vMaskColor;
		// 图片蒙版路径
		std::string			strImageMaskFile;
		// 是否启用TXAA抗抖动
		bool				bTXAAAntiFlicker;
		// 是否启用TXAA混合色调映射
		bool				bTXAAMixToneMap;
		// TXAA锐利程度
		float				fTXAASharpness;
		// 鼠标工具状态
		int					nMouseToolType;
		// 是否启用静态合并批次
		bool				bStaticMergeBatch;
		// 色彩滤镜
		// 色调
		float				fColorHue;
		// 饱和度
		float				fColorSaturation;
		// 明度
		float				fColorValue;
		// 对比度
		float				fColorContrast;
		// 亮度
		float				fColorBrightness;
		// Light Shaft 亮度阈值
		float				fGodRayLuminanceThreshold;
		// Light Shaft 颜色
		Vector3				vGodRayColorTint;
		s_RenderConfig() :
			fSSAORadius(0.5),
			fSSAOIntensity(0.5),
			nSSAOKernelCount(32),
			nDynamicLightMax(8),
			nStationaryLightMax(0),
			fSceneTime(16.0),
			fSunLightAngle(0.0),
			fBloomThreshold(0.0),
			iCompileMaxTriangleCount(500000),
			iCompileMaxNodeCount(10000),
			iShellNodeLoadDistance(1000),
			fBloomIntensity(1.0),
			fCompileTimePerFrame(4.0f),
			fReleaseTimePerFrame(1.0f),
			fCrossFlareThreshold(0.95),
			fDepthOfFiledStart(50000),
			fDepthOfFiledFallOff(0.0001),
			fCameraKeyMoveSpeed(10.0),
			fCameraKeyRotateSpeed(10.0),
			fSceneBaseHeight(0.0),
			fSceneLimitHeight(0.0),
			bGlowDepthEnable(false),
			bFrameUpdateShadow(true),
			bAutoExposure(true),
			fExposureScale(1.0),
			fLuminanceThresholdMax(10.0),
			fLuminanceThresholdMin(0.0),
			fSkyExposureScale(1.0),
			fSkyOcclusionIntensity(1.0),
			fSelectLineWidth(2.0),
			eSelectType(SelectBox),
			fHoverLineWidth(2.0),
			eHoverType(HoverMesh),
			bCSMStableClip(true),
			nCSMShadowCount(4),
			fCSMShadowDistanceMax(500),
			vVideoFusionSize(1920, 1080),
			eDynamicShadowType(SHADOW_SINGLE),
			eRenderQuality(ProfessionalType),
			nCubeReflectCount(32),
			eCubeMapQuality(CUBE_MAP_QUALITY_LOW),
			eSkyMapQuality(SKY_MAP_QUALITY_MEDIUM),
			fPagedNodeMaxRange(9999999),
			fPagedNodeMaxVisible(9999999),
			fPagedNodeMinVisible(0),
			bPagedNodeVisibleDynamic(false),
			fPagedNodeVisibleDynamicScale(1.0),
			bPagedNodeMipmap(false),
			bPagedNodeAcceptLight(false),
			bPagedNodePostProcess(false),
			fPagedNodeRangeScale(1.0f),
			bPagedNodeLoadSceneDynamic(false),
			bPagedNodeFreeSceneDynamic(true),
			bPagedNodeLoadDepthFirst(true),
			fPagedNodeReleaseTimePerFrame(2.0f),
			bTiledNodeAcceptLight(false),
			bTiledNodePostProcess(false),
			bTiledNodeEnable(false),
			pTiledXmlNode(nullptr),
			nPagedNodePoolCount(1000000),
			nModelPoolCount(0),
			eBlurMaskMode(BLUR_MASK_MODE_CIRCLE),
			fBlurMaskRadius(0.646),
			fBlurMaskBase(0.0),
			vBlurMaskMargin(0.646, 0.646, 0.646, 0.0),
			bMaskBlur(true),
			vMaskColor(Vector4(0.0, 0.0, 0.0, 0.6)),
			cCameraBoundSphere(Vector3d(0.0), 9999999),
			vPagedNodeToneColor(Vector4(0.0, 0.0, 0.0, 1.0)),
			vHoverColor(Vector4(0.949, 0.988, 1.0, 0.3)),
			vSelectColor(Vector4(0.949, 0.988, 1.0, 0.6)),
			vIndirectColor(Vector3(0.0)),
			vSkyOcclusionColor(Vector3(1.0)),
			vSkyOcclusionGroundColor(Vector3(0.0)),
			vSkyColor(Vector3(0.310, 0.420, 0.906)),
			bUseToneMappingLUT(false),
			bTXAAAntiFlicker(false),
			bTXAAMixToneMap(true),
			fTXAASharpness(0.5),
			strToneMappingLUTFile("resource/extra/lut/tonelut.cube"),
			bUseColorGradingLUT(false),
			strColorGradingLUTFile(""),
			strImageMaskFile(""),
			bStaticMergeBatch(true),
			fColorHue(0.0f),
			fColorSaturation(0.0f),
			fColorValue(0.0f),
			fColorContrast(0.0f),
			fColorBrightness(0.0f),
			fGodRayLuminanceThreshold(10.0f),
			vGodRayColorTint(Vector3(1.0)),
			nMouseToolType(MouseToolType_e::MOUSE_TOOL_SELECT)
		{
			
		}

		int GetDynamicShadowMax()
		{
			int nMax = 0;
			switch (eDynamicShadowType)
			{
			case bc::DynamicShadowType_e::SHADOW_NONE:
				nMax = 0;
				break;
			case bc::DynamicShadowType_e::SHADOW_SINGLE:
				nMax = 1;
				break;
			case bc::DynamicShadowType_e::SHADOW_MULTIPLE:
				nMax = 4;
				break;
			default:
				nMax = 0;
				break;
			}

			return nMax;
		}
	
	}RenderConfig;
	typedef struct s_MapKey
	{
		int lod;
		int x; int y;
		s_MapKey():
			lod(-1),x(0),y(0)
		{

		}
	}MapKey;

	typedef enum class s_SceneMode
	{
		FLAT,
		EARTH,
	}SceneMode_e;

	class ProjectConfig
	{
	public:
		ProjectConfig()
		{
			strConfigFile = "appconfig.conf";
		}
	public:
		void SetDefault()
		{
			BootConfig t1;
			sBootConfig = t1;

			PathConfig t2;
			sPathConfig = t2;

			ShaderConfig t3;
			sShaderConfig = t3;

			RenderConfig t4;
			sRenderConfig = t4;
		}
		// 配置文件名称
		std::string			strConfigFile;
		// 启动参数
		BootConfig			sBootConfig;
		// 资源路径参数
		PathConfig			sPathConfig;
		// Shader定义
		ShaderConfig		sShaderConfig;
		// 渲染相关设置
		RenderConfig		sRenderConfig;
	};

	typedef struct s_RenderInfo
	{
		int			nFPS;			// 帧率
		float		fFrameTime;		// 一帧时间 单位：秒
		float		fRenderTime;	// 渲染运行时间 单位：秒

		s_RenderInfo()
		{
			nFPS = 0;
			fFrameTime = 0.033;
			fRenderTime = 0.0;
		}
	}RenderInfo;

	class IEngineAPI;
	class IRenderAPI;
	class IModelAPI;
	class ICoreAPI;
	class IProtocolAPI;
	class IProjectBasedAPI;
	class IProjectClientAPI;
	typedef struct _SystemAPI_t
	{
		IEngineAPI*			iEngineAPI;
		IRenderAPI*			iRenderAPI;
		IModelAPI*			iModelAPI;
		ICoreAPI*			iCoreAPI;
		IProtocolAPI*		iProtocolAPI;
		IProjectBasedAPI*	iProjectBasedAPI;
		IProjectClientAPI*	iProjectClientAPI;
		_SystemAPI_t()
		{
			iEngineAPI = nullptr;
			iRenderAPI = nullptr;
			iModelAPI = nullptr;
			iCoreAPI = nullptr;
			iProtocolAPI = nullptr;
			iProjectBasedAPI = nullptr;
			iProjectClientAPI = nullptr;
		}

		~_SystemAPI_t()
		{
			iEngineAPI = nullptr;
			iRenderAPI = nullptr;
			iModelAPI = nullptr;
			iCoreAPI = nullptr;
			iProtocolAPI = nullptr;
			iProjectBasedAPI = nullptr;
			iProjectClientAPI = nullptr;
		}

		struct _SystemAPI_t& operator = (const struct _SystemAPI_t& rhs)
		{
			iEngineAPI = rhs.iEngineAPI;
			iRenderAPI = rhs.iRenderAPI;
			iModelAPI = rhs.iModelAPI;
			iCoreAPI = rhs.iCoreAPI;
			iProtocolAPI = rhs.iProtocolAPI;
			iProjectBasedAPI = rhs.iProjectBasedAPI;
			iProjectClientAPI = rhs.iProjectClientAPI;
			return *this;
		}
	}ISystemAPI;

	class APIProvider
	{
	public:
		APIProvider(ISystemAPI* pSystemAPI)
		{
			m_pSystemAPI = pSystemAPI;
		}

		virtual ~APIProvider() {}

		/*
		 *	获取SystemAPI
		 */
		static ISystemAPI* GetSystemAPI() { return m_pSystemAPI; }
	protected:
		static ISystemAPI* m_pSystemAPI;

	};

	class ThreadTaskParam;
	class BCThreadPool;
	class BCThreadManager;
	class IEngineAPI
	{
	public:

		/*
		 *	设置场景文件
		 */
		virtual void		iSetSceneFile(const std::string& strFile) = 0;

		/*
		 *	重新加载
		 */
		virtual void		iReload() = 0;

		virtual void      iShutDown() = 0;

		/*
		 *	运行引擎渲染一帧
		 */
		virtual void		iRunFrame() = 0;

		/*
		 *	鼠标事件
		 *  nState	[in] 鼠标事件类型
		 *  nMouseX	[in] 鼠标位置 X
		 *  nMouseY	[in] 鼠标位置 Y
		 */
                virtual void		iMouseEvent(const MouseState_e& nState, const int& nMouseX, const int& nMouseY,const uint nKeyCode=0) = 0;

		/*
		 *	键盘事件
		 *  nKeyCode	[in] 键盘按键键值 使用 ASCII 与 Virtual Keys
		 *  bDown		[in] 按下或抬起
		 */
		virtual void		iKeyEvent(const uint& nKeyCode, const bool& bDown) = 0;

		/*
		 *	添加事件处理器
		 *  pProcessor	[in] 事件处理器
		 */
		virtual void		iAddEventProcessor(IEventProcessor* pProcessor) = 0;

		/*
		 *	添加帧更新模块
		 *  pModule		[in] 更新模块
		 */
		virtual void		iAddFrameUpdateModule(IFrameUpdateModule* pModule) = 0;

		/*
		 *	添加一个事件
		 *  tEvent		[in] 事件消息
		 */
		virtual void		iExecuteEvent(const BCEvent& tEvent) = 0;

		/*
		 *	获取线程池
		 *  return	[out]	线程池
		 */
		virtual BCThreadPool*		iGetThreadPool() = 0;

		/*
		 *	获取线程管理器
		 *  return	[out]	线程管理器
		 */
		virtual BCThreadManager*	iGetThreadManager() = 0;

		/*
		 *	获取项目配置属性
		 *  return	[out]	项目的配置属性
		 */
		virtual ProjectConfig&	iGetProjectConfig() = 0;

		/*
		 *	获取渲染信息
		 *  return	[out]	渲染信息
		 */
		virtual RenderInfo&		iGetRenderInfo() = 0;

		/*
		*	设置项目路径
		*/
		virtual bool			iSetPathConfig(const std::string& strXml) = 0;

		/*
		*	读取3D渲染配置信息
		*/
		virtual bool			iLoadConfig(const std::string& strXml = "") = 0;

		virtual std::string		iGetResourceAbsolutePath(const std::string& strPath) = 0;

		virtual void         iBindKey(KeyBoardType type, int value) = 0;

		virtual const std::unordered_map<KeyBoardType, int>& iGetKeyMap() = 0;
	};
}
