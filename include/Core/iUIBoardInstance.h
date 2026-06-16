#pragma once

namespace bc
{
	class UIBoardInstanceData;
	class UserInterfaceParam;
	class IUIBoardInstance : public IComponent
	{
	public:
		/*
		 *	获取节点类型
		 *	return			[out]	部件类型
		 */
		virtual int			iGetComponentType() { return COMPONENT_TYPE_UI_BOARD_INSTANCE; }

		virtual void		iAddBoard(const UIBoardInstanceData& tData) = 0;

		virtual INode*		iCreateUserInterface(UserInterfaceParam& tParam) = 0;

		virtual INode*		iCreateUserInterface() = 0;

		virtual std::string	iGetUserInterfaceFile() = 0;

		virtual void		iSetUserInterfaceFile(const std::string& strFile) = 0;
	};
}

