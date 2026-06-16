#pragma once

#ifdef _MSC_VER
#include <Windows.h>
#endif


#include <atomic>
/*
	该对象为window系统下的信号量封装
*/

#ifdef __GNUC__
#include <unistd.h>
#include <semaphore.h>
#define INFINITE            0xFFFFFFFF  // Infinite timeout
typedef void* HANDLE;
#endif

namespace bc
{
	class BCSap
	{
	public:
		BCSap(unsigned int nCur = 1, unsigned int nMax = 9999999);
		~BCSap();

		virtual int Sub(unsigned long nTimeOutMes = INFINITE);
		virtual int Add(int nDelta = 1);

	private:
		HANDLE m_hHandle;
		std::atomic_int m_nCount;

#ifdef __GNUC__
		sem_t g_semaphore;
#endif
	};
}
