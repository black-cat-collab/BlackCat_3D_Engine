#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/utils/tools/BaseStatisticsTool.h"
#include "Project-Based/utils/tools/RoadAuxiliaryTool.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/utils/ParamNetImage.h"

using namespace bc;

RoadAuxiliaryTool::RoadAuxiliaryTool(ProjectBasedClient* pClient) :
	m_pLocusLineRoad(nullptr),
	m_eState(State_e::STATE_NONE),
	m_bCollectedPosForRoad(false)
{
	m_pClient = pClient;
}

RoadAuxiliaryTool::~RoadAuxiliaryTool()
{
}

void RoadAuxiliaryTool::SetLineEnable(bool bEnable)
{
	m_tLocusLineParam.stDefaultSegment.vColor = Vector4(0.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1);
	m_tLocusLineParam.stDefaultSegment.fWidth = 3.0f;
	m_bCollectedPosForRoad = bEnable;
	if (!m_bCollectedPosForRoad)
	{
		CancelRoadLine();
	}
	else 
	{
		m_vecPosForRoad.clear();
	}
	if (!m_pLocusLineRoad)
	{
		//创建线的节点
		m_tLocusLineParam.bDepthTest = true;
		m_pLocusLineRoad = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(m_tLocusLineParam);
		m_pLocusLineRoad->iLoadModel();
		m_pLocusLineRoad->iSetVisible(false);
		m_pLocusLineRoad->iSetSaveMyself(false);
		APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(m_pLocusLineRoad);
	}
}

void RoadAuxiliaryTool::CancelRoadLine()
{
	if (m_pLocusLineRoad)
	{
		m_pLocusLineRoad->iSetVisible(false);
	}
	m_eState = State_e::STATE_NONE;
}

EventReturnType_e RoadAuxiliaryTool::iProcessEvent(const BCEvent& tEvent)
{
	//画线部分 路线创建
	if (m_bCollectedPosForRoad)
	{
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
				if (m_eState != State_e::STATE_END)
				{
					Vector2 vCurMousePos = Vector2(tEvent.nParam2, tEvent.nParam3);
					if (m_vLastMousePos.Distance(vCurMousePos) > 3)
					{
						return EventReturnType_e::NONE;
					}
					//处理云渲染最后一个重复点
					if (m_vecPosForRoad.size() >= 3)
					{
						//处理云渲染最后一个重复点
						Vector3d vPos = pManipulator->iGetMousePoint();
						if (vPos.Distance(m_vecPosForRoad.back()) > 0.5f)
						{
							m_vecPosForRoad.push_back(pManipulator->iGetMousePoint());
						}
					}
					else
					{
						m_vecPosForRoad.push_back(pManipulator->iGetMousePoint());
					}
					if (m_vecPosForRoad.size() == 1)
					{
						//设置开始
						m_eState = State_e::STATE_MOVE;
					}
				}
			}
			break;
			case MOUSE_LBUTTONDBLCLK:
			{
				if (m_eState == State_e::STATE_END)
				{
					return EventReturnType_e::NONE;
				}
				m_eState = State_e::STATE_END;
				if (m_pLocusLineRoad)
				{
					if (m_vecPosForRoad.size() > 1)
					{
						int nId = atoi(m_pLocusLineRoad->iGetName().c_str());
						m_eState = State_e::STATE_NONE;
						m_bCollectedPosForRoad = false;
					}
				}
			}break;
			case MOUSE_RBUTTONDOWN:
			{
				if (!pManipulator->iIsSwitchRBMB())
				{
					m_eState = State_e::STATE_NONE;
					CancelRoadLine();
				}
			}
			break;
			case MOUSE_MOVE:
			{
				if (m_eState == State_e::STATE_MOVE)
				{
					std::vector<Vector3d> vecPoint = m_vecPosForRoad;
					vecPoint.push_back(pManipulator->iGetMousePoint());
					//更新线
					ILocusLine* pLocus = GetComponent<ILocusLine>(m_pLocusLineRoad);
					if (pLocus)
					{
						m_pLocusLineRoad->iSetVisible(true);
						vector<LocusLineSegment> vecSegment;
						LocusLineSegment sSeg = m_tLocusLineParam.stDefaultSegment;
						sSeg.vecVertex = ConvertVecVector3dTo(vecPoint);
						vecSegment.push_back(sSeg);
						pLocus->iUpdateLocusLineSegment(vecSegment);
						m_pLocusLineRoad->iSetVisible(true);
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
					m_eState = State_e::STATE_NONE;
					CancelRoadLine();
				}
			}
		}
	}

	return EventReturnType_e::NONE;
}

void RoadAuxiliaryTool::CreateRoadNode(RoadParam* pParam)
{
	m_pRoadNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(*pParam);
	m_pRoadNode->GetDynamicComponent<IRoad>()->iGenerate();
	m_pRoadNode->iSetVisible(true);
	m_pRoadNode->GetDynamicComponent<IRenderNode>()->iSetClickEnable(true);
	m_pRoadNode->GetDynamicComponent<IRenderNode>()->iSetSelectEnable(true);
	ParamNetImage::GetInstance()->AddNetTask(m_pRoadNode, m_pClient->m_strApiHost);
	APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(m_pRoadNode);
}

bc::INode* RoadAuxiliaryTool::GetCurRoadNode()
{
	return m_pRoadNode;
}

void RoadAuxiliaryTool::SetRoadParam(RoadParam& tParam)
{
	m_tRoadParam = tParam;
}

void RoadAuxiliaryTool::GetRoadParam(RoadParam& tParam)
{
	tParam = m_tRoadParam;
}

