#pragma once

namespace bc
{
	class IUIBoardGroup : public IComponent
	{
	public:
		/*
		 *	获取节点类型
		 *	return			[out]	部件类型
		 */
		virtual int			iGetComponentType() { return COMPONENT_TYPE_UI_BOARD_GROUP; }
	};
}

