#pragma once

namespace bc
{
	class UIWidgetParam;
	class IUIWidget : public IComponent
	{
	public:
		/*
		 *	获取节点类型
		 *	return			[out]	部件类型
		 */
		virtual int			iGetComponentType() { return COMPONENT_TYPE_UI_WIDGET; }

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

		virtual void		iGetUIWidgetParam(UIWidgetParam& tParam) = 0;

		virtual void		iSetUIWidgetParam(const UIWidgetParam& tParam) = 0;
	};
}

