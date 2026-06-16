#pragma once

namespace bc
{
	class SightAnalysisTool;
	// 分析回调，jResult 统计的最终结果，pLineNode线的节点，
	typedef void(*SightAnalysisCompleteFunc)(INode* pLineNode, SightAnalysisTool *pTool, Json::Value jResult);

	class PROJECT_BASED_API SightAnalysisTool
	{
	protected:
		enum State_e
		{
			STATE_NONE = 0,	//无状态（此状态下才能开始新的一次绘制）
			STATE_MOVE,		//移动
			STATE_END,		//结束
		};

	public:
		SightAnalysisTool();
		~SightAnalysisTool();

		virtual void Start(Vector3d& vStartPos, Vector3d& vEndPos);
		virtual void Start();
		virtual void Stop();
		virtual void Cancel();
		virtual EventReturnType_e iProcessEvent(const BCEvent& tEvent);

		void SetVisibleColor(Vector4& vColor) { m_vVisibleColor = vColor; }
		void SetInVisibleColor(Vector4& vColor) { m_vInVisibleColor = vColor; }
		void SetLineWidth(float fWidth) { m_fLineWidth = fWidth; }
		//设置外部变量
		void AddINTPTR(std::string strKey, INT_PTR pTr);
		//获取外部变量
		INT_PTR GetINTPTR(std::string strKey);
		void AddExtra(std::string strKey, Json::Value jValue) { m_jExtra[strKey] = jValue; }
		Json::Value GetExtra(std::string strKey) { return m_jExtra[strKey]; }
		void AddCompleteFunc(SightAnalysisCompleteFunc pFunc);		//添加回调方法
		void RemoveCompleteFunc(SightAnalysisCompleteFunc pFunc);


	protected:
		void doAnalyze(Vector3d& vStart, Vector3d& vEnd);
		void createLineNode();
		
	protected:
		Vector4 m_vVisibleColor;
		Vector4 m_vInVisibleColor;
		Vector4 m_vNoAnalyzeVisibleColor;		//还未分析的线颜色
		float m_fLineWidth;
		std::map<std::string, INT_PTR> m_mapINTPTR;		//外部变量
		bool m_bEnable;
		INode* m_pLineNode;
		std::vector<SightAnalysisCompleteFunc> m_vecCompleteFunc;
		State_e	 m_eState;
		Vector3d m_vMouseStartPos;
		Vector3d m_vMouseEndPos;
		Vector2	m_vLastMousePos;
		std::vector<Vector3d>	m_vecLinePoint;
		Json::Value m_jExtra;
	};
}


