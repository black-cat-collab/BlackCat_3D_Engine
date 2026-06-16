#pragma once

namespace bc
{
	enum CoordinateAxisPosition
	{
		COORDINATE_AXIS_TOP_LEFT = 0,
		COORDINATE_AXIS_TOP_RIGHT,
		COORDINATE_AXIS_BOTTOM_LEFT,
		COORDINATE_AXIS_BOTTOM_RIGHT
	};
	class ICoordinateAxisView : public IViewComponent
	{
	public:
		virtual int iGetViewComponentType()
		{
			return VIEW_COMPONENT_TYPE_COORDINATE_AXIS;
		}

		virtual void	iSetAxisPosition(const CoordinateAxisPosition& ePosition) = 0;
	};

}

