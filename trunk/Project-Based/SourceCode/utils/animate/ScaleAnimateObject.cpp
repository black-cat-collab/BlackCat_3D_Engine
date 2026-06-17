
#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/animate/EasingAnimate.h"
#include "Project-Based/utils/animate/BaseAnimateObject.h"
#include "Project-Based/utils/animate/ScaleAnimateObject.h"

using namespace bc;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif



ScaleAnimateObject::ScaleAnimateObject(INode* pTarget, ScaleAnimateParam stParam) :BaseAnimateObject(pTarget)
{
	m_stAnimateParam = stParam;
}

ScaleAnimateObject::~ScaleAnimateObject()
{
	
}

void ScaleAnimateObject::iEasingAnimateUpdate(EasingAnimateParam* pEasingParam, std::map<std::string, INT_PTR> m_mapINTPTR,
	int nCurValueIndex, double dCurRate, int nCurLoopCount, int nAllLoopCount, bool bComplete, bool& bManualStop)
{
	ScaleAnimateObject* pThis = (ScaleAnimateObject*)m_mapINTPTR["this"];
	Vector3ProValue& stValue = pThis->m_stAnimateParam.vecValue[nCurValueIndex];
	Vector3d vStartPos = stValue.vStart;
	Vector3d vOffsetPosition = stValue.vEnd - stValue.vStart;
	Vector3d vCurPos;
	vCurPos[0] = vOffsetPosition[0] * dCurRate + vStartPos[0];
	vCurPos[1] = vOffsetPosition[1] * dCurRate + vStartPos[1];
	vCurPos[2] = vOffsetPosition[2] * dCurRate + vStartPos[2];

	if (pThis->m_pTargetNode)
	{
		IBoard* pBoard = GetComponent<IBoard>(pThis->m_pTargetNode);
		if (pThis->m_pTargetNode->iGetNodeType() == NODE_LIGHT_BEAM)
		{
			IBoard* pLightBean = pThis->m_pTargetNode->GetDynamicComponent<IBoard>();
			float fMax = max(vCurPos.x, vCurPos.y);
			fMax = max(fMax, vCurPos.z);
			LightBeamParam* pParam = (LightBeamParam*)(pLightBean->iGetBoardParam());
			pParam->fLength = fMax;
			pParam->fWidth = fMax;
		}
		else if (pBoard)
		{
			float fMax = max(vCurPos.x, vCurPos.y);
			fMax = max(fMax, vCurPos.z);
			//pBoard->iSetResolutionScale(fMax);
			//std::vector<INode*> vecSub;
			//pBoard->iGetButtonBoard(vecSub);
			//pBoard->iGetVideoBoard(vecSub);
			//if (pBoard->iGetFontBoard())
			//{
			//	vecSub.push_back(pBoard->iGetFontBoard());
			//}
			//if (pBoard->iGetDetailFontBoard())
			//{
			//	vecSub.push_back(pBoard->iGetDetailFontBoard());
			//}
			//for (size_t i = 0; i < vecSub.size(); i++)
			//{
			//	IBoard* pTemp = GetComponent<IBoard>(vecSub[i]);
			//	if (pTemp)
			//	{
			//		pTemp->iSetResolutionScale(fMax);
			//	}
			//}

			std::vector<INode*> vecSub;
			vecSub.push_back(pThis->m_pTargetNode);
			pBoard->iGetButtonBoard(vecSub);
			pBoard->iGetVideoBoard(vecSub);
			if (pBoard->iGetFontBoard())
			{
				vecSub.push_back(pBoard->iGetFontBoard());
			}
			if (pBoard->iGetDetailFontBoard())
			{
				vecSub.push_back(pBoard->iGetDetailFontBoard());
			}
			for (size_t i = 0; i < vecSub.size(); i++)
			{
				IBoard* pTemp = GetComponent<IBoard>(vecSub[i]);
				if (!pTemp)
				{
					continue;
				}
				if (m_mapOriData.find(vecSub[i]->iGetID()) == m_mapOriData.end())
				{
					OriData stData;
					float fOriScale = pTemp->iGetBoardParam()->fScale;
					stData.fScale = fOriScale;
					stData.fDynamicScale = pTemp->iGetBoardParam()->fDynamicScale;
					m_mapOriData[vecSub[i]->iGetID()] = stData;
				}
				OriData &oData = m_mapOriData[vecSub[i]->iGetID()];
				if (pTemp->iGetBoardParam()->bDynamicScaleEnable)
				{
					pTemp->iGetBoardParam()->fDynamicScale = oData.fDynamicScale * fMax;
				}
				else
				{
					pTemp->iGetBoardParam()->fScale = oData.fScale * fMax;
				}
			}
		}
		else
		{
			pThis->m_pTargetNode->iSetScale(vCurPos);
		}
	}
	if (m_stAnimateParam.pUpdateFunc)
	{
		m_stAnimateParam.pUpdateFunc(pEasingParam, m_mapINTPTR, nCurValueIndex, dCurRate, nCurLoopCount, nAllLoopCount, bComplete, bManualStop);
	}
}

EasingAnimateParam ScaleAnimateObject::iGetEasingAnimateParam()
{
	EasingAnimateParam stParam;

	for (size_t i = 0; i < m_stAnimateParam.vecValue.size(); i++)
	{
		FloatProValue stProValue;
		stProValue.CopyProValue(m_stAnimateParam.vecValue[i]);
		stProValue.dStartValue = 0;
		stProValue.dEndValue = m_stAnimateParam.vecValue[i].vStart.Distance(m_stAnimateParam.vecValue[i].vEnd);

		stParam.vecValue.push_back(stProValue);
	}

	return stParam;
}

