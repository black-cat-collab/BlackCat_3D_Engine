#pragma once

namespace bc
{
	enum ShaderType_e
	{
		SHADER_TYPE_COMMON_DATA = 0,			// 更新基础数据
		SHADER_TYPE_BASE_COLOR,					// 基础颜色
		SHADER_TYPE_BASE_MAP,					// 渲染漫反射贴图
		SHADER_TYPE_BASE_MAP_GAMMA_CORRECT,		// 渲染漫反射贴图带Gamma校正
		SHADER_TYPE_LIGHT_MAP,					// 渲染灯光贴图
		SHADER_TYPE_EMISSIVE_MAP,				// 渲染带自发光
		SHADER_TYPE_DEPTH_MAP,					// 渲染深度图
		SHADER_TYPE_FRONT_RENDER,				// 正向渲染颜色
		SHADER_TYPE_G_BUFFER,					// 多目标渲染
		SHADER_TYPE_BLOOM_HDR,					// 泛光HDR提取
		SHADER_TYPE_BLOOM_BLEND,				// 泛光混合
		SHADER_TYPE_CROSS_FLARE_HDR,			// 十字泛光HDR提取
		SHADER_TYPE_CROSS_FLARE_BLEND,			// 十字泛光混合
		SHADER_TYPE_GAUSS_BLUR_H,				// 横向高斯模糊
		SHADER_TYPE_GAUSS_BLUR_V,				// 纵向高斯模糊
		SHADER_TYPE_CROSS_BLUR_H,				// 横向十字模糊
		SHADER_TYPE_CROSS_BLUR_V,				// 纵向十字模糊
		SHADER_TYPE_DOWN_SAMPLE_BLUR,			// 混合降采样
		SHADER_TYPE_DOWN_SAMPLE_HIZ_MAX,		// Hiz降采样
		SHADER_TYPE_DOWN_SAMPLE_HIZ_MIN,		// Hiz降采样
		SHADER_TYPE_GLOW_COLOR,					// 辉光颜色图片
		SHADER_TYPE_AVERAGE_TRANSPARENT_COLOR,	// 平均透明算法，只计算颜色
		SHADER_TYPE_AVERAGE_TRANSPARENT,		// 平均透明算法
		SHADER_TYPE_AVERAGE_TRANSPARENT_BLEND,	// 平均透明算法
		SHADER_TYPE_YUV,						// 绘制YUV视频
		SHADER_TYPE_YUV_GAMMA_CORRECT,			// 绘制YUV视频 gamma矫正
		SHADER_TYPE_DEFFERED_LIGHTING,			// 延迟光照
		SHADER_TYPE_DEFFERED_SHADING,			// 延迟渲染
		SHADER_TYPE_FXAA,						// FXAA
		SHADER_TYPE_FXAA_LUMINANCE,				// FXAA的alpha通道设置为亮度
		SHADER_TYPE_TXAA,						// TXAA
		SHADER_TYPE_HEAT_MAP_POINT,				// 绘制热力图点
		SHADER_TYPE_DYNAMIC_SKY,				// 动态天空
		SHADER_TYPE_RADIAL_BLUR,				// 径向模糊
		SHADER_TYPE_SHADOW_MAP,					// 阴影贴图
		SHADER_TYPE_REFLECT_LIGHTING,			// 反射时光照
		SHADER_TYPE_IBL_IRRADIANCE,				// 绘制IBL的辐射度图
		SHADER_TYPE_IBL_PREFILTER,				// 绘制IBL预处理
		SHADER_TYPE_IBL_BRDF,					// 绘制IBL的BRDF
		SHADER_TYPE_SSSHADOW,					// 屏幕空间阴影
		SHADER_TYPE_SSAO,						// 屏幕空间AO
		SHADER_TYPE_SSAO_BLEND,					// 屏幕空间AO混合
		SHADER_TYPE_GTAO,						// GroundTruthAO
		SHADER_TYPE_SSR,						// 屏幕空间反射
		SHADER_TYPE_VIEWSHED_ANALYSIS,			// 视域分析
		SHADER_TYPE_SUN_GOD_RAY_SKY,			// 太阳体积光天空
		SHADER_TYPE_SUN_GOD_RAY_RADIAL_BLUR,	// 太阳体积光径向模糊
		SHADER_TYPE_SUN_GOD_RAY_BLEND,			// 太阳体积光混合
		SHADER_TYPE_AVERAGE_LUMINANCE,			// 平均亮度
		SHADER_TYPE_TONE_MAPPING,				// 色调映射
		SHADER_TYPE_COLOR_FILTER,				// 色彩滤镜
		SHADER_TYPE_DEPTH_OF_FILED,				// 景深
		SHADER_TYPE_RGB_SPLIT_GLITCH,			// RGB故障
		SHADER_TYPE_PARTICLE_FEEDBACK,			// 粒子回调
		SHADER_TYPE_PARTICLE,					// 绘制粒子
		SHADER_TYPE_BLUR_MASK,					// 高斯模糊蒙版
		SHADER_TYPE_CUSTOM_DEPTH,				// 自定义深度
		SHADER_TYPE_BORDER,						// 绘制边缘
		SHADER_TYPE_BORDER_BLEND,				// 与边缘混合
		SHADER_TYPE_SOBEL_EDGE,					// Sobel边缘检测
		SHADER_TYPE_GRID,						// 绘制网格
		SHADER_TYPE_VERTEX_COLOR,						// 顶点颜色
		SHADER_TYPE_DEFFERED_COLOR_DEPTH,		// 绘制延迟渲染颜色和深度
		SHADER_TYPE_SDF_FONT,					// 绘制SDF文字所需信息
		SHADER_TYPE_UI_WIDGET,					// 绘制UI
		SHADER_TYPE_HDR_CUBE,					// 将HDR转为CUBE
		SHADER_TYPE_POLYGON_MESH,
		SHADER_TYPE_NUM							// 结束
	};

	class IGLShader
	{
	public:
		// 获取Shader的类型
		virtual ShaderType_e	iGetShaderType() = 0;
		// 获取Shader名字
		virtual std::string		iGetName() = 0;
	};
}

