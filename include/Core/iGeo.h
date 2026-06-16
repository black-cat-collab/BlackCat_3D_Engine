#pragma once

namespace bc
{
	class IGeo : public IComponent
	{
	public:
		virtual int iGetComponentType() { return COMPONENT_TYPE_GEO; }
		// 阴影
		/*
		*	是否投射阴影
		*	return  [out]
		*/
		virtual bool		iIsCasterShadow() = 0;
		
		/*
		*	设置投射阴影
		*	bCaster  [in]  是否投射阴影
		*/
		virtual void		iSetCasterShadow(const bool& bCaster) = 0;

		/*
		*	设置是否启用动态LOD
		*	bLOD  [in]  是否接收阴影
		*/
		virtual void		iSetDynamicLOD(const bool& bLOD) = 0;

		/*
		*	设置动态LOD范围
		*	vecRange  [in]  范围
		*/
		virtual void		iSetDynamicLODRange(const std::vector<std::pair<Vector2, float>>& vecRange) = 0;
	};
}


