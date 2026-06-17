#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/utils/tools/BaseStatisticsTool.h"
#include "Project-Based/utils/tools/CircleStatisticsTool.h"
#include "Project-Based/base/ProjectBasedClient.h"

using namespace bc;

CircleStatisticsTool::CircleStatisticsTool(bool isPOIStatistics) : BaseStatisticsTool(),
	m_pPolygon(nullptr),
	m_pLocusLine(nullptr)
{
	m_nStatisticsType = StatisticsType::Circle_QX;
	m_eType = STATISTICS_CIRCLE;

	m_tPolygonParam.vBaseColor = Vector4(1, 0, 0, 0.6);
	m_tPolygonParam.vLineColor = Vector4(1, 0, 0, 1);
	m_tPolygonParam.fLineWidth = 3.0f;
	m_tPolygonParam.bDepthTest = false;
	m_tPolygonParam.bWithLine = true;
	
	m_bIsPOIStatistics = isPOIStatistics;
}

CircleStatisticsTool::~CircleStatisticsTool()
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

void CircleStatisticsTool::SetStyleParam(PolygonParam& tParam)
{
	m_tPolygonParam = tParam;
}

void CircleStatisticsTool::SetData(const Vector3d& vCenterPos, const float& fRadius)
{
	if (m_bDoingStatistics)
	{
		return;
	}
	m_fRadius = fRadius;
	m_vCenterPos = vCenterPos;

	//分割线点
	calcLinePoint(vCenterPos,fRadius);

	//生成polygon
	if (m_pPolygon)
	{
		m_pPolygon->iSetVisible(false);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pPolygon, false);
	}

	m_tPolygonParam.vecVertex = ConvertVecVector3dTo(m_vecLinePoint);
	m_tPolygonParam.vecVertex.pop_back();	//去掉最后一个值，不然有重复
	m_pPolygon = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(m_tPolygonParam);
	m_pPolygon->iLoadModel();
	m_pPolygon->iSetVisible(m_bEnable);
	m_pPolygon->iSetSaveMyself(false);
	APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(m_pPolygon);

	m_pLocusLine->iSetVisible(false);
	m_eState = STATE_END;
	m_bCanStatistics = true;
}

void CircleStatisticsTool::SetEnable(const bool& bEnable)
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

void CircleStatisticsTool::calcLinePoint(const Vector3d& vCenterPos, const float& fRadius)
{
	m_vecLinePoint.clear();
	m_vecLinePoint.resize(0);

	//分割
	int nCount = 200;
	float fDeltaAngle = 360.0f / nCount;
	for (int zz = 0; zz <= nCount; zz++)
	{
		Vector3d vScenePos;
		float fAngle = zz * fDeltaAngle;
		double fX = vCenterPos.x + fRadius * sin(DEG_TO_RAD(fAngle));
		double fY = vCenterPos.y + fRadius * cos(DEG_TO_RAD(fAngle));
		vScenePos.x = fX;
		vScenePos.y = fY;
		vScenePos.z = vCenterPos.z;
		m_vecLinePoint.push_back(vScenePos);
	}
}

void CircleStatisticsTool::calcLinePointGis(const Vector3d& vScreenCenterPos, const float& fRadius)
{
	m_vecLinePoint.clear();
	m_vecLinePoint.resize(0);

	INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
	IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));

	//分割
	int nCount = 200;
	float fDeltaAngle = 360.0f / nCount;
	for (int zz = 0; zz <= nCount; zz++)
	{
		Vector3d vScenePos;
		float fAngle = zz * fDeltaAngle;
		double fX = vScreenCenterPos.x + fRadius * sin(DEG_TO_RAD(fAngle));
		double fY = vScreenCenterPos.y + fRadius * cos(DEG_TO_RAD(fAngle));
		pManipulator->iPickedPoint(fX, fY, vScenePos);
		m_vecLinePoint.push_back(vScenePos);
	}
}

EventReturnType_e CircleStatisticsTool::iProcessEvent(const BCEvent& tEvent)
{
	if (!m_bEnable)
	{
		return EventReturnType_e::NONE;
	}

	if (m_bDoingStatistics)
	{
		return EventReturnType_e::NONE;
	}

	//POI不需要以下事件
	if (m_bIsPOIStatistics) 
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
			if (m_eState == STATE_NONE)
			{			
				m_vCenterPos = pManipulator->iGetMousePoint();
				if (m_nStatisticsType == StatisticsType::Circle_QX)
				{
					m_eState = STATE_MOVE;
				}
				else if (m_nStatisticsType == StatisticsType::Circle_FY)
				{
					SetData(m_vCenterPos, m_fRadius);
				}
			}
			else if(m_eState != STATE_END)
			{
				Vector3d vTargetPos = pManipulator->iGetMousePoint();
				Vector2 vStart(m_vCenterPos.x, m_vCenterPos.y);
				Vector2 vTarget(vTargetPos.x, vTargetPos.y);
				m_fRadius = vStart.Distance(vTarget);

				calcLinePoint(m_vCenterPos, m_fRadius);

				//生成polygon
				if (m_pPolygon)
				{
					m_pPolygon->iSetVisible(false);
					APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pPolygon, false);
				}
				if (m_vecLinePoint.size() < 3)
				{
					return EventReturnType_e::NONE;
				}
				m_tPolygonParam.vecVertex = ConvertVecVector3dTo(m_vecLinePoint);
				m_tPolygonParam.vecVertex.pop_back();	//去掉最后一个值，不然有重复
				m_pPolygon = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(m_tPolygonParam);
				m_pPolygon->iLoadModel();
				m_pPolygon->iSetVisible(m_bEnable);
				m_pPolygon->iSetSaveMyself(false);
				APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(m_pPolygon);
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
			if (m_eState == STATE_MOVE)
			{
				Vector3d vTargetPos = pManipulator->iGetMousePoint();
				Vector2 vStart(m_vCenterPos.x, m_vCenterPos.y);
				Vector2 vTarget(vTargetPos.x, vTargetPos.y);
				m_fRadius = vStart.Distance(vTarget);

				calcLinePoint(m_vCenterPos, m_fRadius);

				//更新线
				ILocusLine* pLocus = GetComponent<ILocusLine>(m_pLocusLine);
				if (pLocus)
				{
					m_pLocusLine->iSetVisible(true);
					std::vector<LocusLineSegment> vecSegment;
					LocusLineSegment sSeg;
					sSeg.vColor = m_tPolygonParam.vLineColor;
					sSeg.fWidth = m_tPolygonParam.fLineWidth;
					sSeg.vecVertex = ConvertVecVector3dTo(m_vecLinePoint);
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

bool CircleStatisticsTool::iDoingStatistics(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam)
{
	CircleStatisticsTool* pThis = (CircleStatisticsTool*)mapParams["this"];
	for (size_t i = 0; i < pThis->m_vecStatisTask.size(); i++)
	{
		StatisTaskData* pData = &pThis->m_vecStatisTask[i];
		Vector2 vTarget(pData->vPos.x,pData->vPos.y);
		Vector2 vCenter(pThis->m_vCenterPos.x, pThis->m_vCenterPos.y); 
		Vector2 vOffset = vTarget - vCenter;
		if (vOffset.Length() < pThis->m_fRadius)
		{
			addKey(pData);
		}
	}
	return true;
}

void CircleStatisticsTool::Cancel()
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

	m_vecStatisTask.clear();
	m_vecStatisTask.resize(0);
	m_bCanStatistics = false;
	m_eState = STATE_NONE;
}

void CircleStatisticsTool::SetRadius(float fRadius) 
{
	m_fRadius = fRadius;
}

void CircleStatisticsTool::SetType(int nStatisticsType)
{
	m_nStatisticsType = nStatisticsType;
}

