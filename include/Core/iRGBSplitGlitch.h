#pragma once

namespace bc
{
	class IRGBSplitGlitch : public IViewComponent
	{
	public:
		virtual int iGetViewComponentType()
		{
			return VIEW_COMPONENT_TYPE_RGB_SPLIT_GLITCH;
		}
		
		virtual void	iPlay(const float& fSecond = 1.0f) = 0;
	};

}

