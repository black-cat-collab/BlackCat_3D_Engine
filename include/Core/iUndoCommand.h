#pragma once

namespace bc
{
	class IUndoCommand
	{
	public:
		virtual void iUndo() = 0;
		virtual void iRedo() = 0;
	};
}

