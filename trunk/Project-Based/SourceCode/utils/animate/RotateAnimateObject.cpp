
#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/animate/EasingAnimate.h"
#include "Project-Based/utils/animate/BaseAnimateObject.h"
#include "Project-Based/utils/animate/RotateAnimateObject.h"

using namespace bc;

RotateAnimateObject::RotateAnimateObject(INode* pTarget, RotateAnimateParam stParam) :BaseAnimateObject(pTarget)
{
	m_stAnimateParam = stParam;
}

RotateAnimateObject::~RotateAnimateObject()
{
	if (m_pTargetNode)
	{
		m_pTargetNode->iSetAngles(m_vStandardOrigin);
	}
}

void RotateAnimateObject::Start()
{
	BaseAnimateObject::Start();
	if (m_pTargetNode)
	{
		m_vStandardOrigin = m_pTargetNode->iGetAngles();
	}
}

void RotateAnimateObject::iEasingAnimateUpdate(EasingAnimateParam* pEasingParam, std::map<std::string, INT_PTR> m_mapINTPTR,
	int nCurValueIndex, double dCurRate, int nCurLoopCount, int nAllLoopCount, bool bComplete, bool& bManualStop)
{
	RotateAnimateObject* pThis = (RotateAnimateObject*)m_mapINTPTR["this"];
	Vector3ProValue& stValue = pThis->m_stAnimateParam.vecValue[nCurValueIndex];
	Vector3d vStartPos = stValue.vStart;
	Vector3d vOffsetPosition = stValue.vEnd - stValue.vStart;
	if (m_vStandardOrigin == vStartPos)
	{
		vStartPos = Vector3d(0, 0, 0);
	}
	Vector3d vOffset;
	vOffset[0] = vOffsetPosition[0] * dCurRate + vStartPos[0];
	vOffset[1] = vOffsetPosition[1] * dCurRate + vStartPos[1];
	vOffset[2] = vOffsetPosition[2] * dCurRate + vStartPos[2];

	if (pThis->m_pTargetNode)
	{
		Vector3d vOrigin = pThis->m_pTargetNode->iGetAngles();
		if (m_vPreOrigin.Distance(vOrigin) > 2.0f)
		{
			//调整基准位置为当前设置的
			m_vStandardOrigin = vOrigin;
		}
		Vector3d vCurPos = m_vStandardOrigin + vOffset;
		pThis->m_pTargetNode->iSetAngles(vCurPos);
		m_vPreOrigin = vCurPos;
	}

	if (m_stAnimateParam.pUpdateFunc)
	{
		m_stAnimateParam.pUpdateFunc(pEasingParam, m_mapINTPTR, nCurValueIndex, dCurRate, nCurLoopCount, nAllLoopCount, bComplete, bManualStop);
	}
}

EasingAnimateParam RotateAnimateObject::iGetEasingAnimateParam()
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

