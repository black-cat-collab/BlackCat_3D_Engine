#pragma once

namespace bc
{	

	class BIMRootParam : public DynamicLoadNodeParam
	{
	public:
		Vector3							vPosition;
		Vector3							vCenterPosition;
		Vector3                          vAngles;
		std::string							strTexturePath;

		Vector4 vBaseColor;
		Vector4 vHoverColor;
		Vector4 vSelectColor;
		bool bVisible;

		std::string               strModelPath;
		std::vector<Vector3>      vecIsolocatePoint;
		bool                      bIsolocation{ false };

		bool       bHoverEnable;
		bool       bClickEnable;

		BIMRootParam() : DynamicLoadNodeParam(),
			bVisible(false)
		{
			nNodeType = NODE_BIM_ROOT;
			bIsolocation = false;
			vPosition = Vector3(0.0f);
			strTexturePath = "";
			strModelPath = "";
			vBaseColor = vHoverColor = vSelectColor = Vector4(1.0f);

		}

		BIMRootParam& operator=(const BIMRootParam& right)
		{
			NodeParamBase::operator=(right);
			vPosition = right.vPosition;
			strTexturePath = right.strTexturePath;
			vecIsolocatePoint = right.vecIsolocatePoint;
			bIsolocation = right.bIsolocation;
			strModelPath = right.strModelPath;
			vBaseColor = right.vBaseColor;
			vHoverColor = right.vHoverColor;
			vSelectColor = right.vSelectColor;
			bVisible = right.bVisible;
			bHoverEnable = right.bHoverEnable;
			bClickEnable = right.bClickEnable;
			return *this;
		}
	};

	class BIMProperty
	{
		std::string strType;
		void* pData;
	};

	class BIMNodeParam : public NodeParamBase
	{
	public:

		//std::string strName;
	//	std::string strGlobalId;
		std::string strClassName;
		uint  m_nObjecID;

		Vector3 vPosition;
		Vector3 vCenterPosition;


		bool       bVisible;


		int32_t  iSize;
		int32_t  iOffset;

		std::vector<BIMProperty> vecProperty;

		BIMNodeParam() : NodeParamBase()
		{
			//strName = strGlobalId = "";

			vPosition = vCenterPosition = Vector3(0.0f);


			iOffset = iSize = 0;
		}

		BIMNodeParam& operator=(const BIMNodeParam& right)
		{
			NodeParamBase::operator=(right);
			strClassName = right.strClassName;
			m_nObjecID = right.m_nObjecID;
			vPosition = right.vPosition;
			vCenterPosition = right.vCenterPosition;

			bVisible = right.bVisible;

			iOffset = right.iOffset;
			iSize = right.iSize;
			vecProperty = right.vecProperty;
			return *this;
		}
	};

	class IBIMNode : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_BIM; }
		virtual void      iGetBIMParam(BIMNodeParam& param) = 0;
		virtual void      iSetBIMParam(const BIMNodeParam& param) = 0;

		virtual INT_PTR		iGetExternParam() = 0;
		virtual void		       iSetExternParam(const INT_PTR& ptr) = 0;
		virtual std::string    iGetFlagName() const = 0;
		virtual void		      iSetFlagName(const std::string& strID) = 0;
		virtual bool           iIsVisible() = 0;
		virtual void            iSetVisible(bool b) = 0;
		virtual bool            iIsSelect() = 0;
		virtual void            iSetSelect(bool b) = 0;
		virtual void            iUpdateState() = 0;
		virtual const std::vector<IBIMNode*>& iGetChilds() = 0;
	};

	class IBIMRootNode : public IComponent
	{
	public:
		virtual int					iGetComponentType() { return COMPONENT_TYPE_BIM_ROOT; }

		virtual int					iLoad(XMLNodePtr pXmlNode) = 0;
		virtual void				iGetSelectNode(std::vector<IBIMNode*>& vecNode) = 0;
		virtual IBIMNode*	iGetBIMRoot() = 0;

		virtual void iSetSelectNode(IBIMNode* pNode) = 0;
	};
} // namespace bc


