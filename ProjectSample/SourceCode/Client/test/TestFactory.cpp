#include "UserHeader.h"
#include "TestFactory.h"

using namespace bc;

TestFactory::TestFactory()
	:CommonLabelFactory()
{
}

TestFactory::~TestFactory()
{

}

//////////////////////////////////////////////////////////////Label
INode* TestFactory::iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo)
{
	std::string strStylePath = "C:/Users/Administrator/Desktop/test_style.json";
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
		std::string strKey = "style_moni_xiaoqu_label_";
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
		pNode->iSetFlagName("moni_xiaoqu");
		APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(pNode);
		pBoardNode = pNode;
	}

	return pBoardNode;
}

void TestFactory::iUpdateDataForLabel(FactoryAssemble* pAssemble)
{
	CommonLabelFactory::iUpdateDataForLabel(pAssemble);
}

void TestFactory::iPreFrameUpdateForLabel()
{
}

void TestFactory::iFrameUpdateForLabel(FactoryAssemble* pAssemble)
{
}

void TestFactory::iPostFrameUpdateForLabel()
{
}

///////////////////////Detail////////////////////////////
INode* TestFactory::iCreateNodeForDetail(int nType, ProjectBasedSceneElement* pInfo)
{
	std::string strStylePath = "C:/Users/Administrator/Desktop/test_style_detail.json";
	std::string strStyle = ReadFile(strStylePath);
	INode* pBoardNode = nullptr;
	std::string strFontBoard = strStyle;
	{
		std::string strKey = "style_label_detail_";
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

void TestFactory::iOpenForDetail(FactotryAssembleDetail* pAssemble)
{
	pAssemble->vDetailToLabelOffset = Vector2(0,90.0f);
}

void TestFactory::iUpdateDataForDetail(FactotryAssembleDetail* pAssemble)
{
}

void TestFactory::iCloseForDetail(FactotryAssembleDetail* pAssemble)
{
}

void TestFactory::iFrameUpdateForDetail(FactotryAssembleDetail* pAssemble)
{
}

void TestFactory::iOnSetLabelSelect(FactotryAssembleDetail* pAssmeble, bool bSelect)
{
	CommonLabelFactory::iOnSetLabelSelect(pAssmeble,bSelect);
}

void TestFactory::iOnUpdateSelect(FactotryAssembleDetail* pAssmeble, bool bSelect)
{
}

INode* TestFactory::iCreateClusterLabelNode(int nType)
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

void TestFactory::iUpdateClusterLabelNode(FactoryAssemble* pAssemble)
{
	ClusterFunc::LevelData* pInfo = (ClusterFunc::LevelData*)pAssemble->pData;
	pAssemble->pLabelNode->iSetOrigin(pInfo->vBarycenter);
	SetTextShowByStrKey(pAssemble->pLabelNode, "count", to_string(pInfo->GetAllCount()));
}

void TestFactory::iSetClusterLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{

}

void TestFactory::iSetClusterParam(CommonLabelFactory* pFactory, ClusterFunc* pClusterFunc)
{
	pClusterFunc->SetClusterRange(m_pClient->m_vClusterSceneTopLeft, m_pClient->m_vClusterSceneBottomRight);
	for (size_t i = 0; i < m_pClient->m_vecClusterLevel.size(); ++i)
	{
		pClusterFunc->AddLevel(m_pClient->m_vecClusterLevel[i]);
	}
}

INode* TestFactory::iCreateOverTapLabelNode(int nType)
{
	std::string strStylePath = "C:/Users/Administrator/Desktop/test_style_overtap.json";
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
		std::string strKey = "style_overtap_label_";
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

void TestFactory::iUpdateOverTapLabelNode(FactoryAssemble* pAssemble)
{

}

void TestFactory::iSetOverTapLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{

}

void TestFactory::iHideAssembleForLabel(FactoryAssemble* pAssemble)
{

}

void TestFactory::iShowAssembleForLabel(FactoryAssemble* pAssemble)
{

}
