#pragma once

#ifdef _MSC_VER
#include <Windows.h>
#else
#include<unistd.h>
#endif


#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>



namespace bc
{
    class ThreadTaskParam
    {
    public:
        ThreadTaskParam() :
            nPriority(0),
            nLevel(1)
        {

        }
        // 顺序执行权值（值越小越优先执行）
        size_t						nPriority;
        // 任务等级（值越小越优先执行）
        size_t						nLevel;
        // 任务参数集
        std::vector<void*>			vParam;
        // string类型任务参数集
        std::vector<std::string>	vStringParam;
    };

    typedef void(*ThreadTaskFunc)(const ThreadTaskParam& tParam);

    class ThreadPool;
    class ThreadTask
    {
        friend class BCThreadPool;
    public:
        bool operator<(const ThreadTask& rhs) const
        {
            if (m_Param.nLevel > rhs.m_Param.nLevel)
            {
                return true;
            }
            else if (m_Param.nLevel == rhs.m_Param.nLevel)
            {
                return m_Param.nPriority > rhs.m_Param.nPriority;
            }

            return false;
        }
    protected:
        ThreadTask() :
            m_pFunc(nullptr)
        {

        }

        ThreadTask(ThreadTaskFunc pFunc, const ThreadTaskParam& tParam) :
            m_pFunc(pFunc),
            m_Param(tParam)
        {

        }

        void execute()
        {
            if (m_pFunc)
            {
                m_pFunc(m_Param);
            }
        }
    private:
        ThreadTaskParam         m_Param;
        ThreadTaskFunc          m_pFunc;
    };

    class BCThreadPool
    {
    public:
        BCThreadPool(const size_t& nSize = 0);
        ~BCThreadPool();

        void AddWaitTask(std::vector<std::pair<ThreadTaskFunc, ThreadTaskParam>> vecWaitTask);
        void AddWaitTask(ThreadTaskFunc pFunc, const ThreadTaskParam& tParam);
        void NotifyWaitTask();
        void AddTask(ThreadTaskFunc pFunc, const ThreadTaskParam& tParam);
        void JoinAll();
        size_t GetTaskNum();

        void BindCPU(int nStart = 0, int nEnd = -1);

    private:
        std::mutex m_WaitMutex;
        std::vector<ThreadTask> m_vecWaitTask;
        std::mutex m_Mutex;
        std::vector< std::thread > m_vecWorkers;
        std::priority_queue<ThreadTask> m_queTasks;
        std::condition_variable m_Condition;
        std::atomic<bool> m_bStop;
        uint32_t m_nSize;
    };
}

