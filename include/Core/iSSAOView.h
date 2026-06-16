#pragma once

namespace bc
{
	class ISSAOView : public IViewComponent
	{
	public:
		virtual int iGetViewComponentType()
		{
			return VIEW_COMPONENT_TYPE_SCREEN_SPACE_AO;
		}
		/********************
		* 功能:
		* 获取屏幕空间AO的贴图
		* 参数:
		* 无
		* 返回值：
		* ITexture*：贴图指针
		********************/
		virtual ITexture*	iGetSSAOTexture() = 0;
	};

}

