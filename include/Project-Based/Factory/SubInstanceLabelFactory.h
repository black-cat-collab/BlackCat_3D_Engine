#pragma once

#include "Project-Based/label/InstanceLabelFactory.h"

namespace bc
{
	class SceneElementLoader;
	class PROJECT_BASED_API SubInstanceLabelFactory : public InstanceLabelFactory
	{
	public:
		SubInstanceLabelFactory(SceneElementLoader* pLoader);
		~SubInstanceLabelFactory();

	protected:
		virtual INode* iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo);

		//详情面板
		virtual INode* iCreateNodeForDetail(int nType, ProjectBasedSceneElement* pInfo);
		virtual void iOpenForDetail(FactotryAssembleDetail* pAssemble);
		virtual void iUpdateDataForDetail(FactotryAssembleDetail* pAssemble);
		virtual void iCloseForDetail(FactotryAssembleDetail* pAssemble);

		//聚合面板
		virtual void iSetClusterParam(CommonLabelFactory* pFactory, ClusterFunc* pClusterFunc);
		virtual INode* iCreateClusterLabelNode(int nType);
		virtual void iUpdateClusterLabelNode(INode* pClusterNode, const int nCount);

		//重叠标签
		virtual INode* iCreateOverTapLabelNode(int nType);


		//标签是否选中
		virtual void iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect);
		virtual void iOnSetDetailSelect(FactotryAssembleDetail* pAssemble, std::string strFlagName, bool bSelect);
	private:
		SceneElementLoader* m_pElementLoader;
	};
}


