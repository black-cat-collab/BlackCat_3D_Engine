#pragma once

namespace bc
{
	class ClusterFuncFactory : public CommonLabelFactory
	{
	public:
		ClusterFuncFactory(ClusterFunc *pClusterFunc);
		virtual ~ClusterFuncFactory();

		virtual void RefreshZero();

	protected:
		//创建节点
		virtual INode* iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo);
		//更新数据
		virtual void iUpdateDataForLabel(FactoryAssemble* pAssemble);
		//设置标签是否选中
		virtual void iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect);

	protected:
		ClusterFunc* m_pClusterFunc;

		std::map<std::string,INode*> m_mapNode;
	};
}


