#pragma once

namespace bc
{
	class ISpotLightBoard : public IComponent
	{
	public:
		virtual int		iGetComponentType() { return COMPONENT_TYPE_SPOT_LIGHT_BOARD; }

		virtual void	iSetDirection(const Vector3& vDirection) = 0;

		virtual Vector3	iGetDirection() = 0;
	};
}

