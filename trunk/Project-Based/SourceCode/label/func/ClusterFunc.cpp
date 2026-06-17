#include "ProjectBasedHeader.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/label/func/IFunc.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/label/func/OverTapFunc.h"
#include "Project-Based/label/func/ClusterFunc.h"
#include "./ClusterFuncFactory.h"
#include "Project-Based/base/ProjectBasedClient.h"

using namespace bc;

bool ClusterFunc::compareByLevel(ClusterLevel& p1, ClusterLevel& p2)
{
	return p1.nLevel < p2.nLevel;
}

ClusterFunc::ClusterFunc():
	IFunc(),
	m_bStarted(false),
	m_bThreadRunning(true),
	m_bNeedRefresh(false),
	m_fClusterLabelZ(0),
	m_pClusterBoardFactory(nullptr),
	m_bOutMode(false)
{
	m_bThreadRealStop.store(true);
	m_pClusterBoardFactory = new ClusterFuncFactory(this);
	m_pClusterBoardFactory->SetAllVisible(true);
}

ClusterFunc::~ClusterFunc()
{
	DELETE_PTR(m_pClusterBoardFactory);
	clear();
}

EventReturnType_e ClusterFunc::iProcessEvent(const BCEvent& tEvent)
{
	return m_pClusterBoardFactory->iProcessEvent(tEvent);
}

void ClusterFunc::AddLevel(ClusterLevel stLevel)
{
	m_vecClusterLevel.push_back(stLevel);
}

void ClusterFunc::SetClusterRange(Vector3d vLeftTop, Vector3d vRightBottom)
{
	//统一使用经纬度来算
	m_vTopLeft = vLeftTop;
	m_vBottomRight = vRightBottom;
	ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);

	{
		Vector3d vScenePos = Vector3d(m_vTopLeft.x, m_vTopLeft.y, m_vTopLeft.z);
		double dLng = 0.0f;
		double dLat = 0.0f;
		pClient->ScenePosToWGS(vScenePos, dLng, dLat);
		m_vTopLeft = Vector3d(dLng, dLat, m_vTopLeft.z);
	}
	{
		Vector3d vScenePos = Vector3d(m_vBottomRight.x, m_vBottomRight.y, m_vBottomRight.z);
		double dLng = 0.0f;
		double dLat = 0.0f;
		pClient->ScenePosToWGS(vScenePos, dLng, dLat);
		m_vBottomRight = Vector3d(dLng, dLat, m_vBottomRight.z);
	}
}

void ClusterFunc::AddFactory(CommonLabelFactory* pFactory)
{
	if (!pFactory)
	{
		return;
	}
	if (find(m_vecFactory.begin(), m_vecFactory.end(), pFactory) == m_vecFactory.end())
	{
		m_vecFactory.push_back(pFactory);
	}
}

void ClusterFunc::RemoveFactory(CommonLabelFactory* pFactory)
{
	for (std::vector<CommonLabelFactory*>::iterator it = m_vecFactory.begin(); it != m_vecFactory.end(); it++)
	{
		if ((*it) == pFactory)
		{
			m_vecFactory.erase(it);
			break;
		}
	}
}

void ClusterFunc::iCreateData(FactoryAssemble* pAssemble)
{
	//一开始给它聚合，不然一开始就会显示然后再隐藏
	pAssemble->bAlreadyCluste = true;
}

void ClusterFunc::iUpdateData(FactoryAssemble* pAssemble)
{
	m_bNeedRefresh = true;
}

void ClusterFunc::iChangeVisible(bool bAllVisible, const std::vector<int>& vecVisibleType)
{
	m_bNeedRefresh = true;
}

void ClusterFunc::iPreFrameUpdate()
{
}

void ClusterFunc::iFrameUpdate(FactoryAssemble* pAssemble)
{
}

void ClusterFunc::iPostFrameUpdate()
{
}

void ClusterFunc::Start()
{
	if (m_bStarted)
	{
		return;
	}
	if (m_vecClusterLevel.empty())
	{
		return;
	}
	sort(m_vecClusterLevel.begin(), m_vecClusterLevel.end(), compareByLevel);
	for (int i = 0; i < m_vecClusterLevel.size(); i++)
	{
		if ((i - 1) >= 0)
		{
			m_vecClusterLevel[i].pLowClusterLevel = &(m_vecClusterLevel[i - 1]);
		}
		if ((i + 1) <= m_vecClusterLevel.size() - 1)
		{
			m_vecClusterLevel[i].pHighClusterLevel = &(m_vecClusterLevel[i + 1]);
		}
	}
	calcLevelData();
	doingCalcTaskTread();

	m_bStarted = true;
}

void ClusterFunc::Stop()
{
	m_bStarted = false;
	m_vecCurLevelData.clear();
	m_bThreadRunning = false;
}

void ClusterFunc::clear()
{
	Stop();
	for (size_t i = 0; i < m_vecCurLevelData.size(); i++)
	{
		DELETE_PTR(m_vecCurLevelData[i]);
	}
	m_vecCurLevelData.clear();
	m_vecFactory.clear();
	m_vecCurLevelData.clear();
}

void ClusterFunc::calcLevelData()
{
	for (size_t i = 0; i < m_vecClusterLevel.size(); i++)
	{
		ClusterLevel* pLevel = &m_vecClusterLevel[i];
		float fWidth = (m_vBottomRight.x - m_vTopLeft.x) / pLevel->nColumn;
		float fHeight = (m_vBottomRight.y - m_vTopLeft.y) / pLevel->nRow;
		pLevel->fGridWidth = fWidth;
		pLevel->fGridHeight = fHeight;

		for (int r = 0; r < pLevel->nRow; r++)
		{
			std::vector<LevelData*> vec;
			for (int l = 0; l < pLevel->nColumn; l++)
			{
				LevelData* stInfo = new LevelData;
				stInfo->nLevel = pLevel->nLevel;
				Vector3d vTopLeft = m_vTopLeft + Vector3d(l * fWidth, fHeight * r, 0);
				Vector3d vBottomRight = m_vTopLeft + Vector3d((l + 1) * fWidth, (r + 1) * fHeight, 0);
				stInfo->vTopLeft = vTopLeft;
				stInfo->vBottomRight = vBottomRight;
				stInfo->vCenter = (vTopLeft + vBottomRight) / 2.0f;
				stInfo->fWidth = fWidth;
				stInfo->fHeight = fHeight;

				vec.push_back(stInfo);
			}
			pLevel->vecRowLevelData.push_back(vec);
		}
	}
}

void ClusterFunc::doingCalcTaskTread()
{
	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_CUSTOM;
	stParam.AddParam("this", (INT_PTR)this);
	stParam.pPreDoRequestFunc = static_request_doingCalcTaskTread;
	stParam.pThreadCompleteFunc = static_callback_doingCalcTaskTread;
	stParam.strID = "doingCalcTaskTread";
	stParam.bRepeat = false;
	stParam.bThread = true;
	stParam.bRunCallbackInMainThread = true;
	ThreadWrapper::GetInstance()->AddTask(stParam);
}

void ClusterFunc::static_request_doingCalcTaskTread(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam)
{
	ClusterFunc* pThis = (ClusterFunc*)mapParams["this"];
	while (pThis->m_bThreadRunning)
	{
		pThis->m_bThreadRealStop.store(false);

		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		pThis->m_MutexLock.Lock();
		std::vector<CommonLabelFactory*> vecFactory = pThis->m_vecFactory; 
		pThis->m_MutexLock.UnLock();

		bool bUpdate = false;
		INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
		if (!pCameraNode)
		{
			continue;
		}
		if (pThis->m_vLastCameraPos.Distance(pCameraNode->iGetOrigin()) > 5.0f)
		{
			bUpdate = true;
		}
		bUpdate = bUpdate || pThis->m_bNeedRefresh;
		if (!bUpdate)
		{
			continue;
		}

		pThis->m_vLastCameraPos = pCameraNode->iGetOrigin();

		for (size_t i = 0; i < pThis->m_vecClusterLevel.size(); i++)
		{
			ClusterLevel* pLevel = &pThis->m_vecClusterLevel[i];
			for (size_t j = 0; j < pLevel->vecRowLevelData.size(); j++)
			{
				for (size_t m = 0; m < pLevel->vecRowLevelData[j].size(); m++)
				{
					pLevel->vecRowLevelData[j][m]->mapCount.clear();
					pLevel->vecRowLevelData[j][m]->vBarycenter = Vector3d(0, 0, 0);
				}
			}
		}

		PageData* pPageData = new PageData;
		for (size_t i = 0; i < vecFactory.size(); i++)
		{
			CalcResult* pResult = new CalcResult;
			pThis->doFactoryCluster(vecFactory[i], true, pResult);
			pPageData->vecResult.push_back(pResult);
		}
		//赋值
		std::vector<LevelData*> vec;
		for (size_t i = 0; i < pThis->m_vecClusterLevel.size(); i++)
		{
			ClusterLevel* pLevel = &pThis->m_vecClusterLevel[i];
			for (size_t j = 0; j < pLevel->vecRowLevelData.size(); j++)
			{
				for (size_t m = 0; m < pLevel->vecRowLevelData[j].size(); m++)
				{
					if (pLevel->vecRowLevelData[j][m]->GetAllCount() > 0)
					{
						pLevel->vecRowLevelData[j][m]->vBarycenter /= pLevel->vecRowLevelData[j][m]->GetAllCount();
						vec.push_back(pLevel->vecRowLevelData[j][m]);
						float fZ = pThis->GetClusterLabelZ();
						if (fZ != 0.0f)
						{
							ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
							pClient->AdjustZ(pLevel->vecRowLevelData[j][m]->vBarycenter, fZ, true);
						}
						pLevel->vecRowLevelData[j][m]->vLabelPos = pLevel->vecRowLevelData[j][m]->vBarycenter;
					}
				}
			}
		}
		for (size_t i = 0; i < vec.size(); i++)
		{
			if (pThis->m_vecCurLevelData.size() <= i)
			{
				LevelData* pTemp = new LevelData;
				pThis->m_vecCurLevelData.push_back(pTemp);
			}
			*pThis->m_vecCurLevelData[i] = *vec[i];
			pPageData->vecLevelData.push_back(pThis->m_vecCurLevelData[i]);
		}

		//给到主线程执行
		ThreadCallbackParam* pCallbackParam = new ThreadCallbackParam;
		pCallbackParam->sRequestParam = pParam->sRequestParam;
		pCallbackParam->sRequestParam.bRepeat = false;
		Guid::CreateGuId(pCallbackParam->sRequestParam.strID, "Thread_");
		pCallbackParam->sRequestParam.AddParam("pageData", (INT_PTR)pPageData);
		ThreadWrapper::GetInstance()->AddMainRunCallback(pCallbackParam);

		pThis->m_bNeedRefresh = false;
	}
}

void ClusterFunc::static_callback_doingCalcTaskTread(ThreadCallbackParam_s* pParam)
{
	ClusterFunc* pThis = (ClusterFunc*)pParam->sRequestParam.GetParam("this");
	PageData* pPageData = (PageData*)pParam->sRequestParam.GetParam("pageData");
	if (!pPageData)
	{
		pThis->m_bThreadRealStop = true;
		return;
	}
	std::vector<CalcResult*> vecResult;
	for (size_t i = 0; i < pPageData->vecResult.size(); i++)
	{
		vecResult.push_back(pPageData->vecResult[i]);
		if (pPageData->vecResult[i]->pOverTapResult)
		{
			vecResult.push_back(pPageData->vecResult[i]->pOverTapResult);
		}
	}
	for (size_t i = 0; i < vecResult.size(); i++)
	{
		CalcResult* pResult = vecResult[i];
		for (size_t j = 0; j < pResult->vecShowAssemble.size(); j++)
		{
			pResult->vecShowAssemble[j]->bAlreadyCluste = false;
		}
		pResult->pFactory->RefreshClusterVisible(pResult->vecShowAssemble);
		for (size_t j = 0; j < pResult->vecHideAssemble.size(); j++)
		{
			pResult->vecHideAssemble[j]->bAlreadyCluste = true;
		}
		pResult->pFactory->RefreshClusterVisible(pResult->vecHideAssemble);
	}

	//创建聚合标签
	if (pThis->m_pClusterBoardFactory)
	{
		pThis->m_pClusterBoardFactory->RefreshData((std::vector<ProjectBasedSceneElement*>*)&pPageData->vecLevelData);
	}

	DELETE_PTR(pPageData);
}

void ClusterFunc::doFactoryCluster(CommonLabelFactory* pFactory, bool bAddCount, CalcResult* pCalcResult)
{
	if (!pFactory)
	{
		return;
	}
	pCalcResult->pFactory = pFactory;
	INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
	if (pFactory->IsNoTypeVisible())
	{
		return;
	}
	ICamera* pCamera = pCameraNode->GetDynamicComponent<ICamera>();
	std::vector<FactoryAssemble*> vecAllAssemble;
	pFactory->GetCurAssemble(vecAllAssemble);
	for (size_t m = 0; m < vecAllAssemble.size(); m++)
	{
		if (vecAllAssemble[m]->bModify.load())
		{
			continue;
		}
		Vector3d& vPos = vecAllAssemble[m]->pData->vLabelPos;
		bool bShow = checkShowInLevel(pFactory, pCameraNode, vPos, bAddCount);
		if (bShow)
		{
			if (vecAllAssemble[m]->bAlreadyCluste)
			{
				//聚合状态才添加
				pCalcResult->vecShowAssemble.push_back(vecAllAssemble[m]);
			}
		}
		else
		{
			if (!vecAllAssemble[m]->bAlreadyCluste)
			{
				//聚合状态才添加
				pCalcResult->vecHideAssemble.push_back(vecAllAssemble[m]);
			}
		}
	}
	OverTapFunc* pOverTapFunc = (OverTapFunc*)(pFactory->GetFunc(FUNC_OVERTAP));
	if (pOverTapFunc)
	{
		pCalcResult->pOverTapResult = new CalcResult;
		doFactoryCluster(pOverTapFunc->GetOverTapLabelFactory(), false, pCalcResult->pOverTapResult);
	}
}

bool ClusterFunc::checkShowInLevel(CommonLabelFactory* pFactyory, INode* pCameraNode, Vector3d& vPos, bool bAddCount)
{
	Vector3d vRealPos = vPos;
	ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);

	Vector3d vScenePos = Vector3d(vRealPos.x, vRealPos.y, vRealPos.z);
	double dLng = 0.0f;
	double dLat = 0.0f;
	pClient->ScenePosToWGS(vScenePos, dLng, dLat);
	vRealPos = Vector3d(dLng, dLat, m_vTopLeft.z);

	for (size_t i = 0; i < m_vecClusterLevel.size(); i++)
	{
		ClusterLevel* pLevel = &m_vecClusterLevel[i];
		int nRow = (int)((vRealPos.y - m_vTopLeft.y) / pLevel->fGridHeight);
		int nColumn = (int)((vRealPos.x - m_vTopLeft.x) / pLevel->fGridWidth);
		LevelData* pInfo = nullptr;
		if (nRow >= 0 && nColumn >= 0 && nRow <= (pLevel->nRow - 1) && nColumn <= (pLevel->nColumn - 1))
		{
			pInfo = pLevel->vecRowLevelData[nRow][nColumn];
		}
		if (!pInfo)
		{
			continue;
		}
		Vector3d vRealCenter = pInfo->vCenter;
		ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
		Vector3d vTemp = Vector3d::ZERO;
		//是经纬度
		pClient->ConvertLngToVector3(vRealCenter.x, vRealCenter.y, &vTemp, false);
		vRealCenter = vTemp;
		float fDistance = pCameraNode->iGetOrigin().Distance(vRealCenter);
		bool bMatch = false;
		if (pLevel->pHighClusterLevel)
		{
			//中间
			bMatch = (fDistance >= pLevel->fDistance) && (fDistance < pLevel->pHighClusterLevel->fDistance);
		}
		else
		{
			//最外层
			bMatch = (fDistance >= pLevel->fDistance);
		}
		if (bMatch)
		{
			if (bAddCount)
			{
				pInfo->vBarycenter += vPos;
				pInfo->mapCount[pFactyory] ++;
			}
			return false;
		}
		else if (!pLevel->pLowClusterLevel && fDistance < pLevel->fDistance)
		{
			//最底层但未找到则显示标签
			return true;
		}
		else if (!pLevel->pHighClusterLevel)
		{
			pInfo->vBarycenter += vPos;
			pInfo->mapCount[pFactyory] ++;
			return false;
		}
	}
	return false;
}

CommonLabelFactory* ClusterFunc::GetClusterLabelFactory()
{
	return m_pClusterBoardFactory;
}

bool ClusterFunc::IsInRange(Vector3d& vPos)
{
	double dLng = 0.0f;
	double dLat = 0.0f;
	ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
	pClient->ScenePosToWGS(vPos, dLng, dLat);

	float fMinX = min(m_vTopLeft.x,m_vBottomRight.x);
	float fMinY = min(m_vTopLeft.y, m_vBottomRight.y);
	float fMaxX = max(m_vTopLeft.x, m_vBottomRight.x);
	float fMaxY = max(m_vTopLeft.y, m_vBottomRight.y);

	if (dLng >= fMinX && dLng <= fMaxX
		&& dLat >= fMinY && dLat <= fMaxY)
	{
		return true;
	}
	return false;
}

bool ClusterFunc::isAllStop()
{
	return m_bThreadRealStop.load();
}

void ClusterFunc::ClearLevel()
{
	m_vecClusterLevel.clear();
}

