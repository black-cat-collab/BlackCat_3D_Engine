#pragma once

namespace bc
{
	class IHiZView : public IViewComponent
	{
	public:
		virtual int iGetViewComponentType()
		{
			return VIEW_COMPONENT_TYPE_HIZ;
		}

		/********************
		* 功能:
		* 获取HiZ贴图
		* 参数:
		* nIndex 索引
		* 返回值：
		* ITexture*：贴图指针
		********************/
		virtual ITexture* iGetHiZTexture(const int& nIndex) = 0;
	
	};

}

