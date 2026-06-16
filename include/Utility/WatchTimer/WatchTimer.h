#pragma once

namespace bc
{
	
	class WatchTimer
	{
	public:
		WatchTimer();
		~WatchTimer();
	 
	public:
		void   Start();
		
		void   Restart();
		//默认乘以1000
		 double Stop(double dRate = 1000.0);	
	 
		 double Elapsed_US(); //微妙
		double Elapsed_MS(); //毫秒
		double Elapsed_S(); //秒

		WatchTimer& operator = (const WatchTimer& t);
	 
	private:
		double m_dElapsed; //微妙
	 
	#ifdef _WIN32
		LARGE_INTEGER m_freq;
		LARGE_INTEGER m_beginTime;
	   
	#else	
		struct timeval m_beginTime;
	#endif

		
	};


}

