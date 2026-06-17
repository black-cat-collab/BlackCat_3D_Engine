#include "ProjectBasedHeader.h"
#include "Project-Based/redis/BCRedis.h"

using namespace bc;

BCRedis::BCRedis()
{
	m_bConnectting = false;
}

BCRedis::~BCRedis()
{
	m_pContext = nullptr;
}

bool BCRedis::FConnect(std::string strHost, int nPort, std::string strAuth)
{
	m_pContext = redisConnect(strHost.c_str(), nPort);
	if (m_pContext != nullptr && m_pContext->err)
	{
		return false;
	}
	redisReply* m_pReply = (redisReply*)redisCommand(m_pContext, "AUTH %s", strAuth.c_str());
	if (!m_pReply)
	{
		return false;
	}
	if (m_pReply->type == REDIS_REPLY_STATUS)
	{
		freeReplyObject(m_pReply);
		m_bConnectting = true;
		return true;
	}
	freeReplyObject(m_pReply);
	return false;
}

void BCRedis::FDisConnect()
{
	if (!m_bConnectting)
	{
		return;
	}
	redisFree(m_pContext);
}

bool BCRedis::FSetDataBase(int nIndex)
{
	if (!m_bConnectting)
	{
		return false;
	}
	redisReply* m_pReply = (redisReply*)redisCommand(m_pContext, "SELECT %d", nIndex);
	if (!m_pReply)
	{
		return false;
	}
	if (m_pReply->type == REDIS_REPLY_STATUS)
	{
		freeReplyObject(m_pReply);
		m_bConnectting = true;
		return true;
	}
	freeReplyObject(m_pReply);
	return false;
}

bool BCRedis::FGetSetValue(std::string strKey, std::vector<std::string>& vecValue)
{
	if (!m_bConnectting)
	{
		return false;
	}
	m_mutext.lock();
	redisReply* m_pReply = (redisReply*)redisCommand(m_pContext, "SMEMBERS %s ", strKey.c_str());
	m_mutext.unlock();
	if (!m_pReply)
	{
		return false;
	}
	if (m_pReply->type == REDIS_REPLY_ARRAY)
	{
		for (size_t i = 0; i < m_pReply->elements; i++)
		{
			std::string strValue = "";
			if (m_pReply->element)
			{
				strValue = std::string(m_pReply->element[i]->str);
			}
			vecValue.push_back(strValue);
		}
		freeReplyObject(m_pReply);
		return true;
	}
	freeReplyObject(m_pReply);
	return false;
}

