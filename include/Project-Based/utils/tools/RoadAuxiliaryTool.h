namespace bc 
{
	class ProjectBasedClient;
	class PROJECT_BASED_API RoadAuxiliaryTool final
	{
	public:
		RoadAuxiliaryTool(ProjectBasedClient* pClient);
		~RoadAuxiliaryTool();
		//路线创建 绘制线
		void					SetLineEnable(bool bEnable);
		void					CancelRoadLine();
		void					SetRoadParam(RoadParam& tParam);
		void				    GetRoadParam(RoadParam& tParam);
		EventReturnType_e		iProcessEvent(const BCEvent& tEvent);
		void					CreateRoadNode(RoadParam* pParam);
		INode*					GetCurRoadNode();
		std::vector<Vector3d>&	GetPosForRoad() { return m_vecPosForRoad; };

	protected:
		enum class State_e
		{
			STATE_NONE = 0,	//无状态（此状态下才能开始新的一次绘制）
			STATE_MOVE,		//移动
			STATE_END,		//结束
		};

		//创建路相关的
		LocusLineParam				m_tLocusLineParam;
		INode*						m_pLocusLineRoad;
		State_e						m_eState;
		Vector2						m_vLastMousePos;
		bool						m_bCollectedPosForRoad;
		RoadParam					m_tRoadParam;
		std::vector<Vector3d>	    m_vecPosForRoad;

		//创建的当前结点
		INode*						m_pRoadNode;

		ProjectBasedClient*			m_pClient;
	};
}

