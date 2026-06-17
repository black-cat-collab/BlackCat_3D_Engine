#include "ProjectBasedHeader.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/label/func/IFunc.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/utils/TweenAnimateForNode.h"
#include "Project-Based/label/func/SmoothMoveFunc.h"
#include "Project-Based/base/ProjectBasedClient.h"

using namespace bc;


SmoothMoveFunc::SmoothMoveFunc(CommonLabelFactory* pComFactory):
	IFunc(),
	m_pComFactory(pComFactory)
{

}

SmoothMoveFunc::~SmoothMoveFunc()
{
	for (std::map<std::string, TweenAnimateForNode*>::iterator it = m_mapTween.begin(); it != m_mapTween.end(); ++it)
	{
		it->second->Stop();
		DELETE_PTR(it->second);
	}
	m_mapTween.clear();
}

EventReturnType_e SmoothMoveFunc::iProcessEvent(const BCEvent& tEvent)
{
	return EventReturnType_e::NONE;
}

void SmoothMoveFunc::iCreateData(FactoryAssemble* pAssemble)
{
	//一开始给它平滑移动设置true
	if (!pAssemble->bSmoothMove)
	{
		return;
	}
	TweenAnimateForNode *pTween = GetTweenByID(pAssemble->pData->strUuid,true);
	if (pTween)
	{
		pTween->Stop();
		pTween->ClearAllPoint();
		pTween->Start();
	}
}

void SmoothMoveFunc::iUpdateData(FactoryAssemble* pAssemble)
{
	if (!pAssemble->bSmoothMove)
	{
		return;
	}
	TweenAnimateForNode* pTween = GetTweenByID(pAssemble->pData->strUuid, true);
	if (pTween)
	{
		m_pComFactory->iSetSmoothMoveParam(pTween);
		PointData stPoint;
		stPoint.vPos = pAssemble->pData->vSmoothPos;
		stPoint.vAngle = pAssemble->pData->vSmoothAngle;
		pTween->SetExtraData("assemble", (INT_PTR)pAssemble);
		pTween->AddPoint(stPoint);
	}
}

void SmoothMoveFunc::iPreFrameUpdate()
{

}

void SmoothMoveFunc::iFrameUpdate(FactoryAssemble* pAssemble)
{
}

void SmoothMoveFunc::iPostFrameUpdate()
{
	for (std::map<std::string, TweenAnimateForNode*>::iterator it = m_mapTween.begin(); it != m_mapTween.end(); ++it)
	{
		it->second->Update();
	}
}

CommonLabelFactory* SmoothMoveFunc::GetComFactory()
{
	return m_pComFactory;
}

TweenAnimateForNode* SmoothMoveFunc::GetTweenByID(std::string& strID, bool bCreate)
{
	TweenAnimateForNode* pTween = nullptr;
	for (std::map<std::string, TweenAnimateForNode*>::iterator it = m_mapTween.begin(); it != m_mapTween.end(); ++it)
	{
		if (it->first == strID)
		{
			pTween = it->second;
			break;
		}
	}
	if (bCreate && !pTween)
	{
		pTween = new TweenAnimateForNode;
		pTween->SetStopOnEnd(false);
		pTween->SetSpeedUniform(true);
		pTween->SetLoop(false);
		pTween->SetSpeed(300.0f);
		pTween->SetMin2PointTimeSecond(2);		//2秒
		pTween->SetExtraData("factory", (INT_PTR)m_pComFactory);
		pTween->SetExtraJValue("id", strID);
		pTween->SetExtraJValue("labelType", m_pComFactory->GetType());
		pTween->SetCalcAngleCallback(static_tween_calcAngle);
		pTween->SetMoveCallback(static_tween_move);
		m_pComFactory->iSetSmoothMoveParam(pTween);
		pTween->Start();

		m_mapTween[strID] = pTween;
	}
	return pTween;
}

Vector3d SmoothMoveFunc::static_tween_calcAngle(TweenAnimateForNode* pTween, Vector3d vCurOrigin, Vector3d vPreOrigin)
{
	CommonLabelFactory* pFactory = (CommonLabelFactory*)pTween->GetExtraData("factory");
	int nType = pTween->GetExtraJValue("labelType").asInt();
	return pFactory->iTweenCalcAngle(nType, vCurOrigin, vPreOrigin);
}

void SmoothMoveFunc::static_tween_move(TweenAnimateForNode* pTween, INode* pTarget, Vector3d vPos, Vector3d vAngle, bool bPerEndPoint)
{
	FactoryAssemble *pAssemble = (FactoryAssemble*)pTween->GetExtraData("assemble");
	if (!pAssemble || !pAssemble->pData)
	{
		return;
	}
	CommonLabelFactory* pFactory = (CommonLabelFactory*)pTween->GetExtraData("factory");
	int nType = pTween->GetExtraJValue("labelType").asInt();
	pAssemble->pData->vLabelPos = vPos;
	pFactory->iTweenMove(nType, pAssemble, vPos, vAngle, bPerEndPoint);
	//重新更新聚合
	IFunc* pClusterObj = pFactory->GetFunc(FUNC_CLUSTER);
	if (pClusterObj)
	{
		pClusterObj->iUpdateData(pAssemble);
	}
}

void SmoothMoveFunc::RemoveTweenByID(std::string& strID)
{
	auto it = m_mapTween.find(strID);
	if (it != m_mapTween.end())
	{
		TweenAnimateForNode* pTween = it->second;
		m_mapTween.erase(it);
		DELETE_PTR(pTween);
	}
}

