#pragma once

namespace bc
{
	class IGTAOView : public IViewComponent
	{
	public:
		virtual int iGetViewComponentType()
		{
			return VIEW_COMPONENT_TYPE_GROUND_TRUTH_AO;
		}
		/********************
		* 功能:
		* 获取GTAO的贴图
		* 参数:
		* 无
		* 返回值：
		* ITexture*：贴图指针
		********************/
		virtual ITexture*	iGetGTAOTexture() = 0;
	};

}

