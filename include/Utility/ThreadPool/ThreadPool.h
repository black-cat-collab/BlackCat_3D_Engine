#pragma once
#include <thread>

namespace bc
{
	class ThreadTaskPool;
	class ThreadPool {
	public:
		/*
		 *	创建线程池，并设置池中线程数量
		 *  nPoolCapacity	[in]	线程池中线程的数量，若为0则标识根据当前逻辑cpu数量来设置
		 */
		ThreadPool(size_t nPoolCapacity = 0);
		~ThreadPool();

		using TypeFunc = void(*)(void* param);
		/*
		 *	获取任务池
		 */
		ThreadTaskPool& GetTaskPool() {	return m_tTaskPool;}
		void AddTask(const ThreadTaskParam& itParam);
	protected:
		/*
		 * 线程执行函数
		 */
		static void threadCall(ThreadPool* pool);
	protected:
		ThreadTaskPool m_tTaskPool;
		std::thread** m_pThreadAry;
		size_t m_nPoolCapacity;
	};
}

