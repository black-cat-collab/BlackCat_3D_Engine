#include "ProjectBasedHeader.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/label/func/IFunc.h"
#include "Project-Based/label/func/ReadHeightTifFunc.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "../HeightTifReader.h"

using namespace bc;


ReadHeightTifFunc::ReadHeightTifFunc(CommonLabelFactory* pComFactory):
	IFunc(),
	m_pComFactory(pComFactory),
	m_pNearAssemble(nullptr),
	m_fNearDis(0)
{

}

ReadHeightTifFunc::~ReadHeightTifFunc()
{

}

EventReturnType_e ReadHeightTifFunc::iProcessEvent(const BCEvent& tEvent)
{
	return EventReturnType_e::NONE;
}

void ReadHeightTifFunc::iCreateData(FactoryAssemble* pAssemble)
{

}

void ReadHeightTifFunc::iUpdateData(FactoryAssemble* pAssemble)
{
	if (!pAssemble->bSmoothMove)
	{
		if (m_pComFactory->GetDefaultNoTifZ()  != 0.0f && pAssemble->pData->vLabelPos.z == 0)
		{
			pAssemble->pData->vLabelPos.z = m_pComFactory->GetDefaultNoTifZ();
		}
		HeightTifReader::GetInstance()->AddTask(pAssemble); 
	}
}

void ReadHeightTifFunc::iPreFrameUpdate()
{
	
}

void ReadHeightTifFunc::iFrameUpdate(FactoryAssemble* pAssemble)
{
	if (!pAssemble->bSmoothMove)
	{
		bool bShow = m_pComFactory->CheckAssembleVisible(pAssemble);
		if (!bShow)
		{
			return;
		}
		if (pAssemble->pData->eReadTifStatus != READ_TIF_NEED_NOT_YET_ADD)
		{
			return;
		}
		INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
		float fDis = pCameraNode->iGetOrigin().Distance(pAssemble->pData->vLabelPos);
		if (m_fNearDis == 0.0f || fDis < m_fNearDis)
		{
			m_fNearDis = fDis;
			m_pNearAssemble = pAssemble;
		}
	}
}

void ReadHeightTifFunc::iPostFrameUpdate()
{
	if (m_pNearAssemble)
	{
		HeightTifReader::GetInstance()->AddFirstTask(m_pNearAssemble);
	}
	m_fNearDis = 0.0f;
	if (m_pNearAssemble && m_pNearAssemble->pData && m_pNearAssemble->pData->eReadTifStatus != READ_TIF_NEED_NOT_YET_ADD)
	{
		m_pNearAssemble = nullptr;
	}
}

CommonLabelFactory* ReadHeightTifFunc::GetComFactory()
{
	return m_pComFactory;
}

