#pragma once

namespace bc 
{
	enum ColorType_e
	{
		SCALE_COLOR = 0,
		POW_COLOR,
		NO_COLOR,
		ONLY_COLOR,
		VERTEX_COLOR,
		MIX_COLOR,
		BALANCE_COLOR,
		ALPHA_MODIFY
	};

	class IEffectStage
	{
	public:
		virtual std::string	iGetID() = 0;
		virtual void		iSetID(const std::string &strID) = 0;

		virtual void		iEnable() = 0;
		virtual void		iDisable() = 0;
		virtual bool		iIsEnable() = 0;

		virtual ITexture*	iGetTexture() = 0;
		virtual void		iRegistTexture(const TextureParam& tParam) = 0;
		virtual void		iDeleteTexture() = 0;
		virtual int&		iGetSampler() = 0;
		virtual uint&		iGetTextureHandle() = 0;
		virtual void		iSetChannel(const int& nChannel) = 0;
		virtual int&		iGetChannel() = 0;

		virtual Vector4&	iGetColor() = 0;
		virtual void		iSetColor(const Vector4& vColor) = 0;

		virtual Vector2&	iGetUVScale() = 0;
		virtual void		iSetUVScale(const Vector2& vScale) = 0;

		virtual float&		iGetColorScale() = 0;
		virtual void		iSetColorScale(const float& fScale) = 0;

		virtual Vector4&	iGetLuminance() = 0;
		virtual void		iSetLuminance(const Vector4& vLuminance) = 0;

		virtual void		iSetColorType(const int& nType) = 0;
		virtual int&		iGetColorType() = 0;

		virtual int&		iGetEnable() = 0;
		virtual int*		iGetArrayIndexPtr() = 0;
		virtual bool		iIsArrayMap() = 0;

		virtual bool		iIsChanged() = 0;

		//////////////
		virtual void		iStartTextureChange() = 0;
		virtual void		iStopTextureChange() = 0;

		virtual float		iGetAlpha() = 0;
		virtual void		iSetAlpha(const float& fAlpha) = 0;

		virtual void		iSetAvoidRepeat(const bool& bAvoid) = 0;
		virtual bool		iIsAvoidRepeat() = 0;
		virtual int&		iGetAvoidRepeat() = 0;

		virtual void		iSetGammaCorrect(const bool& bGamma) = 0;
		virtual bool		iIsGammaCorrect() = 0;
		virtual int&		iGetGammaCorrect() = 0;
	};
}

