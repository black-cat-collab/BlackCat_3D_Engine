#include "ProjectBasedHeader.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/Factory/SubLabelFactory.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/utils/ParamParseStream.h"
#include "Project-Based/utils/ParamNetImage.h"
#include "Project-Based/utils/AnimateWithNodeMg.h"
#include "Project-Based/utils/TweenAnimateForNode.h"
#include "Project-Based/base/SceneElementLoader.h"
#include "Project-Based/label/func/ClusterFunc.h"

using namespace bc;

SubLabelFactory::SubLabelFactory(SceneElementLoader* pLoader) :
	CommonLabelFactory(),
	m_pElementLoader(pLoader)
{

}

SubLabelFactory::~SubLabelFactory()
{
	for (map<string, vector<INode*>>::iterator it = m_mapExtraNode.begin(); it != m_mapExtraNode.end(); ++it)
	{
		for (size_t i = 0; i < it->second.size(); ++i)
		{
			ParamNetImage::GetInstance()->RemoveNetTask(it->second[i]);
			AnimateWithNodeMg::GetInstance()->RemoveAnimate(it->second[i]);
			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&it->second[i]);
		}
		it->second.clear();
	}
	m_mapExtraNode.clear();
}

INode* SubLabelFactory::iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pData)
{
	if (!pData)
	{
		return NULL;
	}
	ProjectBasedSceneElement* pInfo = (ProjectBasedSceneElement*)pData;
	ProjectBasedSceneElementTypeInfo* pTypeInfo = m_pElementLoader->GetElementTypeInfoByID(nType);
	if (!pTypeInfo)
	{
		return nullptr;
	}
	vector<INode*> vecNode;
	INode* pReturnNode = NULL;
	string strLabelKey = SCENE_ELEMENT_PARAM_KEY + "label_";
	strLabelKey += to_string(nType);
	strLabelKey += "_";
	strLabelKey += m_strServerHost;
	std::vector<std::string> vecStrLabelStyle = pTypeInfo->vecStrLabelStyle;
	if (!vecStrLabelStyle.empty())
	{
		if (!ParamParseStream::GetInstance()->IsContainKey(strLabelKey))
		{
			ParamParseStream::GetInstance()->ReadParamFromString(vecStrLabelStyle[0], strLabelKey);
		}
		NodeParamBase* pParam = NULL;
		int nSort = 200;
		if (ParamParseStream::GetInstance()->IsContainKey(strLabelKey))
		{
			ReadParam* pReadParam = ParamParseStream::GetInstance()->GetReadParamByKey(strLabelKey);
			pParam = dynamic_cast<NodeParamBase*>(pReadParam->pParam);
			nSort = pReadParam->nSort;
		}
		BoardParam* pBoardParam = static_cast<BoardParam*>(pParam);
		INode* pNode = createBoard(vecStrLabelStyle[0], pInfo, pBoardParam);
		if (pNode)
		{
			GetComponent<IRenderNode>(pNode)->iSetRenderSort(nSort);
		}
		pReturnNode = pNode;
	}

	return pReturnNode;
}

void SubLabelFactory::iUpdateDataForLabel(FactoryAssemble* pAssemble)
{
	//创建额外的
	createExtraNode(pAssemble->nType,pAssemble->pData);
	if (!pAssemble->pLabelNode)
	{
		return;
	}
	ProjectBasedSceneElement* pInfo = (ProjectBasedSceneElement*)pAssemble->pData;
	if (pInfo->strTitle.empty())
	{
		IBoard* pBoard = GetComponent<IBoard>(pAssemble->pLabelNode);
		if (pBoard)
		{
			std::vector<TextShow*> vecTextShow;
			FindAllTextShow(pAssemble->pLabelNode, vecTextShow);
			for (size_t i = 0; i < vecTextShow.size(); i++)
			{
				SetTextShowByStrKey(pAssemble->pLabelNode, vecTextShow[i]->strID, vecTextShow[i]->strText, true);
			}
		}
	}
	else if(pAssemble->pLabelNode)
	{
		SetTextShowByStrKey(pAssemble->pLabelNode, "title", pInfo->strTitle);
	}
	ProjectBasedSceneElementTypeInfo* pTypeInfo = m_pElementLoader->GetElementTypeInfoByID(pInfo->nTypeID);
	m_pElementLoader->iOnSetLabelData(pAssemble->pLabelNode, pTypeInfo, pInfo);
}

void SubLabelFactory::createExtraNode(int nType, ProjectBasedSceneElement* pInfo)
{
	if (!pInfo)
	{
		return;
	}
	ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
	ProjectBasedSceneElementTypeInfo* pTypeInfo = m_pElementLoader->GetElementTypeInfoByID(pInfo->nTypeID);
	if (!pTypeInfo)
	{
		return;
	}
	pInfo->vecExtraNode.clear();
	for (int i = 1; i < pTypeInfo->vecStrLabelStyle.size(); i++)
	{
		//循环创建
		std::string strLabelKey = SCENE_ELEMENT_PARAM_KEY + "label_extra_";
		strLabelKey += to_string(nType);
		strLabelKey += "_";
		strLabelKey += to_string(i);
		strLabelKey += "_";
		strLabelKey += m_strServerHost;

		string strStyle = pTypeInfo->vecStrLabelStyle[i];
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
		if (!pParam)
		{
			continue;
		}
		INode* pNode = NULL;
		if (pParam->nNodeType == NODE_POLYGON)
		{
			pNode = createPolygon(strStyle, pInfo, pParam);
		}
		else if (pParam->nNodeType == NODE_ROAD)
		{
			pNode = createRoad(strStyle, pInfo, pParam);
		}
		else if (pParam->nNodeType == NODE_LOCUS_LINE)
		{
			pNode = createLocusLine(strStyle, pInfo, pParam);
		}
		else if (pParam->nNodeType == NODE_FLYLINE)
		{
			pInfo->vecExtraNode = createFlyLine(strStyle, pInfo, pParam, nSort);
		}
		else if (pParam->nNodeType == NODE_MULTI_FLYLINE)
		{
			pNode = createMultiFlyLine(strStyle, pInfo, pParam, nSort);
		}
		else
		{
			pNode = createBoard(strStyle, pInfo, pParam);
			if (pNode)
			{
				pNode->iSetOrigin(pInfo->vLabelPos);
				pNode->iSetAngles(pInfo->vAngle);
			}
		}
		if (pNode && pParam->nNodeType != NODE_FLYLINE)
		{
			ParamNetImage::GetInstance()->AddNetTask(pNode, m_strServerHost);
			GetComponent<IRenderNode>(pNode)->iSetRenderSort(nSort);
			pInfo->vecExtraNode.push_back(pNode);
		}
		if (pNode)
		{
			pNode->iSetExternParam((INT_PTR)pInfo);
		}
	}

	if (!pInfo->strNodePath.empty())
	{
		INode* pNode = createGeoNode("", pInfo);
		pNode->iSetExternParam((INT_PTR)pInfo);
		pInfo->vecExtraNode.push_back(pNode);
	}

	std::vector<INode*> vecPreExtraNode = m_mapExtraNode[pInfo->strUuid];
	if (pInfo->vecExtraNode.size() > 0)
	{
		
		if (m_mapExtraNode.find(pInfo->strUuid) == m_mapExtraNode.end()) 
		{
			m_mapExtraNode.insert({ pInfo->strUuid, pInfo->vecExtraNode });
		}
		else 
		{
			m_mapExtraNode[pInfo->strUuid] = pInfo->vecExtraNode;
		}
	}
	for (size_t i = 0; i < vecPreExtraNode.size(); ++i)
	{
		ParamNetImage::GetInstance()->RemoveNetTask(vecPreExtraNode[i]);
		AnimateWithNodeMg::GetInstance()->RemoveAnimate(vecPreExtraNode[i]);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&vecPreExtraNode[i]);
	}
}

INode* SubLabelFactory::createBoard(std::string strStyle, ProjectBasedSceneElement* pInfo, NodeParamBase* pParam)
{
	if (!pInfo)
	{
		return NULL;
	}

	INode* pNode = NULL;
	ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI; 
	if (pParam)
	{
		m_pElementLoader->iPreCreateSetParam(pInfo->nTypeID, pParam);
		pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(*pParam); 
		bool bNetLoadModel = ParamNetImage::GetInstance()->CheckNetLoadModel(pNode);
		if (!bNetLoadModel)
		{
			pNode->iLoadModel();
		}
		else
		{
			//模型则添加到队列进行下载检测，所以暂不加载Model
			ParamNetImage::GetInstance()->AddModelTask(pNode, m_strServerHost);
		}
		pNode->iSetSaveMyself(false);
		GetContainerGroup()->iAddChild(pNode);

		SetINTForNodeWithLabel(pNode);
	}
	return pNode;
}

INode* SubLabelFactory::createPolygon(string strStyle, ProjectBasedSceneElement* pInfo, NodeParamBase* pParam)
{
	if (!pInfo)
	{
		return nullptr;
	}
	ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
	PolygonParam tParam;
	if (pParam)
	{
		PolygonParam* pp = (PolygonParam*)pParam;
		tParam = *pp;
		//多指取最大的z值
		float fMaxz = -1;
		for (int i = 0; i < pInfo->vecPoint.size(); i++)
		{
			if (i == 0)
			{
				fMaxz = pInfo->vecPoint[i].z;
			}
			if (pInfo->vecPoint[i].z > fMaxz)
			{
				fMaxz = pInfo->vecPoint[i].z;
			}
		}
		ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
		for (int i = 0; i < pInfo->vecPoint.size(); i++)
		{
			Vector3 vPos = pInfo->vecPoint[i];
			vPos.z = fMaxz;
			tParam.vecVertex.push_back(vPos);
		}
		m_pElementLoader->iPreCreateSetParam(pInfo->nTypeID, &tParam);
		INode* pPolygon = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(tParam);
		pPolygon->iSetVisible(false);
		pPolygon->GetDynamicComponent<IRenderNode>()->iSetClickEnable(tParam.bClickEnable);
		pPolygon->GetDynamicComponent<IRenderNode>()->iSetHoverEnable(tParam.bHoverEnable);
		pPolygon->iLoadModel();
		pPolygon->iSetFlagName(pInfo->strUuid.c_str());
		pPolygon->iSetSaveMyself(false);
		GetContainerGroup()->iAddChild(pPolygon);
		ParamNetImage::GetInstance()->AddNetTask(pPolygon, m_strServerHost);

		return pPolygon;
	}
	return nullptr;
}

INode* SubLabelFactory::createGeoNode(string strStyle, ProjectBasedSceneElement* pInfo)
{
	if (!pInfo)
	{
		return NULL;
	}
	static int nnID = 0;
	nnID++;

	INode* pExtraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(pInfo->strNodePath);
	if (!pExtraNode)
	{
		NodeParamBase tParam;
		tParam.nNodeType = NODE_GEO;
		INode* pGeoNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(tParam);
		pGeoNode->GetDynamicComponent<IRenderNode>()->iSetModelFile(pInfo->strNodePath.c_str());
		pGeoNode->iSetOrigin(Vector3d(pInfo->vLabelPos));
		pGeoNode->iLoadModel();
		pGeoNode->iSetSaveMyself(false);
		string str = pInfo->strNodePath + to_string(nnID);
		pGeoNode->iSetFlagName(str.c_str());
		GetContainerGroup()->iAddChild(pGeoNode);

		pExtraNode = pGeoNode;
	}
	return pExtraNode;
}

INode* SubLabelFactory::createRoad(string strStyle, ProjectBasedSceneElement* pInfo, NodeParamBase* pParam)
{
	if (!pInfo)
	{
		return nullptr;
	}
	RoadParam tParam;
	ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
	if (pParam)
	{
		// TODO
		string strPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strResourceDir;
		strPath += "/road/";
		Mkdirs(strPath.c_str());

		string strTemp = pInfo->strPos;
		if (strTemp.empty())
		{
			strTemp = pInfo->strLngLats;
		}
		strTemp += strStyle;
		MD5 iMD5;
		iMD5.GenerateMD5((unsigned char*)strTemp.data(), strTemp.length());
		const char* pResult = iMD5.ToString();
		string strFlag = pResult;
		vector<FileInfo> vecFile;
		GetFilesFromDir(strPath, vecFile, false);
		for (int i = 0; i < vecFile.size(); i++)
		{
			//判断有没更新
#ifdef _WIN32			
			string strOldName = vecFile[i].tInfo.name;
#else
	        string strOldName = vecFile[i].tInfo->d_name;
#endif	
			if (strOldName.find(pInfo->strUuid) != string::npos)
			{
				//找到文件,在比较是否需要更新
				if (strOldName.find(strFlag) == string::npos)
				{
					//删除
					string strOldPath = strPath + strOldName;
					remove(strOldPath.c_str());
					break;
				}
			}
		}
		string strName = pInfo->strUuid + "_" + strFlag + ".rd";
		string strFile = strPath + strName;
		vector<Vector3> vecPos;
		for (int i = 0; i < pInfo->vecPoint.size(); i++)
		{
			vecPos.push_back(pInfo->vecPoint[i]);
		}
		vector<vector<Vector3>> vecRoad;
		vecRoad.push_back(vecPos);
		RoadParam* pp = (RoadParam*)pParam;
		tParam = *pp;
		//tParam.strModelName = strFile;
		tParam.vecVertex = vecRoad;
		m_pElementLoader->iPreCreateSetParam(pInfo->nTypeID, &tParam);
		INode* pRoad = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(tParam);
		pRoad->iSetVisible(true);
		pRoad->iLoadModel();
		pRoad->iSetSaveMyself(false);
		GetContainerGroup()->iAddChild(pRoad);
		ParamNetImage::GetInstance()->AddNetTask(pRoad, m_strServerHost);
		GetComponent<IRenderNode>(pRoad)->iSetModelFile(tParam.strModelName);

		return pRoad;
	}
	return nullptr;
}

INode* SubLabelFactory::createLocusLine(string strStyle, ProjectBasedSceneElement* pInfo, NodeParamBase* pParam)
{
	if (!pInfo)
	{
		return nullptr;
	}
	ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
	LocusLineParam tParam;
	if (pParam)
	{
		LocusLineParam* pp = (LocusLineParam*)pParam;
		tParam = *pp;
		float fMaxz = 0;
		for (int i = 0; i < pInfo->vecPoint.size(); i++)
		{
			if (i == 0)
			{
				fMaxz = pInfo->vecPoint[i].z;
			}
			if (pInfo->vecPoint[i].z > fMaxz)
			{
				fMaxz = pInfo->vecPoint[i].z;
			}
		}
		vector<Vector3> vecPos;
		ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
		for (int i = 0; i < pInfo->vecPoint.size(); i++)
		{
			Vector3 vPos = pInfo->vecPoint[i];
			//if (!pClient->m_bPointGisConvert)
			//{
			//	vPos.z = fMaxz;
			//}
			vecPos.push_back(vPos);
		}
		tParam.stDefaultSegment.vecVertex = vecPos;
		m_pElementLoader->iPreCreateSetParam(pInfo->nTypeID, &tParam);
		INode* pLocusLine = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(tParam);
		pLocusLine->iSetVisible(false);
		pLocusLine->iLoadModel();
		pLocusLine->iSetFlagName(pInfo->strUuid.c_str());
		pLocusLine->GetDynamicComponent<ILocusLine>()->iAddLocusLineSegment(tParam.stDefaultSegment);
		pLocusLine->iSetSaveMyself(false);
		GetContainerGroup()->iAddChild(pLocusLine);

		return pLocusLine;
	}
	return nullptr;
}

std::vector<INode*> SubLabelFactory::createFlyLine(std::string strStyle, ProjectBasedSceneElement* pInfo, NodeParamBase* pParam, int nSort)
{
	std::vector<INode*> vecFlyLine;
	if (!pInfo)
	{
		return vecFlyLine;
	}
	FlyLineParam tParam;
	ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
	if (pParam)
	{
		FlyLineParam* pp = (FlyLineParam*)pParam;
		tParam = *pp;
		for (int i = 0; i < pInfo->vecPoint.size(); i++)
		{
			if (i > 0)
			{
				Vector3d vCenter = (pInfo->vecPoint[0] + pInfo->vecPoint[1]) / 2.0f;
				ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
				pClient->AdjustZ(vCenter, pInfo->vecPoint[0].Distance(pInfo->vecPoint[1]) / 2, true);
				tParam.vecVertex.push_back(pInfo->vecPoint[1]);
				tParam.vecVertex.push_back(vCenter);
				tParam.vecVertex.push_back(pInfo->vecPoint[0]);

				m_pElementLoader->iPreCreateSetParam(pInfo->nTypeID, &tParam);
				INode* pFlyLine = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(tParam);
				pFlyLine->iSetVisible(false);
				pFlyLine->iLoadModel();
				pFlyLine->iSetFlagName(pInfo->strUuid.c_str());
				GetComponent<IRenderNode>(pFlyLine)->iSetRenderSort(nSort);
				pFlyLine->iSetSaveMyself(false);
				GetContainerGroup()->iAddChild(pFlyLine);
				ParamNetImage::GetInstance()->AddNetTask(pFlyLine, m_strServerHost);

				vecFlyLine.push_back(pFlyLine);
			}

		}

	}
	return vecFlyLine;
}

INode* SubLabelFactory::createMultiFlyLine(std::string strStyle, ProjectBasedSceneElement* pInfo, NodeParamBase* pParam, int nSort)
{
	INode* pNode = nullptr;
	if (!pInfo)
	{
		return pNode;
	}
	if (pInfo->vecPoint.size() < 2)
	{
		return pNode;
	}
	MultiFlyLineParam tParam;
	ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
	if (pParam)
	{
		MultiFlyLineParam* pp = (MultiFlyLineParam*)pParam;
		tParam = *pp;
		std::vector<Vector3d> vec = pInfo->vecPoint;
		for (size_t i = 0; i < vec.size(); ++i)
		{
			if (i == 0)
			{
				tParam.vecVertex.push_back(vec[i]);
			}
			else
			{
				Vector3d vPos = vec[i];
				Vector3d vPrePos = vec[i - 1];
				Vector3d vCenter = (vPos + vPrePos) / 2.0f;
				pClient->AdjustZ(vCenter, vPos.Distance(vPrePos) / 2, true);
				tParam.vecVertex.push_back(vCenter);
				tParam.vecVertex.push_back(vPos);
			}
		}
		m_pElementLoader->iPreCreateSetParam(pInfo->nTypeID, &tParam);
		INode* pFlyLine = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(tParam);
		pFlyLine->iSetVisible(false);
		pFlyLine->iLoadModel();
		pFlyLine->iSetFlagName(pInfo->strUuid.c_str());
		GetComponent<IRenderNode>(pFlyLine)->iSetRenderSort(nSort);
		pFlyLine->iSetSaveMyself(false);
		GetContainerGroup()->iAddChild(pFlyLine);
		ParamNetImage::GetInstance()->AddNetTask(pFlyLine, m_strServerHost);

		pNode = pFlyLine;
	}
	return pNode;
}

void SubLabelFactory::iShowAssembleForLabel(FactoryAssemble* pAssemble)
{
	ProjectBasedSceneElement* pInfo = (ProjectBasedSceneElement*)pAssemble->pData;
	INode* pBoard = pAssemble->pLabelNode;
	if (pBoard)
	{
		IBoard* pIBoard = pBoard->GetDynamicComponent<IBoard>();
		if (pIBoard)
		{
			BoardParam* pParam = pIBoard->iGetBoardParam();
			if (pParam && pParam->fWidth <= 0)
			{
				pBoard->iSetVisible(false);
			}
		}		
	}
	for (int i = 0; i < pInfo->vecExtraNode.size(); i++)
	{
		pInfo->vecExtraNode[i]->iSetVisible(true);
	}
}

void SubLabelFactory::iHideAssembleForLabel(FactoryAssemble* pAssemble)
{
	ProjectBasedSceneElement* pInfo = (ProjectBasedSceneElement*)pAssemble->pData;
	for (int i = 0; i < pInfo->vecExtraNode.size(); i++)
	{
		pInfo->vecExtraNode[i]->iSetVisible(false);
	}
}


INode* SubLabelFactory::iCreateNodeForDetail(int nType, ProjectBasedSceneElement* pData)
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

void SubLabelFactory::iOpenForDetail(FactotryAssembleDetail* pAssemble)
{
	ProjectBasedSceneElement* pInfo = (ProjectBasedSceneElement*)pAssemble->pData;
	ProjectBasedSceneElementTypeInfo* pTypeInfo = m_pElementLoader->GetElementTypeInfoByID(pInfo->nTypeID);
	pAssemble->vDetailToLabelOffset = Vector2(pTypeInfo->fOffsetDetailX, pTypeInfo->fOffsetDetailY);
	m_pElementLoader->iOnOpenDetail(pAssemble->pSelfNode, pTypeInfo, pInfo);
}

void SubLabelFactory::iUpdateDataForDetail(FactotryAssembleDetail* pAssemble)
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

void SubLabelFactory::iCloseForDetail(FactotryAssembleDetail* pAssemble)
{
	ProjectBasedSceneElement* pInfo = (ProjectBasedSceneElement*)pAssemble->pData;
	ProjectBasedSceneElementTypeInfo* pTypeInfo = m_pElementLoader->GetElementTypeInfoByID(pInfo->nTypeID);
	m_pElementLoader->iOnCloseDetail(pAssemble->pSelfNode, pTypeInfo, pInfo);
}

void SubLabelFactory::iSetClusterParam(CommonLabelFactory* pFactory, ClusterFunc* pClusterFunc)
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

INode* SubLabelFactory::iCreateClusterLabelNode(int nType)
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
			FontBoardParam* pFBoardParam = (FontBoardParam*)pParam;
			for (size_t i = 0; i < pFBoardParam->vecText.size(); ++i)
			{
				pFBoardParam->vecText[i].strID = "count";
			}
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

void SubLabelFactory::iUpdateClusterLabelNode(INode* pClusterNode, const int nCount)
{
	SetTextShowByStrKey(pClusterNode, "count", to_string(nCount));
}

INode* SubLabelFactory::iCreateOverTapLabelNode(int nType)
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

void SubLabelFactory::iSetSmoothMoveParam(TweenAnimateForNode* pTween)
{
	ProjectBasedSceneElementTypeInfo* pTypeInfo = m_pElementLoader->GetElementTypeInfoByID(GetType());
	pTween->SetMin2PointTimeSecond(pTypeInfo->Get3DOption_SeondToIgnorePoint());
	pTween->SetSpeed(pTypeInfo->Get3DOption_Smooth_Speed());
}

void SubLabelFactory::iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{
	m_pClient->iOnHCLabelClickLabel(pAssemble, this,bSelect);
}

void SubLabelFactory::iOnSetDetailSelect(FactotryAssembleDetail* pAssemble, std::string strFlagName, bool bSelect)
{
	m_pClient->iOnHCDetailClickLabel(pAssemble, this,strFlagName, bSelect);
}

void SubLabelFactory::RefreshExtraNode(std::vector<ProjectBasedSceneElement*>& vecData)
{
	for (size_t i = 0; i < vecData.size(); ++i)
	{
		ProjectBasedSceneElement* pInfo = vecData[i];
		SubLabelFactory::createExtraNode(pInfo->nTypeID, pInfo);
	}
}


