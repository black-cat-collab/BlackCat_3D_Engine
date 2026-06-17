
#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/animate/EasingAnimate.h"
#include "Project-Based/utils/animate/BaseAnimateObject.h"
#include "Project-Based/utils/animate/TranslateAnimateObject.h"

using namespace bc;

TranslateAnimateObject::TranslateAnimateObject(INode* pTarget, TranslateAnimateParam stParam):BaseAnimateObject(pTarget)
{
	m_stAnimateParam = stParam;
}

TranslateAnimateObject::~TranslateAnimateObject()
{
	if (m_pTargetNode)
	{
		IBoard* pBoard = GetComponent<IBoard>(m_pTargetNode);
		if (m_pTargetNode->iGetNodeType() == NODE_BOARD_INSTANCE)
		{
			INode* pInstance = m_pTargetNode;
			BoardInstanceParam* pParam = (BoardInstanceParam*)pInstance->iGetNodeParam();
			if (pParam->bRotateAroundTarget)
			{
				pParam->vDistanceToTarget = m_vDistanceToTargetOrigin; 
			}
		}
		else if (pBoard)
		{
			if (pBoard->iGetBoardParam()->bRotateAroundTarget)
			{
				pBoard->iGetBoardParam()->vDistanceToTarget = m_vDistanceToTargetOrigin;
			}
			else
			{
				m_pTargetNode->iSetOrigin(m_vStandardOrigin);
			}
		}
	}
}

void TranslateAnimateObject::Start()
{
	BaseAnimateObject::Start();
	if (m_pTargetNode)
	{
		m_vStandardOrigin = m_pTargetNode->iGetOrigin();
	}
}

void TranslateAnimateObject::iEasingAnimateUpdate(EasingAnimateParam* pEasingParam, std::map<std::string, INT_PTR> m_mapINTPTR,
	int nCurValueIndex, double dCurRate, int nCurLoopCount, int nAllLoopCount, bool bComplete, bool& bManualStop)
{
	TranslateAnimateObject* pThis = (TranslateAnimateObject*)m_mapINTPTR["this"];
	Vector3ProValue &stValue = pThis->m_stAnimateParam.vecValue[nCurValueIndex];
	Vector3d vStartPos = stValue.vStart;
	Vector3d vOffsetPosition = stValue.vEnd - stValue.vStart;
	if (m_vStandardOrigin == vStartPos)
	{
		vStartPos = Vector3d(0,0,0);
	}
	Vector3d vOffset;
	vOffset[0] = vOffsetPosition[0]* dCurRate + vStartPos[0];
	vOffset[1] = vOffsetPosition[1] * dCurRate + vStartPos[1];
	vOffset[2] = vOffsetPosition[2] * dCurRate + vStartPos[2];

	if (pThis->m_pTargetNode)
	{
		IBoard* pBoard = GetComponent<IBoard>(pThis->m_pTargetNode);
		if (pThis->m_pTargetNode->iGetNodeType() == NODE_BOARD_INSTANCE)
		{
			INode* pInstance = pThis->m_pTargetNode;
			BoardInstanceParam *pParam = (BoardInstanceParam*)pInstance->iGetNodeParam();
			if (m_vDistanceToTargetOrigin.Empty())
			{
				m_vDistanceToTargetOrigin = pParam->vDistanceToTarget;
			}
			if (pParam->bRotateAroundTarget)
			{
				pParam->vDistanceToTarget.x = m_vDistanceToTargetOrigin.x + vOffset[1];
				pParam->vDistanceToTarget.y = m_vDistanceToTargetOrigin.y + vOffset[2];
			}
		}
		else if (pBoard && pBoard->iGetBoardParam()->bRotateAroundTarget)
		{
			if (m_vDistanceToTargetOrigin.Empty())
			{
				m_vDistanceToTargetOrigin = pBoard->iGetBoardParam()->vDistanceToTarget;
			}
			pBoard->iGetBoardParam()->vDistanceToTarget.x = m_vDistanceToTargetOrigin.x + vOffset[1];
			pBoard->iGetBoardParam()->vDistanceToTarget.y = m_vDistanceToTargetOrigin.y + vOffset[2];
		}
		else
		{
			Vector3d vOrigin = pThis->m_pTargetNode->iGetOrigin();
			if (m_vPreOrigin != vOrigin)
			{
				//调整基准位置为当前设置的
				m_vStandardOrigin = vOrigin;
			}

			Vector3d vCurPos = m_vStandardOrigin + vOffset;
			pThis->m_pTargetNode->iSetOrigin(vCurPos);

			m_vPreOrigin = pThis->m_pTargetNode->iGetOrigin();
		}
	}
	if (m_stAnimateParam.pUpdateFunc)
	{
		m_stAnimateParam.pUpdateFunc(pEasingParam, m_mapINTPTR, nCurValueIndex, dCurRate, nCurLoopCount, nAllLoopCount, bComplete, bManualStop);
	}
}

EasingAnimateParam TranslateAnimateObject::iGetEasingAnimateParam()
{
	EasingAnimateParam stParam;

	for (size_t i = 0; i < m_stAnimateParam.vecValue.size(); i++)
	{
		FloatProValue stProValue;
		stProValue.CopyProValue(m_stAnimateParam.vecValue[i]);
		stProValue.dStartValue = 0;
		stProValue.dEndValue = m_stAnimateParam.vecValue[i].vStart.Distance(m_stAnimateParam.vecValue[i].vEnd);

		stParam.vecValue.push_back(stProValue);
	}

	return stParam;
}

