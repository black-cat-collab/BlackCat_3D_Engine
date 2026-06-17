#include "ProjectBasedHeader.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/label/LabelDetailFactory.h"
#include "InstanceSelector.h"

using namespace bc;

SelectBoardFactory::SelectBoardFactory():CommonLabelFactory()
{

}

SelectBoardFactory::~SelectBoardFactory()
{

}

INode* SelectBoardFactory::iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo)
{
	InstanceSelector* pSelector = (InstanceSelector*)GetExtraData("InstanceSelector");
	BoardParam* pInstanceParam = pSelector->GetComFactory()->GetLabelBoardParam(nType);

	BoardParam stParam;
	stParam = *pInstanceParam;
	Guid::CreateGuId(stParam.strID, "instance_select_");
	stParam.nNodeType = NODE_BOARD;
	stParam.direction = FaceToCamera;
	stParam.strHoverImage = "";
	stParam.strCheckedImage = "";
	stParam.bHoverEnable = false;
	stParam.bCheckEnable = false;
	stParam.bClickEnable = true;
	stParam.fWidth = pInstanceParam->fWidth;
	stParam.fHeight = pInstanceParam->fHeight;
	stParam.fLength = pInstanceParam->fLength;
	stParam.bDynamicScaleEnable = pInstanceParam->bDynamicScaleEnable;
	stParam.strImage = pInstanceParam->strCheckedImage;
	if (stParam.strImage.empty())
	{
		stParam.strImage = pInstanceParam->strHoverImage;
	}
	stParam.fDynamicMaxScale = 2.0 * pInstanceParam->fDynamicMaxScale;
	stParam.fDynamicMinScale = 2.0 * pInstanceParam->fDynamicMinScale;
	stParam.fDynamicScale = 2.0 * pInstanceParam->fDynamicScale;
	stParam.fScale = 2.0 * pInstanceParam->fScale;
	stParam.vTopLeftOffset = pInstanceParam->vDistanceToTarget;
	stParam.eTransparentType = TRANSPARENT_BLEND;

	INode* pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(stParam);
	pNode->iLoadModel();
	pNode->iSetVisible(false);
	APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode()->iAddChild(pNode);

	return pNode;
}

void SelectBoardFactory::iUpdateDataForLabel(FactoryAssemble* pAssemble)
{
	SelectInfo* pInfo = (SelectInfo*)pAssemble->pData;
	FactoryAssemble* pOriginLabel = pInfo->pAssemble;
	InstanceSelector* pSelector = (InstanceSelector*)GetExtraData("InstanceSelector");
	BoardParam* pInstanceParam = pSelector->GetComFactory()->GetLabelBoardParam(pAssemble->nType);
	bool bWithLine = pInstanceParam->bWithLine;
	bool bRotateAroundTarget = pInstanceParam->bRotateAroundTarget;
	if (pOriginLabel->eOverTapState == OVERTAP_SHOW)
	{
		bWithLine = false;
		bRotateAroundTarget = false;
	}
	BoardParam* pSelfParam = pAssemble->pLabelNode->GetDynamicComponent<IBoard>()->iGetBoardParam();
	pSelfParam->bWithLine = bWithLine;
	pSelfParam->bRotateAroundTarget = bRotateAroundTarget;

	if (pSelfParam->bRotateAroundTarget)
	{
		pAssemble->pLabelNode->iSetOrigin(pOriginLabel->pData->vLabelPos); 
	}
	else
	{
		pAssemble->pLabelNode->iSetOrigin(Vector3d(pOriginLabel->pLabelInstance->vOrigin));
	}
}

void  SelectBoardFactory::iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{
	SelectInfo* pInfo = (SelectInfo*)pAssemble->pData;
	InstanceSelector* pSelector = (InstanceSelector*)GetExtraData("InstanceSelector");
	pSelector->SetSelect(pInfo->pAssemble,false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

InstanceSelector::InstanceSelector(CommonLabelFactory* pComFactory):
	m_pComFactory(pComFactory)
{
	m_pSelectBoardFactory = new SelectBoardFactory;
	m_pSelectBoardFactory->SetExtraData("InstanceSelector", (INT_PTR)this);
}

InstanceSelector::~InstanceSelector()
{
	m_pSelectBoardFactory->RefreshZero();
	DELETE_PTR(m_pSelectBoardFactory);

	for (size_t i = 0; i < m_vecSelectData.size(); ++i)
	{
		DELETE_PTR(m_vecSelectData[i]);
	}
	m_vecSelectData.clear();
}

void InstanceSelector::SetSelect(FactoryAssemble* pAssemble, bool bSelect)
{
	if (bSelect)
	{
		bool bFind = false;
		for (std::vector<SelectInfo*>::iterator it = m_vecSelectData.begin(); it != m_vecSelectData.end(); ++it)
		{
			if ((*it)->pAssemble == pAssemble)
			{
				bFind = true;
				break;
			}
		}
		if (!bFind)
		{
			SelectInfo* pInfo = new SelectInfo;
			pInfo->pAssemble = pAssemble;
			pInfo->nTypeID = pAssemble->nType;
			pInfo->vLabelPos = pAssemble->pData->vLabelPos;
			m_vecSelectData.push_back(pInfo);

			std::vector<ProjectBasedSceneElement*> vecTemp;
			vecTemp.push_back(pInfo);
			m_pSelectBoardFactory->AddData(vecTemp);
		}
	}
	else
	{
		for (std::vector<SelectInfo*>::iterator it = m_vecSelectData.begin(); it != m_vecSelectData.end(); ++it)
		{
			if ((*it)->pAssemble == pAssemble)
			{
				//移除
				SelectInfo* pInfo = (*it);
				m_vecSelectData.erase(it);
				FactoryAssemble* pSelectAssemble = m_pSelectBoardFactory->FindAssembleByData(pInfo);
				//关闭详情
				GetComFactory()->GetDetailFactory()->CloseByData(pInfo->pAssemble->pData);
				m_pSelectBoardFactory->RemoveByAssemble(pSelectAssemble);

				DELETE_PTR(pInfo);
				break;
			}
		}
	}
	pAssemble->bInstanceSelected = bSelect;
	bool bVisible = GetComFactory()->CheckAssembleVisible(pAssemble);
	pAssemble->pLabelInstance->bVisible = bVisible;
}

void InstanceSelector::ClearAllSelect()
{
	m_pSelectBoardFactory->RefreshZero();
	for (size_t i = 0; i < m_vecSelectData.size(); ++i)
	{
		DELETE_PTR(m_vecSelectData[i]);
	}
}

CommonLabelFactory* InstanceSelector::GetComFactory()
{
	return m_pComFactory;
}

EventReturnType_e InstanceSelector::iProcessEvent(const BCEvent& tEvent)
{
	m_pSelectBoardFactory->iProcessEvent(tEvent);

	return EventReturnType_e::NONE;
}

void InstanceSelector::SetTypeVisible(std::vector<int>& vecType)
{
	m_pSelectBoardFactory->SetTypeVisible(vecType);
}

bc::INode* InstanceSelector::FindSelectBoardNode(ProjectBasedSceneElement* pData)
{
	std::vector<FactoryAssemble*> vecAssemble;
	m_pSelectBoardFactory->GetCurAssemble(vecAssemble);
	for (auto pAssemble : vecAssemble)
	{
		SelectInfo* pSelectInfo = (SelectInfo*)pAssemble->pData;
		if (pSelectInfo->pAssemble->pData == pData)
		{
			return pAssemble->pLabelNode;
		}
	}
	return nullptr;
}

