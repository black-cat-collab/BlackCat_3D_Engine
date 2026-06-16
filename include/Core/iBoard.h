#pragma once

namespace bc
{
	// 底座参数配置
	class BoardBaseParam
	{
	public:
		float				fWidth;					// 宽度
		float				fHeight;				// 高度
		float				fLength;				// 长度	当使用面片时该参数无效
		std::string			strImage;				// 材质名字
		float				fHeightToBoard;			// 离面板高度
		std::string			strModelName;			// 模型名字
		Vector3				vModelAngle;			// 模型角度
		Vector3				vModelScale;			// 模型缩放比
		bool				bGlowEnable;			// 是否启用辉光
		int					fGlowIntensity;			// 辉光强度
		float				fScale;					// 缩放
		TransparentType_e	eTransparentType;	// 透明模式
		BoardDirect_e		nDirection;			// 朝向
		FrameUpdateFunc		pFrameUpdateFunc;	// 帧动画函数指针
		void*				pFrameUpdateUser;	// 帧动画调用对象
		BoardBaseParam()
		{
			fWidth = 10;
			fHeight = 10;
			fLength = 10;
			strImage = "";
			strModelName = "";
			eTransparentType = TRANSPARENT_BLEND;
			bGlowEnable = false;
			fGlowIntensity = 5.0;
			fHeightToBoard = 0;
			fScale = 1.0;
			pFrameUpdateFunc = nullptr;
			pFrameUpdateUser = nullptr;
			nDirection = NormalDirect;
			vModelScale = Vector3(1.0f, 1.0f, 1.0f);
		}

		BoardBaseParam& operator = (const BoardBaseParam& src)
		{
			fWidth = src.fWidth;
			fHeight = src.fHeight;
			fLength = src.fLength;
			strImage = src.strImage;
			fScale = src.fScale;
			strModelName = src.strModelName;
			vModelAngle = src.vModelAngle;
			vModelScale = src.vModelScale;
			fHeightToBoard = src.fHeightToBoard;
			eTransparentType = src.eTransparentType;
			bGlowEnable = src.bGlowEnable;
			fGlowIntensity = src.fGlowIntensity;
			pFrameUpdateFunc = src.pFrameUpdateFunc;
			nDirection = src.nDirection;
			return *this;
		}
	};

	// BoardParam
	struct BoardMaterialColorParam
	{
		bool m_bValid;
		bool m_bOnlyColor;
		Vector4 m_vColor;

		BoardMaterialColorParam()
		{
			m_bValid = false;
			m_bOnlyColor = true;
			m_vColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	};

	class BoardParam : public NodeParamBase
	{
	public:
		int				nDataType;			// 节点数据，用以区分不同的节点数据类型
		bool			bWriteToFile;		// 是否写入文件
		bool			bDepthTest;			// 是否具有深度测试
		bool			bDepthClamp;		// 是否关闭远近平面裁剪
		float			fMinWidth;			// 最小宽度
		float			fWidth;				// 宽度
		float			fHeight;			// 高度
		float			fLength;			// 长度 当使用面片时该参数无效
		Vector2			vBoardCenter;		// 模型中点 设置为标准坐标系 y朝上x朝右 x/y范围为[-1,1] 默认为(0,0) 左上角为(-0.5, -0.5)
		float			fScale;				// 放大倍数
		float			fSelectedRate;		// 点击后照正常的放大缩小比例
		std::string		strImage;			// 背景图片
		std::string		strHoverImage;		// 悬停背景图片
		std::string		strCheckedImage;	// 选中背景图片
		std::string		strPlaceHolderImage;// 占位图片
		bool			bCreateModel;		// 是否创建新的模型,当不使用模型时无效
		std::string		strModelName;		// 模型名字 若为空则自动创建面片
		Vector3			vModelAngle;		// 模型角度
		Vector3			vModelScale;		// 模型缩放比 x,y,z,比如 1.0,1.0,1.0
		bool			bMipmap;			// 是否启用Mipmap
		bool			bUseModelMaterial;	// 是否使用模型自带材质,当不使用模型时无效
		int				direction;			// 是否面向照相机，当为屏幕精灵时，设置无效
		float			fRotationSpeed;		// direction为Rotation时旋转的速度
		Vector3			vRotationDirection;	// direction为Rotation时旋转的方向
		bool			bClickScale;		// 是否点击后能缩放
		bool			bClickEnable;		// 是否能点击
		bool			bCheckEnable;		// 是否有check状态
		bool			bHoverEnable;		// 是否有鼠标悬停状态
		bool			bDynamicScaleEnable;// 是否根据与相机距离调整大小
		float			fDynamicScale;		// 相机距离动态调整大小比例
		float			fDynamicMaxScale;	// 动态调整大小的最大值
		float			fDynamicMinScale;	// 动态调整大小的最小值
		float			fVisibleDistance;	// 可视距离，若不为0则启用，在该距离内显示
		float			fHideLessDistance;	// 小于此距离隐藏，若不为0则启用，大于该距离内显示
		bool			bGlowEnable;		// 是否有辉光
		float			fGlowIntensity;		// 辉光强度
		bool			bRotateAroundTarget;	// 是否围绕目标物体旋转
		Vector3d			vTargetPosition;	// 目标物体位置
		Vector2			vDistanceToTarget;	// 与目标的距离
		bool			bWithLine;			// 是否有指示线
		Vector4			vLineColor;			// 指示线颜色
		Vector4			vHoverLineColor;	// 悬停时指示线颜色
		Vector4			vCheckedLineColor;	// 选中时指示线颜色
		float			fLineWidth;			// 指示线宽度
		bool			bLinePoint;			// 指示线底部点
		bool			bWithBase;			// 是否有底座
		bool			bNeedClarity;		// 是否需要透明
		float			fAlphaThreshold;	// 透明阈值
		TransparentType_e	eTransparentType;	// 透明模式
		BoardBaseParam	tBaseParam;	// 底座参数
		BoardMaterialColorParam m_tMaterialColor;
		BoardMaterialColorParam m_tHoverMaterialColor;
		BoardMaterialColorParam m_tCheckMaterialColor;
		bool m_bNeedAdjust;
		Vector2 m_vBorderSize;				//边界尺寸
		Vector2 m_vBoardSize;				//面板大小
		Vector2 vTopLeftOffset;
		bool m_bPickSelf;
		virtual void Adjust()
		{
			if (m_bNeedAdjust)
			{
				m_vBoardSize.x = fWidth * fScale;
				m_vBoardSize.y = fHeight * fScale;

				AdjustParam();
				AdjustCalculate();
				m_bNeedAdjust = false;
			}
		}
	protected:
		bool	bUpdate;
	public:
		BoardParam() : NodeParamBase()
		{
			bUpdate = false;
			nNodeType = NODE_BOARD;
			fWidth = 1.0f;
			fMinWidth = 0.0f;
			fHeight = 1.0f;
			fLength = 1.0f;
			fScale = 1.0f;
			fSelectedRate = 3.0f;
			fVisibleDistance = 0.0f;
			fHideLessDistance = 0.0f;
			bDepthTest = true;
			bDepthClamp = false;
			bWriteToFile = false;
			bCreateModel = false;
			strModelName = "";
			bUseModelMaterial = true;
			strCheckedImage = "";
			strHoverImage = "";
			strImage = "";
			bMipmap = true;
			vBoardCenter = Vector2(0, 0);
			direction = NormalDirect;
			fRotationSpeed = 1.0f;
			vRotationDirection = Vector3(0.0, 0.0, 1.0);
			bClickScale = false;
			bClickEnable = true;
			bHoverEnable = false;
			bDynamicScaleEnable = false;
			fDynamicScale = 0.0f;
			fDynamicMaxScale = 0.0f;
			fDynamicMinScale = 0.0f;
			bRotateAroundTarget = false;
			bCheckEnable = false;
			bWithLine = false;
			bLinePoint = false;
			bWithBase = false;
			vTargetPosition = Vector3(0, 0, 0);
			vDistanceToTarget = Vector2(50, 50);
			vLineColor = Vector4(1, 1, 1, 1);
			vHoverLineColor = Vector4(1, 1, 1, 1);
			vCheckedLineColor = Vector4(1, 1, 1, 1);
			fLineWidth = 1.0f;
			bGlowEnable = false;
			fGlowIntensity = 4.0;
			m_bNeedAdjust = true;
			bNeedClarity = true;
			m_vBorderSize = Vector2(0, 0);
			m_vBoardSize = Vector2(0, 0);
			vTopLeftOffset = Vector2(0.0, 0.0);
			m_bPickSelf = true;
			fAlphaThreshold = 0.2f;
			eTransparentType = TRANSPARENT_BLEND;
			vModelScale = Vector3(1.0f, 1.0f, 1.0f);
		}

		BoardParam& operator = (const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);
			return *this;
		}

		BoardParam& operator = (const BoardParam& src)
		{
			NodeParamBase::operator=(src);

			nDataType = src.nDataType;
			fMinWidth = src.fMinWidth;
			fWidth = src.fWidth;
			fHeight = src.fHeight;
			fLength = src.fLength;
			fScale = src.fScale;
			bDepthTest = src.bDepthTest;
			bDepthClamp = src.bDepthClamp;
			vBoardCenter = src.vBoardCenter;
			bWriteToFile = src.bWriteToFile;
			strModelName = src.strModelName;
			vModelAngle = src.vModelAngle;
			vModelScale = src.vModelScale;
			bCreateModel = src.bCreateModel;
			bUseModelMaterial = src.bUseModelMaterial;
			fSelectedRate = src.fSelectedRate;
			strImage = src.strImage;
			strCheckedImage = src.strCheckedImage;
			strHoverImage = src.strHoverImage;
			strPlaceHolderImage = src.strPlaceHolderImage;
			bMipmap = src.bMipmap;
			bClickEnable = src.bClickEnable;
			bCheckEnable = src.bCheckEnable;
			bHoverEnable = src.bHoverEnable;
			direction = src.direction;
			fRotationSpeed = src.fRotationSpeed;
			vRotationDirection = src.vRotationDirection;
			bClickScale = src.bClickScale;
			bDynamicScaleEnable = src.bDynamicScaleEnable;
			fDynamicScale = src.fDynamicScale;
			fDynamicMaxScale = src.fDynamicMaxScale;
			fDynamicMinScale = src.fDynamicMinScale;
			fVisibleDistance = src.fVisibleDistance;
			fHideLessDistance = src.fHideLessDistance;
			bGlowEnable = src.bGlowEnable;
			fGlowIntensity = src.fGlowIntensity;
			bRotateAroundTarget = src.bRotateAroundTarget;
			vDistanceToTarget = src.vDistanceToTarget;
			bWithLine = src.bWithLine;
			bLinePoint = src.bLinePoint;
			vTargetPosition = src.vTargetPosition;
			vLineColor = src.vLineColor;
			vHoverLineColor = src.vHoverLineColor;
			vCheckedLineColor = src.vCheckedLineColor;
			fLineWidth = src.fLineWidth;
			bWithBase = src.bWithBase;
			tBaseParam = src.tBaseParam;
			m_tMaterialColor = src.m_tMaterialColor;
			m_tHoverMaterialColor = src.m_tHoverMaterialColor;
			m_tCheckMaterialColor = src.m_tCheckMaterialColor;
			m_bNeedAdjust = src.m_bNeedAdjust;
			m_vBorderSize = src.m_vBorderSize;
			m_vBoardSize = src.m_vBoardSize;
			vTopLeftOffset = src.vTopLeftOffset;
			bNeedClarity = src.bNeedClarity;
			m_bPickSelf = src.m_bPickSelf;
			fAlphaThreshold = src.fAlphaThreshold;
			eTransparentType = src.eTransparentType;
			return *this;
		}
		inline bool IsUpdate() const { return bUpdate; }
		inline void SetUpdate(const bool& _bUpdate) { bUpdate = _bUpdate; }

	protected:
		virtual void AdjustParam() {}
		virtual void AdjustCalculate() {}
	};

	// FontBoardParam
	class FontBoardParam : public BoardParam
	{
	public:
		std::vector<TextShow> vecText;	// 文字

		FontBoardParam() : BoardParam()
		{
			nNodeType = NODE_FONTBOARD;
			vecText.clear();
			bUpdate = true;
		}

		~FontBoardParam()
		{
			vecText.clear();
			bUpdate = false;
		}

		FontBoardParam& operator = (const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);
			return *this;
		}

		void AddText(const TextShow& tTextShow)
		{
			vecText.push_back(tTextShow);
			bUpdate = true;
		}

		void SetText(const TextShow& tTextShow)
		{
			vecText.clear();

			AddText(tTextShow);

			bUpdate = true;
		}

		FontBoardParam& operator = (const FontBoardParam& src)
		{
			BoardParam::operator=(src);

			vecText.assign(src.vecText.begin(), src.vecText.end());

			bUpdate = true;

			return *this;
		}
	};

	// BoardWithFontBoardParam
	class BoardWithFontBoardParam : public BoardParam
	{
	public:

		enum BoardWithFontBoardLocation
		{
			TOP = 0,
			BOTTOM,
			LEFT,
			RIGHT,
			FRONT,
			BACK,
			CENTER
		};

		BoardWithFontBoardLocation	nFontLocation;	// FontBoard的位置
		float			fFontBoardOffset;		// FontBoard的偏移量
		FontBoardParam	tFontBoardParam;		// FontBoard的参数

		BoardWithFontBoardLocation	nDetailFontLocation; // DetailFontBoard的位置
		float			fDetailFontBoardOffset;		// DetailFontBoard的偏移量	
		bool			bDetailEnable;			// 是否有额外的FontBoard
		FontBoardParam	tDetailFontBoardParam;	// 额外的FontBoard参数

		BoardWithFontBoardParam() : BoardParam()
		{
			nNodeType = NODE_BOARD_WITH_FONTBOARD;
			nFontLocation = BoardWithFontBoardParam::TOP;
			nDetailFontLocation = BoardWithFontBoardParam::RIGHT;
			bDetailEnable = false;
			fFontBoardOffset = 0;
			fDetailFontBoardOffset = 0;
		}

		BoardWithFontBoardParam& operator = (const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		BoardWithFontBoardParam& operator = (const BoardWithFontBoardParam& src)
		{
			BoardParam::operator=(src);

			tFontBoardParam = src.tFontBoardParam;
			nFontLocation = src.nFontLocation;
			nDetailFontLocation = src.nDetailFontLocation;
			tDetailFontBoardParam = src.tDetailFontBoardParam;
			bDetailEnable = src.bDetailEnable;
			fFontBoardOffset = src.fFontBoardOffset;
			fDetailFontBoardOffset = src.fDetailFontBoardOffset;
			return *this;
		}
	};

	// ButtonBoardParam
	class ButtonBoardParam : public FontBoardParam
	{
	public:
		ButtonBoardParam() : FontBoardParam()
		{
		}

		ButtonBoardParam& operator =(const ButtonBoardParam& src)
		{
			FontBoardParam::operator=(src);
			return *this;
		}
	public:
	};

	// FontBoardWithButtonParam
	class FontBoardWithButtonParam : public FontBoardParam
	{
	public:
		FontBoardWithButtonParam() : FontBoardParam()
		{
			nNodeType = NODE_FONTBOARD_WITH_BUTTON;
			vecBoardParam.resize(0);
		}

		FontBoardWithButtonParam& operator = (const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		FontBoardWithButtonParam& operator =(const FontBoardWithButtonParam& src)
		{
			FontBoardParam::operator=(src);

			vecBoardParam.assign(src.vecBoardParam.begin(), src.vecBoardParam.end());

			return *this;
		}

	public:
		std::vector<ButtonBoardParam> vecBoardParam;
	};

	// SpotLightBoardParam
	class SpotLightBoardParam :public BoardParam
	{
	public:
		float fOriginHeight;	// 光线长度
		Vector3 vDirection;		// 初始方向
		SpotLightBoardParam() : BoardParam()
		{
			nNodeType = NODE_SPOT_LIGHT_BOARD;
			fWidth = 10.0;
			fHeight = 500.0;
			fOriginHeight = fHeight;
			bGlowEnable = true;
			eTransparentType = TRANSPARENT_BLEND;
			fAlphaThreshold = 0.0;
			vDirection = Vector3(0, 0, 1);
		}

		SpotLightBoardParam& operator = (const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		SpotLightBoardParam& operator = (const SpotLightBoardParam& src)
		{
			BoardParam::operator=(src);

			vDirection = src.vDirection;
			fOriginHeight = src.fOriginHeight;

			return *this;
		}
	};

	// LightBeamParam
	class LightBeamParam : public BoardParam
	{
	public:

		enum LightBeamFontBoardLocation
		{
			TOP = 0,
			LEFT,
			RIGHT
		};

		bool	bAnimationGrow;		// 高度变化时是否展现变化过程
		float	fAnimationSpeed;	// 高度变化速度
		float	fFontBoardOffset;		// FontBoard的偏移量
		LightBeamFontBoardLocation nFontLocation; // FontBoard的位置
		FontBoardParam tFontBoardParam;	// FontBoard参数

		LightBeamParam() : BoardParam()
		{
			nNodeType = NODE_LIGHT_BEAM;
			nFontLocation = TOP;
			bAnimationGrow = true;
			fFontBoardOffset = 0.0f;
			fAnimationSpeed = 1.0;
		}

		LightBeamParam& operator = (const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);
			return *this;
		}

		LightBeamParam& operator = (const LightBeamParam& src)
		{
			BoardParam::operator=(src);
			fFontBoardOffset = src.fFontBoardOffset;
			tFontBoardParam = src.tFontBoardParam;
			nFontLocation = src.nFontLocation;
			bAnimationGrow = src.bAnimationGrow;
			fAnimationSpeed = src.fAnimationSpeed;
			return *this;
		}
	};

	// VideoBoardParam
	class VideoBoardParam : public BoardParam
	{
	public:
		VideoBoardParam() :
			BoardParam(),
			bUFront(true),
			bNVR(false),
			nPort(-1),
			nChannel(-1),
			bScaleSize(false),
			nScaleWidth(1920),
			nScaleHeight(1080),
			bPlayVideo(false),
			bTimeStamp(true),
			ePlugin(MEDIA_PLUGIN_FFMPEG),
			eProtocol(MEDIA_PROTOCOL_DEFAULT),
			bLoop(true),
			bAutoReconnect(false),
			fReconnectTime(1.0f),
			nInterMilliseconds(0)
		{
			nNodeType = NODE_VIDEO_BOARD;
		}

		VideoBoardParam& operator = (const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		VideoBoardParam& operator =(const VideoBoardParam& src)
		{
			BoardParam::operator=(src);
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

	};

	// VideoBoardWithButtonParam
	class VideoBoardWithButtonParam : public FontBoardWithButtonParam
	{
	public:
		VideoBoardWithButtonParam() : FontBoardWithButtonParam()
		{
			nNodeType = NODE_VIDEO_BOARD_WITH_BUTTON;
			vecVideoParam.resize(0);
		}

		VideoBoardWithButtonParam& operator = (const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		VideoBoardWithButtonParam& operator =(const VideoBoardWithButtonParam& src)
		{
			FontBoardWithButtonParam::operator=(src);

			vecVideoParam.assign(src.vecVideoParam.begin(), src.vecVideoParam.end());

			return *this;
		}

	public:
		std::vector<VideoBoardParam> vecVideoParam;
	};

	class IBoard : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_BOARD; }
		/*
		*	设置面板参数
		*	pParam	 [in]
		*/
		virtual void		iSetBoardParam(BoardParam* pParam) = 0;

		/*
		*	获取面板参数
		*	return	 [out]  面板参数
		*/
		virtual BoardParam* iGetBoardParam() = 0;

		/*
		*	获取面板高度
		*	return  [out]  面板高度
		*/
		virtual float		iGetBoardHeight() = 0;

		/*
		*	获取面板宽度
		*	return  [out]  面板宽度
		*/
		virtual float		iGetBoardWidth() = 0;

		/*
		*	获取面板长度
		*	return  [out]  面板长度
		*/
		virtual float		iGetBoardLength() = 0;

		/*
		*	设置面板分辨率比例
		*	fScale  [in]  比例
		*/
		virtual void		iSetResolutionScale(const float& fScale) = 0;

		/*
		*	重置面板尺寸大小
		*/
		virtual void		iResetSize() = 0;

		/*
		*	获取面板材质
		*	return  [out]  面板材质
		*/
		virtual IMaterial*	iGetBoardMaterial() = 0;

		/*
		*	获取悬停材质
		*	return  [out]  悬停材质
		*/
		virtual IMaterial*	iGetHoverMaterial() = 0;

		/*
		*	获取选中材质
		*	return  [out]  选中材质
		*/
		virtual IMaterial*	iGetCheckMaterial() = 0;

		/*
		*	设置面板材质
		*	strImage  [in]  背景图片
		*/
		virtual void		iSetBoardMaterial(const std::string& strImage) = 0;

		/*
		*	设置悬停时材质
		*	strImage  [in]  悬停背景图片
		*/
		virtual void		iSetHoverMaterial(const std::string& strImage) = 0;

		/*
		*	设置选中时材质
		*	strImage  [in]  选中背景图片
		*/
		virtual void		iSetCheckMaterial(const std::string& strImage) = 0;

		/*
		*	设置面板材质
		*	pMaterial  [in]  面板材质
		*/
		virtual void		iSetBoardMaterial(IMaterial *pMaterial) = 0;

		/*
		*	设置悬停时材质
		*	pMaterial  [in]  悬停时材质
		*/
		virtual void		iSetHoverMaterial(IMaterial *pMaterial) = 0;

		/*
		*	设置选中时材质
		*	pMaterial  [in]  选中时材质
		*/
		virtual void		iSetCheckMaterial(IMaterial *pMaterial) = 0;

		/*
		*	设置启用根据与相机距离调整大小的功能
		*	bEnable  [in]  启用/不启用
		*/
		virtual void		iSetDynamicScaleEnable(const bool& bEnable) = 0;

		/*
		*	设置动态调整大小比例
		*	fScale  [in]  比例大小
		*/
		virtual void		iSetDynamicScale(const float& fScale) = 0;

		/*
		*	设置动态调整大小比例的最大值
		*	fScale  [in]  最大值
		*/
		virtual void		iSetDynamicScaleMax(const float& fScale) = 0;

		/*
		*	设置动态调整大小比例的最小值
		*	fScale  [in]  最小值
		*/
		virtual void		iSetDynamicScaleMin(const float& fScale) = 0;

		/*
		*	设置面板比例
		*	fScale  [in]  比例大小
		*/
		virtual void		iSetBoardScale(const float& fScale) = 0;

		/*
		*	设置可以选中
		*	bChecked  [in]  是否可以选中
		*/
		virtual void		iSetChecked(const bool& bChecked) = 0;

		/*
		*	是否可以选中
		*	return  [out]  是否可以选中
		*/
		virtual bool		iIsChecked() = 0;

		/*
		*	获取底座面板
		*	return  [out]  底座面板节点
		*/
		virtual INode*		iGetBaseBoard() = 0;

		/*
		*	设置面板上的文本
		*	vecText  [int]  存储显示文本的容器
		*/
		virtual void		iSetText(std::vector<TextShow>& vecText) = 0;

		/*
		*	面板是否含有按键
		*	pBoard  [in]   面板节点
		*	return  [out]  
		*/
		virtual bool		iContainButtonBoard(INode* pBoard) = 0;

		/*
		*	获取按键面板
		*	strID   [in]   使用按键面板ID查找
		*	bFuzzy  [in]   是否模糊查找（默认不模糊）
		*	return  [out]  按键面板节点
		*/
		virtual INode*		iGetButtonBoard(const std::string& strID, const bool& bFuzzy = false) = 0;

		/*
		*	获取按键面板
		*	nIndex  [in]   使用索引查找（默认为0）
		*	return  [out]  按键面板节点
		*/
		virtual INode*		iGetButtonBoard(const int& nIndex = 0) = 0;

		/*
		*	获取按键面板
		*	vecBoard  [in]  获取所有的面板
		*/
		virtual void		iGetButtonBoard(std::vector<INode*>& vecBoard) = 0;

		/*
		*	面板是否含有视频
		*	pVideo  [in]  
		*	return  [out]  有/没有
		*/
		virtual bool		iContainVideoBoard(INode* pVideo) = 0;

		/*
		*	获取视频面板
		*	strID   [in]   使用视频面板ID查找
		*	bFuzzy  [in]   是否模糊查找（默认不模糊）
		*	return  [out]  视频面板节点
		*/
		virtual INode*		iGetVideoBoard(const std::string& strID, const bool& bFuzzy = false) = 0;

		/*
		*	获取视频面板
		*	nIndex  [in]   使用索引查找（默认为0）
		*	return  [out]  视频面板节点
		*/	
		virtual INode*		iGetVideoBoard(const int& nIndex = 0) = 0;

		/*
		*	获取视频面板
		*	vecBoard  [in]  获取所有的面板
		*/
		virtual void		iGetVideoBoard(std::vector<INode*>& vecVideo) = 0;

		/*
		*	获取字体面板
		*	return  [out]  字体面板节点
		*/
		virtual INode*		iGetFontBoard() = 0;

		/*
		*	设置字体面板可视
		*	bVisible  [in]  可视/不可视
		*/
		virtual void		iSetFontBoardVisible(const bool& bVisible) = 0;

		/*
		*	获取详细字体面板
		*	return  [out]  详细字体面板节点
		*/
		virtual INode*		iGetDetailFontBoard() = 0;

		/*
		*	设置详细字体面板可视
		*	bVisible  [in]  可视/不可视
		*/
		virtual void		iSetDetailFontBoardVisible(const bool& bVisible) = 0;

		/*
		*	更新材质状态
		*/
		virtual void		iUpdateMaterialState() = 0;

		/*
		*	更新文字
		*/
		virtual void		iUpdateText() = 0;
		
	};
}

