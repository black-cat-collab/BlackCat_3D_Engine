#pragma once

namespace bc
{	

	class ShellRootParam : public DynamicLoadNodeParam
	{
	public:
		std::vector<Triangle>     vecTriangle;
		Vector3d							vCenterPosition;
		std::string							strTexturePath;

		std::string                        strTW4Name;
		std::string                        strMSHName;
		std::string                        strTextureName;

		std::vector<Vector3d>    vecIsolocatePoint;
		std::vector<INode*>       vecIsolocateNode;
		bool                                 bIsolocation{ false };
		bool                                 bIsFromModel = false;
		ShellRootParam() : DynamicLoadNodeParam()
		{
			nNodeType = NODE_SHELL_GROUP;
			bIsolocation = false;
			bIsFromModel = false;
			vCenterPosition = Vector3(0.0f);
			strTexturePath = "";
		}

		ShellRootParam& operator=(const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		ShellRootParam& operator=(const ShellRootParam& right)
		{
			DynamicLoadNodeParam::operator=(right);
			vecTriangle = right.vecTriangle;
			vCenterPosition = right.vCenterPosition;
			strTexturePath = right.strTexturePath;
			vecIsolocatePoint = right.vecIsolocatePoint;
			bIsolocation = right.bIsolocation;
			bIsFromModel = right.bIsFromModel;
			vecIsolocateNode = right.vecIsolocateNode;
			return *this;
		}
	};

	class ShellNodeParam : public NodeParamBase
	{
	public:

		std::string strName;
		std::string strModelPath;
		std::string strTexturePath;
		Vector3d vPosition;
		Vector3d vCenterPosition;

		bool       bHoverEnable;
		bool       bClickEnable;
		bool       bVisible;

		Vector4 vBaseColor;
		Vector4 vHoverColor;
		Vector4 vSelectColor;

		std::vector<Triangle> vecTris;

		ShellNodeParam() : NodeParamBase()
		{
			strName = strTexturePath = strModelPath = "";

			vPosition = vCenterPosition = Vector3(0.0f);
			bHoverEnable = bClickEnable = bVisible = false;

			vBaseColor = vHoverColor = vSelectColor = Vector4(1.0);
		}

		ShellNodeParam& operator=(const ShellNodeParam& right)
		{
			NodeParamBase::operator=(right);

			strName = right.strName;
			strModelPath = right.strModelPath;
			strTexturePath = right.strTexturePath;
			vPosition = right.vPosition;
			vCenterPosition = right.vCenterPosition;
			bHoverEnable = right.bHoverEnable;
			bClickEnable = right.bClickEnable;
			bVisible = right.bVisible;
			vBaseColor = right.vBaseColor;
			vHoverColor = right.vHoverColor;
			vSelectColor = right.vSelectColor;
			vecTris = right.vecTris;
			return *this;
		}
	};
	class IShellRootNode;
	class IShellNode : public IComponent
	{
	public:
		struct ShellAttributeParam
		{
			ShellAttributeParam() : offset(0), size(0) {}
			ShellAttributeParam(int32_t offset_, int32_t size_) : offset(offset_), size(size_) {}
			int32_t offset;
			int32_t size;
		};

	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_SHELL; }
		virtual void      iGetShellParam(ShellNodeParam& param) = 0;
		virtual void      iSetShellParam(ShellNodeParam& param) = 0;
		virtual const ShellNodeParam& iGetShellParam() = 0;
		virtual INT_PTR		iGetExternParam() = 0;
		virtual void				iSetExternParam(const INT_PTR& ptr) = 0;

		virtual void iSetAttributeParam(const ShellAttributeParam& param) = 0;

		virtual std::string		                    iGetFlagName() const = 0;
		virtual void				                    iSetFlagName(const std::string& strID) = 0;
		virtual void                                  iSetSelect(bool b) = 0;
		virtual void                                  iSetHover(bool b) = 0;
		virtual Vector3d                          iGetOrigin() = 0;
		virtual IShellRootNode* iGetRoot() = 0;
	};

	class IShellRootNode : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_SHELL_ROOT; }

		virtual int         iLoad(XMLNodePtr pXmlNode) = 0;
		virtual void      iOutputToFile(const std::string& path) = 0;
		virtual void      iSetEditorState(bool state) = 0;

		//设置文件保存文件夹路径（主要用于业务保存，不设置则使用默认）
		virtual void      iSaveDir(const std::string& path) = 0;
		virtual void      iSave(const std::string& path = "") = 0;
		virtual void      iFinish() = 0;
		virtual void      iCancel() = 0;
		virtual IShellNode* iGetSelectNode() = 0;
		virtual IShellNode* iGetHoverNode() = 0;
		virtual void      iSetSelectNode(int index) = 0;
		virtual void      iRemoveNode(int index) = 0;
		virtual void      iRemoveNode(IShellNode* pNode) = 0;
		virtual const std::vector<IShellNode*>& iGetShellNodes() = 0;

		virtual bool      iIsIsolocateFinish() = 0;

		virtual void      iSplitVerticalPlanel(const Vector3& vStart, const Vector3& vEnd) = 0;
		virtual void      iSplitSingleVerticalPlanel(const Vector3& vStart, const Vector3& vEnd) = 0;
		virtual void      iSplitVerticalRect(std::vector<Vector3>& points) = 0;

		virtual void     iSplitSingleHorizontal(const Vector3& vPoint) = 0;
		virtual void     iSplitAllHorizontal(const Vector3& vPoint) = 0;

		virtual void     iSetGenerateNodeParam(const ShellNodeParam& param) = 0;
	};
} // namespace bc


