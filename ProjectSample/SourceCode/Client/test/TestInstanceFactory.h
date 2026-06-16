#pragma once

namespace bc
{
	class TestInstanceFactory : public InstanceLabelFactory
	{
	public:
		TestInstanceFactory();
		virtual ~TestInstanceFactory();

	protected:
		//创建节点
		virtual INode* iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo);
		//更新数据
		virtual void iUpdateDataForLabel(FactoryAssemble* pAssemble);

		//详情面板
		//创建节点
		virtual INode* iCreateNodeForDetail(int nType, ProjectBasedSceneElement* pInfo);
		//节点打开时
		virtual void iOpenForDetail(FactotryAssembleDetail* pAssemble);
		//更新节点
		virtual void iUpdateDataForDetail(FactotryAssembleDetail* pAssemble);
		//节点关闭时
		virtual void iCloseForDetail(FactotryAssembleDetail* pAssemble);
		// 更新详细信息面板位置
		virtual void iFrameUpdateForDetail(FactotryAssembleDetail* pAssemble);
		//设置标签是否选中
		virtual void iOnSetLabelSelect(FactotryAssembleDetail *pAssemble, bool bSelect);
		//详情面板update后选中标签事件
		virtual void iOnUpdateSelect(FactotryAssembleDetail* pAssemble, bool bSelect);

		virtual INode* iCreateClusterLabelNode(int nType);
		virtual void iUpdateClusterLabelNode(INode* pClusterNode, const int nCount);
		virtual void iSetClusterLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect);
		virtual void iSetClusterParam(CommonLabelFactory* pFactory, ClusterFunc* pClusterFunc);

		//重叠
		virtual INode* iCreateOverTapLabelNode(int nType);
		virtual void iUpdateOverTapLabelNode(FactoryAssemble* pAssemble);
		virtual void iSetOverTapLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect);
	};
}

