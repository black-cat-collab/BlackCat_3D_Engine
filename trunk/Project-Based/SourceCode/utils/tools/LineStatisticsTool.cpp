#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/utils/tools/BaseStatisticsTool.h"
#include "Project-Based/utils/tools/LineStatisticsTool.h"
#include "Project-Based/base/ProjectBasedClient.h"

using namespace bc;

LineStatisticsTool::LineStatisticsTool() : BaseStatisticsTool(),
	m_pRoad(nullptr),
	m_pLocusLine(nullptr)
{
	m_eType = STATISTICS_LINE;
	m_bOutsizePolygon = false;

	LocusLineSegment sSegment;
	sSegment.fWidth = 3.0f;
	sSegment.vColor = Vector4(1, 0, 0, 1);
	m_tLocusLineParam.stDefaultSegment = sSegment;
	m_tLocusLineParam.bDepthTest = false;
}

LineStatisticsTool::~LineStatisticsTool()
{
	if (m_pLocusLine)
	{
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pLocusLine, false);
	}
	if (m_pRoad)
	{
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pRoad, false);
	}
}

void LineStatisticsTool::SetStyleParam(RoadParam& tParam)
{
	m_tRoadParam = tParam;
	m_tRoadParam.bIntersectEnable = true;
}

void LineStatisticsTool::SetLineParam(LocusLineSegment& tParam)
{
	//创建线的节点
	m_tLocusLineParam.stDefaultSegment = tParam;
}

void LineStatisticsTool::SetRoadWidth(float& fRoadWidth)
{
	m_fRoadWidth = fRoadWidth;
}

void LineStatisticsTool::SetData(std::vector<Vector3d> vecPoints)
{
	if (m_bDoingStatistics)
	{
		return;
	}
	m_vecLinePoint = vecPoints;

	//生成polygon
	if (m_pRoad)
	{
		m_pRoad->iSetVisible(false);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pRoad, false);
	}

	std::vector<std::vector<Vector3>> vecRoad;
	vecRoad.push_back(ConvertVecVector3dTo(m_vecLinePoint));

	m_tRoadParam.vecVertex = vecRoad;
	m_pRoad = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(m_tRoadParam);
	m_pRoad->iLoadModel();
	m_pRoad->iSetVisible(m_bEnable);
	m_pRoad->iSetSaveMyself(false);
	APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(m_pRoad);
	m_vecRoad.push_back(m_pRoad);

	m_pLocusLine->iSetVisible(false);
	m_eState = STATE_END;
	m_bCanStatistics = true;
}

void LineStatisticsTool::SetRoad(std::vector<INode*> vecRoad)
{
	Cancel();
	m_vecRoad = vecRoad;
	m_bDoingStatistics = true;
	m_bOutsizePolygon = true;
	m_eState = STATE_END;
}

void LineStatisticsTool::SetEnable(const bool& bEnable)
{
	BaseStatisticsTool::SetEnable(bEnable);
	if (!m_pLocusLine)
	{
		m_pLocusLine = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(m_tLocusLineParam);
		m_pLocusLine->iLoadModel();
		m_pLocusLine->iSetVisible(false);
		m_pLocusLine->iSetSaveMyself(false);
		APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(m_pLocusLine);
	}
}

void LineStatisticsTool::GetAllRoad(std::vector<INode*>& vecRoad)
{
	vecRoad = m_vecRoad;
}

EventReturnType_e LineStatisticsTool::iProcessEvent(const BCEvent& tEvent)
{
	if (!m_bEnable)
	{
		return EventReturnType_e::NONE;
	}

	if (m_bDoingStatistics)
	{
		return EventReturnType_e::NONE;
	}

	if (m_bOutsizePolygon)
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
			Vector2 vCurMousePos = Vector2(tEvent.nParam2, tEvent.nParam3);
			if (m_vLastMousePos.Distance(vCurMousePos) > 3)
			{
				return EventReturnType_e::NONE;
			}
			if (m_eState != STATE_END)
			{
				m_vecLinePoint.push_back(pManipulator->iGetMousePoint());

				LocalPointData* pPointData = new LocalPointData;
				pPointData->vPos = pManipulator->iGetMousePoint();
				m_vecPointData.push_back(pPointData);
				m_pPointFactory->RefreshData((std::vector<ProjectBasedSceneElement*>*) & m_vecPointData);
				m_pPointFactory->SetAllVisible(true);

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
			if (m_pRoad)
			{
				m_pRoad->iSetVisible(false);
				APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pRoad, false);
			}

			std::vector<std::vector<Vector3>> vecRoad;
			vecRoad.push_back(ConvertVecVector3dTo(m_vecLinePoint));
			std::vector<float> vecWidth;
			vecWidth.push_back(m_fRoadWidth);

			m_tRoadParam.vecVertex = vecRoad;
			m_tRoadParam.vecWidth = vecWidth;
			m_pRoad = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(m_tRoadParam);
			m_pRoad->iLoadModel();
			m_pRoad->iSetVisible(m_bEnable);
			m_pRoad->iSetSaveMyself(false);
			APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(m_pRoad);
			m_vecRoad.push_back(m_pRoad);
			if (m_fnCreateOrCancelFunc)
			{
				m_fnCreateOrCancelFunc(this, true);
			}

			m_pLocusLine->iSetVisible(false);
			m_pPointFactory->SetAllVisible(false);
			m_eState = STATE_END;
			m_bCanStatistics = true;

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

bool LineStatisticsTool::iDoingStatistics(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam)
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		bool bLoaded = true;
		for (size_t i = 0; i < m_vecRoad.size(); i++)
		{
			bLoaded = bLoaded && m_vecRoad[i]->iIsLoad();
		}
		if (bLoaded)
		{
			break;
		}
	}
	return true;
}

void LineStatisticsTool::iCompleteStatistics(ThreadCallbackParam_s* pParam)
{
	for (size_t i = 0; i < m_vecStatisTask.size(); i++)
	{
		StatisTaskData* pData = &m_vecStatisTask[i];
		Vector3 vTarget = pData->vPos;
		bool bIn = false;
		for (size_t i = 0; i < m_vecRoad.size(); i++)
		{
			bIn = IsInRoad(m_vecRoad[i], vTarget);
			if (bIn)
			{
				break;
			}
		}
		if (bIn)
		{
			addKey(pData);
		}
	}

	BaseStatisticsTool::iCompleteStatistics(pParam);
}

bool LineStatisticsTool::IsInRoad(INode* pNode, Vector3 vOrigin)
{
	if (!pNode)
	{
		return false;
	}
	BoundingBoxd box = pNode->iGetBBox();
	if (vOrigin.x <= box._vMaxs.x && vOrigin.x >= box._vMins.x
		&& vOrigin.y <= box._vMaxs.y && vOrigin.y >= box._vMins.y)
	{
		//再判断是否有交点
		Vector3d vStart(vOrigin.x, vOrigin.y, 600000.0f);
		Vector3d vEnd(vOrigin.x, vOrigin.y, -4988420.50f);

		SegmentInstrect si;
		si.Set(vStart, vEnd);
		std::vector<IntersectSet> vSet;
		IRenderNode* pRenderNode = static_cast<IRenderNode*>(pNode->iGetComponent(COMPONENT_TYPE_RENDER_NODE));
		if (pRenderNode)
		{
			pRenderNode->iIntersect(si, vSet);
		}

		if (!vSet.empty())
		{
			return true;
		}
	}
	return false;
}

void LineStatisticsTool::Cancel()
{
	BaseStatisticsTool::Cancel();
	if (m_pRoad)
	{
		m_pRoad->iSetVisible(false);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pRoad, false);
		m_pRoad = nullptr;
	}
	if (m_pLocusLine)
	{
		m_pLocusLine->iSetVisible(false);
	}
	m_vecLinePoint.clear();
	m_vecLinePoint.resize(0);
	m_vecRoad.resize(0);

	m_vecStatisTask.clear();
	m_vecStatisTask.resize(0);
	m_bCanStatistics = false;
	m_bOutsizePolygon = false;
	m_eState = STATE_NONE;

	m_pPointFactory->RefreshZero();
	for (size_t i = 0; i < m_vecPointData.size(); i++)
	{
		DELETE_PTR(m_vecPointData[i]);
	}
	m_vecPointData.clear();
}

void LineStatisticsTool::iLoopStatistics(ThreadCallbackParam_s* pParam)
{
	if (m_vecRoad.size() == 0)
	{
		return;
	}
	BaseStatisticsTool::iLoopStatistics(pParam);
}

