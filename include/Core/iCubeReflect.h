#pragma once

namespace bc
{
	class ICubeReflect : public IComponent
	{
	public:
		virtual int iGetComponentType() { return COMPONENT_TYPE_CUBE_REFLECT; }
		
		virtual float				iGetRange() = 0;
		virtual void				iSetRange(const float& fRange) = 0;

		virtual void				iSetClip(const Vector2& vClip) = 0;
		virtual Vector2				iGetClip() = 0;

		virtual void				iSetBoxSize(const Vector3& vSize) = 0;
		virtual Vector3				iGetBoxSize() = 0;

		virtual void				iSetBoxProjection(const bool& bProjection) = 0;
		virtual bool				iIsBoxProjection() = 0;

		virtual void				iSetRender(const bool& bRender) = 0;
		virtual bool				iIsRender() = 0;

		virtual void				iSetIrradiance(const bool& bIrradiance) = 0;
		virtual bool				iIsIrradiance() = 0;

		virtual void				iSetReflectLighting(const bool& bLighting) = 0;
		virtual bool				iIsReflectLighting() = 0;
	};
}


