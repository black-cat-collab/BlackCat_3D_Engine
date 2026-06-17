#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/utils/tools/AreaMeasureTool.h"

using namespace bc;

AreaMeasureTool::PointFactory::PointFactory():
	CommonLabelFactory()
{

}

AreaMeasureTool::PointFactory::~PointFactory()
{

}

INode* AreaMeasureTool::PointFactory::iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo)
{
	AreaMeasureTool* pTool = (AreaMeasureTool*)GetExtraData("tool");
	
	INode *pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(pTool->m_tPointBoardParam);
	pNode->iLoadModel();
	pNode->iSetVisible(false);
	pNode->iSetSaveMyself(false);
	APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode()->iAddChild(pNode);

	return pNode;
}

void AreaMeasureTool::PointFactory::iUpdateDataForLabel(FactoryAssemble *pAssemble)
{
	AreaMeasureTool::LocalPointData* pInfo = (AreaMeasureTool::LocalPointData*)pAssemble->pData;
	pAssemble->pLabelNode->iSetOrigin(pInfo->vLabelPos);
}

/// //////////////////////////////////////////////////////

AreaMeasureTool::AreaMeasureTool():
	m_pPolygon(nullptr),
	m_pLocusLine(nullptr),
	m_pFontBoard(nullptr),
	m_bEnable(false),
	m_eState(STATE_NONE),
	m_bShowFontBoard(true)
{
	m_tPolygonParam.vBaseColor = Vector4(0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 0.6);
	m_tPolygonParam.vLineColor = Vector4(0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1);
	m_tPolygonParam.fLineWidth = 3.0f;
	m_tPolygonParam.bDepthTest = false;
	m_tPolygonParam.bWithLine = true;

	m_pPointFactory = new PointFactory();
	m_pPointFactory->SetExtraData("tool",(INT_PTR)this);
	m_pPointFactory->SetAllVisible(true);
}

AreaMeasureTool::~AreaMeasureTool()
{
	if (m_pLocusLine)
	{
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pLocusLine, false);
	}
	if (m_pPolygon)
	{
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pPolygon, false);
	}
	if (m_pFontBoard)
	{
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pFontBoard, false);
	}

	DELETE_PTR(m_pPointFactory);
}

void AreaMeasureTool::SetStyleParam(PolygonParam& tParam)
{
	m_tPolygonParam = tParam;
}

void AreaMeasureTool::SetFontBoardParam(FontBoardParam& tParam, std::string& strTextID)
{
	m_tFontBoardParam = tParam;
	m_strFontBoardTextID = strTextID;
}

void AreaMeasureTool::SetEnable(const bool& bEnable)
{
	m_bEnable = bEnable;
	if (!m_bEnable)
	{
		Cancel();
	}
	if (!m_pLocusLine)
	{
		//创建线的节点
		m_tLocusLineParam.bDepthTest = false;
		m_pLocusLine = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(m_tLocusLineParam);
		m_pLocusLine->iLoadModel();
		m_pLocusLine->iSetVisible(false);
		m_pLocusLine->iSetSaveMyself(false);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode()->iAddChild(m_pLocusLine);
	}
	if (!m_pFontBoard)
	{
		//创建文字面板
		m_tFontBoardParam.bDepthTest = false;
		m_pFontBoard = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(m_tFontBoardParam);
		m_pFontBoard->GetDynamicComponent<IRenderNode>()->iSetRenderSort(RENDER_SORT_BOARD + 50);
		m_pFontBoard->iLoadModel();
		m_pFontBoard->iSetVisible(false);
		m_pFontBoard->iSetSaveMyself(false);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode()->iAddChild(m_pFontBoard);
		std::vector<TextShow*> vecTextShow;
		FindAllTextShow(m_pFontBoard, vecTextShow,m_strFontBoardTextID);
		//if (vecTextShow.size() > 0)
		//{
		//	vecTextShow[0]->bAdjustWidth = true;
		//	vecTextShow[0]->bCenter = true;
		//}
	}
}

EventReturnType_e AreaMeasureTool::iProcessEvent(const BCEvent& tEvent)
{
	if (!m_bEnable)
	{
		return EventReturnType_e::NONE;
	}

	ISystemAPI* pSystemAPI = APIProvider::GetSystemAPI();
	INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
	ICamera* pMainCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
	IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));
	if (tEvent.eMessage == EVENT_MOUSE)
	{
		if (!APIProvider::GetSystemAPI()->iProjectClientAPI->iCanHandleScene(Vector2(tEvent.nParam2, tEvent.nParam3)))
		{
			return EventReturnType_e::NONE;
		}
		switch (tEvent.nParam1)
		{
		case MOUSE_LBUTTONDOWN:
		{
			m_vLastMousePos = Vector2(tEvent.nParam2, tEvent.nParam3);
		}
		break;
		case MOUSE_LBUTTONUP:
		{
			if (m_eState != STATE_END)
			{
				Vector2 vCurMousePos = Vector2(tEvent.nParam2, tEvent.nParam3);
				if (m_vLastMousePos.Distance(vCurMousePos) > 3)
				{
					return EventReturnType_e::NONE;
				}
				m_vecLinePoint.push_back(pManipulator->iGetMousePoint());

				LocalPointData* pPointData = new LocalPointData;
				pPointData->vLabelPos = pManipulator->iGetMousePoint();
				m_vecPointData.push_back(pPointData); 
				m_pPointFactory->RefreshData((std::vector<ProjectBasedSceneElement*>*)& m_vecPointData);

				m_eState = STATE_MOVE;
			}
		}
		break;
		case MOUSE_LBUTTONDBLCLK:
		{
			//生成polygon
			if (m_eState == STATE_END)
			{
				return EventReturnType_e::NONE;
			}
			if (m_pPolygon)
			{
				m_pPolygon->iSetVisible(false);
				APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pPolygon, false);
			}
			//if (m_vecLinePoint.size() > 0)
			//{
			//	m_vecLinePoint.pop_back();
			//}
			if (m_vecLinePoint.size() < 3)
			{
				return EventReturnType_e::NONE;
			}
			m_tPolygonParam.vecVertex = ConvertVecVector3dTo(m_vecLinePoint);
			m_pPolygon = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(m_tPolygonParam);
			m_pPolygon->iLoadModel();
			m_pPolygon->iSetVisible(m_bEnable);
			m_pPolygon->iSetSaveMyself(false);
			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode()->iAddChild(m_pPolygon);
			//设置文字
			float fAreaTotal = calcArea(m_vecLinePoint);
			char szAreaTotal[128] = { 0 };
			if (fAreaTotal < 1000000)
			{
				//小于1平方千米
				sprintf_s(szAreaTotal, sizeof(szAreaTotal), "总面积:%.2f平方米", fAreaTotal);
			}
			else
			{
				//大于1平方千米
				sprintf_s(szAreaTotal, sizeof(szAreaTotal), "总面积:%.3f平方千米", fAreaTotal/1000000.0f);
			}
			if (m_pFontBoard)
			{
				std::string strAreaTotalAscii = Utf8ToAscii(szAreaTotal);

				SetTextShowByStrKey(m_pFontBoard, m_strFontBoardTextID, strAreaTotalAscii);
				m_pFontBoard->iSetVisible(m_bShowFontBoard);
				Vector3d vFontBoardPos;
				for (size_t i = 0; i < m_vecLinePoint.size(); i++)
				{
					vFontBoardPos += m_vecLinePoint[i];
				}
				vFontBoardPos /= m_vecLinePoint.size();
				m_pFontBoard->iSetOrigin(vFontBoardPos);
			}
			for (size_t i = 0; i < m_vecCompleteFunc.size(); i++)
			{
				m_vecCompleteFunc[i](m_pPolygon,m_mapINTPTR, fAreaTotal);
			}

			m_pLocusLine->iSetVisible(false);
			m_eState = STATE_END;

		}
		break;
		case MOUSE_RBUTTONDOWN:
		{
			if (!pManipulator->iIsSwitchRBMB())
			{
				m_eState = STATE_NONE;
				Cancel();
			}
		}
		break;
		case MOUSE_MOVE:
		{
			if (m_eState == STATE_MOVE)
			{
				std::vector<Vector3d> vecPoint = m_vecLinePoint;
				vecPoint.push_back(pManipulator->iGetMousePoint());
				//更新线
				ILocusLine* pLocus = GetComponent<ILocusLine>(m_pLocusLine);
				if (pLocus)
				{
					m_pLocusLine->iSetVisible(true);
					std::vector<LocusLineSegment> vecSegment;
					LocusLineSegment sSeg;
					sSeg.vColor = m_tPolygonParam.vLineColor;
					sSeg.fWidth = m_tPolygonParam.fLineWidth;
					sSeg.vecVertex = ConvertVecVector3dTo(vecPoint);
					vecSegment.push_back(sSeg);
					pLocus->iUpdateLocusLineSegment(vecSegment);

					m_pLocusLine->iSetVisible(true);
				}
			}
		}
		break;
		default:
			break;
		}
	}
	else if (tEvent.eMessage == EVENT_KEY)
	{
		if (APIProvider::GetSystemAPI()->iProjectClientAPI 
			&& APIProvider::GetSystemAPI()->iProjectClientAPI->iCanRespondKey())
		{
			int nKeyCode = tEvent.nParam1;
			if (nKeyCode == 27)
			{
				//ESC 取消
				m_eState = STATE_NONE;
				Cancel();
			}
		}
	}

	return EventReturnType_e::NONE;
}

void AreaMeasureTool::Cancel()
{
	if (m_pPolygon)
	{
		m_pPolygon->iSetVisible(false);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pPolygon, false);
		m_pPolygon = nullptr;
	}
	if (m_pLocusLine)
	{
		m_pLocusLine->iSetVisible(false);
	}
	if (m_pFontBoard)
	{
		m_pFontBoard->iSetVisible(false);
	}
	m_vecLinePoint.clear();
	m_vecLinePoint.resize(0);

	m_pPointFactory->RefreshZero();
	for (size_t i = 0; i < m_vecPointData.size(); i++)
	{
		DELETE_PTR(m_vecPointData[i]);
	}
	m_vecPointData.clear();

	m_eState = STATE_NONE;
}

void AreaMeasureTool::AddINTPTR(std::string strKey, INT_PTR pTr)
{
	m_mapINTPTR[strKey] = pTr;
}

INT_PTR AreaMeasureTool::GetINTPTR(std::string strKey)
{
	return m_mapINTPTR[strKey];
}

void AreaMeasureTool::AddCompleteFunc(AreaMeasureCompleteFunc pFunc)
{
	std::vector<AreaMeasureCompleteFunc>::iterator it = find(m_vecCompleteFunc.begin(), m_vecCompleteFunc.end(), pFunc);
	if (it != m_vecCompleteFunc.end())
	{
		return;
	}
	m_vecCompleteFunc.push_back(pFunc);
}

void AreaMeasureTool::RemoveCompleteFunc(AreaMeasureCompleteFunc pFunc)
{
	for (std::vector<AreaMeasureCompleteFunc>::iterator it = m_vecCompleteFunc.begin(); it != m_vecCompleteFunc.end(); it++)
	{
		if ((*it) == pFunc)
		{
			m_vecCompleteFunc.erase(it);
			break;
		}
	}
}

double AreaMeasureTool::calcArea(std::vector<Vector3d>& vecPoint)
{
	std::vector<std::vector<Vector3>> vecConvex;
	BaseMath::SegmentPolygon(ConvertVecVector3dTo(vecPoint), vecConvex);
	if (vecConvex.size() == 0)
	{
		return 0.0f;
	}

	float fAreaTotal = 0.0f;
	for (int i = 0; i < vecConvex.size(); i++)
	{
		std::vector<Vector3>& vecArea = vecConvex[i];
		for (int j = 0; j < vecArea.size() - 2; ++j)
		{
			int nElem0 = 0;
			int nElem1 = j + 1;
			int nElem2 = j + 2;

			Vector3 v0 = vecArea[nElem0];
			Vector3 v1 = vecArea[nElem1];
			Vector3 v2 = vecArea[nElem2];

			float fA = v0.Distance(v1);
			float fB = v1.Distance(v2);
			float fC = v2.Distance(v0);
			float fP = (fA + fB + fC) / 2;

			fAreaTotal += sqrt(fP * (fP - fA) * (fP - fB) * (fP - fC));
		}
	}

	return fAreaTotal;
}

void AreaMeasureTool::SetPointParam(BoardParam& tParam)
{
	m_tPointBoardParam = tParam;
}

