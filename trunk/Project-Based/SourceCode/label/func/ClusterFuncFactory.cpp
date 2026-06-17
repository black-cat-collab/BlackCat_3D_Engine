#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/label/func/IFunc.h"
#include "Project-Based/label/func/ClusterFunc.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "ClusterFuncFactory.h"

using namespace bc;

ClusterFuncFactory::ClusterFuncFactory(ClusterFunc* pClusterFunc) :
	CommonLabelFactory(),
	m_pClusterFunc(pClusterFunc)
{
	//m_bNodeCanReuse = false;
}

ClusterFuncFactory::~ClusterFuncFactory()
{
	m_mapNode.clear();
}

INode* ClusterFuncFactory::iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo)
{
	//if (m_mapNode.find(pInfo->strUuid) != m_mapNode.end())
	//{
	//	return m_mapNode[pInfo->strUuid];
	//}
	//INode *pNode = m_pClusterFunc->iCreateClusterLabelNode(nType);
	//if (pNode)
	//{
	//	m_mapNode[pInfo->strUuid] = pNode;
	//}
	//return pNode;

	return m_pClusterFunc->iCreateClusterLabelNode(nType);
}

void ClusterFuncFactory::iUpdateDataForLabel(FactoryAssemble* pAssemble)
{
	m_pClusterFunc->iUpdateClusterLabelNode(pAssemble);
}

void ClusterFuncFactory::iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{
	m_pClusterFunc->iSetClusterLabelSelect(pAssemble,bSelect);
}

void ClusterFuncFactory::RefreshZero()
{
	//for (size_t i = 0; i < m_vecCurAssemble.size(); ++i)
	//{
	//	setAssembleVisible(m_vecCurAssemble[i], false);
	//	m_vecCurAssemble[i]->pData = nullptr;
	//	m_vecCurAssemble[i]->pLabelNode = nullptr;
	//	m_vecCurAssemble[i]->nType = -1;
	//	m_vecCurAssemble[i]->bModify.store(true);
	//};
	////移到空闲中
	//m_vecIdleAssemble.insert(m_vecIdleAssemble.end(), m_vecCurAssemble.begin(), m_vecCurAssemble.end());
	//m_vecCurAssemble.clear();
	//for (auto& it : m_mapIdleNode)
	//{
	//	std::vector<INode*>& vecUsedNode = m_mapUsedNode[it.first];
	//	if (m_bNodeCanReuse)
	//	{
	//		it.second.insert(it.second.end(), vecUsedNode.begin(), vecUsedNode.end());
	//	}
	//	else
	//	{
	//	}
	//	vecUsedNode.clear();
	//}

	//m_vecCurData.clear();

	////临时显隐的去除
	//m_nTemporaryState = 0;
	//m_vecHideOrShowLabelTempID.clear();

	CommonLabelFactory::RefreshZero();
}

