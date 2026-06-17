#include "ProjectBasedHeader.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/module/IStateModule.h"
#include "Project-Based/module/BaseModule.h"
#include "Project-Based/module/YBSSModule.h"
#include "Project-Based/utils/tools/IsolatedTool.h"

#define	BUILDER_ORTHO_EDITOR_VIEW_ID  "BuilderOrthoEditorView"

using namespace bc;

IsolatedTool::IsolatedTool()
{
	m_pSystemAPI = APIProvider::GetSystemAPI();
	m_mapChildVisible.clear();
}

IsolatedTool::~IsolatedTool()
{
}

void IsolatedTool::SetNodeOnlyVisible(INode* pNode, bool bVisible)
{
	INode* pRootNode = m_pSystemAPI->iCoreAPI->iGetViewManager()->iGetRootNode();
	int nCount = pRootNode->iGetChildCount();
	if (pNode)
	{
		for (size_t i = 0; i < nCount; i++)
		{
			ILight* pLight = pRootNode->iGetChild(i)->GetDynamicComponent<ILight>();
			if (pRootNode->iGetChild(i) && !pLight)
			{
				m_mapChildVisible[pRootNode->iGetChild(i)] = pRootNode->iGetChild(i)->iIsVisible();

				pRootNode->iGetChild(i)->iSetVisible(!bVisible);
			}
		}
		INode* pParent = pNode->iGetParent();
		if (pParent)
		{
			std::vector<INode*> vecChild;
			pParent->iSetVisible(bVisible);
			pParent->iGetAllChildren(vecChild);
			for (auto it : vecChild)
			{
				it->iSetVisible(!bVisible);
			}
		}
		pNode->iSetVisible(bVisible);
		IShellRootNode* pShellNode = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
		if (pShellNode)
		{
			pShellNode->iSetEditorState(true);
		}
	}
	else
	{
		if (m_mapChildVisible.size() > 0)
		{
			for (size_t i = 0; i < nCount; i++)
			{
				ILight* pLight = pRootNode->iGetChild(i)->GetDynamicComponent<ILight>();
				if (pRootNode->iGetChild(i) && !pLight)
				{
					if (m_mapChildVisible.find(pRootNode->iGetChild(i)) != m_mapChildVisible.end())
					{
						pRootNode->iGetChild(i)->iSetVisible(m_mapChildVisible[pRootNode->iGetChild(i)]);
					}
					else
					{
						pRootNode->iGetChild(i)->iSetVisible(bVisible);
					}
				}
			}
			//m_mapChildVisible.clear();
		}
		ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
		if (pClient->m_stBCModuleManager.pYBSSModule)
		{
			INode* pParent = pClient->m_stBCModuleManager.pYBSSModule->iGetModuleGroupNode();
			if (pParent)
			{
				std::vector<INode*> vecChild;
				pParent->iSetVisible(bVisible);
				pParent->iGetAllChildren(vecChild);
				for (auto it : vecChild)
				{
					if (it)
					{
						it->iSetVisible(bVisible);
						IShellRootNode* pShellNode = (IShellRootNode*)it->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
						if (pShellNode)
						{
							pShellNode->iSetEditorState(false);
						}
					}
				}
			}
		}
		/*{
			BCEvent tEvent(PROJECTBASED_EVENT_APP_MODE_CHANGED);
			APIProvider::GetSystemAPI()->iProjectBasedAPI->iGetStateManager()->iExecuteEvent(tEvent);
		}*/
	}

	//INode* pRootNode = m_pSystemAPI->iCoreAPI->iGetViewManager()->iGetRootNode();
	//int nCount = pRootNode->iGetChildCount();

	//INode* pBuild = nullptr;
	//ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
	//INode* pTopGroupNode = nullptr;
	//if (pClient->m_stBCModuleManager.pYBSSModule)
	//{
	//    pTopGroupNode = pClient->m_stBCModuleManager.pYBSSModule->iGetModuleGroupNode();
	//}
	//if (!pTopGroupNode)
	//{
	//    pTopGroupNode = pRootNode;
	//}
	////INode* pDrawNode = m_pSystemAPI->iCoreAPI->iGetViewManager()->iFindNodeByID("Isolated");
	//if (pNode)
	//{
	//    pBuild = pNode->iGetParent();
	//    for (size_t i = 0; i < nCount; i++)
	//    {
	//        ILight* pLight = pRootNode->iGetChild(i)->GetDynamicComponent<ILight>();
	//        if (pRootNode->iGetChild(i) != pBuild  && pRootNode->iGetChild(i) && !pLight)
	//        {
	//            pRootNode->iGetChild(i)->iSetVisible(bVisible);
	//        }
	//    }
	//    if(pTopGroupNode && pTopGroupNode != pRootNode)
	//    {
	//        std::vector<INode*> vec;
	//        pTopGroupNode->iGetAllChildren(vec);
	//        for (size_t i = 0; i < vec.size(); ++i)
	//        {
	//            vec[i]->iSetVisible(false);
	//        }
	//        pTopGroupNode->iSetVisible(true);
	//        INode* pParent = pNode->iGetParent();
	//        {
	//            std::vector<INode*> vec;
	//            pParent->iGetAllChildren(vec);
	//            for (size_t i = 0; i < vec.size(); ++i)
	//            {
	//                vec[i]->iSetVisible(true);
	//            }
	//        }
	//        while (pParent != pTopGroupNode)
	//        {
	//            pParent->iSetVisible(true);
	//            pParent = pParent->iGetParent();
	//        }
	//    }
	//}
	//else
	//{
	//    for (size_t i = 0; i < nCount; i++)
	//    {
	//        ILight* pLight = pRootNode->iGetChild(i)->GetDynamicComponent<ILight>();
	//        if (pRootNode->iGetChild(i) && !pLight)
	//        {
	//            pRootNode->iGetChild(i)->iSetVisible(bVisible);
	//        }
	//    }
	//    if (pTopGroupNode)
	//    {
	//        if (pTopGroupNode != pRootNode)
	//        {
	//            pTopGroupNode->iSetVisible(true);
	//            std::vector<INode*> vec;
	//            pTopGroupNode->iGetAllChildren(vec);
	//            for (size_t i = 0; i < vec.size(); ++i)
	//            {
	//                vec[i]->iSetVisible(bVisible);
	//            }
	//        }
	//        else
	//        {
	//            int nCount = pTopGroupNode->iGetChildCount();
	//            for (int i = 0; i < nCount; ++i)
	//            {
	//                pTopGroupNode->iGetChild(i)->iSetVisible(bVisible);
	//            }
	//        }
	//    }

	//    {
	//        BCEvent tEvent(PROJECTBASED_EVENT_APP_MODE_CHANGED);
	//        APIProvider::GetSystemAPI()->iProjectBasedAPI->iGetStateManager()->iExecuteEvent(tEvent);
	//    }
	//}
}

void IsolatedTool::excBuilding()
{
	INode* pNode = m_pSystemAPI->iCoreAPI->iGetViewManager()->iFindNodeByID("Isolated");
	if (pNode)
	{
		IDrawLine* pDrawLine = pNode->GetDynamicComponent<IDrawLine>();
		if (pDrawLine)
		{
			pDrawLine->iStopDraw();
			pDrawLine->iClearPoint();
		}
		pNode->iSetVisible(false);
	}
}

void IsolatedTool::IndividualBuilding(bool bIsIn)
{
	IViewManager* pViewManager = m_pSystemAPI->iCoreAPI->iGetViewManager();
	RETURN_IF_PTR_IS_NULL(pViewManager);

	INode* pNode = pViewManager->iFindNodeByID("PolygonSelector");
	RETURN_IF_PTR_IS_NULL(pNode);

	IPolygonSelector* pPolygonSelector = pNode->GetDynamicComponent<IPolygonSelector>();
	RETURN_IF_PTR_IS_NULL(pPolygonSelector);

	if (pPolygonSelector)
	{
		//回调
		pPolygonSelector->iSetCallback(YBSSModule::Static_CreateShellRootNode);

		pNode->iSetVisible(true);
		if (!bIsIn)
		{
			m_pSystemAPI->iCoreAPI->iGetViewManager()->iGetCurrentView()->iSetView(ViewDriction_e::TOP_VIEW);
		}
		pPolygonSelector->iSetEditor(true);
		pPolygonSelector->iReset();
		m_pSystemAPI->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_POLYGON_SELECTOR;
		pPolygonSelector->iSetSelectType(SelectorType::Selector_Polygon);
	}
}

void IsolatedTool::VerticalTorusCut(bool bIsIn)
{
	IViewManager* pViewManager = m_pSystemAPI->iCoreAPI->iGetViewManager();
	RETURN_IF_PTR_IS_NULL(pViewManager);

	INode* pNode = pViewManager->iFindNodeByID("PolygonSelector");
	RETURN_IF_PTR_IS_NULL(pNode);

	IPolygonSelector* pPolygonSelector = pNode->GetDynamicComponent<IPolygonSelector>();
	RETURN_IF_PTR_IS_NULL(pPolygonSelector);

	if (pPolygonSelector)
	{
		//回调
		pPolygonSelector->iSetCallback(YBSSModule::Static_CreateShellNodesSplitVerticalRect);

		pNode->iSetVisible(true);
		if (!bIsIn)
		{
			m_pSystemAPI->iCoreAPI->iGetViewManager()->iGetCurrentView()->iSetView(ViewDriction_e::EDITOR_VIEW);
		}
		pPolygonSelector->iSetEditor(true);
		pPolygonSelector->iReset();
		m_pSystemAPI->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_POLYGON_SELECTOR;
		pPolygonSelector->iSetSelectType(SelectorType::Selector_Polygon);
	}
}

void IsolatedTool::VerticalFlushCut(bool bIsIn)
{
	IViewManager* pViewManager = m_pSystemAPI->iCoreAPI->iGetViewManager();
	RETURN_IF_PTR_IS_NULL(pViewManager);

	INode* pNode = pViewManager->iFindNodeByID("PolygonSelector");
	RETURN_IF_PTR_IS_NULL(pNode);

	IPolygonSelector* pPolygonSelector = pNode->GetDynamicComponent<IPolygonSelector>();
	RETURN_IF_PTR_IS_NULL(pPolygonSelector);

	if (pPolygonSelector)
	{
		//回调
		pPolygonSelector->iSetCallback(YBSSModule::Static_CreateShellNodesSplitVerticalPlanel);

		pNode->iSetVisible(true);
		if (!bIsIn)
		{
			m_pSystemAPI->iCoreAPI->iGetViewManager()->iGetCurrentView()->iSetView(ViewDriction_e::EDITOR_VIEW);
		}
		pPolygonSelector->iSetEditor(true);
		pPolygonSelector->iReset();
		m_pSystemAPI->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_POLYGON_SELECTOR;
		pPolygonSelector->iSetSelectType(SelectorType::Selector_Line);
	}
}

void IsolatedTool::VerticalSingleCut(bool bIsIn)
{
	IViewManager* pViewManager = m_pSystemAPI->iCoreAPI->iGetViewManager();
	RETURN_IF_PTR_IS_NULL(pViewManager);

	INode* pNode = pViewManager->iFindNodeByID("PolygonSelector");
	RETURN_IF_PTR_IS_NULL(pNode);

	IPolygonSelector* pPolygonSelector = pNode->GetDynamicComponent<IPolygonSelector>();
	RETURN_IF_PTR_IS_NULL(pPolygonSelector);

	if (pPolygonSelector)
	{
		//回调
		pPolygonSelector->iSetCallback(YBSSModule::Static_CreateShellNodesSplitVerticalSingleCut);

		pNode->iSetVisible(true);
		if (!bIsIn)
		{
			m_pSystemAPI->iCoreAPI->iGetViewManager()->iGetCurrentView()->iSetView(ViewDriction_e::EDITOR_VIEW);
		}
		pPolygonSelector->iSetEditor(true);
		pPolygonSelector->iReset();
		m_pSystemAPI->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_POLYGON_SELECTOR;
		pPolygonSelector->iSetSelectType(SelectorType::Selector_Line);
	}
}

void IsolatedTool::HorizontalCut(bool bIsIn)
{
	IViewManager* pViewManager = m_pSystemAPI->iCoreAPI->iGetViewManager();
	RETURN_IF_PTR_IS_NULL(pViewManager);

	INode* pNode = pViewManager->iFindNodeByID("PolygonSelector");
	RETURN_IF_PTR_IS_NULL(pNode);

	IPolygonSelector* pPolygonSelector = pNode->GetDynamicComponent<IPolygonSelector>();
	RETURN_IF_PTR_IS_NULL(pPolygonSelector);

	if (pPolygonSelector)
	{
		//回调
		pPolygonSelector->iSetCallback(YBSSModule::Static_CreateShellNodesSplitSingleHorizontal);

		pNode->iSetVisible(true);
		if (!bIsIn)
		{
			m_pSystemAPI->iCoreAPI->iGetViewManager()->iGetCurrentView()->iSetView(ViewDriction_e::EDITOR_VIEW);
		}
		pPolygonSelector->iSetEditor(true);
		pPolygonSelector->iReset();
		m_pSystemAPI->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_POLYGON_SELECTOR;
		pPolygonSelector->iSetSelectType(SelectorType::Selector_Point);
	}
}

void IsolatedTool::HorizontalFullCut(bool bIsIn)
{
	IViewManager* pViewManager = m_pSystemAPI->iCoreAPI->iGetViewManager();
	RETURN_IF_PTR_IS_NULL(pViewManager);

	INode* pNode = pViewManager->iFindNodeByID("PolygonSelector");
	RETURN_IF_PTR_IS_NULL(pNode);

	IPolygonSelector* pPolygonSelector = pNode->GetDynamicComponent<IPolygonSelector>();
	RETURN_IF_PTR_IS_NULL(pPolygonSelector);

	if (pPolygonSelector)
	{
		//回调
		pPolygonSelector->iSetCallback(YBSSModule::Static_CreateShellNodesSplitAllHorizontal);

		pNode->iSetVisible(true);
		if (!bIsIn)
		{
			m_pSystemAPI->iCoreAPI->iGetViewManager()->iGetCurrentView()->iSetView(ViewDriction_e::EDITOR_VIEW);
		}
		pPolygonSelector->iSetEditor(true);
		pPolygonSelector->iReset();
		m_pSystemAPI->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_POLYGON_SELECTOR;
		pPolygonSelector->iSetSelectType(SelectorType::Selector_Point);
	}
}

void IsolatedTool::SaveChanges()
{
	//setNodeVisable(nullptr, true);

	//INode* pDrawNode = m_pSystemAPI->iCoreAPI->iGetViewManager()->iFindNodeByID("Isolated");
	//RETURN_IF_PTR_IS_NULL(pDrawNode);

	//IDrawLine* pDrawLine = pDrawNode->GetDynamicComponent<IDrawLine>();
	//RETURN_IF_PTR_IS_NULL(pDrawLine);

	//pDrawLine->iSaveSplit();
}

void IsolatedTool::Cancel()
{
	//setNodeVisable(nullptr, true);

	//INode* pDrawNode = m_pSystemAPI->iCoreAPI->iGetViewManager()->iFindNodeByID("Isolated");
	//RETURN_IF_PTR_IS_NULL(pDrawNode);

	//IDrawLine* pDrawLine = pDrawNode->GetDynamicComponent<IDrawLine>();
	//RETURN_IF_PTR_IS_NULL(pDrawLine);

	//pDrawLine->iCancelSplit();
}

void IsolatedTool::CancelPonlygonSelector()
{
	IViewManager* pViewManager = m_pSystemAPI->iCoreAPI->iGetViewManager();
	RETURN_IF_PTR_IS_NULL(pViewManager);

	INode* pNode = pViewManager->iFindNodeByID("PolygonSelector");
	RETURN_IF_PTR_IS_NULL(pNode);

	IPolygonSelector* pPolygonSelector = pNode->GetDynamicComponent<IPolygonSelector>();
	RETURN_IF_PTR_IS_NULL(pPolygonSelector);

	if (pPolygonSelector)
	{
		pNode->iSetVisible(false);
		pPolygonSelector->iFinish();
	}
}

INode* IsolatedTool::GetIsolatedNode()
{
	INode* pPolygonSelector = m_pSystemAPI->iCoreAPI->iGetViewManager()->iFindNodeByID("PolygonSelector");
	return pPolygonSelector;
}

