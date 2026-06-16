#pragma once

namespace bc
{
	typedef struct RoamPathParam
	{
		float					m_fAngle;
		float					m_fHeight;
		std::string				m_strId;
		int						m_nLoop;
		float					m_fSpeed;
		float					m_fTransitionTime;
		std::string				m_strName;
		std::vector<Vector3d>	m_vecPoints;
		RoamPathParam()
		{
			m_fAngle = -45;
			m_fHeight = 200;
			m_strId = "0";
			m_nLoop = -1;
			m_fSpeed = 20;
			m_fTransitionTime = 2.0f;
			m_strName = "";
			m_vecPoints.resize(0);
		}
		struct RoamPathParam& operator=(const struct RoamPathParam& src)
		{
			m_fAngle = src.m_fAngle;
			m_fHeight = src.m_fHeight;
			m_strId = src.m_strId;
			m_nLoop = src.m_nLoop;
			m_fSpeed = src.m_fSpeed;
			m_fTransitionTime = src.m_fTransitionTime;
			m_strName = src.m_strName;
			m_vecPoints = src.m_vecPoints;
			return *this;
		}
	}RoamPathParam;

	class TweenAnimateForNode;
	class PROJECT_BASED_API RoamPathModule : public BaseModule
	{
	protected:
		enum State_e
		{
			STATE_NONE = 0,	//无状态（此状态下才能开始新的一次绘制）
			STATE_MOVE,		//移动
			STATE_END,		//结束
		};
	public:
		RoamPathModule(const std::string& strModuleName);
		virtual ~RoamPathModule();

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

		std::map<std::string, RoamPathParam*> m_mapPathParam;	//自定义漫游
		std::map<std::string, TweenAnimateForNode*> m_mapTween;
		std::string m_strCurrentRoamId;
		static void static_move(TweenAnimateForNode* pTween, INode* pTarget, Vector3d vPos, Vector3d vAngle, bool bPerEndPoint);
		static Vector3d static_caclAngleCallbake(TweenAnimateForNode* pTween, Vector3d vCurOrigin, Vector3d vPreOrigin);
	protected:
		//获取所有的漫游路线
		virtual void				getAllRoamPaths(bool bThread, ThreadCallbackParam* pCallback = nullptr);
		static void					static_CallbackAllRoamPaths(ThreadCallbackParam* pParam);
	};
}
