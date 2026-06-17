#include "ProjectBasedHeader.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Utility/Math/CoordinateTransform.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/tools/DistanceMeasureTool.h"

using namespace bc;

DistanceMeasureTool::DistanceMeasureTool() :
	m_pLocusLine(nullptr),
	m_pFontBoard(nullptr),
	m_bEnable(false),
	m_eState(STATE_NONE),
	m_bShowFontBoard(true),
	m_pStartPontBoard(nullptr),
	m_pEndPontBoard(nullptr)
{
	m_tLocusLineParam.stDefaultSegment.vColor = Vector4(0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1);
	m_tLocusLineParam.stDefaultSegment.fWidth = 3.0f;
}

DistanceMeasureTool::~DistanceMeasureTool()
{
	if (m_pLocusLine)
	{
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pLocusLine, false);
	}
	if (m_pFontBoard)
	{
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pFontBoard, false);
	}
	if (m_pStartPontBoard)
	{
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pStartPontBoard, false);
	}
	if (m_pEndPontBoard)
	{
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pEndPontBoard, false);
	}
	if (m_vecTempPontBoard.size() > 0)
	{
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(m_vecTempPontBoard, false);
	}
}

void DistanceMeasureTool::SetStyleParam(LocusLineSegment& tSegment)
{
	m_tLocusLineParam.stDefaultSegment = tSegment;
	m_tLocusLineParam.bDepthTest = true;
}

void DistanceMeasureTool::SetFontBoardParam(FontBoardParam& tParam, std::string& strTextID)
{
	m_tFontBoardParam = tParam;
	m_strFontBoardTextID = strTextID;
}

void DistanceMeasureTool::SetEnable(const bool& bEnable)
{
	m_bEnable = bEnable;
	if (!m_bEnable)
	{
		Cancel();
	}
	if (!m_pLocusLine)
	{
		//创建线的节点
		m_tLocusLineParam.bDepthTest = true;
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
		m_pFontBoard->iLoadModel();
		m_pFontBoard->iSetVisible(false);
		m_pFontBoard->iSetSaveMyself(false);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode()->iAddChild(m_pFontBoard);
		std::vector<TextShow*> vecTextShow;
		FindAllTextShow(m_pFontBoard, vecTextShow, m_strFontBoardTextID);
		//if (vecTextShow.size() > 0)
		//{
		//	vecTextShow[0]->bAdjustWidth = true;
		//	vecTextShow[0]->bCenter = true;
		//}
	}
}

EventReturnType_e DistanceMeasureTool::iProcessEvent(const BCEvent& tEvent)
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
				if (m_vecLinePoint.size() >= 2)
				{
					//处理云渲染最后一个重复点
					Vector3d vPos = pManipulator->iGetMousePoint();
					if (vPos.Distance(m_vecLinePoint.back()) > 0.5f)
					{
						m_vecLinePoint.push_back(pManipulator->iGetMousePoint());
					}
				}
				else
				{
					m_vecLinePoint.push_back(pManipulator->iGetMousePoint());
				}
				if (m_vecLinePoint.size() == 1)
				{
					//设置开始
					m_eState = STATE_MOVE;
					if (m_pStartPontBoard)
					{
						m_pStartPontBoard->iSetOrigin(m_vecLinePoint[0]);
						m_pStartPontBoard->iSetVisible(true);
					}
				}
				else if (m_vecLinePoint.size() > 1)
				{
					//创建中间点位
					INode* pTempPoint = nullptr;
					pTempPoint = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(m_tBoardParam);
					pTempPoint->iLoadModel();
					pTempPoint->iSetVisible(true);
					pTempPoint->iSetSaveMyself(false);
					APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode()->iAddChild(pTempPoint);
					if (pTempPoint)
					{
						pTempPoint->iSetOrigin(*m_vecLinePoint.crbegin());
						m_vecTempPontBoard.push_back(pTempPoint);
					}
					//创建文字面板
					INode* pTempFontBoard = nullptr;
					m_tFontBoardParam.bDepthTest = false;
					std::string strId = Guid::CreateGuId();
					m_tFontBoardParam.vecText[0].strID = strId;
					pTempFontBoard = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(m_tFontBoardParam);
					pTempFontBoard->iLoadModel();
					pTempFontBoard->iSetVisible(true);
					pTempFontBoard->iSetSaveMyself(false);
					APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode()->iAddChild(pTempFontBoard);
					if (pTempFontBoard)
					{
						m_vecTempFontBoard.push_back(pTempFontBoard);
						//计算两个点的距离并显示
						auto pPreviousPoint = m_vecLinePoint.crbegin();
						auto pThisPoint = m_vecLinePoint.crbegin() + 1;
						if (pThisPoint != m_vecLinePoint.crend())
						{
							float fDis = (*pPreviousPoint).Distance(*pThisPoint);
							pTempFontBoard->iSetOrigin(((*pPreviousPoint) + (*pThisPoint)) / 2);
							char szDis[128] = { 0 };
							if (fDis < 1000)
							{
								sprintf_s(szDis, sizeof(szDis), "总长度: %.2f米", fDis);
							}
							else
							{
								sprintf_s(szDis, sizeof(szDis), "总长度: %.3f千米", fDis / 1000.0f);
							}

							std::string strDistanceAscii = Utf8ToAscii(szDis);

							SetTextShowByStrKey(pTempFontBoard, strId, strDistanceAscii);
							m_vecDistance.push_back(fDis);
						}
					}
				}
			}
		}
		break;
		case MOUSE_LBUTTONDBLCLK:
		{
			if (m_eState == STATE_END) 
			{
				return EventReturnType_e::NONE;
			}
			m_eState = STATE_END;
			for (size_t i = 0; i < m_vecCompleteFunc.size(); i++)
			{
				m_vecCompleteFunc[i](m_pLocusLine, m_mapINTPTR, m_vecDistance);
			}
		}break;
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
					LocusLineSegment sSeg = m_tLocusLineParam.stDefaultSegment;
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

void DistanceMeasureTool::Cancel()
{
	if (m_pLocusLine)
	{
		m_pLocusLine->iSetVisible(false);
	}
	if (m_pFontBoard)
	{
		m_pFontBoard->iSetVisible(false);
	}
	if (m_pStartPontBoard)
	{
		m_pStartPontBoard->iSetVisible(false);
	}
	if (m_pEndPontBoard)
	{
		m_pEndPontBoard->iSetVisible(false);
	}
	for (size_t i = 0; i < m_vecTempPontBoard.size(); i++)
	{
		m_vecTempPontBoard[i]->iSetVisible(false);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_vecTempPontBoard[i], false);
		DELETE_PTR(m_vecTempPontBoard[i]);
	}
	m_vecTempPontBoard.clear();
	m_vecTempPontBoard.resize(0);
	for (size_t i = 0; i < m_vecTempFontBoard.size(); i++)
	{
		m_vecTempFontBoard[i]->iSetVisible(false);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_vecTempFontBoard[i], false);
		DELETE_PTR(m_vecTempFontBoard[i]);
	}
	m_vecTempFontBoard.clear();
	m_vecTempFontBoard.resize(0);
	m_vecLinePoint.clear();
	m_vecLinePoint.resize(0);
	m_vecDistance.clear();
	m_vecDistance.resize(0);
	m_eState = STATE_NONE;
}

void DistanceMeasureTool::AddINTPTR(std::string strKey, INT_PTR pTr)
{
	m_mapINTPTR[strKey] = pTr;
}

INT_PTR DistanceMeasureTool::GetINTPTR(std::string strKey)
{
	return m_mapINTPTR[strKey];
}

void DistanceMeasureTool::AddCompleteFunc(DistanceMeasureCompleteFunc pFunc)
{
	std::vector<DistanceMeasureCompleteFunc>::iterator it = find(m_vecCompleteFunc.begin(), m_vecCompleteFunc.end(), pFunc);
	if (it != m_vecCompleteFunc.end())
	{
		return;
	}
	m_vecCompleteFunc.push_back(pFunc);
}

void DistanceMeasureTool::RemoveCompleteFunc(DistanceMeasureCompleteFunc pFunc)
{
	for (std::vector<DistanceMeasureCompleteFunc>::iterator it = m_vecCompleteFunc.begin(); it != m_vecCompleteFunc.end(); it++)
	{
		if ((*it) == pFunc)
		{
			m_vecCompleteFunc.erase(it);
			break;
		}
	}
}

void DistanceMeasureTool::SetStartPointBoardParam(BoardParam& tParam)
{
	if (m_pStartPontBoard)
	{
		m_pStartPontBoard->iSetVisible(false);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pStartPontBoard, false);
	}
	m_tBoardParam = tParam;
	m_pStartPontBoard = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(tParam);
	m_pStartPontBoard->iLoadModel();
	m_pStartPontBoard->iSetVisible(false);
	m_pStartPontBoard->iSetSaveMyself(false);
	APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode()->iAddChild(m_pStartPontBoard);
}

void DistanceMeasureTool::SetEndPointBoardParam(BoardParam& tParam)
{
	if (m_pEndPontBoard)
	{
		m_pEndPontBoard->iSetVisible(false);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pEndPontBoard, false);
	}
	m_pEndPontBoard = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(tParam);
	m_pEndPontBoard->iLoadModel();
	m_pEndPontBoard->iSetVisible(false);
	m_pEndPontBoard->iSetSaveMyself(false);
	APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode()->iAddChild(m_pEndPontBoard);
}

