#pragma once

namespace bc
{
	class CommonLabelFactory;
	class PROJECT_BASED_API SmoothMoveFunc : public IFunc
	{
	public:
		SmoothMoveFunc(CommonLabelFactory *pComFactory);
		virtual ~SmoothMoveFunc();
		ELabelFunc GetFuncType() { return FUNC_SMOOTH_MOVE; }
		virtual EventReturnType_e iProcessEvent(const BCEvent& tEvent);
		CommonLabelFactory* GetComFactory();


	public:
		virtual void iCreateData(FactoryAssemble* pAssemble);
		virtual void iUpdateData(FactoryAssemble* pAssemble);
		virtual void iPreFrameUpdate();
		virtual void iFrameUpdate(FactoryAssemble* pAssemble);
		virtual void iPostFrameUpdate();

		TweenAnimateForNode* GetTweenByID(std::string& strID, bool bCreate);
		void RemoveTweenByID(std::string& strID);
	private:
		static Vector3d static_tween_calcAngle(TweenAnimateForNode* pTween, Vector3d vCurOrigin, Vector3d vPreOrigin);
		static void static_tween_move(TweenAnimateForNode* pTween, INode* pTarget, Vector3d vPos, Vector3d vAngle, bool bPerEndPoint);

	private:
		CommonLabelFactory* m_pComFactory;

	protected:
		std::map<std::string, TweenAnimateForNode*> m_mapTween;
	};
}


