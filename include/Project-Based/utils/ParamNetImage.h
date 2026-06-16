#pragma once

#include <iostream>

namespace bc
{
	class PROJECT_BASED_API ParamNetImage
	{
	private:
		class NetParam 
		{
		public:
			std::string strSerail;		//哪种图片，指hover,normal,check
			std::string strUrl;
			std::string strDownLoadPath;
			std::string strFileName;
			INode* pNode;		
			int nNodeType;

			NetParam()
			{
				pNode = NULL;
				nNodeType = -1;
			}
		};

		class TaskData
		{
		public:
			INode* pNode;
			std::string strServerIp;

			TaskData()
			{
				pNode = NULL;
			}

		};

		class ReTryPicData
		{
		public:
			INode* pNode;
			std::string strFullPicPath;
			std::string strPicPath;		//resource开始的相对路径
			std::string strSerail;
			bool isCreateByZip;			//是否是通过zip创建的材质
			std::string strScriptPath;	//zip创建材质的script文件绝对路径
			ReTryPicData()
			{
				pNode = NULL;
				isCreateByZip = false;
			}
		};

	public:
		virtual ~ParamNetImage();
		static ParamNetImage* GetInstance();
		void Clear();
		void FrameUpdate();
		void AddNetTask(INode *pNode, std::string strPicServerIp);
		void RemoveNetTask(INode* pNode);
		std::string CheckLocalPath(std::string& strOriUrl);		//加入本地能获取到图片，则直接返回本地图片（主要针对网络地址）
		void SetAddApplicationDir(bool bAdd) { bAddApplicationDir = bAdd; }
		void AddModelTask(INode* pNode, std::string strPicServerIp);		//加入模型下载队列
		bool CheckNetLoadModel(INode* pNode);

	protected:
		ParamNetImage();

		void removeNetParam(std::string strUrl);
		void handleNode(INode* pNode, std::string strPicServerIp);
		void handleTask(INode *pNode,int nNodeType, std::string strUrl, std::string strSerail, std::string strPicServerIp);
		bool checkHasNetTask(std::string strUrl);
		void setMaterial(INode* pNode, std::string strPicPath, std::string strSerial);

	private:
		static void	Callback_DownLoad_Pic(ThreadCallbackParam* pParam);
		static void	static_download_effectPicture(ThreadCallbackParam* pParam);
		static void	static_download_model(ThreadCallbackParam* pParam);

		bool decompressAndCreateScript(const std::string& strZipPath,bool bDeleteZip = false);//解压压缩包 material  并创建script文件 
		void doNetTask(INode* pNode, std::string strPicServerIp);
		void doModelTask(INode* pNode, std::string strPicServerIp);
		void handlenode(XMLDoc& xmlDoc, XMLNodePtr node, std::string strRealPath);
		void doingModifyDownModel(std::string strZipRealPath, std::string strUnzipPath, std::string strZipName,
			std::string strNodeID, std::string  strBoardNodeID,bool bDeleteZip); 

	private:
		static ParamNetImage* m_Instance;

		std::map<std::string, std::vector<NetParam*>> m_mapNetParam;
		std::map<std::string, std::string> m_mapFileWithUrl;	//本地路径和网络地址的对应关系,本地路径为key
		std::vector<TaskData> m_vecAllTask;
		std::vector<ReTryPicData> m_vecRetryPicData;		//图片不存在则重试
		bool	bAddApplicationDir;		//是否添加工程名字
		std::vector<TaskData> m_vecModelTask;		//下载模型任务

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
					std::cout << "ParamNetImage construction" << std::endl;
				}
			}
		};
		static GC gc;  //垃圾回收类的静态成员
	};
}


