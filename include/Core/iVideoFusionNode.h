#pragma once

namespace bc
{
	class IVideoFusionNode : public IComponent
	{
	public:
		virtual int					iGetComponentType() { return COMPONENT_TYPE_VIDEO_FUSION_NODE; }

		virtual void				iSetImage(const std::string& strImage) = 0;

		virtual void				iPlay() = 0;

		virtual void				iStop() = 0;

		virtual bool				iIsPlaying() = 0;

		virtual void				iSetCameraOrigin(const Vector3d& vOrigin) = 0;
		virtual  Vector3d iGetCameraOrigin() = 0;

		virtual void				iSetCameraAngles(const Vector3d& vAngle) = 0;
		virtual  Vector3d iGetCameraAngles() = 0;

		virtual void				iSetCameraFOV(const float& fFOV) = 0;
		virtual float				iGetCameraFOV() = 0;

		virtual bool				iGetEdgeFade() = 0;
		virtual void				iSetEdgeFade(const bool& bFade) = 0;

		virtual float				iGetEdgeFadeWidth() = 0;
		virtual void				iSetEdgeFadeWidth(const float& fWidth) = 0;

		virtual IMonitorSession*	iGetMonitorSession() = 0;
	};

}

