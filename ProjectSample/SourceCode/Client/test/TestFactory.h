#pragma once

namespace bc
{
	class TestFactory : public CommonLabelFactory
	{
	public:
		TestFactory();
		virtual ~TestFactory();

	protected:
		//创建节点
		virtual INode* iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo);
		//更新数据
		virtual void iUpdateDataForLabel(FactoryAssemble* pAssemble);
		//帧更新前
		virtual void iPreFrameUpdateForLabel();
		//帧更新中
		virtual void iFrameUpdateForLabel(FactoryAssemble* pAssemble);
		//帧更新后
		virtual void iPostFrameUpdateForLabel();
		virtual void iHideAssembleForLabel(FactoryAssemble* pAssemble);
		virtual void iShowAssembleForLabel(FactoryAssemble* pAssemble);


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
		virtual void iOnSetLabelSelect(FactotryAssembleDetail*pAssmeble, bool bSelect);
		//详情面板update后选中标签事件
		virtual void iOnUpdateSelect(FactotryAssembleDetail* pAssmeble, bool bSelect);

		//聚合
		virtual INode* iCreateClusterLabelNode(int nType);
		virtual void iUpdateClusterLabelNode(FactoryAssemble* pAssemble);
		virtual void iSetClusterLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect);
		virtual void iSetClusterParam(CommonLabelFactory* pFactory, ClusterFunc* pClusterFunc);

		//重叠
		virtual INode* iCreateOverTapLabelNode(int nType);
		virtual void iUpdateOverTapLabelNode(FactoryAssemble* pAssemble);
		virtual void iSetOverTapLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect);


	};
}

