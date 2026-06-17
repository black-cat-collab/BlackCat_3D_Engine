#include "ProjectBasedHeader.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/module/IStateModule.h"
#include "Project-Based/module/BaseModule.h"
#include "Project-Based/label/CommonLabelFactory.h"

using namespace bc;

BaseModule::BaseModule(const std::string& strModuleName)
	:IStateModule(strModuleName),
	m_bEditMode(false)
{
}

BaseModule::~BaseModule()
{
	
}

EventReturnType_e BaseModule::iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent)
{
	if (tEvent.eMessage == PROJECTBASED_EVENT_SCENE_ELEMENT_DATA_REFRESHED)
	{
		bool bAccountChanged = (tEvent.nParam1 == 1) ? true : false;
		iOnRefreshLogin(bAccountChanged);
	}
	else if (tEvent.eMessage == PROJECTBASED_EVENT_ONLY_LAYER_LOADED)
	{
		iOnLayerLoaded();
	}

	return EventReturnType_e::NONE;
}

EventReturnType_e BaseModule::iExecuteEvent(IStateManager* pStateManager, const BCEvent& tEvent)
{
	if (tEvent.eMessage == PROJECTBASED_EVENT_APP_MODE_CHANGED)
	{
		iOnAppModeChanged(m_pClient->m_nAppMode);
	}
	else if (tEvent.eMessage == PROJECTBASED_EVENT_APPID_CHANGED)
	{
		iOnAppIdChanged(m_pClient->m_sUserInfo.strAppId);
	}
	else if (tEvent.eMessage == PROJECTBASED_EVENT_HC_LABEL_CLICK)
	{
		FactotryAssembleDetail* pAssemble = (FactotryAssembleDetail*)tEvent.nParam1;
		CommonLabelFactory* pFactory = (CommonLabelFactory*)tEvent.nParam2;
		bool bSelect = tEvent.nParam3;
		iOnHCLabelSelect(pAssemble, pFactory,bSelect);
	}
	else if (tEvent.eMessage == PROJECTBASED_EVENT_HC_DETAIL_CLICK)
	{
		FactotryAssembleDetail* pAssemble = (FactotryAssembleDetail*)tEvent.nParam1;
		CommonLabelFactory* pFactory = (CommonLabelFactory*)tEvent.nParam2;
		std::string* pStr = (std::string*)tEvent.nParam3;
		bool bSelect = tEvent.nParam4;
		iOnHCDetailSelect(pAssemble, pFactory, *pStr,bSelect);
	}
	else if (tEvent.eMessage == PROJECTBASED_EVENT_UDP_DATA) 
	{
		int nCommand = (int)tEvent.nParam1;
		char* pData = (char*)tEvent.nParam2;
		iDoUdpData(nCommand, pData);
	}
	else if (tEvent.eMessage == PROJECTBASED_EVENT_SCENE_MODE_SWITCH)
	{
		SceneMode_e eMode = (SceneMode_e)tEvent.nParam1;
		iOnSwitchToSceneMode(eMode);
	}
	return EventReturnType_e::NONE;
}

void BaseModule::iSceneLoad(IStateManager* pStateManager)
{
	m_pModuleGroupNode = createModuleGroupNode();
}


