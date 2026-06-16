#pragma once

namespace bc
{	

	class ITIFNode : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_TIF; }
	};

	class TIFRootNodeParam : public NodeParamBase
	{
	public:
		Vector4d   vRect;
		Vector4     vBaseColor;
		Vector4     vHoverColor;
		Vector4     vSelectColor;
		std::vector<Vector3d> vecKeyPoints;
		std::string strTifPath;
		float          fRadius;
		int             nMinHeight;
		int             nSplitNum;
		bool          bHoverEnable;
		bool          bSelectEnable;
		bool          bGenerateByPoints;
		TIFRootNodeParam() : NodeParamBase()
		{
			nNodeType = NODE_TIF_ROOT;
			strTifPath = "";
			fRadius = 1.0f;
			nMinHeight = 3.0;
			vRect = Vector4d(0.0f);
			nSplitNum = 1;
			vecKeyPoints.clear();
			vBaseColor = Vector4(1.0f);
			vHoverColor = Vector4(1.0f);
			vSelectColor = Vector4(1.0f);
			bGenerateByPoints = false;
			bHoverEnable = false;
			bSelectEnable = false;
		}

		TIFRootNodeParam& operator=(const TIFRootNodeParam& right)
		{
			NodeParamBase::operator=(right);
			strTifPath = right.strTifPath;
			fRadius = right.fRadius;
			nMinHeight = right.nMinHeight;
			vRect = right.vRect;
			nSplitNum = right.nSplitNum;
			vecKeyPoints = right.vecKeyPoints;
			vBaseColor = right.vBaseColor;
			vHoverColor = right.vHoverColor;
			vSelectColor = right.vSelectColor;
			bGenerateByPoints = right.bGenerateByPoints;
			bHoverEnable = right.bHoverEnable;
			bSelectEnable = right.bSelectEnable;
			return *this;
		}
	};
	class ITIFRootNode : public IComponent
	{ 
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_TIF_ROOT; }
	};
} // namespace bc


