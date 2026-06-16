#pragma once

namespace bc
{
	class ISky;
	typedef void(*SkyTimeUpdateFunc)(ISky* pSky);

	class ISky
	{
	public:
		// 获取天空盒半径缩放比例
		virtual float		iGetSkyRadiusScale() = 0;

		/*
		*	设置天空盒半径缩放比例
		*	fScale  [in]  缩放比例
		*/
		virtual void		iSetSkyRadiusScale(const float& fScale) = 0;

		/*
		*	设置是否可视化
		*	bVisible  [in]  
		*/
		virtual void		iSetVisible(const bool& bVisible) = 0;

		/*
		*	是否可视化
		*	return  [out]
		*/
		virtual bool		iIsVisible() = 0;

		/*
		*	设置天空类型
		*	eType  [in]  天空类型
		*/
		virtual void		iSetSkyType(const SkyType_e& eType) = 0;
		
		/*
		*	获取天空类型
		*	return  [out]  天空类型
		*/
		virtual SkyType_e	iGetSkyType() = 0;
	};
} // namespace bc


