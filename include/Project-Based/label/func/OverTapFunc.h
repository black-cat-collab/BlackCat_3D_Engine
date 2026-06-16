#pragma once

namespace bc
{
	class CommonLabelFactory;
	class OverTapFuncFactory;
	class PROJECT_BASED_API OverTapFunc : public IFunc
	{
	public:
		class OverTapData : public ProjectBasedSceneElement
		{
		public:
			std::vector<FactoryAssemble*> vecData;
			bool bExpand;	//是否展开

			OverTapData():
				ProjectBasedSceneElement()
			{
				bExpand = false;
			}

			OverTapData& operator = (const OverTapData& src)
			{
				vecData = src.vecData;
				bExpand = src.bExpand;

				return *this;
			}

			~OverTapData()
			{
			}
		};

		class PageData
		{
		public:
			std::vector<OverTapData*> vecResult;
			std::vector<FactoryAssemble*> vecSingleAssemble;
		};

	public:
		OverTapFunc(CommonLabelFactory *pComFactory);
		virtual ~OverTapFunc();
		ELabelFunc GetFuncType() { return FUNC_OVERTAP; }
		virtual EventReturnType_e iProcessEvent(const BCEvent& tEvent);
		CommonLabelFactory* GetComFactory();
		CommonLabelFactory* GetOverTapLabelFactory();

		//启用线程计算重叠
		void Start();
		void Stop();
		void SetAssembleStatus(FactoryAssemble* pAssemble, bool bOverTap);	//设置Assemble的重叠状态
		void SetJudgeDis(float fDis) { m_fOverTapJudgeDis = fDis; }

	public:
		virtual void iCreateData(FactoryAssemble* pAssemble);
		virtual void iUpdateData(FactoryAssemble* pAssemble);
		virtual void iPreFrameUpdate();
		virtual void iFrameUpdate(FactoryAssemble* pAssemble);
		virtual void iPostFrameUpdate();
		virtual void iShowAssembleForLabel(FactoryAssemble* pAssemble);
		virtual void iHideAssembleForLabel(FactoryAssemble* pAssemble);
		virtual void iChangeVisible(bool bAllVisible, const std::vector<int>& vecVisibleType);
		virtual bool isAllStop();

	private:
		static void static_request_doingCalcTaskTread(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam);
		static void static_callback_doingCalcTaskTread(ThreadCallbackParam_s* pParam);

		void clear();
		void	doingCalcTaskTread();		//线程计算

	private:
		CommonLabelFactory* m_pComFactory;

	protected:
		bool		m_bNeedRefresh;		//是否刷新
		bool		m_bStarted;
		bool		m_bThreadRunning;

		std::vector<OverTapData*> m_vecAllOverTapData;		//全部重叠归类
		std::vector<OverTapData*> m_vecCurOverTapData;		//当前重叠归类
		int		m_nCurOverTapDataIndex;			//当前使用的data索引
		float   m_fOverTapJudgeDis;			//判定距离，在这个距离内就算重叠

		OverTapFuncFactory* m_pLabelFactory;
		std::atomic<bool> m_bThreadRealStop;
	};
}


