#include "ProjectBasedHeader.h"
#include "Project-Based/label/func/IFunc.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/utils/ParamNetImage.h"
#include "Project-Based/label/LabelDetailFactory.h"
#include "Project-Based/label/func/ClusterFunc.h"
#include "./DefaultDetailFactory.h"
#include "./func/DefaultClusterFunc.h"
#include "Project-Based/label/func/OverTapFunc.h"
#include "Project-Based/label/func/SmoothMoveFunc.h"
#include "Project-Based/label/func/ReadHeightTifFunc.h"
#include "Project-Based/label/func/DetailScaleFunc.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/utils/AnimateWithNodeMg.h"
#include "HeightTifReader.h"
#include "Project-Based/utils/ProjectLogger.h"
#include "Project-Based/utils/TweenAnimateForNode.h"

using namespace bc;

#define KEY_LABEL_FACTORY std::string("labelFactory")

CommonLabelFactory::CommonLabelFactory():
	m_bNodeCanReuse(true),
	m_nType(-1),
	m_nPreCreateNodeCount(50),
	m_bAllVisible(false),
	m_pContainerGroupNode(nullptr),
	m_bRevertClick(true),
	m_fDefaultNoTifZ(0.0f),
	m_bChangeShowByLoader(true),
	m_bCalcDisHide(false),
	m_nCurFrameUpdateIndex(0),
	m_pSceneElementLoader(nullptr)
{
	m_pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
	m_strServerHost = m_pClient->m_strApiHost;
	m_pDetailFactory = new DefaultDetailFactory();
	m_pDetailFactory->SetExtraData(KEY_LABEL_FACTORY,(INT_PTR)this);
}

CommonLabelFactory::~CommonLabelFactory()
{
	for (std::map<ELabelFunc, IFunc*>::iterator it = m_mapFuncObject.begin(); it != m_mapFuncObject.end(); ++it)
	{
		DELETE_PTR(it->second);
	}
	std::vector<FactoryAssemble*> vecAllAssemble;
	vecAllAssemble.insert(vecAllAssemble.end(), m_vecCurAssemble.begin(), m_vecCurAssemble.end());
	vecAllAssemble.insert(vecAllAssemble.end(), m_vecIdleAssemble.begin(), m_vecIdleAssemble.end());
	for (size_t i = 0; i < vecAllAssemble.size(); ++i)
	{
		DELETE_PTR(vecAllAssemble[i]);
	}
	vecAllAssemble.clear();
	m_vecCurAssemble.clear();
	m_vecIdleAssemble.clear();

	std::vector<INode*> vecAllNode;
	for (auto& it : m_mapUsedNode)
	{
		vecAllNode.insert(vecAllNode.end(), it.second.begin(), it.second.end());
		it.second.clear();
	}
	for (auto& it : m_mapIdleNode)
	{
		vecAllNode.insert(vecAllNode.end(), it.second.begin(), it.second.end());
		it.second.clear();
	}
	for (size_t i = 0; i < vecAllNode.size(); ++i)
	{
		ParamNetImage::GetInstance()->RemoveNetTask(vecAllNode[i]);
		AnimateWithNodeMg::GetInstance()->RemoveAnimate(vecAllNode[i]);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&vecAllNode[i]);
	}
	vecAllNode.clear();
	m_mapUsedNode.clear();
	m_mapIdleNode.clear();

	m_vecCurData.clear();

	DELETE_PTR(m_pDetailFactory);
}

void CommonLabelFactory::FrameUpdate()
{
	iPreFrameUpdateForLabel();
	for (std::map<ELabelFunc, IFunc*>::iterator it = m_mapFuncObject.begin(); it != m_mapFuncObject.end(); ++it)
	{
		it->second->iPreFrameUpdate();
	}
	if (!IsNoTypeVisible())
	{
		//图层显示了才有意义
		int nCurFrameUpdateIndex = m_nCurFrameUpdateIndex;
		int nPerMaxCount = 5 * 10000;	//每帧处理的最大数量
		int nStartIndex = nCurFrameUpdateIndex;
		int nEnd = nCurFrameUpdateIndex + nPerMaxCount;
		nCurFrameUpdateIndex = nEnd;
		if (nEnd >= m_vecCurAssemble.size())
		{
			nEnd = m_vecCurAssemble.size() - 1;
			nCurFrameUpdateIndex = 0;
		}
		m_nCurFrameUpdateIndex = nCurFrameUpdateIndex;
		if (m_vecCurAssemble.size() > 0)
		{
			for (size_t i = nStartIndex; i <= nEnd; ++i)
			{
				bool bVisible = isTypeVisible(m_vecCurAssemble[i]->nType);
				if (!bVisible)
				{
					continue;
				}
				frameUpdateForLabel(m_vecCurAssemble[i]);
				for (std::map<ELabelFunc, IFunc*>::iterator it = m_mapFuncObject.begin(); it != m_mapFuncObject.end(); ++it)
				{
					it->second->iFrameUpdate(m_vecCurAssemble[i]);
				}
			}
		}
		if (nEnd >= m_vecCurAssemble.size())
		{
			iPostFrameUpdateForLabel();
		}
	}	
	for (std::map<ELabelFunc, IFunc*>::iterator it = m_mapFuncObject.begin(); it != m_mapFuncObject.end(); ++it)
	{
		it->second->iPostFrameUpdate();
	}

	m_pDetailFactory->FrameUpdate();
}

std::vector<FactoryAssemble*> CommonLabelFactory::AddData(std::vector<ProjectBasedSceneElement*>& vecData)
{
	m_vecCurData.insert(m_vecCurData.end(), vecData.begin(), vecData.end());

	std::vector<FactoryAssemble*> vecAssemble;
	for (size_t i = 0; i < vecData.size(); ++i)
	{
		if (!vecData[i])
		{
			vecAssemble.emplace_back(nullptr);
			continue;
		}
		int nType = iGetTypeByData(vecData[i]);
		FactoryAssemble *pAssemble = handleData(vecData[i]);
		if (pAssemble)
		{
			pAssemble->bSmoothMove = GetFunc(FUNC_SMOOTH_MOVE) && pAssemble->pData->bCanSmooth;
			for (std::map<ELabelFunc, IFunc*>::iterator it = m_mapFuncObject.begin(); it != m_mapFuncObject.end(); ++it)
			{
				it->second->iUpdateData(pAssemble);
			}
			if (pAssemble->pLabelNode)
			{
				bool bVisible = CheckAssembleVisible(pAssemble); 
				updateDataForLabel(pAssemble);
				setAssembleVisible(pAssemble, bVisible, false);
			}
		}
		vecAssemble.emplace_back(pAssemble);
	}
	return vecAssemble;
}

FactoryAssemble* CommonLabelFactory::handleData(ProjectBasedSceneElement* pData)
{
	if (!pData)
	{
		return nullptr;
	}
	int nType = iGetTypeByData(pData);
	INode* pNode = nullptr;
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
		FactoryAssemble* pTemp = m_vecIdleAssemble.back();
		pAssemble = pTemp;
		m_vecIdleAssemble.pop_back();
	}

	pNode = bindIdleNode(pData,false);
	BoardParam& tParam = m_mapLabelBoardParam[nType];
	if (tParam.fWidth == 1.0f && pNode)
	{
		if (m_bAllVisible)
		{
			if (!isTypeVisible(nType))
			{
				m_vecVisibleType.push_back(nType);
			}
		}

		if (pNode->GetDynamicComponent<IBoard>())
		{
			tParam = *pNode->GetDynamicComponent<IBoard>()->iGetBoardParam();
			m_bCalcDisHide = m_bCalcDisHide || (tParam.fVisibleDistance > 0 || tParam.fHideLessDistance > 0);
		}
	}

	bool bCluster = GetFunc(FUNC_CLUSTER) ? true : false;
	if (!bCluster && !pNode)
	{
		return nullptr;
	}

	pAssemble->nType = nType;
	pAssemble->pLabelNode = pNode;
	pAssemble->pData = pData;
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

INode* CommonLabelFactory::checkNeedCreateNode(int nType, ProjectBasedSceneElement* pData, bool bForceCreate)
{
	INode* pNode = nullptr;
	bool bPreCreate = GetFunc(FUNC_CLUSTER)?true:false;
	if (bForceCreate)
	{
		bPreCreate = false;
	}
	if (!bPreCreate)
	{
		pNode = iCreateNodeForLabel(nType, pData);
	}
	else
	{
		int nNodeCount = getAllNodeCount(nType);
		if (nNodeCount < m_nPreCreateNodeCount)
		{
			pNode = iCreateNodeForLabel(nType, pData);
		}
	}
	return pNode;
}

int CommonLabelFactory::getAllNodeCount(int nType)
{
	auto it = m_mapUsedNode.find(nType);
	if (it != m_mapUsedNode.end())
	{
		return it->second.size();
	}
	return 0;
}

void CommonLabelFactory::AddFunc(ELabelFunc eFunc, IFunc* pFuncObject)
{
	IFunc* pObject = pFuncObject;
	if (pFuncObject)
	{
		switch (eFunc)
		{
		case FUNC_CLUSTER:
		{
			ClusterFunc* pFunc = (ClusterFunc*)pFuncObject;
			pFunc->AddFactory(this);
		}
			break;
		case FUNC_OVERTAP:
		{
			OverTapFunc* pFunc = (OverTapFunc*)pFuncObject;
		}
		break;
		case FUNC_SMOOTH_MOVE:
		{
			SmoothMoveFunc* pFunc = (SmoothMoveFunc*)pFuncObject;
		}
		break;
		case FUNC_READ_HRIGHT_TIF:
		{
			ReadHeightTifFunc* pFunc = (ReadHeightTifFunc*)pFuncObject;
		}
		break;
		case FUNC_SCALE_DETAIL:
		{
			DetailScaleFunc* pFunc = (DetailScaleFunc*)pFuncObject;
		}
		break;
		default:
			break;
		}
	}
	else
	{
		switch (eFunc)
		{
		case FUNC_CLUSTER:
		{
			DefaultClusterFunc* pClusterFunc = new DefaultClusterFunc(this);
			iSetClusterParam(this,pClusterFunc);
			pClusterFunc->AddFactory(this);
			pClusterFunc->Start();
			pObject = pClusterFunc;
		}
		break;
		case FUNC_OVERTAP:
		{
			OverTapFunc* pClusterFunc = new OverTapFunc(this);
			pClusterFunc->Start();
			pObject = pClusterFunc;
		}
		break;
		case FUNC_SMOOTH_MOVE:
		{
			SmoothMoveFunc* pFuncObj = new SmoothMoveFunc(this);
			pObject = pFuncObj;
		}
		break;
		case FUNC_READ_HRIGHT_TIF:
		{
			ReadHeightTifFunc* pFuncObj = new ReadHeightTifFunc(this);
			pObject = pFuncObj;
		}
		break;
		case FUNC_SCALE_DETAIL:
		{
			DetailScaleFunc* pFuncObj = new DetailScaleFunc(this);
			pObject = pFuncObj;
		}
		break;
		default:
			break;
		}
	}
	m_mapFuncObject[eFunc] = pObject;
}

void CommonLabelFactory::RemoveFunc(ELabelFunc eFunc,bool bDelete)
{
	for (std::map<ELabelFunc, IFunc*>::iterator it = m_mapFuncObject.begin(); it != m_mapFuncObject.end(); ++it)
	{
		if (it->first == eFunc)
		{
			if (eFunc == FUNC_SCALE_DETAIL)
			{
				HeightTifReader::GetInstance()->RemoveFunc(it->second);
			}
			if (eFunc == FUNC_CLUSTER)
			{
				ClusterFunc* pFunc = (ClusterFunc*)GetFunc(FUNC_CLUSTER);
				pFunc->RemoveFactory(this);
			}
			if (bDelete)
			{
				DELETE_PTR(it->second);
			}
			m_mapFuncObject.erase(it);
			break;
		}
	}
}

IFunc* CommonLabelFactory::GetFunc(ELabelFunc eFunc)
{
	for (std::map<ELabelFunc, IFunc*>::iterator it = m_mapFuncObject.begin(); it != m_mapFuncObject.end(); ++it)
	{
		if (it->first == eFunc)
		{
			return it->second;
		}
	}
	return nullptr;
}

void CommonLabelFactory::RefreshData(std::vector<ProjectBasedSceneElement*>* pVecData)
{
	if (!pVecData)
	{
		return;
	}
	//先记下当前打开着的详情面板id，刷新数据后打开着的依旧打开
	std::vector<FactotryAssembleDetail*> vecDetailAssemble;
	GetDetailFactory()->GetCurAssemble(vecDetailAssemble);
	for (size_t i = 0; i < vecDetailAssemble.size(); ++i)
	{
		m_vecLastDetailUUID.push_back(vecDetailAssemble[i]->pData->strUuid);
	}

	RefreshZero();
	std::vector<ProjectBasedSceneElement*> vecData;
	vecData.insert(vecData.end(),pVecData->begin(),pVecData->end());
	AddData(vecData);
	m_vecLastDetailUUID.clear();
}

void CommonLabelFactory::RefreshZero()
{
	for (size_t i = 0; i < m_vecCurAssemble.size(); ++i)
	{
		setAssembleVisible(m_vecCurAssemble[i],false);
		m_vecCurAssemble[i]->pData = nullptr;
		m_vecCurAssemble[i]->pLabelNode = nullptr;
		m_vecCurAssemble[i]->nType = -1;
		m_vecCurAssemble[i]->bModify.store(true);
	};
	//移到空闲中
	m_vecIdleAssemble.insert(m_vecIdleAssemble.end(), m_vecCurAssemble.begin(), m_vecCurAssemble.end());
	m_vecCurAssemble.clear();
	for (auto& it : m_mapIdleNode)
	{
		std::vector<INode*>& vecUsedNode = m_mapUsedNode[it.first];
		if (m_bNodeCanReuse)
		{		
			it.second.insert(it.second.end(), vecUsedNode.begin(), vecUsedNode.end());
		}
		else
		{
			//直接删除
			for (size_t i = 0; i < vecUsedNode.size(); ++i)
			{
				ParamNetImage::GetInstance()->RemoveNetTask(vecUsedNode[i]);
				AnimateWithNodeMg::GetInstance()->RemoveAnimate(vecUsedNode[i]);
				APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&vecUsedNode[i]);
			}
		}
		vecUsedNode.clear();
	}

	m_vecCurData.clear();

	//临时显隐的去除
	m_nTemporaryState = 0;
	m_vecHideOrShowLabelTempID.clear();
}

bool CommonLabelFactory::CheckAssembleVisible(FactoryAssemble* pAssemble)
{
	if (!pAssemble)
	{
		return false;
	}
	bool bVisible = isTypeVisible(pAssemble->nType); 
	bVisible = bVisible && pAssemble->IsVisible();

	pAssemble->bLastVisible = bVisible;
	return bVisible;
}

void CommonLabelFactory::setAssembleVisible(FactoryAssemble* pAssemble,bool bVisible, bool bDynamicNode)
{
	bVisible = bVisible && CheckAssembleVisible(pAssemble);
	if (pAssemble->pLabelNode)
	{
		pAssemble->pLabelNode->iSetVisible(bVisible);
	}
	if (bVisible)
	{
		//如何没创建，则创建Node
		if (!pAssemble->pLabelNode && bDynamicNode)
		{
			pAssemble->pLabelNode = bindIdleNode(pAssemble->pData,true);
			if (pAssemble->pLabelNode)
			{
				pAssemble->pLabelNode->iSetVisible(true);
				updateDataForLabel(pAssemble);
			}
		}
		showAssembleForLabel(pAssemble);
		for (std::map<ELabelFunc, IFunc*>::iterator it = m_mapFuncObject.begin(); it != m_mapFuncObject.end(); ++it)
		{
			it->second->iShowAssembleForLabel(pAssemble);
		}
	}
	else
	{
		if (m_bNodeCanReuse)
		{
			hideAssembleForLabel(pAssemble);
			if (bDynamicNode && GetFunc(FUNC_CLUSTER))
			{
				//只有聚合才生效
				releaseUsedNode(pAssemble->nType, pAssemble->pLabelNode);
				pAssemble->pLabelNode = nullptr;
			}
		}
		else
		{
			hideAssembleForLabel(pAssemble);
		}
	}
}

void CommonLabelFactory::SetAllVisible(bool bVisible)
{
	m_bAllVisible = bVisible;
	m_vecVisibleType.clear();
	if (m_bAllVisible)
	{
		for (auto& it : m_mapLabelBoardParam)
		{
			m_vecVisibleType.push_back(it.first);
		}
	}
	SetTypeVisible(m_vecVisibleType);
}

void CommonLabelFactory::GetCurData(std::vector<ProjectBasedSceneElement*>& vecData)
{
	vecData.insert(vecData.end(),m_vecCurData.begin(),m_vecCurData.end());
}

ProjectBasedSceneElement* CommonLabelFactory::RemoveByAssemble(FactoryAssemble* pAssemble)
{
	if (!pAssemble)
	{
		return nullptr;
	}
	ProjectBasedSceneElement* pInfo = pAssemble->pData;
	//从Assemble中移出
	std::vector<FactoryAssemble*>::iterator it = find(m_vecCurAssemble.begin(),m_vecCurAssemble.end(),pAssemble);
	if (it != m_vecCurAssemble.end())
	{
		m_vecCurAssemble.erase(it);
	}
	//从数据中移出
	std::vector<ProjectBasedSceneElement*>::iterator it2 = find(m_vecCurData.begin(), m_vecCurData.end(), pInfo);
	if (it2 != m_vecCurData.end())
	{
		m_vecCurData.erase(it2);
	}
	//从Node节点中移出
	if (!IsInstanceFactory())
	{
		releaseUsedNode(pAssemble->nType, pAssemble->pLabelNode);
	}
	else
	{
		pAssemble->pLabelInstance->bVisible = false;
	}
	setAssembleVisible(pAssemble, false,false);

	INode* pNode = pAssemble->pLabelNode;
	//重置
	pAssemble->pData = nullptr;
	pAssemble->pLabelNode = nullptr;
	pAssemble->pLabelInstance = nullptr;
	pAssemble->nType = -1;
	pAssemble->bModify.store(true);
	m_vecIdleAssemble.emplace_back(pAssemble);
	//删除该节点
	if (pNode && !m_bNodeCanReuse)
	{
		//平滑移动的删除
		SmoothMoveFunc* pFunc = (SmoothMoveFunc*)GetFunc(FUNC_SMOOTH_MOVE);
		if (pFunc)
		{
			pFunc->RemoveTweenByID(pInfo->strUuid);
		}

		//从节点列表中移除
		for (auto& it : m_mapUsedNode)
		{
			std::vector<INode*>::iterator  vIt = find(it.second.begin(), it.second.end(), pNode);
			if (vIt != it.second.end())
			{
				it.second.erase(vIt);
			}
		}
		for (auto& it : m_mapIdleNode)
		{
			std::vector<INode*>::iterator  vIt = find(it.second.begin(), it.second.end(), pNode);
			if (vIt != it.second.end())
			{
				it.second.erase(vIt);
			}
		}

		ParamNetImage::GetInstance()->RemoveNetTask(pNode);
		AnimateWithNodeMg::GetInstance()->RemoveAnimate(pNode);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&pNode);

	}

	return pInfo;
}

FactoryAssemble* CommonLabelFactory::FindAssembleByData(ProjectBasedSceneElement* pData)
{
	for (size_t i = 0; i < m_vecCurAssemble.size(); ++i)
	{
		if (m_vecCurAssemble[i]->pData == pData)
		{
			return m_vecCurAssemble[i];
		}
	}
	return nullptr;
}

FactoryAssemble* CommonLabelFactory::FindAssembleByNode(INode* pNode)
{
	for (size_t i = 0; i < m_vecCurAssemble.size(); ++i)
	{
		if (m_vecCurAssemble[i]->pLabelNode == pNode)
		{
			return m_vecCurAssemble[i];
		}
	}
	return nullptr;
}

FactoryAssemble* CommonLabelFactory::FindAssembleByID(const std::string& strID)
{
	for (size_t i = 0; i < m_vecCurAssemble.size(); ++i)
	{
		if (m_vecCurAssemble[i]->pData->strUuid == strID)
		{
			return m_vecCurAssemble[i];
		}
	}
	return nullptr;
}

EventReturnType_e CommonLabelFactory::iProcessEvent(const BCEvent& tEvent)
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
		if (IsContainerNode(pNode, false))
		{
			ProjectBasedSceneElement* pInfo = (ProjectBasedSceneElement*)pNode->iGetExternParam();
			FactoryAssemble* pAssemble = FindAssembleByData(pInfo);
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

	return EventReturnType_e::NONE;
}

INode* CommonLabelFactory::GetContainerGroup()
{
	if (!m_pContainerGroupNode)
	{
		m_pContainerGroupNode = APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode();
	}
	return m_pContainerGroupNode;
}

bool CommonLabelFactory::IsContainerNode(INode* pNode, bool bIncludeDetail)
{
	bool bFind = false;
	for (size_t i = 0; i < m_vecCurAssemble.size(); i++)
	{
		bFind = IsContainerNodeFromNode(m_vecCurAssemble[i]->pLabelNode, pNode);
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

void CommonLabelFactory::SetExtraData(const std::string& strKey, INT_PTR pExtraData)
{
	m_mapExtraData[strKey] = pExtraData;
}

INT_PTR CommonLabelFactory::GetExtraData(const std::string& strKey)
{
	if (m_mapExtraData.find(strKey) != m_mapExtraData.end())
	{
		return m_mapExtraData[strKey];
	}
	return NULL;
}

void CommonLabelFactory::GetCurAssemble(std::vector<FactoryAssemble*>& vecAssemble)
{
	BCAutoLock lock(&m_CurAssembleLock);
	vecAssemble.insert(vecAssemble.end(), m_vecCurAssemble.begin(),m_vecCurAssemble.end());
}

void CommonLabelFactory::RefreshAllVisible()
{
	std::vector<FactoryAssemble*> vecAssemble;
	GetCurAssemble(vecAssemble);
	RefreshVisible(vecAssemble);
}

void CommonLabelFactory::RefreshVisible(std::vector<FactoryAssemble*> &vecAssemble)
{
	bool bVisible = false;
	bool bLastVisible = false;
	int nCount = vecAssemble.size();
	std::vector<FactoryAssemble*> vecShowAssemble;
	std::vector<FactoryAssemble*> vecHideAssemble;
	for (size_t i = 0; i < nCount; ++i)
	{
		if (!vecAssemble[i]->pData)
		{
			continue;
		}
		bLastVisible = vecAssemble[i]->bLastVisible;
		bVisible = CheckAssembleVisible(vecAssemble[i]);
		if (bLastVisible != bVisible)
		{
			if (bVisible)
			{
				vecShowAssemble.push_back(vecAssemble[i]);
			}
			else
			{
				vecHideAssemble.push_back(vecAssemble[i]);
			}
		}
	}
	SetAssembleVisible(vecShowAssemble, true);
	SetAssembleVisible(vecHideAssemble, false);
}

INode* CommonLabelFactory::bindIdleNode(ProjectBasedSceneElement* pData,bool bForceCreate)
{
	INode* pNode = nullptr;
	int nType = iGetTypeByData(pData);
	std::vector<INode*>& vecIdleNode = m_mapIdleNode[nType];
	std::vector<INode*>& vecUsedNode = m_mapUsedNode[nType];
	//获取的Node
	if (!m_bNodeCanReuse)
	{

		pNode = iCreateNodeForLabel(nType, pData);
		ParamNetImage::GetInstance()->AddNetTask(pNode, m_strServerHost);
	}
	else
	{
		if (vecIdleNode.size() == 0)
		{
			//没有空闲的则创建
			pNode = checkNeedCreateNode(nType, pData,bForceCreate);
			ParamNetImage::GetInstance()->AddNetTask(pNode, m_strServerHost);
		}
		else
		{
			std::vector<INode*>::iterator it = vecIdleNode.begin();
			pNode = *it;
			vecIdleNode.erase(it);
		}
	}
	if (pNode)
	{
		vecUsedNode.emplace_back(pNode);
	}

	return pNode;
}

void CommonLabelFactory::releaseUsedNode(int nType,INode* pNode)
{
	if (!pNode)
	{
		return;
	}
	std::vector<INode*>& vecIdleNode = m_mapIdleNode[nType];
	std::vector<INode*>& vecUsedNode = m_mapUsedNode[nType];

	std::vector<INode*>::iterator it3 = find(vecUsedNode.begin(), vecUsedNode.end(), pNode);
	if (it3 != vecUsedNode.end())
	{
		vecUsedNode.erase(it3);
	}
	std::vector<INode*>::iterator it = find(vecIdleNode.begin(), vecIdleNode.end(), pNode);
	if (it == vecIdleNode.end())
	{
		vecIdleNode.push_back(pNode);
	}
}

void CommonLabelFactory::SetAssembleVisible(std::vector<FactoryAssemble*>& vecAssemble, bool bVisible)
{
	for (size_t i = 0; i < vecAssemble.size(); ++i)
	{
		setAssembleVisible(vecAssemble[i], bVisible);
	}
}

void CommonLabelFactory::SetAssembleVisible(FactoryAssemble* pAssemble, bool bVisible)
{
	if (!pAssemble)
	{
		return;
	}
	std::vector<FactoryAssemble*> vecAssemble;
	vecAssemble.push_back(pAssemble);
	SetAssembleVisible(vecAssemble, bVisible);
}

void CommonLabelFactory::iSetSmoothMoveParam(TweenAnimateForNode* pTween)
{

}

bc::Vector3d CommonLabelFactory::iTweenCalcAngle(int nType, Vector3d vCurOrigin, Vector3d vPreOrigin)
{
	Vector3d vAngle = CalcAngle(vPreOrigin, vCurOrigin);
	return vAngle;
}

void CommonLabelFactory::iTweenMove(int nType, FactoryAssemble* pAssemble, Vector3d vPos, Vector3d vAngle, bool bPerEndPoint)
{
	if (pAssemble->bInstance)
	{
		pAssemble->pLabelInstance->vOrigin =  vPos;
		pAssemble->pLabelInstance->vTargetPosition = vPos;
		BoardParam* pBoardParam = GetLabelBoardParam(nType);
		if (pBoardParam->direction == NormalDirect || !pBoardParam->strModelName.empty())
		{
			Vector3d vRetAngle = vAngle;
			IInstance* pIInstance = pAssemble->pLabelNode->GetDynamicComponent<IInstance>();
			if (pIInstance)
			{
				vRetAngle += Vector3d(pIInstance->iGetBoardParam()->vModelAngle);
			}
			pAssemble->pLabelInstance->vAngle = vAngle;
		}
	}
	else if (pAssemble->pLabelNode)
	{
		Vector3d vRetAngle = vAngle;
		IBoard* pIBoard = pAssemble->pLabelNode->GetDynamicComponent<IBoard>();
		if (pIBoard)
		{
			vRetAngle += Vector3d(pIBoard->iGetBoardParam()->vModelAngle);
		}
		pAssemble->pLabelNode->iSetOrigin(vPos);
		pAssemble->pLabelNode->iSetAngles(vRetAngle);
	}
}

void CommonLabelFactory::UpdateData(std::vector<FactoryAssemble*>& vecData)
{
	for (size_t i = 0; i < vecData.size(); ++i)
	{
		vecData[i]->bSmoothMove = GetFunc(FUNC_SMOOTH_MOVE) && vecData[i]->pData->bCanSmooth;
		for (std::map<ELabelFunc, IFunc*>::iterator it = m_mapFuncObject.begin(); it != m_mapFuncObject.end(); ++it)
		{
			it->second->iUpdateData(vecData[i]);
		}
		if (vecData[i]->pLabelNode)
		{
			updateDataForLabel(vecData[i]);
		}

		bool bVisible = CheckAssembleVisible(vecData[i]);
		setAssembleVisible(vecData[i], bVisible, false);
	}
}

void CommonLabelFactory::RefreshClusterVisible(std::vector<FactoryAssemble*>& vecAssemble)
{
	RefreshVisible(vecAssemble);
	for (size_t i = 0; i < vecAssemble.size(); ++i)
	{
		bool bVisible = CheckAssembleVisible(vecAssemble[i]);
		if (bVisible)
		{
			updateDataForLabel(vecAssemble[i]);
		}
	}
}

void CommonLabelFactory::updateDataForLabel(FactoryAssemble* pAssemble)
{
	if (pAssemble->pLabelNode)
	{
		if (!pAssemble->bSmoothMove && pAssemble->pLabelNode->iGetNodeType() != NODE_ROAD
			&& pAssemble->pLabelNode->iGetNodeType() != NODE_FLYLINE
			&& pAssemble->pLabelNode->iGetNodeType() != NODE_POLYGON
			&& pAssemble->pLabelNode->iGetNodeType() != NODE_LOCUS_LINE
			&& pAssemble->pLabelNode->iGetNodeType() != NODE_MULTI_FLYLINE)
		{
			//对Road,Polygon这些不设置位置
			if (pAssemble->pData->vLabelPos.Distance(pAssemble->pLabelNode->iGetOrigin()) != 0)
			{
				pAssemble->pLabelNode->iSetOrigin(pAssemble->pData->vLabelPos);
			}
			Vector3d vRetAngle = pAssemble->pData->vAngle;
			IBoard* pIBoard = pAssemble->pLabelNode->GetDynamicComponent<IBoard>();
			if (pIBoard)
			{
				vRetAngle += Vector3d(pIBoard->iGetBoardParam()->vModelAngle);
			}
			pAssemble->pLabelNode->iSetAngles(vRetAngle);
		}
		pAssemble->pLabelNode->iSetExternParam((INT_PTR)pAssemble->pData);
		SetINTForNodeWithLabel(pAssemble->pLabelNode);
		iUpdateDataForLabel(pAssemble);
	}
	m_pDetailFactory->UpdateData(pAssemble->pData);
}

void CommonLabelFactory::showAssembleForLabel(FactoryAssemble* pAssemble)
{
	iShowAssembleForLabel(pAssemble);
}

void CommonLabelFactory::hideAssembleForLabel(FactoryAssemble* pAssemble)
{
	for (std::map<ELabelFunc, IFunc*>::iterator it = m_mapFuncObject.begin(); it != m_mapFuncObject.end(); ++it)
	{
		it->second->iHideAssembleForLabel(pAssemble);
	}
	iHideAssembleForLabel(pAssemble);
	m_pDetailFactory->CloseByData(pAssemble->pData);
}

void CommonLabelFactory::SetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{
	if (pAssemble && pAssemble->pLabelNode->GetDynamicComponent<IBoard>())
	{
		BoardParam* pParam = pAssemble->pLabelNode->GetDynamicComponent<IBoard>()->iGetBoardParam();
		if (!pParam->strCheckedImage.empty())
		{
			pAssemble->pLabelNode->GetDynamicComponent<IBoard>()->iSetChecked(bSelect);
		}
	}

	for (std::map<ELabelFunc, IFunc*>::iterator it = m_mapFuncObject.begin(); it != m_mapFuncObject.end(); ++it)
	{
		it->second->iSetLabelSelect(pAssemble, bSelect);
	}

	iOnSetLabelSelect(pAssemble, bSelect);
}

void CommonLabelFactory::SetDetailSelect(FactotryAssembleDetail* pAssemble, std::string strFlagName, bool bSelect)
{
	iOnSetDetailSelect(pAssemble,strFlagName,bSelect);
}

void CommonLabelFactory::HideLabelTemporaryByID(std::vector<std::string>& vecID)
{
	for (size_t i = 0; i < m_vecHideOrShowLabelTempID.size(); ++i)
	{
		FactoryAssemble* pAssemble = FindAssembleByID(m_vecHideOrShowLabelTempID[i]);
		if (pAssemble)
		{
			bool bShow = CheckAssembleVisible(pAssemble);
			setAssembleVisible(pAssemble, bShow, false);
		}
	}
	m_nTemporaryState = 1;
	m_vecHideOrShowLabelTempID = vecID;
}

void CommonLabelFactory::OnlyShowLabelTemporaryByID(std::vector<std::string>& vecID)
{
	for (size_t i = 0; i < m_vecHideOrShowLabelTempID.size(); ++i)
	{
		FactoryAssemble* pAssemble = FindAssembleByID(m_vecHideOrShowLabelTempID[i]);
		if (pAssemble)
		{
			bool bShow = CheckAssembleVisible(pAssemble);
			setAssembleVisible(pAssemble, bShow, false);
		}
	}
	m_nTemporaryState = 2;
	m_vecHideOrShowLabelTempID = vecID;
}

void CommonLabelFactory::frameUpdateForLabel(FactoryAssemble* pAssemble)
{
	//判断距离显隐
	if (m_bCalcDisHide)
	{
		bool bShow = CheckAssembleVisible(pAssemble);
		if (bShow)
		{
			bool bVisible = isInRangeToCamera(pAssemble);
			setAssembleVisible(pAssemble, bVisible, false);
		}
	}

	if (m_nTemporaryState != 0)
	{
		bool bCurVisible = CheckAssembleVisible(pAssemble);
		while (true)
		{
			if (!bCurVisible)
			{
				break;
			}
			if (m_nTemporaryState == 1)
			{
				//隐藏
				if (find(m_vecHideOrShowLabelTempID.begin(), m_vecHideOrShowLabelTempID.end(), pAssemble->pData->strUuid) != m_vecHideOrShowLabelTempID.end())
				{
					if (pAssemble->bInstance)
					{
						pAssemble->pLabelInstance->bVisible = false;
					}
					else if (pAssemble->pLabelNode)
					{
						pAssemble->pLabelNode->iSetVisible(false);
					}
					m_pDetailFactory->CloseByData(pAssemble->pData);
				}
			}
			else if (m_nTemporaryState == 2)
			{
				//仅显示
				if (find(m_vecHideOrShowLabelTempID.begin(), m_vecHideOrShowLabelTempID.end(), pAssemble->pData->strUuid) == m_vecHideOrShowLabelTempID.end())
				{
					if (pAssemble->bInstance)
					{
						pAssemble->pLabelInstance->bVisible = false;
					}
					else if (pAssemble->pLabelNode)
					{
						pAssemble->pLabelNode->iSetVisible(false);
					}
					m_pDetailFactory->CloseByData(pAssemble->pData);
				}
			}
			break;
		}
	}

	//监听元素变化
	for (size_t n = 0; n < m_pClient->m_vecListenElement.size(); n++)
	{
		ListenElementParam* pListenElement = &m_pClient->m_vecListenElement[n];
		if (pListenElement->strId == pAssemble->pData->strUuid)
		{
			if (pListenElement->vLastPos.Distance2D(pAssemble->pData->vLabelPos) > 0 && pAssemble->pData->vLabelPos != Vector3d(-1.0f, -1.0f, -1.0f))
			{
				pListenElement->vLastPos = pAssemble->pData->vLabelPos;
				pListenElement->nType = pAssemble->nType;
				sendToWebListenElementRealTime(pListenElement);
			}
			break;
		}
	}
	
	iFrameUpdateForLabel(pAssemble);
}

void CommonLabelFactory::iDoWebCommond(WebCommandData* pWebData)
{
	try
	{
		ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
		if (pWebData->strCommand == "Web_JumpToAnyElement" || pWebData->strCommand == "Web_JumpToLabel")
		{
			if (pWebData->vecValues.size() >= 3)
			{
				int nType = pWebData->vecValues[0].asInt();
				std::string strID = pWebData->vecValues[1].asString();
				float fZDistance = pWebData->vecValues[2].asFloat();
				float fYDistance = 0.0f;
				if (pWebData->vecValues.size() >= 4)
				{
					fYDistance = pWebData->vecValues[3].asFloat();
				}
				if (GetType() == nType || nType == -1)
				{
					std::vector<ProjectBasedSceneElement*> vecData;
					GetCurData(vecData);
					for (size_t m = 0; m < vecData.size(); m++)
					{
						std::string& strTempID = vecData[m]->strUuid;
						if (strTempID == strID)
						{
							Vector3d& vPos = vecData[m]->vLabelPos;
							//if (!pInfo->vecPoint.empty())
							//{
							//	//多值的
							//	for (size_t j = 0; j < pInfo->vecPoint.size(); j++)
							//	{
							//		vPos += pInfo->vecPoint[j];
							//	}
							//	vPos = vPos / pInfo->vecPoint.size();
							//}
							INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
							GoViewPoint(pCameraNode, vPos, fYDistance, fZDistance);
							break;
						}
					}
				}
			}
		}
		else if (pWebData->strCommand == "Web_SelectAnyElement" || pWebData->strCommand == "Web_SelectLabel")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				int nType = pWebData->vecValues[0].asInt();
				Json::Value& jIdArray = pWebData->vecValues[1];
				for (int i = 0; i < jIdArray.size(); i++)
				{
					std::string strId = jIdArray[i].asString();
					if (GetType() == nType || nType == -1)
					{
						std::vector<FactoryAssemble*> vecAssemble;
						GetCurAssemble(vecAssemble);
						for (int n = 0; n < vecAssemble.size(); n++)
						{
							std::string& strTempID = vecAssemble[n]->pData->strUuid;
							if (strTempID == strId)
							{
								GetDetailFactory()->AutoSwitchState(vecAssemble[n], IsRevertClick());
								break;
							}
						}
					}
				}
			}
		}
		else if (pWebData->strCommand == "Web_CancelAnySelectElement" || pWebData->strCommand == "Web_CancelSelectLabel")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				int nType = pWebData->vecValues[0].asInt();
				Json::Value& jIdArray = pWebData->vecValues[1];
				for (int i = 0; i < jIdArray.size(); i++)
				{
					std::string strId = jIdArray[i].asString();
					if (GetType() == nType || nType == -1)
					{
						std::vector<ProjectBasedSceneElement*> vecData;
						GetCurData(vecData);
						for (size_t j = 0; j < vecData.size(); j++)
						{
							if (GetDetailFactory())
							{
								std::string& strTempID = vecData[j]->strUuid;
								if (strTempID == strId)
								{
									GetDetailFactory()->CloseByData(vecData[j]);
									break;
								}
							}
						}
					}
				}
			}
		}
		else if (pWebData->strCommand == "Web_CancelAnyOtherAllSelectElement")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				int nType = pWebData->vecValues[0].asInt();
				Json::Value& jIdArray = pWebData->vecValues[1];
				for (int i = 0; i < jIdArray.size(); i++)
				{
					std::string strId = jIdArray[i].asString();
					if (GetType() == nType || nType == -1)
					{
						std::vector<ProjectBasedSceneElement*> vecData;
						GetCurData(vecData);
						for (size_t j = 0; j < vecData.size(); j++)
						{
							if (GetDetailFactory())
							{
								std::string& strTempID = vecData[j]->strUuid;
								if (strTempID != strId)
								{
									GetDetailFactory()->CloseByData(vecData[j]);
								}
							}
						}
					}
				}
			}
		}
		else if (pWebData->strCommand == "Web_CancelAnyAllSelectElement" || pWebData->strCommand == "Web_CancelAllSelectLabel")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				int nType = pWebData->vecValues[0].asInt();

				if (GetType() == nType || nType == -1)
				{
					GetDetailFactory()->CloseAll();
				}
			}
		}
		else if (pWebData->strCommand == "Web_CancelAnyOtherSelectElementType")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				int nType = pWebData->vecValues[0].asInt();

				if (GetType() != nType)
				{
					GetDetailFactory()->CloseAll();
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetAnyElementVisible")
		{
			if (IsNoTypeVisible())
			{
				return;
			}
			if (pWebData->vecValues.size() >= 2)
			{
				std::string strId = pWebData->vecValues[0].asString();
				bool bVisible = pWebData->vecValues[1].asBool();
				std::vector<std::string> vecID = m_vecHideOrShowLabelTempID;
				std::vector<std::string>::iterator it = find(vecID.begin(), vecID.end(), strId);
				if (!bVisible)
				{
					if (it == vecID.end())
					{
						vecID.push_back(strId);
					}
				}
				else if(it != vecID.end())
				{
					vecID.erase(it);
				}
				HideLabelTemporaryByID(vecID);
			}
		}
		else if (pWebData->strCommand == "Web_DeleteAnyElement")
		{
			//if (pWebData->vecValues.size() >= 1)
			//{
			//	std::string strId = pWebData->vecValues[0].asString();
			//	std::vector<INT_PTR>& vecData = GetCurData();
			//	for (size_t m = 0; m < vecData.size(); m++)
			//	{
			//		std::string &strTempID = iGetIDByData(vecData[m]);
			//		if (strTempID == strId)
			//		{
			//			RemoveByData(vecData[m]);

			//			return;
			//		}
			//	}
			//}
		}
		else if (pWebData->strCommand == "Web_DeleteAllElement")
		{
			//std::vector<INT_PTR> vecData = GetCurData();
			//for (size_t m = 0; m < vecData.size(); m++)
			//{
			//	RemoveByData(vecData[m]);
			//}
		}
		else if (pWebData->strCommand == "Web_AddListenElementId")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				int nType = pWebData->vecValues[0].asInt();
				std::string strId = pWebData->vecValues[1].asString();
				bool bFind = false;
				for (size_t i = 0; i < pClient->m_vecListenElement.size(); i++)
				{
					if ((pClient->m_vecListenElement[i].nType == nType || nType == -1) && pClient->m_vecListenElement[i].strId == strId)
					{
						bFind = true;
						pClient->m_vecListenElement[i].nType = nType;
						break;
					}
				}
				if (!bFind && (nType == GetType() || nType == -1))
				{
					ListenElementParam stParam;
					stParam.nType = nType;
					stParam.strId = strId;
					pClient->m_vecListenElement.push_back(stParam);
				}
			}
		}
		else if (pWebData->strCommand == "Web_RemoveListenElementId")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				int nType = pWebData->vecValues[0].asInt();
				std::string strId = pWebData->vecValues[1].asString();
				for (std::vector<ListenElementParam>::iterator it = pClient->m_vecListenElement.begin(); it != pClient->m_vecListenElement.end(); it++)
				{
					if (((*it).nType == nType || (*it).nType == -1) && (*it).strId == strId)
					{
						pClient->m_vecListenElement.erase(it);
						break;
					}
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		ProjectLogger::GetInstance()->DebugMessage("CommonLabelFactory iDoWebCommond error! " + pWebData->strCommand);
		ProjectLogger::GetInstance()->DebugMessage(e.what());
	}
}

void CommonLabelFactory::sendToWebListenElementRealTime(ListenElementParam* pParam)
{
	if (!pParam)
	{
		return;
	}
	char sz[1024] = { 0 };
	double dLng;
	double dLat;
	sprintf_s(sz, sizeof(sz), "%f,%f,%f", pParam->vLastPos.x, pParam->vLastPos.y, pParam->vLastPos.z);
	if (m_pClient)
	{
		m_pClient->ScenePosToWGS(pParam->vLastPos, dLng, dLat);
	}
	char sz_1[1024] = { 0 };
	sprintf_s(sz_1, sizeof(sz_1), "%lf,%lf,%f", dLng, dLat, pParam->vLastPos.z);

	Json::Value jValue = {};
	jValue["type"] = pParam->nType;
	jValue["id"] = pParam->strId;
	jValue["scene_pos"] = sz;
	jValue["lnglat_pos"] = sz_1;
	std::string strData = jValue.toStyledString();

	m_pClient->ToSendWebCommond("VS_ListenElement", m_pClient->m_strListenElementWebId, strData);
}

bool CommonLabelFactory::isInRangeToCamera(FactoryAssemble* pAssemble)
{
	if (!pAssemble || !pAssemble->pLabelNode)
	{
		return true;
	}
	float fDistanceToCamera = 0.0f;
	INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
	Vector3d vCameraOrigin = pCameraNode->iGetOrigin();
	fDistanceToCamera = pAssemble->pData->vLabelPos.Distance(vCameraOrigin);
	bool bInRange = true;
	BoardParam* pParam = GetLabelBoardParam(pAssemble->nType);
	if (pParam->fVisibleDistance > 0 && fDistanceToCamera > pParam->fVisibleDistance)
	{
		bInRange = false;
	}
	if (pParam->fHideLessDistance > 0 && fDistanceToCamera < pParam->fHideLessDistance)
	{
		bInRange = false;
	}
	return bInRange;
}

void CommonLabelFactory::SetDefaultClusterNoTifZ(float fZ)
{
	ClusterFunc* pFunc = (ClusterFunc*)GetFunc(FUNC_CLUSTER);
	if (pFunc)
	{
		pFunc->SetClusterLabelZ(fZ);
	}
}

float CommonLabelFactory::GetDefaultClusterNoTifZ()
{
	float fZ = 0;
	ClusterFunc* pFunc = (ClusterFunc*)GetFunc(FUNC_CLUSTER);
	if (pFunc)
	{
		fZ = pFunc->GetClusterLabelZ();
	}
	return fZ;
}

std::vector<int> CommonLabelFactory::GetVisibleType()
{
	return m_vecVisibleType;
}

void CommonLabelFactory::SetTypeVisible(std::vector<int>& vecType)
{
	m_vecVisibleType = vecType;
	for (std::map<ELabelFunc, IFunc*>::iterator it = m_mapFuncObject.begin(); it != m_mapFuncObject.end(); ++it)
	{
		it->second->iChangeVisible(m_bAllVisible, m_vecVisibleType);
	}
	for (size_t i = 0; i < m_vecCurAssemble.size(); ++i)
	{
		bool bVisible = isTypeVisible(m_vecCurAssemble[i]->nType);
		setAssembleVisible(m_vecCurAssemble[i], bVisible, false);
	}

	//临时显隐的去除
	m_nTemporaryState = 0;
	m_vecHideOrShowLabelTempID.clear();
}

bool CommonLabelFactory::IsNoTypeVisible()
{
	return m_vecVisibleType.empty() && !m_bAllVisible;
}

bool CommonLabelFactory::isTypeVisible(int nType)
{
	bool bVisible = (find(m_vecVisibleType.begin(), m_vecVisibleType.end(), nType) != m_vecVisibleType.end());
	return bVisible;
}

bool CommonLabelFactory::IsAllStop()
{
	bool bStop = true;
	for (std::map<ELabelFunc, IFunc*>::iterator it = m_mapFuncObject.begin(); it != m_mapFuncObject.end(); ++it)
	{
		bStop = bStop && it->second->isAllStop();
	}
	bStop = bStop && (HeightTifReader::GetInstance()->GetTaskSize() == 0);
	return bStop;
}

void CommonLabelFactory::iSetClusterParam(CommonLabelFactory* pFactory, ClusterFunc* pClusterFunc)
{
	ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
	pClusterFunc->SetClusterRange(pClient->m_vClusterSceneTopLeft, pClient->m_vClusterSceneBottomRight);
	for (size_t i = 0; i < m_pClient->m_vecClusterLevel.size(); ++i)
	{
		pClusterFunc->AddLevel(m_pClient->m_vecClusterLevel[i]);
	}
}

SceneElementLoader* CommonLabelFactory::GetSceneElementLoader()
{
	if (!m_pSceneElementLoader)
	{
		return m_pClient->m_pSceneElementLoader;
	}
	return m_pSceneElementLoader;
}

