#pragma once

#include <string>

namespace bc
{
	class IMonitorSession;

    typedef long long BCSoftEncodeHandle;
	typedef int(*MonitorFunc)(INT_PTR wParam, INT_PTR lParam);
	// 播放时出错回调，可以重新来过一遍
	typedef void(*PlayErrorCallback)(IMonitorSession *pSession,
		std::map<std::string, void*>& mapPlayExtraVoid, std::map<std::string, Json::Value>& m_mapPlayExtraJson);

	enum MonitorControl_e
	{
		IOCTL_MONITOR_SET_DVR_CONFIG = 1,
		IOCTL_MONITOR_GET_DVR_CONFIG,

		IOCTL_MONITOR_SET_DEVICE_CONFIG,
		IOCTL_MONITOR_GET_DEVICE_CONFIG,

		IOCTL_MONITOR_ZOOM_IN,					//焦距变大(倍率变大)
		IOCTL_MONITOR_ZOOM_OUT,                 //焦距变小(倍率变小)

		IOCTL_MONITOR_FOCUS_NEAR,               //焦点前调
		IOCTL_MONITOR_FOCUS_FAR,                //焦点后调

		IOCTL_MONITOR_AUTO,                     //云台左右自动扫描

		IOCTL_MONITOR_UP,                       //云台上仰
		IOCTL_MONITOR_DOWN,                     //云台下俯
		IOCTL_MONITOR_LEFT,                     //云台左转
		IOCTL_MONITOR_RIGHT,                    //云台右转
		IOCTL_MONITOR_SET_POS,                  //设置预置点
		IOCTL_MONITOR_CLEAR_POS,                //清除预置点 
		IOCTL_MONITOR_GOTO_POS,                 //转到预置点
		IOCTL_MONITOR_PTZ_STOP,                 //云台控制停止

		IOCTL_MONITOR_GET_PTZSCOPE,
		IOCTL_MONITOR_GET_PTZPOS,
		IOCTL_MONITOR_SET_PTZPOS,

		IOCTL_MONITOR_GET_NVR,                  //获取nvr指针
		IOCTL_MONITOR_GET_HISTORY,

		IOCTL_MONITOR_GET_ABILITY,              //获取设备能力
		IOCTL_MONITOR_GET_STD_ABILITY,          //获取设备行为分析能力

		IOCTL_MONITOR_SETUP_ALARM,               //设置设备消息报警回调
		IOCTL_MONITOR_DEL_ALARM,                 //删除消息报警回调   

		IOCTL_MONITOR_SET_XML_CONFIG,            
		IOCTL_MONITOR_GET_XML_CONFIG,
		IOCTL_MONITOR_MAX
	};

	enum MediaProtocol_e
	{
		MEDIA_PROTOCOL_DEFAULT,
		MEDIA_PROTOCOL_TCP,
		MEDIA_PROTOCOL_UDP
	};

	enum ImageFormat_e
	{
		IMAGE_FORMAT_YUV,
		IMAGE_FORMAT_RGB,
		IMAGE_FORMAT_BGR
	};

	enum MediaPlugin_e
	{
		MEDIA_PLUGIN_ZLVIDEO,
		MEDIA_PLUGIN_LIVEGBS,
		MEDIA_PLUGIN_GB28181,
		MEDIA_PLUGIN_HKV,             //海康
		MEDIA_PLUGIN_FFMPEG
		
	};

	enum PlayState_e
	{
		PLAY_STATE_STOP = 0,
		PLAY_STATE_PLAYING,
		PLAY_STATE_PAUSE
	};

	enum CaptureType_e
	{
		CAPTURE_TYPE_TO_FILE,
		CAPTURE_TYPE_TO_MEMORY,
		CAPTURE_TYPE_MAX
	};

	typedef struct BCTime_s
	{
		int nYear;
		int nMonth;
		int nDay;
		int nHour;
		int nMinute;
		int nSecond;

		BCTime_s()
		{
			nYear = 0;
			nMonth = 0;
			nDay = 0;
			nHour = 0;
			nMinute = 0;
			nSecond = 0;
		}

		inline bool Valid()
		{
			return (nYear || nMonth || nDay || nHour || nMinute || nSecond);
		}

		inline bool Valid() const
		{
			return (nYear || nMonth || nDay || nHour || nMinute || nSecond);
		}
	}BCTime;

	typedef struct VideoBuffer_s
	{
		int				nWidth;
		int				nHeight;
		size_t			nSize;
		uint8_t*		pBuffer;
		std::string		strUrl;
		BCMutexLock		lckBuffer;

		VideoBuffer_s() :
			nSize(0),
			nWidth(0),
			nHeight(0),
			pBuffer(nullptr)
		{

		}
	}VideoBuffer;

	class MonitorParam
	{
	public:
		std::string			strID;
		std::string			strUrl;
		std::string			strLiveUrl;
		std::string			strUser;
		std::string			strPassword;
		std::string			strDeviceID;
		std::string			strChannelID;
		long				nPort;
		long				nChannel;	
		bool				bScaleSize;				//是否缩放	
		int					nScaleWidth;			//视频缩放宽
		int					nScaleHeight;			//视频缩放高
		int					nCodeRate;				//码率:0为默认
		int					nFrameRate;				//帧率:0为默认
		int					nGroupIndex;			//下级索引
		int					nFrameDelay;
		bool				bTimeStamp;				//是否使用时间戳
		bool				bAutoReconnect;			//是否自动重连
		MediaPlugin_e		eMediaPlugin;           //插件:1,HKV,2:ffmpeg
		MediaProtocol_e		eMediaProtocol;         //default:0,tcp:1,udp:2
		ImageFormat_e		eImageFormat;           //default(rgb):0,1:bgr

		MonitorParam() :
			nPort(0),
			nChannel(0),
			nCodeRate(0),
			nFrameRate(0),
			bScaleSize(false),
			nScaleWidth(1920),
			nScaleHeight(1080),
			nGroupIndex(0),
			nFrameDelay(13),
			bTimeStamp(false),
			bAutoReconnect(false),
			eImageFormat(IMAGE_FORMAT_YUV),
			eMediaPlugin(MEDIA_PLUGIN_LIVEGBS),
			eMediaProtocol(MEDIA_PROTOCOL_DEFAULT)
		{

		}
	};

	typedef struct MonitorInstance_s
	{
		MonitorFunc	pCallBack;
		void*		pUser;
		int			nSleep;
		MonitorInstance_s() :
			nSleep(0),
			pUser(nullptr),
			pCallBack(nullptr)
		{

		}
	}MonitorInstance;

	class IMonitorSession
	{
	public:
		virtual void				iRegisterInstance(void* pUser, MonitorFunc pCallBack) = 0;
		virtual void				iDeleteInstance(void* pUser) = 0;

		virtual void				iPlayLive(const bool& bRepeat = false) = 0;
		virtual void				iPlayBackByTime(const BCTime& startTime, const BCTime& endTime) = 0;
		virtual void				iStopPlay() = 0;
		virtual void			    iPausePlay() = 0;
		virtual void			    iResumePlay() = 0;
		virtual bool			    iIsStop() = 0;

		virtual MonitorParam*		iGetParam() = 0;
		virtual PlayState_e			iGetPlayState() = 0;

		virtual void				iCapture(const CaptureType_e& eType, const std::string& strPath, MonitorFunc pCallBack, void* pUser) = 0;
		virtual INT_PTR				iControl(const MonitorControl_e& eCode, INT_PTR wParam, INT_PTR lParam) = 0;
		virtual void				iSetInterMilliseconds(int nTime) = 0;
		virtual void				iSetPlayErrorCallback(PlayErrorCallback pCallback, std::map<std::string, void*> mapExtraVoid = {}, std::map<std::string, Json::Value> mapExtraJson = {}) = 0;
	};

	class IMonitorManager
	{
	public:
		virtual IMonitorSession*	iRegistSession(MonitorParam* pParam) = 0;
		virtual void				iFreeSession(IMonitorSession** ppSession) = 0;
		virtual void				iFreeSession(const std::string& strID) = 0;
		virtual IMonitorSession*	iGetSession(const std::string& strID) = 0;
		
		virtual BCSoftEncodeHandle* iSoftEncodeInit(const bool& isH265, const uint8_t* pSrcImage, const int& nSrcWidth, const int& nSrcHeight, const int& nScaleWidth, const int& nScaleHeight) = 0;
		virtual int					iSoftEncodeRun(BCSoftEncodeHandle* hEncode, uint8_t* pSrcImage, MonitorFunc fCallback, INT_PTR pUser) = 0;
		virtual int					iSoftEncodeClose(BCSoftEncodeHandle* hEncode) = 0;
	};
}
