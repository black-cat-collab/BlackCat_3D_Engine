#pragma once

namespace bc
{
	class IShadowView : public IViewComponent
	{
	public:
		virtual int iGetViewComponentType()
		{
			return VIEW_COMPONENT_TYPE_SHADOW;
		}
		/********************
		* 功能:
		* 获取屏幕空间阴影的贴图
		* 参数:
		* 无
		* 返回值：
		* ITexture*：贴图指针
		********************/
		virtual ITexture*	iGetSSShadowTexture() = 0;
		/********************
		* 功能:
		* 获取当前渲染阴影的光源
		* 参数:
		* 无
		* 返回值：
		* ILight*：光源
		********************/
		virtual ILight* iGetShadowLight() = 0;
	};

}

