#pragma once

namespace bc
{
	// 播放时出错回调，可以重新来过一遍
	typedef void(*VideoPlayingErrorCallback)(IVideo* pIVideo,
		std::map<std::string, void*>& mapPlayExtraVoid, std::map<std::string, Json::Value>& m_mapPlayExtraJson);

	// VideoBoardParam
	class FusionParam : public DynamicLoadNodeParam
	{
	public:
		FusionParam() :
			DynamicLoadNodeParam(),
			bUFront(true),
			bNVR(false),
			nPort(-1),
			nChannel(-1),
			bScaleSize(false),
			nScaleWidth(1920),
			nScaleHeight(1080),
			bPlayVideo(false),
			bTimeStamp(false),
			ePlugin(MEDIA_PLUGIN_FFMPEG),
			eProtocol(MEDIA_PROTOCOL_DEFAULT),
			bLoop(true),
			bAutoReconnect(false),
			fReconnectTime(1.0f),
			nInterMilliseconds(0)
		{
			nNodeType = NODE_FUSION;

			vCenterPosition = Vector3(0.0);
			//fHeight = 0.0;
			vecTriangle.clear();
		}

		FusionParam(const std::vector<Triangle>& tri) :
			DynamicLoadNodeParam(),
			vecTriangle(tri),
			bUFront(true),
			bNVR(false),
			nPort(-1),
			nChannel(-1),
			bScaleSize(false),
			nScaleWidth(1920),
			nScaleHeight(1080),
			bPlayVideo(false),
			bTimeStamp(false),
			ePlugin(MEDIA_PLUGIN_FFMPEG),
			eProtocol(MEDIA_PROTOCOL_DEFAULT),
			bLoop(true),
			bAutoReconnect(false),
			fReconnectTime(1.0f),
			nInterMilliseconds(0)
		{
			nNodeType = NODE_FUSION;

			vCenterPosition = Vector3(0.0);
	
		}

		FusionParam& operator = (const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		FusionParam& operator =(const FusionParam& src)
		{
			DynamicLoadNodeParam::operator=(src);
			bScaleSize = src.bScaleSize;
			nScaleWidth = src.nScaleWidth;
			nScaleHeight = src.nScaleHeight;
			bPlayVideo = src.bPlayVideo;
			strUser = src.strUser;
			nPort = src.nPort;
			bUFront = src.bUFront;
			bTimeStamp = src.bTimeStamp;
			strDeviceID = src.strDeviceID;
			strChannelID = src.strChannelID;
			strPassword = src.strPassword;
			strLiveUrl = src.strLiveUrl;
			eProtocol = src.eProtocol;
			ePlugin = src.ePlugin;
			bNVR = src.bNVR;
			nChannel = src.nChannel;
			sStartTime = src.sStartTime;
			sEndTime = src.sEndTime;
			bLoop = src.bLoop;
			bAutoReconnect = src.bAutoReconnect;
			fReconnectTime = src.fReconnectTime;
			nInterMilliseconds = src.nInterMilliseconds;


			vBaseColor = src.vBaseColor;
			vSelectColor = src.vSelectColor;
			vHoverColor = src.vHoverColor;

			//	fHeight = src.fHeight;
			vecTriangle.assign(src.vecTriangle.begin(), src.vecTriangle.end());
			bDepthTest = src.bDepthTest;
			strModelName = src.strModelName;
			strTexturePath = src.strTexturePath;
			vCenterPosition = src.vCenterPosition;

			return *this;
		}

	public:
		bool			bPlayVideo;			// 开始播放
		int				nChannel;			// 回放通道
		int				nPort;				// 回放端口
		bool			bUFront;			// uv排列时，默认u在前
		bool			bTimeStamp;			// 是否使用时间戳
		bool			bScaleSize;			// 缩放宽高
		int				nScaleWidth;		// 视频宽
		int				nScaleHeight;		// 视频高
		std::string		strUser;			// 用户名
		std::string		strPassword;		// 密码
		std::string		strDeviceID;		// 设备ID
		std::string		strChannelID;		// 通道ID
		std::string		strLiveUrl;			// 播放地址
		MediaPlugin_e	ePlugin;
		MediaProtocol_e eProtocol;			// 协议
		bool			bLoop;				// 是否循环
		bool			bAutoReconnect;		// 自动重连
		float			fReconnectTime;		// 重连等待时间 单位：秒
		int				nInterMilliseconds;		//每帧的间隔时间（自定义需求，默认为0即根据视频）

		// 回放
		bool			bNVR;				// 是否启用回放
		BCTime			sStartTime;			// 回放开始时间
		BCTime			sEndTime;			// 回放结束时间
		/////
		Vector4					vBaseColor;				// 底面颜色 有图片优先使用图片
		Vector4					vSelectColor;			// 底面被选择颜色
		Vector4					vHoverColor;			// 底面鼠标悬浮颜色

		std::vector<Triangle>   vecTriangle;
		bool					bDepthTest;				// 深度测试
		std::string				strModelName;			// Msh模型名字 
		Vector3					vCenterPosition;
		std::string             strTexturePath;

	};

	class IVideo : public IComponent
	{
	public:
		/*
		 *	获取节点类型
		 *	return			[out]	video部件类型
		 */
		virtual int		iGetComponentType() { return COMPONENT_TYPE_VIDEO; }

		/*
		 *	设置视频巡控参数
		 *	strFilePath		[in]	播放地址
		 */
		virtual void	iSetVideoStream(const std::string& strFilePath) = 0;

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
		virtual bool	iPlayVideo(const bool& bRecord = false, const BCTime& startTime = BCTime(), const BCTime& endTime = BCTime()) = 0;

		/*
		 *	停止视频
		 *	return			[out]	是否停止成功
		 */
		virtual bool	iStopVideo() = 0;

		/*
		 *	暂停视频
		 *	return			[out]	是否暂停成功
		 */
		virtual bool	iPauseVideo() = 0;

		/*
		 *	暂停视频
		 *	return			[out]	是否暂停成功
		 */
		virtual bool	iResumeVideo() = 0;

		/*
		 *	是否播放视频
		 *	return			[out]	是否播放视频
		 */
		virtual bool	iIsPlaying() = 0;

		/*
		 *	云台控制
		 *	eControlType	[in]	云台控制
		 *	return			[out]	是否成功
		 */
		virtual bool	iPTZControl(const MonitorControl_e& eControlType) = 0;

		virtual void	iSetPlayErrorCallback(VideoPlayingErrorCallback pCallback,
			std::map<std::string, void*> mapExtraVoid = {},
			std::map<std::string, Json::Value> mapExtraJson = {}) = 0;
	};
}

