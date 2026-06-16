#pragma once

namespace bc
{
	class IMonitorSession;
	class IVideoFusion : public IViewComponent
	{
	public:
		virtual int iGetViewComponentType()
		{
			return VIEW_COMPONENT_TYPE_VIDEO_FUSION;
		}

		virtual INode* 				iAddVideo(MonitorParam* pParam, const std::string& strID, const Vector3d& vOrigin, const Vector3d& vAngle,
			const double& fFOV = 66.0f, const int& nSort = 0) = 0;

		virtual void				iClearVideo() = 0;

		virtual INode*				iGetVideoFusionNode(const std::string& strID) = 0;

		virtual INode*				iGetVideoFusionNode(const int& nIndex) = 0;

		virtual ITexture*			iGetVideoTexture() = 0;

		virtual ITexture*			iGetDepthTexture() = 0;
	};

}

