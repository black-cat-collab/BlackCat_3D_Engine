#pragma once
#include "BCAutoLock.h"
#include <mutex>

/*
	该对应为互斥锁
*/
namespace bc
{
	class BCMutexLock : public BCAbsLock
	{
		friend class BCCondition;
	public:
		virtual void Lock();
		virtual void UnLock();
		virtual bool TryLock();

	private:
		std::mutex m_oMutex;
	};
}
