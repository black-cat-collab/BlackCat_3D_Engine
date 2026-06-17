
#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/base/VideoInspectionFactory.h"
#include "Project-Based/base/ProjectBasedClient.h"


using namespace bc;

VideoInspectionFactory::VideoInspectionFactory(ISystemAPI *pSystemAPI)
{
	m_pCurVideoInpectionPath = NULL;
	m_pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
}


VideoInspectionFactory::~VideoInspectionFactory()
{
	clear();
}

void VideoInspectionFactory::clear()
{
	m_pCurVideoInpectionPath = nullptr;
	for (size_t i = 0; i < m_vecVideoInspectionPath.size(); i++)
	{
		if (GetComponent<IVideoRoute>(m_vecVideoInspectionPath[i]))
		{
			GetComponent<IVideoRoute>(m_vecVideoInspectionPath[i])->iStopPlay();
			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_vecVideoInspectionPath[i], false);
		}
	}
	m_vecVideoInspectionPath.clear();
	
	for (std::map<std::string, std::vector<BCVideoRouterPointInfo*>>::iterator it = m_mapPathInfo.begin(); it != m_mapPathInfo.end(); it++)
	{
		for (size_t i = 0; i < it->second.size(); i++)
		{
			DELETE_PTR(it->second[i]);
		}
		it->second.clear();
	}
	m_mapPathInfo.clear();
}

void VideoInspectionFactory::Create(std::string strUrl,bool bSync)
{
	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_API_GET;
	stParam.bThread = !bSync;
	stParam.bRunCallbackInMainThread = !bSync;
	stParam.pThreadCompleteFunc = Callback_Get_Api_Inspection;
	char szBuffer[1024] = { 0 };
	stParam.strUrl = strUrl;
	m_pClient->AddHttpCommonParam(&stParam);
	stParam.strID = "VideoInspectionFactory";
	stParam.AddParam("this", (INT_PTR)this);
	ThreadWrapper::GetInstance()->AddTask(stParam);
}

IVideoRoute* VideoInspectionFactory::AddInspection(std::string strKey, vector<BCVideoRouterPointInfo*>& vecInfo)
{
	NodeParamBase stParam;
	stParam.nNodeType = NODE_VIDEO_ROUTE;
	INode *pPath = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(stParam);
	pPath->iSetSaveMyself(false);
	IVideoRoute* pRoute = static_cast<IVideoRoute*>(pPath->iGetComponent(COMPONENT_TYPE_VIDEO_ROUTE));
	pPath->iLoadModel();
	pRoute->iSetWaitOnNoVideoPoint(true);
	pRoute->iSetVideoChangeFunc(static_video_change,this);
	std::string strID = std::string("hc_inspection_") + strKey;
	pPath->iSetID(strID.c_str());
	APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(pPath);

	for (int i = 0; i < vecInfo.size(); i++)
	{
		INode* pNode = nullptr;
		if (vecInfo[i]->bWithVideo)
		{
			pNode = pRoute->iAddPoint(vecInfo[i]->vPos, vecInfo[i]->vAngle,&vecInfo[i]->stVideoParam);
		}
		else
		{
			pNode = pRoute->iAddPoint(vecInfo[i]->vPos, vecInfo[i]->vAngle, nullptr);
		}
		vecInfo[i]->pNode = pNode;
	}
	m_vecVideoInspectionPath.push_back(pPath);

	return pRoute;
}

void VideoInspectionFactory::iParseInspectionData(const char* pData)
{
	Json::Value jRetData = Json::arrayValue;
	try
	{
		Json::CharReaderBuilder jsonBuilder;
		std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
		Json::Value root;
		std::string jsError;
		if (reader->parse(pData, pData + strlen(pData), &root, &jsError))
		{
			//清空之前的
			clear();

			root = root["data"];
			for (int i = 0; i < root.size(); i++)
			{
				Json::Value jParam = {};
				std::string strName = "";
				if (root[i].isMember("name"))
				{
					strName = root[i]["name"].asString();
				}
				jParam["name"] = strName;
				jParam["list"] = Json::arrayValue;

				Json::Value items = root[i]["children"];
				for (int j = 0; j < items.size(); j++)
				{
					std::string strID = to_string(items[j]["id"].asInt());
					Json::Value jItem = {};
					std::string strName = "";
					if (items[j].isMember("name"))
					{
						strName = items[j]["name"].asString();
					}
					jItem["name"] = strName;
					jItem["key"] = strID;
					jParam["list"].append(jItem);
					int nTime = items[j]["time"].asInt();
					float fSpeed = items[j]["speed"].asFloat();
					float fHeight = items[j]["height"].asFloat();
					//if (fHeight < 0)
					//{
					//	fHeight = 60.0f;
					//}
					if (fSpeed <= 0)
					{
						fSpeed = 60.0f;
					}
					Json::Value points = items[j]["detail"];
					vector<BCVideoRouterPointInfo*> vecVideoInfo;
					for (int k = 0; k < points.size(); k++)
					{
						std::string strCameraID = points[k]["camera_id"].asString();
						//string strCameraID = "34020000001320000002";
						BCVideoRouterPointInfo* stPointInfo = new BCVideoRouterPointInfo;
						std::string strName = points[k]["name"].asString();
						stPointInfo->strName = strName;
						stPointInfo->strCameraID = strCameraID;
						bool bKey = points[k]["key"].asInt() != 0;
						std::string pos = points[k]["pos"].isNull() ? "" : points[k]["pos"].asString();
						double dLng = points[k]["lng"].asDouble();
						double dLat = points[k]["lat"].asDouble();
						std::string strUrl = "";
						if (points[k].isMember("url"))
						{
							strUrl = points[k]["url"].asString();
						}
						stPointInfo->strUrl = strUrl;
						Vector3d vOrigin;
						Vector3d vAngle;
						if (!pos.empty())
						{
							sscanf_s(pos.c_str(), "%lf,%lf,%lf,%lf,%lf,%lf", &vOrigin.x, &vOrigin.y, &vOrigin.z, &vAngle.x, &vAngle.y, &vAngle.z);
						}
						else
						{
							m_pClient->ConvertLngToVector3(dLng, dLat, &vOrigin, false);
							m_pClient->AdjustZ(vOrigin,fHeight,false);
						}
						stPointInfo->vPos = vOrigin;
						stPointInfo->vAngle = vAngle;
						if (bKey)
						{
							stPointInfo->bWithVideo = true;
						}
						vecVideoInfo.push_back(stPointInfo);
					}
					iModifyPointInfo(strID, vecVideoInfo);
					IVideoRoute* pPath = AddInspection(strID, vecVideoInfo);
					m_mapPathInfo[strID] = vecVideoInfo;
					pPath->iSetMoveSpeed(fSpeed);
					pPath->iSetWaitTime(nTime/1000);
					iModifyInspectionPath(strID, pPath);

				}
				jRetData.append(jParam);
			}
		}
	}
	catch (const std::exception&)
	{

	}

	m_jRetData = jRetData;
	iOnLoaded(jRetData);
}

void VideoInspectionFactory::Callback_Get_Api_Inspection(ThreadCallbackParam *pParam)
{
	VideoInspectionFactory *pThis = (VideoInspectionFactory*)pParam->sRequestParam.mapParams["this"];
	pThis->iParseInspectionData(pParam->strResponse.c_str());
}

void VideoInspectionFactory::iModifyInspectionPath(string strKey, IVideoRoute* pPath)
{
	pPath->iSetWaitOnNoVideoPoint(false);
	pPath->iSetFullScreenOffset(0.0f);
	//pPath->iSetConstantSpeed(true);
}

void VideoInspectionFactory::GetAllVideoInspectionPath(std::vector<IVideoRoute*>& vecRoute)
{
	for (int i = 0; i < m_vecVideoInspectionPath.size(); i++)
	{
		IVideoRoute* pRouter = static_cast<IVideoRoute*>(m_vecVideoInspectionPath[i]->iGetComponent(COMPONENT_TYPE_VIDEO_ROUTE));
		vecRoute.push_back(pRouter);
	}
}

IVideoRoute* VideoInspectionFactory::FindVideoInspectionPathBy(std::string strKey)
{
	for (int i = 0; i < m_vecVideoInspectionPath.size(); i++)
	{
		std::string strID = std::string("hc_inspection_") + strKey;
		if (std::string(m_vecVideoInspectionPath[i]->iGetID()) == strID)
		{
			IVideoRoute* pRouter = static_cast<IVideoRoute*>(m_vecVideoInspectionPath[i]->iGetComponent(COMPONENT_TYPE_VIDEO_ROUTE));
			return pRouter;
		}
	}
	return NULL;
}

//播放
void VideoInspectionFactory::Play(std::string strKey, int nLoop)
{
	if (m_pCurVideoInpectionPath)
	{
		for (int i = 0; i < m_vecVideoInspectionPath.size(); i++)
		{
			std::string strID = std::string("hc_inspection_") + strKey;
			if (std::string(m_vecVideoInspectionPath[i]->iGetID()) == strID)
			{
				IVideoRoute* pCurRouter = static_cast<IVideoRoute*>(m_vecVideoInspectionPath[i]->iGetComponent(COMPONENT_TYPE_VIDEO_ROUTE));
				if (pCurRouter == m_pCurVideoInpectionPath)
				{
					break;
				}
			}
		}
		m_pCurVideoInpectionPath->iStopPlay();
	}
	IVideoRoute* pPath = FindVideoInspectionPathBy(strKey);
	m_pCurVideoInpectionPath = pPath;
	if (pPath)
	{
		pPath->iSetLoop(true, nLoop);
		pPath->iBeginPlay();
	}
}

//暂停
void VideoInspectionFactory::Pause()
{
	if (m_pCurVideoInpectionPath)
	{
		m_pCurVideoInpectionPath->iPause(true);
	}
}

//恢复
void VideoInspectionFactory::Resume()
{
	if (m_pCurVideoInpectionPath)
	{
		m_pCurVideoInpectionPath->iPause(false);
	}
}

//停止
void VideoInspectionFactory::Stop()
{
	if (m_pCurVideoInpectionPath)
	{
		m_pCurVideoInpectionPath->iStopPlay();
	}
	m_pCurVideoInpectionPath = NULL;
}

//是否暂停中
bool VideoInspectionFactory::IsPause()
{
	if (m_pCurVideoInpectionPath)
	{
		return m_pCurVideoInpectionPath->iIsPause();
	}
	return false;
}

bool VideoInspectionFactory::static_video_change(INode* pVideoBoard, void* pUser, bool bOpen)
{
	VideoInspectionFactory* pThis = (VideoInspectionFactory*)pUser;

	bool bNext = true;
	for (std::map<std::string, std::vector<BCVideoRouterPointInfo*>>::iterator it = pThis->m_mapPathInfo.begin(); it != pThis->m_mapPathInfo.end(); it++)
	{
		for (size_t i = 0; i < it->second.size(); i++)
		{
			if (it->second[i]->pNode == pVideoBoard)
			{
				bNext = pThis->iChangeVideoPoint(it->second[i],bOpen);
				break;
			}
		}
	}
	return bNext;
}

