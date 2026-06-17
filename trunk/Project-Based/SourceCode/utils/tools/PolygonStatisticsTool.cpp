#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/utils/tools/BaseStatisticsTool.h"
#include "Project-Based/utils/tools/PolygonStatisticsTool.h"
#include "Project-Based/base/ProjectBasedClient.h"

using namespace bc;

PolygonStatisticsTool::PolygonStatisticsTool(PolygonStatisticsType mType) : BaseStatisticsTool(),
m_pPolygon(nullptr),
m_pLocusLine(nullptr)
{
	m_eType = STATISTICS_POLYGON;
	m_bOutsizePolygon = false;

	m_tPolygonParam.vBaseColor = Vector4(1, 0, 0, 0.6);
	m_tPolygonParam.vLineColor = Vector4(1, 0, 0, 1);
	m_tPolygonParam.fLineWidth = 3.0f;
	m_tPolygonParam.bDepthTest = false;
	m_tPolygonParam.bWithLine = true;

	m_ePolygonStatisticsType = mType;
}

PolygonStatisticsTool::~PolygonStatisticsTool()
{
	if (m_pLocusLine)
	{
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pLocusLine, false);
	}
	if (m_pPolygon)
	{
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pPolygon, false);
	}
}

void PolygonStatisticsTool::SetStyleParam(PolygonParam& tParam)
{
	m_tPolygonParam = tParam;
}

void PolygonStatisticsTool::SetData(std::vector<Vector3d> vecPoints)
{
	if (m_bDoingStatistics)
	{
		return;
	}
	m_vecLinePoint = vecPoints;

	//生成polygon
	if (m_pPolygon)
	{
		m_pPolygon->iSetVisible(false);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pPolygon, false);
	}

	m_tPolygonParam.vecVertex = ConvertVecVector3dTo(m_vecLinePoint);
	m_pPolygon = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(m_tPolygonParam);
	m_pPolygon->iLoadModel();
	m_pPolygon->iSetVisible(m_bEnable);
	m_pPolygon->iSetSaveMyself(false);
	APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(m_pPolygon);
	m_vecPolygon.push_back(m_pPolygon);

	m_pLocusLine->iSetVisible(false);
	m_eState = STATE_END;
	m_bCanStatistics = true;
}

void PolygonStatisticsTool::SetPolygon(std::vector<INode*> vecPolygon)
{
	Cancel();
	m_vecPolygon = vecPolygon;
	m_bCanStatistics = true;
	m_bOutsizePolygon = true;
	m_eState = STATE_END; 
}

void PolygonStatisticsTool::SetEnable(const bool& bEnable)
{
	BaseStatisticsTool::SetEnable(bEnable);
	if (!m_pLocusLine)
	{
		//创建线的节点
		m_tLocusLineParam.bDepthTest = false;
		m_pLocusLine = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(m_tLocusLineParam);
		m_pLocusLine->iLoadModel();
		m_pLocusLine->iSetVisible(false);
		m_pLocusLine->iSetSaveMyself(false);
		APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(m_pLocusLine);
	}
}

void PolygonStatisticsTool::GetAllPolygon(std::vector<INode*>& vecPolygon)
{
	vecPolygon = m_vecPolygon;
}

EventReturnType_e PolygonStatisticsTool::iProcessEvent(const BCEvent& tEvent)
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
			//多边形
			if (m_ePolygonStatisticsType == POLYGON)
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

					m_eState = STATE_MOVE;
				}
			}
			//矩形
			if (m_ePolygonStatisticsType == RECTANGULAR)
			{
				Vector2 vCurMousePos = Vector2(tEvent.nParam2, tEvent.nParam3);
				if (m_vLastMousePos.Distance(vCurMousePos) > 3)
				{
					return EventReturnType_e::NONE;
				}
				if (m_eState != STATE_END)
				{
					if (m_vecLinePoint.size() == 0) 
					{
						m_vecLinePoint.push_back(pManipulator->iGetMousePoint());
					}
					m_eState = STATE_MOVE;
				}
			}
		}
		break;
		case MOUSE_LBUTTONDBLCLK:
		{
			//多边形
			if (m_ePolygonStatisticsType == POLYGON)
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
				APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(m_pPolygon);
				m_vecPolygon.push_back(m_pPolygon);
				if (m_fnCreateOrCancelFunc)
				{
					m_fnCreateOrCancelFunc(this, true);
				}

				m_pLocusLine->iSetVisible(false);
				m_eState = STATE_END;
				m_bCanStatistics = true;
			}
			//矩形
			if (m_ePolygonStatisticsType == RECTANGULAR)
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

				std::vector<Vector3d> vecPoint = m_vecLinePoint;
				Vector2 vPointStart;
				Vector2 vPointEnd = Vector2(tEvent.nParam2, tEvent.nParam3);
				Vector3d vPosEnd;
				if (vecPoint.size() > 0)
				{
					Vector3d vOutStart;
					pMainCamera->iProject(vecPoint[0], vOutStart);
					vPointStart.x = vOutStart.x;
					vPointStart.y = vOutStart.y;
				}

				Vector2 vTemp_1, vTemp_2;
				Vector3d vPosTemp_1, vPosTemp_2;
				vTemp_1.x = vPointStart.x;
				vTemp_1.y = vPointEnd.y;
				vTemp_2.x = vPointEnd.x;
				vTemp_2.y = vPointStart.y;
				
				pMainCamera->iUnProject(vTemp_1.x, vTemp_1.y, vPosTemp_1);
				pMainCamera->iUnProject(vTemp_2.x, vTemp_2.y, vPosTemp_2);
				pMainCamera->iUnProject(vPointEnd.x, vPointEnd.y, vPosEnd);

				vecPoint.push_back(vPosTemp_1);
				vecPoint.push_back(vPosEnd);
				vecPoint.push_back(vPosTemp_2);
				vecPoint.push_back(vecPoint[0]);

				m_vecLinePoint.push_back(vPosTemp_1);
				m_vecLinePoint.push_back(vPosEnd);
				m_vecLinePoint.push_back(vPosTemp_2);

				if (m_vecLinePoint.size() < 3)
				{
					return EventReturnType_e::NONE;
				}
				m_tPolygonParam.vecVertex = ConvertVecVector3dTo(vecPoint);
				m_pPolygon = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(m_tPolygonParam);
				m_pPolygon->iLoadModel();
				m_pPolygon->iSetVisible(m_bEnable);
				m_pPolygon->iSetSaveMyself(false);
				APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(m_pPolygon);
				m_vecPolygon.push_back(m_pPolygon);
				if (m_fnCreateOrCancelFunc)
				{
					m_fnCreateOrCancelFunc(this, true);
				}
				m_pLocusLine->iSetVisible(false);
				m_eState = STATE_END;
				m_bCanStatistics = true;
			}
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
			//多边形
			if (m_ePolygonStatisticsType == POLYGON)
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
			//矩形
			if (m_ePolygonStatisticsType == RECTANGULAR)
			{
				if (m_eState == STATE_MOVE)
				{
					std::vector<Vector3d> vecPoint = m_vecLinePoint;
					Vector2 vPointStart;
					Vector2 vPointEnd = Vector2(tEvent.nParam2, tEvent.nParam3);
					Vector3d vPosEnd;
					if (vecPoint.size() > 0)
					{
						Vector3d vOutStart;
						pMainCamera->iProject(vecPoint[0], vOutStart);
						vPointStart.x = vOutStart.x;
						vPointStart.y = vOutStart.y;
					}

					Vector2 vTemp_1, vTemp_2;
					Vector3d vPosTemp_1, vPosTemp_2;
					vTemp_1.x = vPointStart.x;
					vTemp_1.y = vPointEnd.y;
					vTemp_2.x = vPointEnd.x;
					vTemp_2.y = vPointStart.y;

					pMainCamera->iUnProject(vTemp_1.x, vTemp_1.y, vPosTemp_1);
					pMainCamera->iUnProject(vTemp_2.x, vTemp_2.y, vPosTemp_2);
					pMainCamera->iUnProject(vPointEnd.x, vPointEnd.y, vPosEnd);

					vecPoint.push_back(vPosTemp_1);
					vecPoint.push_back(vPosEnd);
					vecPoint.push_back(vPosTemp_2);
					vecPoint.push_back(vecPoint[0]);
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

bool PolygonStatisticsTool::iDoingStatistics(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam)
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		bool bLoaded = true;
		for (size_t i = 0; i < m_vecPolygon.size(); i++)
		{
			bLoaded = bLoaded && m_vecPolygon[i]->iIsLoad();
		}
		if (bLoaded)
		{
			break;
		}
	}
	return true;
}

void PolygonStatisticsTool::iCompleteStatistics(ThreadCallbackParam_s* pParam)
{
	for (size_t i = 0; i < m_vecStatisTask.size(); i++)
	{
		StatisTaskData* pData = &m_vecStatisTask[i];
		Vector3 vTarget = pData->vPos;
		bool bIn = false;
		for (size_t i = 0; i < m_vecPolygon.size(); i++)
		{
			bIn = IsInPolygon(m_vecPolygon[i], vTarget);
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

bool PolygonStatisticsTool::IsInPolygon(INode* pNode, Vector3 vOrigin)
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

void PolygonStatisticsTool::Cancel()
{
	BaseStatisticsTool::Cancel();

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
	m_vecLinePoint.clear();
	m_vecLinePoint.resize(0);
	m_vecPolygon.resize(0);

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

void PolygonStatisticsTool::iLoopStatistics(ThreadCallbackParam_s* pParam)
{
	if (m_vecPolygon.size() == 0)
	{
		return;
	}
	BaseStatisticsTool::iLoopStatistics(pParam);
}

void PolygonStatisticsTool::GetPolygonParam(PolygonParam& tParam)
{
	tParam = m_tPolygonParam;
}

