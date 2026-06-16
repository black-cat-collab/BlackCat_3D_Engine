#pragma once

#ifdef _MSC_VER
#include <Windows.h>
#include <process.h>

#elif  __GNUC__

#include <atomic>
#define		INT_PTR		 __int64_t

#endif

#include <vector>
#include <map>
#include "../Lock/BCMutexLock.h"

namespace bc
{
	typedef void(*ThreadFunc)(std::map<std::string, INT_PTR>& mapParam);
	class BCThreadManager;
	class BCThread
	{
		friend class BCThreadManager;
	public:
		virtual ~BCThread();
		virtual void		SetParam(ThreadFunc pThreadFunc, std::map<std::string, INT_PTR>& mapParam);
		virtual bool		Start();
		virtual void		Wait();
		virtual bool		IsRunning() { return m_bRunning; }
		virtual void		RequestInterruption() { m_bInterruption = true; }
		virtual bool		IsInterruptionRequested(const bool& bImmediately = false, const float& fSleepms = 10.0f);
		// 单位毫秒
		void				Sleepms(const int& ms);
	protected:
		BCThread(const bool& bOccupy);
		static void			threadCallBack(INT_PTR param);
	protected:
		ThreadFunc			m_pThreadFunc;
		std::atomic<bool>	m_bInterruption;
		std::atomic<bool>	m_bRunning;
		bool				m_bOccupy;
		std::map<std::string, INT_PTR>	m_mapParam;
	};

	class BCThreadManager
	{
	public:
		BCThreadManager();
		virtual ~BCThreadManager();
		BCThread*	RegistThread();
		void		FreeThread(BCThread** ppThread);
		void		Release();
		void		Start() { m_bRelease = false; }
		void		StartThread(ThreadFunc pThreadFunc, std::map<std::string, INT_PTR>& mapParam);
	protected:
		BCMutexLock				m_Mutex;
		std::vector<BCThread*>	m_vecThread;
		std::atomic<bool>		m_bRelease;
	};

	class BCTimer
	{
	public:
		BCTimer(BCThreadManager* pThreadManager);
		~BCTimer();
		virtual void	SetRepeat(const bool& bRepeat) { m_bRepeat = bRepeat; }
		virtual bool	IsRepeat() { return m_bRepeat; }
		virtual void	SetInterval(const int& millisecond, const bool& bRepeat = false);
		virtual void	Start(ThreadFunc pThreadFunc);
		virtual void	Start(ThreadFunc pThreadFunc, std::map<std::string, INT_PTR>& mapParam);
		virtual void	Stop();
		virtual bool	IsStop();
	protected:
		static void		run(std::map<std::string, INT_PTR>& mapParam);
	protected:
		int				m_nInterval;
		bool			m_bRepeat;
		ThreadFunc		m_pThreadFunc;
		BCThread*		m_pThread;
		BCMutexLock		m_Mutex;
	};

}
