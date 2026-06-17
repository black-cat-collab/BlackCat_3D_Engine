#include "ProjectBasedHeader.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/label/InstanceLabelFactory.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/utils/ParamNetImage.h"
#include "Project-Based/label/LabelDetailFactory.h"
#include "./DefaultDetailFactory.h"
#include "Project-Based/label/func/IFunc.h"
#include "./instance/InstanceSelector.h"
#include "Project-Based/utils/AnimateWithNodeMg.h"

using namespace bc;

InstanceLabelFactory::InstanceLabelFactory():
	CommonLabelFactory(),
	m_pInstanceSelector(nullptr)
{
	m_pInstanceSelector = new InstanceSelector(this);
	m_bRefreshInstanceData.store(false);
	m_bDoingThreadFilter = false;
}

InstanceLabelFactory::~InstanceLabelFactory()
{
	for (auto& it : m_mapInstanceNode)
	{
		if (it.second)
		{
			ParamNetImage::GetInstance()->RemoveNetTask(it.second);
			AnimateWithNodeMg::GetInstance()->RemoveAnimate(it.second);
			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&it.second);
		}
	}
}

FactoryAssemble* InstanceLabelFactory::handleData(ProjectBasedSceneElement* pData)
{
	if (!pData)
	{
		return nullptr;
	}
	int nType = iGetTypeByData(pData);
	FactoryAssemble* pAssemble = nullptr;

	//获取Assemble
	if (m_vecIdleAssemble.size() == 0)
	{
		//没有空闲的则创建
		pAssemble = new FactoryAssemble;
		pAssemble->bInstance = IsInstanceFactory();
	}
	else
	{
		FactoryAssemble *pTemp = m_vecIdleAssemble.back();
		pAssemble = pTemp;
		m_vecIdleAssemble.pop_back();
	}

	//实例只需要创建一个
	INode* pInstanceNode = m_mapInstanceNode[nType];
	BoardParam& tParam = m_mapLabelBoardParam[nType];
	if (!pInstanceNode)
	{
		pInstanceNode = iCreateNodeForLabel(nType,pData);
		if (pInstanceNode)
		{
			if (m_bAllVisible)
			{
				if (!isTypeVisible(nType))
				{
					m_vecVisibleType.push_back(nType);
				}
			}
			m_mapInstanceNode[nType] = pInstanceNode;
			bool bVisible = isTypeVisible(nType);
			pInstanceNode->iSetVisible(bVisible);
			if (tParam.fWidth == 1.0f && pInstanceNode)
			{
				if (pInstanceNode->GetDynamicComponent<IInstance>())
				{
					tParam = *pInstanceNode->GetDynamicComponent<IInstance>()->iGetBoardParam();
					m_bCalcDisHide = m_bCalcDisHide || (tParam.fVisibleDistance > 0 || tParam.fHideLessDistance > 0);
				}
			}
			m_pInstanceSelector->SetTypeVisible(m_vecVisibleType);
			ParamNetImage::GetInstance()->AddNetTask(pInstanceNode, m_pClient->m_strApiHost);
		}
		
	}
	if (!pInstanceNode || pInstanceNode->iGetNodeType() != NODE_BOARD_INSTANCE)
	{
		return nullptr;
	}
	//添加数据
	BoardInstanceData *pLabelInstance = iAddInstanceData(pInstanceNode,pData);

	pAssemble->nType = nType;
	pAssemble->pLabelNode = pInstanceNode;
	pAssemble->pData = pData;
	pAssemble->pLabelInstance = pLabelInstance;
	pAssemble->bModify.store(false);

	{
		BCAutoLock lock(&m_CurAssembleLock);
		m_vecCurAssemble.emplace_back(pAssemble);
	}

	//判断是否打开详情面板
	if (find(m_vecLastDetailUUID.begin(), m_vecLastDetailUUID.end(), pAssemble->pData->strUuid) != m_vecLastDetailUUID.end())
	{
		GetDetailFactory()->AutoSwitchState(pAssemble, IsRevertClick());
	}
	for (std::map<ELabelFunc, IFunc*>::iterator it = m_mapFuncObject.begin(); it != m_mapFuncObject.end(); ++it)
	{
		it->second->iCreateData(pAssemble);
	}

	return pAssemble;
}

BoardInstanceData* InstanceLabelFactory::iAddInstanceData(INode* pInstanceNode, ProjectBasedSceneElement* pData)
{
	IInstance* pIInstance = pInstanceNode->GetDynamicComponent<IInstance>();

	BoardInstanceParam* pParam = dynamic_cast<BoardInstanceParam*>(pInstanceNode->iGetNodeParam());
	BoardInstanceData stData;
	stData.externParam = (INT_PTR)pData;
	stData.fWidth = pParam->fWidth;
	stData.fHeight = pParam->fHeight;
	stData.fLength = pParam->fLength;
	stData.fSize = 1.0;
	stData.fNormalSize = 1.0;
	stData.fSelectSize = pParam->fSelectedRate;
	stData.externParam = (INT_PTR)pData;
	stData.bRotateAroundTarget = pParam->bRotateAroundTarget;
	stData.bWithLine = pParam->bWithLine;
	stData.vDistanceToTarget = pParam->vDistanceToTarget;
	stData.fHideGreaterDistance = pParam->fVisibleDistance;
	stData.fHideLessDistance = pParam->fHideLessDistance;
	pIInstance->iAddBoard(stData);

	BoardInstanceData* pInstanceData = static_cast<BoardInstanceData*>(pIInstance->iGetInstancedData().back());

	return pInstanceData;
}

void InstanceLabelFactory::iUpdateDataForLabel(FactoryAssemble* pAssemble)
{
	
}

void InstanceLabelFactory::SetTypeVisible(std::vector<int>& vecType)
{
	CommonLabelFactory::SetTypeVisible(vecType);

	for (auto& it : m_mapInstanceNode)
	{
		if (it.second)
		{
			bool bVisible = isTypeVisible(it.first);
			it.second->iSetVisible(bVisible);
		}
	}
	if (m_pInstanceSelector)
	{
		m_pInstanceSelector->SetTypeVisible(vecType);
	}
}

void InstanceLabelFactory::setAssembleVisible(FactoryAssemble* pAssemble, bool bVisible, bool bDynamicNode)
{
	bool bTempVisible = bVisible && CheckAssembleVisible(pAssemble);
	if (pAssemble->pLabelInstance)
	{
		pAssemble->pLabelInstance->bVisible = bTempVisible;
	}

	if (bTempVisible)
	{
		showAssembleForLabel(pAssemble);
		for (std::map<ELabelFunc, IFunc*>::iterator it = m_mapFuncObject.begin(); it != m_mapFuncObject.end(); ++it)
		{
			it->second->iShowAssembleForLabel(pAssemble);
		}
	}
	else
	{
		if (!pAssemble->bInstanceSelected || !bVisible)
		{
			hideAssembleForLabel(pAssemble);
		}
	}

	m_bRefreshInstanceData.store(true);
}

void InstanceLabelFactory::RefreshZero()
{
	CommonLabelFactory::RefreshZero();
	for (auto& it : m_mapInstanceNode)
	{
		if (it.second)
		{
			it.second->GetDynamicComponent<IInstance>()->iClearInstanceData();
		}
	}
}

EventReturnType_e InstanceLabelFactory::iProcessEvent(const BCEvent& tEvent)
{
	if (tEvent.eMessage == EVENT_NODE_CLICK)
	{
		INode* pNode = (INode*)(tEvent.nParam1);
		string sId = "";
		if (pNode)
		{
			sId = pNode->iGetFlagName();
			if (sId.empty())
			{
				sId = pNode->iGetID();
			}
		}
		if (IsContainerNode(pNode,false))
		{
			IInstance* pIInstance = pNode->GetDynamicComponent<IInstance>();
			int nIndex = tEvent.nParam4;
			ProjectBasedSceneElement* pData = (ProjectBasedSceneElement*)(pIInstance->iGetInstancedData()[nIndex]->externParam);
			FactoryAssemble* pAssemble = FindAssembleByData(pData);
			GetDetailFactory()->AutoSwitchState(pAssemble, IsRevertClick());
		}
		else if (IsContainerNode(pNode, true))
		{
			INode* pSelfNode = (INode*)pNode->iGetExternParam();
			FactotryAssembleDetail* pAssemble = GetDetailFactory()->FindAssembleBySelf(pSelfNode);
			if (pAssemble)
			{
				IBoard* pIBoard = pSelfNode->GetDynamicComponent<IBoard>();
				SetDetailSelect(pAssemble, sId, pIBoard ? pIBoard->iIsChecked() : true);
				if (sId == "close")
				{
					GetDetailFactory()->CloseByAssemble(pAssemble);
				}
			}
		}
	}

	m_pDetailFactory->iProcessEvent(tEvent);

	for (std::map<ELabelFunc, IFunc*>::iterator it = m_mapFuncObject.begin(); it != m_mapFuncObject.end(); ++it)
	{
		if (it->first == FUNC_CLUSTER)
		{
			ClusterFunc* pFunc = (ClusterFunc*)it->second;
			if (pFunc->IsOutMode())
			{
				continue;
			}
		}
		it->second->iProcessEvent(tEvent);
	}

	if (m_pInstanceSelector)
	{
		m_pInstanceSelector->iProcessEvent(tEvent);
	}

	return EventReturnType_e::NONE;
}

void InstanceLabelFactory::updateDataForLabel(FactoryAssemble* pAssemble)
{
	if (!pAssemble->bSmoothMove)
	{
		Vector3d vRetAngle = pAssemble->pData->vAngle;
		IInstance* pIInstance = pAssemble->pLabelNode->GetDynamicComponent<IInstance>();
		if (pIInstance)
		{
			vRetAngle += Vector3d(pIInstance->iGetBoardParam()->vModelAngle);
		}
		pAssemble->pLabelInstance->vAngle = vRetAngle;
		pAssemble->pLabelInstance->vOrigin = pAssemble->pData->vLabelPos;
		pAssemble->pLabelInstance->vTargetPosition = pAssemble->pLabelInstance->vOrigin;
		iUpdateDataForLabel(pAssemble);
	}
	m_pDetailFactory->UpdateData(pAssemble->pData);
}

void InstanceLabelFactory::SetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{
	if (m_pInstanceSelector)
	{
		BoardParam* pParam = GetLabelBoardParam(pAssemble->nType);
		if ((!pParam->strCheckedImage.empty() && pParam->bCheckEnable)
			|| (!pParam->strHoverImage.empty()))
		{
			FactoryAssemble* pLabelAssemble = FindAssembleByData(pAssemble->pData);
			m_pInstanceSelector->SetSelect(pLabelAssemble, bSelect);
		}
	}
	CommonLabelFactory::SetLabelSelect(pAssemble, bSelect);
}

bool InstanceLabelFactory::IsContainerNode(INode* pNode, bool bIncludeDetail /*= true*/)
{
	bool bFind = false;
	for (auto &it : m_mapInstanceNode)
	{
		bFind = IsContainerNodeFromNode(it.second, pNode);
		if (bFind)
		{
			return true;
		}
	}

	if (bIncludeDetail)
	{
		std::vector<FactotryAssembleDetail*> vecAssemble;
		m_pDetailFactory->GetCurAssemble(vecAssemble);
		for (size_t i = 0; i < vecAssemble.size(); i++)
		{
			bFind = IsContainerNodeFromNode(vecAssemble[i]->pSelfNode, pNode);
			if (bFind)
			{
				return true;
			}
		}
	}
	return false;
}

bc::INode* InstanceLabelFactory::GetSelectBoardNode(ProjectBasedSceneElement* pInfo)
{
	INode *pNode = m_pInstanceSelector->FindSelectBoardNode(pInfo);
	return pNode;
}

FactoryAssemble* InstanceLabelFactory::FindAssembleBySelectNode(INode* pNode)
{
	if (!pNode)
	{
		return nullptr;
	}
	FactoryAssemble* pSelectAssemble = m_pInstanceSelector->GetSelectFactory()->FindAssembleByNode(pNode);
	if (!pSelectAssemble)
	{
		return nullptr;
	}
	SelectInfo* pSelectInfo = (SelectInfo*)pSelectAssemble->pData;
	return pSelectInfo->pAssemble;
}

void InstanceLabelFactory::ThreadFilterShowAssemble()
{
	if (!m_bRefreshInstanceData.load())
	{
		return;
	}
	if (m_vecCurAssemble.size() < 10 * 10000)
	{
		return;
	}
	m_bDoingThreadFilter = true;
	std::map<int, std::vector<BoardInstanceData*>> mapInstanceData;
	{
		BCAutoLock lock(&m_CurAssembleLock);
		for (auto pAssemble : m_vecCurAssemble)
		{
			if (pAssemble->pLabelInstance && pAssemble->pLabelInstance->bVisible)
			{
				mapInstanceData[pAssemble->nType].push_back(pAssemble->pLabelInstance);
			}
		}
	}
	{
		BCAutoLock lock(&m_RefreshInstanceDataLock);
		m_mapShowInstanceData = mapInstanceData;
	}
	m_bDoingThreadFilter = true;
}

void InstanceLabelFactory::HandleRefreshInstanceData()
{
	if (!m_bRefreshInstanceData.load())
	{
		return;
	}
	m_bRefreshInstanceData.store(false);
	if (m_vecCurAssemble.size() < 10 * 10000)
	{
		return;
	}
	std::map<int, std::vector<BoardInstanceData*>> mapInstanceData;
	{
		BCAutoLock lock(&m_RefreshInstanceDataLock);
		mapInstanceData = m_mapShowInstanceData;
	}
	for (auto& it : m_mapInstanceNode)
	{
		if (it.second)
		{
			std::vector<BoardInstanceData*>& vecData = mapInstanceData[it.first];
			it.second->GetDynamicComponent<IInstance>()->iRefreshInstancedData(vecData);
		}
	}
}

bool InstanceLabelFactory::IsAllStop()
{
	bool bStop = CommonLabelFactory::IsAllStop();
	if (GetSceneElementLoader()->IsRefreshInstanceDataFactory(this))
	{
		GetSceneElementLoader()->RemoveRefreshInstanceDataFactory(this);
	}
	bStop = bStop && !m_bDoingThreadFilter;
	return bStop;
}

