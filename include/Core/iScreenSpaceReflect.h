#pragma once

namespace bc
{
	class IScreenSpaceReflect : public IViewComponent
	{
	public:
		virtual int iGetViewComponentType()
		{
			return VIEW_COMPONENT_TYPE_SCREEN_SPACE_REFLECT;
		}
		/********************
		* 功能:
		* 获取屏幕空间反射的贴图
		* 参数:
		* 无
		* 返回值：
		* ITexture*：贴图指针
		********************/
		virtual ITexture*	iGetSSReflectTexture() = 0;

		/********************
		* 功能:
		* 获取屏幕空间反射模糊后的贴图
		* 参数:
		* 无
		* 返回值：
		* ITexture*：贴图指针
		********************/
		virtual ITexture*	iGetBlurSSReflectTexture() = 0;
	
	};

}

