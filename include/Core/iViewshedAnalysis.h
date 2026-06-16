#pragma once

namespace bc
{
	enum ViewshedAnalysisMode
	{
		VIEWSHED_ANALYSIS_SINGLE,
		VIEWSHED_ANALYSIS_MULTIPLE
	};
	class IViewshedAnalysis : public IViewComponent
	{
	public:
		virtual int iGetViewComponentType()
		{
			return VIEW_COMPONENT_TYPE_VIEWSHED_ANALYSIS;
		}

		/********************
		* 功能:
		* 设置视域分析模式
		* 参数:
		* eMode：模式
		* 返回值：
		* 无
		********************/
		virtual void		iSetMode(const ViewshedAnalysisMode& eMode) = 0;

		/********************
		* 功能:
		* 获取视域分析模式
		* 参数:
		* 无
		* 返回值：
		* ViewshedAnalysisMode：模式
		********************/
		virtual ViewshedAnalysisMode	iGetMode() = 0;

		/********************
		* 功能:
		* 设置张角
		* 参数:
		* fFOV:张角
		* 返回值：
		* 无
		********************/
		virtual void		iSetFOV(const float& fFOV) = 0;

		/********************
		* 功能:
		* 获取张角
		* 参数:
		* 无
		* 返回值：
		* float：张角
		********************/
		virtual float		iGetFOV() = 0;

		/********************
		* 功能:
		* 设置视角位置
		* 参数:
		* vOrigin：视角位置
		* 返回值：
		* 无
		********************/
		virtual void		iSetOrigin(const Vector3d& vOrigin) = 0;

		/********************
		* 功能:
		* 获取视角位置
		* 参数:
		* 无
		* 返回值：
		* Vector3：视角位置
		********************/
		virtual Vector3d		iGetOrigin() = 0;

		/********************
		* 功能:
		* 设置视角角度
		* 参数:
		* vAngles:视角位置
		* 返回值：
		* 无
		********************/
		virtual void		iSetAngles(const Vector3d& vAngles) = 0;

		/********************
		* 功能:
		* 获取视角角度
		* 参数:
		* 无
		* 返回值：
		* Vector3：视角角度
		********************/
		virtual Vector3d		iGetAngles() = 0;

		/********************
		* 功能:
		* 设置远平面
		* 参数:
		* fFar：远平面
		* 返回值：
		* 无
		********************/
		virtual void		iSetFarClip(const float& fFar) = 0;

		/********************
		* 功能:
		* 获取远平面
		* 参数:
		* 无
		* 返回值：
		* float：远平面
		********************/
		virtual float		iGetFarClip() = 0;

		/********************
		* 功能:
		* 获取相机
		* 参数:
		* 无
		* 返回值：
		* INode*：相机指针
		********************/
		virtual INode*		iGetCamera() = 0;

		/********************
		* 功能:
		* 获取视域分析绘制的贴图
		* 参数:
		* 无
		* 返回值：
		* ITexture*：贴图指针
		********************/
		virtual ITexture*	iGetTexture() = 0;
		

		/********************
		* 功能:
		* 设置可视区域颜色
		* 参数:
		* vColor:颜色
		* 返回值：
		* 无
		********************/
		virtual void		iSetVisibleColor(const Vector3& vColor) = 0;

		/********************
		* 功能:
		* 获取可视区域颜色
		* 参数:
		* 无
		* 返回值：
		* Vector3：颜色
		********************/
		virtual Vector3		iGetVisibleColor() = 0;

		/********************
		* 功能:
		* 设置不可视区域颜色
		* 参数:
		* vColor:颜色
		* 返回值：
		* 无
		********************/
		virtual void		iSetInvisibleColor(const Vector3& vColor) = 0;

		/********************
		* 功能:
		* 获取不可视区域颜色
		* 参数:
		* 无
		* 返回值：
		* Vector3：颜色
		********************/
		virtual Vector3		iGetInvisibleColor() = 0;

		/********************
		* 功能:
		* 设置线框颜色
		* 参数:
		* vColor:颜色
		* 返回值：
		* 无
		********************/
		virtual void		iSetLineColor(const Vector4& vColor) = 0;

		/********************
		* 功能:
		* 获取线框颜色
		* 参数:
		* 无
		* 返回值：
		* Vector3：颜色
		********************/
		virtual Vector4		iGetLineColor() = 0;

		/********************
		* 功能:
		* 设置宽高比
		* 参数:
		* 无
		* 返回值：
		* float：比例
		********************/
		virtual void		iSetVFOV(const float& fFOV) = 0;


	};
}

