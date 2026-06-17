#include "ProjectBasedHeader.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/label/func/IFunc.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/label/func/OverTapFunc.h"
#include "./OverTapFuncFactory.h"
#include "Project-Based/base/ProjectBasedClient.h"

using namespace bc;


OverTapFunc::OverTapFunc(CommonLabelFactory* pComFactory):
	IFunc(),
	m_pComFactory(pComFactory),
	m_bNeedRefresh(false),
	m_bStarted(false),
	m_bThreadRunning(true),
	m_fOverTapJudgeDis(2.0f),
	m_pLabelFactory(nullptr),
	m_nCurOverTapDataIndex(0)
{
	m_bThreadRealStop.store(true);
	m_pLabelFactory = new OverTapFuncFactory(this);
	m_pLabelFactory->AddFunc(FUNC_READ_HRIGHT_TIF);
	m_pLabelFactory->SetAllVisible(pComFactory->IsAllVisible());
	std::vector<int> vec = pComFactory->GetCurVisibleType();
	m_pLabelFactory->SetTypeVisible(vec);
}

OverTapFunc::~OverTapFunc()
{
	DELETE_PTR(m_pLabelFactory);
	clear();
}

EventReturnType_e OverTapFunc::iProcessEvent(const BCEvent& tEvent)
{
	m_pLabelFactory->iProcessEvent(tEvent);

	return EventReturnType_e::NONE;
}

void OverTapFunc::iCreateData(FactoryAssemble* pAssemble)
{
	//一开始给它重叠设置false，而后在计算重叠
	pAssemble->eOverTapState = OVERTAP_NONE;
}

void OverTapFunc::iUpdateData(FactoryAssemble* pAssemble)
{
	m_bNeedRefresh = true;
}

void OverTapFunc::iPreFrameUpdate()
{
	m_pLabelFactory->FrameUpdate();
}

void OverTapFunc::iFrameUpdate(FactoryAssemble* pAssemble)
{
}

void OverTapFunc::iPostFrameUpdate()
{
}

void OverTapFunc::Start()
{
	if (m_bStarted)
	{
		return;
	}

	doingCalcTaskTread();
}

void OverTapFunc::Stop()
{
	m_bStarted = false;
	m_bThreadRunning = false;
}

void OverTapFunc::clear()
{
	Stop();
}

void OverTapFunc::doingCalcTaskTread()
{
	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_CUSTOM;
	stParam.AddParam("this", (INT_PTR)this);
	stParam.pPreDoRequestFunc = static_request_doingCalcTaskTread;
	stParam.pThreadCompleteFunc = static_callback_doingCalcTaskTread;
	stParam.strID = "OverTap_doingCalcTaskTread";
	stParam.bRepeat = false;
	stParam.bThread = true;
	stParam.bRunCallbackInMainThread = true;
	ThreadWrapper::GetInstance()->AddTask(stParam);
}

void OverTapFunc::static_request_doingCalcTaskTread(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam)
{
	OverTapFunc* pThis = (OverTapFunc*)mapParams["this"];
	while (pThis->m_bThreadRunning)
	{
		pThis->m_bThreadRealStop.store(false);

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		if (!pThis->m_bNeedRefresh)
		{
			continue;
		}
		std::vector<FactoryAssemble*> vecAssemble;
		pThis->m_pComFactory->GetCurAssemble(vecAssemble);
		pThis->m_bNeedRefresh = false;

		/******直接遍历排序*******/
		std::map<int, std::vector<FactoryAssemble*>> mapAssemble;
		for (auto pAssemble : vecAssemble)
		{
			mapAssemble[pAssemble->nType].push_back(pAssemble);
		}
		std::map<int, std::vector<OverTapData*>> mapResult;
		for (auto& it : mapAssemble)
		{
			std::vector<OverTapData*> &vecResult = mapResult[it.first];
			OverTapData* pTemp = nullptr;
			bool bNew = true;
			for (size_t i = 0; i < vecAssemble.size(); i++)
			{
				FactoryAssemble* pPoint = vecAssemble[i];
				pPoint->eOverTapState = OVERTAP_NONE;
				if (i > 0)
				{
					if (pTemp->vLabelPos.Distance2D(pPoint->pData->vLabelPos) <= pThis->m_fOverTapJudgeDis)
					{
						pTemp->vecData.push_back(pPoint);
						bNew = false;
					}
					else
					{
						bNew = true;
					}
				}
				if (bNew)
				{
					OverTapData* pNew = nullptr;
					if (pThis->m_vecAllOverTapData.size() > pThis->m_nCurOverTapDataIndex)
					{
						//复用
						pNew = pThis->m_vecAllOverTapData[pThis->m_nCurOverTapDataIndex];
					}
					else
					{
						pNew = new OverTapData;
						pThis->m_vecAllOverTapData.push_back(pNew);
					}
					pNew->vecData.clear();
					pNew->nTypeID = pPoint->nType;
					pNew->vLabelPos = pPoint->pData->vLabelPos;
					pNew->vecData.push_back(pPoint);
					pNew->eReadTifStatus = READ_TIF_NEED_NOT_YET_ADD;
					vecResult.push_back(pNew);

					++pThis->m_nCurOverTapDataIndex;

					pTemp = pNew;
				}
			}
		}
		std::vector<OverTapData*> vecResult;
		for (auto& it : mapResult)
		{
			vecResult.insert(vecResult.end(),it.second.begin(), it.second.end());
		}	

		std::vector<FactoryAssemble*> vecSingleAssemble;
		//一个的则过滤掉
		for (std::vector<OverTapData*>::iterator it =vecResult.begin(); it != vecResult.end(); )
		{
			if ((*it)->vecData.size() == 1)
			{
				OverTapData* pInfo = (*it);
				FactoryAssemble* pAssemble = pInfo->vecData[0];
				vecSingleAssemble.push_back(pAssemble);

				vecResult.erase(it);
				DELETE_PTR(pInfo);
			}
			else
			{
				++it;
			}
		}

		//给到主线程处理
		PageData* pPageData = new PageData;
		pPageData->vecResult = vecResult;
		pPageData->vecSingleAssemble = vecSingleAssemble;

		ThreadCallbackParam* pCallbackParam = new ThreadCallbackParam;
		pCallbackParam->sRequestParam = pParam->sRequestParam;
		pCallbackParam->sRequestParam.bRepeat = false;
		Guid::CreateGuId(pCallbackParam->sRequestParam.strID, "Thread_");
		pCallbackParam->sRequestParam.AddParam("pageData", (INT_PTR)pPageData);
		ThreadWrapper::GetInstance()->AddMainRunCallback(pCallbackParam);
	}
}

void OverTapFunc::static_callback_doingCalcTaskTread(ThreadCallbackParam_s* pParam)
{
	OverTapFunc* pThis = (OverTapFunc*)pParam->sRequestParam.GetParam("this");
	PageData *pPageData = (PageData*)pParam->sRequestParam.GetParam("pageData");
	if (!pPageData)
	{
		pThis->m_bThreadRealStop.store(true);
		return;
	}
	for (size_t i = 0; i < pPageData->vecSingleAssemble.size(); ++i)
	{
		FactoryAssemble* pAssemble = pPageData->vecSingleAssemble[i];
		if (!pAssemble->pData)
		{
			continue;
		}
		if (pAssemble->bInstance)
		{
			pAssemble->pLabelInstance->vTargetPosition = pAssemble->pData->vLabelPos;
			pAssemble->pLabelInstance->vOrigin = pAssemble->pLabelInstance->vTargetPosition;
		}
		else if (pAssemble->pLabelNode)
		{
			pAssemble->pLabelNode->iSetOrigin(pAssemble->pData->vLabelPos);
		}
		pAssemble->eOverTapState = OVERTAP_NONE;
	}
	pThis->m_pComFactory->RefreshVisible(pPageData->vecSingleAssemble);

	pThis->m_pLabelFactory->RefreshData((std::vector<ProjectBasedSceneElement*>*) &pPageData->vecResult);

	DELETE_PTR(pPageData);
}

CommonLabelFactory* OverTapFunc::GetOverTapLabelFactory()
{
	return m_pLabelFactory;
}

CommonLabelFactory* OverTapFunc::GetComFactory()
{
	return m_pComFactory;
}

void OverTapFunc::SetAssembleStatus(FactoryAssemble* pAssemble, bool bOverTap)
{
	BoardParam* pBoardParam = nullptr;
	if (pAssemble->pLabelNode && !pBoardParam)
	{
		pBoardParam = m_pComFactory->GetLabelBoardParam(pAssemble->nType);
	}
	if (!pBoardParam)
	{
		return;
	}
	//先将配置还原
	bool bWithLine = pBoardParam->bWithLine;
	bool bRotateAroundTarget = pBoardParam->bRotateAroundTarget;
	if (bOverTap)
	{
		//重叠显示则没有引线这些
		bWithLine = false;
		bRotateAroundTarget = false;
	}
	if (pAssemble->bInstance)
	{
		pAssemble->pLabelInstance->bWithLine = bWithLine;
		pAssemble->pLabelInstance->bRotateAroundTarget = bRotateAroundTarget;
	}
	else if (pAssemble->pLabelNode)
	{
		IBoard* pBoard = pAssemble->pLabelNode->GetDynamicComponent<IBoard>();
		pBoard->iGetBoardParam()->bWithLine = bWithLine;
		pBoard->iGetBoardParam()->bRotateAroundTarget = bRotateAroundTarget;
	}
}

void OverTapFunc::iShowAssembleForLabel(FactoryAssemble* pAssemble)
{
	if (pAssemble->eOverTapState == OVERTAP_SHOW)
	{
		SetAssembleStatus(pAssemble, true);
	}
	else
	{
		SetAssembleStatus(pAssemble, false);
	}
}

void OverTapFunc::iHideAssembleForLabel(FactoryAssemble* pAssemble)
{
	SetAssembleStatus(pAssemble, false);
}

void OverTapFunc::iChangeVisible(bool bAllVisible, const std::vector<int>& vecVisibleType)
{
	std::vector<int> vecType = vecVisibleType;
	m_pLabelFactory->SetAllVisible(bAllVisible);
	m_pLabelFactory->SetTypeVisible(vecType);
}

bool OverTapFunc::isAllStop()
{
	return m_bThreadRealStop.load();
}

