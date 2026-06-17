
#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/animate/Easing.h"
#include "Project-Based/utils/animate/EasingAnimate.h"
#include "Project-Based/utils/animate/BaseAnimateObject.h"

using namespace bc;

BaseAnimateObject::BaseAnimateObject(INode* pTargetNode)
{
	m_pTargetNode = pTargetNode;
	m_pEasingAnimate = new EasingAnimate;
	m_pEasingAnimate->AddINTPTR("this",(INT_PTR)this);
}

BaseAnimateObject::~BaseAnimateObject()
{
	DELETE_PTR(m_pEasingAnimate);
}

void BaseAnimateObject::FrameUpdate()
{
	m_pEasingAnimate->FrameUpdate();
}

void BaseAnimateObject::Start()
{
	EasingAnimateParam stParam = iGetEasingAnimateParam();
	stParam.pUpdateFunc = static_easingAnimate_update_callback;
	m_pEasingAnimate->SetEasingAnimateParam(stParam);
	m_pEasingAnimate->Start();
}

void BaseAnimateObject::Resume()
{
	m_pEasingAnimate->Start();
}

void BaseAnimateObject::Pause()
{
	m_pEasingAnimate->Pause();
}

void BaseAnimateObject::Stop()
{
	m_pEasingAnimate->Stop();
}

void BaseAnimateObject::static_easingAnimate_update_callback(EasingAnimateParam* pEasingParam, std::map<std::string, INT_PTR> mapINTPTR,
	int nCurValueIndex, double dCurRate, int nCurLoopCount, int nAllLoopCount, bool bComplete, bool& bManualStop)
{
	BaseAnimateObject* pThis = (BaseAnimateObject*)mapINTPTR["this"];
	pThis->iEasingAnimateUpdate(pEasingParam, mapINTPTR, nCurValueIndex,dCurRate,nCurLoopCount,nAllLoopCount, bComplete, bManualStop);
}

