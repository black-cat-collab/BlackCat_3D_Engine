#pragma once

namespace bc
{
	// 统计回调 vPos筛选的位置，jResult 统计的最终结果，pHandleNode 参与统计做交集的节点
	typedef void(*DistanceMeasureCompleteFunc)(INode* pLocusLine, std::map<std::string, INT_PTR> mapINTPTR, std::vector<float> vecResult);
	class PROJECT_BASED_API DistanceMeasureTool
	{
	protected:
		enum State_e
		{
			STATE_NONE = 0,	//无状态（此状态下才能开始新的一次绘制）
			STATE_MOVE,		//移动
			STATE_END,		//结束
		};

	public:
		DistanceMeasureTool();
		virtual ~DistanceMeasureTool();

		void SetStyleParam(LocusLineSegment& tSegment);		
		void SetEnable(const bool& bEnable);
		void SetFontBoardParam(FontBoardParam& tParam,std::string& strTextID);		//文字面板
		void SetStartPointBoardParam(BoardParam &tParam);		//设置开始点的图样
		void SetEndPointBoardParam(BoardParam& tParam);		//设置开始点的图样

		EventReturnType_e iProcessEvent(const BCEvent& tEvent);
		virtual void Cancel();

		//设置外部变量
		virtual void AddINTPTR(std::string strKey, INT_PTR pTr);
		//获取外部变量
		virtual INT_PTR GetINTPTR(std::string strKey);
		void AddCompleteFunc(DistanceMeasureCompleteFunc pFunc);		//添加回调方法
		void RemoveCompleteFunc(DistanceMeasureCompleteFunc pFunc);
		void SetShowFontBoard(bool bShow) { m_bShowFontBoard = bShow; }

	protected:
		bool					m_bEnable;
		FontBoardParam			m_tFontBoardParam;
		std::string				m_strFontBoardTextID;
		LocusLineParam			m_tLocusLineParam;
		INode*					m_pLocusLine;
		INode*					m_pFontBoard;
		std::vector<INode*>		m_vecTempFontBoard;
		std::vector<Vector3d>	m_vecLinePoint;
		std::map<std::string, INT_PTR>	m_mapINTPTR;		//外部变量
		State_e					m_eState;
		DistanceMeasureCompleteFunc		m_fnMeasureComplete;
		std::vector<DistanceMeasureCompleteFunc> m_vecCompleteFunc;
		bool					m_bShowFontBoard;
		Vector2					m_vLastMousePos;
		INode*					m_pStartPontBoard;
		std::vector<INode*>		m_vecTempPontBoard;		//中间点位
		BoardParam				m_tBoardParam;			//中间点位的样式
		INode*					m_pEndPontBoard;
		std::vector<float>		m_vecDistance;			//记录每一段的距离
	};
}


