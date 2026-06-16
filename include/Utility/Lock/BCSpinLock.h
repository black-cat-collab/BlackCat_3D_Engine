#pragma once
#include "BCAutoLock.h"
#include <atomic>

/*
	该类为具有原子属性的自旋锁
*/
namespace bc
{
	class BCSpinLock : public BCAbsLock
	{
	public:
		BCSpinLock();

		virtual void Lock();
		virtual void UnLock();

	private:
		std::atomic_uint m_nNowLock;
		std::atomic_uint m_nNextLock;
	};
}
