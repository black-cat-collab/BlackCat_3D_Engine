#pragma once

#include <iostream>

namespace bc
{
	///////////////////////////////////////ProjectConfig////////////////////////////
	class PROJECT_BASED_API JsonHandler
	{
	public:
		JsonHandler(std::string strConfigPath = "");
		~JsonHandler();

		bool IsLoaded() { return m_bLoaded; }
		std::string GetConfigPath() { return m_strConfigPath; }
		//按父->子层级填入,比如 "is_livegbs,value"
		Json::Value GetItem(std::string strRecursionKeys);
		void ModifyItem(std::string strKey,Json::Value value);
		void WriteConfig();
		void ParseConfig(const char *pData);
		void MergeFromData(const char *pData);		//合并数据
		void MergeFromPath(std::string strFilepath);		//合并数据
		std::string ToString();

	protected:
		virtual void parseConfig();
		Json::Value* getItemRecursion(Json::Value &parent, std::string strKey);
		void merge(Json::Value root);		//合并

	private:
		std::string m_strConfigPath;
		Json::Value m_jRoot;
		bool m_bLoaded;
		BCMutexLock	 m_ObjectLock;
	};

	class PROJECT_BASED_API JsonHandlerManager
	{
	public:
		~JsonHandlerManager();
		
		JsonHandler* GetProjectConfigHandler();
		JsonHandler* GetJsonHandler(std::string strKey);
		void AddProjectConfigHandler(std::string strConfigPath = "");
		void AddJsonHandler(std::string strKey, std::string strFilePath = "");

		static JsonHandlerManager* GetInstance();
		void InitProjectConfig(std::string strConfigFilePath);
		void Clear();
		void FrameUpdate();
		void SendEvent();

	protected:
		JsonHandlerManager();

	private:
		static void	Callback_Request_Config(ThreadCallbackParam*pParam);

		void requestProjectConfig(long lDelayTime,bool bThread);		//请求项目配置

	private:
		static JsonHandlerManager *m_Instance;

		std::map<std::string, JsonHandler*> m_mapJsonHander;
		std::string m_strConfigFilePath;

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
					std::cout << "JsonHandlerManager construction" << std::endl;
				}
			}
		};
		static GC gc;  //垃圾回收类的静态成员
	};

	///////////////////////////////////////ProjectConfig////////////////////////////
}


