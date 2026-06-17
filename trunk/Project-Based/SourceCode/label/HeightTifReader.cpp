#include "ProjectBasedHeader.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "HeightTifReader.h"

using namespace bc;

HeightTifReader* HeightTifReader::m_Instance = NULL;
HeightTifReader::GC HeightTifReader::gc;

HeightTifReader::HeightTifReader():
	m_bThreadRunning(true)
{
	m_pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;

	INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
	if (pCameraNode)
	{
		pCameraNode->iSetFrameUpdateFunc(static_camera_FrameUpdate, this);
	}

	for (int i = 0; i < 1; ++i)
	{
		doingReadHeightTifTread(i + 1);
	}
}

HeightTifReader::~HeightTifReader()
{
	m_bThreadRunning = false;
}

HeightTifReader* HeightTifReader::GetInstance()
{
	if (m_Instance == NULL)
	{
		m_Instance = new HeightTifReader();
	}
	return m_Instance;
}

void HeightTifReader::AddTask(FactoryAssemble* pAssemble)
{
	m_MutexLock.Lock();
	m_stackTask.push(pAssemble);
	m_MutexLock.UnLock();
}

void HeightTifReader::AddFirstTask(FactoryAssemble* pAssemble)
{
	m_MutexLock.Lock();
	pAssemble->pData->eReadTifStatus = READ_TIF_NEED_ALREADY_ADD;
	m_stackFirstTask.push(pAssemble);
	m_MutexLock.UnLock();
}

void HeightTifReader::FrameUpdate()
{
}

void HeightTifReader::Clear()
{
	m_MutexLock.Lock();
	std::stack<FactoryAssemble*> stackTemp;
	m_stackTask.swap(stackTemp);

	std::stack<FactoryAssemble*> stackTemp2;
	m_stackFirstTask.swap(stackTemp2);
	m_MutexLock.UnLock();
}

void HeightTifReader::static_camera_FrameUpdate(ISystemAPI* pSystemAPI, INode* pNode, void* pUser)
{
	HeightTifReader* pThis = (HeightTifReader*)pUser;
	for (size_t i = 0; i < pThis->m_vecFunc.size(); ++i)
	{
		pThis->m_vecFunc[i]->iCameraFrameUpdate(pNode);
	}
}

void HeightTifReader::AddFunc(IFunc* pFuncObject)
{
	if (pFuncObject->GetFuncType() != FUNC_READ_HRIGHT_TIF)
	{
		return;
	}
	if (find(m_vecFunc.begin(), m_vecFunc.end(), pFuncObject) == m_vecFunc.end())
	{
		m_vecFunc.push_back(pFuncObject);
	}
}

void HeightTifReader::RemoveFunc(IFunc* pFuncObject)
{
	for (std::vector<IFunc*>::iterator it = m_vecFunc.begin(); it != m_vecFunc.end(); ++it)
	{
		if ((*it) == pFuncObject)
		{
			m_vecFunc.erase(it);
			break;
		}
	}
}

void HeightTifReader::doingReadHeightTifTread(int nIndex)
{
	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_CUSTOM;
	stParam.AddParam("this", (INT_PTR)this);
	stParam.pPreDoRequestFunc = static_request_doingReadHeightTif;
	stParam.pThreadCompleteFunc = static_callback_doingReadHeightTif;
	stParam.strID = "doingReadHeightTifTread_" + to_string(nIndex) ;
	stParam.bRepeat = false;
	stParam.bThread = true;
	stParam.bRunCallbackInMainThread = true;
	ThreadWrapper::GetInstance()->AddTask(stParam);
}

void HeightTifReader::static_request_doingReadHeightTif(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam)
{
	HeightTifReader* pThis = (HeightTifReader*)mapParams["this"];
	while (pThis->m_bThreadRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		std::vector<Vector3d> vecPoint;
		std::vector<FactoryAssemble*> vecAssemble;
		pThis->m_MutexLock.Lock();
		std::stack<FactoryAssemble*> stack1 = pThis->m_stackFirstTask;
		std::stack<FactoryAssemble*> stack2 = pThis->m_stackTask;
		pThis->m_MutexLock.UnLock();
		while (!stack1.empty())
		{
			stack2.push(stack1.top());
			stack1.pop();
		}

		int nCount = 0;
		int nMax = 100;
		while (!stack2.empty() && nCount < nMax)
		{
			FactoryAssemble* pAssemble = stack2.top();
			if (!pAssemble->bSmoothMove && pAssemble->pData 
				&& (pAssemble->pData->eReadTifStatus == READ_TIF_NEED_NOT_YET_ADD
					|| pAssemble->pData->eReadTifStatus == READ_TIF_NEED_ALREADY_ADD))
			{
				vecPoint.push_back(pAssemble->pData->vLabelPos);
				vecAssemble.push_back(pAssemble);

				nCount++;
			}

			stack2.pop();
		}
		pThis->m_MutexLock.Lock();
		pThis->m_stackFirstTask = stack1;
		pThis->m_stackTask = stack2;
		pThis->m_MutexLock.UnLock();

		if (vecPoint.size() == 0)
		{
			continue;
		}
		InnerTaskData* pTaskData = new InnerTaskData;
		pTaskData->vecAssemble = vecAssemble; 
		//取高地图
		std::vector<double> vecHeight;
		pThis->m_pClient->GetZFromTif(vecPoint, vecHeight);
		pTaskData->vecHeight = vecHeight;

		//给到主线程处理
		ThreadCallbackParam* pCallbackParam = new ThreadCallbackParam;
		pCallbackParam->sRequestParam = pParam->sRequestParam;
		pCallbackParam->sRequestParam.bRepeat = false;
		Guid::CreateGuId(pCallbackParam->sRequestParam.strID, "Thread_");
		pCallbackParam->sRequestParam.AddParam("resultData", (INT_PTR)pTaskData);
		ThreadWrapper::GetInstance()->AddMainRunCallback(pCallbackParam);
	}
}

void HeightTifReader::static_callback_doingReadHeightTif(ThreadCallbackParam_s* pParam)
{
	HeightTifReader* pThis = (HeightTifReader*)pParam->sRequestParam.GetParam("this");
	InnerTaskData* pResultData = (InnerTaskData*)pParam->sRequestParam.GetParam("resultData");
	if (!pResultData)
	{
		return;
	}
	for (size_t i = 0; i < pResultData->vecHeight.size(); ++i)
	{
		FactoryAssemble* pAssemble = pResultData->vecAssemble[i];
		if (!pAssemble->pData)
		{
			continue;
		}
		Vector3d &vPos = pAssemble->pData->vLabelPos;
		pAssemble->pData->eReadTifStatus = READ_TIF_NEED_DONE;
		pAssemble->pData->dZ = pResultData->vecHeight[i];
		pThis->m_pClient->AdjustZ(vPos, pResultData->vecHeight[i], false);
		pAssemble->pData->vLabelPos = vPos;
		if (pAssemble->bInstance)
		{
			pAssemble->pLabelInstance->vTargetPosition = pAssemble->pData->vLabelPos;
			pAssemble->pLabelInstance->vOrigin = pAssemble->pData->vLabelPos;
		}
		else if (pAssemble->pLabelNode && pAssemble->eOverTapState != OVERTAP_SHOW)
		{
			pAssemble->pLabelNode->iSetOrigin(vPos);
		}
	}

	DELETE_PTR(pResultData);
}

int HeightTifReader::GetTaskSize()
{
	int nLength = 0;
	m_MutexLock.Lock();
	nLength = m_stackFirstTask.size();
	nLength += m_stackTask.size();
	m_MutexLock.UnLock();

	return nLength;
}

