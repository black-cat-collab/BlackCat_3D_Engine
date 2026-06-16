#pragma once

#include "Project-Based/utils/animate/Easing.h"

namespace bc
{
	class EasingAnimateParam;

	// 动画的update回调,bComplete 是否完成，dCurRate 当前的比例,bManualStop 是否手动停止，手动设置,nCurLoopCount 当前循环次数,nCurValueIndex当前的value索引
	typedef void(*EasingAnimateUpdateFunc)(EasingAnimateParam* pEasingParam, std::map<std::string, INT_PTR> mapINTPTR,
		int nCurValueIndex, double dCurRate, int nCurLoopCount, int nAllLoopCount, bool bComplete, bool& bManualStop);

	enum EEasingState
	{
		EASING_ANIMATE_STATE_PLAY,
		EASING_ANIMATE_STATE_PAUSE,
		EASING_ANIMATE_STATE_STOP,
	};

	class BaseProValue
	{
	public:
		int nEasingType;
		int nAllTimeMill;		//总共的时间（毫秒）
		int nIntervalTimeMill;	//间隔时间（毫秒）
		int nDelayTimeMill;		//延时（毫秒）

		BaseProValue()
		{
			nEasingType = EasingType_Linear;
			nAllTimeMill = 0;
			nIntervalTimeMill = (int)(1000 / 30);
			nDelayTimeMill = 0;
		}

		class BaseProValue& operator = (const class BaseProValue& src)
		{
			nEasingType = src.nEasingType;
			nAllTimeMill = src.nAllTimeMill;
			nIntervalTimeMill = src.nIntervalTimeMill;
			nDelayTimeMill = src.nDelayTimeMill;

			return *this;
		}

		void CopyProValue(const class BaseProValue& src)
		{
			BaseProValue::operator=(src);
		}

	};

	class FloatProValue : public BaseProValue
	{
	public:
		double dStartValue;		//开始值
		double dEndValue;		//结束值

		FloatProValue():BaseProValue()
		{
			dStartValue = 0;
			dEndValue = 0;
		}

		class FloatProValue& operator = (const class FloatProValue& src)
		{
			BaseProValue::operator=(src);

			dStartValue = src.dStartValue;
			dEndValue = src.dEndValue;

			return *this;
		}

	};

	class Vector3ProValue : public BaseProValue
	{
	public:
		Vector3d vStart;		//开始值
		Vector3d vEnd;		//结束值

		Vector3ProValue() :BaseProValue()
		{

		}

		class Vector3ProValue& operator = (const class Vector3ProValue& src)
		{
			BaseProValue::operator=(src);

			vStart = src.vStart;
			vEnd = src.vEnd;

			return *this;
		}

	};

	enum EAnimateObjectType
	{
		ANIMATE_OBJECT_TRANSLATE = 0, //移动缓动动画
		ANIMATE_OBJECT_ROTATE,			//旋转缓动动画
		ANIMATE_OBJECT_SCALE,			//大小缩放缓动动画
		ANIMATE_EASING, //Easing动画
	};

	class BaseAnimateParam
	{
	public:
		EAnimateObjectType eType;
		EasingAnimateUpdateFunc pUpdateFunc;		//回调方法

		BaseAnimateParam()
		{
			eType = ANIMATE_OBJECT_TRANSLATE;
			pUpdateFunc = nullptr;
		}

		class BaseAnimateParam& operator = (const class BaseAnimateParam& src)
		{
			eType = src.eType;
			pUpdateFunc = src.pUpdateFunc;

			return *this;
		}

	};

	class EasingAnimateParam : public BaseAnimateParam
	{
	public:
		std::vector<FloatProValue> vecValue;

		EasingAnimateParam() : BaseAnimateParam()
		{
			eType = ANIMATE_EASING;
		}

		class EasingAnimateParam& operator = (const class EasingAnimateParam& src)
		{
			BaseAnimateParam::operator=(src);

			vecValue = src.vecValue;

			return *this;
		}

	};

	class PROJECT_BASED_API EasingAnimate
	{
	public:
		EasingAnimate();
		virtual ~EasingAnimate();

		virtual void FrameUpdate();
		virtual void Start();
		virtual void Pause();
		virtual void Stop();


		//设置外部变量
		void AddINTPTR(std::string strKey, INT_PTR pTr);
		//获取外部变量
		INT_PTR GetINTPTR(std::string strKey);
		void SetEasingAnimateParam(EasingAnimateParam stParam);
		EasingAnimateParam* GetEasingAnimateParam() { return &m_stAnimateParam; }
		EEasingState		GetEasingState() { return m_eEasingState; }

		virtual void		SetLoop(bool bLoop, int nLoopCount = 1);
		bool				IsLoop() { return m_bLoop; }
		int					GetLoopCount() { return m_nLoopCount; }

		virtual void		SetLoopIntervalTime(int nTimeMill) { m_nLoopIntervalTimeMill = nTimeMill; }
		int					GetLoopIntervalTime() { return m_nLoopIntervalTimeMill; }

	protected:
		//获取当前的点位value
		FloatProValue* getCurEasingPointValue();		


	protected:
		std::map<std::string, INT_PTR>	m_mapINTPTR;		//外部变量
		EasingAnimateParam			m_stAnimateParam;
		EEasingState				m_eEasingState;		//状态
		bool						m_bLoop;
		int							m_nLoopCount;
		int							m_nCurLoopCount;	//当前的循环次数

		int							m_nProAllCount;		//总共的过程数量
		int							m_nCurProIndex;		//当前的过程索引
		long						m_lLastTime;		//上一次的时间
		Easing*						m_pEasing;
		bool						m_bLoopSleep;		//是否是循环间隔时间
		int							m_nCurValueIndex;	//当前取值的index
		int							m_nLoopIntervalTimeMill;	//循环每次间隔时间（毫秒）

	};
}


