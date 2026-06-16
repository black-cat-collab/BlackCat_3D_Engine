#pragma once

#include <iostream>
#include "Project-Based/utils/spdlog/include/spdlog/spdlog.h"

#define LOGGER_FILE_NAME_INFO	std::string("info")
#define LOGGER_FILE_NAME_ERROR	std::string("error")
#define LOGGER_FILE_NAME_DEBUG	std::string("debug")
#define LOGGER_FILE_NAME_NETWORK	std::string("network")
#define LOGGER_FILE_NAME_SYSTEM	std::string("system")
#define LOGGER_FILE_NAME_WEB	std::string("web")
#define LOGGER_FILE_NAME_UDP	std::string("udp")
#define LOGGER_FILE_QT			std::string("qt")
#define LOGGER_FILE_CLOUD		std::string("cloud")

namespace bc
{
	class spdlog::logger;

	class PROJECT_BASED_API ProjectLogger
	{
	public:
		virtual ~ProjectLogger();

		static ProjectLogger* GetInstance();

		void Create(const int& nMaxFileCount, const int& nMaxFileSize = 20);

		void InfoMessage(const std::string& strMessage, const long& lIntervalMill = 0);
		void ErrorMessage(const std::string& strMessage, const long& lIntervalMill = 0);
		void DebugMessage(const std::string& strMessage, const long& lIntervalMill = 0);
		void NetworkMessage(const std::string& strMessage, const long& lIntervalMill = 0);
		void SystemMessage(const std::string& strMessage, const long& lIntervalMill = 0);
		void WebMessage(const std::string& strMessage, const long& lIntervalMill = 0);
		void QtMessage(const std::string& strMessage, const long& lIntervalMill = 0);
		void CloudMessage(const std::string& strMessage, const long& lIntervalMill = 0);
		//间隔日志，strLoggerID为logger的标识，比如上面的 LOGGER_FILE_NAME_INFO	std::string("info")
		void WriteIntervalLog(std::string& strLoggerID, const std::string& strMessage, const long& lIntervalMill);
		//Udp日志，nType--类型（同后台协定好），lIntervalMill同类型两次记录的间隔时间（毫秒）
		void UdpMessage(std::string &strType,const std::string& strMessage,const long &lIntervalMill);

	protected:
		ProjectLogger();

		//写日志
		virtual void writeMessage(const std::string& strLoggerID,const std::string& strMessage, const long& lIntervalMill = 0);

	private:
		static ProjectLogger* m_Instance;
		long long m_llMaxFileSize;

		std::map<std::string, std::shared_ptr<spdlog::logger>> m_mapLooger;
		std::map<std::string, long> m_mapLoggerLastRecord;	//udp数据上次的
		std::map<std::string, long> m_mapUdpTypeLast;		//udp数据上次的

	private:
		class GC // 垃圾回收类
		{
		public:
			GC()
			{
				std::cout << "GC construction" << std::endl;
			}

			~GC()
			{
				std::cout << "GC destruction" << std::endl;
				if (m_Instance != NULL)
				{
					delete m_Instance;
					m_Instance = NULL;
					std::cout << "ProjectLogger construction" << std::endl;
				}
			}
		};
		static GC gc;  //垃圾回收类的静态成员
	};
}


