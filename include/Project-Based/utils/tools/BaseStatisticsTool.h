#pragma once

namespace bc
{
	class BaseStatisticsTool;
	enum EStatisticsType
	{
		STATISTICS_CIRCLE = 0,	//圈选
		STATISTICS_LINE,		//线选
		STATISTICS_POLYGON,		//多边形
	};

	class StatisTaskData
	{
	public:
		Vector3d vPos;
		Json::Value extraValue;
		std::vector<std::string> vecKey;
		INT_PTR pTr;

		StatisTaskData()
		{
			extraValue = Json::nullValue;
			pTr = 0;
		}

	};

	// 统计回调 vPos筛选的位置，jResult 统计的最终结果，pHandleNode 参与统计做交集的节点
	typedef void(*StatisticsCompleteFunc)(BaseStatisticsTool *pTool, const Json::Value& jResult,std::vector<StatisTaskData*> vecFilterStatisTask,const bool bCancel);
	// 生成或者取消的回调
	typedef void(*CreateOrCancelFunc)(BaseStatisticsTool* pTool,bool bCreate);

	class CommonLabelFactory;
	class PROJECT_BASED_API BaseStatisticsTool
	{
	protected:
		enum State_e
		{
			STATE_NONE = 0,	//无状态（此状态下才能开始新的一次绘制）
			STATE_MOVE,		//移动
			STATE_END,		//结束
		};

		class LocalPointData : public ProjectBasedSceneElement
		{
		public:
			Vector3d vPos;

		public:
			LocalPointData() : ProjectBasedSceneElement()
			{
			}
		};

		class PointFactory : public CommonLabelFactory
		{
		public:
			PointFactory();
			virtual ~PointFactory();

		protected:
			virtual INode* iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo);
			virtual void iUpdateDataForLabel(FactoryAssemble* pAssemble);


		};

	public:
		BaseStatisticsTool();
		virtual ~BaseStatisticsTool();


		void SetStatisticsTemplet(const Json::Value& jTemplet);		//设置统计模板
		void SetEnable(const bool& bEnable);
		bool IsEnable() { return m_bEnable; }
		void RefreshStatistics();		//刷新统计
		void AddStatisticsTask(const Vector3d& vPos, const std::string& strKey, const Json::Value& extraValue = Json::nullValue,INT_PTR pTr = 0);	//添加统计任务
		void AddStatisticsTask(const Vector3d& vPos, std::vector<std::string> vecKey, const Json::Value& extraValue = Json::nullValue,INT_PTR pTr = 0);	//添加统计任务
		void StartStatistics(long lIntervalMill = 0);		//开始统计,循环统计间隔时间(毫秒)
		void AddCompleteFunc(StatisticsCompleteFunc pFunc);		//添加回调方法
		void RemoveCompleteFunc(StatisticsCompleteFunc pFunc);
		EventReturnType_e iProcessEvent(const BCEvent& tEvent);
		bool IsCanStatistics() { return m_bCanStatistics; }
		EStatisticsType GetType() { return m_eType; }
		void SetPointParam(BoardParam& tParam);

		//设置外部变量
		virtual void AddINTPTR(std::string strKey, INT_PTR pTr);
		//获取外部变量
		virtual INT_PTR GetINTPTR(std::string strKey);
		//设置json外部变量
		virtual void AddJsonExtra(std::string strKey, Json::Value jValue);
		//获取json外部变量
		virtual Json::Value GetJsonExtra(std::string strKey);
		virtual void Cancel();		//取消
		void SetCreateOrCancelFunc(CreateOrCancelFunc fnCallback) { m_fnCreateOrCancelFunc = fnCallback; }

	protected:
		virtual bool iDoingStatistics(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam) = 0;
		virtual void iCompleteStatistics(ThreadCallbackParam_s* pParam);
		virtual void iLoopStatistics(ThreadCallbackParam_s* pParam);

		virtual void addKey(StatisTaskData *pData);		//匹配成功，进行组装json


		void addLoopStatisticsThread();		//添加循环统计现场


	private:
		static bool static_doing_statistics(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam);
		static void static_complete_statistics(ThreadCallbackParam_s* pParam);
		static void static_loop_statistics(ThreadCallbackParam_s* pParam);

	public:
		BoardParam				m_tPointBoardParam;

	protected:
		EStatisticsType  m_eType;
		bool					m_bEnable;
		State_e					m_eState;
		std::vector<StatisticsCompleteFunc> m_vecCompleteFunc;
		std::vector<StatisTaskData> m_vecStatisTask;
		bool					m_bDoingStatistics;
		Json::Value				m_jTemplet;				//外界传入的模板
		Json::Value				m_jResult;				//结果
		bool					m_bCanStatistics;		//是否可以进行统计
		std::map<std::string, INT_PTR>	m_mapINTPTR;		//外部变量
		std::string				m_strThreadID;
		std::string				m_strLoopThreadID;		//循环的线程id
		long					m_lIntervalMill;		//循环间隔时间（毫秒）
		bool					m_bAddedLoopThread;		//是否已经添加循环线程
		std::vector<StatisTaskData*>	m_vecFilterStatisTask;	//筛选后的
		CreateOrCancelFunc		m_fnCreateOrCancelFunc;	//生成或取消的回调
		PointFactory*					m_pPointFactory;
		std::vector<LocalPointData*>	m_vecPointData;
		Json::Value				m_jExtra;				//额外信息

	};
}


