#pragma once

namespace bc
{
	class ExtraHeatmapData
	{
	public:
		Vector3d vPos;		//位置
		float nWeight;		//权重
		float fSize;		//范围
		std::string strKey;			//区域key
		std::string strTime;		//时间
		long  lTime;		//时间的时间戳（秒）

		ExtraHeatmapData()
		{
			nWeight = 0;
			fSize = 0;
			lTime = 0;
		}

		ExtraHeatmapData& operator = (const class ExtraHeatmapData &src)
		{
			vPos = src.vPos;
			nWeight = src.nWeight;
			fSize = src.fSize;
			strKey = src.strKey;
			strTime = src.strTime;
			lTime = src.lTime;

			return *this;
		}
	};

	//热力图item的数据填入
	class HeatmapItemData
	{
	public:
		//热力图请求的回调
		typedef void(*HeatmapRequestCallback)(HeatmapItemData *pHeatmapItemData,
			std::string strResponce, std::vector<ExtraHeatmapData> &outVecData);
		//边界请求的回调
		typedef void(*BoundaryRequestCallback)(HeatmapItemData *pHeatmapItemData,
			std::string strResponce, std::vector<Vector3d> &outVecData);

	public:
		std::string strKey;		//key
		std::string strHeatmapUrl;		//热力图url路径
		std::string strBoundaryUrl;		//边界url路径
		HeatmapRequestCallback fnHeatmapUrlRequestCallback;	//热力图url网络请求的回调
		BoundaryRequestCallback fnBoundaryUrlRequestCallback;	//边界url网络请求的回调
		std::vector<ExtraHeatmapData> vecExtraHeatmapData;	//直接传入热力图数据
		std::vector<Vector3d> vecBoundaryData;	//直接传入边界数据
		float fIntensityMax;		//红色临界值
		float fDotSize;		//点的辐射范围（ExtraHeatmapData.fSize 优先）
		Vector4d vBoundaryColor;		//边界颜色
		float fBoundaryWidth;		//边界线宽
		long lHeatmapIntervalMillisecond;			//热力图 请求间隔时间（毫秒） 0：不会循环
		bool bShow;									//是否显示
		INode*	pLocusLine;						//轨迹线
		bool bSelected;							//是否选中状态
		long long lLastIntervalMillisecond;		//最近获取的时间
		std::map<std::string, INT_PTR> mapINTPTR;			//变量存储区

		std::string strViewport;				//视点
		int nClusterCount;						//群聚数量
		int nClusterLevel;						//群聚等级
		bool bCluster;							//是否支持群聚检测
		std::string strName;					//名称

		HeatmapItemData()
		{
			fnHeatmapUrlRequestCallback = NULL;
			fnBoundaryUrlRequestCallback = NULL;
			fIntensityMax = 60.0f;
			fDotSize = 50.0f;
			vBoundaryColor = Vector3d(26.0 / 255.0f, 230.0 / 255.0f, 189.0 / 255.0f);
			fBoundaryWidth = 3.0f;
			lHeatmapIntervalMillisecond = 0;
			bShow = true;
			pLocusLine = NULL;
			bSelected = false;
			lLastIntervalMillisecond = 0;
			nClusterCount = 0;
			nClusterLevel = 0;
			bCluster = false;
		}

		HeatmapItemData& operator = (const class HeatmapItemData &src)
		{
			strKey = src.strKey;
			strHeatmapUrl = src.strHeatmapUrl;
			strBoundaryUrl = src.strBoundaryUrl;
			fnHeatmapUrlRequestCallback = src.fnHeatmapUrlRequestCallback;
			fnBoundaryUrlRequestCallback = src.fnBoundaryUrlRequestCallback;
			vecExtraHeatmapData = src.vecExtraHeatmapData;
			vecBoundaryData = src.vecBoundaryData;
			fIntensityMax = src.fIntensityMax;
			fDotSize = src.fDotSize;
			vBoundaryColor = src.vBoundaryColor;
			fBoundaryWidth = src.fBoundaryWidth;
			lHeatmapIntervalMillisecond = src.lHeatmapIntervalMillisecond;
			bShow = src.bShow;
			pLocusLine = src.pLocusLine;
			bSelected = src.bSelected;
			lLastIntervalMillisecond = src.lLastIntervalMillisecond;
			mapINTPTR = src.mapINTPTR;

			strViewport = src.strViewport;
			nClusterCount = src.nClusterCount;
			nClusterLevel = src.nClusterLevel;
			bCluster = src.bCluster;
			strName = src.strName;

			return *this;
		}

		~HeatmapItemData()
		{
			vecExtraHeatmapData.shrink_to_fit();
		}
	};

	class PROJECT_BASED_API HeatmapFactory
	{
	public:
		HeatmapFactory(ISystemAPI *pSystemAPI);
		virtual ~HeatmapFactory();
		virtual void UninitializeThread();
		//在每帧的地方调用
		virtual void FrameUpdate();

		//开始
		virtual void StartRequest();
		//重新请求
		virtual bool IsStarted() { return m_bStarted; }
		//停止
		virtual void StopRequest();
		virtual void Clear();
		//更新，
		virtual void UpdateAndSetDataChanged(bool bOnRequest = false);
		//仅显示部分区域
		virtual void ShowOnlyAreas(std::vector<std::string> vecAreaID);
		//显示所有的区域
		virtual void ShowAllAreas();
		//设置区域是否显示
		virtual void SetAreaVisible(std::string strKey, bool bVisible);
		//设置区域是否显示
		virtual void SetAreaVisible(std::vector<std::string> vecKeys, bool bVisible);
		//设置外部变量
		virtual void AddINTPTR(std::string strKey,INT_PTR pTr);
		//获取外部变量
		virtual INT_PTR GetINTPTR(std::string strKey);
		//添加
		virtual void AddHeatmap(HeatmapItemData itemData);
		//移除
		virtual void RemoveHeatmap(std::string strKey);
		//移除所有
		virtual void RemoveAllHeatmap();
		//获取热力图item
		virtual HeatmapItemData* GetHeatmapItem(std::string strKey);
		//刷新
		virtual void RefreshHeatmapItem();
		//获取热力图item
		virtual void GetCurHeatmapItems(std::vector<HeatmapItemData*> &vecHeatmapItems);
		//获取所有图item
		virtual void GetAllHeatmapItems(std::vector<HeatmapItemData*> &vecHeatmapItems);
		//返回最小间隔,返回-1 表示无轮询
		virtual long GetHeatmapIntervalMillisecond();
		void SetWightScale(float fScale) { m_fWightScale = fScale; }

	protected:
		//数据加载完成后
		virtual void iLoadedData() {}
		//边框数据加载完成
		virtual void iLoadedBoundaryData() {}
		//解析边界数据
		virtual void iParseBoundaryData(HeatmapItemData *pHeatmapItemData,
			std::string strResponce, std::vector<Vector3d> &outVecData) {}
		//解析边界数据
		virtual void iParseHeatmapData(HeatmapItemData *pHeatmapItemData,
			std::string strResponce, std::vector<ExtraHeatmapData> &outVecData) {}
		//对数据进行过滤
		virtual bool iFilter(ExtraHeatmapData *pData) { return true; }
		//网络请求的拦截处理
		virtual void iInterceptHttpRequest(ThreadRequestParam* pRequest) {};

	private:
		//解析边界数据
		static void Static_Boundary_Callback(HeatmapItemData *pHeatmapItemData,
			std::string strResponce, std::vector<Vector3d> &outVecData);
		//解析热力图数据
		static void Static_Heatmap_Callback(HeatmapItemData *pHeatmapItemData,
			std::string strResponce, std::vector<ExtraHeatmapData> &outVecData);

		static void TimingGetData(std::map<std::string, INT_PTR>& mapParam);		//定时获取热力图数据
		static bool CompWeiht(const ExtraHeatmapData &a, const ExtraHeatmapData &b);
		void updateShowBoundaryLocus();		//更新边界的显示
		void requestBoundary(std::vector<HeatmapItemData*>& vecHeatmapItems);
		void requestBoundary(HeatmapItemData *pItemData);		//请求边界

	protected:
		ISystemAPI *m_pSystemAPI;

		BCMutexLock m_ThMutex;
		BCThread *m_TheThread;
		bool m_bStarted;		//是否已经开始
		bool m_bRefresh;		//是否刷新
		bool m_bOnRequestApi;		//是否马上进行api请求
		std::map<std::string, INT_PTR> m_mapINTPTR;		//外部变量
		std::vector<HeatmapItemData*> m_vecHeatmapItem;		//所有的
		float		m_fWightScale;		//weight比例

	};
}


