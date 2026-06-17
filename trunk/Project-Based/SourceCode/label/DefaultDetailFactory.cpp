#include "ProjectBasedHeader.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/utils/ParamNetImage.h"
#include "Project-Based/utils/ProjectLogger.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/label/InstanceLabelFactory.h"
#include "Project-Based/label/LabelDetailFactory.h"
#include "DefaultDetailFactory.h"

#define KEY_LABEL_FACTORY std::string("labelFactory")

using namespace bc;

DefaultDetailFactory::DefaultDetailFactory()
{
}

DefaultDetailFactory::~DefaultDetailFactory()
{
}

INode* DefaultDetailFactory::iCreateNode(int nType, ProjectBasedSceneElement* pData)
{
	CommonLabelFactory* pLabelFactory = (CommonLabelFactory*)GetExtraData(KEY_LABEL_FACTORY);
	return pLabelFactory->iCreateNodeForDetail(nType, pData);
}

int DefaultDetailFactory::iGetTypeByData(ProjectBasedSceneElement* pInfo)
{
	CommonLabelFactory* pLabelFactory = (CommonLabelFactory*)GetExtraData(KEY_LABEL_FACTORY);
	return pLabelFactory->iGetTypeByData(pInfo);
}

void DefaultDetailFactory::iOpenWithNode(FactotryAssembleDetail* pAssemble)
{
	CommonLabelFactory* pLabelFactory = (CommonLabelFactory*)GetExtraData(KEY_LABEL_FACTORY);

	pLabelFactory->SetLabelSelect(pAssemble, true);
	if (pAssemble->pSelfNode)
	{
		pLabelFactory->iOpenForDetail(pAssemble);
	}
}

void DefaultDetailFactory::iUpdateWithNode(FactotryAssembleDetail* pAssemble)
{
	CommonLabelFactory* pLabelFactory = (CommonLabelFactory*)GetExtraData(KEY_LABEL_FACTORY);
	if (pAssemble->pSelfNode)
	{
		pAssemble->pSelfNode->iSetExternParam((INT_PTR)pAssemble->pData);
		SetINTForNodeWithDetail(pAssemble->pSelfNode);

		pLabelFactory->iUpdateDataForDetail(pAssemble);
	}
}

void DefaultDetailFactory::iCloseWithNode(FactotryAssembleDetail* pAssemble)
{
	CommonLabelFactory* pLabelFactory = (CommonLabelFactory*)GetExtraData(KEY_LABEL_FACTORY);

	pLabelFactory->SetLabelSelect(pAssemble, false);
	if (pAssemble->pSelfNode)
	{
		pLabelFactory->iCloseForDetail(pAssemble);
	}
}

void DefaultDetailFactory::iFrameUpdateWithNode(FactotryAssembleDetail* pAssemble)
{
	if (!pAssemble->bAdjustPosWithLabel)
	{
		return;
	}
	CommonLabelFactory* pLabelFactory = (CommonLabelFactory*)GetExtraData(KEY_LABEL_FACTORY);

	INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
	Vector3d vLabelOrign;
	if (!pAssemble->bInstance)
	{
		Vector3d vOrigin = pAssemble->pLabelNode->iGetOrigin();
		vLabelOrign = vOrigin;
		if (!pCameraNode->GetDynamicComponent<ICamera>()->iCullPoint(vOrigin))
		{
			//显示在相机内,则使用原生位置
			if (pAssemble->pLabelNode->iIsVisible())
			{
				vLabelOrign = pAssemble->pLabelNode->iGetBBox().GetCenter();
			}
		}
		if (vLabelOrign.Empty())
		{
			IBoard* pBoard = pAssemble->pLabelNode->GetDynamicComponent<IBoard>();
			if (pBoard)
			{
				BoardParam* pParam = pBoard->iGetBoardParam();
				vLabelOrign = pParam->vTargetPosition;
			}

		}
	}
	else if (pAssemble->pBoardInstanceData != NULL)
	{
		INode *pNode = ((InstanceLabelFactory*)pLabelFactory)->GetSelectBoardNode(pAssemble->pData);
		if (pNode)
		{
			vLabelOrign = pNode->iGetBBox().GetCenter();
		}
		else
		{
			vLabelOrign = pAssemble->pBoardInstanceData->tBBox.GetCenter();
		}
		if (vLabelOrign.Empty())
		{
			vLabelOrign = pAssemble->pBoardInstanceData->vOrigin;
		}
	}
	INode* pDetailBoard = pAssemble->pSelfNode;

	if (pDetailBoard && pDetailBoard->iIsVisible())
	{
		Vector2 vDistanceToTarget = pAssemble->vDetailToLabelOffset;

		BoardParam* m_pBoardParam = pDetailBoard->GetDynamicComponent<IBoard>()->iGetBoardParam();
		if (m_pBoardParam->bDynamicScaleEnable)
		{
			Vector3d vOrigin = vLabelOrign;
			Vector3d vCameraOrign = pCameraNode->iGetOrigin();
			m_pBoardParam->fScale = vCameraOrign.Distance(vOrigin) * m_pBoardParam->fDynamicScale;

			vDistanceToTarget = pAssemble->vDetailToLabelOffset * pCameraNode->iGetOrigin().Distance(vLabelOrign) * 0.001;

			if (m_pBoardParam->fDynamicMaxScale > 0 && m_pBoardParam->fScale > m_pBoardParam->fDynamicMaxScale)
			{
				m_pBoardParam->fScale = m_pBoardParam->fDynamicMaxScale;

				vDistanceToTarget = pAssemble->vDetailToLabelOffset *
					(m_pBoardParam->fDynamicMaxScale / m_pBoardParam->fDynamicScale) * 0.001;
			}

			if (m_pBoardParam->fDynamicMinScale > 0 && m_pBoardParam->fScale < m_pBoardParam->fDynamicMinScale)
			{
				m_pBoardParam->fScale = m_pBoardParam->fDynamicMinScale;

				vDistanceToTarget = pAssemble->vDetailToLabelOffset *
					(m_pBoardParam->fDynamicMinScale / m_pBoardParam->fDynamicScale) * 0.001;
			}
		}

		Vector3d vUp(0, 0, 1);
		Vector3d vViewDir = pCameraNode->iGetOrigin() - vLabelOrign;
		vViewDir.Normalise();
		Vector3d vDir = vUp.CrossProduct(vViewDir);
		if (vDir.Length() < FLT_EPSILON)
		{
			vDir = -pCameraNode->iGetAxis()[1];
		}
		else
		{
			vDir.Normalise();
		}

		Vector3d vOrigin = vLabelOrign;
		vOrigin = vOrigin.MoveForward(vDir, vDistanceToTarget.x);
		vOrigin = vOrigin.MoveForward(Vector3d(0, 0, 1), vDistanceToTarget.y);
		float fHeight = pDetailBoard->GetDynamicComponent<IBoard>()->iGetBoardHeight() / 2;
		ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
		pClient->AdjustZ(vOrigin, fHeight, true);
		pDetailBoard->iSetOrigin(vOrigin);
	}

	if (pAssemble->pSelfNode)
	{
		pLabelFactory->iFrameUpdateForDetail(pAssemble);
	}
}

void bc::DefaultDetailFactory::sendDetailScreenPos(FactotryAssembleDetail* pAssemble)
{
	if (pAssemble)
	{
		Vector3d vLabelPos;
		ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)pAssemble->pData;
		INode* pLabelNode = pAssemble->pLabelNode;
		if (!pAssemble->bInstance && pLabelNode)
		{
			vLabelPos = pLabelNode->iGetBBox().GetCenter();
			vLabelPos.z = pLabelNode->iGetBBox()._vMaxs.z;
		}
		else
		{
			vLabelPos = pAssemble->pBoardInstanceData->tBBox.GetCenter();
			vLabelPos.z = pAssemble->pBoardInstanceData->tBBox._vMaxs.z;
		}

		static Vector3d vLastCameraPos;
		static Vector3d vLastCameraAngle;
		Vector3d vCurCameraPos = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->iGetOrigin();
		Vector3d vCurCameraAngle = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->iGetAngles();

		if (pInfo->vLastRealLabelPos.Distance(vLabelPos) > 0.2f || vLastCameraPos.Distance(vCurCameraPos) > 0.2f
			|| vLastCameraAngle.Distance(vCurCameraAngle) > 0.2f)
		{
			INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
			ICamera* pCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
			Vector3d vScreenOut;
			pCamera->iProject(vLabelPos, vScreenOut);

			int nScreenX = (int)vScreenOut.x;
			int nScreenY = (int)vScreenOut.y;

			std::string str = "";
			str += ",";
			str += "'" + pInfo->strUuid + "'";
			str += ",";
			str += to_string(nScreenX);
			str += ",";
			str += to_string(nScreenY);

			std::string strData = str;
			m_pClient->ToSendWebCommond("VS_ToWebSelectLabelScreenPos", "", strData);

			pInfo->vLastRealLabelPos = vLabelPos;
			vLastCameraPos = vCurCameraPos;
			vLastCameraAngle = vCurCameraAngle;
		}
	}
}

