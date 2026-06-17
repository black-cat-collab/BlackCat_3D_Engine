#include "ProjectBasedHeader.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/utils/ProjectUtils.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ProjectLogger.h"


using namespace bc;
#define MAX_REQUEST_COUNT  5	//最大并行线程个数

ThreadWrapper* ThreadWrapper::m_pThis = nullptr;

ThreadWrapper::ThreadWrapper()
{
	m_strCertFile = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + "/" +
		APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strCertFile;

	m_strAesKey = "123456708123456781234567";
}

ThreadWrapper::~ThreadWrapper()
{
	Clear();
}

void ThreadWrapper::Clear()
{
	for (int i = 0; i < m_vecNetTask.size(); i++)
	{
		m_vecNetTask[i]->bStop = true;
	}
	m_vecNetTask.clear();
	std::queue<ThreadCallbackParam*>().swap(m_queThreadCallbackParam);
}

ThreadWrapper* ThreadWrapper::GetInstance()
{
	if (m_pThis == nullptr)
	{
		m_pThis = new ThreadWrapper;
	}

	return m_pThis;
}

void ThreadWrapper::Destroy()
{
	DELETE_PTR(m_pThis);
}

int ThreadWrapper::AddTask(ThreadRequestParam &pParam)
{
	if (!(StartWith(pParam.strUrl, "http") || StartWith(pParam.strUrl, "https")) && pParam.eRequestType != THREAD_REQUEST_CUSTOM
		&& pParam.pPreDoRequestFunc == nullptr
		&& pParam.pThreadRuningFunc == nullptr)
	{
		//无效地址则直接跳过
		std::string strError = "无效地址--" + pParam.strUrl;
		ProjectLogger::GetInstance()->NetworkMessage(strError);
		//return 0;
	}
	if (pParam.strID.empty())
	{
		Guid::CreateGuId(pParam.strID, "Thread_");
	}

	if (pParam.nTimeoutTime == -1)
	{
		pParam.nTimeoutTime = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nHttpTimeOut;
	}
	if (pParam.nRetryCount == -1)
	{
		pParam.nRetryCount = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nHttpRetryCount;
	}
	if (pParam.bRepeat && pParam.lInterval > 0)
	{
		pParam.SetInnerTask(true);
		ThreadRequestParam* pRequest = GetNetTask(pParam.strID);
		if (!pRequest)
		{
			pRequest = new ThreadRequestParam;
			*pRequest = pParam;
			std::string strUUID;
			Guid::CreateGuId(strUUID);
			pRequest->SetUUID(strUUID);
			pParam.SetUUID(strUUID);
			{
				BCAutoLock lock(&m_MutexLock);
				m_vecNetTask.push_back(pRequest);
			}
		}
		else
		{
			pParam.bStop = false;
			*pRequest = pParam;
			////已经存在，则只做执行一次的处理
			//Guid::CreateGuId(pParam.strID, "Thread_");//临时的
			//pParam.bRepeat = false;
		}
	}
	ThreadCallbackParam* pCallbackParam = new ThreadCallbackParam;
	pCallbackParam->sRequestParam = pParam;
	if (pParam.bThread)
	{
		if (pParam.bInQueue)
		{
			m_mapQueThreadRequest[pParam.strQueueID].push(pCallbackParam); //做类似线程池管理
			if (m_mapCurRunningRequestCount.find(pParam.strQueueID) == m_mapCurRunningRequestCount.end())
			{
				m_mapCurRunningRequestCount[pParam.strQueueID] = 0;
			}
		}
		else
		{
			std::map<std::string, INT_PTR> mapParam;
			mapParam["ThreadWrapper"] = (INT_PTR)this;
			mapParam["ThreadCallbackParam"] = (INT_PTR)pCallbackParam;
			//使用线程
			APIProvider::GetSystemAPI()->iEngineAPI->iGetThreadManager()->StartThread(threadCallbackFunc, mapParam);
		}
	}
	else
	{
		//不使用线程
		DoNetRequest(pCallbackParam->sRequestParam, pCallbackParam);
		if (!pCallbackParam->sRequestParam.bRunCallbackInMainThread)
		{
			if (pParam.pThreadCompleteFunc)
			{
				pParam.pThreadCompleteFunc(pCallbackParam);
				DELETE_PTR(pCallbackParam);
			}
		}
		else
		{
			{
				BCAutoLock lock(&m_MutexLock);
				m_queThreadCallbackParam.push(pCallbackParam);
			}
		}
	}


	return 0;
}

int ThreadWrapper::decodeCallbackFunc(const char* pKey, const char* pDataIn, const int& nLengthIn, char*& pDataOut, int& nLengthOut)
{
	return APIProvider::GetSystemAPI()->iProtocolAPI->iGetNetworkManager()->iDecryptData(pKey, pDataIn, nLengthIn, pDataOut, nLengthOut);
}

void ThreadWrapper::threadCallbackFunc(std::map<std::string, INT_PTR>& mapParam)
{
	ThreadWrapper *pThis = (ThreadWrapper*)mapParam["ThreadWrapper"];
	ThreadCallbackParam *pCallbackParam = (ThreadCallbackParam*)mapParam["ThreadCallbackParam"];
	BCThread* pThread = (BCThread*)mapParam["BCThread"];
	long lDelayCounter = 0;
	while (1)
	{
		if (pThread->IsInterruptionRequested(true))
		{
			break;
		}

		if (pCallbackParam->sRequestParam.lDelay == 0)
		{
			break;
		}

		pThread->Sleepms(100);

		lDelayCounter += 100;

		if (lDelayCounter > pCallbackParam->sRequestParam.lDelay)
		{
			break;
		}
	}

	//if (pCallbackParam->sRequestParam.IsInnerTask())
	//{
	//	//循环的任务
	//	ThreadRequestParam* pRequest = pThis->GetNetTask(pCallbackParam->sRequestParam.strID);
	//	if (!pRequest || pRequest->bStop)
	//	{
	//		pCallbackParam->sRequestParam.bStop = true;
	//	}
	//}
	if (pCallbackParam->sRequestParam.bStop)
	{
		//停止
		//移除task
		pThis->removeNetTask(pCallbackParam->sRequestParam.strID);
		DELETE_PTR(pCallbackParam);
		return;
	}

	int nRet = pThis->DoNetRequest(pCallbackParam->sRequestParam,pCallbackParam);

	std::string strQueueID = "";
	if (pCallbackParam && pCallbackParam->sRequestParam.bInQueue)
	{
		strQueueID = pCallbackParam->sRequestParam.strQueueID;
	}
	
	if (pCallbackParam && !pCallbackParam->sRequestParam.bRunCallbackInMainThread)
	{
		//不在主线程中执行
		if (pCallbackParam->sRequestParam.pThreadCompleteFunc && !pCallbackParam->sRequestParam.bStop)
		{
			pCallbackParam->sRequestParam.pThreadCompleteFunc(pCallbackParam);
		}
		if (pCallbackParam->sRequestParam.IsInnerTask())
		{
			pThis->checkToNextTask(pCallbackParam->sRequestParam.strID, 
				pCallbackParam->sRequestParam.GetUUID());
		}
		DELETE_PTR(pCallbackParam);
	}
	else
	{
		{
			BCAutoLock lock(&pThis->m_MutexLock);
			pThis->m_queThreadCallbackParam.push(pCallbackParam);
		}
	}
	if (!strQueueID.empty())
	{
		if (pThis->m_mapCurRunningRequestCount.find(strQueueID) != pThis->m_mapCurRunningRequestCount.end())
		{
			int nCount = pThis->m_mapCurRunningRequestCount[strQueueID];
			nCount--;
			pThis->m_mapCurRunningRequestCount[strQueueID] = nCount;
		}
	}
}

int ThreadWrapper::DoNetRequest(ThreadRequestParam &sRequestParam, ThreadCallbackParam *pCallbackParam)
{
	int nRet = STATUS_ERROR;
	if (!pCallbackParam)
	{
		return nRet;
	}
	
	HttpParam tHttpParam;
	tHttpParam.strID = sRequestParam.strID;
	std::string strCertFile = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strCertFile;
	if (StartWith(strCertFile, APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strResourceDirName))
	{
		strCertFile = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir + "/" + strCertFile;
	}
	else if(!strCertFile.empty())
	{
		strCertFile = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + "/" + strCertFile;
	}
	tHttpParam.strCert = strCertFile;
	IHttp* pHttp = APIProvider::GetSystemAPI()->iProtocolAPI->iGetNetworkManager()->iRegistHttp(tHttpParam);
	if (sRequestParam.pDecodeFunc == nullptr)
	{
		sRequestParam.pDecodeFunc = decodeCallbackFunc;
	}
	if (sRequestParam.pPreDoRequestFunc)
	{
		sRequestParam.pPreDoRequestFunc(sRequestParam.mapParams, pCallbackParam);
	}
	switch (sRequestParam.eRequestType)
	{
	case THREAD_REQUEST_CUSTOM:
		if (sRequestParam.pThreadRuningFunc)
		{
			if (sRequestParam.pThreadRuningFunc(sRequestParam.mapParams, pCallbackParam))
			{
				nRet = STATUS_SUCCESS;
			}
		}
		else
		{
			nRet = STATUS_SUCCESS;
		}
		break;
	case THREAD_REQUEST_API_GET:
		if (pHttp)
		{
			nRet = pHttp->iDownloadUrl(sRequestParam.strUrl, pCallbackParam->strResponse, pCallbackParam->mapResponseHeader,HTTP_HEADER_TYPE_JSON,
				sRequestParam.nTimeoutTime, sRequestParam.nRetryCount, &sRequestParam.vecHttpCustomHeaderParam);
		}
		break;
	case THREAD_REQUEST_API_POST:
		if (pHttp)
		{
			nRet = pHttp->iPostUrl(sRequestParam.strUrl, pCallbackParam->strResponse, pCallbackParam->mapResponseHeader,sRequestParam.strPostData,
				HTTP_HEADER_TYPE_JSON, &(sRequestParam.vecHttpPostParam), sRequestParam.nTimeoutTime, sRequestParam.nRetryCount ,&sRequestParam.vecHttpCustomHeaderParam);
		}
		break;
	case THREAD_REQUEST_DOWNLOAD:
		if (pHttp)
		{
			nRet = pHttp->iDownloadFile(sRequestParam.strUrl, sRequestParam.strDownloadPath, nullptr,
				sRequestParam.nTimeoutTime, sRequestParam.nRetryCount, &sRequestParam.vecHttpCustomHeaderParam);
		}
		break;
	case THREAD_REQUEST_UPLOAD:
		if (pHttp)
		{
			nRet = pHttp->iUploadFile(sRequestParam.strUrl, pCallbackParam->strResponse, sRequestParam.strUploadFilePath,
				sRequestParam.strUploadFileFieldName,sRequestParam.strPostData, HTTP_HEADER_TYPE_EXPECT, &(sRequestParam.vecHttpPostParam), sRequestParam.nTimeoutTime,
				sRequestParam.nRetryCount,&sRequestParam.vecHttpCustomHeaderParam);
		}
		break;
	default:
		break;
	}
	
	if (nRet == STATUS_SUCCESS && sRequestParam.pDecodeFunc && sRequestParam.eRequestType != THREAD_REQUEST_CUSTOM)
	{
		try
		{
			Json::CharReaderBuilder jsonBuilder;
			std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
			Json::Value root;
			std::string jsError;
			if (reader->parse(pCallbackParam->strResponse.c_str(), pCallbackParam->strResponse.c_str() + pCallbackParam->strResponse.length(), &root, &jsError))
			{
				if ((root.isMember("encrypt") && root["encrypt"].asInt()) || sRequestParam.bDecode)
				{
					char* pDecode = nullptr;
					int nDecodeSize = 0;

					if (root.isMember("data"))
					{
						std::string strData = root["data"].asString();

						if (sRequestParam.pDecodeFunc)
						{
							sRequestParam.pDecodeFunc(m_strAesKey.c_str(), strData.c_str(), strData.length(), pDecode, nDecodeSize);
							Json::Value dataItem;
							if (reader->parse(pDecode, pDecode + strlen(pDecode), &dataItem, &jsError))
							{
								if (dataItem.isArray() || dataItem.isObject())
								{
									root["data"] = dataItem;
								}
							}
							else
							{
								root["data"] = std::string(pDecode);
							}
							std::string strUnicode = root.toStyledString();
							pCallbackParam->strResponse = root.toStyledString();
						}	
					}
					else
					{
						std::string strData = root["data"].asString();

						sRequestParam.pDecodeFunc(m_strAesKey.c_str(), strData.c_str(), strData.length(), pDecode, nDecodeSize);
						if (pDecode)
						{
							pCallbackParam->strResponse = pDecode;
						}
					}

					DELETE_ARRAY(pDecode);
				}
			}

			if(sRequestParam.eReturnType == RETURN_FORMAT_JSON && !pCallbackParam->strResponse.empty())
			{
				Json::Value value;
				Json::CharReaderBuilder jsonBuilder;
				std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
				std::string jsError;
				if (reader->parse(pCallbackParam->strResponse.c_str(), pCallbackParam->strResponse.c_str() + pCallbackParam->strResponse.length(), &value, &jsError))
				{
					pCallbackParam->jRoot = value;
				}
			}
		}
		catch (const std::exception&)
		{
			pCallbackParam->jRoot = Json::nullValue;
			pCallbackParam->nResult = STATUS_ERROR;
			pCallbackParam->strResponse = "reader to json responce error!";
		}
	}

	pCallbackParam->sRequestParam = sRequestParam;
	pCallbackParam->nResult = nRet;

	APIProvider::GetSystemAPI()->iProtocolAPI->iGetNetworkManager()->iFreeHttp(&pHttp);

	//记录日志
	if (sRequestParam.eRequestType != THREAD_REQUEST_CUSTOM)
	{
		std::string strMessage = sRequestParam.strUrl;
		try
		{
			strMessage += "--result:" + std::to_string(nRet);
			if (pCallbackParam->jRoot.isMember("code"))
			{
				strMessage += ",api result:" + std::to_string(pCallbackParam->jRoot["code"].asInt());
			}
			strMessage += "\n";
			if (sRequestParam.vecHttpCustomHeaderParam.size() > 0)
			{
				strMessage += "vecHttpCustomHeaderParam:\n";
			}
			for (size_t i = 0; i < sRequestParam.vecHttpCustomHeaderParam.size(); i++)
			{
				strMessage += sRequestParam.vecHttpCustomHeaderParam[i].first;
				strMessage += ":";
				strMessage += sRequestParam.vecHttpCustomHeaderParam[i].second.toStyledString();
				strMessage += "\n";
			}
			if (sRequestParam.vecHttpPostParam.size() > 0)
			{
				strMessage += "vecHttpPostParam:\n";
			}
			for (size_t i = 0; i < sRequestParam.vecHttpPostParam.size(); i++)
			{
				strMessage += sRequestParam.vecHttpPostParam[i].first;
				strMessage += ":";
				strMessage += sRequestParam.vecHttpPostParam[i].second.toStyledString();
				strMessage += "\n";
			}
			if (pCallbackParam->jRoot.isMember("data"))
			{
				strMessage += "result data:";
				if (!pCallbackParam->jRoot["data"].isNull())
				{
					strMessage += "is not null";
				}
				else
				{
					strMessage += "is null";
				}
			}
		}
		catch (const std::exception&)
		{

		}
		ProjectLogger::GetInstance()->NetworkMessage(strMessage);
	}

	return nRet;
}

bool ThreadWrapper::handleCustomFunc(ThreadCallbackParam *pParam)
{
	if (!pParam)
	{
		return false;
	}
	return pParam->sRequestParam.pThreadRuningFunc(pParam->sRequestParam.mapParams, pParam);
}

void ThreadWrapper::AddMainRunCallback(ThreadCallbackParam* pCallbackParam)
{
	if (!pCallbackParam)
	{
		return;
	}
	BCAutoLock lock(&m_MutexLock);
	m_queThreadCallbackParam.push(pCallbackParam);
}

void ThreadWrapper::SetAesKey(const std::string& strKey)
{
	int nLength = strKey.length();
	if (nLength != 16 || nLength != 24 || nLength != 32)
	{
		return;
	}

	m_strAesKey = strKey;
}

void ThreadWrapper::RunCallback()
{
	while (!m_queThreadCallbackParam.empty())
	{
		ThreadCallbackParam* pParam = nullptr;
		{
			BCAutoLock lock(&m_MutexLock);
			pParam = m_queThreadCallbackParam.front();
			m_queThreadCallbackParam.pop();
		}
		if (pParam->sRequestParam.pThreadCompleteFunc != nullptr)
		{
			pParam->sRequestParam.pThreadCompleteFunc(pParam); 
		}
		if (pParam->sRequestParam.IsInnerTask())
		{
			checkToNextTask(pParam->sRequestParam.strID, pParam->sRequestParam.GetUUID());
		}
		DELETE_PTR(pParam);
	}

	for (std::map<std::string, std::queue<ThreadCallbackParam*>>::iterator it = m_mapQueThreadRequest.begin(); 
		it != m_mapQueThreadRequest.end(); ++it)
	{
		std::queue<ThreadCallbackParam*>& queThreadRequest = it->second;
		int nCount = -1000;
		if (m_mapCurRunningRequestCount.find(it->first) != m_mapCurRunningRequestCount.end())
		{
			nCount = m_mapCurRunningRequestCount[it->first];
		}
		while (!queThreadRequest.empty() && (nCount != -1000 && nCount < MAX_REQUEST_COUNT))
		{
			++m_mapCurRunningRequestCount[it->first];
			++nCount;

			ThreadCallbackParam* pParam = queThreadRequest.front();

			std::map<std::string, INT_PTR> mapParam;
			mapParam["ThreadWrapper"] = (INT_PTR)this;
			mapParam["ThreadCallbackParam"] = (INT_PTR)pParam;
			//使用线程
			APIProvider::GetSystemAPI()->iEngineAPI->iGetThreadManager()->StartThread(threadCallbackFunc, mapParam);

			queThreadRequest.pop();
		}
	}
}

ThreadRequestParam* ThreadWrapper::GetNetTask(const std::string& strID)
{
	{
		BCAutoLock lock(&m_MutexLock);
		for (size_t i = 0; i < m_vecNetTask.size(); i++)
		{
			if (m_vecNetTask[i]->strID == strID)
			{
				return m_vecNetTask[i];
			}
		}
	}
	return nullptr;
}

void ThreadWrapper::StopNetTask(const std::string& strID, const bool& bFuzzy)
{
	{
		BCAutoLock lock(&m_MutexLock);
		for (std::vector<ThreadRequestParam*>::iterator it = m_vecNetTask.begin(); it != m_vecNetTask.end(); it++)
		{
			bool bFind = false;
			if (bFuzzy)
			{
				if ((*it)->strID.find(strID) != std::string::npos)
				{
					bFind = true;
				}
			}
			else
			{
				if ((*it)->strID == strID)
				{
					bFind = true;
				}
			}
			if (bFind)
			{
				(*it)->bStop = true;
			}
		}
	}
}

void ThreadWrapper::removeNetTask(const std::string& strID)
{
	{
		BCAutoLock lock(&m_MutexLock);
		for (std::vector<ThreadRequestParam*>::iterator it = m_vecNetTask.begin() ; it != m_vecNetTask.end(); it++)
		{
			if ((*it)->strID == strID)
			{
				DELETE_PTR(*it);

				m_vecNetTask.erase(it);
				break;
			}
		}
	}
}

void ThreadWrapper::checkToNextTask(std::string strID, std::string strUUID)
{
	ThreadRequestParam* pRequest = GetNetTask(strID);
	if (pRequest)
	{
		if (pRequest->bStop)
		{
			removeNetTask(strID);
			return;
		}
		if (pRequest->bRepeat && pRequest->lInterval > 0 && pRequest->GetUUID() == strUUID)
		{
			ThreadRequestParam stRequest;
			stRequest = *pRequest;
			stRequest.bRepeat = false;
			stRequest.lDelay = pRequest->lInterval;
			AddTask(stRequest);
		}
	}
}

void ThreadWrapper::ClearQueueTask(std::string strQueueID)
{
	for (std::map<std::string, std::queue<ThreadCallbackParam*>>::iterator it = m_mapQueThreadRequest.begin();
		it != m_mapQueThreadRequest.end(); ++it)
	{
		if (it->first == strQueueID || strQueueID.empty())
		{
			while (!it->second.empty())
			{
				ThreadCallbackParam* pParam = it->second.front();
				DELETE_PTR(pParam);
				it->second.pop();
			}
			m_mapCurRunningRequestCount[it->first] = 0;
		}
	}
}

