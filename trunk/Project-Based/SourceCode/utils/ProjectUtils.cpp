
#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/utils/ProjectUtils.h"
#include "Project-Based/utils/ProjectLogger.h"

using namespace bc;

///////////////////////////////////////ProjectConfig////////////////////////////
JsonHandler::JsonHandler(std::string strConfigPath)
{
	m_jRoot = Json::Value(Json::objectValue);
	m_strConfigPath = strConfigPath;
	m_bLoaded = false;
	if (!m_strConfigPath.empty())
	{
		parseConfig();
	}
}


JsonHandler::~JsonHandler()
{
}

void JsonHandler::parseConfig()
{
	std::string strData = ReadFile(m_strConfigPath);
	ParseConfig(strData.c_str());
}

void JsonHandler::ParseConfig(const char *pData)
{
	if (pData)
	{
		Json::CharReaderBuilder jsonBuilder;
		std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
		Json::Value root;
		std::string jsError;
		if (reader->parse(pData, pData + strlen(pData), &root, &jsError))
		{
			BCAutoLock lock(&m_ObjectLock);
			m_jRoot = root;
			m_bLoaded = true;
		}
	}
}

Json::Value JsonHandler::GetItem(std::string strRecursionKeys)
{
	BCAutoLock lock(&m_ObjectLock);
	std::vector<std::string> vecTree;
	SplitStringBySpecial(strRecursionKeys,vecTree,",");
	Json::Value value(Json::nullValue);
	Json::Value item = m_jRoot;
	try
	{
		if (vecTree.size() >= 1)
		{
			for (int i = 0; i < vecTree.size(); i++)
			{
				if (item.isNull())
				{
					break;
				}
				if (item.isObject() && item.isMember(vecTree[i]))
				{
					item = item[vecTree[i]];
				}
				else
				{
					break;
				}
				if (i == (vecTree.size() - 1))
				{
					value = item;
					return value;
				}
			}
		}
		else if (vecTree.size() == 0)
		{
			Json::Value *item = getItemRecursion(m_jRoot, vecTree[0]);
			Json::Value value(Json::nullValue);
			if (item)
			{
				value = *item;
			}
			return value;
		}
	}
	catch (const std::exception&)
	{

	}
	return value;
}

void JsonHandler::ModifyItem(std::string strKey, Json::Value value)
{
	BCAutoLock lock(&m_ObjectLock);
	Json::Value *item = getItemRecursion(m_jRoot,strKey);
	if (item)
	{
		*item = value;
	}
}

void JsonHandler::WriteConfig()
{
	std::string strData = m_jRoot.toStyledString();
	WriteToFile(m_strConfigPath,strData.c_str());
}

Json::Value* JsonHandler::getItemRecursion(Json::Value &parent, std::string strKey)
{
	if (parent.isArray())
	{
		return NULL;
	}
	if (parent.isObject())
	{
		if (parent.isMember(strKey))
		{
			return &parent[strKey];
		}
		else
		{
			Json::Value::Members mem = parent.getMemberNames();
			for (auto iter = mem.begin(); iter != mem.end(); iter++)
			{
				Json::Value* item = getItemRecursion(parent[*iter],strKey);
				if (item)
				{
					return item;
				}
			}
		}
	}

	return NULL;
}

void JsonHandler::MergeFromData(const char *pData)
{
	if (pData)
	{
		std::string str = pData;
		Json::CharReaderBuilder jsonBuilder;
		std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
		Json::Value root;
		std::string jsError;
		if (reader->parse(str.c_str(), str.c_str() + str.length(), &root, &jsError))
		{
			merge(root);
		}
	}
}

void JsonHandler::MergeFromPath(std::string strFilepath)
{
	std::string strData = ReadFile(strFilepath);
	MergeFromData(strData.c_str());
}

void JsonHandler::merge(Json::Value root)
{
	BCAutoLock lock(&m_ObjectLock);
	//必须是object
	if (!m_jRoot.isObject() || !root.isObject())
	{
		return;
	}
	Json::Value::Members mem = root.getMemberNames();
	for (auto iter = mem.begin(); iter != mem.end(); iter++)
	{
		if ((root[*iter].type() != Json::objectValue) 
			&& (root[*iter].type() != Json::arrayValue))
		{
			continue;
		}
		m_jRoot[*iter] = root[*iter];
	}
}

std::string JsonHandler::ToString()
{
	return m_jRoot.toStyledString();
}

///////////////////////////////////////ProjectConfig////////////////////////////

///////////////////////////////////////JsonHandlerManager////////////////////////////
#define PROJECT_CONFIG_NAME std::string("project")

JsonHandlerManager* JsonHandlerManager::m_Instance = nullptr;
JsonHandlerManager::GC JsonHandlerManager::gc;

JsonHandlerManager::JsonHandlerManager()
{
}

JsonHandlerManager::~JsonHandlerManager()
{
	Clear();
}

void JsonHandlerManager::Clear()
{
	for (std::map<std::string, JsonHandler*>::iterator it = m_mapJsonHander.begin(); it != m_mapJsonHander.end(); it++)
	{
		delete it->second;
		it->second = nullptr;
	}
	m_mapJsonHander.clear();
}

JsonHandlerManager* JsonHandlerManager::GetInstance()
{
	if (m_Instance == NULL)
	{
		m_Instance = new JsonHandlerManager();
	}
	return m_Instance;
}

void JsonHandlerManager::InitProjectConfig(std::string strConfigFilePath)
{
	m_strConfigFilePath = strConfigFilePath;

	//GetProjectConfigHandler()->MergeFromData(APIProvider::GetSystemAPI()->iEngineAPI
	//	->iGetProjectConfig().sBootConfig.strProjectCustomConfig.c_str());

	requestProjectConfig(0,false);
}

void JsonHandlerManager::FrameUpdate()
{
	//ThreadWrapper::GetInstance()->RunCallback();
}

void JsonHandlerManager::SendEvent()
{
	bc::BCEvent _tEvent(PROJECTBASED_EVENT_PROJECT_CONFIG_REFRESH, 0, 0, 0);
	APIProvider::GetSystemAPI()->iEngineAPI->iExecuteEvent(_tEvent);
}

void JsonHandlerManager::requestProjectConfig(long lDelayTime, bool bThread)
{
	std::string strApiHost = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strServerIp;
	if (strApiHost.empty())
	{
		//strApiHost = "http://127.0.0.1:8088";
	}
	if (lDelayTime >= 0)
	{
		ThreadRequestParam stParam;
		stParam.eRequestType = THREAD_REQUEST_API_GET;
		stParam.pThreadRuningFunc = NULL;
		stParam.pThreadCompleteFunc = Callback_Request_Config;
		stParam.mapParams["this"] = (INT_PTR)this;
		char szBuffer[1024] = { 0 };
		EServerServion eType = APIProvider::GetSystemAPI()->iProtocolAPI->iGetServerVersion();
		if (eType >= EServerServion::SERVER_VERSION_V10)
		{
			sprintf_s(szBuffer, sizeof(szBuffer), "%s/api/v10/getProjectConfig?projectName=%s", strApiHost.c_str(),
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strApplicationName.c_str());
		}
		else
		{
			sprintf_s(szBuffer, sizeof(szBuffer), "%s/admin/project/getconfig?project_name=%s", strApiHost.c_str(),
				APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strApplicationName.c_str());
		}
		stParam.strID = "requestProjectConfig";
		stParam.strUrl = szBuffer;
		stParam.bRepeat = false;
		stParam.bThread = bThread;
		stParam.lDelay = lDelayTime;
		stParam.bRunCallbackInMainThread = bThread;
		ThreadWrapper::GetInstance()->AddTask(stParam);
	}
}

void JsonHandlerManager::Callback_Request_Config(ThreadCallbackParam*pParam)
{
	bool bForceNetConfig = false;
	JsonHandlerManager *pThis = (JsonHandlerManager*)pParam->sRequestParam.mapParams["this"];
	if (pParam->nResult == STATUS_SUCCESS)
	{
		try
		{
			Json::Value root = pParam->jRoot;
			if ((root["code"].isInt()) && (root["code"].asInt() == 0))
			{
				std::string strConfig = root["data"]["config"].asString();
				if (!strConfig.empty())
				{
					pThis->AddProjectConfigHandler();
					pThis->GetProjectConfigHandler()->ParseConfig(strConfig.c_str());
					Json::Value item = pThis->GetProjectConfigHandler()->GetItem("is_force_net_config,value");
					if (item.isBool())
					{
						bForceNetConfig = item.asBool();
					}

					std::string str = pThis->GetProjectConfigHandler()->ToString();
					APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strProjectCustomConfig = str;
				}
			}
		}
		catch (const std::exception&)
		{
			
		}
	}

	if (!bForceNetConfig)
	{
		pThis->GetProjectConfigHandler()->MergeFromPath(pThis->m_strConfigFilePath);
	}
	long lDelayTime = -1;
	Json::Value item = pThis->GetProjectConfigHandler()->GetItem("refresh_config_time,value");
	if (item.isDouble())
	{
		lDelayTime = item.asFloat();
	}
	if (lDelayTime >= 0)
	{
		pThis->requestProjectConfig(lDelayTime, true);
	}
	std::string strMessage = "project config:\n";
	strMessage += pThis->GetProjectConfigHandler()->ToString();
	ProjectLogger::GetInstance()->InfoMessage(strMessage);

	pThis->SendEvent();
}

JsonHandler* JsonHandlerManager::GetProjectConfigHandler()
{
	return GetJsonHandler(PROJECT_CONFIG_NAME);
}

void JsonHandlerManager::AddProjectConfigHandler(std::string strConfigPath)
{
	AddJsonHandler(PROJECT_CONFIG_NAME,strConfigPath);
}

JsonHandler* JsonHandlerManager::GetJsonHandler(std::string strKey)
{
	JsonHandler *pHander = m_mapJsonHander[strKey];
	if (!pHander)
	{
		pHander = new JsonHandler;
		m_mapJsonHander[strKey] = pHander;
	}
	return pHander;
}

void JsonHandlerManager::AddJsonHandler(std::string strKey, std::string strFilePath)
{
	if (m_mapJsonHander.find(strKey) != m_mapJsonHander.end())
	{
		return;
	}
	JsonHandler *pHandler = new JsonHandler(strFilePath);
	m_mapJsonHander[strKey] = pHandler;
}

///////////////////////////////////////JsonHandlerManager////////////////////////////

