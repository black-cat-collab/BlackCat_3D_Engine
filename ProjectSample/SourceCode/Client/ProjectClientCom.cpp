#include "UserHeader.h"
#include "ProjectClientCom.h"

bc::IProjectClientAPI*	InitializeAPIProjectClient(bc::ISystemAPI* pSystemAPI);
void					UninitializeAPIProjectClient(bc::ISystemAPI* pSystemAPI);

bc::IProjectClientAPI* InitializeAPIProjectClient(bc::ISystemAPI* pSystemAPI)
{
	bc::ProjectClientAPI* pProjectClientAPI = bc::ProjectClientAPI::GetProjectClientAPI();

	if (pProjectClientAPI == nullptr)
	{
		return nullptr;
	}
	pSystemAPI->iProjectClientAPI = pProjectClientAPI;
	pProjectClientAPI->iInitialize(pSystemAPI);

	return pProjectClientAPI;
}

void UninitializeAPIProjectClient(bc::ISystemAPI* pSystemAPI)
{
	bc::ProjectClientAPI::Destroy();
	pSystemAPI->iProjectClientAPI = nullptr;
}
