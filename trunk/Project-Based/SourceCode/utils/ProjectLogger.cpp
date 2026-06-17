#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ProjectLogger.h"
#include "Project-Based/utils/spdlog/include/spdlog/sinks/daily_file_sink.h"
#include "Project-Based/utils/spdlog/include/spdlog/spdlog.h"

#include "Utility/LinuxOS/LinuxOS.h"


#ifdef _MSC_VER
#include <atlstr.h>
#endif



using namespace bc;
using namespace spdlog;

ProjectLogger* ProjectLogger::m_Instance = NULL;
ProjectLogger::GC ProjectLogger::gc;

ProjectLogger::ProjectLogger()
{
	m_llMaxFileSize = 20 * 1024 * 1024;  //1G
}

ProjectLogger::~ProjectLogger()
{
	spdlog::drop_all();
}

ProjectLogger* ProjectLogger::GetInstance()
{
	if (m_Instance == NULL)
	{
		m_Instance = new ProjectLogger();
	}
	return m_Instance;
}

void ProjectLogger::Create(const int& nMaxFileCount, const int& nMaxFileSize)
{
	//获取exe的名称
	std::string strTemp = "";
	TCHAR exeFullPath[_MAX_PATH];
	GetModuleFileName(NULL, exeFullPath, _MAX_PATH);
	strTemp = exeFullPath;

	int index = 0;
	while (true)
	{
		index = strTemp.find("\\");
		if (index != -1)
		{
			strTemp = strTemp.substr(index + 1);
		}
		else
		{
			break;
		}
	}
	std::string strExeName = strTemp;
	ReplaceAllString(strExeName, ".exe", "");

	m_llMaxFileSize = (long long)nMaxFileSize * 1 * 1024 * 1024;
	std::string strFilePath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir;
	strFilePath += "/project_logs/";
	std::string str = strExeName;
	std::vector<std::string> vec;
	SplitStringBySpecial(str, vec, ":");
	std::string strAppName = "";
	for (size_t i = 0; i < vec.size(); ++i)
	{
		if (i > 0)
		{
			strAppName += "_";
		}
		strAppName += vec[i];
	}
	strFilePath += strAppName;
	Mkdirs(strFilePath.c_str());
	std::vector<std::string> vecStrFileName;
	vecStrFileName.push_back(LOGGER_FILE_NAME_INFO);
	vecStrFileName.push_back(LOGGER_FILE_NAME_ERROR);
	vecStrFileName.push_back(LOGGER_FILE_NAME_DEBUG);
	vecStrFileName.push_back(LOGGER_FILE_NAME_NETWORK);
	vecStrFileName.push_back(LOGGER_FILE_NAME_SYSTEM);
	vecStrFileName.push_back(LOGGER_FILE_NAME_WEB);
	vecStrFileName.push_back(LOGGER_FILE_NAME_UDP);
	vecStrFileName.push_back(LOGGER_FILE_QT);
	vecStrFileName.push_back(LOGGER_FILE_CLOUD);
	for (size_t i = 0; i < vecStrFileName.size(); i++)
	{
		std::string strName = vecStrFileName[i];
		std::string strPath = strFilePath + "/" + strName + "/";
		std::string strFileName = strName + ".txt";
		std::shared_ptr<logger> pLogger;
		if (strName == LOGGER_FILE_NAME_NETWORK || strName == LOGGER_FILE_NAME_UDP)
		{
			pLogger = daily_logger_st(strName, strPath + strFileName, 0, 0, false, nMaxFileCount);
		}
		else
		{
			pLogger = daily_logger_mt(strName, strPath + strFileName, 0, 0, false, nMaxFileCount);
		}
		m_mapLooger[strName] = pLogger;

		//删除过期的日志
		std::vector<FileInfo> vecLocalFiles;
		GetFilesFromDir(strPath, vecLocalFiles, false, "txt");
		for (size_t j = 0; j < vecLocalFiles.size(); j++)
		{
#ifdef _MSC_VER
			std::string strFile = vecLocalFiles[j].tInfo.name;
#else
			std::string strFile = vecLocalFiles[j].tInfo->d_name;
#endif

			std::string strDataName = Replace_all(strFile, strName + "_", "");
			strDataName = Replace_all(strDataName, ".txt", "");
			long lCurTime = time(nullptr);
			long lFileTime = StrTotimestamp(strDataName.c_str());
			long lDeltaTime = lCurTime - lFileTime;
			if (lDeltaTime > nMaxFileCount * 3600 * 24)
			{
				//过期了，删除
				std::string strOldFile = strPath + strFile;
				remove(strOldFile.c_str());
			}
			//else
			//{
			//	std::string strFilePath = strPath + vecLocalFiles[j].name;
			//	long lSize = GetFileSize(strFilePath);
			//	if (lSize > 1 * 1024 * 1024 * 8)
			//	{
			//		WriteToFile(strFilePath, "", ios::trunc);
			//	}
			//}
		}
		pLogger->info("##########################################################");
	}
}

void ProjectLogger::InfoMessage(const std::string& strMessage, const long& lIntervalMill)
{
	writeMessage(LOGGER_FILE_NAME_INFO, strMessage, lIntervalMill);
}

void ProjectLogger::ErrorMessage(const std::string& strMessage, const long& lIntervalMill)
{
	writeMessage(LOGGER_FILE_NAME_ERROR, strMessage, lIntervalMill);
}

void ProjectLogger::DebugMessage(const std::string& strMessage, const long& lIntervalMill)
{
	writeMessage(LOGGER_FILE_NAME_DEBUG, strMessage, lIntervalMill);
}

void ProjectLogger::NetworkMessage(const std::string& strMessage, const long& lIntervalMill)
{
	writeMessage(LOGGER_FILE_NAME_NETWORK, strMessage, lIntervalMill);
}

void ProjectLogger::SystemMessage(const std::string& strMessage, const long& lIntervalMill)
{
	writeMessage(LOGGER_FILE_NAME_SYSTEM, strMessage, lIntervalMill);
}

void ProjectLogger::WebMessage(const std::string& strMessage, const long& lIntervalMill)
{
	writeMessage(LOGGER_FILE_NAME_WEB, strMessage, lIntervalMill);
}

void ProjectLogger::QtMessage(const std::string& strMessage, const long& lIntervalMill)
{
	writeMessage(LOGGER_FILE_QT, strMessage, lIntervalMill);
}

void ProjectLogger::CloudMessage(const std::string& strMessage, const long& lIntervalMill)
{
	writeMessage(LOGGER_FILE_CLOUD, strMessage, lIntervalMill);
}

void ProjectLogger::UdpMessage(std::string& strType, const std::string& strMessage, const long& lIntervalMill)
{
	std::map<std::string, long>::iterator itLoastRecord = m_mapUdpTypeLast.find(strType);
	bool bCanWrite = true;
	long lCurTime = GetTickCount64();
	if (itLoastRecord != m_mapUdpTypeLast.end())
	{
		long lLastTime = itLoastRecord->second;
		long lDelate = lCurTime - lLastTime;
		if (lDelate < lIntervalMill)
		{
			bCanWrite = false;
		}
	}
	if (bCanWrite)
	{
		std::string str = strType + "--" + strMessage;
		writeMessage(LOGGER_FILE_NAME_UDP, str);
		m_mapUdpTypeLast[strType] = lCurTime;
	}
}

void ProjectLogger::writeMessage(const std::string& strLoggerID, const std::string& strMessage, const long& lIntervalMill)
{
	std::map<std::string, std::shared_ptr<spdlog::logger>>::iterator it = m_mapLooger.find(strLoggerID);
	if (it == m_mapLooger.end())
	{
		return;
	}
	logger* pLogger = it->second.get();
	if (!pLogger)
	{
		return;
	}

	//格式化日志
	std::string strPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir;
	strPath += "/project_logs/" + strLoggerID + "/";
	std::vector<FileInfo> vecLocalFiles;
	GetFilesFromDir(strPath, vecLocalFiles, false, "txt");
	for (size_t j = 0; j < vecLocalFiles.size(); j++)
	{
#ifdef _MSC_VER
		std::string strFilePath = strPath + vecLocalFiles[j].tInfo.name;
#else
		std::string strFilePath = strPath + vecLocalFiles[j].tInfo->d_name;
#endif

		long lSize = GetFileSize(strFilePath);
		if (lSize > m_llMaxFileSize)
		{
			WriteToFile(strFilePath, "", std::ios::trunc);
		}
	}

	std::map<std::string, long>::iterator itLoastRecord = m_mapLoggerLastRecord.find(strLoggerID);
	bool bCanWrite = true;
	long lCurTime = GetTickCount64();
	if (itLoastRecord != m_mapLoggerLastRecord.end())
	{
		long lLastTime = itLoastRecord->second;
		long lDelate = lCurTime - lLastTime;
		if (lDelate < lIntervalMill)
		{
			bCanWrite = false;
		}
	}
	if (bCanWrite)
	{
		pLogger->info(strMessage);
		pLogger->flush();
		m_mapLoggerLastRecord[strLoggerID] = lCurTime;
	}
}
