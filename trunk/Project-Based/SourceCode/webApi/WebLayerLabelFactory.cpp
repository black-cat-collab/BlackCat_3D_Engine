#include "ProjectBasedHeader.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/webApi/WebLayerLabelFactory.h"
#include "Project-Based/webApi/WebLabelParser.h"
#include "Project-Based/utils/ParamParseStream.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/utils/ParamNetImage.h"
#include "Project-Based/utils/AnimateWithNodeMg.h"
#include "Project-Based/utils/TweenAnimateForNode.h"
#include "Project-Based/base/SceneElementLoader.h"
#include "Project-Based/webApi/IWebLabelFactory.h"

using namespace bc;

WebLayerLabelFactory::WebLayerLabelFactory() :
	IWebLabelFactory(),
	CommonLabelFactory()
{
	m_pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
	setFactory(this);
	SetContainerGroup(m_pClient->iGetApiGroupNode());
}

WebLayerLabelFactory::~WebLayerLabelFactory()
{
}

INode* WebLayerLabelFactory::iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pData)
{
	if (!m_pLabelNodeParam)
	{
		return nullptr;
	}

	ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)pData;
	if (m_pLabelNodeParam->nNodeType == NODE_POLYGON)
	{
		m_bNodeCanReuse = false;		//设置不可复用

		PolygonParam* pParam = (PolygonParam*)m_pLabelNodeParam;
		//多指取最大的z值
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
		for (int i = 0; i < pInfo->vecPoint.size(); i++)
		{
			Vector3d vPos = pInfo->vecPoint[i];
			pParam->vecVertex.push_back(vPos);
		}
	}
	else if (m_pLabelNodeParam->nNodeType == NODE_ROAD)
	{
		m_bNodeCanReuse = false;		//设置不可复用

		RoadParam* pParam = (RoadParam*)m_pLabelNodeParam;
		pParam->vecVertex.push_back(ConvertVecVector3dTo(pInfo->vecPoint));
	}
	else if (m_pLabelNodeParam->nNodeType == NODE_LOCUS_LINE)
	{
		//LocusLineParam* pParam = (LocusLineParam*)m_pLabelNodeParam;
		//float fMaxz = 0;
		//for (int i = 0; i < pInfo->vecPoints.size(); i++)
		//{
		//	if (i == 0)
		//	{
		//		fMaxz = pInfo->vecPoints[i].z;
		//	}
		//	if (pInfo->vecPoints[i].z > fMaxz)
		//	{
		//		fMaxz = pInfo->vecPoints[i].z;
		//	}
		//}
		//vector<Vector3d> vecPos;
		//for (int i = 0; i < pInfo->vecPoints.size(); i++)
		//{
		//	Vector3d vPos = pInfo->vecPoints[i];
		//	vPos.z = fMaxz;
		//	vecPos.push_back(vPos);
		//}
		//if (pParam->vecSegment.size() > 0)
		//{
		//	pParam->vecSegment[0].vecVertex = vecPos;
		//}
	}
	else if (m_pLabelNodeParam->nNodeType == NODE_FLYLINE)
	{
		m_bNodeCanReuse = false;		//设置不可复用

		FlyLineParam* pParam = (FlyLineParam*)m_pLabelNodeParam;
		pParam->vecVertex.clear();
		if (pInfo->vecPoint.size() >= 2)
		{
			Vector3d vCenter = (pInfo->vecPoint[0] + pInfo->vecPoint[1]) / 2.0f;
			ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
			pClient->AdjustZ(vCenter, pInfo->vecPoint[0].Distance(pInfo->vecPoint[1]) / 2, true);
			pParam->vecVertex.push_back(pInfo->vecPoint[1]);
			pParam->vecVertex.push_back(vCenter);
			pParam->vecVertex.push_back(pInfo->vecPoint[0]);
		}
	}

	INode* pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(*m_pLabelNodeParam);
	pNode->iSetVisible(false);
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
	pNode->iSetExternParam((INT_PTR)pInfo);
	pNode->iSetSaveMyself(false);
	GetContainerGroup()->iAddChild(pNode);

	return pNode;
}

void WebLayerLabelFactory::iUpdateDataForLabel(FactoryAssemble* pAssemble)
{
	ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)pAssemble->pData;
	if (pAssemble->pLabelNode)
	{
		IBoard* pIBoard = pAssemble->pLabelNode->GetDynamicComponent<IBoard>();
		if (pIBoard)
		{
			BoardParam* pParam = pIBoard->iGetBoardParam();
			pParam->vModelAngle += pInfo->vAngle;
		}
	}
	setText(pAssemble->pLabelNode, pInfo->j3DTextMatchObject);
}

INode* WebLayerLabelFactory::iCreateNodeForDetail(int nType, ProjectBasedSceneElement* pData)
{
	INode* pNode = nullptr;
	if (m_pDetailNodeParam)
	{
		pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(*m_pDetailNodeParam);
		pNode->iSetVisible(false);
		pNode->iLoadModel();
		pNode->iSetSaveMyself(false);
		GetContainerGroup()->iAddChild(pNode);
	}
	return pNode;
}

void WebLayerLabelFactory::iOpenForDetail(FactotryAssembleDetail* pAssemble)
{
	if (!pAssemble->pSelfNode)
	{
		return;
	}
	ProjectBasedSceneElementTypeInfo* pTypeInfo = m_pClient->m_pSceneElementLoader->GetElementTypeInfoByID(pAssemble->nType);
	if (pTypeInfo)
	{
		pAssemble->vDetailToLabelOffset = Vector2(pTypeInfo->fOffsetDetailX, pTypeInfo->fOffsetDetailY);
	}
	if (!m_jLabelOption.isNull())
	{
		float offsetX = 0.0f;
		float offsetY = 0.0f;
		if (m_jLabelOption.isMember("detailOffsetX"))
		{
			offsetX = m_jLabelOption["detailOffsetX"].asFloat();
		}
		if (m_jLabelOption.isMember("detailOffsetY"))
		{
			offsetY = m_jLabelOption["detailOffsetY"].asFloat();
		}
		pAssemble->vDetailToLabelOffset = Vector2(offsetX, offsetY);
	}

	ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)pAssemble->pData;
	IBoard* pIBoard = pAssemble->pSelfNode->GetDynamicComponent<IBoard>();
	std::vector<INode*> vecVideo;
	pIBoard->iGetVideoBoard(vecVideo);
	if (vecVideo.size() > 0)
	{
		IVideo* pVideo = vecVideo[0]->GetDynamicComponent<IVideo>();
		if (pVideo)
		{
			if (pInfo->bLivegbs)
			{
				pVideo->iStopVideo();
				pVideo->iSetVideoStream(pInfo->strLiveUrl, pInfo->strDeviceID, pInfo->strChannelID);
				pVideo->iPlayVideo();
			}
			else
			{
				if (StartWith(pInfo->strLiveUrl, "resource"))
				{
					//使用本地的地址
					pInfo->strLiveUrl = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir
						+ "/" + pInfo->strLiveUrl;
				}
				pVideo->iSetVideoStream(pInfo->strLiveUrl);
				pVideo->iPlayVideo();
			}
		}
	}
}

void WebLayerLabelFactory::iUpdateDataForDetail(FactotryAssembleDetail* pAssemble)
{
	if (!pAssemble->pSelfNode)
	{
		return;
	}
	ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)pAssemble->pData;
	IBoard* pIBoard = pAssemble->pSelfNode->GetDynamicComponent<IBoard>();
	if (pIBoard)
	{
		setText(pAssemble->pSelfNode, pInfo->j3DTextMatchObject);
	}
	pAssemble->pSelfNode->iSetExternParam((INT_PTR)pAssemble->pData);
	SetINTForNodeWithDetail(pAssemble->pSelfNode);
}

void WebLayerLabelFactory::iCloseForDetail(FactotryAssembleDetail* pAssemble)
{
	if (!pAssemble->pSelfNode)
	{
		return;
	}
	IBoard* pIBoard = pAssemble->pSelfNode->GetDynamicComponent<IBoard>();
	if (pIBoard)
	{
		std::vector<INode*> vecVideo;
		pIBoard->iGetVideoBoard(vecVideo);
		if (vecVideo.size() > 0)
		{
			IVideo* pVideo = vecVideo[0]->GetDynamicComponent<IVideo>();
			if (pVideo)
			{
				pVideo->iStopVideo();
			}
		}
	}
}

//聚合面板
INode* WebLayerLabelFactory::iCreateClusterLabelNode(int nType)
{
	INode* pNode = nullptr;
	if (m_pLabelClusterParam)
	{
		pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(*m_pLabelClusterParam);
		pNode->iSetVisible(false);
		pNode->iLoadModel();
		pNode->iSetSaveMyself(false);
		GetContainerGroup()->iAddChild(pNode);
	}
	ParamNetImage::GetInstance()->AddNetTask(pNode, m_strServerHost);
	return pNode;
}

void WebLayerLabelFactory::iUpdateClusterLabelNode(INode* pClusterNode, const int nCount)
{
	SetTextShowByStrKey(pClusterNode, "count", to_string(nCount));
}

void WebLayerLabelFactory::iSetClusterLabelSelect(INode* pClusterNode, bool bSelect)
{
	Vector3d vOrigin = pClusterNode->iGetOrigin();
	INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
	GoViewPoint(pCameraNode, vOrigin, 0, 500);
}

void WebLayerLabelFactory::iSetSmoothMoveParam(TweenAnimateForNode* pTween)
{
	if (m_jLabelOption.isNull())
	{
		return;
	}
	Json::Value& jOption = GetLabelOption();
	int nSecondToIgnorePoint = 2;
	float fSpeed = 60.0f;
	if (jOption.isMember("nSecondToIgnorePoint"))
	{
		nSecondToIgnorePoint = jOption["nSecondToIgnorePoint"].asInt();
	}
	if (jOption.isMember("fSmoothSpeed"))
	{
		fSpeed = jOption["fSmoothSpeed"].asFloat();
	}
	pTween->SetMin2PointTimeSecond(nSecondToIgnorePoint);
	pTween->SetSpeed(fSpeed);
}

void WebLayerLabelFactory::iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{
	m_pClient->iOnWebLabelClickLabel(pAssemble, this, bSelect);
}

void WebLayerLabelFactory::iOnSetDetailSelect(FactotryAssembleDetail* pAssemble, std::string strFlagName, bool bSelect)
{
	m_pClient->iOnWebDetailClickLabel(pAssemble, this, strFlagName, bSelect);
}

void WebLayerLabelFactory::iDoWebCommond(WebCommandData* pWebData)
{
	CommonLabelFactory::iDoWebCommond(pWebData);
	if (pWebData->strCommand == "Web_DeleteAllElement")
	{
		std::vector<FactoryAssemble*> vecAssemble;
		GetCurAssemble(vecAssemble);
		for (size_t m = 0; m < vecAssemble.size(); m++)
		{
			RemoveByAssemble(vecAssemble[m]);
		}
	}
	else if (pWebData->strCommand == "Web_DeleteAnyElement")
	{
		if (pWebData->vecValues.size() >= 1)
		{
			std::string strId = pWebData->vecValues[0].asString();
			std::vector<FactoryAssemble*> vecAssemble;
			GetCurAssemble(vecAssemble);
			for (size_t m = 0; m < vecAssemble.size(); m++)
			{
				std::string& strTempID = vecAssemble[m]->pData->strUuid;
				if (strTempID == strId)
				{
					RemoveByAssemble(vecAssemble[m]);

					return;
				}
			}
		}
	}
}

