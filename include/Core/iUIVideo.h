#pragma once

namespace bc
{
	class UIVideoParam;
	class IUIVideo : public IComponent
	{
	public:
		/*
		 *	获取节点类型
		 *	return			[out]	video部件类型
		 */
		virtual int			iGetComponentType() { return COMPONENT_TYPE_UI_VIDEO; }

		/*
		 *	设置视频巡控参数
		 *	strFilePath		[in]	播放地址
		 */
		virtual void		iSetVideoStream(const std::string& strFilePath) = 0;

		/*
		 *	设置视频巡控参数
		 *	strUrl			[in]	播放地址
		 *	strDeviceID		[in]	设备ID
		 *	strChannelID	[in]	通道ID
		 * 	ePlugin			[in]	播放媒体
		 */
		virtual void		iSetVideoStream(const std::string& strUrl, const std::string& strDeviceID,
			const std::string& strChannelID, const MediaPlugin_e& ePlugin = MEDIA_PLUGIN_LIVEGBS) = 0;

		/*
		 *	播放视频
		 *	bRecord			[in]	是否启用回放
		 *	startTime		[in]	开始时间
		 *	endTime			[in]	结束时间
		 *	return			[out]	是否播放成功
		 */
		virtual bool		iPlayVideo(const bool& bRecord = false, const BCTime& startTime = BCTime(), const BCTime& endTime = BCTime()) = 0;

		/*
		 *	停止视频
		 *	return			[out]	是否停止成功
		 */
		virtual bool		iStopVideo() = 0;

		/*
		 *	是否播放视频
		 *	return			[out]	是否播放视频
		 */
		virtual bool		iIsPlaying() = 0;

		/*
		 *	云台控制
		 *	eControlType	[in]	云台控制
		 *	return			[out]	是否成功
		 */
		virtual bool		iPTZControl(const MonitorControl_e& eControlType) = 0;

		virtual void		iGetUIVideoParam(UIVideoParam& tParam) = 0;

		virtual void		iSetUIVideoParam(const UIVideoParam& tParam) = 0;
	};
}

