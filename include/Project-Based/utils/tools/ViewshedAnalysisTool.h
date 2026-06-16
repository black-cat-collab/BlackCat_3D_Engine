#pragma once

namespace bc
{
	class ViewshedState
	{
	public:
		Vector3d vOrigin;
		Vector3d vAngles;
		float fHorizontal, fVertical, fClip;
		ViewshedState();
		ViewshedState(Vector3d vOriginArg, Vector3d vAnglesArg, float fHorizontalArg, float fVerticalArg, float fClipArg);
		ViewshedState& operator=(const ViewshedState& vCopy);
		bool IsChange(const ViewshedState& vCompare);
	};

	enum EViewshedType
	{
		VIEWSHED_TYPE_SINGLE = 0,		//单面视域分析
		VIEWSHED_TYPE_MULTI = 0,		//多面视域分析
	};

	class ViewshedParam {
	public:
		float fFOV;
		float fFarClip;
		Vector3d vVisibleColor;			//可见颜色
		Vector3d vInvisibleColor;		//不可见颜色
		Vector4 vLineColor;				//线颜色
	};

	class PROJECT_BASED_API ViewshedAnalysisTool
	{
	public:
		ViewshedAnalysisTool();
		virtual ~ViewshedAnalysisTool();

		void Create();
		void SetViewshedType(EViewshedType eType);
		void SetAngle(Vector3d vAngle);
		void SetOrigin(Vector3d vOrigin);
		void SetHFov(float fFov);
		void SetVFov(float fFov);
		float GetVFov() { return m_fVFov; }
		void SetFarClip(float fFarClip);
		void SetVisibleColor(Vector3d vVisibleColor);
		void SetInvisibleColor(Vector3d vInvisibleColor);
		void SetLineColor(Vector4 vLineColor);
		void Start(bool bGrapScenePoint);
		void SetOffsetZ(float fOffsetZ);
		void Stop();
		IViewshedAnalysis* GetViewshedAnalysis() { return m_pViewshedAnalysisView; }

		EventReturnType_e iProcessEvent(const BCEvent& tEvent);

	protected:
		EViewshedType		m_eViewshedType;
		IView* m_pView;
		IViewshedAnalysis* m_pViewshedAnalysisView;
		bool				m_bGrapScenePoint;
		float				m_fOffsetZ;
		float				m_fVFov;		//垂直fov
	};
}

