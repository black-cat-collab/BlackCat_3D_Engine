#pragma once

namespace bc
{
	class UserInterfaceParam :public NodeParamBase
	{
	public:
		int		nWidth;					// 画布宽度
		int		nHeight;				// 画布高度
		bool	bSDFFont;				// 是否使用SDF字体
		bool	bMipmap;				// 是否生成mipmap
		bool	bFrameUpdateRender;		// 是否每一帧更新
		INode*	pHostNode;				// 宿主节点
		UserInterfaceParam() : NodeParamBase()
		{
			nNodeType = NODE_USER_INTERFACE;
			nWidth = 1920;
			nHeight = 1080;
			bMipmap = true;
			bSDFFont = true;
			bFrameUpdateRender = false;
			pHostNode = nullptr;
		}

		UserInterfaceParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		UserInterfaceParam& operator = (const UserInterfaceParam& rhs)
		{
			NodeParamBase::operator=(rhs);
			nWidth = rhs.nWidth;
			nHeight = rhs.nHeight;
			bMipmap = rhs.bMipmap;
			bSDFFont = rhs.bSDFFont;
			bFrameUpdateRender = rhs.bFrameUpdateRender;
			pHostNode = rhs.pHostNode;
			return *this;
		}
	};

	class UIWidgetParam : public NodeParamBase
	{
	public:
		Vector2			vSize;					// 窗口大小
		Vector2			vPosition;				// 坐标 以左上角为原点
		std::string		strMaterial;
		std::string		strHoverMaterial;
		std::string		strCheckMaterial;
		UIWidgetParam() : NodeParamBase()
		{
			nNodeType = NODE_UI_WIDGET;
			vPosition = Vector2(0.0f);
			vSize = Vector2(1920.0f, 1080.0f);
		}

		UIWidgetParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		UIWidgetParam& operator = (const UIWidgetParam& rhs)
		{
			NodeParamBase::operator=(rhs);
			vSize = rhs.vSize;
			vPosition = rhs.vPosition;
			strMaterial = rhs.strMaterial;
			strHoverMaterial = rhs.strHoverMaterial;
			strCheckMaterial = rhs.strCheckMaterial;
			return *this;
		}
	};

	class UIButtonParam : public UIWidgetParam
	{
	public:
		ClickFunc	pClickFunc;
		void*		pClickUser;
		UIButtonParam() : UIWidgetParam()
		{
			nNodeType = NODE_UI_BUTTON;
			pClickFunc = nullptr;
			pClickUser = nullptr;
		}

		UIButtonParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		UIButtonParam& operator = (const UIButtonParam& rhs)
		{
			UIWidgetParam::operator=(rhs);
			pClickFunc = rhs.pClickFunc;
			pClickUser = rhs.pClickUser;
			return *this;
		}
	};

	class UIVideoParam : public UIWidgetParam
	{
	public:
		bool			bScaleSize;			// 是否缩放
		int				nScaleWidth;		// 视频宽
		int				nScaleHeight;		// 视频高
		bool			bPlayVideo;			// 开始播放
		int				nChannel;			// 回放通道
		int				nPort;				// 回放端口
		bool			bUFront;			// uv排列时，默认u在前
		bool			bTimeStamp;			// 是否使用时间戳
		std::string		strUser;			// 用户名
		std::string		strPassword;		// 密码
		std::string		strDeviceID;		// 设备ID
		std::string		strChannelID;		// 通道ID
		std::string		strLiveUrl;			// 播放地址
		MediaPlugin_e	ePlugin;
		MediaProtocol_e eProtocol;			// 协议
		bool			bLoop;				// 是否循环播放

		// 回放
		bool			bNVR;				// 是否启用回放
		BCTime			sStartTime;			// 回放开始时间
		BCTime			sEndTime;			// 回放结束时间

		UIVideoParam() : UIWidgetParam()
		{
			bScaleSize = false;
			nNodeType = NODE_UI_VIDEO;
			nScaleWidth = 1920;
			nScaleHeight = 1080;
			bPlayVideo = false;
			nChannel = -1;
			nPort = -1;
			bUFront = true;
			bTimeStamp = false;
			bNVR = false;
			ePlugin = MEDIA_PLUGIN_FFMPEG;
			eProtocol = MEDIA_PROTOCOL_DEFAULT;
			bLoop = true;
		}

		UIVideoParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		UIVideoParam& operator = (const UIVideoParam& rhs)
		{
			UIWidgetParam::operator=(rhs);
			bScaleSize = rhs.bScaleSize;
			nScaleWidth = rhs.nScaleWidth;
			nScaleHeight = rhs.nScaleHeight;
			bPlayVideo = rhs.bPlayVideo;
			strUser = rhs.strUser;
			nPort = rhs.nPort;
			bUFront = rhs.bUFront;
			bTimeStamp = rhs.bTimeStamp;
			strDeviceID = rhs.strDeviceID;
			strChannelID = rhs.strChannelID;
			strPassword = rhs.strPassword;
			strLiveUrl = rhs.strLiveUrl;
			eProtocol = rhs.eProtocol;
			ePlugin = rhs.ePlugin;
			bNVR = rhs.bNVR;
			nChannel = rhs.nChannel;
			sStartTime = rhs.sStartTime;
			sEndTime = rhs.sEndTime;
			bLoop = rhs.bLoop;

			return *this;
		}
	};

	class UIBoardParam : public NodeParamBase
	{
	public:
		float			fWidth;					// 宽度
		float			fHeight;				// 高度
		float			fLength;				// 长度 当使用面片时该参数无效
		bool			bDepthTest;				// 是否具有深度测试
		bool			bDepthClamp;			// 是否关闭远近平面裁剪
		std::string		strModelName;			// 模型名字 若为空则自动创建面片
		Vector2d			vBoardCenter;			// 模型中点 设置为标准坐标系 y朝上x朝右 x/y范围为[-1,1] 默认为(0,0)
		int				nDirection;				// 朝向
		float			fRotationSpeed;			// Direction为Rotation时旋转的速度
		Vector3d			vRotationDirection;		// Direction为Rotation时旋转的方向
		Vector3d			vScale;					// 放大倍数
		bool			bStableSize;			// 固定大小
		float			fStableMaxDistance;		// 固定大小最远距离
		float			fStableMinDistance;		// 固定大小最远距离
		float			fVisibleMaxDistance;	// 可视距离，若不为0则启用，在小于该距离内显示
		float			fVisibleMinDistance;	// 可视距离，若不为0则启用，在大于该距离内显示
		bool			bClickEnable;			// 是否可点击
		bool			bHoverEnable;			// 是否可悬停
		bool			bRotateAroundTarget;	// 是否围绕目标物体旋转
		Vector3d			vTargetPosition;		// 目标物体位置
		Vector2d			vDistanceToTarget;		// 与目标的距离
		Vector3d			vDirectionToTargetY;	// 与目标竖直方向
		bool			bWithLine;				// 是否有指示线
		Vector4			vLineColor;				// 指示线颜色
		Vector4			vHoverLineColor;		// 悬停时指示线颜色
		Vector4			vCheckedLineColor;		// 选中时指示线颜色
		float			fLineWidth;				// 指示线宽度
		bool			bLinePoint;				// 指示线底部点
		float			fAlphaThreshold;		// 透明阈值
		TransparentType_e	eTransparentType;	// 透明模式
		UIBoardParam() : NodeParamBase()
		{
			nNodeType = NODE_UI_BOARD;

			fWidth = 1.0f;
			fHeight = 1.0f;
			fLength = 1.0f;		
			bClickEnable = false;
			bHoverEnable = false;
			vScale = Vector3(1.0);
			bStableSize = false;
			fStableMaxDistance = 0.0f;
			fStableMinDistance = 0.0f;
			fRotationSpeed = 1.0f;
			vRotationDirection = Vector3(0.0, 0.0, 1.0);
			nDirection = NormalDirect;
			vBoardCenter = Vector2(0, 0);
			fVisibleMaxDistance = 0.0f;
			fVisibleMinDistance = 0.0f;
			bDepthTest = true;
			bDepthClamp = false;
			strModelName = "";
			bRotateAroundTarget = false;
			bWithLine = false;
			bLinePoint = false;
			vTargetPosition = Vector3(0, 0, 0);
			vDistanceToTarget = Vector2(50, 50);
			vDirectionToTargetY = Vector3(0, 0, 1);
			vLineColor = Vector4(1.0f);
			vHoverLineColor = Vector4(1.0f);
			vCheckedLineColor = Vector4(1.0f);
			fLineWidth = 1.0f;
			fAlphaThreshold = 0.0f;
			eTransparentType = TRANSPARENT_BLEND;
		}

		UIBoardParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		UIBoardParam& operator = (const UIBoardParam& rhs)
		{
			NodeParamBase::operator=(rhs);

			fWidth = rhs.fWidth;
			fHeight = rhs.fHeight;
			fLength = rhs.fLength;
			nDirection = rhs.nDirection;
			bHoverEnable = rhs.bHoverEnable;
			bClickEnable = rhs.bClickEnable;
			vScale = rhs.vScale;
			bStableSize = rhs.bStableSize;
			fStableMaxDistance = rhs.fStableMaxDistance;
			fStableMinDistance = rhs.fStableMinDistance;
			fRotationSpeed = rhs.fRotationSpeed;
			vRotationDirection = rhs.vRotationDirection;
			fVisibleMaxDistance = rhs.fVisibleMaxDistance;
			fVisibleMinDistance = rhs.fVisibleMinDistance;
			bDepthTest = rhs.bDepthTest;
			bDepthClamp = rhs.bDepthClamp;
			vBoardCenter = rhs.vBoardCenter;
			strModelName = rhs.strModelName;
			bRotateAroundTarget = rhs.bRotateAroundTarget;
			vDistanceToTarget = rhs.vDistanceToTarget;
			vDirectionToTargetY = rhs.vDirectionToTargetY;
			bWithLine = rhs.bWithLine;
			bLinePoint = rhs.bLinePoint;
			vTargetPosition = rhs.vTargetPosition;
			vLineColor = rhs.vLineColor;
			vHoverLineColor = rhs.vHoverLineColor;
			vCheckedLineColor = rhs.vCheckedLineColor;
			fLineWidth = rhs.fLineWidth;
			fAlphaThreshold = rhs.fAlphaThreshold;
			eTransparentType = rhs.eTransparentType;

			return *this;
		}
	};

	class TextShow;
	class IUserInterface : public IComponent
	{
	public:
		/*
		 *	获取节点类型
		 *	return			[out]	部件类型
		 */
		virtual int			iGetComponentType() { return COMPONENT_TYPE_USER_INTERFACE; }

		virtual INode*		iAddWidget(UIWidgetParam& tParam) = 0;

		virtual INode*		iGetWidget(const std::string& strID) = 0;

		virtual void		iGetWidget(std::vector<INode*>& vecWidget) = 0;

		virtual int			iGetWidgetCount() = 0;

		virtual INode*		iGetHostNode() = 0;

		virtual void		iSetHostNode(INode* pHostNode) = 0;

		virtual bool		iAddSDFText(const SDFTextShow& tText) = 0;

		virtual void		iClearSDFText() = 0;

		virtual bool		iAddText(const TextShow& tText) = 0;

		virtual void		iClearText() = 0;
	};
}

