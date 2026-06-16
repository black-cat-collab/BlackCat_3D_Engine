#pragma once

namespace bc
{
	class IFusion;
	typedef void (*FusionCreatedCallback)(IFusion* pFusionNode);

	typedef struct ListParam_s
	{
		std::string  strName;
		Vector3  vPos;
		Vector3  vAngle;
		float vFov;
		ListParam_s()
		{

		}

	}ListParam;

	// FusionCameraNode
	class FusionCameraParam : public NodeParamBase
	{
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

		bool			bDepthTest;			// 是否具有深度测试

	public:
		FusionCameraParam():
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
			nInterMilliseconds(0),
			bDepthTest(true)
		{
			nNodeType = NODE_FUSION_CAMERA;
		}

		FusionCameraParam& operator = (const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		FusionCameraParam& operator =(const FusionCameraParam& src)
		{
			NodeParamBase::operator=(src);

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
			bDepthTest = src.bDepthTest;
			
			return *this;
		}

	public:

	};


	class IFusionCamera : public IComponent
	{
	public:
		/*
		 *	获取节点类型
		 *	return			[out]	video部件类型
		 */
		virtual int		  iGetComponentType() { return COMPONENT_TYPE_FUSION_CAMERA; }
		virtual void      iSetTopGroupNode(INode* pTopGroupNode) = 0;
		virtual INode*	  iGetTopGroupNode() = 0;
		virtual void	  iGetParam(FusionCameraParam& tParam) = 0;
		virtual void	  iSetParam(const FusionCameraParam& tParam) = 0;
		//是否裁剪
		virtual void	  iSetCutEnable(bool bEnable) = 0;
		virtual void	  iSetPosition(const Vector3d& vOrg) = 0;
		virtual void	  iSetAngle(const Vector3d& vAngle) = 0;
		virtual void      iSetFOV(float fFOV) = 0;
		virtual void      iSetAspect(float fRatio) = 0;
		virtual void      iSetDistance(float fDistane) = 0;
		virtual void      iSetLatLng(const Vector3& latlng) = 0;
		virtual float      iGetAlpha() = 0;
		virtual const Vector3d& iGetPosition() = 0;
		virtual const Vector3d&  iGetAngle() = 0;
		virtual float     iGetFOV() = 0;
		virtual float     iGetAspect() = 0;
		virtual float     iGetDistance() = 0;
		virtual void     iSetAlpha(float alpha) = 0;
		//设置融合的预览图片
		virtual void      iSetFuseImage(const std::string& strImage) = 0;
		virtual void      iSetImageVisiable(bool b) = 0;
		virtual Vector3d iGetShowPosition() = 0;
		//设置融合参数
		virtual void	  iSetFuseParam(const FusionParam& tFusionParam) = 0;
		virtual void	  iGetCurFuseParam(FusionParam& tFusionParam) = 0;
		virtual void	  iStart() = 0;
		virtual void      iStop() = 0;
		virtual void     iSetCameraPosition(const Vector3d& pos) = 0;
		virtual void     iSetCameraAngle(const Vector3d& angle) = 0;
		virtual void      iUpdateDefaultMesh() = 0;
		virtual void     iGenerateNewMesh() = 0;
		//获取平移值
		virtual const Vector3d& iGetMoveDelta() = 0;
		//获取旋转值
		virtual  const Vector3d& iGetRotateDelta() = 0;
		virtual void      iSetSaveDir(const std::string& path) = 0;
		virtual void      iSetCreatedCallback(FusionCreatedCallback callback) = 0;

		virtual void	  iAddListParam(ListParam* pParam) = 0;
		virtual std::vector<ListParam*>	  iGetListParamList() = 0;
		virtual void	  iDeleteListParam(ListParam* pParam) = 0;

	};
}

