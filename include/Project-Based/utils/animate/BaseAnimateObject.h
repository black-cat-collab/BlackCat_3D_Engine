#pragma once

namespace bc
{
	class PROJECT_BASED_API BaseAnimateObject
	{
	public:
		BaseAnimateObject(INode* pTargetNode);
		virtual ~BaseAnimateObject();

		virtual void FrameUpdate();
		virtual void Start();
		virtual void Resume();
		virtual void Pause();
		virtual void Stop();

		EasingAnimate* GetEasingAnimate() { return m_pEasingAnimate; }
		void SetAnimateID(const std::string& strAnimateID) { m_strAnimateID = strAnimateID; }
		std::string GetAnimateID() { return m_strAnimateID; }

	protected:
		//设置缓动动画参数
		virtual void iEasingAnimateUpdate(EasingAnimateParam* pEasingParam, std::map<std::string, INT_PTR> m_mapINTPTR,
			int nCurValueIndex, double dCurRate, int nCurLoopCount, int nAllLoopCount, bool bComplete, bool& bManualStop) = 0;
		//设置EasingAnimateParam
		virtual EasingAnimateParam iGetEasingAnimateParam() = 0;

	private:
		static void static_easingAnimate_update_callback(EasingAnimateParam* pEasingParam, std::map<std::string, INT_PTR> mapINTPTR,
			int nCurValueIndex, double dCurRate, int nCurLoopCount, int nAllLoopCount,bool bComplete, bool& bManualStop);

	protected:
		EasingAnimate*		m_pEasingAnimate;
		INode*				m_pTargetNode;
		std::string			m_strAnimateID;

	};
}


