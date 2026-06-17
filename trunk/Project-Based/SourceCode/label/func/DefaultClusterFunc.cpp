#include "ProjectBasedHeader.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/label/func/IFunc.h"
#include "Project-Based/label/func/ClusterFunc.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "DefaultClusterFunc.h"

using namespace bc;

DefaultClusterFunc::DefaultClusterFunc(CommonLabelFactory* pFactory):
	m_pFactory(pFactory)
{
}

DefaultClusterFunc::~DefaultClusterFunc()
{

}

INode* DefaultClusterFunc::iCreateClusterLabelNode(int nType)
{
	return m_pFactory->iCreateClusterLabelNode(nType);
}

void DefaultClusterFunc::iUpdateClusterLabelNode(FactoryAssemble* pAssemble)
{
	ClusterFunc::LevelData* pInfo = (ClusterFunc::LevelData*)pAssemble->pData;
	pAssemble->pLabelNode->iSetExternParam((INT_PTR)pInfo);
	SetINTForNodeWithLabel(pAssemble->pLabelNode);
	m_pFactory->iUpdateClusterLabelNode(pAssemble->pLabelNode, pInfo->GetAllCount());
}

void DefaultClusterFunc::iSetClusterLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{
	m_pFactory->iSetClusterLabelSelect(pAssemble->pLabelNode,bSelect);
}

