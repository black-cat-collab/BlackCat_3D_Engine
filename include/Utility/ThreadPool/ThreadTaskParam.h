#pragma once
#include <map>
#include <vector>
#include <string>

namespace bc
{
	class ThreadTaskParam
	{
	public:
		ThreadTaskParam();

		//任务的回调执行类型
		using TypeFunc = void(*)(ThreadTaskParam& tParam);

		//顺序执行权值（权值越小越优先执行）
		size_t	m_nWeight;
		//任务节点的类型（相同类型的任务都聚簇在一个桶中） 
		//自定义值小于USER_DEFINE_TYPE_ID_MAX
		size_t	m_nType;		//聚簇类型id
		//任务id（回调任务唯一id）,自动生成不需要赋值
		size_t	m_nTaskId;

		//前置权值（前置任务未完成，即使轮到该任务，也不会被执行）
		size_t	m_nFrontWeight;	//前置顺序执行权值
		//前置聚簇类型（前置聚簇类型未完成，即使轮到该任务，也不会被执行）
		size_t	m_nFrontType;
		//前置任务id（前置指定任务未完成，即使轮到该任务，也不会被执行）
		size_t	m_nFrontTaskId;

		//回调执行任务
		TypeFunc	m_pCallFunc;
		// 任务参数集
		std::vector<void*> vParam;
		// string类型任务参数集
		std::vector<std::string> vStringParam;
		//保留字段
		std::map<void*, void*>	m_pReserveSeg;
	};
}

