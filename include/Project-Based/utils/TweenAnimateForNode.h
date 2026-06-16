#pragma once

namespace bc
{
	class TweenAnimateForNode;
	class PointData
	{
	public:
		Vector3d vPos;
		Vector3d vAngle;
		float fSpeed;

		PointData()
		{
			fSpeed = -1.0f;
		}
	};

	enum TweenAnimateStatus_e
	{
		TWEEN_ANIMATE_STATUS_PLAY,
		TWEEN_ANIMATE_STATUS_PAUSE,
		TWEEN_ANIMATE_STATUS_STOP,
	};

	typedef void(*TweenAnimateMoveFunc)(TweenAnimateForNode *pTween, INode *pTarget,Vector3d vPos,Vector3d vAngle,bool bPerEndPoint);		//移动的回调
	typedef void(*TweenAnimateEndFunc)(TweenAnimateForNode* pTween, INode *pTarget,int nCurLoopNum,int nLoopCount);		//本次结束
	typedef Vector3d(*TweenAnimateCalcAngleFunc)(TweenAnimateForNode* pTween,Vector3d vCurOrigin,Vector3d vPreOrigin);

	class PROJECT_BASED_API TweenAnimateForNode
	{
	public:
		TweenAnimateForNode(INode* pTargetNode = NULL);
		virtual ~TweenAnimateForNode();

		void SetSpeedUniform(bool bUniform) { m_bSpeedUniform = bUniform; }
		virtual void Update();		//在每帧的地方调用即可
		void SetPoints(std::vector<PointData> vecPoints);
		void AddPoint(PointData vPoint);		//单点添加
		void SetLoop(bool bLoop,int nCount = -1);		//设置循环,nCount=-1 表示无线循环
		void SetPathEndCallback(TweenAnimateEndFunc fnFunc) { m_fnPathEndCallback = fnFunc; }
		void SetMoveCallback(TweenAnimateMoveFunc fnFunc) { m_fnMoveCallback = fnFunc; }
		void SetCalcAngleCallback(TweenAnimateCalcAngleFunc fnFunc) { m_fnCalcAngleCallback = fnFunc; }
		// km/h
		void SetSpeed(float fSpeedMeterPerSecond) { m_fSpeed = fSpeedMeterPerSecond; }
		float GetCurSpeed();
		void SetCurrentIndex(int nIndex);
		TweenAnimateStatus_e GetCurrentStatus() { return m_eCurrentAnimateStatus; }
		//设置是否延迟
		void Start(long fDelayMillsecond = 0);
		void Pause();
		void Stop();
		void ResetPosition();		//回到初始点
		int GetCurrentIndex() { return m_nCurrentIndex; }
		PointData* GetCurrentPointData();
		std::vector<PointData> GetAllPoints() { return m_vecPointData; }
		void SetStopOnEnd(bool bStopOnEnd) { m_bStopOnEnd = bStopOnEnd; }
		//当达到这个数后，把之前的删除
		void SetPointMaxCount(int nPointMax);
		int  GetPointMaxCount() { return m_nPointMaxCount; }
		bool IsLoop() { return m_bLoop; }
		void SetPerFramePoint(bool bPreFrame) { m_bPerFramePoint = bPreFrame; }
		void SetDelayBySelfFrame(bool bSelfFrame) { m_bDelayBySelfFrame = bSelfFrame; }
		//两点间最短的时间（大于这个时间则不插值，直接到下一个点，目的是防止两个点太远而造成堆积，不实时的问题）
		void SetMin2PointTimeSecond(float fScecond) { m_fMin2PointTimeSecond = fScecond; }
		//删除所有的点位
		void ClearAllPoint();	
		void SetTargetNode(INode* pNode) { m_pTargetNode = pNode; }
		void SetExtraData(std::string strKey, INT_PTR pTr);
		INT_PTR GetExtraData(std::string strKey);
		void SetExtraJValue(std::string strKey, Json::Value jValue);
		Json::Value GetExtraJValue(std::string strKey);
		Vector3d iGetPreOrigin() { return m_vPreOrigin; }
		Vector3d iGetPreAngle() { return m_vPreAngle; }

	protected:
		virtual void Move();

	public:
		std::map<std::string, INT_PTR> m_mapINTPTR;			//指针存储区
		std::map<std::string, Json::Value> m_mapJValue;		//变量存储区

	private:
		INode* m_pTargetNode;

		bool m_bSpeedUniform;		//是否匀速
		float m_fSpeed;			//当匀速的时候则就是速度(km/h)
		long m_lDelayStart;		//毫秒
		long m_lPreTime;
		float m_fCurDistance;
		float m_fDistance;
		int m_nCurrentIndex;		//当前点位
		bool m_bLoop;			//是否循环
		int m_nLoopCount;		//循环的次数
		int m_nCurLoopNum;		//当前的次数
		bool m_bStopOnEnd;		//到最后是否停止

		Vector3d m_vSrcPoint;
		Vector3d m_vDstPoint;
		Vector3d m_vSrcAngles;
		Vector3d m_vDstAngles;
		Vector3d m_vOffsetPosition;
		Quaternion m_qDstQuaternion;
		Quaternion m_qSrcQuaternion;

		std::vector<PointData> m_vecPointData;		//点位数据
		TweenAnimateStatus_e m_eCurrentAnimateStatus;		//当前状态

		TweenAnimateEndFunc m_fnPathEndCallback;		//每次完成的回调
		TweenAnimateMoveFunc m_fnMoveCallback;		//没跳一个点的回调
		TweenAnimateCalcAngleFunc m_fnCalcAngleCallback;	//角度计算
		int m_nPointMaxCount;						//移动点的最大数  
		bool				m_bPerFramePoint;			//是否每帧走一个点
		bool				m_bDelayBySelfFrame;		//根据程序的时间来延迟（否：系统）
		float				m_fCurDelaySelfFrame;			//当前的系统渲染时间
		float				m_fStartAngleDis;

		Vector3d				m_vPreOrigin;
		Vector3d				m_vPreAngle;
		//两点间最短的时间（大于这个时间则不插值，直接到下一个点，目的是防止两个点太远而造成堆积，不实时的问题）
		float				m_fMin2PointTimeSecond;			

	};
}


