#pragma once

#include <string>

namespace bc
{
	enum ViewType_e
	{
		VIEW_TYPE_NONE = 0,
		VIEW_TYPE_SCENE_VIEW,						// 用于显示主视图
		VIEW_TYPE_EFFECT_VIEW,						// 特效视图
		VIEW_TYPE_GBUFFER,							// 绘制GBuffer
		VIEW_TYPE_HIZ,								// 绘制HiZ
		VIEW_TYPE_GAUSS_BLUR,						// 高斯模糊
		VIEW_TYPE_CROSS_BLUR,						// 十字模糊
		VIEW_TYPE_RADIAL_BLUR,						// 径向模糊
		VIEW_TYPE_SUN_GOD_RAY,						// 太阳体积光
		VIEW_TYPE_BLOOM,							// 泛光
		VIEW_TYPE_CROSS_FLARE,						// 十字泛光
		VIEW_TYPE_GLOW,								// 辉光
		VIEW_TYPE_SKY_BLOOM,						// 天空泛光
		VIEW_TYPE_HEAT_MAP,							// 热力图
		VIEW_TYPE_AVERAGE_TRANSPARENT,				// 平均透明
		VIEW_TYPE_SHADOW_RENDER,					// 阴影
		VIEW_TYPE_IBL_RENDER,						// IBL
		VIEW_TYPE_IRRADIANCE_RENDER,				// 辐照度
		VIEW_TYPE_CUBE_REFLECT_MAP_RENDER,			// 球形反射
		VIEW_TYPE_PLANE_REFLECT_MAP_RENDER,			// 平面反射
		VIEW_TYPE_GBUFFER_RENDER,					// G-Buffer
		VIEW_TYPE_DEFFERED_LIGHTING_RENDER,			// 延迟光照
		VIEW_TYPE_DEFFERED_SHADING_RENDER,			// 延迟渲染
		VIEW_TYPE_SKY_OCCLUSION_RENDER,				// 天空反射
		VIEW_TYPE_SSAO_RENDER,						// 屏幕空间AO
		VIEW_TYPE_GTAO_RENDER,						// GroundTruthAO
		VIEW_TYPE_SCREEN_SPACE_REFLECT_RENDER,		// 屏幕空间反射
		VIEW_TYPE_DEPTH_OF_FILED,					// 景深
		VIEW_TYPE_VIEWSHED_ANALYSIS,				// 视域分析
		VIEW_TYPE_VIDEO_FUSION,						// 视频融合
		VIEW_TYPE_FXAA,								// FXAA
		VIEW_TYPE_TXAA,								// TXAA
		VIEW_TYPE_TONE_MAPPING,						// 色调映射
		VIEW_TYPE_COLOR_FILTER,						// 色彩滤镜
		VIEW_TYPE_SUN_LENS_FLARE,					// 镜头光晕
		VIEW_TYPE_RESOLUTION,						// 分辨率修改
		VIEW_TYPE_RGB_SPLIT_GLITCH,					// 故障后处理 RGB分离
		VIEW_TYPE_IMAGE_MASK,						// 图片蒙版
		VIEW_TYPE_BLUR_MASK,						// 高斯模糊蒙版
		VIEW_TYPE_COLOR_TRANSITION,					// 颜色转场
		VIEW_TYPE_HOVER_BORDER,						// 悬停边缘高亮
		VIEW_TYPE_SELECT_BORDER,					// 选中边缘高亮
		VIEW_TYPE_COORDINATE_AXIS,					// 坐标轴
		VIEW_TYPE_USER_INTERFACE,					// UI界面
		VIEW_TYPE_ENVIRONMENT_CUBE_REFLECT,			// HDR 转 Cube
		VIEW_TYPE_POI,								// POI
		VIEW_TYPE_POLYGON_MESH,                 //路网
		VIEW_TYPE_END
	};

	//enum ViewOperateStatus_e						// 操作状态
	//{
	//	OPERATE_STATUS_SELECT = 0,					// 选择
	//	OPERATE_STATUS_CAMERA_TO_POINT,				// 相机跳转
	//	OPERATE_STATUS_ANIMATION_PLAY,				// 漫游播放
	//	OPERATE_STATUS_MEASURE,						// 测量 
	//	OPERATE_STATUS_TRIANGLE_SELECT,				// 框选模型
	//	OPERATE_STATUS_NO_OPERATE,					// 禁止操作相机
	//	OPERATE_STATUS_SPLIT_STOREY,					// 切割层
	//	OPERATE_STATUS_SPLIT_APARTMENT,					// 切割户室
	//	OPERATE_TYPE_END							// 结束
	//};

	enum SceneViewTextureType_e
	{
		FRONT_RENDER_TYPE_COLOR = 0,			// 正向渲染颜色
		GBUFFER_TYPE_DIFFUSE_COLOR,				// 片段漫反射颜色
		GBUFFER_TYPE_EMISSIVE_COLOR,			// 片段自发光颜色
		GBUFFER_TYPE_VERTEX_WS,					// 世界空间的顶点坐标
		GBUFFER_TYPE_NORMAL_WS,					// xyz：世界空间的法线 
		GBUFFER_TYPE_NORMAL_VS,					// xyz：观察空间法线
		GBUFFER_TYPE_VELOCITY,					// xy:场景的速度 z:深度 w:光照模式
		GBUFFER_TYPE_MATERIAL,					// 材质参数  x：PBR金属度 y：PBR粗糙度 z：PBR AO
		GBUFFER_TYPE_STATIC_SHADOW,				// 静态阴影贴图
		GBUFFER_TYPE_PREVIOUS_VELOCITY			// 上一帧场景的速度
	};

	enum ViewComponentType_e
	{
		VIEW_COMPONENT_TYPE_VIEWSHED_ANALYSIS = 0,
		VIEW_COMPONENT_TYPE_HEAT_MAP,
		VIEW_COMPONENT_TYPE_VIDEO_FUSION,
		VIEW_COMPONENT_TYPE_SKY_OCCLUSION,
		VIEW_COMPONENT_TYPE_CUBE_REFLECT,
		VIEW_COMPONENT_TYPE_HIZ,
		VIEW_COMPONENT_TYPE_SHADOW,
		VIEW_COMPONENT_TYPE_SCREEN_SPACE_AO,
		VIEW_COMPONENT_TYPE_GROUND_TRUTH_AO,
		VIEW_COMPONENT_TYPE_SCREEN_SPACE_REFLECT,
		VIEW_COMPONENT_TYPE_DEFFERED_LIGHTING,
		VIEW_COMPONENT_TYPE_RGB_SPLIT_GLITCH,
		VIEW_COMPONENT_TYPE_COLOR_TRANSITION,
		VIEW_COMPONENT_TYPE_COORDINATE_AXIS,
		VIEW_COMPONENT_TYPE_USER_INTERFACE,
		VIEW_COMPONENT_TYPE_POI,
		VIEW_COMPONENT_TYPE_POLYGON_MESH,
		VIEW_COMPONENT_TYPE_END
	};

	enum ViewDriction_e
	{
		TOP_VIEW = 0,
		BOTTOM_VIEW,
		LEFT_VIEW,
		RIGHT_VIEW,
		FRONT_VIEW,
		BACK_VIEW,
		PROJECTION_VIEW,
		EDITOR_VIEW,
		EDITOR_RIGHT_VIEW
	};

	class IViewshedAnalysis;
	class IHeatMap;
	class IVideoFusion;
	class ISkyOcclusion;
	class IShadowView;
	class IDefferedLighting;
	class IRGBSplitGlitch;
	class IColorTransition;
	class IHiZView;
	class ISSAOView;
	class IGTAOView;
	class ICoordinateAxisView;
	class IPOIView;
	class ICubeReflectView;
	class IVirtualRelationship;
	class IViewComponent
	{
	public:
		virtual int iGetViewComponentType() = 0;
	};

	class ICamera;
	class IFrameBuffer;
	class IView
	{
	public:
		/*
		*	获取视图ID
		*	return  [out]  视图ID
		*/
		virtual std::string		iGetID() = 0;

		/*
		*	获取视图类型
		*	return  [out]  视图类型
		*/
		virtual ViewType_e		iGetViewType() = 0;

		/*
		*	设置视图是否可视化
		*	bVisible  [in]  视图是否可视化
		*/
		virtual void			iSetVisible(const bool& bVisible) = 0;

		/*
		*	判断视图是否可视化
		*	return  [out]  是否可视化
		*/
		virtual bool			iIsVisible() = 0;

		/*
		*	获取主相机
		*	return  [out]  主相机
		*/
		virtual INode* iGetMainCamera() = 0;
		/*
		*	获取编辑相机
		*	return  [out]  主相机
		*/
		virtual INode* iGetEditorCamera() = 0;
		/*
		*	获取视图宽度
		*	return  [out]  视图宽度
		*/
		virtual int				iGetViewWidth() = 0;

		/*
		*	获取视图高度
		*	return  [out]  视图高度
		*/
		virtual int				iGetViewHeight() = 0;

		/*
		*	设置视图大小
		*	nWidth   [in]  视图宽度
		*	nHeight  [in]  视图高度
		*/
		virtual void			iSetViewSize(const int& nWidth, const int& nHeight) = 0;

		/*
		*	获取输出宽度
		*	return  [out]  输出宽度
		*/
		virtual int				iGetOutputWidth() = 0;

		/*
		*	获取输出高度
		*	return  [out]  输出高度
		*/
		virtual int				iGetOutputHeight() = 0;

		/*
		*	设置输出大小
		*	nWidth   [in]  宽度
		*	nHeight  [in]  高度
		*/
		virtual void			iSetOutputSize(const int& nWidth, const int& nHeight) = 0;

		/*
		*	添加视图组件
		*	nType		[in]  视图类型
		*	pComponent  [in]  视图组件
		*/
		virtual void			iAddViewComponent(const int& nType, IViewComponent* pComponent) = 0;

		/*
		*	获取视图组件
		*	nType			[in]   视图类型
		*	IViewComponent  [out]  视图组件
		*/
		virtual IViewComponent* iGetViewComponent(const int& nType) = 0;

		/*
		*	获取帧缓冲
		*	return  [out]  帧缓冲对象
		*/
		virtual IFrameBuffer* iGetFrameBuffer() = 0;

		/*
	*	nType [in] 设置操作状态
	*/
		virtual void iSetOperateType(uint nType) = 0;
		/*
		 *	获取视图组件
		 *	return		[out]	视图组件
		 */
		virtual void iSetView(ViewDriction_e nType, const Vector3d& vViewCenter, const double& fWidth, const double& fHeight, const int& nWidth, const int& nHeight) = 0;
		virtual void iSetView(ViewDriction_e nType) = 0;

		virtual void iSaveScreen(const std::string& path) = 0;
		/*
			设置主窗口的占有比例
		*/
		virtual void  iSetMainScreenRatio(float ratio) = 0;
		virtual float iGetMainScreenRatio() = 0;

		virtual IVirtualRelationship* iGetVirtualRelationship() = 0;
		/*
			设置两个渲染窗口之间的间隙
		*/
		virtual void  iSetMainScreenLine(float width) = 0;
		virtual float iGetMainScreenLine() = 0;
		template <typename T>
		T* GetDynamicViewComponent()
		{
			if (typeid(T) == typeid(bc::IViewshedAnalysis))
			{
				return static_cast<T*>(iGetViewComponent(bc::VIEW_COMPONENT_TYPE_VIEWSHED_ANALYSIS));
			}
			else if (typeid(T) == typeid(bc::IHeatMap))
			{
				return static_cast<T*>(iGetViewComponent(bc::VIEW_COMPONENT_TYPE_HEAT_MAP));
			}
			else if (typeid(T) == typeid(bc::IVideoFusion))
			{
				return static_cast<T*>(iGetViewComponent(bc::VIEW_COMPONENT_TYPE_VIDEO_FUSION));
			}
			else if (typeid(T) == typeid(bc::ISkyOcclusion))
			{
				return static_cast<T*>(iGetViewComponent(bc::VIEW_COMPONENT_TYPE_SKY_OCCLUSION));
			}
			else if (typeid(T) == typeid(bc::ICubeReflectView))
			{
				return static_cast<T*>(iGetViewComponent(bc::VIEW_COMPONENT_TYPE_CUBE_REFLECT));
			}
			else if (typeid(T) == typeid(bc::IShadowView))
			{
				return static_cast<T*>(iGetViewComponent(bc::VIEW_COMPONENT_TYPE_SHADOW));
			}
			else if (typeid(T) == typeid(bc::IDefferedLighting))
			{
				return static_cast<T*>(iGetViewComponent(bc::VIEW_COMPONENT_TYPE_DEFFERED_LIGHTING));
			}
			else if (typeid(T) == typeid(bc::IRGBSplitGlitch))
			{
				return static_cast<T*>(iGetViewComponent(bc::VIEW_COMPONENT_TYPE_RGB_SPLIT_GLITCH));
			}
			else if (typeid(T) == typeid(bc::IColorTransition))
			{
				return static_cast<T*>(iGetViewComponent(bc::VIEW_COMPONENT_TYPE_COLOR_TRANSITION));
			}
			else if (typeid(T) == typeid(bc::IHiZView))
			{
				return static_cast<T*>(iGetViewComponent(bc::VIEW_COMPONENT_TYPE_HIZ));
			}
			else if (typeid(T) == typeid(bc::ISSAOView))
			{
				return static_cast<T*>(iGetViewComponent(bc::VIEW_COMPONENT_TYPE_SCREEN_SPACE_AO));
			}
			else if (typeid(T) == typeid(bc::IGTAOView))
			{
				return static_cast<T*>(iGetViewComponent(bc::VIEW_COMPONENT_TYPE_GROUND_TRUTH_AO));
			}
			else if (typeid(T) == typeid(bc::ICoordinateAxisView))
			{
				return static_cast<T*>(iGetViewComponent(bc::VIEW_COMPONENT_TYPE_COORDINATE_AXIS));
			}
			else if (typeid(T) == typeid(bc::IPOIView))
			{
				return static_cast<T*>(iGetViewComponent(bc::VIEW_COMPONENT_TYPE_POI));
			}
			return nullptr;
		}
	};

	typedef void(*RenderPassFunc)(IView*);

	class RenderPass
	{
	public:
		RenderPass()
		{
			nSort = 100;
			pRenderPassFunc = nullptr;
			pEffectView = nullptr;
		}

		virtual ~RenderPass() {}

		int GetSort() const { return nSort; }
		void SetSort(const int& _nSort) { nSort = _nSort; }

		RenderPass& operator = (const RenderPass& src)
		{
			nSort = src.nSort;
			pRenderPassFunc = src.pRenderPassFunc;
			pEffectView = src.pEffectView;
			return *this;
		}

	public:
		int				nSort;				// 排序
		IView* pEffectView;		// 执行渲染的View
		RenderPassFunc	pRenderPassFunc;	// 渲染回调函数

	};

	class AfterLightRenderPass : public RenderPass
	{
	public:
		AfterLightRenderPass() : RenderPass() {}
		virtual ~AfterLightRenderPass() {}

		AfterLightRenderPass& operator = (const AfterLightRenderPass& src)
		{
			RenderPass::operator=(src);

			return *this;
		}

	};

	class PostRenderPass : public RenderPass
	{
	public:
		PostRenderPass() : RenderPass() {}
		virtual ~PostRenderPass() {}

		PostRenderPass& operator = (const PostRenderPass& src)
		{
			RenderPass::operator=(src);

			return *this;
		}

	};
}

