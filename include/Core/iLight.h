#pragma once

namespace bc
{
	class LightParam : public NodeParamBase
	{
	public:
		LightType_e			eLightType;
		LightShadowType_e	eShadowType;
		ShadowMapQuality_e	eShadowQuality;
		ShadowFilterType_e	eShadowFilter;
		std::string			strModelName;
		LightParam() :
			NodeParamBase(),
			eLightType(LIGHT_POINT),
			eShadowType(CAST_DYNAMIC_SHADOW),
			eShadowQuality(SHADOW_MAP_QUALITY_LOW),
			eShadowFilter(SHADOW_PCF_AVERAGE_FILTER)
		{
			strName = "Light";
			strModelName = "PointLight";
		}

		LightParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}
	};

	class ILight : public IComponent
	{
	public:
		virtual int iGetComponentType() { return COMPONENT_TYPE_LIGHT; }

		/*
		*	获取光源类型
		*	return  [in]  光源类型（默认为点光源）
		*/
		virtual LightType_e	iGetLightType() = 0;
		
		/*
		*	获取光照颜色
		*	return  [out]  光照颜色
		*/
		virtual Vector3 iGetDiffuse() = 0;
	
		/*
		*	设置漫反射光照颜色
		*	vDiffuse  [in]  漫反射光照颜色
		*/
		virtual void	iSetDiffuse(const Vector3 &vDiffuse) = 0;
		
		/*
		*	获取漫反射光照强度
		*	return  [out]  漫反射光照强度值
		*/
		virtual float	iGetIntensity() = 0;
		
		/*
		*	设置漫反射光照强度
		*	fIntensity  [in]  漫反射光照强度值
		*/
		virtual void	iSetIntensity(const float &fIntensity) = 0;

		/*
		*	获取高光光照强度
		*	return  [out]  高光光照强度值
		*/
		virtual float	iGetSpecularIntensity() = 0;

		/*
		*	设置高光光照强度
		*	fIntensity  [in]  高光光照强度值
		*/
		virtual void	iSetSpecularIntensity(const float& fIntensity) = 0;

		/*
		*	设置内部切光角角度（聚光）
		*	fCutOff  [in]  内部切光角角度大小
		*/
		virtual void	iSetCutOffAngle(const float &fCutOff) = 0;
		
		/*
		*	设置外部切光角角度（聚光）
		*	fCutOff  [in]  外部部切光角角度大小
		*/
		virtual void	iSetOuterCutOffAngle(const float &fOuterCutOff) = 0;

		/*
		*	获取内部切光角余弦值（聚光）
		*	return  [out]  内部切光角余弦值大小
		*/
		virtual float	iGetCutOff() = 0;

		/*
		*	获取外部切光角余弦值（聚光）
		*	return  [out]  外部切光角余弦值大小
		*/
		virtual float	iGetOuterCutOff() = 0;
		
		/*
		*	获取内部切光角角度（聚光）
		*	return  [out]  内部切光角角度大小  
		*/
		virtual float	iGetCutOffAngle() = 0;

		/*
		*	获取外部切光角角度（聚光）
		*	return  [out]  外部切光角角度大小
		*/
		virtual float	iGetOuterCutOffAngle() = 0;

		/*
		*	获取阴影强度
		*	return  [out]  阴影强度大小
		*/
		virtual float	iGetShadowIntensity() = 0;

		/*
		*	设置阴影强度
		*	fIntensity  [in]  阴影强度大小
		*/
		virtual void	iSetShadowIntensity(const float &fIntensity) = 0;
		
		/*
		*	是否开启阴影
		*	return  [out]
		*/
		virtual bool	iIsOpen() = 0;
		
		/*
		*	设置是否开启阴影
		*	bOpen  [in]  
		*/
		virtual void	iSetOpen(const bool &bOpen) = 0;
		
		/*
		*	获取阴影类型
		*	return  [out]  阴影类型
		*/
		virtual LightShadowType_e iGetShadowType() = 0;

		/*
		*	设置阴影类型
		*	eType  [in]  阴影类型
		*/
		virtual void	iSetShadowType(const LightShadowType_e &eType) = 0;

		/*
		*	获取光源方向
		*	return  [in]  光源方向向量
		*/
		virtual Vector3	iGetDirection() = 0;
		
		/*
		*	设置是否渲染
		*	bRender  [in]  是否渲染
		*/
		virtual void	iSetRender(const bool &bRender) = 0;
		
		/*
		*	是否渲染
		*	return  [out]  
		*/
		virtual bool	iIsRender() = 0;

		/*
		*	阴影品质
		*	return  [out]  阴影品质
		*/
		virtual ShadowMapQuality_e iGetShadowQuality() = 0;

		/*
		*	设置阴影品质
		*	eQuality  [in]  阴影品质
		*/
		virtual void				iSetShadowQuality(const ShadowMapQuality_e& eQuality) = 0;

		/*
		*	设置动态阴影索引
		*	nIndex  [in]  索引数值
		*/
		virtual void	iSetDynamicShadowIndex(const int &nIndex) = 0;

		/*
		*	获取动态阴影索引
		*	return  [out]  索引数值
		*/
		virtual int		iGetDynamicShadowIndex() = 0;

		/*
		*	获取矩形光源长度
		*	return  [out]  长度大小
		*/
		virtual float	iGetSquareLengthX() = 0;

		/*
		*	设置矩形光源长度
		*	fLength  [out]  长度
		*/
		virtual void	iSetSquareLengthX(const float &fLength) = 0;

		/*
		*	获取矩形光源宽度
		*	return  [out]  宽度大小
		*/
		virtual float	iGetSquareLengthY() = 0;

		/*
		*	设置矩形光源宽度
		*	fLength  [out]  宽度
		*/
		virtual void	iSetSquareLengthY(const float &fLength) = 0;

		/*
		*	获取矩形光源高度
		*	return  [out]  高度大小
		*/
		virtual float	iGetSquareLengthZ() = 0;

		/*
		*	设置矩形光源高度
		*	fLength  [out]  高度
		*/
		virtual void	iSetSquareLengthZ(const float &fLength) = 0;

		/*
		*	设置矩形光源的衰减系数
		*	fAttenuation  [in]  衰减系数
		*/
		virtual void	iSetSquareAttenuation(const float &fAttenuation) = 0;

		/*
		*	获取矩形衰减系数
		*	return  [out]  衰减系数
		*/
		virtual float	iGetSquareAttenuation() = 0;

		/*
		*	获取动态阴影通道
		*	return  [out]  
		*/
		virtual Vector4	iGetDynamicShadowChannel() = 0;

		/*
		*	设置动态阴影通道
		*	vChannel  [in]  
		*/
		virtual void	iSetDynamicShadowChannel(const Vector4 &vChannel) = 0;

		/*
		*	获取静态阴影通道
		*	return  [out]  
		*/
		virtual Vector4		iGetStaticShadowChannel() = 0;

		/*
		*	设置静态阴影通道
		*	vChannel  [in]
		*/
		virtual void	iSetStaticShadowChannel(const Vector4 &vChannel) = 0;

		/*
		*	是否启用包围盒
		*	return  [out]  
		*/
		virtual bool	iIsBoxEnable() = 0;

		/*
		*	设置是否启用包围盒
		*	bBox  [in]  
		*/
		virtual void	iSetBoxEnable(bool bBox) = 0;

		/*
		*	获取包围盒矩阵
		*	return  [out]  包围盒矩阵
		*/
		virtual const Matrix4& iGetBoxMatrix() = 0;

		/*
		*	设置包围盒矩阵
		*	mat  [in]  包围盒矩阵
		*/
		virtual void	iSetBoxMatrix(const Matrix4& mat) = 0;

		/*
		*	获取包围盒大小
		*	return  [in]  包围盒长宽高大小
		*/
		virtual const Vector3& iGetBoxSize() = 0;

		/*
		*	设置包围盒大小
		*	vSize  [in]  包围盒长宽高大小
		*/
		virtual void	iSetBoxSize(const Vector3& vSize) = 0;

		/*
		*	获取日光色温基数
		*	return  [in]  色温基数
		*/
		virtual float	iGetTemperatureBase() = 0;

		/*
		*	设置日光色温基数
		*	fBase  [in]  色温基数
		*/
		virtual void	iSetTemperatureBase(const float& fBase) = 0;

		/*
		*	获取日光色温缩放
		*	return  [in]  色温缩放
		*/
		virtual float	iGetTemperatureScale() = 0;

		/*
		*	设置日光色温缩放
		*	fScale  [in]  色温缩放
		*/
		virtual void	iSetTemperatureScale(const float& fScale) = 0;

		virtual void	iSetUseTemperature(const bool& bUse) = 0;

		virtual bool	iIsUseTemperature() = 0;

		virtual bool	iIsChange() = 0;

		virtual void	iSetChange(const bool& bChange) = 0;

		virtual float	iGetDiscardThreshold() = 0;

		virtual void	iSetDiscardThreshold(const double& fThreshold) = 0;

		virtual void	iSetBiasBase(const int& nSplit, const double& fBias) = 0;

		virtual float	iGetBiasBase(const int& nSplit) = 0;

		virtual void	iSetBiasScale(const int& nSplit, const double& fBias) = 0;

		virtual float	iGetBiasScale(const int& nSplit) = 0;

		virtual void	iSetLightWidth(const double& fWidth) = 0;

		virtual float	iGetLightWidth() = 0;

		virtual ShadowFilterType_e	iGetShadowFilter() = 0;

		virtual void	iSetShadowFilter(const ShadowFilterType_e& eType) = 0;
	};
}

