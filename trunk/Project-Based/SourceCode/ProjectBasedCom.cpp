#include "ProjectBasedHeader.h"
#include "ProjectBasedCom.h"

bc::IProjectBasedAPI*	InitializeAPIProjectBased(bc::ISystemAPI* pSystemAPI);
void					UninitializeAPIProjectBased(bc::ISystemAPI* pSystemAPI);

// 加载
bc::IProjectBasedAPI* InitializeAPIProjectBased(bc::ISystemAPI* pSystemAPI)
{
	bc::ProjectBasedAPI* pProjectBasedAPI = bc::ProjectBasedAPI::GetProjectBasedAPI();
	if (pProjectBasedAPI == nullptr)
	{
		return nullptr;
	}

	pSystemAPI->iProjectBasedAPI = pProjectBasedAPI;
	pProjectBasedAPI->iInitialize(pSystemAPI);

	return pProjectBasedAPI;
}

void UninitializeAPIProjectBased(bc::ISystemAPI* pSystemAPI)
{
	bc::ProjectBasedAPI::Destroy();
	pSystemAPI->iProjectBasedAPI = nullptr;
}

