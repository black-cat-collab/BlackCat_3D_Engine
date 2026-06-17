#pragma once

#include "InterfaceDef.h"

#ifdef _MSC_VER
extern bc::IProjectBasedAPI*	InitializeAPIProjectBased(bc::ISystemAPI* pSystemAPI);
extern void						UninitializeAPIProjectBased(bc::ISystemAPI* pSystemAPI);

#else
extern "C"
{
	bc::IProjectBasedAPI*	InitializeAPIProjectBased(bc::ISystemAPI* pSystemAPI);
	void				    UninitializeAPIProjectBased(bc::ISystemAPI* pSystemAPI);
}

#endif


