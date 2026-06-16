#pragma once

namespace bc
{
	class CommonLabelFactory;
	class ClusterFuncFactory;

	class PROJECT_BASED_API ClusterFunc : public IFunc
	{
	public:
		class LevelData : public ProjectBasedSceneElement
		{
		public:
			int nLevel;			//对应的等级
			Vector3d vTopLeft;
			Vector3d vBottomRight;
			std::map<CommonLabelFactory*, int> mapCount;
			Vector3d vCenter;
			float fWidth;
			float fHeight;
			Vector3d vBarycenter;		//质心

			LevelData() : ProjectBasedSceneElement()
			{
				nLevel = -1;
				fWidth = 0.0f;
				fHeight = 0.0f;
			}

			class LevelData& operator = (const class LevelData& src)
			{
				ProjectBasedSceneElement::operator=(src);

				nLevel = src.nLevel;
				vTopLeft = src.vTopLeft;
				vBottomRight = src.vBottomRight;
				mapCount = src.mapCount;
				vCenter = src.vCenter;
				fWidth = src.fWidth;
				fHeight = src.fHeight;
				vBarycenter = src.vBarycenter;

				return *this;
			}

			int GetAllCount()
			{
				int nCount = 0;
				for (std::map<CommonLabelFactory*, int>::iterator it = mapCount.begin(); it != mapCount.end(); it++)
				{
					nCount += it->second;
				}
				return nCount;
			}

		};

		class ClusterLevel
		{
		public:
			int nLevel;			//层级数
			int nRow;			//行
			int nColumn;		//列
			float fDistance;	//跟相机的距离小于此距离则展开	
			ClusterLevel* pLowClusterLevel;		    //下一级等级，null表示最低等级
			ClusterLevel* pHighClusterLevel;		//上一级等级，null表示最高等级
			std::vector<std::vector<LevelData*>> vecRowLevelData;
			float fGridWidth;
			float fGridHeight;

			ClusterLevel()
			{
				nLevel = -1;		//表示无效
				nRow = -1;
				nColumn = -1;
				fDistance = -1;
				pLowClusterLevel = nullptr;
				pHighClusterLevel = nullptr;
				fGridWidth = -1.0f;
				fGridHeight = -1.0f;
			}
		};

		class CalcResult
		{
		public:
			std::vector<FactoryAssemble*> vecShowAssemble;
			std::vector<FactoryAssemble*> vecHideAssemble;
			CommonLabelFactory* pFactory;
			CalcResult* pOverTapResult;
			bool bChangeData;		//factory是否有刷新，有刷新则丢弃此次任务

			CalcResult()
			{
				pFactory = nullptr;
				pOverTapResult = nullptr;
				bChangeData = false;
			}

			~CalcResult()
			{
				vecShowAssemble.clear();
				vecHideAssemble.clear();

				if (pOverTapResult)
				{
					DELETE_PTR(pOverTapResult);
				}
			}
		};

		class PageData
		{
		public:
			std::vector<CalcResult*> vecResult;
			std::vector<LevelData*> vecLevelData;

			~PageData()
			{
				for (size_t i = 0; i < vecResult.size(); i++)
				{
					DELETE_PTR(vecResult[i]);
				}
				vecResult.clear();
				vecLevelData.clear();
			}

		};

	public:
		ClusterFunc();
		virtual ~ClusterFunc();
		ELabelFunc GetFuncType() { return FUNC_CLUSTER; }
		virtual EventReturnType_e iProcessEvent(const BCEvent& tEvent);

		void AddFactory(CommonLabelFactory* pFactory);
		void RemoveFactory(CommonLabelFactory* pFactory);	//移除聚合的Factory
		void AddLevel(ClusterLevel stLevel);		//添加层级
		void ClearLevel();		//删除层级
		void SetClusterRange(Vector3d vLeftTop, Vector3d vRightBottom);
		void SetClusterLabelZ(float fHeight) { m_fClusterLabelZ = fHeight; }
		float GetClusterLabelZ() { return m_fClusterLabelZ; }
		CommonLabelFactory* GetClusterLabelFactory();
		bool IsInRange(Vector3d& vPos);
		//是不是外部模式
		void SetOutMode(bool bOutMode) { m_bOutMode = bOutMode; }
		bool IsOutMode() { return m_bOutMode; }

		//启用线程计算聚合
		void Start();
		void Stop();

	public:
		virtual void iCreateData(FactoryAssemble* pAssemble);
		virtual void iUpdateData(FactoryAssemble* pAssemble);
		virtual void iPreFrameUpdate();
		virtual void iFrameUpdate(FactoryAssemble* pAssemble);
		virtual void iPostFrameUpdate();
		virtual INode* iCreateClusterLabelNode(int nType) { return nullptr; }		//创建聚合标签
		virtual void iUpdateClusterLabelNode(FactoryAssemble* pAssemble) {}		//更新聚合标签
		virtual void iSetClusterLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect) {}	//标签点击
		virtual void iChangeVisible(bool bAllVisible, const std::vector<int>& vecVisibleType);
		virtual bool isAllStop();

	private:
		static bool compareByLevel(ClusterLevel& p1, ClusterLevel& p2);
		static void static_request_doingCalcTaskTread(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam);
		static void	static_callback_doingCalcTaskTread(ThreadCallbackParam_s* pParam);

		void clear();
		virtual void calcLevelData();		//计算分级
		void		 doingCalcTaskTread();		//线程计算
		void		 doFactoryCluster(CommonLabelFactory* pFactory, bool bAddCount, CalcResult* pCalcResult);
		bool		 checkShowInLevel(CommonLabelFactory* pFactyory, INode* pCameraNode, Vector3d& vPos, bool bAddCount);

	protected:
		std::vector<CommonLabelFactory*> m_vecFactory;
		Vector3d		m_vTopLeft;				// 左上顶点 坐标轴为：x朝右，y朝上
		Vector3d		m_vBottomRight;			// 右下顶点
		std::vector<ClusterLevel> m_vecClusterLevel;	//总共层级
		bool		m_bStarted;
		Vector3d		m_vLastCameraPos;
		BCMutexLock					m_MutexLock;
		bool		m_bThreadRunning;
		bool		m_bNeedRefresh;		//是否刷新
		float		m_fClusterLabelZ;		//聚合标签的高度，=0则使用计算出来的高度,>0则使用设置进来的
		std::vector<LevelData*> m_vecCurLevelData;
		ClusterFuncFactory* m_pClusterBoardFactory;
		std::atomic<bool> m_bThreadRealStop;	
		bool		m_bOutMode;
	};
}


