#pragma once

namespace bc
{
	class IDefferedLighting : public IViewComponent
	{
	public:
		virtual int iGetViewComponentType()
		{
			return VIEW_COMPONENT_TYPE_DEFFERED_LIGHTING;
		}
		/********************
		* 功能:
		* 获取延迟光照的贴图
		* 参数:
		* 无
		* 返回值：
		* ITexture*：贴图指针
		********************/
		virtual ITexture*	iGetDefferedLightingTexture() = 0;
	
	};
}

