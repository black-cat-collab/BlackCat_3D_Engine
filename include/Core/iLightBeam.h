#pragma once

namespace bc
{
	class ILightBeam : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_LIGHTBEAM; }

		virtual void		iSetWidth(const float& fW) = 0;
		virtual float		iGetWidth() = 0;
		virtual void		iSetHeight(const float& fH, const float& fCurrentH = -1) = 0;
		virtual float		iGetHeight() = 0;
	};
}

