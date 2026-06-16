#pragma once

namespace bc
{
	class IParticle : public IComponent
	{
	public:
		virtual int iGetComponentType() { return COMPONENT_TYPE_PARTICLE; }
		
		virtual void		iGetParticleParam(ParticleNodeParam& tParam) = 0;
		virtual void		iSetParticleParam(const ParticleNodeParam& tParam) = 0;
	};
}


