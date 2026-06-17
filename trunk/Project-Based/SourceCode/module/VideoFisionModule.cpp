#include "ProjectBasedHeader.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/module/IStateModule.h"
#include "Project-Based/module/BaseModule.h"
#include "Project-Based/utils/tools/VideoFusionTool.h"
#include "Project-Based/module/VideoFusionModule.h"
#include "Project-Based/utils/ProjectLogger.h"

using namespace bc;

VideoFusionModule::VideoFusionModule(const std::string& strModuleName)
	: BaseModule(strModuleName),
	m_pVideoFusionTool(nullptr),
	m_pVideoFusionGroup(nullptr),
	m_pCurVideoFusionInfo(nullptr),
	m_bVideoFusionVisible(false)
{
	m_pClient->m_stBCModuleManager.pVideoFuseModule = this;

	m_strSaveXmlPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strProjectDir;
	m_strSaveXmlPath += "/models/videofusions";
	Mkdirs(m_strSaveXmlPath.c_str());
	m_strSaveXmlPath += "/videofusion.tw4";
	m_strSaveModelPath = "models/videofusions";
}

VideoFusionModule::~VideoFusionModule()
{
}

void VideoFusionModule::iSceneLoad(IStateManager* pStateManager)
{
	BaseModule::iSceneLoad(pStateManager);

	m_pVideoFusionTool = new VideoFusionTool(m_pModuleGroupNode);
	m_pVideoFusionTool->SetModelSavePath(m_strSaveModelPath);

	loadXml();
}

void VideoFusionModule::iRefreshData()
{
}

void VideoFusionModule::iDoWebCommond(WebCommandData* pWebData)
{
	BaseModule::iDoWebCommond(pWebData);
	try
	{
		if (pWebData->strCommand == "slotWebKeyDown")
		{
			if (pWebData->vecValues.size() < 1)
			{
				return;
			}
			int nCode = pWebData->vecValues[0].asInt();
			std::string strKey = "shift";
			if (pWebData->vecValues.size() >= 2 && pWebData->vecValues[1][strKey].asBool())
			{
				if (nCode == 68)
				{
					/*INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();*/
					//ICamera* pICamera = pCameraNode->GetDynamicComponent<ICamera>();
					////D 开始
					//m_pVideoFusionTool->SetPosition(Vector3d(4547.30664, -19191.1113, 51.1163635));
					//m_pVideoFusionTool->SetAngle(Vector3d(9.99999523, 77.6599503, 0));
					////m_pVideoFusionTool->SetFOV(70);
					////m_pVideoFusionTool->SetAspect(0.5625f);
					//m_pVideoFusionTool->SetFOV(pICamera->iGetFOV());
					//m_pVideoFusionTool->SetAspect(pICamera->iGetAspect());
					////m_pVideoFusionTool->SetFuseImage("C:/Users/Administrator/Desktop/fusion_test/Fusionvideo.png");
					//m_pVideoFusionTool->SetFuseImage("");
					//FusionParam stFusionParam;
					//stFusionParam.strID = "aaaaaaaaaaa_123";
					//stFusionParam.ePlugin = MEDIA_PLUGIN_FFMPEG;
					//stFusionParam.strLiveUrl = "C:/Users/Administrator/Desktop/fusion_test/fuse_test.mp4";
					//stFusionParam.bTimeStamp = true;
					//m_pVideoFusionTool->SetFuseParam(stFusionParam);
					//m_pVideoFusionTool->Render();
				}
				else if (nCode == 69)
				{
					//E 结束
					Stop();
					m_pCurVideoFusionInfo = nullptr;
				}
				else if (nCode == 75)
				{
					//K 切割开始
					m_pVideoFusionTool->SetCutEnable(true);
				}
				else if (nCode == 38)
				{
					//方向键上
					m_pVideoFusionTool->MoveForward(0.5f);
				}
				else if (nCode == 40)
				{
					//方向键下
					m_pVideoFusionTool->MoveForward(-0.5f);
				}
				else if (nCode == 37)
				{
					//方向键左
					m_pVideoFusionTool->MoveLeft(0.5f);
				}
				else if (nCode == 39)
				{
					//方向键右
					m_pVideoFusionTool->MoveLeft(-0.5f);
				}
				else if (nCode == 90)
				{
					//Z 高度调高
					m_pVideoFusionTool->MoveZ(0.5f);
				}
				else if (nCode == 88)
				{
					//X 高度调低
					m_pVideoFusionTool->MoveLeft(-0.5f);
				}
				else if (nCode == 67)
				{
					//C  俯仰高
					m_pVideoFusionTool->RotateX(0.5f);
				}
				else if (nCode == 86)
				{
					//V  俯仰低
					m_pVideoFusionTool->RotateX(-0.5f);
				}
				else if (nCode == 66)
				{
					//B  旋转左
					m_pVideoFusionTool->RotateY(0.5f);
				}
				else if (nCode == 78)
				{
					//N  旋转右
					m_pVideoFusionTool->RotateY(-0.5f);
				}
				else if (nCode == 79)
				{
					//O  Fov减
					float fFov = m_pVideoFusionTool->GetFusionCameraNode()->GetDynamicComponent<IFusionCamera>()->iGetFOV();
					fFov -= 0.5f;
					m_pVideoFusionTool->SetFOV(fFov);
				}
				else if (nCode == 80)
				{
					//P  Fov加
					float fFov = m_pVideoFusionTool->GetFusionCameraNode()->GetDynamicComponent<IFusionCamera>()->iGetFOV();
					fFov += 0.5f;
					m_pVideoFusionTool->SetFOV(fFov);
				}
				else if (nCode == 83)
				{
					//S 保存
					//Stop();
					IFusionCamera* pIFusionCamera = m_pVideoFusionTool->GetFusionCameraNode()->GetDynamicComponent<IFusionCamera>();
					if (m_pCurVideoFusionInfo)
					{
						m_pCurVideoFusionInfo->fAspect = pIFusionCamera->iGetAspect();
						m_pCurVideoFusionInfo->fFov = pIFusionCamera->iGetFOV();
						m_pCurVideoFusionInfo->vPos = pIFusionCamera->iGetPosition();
						m_pCurVideoFusionInfo->vAngle = pIFusionCamera->iGetAngle();
						m_pCurVideoFusionInfo = nullptr;
					}

					m_pVideoFusionGroup->GetDynamicComponent<IGroup>()->iSave(m_strSaveXmlPath);
				}
				else if (nCode == 82)
				{
					//R 重置
					m_pVideoFusionTool->Reset();
					ToRender(m_pCurVideoFusionInfo);
				}
				else if (nCode == 70)
				{
					//F 同位置
					INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
					m_pVideoFusionTool->SetPosition(pCameraNode->iGetOrigin());
					m_pVideoFusionTool->SetAngle(pCameraNode->iGetAngles());
				}
			}
		}
		//模拟发送的信息
		else if (pWebData->strCommand == "VS_OpenFusionLabel")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strID = pWebData->vecValues[0].asString();
				std::vector<INode*> vecNode;
				m_pModuleGroupNode->iGetAllChildren(vecNode);
				for (size_t i = 0; i < vecNode.size(); ++i)
				{
					if (vecNode[i]->iGetNodeType() == NODE_FUSION && vecNode[i]->iGetFlagName() == strID)
					{
						vecNode[i]->iSetVisible(true);
						iOnVideoFusionChangeVisible(vecNode[i], true);
						break;
					}
				}
			}
		}
		//模拟发送的信息
		else if (pWebData->strCommand == "VS_CloseFusionLabel")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strID = pWebData->vecValues[0].asString();
				std::vector<INode*> vecNode;
				m_pModuleGroupNode->iGetAllChildren(vecNode);
				for (size_t i = 0; i < vecNode.size(); ++i)
				{
					if (vecNode[i]->iGetNodeType() == NODE_FUSION && vecNode[i]->iGetFlagName() == strID)
					{
						vecNode[i]->iSetVisible(false);
						iOnVideoFusionChangeVisible(vecNode[i], false);
						break;
					}
				}
			}
		}
	}
	catch (const std::exception&)
	{
	}
}

void VideoFusionModule::iOnAppModeChanged(int nAppMode)
{
	bool bVisible = (m_pClient->m_nAppMode & APP_MODE_EXHIBITION);
	m_pModuleGroupNode->iSetVisible(bVisible);
}

void VideoFusionModule::iOnAppIdChanged(std::string  strAppId)
{
	//loadXml();
}

void VideoFusionModule::loadXml()
{
	if (m_pVideoFusionGroup)
	{
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pVideoFusionGroup, false);
	}
	NodeParamBase stParam;
	stParam.nNodeType = NODE_GROUP;
	m_pVideoFusionGroup = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(stParam);
	m_pVideoFusionGroup->iLoadModel();
	m_pModuleGroupNode->iAddChild(m_pVideoFusionGroup);
	m_pVideoFusionTool->GetFusionCameraNode()->GetDynamicComponent<IFusionCamera>()->iSetTopGroupNode(m_pVideoFusionGroup);

	ProjectLogger::GetInstance()->InfoMessage(std::string("videoFusion load xml=") + m_strSaveXmlPath);
	if (IsFileExist(m_strSaveXmlPath))
	{
		BCXmlFile xFile;
		XMLDoc xmlDoc;
		if (xFile.Load(m_strSaveXmlPath.c_str(), true))
		{
			char* pXml = xmlDoc.allocate_string(0, xFile.GetBufferLength());
			memset(pXml, 0, xFile.GetBufferLength());
			//size_t nLen = xFile.ToASCII(pXml);
			size_t nLen = xFile.GetUtf8Buff(pXml, xFile.GetBufferLength());
			try
			{
				xmlDoc.parse<0>(pXml);
				XMLNodePtr pScene = xmlDoc.first_node("SCENE");
				XMLNodePtr pRoot = pScene->first_node("ROOT");
				m_pVideoFusionGroup->GetDynamicComponent<IGroup>()->iLoad(pRoot);

				ProjectLogger::GetInstance()->InfoMessage(std::string("videoFusion xml sucess!"));
			}
			catch (rapidxml::parse_error&)
			{
			}
		}
	}
	SetVideoFusionVisible(m_bVideoFusionVisible);
	iOnLoadedXml();
}

void VideoFusionModule::ToRender(VideoFusionInfo* pInfo)
{
	if (!pInfo)
	{
		return;
	}
	m_pCurVideoFusionInfo = pInfo;
	INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
	ICamera* pICamera = pCameraNode->GetDynamicComponent<ICamera>();
	m_pVideoFusionTool->SetPosition(pInfo->vPos);
	m_pVideoFusionTool->SetAngle(pInfo->vAngle);
	m_pVideoFusionTool->SetFOV(pInfo->fFov);
	m_pVideoFusionTool->SetAspect(pInfo->fAspect);
	m_pVideoFusionTool->SetFuseImage("");
	FusionParam stFusionParam;
	stFusionParam.strFlagName = pInfo->strID;
	stFusionParam.ePlugin = MEDIA_PLUGIN_FFMPEG;
	stFusionParam.strLiveUrl = pInfo->strVideoUrl;
	stFusionParam.bTimeStamp = true;
	m_pVideoFusionTool->SetFuseParam(stFusionParam);
	m_pVideoFusionTool->Render();

	//取消三维自身的键盘操作和鼠标操作
	WebCommandData stWebData;
	stWebData.strCommand = "Web_SetProjectActionEnable";
	stWebData.vecValues.push_back(false);
	m_pClient->AddWebCommond(stWebData);
}

void VideoFusionModule::Stop()
{
	m_pVideoFusionTool->Stop();
	//取消三维自身的键盘操作和鼠标操作
	WebCommandData stWebData;
	stWebData.strCommand = "Web_SetProjectActionEnable";
	stWebData.vecValues.push_back(true);
	m_pClient->AddWebCommond(stWebData);
}

void VideoFusionModule::SetVideoFusionVisible(bool bVisible)
{
	m_bVideoFusionVisible = bVisible;
	std::vector<INode*> vecNode;
	m_pModuleGroupNode->iGetAllChildren(vecNode);
	for (size_t i = 0; i < vecNode.size(); ++i)
	{
		if (vecNode[i]->iGetNodeType() == NODE_FUSION)
		{
			vecNode[i]->iSetVisible(bVisible);
			iOnVideoFusionChangeVisible(vecNode[i], bVisible);
		}
	}
}

