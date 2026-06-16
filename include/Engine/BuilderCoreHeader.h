#pragma once

#define EDIT_VIEW_SCALE				1000.0
#define RENDER_SORT_EDIT_BASE		1000
#define	RENDER_SORT_EDIT_WIDGET		(RENDER_SORT_EDIT_BASE + 100)
#define	RENDER_SORT_EDIT_FONT		(RENDER_SORT_EDIT_BASE + 8999)
#define	BUILDER_VIEW_ID				  "BuilderView"
#define	BUILDER_ORTHO_VIEW_ID	      "BuilderOrthoView"
#define	BUILDER_ORTHO_EDITOR_VIEW_ID  "BuilderOrthoEditorView"
#define BUILDER_ENTITY_MANAGER_ID	  "EntityManagerBuilder"
#define	EDIT_UI_VIEW_ID				  "EditUIView"
#define EDIT_UI_ENTITY_MANAGER_ID	  "EntityManagerEditUI"

namespace bc
{
	enum BuilderNodeType_e
	{
		NODE_EDIT_CANVAS = NodeType_e::NODE_TYPE_END + 1,
		NODE_EDIT_WIDGET,
		NODE_EDIT_BUTTON,
		NODE_EDIT_VIDEO,
		NODE_EDIT_FONT,
		NODE_EDIT_SDF_FONT,
		NODE_EDIT_MARK,
		NODE_EDIT_SCALE,
		NODE_EDIT_ASSIST_TOOL,
		NODE_BUILDER_END
	};

	enum BuilderComponentType_e
	{
		COMPONENT_TYPE_EDIT_CANVAS = ComponentType_e::COMPONENT_TYPE_END + 1,
		COMPONENT_TYPE_EDIT_WIDGET,
		COMPONENT_TYPE_EDIT_VIDEO,
		COMPONENT_TYPE_EDIT_FONT,
		COMPONENT_TYPE_EDIT_SDF_FONT
	};

	enum EditScaleType
	{
		EDIT_SCALE_TOP_LEFT = 0,
		EDIT_SCALE_TOP_CENTER,
		EDIT_SCALE_TOP_RIGHT,
		EDIT_SCALE_MIDDLE_LEFT,
		EDIT_SCALE_MIDDLE_RIGHT,
		EDIT_SCALE_BOTTOM_LEFT,
		EDIT_SCALE_BOTTOM_CENTER,
		EDIT_SCALE_BOTTOM_RIGHT
	};

	enum EditMarkType
	{
		EDIT_MARK_WIDTH = 0,
		EDIT_MARK_HEIGHT,
		EDIT_MARK_POSITION_X,
		EDIT_MARK_POSITION_Y
	};

	enum EditCanvasMode
	{
		EDIT_CANVAS_LINE = 0x01,
		EDIT_CANVAS_MESH = 0x10
	};

	class EditMarkParam :public NodeParamBase
	{
	public:
		int		nMarkType;
		Vector4	vFontColor;
		Vector4 vLineColor;
		Vector2 vFontSize;
		float	fOffset;
		EditMarkParam() : NodeParamBase()
		{
			nNodeType = NODE_EDIT_MARK;
			nMarkType = EDIT_MARK_WIDTH;
			vFontColor = Vector4(1.0);
			vLineColor = Vector4(1.0, 1.0, 1.0, 0.5);
			vFontSize = Vector2(30.0);
			fOffset = 10;
		}

		EditMarkParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		EditMarkParam& operator = (const EditMarkParam& rhs)
		{
			NodeParamBase::operator=(rhs);
			nMarkType = rhs.nMarkType;
			vFontColor = rhs.vFontColor;
			vLineColor = rhs.vLineColor;
			vFontSize = rhs.vFontSize;
			fOffset = rhs.fOffset;
			return *this;
		}
	};

	class EditScaleParam :public NodeParamBase
	{
	public:
		int		nScaleType;
		float	fScale;
		Vector4	vColor;
		Vector4 vHoverColor;
		float	fOffset;
		EditScaleParam() : NodeParamBase()
		{
			nNodeType = NODE_EDIT_SCALE;
			fScale = 20.0;
			fOffset = 10.0;
			nScaleType = EDIT_SCALE_MIDDLE_RIGHT;
			vColor = Vector4(1.0);
			vHoverColor = Vector4(1.0, 1.0, 1.0, 0.5);
		}

		EditScaleParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		EditScaleParam& operator = (const EditScaleParam& rhs)
		{
			NodeParamBase::operator=(rhs);
			vColor = rhs.vColor;
			fScale = rhs.fScale;
			fOffset = rhs.fOffset;
			nScaleType = rhs.nScaleType;
			vHoverColor = rhs.vHoverColor;
			return *this;
		}
	};

	class EditAssistToolParam : public NodeParamBase
	{
	public:
		std::vector<EditScaleParam> vecScale;
		std::vector<EditMarkParam> vecMark;
		EditAssistToolParam() : NodeParamBase()
		{
			nNodeType = NODE_EDIT_ASSIST_TOOL;
		}

		EditAssistToolParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		EditAssistToolParam& operator = (const EditAssistToolParam& rhs)
		{
			NodeParamBase::operator=(rhs);

			vecScale = rhs.vecScale;
			vecMark = rhs.vecMark;

			return *this;
		}
	};

	class EditCanvasParam : public UserInterfaceParam
	{
	public:
		int		nMode;
		Vector4 vLineColor;
		float	fLineWidth;
		Vector4	vBackgroundColor;
		EditCanvasParam() : UserInterfaceParam()
		{
			nMode = EDIT_CANVAS_MESH;
			nNodeType = NODE_EDIT_CANVAS;
			vLineColor = Vector4(0.0, 0.55, 1.0, 1.0);
			fLineWidth = 2.0;
			vBackgroundColor = Vector4(0.2, 0.2, 0.2, 1.0);
		}

		EditCanvasParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		EditCanvasParam& operator = (const EditCanvasParam& rhs)
		{
			UserInterfaceParam::operator=(rhs);
			nMode = rhs.nMode;
			vLineColor = rhs.vLineColor;
			fLineWidth = rhs.fLineWidth;
			vBackgroundColor = rhs.vBackgroundColor;
			return *this;
		}
	};

	class EditWidgetParam : public UIWidgetParam
	{
	public:
		EditWidgetParam() : UIWidgetParam()
		{
			nNodeType = NODE_EDIT_WIDGET;
		}

		EditWidgetParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		EditWidgetParam& operator = (const UIWidgetParam& rhs)
		{
			UIWidgetParam::operator=(rhs);

			return *this;
		}

		EditWidgetParam& operator = (const EditWidgetParam& rhs)
		{
			UIWidgetParam::operator=(rhs);

			return *this;
		}
	};

	class EditButtonParam : public EditWidgetParam
	{
	public:
		EditButtonParam() : EditWidgetParam()
		{
			nNodeType = NODE_EDIT_BUTTON;
		}

		EditButtonParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		EditButtonParam& operator = (const EditButtonParam& rhs)
		{
			EditWidgetParam::operator=(rhs);
			return *this;
		}
	};

	class EditVideoParam : public EditWidgetParam
	{
	public:
		bool			bScaleSize;			// 缩放宽高
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

		// 回放
		bool			bNVR;				// 是否启用回放
		BCTime			sStartTime;			// 回放开始时间
		BCTime			sEndTime;			// 回放结束时间

		EditVideoParam() : EditWidgetParam()
		{
			nNodeType = NODE_EDIT_VIDEO;
			bScaleSize = false;
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
		}

		EditVideoParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		EditVideoParam& operator = (const EditVideoParam& rhs)
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

			return *this;
		}
	};

	class EditSDFFontParam : public NodeParamBase
	{
	public:
		SDFTextShow	tSDFTextShow;
		EditSDFFontParam() : NodeParamBase()
		{
			nNodeType = NODE_EDIT_SDF_FONT;
		}

		EditSDFFontParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		EditSDFFontParam& operator = (const EditSDFFontParam& rhs)
		{
			NodeParamBase::operator=(rhs);
			tSDFTextShow = rhs.tSDFTextShow;
			return *this;
		}
	};

	class EditFontParam : public NodeParamBase
	{
	public:
		TextShow tTextShow;
		EditFontParam() : NodeParamBase()
		{
			nNodeType = NODE_EDIT_FONT;
		}

		EditFontParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		EditFontParam& operator = (const EditFontParam& rhs)
		{
			NodeParamBase::operator=(rhs);
			tTextShow = rhs.tTextShow;
			return *this;
		}
	};

	class IEditCanvas : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_EDIT_CANVAS; }
		virtual void		iGetEditCanvasParam(EditCanvasParam& tParam) = 0;
		virtual void		iSetEditCanvasParam(const EditCanvasParam& tParam) = 0;
	};

	class IEditWidget : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_EDIT_WIDGET; }
		virtual void		iLayerUp() = 0;
		virtual void		iLayerDown() = 0;
		virtual void		iLayerTop() = 0;
		virtual void		iLayerBottom() = 0;
		virtual void		iSetMaterial(const std::string& strFile) = 0;
		virtual void		iSetHoverMaterial(const std::string& strFile) = 0;
		virtual void		iSetCheckMaterial(const std::string& strFile) = 0;
		virtual IMaterial*	iGetMaterial() = 0;
		virtual IMaterial*	iGetHoverMaterial() = 0;
		virtual IMaterial*	iGetCheckMaterial() = 0;
		virtual void		iGetEditWidgetParam(EditWidgetParam& tParam) = 0;
		virtual void		iSetEditWidgetParam(const EditWidgetParam& tParam) = 0;
	};

	class IEditVideo : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_EDIT_VIDEO; }
		virtual bool		iPlayVideo() = 0;
		virtual bool		iStopVideo() = 0;
		virtual bool		iIsPlaying() = 0;
		virtual void		iGetEditVideoParam(EditVideoParam& tParam) = 0;
		virtual void		iSetEditVideoParam(const EditVideoParam& tParam) = 0;
	};

	class IEditFont : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_EDIT_FONT; }
		virtual void		iGetEditFontParam(EditFontParam& tParam) = 0;
		virtual void		iSetEditFontParam(const EditFontParam& tParam) = 0;
	};

	class IEditSDFFont : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_EDIT_SDF_FONT; }
		virtual void		iGetEditSDFFontParam(EditSDFFontParam& tParam) = 0;
		virtual void		iSetEditSDFFontParam(const EditSDFFontParam& tParam) = 0;
	};
}

