#pragma once

#include "IWebLabelFactory.h"

namespace bc
{
	class IWebLabelFactory;
	class InstanceLabelFactory;
	class WebLayerInsLabelFactory : public InstanceLabelFactory,public IWebLabelFactory
	{
	public:
		WebLayerInsLabelFactory();
		~WebLayerInsLabelFactory();

		virtual void iDoWebCommond(WebCommandData* pWebData);

	protected:
		//创建节点
		virtual INode* iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo);

		//详情面板
		virtual INode* iCreateNodeForDetail(int nType, ProjectBasedSceneElement* pInfo);
		virtual void iOpenForDetail(FactotryAssembleDetail* pAssemble);
		virtual void iUpdateDataForDetail(FactotryAssembleDetail* pAssemble);
		virtual void iCloseForDetail(FactotryAssembleDetail* pAssemble);

		//聚合面板
		virtual INode* iCreateClusterLabelNode(int nType);
		virtual void iUpdateClusterLabelNode(INode* pClusterNode, const int nCount);
		virtual void iSetClusterLabelSelect(INode* pClusterNode, bool bSelect);

		//平滑移动
		virtual void iSetSmoothMoveParam(TweenAnimateForNode* pTween);

		//标签是否选中
		virtual void iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect);
		virtual void iOnSetDetailSelect(FactotryAssembleDetail* pAssemble, std::string strFlagName, bool bSelect);

	};
}


