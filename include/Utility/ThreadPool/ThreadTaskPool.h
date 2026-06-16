#pragma once
#include "ThreadTaskParam.h"
#include <map>
#include <list>
#include <atomic>
#include "../Lock/hcsaplock.h"
#include "../Lock/hcmutexlock.h"
#include "../Lock/hcspinlock.h"
#include "../Container/hcdiylist.h"

namespace bc
{
	class ThreadTaskPool
	{
		//类别查询表
		struct _TypeSearchMap {
			struct _TypeTypeMap {
				struct _TypeTaskList {
					size_t m_nValidId = 0;		//任务类别有效id
					HCDiyList< ThreadTaskParam*> m_lTaskList;
				};

				size_t m_nValidId = 0;			//聚簇有效id
				size_t m_nTaskCount = 0;		//聚簇总人数数量
				std::map<size_t, _TypeTaskList> m_mTaskMap;
			};
			std::map<size_t, _TypeTypeMap> m_mTypeMap;
		};

		//顺序查询表
		struct _TypeProcessMap{
			struct _TypeWeigthMap {
				struct _TypeTaskList {

					HCDiyList< ThreadTaskParam*> m_lTaskList;
				};

				size_t m_nTaskCount = 0;	//权值所对应的任务数量
				std::map<size_t, _TypeTaskList> m_mTaskMap;
			};

			std::map<size_t, _TypeWeigthMap> m_mWeigthMap;
		};
	public:
		/*
		 *	创建任务池，并设置池相关属性
		 *	pLock				[in]	任务池的锁属性，为空则标识为单线程容器
		 *	nClearReadyTimes	[in]	最高运行多少次后，任务池将执行一次清理计划
		 */
		ThreadTaskPool(HCAbsLock* pLock = nullptr, size_t nClearReadyTimes = 100);
		~ThreadTaskPool();
		/*
		 *	添加一个线程任务到任务池中
		 *  itParam	[in]	
		 *  return	[out]	生成的任务ID	
		 */
		size_t PushTask(const ThreadTaskParam& itParam);
		/*
		 *	从任务池中移除一个任务或者一聚簇任务
		 *	nType	[in]	聚簇类别
		 *	nSelfId	[in]	任务节点自身id（为0时将移除整个聚簇）
		 *  return	[out]	是否成功
		 */
		bool EraseTask(size_t inType, size_t inTaskId = 0);
		/*
		 *	从任务池中取出一个任务
		 */
		bool PopTask(ThreadTaskParam& otParam);

	protected:
		/*
		 *	检查任务的有效性
		 *  tParam	[in]	被校验的任务
		 *  return	[out]	任务有效返回true，无效返回false
		 */
		bool checkTaskValid(ThreadTaskParam& tParam);
		/*
		 *	检查任务的前置条件
		 *  tParam	[in]	被检测的任务
		 *  return	[out]	任务当前具有前置条件返回true，无前置条件返回false
		 */
		bool checkTaskFront(ThreadTaskParam& tParam);
		/*
		 *  将任务从池中移除
		 */
		void eraseTask(ThreadTaskParam* pParam);
		/*
		 *  拷贝并创建任务参数对象
		 */
		ThreadTaskParam* createTask(const ThreadTaskParam& tParam);
		/*
		 *  清理任务池中的空表项
		 */
		void clearMap();
		/*
		 *  获取一个可执行的任务
		 *  otParam		[out]	被执行的任务参数
		 *  bIsContinue	[out]	当前未获取到任务任务数据，继续执行后续清理任务
		 *  return		[out]	是否获取到有效任务数据
		 */
		bool getProcessTask(ThreadTaskParam& otParam, bool& bIsContrinue);
		/*
		 *  获取无序可执行任务
		 */
		bool getProcessDisorderTask(ThreadTaskParam& otParam, bool& bIsContrinue);
		/*  获取顺序执行任务
		 */
		bool getProcessOrderTask(ThreadTaskParam& otParam, bool& bIsContrinue);
		/*  从任务表中取出一个任务
		 */
		bool getProcessTaskFromTaskMap(_TypeProcessMap::_TypeWeigthMap&  weigthTask, ThreadTaskParam& otParam, bool& bIsContrinue);
	protected:
		_TypeSearchMap m_mSearchMap;
		_TypeProcessMap m_mProcessMap;

		HCSap m_tProcessSap;
		HCAbsLock* m_pLock;
		size_t m_nTaskCount;
		std::pair<size_t, size_t> m_tClearReadyTimes;
		bool m_bAllotDisorder;				//分配无序任务
		std::atomic<size_t>	m_nGeneratorTaskID;
		std::atomic<size_t>	m_nGeneratorTypeID;
	};
}

