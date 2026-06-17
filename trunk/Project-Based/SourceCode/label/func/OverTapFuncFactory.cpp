#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/label/func/IFunc.h"
#include "Project-Based/label/func/OverTapFunc.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "OverTapFuncFactory.h"

using namespace bc;

OverTapFuncFactory::OverTapFuncFactory(OverTapFunc* pOverTapFunc) :
	CommonLabelFactory(),
	m_pOverTapFunc(pOverTapFunc)
{
}

OverTapFuncFactory::~OverTapFuncFactory()
{

}

INode* OverTapFuncFactory::iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo)
{
	return m_pOverTapFunc->GetComFactory()->iCreateOverTapLabelNode(nType);
}

void OverTapFuncFactory::iUpdateDataForLabel(FactoryAssemble* pAssemble)
{
	OverTapFunc::OverTapData* pInfo = (OverTapFunc::OverTapData*)pAssemble->pData;
	pAssemble->pLabelNode->iSetOrigin(pInfo->vLabelPos);
	m_pOverTapFunc->GetComFactory()->iUpdateOverTapLabelNode(pAssemble);
	RefreshExpandData(pInfo);
}

void OverTapFuncFactory::iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{
	CommonLabelFactory::iOnSetLabelSelect(pAssemble, bSelect);
	m_pOverTapFunc->GetComFactory()->iSetOverTapLabelSelect(pAssemble,bSelect);

	if (pAssemble->pLabelNode)
	{
		OverTapFunc::OverTapData* pInfo = (OverTapFunc::OverTapData*)pAssemble->pData;
		if (pInfo->bExpand)
		{
			bSelect = false;
		}
		pInfo->bExpand = bSelect;
		RefreshExpandData(pInfo);
	}
}

void OverTapFuncFactory::iFrameUpdateForLabel(FactoryAssemble* pAssemble)
{
	CommonLabelFactory::iFrameUpdateForLabel(pAssemble);

	//计算被重叠的原始位置
	OverTapFunc::OverTapData* pInfo = (OverTapFunc::OverTapData*)pAssemble->pData;
	if (!pAssemble->pLabelNode)
	{
		return;
	}
	if (pInfo->bExpand)
	{
		//展开的情况
		IBoard* pSelfBoard = pAssemble->pLabelNode->GetDynamicComponent<IBoard>();
		if (!pSelfBoard)
		{
			return;
		}
		INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
		float fSelfWidth = pAssemble->pLabelNode->iGetBBox().Width();
		float fSelfHeight = pAssemble->pLabelNode->iGetBBox().Height();
		float fSelfBoxLength = sqrt(fSelfWidth * fSelfWidth + fSelfHeight * fSelfHeight);
		Vector3d vSelfPos = pAssemble->pLabelNode->iGetBBox().GetCenter();
		float fWidth = 1.0f;
		float fHeight = 1.0f;
		if (pInfo->vecData.size() > 0)
		{
			INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
			if (!m_pOverTapFunc->GetComFactory()->IsInstanceFactory())
			{
				if (pInfo->vecData[0] && pInfo->vecData[0]->pLabelNode)
				{
					INode* pNode = pInfo->vecData[0]->pLabelNode;
					fWidth = pNode->iGetBBox().Width();
					fHeight = pNode->iGetBBox().Height();
				}
			}
			else
			{
				BoardInstanceData* pBoardInstanceData = (BoardInstanceData*)pInfo->vecData[0]->pLabelInstance;
				fWidth = 1.2f * pInfo->vecData[0]->pLabelInstance->tBBox.Length();
				fHeight = 1.2f * pInfo->vecData[0]->pLabelInstance->tBBox.Height();
			}
		}
		float fBoxLength = sqrt(fWidth * fWidth + fHeight * fHeight);
		float fRadius = fBoxLength / 2 + fSelfBoxLength / 2;
		int nCount = 0;
		int nProvideCount = 0;
		for (size_t i = 0; i < pInfo->vecData.size(); i++)
		{
			INode* pNode = nullptr;
			BoardInstanceData* pBoardInstanceData = nullptr;
			if (!pInfo->vecData[i]->bInstance)
			{
				pNode = pInfo->vecData[i]->pLabelNode;
			}
			else
			{
				pBoardInstanceData = (BoardInstanceData*)pInfo->vecData[i]->pLabelInstance;
			}
			if (nCount != 0 && (i - nProvideCount) % nCount == 0)
			{
				nProvideCount += nCount;
				fRadius += fBoxLength * 1.2f;
			}
			//余弦定理求角度
			float fResult = (fRadius * fRadius + fRadius * fRadius - fBoxLength * fBoxLength) / (2 * fRadius * fRadius);
			fResult = acos(fResult);
			float fMinAngle = RAD_TO_DEG(fResult);
			nCount = (int)(180.0f / fMinAngle) + 1;
			int fOffsetAngle = (180.0f - (nCount - 1) * fMinAngle) / 2;
			//计算自己的位置
			float x = 0.0f;
			float y = 0.0f;
			int nRow = i / nCount;
			int nColumn = i % nCount;
			float fStartAngle = fOffsetAngle + nColumn * fMinAngle;
			float fCurRadius = fRadius; //+nRow * fBoxLength;
			x = fCurRadius * cos(DEG_TO_RAD(fStartAngle));
			y = fCurRadius * sin(DEG_TO_RAD(fStartAngle));
			Vector3d vPos = calcAbsolutePos(pAssemble->pLabelNode, Vector2(x, y));
			if (pBoardInstanceData)
			{
				pBoardInstanceData->vTargetPosition = vPos;
				pBoardInstanceData->vOrigin = vPos;
			}
			if (pNode)
			{
				pNode->iSetOrigin(vPos);
			}
		}
	}
}

Vector3d OverTapFuncFactory::calcAbsolutePos(INode* pNode, Vector2 vOffset)
{
	Vector3d vDir = pNode->iGetAxis()[1];
	vDir.Normalise();

	Vector3d vOrigin = pNode->iGetBBox().GetCenter();
	vOrigin = vOrigin.MoveForward(vDir, vOffset.x);
	float fOffsetY = vOffset.y;
	vDir = pNode->iGetAxis()[2];
	vDir.Normalise();
	vOrigin = vOrigin.MoveForward(vDir, fOffsetY);
	return vOrigin;
}

void OverTapFuncFactory::RefreshExpandData(OverTapFunc::OverTapData* pInfo)
{
	bool bShow = pInfo->bExpand;
	for (size_t i = 0; i < pInfo->vecData.size(); ++i)
	{
		pInfo->vecData[i]->eOverTapState = bShow?OVERTAP_SHOW: OVERTAP_HIDE;
		std::vector<FactoryAssemble*> vec;
		vec.push_back(pInfo->vecData[i]);
		m_pOverTapFunc->GetComFactory()->RefreshVisible(vec);
	}
}

void OverTapFuncFactory::RefreshExpandData()
{
	for (size_t i = 0; i < m_vecCurAssemble.size(); i++)
	{
		OverTapFunc::OverTapData* pInfo = (OverTapFunc::OverTapData*)m_vecCurAssemble[i]->pData;
		RefreshExpandData(pInfo);
	}
}

void OverTapFuncFactory::iHideAssembleForLabel(FactoryAssemble* pAssemble)
{

}

void OverTapFuncFactory::iShowAssembleForLabel(FactoryAssemble* pAssemble)
{

}

