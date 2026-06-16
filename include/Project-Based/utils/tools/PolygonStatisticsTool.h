#pragma once

namespace bc
{
	enum PolygonStatisticsType 
	{
		POLYGON = 0,		//统计多边形
		RECTANGULAR			//统计矩形	
	};

	class PROJECT_BASED_API PolygonStatisticsTool : public BaseStatisticsTool
	{
	public:
		PolygonStatisticsTool(PolygonStatisticsType mType = POLYGON);
		virtual ~PolygonStatisticsTool();

		void SetStyleParam(PolygonParam& tParam);		//用polygon绘制
		void SetEnable(const bool& bEnable);
		void SetData(std::vector<Vector3d> vecPoints);	
		void SetPolygon(std::vector<INode*> vecPolygon);	//传入外部polygon
		void GetAllPolygon(std::vector<INode*>& vecPolygon);
		void GetPolygonParam(PolygonParam& tParam);

		EventReturnType_e iProcessEvent(const BCEvent& tEvent);
		virtual void Cancel();

	protected:
		virtual bool iDoingStatistics(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam);
		virtual void iCompleteStatistics(ThreadCallbackParam_s* pParam);
		virtual void iLoopStatistics(ThreadCallbackParam_s* pParam);

		bool		IsInPolygon(INode* pNode, Vector3 vOrigin);

	protected:
		PolygonParam			m_tPolygonParam;
		LocusLineParam			m_tLocusLineParam;
		std::vector<INode*>		m_vecPolygon;
		INode*					m_pPolygon;
		INode*					m_pLocusLine;
		std::vector<Vector3d>	m_vecLinePoint;
		bool					m_bOutsizePolygon;	//外部出入的polygon
		Vector2					m_vLastMousePos;
		PolygonStatisticsType	m_ePolygonStatisticsType;
	};
}


