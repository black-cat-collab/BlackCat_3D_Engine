#pragma once

namespace bc
{
	class ProjectBasedClient;
	class BCVideoRouterPointInfo
	{
	public:
		Vector3d vPos;		//位置
		Vector3d vAngle;		//角度
		VideoBoardWithButtonParam stVideoParam;	//面板的param
		bool bWithVideo;			//是否有适配面板
		std::string strCameraID;	//国标id
		std::string strUrl;			//视频流地址
		std::string strName;		//名称
		INode* pNode;				//生成的节点

		BCVideoRouterPointInfo()
		{
			bWithVideo = false;
			pNode = nullptr;
		}

		BCVideoRouterPointInfo& operator = (const class BCVideoRouterPointInfo& src)
		{
			vPos = src.vPos;
			vAngle = src.vAngle;
			stVideoParam = src.stVideoParam;
			bWithVideo = src.bWithVideo;
			strCameraID = src.strCameraID;
			strUrl = src.strUrl;
			strName = src.strName;
			pNode = src.pNode;

			return *this;
		}
	};

	class PROJECT_BASED_API VideoInspectionFactory
	{
	public:
		VideoInspectionFactory(ISystemAPI *pSystemAPI);
		virtual ~VideoInspectionFactory();

		//添加巡控路线
		IVideoRoute* AddInspection(std::string strKey, std::vector<BCVideoRouterPointInfo*> &vecInfo);
		//是否同步,是否进行返回数据解密
		void Create(std::string strUrl, bool bSync = true);
		IVideoRoute* FindVideoInspectionPathBy(std::string strKey);
		void GetAllVideoInspectionPath(std::vector<IVideoRoute*> &vecRoute);
		//播放,nLoop:循环次数  -1则表示无线循环
		void Play(std::string strKey,int nLoop = 1);
		//暂停
		void Pause();
		//继续
		void Resume();
		//停止
		void Stop();
		//是否暂停中
		bool IsPause();
		//获取所有的路径的点位信息
		std::map<std::string, std::vector<BCVideoRouterPointInfo*>>& GetAllPathPointInfo() { return m_mapPathInfo; }
		Json::Value& GetJsonDataResult() { return m_jRetData; }

	protected:
		//解析
		virtual void iModifyPointInfo(const std::string& strKey, std::vector<BCVideoRouterPointInfo*>& vecPointInfo) = 0;		//主要设置livegbs的channelID
		virtual void iParseInspectionData(const char* pData);
		virtual void iModifyInspectionPath(std::string strKey, IVideoRoute* pPath);
		//数据以请求完
		virtual void iOnLoaded(Json::Value jAllData) {};
		//切换视频点位的时候
		virtual bool iChangeVideoPoint(BCVideoRouterPointInfo* pInfo, bool bOpen) { return true; };

	private:
		static void	Callback_Get_Api_Inspection(ThreadCallbackParam *pParam);
		static bool static_video_change(INode* pVideoBoard, void* pUser, bool bOpen);

		void clear();

	protected:
		std::vector<INode*> m_vecVideoInspectionPath;
		IVideoRoute* m_pCurVideoInpectionPath;
		std::map<std::string, std::vector<BCVideoRouterPointInfo*>> m_mapPathInfo;
		Json::Value m_jRetData;
		ProjectBasedClient*				m_pClient;
	};
}


