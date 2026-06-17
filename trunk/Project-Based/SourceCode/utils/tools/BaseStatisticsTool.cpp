#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/utils/tools/BaseStatisticsTool.h"

using namespace bc;

BaseStatisticsTool::PointFactory::PointFactory() :
	CommonLabelFactory()
{

}

BaseStatisticsTool::PointFactory::~PointFactory()
{

}

INode* BaseStatisticsTool::PointFactory::iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo)
{
	BaseStatisticsTool* pTool = (BaseStatisticsTool*)GetExtraData("tool");

	INode* pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(pTool->m_tPointBoardParam);
	pNode->iLoadModel();
	pNode->iSetVisible(false);
	pNode->iSetSaveMyself(false);
	APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(pNode);

	return pNode;
}

void BaseStatisticsTool::PointFactory::iUpdateDataForLabel(FactoryAssemble* pAssemble)
{
	BaseStatisticsTool::LocalPointData* pInfo = (BaseStatisticsTool::LocalPointData*)pAssemble->pData;
	pAssemble->pLabelNode->iSetOrigin(pInfo->vPos);
}

/// //////////////////////////////////////////////////////

BaseStatisticsTool::BaseStatisticsTool() :
	m_bEnable(false),
	m_eState(STATE_NONE),
	m_bDoingStatistics(false),
	m_bCanStatistics(false),
	m_eType(STATISTICS_CIRCLE),
	m_lIntervalMill(0),
	m_bAddedLoopThread(false),
	m_fnCreateOrCancelFunc(nullptr)
{
	m_strThreadID = std::string("StartStatistics_tool_");
	std::string strID = "";
	Guid::CreateGuId(strID, m_strThreadID);
	m_strThreadID = strID;

	m_strLoopThreadID = std::string("StartStatistics_tool_loop_");
	strID = "";
	Guid::CreateGuId(strID, m_strLoopThreadID);
	m_strLoopThreadID = strID;

	m_pPointFactory = new PointFactory();
	m_pPointFactory->SetExtraData("tool", (INT_PTR)this);
	m_pPointFactory->SetAllVisible(true);
}

BaseStatisticsTool::~BaseStatisticsTool()
{
	DELETE_PTR(m_pPointFactory);
}

void BaseStatisticsTool::SetStatisticsTemplet(const Json::Value& jTemplet)
{
	m_jTemplet = jTemplet;
}

void BaseStatisticsTool::SetEnable(const bool& bEnable)
{
	m_bCanStatistics = false;
	if (!bEnable)
	{
		m_vecStatisTask.clear();
		m_vecStatisTask.resize(0);
	}
	if (m_bEnable != bEnable)
	{
		Cancel();
	}
	m_bEnable = bEnable;
	RefreshStatistics();
}

void BaseStatisticsTool::RefreshStatistics()
{
	if (m_eState == STATE_END)
	{
		m_vecStatisTask.clear();
		m_vecStatisTask.resize(0);

		m_bCanStatistics = true;
	}
}

void BaseStatisticsTool::AddStatisticsTask(const Vector3d& vPos, const std::string& strKey, const Json::Value& extraValue, INT_PTR pTr)
{
	std::vector<std::string> vecKey;
	vecKey.push_back(strKey);
	AddStatisticsTask(vPos, vecKey,extraValue, pTr);
}

void BaseStatisticsTool::AddStatisticsTask(const Vector3d& vPos, std::vector<std::string> vecKey, const Json::Value& extraValue, INT_PTR pTr)
{
	if (m_bDoingStatistics)
	{
		return;
	}
	StatisTaskData stData;
	stData.vPos = vPos;
	stData.vecKey = vecKey;
	stData.extraValue = extraValue;
	stData.pTr = pTr;
	m_vecStatisTask.push_back(stData);
}

void BaseStatisticsTool::StartStatistics(long lIntervalMill)
{
	m_bCanStatistics = false;
	m_lIntervalMill = lIntervalMill;
	m_bDoingStatistics = true;
	m_jResult = m_jTemplet;

	ThreadWrapper::GetInstance()->StopNetTask(m_strThreadID);

	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_CUSTOM;
	stParam.pThreadRuningFunc = static_doing_statistics;
	stParam.pThreadCompleteFunc = static_complete_statistics;
	stParam.AddParam("this", (INT_PTR)this);
	stParam.strID = m_strThreadID;
	stParam.bThread = true;
	stParam.bRunCallbackInMainThread = true;	//等待在主线程中执行
	ThreadWrapper::GetInstance()->AddTask(stParam);
}

bool BaseStatisticsTool::static_doing_statistics(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam)
{
	BaseStatisticsTool* pThis = (BaseStatisticsTool*)pParam->sRequestParam.mapParams["this"];
	pThis->m_vecFilterStatisTask.resize(0);

	return pThis->iDoingStatistics(mapParams,pParam);
}

void BaseStatisticsTool::static_complete_statistics(ThreadCallbackParam_s* pParam)
{
	BaseStatisticsTool* pThis = (BaseStatisticsTool*)pParam->sRequestParam.mapParams["this"];
	pThis->iCompleteStatistics(pParam);
}

void BaseStatisticsTool::iCompleteStatistics(ThreadCallbackParam_s* pParam)
{
	for (size_t i = 0; i < m_vecCompleteFunc.size(); i++)
	{
		m_vecCompleteFunc[i](this,m_jResult, m_vecFilterStatisTask,false);
	}
	m_bDoingStatistics = false;

	addLoopStatisticsThread();
}

void BaseStatisticsTool::AddCompleteFunc(StatisticsCompleteFunc pFunc)
{
	std::vector<StatisticsCompleteFunc>::iterator it = find(m_vecCompleteFunc.begin(), m_vecCompleteFunc.end(), pFunc);
	if (it != m_vecCompleteFunc.end())
	{
		return;
	}
	m_vecCompleteFunc.push_back(pFunc);
}

void BaseStatisticsTool::RemoveCompleteFunc(StatisticsCompleteFunc pFunc)
{
	for (std::vector<StatisticsCompleteFunc>::iterator it = m_vecCompleteFunc.begin(); it != m_vecCompleteFunc.end(); it++)
	{
		if ((*it) == pFunc)
		{
			m_vecCompleteFunc.erase(it);
			break;
		}
	}
}


void BaseStatisticsTool::AddINTPTR(std::string strKey, INT_PTR pTr)
{
	m_mapINTPTR[strKey] = pTr;
}

INT_PTR BaseStatisticsTool::GetINTPTR(std::string strKey)
{
	return m_mapINTPTR[strKey];
}

void BaseStatisticsTool::addKey(StatisTaskData* pData)
{
	for (size_t i = 0; i < pData->vecKey.size(); i++)
	{
		std::string strKey = pData->vecKey[i];
		if (!m_jResult.isMember(strKey))
		{
			m_jResult[strKey] = Json::arrayValue;
		}

		if (m_jResult[strKey].isInt())
		{
			int nCount = m_jResult[strKey].asInt();
			nCount++;
			m_jResult[strKey] = nCount;
		}
		else if (m_jResult[strKey].isArray())
		{
			m_jResult[strKey].append(pData->extraValue);
		}
	}
	m_vecFilterStatisTask.push_back(pData);
}

void BaseStatisticsTool::Cancel()
{
	m_eState = STATE_NONE;
	ThreadWrapper::GetInstance()->StopNetTask(m_strThreadID);
	ThreadWrapper::GetInstance()->StopNetTask(m_strLoopThreadID);
	m_vecStatisTask.clear();
	m_vecFilterStatisTask.clear();

	if (m_fnCreateOrCancelFunc)
	{
		m_fnCreateOrCancelFunc(this, false);
	}
	for (size_t i = 0; i < m_vecCompleteFunc.size(); i++)
	{
		m_vecCompleteFunc[i](this, m_jTemplet, m_vecFilterStatisTask,true);
	}
}

void BaseStatisticsTool::addLoopStatisticsThread()
{
	ThreadWrapper::GetInstance()->StopNetTask(m_strLoopThreadID);
	if (m_lIntervalMill > 0 && !m_bAddedLoopThread)
	{
		ThreadRequestParam stParam;
		stParam.eRequestType = THREAD_REQUEST_CUSTOM;
		stParam.pThreadRuningFunc = nullptr;
		stParam.pThreadCompleteFunc = static_loop_statistics;
		stParam.AddParam("this", (INT_PTR)this);
		stParam.strID = m_strLoopThreadID;
		stParam.bThread = true;
		stParam.lDelay = m_lIntervalMill;
		stParam.bRunCallbackInMainThread = true;	//等待在主线程中执行
		ThreadWrapper::GetInstance()->AddTask(stParam);

		m_bAddedLoopThread = true;
	}
}

void BaseStatisticsTool::static_loop_statistics(ThreadCallbackParam_s* pParam)
{
	BaseStatisticsTool* pThis = (BaseStatisticsTool*)pParam->sRequestParam.mapParams["this"];
	pThis->m_bAddedLoopThread = false;
	pThis->iLoopStatistics(pParam);
}

void BaseStatisticsTool::iLoopStatistics(ThreadCallbackParam_s* pParam)
{
	RefreshStatistics();
}

void BaseStatisticsTool::SetPointParam(BoardParam& tParam)
{
	m_tPointBoardParam = tParam;
}

void BaseStatisticsTool::AddJsonExtra(std::string strKey, Json::Value jValue)
{
	m_jExtra[strKey] = jValue;
}

Json::Value BaseStatisticsTool::GetJsonExtra(std::string strKey)
{
	if (m_jExtra.isMember(strKey))
	{
		return m_jExtra[strKey];
	}
	return Json::nullValue;
}

