#include "UserHeader.h"
#include "TestInstanceFactory.h"

using namespace bc;

TestInstanceFactory::TestInstanceFactory()
	:InstanceLabelFactory()
{
}

TestInstanceFactory::~TestInstanceFactory()
{

}

//////////////////////////////////////////////////////////////Label
INode* TestInstanceFactory::iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo)
{
	std::string strStylePath = "C:/Users/Administrator/Desktop/test_instance_style.json";
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
		std::string strKey = "style_instance_label_";
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
		pNode->iSetFlagName("moni_instance_xiaoqu");
		APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(pNode);
		pBoardNode = pNode;
	}

	return pBoardNode;
}

void TestInstanceFactory::iUpdateDataForLabel(FactoryAssemble* pAssemble)
{
	InstanceLabelFactory::iUpdateDataForLabel(pAssemble);
}

///////////////////////Detail////////////////////////////
INode* TestInstanceFactory::iCreateNodeForDetail(int nType, ProjectBasedSceneElement* pInfo)
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

void TestInstanceFactory::iOpenForDetail(FactotryAssembleDetail* pAssemble)
{
}

void TestInstanceFactory::iUpdateDataForDetail(FactotryAssembleDetail* pAssemble)
{
}

void TestInstanceFactory::iCloseForDetail(FactotryAssembleDetail* pAssemble)
{
}

void TestInstanceFactory::iFrameUpdateForDetail(FactotryAssembleDetail* pAssemble)
{
}

void TestInstanceFactory::iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{
	InstanceLabelFactory::iOnSetLabelSelect(pAssemble, bSelect);
}

void TestInstanceFactory::iOnUpdateSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{
}

INode* TestInstanceFactory::iCreateClusterLabelNode(int nType)
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

void TestInstanceFactory::iUpdateClusterLabelNode(INode* pClusterNode, const int nCount)
{
	SetTextShowByStrKey(pClusterNode, "count", to_string(nCount));
}

void TestInstanceFactory::iSetClusterLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{

}

void TestInstanceFactory::iSetClusterParam(CommonLabelFactory* pFactory, ClusterFunc* pClusterFunc)
{
	pClusterFunc->SetClusterRange(m_pClient->m_vClusterSceneTopLeft, m_pClient->m_vClusterSceneBottomRight);
	for (size_t i = 0; i < m_pClient->m_vecClusterLevel.size(); ++i)
	{
		pClusterFunc->AddLevel(m_pClient->m_vecClusterLevel[i]);
	}
}

INode* TestInstanceFactory::iCreateOverTapLabelNode(int nType)
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

void TestInstanceFactory::iUpdateOverTapLabelNode(FactoryAssemble* pAssemble)
{

}

void TestInstanceFactory::iSetOverTapLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{

}
