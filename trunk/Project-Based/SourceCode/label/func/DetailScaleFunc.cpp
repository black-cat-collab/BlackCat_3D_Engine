#include "ProjectBasedHeader.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/label/func/IFunc.h"
#include "Project-Based/label/func/DetailScaleFunc.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/label/LabelDetailFactory.h"
#include "../HeightTifReader.h"

using namespace bc;


DetailScaleFunc::DetailScaleFunc(CommonLabelFactory* pComFactory):
	IFunc(),
	m_pComFactory(pComFactory)
{
	HeightTifReader::GetInstance()->AddFunc(this);
}

DetailScaleFunc::~DetailScaleFunc()
{

}

void DetailScaleFunc::iCreateData(FactoryAssemble* pAssemble)
{

}

void DetailScaleFunc::iUpdateData(FactoryAssemble* pAssemble)
{
	
}

void DetailScaleFunc::iPreFrameUpdate()
{

}

void DetailScaleFunc::iFrameUpdate(FactoryAssemble* pAssemble)
{
}

void DetailScaleFunc::iPostFrameUpdate()
{

}

CommonLabelFactory* DetailScaleFunc::GetComFactory()
{
	return m_pComFactory;
}

void DetailScaleFunc::ToScaleDetail(float fDisToCamera, FactotryAssembleDetail* pDetailAssemble)
{
	if (!pDetailAssemble)
	{
		return;
	}
	m_mapDisToCamera[pDetailAssemble->nType] = fDisToCamera;
	if (!pDetailAssemble->pSelfNode)
	{
		return;
	}
	if (find(m_vecCurDetailAssemble.begin(), m_vecCurDetailAssemble.end(), pDetailAssemble) != m_vecCurDetailAssemble.end())
	{
		return;
	}
	IBoard* pIBoard = pDetailAssemble->pSelfNode->GetDynamicComponent<IBoard>();
	if (!pIBoard)
	{
		return;
	}
	BoardParam* pParam = pIBoard->iGetBoardParam();
	pDetailAssemble->bAdjustPosWithLabel = false;
	pParam->bDynamicScaleEnable = false;
	pParam->bRotateAroundTarget = false;
	pParam->fScale = 1.0f;
	vector<INode*> vecVideo;
	vector<INode*> vecButton;
	pIBoard->iGetVideoBoard(vecVideo);
	pIBoard->iGetButtonBoard(vecButton);
	for (size_t i = 0; i < vecVideo.size(); i++)
	{
		BoardParam* pParam2 = vecVideo[i]->GetDynamicComponent<IBoard>()->iGetBoardParam();
		if (pParam2)
		{
			pParam2->bDynamicScaleEnable = pParam->bDynamicScaleEnable;
			pParam2->fScale = pParam->fScale;
		}
	}
	for (size_t i = 0; i < vecButton.size(); i++)
	{
		BoardParam* pParam3 = vecButton[i]->GetDynamicComponent<IBoard>()->iGetBoardParam();
		if (pParam3)
		{
			pParam3->bDynamicScaleEnable = pParam->bDynamicScaleEnable;
			pParam3->fScale = pParam->fScale;
		}
	}

	m_vecCurDetailAssemble.emplace_back(pDetailAssemble);
}

void DetailScaleFunc::ToNormalDetail(FactotryAssembleDetail* pDetailAssemble)
{
	pDetailAssemble->bAdjustPosWithLabel = true;
	if (!pDetailAssemble->pSelfNode)
	{
		return;
	}
	std::vector<FactotryAssembleDetail*>::iterator it = find(m_vecCurDetailAssemble.begin(), m_vecCurDetailAssemble.end(), pDetailAssemble);
	if (it != m_vecCurDetailAssemble.end())
	{
		m_vecCurDetailAssemble.erase(it);
	}

	IBoard* pIBoard = pDetailAssemble->pSelfNode->GetDynamicComponent<IBoard>();
	if (!pIBoard)
	{
		return;
	}
	BoardParam* pParam = pIBoard->iGetBoardParam();
	BoardParam* pOriginParam = GetComFactory()->GetDetailFactory()->GetDetailBoardParam(pDetailAssemble->nType);
	pParam->bDynamicScaleEnable = pOriginParam->bDynamicScaleEnable;
	pParam->bRotateAroundTarget = pOriginParam->bRotateAroundTarget;
	pParam->fScale = pOriginParam->fDynamicScale;
	vector<INode*> vecVideo;
	vector<INode*> vecButton;
	pIBoard->iGetVideoBoard(vecVideo);
	pIBoard->iGetButtonBoard(vecButton);
	for (size_t i = 0; i < vecVideo.size(); i++)
	{
		BoardParam* pParam2 = vecVideo[i]->GetDynamicComponent<IBoard>()->iGetBoardParam();
		if (pParam2)
		{
			pParam2->bDynamicScaleEnable = pParam->bDynamicScaleEnable;
		}
	}
	for (size_t i = 0; i < vecButton.size(); i++)
	{
		BoardParam* pParam3 = vecButton[i]->GetDynamicComponent<IBoard>()->iGetBoardParam();
		if (pParam3)
		{
			pParam3->bDynamicScaleEnable = pParam->bDynamicScaleEnable;
		}
	}
}

void DetailScaleFunc::iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{
	//只处理关闭事件
	if (bSelect)
	{
		return;
	}
	if (!pAssemble->pSelfNode)
	{
		return;
	}
	IBoard* pIBoard = pAssemble->pSelfNode->GetDynamicComponent<IBoard>();
	if (!pIBoard)
	{
		return;
	}
	BoardParam* pParam = pIBoard->iGetBoardParam();
	pParam->bDynamicScaleEnable = true;
	pParam->bRotateAroundTarget = true;
	ToNormalDetail(pAssemble);
}

void DetailScaleFunc::iCameraFrameUpdate(INode* pCameraNode)
{
	for (size_t i = 0; i < m_vecCurDetailAssemble.size(); ++i)
	{
		if (!m_vecCurDetailAssemble[i]->pSelfNode)
		{
			continue;
		}
		FactotryAssembleDetail* pAssemble = m_vecCurDetailAssemble[i];
		float fWidth = pAssemble->pSelfNode->GetDynamicComponent<IBoard>()->iGetBoardWidth();
		float fDisToCamera = m_mapDisToCamera[pAssemble->nType];
		Vector3d vPos = CalacuatePostion(pCameraNode, pCameraNode->iGetOrigin(), pCameraNode->iGetAngles(), fDisToCamera, fWidth);
		pAssemble->pSelfNode->iSetOrigin(vPos);
	}
}

