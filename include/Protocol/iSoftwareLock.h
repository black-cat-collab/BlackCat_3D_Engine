#pragma once

namespace bc
{
	typedef enum LockType_e
	{
		SOFTWARE_LOCK_OFFLINE = 0,
		SOFTWARE_LOCK_ONLINE
	}LockType;

	class ISoftwareLockManager
	{
	public:
		virtual bool	iLock(int nType, const std::string& strTime) = 0;
	};
}
