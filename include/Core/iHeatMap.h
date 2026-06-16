#pragma once

namespace bc
{
	class IHeatMap : public IViewComponent
	{
	public:
		virtual int iGetViewComponentType()
		{
			return VIEW_COMPONENT_TYPE_HEAT_MAP;
		}

		/********************
		* 功能:
		* 添加点位
		* 参数:
		* x : x坐标
		* y : y坐标
		* fSize : 辐射直径
		* fIntensity : 强度
		* nIndex : 图层索引
		* 返回值：
		* 无
		********************/
		virtual void		iAddVertex(const float& x, const float& y, const float& fSize, const float& fIntensity, const int& nIndex = 0) = 0;

		/********************
		* 功能:
		* 设置显隐
		* 参数:
		* nIndex : 图层索引
		* bVisible : 显隐
		* 返回值：
		* 无
		********************/
		virtual void		iSetVisible(bool bVisivle, const int& nIndex) = 0;

		/********************
		* 功能:
		* 清空点位
		* 参数:
		* nIndex : 图层索引
		* 返回值：
		* 无
		********************/
		virtual void		iClearVertex(const int& nIndex = 0) = 0;

		/********************
		* 功能:
		* 刷新热力图
		* 参数:
		* nIndex : 图层索引
		* 返回值：
		* 无
		********************/
		virtual void		iRefresh(const int& nIndex = 0) = 0;

		/********************
		* 功能:
		* 设置最大强度
		* 参数:
		* fMax : 最大强度值
		* nIndex : 图层索引
		* 返回值：
		* 无
		********************/
		virtual void		iSetMaxIntensity(const float& fMax, const int& nIndex = 0) = 0;

		/********************
		* 功能:
		* 获取最大强度
		* 参数:
		* nIndex : 图层索引
		* 返回值：
		* float : 最大强度值
		********************/
		virtual float		iGetMaxIntensity(const int& nIndex = 0) = 0;

		/********************
		* 功能:
		* 获取热力图绘制的点位图
		* 参数:
		* 无
		* 返回值：
		* ITexture* : 贴图指针
		********************/
		virtual ITexture*	iGetTexture() = 0;
	};

}

