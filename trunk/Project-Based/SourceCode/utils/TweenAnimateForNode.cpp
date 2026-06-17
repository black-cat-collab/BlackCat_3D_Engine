
#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/TweenAnimateForNode.h"
#include "Utility/LinuxOS/LinuxOS.h"


using namespace bc;

#define LOCUS_MAX_POINT  5000
TweenAnimateForNode::TweenAnimateForNode(INode* pTargetNode)
{
	m_pTargetNode = pTargetNode;
	m_nCurrentIndex = 0;
	m_bSpeedUniform = false;
	m_fSpeed = 30.0f;
	m_fCurDistance = 0.0f;
	m_fDistance = 0.0f;
	m_eCurrentAnimateStatus = TWEEN_ANIMATE_STATUS_STOP;
	m_bLoop = false;
	m_nLoopCount = 1;
	m_nCurLoopNum = 1;
	m_fnPathEndCallback = NULL;
	m_fnMoveCallback = NULL;
	m_fnCalcAngleCallback = NULL;
	m_bStopOnEnd = true;
	m_nPointMaxCount = LOCUS_MAX_POINT;
	m_bPerFramePoint = false;
	m_bDelayBySelfFrame = false;
	m_fStartAngleDis = 0;
	m_fMin2PointTimeSecond = -1;
}


TweenAnimateForNode::~TweenAnimateForNode()
{
	Stop();
}

void TweenAnimateForNode::Update()
{
	Move();
}

void TweenAnimateForNode::SetLoop(bool bLoop, int nCount)
{ 
	m_bLoop = bLoop;
	m_nLoopCount = nCount;
}

void TweenAnimateForNode::SetCurrentIndex(int nIndex) 
{
	m_nCurrentIndex = nIndex; 
	m_fCurDistance = 0.0f;		//立即生效
	if (m_nCurrentIndex > m_vecPointData.size()-1)
	{
		m_nCurrentIndex = m_vecPointData.size() - 1;
	}
}

float TweenAnimateForNode::GetCurSpeed()
{
	PointData *ptData = GetCurrentPointData();
	if (!ptData)
	{
		return m_fSpeed;
	}
	if (ptData->fSpeed < 0)
	{
		return m_fSpeed;
	}
	return ptData->fSpeed;
}

void TweenAnimateForNode::SetPoints(std::vector<PointData> vecPoints)
{
	//计算角度
	m_vecPointData.clear();
	m_vecPointData.resize(0);
	m_vecPointData = vecPoints;
	for (int i = 1; i< m_vecPointData.size(); ++i)
	{
		if (!m_vecPointData[i].vAngle.Empty())
		{
			continue;
		}
		Vector3d vAngle;
		if (m_fnCalcAngleCallback)
		{
			vAngle = m_fnCalcAngleCallback(this, m_vecPointData[i].vPos, m_vecPointData[i-1].vPos);
		}
		m_vecPointData[i - 1].vAngle = vAngle;
		if (i == m_vecPointData.size() - 1)
		{
			//最后一个
			m_vecPointData[i].vAngle = vAngle;
		}
	}

	if (m_nCurrentIndex > m_vecPointData.size() - 1)
	{
		m_nCurrentIndex = m_vecPointData.size() - 1;
	}
}

void TweenAnimateForNode::AddPoint(PointData vPoint)
{
	if (m_vecPointData.size() > 0)
	{
		PointData& sLastPoint = m_vecPointData[m_vecPointData.size() - 1];
		if (sLastPoint.vPos == vPoint.vPos && sLastPoint.vAngle == vPoint.vAngle)
		{
			return;
		}
	}
	m_vecPointData.push_back(vPoint);
	if (m_vecPointData.size() >= 2 && vPoint.vAngle.Empty())
	{
		int nSize = m_vecPointData.size();
		Vector3d vCur = m_vecPointData[nSize - 1].vPos;
		Vector3d vPre = m_vecPointData[nSize - 2].vPos;
		Vector3d vAngle;
		if (m_fnCalcAngleCallback)
		{
			vAngle = m_fnCalcAngleCallback(this, vCur, vPre);
		}
		m_vecPointData[nSize - 2].vAngle = vAngle;
		m_vecPointData[nSize - 1].vAngle = vAngle;
	}
}

void TweenAnimateForNode::Move()
{
	if (m_eCurrentAnimateStatus == TWEEN_ANIMATE_STATUS_PAUSE
		|| m_eCurrentAnimateStatus == TWEEN_ANIMATE_STATUS_STOP)
	{
		return;
	}

	if (m_vecPointData.size() == 0)
	{
		return;
	}

	Vector3d vOrig = Vector3d(-1, -1, -1);
	Vector3d vAngle = Vector3d(-1, -1, -1);
	if (m_vecPointData.size() == 1)
	{
		vOrig = m_vecPointData[0].vPos;
		vAngle = m_vecPointData[0].vAngle;
		
		if (m_fnMoveCallback &&(vOrig != m_vPreOrigin || vAngle != m_vPreAngle))
		{
			m_vPreOrigin = vOrig;
			m_vPreAngle = vAngle;

			m_fnMoveCallback(this, m_pTargetNode, m_vecPointData[0].vPos, m_vecPointData[0].vAngle, true);
		}
		if (m_bStopOnEnd)
		{
			Stop();
		}

		return;
	}

	float fFrameTime = 1.0f / 30.0f;
	if (m_bDelayBySelfFrame)
	{
		//秒
		if (m_fCurDelaySelfFrame < m_lDelayStart)
		{
			m_fCurDelaySelfFrame += fFrameTime*1000;
			return;
		}
	}
	else
	{
		long lCurTime = ::GetTickCount();
		if (lCurTime - m_lPreTime < m_lDelayStart)
		{
			return;
		}
	}

	int nCount = m_vecPointData.size() - 1;
	bool bPerEndPoint = false;
	float fSpeed = m_fSpeed;
	if (m_vecPointData.size() > m_nCurrentIndex && m_vecPointData[m_nCurrentIndex].fSpeed >= 0)
	{
		fSpeed = m_vecPointData[m_nCurrentIndex].fSpeed;
	}

	if (m_nCurrentIndex < nCount)
	{
		if (!m_bPerFramePoint)
		{
			if (m_fCurDistance == 0)
			{
				m_vSrcPoint = m_vecPointData[m_nCurrentIndex].vPos;
				m_vSrcAngles = m_vecPointData[m_nCurrentIndex].vAngle;
				m_vDstPoint = m_vecPointData[m_nCurrentIndex + 1].vPos;
				m_vDstAngles = m_vecPointData[m_nCurrentIndex + 1].vAngle;
				m_vOffsetPosition = m_vDstPoint - m_vSrcPoint;
				m_fDistance = m_vOffsetPosition.Length();
				if (m_nCurrentIndex == 0)
				{
					Axisd tAxis;
					tAxis.FromAngles(m_vSrcAngles);
					m_qSrcQuaternion.FromAxis(tAxis.Getf());
					tAxis.FromAngles(m_vDstAngles);
					m_qDstQuaternion.FromAxis(tAxis.Getf());
				}
				else
				{
					Axis tAxis;
					tAxis.FromAngles(m_vecPointData[m_nCurrentIndex - 1].vAngle);
					m_qSrcQuaternion.FromAxis(tAxis);
					tAxis.FromAngles(m_vecPointData[m_nCurrentIndex].vAngle);
					m_qDstQuaternion.FromAxis(tAxis);
				}
			}

			float fDeltaTimeSecond = m_fDistance / (fSpeed*1000/3600.0f);
			if (fDeltaTimeSecond >= m_fMin2PointTimeSecond && m_fMin2PointTimeSecond >=0)
			{
				//超过设定的两点最小插值时间
				m_fCurDistance = m_fDistance;
				vOrig = m_vDstPoint;
				vAngle = m_vDstAngles;
			}
			else
			{
				if (m_bSpeedUniform)
				{
					m_fCurDistance += fFrameTime * (1000 * (fSpeed) / 3600.0f);
					//m_fCurDistance += 10.f*fFrameTime*(m_fSpeed / 60.0f);
				}
				else
				{
					if (m_fDistance < 5.0f)
					{
						m_fCurDistance += 3.f * fFrameTime * (m_fSpeed / 60.0f);
					}
					else if (m_fDistance < 7.5f)
					{
						m_fCurDistance += 4.0f * fFrameTime * (m_fSpeed / 60.0f);
					}
					else if (m_fDistance < 7.5f)
					{
						m_fCurDistance += 8.f * fFrameTime * (m_fSpeed / 60.0f);
					}
					else
					{
						m_fCurDistance += 15.0f * fFrameTime * (m_fSpeed / 60.0f);
					}
				}

				float fDis = m_fCurDistance;
				vOrig[0] = m_vOffsetPosition[0] * (fDis / m_fDistance) + m_vSrcPoint[0];
				vOrig[1] = m_vOffsetPosition[1] * (fDis / m_fDistance) + m_vSrcPoint[1];
				vOrig[2] = m_vOffsetPosition[2] * (fDis / m_fDistance) + m_vSrcPoint[2];
				//到点了再进行插值(1%的距离内插值完成)
				if (m_nCurrentIndex > 0)
				{
					float fAngleDis = m_fDistance * 0.1;
					if (m_fCurDistance < fAngleDis && m_nCurrentIndex > 0)
					{
						Quaternion  q = Quaternion::Slerp(m_fCurDistance / fAngleDis, m_qSrcQuaternion, m_qDstQuaternion, true);
						Axis mQuat = q.ToAxis();
						vAngle = mQuat.ToAngles();
					}
					else
					{
						vAngle = m_qDstQuaternion.ToAxis().ToAngles();
					}
				}
				else
				{
					vAngle = m_qSrcQuaternion.ToAxis().ToAngles();
				}
			}
			
			if (m_fCurDistance >= m_fDistance)
			{
				m_nCurrentIndex++;
				m_fCurDistance = 0;
				m_fStartAngleDis = 0;
				bPerEndPoint = true;

				vOrig = m_vDstPoint;
				vAngle = m_vDstAngles;
			}
		}
		else
		{
			vOrig = m_vecPointData[m_nCurrentIndex].vPos;
			vAngle = m_vecPointData[m_nCurrentIndex].vAngle;
			bPerEndPoint = true;
			m_nCurrentIndex++;
		}
		
	}

	if (m_nCurrentIndex == nCount)
	{
		//最后一个
		int nCurLoopNum = m_nCurLoopNum;
		if (m_bLoop && (m_nCurLoopNum < m_nLoopCount || m_nLoopCount < 0))
		{
			m_nCurrentIndex = 0;
			nCurLoopNum++;
		}
		else
		{
			if (m_bStopOnEnd)
			{
				Stop();
				nCurLoopNum = 1;
			}
		}

		if (m_fnPathEndCallback)
		{
			m_fnPathEndCallback(this, m_pTargetNode, m_nCurLoopNum, m_nLoopCount);
		}
		m_nCurLoopNum = nCurLoopNum;

		if (!m_bStopOnEnd)
		{
			vOrig = m_vecPointData[m_nCurrentIndex].vPos;
			vAngle = m_vecPointData[m_nCurrentIndex].vAngle;
		}
	}

	if (m_fnMoveCallback && (vOrig != m_vPreOrigin || vAngle != m_vPreAngle))
	{
		m_vPreOrigin = vOrig;
		m_vPreAngle = vAngle;

		m_fnMoveCallback(this, m_pTargetNode, vOrig, vAngle, bPerEndPoint);
	}

	if (m_vecPointData.size() >= m_nPointMaxCount && !m_bLoop && bPerEndPoint)
	{
		//完成一个点后，移除之前的
		m_vecPointData.erase(m_vecPointData.begin());
		m_nCurrentIndex = m_nCurrentIndex -1;
	}
}

void TweenAnimateForNode::Start(long fDelayMillsecond)
{
	if (m_eCurrentAnimateStatus == TWEEN_ANIMATE_STATUS_PLAY)
	{
		return;
	}
	m_eCurrentAnimateStatus = TWEEN_ANIMATE_STATUS_PLAY;
	m_lDelayStart = fDelayMillsecond;
	m_lPreTime = ::GetTickCount();
	m_fCurDelaySelfFrame = 0;
}

void TweenAnimateForNode::Pause()
{
	m_eCurrentAnimateStatus = TWEEN_ANIMATE_STATUS_PAUSE;
}

void TweenAnimateForNode::Stop()
{
	if (m_eCurrentAnimateStatus == TWEEN_ANIMATE_STATUS_STOP)
	{
		return;
	}
	m_fCurDistance = 0;
	m_nCurrentIndex = 0;
	m_eCurrentAnimateStatus = TWEEN_ANIMATE_STATUS_STOP;
	m_lDelayStart = 0;
	m_lPreTime = 0;
	m_fStartAngleDis = 0;
	m_vPreOrigin = Vector3(0, 0, 0);
	m_vPreAngle = Vector3(0, 0, 0);
}

void TweenAnimateForNode::ResetPosition()
{
	Stop();
	if (m_fnMoveCallback && m_vecPointData.size() > 0)
	{
		m_fnMoveCallback(this, m_pTargetNode, m_vecPointData[0].vPos, m_vecPointData[0].vAngle,false);
	}
}

void TweenAnimateForNode::SetPointMaxCount(int nPointMax)
{
	m_nPointMaxCount = nPointMax;
	if (m_nPointMaxCount < 3)
	{
		m_nPointMaxCount = LOCUS_MAX_POINT;
	}
}

PointData* TweenAnimateForNode::GetCurrentPointData()
{
	if (m_nCurrentIndex < m_vecPointData.size())
	{
		return &m_vecPointData[m_nCurrentIndex];
	}
	return NULL;
}

void TweenAnimateForNode::ClearAllPoint()
{
	Stop();
	m_vecPointData.clear();
	m_vecPointData.resize(0);
}

void TweenAnimateForNode::SetExtraData(std::string strKey, INT_PTR pTr)
{
	m_mapINTPTR[strKey] = pTr;
}

INT_PTR TweenAnimateForNode::GetExtraData(std::string strKey)
{
	if (m_mapINTPTR.find(strKey) != m_mapINTPTR.end())
	{
		return m_mapINTPTR[strKey];
	}
	return NULL;
}

void TweenAnimateForNode::SetExtraJValue(std::string strKey, Json::Value jValue)
{
	m_mapJValue[strKey] = jValue;
}

Json::Value TweenAnimateForNode::GetExtraJValue(std::string strKey)
{
	if (m_mapJValue.find(strKey) != m_mapJValue.end())
	{
		return m_mapJValue[strKey];
	}
	return Json::nullValue;
}

