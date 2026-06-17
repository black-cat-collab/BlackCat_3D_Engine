#include "ProjectBasedHeader.h"
#include "Project-Based/utils/tools/SightAnalysisTool.h"
#include "Project-Based/base/ProjectBasedClient.h"

using namespace bc;

SightAnalysisTool::SightAnalysisTool():
	m_bEnable(false),
	m_vVisibleColor(Vector4(0.0f,1.0f,0.0f,1.0f)),
	m_vInVisibleColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f)),
	m_vNoAnalyzeVisibleColor(Vector4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f)),
	m_fLineWidth(2.0f),
	m_eState(STATE_NONE),
	m_pLineNode(nullptr)
{
}

SightAnalysisTool::~SightAnalysisTool()
{
}

void SightAnalysisTool::Start(Vector3d& vStartPos, Vector3d& vEndPos)
{
	Cancel();
	createLineNode();
	m_bEnable = false;
	doAnalyze(vStartPos,vEndPos);
}

void SightAnalysisTool::Start()
{
	Cancel();
	createLineNode();
	m_bEnable = true;
}

void SightAnalysisTool::Stop()
{
	Cancel();
	m_bEnable = false;
}

void SightAnalysisTool::Cancel()
{
	m_eState = STATE_NONE;
	m_vecLinePoint.clear();
	if (m_pLineNode)
	{
		m_pLineNode->iSetVisible(false);
	}
}

//设置外部变量
void SightAnalysisTool::AddINTPTR(std::string strKey, INT_PTR pTr)
{
	m_mapINTPTR[strKey] = pTr;
}

//获取外部变量
INT_PTR SightAnalysisTool::GetINTPTR(std::string strKey)
{
	return m_mapINTPTR[strKey];
}

void SightAnalysisTool::doAnalyze(Vector3d& vStart, Vector3d& vEnd)
{
	char sz[1024] = { 0 };
	std::string strCrossScenePos, strCrossLnglat;
	std::string stStartScenePos, strStartLnglat;
	std::string stEndScenePos, strEndLnglat;
	INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
	ICamera* pCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
	IntersectSet setObj;
	pCamera->iIntersectObject(vStart, vEnd, setObj);
	std::vector<LocusLineSegment> vecSeg;
	if (setObj.pNode)
	{
		LocusLineSegment stVisibleSegment;
		stVisibleSegment.fWidth = m_fLineWidth;
		stVisibleSegment.vColor = m_vVisibleColor;
		stVisibleSegment.vecVertex.push_back(vStart);
		stVisibleSegment.vecVertex.push_back(setObj.vCrossPoint);
		vecSeg.push_back(stVisibleSegment);

		LocusLineSegment stInVisibleSegment;
		stInVisibleSegment.fWidth = m_fLineWidth;
		stInVisibleSegment.vColor = m_vInVisibleColor;
		stInVisibleSegment.vecVertex.push_back(setObj.vCrossPoint);
		stInVisibleSegment.vecVertex.push_back(vEnd);
		vecSeg.push_back(stInVisibleSegment);

		sprintf_s(sz, sizeof(sz), "%f,%f,%f", setObj.vCrossPoint.x, setObj.vCrossPoint.y, setObj.vCrossPoint.z);
		strCrossScenePos = sz;
		double dLng, dLat = 0.0f;
		ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
		pClient->ScenePosToWGS(setObj.vCrossPoint,dLng,dLat);
		sprintf_s(sz, sizeof(sz), "%f,%f,%f", dLng, dLat, setObj.vCrossPoint.z);
		strCrossLnglat = sz;
	}
	else
	{
		LocusLineSegment stVisibleSegment;
		stVisibleSegment.fWidth = m_fLineWidth;
		stVisibleSegment.vColor = m_vVisibleColor;
		stVisibleSegment.vecVertex.push_back(vStart);
		stVisibleSegment.vecVertex.push_back(vEnd);
		vecSeg.push_back(stVisibleSegment);
	}
	m_pLineNode->GetDynamicComponent<ILocusLine>()->iUpdateLocusLineSegment(vecSeg);
	m_pLineNode->iSetVisible(true);

	{
		sprintf_s(sz, sizeof(sz), "%f,%f,%f", vStart.x, vStart.y, vStart.z);
		stStartScenePos = sz;
		double dLng, dLat = 0.0f;
		ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
		pClient->ScenePosToWGS(vStart, dLng, dLat);
		sprintf_s(sz, sizeof(sz), "%f,%f,%f", dLng, dLat, vStart.z);
		strStartLnglat = sz;
	}
	{
		sprintf_s(sz, sizeof(sz), "%f,%f,%f", vEnd.x, vEnd.y, vEnd.z);
		stEndScenePos = sz;
		double dLng, dLat = 0.0f;
		ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
		pClient->ScenePosToWGS(vEnd, dLng, dLat);
		sprintf_s(sz, sizeof(sz), "%f,%f,%f", dLng, dLat, vEnd.z);
		strEndLnglat = sz;
	}

	//组装结果给网页
	float fVisibleDis = setObj.pNode ? vStart.Distance(setObj.vCrossPoint) : vStart.Distance(vEnd);
	Json::Value jResult = {};
	jResult["distance"] = vStart.Distance(vEnd);
	jResult["start_scene"] = stStartScenePos;
	jResult["start_lnglat"] = strStartLnglat;
	jResult["end_scene"] = stEndScenePos;
	jResult["end_lnglat"] = strEndLnglat;
	jResult["cross_scene"] = strCrossScenePos;
	jResult["cross_lnglat"] = strCrossLnglat;
	jResult["visible_dis"] = fVisibleDis;
	jResult["horizontal_dis"] = vStart.Distance2D(vEnd);
	jResult["vertical_dis"] = abs(vStart.z - vEnd.z);
	jResult["visible"] = setObj.pNode ? false : true;

	for (size_t i = 0; i < m_vecCompleteFunc.size(); i++)
	{
		m_vecCompleteFunc[i](m_pLineNode, this, jResult);
	}
}

void SightAnalysisTool::AddCompleteFunc(SightAnalysisCompleteFunc pFunc)
{
	std::vector<SightAnalysisCompleteFunc>::iterator it = find(m_vecCompleteFunc.begin(), m_vecCompleteFunc.end(), pFunc);
	if (it != m_vecCompleteFunc.end())
	{
		return;
	}
	m_vecCompleteFunc.push_back(pFunc);
}

void SightAnalysisTool::RemoveCompleteFunc(SightAnalysisCompleteFunc pFunc)
{
	for (std::vector<SightAnalysisCompleteFunc>::iterator it = m_vecCompleteFunc.begin(); it != m_vecCompleteFunc.end(); it++)
	{
		if ((*it) == pFunc)
		{
			m_vecCompleteFunc.erase(it);
			break;
		}
	}
}

void SightAnalysisTool::createLineNode()
{
	if (!m_pLineNode)
	{
		LocusLineParam stParam;
		m_pLineNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(stParam);
		m_pLineNode->iLoadModel();
		m_pLineNode->iSetVisible(false);
		m_pLineNode->iSetSaveMyself(false);
		APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(m_pLineNode);
	}
}

EventReturnType_e SightAnalysisTool::iProcessEvent(const BCEvent& tEvent)
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
				if (m_vLastMousePos.Distance(vCurMousePos) > 2)
				{
					return EventReturnType_e::NONE;
				}
				if (m_eState == STATE_NONE)
				{
					m_vecLinePoint.push_back(pManipulator->iGetMousePoint());
					m_eState = STATE_MOVE;
				}
				else if (m_eState == STATE_MOVE)
				{
					m_vecLinePoint.push_back(pManipulator->iGetMousePoint());
					m_eState = STATE_END;

					doAnalyze(m_vecLinePoint[0],m_vecLinePoint[1]);
				}
			}
		}
		break;
		case MOUSE_RBUTTONDOWN:
		{
			if (!pManipulator->iIsSwitchRBMB())
			{
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
				ILocusLine* pLocus = GetComponent<ILocusLine>(m_pLineNode);
				if (pLocus)
				{
					vector<LocusLineSegment> vecSegment;
					LocusLineSegment sSeg;
					sSeg.vColor = m_vNoAnalyzeVisibleColor;
					sSeg.fWidth = m_fLineWidth;
					sSeg.vecVertex = ConvertVecVector3dTo(vecPoint);
					vecSegment.push_back(sSeg);
					pLocus->iUpdateLocusLineSegment(vecSegment);

					m_pLineNode->iSetVisible(true);
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
				Cancel();
			}
		}
	}

	return EventReturnType_e::NONE;
}

