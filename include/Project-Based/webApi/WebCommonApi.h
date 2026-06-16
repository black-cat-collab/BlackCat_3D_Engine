#pragma once

namespace bc
{
	class TweenAnimateForNode;
	class ProjectBasedClient;
	class CircleStatisticsTool;
	class BaseStatisticsTool;
	class StatisTaskData;
	class ThreadCallbackParam_s;
	class WebCommonApi
	{
	public:

		WebCommonApi(ISystemAPI* pSystemAPI, ProjectBasedClient* pClient);
		virtual ~WebCommonApi();

		void						iDoWebCommond(WebCommandData* pWebData);
		EventReturnType_e			iProcessEvent(const BCEvent& tEvent);
		void						iFrameUpdate();
		void						iPostFrameUpdate();

		//设置外部变量
		virtual void				AddINTPTR(std::string strKey, INT_PTR pTr);
		//获取外部变量
		virtual INT_PTR				GetINTPTR(std::string strKey);

	protected:
		bool						m_bRotateAroundCenter;					//相机绕着中心旋转
		Vector3d						m_vAroundCenterStartCameraPos;			//相机旋转开始时的位置
		Vector3d						m_vAroundCenterStartCameraAngle;		//相机旋转开始时的角度
		Vector3d						m_vAroundCenterPos;						//相机旋转开始时的中心点位置
		float						m_fCurAroundCenterAngle;				//相机旋转开始时当前的角度
		float						m_fAroundCenterSpeed;					//相机旋转速度

		std::map<std::string, TweenAnimateForNode*> m_mapTween;				//自定义漫游
		std::string				m_strCurRoamKey;							//当前漫游的路径名称

	private:
		static void	static_callback_PlanningRoadPath(ThreadCallbackParam_s* pParam);	//路径规划回调

	private:
		ProjectBasedClient*				m_pClient;
		std::map<std::string, INT_PTR>	m_mapINTPTR;							//外部变量

		std::string					m_strCurRoamPath;				// 当前的漫游路径
		bool							m_bRoamLoop;					// 是否循环漫游
	};
}

