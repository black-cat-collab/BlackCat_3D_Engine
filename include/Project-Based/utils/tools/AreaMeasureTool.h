#pragma once

namespace bc
{
	// 统计回调 vPos筛选的位置，jResult 统计的最终结果，pHandleNode 参与统计做交集的节点
	typedef void(*AreaMeasureCompleteFunc)(INode* pPolygon, std::map<std::string, INT_PTR> mapINTPTR, float fResult);

	class CommonLabelFactory;
	class PROJECT_BASED_API AreaMeasureTool
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
		AreaMeasureTool();
		virtual ~AreaMeasureTool();

		void SetStyleParam(PolygonParam& tParam);		//用polygon绘制
		void SetEnable(const bool& bEnable);
		void SetFontBoardParam(FontBoardParam& tParam,std::string& strTextID);		//文字面板
		void SetPointParam(BoardParam& tParam);

		EventReturnType_e iProcessEvent(const BCEvent& tEvent);
		virtual void Cancel();

		//设置外部变量
		virtual void AddINTPTR(std::string strKey, INT_PTR pTr);
		//获取外部变量
		virtual INT_PTR GetINTPTR(std::string strKey);
		void AddCompleteFunc(AreaMeasureCompleteFunc pFunc);		//添加回调方法
		void RemoveCompleteFunc(AreaMeasureCompleteFunc pFunc);
		void SetShowFontBoard(bool bShow) { m_bShowFontBoard = bShow; }

	protected:
		double	calcArea(std::vector<Vector3d> &vecPoint);

	public:
		BoardParam				m_tPointBoardParam;

	protected:
		bool					m_bEnable;
		PolygonParam			m_tPolygonParam;
		FontBoardParam			m_tFontBoardParam;
		std::string				m_strFontBoardTextID;
		LocusLineParam			m_tLocusLineParam;
		INode*					m_pPolygon;
		INode*					m_pLocusLine;
		INode*					m_pFontBoard;
		std::vector<Vector3d>	m_vecLinePoint;
		std::map<std::string, INT_PTR>	m_mapINTPTR;		//外部变量
		State_e					m_eState;
		AreaMeasureCompleteFunc		m_fnMeasureComplete;
		std::vector<AreaMeasureCompleteFunc> m_vecCompleteFunc;
		bool					m_bShowFontBoard;
		Vector2					m_vLastMousePos;
		PointFactory*			m_pPointFactory;
		std::vector<LocalPointData*> m_vecPointData;

	};
}


