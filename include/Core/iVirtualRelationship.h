#pragma once

#include <string>

namespace bc
{
	class IVirtualFolder
	{
	public:
		virtual const std::string& iGetName() = 0;
		virtual const std::string& iGetId() = 0;
		virtual IVirtualFolder* iGetParent() = 0;
		virtual std::vector< IVirtualFolder*> iGetChilds() = 0;
	};

	class IVirtualRelationship
	{
	public:
		virtual IVirtualFolder* iCreateFolder(const std::string& name) = 0;
		virtual void iRemoveFolder(IVirtualFolder* pFolder) = 0;
		virtual void iAppendToFolder(IVirtualFolder* pFolder, std::vector<INode*>& vecNodes) = 0;
	};
}
