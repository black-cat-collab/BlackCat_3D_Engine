#pragma once

#include <condition_variable>

namespace bc
{
	class BCMutexLock;
	class BCCondition
	{
	public:
		int wait(BCMutexLock& lock);
		int wait(BCMutexLock& lock, uint64_t ms);
		int signal();
		int broadcast();

	private:
		std::condition_variable m_condition;
	};
}
