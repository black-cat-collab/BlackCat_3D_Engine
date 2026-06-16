#pragma once

namespace bc
{
	class IPolygon : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_POLYGON; }

		virtual void		iSetBaseMaterial(const std::string& strImage) = 0;
		virtual void		iSetFenceMaterial(const std::string& strImage) = 0;
		virtual void		iSetHoverBaseMaterial(const std::string& strImage) = 0;
		virtual void		iSetHoverFenceMaterial(const std::string& strImage) = 0;
	};

	class IPolygonMesh : public IViewComponent
	{
	public:
		virtual int			iGetViewComponentType() { return VIEW_COMPONENT_TYPE_POLYGON_MESH; }

		virtual ITexture* iGetTexture() = 0;
		virtual void iSetColor(Vector4& color) = 0;
		virtual void iSetWidth(float width) = 0;
		virtual void iRefresh() = 0;
		virtual void iAddPoints(std::vector<Vector3>& vecPoints) = 0;
	};
} // namespace bc

