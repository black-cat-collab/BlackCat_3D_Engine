#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/utils/ParamNetImage.h"
#include "Project-Based/label/LabelDetailFactory.h"
#include "Project-Based/utils/AnimateWithNodeMg.h"

using namespace bc;

LabelDetailFactory::LabelDetailFactory():
	m_nMaxShowCount(5),
	m_bNodeCanReuse(true),
	m_pNoSelectEffectLastData(nullptr)
{
	m_pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
	m_bRevertClick = false;
	m_strServerHost = m_pClient->m_strApiHost;
}


LabelDetailFactory::~LabelDetailFactory()
{
	std::vector<FactotryAssembleDetail*> vecAllAssemble;
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
}

FactotryAssembleDetail* LabelDetailFactory::AutoSwitchState(FactoryAssemble* pLabelAssemble, bool bRevertClick)
{
	if (!pLabelAssemble)
	{
		return nullptr;
	}

	FactotryAssembleDetail* pAssemble = handleData(pLabelAssemble,bRevertClick);
	return pAssemble;
}

void LabelDetailFactory::UpdateData(ProjectBasedSceneElement* pData)
{
	FactotryAssembleDetail* pAssemble = FindAssembleByData(pData);
	if (pAssemble)
	{
		iUpdateWithNode(pAssemble);
	}
}

void LabelDetailFactory::FrameUpdate()
{
	for (size_t i = 0; i < m_vecCurAssemble.size(); ++i)
	{
		iFrameUpdateWithNode(m_vecCurAssemble[i]);
	}
}

FactotryAssembleDetail* LabelDetailFactory::handleData(FactoryAssemble* pLabelAssemble, bool bRevertClick)
{
	FactotryAssembleDetail* pAssemble = FindAssembleByData(pLabelAssemble->pData);
	int nType = iGetTypeByData(pLabelAssemble->pData);
	INode* pNode = nullptr;
	if (bRevertClick && pAssemble)
	{
		//关闭(反制为空)
		CloseByAssemble(pAssemble);
		return nullptr;
	}
	else if (pAssemble)
	{
		//不反置但已经找到了，则直接返回，及点击无效
		return nullptr;
	}

	//判断是不是没有选中效果，又没有详情面板，是则一直给发选中状态（为了解决因没有选中和非选中的区分问题）
	bool bHasSelectEffect = CheckNodeHasSelectEffect(pLabelAssemble->pLabelNode);	//是否有选中效果
	if (m_bNodeCanReuse)
	{
		//判断是否需要关闭之前的
		if (m_nMaxShowCount > 0 && m_vecCurAssemble.size() >= m_nMaxShowCount)
		{
			//关闭最开始的
			CloseByAssemble(m_vecCurAssemble.front());
			pAssemble = handleData(pLabelAssemble, bRevertClick);
			return pAssemble;
		}
	}
	//获取Assemble
	if (m_vecIdleAssemble.size() == 0)
	{
		//没有空闲的则创建
		pAssemble = new FactotryAssembleDetail;
		pAssemble->nType = nType;
	}
	else
	{
		std::vector<FactotryAssembleDetail*>::iterator it = m_vecIdleAssemble.begin();
		pAssemble = *it;
		m_vecIdleAssemble.erase(it);
	}
	std::vector<INode*>& vecIdleNode = m_mapIdleNode[nType];
	std::vector<INode*>& vecUsedNode = m_mapUsedNode[nType];
	BoardParam tParam = m_mapLabelBoardParam[nType];
	if (m_bNodeCanReuse)
	{
		//获取的Node
		if (vecIdleNode.size() == 0)
		{
			//没有空闲的则创建
			pNode = iCreateNode(nType, pLabelAssemble->pData);
			ParamNetImage::GetInstance()->AddNetTask(pNode, m_strServerHost);
		}
		else
		{
			std::vector<INode*>::iterator it = vecIdleNode.begin();
			pNode = *it;
			vecIdleNode.erase(it);
		}
	}
	else
	{
		pNode = iCreateNode(nType, pLabelAssemble->pData);
		ParamNetImage::GetInstance()->AddNetTask(pNode, m_strServerHost);
	}
	if (!tParam.fWidth == 0.0f && pNode)
	{
		if (pNode->GetDynamicComponent<IBoard>())
		{
			tParam = *pNode->GetDynamicComponent<IBoard>()->iGetBoardParam();
		}
	}

	bHasSelectEffect = bHasSelectEffect || pNode;

	pAssemble->pData = pLabelAssemble->pData;
	pAssemble->pLabelNode = pLabelAssemble->pLabelNode;
	pAssemble->pBoardInstanceData = pLabelAssemble->pLabelInstance;
	pAssemble->pSelfNode = pNode;
	pAssemble->bInstance = pLabelAssemble->bInstance;
	pAssemble->nType = nType;
	if (bHasSelectEffect)
	{
		m_vecCurAssemble.emplace_back(pAssemble);
	}
	else
	{
		//还是放回空闲中
		m_vecIdleAssemble.emplace_back(pAssemble);
		//为了兼容以前的网页代码做特殊处理
		m_pNoSelectEffectLastData = pAssemble->pData;
		//为了兼容以前的网页代码做特殊处理
	}
	if (pAssemble->pSelfNode)
	{
		pAssemble->pSelfNode->iSetVisible(true);
		vecUsedNode.push_back(pNode);
	}
	iOpenWithNode(pAssemble);
	iUpdateWithNode(pAssemble);

	return pAssemble;
}

FactotryAssembleDetail* LabelDetailFactory::FindAssembleByData(ProjectBasedSceneElement* pData)
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

FactotryAssembleDetail* LabelDetailFactory::FindAssembleBySelf(INode* pNode)
{
	for (size_t i = 0; i < m_vecCurAssemble.size(); ++i)
	{
		if (m_vecCurAssemble[i]->pSelfNode == pNode)
		{
			return m_vecCurAssemble[i];
		}
	}
	return nullptr;
}

void LabelDetailFactory::CloseByData(ProjectBasedSceneElement* pData)
{
	//为了兼容以前的网页代码做特殊处理
	if (m_pNoSelectEffectLastData && m_pNoSelectEffectLastData == pData)
	{
		toWebNoSelectEffect();
	}
	//为了兼容以前的网页代码做特殊处理

	FactotryAssembleDetail* pAssemble = FindAssembleByData(pData);
	CloseByAssemble(pAssemble);
}

void LabelDetailFactory::CloseByAssemble(FactotryAssembleDetail* pAssemble)
{
	if (!pAssemble)
	{
		return;
	}
	int nType = iGetTypeByData(pAssemble->pData);
	std::vector<INode*>& vecIdleNode = m_mapIdleNode[nType];
	std::vector<INode*>& vecUsedNode = m_mapUsedNode[nType];

	if (pAssemble->pSelfNode)
	{
		pAssemble->pSelfNode->iSetVisible(false);
	}
	//从Assemble中移出
	std::vector<FactotryAssembleDetail*>::iterator it = find(m_vecCurAssemble.begin(), m_vecCurAssemble.end(),pAssemble);
	if (it != m_vecCurAssemble.end())
	{
		m_vecCurAssemble.erase(it);
	}
	m_vecIdleAssemble.emplace_back(pAssemble);
	if (m_bNodeCanReuse)
	{
		//从Node节点中移出
		std::vector<INode*>::iterator it2 = find(vecUsedNode.begin(), vecUsedNode.end(), pAssemble->pSelfNode);
		if (it2 != vecUsedNode.end())
		{
			vecUsedNode.erase(it2);
		}
		if (pAssemble->pSelfNode)
		{
			vecIdleNode.push_back(pAssemble->pSelfNode);
		}
	}
	iCloseWithNode(pAssemble);

	pAssemble->pData = nullptr;
	pAssemble->pSelfNode = nullptr;
}

void LabelDetailFactory::CloseAll()
{
	//为了兼容以前的网页代码做特殊处理
	toWebNoSelectEffect();
	//为了兼容以前的网页代码做特殊处理

	std::vector<FactotryAssembleDetail*> vecTemp = m_vecCurAssemble;
	for (size_t i = 0; i < vecTemp.size(); ++i)
	{
		CloseByAssemble(vecTemp[i]);
	}
}

void LabelDetailFactory::SetExtraData(const std::string& strKey, INT_PTR pExtraData)
{
	m_mapExtraData[strKey] = pExtraData;
}

INT_PTR LabelDetailFactory::GetExtraData(const std::string& strKey)
{
	if (m_mapExtraData.find(strKey) != m_mapExtraData.end())
	{
		return m_mapExtraData[strKey];
	}
	return NULL;
}

EventReturnType_e LabelDetailFactory::iProcessEvent(const BCEvent& tEvent)
{
	return EventReturnType_e::NONE;
}

void LabelDetailFactory::GetCurAssemble(std::vector<FactotryAssembleDetail*>& vecAssemble)
{
	vecAssemble.insert(vecAssemble.end(), m_vecCurAssemble.begin(), m_vecCurAssemble.end());
}

void LabelDetailFactory::DeleteByAssemble(FactotryAssembleDetail* pAssemble)
{
	INode* pNode = pAssemble->pSelfNode;
	CloseByAssemble(pAssemble);
	ParamNetImage::GetInstance()->RemoveNetTask(pNode);
	AnimateWithNodeMg::GetInstance()->RemoveAnimate(pNode);
	APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&pNode);
}

bool LabelDetailFactory::CheckNodeHasSelectEffect(INode* pLabelNode)
{
	if (!pLabelNode)
	{
		return false;
	}
	bool bHasSelectEffect = true;	//是否有选中效果
	//判断是不是没有选中效果，又没有详情面板，是则一直给发选中状态（为了解决因没有选中和非选中的区分问题）
	if (pLabelNode)
	{
		if (pLabelNode->iGetNodeType() == NODE_BOARD_INSTANCE)
		{
			IInstance* pInstance = pLabelNode->GetDynamicComponent<IInstance>();
			if (pInstance)
			{
				BoardInstanceParam* pParam = pInstance->iGetBoardParam();
				if ((pParam->strCheckedImage.empty()) && pParam->strHoverImage.empty())
				{
					bHasSelectEffect = false;
				}
			}
		}
		else
		{
			IBoard* pBoard = pLabelNode->GetDynamicComponent<IBoard>();
			if (pBoard)
			{
				BoardParam* pParam = pBoard->iGetBoardParam();
				if (pParam->strCheckedImage.empty())
				{
					bHasSelectEffect = false;
				}
			}
		}
	}
	return bHasSelectEffect;
}

void LabelDetailFactory::toWebNoSelectEffect()
{
	//为了兼容以前的网页代码做特殊处理
	if (m_pNoSelectEffectLastData)
	{
		//给网页发送关闭消息
		bool bClick = false;
		std::string strExtra = m_pNoSelectEffectLastData->strExtra;
		if (strExtra.empty())
		{
			strExtra = "''";
		}
		std::string str = "";
		str += "'" + m_pNoSelectEffectLastData->strUuid + "'";
		str += ",";
		str += to_string(m_pNoSelectEffectLastData->nTypeID);
		str += ",";
		str += strExtra;
		str += ",";
		str += bClick ? "1" : "0";
		m_pClient->ToSendWebCommond("VS_ClickHCMapLabel", "", str);

		m_pNoSelectEffectLastData = nullptr;
	}
	//为了兼容以前的网页代码做特殊处理
}


