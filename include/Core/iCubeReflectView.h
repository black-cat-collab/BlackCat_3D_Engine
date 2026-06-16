#pragma once

namespace bc
{
	class ICubeReflectView : public IViewComponent
	{
	public:
		virtual int iGetViewComponentType()
		{
			return VIEW_COMPONENT_TYPE_CUBE_REFLECT;
		}

		/********************
		* 功能:
		* 获取反射贴图
		* 参数:
		* 无
		* 返回值：
		* ITexture*：贴图指针
		********************/
		virtual ITexture*	iGetReflectTexture() = 0;

		/********************
		* 功能:
		* 获取辐照度贴图
		* 参数:
		* 无
		* 返回值：
		* ITexture*：贴图指针
		********************/
		virtual ITexture*	iGetIrradianceTexture() = 0;

		/********************
		* 功能:
		* 获取预处理贴图
		* 参数:
		* 无
		* 返回值：
		* ITexture*：贴图指针
		********************/
		virtual ITexture*	iGetPrefilterTexture() = 0;
	};
}

