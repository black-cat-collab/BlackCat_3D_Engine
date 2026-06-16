#pragma once

#include "InterfaceDef.h"
#ifdef _MSC_VER
extern bc::IProjectClientAPI*	InitializeAPIProjectClient(bc::ISystemAPI* pSystemAPI);
extern void						UninitializeAPIProjectClient(bc::ISystemAPI* pSystemAPI);
#else
extern "C"
{
	bc::IProjectClientAPI* InitializeAPIProjectClient(bc::ISystemAPI* pSystemAPI);
	void				    UninitializeAPIProjectClient(bc::ISystemAPI* pSystemAPI);
}

#endif

