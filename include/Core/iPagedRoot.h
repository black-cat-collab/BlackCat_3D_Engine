#pragma once

namespace bc
{	
	class IPagedNode : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_PAGED_NODE; }

		virtual void     iSetSelect(bool select) = 0;
		virtual bool     iIsSelect() = 0;
		virtual void     iSetBoxColor(const Vector4& color) = 0;
		virtual const std::string& iGetName() = 0;
	};

	class IPagedRootNode : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_PAGED_ROOT; }

		virtual void iRemovePagedNode(IPagedNode* pNode) = 0;
		virtual void iGetSelectNodes(std::vector<IPagedNode*>& vecNodes) = 0;
		virtual void iGetNodes(std::vector<IPagedNode*>& vecNodes) = 0;
		virtual void iIntersect(const std::vector<Vector3>& points) = 0;
		virtual void iSetEnableColorBox(bool b) = 0;
	};
} // namespace bc


