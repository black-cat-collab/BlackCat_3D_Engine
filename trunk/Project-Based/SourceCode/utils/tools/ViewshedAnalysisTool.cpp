#include "ProjectBasedHeader.h"
#include "Project-Based/utils/tools/ViewshedAnalysisTool.h"
#include "Project-Based/base/ProjectBasedClient.h"

using namespace bc;

ViewshedAnalysisTool::ViewshedAnalysisTool() :
	m_eViewshedType(VIEWSHED_TYPE_SINGLE),
	m_pViewshedAnalysisView(nullptr),
	m_bGrapScenePoint(false),
	m_pView(nullptr),
	m_fOffsetZ(0.0f),
	m_fVFov(-1.0f)
{
}

ViewshedAnalysisTool::~ViewshedAnalysisTool()
{
}

void ViewshedAnalysisTool::Create()
{
	m_pView = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentEffectView(VIEW_TYPE_VIEWSHED_ANALYSIS);
	m_pViewshedAnalysisView = dynamic_cast<IViewshedAnalysis*>(m_pView);
}

void ViewshedAnalysisTool::SetAngle(Vector3d vAngle)
{
	if (m_pViewshedAnalysisView)
	{
		m_pViewshedAnalysisView->iSetAngles(vAngle);
	}
}

void ViewshedAnalysisTool::SetOrigin(Vector3d vOrigin)
{
	if (m_pViewshedAnalysisView)
	{
		ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
		pClient->AdjustZ(vOrigin, m_fOffsetZ,true);
		m_pViewshedAnalysisView->iSetOrigin(vOrigin);
	}
}

void ViewshedAnalysisTool::SetHFov(float fFov)
{
	if (m_pViewshedAnalysisView)
	{
		m_pViewshedAnalysisView->iSetFOV(fFov);
	}
}

void ViewshedAnalysisTool::SetVFov(float fFov)
{
	if (m_pViewshedAnalysisView)
	{
		m_pViewshedAnalysisView->iSetVFOV(fFov);
		m_fVFov = fFov;
	}
}

void ViewshedAnalysisTool::SetFarClip(float fFarClip)
{
	if (m_pViewshedAnalysisView)
	{
		m_pViewshedAnalysisView->iSetFarClip(fFarClip);
	}
}

void ViewshedAnalysisTool::SetVisibleColor(Vector3d vVisibleColor)
{
	if (m_pViewshedAnalysisView)
	{
		m_pViewshedAnalysisView->iSetVisibleColor(vVisibleColor);
	}
}

void ViewshedAnalysisTool::SetInvisibleColor(Vector3d vInvisibleColor)
{
	if (m_pViewshedAnalysisView)
	{
		m_pViewshedAnalysisView->iSetInvisibleColor(vInvisibleColor);
	}
}

void ViewshedAnalysisTool::SetLineColor(Vector4 vLineColor)
{
	if (m_pViewshedAnalysisView)
	{
		m_pViewshedAnalysisView->iSetLineColor(vLineColor);
	}
}

void ViewshedAnalysisTool::Start(bool bGrapScenePoint)
{
	m_pView = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentEffectView(VIEW_TYPE_VIEWSHED_ANALYSIS);
	m_pViewshedAnalysisView = dynamic_cast<IViewshedAnalysis*>(m_pView);

	m_bGrapScenePoint = bGrapScenePoint;
	if (!m_bGrapScenePoint && m_pView)
	{
		m_pView->iSetVisible(true);
	}
}

void ViewshedAnalysisTool::Stop()
{
	if (m_pView)
	{
		m_pView->iSetVisible(false);
	}
	m_bGrapScenePoint = false;
}

EventReturnType_e ViewshedAnalysisTool::iProcessEvent(const BCEvent& tEvent)
{
	if (!m_bGrapScenePoint)
	{
		return EventReturnType_e::NONE;
	}
	INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
	ICamera* pMainCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
	IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));
	if (tEvent.eMessage == EVENT_MOUSE)
	{
		if (!APIProvider::GetSystemAPI()->iProjectClientAPI->iCanHandleScene(Vector2(tEvent.nParam2, tEvent.nParam3)))
		{
			return EventReturnType_e::NONE;
		}

		if (tEvent.nParam1 == MOUSE_LBUTTONUP)
		{
			ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
			IntersectSet tIntersectSet;
			pMainCamera->iIntersectObject(tEvent.nParam2, tEvent.nParam3, tIntersectSet);
			Vector3d vCurrentPoint = tIntersectSet.vCrossPoint;

			if (m_pViewshedAnalysisView)
			{
				pClient->AdjustZ(vCurrentPoint, m_fOffsetZ,true);
				m_pViewshedAnalysisView->iSetOrigin(vCurrentPoint);
				m_pView->iSetVisible(true);
			}
			m_bGrapScenePoint = false;
		}
	}

	return EventReturnType_e::NONE;
}

void ViewshedAnalysisTool::SetViewshedType(EViewshedType eType)
{
	m_eViewshedType = eType;
	if (m_pViewshedAnalysisView)
	{
		m_pViewshedAnalysisView->iSetMode(eType == VIEWSHED_ANALYSIS_SINGLE ? ViewshedAnalysisMode::VIEWSHED_ANALYSIS_SINGLE
			: ViewshedAnalysisMode::VIEWSHED_ANALYSIS_MULTIPLE);
	}
}

void ViewshedAnalysisTool::SetOffsetZ(float fOffsetZ)
{
	if (m_pViewshedAnalysisView)
	{
		Vector3d vOrigin = m_pViewshedAnalysisView->iGetOrigin();
		ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
		pClient->AdjustZ(vOrigin, -m_fOffsetZ,true);
		pClient->AdjustZ(vOrigin, fOffsetZ,true);
		m_pViewshedAnalysisView->iSetOrigin(vOrigin);
		m_fOffsetZ = fOffsetZ;
	}
}

ViewshedState::ViewshedState()
{
	vOrigin = Vector3d(0.0, 0.0, 0.0);
	vAngles = Vector3d(0.0, 0.0, 0.0);
	fHorizontal = 0.0;
	fVertical = 0.0;
	fClip = 0.0;
}

ViewshedState::ViewshedState(Vector3d vOriginArg, Vector3d vAnglesArg, float fHorizontalArg, float fVerticalArg, float fClipArg)
{
	vOrigin = vOriginArg;
	vAngles = vAnglesArg;
	fHorizontal = fHorizontalArg;
	fVertical = fVerticalArg;
	fClip = fClipArg;
}

bool ViewshedState::IsChange(const ViewshedState& vCompare)
{
	return ((vOrigin.Distance(vCompare.vOrigin) >= 2) || (vAngles.Distance(vCompare.vAngles) >= 2) ||
		abs(fHorizontal - vCompare.fHorizontal) >= 1 || abs(fVertical - vCompare.fVertical) >= 1 ||
		abs(fClip - vCompare.fClip) >= 1);
}

ViewshedState& ViewshedState::operator=(const ViewshedState& vCopy)
{
	this->vOrigin = vCopy.vOrigin;
	this->vAngles = vCopy.vAngles;
	this->fHorizontal = vCopy.fHorizontal;
	this->fVertical = vCopy.fVertical;
	this->fClip = vCopy.fClip;
	return *this;
}

