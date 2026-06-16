#pragma once

namespace bc
{
	class UserInterfaceParam;
	class IUIBoard : public IComponent
	{
	public:
		/*
		 *	获取节点类型
		 *	return			[out]	部件类型
		 */
		virtual int			iGetComponentType() { return COMPONENT_TYPE_UI_BOARD; }

		virtual INode*		iCreateUserInterface() = 0;

		virtual INode*		iCreateUserInterface(UserInterfaceParam& tParam) = 0;

		virtual INode*		iGetUserInterface() = 0;

		virtual std::string	iGetUserInterfaceFile() = 0;

		virtual void		iSetUserInterfaceFile(const std::string& strFile) = 0;

		virtual void		iReloadUserInterface(XMLDoc* pUIDoc) = 0;

		virtual void		iReloadUserInterface() = 0;

		virtual bool		iGenerateXmlDoc(XMLDoc* pUIDoc) = 0;

		virtual float		iGetBoardHeight() = 0;

		virtual float		iGetBoardWidth() = 0;

		virtual float		iGetBoardLength() = 0;

		virtual void		iGetBoardParam(UIBoardParam& tParam) = 0;

		virtual void		iSetBoardParam(const UIBoardParam& tParam) = 0;

		
	};
}

