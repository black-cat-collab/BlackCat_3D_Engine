#include "ProjectBasedHeader.h"
#include "ProjectBasedAPI.h"

#include "Project-Based/utils/ParamNetImage.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/utils/ProjectUtils.h"
#include "Project-Based/utils/AnimateWithNodeMg.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/tools/IsolatedTool.h"

using namespace bc;

ProjectBasedAPI* ProjectBasedAPI::m_pProjectBasedAPI = nullptr;
ISystemAPI* APIProvider::m_pSystemAPI = nullptr;

ProjectBasedAPI::ProjectBasedAPI() :
	m_pAPIProvider(nullptr)
{

}

ProjectBasedAPI::~ProjectBasedAPI()
{
	StateManager::Destroy();

	ThreadWrapper::Destroy();


	DELETE_PTR(m_pAPIProvider);
}

void ProjectBasedAPI::iInitialize(ISystemAPI* pSystemAPI)
{
	srand((int)time(nullptr));
	m_pAPIProvider = new APIProvider(pSystemAPI);

	std::string strTempDir = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strTempDir;
	if (!strTempDir.empty())
	{
		Mkdirs(strTempDir.c_str());
	}
}

void ProjectBasedAPI::iInitializeScene()
{
	StateManager* pStateManager = StateManager::GetInstance();
	if (pStateManager)
	{
		pStateManager->InitializeScene();
	}
}

void ProjectBasedAPI::iSceneLoad()
{
	StateManager* pStateManager = StateManager::GetInstance();
	if (pStateManager)
	{
		pStateManager->SceneLoad();
	}
}

void ProjectBasedAPI::iFrameUpdate()
{
	ThreadWrapper::GetInstance()->RunCallback();
	ParamNetImage::GetInstance()->FrameUpdate();
	JsonHandlerManager::GetInstance()->FrameUpdate();

	StateManager* pStateManager = StateManager::GetInstance();
	if (pStateManager)
	{
		pStateManager->FrameUpdate();
	}

	AnimateWithNodeMg::GetInstance()->FrameUpdate();
}

EventReturnType_e ProjectBasedAPI::iProcessEvent(const BCEvent& tEvent)
{
	StateManager* pStateManager = StateManager::GetInstance();
	if (pStateManager)
	{
		if (pStateManager->ProcessEvent(tEvent) == EventReturnType_e::BREAK)
		{
			return EventReturnType_e::BREAK;
		}
	}

	return EventReturnType_e::NONE;
}

IStateManager* ProjectBasedAPI::iGetStateManager()
{
	return StateManager::GetInstance();
}

void ProjectBasedAPI::iShutDown()
{
	ThreadWrapper::GetInstance()->Clear();

	ParamNetImage::GetInstance()->Clear();

	JsonHandlerManager::GetInstance()->Clear();

	StateManager::GetInstance()->Clear();

	AnimateWithNodeMg::GetInstance()->Clear();
}

ProjectBasedAPI* ProjectBasedAPI::GetProjectBasedAPI()
{
	if (m_pProjectBasedAPI == nullptr)
	{
		m_pProjectBasedAPI = new ProjectBasedAPI;
	}

	return m_pProjectBasedAPI;
}

void ProjectBasedAPI::Destroy()
{
	DELETE_PTR(m_pProjectBasedAPI);
}

void ProjectBasedAPI::iSetExtraParam(const std::string& strKey, INT_PTR pValue)
{
	m_mapExtraParam[strKey] = pValue;
}

INT_PTR ProjectBasedAPI::iGetExtraParam(const std::string& strKey)
{
	std::map<std::string, INT_PTR>::iterator it = m_mapExtraParam.find(strKey);
	if (it != m_mapExtraParam.end())
	{
		return it->second;
	}
	return 0;
}

