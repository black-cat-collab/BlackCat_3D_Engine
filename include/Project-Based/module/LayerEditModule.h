#pragma once
namespace bc 
{
	class UdpScribeInfo
	{
	public:
		int nType;
		std::vector <std::string> vecData;

	public:
		UdpScribeInfo()
		{
			nType = -1;
		}

	};

	class IStateModule;
	class BaseModule;
	class PROJECT_BASED_API LayerEditModule :
		public BaseModule
	{
	public:
		LayerEditModule(const std::string& strModuleName);
		virtual ~LayerEditModule();

		virtual void				iFrameUpdate(IStateManager* pStateManager);
		virtual void				iDoWebCommond(WebCommandData* pWebData);	//处理网页数据
		virtual void				iOnAppModeChanged(int nAppMode);
		virtual void				iDoUdpData(int nCommand, char* pData);

	protected:
		std::queue<UdpScribeInfo*>	m_queueUdpSubscribe;
		BCMutexLock					m_UdpSubscribeMutexLock;
	};
}


