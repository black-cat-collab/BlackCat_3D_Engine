#pragma once

namespace bc
{
	class IOrthoManipulator : public IComponent
	{
	public:
		/*
		 *	获取类型
		 *	return	[out]	类型
		 */
		virtual int			iGetComponentType() { return ComponentType_e::COMPONENT_TYPE_ORTHO_MANIPULATOR; }

		virtual void		iSetMaxZoomRate(const float& fRate) = 0;
		virtual float		iGetMaxZoomRate() = 0;

		virtual void		iSetZoomRate(const float& fRate) = 0;
		virtual float		iGetZoomRate() = 0;

		virtual void		iSetZoomSpeed(const float& fSpeed) = 0;
		virtual float		iGetZoomSpeed() = 0;
	};
}

