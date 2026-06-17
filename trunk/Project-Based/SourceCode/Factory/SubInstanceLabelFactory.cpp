#include "ProjectBasedHeader.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/label/InstanceLabelFactory.h"
#include "Project-Based/Factory/SubInstanceLabelFactory.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/utils/ParamParseStream.h"
#include "Project-Based/utils/ParamNetImage.h"
#include "Project-Based/utils/AnimateWithNodeMg.h"
#include "Project-Based/base/SceneElementLoader.h"

using namespace bc;

SubInstanceLabelFactory::SubInstanceLabelFactory(SceneElementLoader* pLoader):
	InstanceLabelFactory(),
	m_pElementLoader(pLoader)
{
}

SubInstanceLabelFactory::~SubInstanceLabelFactory()
{
}

INode* SubInstanceLabelFactory::iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pData)
{
	ProjectBasedSceneElementTypeInfo* pTypeInfo = m_pElementLoader->GetElementTypeInfoByID(GetType());
	INode* pReturnNode = NULL;
	std::vector<std::string> vecStrLabelStyle = pTypeInfo->vecStrLabelStyle;
	if (vecStrLabelStyle.size() > 0)
	{
		string strLabelKey = SCENE_ELEMENT_PARAM_KEY + "label_";
		strLabelKey += to_string(pTypeInfo->nTypeID);

		string strStyle = vecStrLabelStyle[0];
		if (!ParamParseStream::GetInstance()->IsContainKey(strLabelKey))
		{
			ParamParseStream::GetInstance()->ReadParamFromString(strStyle, strLabelKey);
		}
		NodeParamBase* pParam = NULL;
		int nSort = 200;
		if (ParamParseStream::GetInstance()->IsContainKey(strLabelKey))
		{
			ReadParam* pReadParam = ParamParseStream::GetInstance()->GetReadParamByKey(strLabelKey);
			pParam = dynamic_cast<NodeParamBase*>(pReadParam->pParam);
			nSort = pReadParam->nSort;
		}
		pReturnNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(*pParam);
		bool bNetLoadModel = ParamNetImage::GetInstance()->CheckNetLoadModel(pReturnNode);
		if (!bNetLoadModel)
		{
			pReturnNode->iLoadModel();
		}
		else
		{
			//模型则添加到队列进行下载检测，所以暂不加载Model
			ParamNetImage::GetInstance()->AddModelTask(pReturnNode, m_strServerHost);
		}
		pReturnNode->iSetVisible(false);
		pReturnNode->iSetSaveMyself(false);
		GetContainerGroup()->iAddChild(pReturnNode);
		GetComponent<IRenderNode>(pReturnNode)->iSetRenderSort(nSort);
	}

	return pReturnNode;
}

INode* SubInstanceLabelFactory::iCreateNodeForDetail(int nType, ProjectBasedSceneElement* pData)
{
	ProjectBasedSceneElementTypeInfo* pInfo = m_pElementLoader->GetElementTypeInfoByID(nType);
	if (!pData)
	{
		return NULL;
	}
	if (!pInfo->Is3DOption_Detail_Label())
	{
		return nullptr;
	}
	std::string strDetailStyle = pInfo->strDetailStyle;
	if (strDetailStyle.empty())
	{
		return nullptr;
	}
	bool bQuancheng = false;
	string strLabelKey = SCENE_ELEMENT_PARAM_KEY + "detail_factory_";
	strLabelKey += to_string(nType);
	strLabelKey += "_";
	strLabelKey += m_strServerHost;
	if (!ParamParseStream::GetInstance()->IsContainKey(strLabelKey))
	{
		ParamParseStream::GetInstance()->ReadParamFromString(strDetailStyle, strLabelKey);
	}
	NodeParamBase* pParam = NULL;
	int nSort = 200;
	if (ParamParseStream::GetInstance()->IsContainKey(strLabelKey))
	{
		ReadParam* pReadParam = ParamParseStream::GetInstance()->GetReadParamByKey(strLabelKey);
		pParam = dynamic_cast<NodeParamBase*>(pReadParam->pParam);
		nSort = pReadParam->nSort;
	}
	INode* pNode = NULL;
	if (pParam)
	{
		pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(*pParam);
		pNode->iSetVisible(false);
		pNode->iLoadModel();
		pNode->GetDynamicComponent<IRenderNode>()->iSetRenderSort(nSort);
		pNode->iSetSaveMyself(false);
		GetContainerGroup()->iAddChild(pNode);

		SetINTForNodeWithDetail(pNode);
	}

	return pNode;
}

void SubInstanceLabelFactory::iOpenForDetail(FactotryAssembleDetail* pAssemble)
{
	ProjectBasedSceneElement* pInfo = (ProjectBasedSceneElement*)pAssemble->pData;
	ProjectBasedSceneElementTypeInfo* pTypeInfo = m_pElementLoader->GetElementTypeInfoByID(pInfo->nTypeID);
	pAssemble->vDetailToLabelOffset = Vector2(pTypeInfo->fOffsetDetailX, pTypeInfo->fOffsetDetailY);
	m_pElementLoader->iOnOpenDetail(pAssemble->pSelfNode, pTypeInfo, pInfo);
}

void SubInstanceLabelFactory::iUpdateDataForDetail(FactotryAssembleDetail* pAssemble)
{
	if (!pAssemble->pSelfNode)
	{
		return;
	}
	ProjectBasedSceneElement* pInfo = (ProjectBasedSceneElement*)pAssemble->pData;
	if (pInfo->strTitle.empty())
	{
		IBoard* pBoard = GetComponent<IBoard>(pAssemble->pSelfNode);
		if (pBoard)
		{
			std::vector<TextShow*> vecTextShow;
			FindAllTextShow(pAssemble->pSelfNode, vecTextShow);
			for (size_t i = 0; i < vecTextShow.size(); i++)
			{
				std::string strText = Replace_all(vecTextShow[i]->strText, "\n", "");
				SetTextShowByStrKey(pAssemble->pSelfNode, vecTextShow[i]->strID, strText, true);
			}
		}
	}
	pAssemble->pSelfNode->iSetExternParam((INT_PTR)pInfo);
	SetINTForNodeWithDetail(pAssemble->pSelfNode);

	ProjectBasedSceneElementTypeInfo* pTypeInfo = m_pElementLoader->GetElementTypeInfoByID(pInfo->nTypeID);
	m_pElementLoader->iOnSetDetailData(pAssemble->pSelfNode, pTypeInfo, pInfo);
}

void SubInstanceLabelFactory::iCloseForDetail(FactotryAssembleDetail* pAssemble)
{
	ProjectBasedSceneElement* pInfo = (ProjectBasedSceneElement*)pAssemble->pData;
	ProjectBasedSceneElementTypeInfo* pTypeInfo = m_pElementLoader->GetElementTypeInfoByID(pInfo->nTypeID);
	m_pElementLoader->iOnCloseDetail(pAssemble->pSelfNode, pTypeInfo, pInfo);
}

void SubInstanceLabelFactory::iSetClusterParam(CommonLabelFactory* pFactory, ClusterFunc* pClusterFunc)
{
	ProjectBasedSceneElementTypeInfo* pTypeInfo = m_pElementLoader->GetElementTypeInfoByID(m_nType);
	if (!pTypeInfo)
	{
		CommonLabelFactory::iSetClusterParam(pFactory, pClusterFunc);
		return;
	}
	std::vector<ProjectBasedSceneClusterLevel> vecLevel;
	Vector3d vLeftTop;
	Vector3d vRightBottom;
	bool bEnable = pTypeInfo->Parse3DOption_Cluster(vecLevel, vLeftTop, vRightBottom);
	bool bWgs84 = pTypeInfo->Is3DOption_Cluster_WGS84();
	if (bWgs84)
	{
		m_pClient->ConvertLngToVector3(vLeftTop.x, vLeftTop.y, &vLeftTop);
		m_pClient->ConvertLngToVector3(vRightBottom.x, vRightBottom.y, &vRightBottom);
	}
	if (bEnable)
	{
		if (vecLevel.size() == 0)
		{
			CommonLabelFactory::iSetClusterParam(pFactory, pClusterFunc);
		}
		else
		{
			pClusterFunc->SetClusterRange(vLeftTop, vRightBottom);
			for (size_t i = 0; i < vecLevel.size(); ++i)
			{
				ClusterFunc::ClusterLevel tLevel;
				tLevel.nLevel = vecLevel[i].nIndex;
				tLevel.nRow = vecLevel[i].nRow;
				tLevel.nColumn = vecLevel[i].nColumn;
				tLevel.fDistance = vecLevel[i].fDistance;
				pClusterFunc->AddLevel(tLevel);
			}
		}
	}
	else
	{
		CommonLabelFactory::iSetClusterParam(pFactory, pClusterFunc);
	}
}

INode* SubInstanceLabelFactory::iCreateClusterLabelNode(int nType)
{
	INode* pNode = nullptr;
	ProjectBasedSceneElementTypeInfo* pTypeInfo = m_pElementLoader->GetElementTypeInfoByID(GetType());
	if (!pTypeInfo)
	{
		return NULL;
	}
	string strLabelKey = SCENE_ELEMENT_PARAM_KEY + "label_cluster_";
	strLabelKey += to_string(GetType());
	strLabelKey += "_";
	strLabelKey += m_strServerHost;
	std::string strStyle = pTypeInfo->strClusterStyle;
	if (!strStyle.empty())
	{
		if (!ParamParseStream::GetInstance()->IsContainKey(strLabelKey))
		{
			ParamParseStream::GetInstance()->ReadParamFromString(strStyle, strLabelKey);
		}
		NodeParamBase* pParam = NULL;
		int nSort = 200;
		if (ParamParseStream::GetInstance()->IsContainKey(strLabelKey))
		{
			ReadParam* pReadParam = ParamParseStream::GetInstance()->GetReadParamByKey(strLabelKey);
			pParam = dynamic_cast<NodeParamBase*>(pReadParam->pParam);
			nSort = pReadParam->nSort;
		}
		if (pParam)
		{
			pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(*pParam);
			pNode->iLoadModel();
			pNode->iSetSaveMyself(false);
			GetContainerGroup()->iAddChild(pNode);
		}
		if (pNode)
		{
			GetComponent<IRenderNode>(pNode)->iSetRenderSort(nSort);
		}
	}

	ParamNetImage::GetInstance()->AddNetTask(pNode, m_strServerHost);
	return pNode;
}

void SubInstanceLabelFactory::iUpdateClusterLabelNode(INode* pClusterNode, const int nCount)
{
	SetTextShowByStrKey(pClusterNode, "count", to_string(nCount));
}

INode* SubInstanceLabelFactory::iCreateOverTapLabelNode(int nType)
{
	ProjectBasedSceneElementTypeInfo* pTypeInfo = m_pElementLoader->GetElementTypeInfoByID(GetType());
	INode* pReturnNode = nullptr;
	string strOverTapKey = SCENE_ELEMENT_PARAM_KEY + "overtap_";
	strOverTapKey += to_string(nType);
	strOverTapKey += "_";
	strOverTapKey += m_strServerHost;
	std::string strOverTapStyle = pTypeInfo->strOverTapStyle;
	if (!strOverTapStyle.empty())
	{
		if (!ParamParseStream::GetInstance()->IsContainKey(strOverTapKey))
		{
			ParamParseStream::GetInstance()->ReadParamFromString(strOverTapStyle, strOverTapKey);
		}
		NodeParamBase* pParam = NULL;
		int nSort = 200;
		if (ParamParseStream::GetInstance()->IsContainKey(strOverTapKey))
		{
			ReadParam* pReadParam = ParamParseStream::GetInstance()->GetReadParamByKey(strOverTapKey);
			pParam = dynamic_cast<NodeParamBase*>(pReadParam->pParam);
			nSort = pReadParam->nSort;
		}
		BoardParam* pBoardParam = static_cast<BoardParam*>(pParam);
		if (!pBoardParam)
		{
			return pReturnNode;
		}
		pReturnNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(*pBoardParam);
		pReturnNode->iLoadModel();
		pReturnNode->iSetSaveMyself(false);
		GetContainerGroup()->iAddChild(pReturnNode);
		if (pReturnNode)
		{
			GetComponent<IRenderNode>(pReturnNode)->iSetRenderSort(nSort);
		}
	}
	ParamNetImage::GetInstance()->AddNetTask(pReturnNode, m_strServerHost);
	return pReturnNode;
}

void SubInstanceLabelFactory::iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{
	m_pClient->iOnHCLabelClickLabel(pAssemble, this, bSelect);
}

void SubInstanceLabelFactory::iOnSetDetailSelect(FactotryAssembleDetail* pAssemble, std::string strFlagName, bool bSelect)
{
	m_pClient->iOnHCDetailClickLabel(pAssemble, this, strFlagName, bSelect);
}

