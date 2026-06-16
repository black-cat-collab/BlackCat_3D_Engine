#include "UserHeader.h"
#include "TestClusterFunc.h"

using namespace bc;

TestClusterFunc::TestClusterFunc()
{
}

TestClusterFunc::~TestClusterFunc()
{

}

INode* TestClusterFunc::iCreateClusterLabelNode(int nType)
{
	std::string strStylePath = "C:/Users/Administrator/Desktop/test_style_cluster.json";
	std::string strStyle = ReadFile(strStylePath);
	Json::Value jArray;
	ParseJsonByString(strStyle, jArray);
	if (!jArray.isArray())
	{
		return nullptr;
	}
	INode* pBoardNode = nullptr;
	std::string strFontBoard = jArray[0].toStyledString();
	{
		std::string strKey = "style_cluster_label_";
		strKey += to_string(nType);
		if (!ParamParseStream::GetInstance()->IsContainKey(strKey))
		{
			ParamParseStream::GetInstance()->ReadParamFromString(strFontBoard, strKey);
		}
		NodeParamBase* pParam = NULL;
		if (ParamParseStream::GetInstance()->IsContainKey(strKey))
		{
			ReadParam* pReadParam = ParamParseStream::GetInstance()->GetReadParamByKey(strKey);
			pParam = dynamic_cast<NodeParamBase*>(pReadParam->pParam);
		}
		if (!pParam)
		{
			return nullptr;
		}
		INode* pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(*pParam);
		pNode->iLoadModel();
		APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(pNode);
		pBoardNode = pNode;
	}

	return pBoardNode;
}

void TestClusterFunc::iUpdateClusterLabelNode(FactoryAssemble* pAssemble)
{
	ClusterFunc::LevelData* pInfo = (ClusterFunc::LevelData*)pAssemble->pData;
	pAssemble->pLabelNode->iSetExternParam((INT_PTR)pInfo);
	SetINTForNodeWithLabel(pAssemble->pLabelNode);
	pAssemble->pLabelNode->iSetOrigin(pInfo->vBarycenter);
	SetTextShowByStrKey(pAssemble->pLabelNode,"count",to_string(pInfo->GetAllCount()));
}

void TestClusterFunc::iSetClusterLabelSelect(FactoryAssemble* pAssemble, bool bSelect)
{
}
