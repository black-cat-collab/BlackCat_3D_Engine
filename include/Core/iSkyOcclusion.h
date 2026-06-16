#pragma once

namespace bc
{
	class ISkyOcclusion : public IViewComponent
	{
	public:
		virtual int iGetViewComponentType()
		{
			return VIEW_COMPONENT_TYPE_SKY_OCCLUSION;
		}
		/********************
		* 功能:
		* 获取天空辐照度贴图
		* 参数:
		* 无
		* 返回值：
		* ITexture*：贴图指针
		********************/
		virtual ITexture*	iGetSkyIrradianceTexture() = 0;

		/********************
		* 功能:
		* 获取天空预处理贴图
		* 参数:
		* 无
		* 返回值：
		* ITexture*：贴图指针
		********************/
		virtual ITexture*	iGetSkyPrefilterTexture() = 0;
		/********************
		* 功能:
		* 获取天空环境贴图
		* 参数:
		* 无
		* 返回值：
		* ITexture*：贴图指针
		********************/
		virtual ITexture*	iGetSkyTexture() = 0;

		/********************
		* 功能:
		* 获取BRDF贴图
		* 参数:
		* 无
		* 返回值：
		* ITexture*：贴图指针
		********************/
		virtual ITexture*	iGetBRDFTexture() = 0;
	};
}

