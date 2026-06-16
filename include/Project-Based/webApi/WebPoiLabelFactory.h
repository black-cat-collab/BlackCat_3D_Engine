#pragma once

#include "./IWebLabelFactory.h"

namespace bc
{
	class CommonLabelFactory;
	class RoadAuxiliaryTool;
	class WebPoiLabelFactory : public CommonLabelFactory, public IWebLabelFactory
	{
	public:
		WebPoiLabelFactory();
		~WebPoiLabelFactory();

		virtual void iDoWebCommond(WebCommandData* pWebData);
		virtual EventReturnType_e iProcessEvent(const BCEvent& tEvent);
		virtual void FrameUpdate();

	public:
		virtual void DeletePoi(std::string strID);
		virtual void DeleteAllPoi();
		virtual void CreatePoi(ProjectBasedWebLabelData* pInfo);
		virtual void UpdatePoi(ProjectBasedWebLabelData* pInfo);
		virtual void AddExtraNode(std::string strID, INode* pNode);
		virtual void RefreshZero();


	protected:
		virtual INode* iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo);
		virtual void iUpdateDataForLabel(FactoryAssemble* pAssemble);

		//Detail 方法
		virtual INode* iCreateNodeForDetail(int nType, ProjectBasedSceneElement* pInfo);
		virtual void iOpenForDetail(FactotryAssembleDetail* pAssemble);
		virtual void iUpdateDataForDetail(FactotryAssembleDetail* pAssemble);
		virtual void iCloseForDetail(FactotryAssembleDetail* pAssemble);

		//标签是否选中
		virtual void iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect);
		virtual void iOnSetDetailSelect(FactotryAssembleDetail* pAssemble, std::string strFlagName, bool bSelect);

	private:
		std::map<std::string, INode*> m_mapExtraAddNode;
		std::queue<INode*> m_queueDeleteNode;

		//创建路线的辅助工具
		RoadAuxiliaryTool* m_pRoadAuxiliaryTool;

	};
}



