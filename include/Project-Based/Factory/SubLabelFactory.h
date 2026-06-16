#pragma once

#define SCENE_ELEMENT_PARAM_KEY std::string("scene_element_param_")
namespace bc
{
	class CommonLabelFactory;
	class SceneElementLoader;
	class PROJECT_BASED_API SubLabelFactory : public CommonLabelFactory
	{
	public:
		SubLabelFactory(SceneElementLoader *pLoader);
		~SubLabelFactory();

		virtual void RefreshExtraNode(std::vector<ProjectBasedSceneElement*> &vecData);		//更新extra的节点，比如Polygon,FlyLine等这些

	protected:
		virtual INode* iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo);
		virtual void iUpdateDataForLabel(FactoryAssemble* pAssemble);
		virtual void iShowAssembleForLabel(FactoryAssemble* pAssemble);
		virtual void iHideAssembleForLabel(FactoryAssemble* pAssemble);	

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

		//平滑移动
		virtual void iSetSmoothMoveParam(TweenAnimateForNode* pTween);

		//创建extra Node
		virtual void createExtraNode(int nType, ProjectBasedSceneElement* pInfo);
		INode* createBoard(std::string strStyle, ProjectBasedSceneElement* pInfo, NodeParamBase* pParam);
		INode* createPolygon(std::string strStyle, ProjectBasedSceneElement* pInfo, NodeParamBase* pParam);
		INode* createGeoNode(std::string strStyle, ProjectBasedSceneElement* pInfo);
		INode* createRoad(std::string strStyle, ProjectBasedSceneElement* pInfo, NodeParamBase* pParam);
		INode* createLocusLine(std::string strStyle, ProjectBasedSceneElement* pInfo, NodeParamBase* pParam);
		std::vector<INode*> createFlyLine(std::string strStyle, ProjectBasedSceneElement* pInfo, NodeParamBase* pParam, int nSort);
		INode* createMultiFlyLine(std::string strStyle, ProjectBasedSceneElement* pInfo, NodeParamBase* pParam, int nSort);

		//标签是否选中
		virtual void iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect);
		virtual void iOnSetDetailSelect(FactotryAssembleDetail* pAssemble, std::string strFlagName, bool bSelect);

	protected:
		std::map<std::string, std::vector<INode*>>	m_mapExtraNode;
		SceneElementLoader*							m_pElementLoader;

	};
}



