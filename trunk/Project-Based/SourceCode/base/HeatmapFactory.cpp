#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ProjectLogger.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/base/HeatmapFactory.h"

using namespace bc;

bool HeatmapFactory::CompWeiht(const ExtraHeatmapData &a, const ExtraHeatmapData &b)
{
	return a.nWeight > b.nWeight;
}

HeatmapFactory::HeatmapFactory(ISystemAPI *pSystemAPI)
{
	m_pSystemAPI = pSystemAPI;

	m_bStarted = false;
	m_bOnRequestApi = false;
	m_bRefresh = false;
	m_TheThread = pSystemAPI->iEngineAPI->iGetThreadManager()->RegistThread();
	m_fWightScale = 1.0f;
}


HeatmapFactory::~HeatmapFactory()
{
	Clear();
}

void HeatmapFactory::UninitializeThread()
{
	if (m_bStarted)
	{
		StopRequest();
	}
}

void HeatmapFactory::Clear()
{
	UninitializeThread();

	std::vector<std::string> vec;
	ShowOnlyAreas(vec);

	for (std::vector<HeatmapItemData*>::iterator it = m_vecHeatmapItem.begin(); it != m_vecHeatmapItem.end(); it++)
	{
		HeatmapItemData* pItem = (*it);
		if (pItem->pLocusLine)
		{
			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&pItem->pLocusLine);
		}
		m_vecHeatmapItem.erase(it);
		DELETE_PTR(pItem);
	}
	m_vecHeatmapItem.clear();
}

void HeatmapFactory::StopRequest()
{
	m_bStarted = false;

	if (m_TheThread->IsRunning())
	{
		m_TheThread->RequestInterruption();
		m_TheThread->Wait();
	}
}

void HeatmapFactory::StartRequest()
{
	if (m_bStarted)
	{
		return;
	}
	std::map<std::string, INT_PTR> mapParam;
	mapParam["this"] = (INT_PTR)this;
	m_TheThread->SetParam(TimingGetData, mapParam);
	m_TheThread->Start();
	m_bStarted = true;
}

void HeatmapFactory::Static_Boundary_Callback(HeatmapItemData *pHeatmapItemData,
	std::string strResponse, std::vector<Vector3d> &outVecData)
{
	HeatmapFactory *pFactory = (HeatmapFactory*)pHeatmapItemData->mapINTPTR["factory"];
	if (pFactory)
	{
		pFactory->iParseBoundaryData(pHeatmapItemData, strResponse, outVecData);
	}
}

void HeatmapFactory::requestBoundary(std::vector<HeatmapItemData*>& vecHeatmapItems)
{
	bool bUpdate = false;  
	//请求边界  
	for (int i = 0; i < vecHeatmapItems.size(); i++) 
	{
		if (!vecHeatmapItems[i]->strBoundaryUrl.empty() && vecHeatmapItems[i]->vecBoundaryData.empty())
		{
			requestBoundary(vecHeatmapItems[i]);
			bUpdate = true;
		}
	}
	//加载完
	if (bUpdate)
	{
		iLoadedBoundaryData();
	}
}

void HeatmapFactory::requestBoundary(HeatmapItemData *pItemData)
{
	if (!pItemData)
	{
		return;
	}
	if (pItemData->fnBoundaryUrlRequestCallback == NULL)
	{
		pItemData->fnBoundaryUrlRequestCallback = Static_Boundary_Callback;
	}
	std::string strPath = pItemData->strBoundaryUrl;
	if (strPath.empty())
	{
		return;
	}
	std::string strData = "";
	if (StartWith(strPath, "http://") || StartWith(strPath, "https://"))
	{
		//互联网地址
		ThreadCallbackParam stCallback;
		ThreadRequestParam stParam;
		stParam.eRequestType = THREAD_REQUEST_API_GET;
		stParam.strUrl = pItemData->strBoundaryUrl;
		iInterceptHttpRequest(&stParam);
		ThreadWrapper::GetInstance()->DoNetRequest(stParam, &stCallback);
		if (stCallback.nResult == STATUS_SUCCESS)
		{
			strData = stCallback.strResponse;
		}
	}
	else
	{
		//本地地址
		strData = ReadFile(strPath);	
	}
	if (strData.empty())
	{
		return;
	}
	try
	{
		pItemData->fnBoundaryUrlRequestCallback(pItemData, strData,
			pItemData->vecBoundaryData);
	}
	catch (const std::exception&)
	{

	}
	INode* pLocusLine = pItemData->pLocusLine;
	//创建LocusLine
	if (!pLocusLine)
	{
		LocusLineParam stParam;
		stParam.bDepthTest = false;
		pLocusLine = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(stParam);
		pLocusLine->iLoadModel();
		pLocusLine->iSetSaveMyself(false);
		APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(pLocusLine);
		pItemData->pLocusLine = pLocusLine;
	}

	std::vector<LocusLineSegment> vecData;
	LocusLineSegment stSegment;
	stSegment.vColor = pItemData->vBoundaryColor;
	stSegment.fWidth = pItemData->fBoundaryWidth;
	std::vector<Vector3> vec;
	for (size_t i = 0; i < pItemData->vecBoundaryData.size(); ++i)
	{
		vec.push_back(pItemData->vecBoundaryData[i]);
	}
	stSegment.vecVertex = vec;
	vecData.push_back(stSegment);
	pLocusLine->GetDynamicComponent<ILocusLine>()->iUpdateLocusLineSegment(vecData);

	pLocusLine->iSetVisible(pItemData->bShow);
}

//解析热力图数据
void HeatmapFactory::Static_Heatmap_Callback(HeatmapItemData *pHeatmapItemData,
	std::string strResponse, std::vector<ExtraHeatmapData> &outVecData)
{
	HeatmapFactory *pFactory = (HeatmapFactory*)pHeatmapItemData->mapINTPTR["factory"];
	if (pFactory)
	{
		pFactory->iParseHeatmapData(pHeatmapItemData, strResponse, outVecData);
	}
}

void HeatmapFactory::TimingGetData(std::map<std::string, INT_PTR>& mapParam)
{
	long long lPreStartTime = 0;
	HeatmapFactory* pThis = (HeatmapFactory*)mapParam["this"];
	while (!pThis->m_TheThread->IsInterruptionRequested())
	{
		if (!pThis->m_bStarted)
		{
			break;
		}

		bool bUdpate = false;
		long long lCurTime = GetSystemTime();
		std::vector<HeatmapItemData*> vecItems;
		if (pThis->m_ThMutex.TryLock())
		{
			vecItems = pThis->m_vecHeatmapItem;
			pThis->m_ThMutex.UnLock();
		}
		//vector<HeatmapItemData> vecItems = pThis->m_vecHeatmapItem;
		if (vecItems.empty())
		{
			continue;
		}
		
		for (int nItem = 0; nItem < vecItems.size(); nItem++)
		{
			HeatmapItemData *pInfo = vecItems[nItem];
			if (pInfo->fnHeatmapUrlRequestCallback == NULL)
			{
				pInfo->fnHeatmapUrlRequestCallback = Static_Heatmap_Callback;
			}
			if ((lCurTime - pInfo->lLastIntervalMillisecond) > pInfo->lHeatmapIntervalMillisecond || pThis->m_bOnRequestApi)
			{
				std::string strPath = pInfo->strHeatmapUrl;
				if (strPath.empty())
				{
					continue;
				}
				std::string strData = "";
				if (StartWith(strPath, "http://") || StartWith(strPath, "https://"))
				{
					ThreadCallbackParam stCallback;
					ThreadRequestParam stParam;
					std::string strID;
					Guid::CreateGuId(strID, "HeatmapGetData");
					stParam.eRequestType = THREAD_REQUEST_API_GET;
					stParam.strUrl = pInfo->strHeatmapUrl;
					pThis->iInterceptHttpRequest(&stParam);
					stParam.strID = strID;
					ThreadWrapper::GetInstance()->DoNetRequest(stParam, &stCallback);
					if (stCallback.nResult == STATUS_SUCCESS && !stCallback.strResponse.empty())
					{
						strData = stCallback.strResponse;
					}
				}
				else
				{
					//本地地址
					strData = ReadFile(strPath);
				}
				pInfo->lLastIntervalMillisecond = lCurTime;
				std::vector<ExtraHeatmapData> vecTempData;
				pInfo->fnHeatmapUrlRequestCallback(pInfo, strData, vecTempData);
				//设置区域id
				for (int i = 0; i < vecTempData.size(); i++)
				{
					vecTempData[i].strKey = pInfo->strKey;
				}
				if (vecTempData.size() > 0)
				{
					pInfo->vecExtraHeatmapData = vecTempData;
				}

				pThis->m_bOnRequestApi = false;
				
				bUdpate = true;
			}
		}

		if (bUdpate)
		{
			pThis->m_bRefresh = true;

			pThis->iLoadedData();
		}
	}

	pThis->m_TheThread->RequestInterruption();
}

void HeatmapFactory::FrameUpdate()
{
	if (!m_bRefresh)
	{
		return;
	}
	//进行过滤
	std::vector<HeatmapItemData*> vecTemp;
	if (m_ThMutex.TryLock())
	{
		vecTemp = m_vecHeatmapItem;
		m_ThMutex.UnLock();
	}

	//if (vecTemp.empty())
	//{
	//	return;
	//}
	std::vector<HeatmapItemData*> vecHeatmap;
	GetAllHeatmapItems(vecHeatmap);
	requestBoundary(vecHeatmap);      

	IHeatMap* pHeatMap = static_cast<IHeatMap*>(APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()
		->iGetViewComponent(VIEW_COMPONENT_TYPE_HEAT_MAP));
	RETURN_IF_PTR_IS_NULL(pHeatMap);

	IViewManager* pViewManager = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager();
	pViewManager->iSetCurrentEffectViewVisible(VIEW_TYPE_HEAT_MAP, true);

	pHeatMap->iClearVertex();
	for (int i = 0; i < vecTemp.size(); i++)
	{
		HeatmapItemData *pInfo = vecTemp[i];
		if (pInfo->bShow)
		{
			ProjectLogger::GetInstance()->DebugMessage(pInfo->strKey);
			//设置红色临界值
			pHeatMap->iSetMaxIntensity(pInfo->fIntensityMax);
			//设置点
			std::sort(pInfo->vecExtraHeatmapData.begin(), pInfo->vecExtraHeatmapData.end(), HeatmapFactory::CompWeiht);
			for (int i = 0; i < pInfo->vecExtraHeatmapData.size(); i++)
			{
				bool bAdd = iFilter(&pInfo->vecExtraHeatmapData[i]);
				if (!bAdd)
				{
					continue;
				}
				float fscale = m_fWightScale;
				int weight = (int)(pInfo->vecExtraHeatmapData[i].nWeight*fscale);
				Vector3d vPos = pInfo->vecExtraHeatmapData[i].vPos;
				float fSize = pInfo->fDotSize;
				if (pInfo->vecExtraHeatmapData[i].fSize > 0)
				{
					fSize = pInfo->vecExtraHeatmapData[i].fSize;
				}
				pHeatMap->iAddVertex(vPos.x, vPos.y, fSize, weight);
			}
		}
	}
	pHeatMap->iRefresh();


	vecTemp.clear();
	vecTemp.resize(0);

	m_bRefresh = false;
}

void HeatmapFactory::UpdateAndSetDataChanged(bool bOnRequest)
{
	m_bRefresh = true;
	m_bOnRequestApi = bOnRequest;
	updateShowBoundaryLocus();
}

void HeatmapFactory::ShowOnlyAreas(std::vector<std::string> vecAreaID)
{
	for (int i = 0; i < m_vecHeatmapItem.size(); i++)
	{
		if (find(vecAreaID.begin(), vecAreaID.end(), m_vecHeatmapItem[i]->strKey) != vecAreaID.end())
		{
			m_vecHeatmapItem[i]->bShow = true;
		}
		else
		{
			m_vecHeatmapItem[i]->bShow = false;
		}
	}
	UpdateAndSetDataChanged();
}

void HeatmapFactory::ShowAllAreas()
{
	for (int i = 0; i < m_vecHeatmapItem.size(); i++)
	{
		m_vecHeatmapItem[i]->bShow = true;
	}
	UpdateAndSetDataChanged();
}

void HeatmapFactory::SetAreaVisible(std::string strKey, bool bVisible)
{
	for (int i = 0; i < m_vecHeatmapItem.size(); i++)
	{
		if (m_vecHeatmapItem[i]->strKey == strKey)
		{
			m_vecHeatmapItem[i]->bShow = bVisible;
			break;
		}
	}
	UpdateAndSetDataChanged();
}

void HeatmapFactory::SetAreaVisible(std::vector<std::string> vecKeys, bool bVisible)
{
	for (int i = 0; i < m_vecHeatmapItem.size(); i++)
	{
		if (std::find(vecKeys.begin(), vecKeys.end(), m_vecHeatmapItem[i]->strKey) != vecKeys.end())
		{
			m_vecHeatmapItem[i]->bShow = bVisible;
		}
	}
	UpdateAndSetDataChanged();
}

void HeatmapFactory::AddINTPTR(std::string strKey, INT_PTR pTr)
{
	m_mapINTPTR[strKey] = pTr;
}

INT_PTR HeatmapFactory::GetINTPTR(std::string strKey)
{
	if (m_mapINTPTR.find(strKey) != m_mapINTPTR.end())
	{
		return m_mapINTPTR[strKey];
	}
	return NULL;
}

void HeatmapFactory::updateShowBoundaryLocus()
{
	for (int i = 0; i < m_vecHeatmapItem.size(); i++)
	{
		//创建LocusLine
		bool bFirst = false;
		if (!m_vecHeatmapItem[i]->pLocusLine)
		{
			LocusLineParam stParam;
			stParam.bDepthTest = false;
			INode* pLocusLine = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(stParam);
			pLocusLine->iLoadModel();
			pLocusLine->iSetSaveMyself(false);
			APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(pLocusLine);
			m_vecHeatmapItem[i]->pLocusLine = pLocusLine;
			bFirst = true;
		}
		if (m_vecHeatmapItem[i]->pLocusLine)
		{
			if (m_vecHeatmapItem[i]->bShow || bFirst)
			{
				ILocusLine* pLine = static_cast<ILocusLine*>(m_vecHeatmapItem[i]->pLocusLine->iGetComponent(COMPONENT_TYPE_LOCUS_LINE));
				if (pLine == nullptr) 
				{
					continue;
				}
				std::vector<LocusLineSegment> vecData;
				LocusLineSegment stSegment;
				stSegment.vColor = m_vecHeatmapItem[i]->vBoundaryColor;
				stSegment.fWidth = m_vecHeatmapItem[i]->fBoundaryWidth;
				std::vector<Vector3> vec;
				for (size_t j = 0; j < m_vecHeatmapItem[i]->vecBoundaryData.size(); ++j)
				{
					vec.push_back(m_vecHeatmapItem[i]->vecBoundaryData[j]);
				}
				stSegment.vecVertex = vec;
				vecData.push_back(stSegment);

				pLine->iUpdateLocusLineSegment(vecData);
			}
			m_vecHeatmapItem[i]->pLocusLine->iSetVisible(m_vecHeatmapItem[i]->bShow);
		}
	}
}

void HeatmapFactory::AddHeatmap(HeatmapItemData itemData)
{
	itemData.mapINTPTR["factory"] = (INT_PTR)this;
	HeatmapItemData* pItem = new HeatmapItemData;
	*pItem = itemData;
	m_ThMutex.Lock();
	m_vecHeatmapItem.push_back(pItem);
	m_ThMutex.UnLock();
}

void HeatmapFactory::RemoveHeatmap(std::string strKey)
{
	m_ThMutex.Lock();
	for (std::vector<HeatmapItemData*>::iterator it = m_vecHeatmapItem.begin() ; it != m_vecHeatmapItem.end() ; it++)
	{
		HeatmapItemData* pItem = (*it);
		if (pItem->strKey == strKey)
		{
			if (pItem->pLocusLine)
			{
				APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&pItem->pLocusLine);
			}
			m_vecHeatmapItem.erase(it);
			DELETE_PTR(pItem);
			break;
		}
	}
	m_ThMutex.UnLock();
	UpdateAndSetDataChanged(false);
}

void HeatmapFactory::RemoveAllHeatmap()
{
	m_ThMutex.Lock();
	for (int i = 0; i < m_vecHeatmapItem.size(); ++i)
	{
		if (m_vecHeatmapItem[i]->pLocusLine)
		{
			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&(m_vecHeatmapItem[i]->pLocusLine));
		}
		DELETE_PTR(m_vecHeatmapItem[i]);
	}
	m_vecHeatmapItem.clear();
	m_ThMutex.UnLock();
	UpdateAndSetDataChanged(false);
}

HeatmapItemData* HeatmapFactory::GetHeatmapItem(std::string strKey)
{
	for (int i = 0; i < m_vecHeatmapItem.size(); i++)
	{
		if (m_vecHeatmapItem[i]->strKey == strKey)
		{
			return m_vecHeatmapItem[i];
		}
	}
	return NULL;
}


void HeatmapFactory::RefreshHeatmapItem()
{
	UpdateAndSetDataChanged(false);
}

void HeatmapFactory::GetCurHeatmapItems(std::vector<HeatmapItemData*> &vecHeatmapItems)
{
	for (int i = 0; i < m_vecHeatmapItem.size(); i++)
	{
		if (m_vecHeatmapItem[i]->bShow)
		{
			vecHeatmapItems.push_back(m_vecHeatmapItem[i]);
		}
	}
}

void HeatmapFactory::GetAllHeatmapItems(std::vector<HeatmapItemData*> &vecHeatmapItems)
{
	for (int i = 0; i < m_vecHeatmapItem.size(); i++)
	{
		vecHeatmapItems.push_back(m_vecHeatmapItem[i]);
	}
}

long HeatmapFactory::GetHeatmapIntervalMillisecond()
{
	long lTime = -1;
	for (int i = 0; i < m_vecHeatmapItem.size(); i++)
	{
		if (i == 0)
		{
			lTime = m_vecHeatmapItem[i]->lHeatmapIntervalMillisecond;
		}
		if (m_vecHeatmapItem[i]->lHeatmapIntervalMillisecond > lTime)
		{
			lTime = m_vecHeatmapItem[i]->lHeatmapIntervalMillisecond;
		}
	}
	return lTime;
}

