#pragma once

namespace bc
{
	class BlankModelNodeParam : public DynamicLoadNodeParam
	{
	public:
		std::string strBlankModelPath;
		BlankModelNodeParam() : DynamicLoadNodeParam()
		{
			nNodeType = NODE_BLANK_MODEL;
			strBlankModelPath = "";
		}

		BlankModelNodeParam& operator=(const BlankModelNodeParam& right)
		{
			DynamicLoadNodeParam::operator=(right);
			strBlankModelPath = right.strBlankModelPath;
			return *this;
		}
	};

	class IBlankModelNode : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_BLANK_MODEL; }
	};
}

