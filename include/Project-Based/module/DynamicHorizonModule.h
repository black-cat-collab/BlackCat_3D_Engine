#pragma once

namespace bc
{
	class TweenAnimateForNode;
	typedef struct HorizonPathParam
	{
		float					m_fDistance;
		std::string				m_strId;
		int						m_nLoop;
		std::string				m_strName;
		std::vector<Vector3d>	m_vecPoints;
		float					m_fSpeed;
		Vector3d					m_vec3VisibleColor;
		Vector3d					m_vec3UnvisibleColor;
		HorizonPathParam()
		{
			m_fDistance = 400;
			m_strId = "0";
			m_nLoop = -1;
			m_fSpeed = 20;
			m_strName = "";
			m_vecPoints.resize(0);
			m_vec3VisibleColor = Vector3d(46, 223, 31);
			m_vec3UnvisibleColor = Vector3d(223, 42, 42);
		}
		struct HorizonPathParam& operator=(const struct HorizonPathParam& src)
		{
			m_fDistance = src.m_fDistance;
			m_vec3VisibleColor = src.m_vec3VisibleColor;
			m_strId = src.m_strId;
			m_nLoop = src.m_nLoop;
			m_fSpeed = src.m_fSpeed;
			m_vec3UnvisibleColor = src.m_vec3UnvisibleColor;
			m_strName = src.m_strName;
			m_vecPoints = src.m_vecPoints;
			return *this;
		}
	}HorizonPathParam;

	class PROJECT_BASED_API DynamicHorizonModule : public BaseModule
	{
	protected:
		enum State_e
		{
			STATE_NONE = 0,	//无状态（此状态下才能开始新的一次绘制）
			STATE_MOVE,		//移动
			STATE_END,		//结束
		};
	public:
		DynamicHorizonModule(const std::string& strModuleName);
		virtual ~DynamicHorizonModule();

	public:
		virtual std::string			iGetModuleName();
		virtual INode* iGetRootNode();

		virtual void				iStateChangeUpdate(IStateManager* pStateManager);
		virtual void				iDataChangeUpdate(IStateManager* pStateManager);
		virtual void				iFrameUpdate(IStateManager* pStateManager);
		virtual void				iInitializeScene(IStateManager* pStateManager);
		virtual void				iSceneLoad(IStateManager* pStateManager);
		virtual EventReturnType_e	iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent);
		virtual EventReturnType_e	iExecuteEvent(IStateManager* pStateManager, const BCEvent& tEvent);

		virtual void				iRefreshData();
		virtual void				iDoWebCommond(WebCommandData* pWebData);	//处理网页数据

		virtual INode* iGetModuleGroupNode();
		virtual void				iOnAppModeChanged(int nAppMode);
	public:
		void						SetLineEnable(bool bEnable);
		void						Cancel();
	private:
		//划线
		State_e						m_eState;
		Vector2						m_vLastMousePos;
		std::vector<Vector3d>		m_vecLinePoint;
		LocusLineParam				m_tLocusLineParam;
		INode* m_pLocusLine;
		INode* m_pLine;
		bool						m_bEnable;

		std::map<std::string, HorizonPathParam*>	m_mapPathParam;	//自定义视域分析
		std::map<std::string, TweenAnimateForNode*> m_mapTween;
		IView* m_pView;
		IViewshedAnalysis* m_pViewshedAnalysis;
		std::string									m_strCurrentRoamId;
		static void static_move(TweenAnimateForNode* pTween, INode* pTarget, Vector3d vPos, Vector3d vAngle, bool bPerEndPoint);
		static Vector3d static_caclAngleCallbake(TweenAnimateForNode* pTween, Vector3d vCurOrigin, Vector3d vPreOrigin);
	protected:
		//获取所有的漫游路线
		virtual void				getAllHorizonPaths(bool bThread, ThreadCallbackParam* pCallback = nullptr);
		static void					static_CallbackAllHorizonPaths(ThreadCallbackParam* pParam);
	};
}
