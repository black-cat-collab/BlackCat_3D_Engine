#pragma once

namespace bc
{
	class BaseStatisticsTool;
	class PROJECT_BASED_API CircleStatisticsTool : public BaseStatisticsTool
	{
		enum StatisticsType
		{
			Circle_QX = 0,						//圈选
			Circle_FY							//方圆
		};

	public:
		CircleStatisticsTool(bool bIsPOIStatistics = false);
		virtual ~CircleStatisticsTool();

		void SetStyleParam(PolygonParam& tParam);		//用polygon绘制
		void SetEnable(const bool& bEnable);
		void SetData(const Vector3d& vCenterPos, const float& fRadius);	//设置中心点和半径
		INode* GetPolygonNode() { return m_pPolygon; };

		EventReturnType_e iProcessEvent(const BCEvent& tEvent);

		void calcLinePoint(const Vector3d& vCenterPos, const float& fRadius);		//分割线点
		void calcLinePointGis(const Vector3d& vScreenCenterPos, const float& fRadius);		//分割线点

		virtual void Cancel();
		void	SetRadius(float fRadius);
		void	SetType(int nStatisticsType);

	protected:
		virtual bool iDoingStatistics(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam);

	protected:
		PolygonParam			m_tPolygonParam;
		LocusLineParam			m_tLocusLineParam;
		INode*					m_pPolygon;
		INode*					m_pLocusLine;
		float					m_fRadius;
		Vector3d					m_vCenterPos;
		std::vector<Vector3d>	m_vecLinePoint;
		Vector2					m_vLastMousePos;
		int						m_nStatisticsType;
		bool					m_bIsPOIStatistics;
	};
}


