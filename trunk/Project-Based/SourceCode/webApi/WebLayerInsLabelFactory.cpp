#include "ProjectBasedHeader.h"
#include "Project-Based/webApi/IWebLabelFactory.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/label/InstanceLabelFactory.h"
#include "Project-Based/webApi/WebLayerInsLabelFactory.h"
#include "Project-Based/webApi/WebLabelParser.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/utils/ParamNetImage.h"
#include "Project-Based/utils/TweenAnimateForNode.h"

using namespace bc;

WebLayerInsLabelFactory::WebLayerInsLabelFactory():
	InstanceLabelFactory(),
	IWebLabelFactory()
{
	m_pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
	setFactory(this);
	SetContainerGroup(m_pClient->iGetApiGroupNode());
}

WebLayerInsLabelFactory::~WebLayerInsLabelFactory()
{
}

INode* WebLayerInsLabelFactory::iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo)
{
	if (!m_pLabelNodeParam)
	{
		return nullptr;
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

INode* WebLayerInsLabelFactory::iCreateNodeForDetail(int nType, ProjectBasedSceneElement* pInfo)
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

void WebLayerInsLabelFactory::iOpenForDetail(FactotryAssembleDetail* pAssemble)
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

void WebLayerInsLabelFactory::iUpdateDataForDetail(FactotryAssembleDetail* pAssemble)
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

void WebLayerInsLabelFactory::iCloseForDetail(FactotryAssembleDetail* pAssemble)
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

INode* WebLayerInsLabelFactory::iCreateClusterLabelNode(int nType)
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

void WebLayerInsLabelFactory::iUpdateClusterLabelNode(INode* pClusterNode, const int nCount)
{
	SetTextShowByStrKey(pClusterNode, "count", to_string(nCount));
}

void WebLayerInsLabelFactory::iSetClusterLabelSelect(INode* pClusterNode, bool bSelect)
{
	Vector3d vOrigin = pClusterNode->iGetOrigin();
	INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
	GoViewPoint(pCameraNode, vOrigin, 0, 500);
}

void WebLayerInsLabelFactory::iSetSmoothMoveParam(TweenAnimateForNode* pTween)
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

void WebLayerInsLabelFactory::iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect)
{
	m_pClient->iOnWebLabelClickLabel(pAssemble, this, bSelect);
}

void WebLayerInsLabelFactory::iOnSetDetailSelect(FactotryAssembleDetail* pAssemble, std::string strFlagName, bool bSelect)
{
	m_pClient->iOnWebDetailClickLabel(pAssemble, this, strFlagName, bSelect);
}

void WebLayerInsLabelFactory::iDoWebCommond(WebCommandData* pWebData)
{
	InstanceLabelFactory::iDoWebCommond(pWebData);

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

