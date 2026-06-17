#pragma once

namespace hc
{
	class TweenAnimateForNode;
	class TranslateAnimateObject : public IAnimateWithNode
	{
	public:
		TranslateAnimateObject(INode* pTarget, TranslateAnimateParam stParam);
		virtual ~TranslateAnimateObject();

		TranslateAnimateParam* iGetAnimateParam();

	public:
		virtual EAnimateObject iGetAnimateObjectType();
		virtual void iFrameUpdate();

	protected:
		virtual void iMove(map<string, INT_PTR> mapINTPTR, INode* pTarget, Vector3 vPos,
			Vector3 vAngle, bool bPerEndPoint);
		virtual void iEnd(map<string, INT_PTR> mapINTPTR, INode* pTarget, int nCurLoopNum, int nLoopCount);

	private:
		static Vector3 Callback_CalcAngle(Vector3 vCurOrigin, Vector3 vPreOrigin);
		static void Callback_Move(map<string, INT_PTR> mapINTPTR, INode* pTarget, Vector3 vPos,
			Vector3 vAngle, bool bPerEndPoint);		//移动的回调
		static void Callback_PathEnd(map<string, INT_PTR> mapINTPTR, INode* pTarget, int nCurLoopNum, int nLoopCount);		//本次结束

	protected:
		TranslateAnimateParam m_stAnimateParam;
		INode*				  m_pTargetNode;
		TweenAnimateForNode* m_pTween;

	};
}


