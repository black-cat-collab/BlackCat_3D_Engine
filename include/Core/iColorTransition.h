#pragma once

namespace bc
{
	typedef void(*ColorTransitionFunc)(ISystemAPI* pSystemAPI, IView* pView, void* pUser);

	class IColorTransition : public IViewComponent
	{
	public:
		virtual int iGetViewComponentType()
		{
			return VIEW_COMPONENT_TYPE_COLOR_TRANSITION;
		}
		
		virtual void	iPlay(const float& fSecond = 1.0f) = 0;

		virtual void	iSetColor(const Vector3& vColor) = 0;

		virtual void	iSetMiddleFunc(ColorTransitionFunc pFunc, void* pUser) = 0;

		virtual void	iSetEndFunc(ColorTransitionFunc pFunc, void* pUser) = 0;
	};

}

