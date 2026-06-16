#pragma once

/*
	该类为所有锁的抽象类
*/

namespace bc
{
	class BCAbsLock
	{
	public:
		virtual ~BCAbsLock() {}

		virtual void Lock() = 0;
		virtual void UnLock() = 0;
	};

	class BCAutoLock
	{
	public:
		BCAutoLock(BCAbsLock* pLock);
		~BCAutoLock();

	private:
		BCAbsLock* m_pLock;
	};
}
