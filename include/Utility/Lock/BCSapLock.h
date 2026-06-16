#pragma once
#include "BCAutoLock.h"
#include "BCSap.h"

/*
	该对象为将信号量封装后的锁
*/
namespace bc
{
	class BCSapLock : public BCAbsLock
	{
	public:
		BCSapLock(int nMax = 999999);
		~BCSapLock();

		virtual void Lock();
		virtual void UnLock();

	private:
		BCSap m_oSap;
	};
}
