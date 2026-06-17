#include "ProjectBasedHeader.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/webApi/IWebLabelFactory.h"
#include "Project-Based/base/ProjectBasedClient.h"

using namespace bc;

IWebLabelFactory::IWebLabelFactory() :
	m_pLabelNodeParam(nullptr),
	m_pDetailNodeParam(nullptr),
	m_pLabelClusterParam(nullptr),
	m_eLabelNodeType(NODE_UNKNOWN),
	m_nLabelType(-1),
	m_bInstanceFactory(false),
	m_pFactory(nullptr),
	m_jLabelOption(Json::nullValue)
{

}

IWebLabelFactory::~IWebLabelFactory()
{
	DELETE_PTR(m_pLabelNodeParam);
	DELETE_PTR(m_pDetailNodeParam);
	DELETE_PTR(m_pLabelClusterParam);
}

void IWebLabelFactory::SetLabelParam(NodeParamBase* pParam)
{
	m_pLabelNodeParam = pParam;
}

void IWebLabelFactory::SetLabelDetailParam(NodeParamBase* pParam)
{
	m_pDetailNodeParam = pParam;
}

void IWebLabelFactory::SetLabelClusterParam(NodeParamBase* pParam)
{
	m_pLabelClusterParam = pParam;
	if (m_pLabelClusterParam)
	{
		GetFactory()->AddFunc(FUNC_CLUSTER);
	}
}

void IWebLabelFactory::SetLabelNodeType(NodeType_e eType)
{
	m_eLabelNodeType = eType;
}

void IWebLabelFactory::SetLabelType(int nLabelType)
{
	m_nLabelType = nLabelType;
}

int IWebLabelFactory::GetLabelType()
{
	return m_nLabelType;
}

void IWebLabelFactory::setText(INode* pNode, Json::Value& j3DTextMatchObject)
{
	std::vector<TextShow*> vecTextShow;
	FindAllTextShow(pNode, vecTextShow);
	try
	{
		for (size_t i = 0; i < vecTextShow.size(); i++)
		{
			if (j3DTextMatchObject.isMember(vecTextShow[i]->strID))
			{
				std::string strValue = "";
				Json::Value value = j3DTextMatchObject[vecTextShow[i]->strID];
				if (value.isInt())
				{
					strValue = std::to_string(value.asInt());
				}
				else if (value.isDouble())
				{
					strValue = std::to_string(value.asFloat());
				}
				else if (value.isBool())
				{
					strValue = value.isBool() ? "true" : "false";
				}
				else if (value.isString())
				{
					strValue = value.asString();
				}
				else if (value.isObject())
				{
					strValue = value.toStyledString();
				}
				else if (value.isArray())
				{
					strValue = value.toStyledString();
				}
				SetTextShowByStrKey(pNode, vecTextShow[i]->strID, strValue);
			}
		}
	}
	catch (const std::exception&)
	{

	}
}

CommonLabelFactory* IWebLabelFactory::GetFactory()
{
	return m_pFactory;
}

void IWebLabelFactory::setFactory(CommonLabelFactory* pFactory)
{
	m_pFactory = pFactory;
}

void IWebLabelFactory::iToWebSelectLabelPos(FactoryAssemble* pAssemble)
{
	if (GetFactory()->GetDetailFactory())
	{
		if (pAssemble)
		{
			Vector3d vLabelPos;
			ProjectBasedWebLabelData* pInfo = (ProjectBasedWebLabelData*)pAssemble->pData;
			if (GetFactory()->IsInstanceFactory())
			{
				vLabelPos = pAssemble->pLabelInstance->tBBox.GetCenter();
				vLabelPos.z = pAssemble->pLabelInstance->tBBox._vMaxs.z;
			}
			else if (pAssemble->pLabelNode)
			{
				vLabelPos = pAssemble->pLabelNode->iGetBBox().GetCenter();
				vLabelPos.z = pAssemble->pLabelNode->iGetBBox()._vMaxs.z;
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
				str += std::to_string(nScreenX);
				str += ",";
				str += std::to_string(nScreenY);

				ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
				std::string strData = str;
				pClient->ToSendWebCommond("VS_ToWebSelectLabelScreenPos", "", strData);

				pInfo->vLastRealLabelPos = vLabelPos;
				vLastCameraPos = vCurCameraPos;
				vLastCameraAngle = vCurCameraAngle;
			}
		}
	}
}

bool IWebLabelFactory::CheckStyleChanged(const std::string& strLabelStyleMD5, const std::string& strDetailStyleMD5, const std::string& strClusterStyleMD5)
{
	if (m_strLabelNodeParamMD5 != strLabelStyleMD5
		|| m_strDetailNodeParamMD5 != strDetailStyleMD5
		|| m_strClusterNodeParamMD5 != strClusterStyleMD5)
	{
		return true;
	}

	if (m_pLabelNodeParam)
	{
		if (m_pLabelNodeParam->nNodeType == NODE_POLYGON
			|| m_pLabelNodeParam->nNodeType == NODE_FLYLINE
			|| m_pLabelNodeParam->nNodeType == NODE_LOCUS_LINE
			|| m_pLabelNodeParam->nNodeType == NODE_ROAD)
		{
			return true;
		}
	}

	return false;
}

void IWebLabelFactory::SetStyleMD5(const std::string& strLabelStyleMD5, const std::string& strDetailStyleMD5, const std::string& strClusterStyleMD5)
{
	m_strLabelNodeParamMD5 = strLabelStyleMD5;
	m_strDetailNodeParamMD5 = strDetailStyleMD5;
	m_strClusterNodeParamMD5 = strClusterStyleMD5;
}

