
#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/animate/EasingAnimate.h"

#include "Utility/LinuxOS/LinuxOS.h"


using namespace bc;

EasingAnimate::EasingAnimate():
	m_eEasingState(EASING_ANIMATE_STATE_STOP),
	m_bLoop(false),
	m_nLoopCount(-1),
	m_nCurLoopCount(0),
	m_nProAllCount(0),
	m_nCurProIndex(0),
	m_lLastTime(0),
	m_pEasing(nullptr),
	m_bLoopSleep(false),
	m_nCurValueIndex(0)
{
	m_pEasing = new Easing();
	m_nLoopIntervalTimeMill = 0;
}

EasingAnimate::~EasingAnimate()
{
	Stop();
	DELETE_PTR(m_pEasing);
}

void EasingAnimate::FrameUpdate()
{
	if (m_eEasingState != EASING_ANIMATE_STATE_PLAY)
	{
		return;
	}

	FloatProValue* pProValue = getCurEasingPointValue();
	if (!pProValue || pProValue->nAllTimeMill <= 0)
	{
		return;
	}

	long lTime = GetTickCount64();
	bool bRun = false;
	if (m_lLastTime == 0)
	{
		m_lLastTime = lTime;
	}
	if (!m_bLoopSleep)
	{
		/*bRun = (lTime - m_lLastTime >= pProValue->nIntervalTimeMill);*/
		bRun = true;
		if (m_nCurProIndex == 0 && m_nCurLoopCount == 0 && m_nCurValueIndex == 0)
		{
			//第一次
			bRun = (lTime - m_lLastTime >= pProValue->nDelayTimeMill);
		}
	}
	else
	{
		bRun = (lTime - m_lLastTime >= m_nLoopIntervalTimeMill);
	}
	bool bStop = false;
	if (bRun)
	{
		m_lLastTime = lTime;

		double dCurValue = 0;
		bool bComplete = false;
		double fRate = 0;
		int nCount = (int)(pProValue->nAllTimeMill / pProValue->nIntervalTimeMill);
		if (m_nCurProIndex >= nCount)
		{
			fRate = 1;
			dCurValue = pProValue->dEndValue - pProValue->dStartValue;
			bComplete = true;
		}
		if (fRate < 1)
		{
			fRate = m_nCurProIndex * (double)pProValue->nIntervalTimeMill / (double)pProValue->nAllTimeMill;
			bool bResult = m_pEasing->Easing_GetProgress(pProValue->nEasingType,
				pProValue->dStartValue,
				pProValue->dEndValue,
				fRate,
				dCurValue);
			if (!bResult)
			{
				fRate = 1;
			}
		}
		if (bStop)
		{
			//手动停止
			Stop();
		}
		if (m_stAnimateParam.pUpdateFunc)
		{
			float fResultRate = abs(dCurValue / (pProValue->dEndValue - pProValue->dStartValue));
			m_stAnimateParam.pUpdateFunc(&m_stAnimateParam, m_mapINTPTR, m_nCurValueIndex, fResultRate, m_nCurLoopCount, m_nLoopCount, bComplete, bStop);
		}
		m_bLoopSleep = false;
		if (fRate == 1)
		{ 
			m_nCurProIndex = 0;
			m_nCurValueIndex++;
			if (m_nCurValueIndex == m_stAnimateParam.vecValue.size())
			{
				//一次完成
				m_nCurLoopCount++;
				if (m_bLoop && (m_nLoopCount == -1 || m_nCurLoopCount < m_nLoopCount))
				{
					//重新开始
					if (m_nLoopCount == -1)
					{
						m_nCurLoopCount = -1;
					}
					m_nCurValueIndex = 0;
				}
				else
				{
					//结束
					Stop();
				}
				m_bLoopSleep = true;
			}
		}
		else
		{
			m_nCurProIndex += 1;
		}
	}
}

void EasingAnimate::SetEasingAnimateParam(EasingAnimateParam stParam)
{
	m_stAnimateParam = stParam;
}

FloatProValue* EasingAnimate::getCurEasingPointValue()
{
	if (m_nCurValueIndex >= m_stAnimateParam.vecValue.size())
	{
		return nullptr;
	}
	return &m_stAnimateParam.vecValue[m_nCurValueIndex];
}

void EasingAnimate::Start()
{
	if (m_eEasingState == EASING_ANIMATE_STATE_PLAY)
	{
		return;
	}
	if (m_stAnimateParam.vecValue.size() == 0)
	{
		return;
	}
	m_eEasingState = EASING_ANIMATE_STATE_PLAY;
}

void EasingAnimate::Pause()
{
	if (m_eEasingState != EASING_ANIMATE_STATE_PLAY)
	{
		return;
	}
	m_eEasingState = EASING_ANIMATE_STATE_PAUSE;
}

void EasingAnimate::Stop()
{
	if (m_eEasingState == EASING_ANIMATE_STATE_STOP)
	{
		return;
	}
	m_eEasingState = EASING_ANIMATE_STATE_STOP;
	m_bLoopSleep = false;
	m_nCurValueIndex = 0;
	m_nCurLoopCount = 0;
	m_nCurProIndex = 0;
}

void EasingAnimate::SetLoop(bool bLoop, int nLoopCount)
{
	m_bLoop = bLoop;
	m_nLoopCount = nLoopCount;
}

void EasingAnimate::AddINTPTR(std::string strKey, INT_PTR pTr)
{
	m_mapINTPTR[strKey] = pTr;
}

INT_PTR EasingAnimate::GetINTPTR(std::string strKey)
{
	return m_mapINTPTR[strKey];
}

