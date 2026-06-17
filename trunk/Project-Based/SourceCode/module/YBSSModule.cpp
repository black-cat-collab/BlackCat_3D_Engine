#include "ProjectBasedHeader.h"

#include "Utility/LinuxOS/LinuxOS.h"
#include "Utility/WatchTimer/WatchTimer.h"
#include "Engine/BuilderCoreHeader.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/utils/tools/IsolatedTool.h"
#include "Project-Based/module/IStateModule.h"
#include "Project-Based/module/BaseModule.h"
#include "Project-Based/module/YBSSModule.h"
#include "Project-Based/utils/ProjectLogger.h"
#include <set>
#include <fstream>
#define KEY_DOWN(vKey) ((::GetAsyncKeyState(vKey) & 0x8000)?1:0)

using namespace bc;

#define THREAD_QUEUE_ID  std::string("Queue_YBSSModule")

YBSSModule* YBSSModule::s_pThis = nullptr;

YBSSModule::YBSSModule(const std::string& strModuleName)
	:BaseModule(strModuleName),
	m_bIsolatedEnable(false),
	m_bDoingSplite(false),
	m_bIndividualBuilding(false),
	m_pCurThreadBindListCallback(nullptr),
	m_bVisible(false),
	m_pCurIsolateNode(nullptr),
	m_fShellLoadDis(1200.0f),
	m_bReLoadXml(false),
	m_bAutoBackup(false),
	m_bContinuousCutting(false),
	m_bBackupDone(true),
	m_nNeedLoadCount(0),
	m_nLoadedCount(0)
{
	s_pThis = this;
	m_pIsolatedTool = new IsolatedTool;
	m_lLastTime = 0L;
	m_lLastTimeCheckBackup = 0L;
	m_pClient->m_stBCModuleManager.pYBSSModule = this;
}

YBSSModule::~YBSSModule()
{
	DELETE_PTR(m_pIsolatedTool);
	clear();
}

void YBSSModule::iFrameUpdate(IStateManager* pStateManager)
{
	doRequest();

	//单体化的时候 进行保存
	int nExecutions = 0;
	while (m_vecNewBuilding.size() > 0 && nExecutions < 1)
	{
		nExecutions++;

		std::vector<std::string> vecNodeId;
		std::map<std::string, std::string> mapTw4Path;
		//保存到指定路径 Data 目录
		PathConfig strPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig;
		std::string strShellPath = strPath.strProjectDir;
		//复制相关的文件  Dir
		std::string strDataPath = strPath.strModelsDir + "/shells/" + m_strAppID;
		if (!IsDir(strDataPath.c_str()))
		{
			Mkdirs(strDataPath.c_str());
		}

		if (m_vecNewBuilding.size() > 0)
		{
			INode* pNodeBuilding = m_vecNewBuilding.back();
			if (pNodeBuilding && pNodeBuilding->iGetComponent(COMPONENT_TYPE_SHELL_ROOT))
			{
				IShellRootNode* pShellRootNode = (IShellRootNode*)pNodeBuilding->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
				if (!pShellRootNode->iIsIsolocateFinish())
				{
					continue;
				}
				pShellRootNode->iSaveDir(strShellPath);
				std::string strSrc = strShellPath + "/" + pNodeBuilding->iGetID() + "_out.tw4";
				std::string strDst = strDataPath + "/" + pNodeBuilding->iGetID() + ".tw4";
				mapTw4Path[strSrc] = strDst;
				std::vector<IShellNode*> vecShell = pShellRootNode->iGetShellNodes();
				for (auto pShell : vecShell)
				{
					if (pShell)
					{
						ShellNodeParam tParam = pShell->iGetShellParam();
						m_mapShellAndShellRootId[tParam.strID] = pNodeBuilding->iGetID();
					}
				}
				vecNodeId.push_back(pNodeBuilding->iGetID());
			}
			m_vecNewBuilding.pop_back();
		}

		for (auto strTw4Path : mapTw4Path)
		{
			std::string strInFile = strTw4Path.first;
			std::string strOutFile = strTw4Path.second;
			YBSSModule::Copy(strInFile, strOutFile);
			if (IsFileExist(strInFile))
			{
				remove(strTw4Path.first.c_str());
			}
		}

		if (m_bAutoBackup)
		{
			saveBackup(vecNodeId);
		}
	}

	//不必每帧都去做事情
	long long lCurTime = ::GetTickCount64();
	if (lCurTime >= m_lLastTime + 100LL)
	{
		m_lLastTime = lCurTime;
		//frameUpdateLoadXml();
	}

	//检测备份一标三实的数据 20分钟
	if (m_bAutoBackup && !m_strSaveDirPath.empty() && (lCurTime >= m_lLastTimeCheckBackup + 1200000LL))
		//if (m_bAutoBackup && !m_strSaveDirPath.empty() && (lCurTime >= m_lLastTimeCheckBackup + 10000LL) && m_bBackupDone)
	{
		m_lLastTimeCheckBackup = lCurTime;
		doCheckBackup();
	}
}

void YBSSModule::iSceneLoad(IStateManager* pStateManager)
{
	BaseModule::iSceneLoad(pStateManager);

	INode* pNode = m_pIsolatedTool->GetIsolatedNode();
	if (pNode)
	{
		PolygonSelectorParam tParam;
		pNode->GetDynamicComponent<IPolygonSelector>()->iGetParam(tParam);
		tParam.fHeight = 50;
		tParam.bDepthTest = false;
		pNode->GetDynamicComponent<IPolygonSelector>()->iSetParam(tParam);
	}
}

EventReturnType_e YBSSModule::iProcessEvent(IStateManager* pStateManager, const BCEvent& tEvent)
{
	BaseModule::iProcessEvent(pStateManager, tEvent);
	if (tEvent.eMessage == EVENT_MOUSE_CLICK)
	{
		//左键按下
		//如果在编辑模式下
		if (m_bIsolatedEnable)
		{
			INode* pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentSelectNode();
			std::string strSelect = "false";
			std::string strData = "";
			IShellRootNode* pShellNode = nullptr;
			if (pNode)
			{
				pShellNode = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
			}
			if (pNode && pShellNode)
			{
				strSelect = "true";
				strData = "'" + pNode->iGetID() + "'";
				strData += ",";
				strData += strSelect;
				if (!m_bDoingSplite)
				{
					m_pCurIsolateNode = pNode;
				}
			}
			else
			{
				strData = "''";
				strData += ",";
				strData += strSelect;
				if (!m_bDoingSplite)
				{
					m_pCurIsolateNode = nullptr;
				}
			}
			if (!m_bIndividualBuilding && !m_bDoingSplite)
			{
				m_pClient->ToSendWebCommond("VS_IsolateSelectedNode", "", strData);
			}
		}
		else
		{
			INode* pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentSelectNode();
			if (pNode)
			{
				std::string strSelect = "false";
				std::string strData = "";
				if (pNode->iGetNodeType() == NODE_SHELL_GROUP)
				{
					if (pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT))
					{
						IShellRootNode* pShellRoot = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
						IShellNode* pShell = pShellRoot->iGetSelectNode();
						if (pShell)
						{
							ShellNodeParam tParam;
							pShell->iGetShellParam(tParam);

							strSelect = "true";
							strData = "'" + tParam.strID + "'";
							strData += ",";
							strData += strSelect;
							m_pClient->ToSendWebCommond("VS_IsolateSelectedNode", "", strData);
						}
					}
				}
			}
		}
	}

	if (m_bIsolatedEnable && m_bDoingSplite)
	{
		if (tEvent.eMessage == EVENT_NODE_CLICK)
		{
			INode* pNode = (INode*)tEvent.nParam1;
			if (pNode)
			{
				IShellRootNode* pShellRootNode = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
				if (pShellRootNode)
				{
					//std::vector<IShellNode*> vecShells = pShellRootNode->iGetShellNodes();
					//for (auto it : vecShells)
					//{
					//	if (it)
					//	{
					//		it->iSetSelect(false);
					//	}
					//}
					IShellNode* pShellHover = pShellRootNode->iGetHoverNode();
					IShellNode* pShellSelect = pShellRootNode->iGetSelectNode();
					if (pShellHover != pShellSelect)
					{
						if (pShellSelect)
						{
							pShellSelect->iSetSelect(false);
						}
						if (pShellHover)
						{
							pShellHover->iSetSelect(true);
						}
					}
				}
			}
		}
	}

	return EventReturnType_e::NONE;
}

void YBSSModule::iRefreshData()
{
	if (m_pIsolatedTool)
	{
		m_pIsolatedTool->SetNodeOnlyVisible(nullptr, true);
	}
	//恢复到正常视图
	if (m_bIsolatedEnable)
	{
		APIProvider::GetSystemAPI()->iEngineAPI->iKeyEvent(UINT('P'), true);
		m_bDoingSplite = false;
		m_bIndividualBuilding = false;
		if (!m_strIndividualBuildingNodeId.empty() && m_pModuleGroupNode)
		{
			INode* pNode = m_pModuleGroupNode->iFindNodeByID(m_strIndividualBuildingNodeId);
			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&pNode);
		}
		m_strIndividualBuildingNodeId = "";
	}
}

void YBSSModule::iDoWebCommond(WebCommandData* pWebData)
{
	try
	{
		if (pWebData->strCommand == "Web_SetIsolateEnable")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				m_bIsolatedEnable = pWebData->vecValues[0].asBool();

				if (!m_bIsolatedEnable && m_pModuleGroupNode)
				{
					std::vector<INode*> vec;
					m_pModuleGroupNode->iGetAllChildren(vec);
					for (size_t i = 0; i < vec.size(); ++i)
					{
						IShellRootNode* pShellRoot = (IShellRootNode*)vec[i]->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
						if (pShellRoot)
						{
							pShellRoot->iSetEditorState(false);
						}
					}
				}

				if (!m_bIsolatedEnable)
				{
					APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iSetView(PROJECTION_VIEW);
					WebCommandData stData;
					stData.strCommand = "Web_IsolateCancel";
					iDoWebCommond(&stData);
				}

				IView* pView = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetSceneView(BUILDER_ORTHO_VIEW_ID);
				if (pView)
				{
					pView->iSetVisible(m_bIsolatedEnable);
				}
				pView = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetSceneView(BUILDER_ORTHO_EDITOR_VIEW_ID);
				if (pView)
				{
					m_dRatio = 0.5;
					pView->iSetMainScreenRatio(m_dRatio);
					pView->iSetVisible(m_bIsolatedEnable);
				}
			}
		}
		else if (pWebData->strCommand == "Web_IsolateIndividualBuilding")
		{
			if (!m_bIsolatedEnable)
			{
				return;
			}
			if (!m_bIndividualBuilding)
			{
				if (APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetID() == BUILDER_VIEW_ID)
				{
					APIProvider::GetSystemAPI()->iEngineAPI->iKeyEvent(UINT('T'), true);
				}
			}
			m_pIsolatedTool->IndividualBuilding(m_bIndividualBuilding);
			m_bIndividualBuilding = true;
		}
		else if (pWebData->strCommand == "Web_IsolateVerticalTorusCut")
		{
			if (!m_bIsolatedEnable || !m_pCurIsolateNode)
			{
				return;
			}
			IShellRootNode* pShellRootNode = (IShellRootNode*)m_pCurIsolateNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
			if (pShellRootNode)
			{
				ShellNodeParam tParam = SetShellGenerateParam();
				pShellRootNode->iSetGenerateNodeParam(tParam);
			}
			if (!m_bDoingSplite)
			{
				m_pIsolatedTool->SetNodeOnlyVisible(m_pCurIsolateNode, true);
			}
			m_pIsolatedTool->VerticalTorusCut(m_bDoingSplite);
			m_bDoingSplite = true;
		}
		else if (pWebData->strCommand == "Web_IsolateVerticalFlushCut")
		{
			if (!m_bIsolatedEnable || !m_pCurIsolateNode)
			{
				return;
			}
			IShellRootNode* pShellRootNode = (IShellRootNode*)m_pCurIsolateNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
			if (pShellRootNode)
			{
				ShellNodeParam tParam = SetShellGenerateParam();
				pShellRootNode->iSetGenerateNodeParam(tParam);
			}
			if (!m_bDoingSplite)
			{
				m_pIsolatedTool->SetNodeOnlyVisible(m_pCurIsolateNode, true);
			}
			m_pIsolatedTool->VerticalFlushCut(m_bDoingSplite);
			m_bDoingSplite = true;
		}
		else if (pWebData->strCommand == "Web_IsolateVerticalSingleCut")
		{
			if (!m_bIsolatedEnable || !m_pCurIsolateNode)
			{
				return;
			}
			IShellRootNode* pShellRootNode = (IShellRootNode*)m_pCurIsolateNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
			if (pShellRootNode)
			{
				ShellNodeParam tParam = SetShellGenerateParam();
				pShellRootNode->iSetGenerateNodeParam(tParam);
			}
			if (!m_bDoingSplite)
			{
				m_pIsolatedTool->SetNodeOnlyVisible(m_pCurIsolateNode, true);
			}
			m_pIsolatedTool->VerticalSingleCut(m_bDoingSplite);
			m_bDoingSplite = true;
		}
		else if (pWebData->strCommand == "Web_IsolateHorizontalCut")
		{
			if (!m_bIsolatedEnable || !m_pCurIsolateNode)
			{
				return;
			}
			IShellRootNode* pShellRootNode = (IShellRootNode*)m_pCurIsolateNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
			if (pShellRootNode)
			{
				ShellNodeParam tParam = SetShellGenerateParam();
				pShellRootNode->iSetGenerateNodeParam(tParam);
			}
			if (!m_bDoingSplite)
			{
				m_pIsolatedTool->SetNodeOnlyVisible(m_pCurIsolateNode, true);
			}
			m_pIsolatedTool->HorizontalCut(m_bDoingSplite);
			m_bDoingSplite = true;
		}
		else if (pWebData->strCommand == "Web_IsolateHorizontalFullCut")
		{
			if (!m_bIsolatedEnable || !m_pCurIsolateNode)
			{
				return;
			}
			IShellRootNode* pShellRootNode = (IShellRootNode*)m_pCurIsolateNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
			if (pShellRootNode)
			{
				ShellNodeParam tParam = SetShellGenerateParam();
				pShellRootNode->iSetGenerateNodeParam(tParam);
			}
			if (!m_bDoingSplite)
			{
				m_pIsolatedTool->SetNodeOnlyVisible(m_pCurIsolateNode, true);
			}
			m_pIsolatedTool->HorizontalFullCut(m_bDoingSplite);
			m_bDoingSplite = true;
		}
		else if (pWebData->strCommand == "Web_IsolateSaveChanges")
		{
			if (!m_bIsolatedEnable)
			{
				return;
			}
			//没有单体化就  “完成”
			if (m_bIndividualBuilding || m_bDoingSplite)
			{
				m_pIsolatedTool->CancelPonlygonSelector();
			}
			m_bIndividualBuilding = false;
			//保存到文件
			SaveChange();
			refreshAllShell();
			m_bDoingSplite = false;
			////恢复到正常视图
			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iSetView(PROJECTION_VIEW);
			//获取当前的shell传给网页
			if (s_pThis->m_pCurIsolateNode)
			{
				AssembleShellToWeb(s_pThis->m_pCurIsolateNode, "");
				s_pThis->m_pCurIsolateNode = nullptr;
			}
			m_pIsolatedTool->SetNodeOnlyVisible(nullptr, true);
		}
		else if (pWebData->strCommand == "Web_IsolateCancel")
		{
			//没有单体化就  “取消”
			if (m_bIndividualBuilding || m_bDoingSplite)
			{
				m_pIsolatedTool->CancelPonlygonSelector();
			}
			m_bDoingSplite = false;
			m_bIndividualBuilding = false;
			CancelChange();
			m_pCurIsolateNode = nullptr;
			////恢复到正常视图
			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iSetView(PROJECTION_VIEW);
			//更新网页的模型节点
			AssembleShellToWeb(nullptr, "");
			m_pIsolatedTool->SetNodeOnlyVisible(nullptr, true);
		}
		else if (pWebData->strCommand == "Web_IsolateSaveFile")
		{
			if (!m_bIsolatedEnable)
			{
				return;
			}
			if (m_pCurIsolateNode)
			{
				std::string strGroupID = m_pCurIsolateNode->iGetParent()->iGetFlagName();
				INode* pGroup = m_pModuleGroupNode->iFindNodeByID(strGroupID);
				if (pGroup)
				{
					pGroup->GetDynamicComponent<IGroup>()->iSave(pGroup->iGetFlagName());
				}
			}
			SaveChange();
		}
		else if (pWebData->strCommand == "Web_IsolateGetAllNode")
		{
			if (!m_bIsolatedEnable)
			{
				return;
			}
			AssembleShellToWeb(nullptr, pWebData->strID);
		}
		else if (pWebData->strCommand == "Web_IsolateDeleteNode")
		{
			if (!m_bIsolatedEnable)
			{
				return;
			}
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strNodeId = pWebData->vecValues[0].asString();
				INode* pNode = iGetRootNode()->iFindNodeByID(strNodeId);
				if (pNode)
				{
					if (std::find(m_vecNewBuilding.begin(), m_vecNewBuilding.end(), pNode) != m_vecNewBuilding.end())
					{
						m_vecNewBuilding.erase(std::find(m_vecNewBuilding.begin(), m_vecNewBuilding.end(), pNode));
					}
					if (std::find(m_vecSplitBuilding.begin(), m_vecSplitBuilding.end(), pNode) != m_vecSplitBuilding.end())
					{
						m_vecSplitBuilding.erase(std::find(m_vecSplitBuilding.begin(), m_vecSplitBuilding.end(), pNode));
					}
					IShellRootNode* pShellRootNode = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
					if (pShellRootNode)
					{
						std::vector<IShellNode*> vecShell = pShellRootNode->iGetShellNodes();
						for (auto pShell : vecShell)
						{
							if (pShell)
							{
								ShellNodeParam tParam = pShell->iGetShellParam();
								m_mapShellAndShellRootId.erase(tParam.strID);
							}
						}
						APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&pNode);
						//场景路径
						std::string strShellPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strProjectDir;
						//删除文件
						std::string strDeleteTw4 = m_strSaveDirPath + "/" + strNodeId + ".tw4";
						if (IsFileExist(strDeleteTw4))
						{
							BCXmlFile xFile;
							XMLDoc xmlDoc;
							std::string strSceneFile;
							std::string strScene;
							std::string strMshFile;
							std::string strMsh;
							std::string strTextureFile;
							std::string strTexture;
							if (xFile.Load(strDeleteTw4.c_str(), true))
							{
								char* pXml = xmlDoc.allocate_string(0, xFile.GetBufferLength());
								memset(pXml, 0, xFile.GetBufferLength());
								//size_t nLen = xFile.ToASCII(pXml);
								size_t nLen = xFile.GetUtf8Buff(pXml, xFile.GetBufferLength());
								xmlDoc.parse<0>(pXml);
								XMLNodePtr pScene = xmlDoc.first_node("SCENE");
								XMLNodePtr pRoot = pScene->first_node("ROOT");
								XMLNodePtr pNode = pRoot->first_node("NODE");
								strScene = pNode->first_node("SUB_SCENE")->value();
								strSceneFile = strShellPath + "/Shell/TW4/" + strScene;
								strMsh = pNode->first_node("MSH_NAME")->value();
								strMshFile = strShellPath + "/Shell/MSH/" + strMsh;
								strTexture = pNode->first_node("TEXTURE_NAME")->value();
								strTextureFile = strShellPath + "/Shell/TEXTURES/" + strTexture;
							}
							if (m_bAutoBackup)
							{
								struct tm ptrTime;
								std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
								localtime_s(&ptrTime, &time);
								std::string strCurDate = std::to_string(ptrTime.tm_year + 1900) + "_" +
									(ptrTime.tm_mon + 1 > 9 ? std::to_string(ptrTime.tm_mon + 1) : "0" + std::to_string(ptrTime.tm_mon + 1)) + "_" +
									(ptrTime.tm_mday > 9 ? std::to_string(ptrTime.tm_mday) : "0" + std::to_string(ptrTime.tm_mday));
								std::string strResourcePath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strResourceDir;
								std::string strProjectName = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strClientDLL;
								std::string strShellBackupPath = strResourcePath + "/shell_backup/" + strProjectName;
								std::string strBackOperatorShellTw4 = strShellBackupPath + "/BackOperator/Deleted/Shell/TW4/" + strCurDate;
								std::string strBackOperatorShellMsh = strShellBackupPath + "/BackOperator/Deleted/Shell/MSH/" + strCurDate;
								std::string strBackOperatorShellTextures = strShellBackupPath + "/BackOperator/Deleted/Shell/TEXTURES/" + strCurDate;
								std::string strBackOperatorSaveTw4 = strShellBackupPath + "/BackOperator/Deleted/shells/Tw4/" + strCurDate;
								Mkdirs(strBackOperatorShellTw4.c_str());
								Mkdirs(strBackOperatorShellMsh.c_str());
								Mkdirs(strBackOperatorShellTextures.c_str());
								Mkdirs(strBackOperatorSaveTw4.c_str());
								if (IsFileExist(strSceneFile))
								{
									Copy(strSceneFile, strBackOperatorShellTw4 + "/" + strScene, true);
								}
								if (IsFileExist(strMshFile))
								{
									Copy(strMshFile, strBackOperatorShellMsh + "/" + strMsh, true);
								}
								if (IsFileExist(strTextureFile))
								{
									Copy(strTextureFile, strBackOperatorShellTextures + "/" + strTexture, true);
								}
								Copy(strDeleteTw4, strBackOperatorSaveTw4 + "/" + strNodeId + ".tw4", true);
							}
							remove(strSceneFile.c_str());
							remove(strMshFile.c_str());
							remove(strTextureFile.c_str());
							remove(strDeleteTw4.c_str());
						}
					}
				}
			}
			refreshAllShell();
		}
		else if (pWebData->strCommand == "Web_IsolateSelectNode")
		{
			if (pWebData->vecValues.size() >= 2)
			{
				std::string strNodeId = pWebData->vecValues[0].asString();
				INode* pNode = m_pModuleGroupNode->iFindNodeByID(strNodeId);
				if (pNode)
				{
					IComponent* pComponent = pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
					if (pComponent)
					{
						std::string strSelect = "false";
						std::string strData = "'" + pNode->iGetID() + "'";
						strData += ",";
						strData += strSelect;
						m_pClient->ToSendWebCommond("VS_IsolateSelectedNode", "", strData);
					}
				}

				bool bJump = pWebData->vecValues[1].asBool();
				IShellNode* pShell = iGetShellNodeById(strNodeId);
				if (pShell)
				{
					INode* pShellParentNode = pShell->iGetRoot()->iGetOriginNode();
					if (pShellParentNode)
					{
						if (bJump && !m_bDoingSplite && !m_bIndividualBuilding)
						{
							Vector3d vPos = pShellParentNode->iGetOrigin();
							Vector3d vAngle(30.0f, 89.0f, 0.0f);
							INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
							GoViewPoint(pCameraNode, vPos, vAngle, 200.0f, Vector2(0, 0), false);
						}
					}
				}
			}
		}
		else if (pWebData->strCommand == "Web_IsolateCheckMatch")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strId = pWebData->vecValues[0].asString();
				INode* pNode = iGetRootNode()->iFindNodeByID(strId);
				Json::Value jArray = Json::arrayValue;
				if (pNode)
				{
					IShellRootNode* pShellRoot = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
					if (pShellRoot)
					{
						std::vector<IShellNode*> vecShells = pShellRoot->iGetShellNodes();
						for (auto pShell : vecShells)
						{
							if (pShell)
							{
								std::string strData = "";
								YBSSInfo* pInfo = (YBSSInfo*)pShell->iGetExternParam();
								if (pInfo)
								{
									strData = pInfo->strExtra;
								}
								else
								{
									YBSSInfo stInfo;
									strData = stInfo.strExtra;
								}
								Json::Value jResult;
								ParseJsonByString(strData, jResult);
								if (jResult["bind_type"] == 2)
								{
									jArray.append(jResult);
								}
							}
						}
					}
				}
				IShellNode* pShell = iGetShellNodeById(strId);
				if (pShell)
				{
					std::string strData = "";
					YBSSInfo* pInfo = (YBSSInfo*)pShell->iGetExternParam();
					if (pInfo)
					{
						strData = pInfo->strExtra;
					}
					else
					{
						YBSSInfo stInfo;
						strData = stInfo.strExtra;
					}
					Json::Value jResult;
					ParseJsonByString(strData, jResult);
					if (jResult["bind_type"] == 2)
					{
						jArray.append(jResult);
					}
				}
				std::string strResult = jArray.toStyledString();
				m_pClient->ToSendWebCommond("VS_IsolateCheckMatch", pWebData->strID, strResult);
			}
		}
		else if (pWebData->strCommand == "Web_IsolateMatchResult")
		{
			if (pWebData->vecValues.size() >= 3)
			{
				std::string strShellId = pWebData->vecValues[0].asString();
				bool bMatch = pWebData->vecValues[1].asBool();
				IShellNode* pShell = iGetShellNodeById(strShellId);
				if (pShell)
				{
					Json::Value& jInfo = pWebData->vecValues[2];
					if (bMatch)
					{
						YBSSInfo* pInfo = (YBSSInfo*)pShell->iGetExternParam();
						if (pInfo == nullptr)
						{
							pInfo = new YBSSInfo;
						}
						parseYBSSInfo(pInfo, jInfo);
						pInfo->strNodeID = strShellId;
						pShell->iSetExternParam((INT_PTR)pInfo);
					}
					else
					{
						YBSSInfo* pInfo = (YBSSInfo*)pShell->iGetExternParam();
						DELETE_PTR(pInfo);
						pInfo = new YBSSInfo;
						pInfo->strNodeID = strShellId;
						pShell->iSetExternParam((INT_PTR)pInfo);
					}
					iSetShellAttr(pShell);
				}
			}
		}
		else if (pWebData->strCommand == "Web_GetChildrenByNodeID")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strNodeID = pWebData->vecValues[0].asString();
				Json::Value jItem = {};
				jItem["id"] = "";
				jItem["name"] = "";
				jItem["children"] = Json::arrayValue;
				std::vector<IShellNode*> vecShell;
				if (!strNodeID.empty())
				{
					INode* pNode = m_pModuleGroupNode->iFindNodeByID(strNodeID);
					if (pNode)
					{
						jItem["id"] = pNode->iGetID();
						jItem["name"] = pNode->iGetID();
						IShellRootNode* pShellRoot = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
						if (pShellRoot)
						{
							std::vector<IShellNode*> vecShellNode = pShellRoot->iGetShellNodes();
							vecShell.insert(vecShell.end(), vecShellNode.begin(), vecShellNode.end());
						}
						for (auto pShell : vecShell)
						{
							if (pShell)
							{
								Json::Value jItemShell;
								ShellNodeParam tParam;
								pShell->iGetShellParam(tParam);
								jItemShell["id"] = tParam.strID;
								jItemShell["name"] = tParam.strID;
								jItem["children"].append(jItemShell);
							}
						}
					}
				}
				std::string strData = jItem.toStyledString();
				m_pClient->ToSendWebCommond("VS_GetChildrenByNodeID", pWebData->strID, strData);
			}
		}
		else if (pWebData->strCommand == "Web_SearchShellNodeByKey")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strKey = pWebData->vecValues[0].asString();
				std::vector<INode*> vecNode;
				m_pModuleGroupNode->iGetAllChildren(vecNode);
				std::map<INode*, std::vector<INode*>> mapTree;
				int nAllCount = 0;
				for (size_t i = 0; i < vecNode.size(); ++i)
				{
					INode* pTemp = vecNode[i];
					if (pTemp->iGetNodeType() != NODE_SHELL)
					{
						continue;
					}
					if (pTemp->iGetID().find(strKey) != std::string::npos
						|| pTemp->iGetName().find(strKey) != std::string::npos)
					{
						mapTree[pTemp->iGetParent()].push_back(pTemp);
						nAllCount++;
					}
					if (nAllCount > 5000)
					{
						break;
					}
				}
				Json::Value jArray = Json::arrayValue;
				for (std::map<INode*, std::vector<INode*>>::iterator it = mapTree.begin(); it != mapTree.end(); ++it)
				{
					Json::Value jGroupItem = {};
					jGroupItem["id"] = it->first->iGetID();
					jGroupItem["name"] = it->first->iGetID();
					jGroupItem["children"] = Json::arrayValue;
					for (size_t i = 0; i < it->second.size(); ++i)
					{
						Json::Value item = {};
						item["id"] = it->second[i]->iGetID();
						item["name"] = it->second[i]->iGetID();
						jGroupItem["children"].append(item);
					}
					jArray.append(jGroupItem);
				}
				std::string strData = jArray.toStyledString();
				m_pClient->ToSendWebCommond("VS_SearchShellNodeByKey", pWebData->strID, strData);
			}
		}

		INode* pTemp = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentSelectNode();
		if (m_bDoingSplite && pTemp)
		{
			setShellHoverEnable(pTemp, true);
			pTemp->iSetSelect(false);
		}
	}
	catch (const std::exception&)
	{
	}
}

IShellNode* YBSSModule::getSelectShellNode()
{
	//if (!m_pModuleGroupNode)
	//{
	//	return nullptr;
	//}
	//INode* pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentSelectNode();
	//if (pNode)
	//{
	//	IShellRootNode* pShellRoot = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
	//	if (pShellRoot && pShellRoot->iGetSelectNode())
	//	{
	//		return pShellRoot->iGetSelectNode();
	//	}
	//}
	return nullptr;
}

void YBSSModule::AssembleShellToWeb(INode* pHouseShellNode, const std::string& strWebId)
{
	Json::Value jArray = Json::arrayValue;
	std::map<INode*, std::vector<IShellNode*>> mapShell;
	std::vector<IShellNode*> vecShell;
	if (pHouseShellNode)
	{
		IShellRootNode* pShellRoot = (IShellRootNode*)pHouseShellNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
		if (pShellRoot)
		{
			std::vector<IShellNode*> vecShellNode = pShellRoot->iGetShellNodes();
			vecShell.insert(vecShell.end(), vecShellNode.begin(), vecShellNode.end());
			mapShell[pHouseShellNode] = vecShell;
		}
	}
	else
	{
		std::vector<INode*> vecNode;
		if (!m_pModuleGroupNode)
		{
			return;
		}
		std::vector<INode*> vecChildren;
		m_pModuleGroupNode->iGetAllChildren(vecChildren);
		for (int i = 0; i < vecChildren.size(); ++i)
		{
			IShellRootNode* pShellRoot = (IShellRootNode*)vecChildren[i]->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
			if (pShellRoot)
			{
				std::vector<IShellNode*> vecShellNode = pShellRoot->iGetShellNodes();
				vecShell.insert(vecShell.end(), vecShellNode.begin(), vecShellNode.end());
				mapShell[vecChildren[i]] = vecShell;
				vecShell.clear();
			}
		}
	}
	for (auto it : mapShell)
	{
		INode* pNode = it.first;
		std::vector<IShellNode*> vecShell = it.second;
		if (!pNode)
		{
			continue;
		}
		else
		{
			Json::Value jItem;
			jItem["id"] = pNode->iGetID();
			jItem["name"] = pNode->iGetID();
			jItem["children"] = Json::arrayValue;
			//for (auto pShell : vecShell)
			//{
			//	if (pShell)
			//	{
			//		Json::Value jItemShell;
			//		ShellNodeParam tParam;
			//		pShell->iGetShellParam(tParam);
			//		jItemShell["id"] = tParam.strId;
			//		jItemShell["name"] = tParam.strId;
			//		jItem["children"].append(jItemShell);
			//	}
			//}
			jArray.append(jItem);
		}
	}
	std::string strData = jArray.toStyledString();
	m_pClient->ToSendWebCommond("VS_IsolateChangeShell", strWebId, strData);
}

void YBSSModule::clear()
{
	//for (size_t i = 0; i < m_vecYBSSInfo.size(); ++i)
	//{
	//	DELETE_PTR(m_vecYBSSInfo[i]);
	//}
	//m_vecYBSSInfo.clear();

	std::vector<INode*> vecShellRoot;
	APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetTypeNode(vecShellRoot, NODE_SHELL_GROUP, m_pModuleGroupNode);

	for (size_t i = 0; i < vecShellRoot.size(); ++i)
	{
		IShellRootNode* pShellRoot = (IShellRootNode*)vecShellRoot[i]->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
		if (pShellRoot)
		{
			std::vector<IShellNode*> vecShellNode = pShellRoot->iGetShellNodes();
			for (auto it : vecShellNode)
			{
				YBSSInfo* pInfo = (YBSSInfo*)it->iGetExternParam();
				if (pInfo)
				{
					DELETE_PTR(pInfo);
				}
			}
		}
	}
	std::stack<IShellNode*> stackTemp1;
	std::swap(m_stackRequestShellNode, stackTemp1);
}

void YBSSModule::refreshAllShell()
{
}

void YBSSModule::doRequest()
{
	int nCount = 0;
	//一次处理100个
	int nMaxCount = 1000;
	std::string strIds = "";
	std::vector<std::string> vecNode;
	if (m_stackRequestShellNode.size() > 0)
	{
		ProjectLogger::GetInstance()->InfoMessage(std::string("m_stackRequestShell current count=") + to_string(m_stackRequestShellNode.size()));
	}
	while (!m_stackRequestShellNode.empty() && nCount < nMaxCount)
	{
		IShellNode* pShellNode = m_stackRequestShellNode.top();
		if (!strIds.empty())
		{
			strIds += ",";
		}
		const ShellNodeParam& tParam = pShellNode->iGetShellParam();
		strIds += tParam.strID;
		vecNode.push_back(tParam.strID);
		++nCount;
		m_stackRequestShellNode.pop();
	}
	if (strIds.empty())
	{
		return;
	}
	ProjectLogger::GetInstance()->InfoMessage(std::string("m_stackRequestShell last count=") + to_string(m_stackRequestShellNode.size()));
	//进行网络请求
	YBSSNetTask* pTask = new YBSSNetTask;
	pTask->vecNode = vecNode;

	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_API_POST;
	stParam.pPreDoRequestFunc = static_pre_request_BindList;
	stParam.pThreadCompleteFunc = static_callback_BindList;
	stParam.bRunCallbackInMainThread = true;
	//stParam.bRunCallbackInMainThread = false;
	stParam.bThread = true;
	stParam.bInQueue = true;
	stParam.strQueueID = THREAD_QUEUE_ID;
	char szBuffer[1024] = { 0 };
	sprintf_s(szBuffer, sizeof(szBuffer), "%s/api/tz/shangtu/getBindList?pageSize=%d", m_pClient->m_strApiHost.c_str(), (nMaxCount + 100));
	//sprintf_s(szBuffer, sizeof(szBuffer), "http://192.168.30.209:8080/api/tz/shangtu/getBindList");
	stParam.strUrl = szBuffer;
	m_pClient->AddHttpCommonParam(&stParam);
	stParam.AddParam("this", (INT_PTR)this);
	stParam.AddParam("netTask", (INT_PTR)pTask);
	stParam.vecHttpPostParam.push_back({ "zone",strIds });
	ThreadWrapper::GetInstance()->AddTask(stParam);
}

void YBSSModule::static_pre_request_BindList(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam)
{
	YBSSModule* pThis = (YBSSModule*)mapParams["this"];
	pThis->m_pCurThreadBindListCallback = pParam;
}

void YBSSModule::static_callback_BindList(ThreadCallbackParam* pParam)
{
	YBSSModule* pThis = (YBSSModule*)pParam->sRequestParam.mapParams["this"];
	if (pParam == pThis->m_pCurThreadBindListCallback)
	{
		pThis->m_pCurThreadBindListCallback = nullptr;
	}
	YBSSNetTask* pTask = (YBSSNetTask*)pParam->sRequestParam.mapParams["netTask"];
	bool bIgnore = false;
	if (pParam->sRequestParam.mapJValue.find("is_ignore") != pParam->sRequestParam.mapJValue.end())
	{
		if (pParam->sRequestParam.mapJValue["is_ignore"].asBool())
		{
			bIgnore = true;
		}
	}
	if (bIgnore)
	{
		//忽略则作废
		return;
	}
	if (pParam->nResult == STATUS_SUCCESS)
	{
		try
		{
			Json::Value& jData = pParam->jRoot["data"];
			for (int i = 0; i < jData.size(); ++i)
			{
				if (!s_pThis->m_pModuleGroupNode)
				{
					return;
				}
				Json::Value& jItem = jData[i];

				std::string strId = jItem["bind_shell"].asString();
				if (strId.empty() && pTask->vecNode.size() > i)
				{
					strId = pTask->vecNode[i];
				}
				std::map<std::string, std::string>::iterator it = pThis->m_mapShellAndShellRootId.find(strId);
				if (it == pThis->m_mapShellAndShellRootId.end())
				{
					continue;
				}
				IShellNode* pShellNode = pThis->iGetShellNodeById(strId, pThis->m_mapShellAndShellRootId[strId]);
				if (!pShellNode)
				{
					continue;
				}
				YBSSInfo* pInfo = (YBSSInfo*)pShellNode->iGetExternParam();
				if (pInfo)
				{
					pThis->parseYBSSInfo(pInfo, jItem);
					pInfo->strNodeID = strId;
					pInfo->bRequested = true;
				}
				pThis->iSetShellAttr(pShellNode);
			}
		}
		catch (const std::exception&)
		{
		}
	}
	else
	{
		for (size_t i = 0; i < pTask->vecNode.size(); ++i)
		{
			if (!s_pThis->m_pModuleGroupNode)
			{
				return;
			}
			IShellNode* pShellNode = s_pThis->iGetShellNodeById(pTask->vecNode[i]);
			//INode* pNode = s_pThis->m_pModuleGroupNode->iFindNodeByID(pTask->vecNode[i]);
			if (!pShellNode)
			{
				continue;
			}
			YBSSInfo* pInfo = (YBSSInfo*)pShellNode->iGetExternParam();
			if (pInfo)
			{
				pInfo->bRequested = true;
				pInfo->strNodeID = pTask->vecNode[i];
			}
			pThis->iSetShellAttr(pShellNode);
		}
	}
	pTask->vecNode.clear();
	DELETE_PTR(pTask);
}

void YBSSModule::parseYBSSInfo(YBSSInfo* pInfo, Json::Value& jObject)
{
	try
	{
		pInfo->strAddress = jObject["address"].asString();
		pInfo->strBindId = jObject["bind_id"].asString();
		if (jObject["bind_type"].isInt())
		{
			pInfo->nBindType = jObject["bind_type"].asInt();
		}
		pInfo->strBuildingId = jObject["building_id"].asString();
		pInfo->strBindShell = jObject["bind_shell"].asString();
		pInfo->strExtra = jObject.toStyledString();
	}
	catch (const std::exception&)
	{
	}
}

void YBSSModule::setColor(IShellNode* pNode)
{
	if (!pNode)
	{
		return;
	}
	ShellNodeParam stParam;
	pNode->iGetShellParam(stParam);

	YBSSInfo* pInfo = (YBSSInfo*)pNode->iGetExternParam();
	bool bMatch = false;	//是否关联
	if (pInfo)
	{
		bMatch = (pInfo->nBindType == 2);
	}
	Vector4 vBaseColor = Vector4(255 / 255.0f, 0 / 255.0f, 255 / 255.0f, 0.5);
	Vector4 vSelectColor = Vector4(212 / 255.0f, 152 / 255.0f, 255 / 255.0f, 0.5);
	Vector4 vHoverColor = Vector4(112 / 255.0f, 25 / 255.0f, 174 / 255.0f, 0.2);
	if (bMatch)
	{
		vBaseColor = Vector4(0 / 255.0f, 255 / 255.0f, 255 / 255.0f, 0.3);
		vSelectColor = Vector4(0 / 255.0f, 222 / 255.0f, 222 / 255.0f, 0.5);
		vHoverColor = Vector4(0 / 255.0f, 255 / 255.0f, 255 / 255.0f, 0.2);
	}
	stParam.bHoverEnable = true;
	stParam.bClickEnable = true;
	stParam.bVisible = true;
	stParam.vBaseColor = vBaseColor;
	stParam.vSelectColor = vSelectColor;
	stParam.vHoverColor = vHoverColor;
	pNode->iSetShellParam(stParam);
}

void YBSSModule::LoadYBSSXml(std::string& strAppId)
{
	//切换到不可编辑状态
	WebCommandData stWebData;
	stWebData.strCommand = "Web_SetIsolateEnable";
	stWebData.vecValues.push_back(false);
	iDoWebCommond(&stWebData);
	{
		m_lLastTime = 0;
		if (m_pModuleGroupNode)
		{
			if (m_pCurThreadBindListCallback)
			{
				m_pCurThreadBindListCallback->sRequestParam.AddExtraJValue("is_ignore", true);
			}
			clear();
			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pModuleGroupNode);
			m_pModuleGroupNode = createModuleGroupNode();
			m_vecNeedGenerateMeshShellGroup.clear();
			std::stack<IShellNode*> stackTemp1;
			m_stackRequestShellNode.swap(stackTemp1);
			std::queue<std::string> queueTemp1;
			m_queueLoadXmlFile.swap(queueTemp1);
			ThreadWrapper::GetInstance()->ClearQueueTask(THREAD_QUEUE_ID);
		}

		std::string strFilePath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strModelsDir;
		strFilePath += "/shells/";
		strFilePath += m_strAppID;
		m_strSaveDirPath = strFilePath;
		Mkdirs(strFilePath.c_str());
		//获取appid的所有shell并加载
		std::vector<FileInfo> vecFile;
		//不加载子文件夹的tw4
		GetFilesFromDir(strFilePath, vecFile, false, "tw4");
		for (size_t i = 0; i < vecFile.size(); ++i)
		{
			for (int zz = 0; zz < 1; zz++)
			{
#ifdef _WIN32
				std::string strPath = vecFile[i].strAbsoluteDir + "/" + vecFile[i].tInfo.name;
#else
				std::string strPath = vecFile[i].strAbsoluteDir + "/" + vecFile[i].tInfo->d_name;
#endif
				m_queueLoadXmlFile.push(strPath);
			}
		}

		//所有需要加载的一标三十XML文件
		m_nNeedLoadCount = m_queueLoadXmlFile.size();
		loadShellRootNode();

		ProjectLogger::GetInstance()->InfoMessage(std::string("YBSS path=") + strFilePath);
		ProjectLogger::GetInstance()->InfoMessage(std::string("YBSS load start xml count=") + to_string(m_queueLoadXmlFile.size()));
	}
}

void YBSSModule::setShellHoverEnable(INode* pNode, bool bEnable)
{
	if (!pNode)
	{
		return;
	}
	std::vector<INode*> vecShell;
	if (pNode->iGetNodeType() == NODE_GROUP
		|| pNode->iGetNodeType() == NODE_SHELL_GROUP)
	{
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetTypeNode(vecShell, NODE_SHELL, pNode);
	}
	else if (pNode->iGetNodeType() == NODE_SHELL)
	{
		vecShell.push_back(pNode);
	}
	for (size_t i = 0; i < vecShell.size(); ++i)
	{
		IShell* pShell = vecShell[i]->GetDynamicComponent<IShell>();
		if (pShell)
		{
			ShellParam stParam;
			pShell->iGetShellParam(stParam);
			stParam.bHoverEnable = bEnable;
			pShell->iSetShellParam(stParam);
		}
	}
}

void YBSSModule::SetVisible(bool bVisible)
{
	if (bVisible == m_bVisible)
	{
		return;
	}
	m_bVisible = bVisible;
	if (m_pModuleGroupNode)
	{
		m_pModuleGroupNode->iSetVisible(m_bVisible);
	}
}

void YBSSModule::iOnAppModeChanged(int nAppMode)
{
	APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iSetVisible((m_pClient->m_nAppMode & APP_MODE_EXHIBITION));
	checkReLoadXml();
	setShellShow();
}

void YBSSModule::iOnAppIdChanged(std::string strAppId)
{
	m_bReLoadXml = (m_strAppID != strAppId);
	m_strAppID = strAppId;
	m_strAccount = m_pClient->m_sUserInfo.strAccount;
	checkReLoadXml();
}

void YBSSModule::checkReLoadXml()
{
	if (m_bReLoadXml && (m_pClient->m_nAppMode & APP_MODE_EXHIBITION || m_pClient->m_nAppMode & APP_MODE_YBSS_EDIT))
	{
		m_bReLoadXml = false;
		LoadYBSSXml(m_strAppID);
	}
}

void YBSSModule::SetShellLoadDis(float fDis)
{
	m_fShellLoadDis = fDis;
	std::vector<INode*> vecShell;
	APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetTypeNode(vecShell, NODE_SHELL, m_pModuleGroupNode);
	for (size_t i = 0; i < vecShell.size(); ++i)
	{
		ShellParam stParam;
		vecShell[i]->GetDynamicComponent<IShell>()->iGetShellParam(stParam);
		stParam.fLoadDistance = m_fShellLoadDis;
		vecShell[i]->GetDynamicComponent<IShell>()->iSetShellParam(stParam);
	}
}

void YBSSModule::setShellShow()
{
	if (m_pModuleGroupNode)
	{
		int nAppMode = m_pClient->m_nAppMode;
		m_pModuleGroupNode->iSetVisible((m_bVisible && (nAppMode & APP_MODE_EXHIBITION)) || nAppMode & APP_MODE_YBSS_EDIT);
		std::vector<INode*> vecNode;
		m_pModuleGroupNode->iGetAllChildren(vecNode);
		if (nAppMode & APP_MODE_YBSS_EDIT)
		{
			for (size_t i = 0; i < vecNode.size(); ++i)
			{
				vecNode[i]->iSetVisible(true);
				IShellRootNode* pShellRoot = (IShellRootNode*)vecNode[i]->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
				if (pShellRoot)
				{
					std::vector<IShellNode*> vecShellNode = pShellRoot->iGetShellNodes();
					for (auto it : vecShellNode)
					{
						iSetShellAttr(it);
					}
				}
			}
		}
		else if (nAppMode & APP_MODE_EXHIBITION)
		{
			for (size_t i = 0; i < vecNode.size(); ++i)
			{
				IShellRootNode* pShellRoot = (IShellRootNode*)vecNode[i]->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
				if (pShellRoot)
				{
					std::vector<IShellNode*> vecShellNode = pShellRoot->iGetShellNodes();
					for (auto it : vecShellNode)
					{
						iSetShellAttr(it);
					}
				}
			}
		}
	}
}

void YBSSModule::frameUpdateLoadXml()
{
	int nCount = 0;
	while (!m_queueLoadXmlFile.empty() && nCount < 1)
	{
		nCount++;
		std::string strPath = m_queueLoadXmlFile.front();
		//ProjectLogger::GetInstance()->InfoMessage(std::string("YBSS load xml=") + strPath);
		if (IsFileExist(strPath))
		{
			BCXmlFile xFile;
			XMLDoc xmlDoc;
			if (xFile.Load(strPath.c_str(), true))
			{
				char* pXml = xmlDoc.allocate_string(0, xFile.GetBufferLength());
				memset(pXml, 0, xFile.GetBufferLength());
				size_t nLen = xFile.GetUtf8Buff(pXml, xFile.GetBufferLength());
				try
				{
					NodeParamBase stParam;
					stParam.nNodeType = NodeType_e::NODE_SHELL_GROUP;
					INode* pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(stParam);
					pNode->iSetFlagName(strPath);

					xmlDoc.parse<0>(pXml);
					XMLNodePtr pScene = xmlDoc.first_node("SCENE");
					XMLNodePtr pRoot = pScene->first_node("ROOT");
					XMLNodePtr pFirstNode = pRoot->first_node("NODE");
					IShellRootNode* pIShellRootNode = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
					if (pIShellRootNode)
					{
						pIShellRootNode->iLoad(pFirstNode);
						pNode->iLoadModel();
						m_pModuleGroupNode->iAddChild(pNode);
						pNode->GetDynamicComponent<IRenderNode>()->iSetClickEnable(true);
						pNode->GetDynamicComponent<IRenderNode>()->iSetHoverEnable(true);
						pNode->GetDynamicComponent<IRenderNode>()->iSetSelectEnable(true);
						std::vector<IShellNode*> vecShellNode = pIShellRootNode->iGetShellNodes();
						for (size_t j = 0; j < vecShellNode.size(); ++j)
						{
							std::string strFlagName = pNode->iGetID();
							vecShellNode[j]->iSetFlagName(strFlagName);
							vecShellNode[j]->iSetExternParam((INT_PTR)iCreateYBSSInfo());
							const ShellNodeParam& tParam = vecShellNode[j]->iGetShellParam();
							m_mapShellAndShellRootId[tParam.strID] = strFlagName;
						}
					}
					//ProjectLogger::GetInstance()->InfoMessage(std::string("YBSS xml sucess! File is:  ") + strPath);
				}
				catch (rapidxml::parse_error&)
				{
				}
			}
		}
		refreshAllShell();
		m_queueLoadXmlFile.pop();
		if (m_queueLoadXmlFile.empty())
		{
			//进行绑定请求
			std::vector<IShellNode*> vecAllShell;
			if (m_pModuleGroupNode)
			{
				std::vector<INode*> vec;
				m_pModuleGroupNode->iGetAllChildren(vec);
				for (size_t i = 0; i < vec.size(); ++i)
				{
					IShellRootNode* pShellRoot = (IShellRootNode*)vec[i]->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
					if (pShellRoot)
					{
						std::vector<IShellNode*> vecShell = pShellRoot->iGetShellNodes();
						vecAllShell.insert(vecAllShell.end(), vecShell.begin(), vecShell.end());
					}
				}
			}
			std::stack<IShellNode*> stackTemp1;
			std::swap(m_stackRequestShellNode, stackTemp1);
			for (size_t i = 0; i < vecAllShell.size(); ++i)
			{
				iSetShellAttr(vecAllShell[i]);
				YBSSInfo* pInfo = (YBSSInfo*)vecAllShell[i]->iGetExternParam();
				if (pInfo->bRequested)
				{
					//已经请求到了,不在请求
					continue;
				}
				m_stackRequestShellNode.push(vecAllShell[i]);
			}
			iOnAppModeChanged(m_pClient->m_nAppMode);
			iOnXmlLoaded();
			ProjectLogger::GetInstance()->InfoMessage(std::string("All ShellNode count=") + to_string(vecAllShell.size()));
			ProjectLogger::GetInstance()->InfoMessage(std::string("Request ShellNode count=") + to_string(m_stackRequestShellNode.size()));
		}
	}
}

YBSSInfo* YBSSModule::iCreateYBSSInfo()
{
	YBSSInfo* pInfo = new YBSSInfo;
	return pInfo;
}

void YBSSModule::iSetShellAttr(IShellNode* pShellNode)
{
	setColor(pShellNode);
}

Vector3 YBSSModule::getNodeOrigin(INode* pNode)
{
	Vector3d vOrigin;
	if (!pNode)
	{
		return vOrigin;
	}
	int nCount = 0;
	std::vector<INode*> vecNode;
	pNode->iGetAllChildren(vecNode);
	vecNode.push_back(pNode);
	for (size_t i = 0; i < vecNode.size(); ++i)
	{
		if (vecNode[i]->iGetNodeType() != NODE_GROUP)
		{
			vOrigin += vecNode[i]->iGetOrigin();
			nCount++;
		}
	}
	if (nCount > 0)
	{
		vOrigin /= nCount;
	}
	return vOrigin;
}

IShellNode* YBSSModule::iGetShellNodeById(std::string strShellId, std::string strShellRootId)
{
	if (m_pModuleGroupNode)
	{
		if (strShellRootId.empty())
		{
			std::vector<INode*> vecNode;
			m_pModuleGroupNode->iGetAllChildren(vecNode);
			for (int i = 0; i < vecNode.size(); ++i)
			{
				IShellRootNode* pShellRoot = (IShellRootNode*)vecNode[i]->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
				if (pShellRoot)
				{
					std::vector<IShellNode*> vecShellNode = pShellRoot->iGetShellNodes();
					for (size_t j = 0; j < vecShellNode.size(); ++j)
					{
						//ShellNodeParam tParam;
						//vecShellNode[j]->iGetShellParam(tParam);
						const ShellNodeParam& tParam = vecShellNode[j]->iGetShellParam();
						if (strShellId == tParam.strID)
						{
							return vecShellNode[j];
						}
					}
				}
			}
		}
		else
		{
			INode* pNode = m_pModuleGroupNode->iFindNodeByID(strShellRootId);
			if (pNode)
			{
				IShellRootNode* pShellRoot = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
				if (pShellRoot)
				{
					std::vector<IShellNode*> vecShellNode = pShellRoot->iGetShellNodes();
					for (size_t j = 0; j < vecShellNode.size(); ++j)
					{
						//ShellNodeParam tParam;
						//vecShellNode[j]->iGetShellParam(tParam);
						const ShellNodeParam& tParam = vecShellNode[j]->iGetShellParam();
						if (strShellId == tParam.strID)
						{
							return vecShellNode[j];
						}
					}
				}
			}
		}
		return nullptr;
	}
	else
	{
		return nullptr;
	}
}

void YBSSModule::Static_CreateShellRootNode(INode* pSelector)
{
	IPolygonSelector* pPolygonSelector = pSelector->GetDynamicComponent<IPolygonSelector>();
	if (pPolygonSelector)
	{
		std::vector<Vector3> vecPoints;
		pPolygonSelector->iGetControlPoints(vecPoints);
		if (vecPoints.size() < 3)
		{
			pPolygonSelector->iFinish();
			APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_SELECT;
			return;
		}
		ShellRootParam tParam;
		tParam.bIsolocation = true;
		tParam.vecIsolocatePoint = ConvertVecVector3ToD(vecPoints);
		INode* pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(tParam);
		pNode->iSetVisible(true);
		pNode->GetDynamicComponent<IRenderNode>()->iSetHoverEnable(true);
		pNode->GetDynamicComponent<IRenderNode>()->iSetSelectEnable(true);
		pNode->GetDynamicComponent<IRenderNode>()->iSetClickEnable(true);
		pNode->iLoadModel();
		if (s_pThis != nullptr)
		{
			s_pThis->iGetModuleGroupNode()->iAddChild(pNode);
			s_pThis->AssembleShellToWeb(pNode, "");

			IShellRootNode* pShellRoot = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
			if (pShellRoot)
			{
				std::vector<IShellNode*> vecShellNode = pShellRoot->iGetShellNodes();
				for (auto pShell : vecShellNode)
				{
					ShellNodeParam tParam;
					pShell->iGetShellParam(tParam);
					ShellNodeParam tGenerataParam = s_pThis->SetShellGenerateParam();
					tParam.vBaseColor = tGenerataParam.vBaseColor;
					tParam.vHoverColor = tGenerataParam.vHoverColor;
					tParam.vSelectColor = tGenerataParam.vSelectColor;
					pShell->iSetShellParam(tParam);
				}
			}
			//添加到新的单体化楼栋
			s_pThis->m_vecNewBuilding.push_back(pNode);
		}
		else
		{
			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode()->iAddChild(pNode);
		}
		pPolygonSelector->iFinish();
		APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_SELECT;
	}
}

void YBSSModule::Static_CreateShellNodesSplitVerticalRect(INode* pSelector)
{
	IPolygonSelector* pPolygonSelector = pSelector->GetDynamicComponent<IPolygonSelector>();
	if (pPolygonSelector)
	{
		std::vector<Vector3> vecPoints;
		pPolygonSelector->iGetControlPoints(vecPoints);
		if (vecPoints.size() < 3)
		{
			//s_pThis->m_bDoingSplite = false;
			pPolygonSelector->iFinish();
			APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_SELECT;
			return;
		}
		INode* pNode = s_pThis->m_pCurIsolateNode;
		if (pNode && pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT))
		{
			IShellRootNode* pShellRootNode = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
			pShellRootNode->iSplitVerticalRect(vecPoints);

			std::vector<IShellNode*> vecShellNode = pShellRootNode->iGetShellNodes();
			for (auto pShell : vecShellNode)
			{
				ShellNodeParam tParam;
				pShell->iGetShellParam(tParam);
				ShellNodeParam tGenerataParam = s_pThis->SetShellGenerateParam();
				tParam.vBaseColor = tGenerataParam.vBaseColor;
				tParam.vHoverColor = tGenerataParam.vHoverColor;
				tParam.vSelectColor = tGenerataParam.vSelectColor;
				tParam.bVisible = tParam.bClickEnable = tParam.bHoverEnable = true;
				pShell->iSetShellParam(tParam);
			}
			s_pThis->m_vecSplitBuilding.push_back(pNode);
		}
		//s_pThis->m_bDoingSplite = false;
		pPolygonSelector->iFinish();
		APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_SELECT;
	}
	if (s_pThis->m_bContinuousCutting)
	{
		s_pThis->m_pIsolatedTool->VerticalTorusCut(true);
	}
}

void YBSSModule::Static_CreateShellNodesSplitVerticalPlanel(INode* pSelector)
{
	IPolygonSelector* pPolygonSelector = pSelector->GetDynamicComponent<IPolygonSelector>();
	if (pPolygonSelector)
	{
		std::vector<Vector3> vecPoints;
		pPolygonSelector->iGetControlPoints(vecPoints);
		if (vecPoints.size() < 2)
		{
			//s_pThis->m_bDoingSplite = false;
			pPolygonSelector->iFinish();
			APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_SELECT;
			return;
		}
		INode* pNode = s_pThis->m_pCurIsolateNode;
		if (pNode && pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT))
		{
			IShellRootNode* pShellRootNode = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
			pShellRootNode->iSplitVerticalPlanel(vecPoints[0], vecPoints[1]);

			std::vector<IShellNode*> vecShellNode = pShellRootNode->iGetShellNodes();
			for (auto pShell : vecShellNode)
			{
				ShellNodeParam tParam;
				pShell->iGetShellParam(tParam);
				ShellNodeParam tGenerataParam = s_pThis->SetShellGenerateParam();
				tParam.vBaseColor = tGenerataParam.vBaseColor;
				tParam.vHoverColor = tGenerataParam.vHoverColor;
				tParam.vSelectColor = tGenerataParam.vSelectColor;
				tParam.bVisible = tParam.bClickEnable = tParam.bHoverEnable = true;
				pShell->iSetShellParam(tParam);
			}
			s_pThis->m_vecSplitBuilding.push_back(pNode);
		}
		//s_pThis->m_bDoingSplite = false;
		pPolygonSelector->iFinish();
		APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_SELECT;
	}
	if (s_pThis->m_bContinuousCutting)
	{
		s_pThis->m_pIsolatedTool->VerticalFlushCut(true);
	}
}

void YBSSModule::Static_CreateShellNodesSplitVerticalSingleCut(INode* pSelector)
{
	IPolygonSelector* pPolygonSelector = pSelector->GetDynamicComponent<IPolygonSelector>();
	if (pPolygonSelector)
	{
		std::vector<Vector3> vecPoints;
		pPolygonSelector->iGetControlPoints(vecPoints);
		if (vecPoints.size() < 2)
		{
			//s_pThis->m_bDoingSplite = false;
			pPolygonSelector->iFinish();
			APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_SELECT;
			return;
		}
		INode* pNode = s_pThis->m_pCurIsolateNode;
		if (pNode && pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT))
		{
			IShellRootNode* pShellRootNode = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
			pShellRootNode->iSplitSingleVerticalPlanel(vecPoints[0], vecPoints[1]);

			std::vector<IShellNode*> vecShellNode = pShellRootNode->iGetShellNodes();
			for (auto pShell : vecShellNode)
			{
				ShellNodeParam tParam;
				pShell->iGetShellParam(tParam);
				ShellNodeParam tGenerataParam = s_pThis->SetShellGenerateParam();
				tParam.vBaseColor = tGenerataParam.vBaseColor;
				tParam.vHoverColor = tGenerataParam.vHoverColor;
				tParam.vSelectColor = tGenerataParam.vSelectColor;
				tParam.bVisible = tParam.bClickEnable = tParam.bHoverEnable = true;
				pShell->iSetShellParam(tParam);
			}
			s_pThis->m_vecSplitBuilding.push_back(pNode);
		}
		//s_pThis->m_bDoingSplite = false;
		pPolygonSelector->iFinish();
		APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_SELECT;
	}
	if (s_pThis->m_bContinuousCutting)
	{
		s_pThis->m_pIsolatedTool->VerticalSingleCut(true);
	}
}

void YBSSModule::Static_CreateShellNodesSplitSingleHorizontal(INode* pSelector)
{
	IPolygonSelector* pPolygonSelector = pSelector->GetDynamicComponent<IPolygonSelector>();
	if (pPolygonSelector)
	{
		std::vector<Vector3> vecPoints;
		pPolygonSelector->iGetControlPoints(vecPoints);
		if (vecPoints.size() < 1)
		{
			//s_pThis->m_bDoingSplite = false;
			pPolygonSelector->iFinish();
			APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_SELECT;
			return;
		}

		INode* pNode = s_pThis->m_pCurIsolateNode;
		if (pNode && pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT))
		{
			IShellRootNode* pShellRootNode = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
			pShellRootNode->iSplitSingleHorizontal(vecPoints[0]);

			std::vector<IShellNode*> vecShellNode = pShellRootNode->iGetShellNodes();
			for (auto pShell : vecShellNode)
			{
				ShellNodeParam tParam;
				pShell->iGetShellParam(tParam);
				ShellNodeParam tGenerataParam = s_pThis->SetShellGenerateParam();
				tParam.vBaseColor = tGenerataParam.vBaseColor;
				tParam.vHoverColor = tGenerataParam.vHoverColor;
				tParam.vSelectColor = tGenerataParam.vSelectColor;
				tParam.bVisible = tParam.bClickEnable = tParam.bHoverEnable = true;
				pShell->iSetShellParam(tParam);
			}
			s_pThis->m_vecSplitBuilding.push_back(pNode);
		}
		//s_pThis->m_bDoingSplite = false;
		pPolygonSelector->iFinish();
		APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_SELECT;
	}
	if (s_pThis->m_bContinuousCutting)
	{
		s_pThis->m_pIsolatedTool->HorizontalCut(true);
	}
}

void YBSSModule::Static_CreateShellNodesSplitAllHorizontal(INode* pSelector)
{
	IPolygonSelector* pPolygonSelector = pSelector->GetDynamicComponent<IPolygonSelector>();
	if (pPolygonSelector)
	{
		std::vector<Vector3> vecPoints;
		pPolygonSelector->iGetControlPoints(vecPoints);
		if (vecPoints.size() < 1)
		{
			//s_pThis->m_bDoingSplite = false;
			pPolygonSelector->iFinish();
			APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_SELECT;
			return;
		}
		INode* pNode = s_pThis->m_pCurIsolateNode;
		if (pNode && pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT))
		{
			IShellRootNode* pShellRootNode = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
			pShellRootNode->iSplitAllHorizontal(vecPoints[0]);

			std::vector<IShellNode*> vecShellNode = pShellRootNode->iGetShellNodes();
			for (auto pShell : vecShellNode)
			{
				ShellNodeParam tParam;
				pShell->iGetShellParam(tParam);
				ShellNodeParam tGenerataParam = s_pThis->SetShellGenerateParam();
				tParam.vBaseColor = tGenerataParam.vBaseColor;
				tParam.vHoverColor = tGenerataParam.vHoverColor;
				tParam.vSelectColor = tGenerataParam.vSelectColor;
				tParam.bVisible = tParam.bClickEnable = tParam.bHoverEnable = true;
				pShell->iSetShellParam(tParam);
			}
			s_pThis->m_vecSplitBuilding.push_back(pNode);
		}
		//s_pThis->m_bDoingSplite = false;
		pPolygonSelector->iFinish();
		APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sRenderConfig.nMouseToolType = MouseToolType_e::MOUSE_TOOL_SELECT;
	}
	if (s_pThis->m_bContinuousCutting)
	{
		s_pThis->m_pIsolatedTool->HorizontalFullCut(true);
	}
}

void YBSSModule::SaveChange()
{
	std::vector<std::string> vecNodeId;
	std::map<std::string, std::string> mapTw4Path;
	//保存到指定路径
	//Data 目录
	PathConfig strPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig;
	std::string strShellPath = strPath.strProjectDir;
	//复制相关的文件
	//Dir
	std::string strDataPath = strPath.strModelsDir + "/shells/" + m_strAppID;
	Mkdirs(strDataPath.c_str());

	std::set<INode*> setSplit;
	for (auto it : m_vecSplitBuilding)
	{
		setSplit.insert(it);
	}

	for (auto pNode : setSplit)
	{
		if (pNode && pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT))
		{
			IShellRootNode* pShellRootNode = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
			pShellRootNode->iSaveDir(strShellPath);
			std::string strSrc = strShellPath + "/" + pNode->iGetID() + "_out.tw4";
			std::string strDst = strDataPath + "/" + pNode->iGetID() + ".tw4";
			mapTw4Path[strSrc] = strDst;

			std::vector<IShellNode*> vecShell = pShellRootNode->iGetShellNodes();
			for (auto pShell : vecShell)
			{
				if (pShell)
				{
					ShellNodeParam tParam = pShell->iGetShellParam();
					m_mapShellAndShellRootId[tParam.strID] = pNode->iGetID();
				}
			}
			if (std::find(vecNodeId.begin(), vecNodeId.end(), pNode->iGetID()) == vecNodeId.end())
			{
				vecNodeId.push_back(pNode->iGetID());
			}
		}
	}

	for (auto strTw4Path : mapTw4Path)
	{
		std::string strInFile = strTw4Path.first;
		std::string strOutFile = strTw4Path.second;
		YBSSModule::Copy(strInFile, strOutFile);
		if (IsFileExist(strInFile))
		{
			remove(strTw4Path.first.c_str());
		}
	}

	if (m_bAutoBackup)
	{
		saveBackup(vecNodeId);
	}

	m_vecSplitBuilding.clear();
}

void YBSSModule::CancelChange()
{
	for (auto pNode : m_vecSplitBuilding)
	{
		if (pNode && pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT))
		{
			IShellRootNode* pShellRootNode = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
			pShellRootNode->iCancel();
		}
	}
	for (auto pNode : m_vecNewBuilding)
	{
		if (pNode)
		{
			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&pNode);
		}
	}
	m_vecNewBuilding.clear();
	m_vecSplitBuilding.clear();
}

ShellNodeParam YBSSModule::SetShellGenerateParam()
{
	ShellNodeParam tParam;
	tParam.vBaseColor = Vector4(1.0, 0.0, 1.0, 0.5);
	tParam.vHoverColor = Vector4(1.0, 1.0, 0.0, 0.5);
	tParam.vSelectColor = Vector4(0.0, 1.0, 1.0, 0.5);
	return tParam;
}

void YBSSModule::doCheckBackup()
{
	try
	{
		std::string strShellPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strProjectDir + "/Shell";
		std::string strResourcePath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strResourceDir;
		std::string strProjectName = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strClientDLL;
		std::string strShellBackupPath = strResourcePath + "/shell_backup/" + strProjectName;
		std::string strDoneFile = strShellBackupPath + "/Done.json";
		Mkdirs(strShellBackupPath.c_str());
		if (IsFileExist(strDoneFile))
		{
			std::string strData = ReadFile(strDoneFile);
			Json::Value jRoot;
			Json::CharReaderBuilder jsonBuilder;
			std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
			std::string jsError;
			if (!reader->parse(strData.c_str(), strData.c_str() + strData.length(), &jRoot, &jsError))
			{
				jRoot = Json::nullValue;
			}

			if (!jRoot.isNull())
			{
				int nIndex = jRoot["Index"].asInt();
				bool bDone = jRoot["IsDone"].asBool();
				std::string strLastTime = jRoot["LastTime"].asString();
				long long lLastTime = atoll(strLastTime.c_str());
				std::chrono::seconds timeCur = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
				long long lTimeCur = timeCur.count();
				//七天
				if ((lTimeCur - lLastTime) > 604800LL)
					//if ((lTimeCur - lLastTime) > 30LL)
				{
					ThreadRequestParam stParam;
					stParam.eRequestType = THREAD_REQUEST_CUSTOM;
					stParam.pThreadRuningFunc = static_Backupping;
					stParam.pThreadCompleteFunc = static_Backupped;
					stParam.bRunCallbackInMainThread = true;
					stParam.bThread = true;
					stParam.AddParam("pThis", (INT_PTR)this);
					Json::Value jArg = Json::objectValue;
					jArg["ShellPath"] = strShellPath;
					jArg["ShellBackupPath"] = strShellBackupPath;
					jArg["SavePath"] = m_strSaveDirPath;
					jArg["DoneFilePath"] = strDoneFile;
					if (bDone)
					{
						nIndex = nIndex % 2 + 1;
						jArg["Index"] = nIndex;
					}
					else
					{
						jArg["Index"] = nIndex;
					}
					stParam.AddExtraJValue("jArg", jArg);

					std::ofstream out(strDoneFile, std::ios::trunc);
					Json::Value jRoot = Json::objectValue;
					jRoot["IsDone"] = false;
					jRoot["LastTime"] = std::to_string(timeCur.count());
					jRoot["Index"] = nIndex;
					out << jRoot.toStyledString();
					out.close();

					if (!m_strSaveDirPath.empty())
					{
						ThreadWrapper::GetInstance()->AddTask(stParam);
					}
				}
			}
		}
		else
		{
			int nIndex = 1;
			std::chrono::seconds timeCur = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
			std::ofstream out(strDoneFile, std::ios::trunc);
			Json::Value jRoot = Json::objectValue;
			jRoot["IsDone"] = false;
			jRoot["LastTime"] = std::to_string(timeCur.count());
			jRoot["Index"] = nIndex;
			out << jRoot.toStyledString();
			out.close();

			ThreadRequestParam stParam;
			stParam.eRequestType = THREAD_REQUEST_CUSTOM;
			stParam.pThreadRuningFunc = static_Backupping;
			stParam.pThreadCompleteFunc = static_Backupped;
			stParam.bRunCallbackInMainThread = true;
			stParam.bThread = true;
			stParam.AddParam("pThis", (INT_PTR)this);
			Json::Value jArg = Json::objectValue;
			jArg["ShellPath"] = strShellPath;
			jArg["ShellBackupPath"] = strShellBackupPath;
			jArg["SavePath"] = m_strSaveDirPath;
			jArg["DoneFilePath"] = strDoneFile;
			jArg["Index"] = nIndex;
			stParam.AddExtraJValue("jArg", jArg);
			if (!m_strSaveDirPath.empty())
			{
				ThreadWrapper::GetInstance()->AddTask(stParam);
			}
		}
	}
	catch (const std::exception&)
	{
	}
}

bool YBSSModule::static_Backupping(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam)
{
#ifdef _MSC_VER
	try
	{
		YBSSModule* pThis = (YBSSModule*)pParam->sRequestParam.GetParam("pThis");
		if (pThis)
		{
			if (!pThis->m_bBackupDone)
			{
				return false;
			}
			pThis->m_bBackupDone = false;
			Json::Value jArg = pParam->sRequestParam.GetExtraJValue("jArg");
			std::string strShellPath = jArg["ShellPath"].asString();
			std::string strSavePath = jArg["SavePath"].asString();
			std::string strShellBackupPath = jArg["ShellBackupPath"].asString();
			int nIndex = jArg["Index"].asInt();
			std::string strBack = strShellBackupPath + "/Back" + std::to_string(nIndex);
			std::string strBackTotal = strShellBackupPath + "/Back";
			if (IsDir(strBack))
			{
				std::function<void(std::string)> deleteFiles = [&deleteFiles](std::string strPath)
				{
					struct _finddata_t fileinfo;
					std::string strFind = strPath + "/*.*";
					intptr_t handle = _findfirst(strFind.c_str(), &fileinfo);
					do
					{
						std::string strFileName(fileinfo.name);
						if (strFileName == "." || strFileName == "..")
						{
						}
						else if (fileinfo.attrib & _A_SUBDIR)
						{
							deleteFiles(strPath + "/" + fileinfo.name);
						}
						else
						{
							std::string strFile = strPath + "/" + fileinfo.name;
							remove(strFile.c_str());
						}
					} while (_findnext(handle, &fileinfo) == 0);
					int nResult = _rmdir(strPath.c_str());
				};
				deleteFiles(strBack);
			}
			std::string strBackShellMSH = strBack + "/Shell/MSH";
			std::string strBackShellTW4 = strBack + "/Shell/TW4";
			std::string strBackShellTEXTURES = strBack + "/Shell/TEXTURES";
			std::string strBackSavePath = strBack + "/shells";
			Mkdirs(strBackShellMSH.c_str());
			Mkdirs(strBackShellTW4.c_str());
			Mkdirs(strBackShellTEXTURES.c_str());
			Mkdirs(strBackSavePath.c_str());

			std::string strBackTotalShellMSH = strBackTotal + "/Shell/MSH";
			std::string strBackTotalShellTW4 = strBackTotal + "/Shell/TW4";
			std::string strBackTotalShellTEXTURES = strBackTotal + "/Shell/TEXTURES";
			std::string strBackTotalSavePath = strBackTotal + "/shells";
			Mkdirs(strBackTotalShellMSH.c_str());
			Mkdirs(strBackTotalShellTW4.c_str());
			Mkdirs(strBackTotalShellTEXTURES.c_str());
			Mkdirs(strBackTotalSavePath.c_str());

			std::string strShellMSH = strShellPath + "/MSH";
			std::string strShellTW4 = strShellPath + "/TW4";
			std::string strShellTEXTURES = strShellPath + "/TEXTURES";

			std::vector<FileInfo> vecMshFiles;
			GetFilesFromDir(strShellMSH, vecMshFiles, false, "msh");
			std::vector<FileInfo> vecTw4Files;
			GetFilesFromDir(strShellTW4, vecTw4Files, false, "tw4");
			std::vector<FileInfo> vecTexturesFiles;
			GetFilesFromDir(strShellTEXTURES, vecTexturesFiles, false, "jpg");
			std::vector<FileInfo> vecSaveTw4Files;
			GetFilesFromDir(strSavePath, vecSaveTw4Files, false, "tw4");

			for (auto fileinfo : vecTw4Files)
			{
				std::string strInFile = fileinfo.strAbsoluteDir + "/" + fileinfo.tInfo.name;
				std::string strOutFile = strBackShellTW4 + "/" + fileinfo.tInfo.name;
				std::string strOutTotalFile = strBackTotalShellTW4 + "/" + fileinfo.tInfo.name;
				YBSSModule::Copy(strInFile, strOutFile);
				YBSSModule::Copy(strInFile, strOutTotalFile, true);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			for (auto fileinfo : vecMshFiles)
			{
				std::string strInFile = fileinfo.strAbsoluteDir + "/" + fileinfo.tInfo.name;
				std::string strOutFile = strBackShellMSH + "/" + fileinfo.tInfo.name;
				std::string strOutTotalFile = strBackTotalShellMSH + "/" + fileinfo.tInfo.name;
				YBSSModule::Copy(strInFile, strOutFile);
				YBSSModule::Copy(strInFile, strOutTotalFile);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			for (auto fileinfo : vecTexturesFiles)
			{
				std::string strInFile = fileinfo.strAbsoluteDir + "/" + fileinfo.tInfo.name;
				std::string strOutFile = strBackShellTEXTURES + "/" + fileinfo.tInfo.name;
				std::string strOutTotalFile = strBackTotalShellTEXTURES + "/" + fileinfo.tInfo.name;
				YBSSModule::Copy(strInFile, strOutFile);
				YBSSModule::Copy(strInFile, strOutTotalFile);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			for (auto fileinfo : vecSaveTw4Files)
			{
				std::string strInFile = fileinfo.strAbsoluteDir + "/" + fileinfo.tInfo.name;
				std::string strOutFile = strBackSavePath + "/" + fileinfo.tInfo.name;
				std::string strOutTotalFile = strBackTotalSavePath + "/" + fileinfo.tInfo.name;
				YBSSModule::Copy(strInFile, strOutFile);
				YBSSModule::Copy(strInFile, strOutTotalFile, true);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			Json::Value jCopy = Json::objectValue;
			jCopy["IsCopy"] = true;
			pParam->sRequestParam.AddExtraJValue("jCopy", jCopy);
			pThis->m_bBackupDone = true;
		}
	}
	catch (const std::exception&)
	{
		return false;
	}
#else

#endif

	return true;
}

void YBSSModule::static_Backupped(ThreadCallbackParam* pParam)
{
	YBSSModule* pThis = (YBSSModule*)pParam->sRequestParam.GetParam("pThis");
	if (pThis)
	{
		Json::Value jCopy = pParam->sRequestParam.GetExtraJValue("jCopy");
		if (!jCopy.isNull() && jCopy.isMember("IsCopy"))
		{
			bool bCopy = jCopy["IsCopy"].asBool();
			if (bCopy)
			{
				Json::Value jArg = pParam->sRequestParam.GetExtraJValue("jArg");
				std::string strDoneFile = jArg["DoneFilePath"].asString();
				int nIndex = jArg["Index"].asInt();
				std::chrono::seconds timeCur = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
				struct tm ptrTime;
				std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				localtime_s(&ptrTime, &time);
				std::string strCurDate = std::to_string(ptrTime.tm_year + 1900) + "_" +
					(ptrTime.tm_mon + 1 > 9 ? std::to_string(ptrTime.tm_mon + 1) : "0" + std::to_string(ptrTime.tm_mon + 1)) + "_" +
					(ptrTime.tm_mday > 9 ? std::to_string(ptrTime.tm_mday) : "0" + std::to_string(ptrTime.tm_mday)) + "_" +
					(ptrTime.tm_hour > 9 ? std::to_string(ptrTime.tm_hour) : "0" + std::to_string(ptrTime.tm_hour)) + "_" +
					(ptrTime.tm_min > 9 ? std::to_string(ptrTime.tm_min) : "0" + std::to_string(ptrTime.tm_min)) + "_" +
					(ptrTime.tm_sec > 9 ? std::to_string(ptrTime.tm_sec) : "0" + std::to_string(ptrTime.tm_sec));
				std::ofstream out(strDoneFile, std::ios::trunc);
				Json::Value jRoot = Json::objectValue;
				jRoot["IsDone"] = true;
				jRoot["LastTime"] = std::to_string(timeCur.count());
				jRoot["LastDateTime"] = strCurDate;
				jRoot["Index"] = nIndex;
				out << jRoot.toStyledString();
				out.close();
			}
		}
	}
}

bool YBSSModule::Copy(const std::string& src, const std::string& dst, bool bCover /*= false*/)
{
	if (!bCover)
	{
		if (IsFileExist(dst))
		{
			return true;
		}
	}
	if (IsFileExist(src))
	{
		std::ifstream inFile;
		std::ofstream outFile;
		inFile.open(src.c_str(), std::ios::binary);
		if (inFile.is_open())
		{
			outFile.open(dst.c_str(), std::ios::binary | std::ios::trunc);
			if (outFile.is_open())
			{
				std::istreambuf_iterator<char> begin_source(inFile);
				std::istreambuf_iterator<char> end_source;
				std::ostreambuf_iterator<char> begin_dest(outFile);
				std::copy(begin_source, end_source, begin_dest);
				inFile.close();
				outFile.close();
				return true;
			}
		}
	}
	return false;
}

void YBSSModule::saveBackup(const std::vector<std::string>& vecNodeId)
{
	if (!vecNodeId.empty() && !m_strSaveDirPath.empty())
	{
		std::string strResourcePath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strResourceDir;
		std::string strProjectName = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strClientDLL;
		std::string strShellBackupPath = strResourcePath + "/shell_backup/" + strProjectName;
		std::string strShellTw4Path = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strProjectDir + "/Shell/TW4";
		ThreadRequestParam stParam;
		stParam.eRequestType = THREAD_REQUEST_CUSTOM;
		stParam.pThreadRuningFunc = static_BackuppingTw4;
		stParam.bRunCallbackInMainThread = false;
		stParam.bThread = true;
		stParam.AddParam("pThis", (INT_PTR)this);
		Json::Value jArg = Json::objectValue;
		jArg["ShellTw4Path"] = strShellTw4Path;
		jArg["SavePath"] = m_strSaveDirPath;
		Json::Value jArray = Json::arrayValue;
		for (int i = 0; i < vecNodeId.size(); i++)
		{
			jArray[i] = vecNodeId[i];
		}
		jArg["NodeId"] = jArray;
		jArg["ShellBackupPath"] = strShellBackupPath;
		stParam.AddExtraJValue("jArg", jArg);
		ThreadWrapper::GetInstance()->AddTask(stParam);
	}
}

bool YBSSModule::static_BackuppingTw4(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam)
{
	try
	{
		struct tm ptrTime;
		std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		localtime_s(&ptrTime, &time);
		std::string strCurDate = std::to_string(ptrTime.tm_year + 1900) + "_" +
			(ptrTime.tm_mon + 1 > 9 ? std::to_string(ptrTime.tm_mon + 1) : "0" + std::to_string(ptrTime.tm_mon + 1)) + "_" +
			(ptrTime.tm_mday > 9 ? std::to_string(ptrTime.tm_mday) : "0" + std::to_string(ptrTime.tm_mday));
		Json::Value jArg = pParam->sRequestParam.GetExtraJValue("jArg");
		std::string strShellTw4Path = jArg["ShellTw4Path"].asString();
		std::string strSavePath = jArg["SavePath"].asString();
		Json::Value jArray = jArg["NodeId"];
		std::string strShellBackupPath = jArg["ShellBackupPath"].asString();
		std::string strBackTotal = strShellBackupPath + "/Back";
		std::string strBackOperatorShellTw4 = strShellBackupPath + "/BackOperator/Modified/Shell/TW4/" + strCurDate;
		std::string strBackOperatorSaveTw4 = strShellBackupPath + "/BackOperator/Modified/shells/Tw4/" + strCurDate;
		Mkdirs(strBackOperatorShellTw4.c_str());
		Mkdirs(strBackOperatorSaveTw4.c_str());
		if (!jArray.isNull() && jArray.isArray())
		{
			for (int i = 0; i < jArray.size(); i++)
			{
				std::string strShellRootNodeId = jArray[i].asString();
				std::string strShellFile = strShellTw4Path + "/" + strShellRootNodeId + ".tw4";
				std::string strSaveFile = strSavePath + "/" + strShellRootNodeId + ".tw4";
				time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				localtime_s(&ptrTime, &time);
				std::string strCurTime = std::to_string(ptrTime.tm_year + 1900) + "_" +
					(ptrTime.tm_mon + 1 > 9 ? std::to_string(ptrTime.tm_mon + 1) : "0" + std::to_string(ptrTime.tm_mon + 1)) + "_" +
					(ptrTime.tm_mday > 9 ? std::to_string(ptrTime.tm_mday) : "0" + std::to_string(ptrTime.tm_mday)) + "_" +
					(ptrTime.tm_hour > 9 ? std::to_string(ptrTime.tm_hour) : "0" + std::to_string(ptrTime.tm_hour)) + "_" +
					(ptrTime.tm_min > 9 ? std::to_string(ptrTime.tm_min) : "0" + std::to_string(ptrTime.tm_min));
				if (IsFileExist(strShellFile))
				{
					YBSSModule::Copy(strShellFile, strBackOperatorShellTw4 + "/" + strCurTime + "_" + strShellRootNodeId + ".tw4");
				}
				if (IsFileExist(strSaveFile))
				{
					YBSSModule::Copy(strSaveFile, strBackOperatorSaveTw4 + "/" + strCurTime + "_" + strShellRootNodeId + ".tw4");					BCXmlFile xFile;
					XMLDoc xmlDoc;
					if (xFile.Load(strSaveFile.c_str(), true))
					{
						char* pXml = xmlDoc.allocate_string(0, xFile.GetBufferLength());
						//size_t nLen = xFile.ToASCII(pXml);
						size_t nLen = xFile.GetUtf8Buff(pXml, xFile.GetBufferLength());
						xmlDoc.parse<0>(pXml);
						XMLNodePtr pScene = xmlDoc.first_node("SCENE");
						XMLNodePtr pRoot = pScene->first_node("ROOT");
						XMLNodePtr pNode = pRoot->first_node("NODE");
						std::string strShellPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strProjectDir;
						std::string strScene = pNode->first_node("SUB_SCENE")->value();
						std::string strMsh = pNode->first_node("MSH_NAME")->value();
						std::string strMshFile = strShellPath + "/Shell/MSH/" + strMsh;
						if (IsFileExist(strMshFile))
						{
							YBSSModule::Copy(strMshFile, strBackTotal + "/Shell/MSH/" + strMsh);
						}

						std::string strTexture = pNode->first_node("TEXTURE_NAME")->value();
						std::string strTextureFile = strShellPath + "/Shell/TEXTURES/" + strTexture;
						if (IsFileExist(strTextureFile))
						{
							YBSSModule::Copy(strMshFile, strBackTotal + "/Shell/TEXTURES/" + strTexture);
						}
					}
				}
			}
		}
	}
	catch (const std::exception&)
	{
	}
	return true;
}

void YBSSModule::loadShellRootNode()
{
	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_CUSTOM;
	stParam.AddParam("this", (INT_PTR)this);
	stParam.pThreadCompleteFunc = nullptr;
	stParam.pThreadRuningFunc = static_load_shellnode;
	stParam.strID = "loadShellRootNode";
	stParam.bRepeat = false;
	stParam.bThread = true;
	stParam.bRunCallbackInMainThread = false;
	ThreadWrapper::GetInstance()->AddTask(stParam);
}

bool YBSSModule::static_load_shellnode(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam)
{
	try
	{
		YBSSModule* pThis = (YBSSModule*)pParam->sRequestParam.GetParam("this");
		if (pThis)
		{
			while (!pThis->m_queueLoadXmlFile.empty())
			{
				std::string strPath = pThis->m_queueLoadXmlFile.front();
				if (IsFileExist(strPath))
				{
					BCXmlFile xFile;
					XMLDoc xmlDoc;
					if (xFile.Load(strPath.c_str(), true))
					{
						char* pXml = xmlDoc.allocate_string(0, xFile.GetBufferLength());
						memset(pXml, 0, xFile.GetBufferLength());
						size_t nLen = xFile.GetUtf8Buff(pXml, xFile.GetBufferLength());
						try
						{
							NodeParamBase stParam;
							stParam.nNodeType = NodeType_e::NODE_SHELL_GROUP;
							INode* pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(stParam);
							pNode->iSetExtraINTPTR("this", (INT_PTR)pThis);
							NodeLoad* pCallback = new NodeLoad;
							pNode->iSetLoadingCallback(pCallback);
							pNode->iSetFlagName(strPath);

							xmlDoc.parse<0>(pXml);
							XMLNodePtr pScene = xmlDoc.first_node("SCENE");
							XMLNodePtr pRoot = pScene->first_node("ROOT");
							XMLNodePtr pFirstNode = pRoot->first_node("NODE");
							IShellRootNode* pIShellRootNode = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
							if (pIShellRootNode)
							{
								pIShellRootNode->iLoad(pFirstNode);
								pNode->iLoadModel();
								pThis->m_pModuleGroupNode->iAddChild(pNode);
								pNode->GetDynamicComponent<IRenderNode>()->iSetClickEnable(true);
								pNode->GetDynamicComponent<IRenderNode>()->iSetHoverEnable(true);
								pNode->GetDynamicComponent<IRenderNode>()->iSetSelectEnable(true);
								std::vector<IShellNode*> vecShellNode = pIShellRootNode->iGetShellNodes();
								for (size_t j = 0; j < vecShellNode.size(); ++j)
								{
									std::string strFlagName = pNode->iGetID();
									vecShellNode[j]->iSetFlagName(strFlagName);
									vecShellNode[j]->iSetExternParam((INT_PTR)pThis->iCreateYBSSInfo());
									const ShellNodeParam& tParam = vecShellNode[j]->iGetShellParam();
									pThis->m_mapShellAndShellRootId[tParam.strID] = strFlagName;
								}
							}
						}
						catch (rapidxml::parse_error&)
						{
						}
					}
				}
				pThis->refreshAllShell();
				pThis->m_queueLoadXmlFile.pop();
			}
			if (pThis->m_queueLoadXmlFile.empty()) 
			{
				pThis->iOnAppModeChanged(pThis->m_pClient->m_nAppMode);
				pThis->iOnXmlLoaded();
				ProjectLogger::GetInstance()->InfoMessage(std::string("All ShellNode count=") + to_string(pThis->m_nLoadedCount));
			}
			
			//if (pThis->m_queueLoadXmlFile.empty())
			//{
			//	//进行绑定请求
			//	std::vector<IShellNode*> vecAllShell;
			//	if (pThis->m_pModuleGroupNode)
			//	{
			//		std::vector<INode*> vec;
			//		pThis->m_pModuleGroupNode->iGetAllChildren(vec);
			//		for (size_t i = 0; i < vec.size(); ++i)
			//		{
			//			IShellRootNode* pShellRoot = (IShellRootNode*)vec[i]->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
			//			if (pShellRoot)
			//			{
			//				std::vector<IShellNode*> vecShell = pShellRoot->iGetShellNodes();
			//				vecAllShell.insert(vecAllShell.end(), vecShell.begin(), vecShell.end());
			//			}
			//		}
			//	}
			//	std::stack<IShellNode*> stackTemp1;
			//	std::swap(pThis->m_stackRequestShellNode, stackTemp1);
			//	for (size_t i = 0; i < vecAllShell.size(); ++i)
			//	{
			//		pThis->iSetShellAttr(vecAllShell[i]);
			//		YBSSInfo* pInfo = (YBSSInfo*)vecAllShell[i]->iGetExternParam();
			//		if (pInfo->bRequested)
			//		{
			//			//已经请求到了,不在请求
			//			continue;
			//		}
			//		pThis->m_stackRequestShellNode.push(vecAllShell[i]);
			//	}
			//	pThis->iOnAppModeChanged(pThis->m_pClient->m_nAppMode);
			//	pThis->iOnXmlLoaded();
			//	ProjectLogger::GetInstance()->InfoMessage(std::string("All ShellNode count=") + to_string(vecAllShell.size()));
			//	ProjectLogger::GetInstance()->InfoMessage(std::string("Request ShellNode count=") + to_string(pThis->m_stackRequestShellNode.size()));
			//}
		}
	}
	catch (const std::exception&)
	{
		return false;
	}
	return true;
}

void YBSSModule::NodeLoad::Loading(INode* pNode, float progress)
{
}

void YBSSModule::NodeLoad::Loaded(INode* pNode)
{
	YBSSModule* pModule = (YBSSModule*)pNode->iGetExtraINTPTR("this");
	if (pModule)
	{
		pModule->m_nLoadedCount++;
		//进行绑定请求
		std::vector<IShellNode*> vecAllShell;
		std::vector<INode*> vec;
		IShellRootNode* pShellRoot = (IShellRootNode*)pNode->iGetComponent(COMPONENT_TYPE_SHELL_ROOT);
		if (pShellRoot)
		{
			std::vector<IShellNode*> vecShell = pShellRoot->iGetShellNodes();
			vecAllShell.insert(vecAllShell.end(), vecShell.begin(), vecShell.end());
		}
		for (size_t i = 0; i < vecAllShell.size(); ++i)
		{
			pModule->iSetShellAttr(vecAllShell[i]);
			YBSSInfo* pInfo = (YBSSInfo*)vecAllShell[i]->iGetExternParam();
			if (pInfo->bRequested)
			{
				//已经请求到了,不在请求
				continue;
			}
			pModule->m_stackRequestShellNode.push(vecAllShell[i]);
		}

		if (pModule->m_nLoadedCount == pModule->m_nNeedLoadCount)
		{
			pModule->m_nLoadedCount = 0; 
		}
	}
}

void YBSSModule::NodeLoad::Preloading(INode* pNode)
{
}
