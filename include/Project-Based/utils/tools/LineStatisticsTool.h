#pragma once

namespace bc
{
	class PROJECT_BASED_API LineStatisticsTool : public BaseStatisticsTool
	{
	public:
		LineStatisticsTool();
		virtual ~LineStatisticsTool();

		void SetStyleParam(RoadParam& tParam);		
		void SetLineParam(LocusLineSegment& tParam);		//用电线的绘制
		void SetRoadWidth(float &fRoadWidth);		//设置生成Road的宽度
		void SetEnable(const bool& bEnable);
		void SetData(std::vector<Vector3d> vecPoints);	
		void SetRoad(std::vector<INode*> vecRoad);	//传入外部Road
		void GetAllRoad(std::vector<INode*>& vecRoad);

		EventReturnType_e iProcessEvent(const BCEvent& tEvent);
		virtual void Cancel();

	protected:
		virtual bool iDoingStatistics(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam);
		virtual void iCompleteStatistics(ThreadCallbackParam_s* pParam);
		virtual void iLoopStatistics(ThreadCallbackParam_s* pParam);

		bool		IsInRoad(INode* pNode, Vector3 vOrigin);

	protected:
		RoadParam				m_tRoadParam;
		LocusLineParam			m_tLocusLineParam;
		std::vector<INode*>		m_vecRoad;
		INode*					m_pRoad;
		INode*					m_pLocusLine;
		std::vector<Vector3d>	m_vecLinePoint;
		bool					m_bOutsizePolygon;	//外部出入的polygon
		float					m_fRoadWidth;
		Vector2					m_vLastMousePos;

	};
}


