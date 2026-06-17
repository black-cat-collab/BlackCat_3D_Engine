#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/utils/ParamNetImage.h"

#ifdef _MSC_VER
#include <direct.h>
#include <io.h>
#endif

#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace bc;

#define PIC_DIR std::string("resource/boardpic/")
#define SERIAL_NORMAL std::string("normal")
#define SERIAL_HOVER std::string("hover")
#define SERIAL_CHECK std::string("check")
#define SERIAL_BASE std::string("base")
#define SERIAL_FENCE std::string("fence")
#define SERIAL_HOVER_FENCE std::string("hover_fence")
#define SERIAL_MULTI_FLYLINE_FIRST std::string("multi_flyline_first")
#define SERIAL_MULTI_FLYLINE_SECOND std::string("multi_flyline_second")
#define MODEL_DIR std::string("resource/apimodel/")

ParamNetImage* ParamNetImage::m_Instance = NULL;
ParamNetImage::GC ParamNetImage::gc;

ParamNetImage::ParamNetImage()
{
	bAddApplicationDir = true;
	//#ifndef __GNUC__
	//	//每次启动删除 下载的标签图片 文件 以及文件夹
	//	std::function<void(std::string)> deleteFiles = [&deleteFiles](std::string strPath)
	//	{
	//		if (!IsDir(strPath))
	//		{
	//			return;
	//		}
	//		struct _finddata_t fileinfo;
	//		std::string strFind = strPath + "/*.*";
	//		intptr_t handle = _findfirst(strFind.c_str(), &fileinfo);
	//		do
	//		{
	//			std::string strFileName(fileinfo.name);
	//			if (strFileName == "." || strFileName == "..")
	//			{
	//			}
	//			else if (fileinfo.attrib & _A_SUBDIR)
	//			{
	//				deleteFiles(strPath + "/" + fileinfo.name);
	//			}
	//			else
	//			{
	//				std::string strFile = strPath + "/" + fileinfo.name;
	//				remove(strFile.c_str());
	//			}
	//		} while (_findnext(handle, &fileinfo) == 0);
	//		int nResult = rmdir(strPath.c_str());
	//	};
	//	std::string strDataPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir;
	//	std::string strClientName = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strClientDLL;
	//	std::string strDeletePath = strDataPath + "/" + PIC_DIR + strClientName;
	//#ifndef __GNUC__
	//	std::string strResult = GetProjectExecuteName();
	//	strDeletePath = strDeletePath + "/" + strResult;
	//#else
	//	std::string strDeletePath = strDataPath + "/" + PIC_DIR + strClientName;
	//#endif
	//
	//	deleteFiles(strDeletePath);
	//#endif
}

ParamNetImage::~ParamNetImage()
{
	Clear();
}

void ParamNetImage::Clear()
{
	for (std::map<std::string, std::vector<NetParam*>>::iterator it = m_mapNetParam.begin(); it != m_mapNetParam.end(); it++)
	{
		for (int i = 0; i < it->second.size(); i++)
		{
			delete it->second[i];
			it->second[i] = nullptr;
		}
		it->second.resize(0);
	}
	m_mapNetParam.clear();
	m_vecAllTask.clear();
	m_vecRetryPicData.clear();
}

ParamNetImage* ParamNetImage::GetInstance()
{
	if (m_Instance == NULL)
	{
		m_Instance = new ParamNetImage();
	}
	return m_Instance;
}

void ParamNetImage::FrameUpdate()
{
	for (std::vector<TaskData>::iterator it = m_vecAllTask.begin(); it != m_vecAllTask.end();)
	{
		if (it->pNode && it->pNode->iIsLoad())
		{
			doNetTask(it->pNode, it->strServerIp);
			it = m_vecAllTask.erase(it);
		}
		else
		{
			++it;
		}
	}
	for (std::vector<TaskData>::iterator it = m_vecModelTask.begin(); it != m_vecModelTask.end();)
	{
		if (it->pNode)
		{
			doModelTask(it->pNode, it->strServerIp);
			it = m_vecModelTask.erase(it);
		}
		else
		{
			++it;
		}
	}
	//重复的
	for (std::vector<ReTryPicData>::iterator it = m_vecRetryPicData.begin(); it != m_vecRetryPicData.end();)
	{
		//zip 创建的
		if (it->isCreateByZip)
		{
			if (IsFileExist(it->strScriptPath))
			{
				setMaterial(it->pNode, it->strPicPath, it->strSerail);
				it = m_vecRetryPicData.erase(it);
			}
			else
			{
				++it;
			}
		}
		else
		{
			if (IsFileExist(it->strFullPicPath))
			{
				setMaterial(it->pNode, it->strPicPath, it->strSerail);
				it = m_vecRetryPicData.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

void ParamNetImage::AddNetTask(INode* pNode, std::string strPicServerIp)
{
	if (!pNode)
	{
		return;
	}
	for (std::vector<TaskData>::iterator it = m_vecAllTask.begin(); it != m_vecAllTask.end(); ++it)
	{
		if (it->pNode == pNode)
		{
			return;
		}
	}
	TaskData sData;
	sData.pNode = pNode;
	sData.strServerIp = strPicServerIp;
	m_vecAllTask.push_back(sData);
}

void ParamNetImage::RemoveNetTask(INode* pNode)
{
	for (std::vector<TaskData>::iterator it = m_vecAllTask.begin(); it != m_vecAllTask.end(); )
	{
		if (it->pNode == pNode)
		{
			m_vecAllTask.erase(it);
		}
		else
		{
			++it;
		}
	}
	for (std::vector<TaskData>::iterator it = m_vecModelTask.begin(); it != m_vecModelTask.end();)
	{
		if (it->pNode == pNode)
		{
			m_vecModelTask.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void ParamNetImage::doNetTask(INode* pNode, std::string strPicServerIp)
{
	handleNode(pNode, strPicServerIp);
	IBoard* pIBoard = dynamic_cast<IBoard*>(pNode->iGetComponent(COMPONENT_TYPE_BOARD));
	if (pIBoard)
	{
		if (pNode->iGetNodeType() == NODE_FONTBOARD_WITH_BUTTON)
		{
			IBoard* pConvertNode = pNode->GetDynamicComponent<IBoard>();
			std::vector<INode*> vecNode;
			pConvertNode->iGetButtonBoard(vecNode);
			for (size_t i = 0; i < vecNode.size(); i++)
			{
				handleNode(vecNode[i], strPicServerIp);
			}
		}
		else if (pNode->iGetNodeType() == NODE_BOARD_WITH_FONTBOARD)
		{
			INode* pFontBoard = pIBoard->iGetFontBoard();
			if (pFontBoard)
			{
				handleNode(pFontBoard, strPicServerIp);
			}
			pFontBoard = pIBoard->iGetDetailFontBoard();
			if (pFontBoard)
			{
				handleNode(pFontBoard, strPicServerIp);
			}
		}
		else if (pNode->iGetNodeType() == NODE_FONTBOARD_WITH_BUTTON)
		{
			IBoard* pConvertNode = pNode->GetDynamicComponent<IBoard>();
			std::vector<INode*> vecNode;
			pConvertNode->iGetButtonBoard(vecNode);
			for (size_t i = 0; i < vecNode.size(); i++)
			{
				handleNode(vecNode[i], strPicServerIp);
			}
		}
		else if (pNode->iGetNodeType() == NODE_VIDEO_BOARD_WITH_BUTTON)
		{
			IBoard* pConvertNode = pNode->GetDynamicComponent<IBoard>();
			std::vector<INode*> vecNode;
			pConvertNode->iGetButtonBoard(vecNode);
			for (size_t i = 0; i < vecNode.size(); i++)
			{
				handleNode(vecNode[i], strPicServerIp);
			}

			std::vector<INode*> vecVideoNode;
			pConvertNode->iGetVideoBoard(vecVideoNode);
			for (size_t i = 0; i < vecVideoNode.size(); i++)
			{
				handleNode(vecVideoNode[i], strPicServerIp);
			}
		}
	}
}

void ParamNetImage::handleNode(INode* pNode, std::string strPicServerIp)
{
	if (!pNode)
	{
		return;
	}
	IBoard* pIBoard = dynamic_cast<IBoard*>(pNode->iGetComponent(COMPONENT_TYPE_BOARD));
	if (pIBoard)
	{
		BoardParam* pParam = pIBoard->iGetBoardParam();
		handleTask(pNode, pParam->nNodeType, pParam->strImage, SERIAL_NORMAL, strPicServerIp);
		handleTask(pNode, pParam->nNodeType, pParam->strHoverImage, SERIAL_HOVER, strPicServerIp);
		handleTask(pNode, pParam->nNodeType, pParam->strCheckedImage, SERIAL_CHECK, strPicServerIp);
		if (pIBoard->iGetBaseBoard())
		{
			handleTask(pNode, pParam->nNodeType, pParam->tBaseParam.strImage, SERIAL_BASE, strPicServerIp);
		}
	}

	if (pNode->iGetNodeType() == NODE_BOARD_INSTANCE)
	{
		INode* pBoardInstanced = pNode;
		BoardInstanceParam* pParam = (BoardInstanceParam*)pBoardInstanced->iGetNodeParam();
		if (pParam)
		{
			handleTask(pBoardInstanced, pParam->nNodeType, pParam->strImage, SERIAL_NORMAL, strPicServerIp);
			handleTask(pBoardInstanced, pParam->nNodeType, pParam->strHoverImage, SERIAL_HOVER, strPicServerIp);
			handleTask(pBoardInstanced, pParam->nNodeType, pParam->strCheckedImage, SERIAL_CHECK, strPicServerIp);
		}
	}

	if (pNode->iGetNodeType() == NODE_POLYGON)
	{
		INode* pPolygon = pNode;
		PolygonParam* pParam = (PolygonParam*)pPolygon->iGetNodeParam();
		if (pParam)
		{
			handleTask(pPolygon, pParam->nNodeType, pParam->strBaseImage, SERIAL_NORMAL, strPicServerIp);
			handleTask(pPolygon, pParam->nNodeType, pParam->strFenceImage, SERIAL_FENCE, strPicServerIp);
			handleTask(pPolygon, pParam->nNodeType, pParam->strHoverBaseImage, SERIAL_HOVER, strPicServerIp);
			handleTask(pPolygon, pParam->nNodeType, pParam->strHoverFenceImage, SERIAL_HOVER_FENCE, strPicServerIp);
		}
	}

	if (pNode->iGetNodeType() == NODE_ROAD)
	{
		INode* pRoad = pNode;
		RoadParam* pParam = (RoadParam*)pRoad->iGetNodeParam();
		if (pParam)
		{
			handleTask(pRoad, pParam->nNodeType, pParam->strImage, SERIAL_NORMAL, strPicServerIp);
		}
	}

	if (pNode->iGetNodeType() == NODE_FLYLINE)
	{
		INode* pFlyNode = pNode;
		FlyLineParam* pParam = (FlyLineParam*)pFlyNode->iGetNodeParam();
		if (pParam)
		{
			handleTask(pFlyNode, pParam->nNodeType, pParam->strMaterial, SERIAL_NORMAL, strPicServerIp);
		}
	}

	if (pNode->iGetNodeType() == NODE_MULTI_FLYLINE)
	{
		INode* pFlyNode = pNode;
		MultiFlyLineParam* pParam = (MultiFlyLineParam*)pFlyNode->iGetNodeParam();
		if (pParam)
		{
			handleTask(pFlyNode, pParam->nNodeType, pParam->strMaterialFirst, SERIAL_MULTI_FLYLINE_FIRST, strPicServerIp);
			handleTask(pFlyNode, pParam->nNodeType, pParam->strMaterialSecond, SERIAL_MULTI_FLYLINE_SECOND, strPicServerIp);
		}
	}
}

void ParamNetImage::handleTask(INode* pNode, int nNodeType, std::string strUrl, std::string strSerail, std::string strPicServerIp)
{
	if (!pNode || strUrl.empty())
	{
		return;
	}
	bool bNet = false;
	if (StartWith(strUrl, "http://") || StartWith(strUrl, "https://"))
	{
		bNet = true;
	}
	else
	{
		std::string strPath = strUrl;
		if (!IsFileExist(strPath))
		{
			strPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir + "/" + strUrl;
			if (!IsFileExist(strPath))
			{
				if (m_mapFileWithUrl.find(strUrl) != m_mapFileWithUrl.end())
				{
					//找到过,返回为网络图片
					strUrl = m_mapFileWithUrl[strUrl];
				}
				bNet = true;
			}
			//只针对压缩包做处理
			else
			{
				strPath = Replace_all(strPath, "\\", "/");
				if (strPath.substr(strPath.length() - 4) == ".zip")
				{
					int nStart = strPath.find_last_of("/");
					int nEnd = strPath.find_last_of(".");
					std::string strDataDir = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir;
					std::string strClientDLL = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strClientDLL;
					std::transform(strClientDLL.begin(), strClientDLL.end(),
						strClientDLL.begin(), [](unsigned char c)
						{
							return std::tolower(c);
						});
					std::string strFileName = strPath.substr(nStart + 1, nEnd - nStart - 1);
					std::string strMaterialName = strFileName + "_" + strClientDLL;
					std::string strScriptFile = strDataDir + "/" + PIC_DIR + strClientDLL
						+ "/" + strFileName + "/" + strFileName + ".script";
					bool bCanLoad = false;
					if (IsFileExist(strScriptFile))
					{
						bCanLoad = true;
					}
					else
					{
						bCanLoad = decompressAndCreateScript(strPath);
					}
					if (bCanLoad)
					{
						APIProvider::GetSystemAPI()->iRenderAPI->iLoadScriptFile(strScriptFile);
						ReTryPicData sData;
						sData.pNode = pNode;
						sData.isCreateByZip = true;
						sData.strPicPath = strMaterialName;//直接使用材质名称
						sData.strFullPicPath = strPath;
						sData.strScriptPath = strScriptFile;
						sData.strSerail = strSerail;
						m_vecRetryPicData.push_back(sData);
					}
				}
			}
		}
	}
	if (bNet)
	{
		if (!StartWith(strUrl, "http://") && !StartWith(strUrl, "https://"))
		{
			strUrl = strPicServerIp + "/" + strUrl;
		}
		std::string strDownPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir
			+ std::string("/") + PIC_DIR;
		if (bAddApplicationDir)
		{
			std::string strClientDLL = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strClientDLL;
			std::transform(strClientDLL.begin(), strClientDLL.end(),
				strClientDLL.begin(), [](unsigned char c)
				{
					return std::tolower(c);
				});
#ifndef __GNUC__
			std::string strResult = GetProjectExecuteName();
			strDownPath += strClientDLL + "/" + strResult + "/";
#else
			strDownPath += strClientDLL + "/";
#endif
	}
		MakeDirs(strDownPath.c_str());

		//是网络图片
		int nPos1 = strUrl.find_last_of("/");
		int nPos2 = strUrl.find_last_of(".");
		if (nPos2 == std::string::npos)
		{
			nPos2 = strUrl.length();
		}
		bool bIsImageFile = true;
		//判断是不是压缩包（zip）
		std::string strSuffix = strUrl.substr(nPos2 + 1);
		if (strSuffix == "zip")
		{
			bIsImageFile = false;
		}

		if (bIsImageFile)
		{
			//是网络图片
			std::string strFileName = strUrl.substr(nPos1 + 1, nPos2 - nPos1 - 1);
			if (strFileName.empty())
			{
				return;
			}
			strFileName += ".png";

			ThreadRequestParam stParam;
			stParam.pThreadCompleteFunc = Callback_DownLoad_Pic;
			stParam.AddParam("this", (INT_PTR)this);
			stParam.strUrl = strUrl;
			stParam.bRunCallbackInMainThread = true;

			//添加retryPicData中
			ReTryPicData sData;
			sData.pNode = pNode;
			std::string strResultPicPath = PIC_DIR;
			if (bAddApplicationDir)
			{
				std::string strClientDLL = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strClientDLL;
				std::transform(strClientDLL.begin(), strClientDLL.end(),
					strClientDLL.begin(), [](unsigned char c)
					{
						return std::tolower(c);
					});
#ifndef __GNUC__
				
				std::string strResult = GetProjectExecuteName();
				strResultPicPath += strClientDLL +"/" + strResult + "/";
#else
				strResultPicPath += strClientDLL + "/";
#endif
		}
			strResultPicPath += strFileName;
			sData.strPicPath = strResultPicPath;
			sData.strFullPicPath = strDownPath + strFileName;
			sData.strSerail = strSerail;
			m_vecRetryPicData.push_back(sData);

			//设置placeHolder
			{
				IBoard* pBoard = pNode->GetDynamicComponent<IBoard>();
				if (pBoard)
				{
					if (!pBoard->iGetBoardParam()->strPlaceHolderImage.empty())
					{
						pBoard->iSetBoardMaterial(pBoard->iGetBoardParam()->strPlaceHolderImage);
						pBoard->iSetHoverMaterial(pBoard->iGetBoardParam()->strPlaceHolderImage);
						pBoard->iSetCheckMaterial(pBoard->iGetBoardParam()->strPlaceHolderImage);
					}
				}
			}

			{
				INode* pBoard = pNode;
				if (pBoard->iGetNodeType() == NODE_BOARD_INSTANCE)
				{
					BoardInstanceParam* pBoardInstanceParam = (BoardInstanceParam*)pBoard->iGetNodeParam();
					if (!pBoardInstanceParam->strPlaceHolderImage.empty())
					{
						pBoard->GetDynamicComponent<IInstance>()->iSetMaterial(pBoardInstanceParam->strPlaceHolderImage);
					}
				}
			}

			bool bHasTask = checkHasNetTask(strUrl);
			if (bHasTask || IsFileExist(sData.strFullPicPath))
			{
				//存在下载任务了
				return;
			}

			NetParam* pNetParam = new NetParam;
			pNetParam->strFileName = strFileName;
			pNetParam->strUrl = strUrl;
			pNetParam->strDownLoadPath = strDownPath;
			pNetParam->strSerail = strSerail;
			pNetParam->nNodeType = nNodeType;
			pNetParam->pNode = pNode;
			std::string strFilePath = strDownPath + pNetParam->strFileName;

			std::vector<NetParam*> vec;
			m_mapNetParam[strUrl] = vec;

			stParam.eRequestType = THREAD_REQUEST_DOWNLOAD;
			stParam.bThread = true;

			Mkdirs(pNetParam->strDownLoadPath.c_str());
			stParam.AddParam("netParam", (INT_PTR)pNetParam);
			stParam.strDownloadPath = strDownPath + strFileName;

			ThreadWrapper::GetInstance()->AddTask(stParam);
}
		else
		{
			//是压缩包
			std::string strFileName = strUrl.substr(nPos1 + 1, nPos2 - nPos1 - 1);
			std::string strScriptFileName = strUrl.substr(nPos1 + 1, nPos2 - nPos1 - 1);
			std::string strClientDLL = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strClientDLL;
			std::transform(strClientDLL.begin(), strClientDLL.end(),
				strClientDLL.begin(), [](unsigned char c)
				{
					return std::tolower(c);
				});
			std::string strClient = strClientDLL;
			std::string strMaterialName = strFileName + "_" + strClient;
			if (strFileName.empty())
			{
				return;
			}
			strFileName += ".zip";
			strDownPath = Replace_all(strDownPath, "\\", "/");
			ThreadRequestParam stParam;
			stParam.pThreadCompleteFunc = static_download_effectPicture;
			stParam.AddParam("this", (INT_PTR)this);
			stParam.strUrl = strUrl;
			stParam.bRunCallbackInMainThread = true;
			//添加retryPicData中
			ReTryPicData sData;
			sData.pNode = pNode;
			std::string strResultPicPath = PIC_DIR;
			if (bAddApplicationDir)
			{
#ifndef __GNUC__

				std::string strResult = GetProjectExecuteName();
				strResultPicPath += strClient + "/" + strResult + "/";
#else
				strResultPicPath += strClient + "/";
#endif
			}
			strResultPicPath += strFileName;
			sData.isCreateByZip = true;
			sData.strPicPath = strMaterialName;//直接使用材质名称
			sData.strFullPicPath = strDownPath + strFileName;
			std::string strScriptFileDir = sData.strFullPicPath.substr(0, sData.strFullPicPath.length() - 4);
			sData.strScriptPath = strScriptFileDir + "\\" + strScriptFileName + ".script";
			sData.strSerail = strSerail;
			m_vecRetryPicData.push_back(sData);
			//设置placeHolder
			{
				IBoard* pBoard = pNode->GetDynamicComponent<IBoard>();
				if (pBoard)
				{
					if (!pBoard->iGetBoardParam()->strPlaceHolderImage.empty())
					{
						pBoard->iSetBoardMaterial(pBoard->iGetBoardParam()->strPlaceHolderImage);
						pBoard->iSetHoverMaterial(pBoard->iGetBoardParam()->strPlaceHolderImage);
						pBoard->iSetCheckMaterial(pBoard->iGetBoardParam()->strPlaceHolderImage);
					}
				}
			}
			{
				INode* pBoard = pNode;
				if (pBoard->iGetNodeType() == NODE_BOARD_INSTANCE)
				{
					BoardInstanceParam* pBoardInstanceParam = (BoardInstanceParam*)pBoard->iGetNodeParam();
					if (!pBoardInstanceParam->strPlaceHolderImage.empty())
					{
						pBoard->GetDynamicComponent<IInstance>()->iSetMaterial(pBoardInstanceParam->strPlaceHolderImage);
					}
				}
			}
			//是否存在下载任务了
			//是否存在文件夹 以及文件
			std::string strDir = sData.strFullPicPath.substr(0, sData.strFullPicPath.length() - 4);
			std::string strScriptFile = strDir + "\\" + strScriptFileName + ".script";
			bool bHasTask = checkHasNetTask(strUrl);
			bool bHasDirAndScript = IsDir(strDir) && IsFileExist(strScriptFile);
			if (bHasTask || bHasDirAndScript)
			{
				if (bHasDirAndScript)
				{
					APIProvider::GetSystemAPI()->iRenderAPI->iLoadScriptFile(strScriptFile);
				}
				return;
			}
			NetParam* pNetParam = new NetParam;
			pNetParam->strFileName = strFileName;
			pNetParam->strUrl = strUrl;
			pNetParam->strDownLoadPath = strDownPath;
			pNetParam->strSerail = strSerail;
			pNetParam->nNodeType = nNodeType;
			pNetParam->pNode = pNode;
			std::string strFilePath = strDownPath + pNetParam->strFileName;
			std::vector<NetParam*> vec;
			m_mapNetParam[strUrl] = vec;
			stParam.eRequestType = THREAD_REQUEST_DOWNLOAD;
			stParam.bThread = true;
			Mkdirs(pNetParam->strDownLoadPath.c_str());
			stParam.AddParam("netParam", (INT_PTR)pNetParam);
			stParam.strDownloadPath = strDownPath + strFileName;
			ThreadWrapper::GetInstance()->AddTask(stParam);
		}
	}
}

void ParamNetImage::Callback_DownLoad_Pic(ThreadCallbackParam_s* pParam)
{
	ParamNetImage* pThis = (ParamNetImage*)pParam->sRequestParam.mapParams["this"];
	NetParam* pNetParam = (NetParam*)pParam->sRequestParam.mapParams["netParam"];
	if (!pNetParam || !pThis)
	{
		return;
	}
	std::string strUrl = pNetParam->strUrl;
	std::string strPicPath = pNetParam->strDownLoadPath + pNetParam->strFileName;
	pThis->m_mapFileWithUrl[strPicPath] = strUrl;

	delete pNetParam;
	pNetParam = NULL;

	pThis->removeNetParam(strUrl);

	if (pParam->nResult != STATUS_SUCCESS)
	{
		//下载失败,则移除
		for (std::vector<ReTryPicData>::iterator it = pThis->m_vecRetryPicData.begin(); it != pThis->m_vecRetryPicData.end();)
		{
			if (it->strFullPicPath == strPicPath)
			{
				it = pThis->m_vecRetryPicData.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

void ParamNetImage::static_download_effectPicture(ThreadCallbackParam* pParam)
{
	ParamNetImage* pThis = (ParamNetImage*)pParam->sRequestParam.mapParams["this"];
	NetParam* pNetParam = (NetParam*)pParam->sRequestParam.mapParams["netParam"];
	if (!pNetParam || !pThis)
	{
		return;
	}
	std::string strMaterialName = pNetParam->strFileName.substr(0, pNetParam->strFileName.length() - 4);
	std::string strUrl = pNetParam->strUrl;
	std::string strPicPath = pNetParam->strDownLoadPath + pNetParam->strFileName;
	pThis->m_mapFileWithUrl[strPicPath] = strUrl;

	delete pNetParam;
	pNetParam = NULL;

	pThis->removeNetParam(strUrl);

	//下载成功 解压
	if (pParam->nResult == STATUS_SUCCESS)
	{
		bool bIsSuccess = pThis->decompressAndCreateScript(strPicPath, true);
		std::string strScriptFile = Replace_all(strPicPath, ".zip", "\\" + strMaterialName + ".script");
		if (bIsSuccess)
		{
			APIProvider::GetSystemAPI()->iRenderAPI->iLoadScriptFile(strScriptFile);
		}
	}
	if (pParam->nResult != STATUS_SUCCESS)
	{
		//下载失败,则移除
		for (std::vector<ReTryPicData>::iterator it = pThis->m_vecRetryPicData.begin(); it != pThis->m_vecRetryPicData.end();)
		{
			if (it->strFullPicPath == strPicPath)
			{
				it = pThis->m_vecRetryPicData.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}
void ParamNetImage::removeNetParam(std::string strUrl)
{
	for (std::map<std::string, std::vector<NetParam*>>::iterator it = m_mapNetParam.begin(); it != m_mapNetParam.end(); it++)
	{
		if (it->first == strUrl)
		{
			it->second.clear();
			it->second.resize(0);
			m_mapNetParam.erase(it);
			break;
		}
	}
}

bool ParamNetImage::checkHasNetTask(std::string strUrl)
{
	for (std::map<std::string, std::vector<NetParam*>>::iterator it = m_mapNetParam.begin(); it != m_mapNetParam.end(); it++)
	{
		if (it->first == strUrl)
		{
			return true;
		}
	}
	return false;
}

std::string ParamNetImage::CheckLocalPath(std::string& strOriUrl)
{
	std::string strUrl = strOriUrl;
	bool bNet = true;
	if (StartWith(strUrl, APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strResourceDirName) || IsFileExist(strUrl))
	{
		bNet = false;
	}
	if (bNet)
	{
		std::string strDownPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir + std::string("/") + PIC_DIR;
		if (bAddApplicationDir)
		{
			std::string strClientDLL = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strClientDLL;
			std::transform(strClientDLL.begin(), strClientDLL.end(),
				strClientDLL.begin(), [](unsigned char c)
				{
					return std::tolower(c);
				});
			strDownPath += strClientDLL + "/";
		}

		//是网络图片
		int nPos1 = strUrl.find_last_of("/");
		int nPos2 = strUrl.find_last_of(".");
		if (nPos2 == std::string::npos)
		{
			nPos2 = strUrl.length();
		}
		std::string strFileName = strUrl.substr(nPos1 + 1, nPos2 - nPos1 - 1);
		if (strFileName.empty())
		{
			return strOriUrl;
		}
		strFileName += ".png";
		strDownPath = Replace_all(strDownPath, "/", "\\");

		std::string strFilePath = strDownPath + strFileName;
		bool bExistFile = IsFileExist(strFilePath);
		if (bExistFile)
		{
			return strFilePath;
		}
	}
	return strOriUrl;
}

void ParamNetImage::setMaterial(INode* pNode, std::string strPicPath, std::string strSerial)
{
	{
		IBoard* pBoard = GetComponent<IBoard>(pNode);
		if (pBoard)
		{
			if (strSerial == SERIAL_NORMAL)
			{
				pBoard->iSetBoardMaterial(strPicPath);
			}
			else if (strSerial == SERIAL_HOVER)
			{
				pBoard->iSetHoverMaterial(strPicPath);
			}
			else if (strSerial == SERIAL_CHECK)
			{
				pBoard->iSetCheckMaterial(strPicPath);
			}
			else if (strSerial == SERIAL_BASE)
			{
				if (pBoard->iGetBaseBoard())
				{
					pBoard->iGetBaseBoard()->GetDynamicComponent<IBoard>()->iSetBoardMaterial(strPicPath);
				}
			}
		}
	}

	{
		if (pNode->iGetNodeType() == NODE_BOARD_INSTANCE)
		{
			IInstance* pBoardInstanced = pNode->GetDynamicComponent<IInstance>();
			if (strSerial == SERIAL_NORMAL)
			{
				pBoardInstanced->iSetMaterial(strPicPath);
			}
		}

		if (pNode->iGetNodeType() == NODE_POLYGON)
		{
			IPolygon* pPolygon = pNode->GetDynamicComponent<IPolygon>();
			if (strSerial == SERIAL_NORMAL)
			{
				pPolygon->iSetBaseMaterial(strPicPath);
			}
			else if (strSerial == SERIAL_FENCE)
			{
				pPolygon->iSetFenceMaterial(strPicPath);
			}
			else if (strSerial == SERIAL_HOVER)
			{
				pPolygon->iSetHoverBaseMaterial(strPicPath);
			}
			else if (strSerial == SERIAL_HOVER_FENCE)
			{
				pPolygon->iSetHoverFenceMaterial(strPicPath);
			}
		}

		if (pNode->iGetNodeType() == NODE_ROAD)
		{
			IRoad* pRoad = pNode->GetDynamicComponent<IRoad>();
			pRoad->iSetMaterialTexturePath(strPicPath);
		}

		if (pNode->iGetNodeType() == NODE_FLYLINE)
		{
			IFlyLine* pFlyLine = pNode->GetDynamicComponent<IFlyLine>();
			pFlyLine->iSetMaterial(strPicPath);
		}
		if (pNode->iGetNodeType() == NODE_MULTI_FLYLINE)
		{
			IMultiFlyLine* pFlyLine = pNode->GetDynamicComponent<IMultiFlyLine>();
			if (strSerial == SERIAL_MULTI_FLYLINE_FIRST)
			{
				pFlyLine->iSetFirstMaterial(strPicPath);
			}
			else if (strSerial == SERIAL_MULTI_FLYLINE_SECOND)
			{
				pFlyLine->iSetSecondMaterial(strPicPath);
			}
		}
	}
}

void ParamNetImage::AddModelTask(INode* pNode, std::string strPicServerIp)
{
	if (!pNode)
	{
		return;
	}

	TaskData sData;
	sData.pNode = pNode;
	sData.strServerIp = strPicServerIp;
	m_vecModelTask.push_back(sData);
}

void ParamNetImage::doModelTask(INode* pNode, std::string strPicServerIp)
{
	if (!pNode)
	{
		return;
	}
	INode* pRetNode = pNode;
	bool bBaseMode = false;		//是不是底座模式
	IBoard* pIBoard = pNode->GetDynamicComponent<IBoard>();
	BoardParam* pParam = pIBoard->iGetBoardParam();
	if (pParam->strModelName.empty() && pIBoard->iGetBaseBoard())
	{
		//底座
		INode* pBaseNode = pIBoard->iGetBaseBoard();
		pParam = pBaseNode->GetDynamicComponent<IBoard>()->iGetBoardParam();
		pRetNode = pBaseNode;
		bBaseMode = true;
	}
	std::string strPath = pParam->strModelName;
	//目前仅支持zip格式
	if (strPath.find(".zip") == std::string::npos)
	{
		return;
	}
	std::string strZipName;
	std::vector<std::string> vecTemp;
	SplitStringBySpecial(strPath, vecTemp, "/");
	if (vecTemp.empty())
	{
		return;
	}
	strZipName = vecTemp[vecTemp.size() - 1];
	strZipName = Replace_all(strZipName, ".zip", "");

	//strPath = Replace_all(strPath, ".zip","");
	//strFileName = strPath.substr(strPath.find_last_of("/") + 1);

	if (strZipName.empty())
	{
		return;
	}

	std::string strFullPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir + "/" + MODEL_DIR;
	MakeDirs(strFullPath.c_str());
	strFullPath += strZipName;
	std::string strMshName;
	bool bDownLoad = true;
	if (IsFileExist(strFullPath))
	{
		std::vector<FileInfo> vecFiles;
		GetFilesFromDir(strFullPath, vecFiles, true, "msh");
		if (!vecFiles.empty())
		{
#ifdef _WIN32
			strMshName = vecFiles[0].tInfo.name;
#else
			strMshName = vecFiles[0].tInfo->d_name;
#endif
			bDownLoad = false;
		}
	}
	//下载
	if (!bDownLoad)
	{
		std::string strModelFile = MODEL_DIR + strZipName + "/models/" + strMshName;
		pRetNode->GetDynamicComponent<IRenderNode>()->iSetModelFile(strModelFile);
		pRetNode->iLoadModel();
		if (bBaseMode)
		{
			pNode->iLoadModel();
		}
	}
	else
	{
		//真实网络地址
		bool bLocal = false;
		if (StartWith(strPath, "resource"))
		{
			bLocal = true;
		}
		else if (!StartWith(strPath, "http") && !StartWith(strPath, "https"))
		{
			strPath = strPicServerIp + "/" + strPath;
		}
		Mkdirs(strFullPath.c_str());
		std::string strDownZipPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir + "/" + MODEL_DIR;
		std::string strRealDownPath = strDownZipPath + "/" + strZipName + ".zip";
		if (!bLocal)
		{
			//启用线程下载
			ThreadRequestParam stParam;
			stParam.eRequestType = THREAD_REQUEST_DOWNLOAD;
			stParam.AddExtraJValue("down_zip_path", strDownZipPath);
			stParam.AddExtraJValue("zip_name", strZipName);
			stParam.AddExtraJValue("node_id", pRetNode->iGetID());
			if (bBaseMode)
			{
				stParam.AddExtraJValue("board_node_id", pNode->iGetID());
			}
			else
			{
				stParam.AddExtraJValue("board_node_id", "");
			}
			stParam.AddParam("this", (INT_PTR)this);
			stParam.strDownloadPath = strRealDownPath;
			stParam.strUrl = strPath;
			stParam.pThreadCompleteFunc = static_download_model;
			stParam.bRunCallbackInMainThread = true;
			ThreadWrapper::GetInstance()->AddTask(stParam);
		}
		else
		{
			//本地resource的路径
			std::string strRealZipPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir + "/";
			strRealZipPath += strPath;
			doingModifyDownModel(strRealZipPath, strDownZipPath, strZipName, pRetNode->iGetID(), bBaseMode ? pNode->iGetID() : "", false);
		}
	}
}

void ParamNetImage::static_download_model(ThreadCallbackParam* pParam)
{
	ParamNetImage* pThis = (ParamNetImage*)pParam->sRequestParam.mapParams["this"];
	std::string strDownZipPath = pParam->sRequestParam.GetExtraJValue("down_zip_path").asString();
	std::string strZipName = pParam->sRequestParam.GetExtraJValue("zip_name").asString();
	std::string strNodeID = pParam->sRequestParam.GetExtraJValue("node_id").asString();
	std::string strBoardNodeID = pParam->sRequestParam.GetExtraJValue("board_node_id").asString();
	if (pParam->nResult == STATUS_SUCCESS)
	{
		std::string strRealZipPath = pParam->sRequestParam.strDownloadPath;
		pThis->doingModifyDownModel(strRealZipPath, strDownZipPath, strZipName, strNodeID, strBoardNodeID, true);
	}
}

void ParamNetImage::handlenode(XMLDoc& xmlDoc, XMLNodePtr node, std::string strRealPath)
{
	if (node != nullptr)
	{
		for (node = node->first_node(); node != nullptr; node = node->next_sibling())
		{
			if (node == nullptr)
			{
				return;
			}
			if (node->type() == rapidxml::node_type::node_element)
			{
				std::string strNodeName = node->name();
				std::string strNodeValue = node->value();
				if (strNodeName == "MAP")
				{
					if (StartWith(strNodeValue, strRealPath))
					{
						continue;
					}
					std::string strResult;
					int nFirst = strNodeValue.find_first_of("/");
					strResult = strNodeValue.substr(nFirst + 1);
					strResult = strRealPath + "/" + strResult;
					XMLNodePtr newMap = xmlDoc.allocate_node(rapidxml::node_type::node_element,
						xmlDoc.allocate_string("MAP"), xmlDoc.allocate_string(strResult.c_str()));

					for (rapidxml::xml_attribute<>* attr = node->first_attribute(); attr != NULL; attr = attr->next_attribute())
					{
						rapidxml::xml_attribute<>* copy = xmlDoc.allocate_attribute(xmlDoc.allocate_string(attr->name()),
							xmlDoc.allocate_string(xmlDoc.allocate_string(attr->value())));
						newMap->append_attribute(copy);
					}

					node->parent()->insert_node(node, newMap);
					node->parent()->remove_node(node);
					node = newMap;
				}

				handlenode(xmlDoc, node, strRealPath);
			}
		}
	}
}

void ParamNetImage::doingModifyDownModel(std::string strZipRealPath, std::string strUnzipPath,
	std::string strZipName, std::string strNodeID, std::string  strBoardNodeID, bool bDeleteZip)
{
	std::string strRealZipPath = strZipRealPath;
	if (!IsFileExist(strRealZipPath))
	{
		return;
	}
	std::string strUnZipPath = strUnzipPath + strZipName;
	std::string strRealFileName;			//msh等的真实名称
	Mkdirs(strUnZipPath.c_str());
	//进行解压
	UnZipFile(strRealZipPath.c_str(), strUnZipPath.c_str());
	if (bDeleteZip)
	{
		remove(strRealZipPath.c_str());
	}
	std::vector<FileInfo> vecFiles;
	GetFilesFromDir(strUnZipPath, vecFiles, true, "msh");
	if (!vecFiles.empty())
	{
#ifdef _WIN32
		strRealFileName = vecFiles[0].tInfo.name;
#else
		strRealFileName = vecFiles[0].tInfo->d_name;
#endif
	}
	if (strRealFileName.empty())
	{
		return;
	}
	strRealFileName = Replace_all(strRealFileName, ".msh", "");
	//更改script文件中的贴图路径
	std::string strScriptlFile = strUnZipPath + "/models/" + strRealFileName + ".script";
	BCXmlFile xFile;
	XMLDoc xmlDoc;
	std::string strTexturesPath = MODEL_DIR + strZipName + "/textures";
	if (xFile.Load(strScriptlFile.c_str(), true))
	{
		char* pXml = xmlDoc.allocate_string(0, xFile.GetBufferLength());
		memset(pXml, 0, xFile.GetBufferLength());
		//size_t nLen = xFile.ToASCII(pXml);
		size_t nLen = xFile.GetUtf8Buff(pXml, xFile.GetBufferLength());
		try
		{
			xmlDoc.parse<0>(pXml);
			XMLNodePtr pScript = xmlDoc.first_node("SCRIPT");
			handlenode(xmlDoc, pScript, strTexturesPath);
			std::ofstream outFile(strScriptlFile);
			XMLDoc documentOut;
			XMLNodePtr nodePi = documentOut.allocate_node(rapidxml::node_type::node_pi, "xml version='1.0' encoding='utf-8'");
			documentOut.append_node(nodePi);
			documentOut.append_node(pScript);
			outFile << documentOut;
			outFile.close();
		}
		catch (rapidxml::parse_error&)
		{
		}
	}
	//iLoadModel
	std::string strModelFile = MODEL_DIR + strZipName + "/models/" + strRealFileName + ".msh";
	INode* pTargetNode = APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iFindNodeByID(strNodeID);
	if (!pTargetNode)
	{
		return;
	}
	pTargetNode->GetDynamicComponent<IRenderNode>()->iSetModelFile(strModelFile);
	pTargetNode->iLoadModel();
	if (!strBoardNodeID.empty())
	{
		INode* pBoardNode = APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iFindNodeByID(strBoardNodeID);
		if (!pBoardNode)
		{
			return;
		}
		pBoardNode->iLoadModel();
	}
}

bool ParamNetImage::decompressAndCreateScript(const std::string& strZipPath, bool bDeleteZip)
{
	std::string strZipFilePath = Replace_all(strZipPath, "\\", "/");
	if (!IsFileExist(strZipFilePath))
	{
		return false;
	}
	int nPosStart = strZipFilePath.find_last_of("/");
	int nPosEnd = strZipFilePath.find_last_of(".");
	std::string strFileName = strZipFilePath.substr(nPosStart + 1, nPosEnd - nPosStart - 1);
	std::string strClientDLL = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strClientDLL;
	std::transform(strClientDLL.begin(), strClientDLL.end(),
		strClientDLL.begin(), [](unsigned char c)
		{
			return std::tolower(c);
		});
	std::string strClient = strClientDLL;
	std::string strResult = GetProjectExecuteName();
#ifndef __GNUC__
	std::string strFolder = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir +
		"/" + PIC_DIR + "/" + strClient + "/" + strResult + "/" + strFileName;
#else
	std::string strFolder = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir +
		"/" + PIC_DIR + "/" + strClient + "/" + strFileName;
#endif
	Mkdirs(strFolder.c_str());
	if (UnZipFile(strZipPath.c_str(), strFolder.c_str()))
	{
		//创建Xml文件
		XMLDoc xmlDocument;
		XMLNodePtr pNodeType = xmlDocument.allocate_node(rapidxml::node_pi, "xml version='1.0' encoding='utf-8'");
		XMLNodePtr pNodeScript = xmlDocument.allocate_node(rapidxml::node_element, "SCRIPT");

		std::string strMaterial = strFileName + "_" + strClient;
		XMLAttributePtr pAttrMaterialName = xmlDocument.allocate_attribute("name", strMaterial.c_str());
		XMLNodePtr pNodeMaterial = xmlDocument.allocate_node(rapidxml::node_element, "MATERIAL");
		pNodeMaterial->append_attribute(pAttrMaterialName);
		pNodeScript->append_node(pNodeMaterial);

		XMLNodePtr pNodeCull = xmlDocument.allocate_node(rapidxml::node_element, "CULL", "none");
		pNodeMaterial->append_node(pNodeCull);

		XMLNodePtr pNodeBlendmode = xmlDocument.allocate_node(rapidxml::node_element, "BLENDMODE");
		XMLAttributePtr pAttrBlendmodeMode = xmlDocument.allocate_attribute("mode", "blend");
		pNodeBlendmode->append_attribute(pAttrBlendmodeMode);
		pNodeMaterial->append_node(pNodeBlendmode);

		XMLNodePtr pNodeBlend = xmlDocument.allocate_node(rapidxml::node_element, "BLEND");
		XMLAttributePtr pAttrBlendmodeFunc = xmlDocument.allocate_attribute("func", "blend");
		XMLAttributePtr pAttrBlendmodeAlphaThreshold = xmlDocument.allocate_attribute("AlphaThreshold", "0.000000");
		pNodeBlend->append_attribute(pAttrBlendmodeFunc);
		pNodeBlend->append_attribute(pAttrBlendmodeAlphaThreshold);
		pNodeBlendmode->append_node(pNodeBlend);

		XMLNodePtr pNodeCompileim = xmlDocument.allocate_node(rapidxml::node_element, "COMPILEIM", "true");
		pNodeMaterial->append_node(pNodeCompileim);

		XMLNodePtr pNodeTexanimation = xmlDocument.allocate_node(rapidxml::node_element, "TEXANIMATION");
		XMLAttributePtr pAttrTexanimationEnable = xmlDocument.allocate_attribute("enable", "false");
		XMLAttributePtr pAttrTexanimationType = xmlDocument.allocate_attribute("type", "0");
		XMLAttributePtr pAttrTexanimationSpeed = xmlDocument.allocate_attribute("speed", "0.500000");
		pNodeTexanimation->append_attribute(pAttrTexanimationEnable);
		pNodeTexanimation->append_attribute(pAttrTexanimationType);
		pNodeTexanimation->append_attribute(pAttrTexanimationSpeed);
		pNodeMaterial->append_node(pNodeTexanimation);

		XMLNodePtr pNodePlanereflect = xmlDocument.allocate_node(rapidxml::node_element, "PLANEREFLECT");
		XMLAttributePtr pAttrPlanereflectEnable = xmlDocument.allocate_attribute("Enable", "false");
		XMLAttributePtr pAttrPlanereflectRate = xmlDocument.allocate_attribute("Rate", "0.500000");
		XMLAttributePtr pAttrPlanereflectBlur = xmlDocument.allocate_attribute("Blur", "false");
		pNodePlanereflect->append_attribute(pAttrPlanereflectEnable);
		pNodePlanereflect->append_attribute(pAttrPlanereflectRate);
		pNodePlanereflect->append_attribute(pAttrPlanereflectBlur);
		pNodeMaterial->append_node(pNodePlanereflect);

		XMLNodePtr pNodeLightingmode = xmlDocument.allocate_node(rapidxml::node_element, "LIGHTINGMODE");
		XMLAttributePtr pAttrLightingmodeMode = xmlDocument.allocate_attribute("Mode", "NONE");
		pNodeLightingmode->append_attribute(pAttrLightingmodeMode);
		XMLNodePtr pNodePbr = xmlDocument.allocate_node(rapidxml::node_element, "PBR");
		XMLAttributePtr pAttrPbrAO = xmlDocument.allocate_attribute("AO", "1.000000");
		XMLAttributePtr pAttrPbrMetallic = xmlDocument.allocate_attribute("Metallic", "0.000000");
		XMLAttributePtr pAttrPbrRoughness = xmlDocument.allocate_attribute("Roughness", "0.700000");
		XMLAttributePtr pAttrPbrSpecular = xmlDocument.allocate_attribute("Specular", "1.000000");
		pNodePbr->append_attribute(pAttrPbrAO);
		pNodePbr->append_attribute(pAttrPbrMetallic);
		pNodePbr->append_attribute(pAttrPbrRoughness);
		pNodePbr->append_attribute(pAttrPbrSpecular);
		pNodeLightingmode->append_node(pNodePbr);
		pNodeMaterial->append_node(pNodeLightingmode);

		XMLNodePtr pNodeGlow = xmlDocument.allocate_node(rapidxml::node_element, "GLOW");
		XMLNodePtr pNodeEnable = xmlDocument.allocate_node(rapidxml::node_element, "ENABLE", "false");
		XMLNodePtr pNodeScale = xmlDocument.allocate_node(rapidxml::node_element, "SCALE", "0.000000");
		XMLNodePtr pNodeGlowColor = xmlDocument.allocate_node(rapidxml::node_element, "COLOR");
		XMLAttributePtr pAttrColorGlowR = xmlDocument.allocate_attribute("r", "0.000000");
		XMLAttributePtr pAttrColorGlowG = xmlDocument.allocate_attribute("g", "0.000000");
		XMLAttributePtr pAttrColorGlowB = xmlDocument.allocate_attribute("b", "0.000000");
		XMLAttributePtr pAttrColorGlowA = xmlDocument.allocate_attribute("a", "1.000000");
		XMLAttributePtr pAttrColorGlowEnable = xmlDocument.allocate_attribute("enable", "false");
		pNodeGlowColor->append_attribute(pAttrColorGlowR);
		pNodeGlowColor->append_attribute(pAttrColorGlowG);
		pNodeGlowColor->append_attribute(pAttrColorGlowB);
		pNodeGlowColor->append_attribute(pAttrColorGlowA);
		pNodeGlowColor->append_attribute(pAttrColorGlowEnable);
		XMLNodePtr pNodeFlash = xmlDocument.allocate_node(rapidxml::node_element, "FLASH");
		XMLAttributePtr pAttrFlashEnable = xmlDocument.allocate_attribute("enable", "fasle");
		XMLAttributePtr pAttrFlashSpeed = xmlDocument.allocate_attribute("speed", "2.0");
		pNodeFlash->append_attribute(pAttrFlashEnable);
		pNodeFlash->append_attribute(pAttrFlashSpeed);
		XMLNodePtr pNodeColorscale = xmlDocument.allocate_node(rapidxml::node_element, "COLORSCALE", "5.000000");
		pNodeGlow->append_node(pNodeEnable);
		pNodeGlow->append_node(pNodeScale);
		pNodeGlow->append_node(pNodeGlowColor);
		pNodeGlow->append_node(pNodeFlash);
		pNodeGlow->append_node(pNodeColorscale);
		pNodeMaterial->append_node(pNodeGlow);

		XMLNodePtr pNodeViewspecular = xmlDocument.allocate_node(rapidxml::node_element, "VIEWSPECULAR");
		XMLAttributePtr pAttrViewspecularEnable = xmlDocument.allocate_attribute("enable", "false");
		XMLAttributePtr pAttrViewspecularIntensity = xmlDocument.allocate_attribute("Intensity", "0.5");
		XMLAttributePtr pAttrViewspecularBaseIntensity = xmlDocument.allocate_attribute("BaseIntensity", "1.0");
		pNodeViewspecular->append_attribute(pAttrViewspecularEnable);
		pNodeViewspecular->append_attribute(pAttrViewspecularIntensity);
		pNodeViewspecular->append_attribute(pAttrViewspecularBaseIntensity);
		pNodeMaterial->append_node(pNodeViewspecular);

		XMLNodePtr pNodeSkyocclusion = xmlDocument.allocate_node(rapidxml::node_element, "SKYOCCLUSION");
		XMLAttributePtr pAttrSkyocclusionEnable = xmlDocument.allocate_attribute("enable", "false");
		XMLAttributePtr pAttrSkyocclusionIntensity = xmlDocument.allocate_attribute("Intensity", "1.0");
		pNodeSkyocclusion->append_attribute(pAttrSkyocclusionEnable);
		pNodeSkyocclusion->append_attribute(pAttrSkyocclusionIntensity);
		pNodeMaterial->append_node(pNodeSkyocclusion);

		XMLNodePtr pNodeFog = xmlDocument.allocate_node(rapidxml::node_element, "FOG");
		XMLNodePtr pNodeFogEnable = xmlDocument.allocate_node(rapidxml::node_element, "ENABLE", "false");
		pNodeFog->append_node(pNodeFogEnable);
		pNodeMaterial->append_node(pNodeFog);

		XMLNodePtr pNodeDepthfunc = xmlDocument.allocate_node(rapidxml::node_element, "DEPTHFUNC");
		XMLAttributePtr pAttrDepthfuncFunc = xmlDocument.allocate_attribute("func", "lequal");
		pNodeDepthfunc->append_attribute(pAttrDepthfuncFunc);
		pNodeMaterial->append_node(pNodeDepthfunc);

		XMLNodePtr pNodeCubereflect = xmlDocument.allocate_node(rapidxml::node_element, "CUBEREFLECT");
		XMLAttributePtr pAttrCubereflectEnable = xmlDocument.allocate_attribute("Enable", "false");
		XMLAttributePtr pAttrCubereflectRate = xmlDocument.allocate_attribute("Rate", "0.500000");
		XMLAttributePtr pAttrCubereflectCubeID = xmlDocument.allocate_attribute("CubeID", "");
		pNodeCubereflect->append_attribute(pAttrCubereflectEnable);
		pNodeCubereflect->append_attribute(pAttrCubereflectRate);
		pNodeCubereflect->append_attribute(pAttrCubereflectCubeID);
		pNodeMaterial->append_node(pNodeCubereflect);

		XMLNodePtr pNodeDepthwrite = xmlDocument.allocate_node(rapidxml::node_element, "DEPTHWRITE", "false");
		pNodeMaterial->append_node(pNodeDepthwrite);

		XMLNodePtr pNodeInstanced = xmlDocument.allocate_node(rapidxml::node_element, "INSTANCED", "false");
		pNodeMaterial->append_node(pNodeInstanced);

		XMLNodePtr pNodeColor = xmlDocument.allocate_node(rapidxml::node_element, "COLOR");
		XMLAttributePtr pAttrColorR = xmlDocument.allocate_attribute("r", "0.000000");
		XMLAttributePtr pAttrColorG = xmlDocument.allocate_attribute("g", "0.000000");
		XMLAttributePtr pAttrColorB = xmlDocument.allocate_attribute("b", "0.000000");
		XMLAttributePtr pAttrColorA = xmlDocument.allocate_attribute("a", "1.000000");
		pNodeColor->append_attribute(pAttrColorR);
		pNodeColor->append_attribute(pAttrColorG);
		pNodeColor->append_attribute(pAttrColorB);
		pNodeColor->append_attribute(pAttrColorA);
		pNodeMaterial->append_node(pNodeColor);

		XMLNodePtr pNodeBone = xmlDocument.allocate_node(rapidxml::node_element, "BONE");
		XMLAttributePtr pAttrBoneEnable = xmlDocument.allocate_attribute("Enable", "false");
		pNodeBone->append_attribute(pAttrBoneEnable);
		pNodeMaterial->append_node(pNodeBone);

		XMLNodePtr pNodeDiffuse_map = xmlDocument.allocate_node(rapidxml::node_element, "DIFFUSE_MAP");
		XMLNodePtr pNodeMapEnable = xmlDocument.allocate_node(rapidxml::node_element, "ENABLE", "true");
		XMLNodePtr pNodeChannel = xmlDocument.allocate_node(rapidxml::node_element, "CHANNEL", "0");
		XMLNodePtr pNodeMap = xmlDocument.allocate_node(rapidxml::node_element, "MAP");
		XMLAttributePtr pAttrMapType = xmlDocument.allocate_attribute("type", "animmap");
		XMLAttributePtr pAttrMapWrap = xmlDocument.allocate_attribute("wrap", "repeat");
		XMLAttributePtr pAttrMapMipmap = xmlDocument.allocate_attribute("mipmap", "true");
		XMLAttributePtr pAttrMapFps = xmlDocument.allocate_attribute("fps", "30.0");
		XMLAttributePtr pAttrMapFilter = xmlDocument.allocate_attribute("filter", "LINEAR");
		XMLAttributePtr pAttrMapCompileim = xmlDocument.allocate_attribute("compileim", "true");
		pNodeMap->append_attribute(pAttrMapType);
		pNodeMap->append_attribute(pAttrMapWrap);
		pNodeMap->append_attribute(pAttrMapMipmap);
		pNodeMap->append_attribute(pAttrMapFps);
		pNodeMap->append_attribute(pAttrMapFilter);
		pNodeMap->append_attribute(pAttrMapCompileim);
		pNodeDiffuse_map->append_node(pNodeMapEnable);
		pNodeDiffuse_map->append_node(pNodeChannel);
		pNodeDiffuse_map->append_node(pNodeMap);
		std::vector<FileInfo> vecFiles;
		//GetFilesFromDir(strFolder + "\\" + strFileName, vecFiles, false);
		GetFilesFromDir(strFolder, vecFiles, true);
		size_t nPos = strFolder.find("resource");
		std::vector<std::string*> vecStrPtr;
		if (nPos != std::string::npos)
		{
			std::string strTex = strFolder.substr(nPos);
			strTex = Replace_all(strTex, "\\", "/");
			std::string strPathName;
			if (vecFiles.size() > 0)
			{
				std::string strFilePathName = Replace_all(vecFiles[0].strAbsoluteDir, "\\", "/");
				strPathName = strFilePathName.substr(strFilePathName.find_last_of("/") + 1);
			}
			strTex += "/" + strPathName + "/";
			for (int i = 0; i < vecFiles.size(); i++)
			{
#ifdef 	_WIN32
				std::string* pStrTexTemp = new std::string(strTex + vecFiles[i].tInfo.name);
#else
				std::string* pStrTexTemp = new std::string(strTex + vecFiles[i].tInfo->d_name);
#endif

				vecStrPtr.push_back(pStrTexTemp);
				XMLNodePtr pNodeTex = xmlDocument.allocate_node(rapidxml::node_element, "TEX", (*pStrTexTemp).c_str());
				pNodeMap->append_node(pNodeTex);
			}
		}

		pNodeMaterial->append_node(pNodeDiffuse_map);

		xmlDocument.append_node(pNodeType);
		xmlDocument.append_node(pNodeScript);

		std::ofstream outFile(strFolder + "\\" + strFileName + ".script");
		if (outFile.is_open())
		{
			outFile << xmlDocument;
			outFile.close();
		}
		for (auto it : vecStrPtr)
		{
			DELETE_PTR(it);
		}
	}
	if (bDeleteZip)
	{
		remove(strZipPath.c_str());
	}
	return true;
}

bool ParamNetImage::CheckNetLoadModel(INode* pNode)
{
	if (!pNode)
	{
		return false;
	}
	bool bNetLoadModel = false;
	IBoard* pIBoard = pNode->GetDynamicComponent<IBoard>();
	if (pIBoard)
	{
		BoardParam* pBoardParam = pIBoard->iGetBoardParam();
		std::string strModelPath = pBoardParam->strModelName;
		if (StartWith(strModelPath, "http") || StartWith(strModelPath, "https"))
		{
			bNetLoadModel = true;
		}
		else if (strModelPath.find(".zip") != std::string::npos)
		{
			bNetLoadModel = true;
		}
		if (pBoardParam->strModelName.empty())
		{
			bNetLoadModel = false;
		}

		if (pIBoard->iGetBaseBoard())
		{
			bNetLoadModel = bNetLoadModel || CheckNetLoadModel(pIBoard->iGetBaseBoard());
		}
	}
	return bNetLoadModel;
}
