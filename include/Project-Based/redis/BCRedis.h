#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "Project-Based/redis/hiredis/hiredis.h"

#pragma comment(lib,"redis/WS2_32.Lib")
#ifdef _DEBUG
#pragma comment(lib,"redis/hiredis_d.lib")
#else
#pragma comment(lib,"redis/hiredis.lib")
#endif // _DEBUG

namespace bc 
{
	class PROJECT_BASED_API BCRedis
	{
	public:
		BCRedis();
		~BCRedis();
		bool FConnect(std::string strHost, int nPort,std::string strAuth = "");
		void FDisConnect();
		bool FSetDataBase(int nIndex);
		bool FGetSetValue(std::string strKey,std::vector<std::string> &vecValue);
	private:
		bool m_bConnectting;
		redisContext*	m_pContext;
		std::mutex		m_mutext;
	};
}

