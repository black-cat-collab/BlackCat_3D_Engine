#pragma once

namespace bc
{
	class VideoInspectionFactory;
	class PROJECT_BASED_API BCVideoInspectionFactory : public VideoInspectionFactory
	{
	public:
		BCVideoInspectionFactory(ISystemAPI* pSystemAPI);
		virtual ~BCVideoInspectionFactory();

	protected:
		virtual void iModifyPointInfo(const std::string& strKey, std::vector<BCVideoRouterPointInfo*>& vecPointInfo);
		//面板的样式,设置视频的livegbs的liveUrl和ChannelID
		virtual void iSetVideoBoardParam(const std::string& strKey, BCVideoRouterPointInfo* pInfo,VideoBoardWithButtonParam* pParam) = 0;
	};
}


