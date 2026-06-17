#include "ProjectBasedHeader.h"
#include "Engine/BuilderCoreHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ParamNetImage.h"
#include "Project-Based/utils/ProjectLogger.h"
#include "Project-Based/module/IStateModule.h"
#include "Project-Based/module/BaseModule.h"
#include "Project-Based/module/YBSSModule.h"
#include "Project-Based/module/VideoFusionModule.h"
#include "Project-Based/base/ProjectBasedClient.h"
#include "Project-Based/webApi/WebCommonApi.h"
#include "Project-Based/utils/KMapAdapter.h"
#include "Project-Based/label/CommonLabelFactory.h"
#include "Project-Based/label/LabelDetailFactory.h"
#include "Project-Based/utils/tools/BaseStatisticsTool.h"
#include "Project-Based/utils/tools/CircleStatisticsTool.h"
#include "Project-Based/utils/tools/LineStatisticsTool.h"
#include "Project-Based/utils/tools/PolygonStatisticsTool.h"
#include "Project-Based/utils/tools/AreaMeasureTool.h"
#include "Project-Based/utils/tools/DistanceMeasureTool.h"
#include "Project-Based/utils/tools/SightAnalysisTool.h"
#include "Project-Based/utils/ProjectUtils.h"
#include "Project-Based/utils/ParamParseStream.h"
#include "Project-Based/base/HeatmapFactory.h"
#include "Project-Based/Factory/TencentHeatMapFactory.h"
#include "../label/HeightTifReader.h"
#include "../Factory/LayerDebugSceneLoader.h"

#ifdef _MSC_VER
#include <process.h>
#include <tchar.h>
#endif

#define DPI  96

using namespace bc;

#define CLIENT_AES_KEY "123456708123456781234567"
DefaultVideoInspectionFactory::DefaultVideoInspectionFactory(ProjectBasedClient* pClient) :
	BCVideoInspectionFactory(APIProvider::GetSystemAPI())
{
	m_pClient = pClient;
}

DefaultVideoInspectionFactory::~DefaultVideoInspectionFactory()
{
}

void DefaultVideoInspectionFactory::iSetVideoBoardParam(const string& strKey, BCVideoRouterPointInfo* pInfo, VideoBoardWithButtonParam* pParam)
{
	m_pClient->iOnSetVideoInspectionParam(strKey, pInfo, pParam);
}

void DefaultVideoInspectionFactory::iInterceptHttpRequest(ThreadRequestParam* pRequest)
{
	m_pClient->AddHttpCommonParam(pRequest);
}

void DefaultVideoInspectionFactory::iLnglatToScenePos(const double& dLng, const double& dLat, const double& dZ, Vector3d& vOutPos, Vector3d& vOutAngle)
{
	m_pClient->iOnVideoInspectionToScenePos(dLng, dLat, dZ, vOutPos, vOutAngle);
}

void DefaultVideoInspectionFactory::iOnLoaded(Json::Value jRetData)
{
	m_pClient->iOnVideoInspectionLoadedData(jRetData);
}

bool DefaultVideoInspectionFactory::iChangeVideoPoint(BCVideoRouterPointInfo* pInfo, bool bOpen)
{
	return m_pClient->iOnChangeVideoPoint(pInfo, bOpen);
}

////////////////////////////////////////////////////////////////////////
DefaultHeatMapFactory::DefaultHeatMapFactory(ISystemAPI* pSystemAPI, ProjectBasedClient* pClient) :
	TencentHeatMapFactory(pSystemAPI)
{
	m_pClient = pClient;
}

DefaultHeatMapFactory::~DefaultHeatMapFactory()
{
}

Vector3d DefaultHeatMapFactory::iBoundaryLnglatToScenePos(string strKey, const double dlng,
	const double dlat)
{
	Vector3d vPos;

	double dx, dy;
	m_pClient->GCJ02_to_WGS(dlng, dlat, dx, dy);
	m_pClient->ConvertLngToVector3(dx, dy, &vPos, false);
	return vPos;
}

Vector3d DefaultHeatMapFactory::iHeatmapLnglatToScenePos(string strKey, const double dlng,
	const double dlat)
{
	Vector3d vPos;

	double dx, dy;
	m_pClient->GCJ02_to_WGS(dlng, dlat, dx, dy);
	m_pClient->ConvertLngToVector3(dx, dy, &vPos, false);
	return vPos;
}

void DefaultHeatMapFactory::iLoadedData()
{
	std::string strData = "";
	m_pClient->ToSendWebCommond("VS_RefreshHeatmap", "", strData);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
DefaultIndoorFactory::DefaultIndoorFactory(ProjectBasedClient* pClient) :
	BuildIndoorFactory()
{
	m_pClient = pClient;
}

DefaultIndoorFactory::~DefaultIndoorFactory()
{
}

void DefaultIndoorFactory::iChangeBuilding(BuildingData* pData)
{
	if (!pData)
	{
		return;
	}
	INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
	m_vLastCameraOrigin = pCameraNode->iGetOrigin();
	m_vLastCameraAngle = pCameraNode->iGetAngles();
	GoViewPoint(pCameraNode, pData->strInnerViewport, false);
}

void DefaultIndoorFactory::iLeaveAreaScene(AreaSceneData* pData)
{
	BuildIndoorFactory::iLeaveAreaScene(pData);

	if (!m_vLastCameraAngle.Empty() || !m_vLastCameraOrigin.Empty())
	{
		INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
		GoViewPoint(pCameraNode, m_vLastCameraOrigin, m_vLastCameraAngle, false);

		m_vLastCameraAngle = Vector3d(0, 0, 0);
		m_vLastCameraOrigin = Vector3d(0, 0, 0);
	}
}

void DefaultIndoorFactory::iLoadedData()
{
	m_pClient->iOnBuildingDataLoadedData();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
DefaultProjectElementLoader::DefaultProjectElementLoader(ProjectBasedClient* pClient) :
	SceneElementLoader(),
	m_pClient(pClient)
{
}

DefaultProjectElementLoader::~DefaultProjectElementLoader()
{
}

void DefaultProjectElementLoader::iOnLoadedData(bool bAllComplete)
{
	if (bAllComplete)
	{
		m_pClient->ToSendWebCommond("VS_LoginSuccess3D", "", "");
		//给网页发送图层
		m_pClient->ToWebAllLayer("");
		//给网页发送建筑数据
		m_pClient->ToWebAllBuilding("");
		//给网页漫游数据
		m_pClient->ToWebAllRoamPath("");
		//给网页视频巡控数据
		m_pClient->ToWebAllVideoInspection("");

		//开始订阅
		m_pClient->RequestSubscribeBindIp();
	}
	m_pClient->DoOnSceneElementLoadedData(bAllComplete);
}

////////////////////////////////////////////////////////////////////////
ProjectBasedClient::ProjectBasedClient() :
	m_bLabelDebug(false),
	m_pConnector(nullptr),
	m_pNetCommand(nullptr),
	m_bRequestHeartbeat(true),
	m_bKillProcess(false),
	m_bNeedKillProcess(false),
	m_strVersion("not set project version!!!"),
	m_strEngineVersion("1.0.8.2024030280958_engine"), 
	m_fCGCS2000CentralMeridian(120.0f),
	m_bCameraInertialEnable(false),
	m_nAppMode(APP_MODE_EXHIBITION),
	m_pProjectGroupNode(nullptr),
	m_bAxisToolEnable(false),
	m_pSceneElementLoader(nullptr),
	m_bEnterAreaScene(true),
	m_pHeatMapFactory(nullptr),
	m_pWebCommonApi(nullptr),
	m_pApiGroupNode(nullptr)
{
	m_strApiHost = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strServerIp.c_str();
	//后台给的，默认userid=0的
	m_sUserInfo.strUserToken = "MRIZsuXm4bOkVZ6Z_az-BzMhLTbX5cFnXvmmlW0PKQoropxdO6DLBfytiTqUcn06SZ9bsqYBr6GFyXU6dm_ATA==";
}

ProjectBasedClient::~ProjectBasedClient()
{
	DELETE_PTR(m_pMapAdapter);
	DELETE_PTR(m_pHeatMapFactory);
}

void ProjectBasedClient::iInitialize(ISystemAPI* pSystemAPI)
{
	m_pWebCommonApi = new WebCommonApi(pSystemAPI, this);
	m_pMapAdapter = new KMapAdapter;

	EServerServion eType = APIProvider::GetSystemAPI()->iProtocolAPI->iGetServerVersion();
	m_strHeartbeatUrl = m_strApiHost + "/api/v10/heartbeat";
	ProjectLogger::GetInstance()->SystemMessage("ProjectBasedClient iInitialize");
	m_strRoamUrl = m_strApiHost + "/api/v10/roamList";
	m_strBuildingUrl = m_strApiHost + "/api/v10/buildingList?isTree=1";
	m_strVideoInspectionUrl = m_strApiHost + "/api/v10/inspectionList";

	string strConfigPath = pSystemAPI->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir + "/" + m_strProjectConfigFilePath;
	JsonHandlerManager::GetInstance()->InitProjectConfig(strConfigPath);

	// 是否开启三维图标工具
	string strToolIP = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strToolIp;
	if (strToolIP == "")
	{
		m_bLabelDebug = false;
	}
	else
	{
		m_bLabelDebug = true;
	}

	std::string strProjectDllPath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strClientDLL;
	std::string strVersion;
	if (GetProjectVersion(strProjectDllPath, strVersion))
	{
		m_strVersion = strVersion;
	}
	std::string str = "project version:" + m_strVersion;
	str += ",EngineAPI version:";
	//str += APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strEngineVersion;
	str += m_strEngineVersion;
	ProjectLogger::GetInstance()->InfoMessage(str);
	char sz[1024] = { 0 };
	sprintf_s(sz, sizeof(sz), "nCpuAllProcessorsNum=%d,nCpuProcessorsBegin=%d,nCpuProcessorsEnd=%d,nSelfClientIndex=%d,nRunningClientCount=%d",
		APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nCpuAllProcessorsNum,
		APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nCpuProcessorsBegin,
		APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nCpuProcessorsEnd,
		APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nSelfClientIndex,
		APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nRunningClientCount);
	ProjectLogger::GetInstance()->InfoMessage(sz);

	//读取场景文件夹coordinate_point.xml文件获取坐标转换参考点信息
	std::string strCoordinateFile = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strScenesDir;
	strCoordinateFile += "/../coordinate_point.xml";
	BCXmlFile xFile;
	if (xFile.Load(strCoordinateFile.c_str(), true))
	{
		XMLDoc	xmlDoc;
		char* pXml = xmlDoc.allocate_string(0, xFile.GetBufferLength());
		memset(pXml, 0, xFile.GetBufferLength());
		//size_t nLen = xFile.ToASCII(pXml);
		size_t nLen = xFile.GetUtf8Buff(pXml, xFile.GetBufferLength());
		try
		{
			xmlDoc.parse<0>(pXml);
			XMLNodePtr pAttr = xmlDoc.first_node("coordinate");
			if (pAttr)
			{
				//经纬度转换参考点
				XMLNodePtr pChild = pAttr->first_node("scene_convert");
				if (pChild)
				{
					XMLNodePtr pChild2 = pChild->first_node("utm_zoom");
					if (pChild2)
					{
						m_nUTM_project_zone = atoi(pChild2->value());
					}
					pChild2 = pChild->first_node("point_1");
					if (pChild2)
					{
						XMLNodePtr pChild3 = pChild2->first_node("lng_lat");
						if (pChild3)
						{
							std::string strTemp = pChild3->value();
							sscanf_s(strTemp.c_str(), "%f,%f", &m_vLngLatLeftTop.x, &m_vLngLatLeftTop.y);
							m_vSelfLngLatLeftTop = m_vLngLatLeftTop;
						}
						pChild3 = pChild2->first_node("scene");
						if (pChild3)
						{
							std::string strTemp = pChild3->value();
							sscanf_s(strTemp.c_str(), "%f,%f", &m_vScenePosLeftTop.x, &m_vScenePosLeftTop.y);
							m_vSelfScenePosLeftTop = m_vScenePosLeftTop;
						}
					}
					pChild2 = pChild->first_node("point_2");
					if (pChild2)
					{
						XMLNodePtr pChild3 = pChild2->first_node("lng_lat");
						if (pChild3)
						{
							std::string strTemp = pChild3->value();
							sscanf_s(strTemp.c_str(), "%f,%f", &m_vLngLatRightBottom.x, &m_vLngLatRightBottom.y);
							m_vSelfLngLatRightBottom = m_vLngLatRightBottom;
						}
						pChild3 = pChild2->first_node("scene");
						if (pChild3)
						{
							std::string strTemp = pChild3->value();
							sscanf_s(strTemp.c_str(), "%f,%f", &m_vScenePosRightBottom.x, &m_vScenePosRightBottom.y);
							m_vSelfScenePosRightBottom = m_vScenePosRightBottom;
						}
					}
				}
				//自身正射影像图经纬度转换参考点
				pChild = pAttr->first_node("map_convert");
				if (pChild)
				{
					XMLNodePtr pChild2 = pChild->first_node("point_1");
					if (pChild2)
					{
						XMLNodePtr pChild3 = pChild2->first_node("lng_lat");
						if (pChild3)
						{
							std::string strTemp = pChild3->value();
							sscanf_s(strTemp.c_str(), "%f,%f", &m_vSelfLngLatLeftTop.x, &m_vSelfLngLatLeftTop.y);
						}
						pChild3 = pChild2->first_node("scene");
						if (pChild3)
						{
							std::string strTemp = pChild3->value();
							sscanf_s(strTemp.c_str(), "%f,%f", &m_vSelfScenePosLeftTop.x, &m_vSelfScenePosLeftTop.y);
						}
					}
					pChild2 = pChild->first_node("point_2");
					if (pChild2)
					{
						XMLNodePtr pChild3 = pChild2->first_node("lng_lat");
						if (pChild3)
						{
							std::string strTemp = pChild3->value();
							sscanf_s(strTemp.c_str(), "%f,%f", &m_vSelfLngLatRightBottom.x, &m_vSelfLngLatRightBottom.y);
						}
						pChild3 = pChild2->first_node("scene");
						if (pChild3)
						{
							std::string strTemp = pChild3->value();
							sscanf_s(strTemp.c_str(), "%f,%f", &m_vSelfScenePosRightBottom.x, &m_vSelfScenePosRightBottom.y);
						}
					}
				}
				//聚合
				pChild = pAttr->first_node("cluster");
				if (pChild)
				{
					XMLNodePtr pChild2 = pChild->first_node("point_1");
					if (pChild2)
					{
						std::string strTemp = pChild2->value();
						sscanf_s(strTemp.c_str(), "%lf,%lf", &m_vClusterSceneTopLeft.x, &m_vClusterSceneTopLeft.y);
					}
					pChild2 = pChild->first_node("point_2");
					if (pChild2)
					{
						std::string strTemp = pChild2->value();
						sscanf_s(strTemp.c_str(), "%lf,%lf", &m_vClusterSceneBottomRight.x, &m_vClusterSceneBottomRight.y);
					}
					//解析聚合层级
					XMLNodePtr pSub = pChild->first_node("level");
					while (pSub)
					{
						ClusterFunc::ClusterLevel stLevel;
						XMLNodePtr pTemp = pSub->first_node("index");
						stLevel.nLevel = atoi(pTemp->value());
						pTemp = pSub->first_node("row");
						stLevel.nRow = atoi(pTemp->value());
						pTemp = pSub->first_node("column");
						stLevel.nColumn = atoi(pTemp->value());
						pTemp = pSub->first_node("distance");
						stLevel.fDistance = atof(pTemp->value());

						m_vecClusterLevel.push_back(stLevel);

						pSub = pSub->next_sibling("level");
					}
				}
				//高地图
				pChild = pAttr->first_node("tif_relative_path");
				if (pChild)
				{
					std::string strTemp = pChild->value();
					//已场景目录为基础
					std::string strTifFilePath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strScenesDir;
					strTifFilePath += "/../" + strTemp;
					m_strTifFilePath = strTifFilePath;
				}
			}
		}
		catch (rapidxml::parse_error&)
		{
		}
	}
	if (StartWith(m_strTifFilePath, "resource"))
	{
		m_strTifFilePath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir + "/" + m_strTifFilePath;
	}
	loadTifLevel(m_strTifFilePath);

	//{
	//	FlyLineParam stParam;
	//	string strKey = "FlyLineParam";
	//	string strPath2 = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + string("/style_json/") + strKey + ".json";

	//	stParam.bAnimation = true;

	//	if (!ParamParseStream::GetInstance()->IsContainKey(strKey))
	//	{
	//		ParamParseStream::GetInstance()->AddParamToJson(&stParam, strKey, "静态上图", strPath2);
	//	}
	//}

	//{
	//	BoardParam stParam;
	//	stParam.vBoardCenter = Vector2(0, 0.5f);
	//	stParam.bDynamicScaleEnable = true;
	//	stParam.fDynamicScale = 0.005f;
	//	stParam.bRotateAroundTarget = true;
	//	stParam.vDistanceToTarget = Vector2(0, 10.0f);
	//	stParam.direction = FaceToCamera;
	//	string strKey = "BoardParam";
	//	string strPath2 = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + string("/style_json/") + strKey + ".json";
	//	BoardBaseParam sBaseparam;
	//	stParam.bWithBase = false;
	//	stParam.tBaseParam = sBaseparam;
	//	if (!ParamParseStream::GetInstance()->IsContainKey(strKey))
	//	{
	//		ParamParseStream::GetInstance()->AddParamToJson(&stParam, strKey, "静态上图", strPath2);
	//	}
	//}

	//{
	//	FontBoardParam stParam;
	//	stParam.vBoardCenter = Vector2(0, 0.5f);
	//	stParam.bDynamicScaleEnable = true;
	//	stParam.fDynamicScale = 0.005f;
	//	stParam.bRotateAroundTarget = true;
	//	stParam.vDistanceToTarget = Vector2(0, 10.0f);
	//	stParam.direction = FaceToCamera;
	//	TextShow stText;
	//	stParam.SetText(stText);
	//	string strKey = "FontBoardParam";
	//	string strPath2 = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + string("/style_json/") + strKey + ".json";
	//	if (!ParamParseStream::GetInstance()->IsContainKey(strKey))
	//	{
	//		ParamParseStream::GetInstance()->AddParamToJson(&stParam, strKey, "静态上图", strPath2);
	//	}
	//}

	//{
	//	VideoBoardParam stParam;
	//	string strKey = "VideoBoardParam";
	//	string strPath2 = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + string("/style_json/") + strKey + ".json";
	//	if (!ParamParseStream::GetInstance()->IsContainKey(strKey))
	//	{
	//		ParamParseStream::GetInstance()->AddParamToJson(&stParam, strKey, "静态上图", strPath2);
	//	}
	//}
	//{
	//	ButtonBoardParam stParam;
	//	string strKey = "ButtonBoardParam";
	//	string strPath2 = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + string("/style_json/") + strKey + ".json";
	//	if (!ParamParseStream::GetInstance()->IsContainKey(strKey))
	//	{
	//		Json::Value item;
	//		string strJson = ParamParseStream::GetInstance()->ConvertParamToJson(&stParam, item);
	//		WriteToFile(strPath2, item.toStyledString().c_str());
	//	}
	//}
	//{
	//	FontBoardWithButtonParam stParam;
	//	stParam.vBoardCenter = Vector2(0, 0.5f);
	//	stParam.bDynamicScaleEnable = true;
	//	stParam.fDynamicScale = 0.005f;
	//	stParam.bRotateAroundTarget = true;
	//	stParam.vDistanceToTarget = Vector2(0, 10.0f);
	//	stParam.direction = FaceToCamera;
	//	string strKey = "FontBoardWithButtonParam";
	//	string strPath2 = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + string("/style_json/") + strKey + ".json";
	//	if (!ParamParseStream::GetInstance()->IsContainKey(strKey))
	//	{
	//		ParamParseStream::GetInstance()->AddParamToJson(&stParam, strKey, "静态上图", strPath2);
	//	}
	//}
	//{
	//	VideoBoardWithButtonParam stParam;
	//	stParam.vBoardCenter = Vector2(0, 0.5f);
	//	stParam.bDynamicScaleEnable = true;
	//	stParam.fDynamicScale = 0.005f;
	//	stParam.bRotateAroundTarget = true;
	//	stParam.vDistanceToTarget = Vector2(0, 10.0f);
	//	stParam.direction = FaceToCamera;
	//	string strKey = "VideoBoardWithButtonParam";

	//	VideoBoardParam tParam;
	//	stParam.vecVideoParam.push_back(tParam);

	//	ButtonBoardParam btParam;
	//	stParam.vecBoardParam.push_back(btParam);

	//	string strPath2 = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + string("/style_json/") + strKey + ".json";
	//	if (!ParamParseStream::GetInstance()->IsContainKey(strKey))
	//	{
	//		ParamParseStream::GetInstance()->AddParamToJson(&stParam, strKey, "静态上图", strPath2);
	//	}
	//}
	//{
	//	BoardWithFontBoardParam stParam;
	//	stParam.vBoardCenter = Vector2(0, 0.5f);
	//	stParam.bDynamicScaleEnable = true;
	//	stParam.fDynamicScale = 0.005f;
	//	stParam.bRotateAroundTarget = true;
	//	stParam.vDistanceToTarget = Vector2(0, 10.0f);
	//	stParam.direction = FaceToCamera;
	//	string strKey = "BoardWithFontBoardParam";
	//	string strPath2 = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + string("/style_json/") + strKey + ".json";
	//	if (!ParamParseStream::GetInstance()->IsContainKey(strKey))
	//	{
	//		ParamParseStream::GetInstance()->AddParamToJson(&stParam, strKey, "静态上图", strPath2);
	//	}
	//}
	//{
	//	PolygonParam stParam;
	//	string strKey = "PolygonParam";
	//	string strPath2 = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + string("/style_json/") + strKey + ".json";
	//	if (!ParamParseStream::GetInstance()->IsContainKey(strKey))
	//	{
	//		ParamParseStream::GetInstance()->AddParamToJson(&stParam, strKey, "静态上图", strPath2);
	//	}
	//}
	//{
	//	RoadParam stParam;
	//	stParam.eType = Road_Image;
	//	string strKey = "RoadParam";
	//	string strPath2 = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + string("/style_json/") + strKey + ".json";
	//	if (!ParamParseStream::GetInstance()->IsContainKey(strKey))
	//	{
	//		ParamParseStream::GetInstance()->AddParamToJson(&stParam, strKey, "静态上图", strPath2);
	//	}
	//}
	//{
	//	BoardInstanceParam stParam;
	//	stParam.vBoardCenter = Vector2(0, 0.5f);
	//	stParam.bDynamicScaleEnable = true;
	//	stParam.fDynamicScale = 0.005f;
	//	stParam.bRotateAroundTarget = true;
	//	stParam.vDistanceToTarget = Vector2(0, 10.0f);
	//	stParam.direction = FaceToCamera;
	//	string strKey = "BoardInstanceParam";
	//	string strPath2 = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + string("/style_json/") + strKey + ".json";
	//	if (!ParamParseStream::GetInstance()->IsContainKey(strKey))
	//	{
	//		ParamParseStream::GetInstance()->AddParamToJson(&stParam, strKey, "静态上图", strPath2);
	//	}
	//}
	//{
	//	LocusLineParam stParam;
	//	string strKey = "LocusLineParam";
	//	string strPath2 = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + string("/style_json/") + strKey + ".json";
	//	if (!ParamParseStream::GetInstance()->IsContainKey(strKey))
	//	{
	//		ParamParseStream::GetInstance()->AddParamToJson(&stParam, strKey, "静态上图", strPath2);
	//	}
	//}
	//{
	//	TextShow stParam;
	//	string strKey = "TextShow";
	//	string strPath2 = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + string("/style_json/") + strKey + ".json";
	//	if (!ParamParseStream::GetInstance()->IsContainKey(strKey))
	//	{
	//		string str = ParamParseStream::GetInstance()->ConvertTextShowToJson(stParam);
	//		WriteToFile(strPath2, str.c_str());
	//	}
	//}
	//{
	//	MultiFlyLineParam stParam;
	//	string strKey = "MultiFlyLineParam";
	//	string strPath2 = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + string("/style_json/") + strKey + ".json";
	//	if (!ParamParseStream::GetInstance()->IsContainKey(strKey))
	//	{
	//		ParamParseStream::GetInstance()->AddParamToJson(&stParam, strKey, "静态上图", strPath2);
	//	}
	//}
}

void ProjectBasedClient::iInitializeScene()
{
	ProjectLogger::GetInstance()->SystemMessage("ProjectBasedClient iInitializeScene");

	{
		NodeParamBase stParam;
		stParam.nNodeType = NODE_GROUP;
		m_pProjectGroupNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(stParam);
		m_pProjectGroupNode->iLoadModel();
		m_pProjectGroupNode->iSetID("project_group");
		m_pProjectGroupNode->iSetVisible(true);
		m_pProjectGroupNode->iSetSaveMyself(false);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode()->iAddChild(m_pProjectGroupNode);
	}
	{
		NodeParamBase stParam;
		stParam.nNodeType = NODE_GROUP;
		m_pApiGroupNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(stParam);
		m_pApiGroupNode->iLoadModel();
		m_pApiGroupNode->iSetID("api_group");
		m_pApiGroupNode->iSetVisible(true);
		m_pApiGroupNode->iSetSaveMyself(false);
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode()->iAddChild(m_pApiGroupNode);
	}

	Vector3d xf0 = Vector3d(m_vLngLatLeftTop.x, m_vLngLatLeftTop.y, 0);
	Vector3d xf0Point = Vector3d(m_vScenePosLeftTop.x, m_vScenePosLeftTop.y, 0.0f);
	m_pMapAdapter->SetReferencePoint0_UTM(xf0.x, xf0.y, 0, xf0Point);

	Vector3d xf1 = Vector3d(m_vLngLatRightBottom.x, m_vLngLatRightBottom.y, 0);
	Vector3d xf1Point = Vector3d(m_vScenePosRightBottom.x, m_vScenePosRightBottom.y, 0.0f);
	m_pMapAdapter->SetReferencePoint1_UTM(xf1.x, xf1.y, 0, xf1Point);

	m_pSceneElementLoader = iCreateSceneElementLoader();
	m_pSceneElementLoader->SetServerIp(m_strApiHost);
	m_pHeatMapFactory = iCreateHeatMapFactory();

	m_pCircleStatisticsTool = iCreateCircleStatisticsTool();
	m_pPOICircleStatisticsTool = iCreatePOICircleStatisticsTool();
	m_pLineStatisticsTool = iCreateLineStatisticsTool();
	m_pPolygonStatisticsTool = iCreatePolygonStatisticsTool();
	m_pPolygonStatisticsRectangularTool = iCreatePolygonStatisticsRectangularTool();

	m_vecStatisticsTool.push_back(m_pCircleStatisticsTool);
	m_vecStatisticsTool.push_back(m_pPOICircleStatisticsTool);
	m_vecStatisticsTool.push_back(m_pLineStatisticsTool);
	m_vecStatisticsTool.push_back(m_pPolygonStatisticsTool);
	m_vecStatisticsTool.push_back(m_pPolygonStatisticsRectangularTool);

	m_pAreaMeasureTool = iCreateAreaMeasureTool();
	m_pDistanceMeasureTool = iCreateDistanceMeasureTool();
	m_pViewshedAnalysisTool = new ViewshedAnalysisTool();
	m_pBuildingIndoorFactory = iCreateBuildingIndoorFactory();
	m_pVideoInspectionFactory = iCreateVideoInspectionFactory();
	m_pSightAnalysisTool = iCreateSightAnalysisTool();
	m_pWebLabelParser = iCreateWebLabelParser();
	m_pWebLabelParser->SetProjectPath(m_strProjectPath);
}

void ProjectBasedClient::iSceneLoad()
{
	{
		ToSendWebCommond("VS_PreSceneLoaded", "", "");
	}

	ProjectLogger::GetInstance()->SystemMessage("ProjectBasedClient iSceneLoad");
	if (m_bLabelDebug)
	{
		m_pLabelRegulator = new LayerDebugSceneLoader();
		string strToolIP = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strToolIp;
		m_pLabelRegulator->SetServerIp(strToolIP);
		m_pLabelRegulator->iRequestData(true);
	}
	{
		ToSendWebCommond("VS_SceneLoaded", "", "");
	}

	if (m_bRequestHeartbeat)
	{
		RequestHeartbeat();
	}
	requestRoamPath();

	if (m_pSceneElementLoader)
	{
		//一进系统加载一下图层数据
		m_pSceneElementLoader->OnlyRequestBaseInfo(true, false);
	}
	for (size_t i = 0; i < m_vecStatisticsTool.size(); i++)
	{
		if (m_vecStatisticsTool[i])
		{
			m_vecStatisticsTool[i]->SetStatisticsTemplet(m_jStatisticTemplate);
			m_vecStatisticsTool[i]->AddINTPTR("this", (INT_PTR)this);
			m_vecStatisticsTool[i]->AddCompleteFunc(static_callback_statistics);
		}
	}
	if (m_pAreaMeasureTool)
	{
		m_pAreaMeasureTool->AddINTPTR("this", (INT_PTR)this);
		WebCommandData* pCommond = new WebCommandData;
		m_pAreaMeasureTool->AddINTPTR("web_commond", (INT_PTR)pCommond);
		m_pAreaMeasureTool->AddCompleteFunc(static_area_measure);
	}
	if (m_pDistanceMeasureTool)
	{
		m_pDistanceMeasureTool->AddINTPTR("this", (INT_PTR)this);
		WebCommandData* pCommond = new WebCommandData;
		m_pDistanceMeasureTool->AddINTPTR("web_commond", (INT_PTR)pCommond);
		m_pDistanceMeasureTool->AddCompleteFunc(static_distance_measure);
	}
	if (m_pSightAnalysisTool)
	{
		m_pSightAnalysisTool->AddINTPTR("this", (INT_PTR)this);
		m_pSightAnalysisTool->AddCompleteFunc(static_callback_sight_analysis);
	}
	m_pViewshedAnalysisTool->Create();
	if (m_pBuildingIndoorFactory && !m_strBuildingUrl.empty())
	{
		m_pBuildingIndoorFactory->RequestData(m_strBuildingUrl, false);
	}
	if (m_pVideoInspectionFactory && !m_strVideoInspectionUrl.empty())
	{
		m_pVideoInspectionFactory->Create(m_strVideoInspectionUrl, false);
	}

	//将其他view隐藏，需要显示自己代码显示
	{
		IView* pView = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetSceneView(BUILDER_ORTHO_VIEW_ID);
		if (pView)
		{
			pView->iSetVisible(false);
		}
		pView = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetSceneView(BUILDER_ORTHO_EDITOR_VIEW_ID);
		if (pView)
		{
			pView->iSetVisible(false);
		}
	}
}

void ProjectBasedClient::iFrameUpdate()
{
	INode* pRoot = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode();
	int nCount = pRoot->iGetChildCount();
	for (int i = 0; i < nCount; ++i)
	{
		if (pRoot->iGetChild(i)->iGetNodeType() == NODE_MOVE_TOOL
			|| pRoot->iGetChild(i)->iGetNodeType() == NODE_ROTATE_TOOL
			|| pRoot->iGetChild(i)->iGetNodeType() == NODE_SCALE_TOOL)
		{
			pRoot->iGetChild(i)->iSetVisible(m_bAxisToolEnable);
		}
	}

	BCAutoLock lock(&m_MutexLock);
	{
		for (size_t i = 0; i < m_vecNextFrameWebComondData.size(); ++i)
		{
			m_queueWebCommand.push(m_vecNextFrameWebComondData[i]);
		}
		m_vecNextFrameWebComondData.clear();
		while (!m_queueWebCommand.empty())
		{
			WebCommandData& sData = m_queueWebCommand.front();
			BCEvent tEvent(PROJECTBASED_EVENT_PROJECT_WEB_COMMOND, (EVENT_PARAM)&sData);
			iDoWebCommond(&sData);
			if (sData.strCommand == "Web_Refesh3D")
			{
				iRefreshData();
			}
			APIProvider::GetSystemAPI()->iProjectBasedAPI->iGetStateManager()->iExecuteEvent(tEvent);

			m_queueWebCommand.pop();
		}
	}

	{
		BCAutoLock lock2(&m_MutexSubcribeLock);
		while (!m_queueSubscribeData.empty())
		{
			std::string* pData = m_queueSubscribeData.front();
			m_pSceneElementLoader->ParseSubscribeData(pData);
			m_queueSubscribeData.pop();
		}
	}

	if (m_pSceneElementLoader)
	{
		m_pSceneElementLoader->FrameUpdate();
	}
	if (m_pLabelRegulator)
	{
		m_pLabelRegulator->FrameUpdate();
	}

	if (m_pWebLabelParser)
	{
		m_pWebLabelParser->iFrameUpdate();
	}

	if (m_pHeatMapFactory)
	{
		m_pHeatMapFactory->FrameUpdate();
	}

	if (m_pBuildingIndoorFactory)
	{
		m_pBuildingIndoorFactory->FrameUpdate();
	}

	//判断相机位置
	sendCameraViewport(m_strListenCameraViewportWebId, false);
	sendListenViewShedState();
	m_pWebCommonApi->iFrameUpdate();

	HeightTifReader::GetInstance()->FrameUpdate();

	{
		int nMaxCount = 30;
		int nCount = 0;
		for (std::vector<std::string>::iterator it = m_vecNeedRefreshFontBoard.begin(); it != m_vecNeedRefreshFontBoard.end();)
		{
			INode* pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID((*it));
			m_vecNeedRefreshFontBoard.erase(it);
			if (!pNode || !pNode->GetDynamicComponent<IBoard>())
			{
				continue;
			}
			pNode->GetDynamicComponent<IBoard>()->iUpdateText();

			nCount++;
			if (nCount > nMaxCount)
			{
				break;
			}
		}
	}
}

void ProjectBasedClient::iPostFrameUpdate()
{
	for (size_t i = 0; i < m_vecStatisticsTool.size(); i++)
	{
		if (m_vecStatisticsTool[i] && m_vecStatisticsTool[i]->IsCanStatistics())
		{
			m_vecStatisticsTool[i]->StartStatistics();
		}
	}
	m_pWebCommonApi->iPostFrameUpdate();
}

void ProjectBasedClient::iPostSceneLoad()
{
	ProjectLogger::GetInstance()->SystemMessage("ProjectBasedClient iPostSceneLoad");
	ToSendWebCommond("VS_ShowAppPage", "", "");
}

EventReturnType_e ProjectBasedClient::iExecuteEvent(const BCEvent& tEvent)
{
	//接受Web的信息
	INT_PTR wParam = tEvent.nParam1;
	INT_PTR lParam = tEvent.nParam2;
	if (!wParam)
	{
		return EventReturnType_e::NONE;
	}

	switch (tEvent.eMessage)
	{
	case IOCTL_PROJECTBASED_PARSE_COMMAND:
	{
		parseCommand((const char*)wParam);
	}
	break;
	case IOCTL_PROJECTBASED_SUBSCRIBE_WEBSOCKET_MESSAGE:
	{
		char* pChar = (char*)wParam;
		BCAutoLock lock(&m_MutexSubcribeLock);
		std::string* pData = new std::string(pChar);
		m_queueSubscribeData.push(pData);

		iDoSubscribeWebsocketData((const char*)wParam);
	}
	break;
	}
	return EventReturnType_e::NONE;
}

EventReturnType_e ProjectBasedClient::iProcessEvent(const BCEvent& tEvent)
{
	if (tEvent.eMessage == EVENT_MOUSE)
	{
		int nX = tEvent.nParam2;
		int nY = tEvent.nParam3;
		int nScreenWidth = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nSceneWidth;
		int nScreenHeight = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nSceneWidth;
		if ((nX < 0 || nX > nScreenWidth) || (nY < 0 || nY > nScreenHeight))
		{
			return EventReturnType_e::NONE;
		}
		switch (tEvent.nParam1)
		{
		case MOUSE_RBUTTONUP:
		{
			INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
			IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));
			sendListenMouseRB(0, pManipulator->iGetMousePoint());
		}
		break;
		case MOUSE_RBUTTONDOWN:
		{
			INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
			IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));
			sendListenMouseRB(1, pManipulator->iGetMousePoint());
		}
		break;
		case MOUSE_MOVE:
		{
			INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
			IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));
			sendListenMouseMovePos(pManipulator->iGetMousePoint());
		}
		break;
		case MOUSE_LBUTTONDOWN:
		{
			INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
			ICamera* pCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
			IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));
			m_vLastMouseDownPos = pManipulator->iGetMousePoint();

			sendListenMouseLB(1, m_vLastMouseDownPos);
		}
		break;
		case MOUSE_LBUTTONUP:
		{
			//发送鼠标点击的坐标
			INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
			ICamera* pCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
			char szPos[LENGTH_1K] = { 0 };

			IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));
			Vector3d vPos = pManipulator->iGetMousePoint();
			if (m_vLastMouseDownPos.Distance(vPos) <= 2.0f)
			{
				m_vCurMouseDownPos = m_vLastMouseDownPos;
				//if (m_bListenMouseClickPos)
				{
					//vPos = m_vCurMouseDownPos;
					//vPos.z += 0.1f;
					sprintf_s(szPos, sizeof(szPos), "%lf, %lf, %lf", vPos.x, vPos.y, vPos.z);

					double dLng, dLat;
					char szWgs84[LENGTH_1K] = { 0 };
					ScenePosToWGS(vPos, dLng, dLat);
					sprintf_s(szWgs84, sizeof(szWgs84), "%.9f, %.9f,%.9f", dLng, dLat, vPos.z);

					std::string str = "";
					str += "'" + std::string(szPos) + "'";
					str += ",";
					str += "'" + std::string(szWgs84) + "'";
					ToSendWebCommond("VS_MouseClickScenePos", m_strListenMouseClickPosWebId, str);
				}

				iMouseClick(m_vCurMouseDownPos);
				{
					bc::BCEvent _tEvent(PROJECTBASED_EVENT_MOUSE_CLICK, (EVENT_PARAM)(&m_vCurMouseDownPos), 0, 0);
					APIProvider::GetSystemAPI()->iProjectBasedAPI->iGetStateManager()->iExecuteEvent(_tEvent);
				}
			}

			sendListenMouseLB(0, vPos);
		}
		break;
		case MOUSE_LBUTTONDBLCLK:
		{
			//发送鼠标点击的坐标
			INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
			ICamera* pCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
			char szPos[LENGTH_1K] = { 0 };

			IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));
			Vector3d vPos = pManipulator->iGetMousePoint();
			if (!m_strListenMouseLDBWebId.empty())
			{
				sprintf_s(szPos, sizeof(szPos), "%lf, %lf, %lf", vPos.x, vPos.y, vPos.z);

				double dLng, dLat;
				char szWgs84[LENGTH_1K] = { 0 };
				ScenePosToWGS(vPos, dLng, dLat);
				sprintf_s(szWgs84, sizeof(szWgs84), "%.9f, %.9f,%.9f", dLng, dLat, vPos.z);

				std::string str = "";
				str += "'" + std::string(szPos) + "'";
				str += ",";
				str += "'" + std::string(szWgs84) + "'";
				ToSendWebCommond("VS_ListenMouseLDB", m_strListenMouseLDBWebId, str);
			}
		}
		break;
		case MOUSE_WHEEL:
		{
			//三维不提供鼠标中键值 默认1.0f
			float fValue = 1.0f;
			sendListenMouseWheel(fValue);
		}
		break;
		}
	}
	else if (tEvent.eMessage == PROJECTBASED_EVENT_ONLY_LAYER_LOADED)
	{
		iOnLayerLoaded();
	}

	if (m_pSceneElementLoader)
	{
		m_pSceneElementLoader->iProcessEvent(tEvent);
	}

	if (m_pWebLabelParser)
	{
		m_pWebLabelParser->iProcessEvent(tEvent);
	}

	if (m_pCircleStatisticsTool)
	{
		m_pCircleStatisticsTool->iProcessEvent(tEvent);
	}
	if (m_pPOICircleStatisticsTool)
	{
		m_pPOICircleStatisticsTool->iProcessEvent(tEvent);
	}
	if (m_pLineStatisticsTool)
	{
		m_pLineStatisticsTool->iProcessEvent(tEvent);
	}
	if (m_pPolygonStatisticsTool)
	{
		m_pPolygonStatisticsTool->iProcessEvent(tEvent);
	}
	if (m_pPolygonStatisticsRectangularTool)
	{
		m_pPolygonStatisticsRectangularTool->iProcessEvent(tEvent);
	}
	if (m_pAreaMeasureTool)
	{
		m_pAreaMeasureTool->iProcessEvent(tEvent);
	}
	if (m_pDistanceMeasureTool)
	{
		m_pDistanceMeasureTool->iProcessEvent(tEvent);
	}
	m_pViewshedAnalysisTool->iProcessEvent(tEvent);
	if (m_pBuildingIndoorFactory)
	{
		m_pBuildingIndoorFactory->iProcessEvent(tEvent);
	}
	if (m_pWebCommonApi)
	{
		m_pWebCommonApi->iProcessEvent(tEvent);
	}
	if (m_pSightAnalysisTool)
	{
		m_pSightAnalysisTool->iProcessEvent(tEvent);
	}
	if (m_pLabelRegulator)
	{
		m_pLabelRegulator->iProcessEvent(tEvent);
	}
	return EventReturnType_e::NONE;
}

std::string ProjectBasedClient::iPreLoadWebPage(const std::string& strOriUrl, const std::string& strConcatUrl)
{
	std::string strWebUrl = IProjectClientAPI::iPreLoadWebPage(strOriUrl, strConcatUrl);
	ProjectLogger::GetInstance()->SystemMessage("ProjectBasedClient WebUrl--" + strWebUrl);
	m_strConcatWebUrl = strWebUrl;
	return strWebUrl;
}

///////////////////////////////////////////////////////////////////////////////////////////
void ProjectBasedClient::parseCommand(const char* strJsonData)
{
	if (strJsonData == NULL)
	{
		return;
	}

	try
	{
		std::string strJson = strJsonData;
		Json::CharReaderBuilder jsonBuilder;
		std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
		Json::Value root;
		std::string jsError;
		std::vector<Json::Value> vecParamList;
		vecParamList.clear();
		if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
		{
			string strFunc = root["key"].asString();
			string strID = "";
			if (root.isMember("id"))
			{
				strID = root["id"].asString();
			}
			Json::Value jParamList = root["data"];
			if (jParamList.isArray() && jParamList.size() > 0)
			{
				int nParamNum = jParamList.size();
				for (int i = 0; i < nParamNum; i++)
				{
					vecParamList.push_back(jParamList[i]["param"]);
				}
			}

			bool bNext = true;
			if (strFunc == "Web_Loaded")
			{
				string m_strAppName = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strToolProjectName;
				{
					// 网页加载完成，发送项目名称给网页
					std::string strData = "'" + m_strAppName + "'";
					ToSendWebCommond("VS_SetProjectName", "", strData);
				}
				//{
				//	//告知网页真实宽高
				//	std::string strData = to_string(APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nOutputWidth);
				//	strData += ",";
				//	strData += to_string(APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nOutputHeight);
				//	ToSendWebCommond("VS_AppWindowSize", "", strData);
				//}

				{
					bc::BCEvent _tEvent(PROJECTBASED_EVENT_WEB_LOADED, 0, 0, 0);
					APIProvider::GetSystemAPI()->iEngineAPI->iExecuteEvent(_tEvent);
				}
			}
			else if (strFunc == "Web_ReplacePicture")
			{
				if (vecParamList.size() > 0)
				{
					string strUrl = vecParamList[0].asString();
					string strLocalUrl = ParamNetImage::GetInstance()->CheckLocalPath(strUrl);
					remove(strLocalUrl.c_str());
				}
				bNext = false;
			}
			else if (strFunc == "Web_InitScene")
			{
				if (APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iIsSceneLoad())
				{
					std::string strData = "";
					ToSendWebCommond("VS_ShowAppPage", "", strData);
				}
				bNext = false;
			}
			else if (strFunc == "Web_LogToVs")
			{
				std::string strMessage;
				if (vecParamList[0].isInt())
				{
					strMessage = to_string(vecParamList[0].isInt());
				}
				else if (vecParamList[0].isString())
				{
					strMessage = vecParamList[0].asString();
				}
				else if (vecParamList[0].isArray() || vecParamList[0].isObject())
				{
					strMessage = vecParamList[0].toStyledString();
				}
				ProjectLogger::GetInstance()->WebMessage(strMessage);
				bNext = false;
			}
			else if (strFunc == "LoginCloud")
			{
				//20231226 对外JS本地端也要登录校验
				std::string strUseName = vecParamList[0].asString();
				std::string strPassword = vecParamList[1].asString();
				std::string strAppKey = vecParamList[2].asString();
				bool bNeedLogin = true;
				if (m_strConcatWebUrl.find("ignoreLogin=1") != std::string::npos)
				{
					bNeedLogin = false;
				}
				if (bNeedLogin)
				{
					MD5 tMD5Password(strPassword.c_str());
					tMD5Password.GenerateMD5((unsigned char*)strPassword.c_str(), strPassword.length());
					strPassword = tMD5Password.ToString();

					char sz[1024] = { 0 };
					sprintf_s(sz, sizeof(sz), "%s/api/v10/login?appKey=%s&username=%s&password=%s&isThirdClient=0&isClient=1&confirmLogin=1",
						m_strApiHost.c_str(), strAppKey.c_str(), strUseName.c_str(), strPassword.c_str());

					ThreadWrapper::GetInstance()->StopNetTask(strID);
					ThreadRequestParam stParam;
					stParam.strUrl = sz;
					stParam.eRequestType = THREAD_REQUEST_API_GET;
					stParam.pThreadCompleteFunc = static_request_CloudLogin_callback;
					stParam.bRunCallbackInMainThread = false;
					stParam.strID = strID;
					AddHttpCommonParam(&stParam);
					stParam.AddParam("this", (INT_PTR)this);
					stParam.AddExtraJValue("web_commond_id", strID);

					ThreadWrapper::GetInstance()->AddTask(stParam);
				}
				else
				{
					Json::Value jObject;
					jObject["code"] = 0;
					jObject["token"] = "NoNeedLogin";
					jObject["msg"] = "NoNeedLogin";

					ToSendWebCommond("VS_CloudLogin", strID, jObject.toStyledString());
					//登录成功,启动三维

					if (APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iIsSceneLoad())
					{
						std::string strData = "";
						ToSendWebCommond("VS_ShowAppPage", "", strData);
					}
					else
					{
						std::string strQtVSCommond = "start_game";
						iExecuteQtVsCommond(strQtVSCommond, nullptr, nullptr);
					}
				}

				bNext = false;
			}

			if (bNext)
			{
				WebCommandData sData;
				sData.strCommand = strFunc;
				sData.vecValues = vecParamList;
				sData.strID = strID;

				{
					BCAutoLock lock(&m_MutexLock);
					m_queueWebCommand.push(sData);
				}
			}
		}
	}
	catch (exception& e)
	{
	}
}

void ProjectBasedClient::getConfigInfoFromNet()
{
	int nWebPort = DEFAULT_REMOTERENDER_PORT;

	char szUrl[LENGTH_1K] = { 0 };
	char* pResponse = nullptr;
	do
	{
		EServerServion eType = APIProvider::GetSystemAPI()->iProtocolAPI->iGetServerVersion();
		sprintf_s(szUrl, sizeof(szUrl), "%s/api/v10/getProjectConfig?projectName=%s",
			APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strServerIp.c_str(),
			APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strApplicationName.c_str());
		ProjectLogger::GetInstance()->NetworkMessage(szUrl);
		string strResponse;
		HttpParam tHttpParam;
		tHttpParam.strID = "getConfigInfoFromNet";
		std::string strCertFile = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strCertFile;
		if (StartWith(strCertFile, APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strResourceDirName))
		{
			strCertFile = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir + "/" + strCertFile;
		}
		else if (!strCertFile.empty())
		{
			strCertFile = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strAppDir + "/" + strCertFile;
		}
		tHttpParam.strCert = strCertFile;
		IHttp* pHttp = APIProvider::GetSystemAPI()->iProtocolAPI->iGetNetworkManager()->iRegistHttp(tHttpParam);
		std::map<std::string, std::string> mapResponseHeader;
		int nRet = pHttp->iDownloadUrl(szUrl, strResponse, mapResponseHeader);
		if (nRet == STATUS_SUCCESS)
		{
			try
			{
				Json::CharReaderBuilder jsonBuilder;
				std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
				Json::Value root;
				std::string jsError;
				if (reader->parse(strResponse.c_str(), strResponse.c_str() + strResponse.length(), &root, &jsError))
				{
					Json::Value resultValue = Json::nullValue;
					if ((root["code"].isInt()) && (root["code"].asInt() == 0))
					{
						int nEncrypt = root["encrypt"].asInt();
						if (nEncrypt > 0)
						{
							std::string strResponse = root["data"].asString();
							int nDecryptSize = 0;
							APIProvider::GetSystemAPI()->iProtocolAPI->iGetNetworkManager()->iDecryptData(CLIENT_AES_KEY, strResponse.c_str(), strlen(strResponse.c_str()), pResponse, nDecryptSize);
							if (reader->parse(pResponse, pResponse + nDecryptSize, &root, &jsError))
							{
								resultValue = root;
							}
						}
						else
						{
							resultValue = root["data"];
						}

						if (!resultValue.isNull())
						{
							int nEnableRemote = 0;
							int nRemoteType = -1;
							//string strRtcServer = resultValue["rtc_server"].asString();
							//APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strWebRTCServerIP = strRtcServer;

							APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strProjectCustomConfig = resultValue["config"].asString();

							//string strCloudRender = resultValue["cloud_render"].asString();

							//if (strCloudRender.length() > 0)
							//{
							//	Json::Value jsonCloud;
							//	if (reader->parse(strCloudRender.c_str(), strCloudRender.c_str() + strCloudRender.length(), &jsonCloud, &jsError))
							//	{
							//		nEnableRemote = jsonCloud["enable"].asInt();
							//		nRemoteType = jsonCloud["encode_type"].asInt();

							//	}
							//}

							//APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender = nEnableRemote > 0 ? true : false;

							//bool bUseLocal = atoi(resultValue["force_use_local"].asCString()) > 0 ? true : false;
							//if (bUseLocal)
							//{
							//	//再加载一次本地,覆盖网络上的
							//	APIProvider::GetSystemAPI()->iEngineAPI->iLoadConfig();
							//}
							//if (resultValue.isMember("web_url") && APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strWebUrl.empty())
							//{
							//	APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strWebUrl =
							//		APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strServerIp + "/" + resultValue["web_url"].asString();
							//}
						}
					}
				}
			}
			catch (const std::exception&)
			{
			}
		}
		nRet = STATUS_ERROR;
	} while (0);

	if (pResponse)
	{
		delete[] pResponse;
		pResponse = nullptr;
	}
}

void ProjectBasedClient::iAddQtVsConnectFunc(QtVsConnectFunc fn)
{
	std::vector<QtVsConnectFunc>::iterator it = find(m_vecQtVsConnectFunc.begin(), m_vecQtVsConnectFunc.end(), fn);
	if (it == m_vecQtVsConnectFunc.end())
	{
		m_vecQtVsConnectFunc.push_back(fn);
	}
}

void ProjectBasedClient::iRemoveQtVsConnectFunc(QtVsConnectFunc fn)
{
	std::vector<QtVsConnectFunc>::iterator it = find(m_vecQtVsConnectFunc.begin(), m_vecQtVsConnectFunc.end(), fn);
	if (it != m_vecQtVsConnectFunc.end())
	{
		m_vecQtVsConnectFunc.erase(it);
	}
}

void ProjectBasedClient::iExecuteQtVsCommond(const std::string& strCommond, void* pInData, void* pOutData)
{
	for (size_t i = 0; i < m_vecQtVsConnectFunc.size(); i++)
	{
		m_vecQtVsConnectFunc[i](strCommond, m_mapExtraParam, pInData, pOutData);
	}
}

void ProjectBasedClient::iSetExtraParam(const std::string& strKey, INT_PTR pValue)
{
	m_mapExtraParam[strKey] = pValue;
}

INT_PTR ProjectBasedClient::iGetExtraParam(const std::string& strKey)
{
	std::map<std::string, INT_PTR>::iterator it = m_mapExtraParam.find(strKey);
	if (it != m_mapExtraParam.end())
	{
		return it->second;
	}
	return 0;
}

bool ProjectBasedClient::iCanHandleScene(const Vector2& vScreenPos)
{
	int nResult = 0;
	Vector2 vPos = vScreenPos;
	iExecuteQtVsCommond("iCheckWebCanHandleScene", &vPos, &nResult);
	return (nResult > 0);
}

bool ProjectBasedClient::iCanRespondKey()
{
	int nResult = 0;
	iExecuteQtVsCommond("iGetCanKeyEvent", nullptr, &nResult);
	return (nResult > 0);
}

void ProjectBasedClient::iSaveLog(ELogType eType, const std::string& strMsg)
{
	if (eType == LOG_INFO)
	{
		ProjectLogger::GetInstance()->InfoMessage(strMsg);
	}
	else if (eType == LOG_ERROR)
	{
		ProjectLogger::GetInstance()->ErrorMessage(strMsg);
	}
	else if (eType == LOG_DEBUG)
	{
		ProjectLogger::GetInstance()->DebugMessage(strMsg);
	}
	else if (eType == LOG_NETWORK)
	{
		ProjectLogger::GetInstance()->NetworkMessage(strMsg);
	}
	else if (eType == LOG_SYSTEM)
	{
		ProjectLogger::GetInstance()->SystemMessage(strMsg);
	}
	else if (eType == LOG_WEB)
	{
		ProjectLogger::GetInstance()->WebMessage(strMsg);
	}
	else if (eType == LOG_QT)
	{
		ProjectLogger::GetInstance()->QtMessage(strMsg);
	}
	else if (eType == LOG_CLOUD)
	{
		ProjectLogger::GetInstance()->CloudMessage(strMsg);
	}
	else
	{
		ProjectLogger::GetInstance()->InfoMessage(strMsg);
	}
	}

void ProjectBasedClient::iRefreshData()
{
	ProjectLogger::GetInstance()->SystemMessage("ProjectBasedClient iRefreshData");
	iReset();
}

bool ProjectBasedClient::iSetUdpParam(UdpParam* pUdpParam)
{
	return true;
}

void ProjectBasedClient::CB_DataCome(INT_PTR wParam, INT_PTR lParam)
{
	int nRet = STATUS_SUCCESS;
	int nCommandType = 0;
	ProjectBasedClient* pThis = (ProjectBasedClient*)lParam;

#ifndef LOCAL_CLIENT
	IPacketBuffer* pPacket = (IPacketBuffer*)wParam;
#else
	KPacketBuffer* pPacket = (KPacketBuffer*)wParam;
#endif

	if (!pPacket)
	{
		return;
	}

	pThis->m_pNetCommand->iSetPointer(pPacket->iGetData(), pPacket->iGetDataLength());
	nCommandType = pThis->m_pNetCommand->iGetCommand();

	char* pData = pThis->m_pNetCommand->iGetData();
	pThis->iDoUdpData(nCommandType, pData);
	std::string str = to_string(nCommandType);
	ProjectLogger::GetInstance()->UdpMessage(str, string(pData), 60 * 1000);

	BCEvent tEvent(PROJECTBASED_EVENT_UDP_DATA, (EVENT_PARAM)nCommandType, (EVENT_PARAM)pData);
	APIProvider::GetSystemAPI()->iProjectBasedAPI->iGetStateManager()->iExecuteEvent(tEvent);
}

void ProjectBasedClient::RequestHeartbeat()
{
	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_API_GET;
	stParam.AddParam("this", (INT_PTR)this);
	stParam.pPreDoRequestFunc = static_pre_request_heartbeat;
	stParam.pThreadCompleteFunc = static_request_heartbeat_callback;
	stParam.strID = "requestHeartbeat";
	stParam.bRepeat = true;
	stParam.bThread = true;
	int nTime = 2 * 60 * 1000;		//2分钟请求
	stParam.lInterval = nTime;		//4分钟
	stParam.bRunCallbackInMainThread = false;
	AddHttpCommonParam(&stParam);
	ThreadWrapper::GetInstance()->AddTask(stParam);
}

void ProjectBasedClient::static_pre_request_heartbeat(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam)
{
	ProjectBasedClient* pThis = (ProjectBasedClient*)mapParams["this"];
	std::vector<MacInfo> vecMac;
	std::string strMac = GetComputeMacAddress(vecMac);
	std::string strPcName = GetComputeName();
	char szBuffer[1024] = { 0 };
	sprintf_s(szBuffer, sizeof(szBuffer), "%s?mac=%s&pcName=%s&port=%d", pThis->m_strHeartbeatUrl.c_str(),
		strMac.c_str(),
		strPcName.c_str(),
		APIProvider::GetSystemAPI()->iProtocolAPI->iGetCloudRenderManager()->iGetCloudParam()->nWebPort
	);
	std::string strUrl = szBuffer;
	pParam->sRequestParam.strUrl = strUrl;
}

void ProjectBasedClient::static_request_heartbeat_callback(ThreadCallbackParam_s* pParam)
{
	ProjectBasedClient* pThis = (ProjectBasedClient*)pParam->sRequestParam.mapParams["this"];
	bool bAccept = false;
	if (pParam->nResult == STATUS_SUCCESS)
	{
		try
		{
			if (pParam->jRoot.isMember("code") && pParam->jRoot["code"] == 0)
			{
				if (pParam->jRoot.isMember("data") && pParam->jRoot["data"] == 1)
				{
					bAccept = true;
				}
			}
		}
		catch (const std::exception&)
		{
		}
	}
	if (!bAccept && !pThis->m_bKillProcess && pThis->m_bNeedKillProcess)
	{
		//杀掉进程
		pThis->m_bKillProcess = true;
		pThis->iExecuteQtVsCommond("heartbeat_reject", nullptr, nullptr);
	}
}

std::string ProjectBasedClient::GetVersion()
{
	return m_strVersion;
}

void ProjectBasedClient::iDoWebCommond(WebCommandData* pWebData)
{
	try
	{
		if (pWebData->strCommand == "Web_GetMouseClickPos" || pWebData->strCommand == "Web_ListenMouseClickPos")
		{
			if (pWebData->vecValues.size() > 0)
			{
				bool bEnable = pWebData->vecValues[0].asBool();
				if (!bEnable)
				{
					m_strListenMouseClickPosWebId = "";
				}
				else
				{
					m_strListenMouseClickPosWebId = pWebData->strID;
				}
			}
		}
		else if (pWebData->strCommand == "Web_ListenMouseMovePos")
		{
			if (pWebData->vecValues.size() > 0)
			{
				bool bEnable = pWebData->vecValues[0].asBool();
				if (!bEnable)
				{
					m_strListenMouseMovePosWebId = "";
				}
				else
				{
					m_strListenMouseMovePosWebId = pWebData->strID;
				}
			}
		}
		else if (pWebData->strCommand == "Web_ListenMouseWheel")
		{
			if (pWebData->vecValues.size() > 0)
			{
				bool bEnable = pWebData->vecValues[0].asBool();
				if (!bEnable)
				{
					m_strListenMouseWheelId = "";
				}
				else
				{
					m_strListenMouseWheelId = pWebData->strID;
				}
			}
		}
		else if (pWebData->strCommand == "Web_ListenMouseRB")
		{
			if (pWebData->vecValues.size() > 0)
			{
				bool bEnable = pWebData->vecValues[0].asBool();
				if (!bEnable)
				{
					m_strListenMouseRBWebId = "";
				}
				else
				{
					m_strListenMouseRBWebId = pWebData->strID;
				}
			}
		}
		else if (pWebData->strCommand == "Web_ListenMouseLB")
		{
			if (pWebData->vecValues.size() > 0)
			{
				bool bEnable = pWebData->vecValues[0].asBool();
				if (!bEnable)
				{
					m_strListenMouseLBWebId = "";
				}
				else
				{
					m_strListenMouseLBWebId = pWebData->strID;
				}
			}
		}
		else if (pWebData->strCommand == "Web_ListenMouseLDB")
		{
			if (pWebData->vecValues.size() > 0)
			{
				bool bEnable = pWebData->vecValues[0].asBool();
				if (!bEnable)
				{
					m_strListenMouseLDBWebId = "";
				}
				else
				{
					m_strListenMouseLDBWebId = pWebData->strID;
				}
			}
		}
		else if (pWebData->strCommand == "Web_ListenViewShedState")
		{
			if (pWebData->vecValues.size() > 0);;
			{
				bool bEnable = pWebData->vecValues[0].asBool();
				if (!bEnable)
				{
					m_strListenViewShedStateWebId = "";
				}
				else
				{
					m_strListenViewShedStateWebId = pWebData->strID;
				}
			}
		}
		else if (pWebData->strCommand == "Web_ListenCameraViewport")
		{
			if (pWebData->vecValues.size() > 0);
			{
				bool bEnable = pWebData->vecValues[0].asBool();
				if (!bEnable)
				{
					m_strListenCameraViewportWebId = "";
					m_vLastCameraViewport = Vector3(0, 0, 0);
					m_vLastCameraAngle = Vector3(0, 0, 0);
				}
				else
				{
					m_strListenCameraViewportWebId = pWebData->strID;
				}
			}
		}
		else if (pWebData->strCommand == "Web_GetCameraViewport")
		{
			sendCameraViewport(pWebData->strID, true);
		}
		else if (pWebData->strCommand == "Web_GetAllStoreyBuilding")
		{
			ToWebAllBuilding(pWebData->strID);
		}
		else if (pWebData->strCommand == "Web_GetAllLayer")
		{
			ToWebAllLayer(pWebData->strID);
		}
		else if (pWebData->strCommand == "Web_GetAllVideoInspection")
		{
			ToWebAllVideoInspection(pWebData->strID);
		}
		else if (pWebData->strCommand == "Web_GetAllRoamPath")
		{
			ToWebAllRoamPath(pWebData->strID);
		}
		else if (pWebData->strCommand == "slotWebKeyDown")
		{
			if (pWebData->vecValues.size() > 0)
			{
				int nCode = pWebData->vecValues[0].asInt();
				if (nCode == 120)
				{
					//F9
					m_bCameraInertialEnable = !m_bCameraInertialEnable;
					IManipulator* pManipulator = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->GetDynamicComponent<IManipulator>();
					pManipulator->iSetInertialEnable(m_bCameraInertialEnable);
				}
			}
		}
		else if (pWebData->strCommand == "Web_ListenElement")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bEnable = pWebData->vecValues[0].asBool();
				if (!bEnable)
				{
					m_strListenElementWebId = "";
					m_vecListenElement.clear();
				}
				else
				{
					m_strListenElementWebId = pWebData->strID;
				}
			}
		}
		else if (pWebData->strCommand == "Web_GetElevationPoint")
		{
			//制高点分析
			if (pWebData->vecValues.size() >= 5)
			{
				if (m_vecTifLevel.empty())
				{
					return;
				}
				m_strGetElevationPointWebId = pWebData->strID;

				float fCenterSceneX = pWebData->vecValues[0].asFloat();
				float fCenterSceneY = pWebData->vecValues[1].asFloat();
				float fRadius = pWebData->vecValues[2].asFloat();
				float fIgnoreRange = pWebData->vecValues[3].asFloat();
				int nCount = pWebData->vecValues[4].asInt();

				ThreadRequestParam stParam;
				stParam.eRequestType = THREAD_REQUEST_CUSTOM;
				stParam.AddParam("this", (INT_PTR)this);
				stParam.AddExtraJValue("fCenterSceneX", fCenterSceneX);
				stParam.AddExtraJValue("fCenterSceneY", fCenterSceneY);
				stParam.AddExtraJValue("fRadius", fRadius);
				stParam.AddExtraJValue("fIgnoreRange", fIgnoreRange);
				stParam.AddExtraJValue("nCount", nCount);
				stParam.pThreadCompleteFunc = nullptr;
				stParam.pThreadRuningFunc = static_request_getElevationPoint;
				stParam.strID = "GetElevationPoint";
				stParam.bRepeat = false;
				stParam.bThread = true;
				stParam.bRunCallbackInMainThread = false;
				AddHttpCommonParam(&stParam);
				ThreadWrapper::GetInstance()->AddTask(stParam);
			}
		}
		else if (pWebData->strCommand == "Web_StartSightAnalysisWithPos")
		{
			if (pWebData->vecValues.size() >= 5)
			{
				Vector3d vStart = m_pWebLabelParser->ParseVector3(pWebData->vecValues[0]);
				Vector3d vEnd = m_pWebLabelParser->ParseVector3(pWebData->vecValues[1]);
				Vector4 vVisibleColor = m_pWebLabelParser->ParseColor(pWebData->vecValues[2]);
				Vector4 vInVisibleColor = m_pWebLabelParser->ParseColor(pWebData->vecValues[3]);
				float fWidth = pWebData->vecValues[4].asFloat();

				if (m_pSightAnalysisTool)
				{
					m_pSightAnalysisTool->SetVisibleColor(vVisibleColor);
					m_pSightAnalysisTool->SetInVisibleColor(vInVisibleColor);
					m_pSightAnalysisTool->SetLineWidth(fWidth);
					m_pSightAnalysisTool->AddExtra("web_id", pWebData->strID);
					m_pSightAnalysisTool->Start(vStart, vEnd);
				}
			}
		}
		else if (pWebData->strCommand == "Web_StartSightAnalysis")
		{
			if (pWebData->vecValues.size() >= 3)
			{
				Vector4 vVisibleColor = m_pWebLabelParser->ParseColor(pWebData->vecValues[0]);
				Vector4 vInVisibleColor = m_pWebLabelParser->ParseColor(pWebData->vecValues[1]);
				float fWidth = pWebData->vecValues[2].asFloat();

				if (m_pSightAnalysisTool)
				{
					m_pSightAnalysisTool->SetVisibleColor(vVisibleColor);
					m_pSightAnalysisTool->SetInVisibleColor(vInVisibleColor);
					m_pSightAnalysisTool->SetLineWidth(fWidth);
					m_pSightAnalysisTool->AddExtra("web_id", pWebData->strID);
					m_pSightAnalysisTool->Start();
				}
			}
		}
		else if (pWebData->strCommand == "Web_StopSightAnalysis")
		{
			if (m_pSightAnalysisTool)
			{
				m_pSightAnalysisTool->Stop();
			}
		}
		else if (pWebData->strCommand == "Web_SwitchAppMode")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strAppMode = pWebData->vecValues[0].asString();
				std::vector<std::string> vec;
				SplitStringBySpecial(strAppMode, vec, ",");
				m_nAppMode = APP_MODE_NONE;
				if (find(vec.begin(), vec.end(), "ybss_edit") != vec.end())
				{
					m_nAppMode = m_nAppMode | APP_MODE_YBSS_EDIT;
				}
				if (find(vec.begin(), vec.end(), "exhibition") != vec.end())
				{
					m_nAppMode = m_nAppMode | APP_MODE_EXHIBITION;
				}
				if (find(vec.begin(), vec.end(), "plotting") != vec.end())
				{
					m_nAppMode = m_nAppMode | APP_MODE_PLOTTING;
				}
				if (find(vec.begin(), vec.end(), "video_fuse_edit") != vec.end())
				{
					m_nAppMode = m_nAppMode | APP_MODE_VIDEO_FUSE_EDIT;
				}
				if (find(vec.begin(), vec.end(), "route_edit") != vec.end())
				{
					m_nAppMode = m_nAppMode | APP_MODE_ROUTE_EDIT;
				}
				if (find(vec.begin(), vec.end(), "view_edit") != vec.end())
				{
					m_nAppMode = m_nAppMode | APP_MODE_VIEW_EDIT;
				}
				if (find(vec.begin(), vec.end(), "dynamic_edit") != vec.end())
				{
					m_nAppMode = m_nAppMode | APP_MODE_DYNAMIC_EDIT;
				}
				if (find(vec.begin(), vec.end(), "gis_earth") != vec.end())
				{
					m_nAppMode = m_nAppMode | APP_MODE_GIS_EARTH;
				}
				if (find(vec.begin(), vec.end(), "layer_edit") != vec.end())
				{
					//场景要素上图
					m_nAppMode = m_nAppMode | APP_MODE_LAYER_EDIT;
				}

				{
					BCEvent tEvent(PROJECTBASED_EVENT_APP_MODE_CHANGED);
					APIProvider::GetSystemAPI()->iProjectBasedAPI->iGetStateManager()->iExecuteEvent(tEvent);
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetAppId")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				std::string strAppId = pWebData->vecValues[0].asString();
				m_sUserInfo.strAppId = strAppId;
				BCEvent tEvent(PROJECTBASED_EVENT_APPID_CHANGED);
				APIProvider::GetSystemAPI()->iProjectBasedAPI->iGetStateManager()->iExecuteEvent(tEvent);
			}
		}
		else if (pWebData->strCommand == "Web_MouseButtonEnable")
		{
			int mouseButton = pWebData->vecValues[0].asInt();
			bool bEnable = pWebData->vecValues[1].asBool();
			INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
			IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));
			pManipulator->iSetMouseButtonEnable((MouseButton_e)mouseButton, bEnable);
		}
		else if (pWebData->strCommand == "Web_MouseButtonClickEnable")
		{
			int mouseButton = pWebData->vecValues[0].asInt();
			bool bEnable = pWebData->vecValues[1].asBool();
			INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
			IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));
			pManipulator->iSetMouseClickEnable((MouseButton_e)mouseButton, bEnable);
		}
		else if (pWebData->strCommand == "Web_ActionEnable")
		{
			int actionCode = pWebData->vecValues[0].asInt();
			bool bEnable = pWebData->vecValues[1].asBool();
			INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
			IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));
			pManipulator->iSetKeyActionEnable(actionCode, bEnable);
		}
		else if (pWebData->strCommand == "Web_SetProjectMouseClickEnable")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bEnable = pWebData->vecValues[0].asBool();
				std::vector<MouseButton_e> vecType;
				vecType.push_back(MouseButton_e::MOUSE_BUTTON_L);
				vecType.push_back(MouseButton_e::MOUSE_BUTTON_M);
				vecType.push_back(MouseButton_e::MOUSE_BUTTON_R);
				for (size_t i = 0; i < vecType.size(); ++i)
				{
					WebCommandData stWebData;
					stWebData.strCommand = "Web_MouseButtonClickEnable";
					stWebData.vecValues.push_back(vecType[i]);
					stWebData.vecValues.push_back(bEnable);

					iDoWebCommond(&stWebData);
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetProjectActionEnable")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bEnable = pWebData->vecValues[0].asBool();
				std::vector<KeyAction_e> vecType;
				vecType.push_back(ACTION_MOVE_FORWARD);
				vecType.push_back(ACTION_MOVE_BACK);
				vecType.push_back(ACTION_MOVE_RIGHT);
				vecType.push_back(ACTION_MOVE_LEFT);
				vecType.push_back(ACTION_APPLY_VIEWPOINT);
				vecType.push_back(ACTION_ROTATE_LEFT);
				vecType.push_back(ACTION_ROTATE_RIGHT);
				for (size_t i = 0; i < vecType.size(); ++i)
				{
					WebCommandData stWebData;
					stWebData.strCommand = "Web_ActionEnable";
					stWebData.vecValues.push_back(vecType[i]);
					stWebData.vecValues.push_back(bEnable);

					iDoWebCommond(&stWebData);
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetProjectMouseEnable")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bEnable = pWebData->vecValues[0].asBool();
				std::vector<MouseButton_e> vecType;
				vecType.push_back(MouseButton_e::MOUSE_BUTTON_L);
				vecType.push_back(MouseButton_e::MOUSE_BUTTON_M);
				vecType.push_back(MouseButton_e::MOUSE_BUTTON_R);
				for (size_t i = 0; i < vecType.size(); ++i)
				{
					WebCommandData stWebData;
					stWebData.strCommand = "Web_MouseButtonEnable";
					stWebData.vecValues.push_back(vecType[i]);
					stWebData.vecValues.push_back(bEnable);

					iDoWebCommond(&stWebData);
				}
			}
		}
		else if (pWebData->strCommand == "Web_SetProjectMouseAndActionEnable")
		{
			if (pWebData->vecValues.size() >= 1)
			{
				bool bEnable = pWebData->vecValues[0].asBool();
				{
					WebCommandData stWebData;
					stWebData.strCommand = "Web_SetProjectMouseEnable";
					stWebData.vecValues.push_back(bEnable);

					iDoWebCommond(&stWebData);
				}
				{
					WebCommandData stWebData;
					stWebData.strCommand = "Web_SetProjectActionEnable";
					stWebData.vecValues.push_back(bEnable);

					iDoWebCommond(&stWebData);
				}
			}
		}
	}
	catch (const std::exception&)
	{
	}

	m_pWebCommonApi->iDoWebCommond(pWebData);

	if (m_pWebLabelParser)
	{
		m_pWebLabelParser->iDoWebCommond(pWebData);
	}
	if (m_pSceneElementLoader)
	{
		m_pSceneElementLoader->iDoWebCommond(pWebData);
	}
	if (m_pLabelRegulator)
	{
		m_pLabelRegulator->iDoWebCommond(pWebData);
	}
}

void ProjectBasedClient::ConvertLngToVector3(const double dlng, const double dlat, Vector3d* vecPos, bool bUseTif)
{
	ConvertToVector3WithCoorSystem(dlng, dlat, vecPos, ElementCoordinateType_e::COOR_WGS84, bUseTif);
}

void ProjectBasedClient::ConvertToVector3WithCoorSystem(const double dlng, const double dlat, Vector3d* vecPos,
	EElementCoordinateType eType, bool bUseTif)
{
	float fZ = vecPos->z;
	double dX = dlng;
	double dY = dlat;
	bool bDoConvert = true;

	Vector3d vPos;
	if (eType == EElementCoordinateType::COOR_WGS84)
	{
		vPos.z = fZ;
	}
	else if (eType == EElementCoordinateType::COOR_GCJ02)
	{
		GCJ02_to_WGS(dlng, dlat, dX, dY);
	}
	else if (eType == EElementCoordinateType::COOR_BD09)
	{
		Vector3d vTemp;
		m_pMapAdapter->BD09_to_GCJ02(dlng, dlat, &vTemp);
		m_pMapAdapter->GCJ02_to_WGS(vTemp.x, vTemp.y, dX, dY);
	}
	else if (eType == EElementCoordinateType::COOR_SCENE_LOCAL)
	{
		vPos = Vector3d(dlng, dlat, fZ);
		bDoConvert = false;
	}
	else if (eType == EElementCoordinateType::COOR_WGS84_SELF)
	{
		vPos = ConvertLngLatFromSelfMap(dlng, dlat, fZ, false);
		bDoConvert = false;
	}
	else if (eType == EElementCoordinateType::COOR_CGCS2000)
	{
		//double dOutX, dOutY;
		//CGCS2000_to_WGS(dlng, dlat, dOutX, dOutY);
		//m_pMapAdapter->GetCoord_UTM(dOutX, dOutY, fZ, &vPos);

		vPos.z = fZ;
	}
	if (bDoConvert)
	{
		m_pMapAdapter->GetCoord_UTM(dX, dY, fZ, &vPos);
	}
	
	if (bUseTif)
	{
		fZ = GetZFromTif(vPos);
	}
	vPos.z = fZ;

	*vecPos = vPos;
}

bool ComHeight(Vector3d& f1, Vector3d& f2)
{
	return f1.z > f2.z;
}

float ProjectBasedClient::GetSceneZFromLngLat(double dLng, double dLat)
{
	Vector3d vPos;
	ConvertLngToVector3(dLng, dLat, &vPos, false);
	return GetZFromTif(vPos);
}

float ProjectBasedClient::GetZFromTif(Vector3d& vPos)
{
	std::vector<Vector3d> vecTemp;
	vecTemp.push_back(vPos);
	std::vector<double> vecHeight;
	GetZFromTif(vecTemp, vecHeight);
	if (vecHeight.size() > 0)
	{
		return vecHeight[0];
	}
	return -1.0f;
}

void ProjectBasedClient::GetZFromTif(std::vector<Vector3d>& vecPos, std::vector<double>& vecHeight)
{
	std::vector<Vector3d> vecCopyPos = vecPos;
	std::map<std::string, std::vector<Vector3d*>> mapTemp;
	for (size_t i = 0; i < vecCopyPos.size(); ++i)
	{
		if (m_vecTifLevel.size() == 1)
		{
			mapTemp[m_vecTifLevel[0].strFilePath].push_back(&vecCopyPos[i]);
		}
		else
		{
			for (size_t j = 0; j < m_vecTifLevel.size(); ++j)
			{
				Vector2 vPos(vecCopyPos[i].x, vecCopyPos[i].y);
				if (m_vecTifLevel[j].IsMatch(vPos))
				{
					mapTemp[m_vecTifLevel[j].strFilePath].push_back(&vecCopyPos[i]);
					break;
				}
			}
		}
		vecCopyPos[i].z = -1.0f;
	}


	for (std::map<std::string, std::vector<Vector3d*>>::iterator it = mapTemp.begin(); it != mapTemp.end(); ++it)
	{
		std::string str = it->first;
		if (IsFileExist(str))
		{
			std::vector<Vector3d> vec;
			for (size_t i = 0; i < it->second.size(); ++i)
			{
				vec.push_back(*(it->second[i]));
			}
			
			for (size_t i = 0; i < vec.size(); ++i)
			{
				it->second[i]->z = vecHeight[i];
			}
		}
	}

	//最终给值
	vecHeight.clear();
	for (size_t i = 0; i < vecCopyPos.size(); ++i)
	{
		vecHeight.push_back(vecCopyPos[i].z);
	}
}

void ProjectBasedClient::AdjustZ(Vector3d& vPos, float fZ, bool bAdd)
{
	if (bAdd)
	{
		vPos.z += fZ;
	}
	else
	{
		vPos.z = fZ;
	}
}

int ProjectBasedClient::LoadTif(vector<Vector3d>& vecHeight, double* dLeftTopCoord, double* dRightBtmCoord)
{
	std::string strFilePath = "";
	if (m_vecTifLevel.size() == 1)
	{
		strFilePath = m_vecTifLevel[0].strFilePath;
	}
	else
	{
		for (size_t j = 0; j < m_vecTifLevel.size(); ++j)
		{
			bool bMatch = false;
			Vector2 vPos(dLeftTopCoord[0], dLeftTopCoord[1]);
			bMatch = m_vecTifLevel[j].IsMatch(vPos);
			vPos = Vector2(dRightBtmCoord[0], dRightBtmCoord[1]);
			bMatch = bMatch && m_vecTifLevel[j].IsMatch(vPos);
			if (bMatch)
			{
				strFilePath = m_vecTifLevel[j].strFilePath;
				break;
			}
		}
	}

	return 0;
}

Vector3d ProjectBasedClient::ConvertLngLatFromSelfMap(double dLng, double dLat, double dZ, bool bUseTif)
{
	//场景地图渲染的坐标转换
	Vector3d vPos;
	vPos.x = double(dLng - m_vSelfLngLatLeftTop.x) / double(m_vSelfLngLatRightBottom.x - m_vSelfLngLatLeftTop.x) *
		double(m_vSelfScenePosRightBottom.x - m_vSelfScenePosLeftTop.x) + double(m_vSelfScenePosLeftTop.x);
	vPos.y = double(dLat - m_vSelfLngLatLeftTop.y) / double(m_vSelfLngLatRightBottom.y - m_vSelfLngLatLeftTop.y) *
		double(m_vSelfScenePosRightBottom.y - m_vSelfScenePosLeftTop.y) + double(m_vSelfScenePosLeftTop.y);
	vPos.z = dZ;
	if (bUseTif)
	{
		float fZ = GetZFromTif(vPos);
		AdjustZ(vPos, fZ, false);
	}
	return vPos;
}

void ProjectBasedClient::ScenePosToWGS(Vector3d& vPos, double& dlng, double& dlat)
{
	m_pMapAdapter->GetPointUTM_LngLa((Vector3d*)&vPos, m_nUTM_project_zone, dlng, dlat);
}

void ProjectBasedClient::WGS_TO_GCJ02(double dlng, double dlat, double& outX, double& outY)
{
	m_pMapAdapter->WGS_to_GCJ02(dlng, dlat, outX, outY);
}

void ProjectBasedClient::GCJ02_to_WGS(double dlng, double dlat, double& outX, double& outY)
{
	m_pMapAdapter->GCJ02_to_WGS(dlng, dlat, outX, outY);
}

void ProjectBasedClient::WGS_TO_CGCS2000(double dlng, double dlat, double& outX, double& outY)
{
	m_pMapAdapter->WGS_to_CGCS2000(dlng, dlat, m_fCGCS2000CentralMeridian, outX, outY);
}

void ProjectBasedClient::CGCS2000_to_WGS(double dlng, double dlat, double& outX, double& outY)
{
	m_pMapAdapter->CGCS2000_to_WGS(dlng, dlat, m_fCGCS2000CentralMeridian, outX, outY);
}

void ProjectBasedClient::ToSendWebCommond(std::string strFunName, std::string strWebId, std::string strRealData)
{
	std::string str = "";
	str += "VS_ToCommonWebCommandV2(";
	str += "'" + strFunName + "'";
	str += ",";
	str += "'" + strWebId + "'";
	if (strRealData.empty())
	{
		str += ")";
	}
	else
	{
		str += ",";
		str += strRealData;
		str += ")";
	}

	std::string strQtVSCommond = "send_web_commond";
	std::string strWebID = strWebId;
	iExecuteQtVsCommond(strQtVSCommond, &str, &strWebID);
}

void ProjectBasedClient::loadTifLevel(const std::string& strPath)
{
	if (IsDir(strPath))
	{
		std::vector<FileInfo> vecFiles;
		GetFilesFromDir(strPath, vecFiles, false, "json");
		if (vecFiles.size() == 0)
		{
			return;
		}
#ifdef _WIN32
		std::string strConfigPath = strPath + "/" + vecFiles[0].tInfo.name;
#else
		std::string strConfigPath = strPath + "/" + vecFiles[0].tInfo->d_name;
#endif

		Json::Value jResult;
		std::string strData = ReadFile(strConfigPath);
		ParseJsonByString(strData, jResult);
		if (jResult.isArray())
		{
			for (int i = 0; i < jResult.size(); ++i)
			{
				std::string strRange = jResult[i]["range"].asString();
				std::vector<std::string> vec;
				SplitStringBySpecial(strRange, vec, ",");
				if (vec.size() >= 4)
				{
					TifLevel stLevel;
					stLevel.strFilePath = jResult[i]["file"].asString();
					stLevel.strFilePath = strPath + "/" + stLevel.strFilePath;
					if (!IsFileExist(stLevel.strFilePath))
					{
						continue;
					}
					stLevel.vLeftTop.x = atof(vec[0].c_str());
					stLevel.vLeftTop.y = atof(vec[2].c_str());
					stLevel.vRightBottom.x = atof(vec[1].c_str());
					stLevel.vRightBottom.y = atof(vec[3].c_str());
					stLevel.fArea = abs(stLevel.vLeftTop.x - stLevel.vRightBottom.x) * abs(stLevel.vLeftTop.y - stLevel.vRightBottom.y);
					m_vecTifLevel.push_back(stLevel);
				}
			}
		}
	}
	else
	{
		TifLevel stLevel;
		stLevel.strFilePath = strPath;
		if (IsFileExist(stLevel.strFilePath))
		{
			m_vecTifLevel.push_back(stLevel);
		}
	}

	sort(m_vecTifLevel.begin(), m_vecTifLevel.end(), [&](const TifLevel& a, const TifLevel& b)->bool {
		return a.fArea < b.fArea;
		});
}

void ProjectBasedClient::AddHttpCommonParam(ThreadRequestParam* pRequestParam)
{
	pRequestParam->vecHttpCustomHeaderParam.push_back({ "Token",m_sUserInfo.strUserToken });
	pRequestParam->vecHttpCustomHeaderParam.push_back({ "H-App-Via",m_sUserInfo.strAppId });
}

void ProjectBasedClient::AddWebCommond(WebCommandData& stWebCommond, bool bNextFrame)
{
	if (!bNextFrame)
	{
		m_queueWebCommand.push(stWebCommond);
	}
	else
	{
		m_vecNextFrameWebComondData.push_back(stWebCommond);
	}
}

INode* ProjectBasedClient::iGetProjectGroupNode()
{
	return m_pProjectGroupNode;
}

void ProjectBasedClient::iReset()
{
	{
		//模拟网页发送删除所有api外部创建节点
		WebCommandData stWebData;
		stWebData.strCommand = "Web_DeleteAllElement";
		AddWebCommond(stWebData, false);
	}
	{
		{
			//隐藏室外图层
			WebCommandData stWebData;
			stWebData.strCommand = "Web_ShowLabelType";
			stWebData.vecValues.push_back("");
			AddWebCommond(stWebData, false);
		}
		{
			//隐藏室内图层
			WebCommandData stWebData;
			stWebData.strCommand = "Web_ShowIndoorLabelType";
			stWebData.vecValues.push_back("");
			AddWebCommond(stWebData, false);
		}
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_ListenMouseClickPos";
		stWebData.vecValues.push_back(false);
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_ListenMouseMovePos";
		stWebData.vecValues.push_back(false);
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_ListenMouseRB";
		stWebData.vecValues.push_back(false);
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_ListenElement";
		stWebData.vecValues.push_back(false);
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_SetProjectMouseClickEnable";
		stWebData.vecValues.push_back(true);
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_SetProjectMouseAndActionEnable";
		stWebData.vecValues.push_back(true);
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_SetSceneRotateEnable";
		stWebData.vecValues.push_back(true);
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_StopSightAnalysis";
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_StopRoam";
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_ShowHeatmapOnlyArea";
		stWebData.vecValues.push_back("");
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_CloseStatistics";
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_SetEnableAreaMeasure";
		stWebData.vecValues.push_back(false);
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_SetEnableDistanceMeasure";
		stWebData.vecValues.push_back(false);
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_StopViewshed";
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_ListenViewShedState";
		stWebData.vecValues.push_back(false);
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_LeavelIndoorBuilding";
		stWebData.vecValues.push_back(false);
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_StopVideoInspection";
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_ListenCameraViewport";
		stWebData.vecValues.push_back(false);
		AddWebCommond(stWebData, false);
	}
	{
		WebCommandData stWebData;
		stWebData.strCommand = "Web_SetIsolateEnable";
		stWebData.vecValues.push_back(false);
		AddWebCommond(stWebData, false);
	}
}

void ProjectBasedClient::iOnVideoInspectionLoadedData(Json::Value jRetData)
{
	std::string str = "";
	str += jRetData.toStyledString();
	ToSendWebCommond("VS_ToWebVideoInspection", "", str);
}

void ProjectBasedClient::iOnVideoInspectionToScenePos(const double& dLng, const double& dLat, const double& dZ, Vector3d& vOutPos, Vector3d& vOutAngle)
{
	ConvertLngToVector3(dLng, dLat, &vOutPos, false);
	AdjustZ(vOutPos, dZ, false);
}

HeatmapFactory* ProjectBasedClient::iCreateHeatMapFactory()
{
	return new DefaultHeatMapFactory(APIProvider::GetSystemAPI(), this);
}

DefaultVideoInspectionFactory* ProjectBasedClient::iCreateVideoInspectionFactory()
{
	return new DefaultVideoInspectionFactory(this);
}

BuildIndoorFactory* ProjectBasedClient::iCreateBuildingIndoorFactory()
{
	return new DefaultIndoorFactory(this);
}

void ProjectBasedClient::iOnBuildingDataLoadedData()
{
	ToWebAllBuilding("");
}

void ProjectBasedClient::ToWebAllBuilding(std::string strWebId)
{
	Json::Value jArray = Json::arrayValue;
	std::vector<BuildingData*> vecAllBuildingData;
	m_pBuildingIndoorFactory->GetAllBuildingData(vecAllBuildingData);
	char szName[1024] = { 0 };
	for (size_t i = 0; i < vecAllBuildingData.size(); i++)
	{
		Json::Value jBuilding = Json::objectValue;
		BuildingData* pBuildingData = vecAllBuildingData[i];
		AsciiToUtf8(pBuildingData->strName.c_str(), sizeof(szName), szName);
		//jBuilding["name"] = pBuildingData->strName;
		jBuilding["name"] = szName;
		jBuilding["node_id"] = pBuildingData->strBuildingNodeID;
		jBuilding["real_id"] = pBuildingData->pBuildingGroupData->strRealID;
		jBuilding["type"] = pBuildingData->pBuildingGroupData->nType;
		jBuilding["floor"] = Json::arrayValue;
		std::vector<FloorData*> vecFloor = pBuildingData->vecFloorData;
		sort(vecFloor.begin(), vecFloor.end(), static_compareFloorIndex);
		for (int i = 0; i < vecFloor.size(); i++)
		{
			jBuilding["floor"].append(dataToIndoorJsonValue(vecFloor[i]));
		}

		jArray.append(jBuilding);
	}

	std::string strData = jArray.toStyledString();
	ToSendWebCommond("VS_ToWebAllBuilding", strWebId, strData);
}

bool ProjectBasedClient::static_compareFloorIndex(const FloorData* t1, const FloorData* t2)
{
	return t1->nFloorIndex > t2->nFloorIndex;
}

Json::Value ProjectBasedClient::dataToIndoorJsonValue(FloorData* pFloorData)
{
	if (!pFloorData)
	{
		return NULL;
	}

	Json::Value jFloorData;

	jFloorData["FloorNodeId"] = pFloorData->strFloorNodeID;
	jFloorData["FloorIndex"] = pFloorData->nFloorIndex;
	jFloorData["FloorName"] = pFloorData->strFloorName;

	return jFloorData;
}

SceneElementLoader* ProjectBasedClient::iCreateSceneElementLoader()
{
	return new DefaultProjectElementLoader(this);
}

void ProjectBasedClient::ToWebAllVideoInspection(std::string strWebId)
{
	std::string strData = m_pVideoInspectionFactory->GetJsonDataResult().toStyledString();
	ToSendWebCommond("VS_ToWebVideoInspection", strWebId, strData);
}

void ProjectBasedClient::ToWebAllLayer(std::string strWebId)
{
	Json::Value jArray = Json::arrayValue;
	char szBuffer[1024] = { 0 };
	std::vector<ProjectBasedSceneElementTypeInfo*> vecSceneTypeInfo = m_pSceneElementLoader->GetAllSceneElementTypeInfo();
	for (size_t i = 0; i < vecSceneTypeInfo.size(); i++)
	{
		ProjectBasedSceneElementTypeInfo* pInfo = vecSceneTypeInfo[i];
		Json::Value jItem = Json::objectValue;
		//AsciiToUtf8(pInfo->strName.c_str(), sizeof(szBuffer), szBuffer);
		jItem["id"] = pInfo->nTypeID;
		jItem["name"] = pInfo->strName;
		jItem["indoor"] = pInfo->nIndoor;

		jArray.append(jItem);
	}

	std::string strData = jArray.toStyledString();
	ToSendWebCommond("VS_ToWebAllLayer", strWebId, strData);
}

void ProjectBasedClient::ToWebAllRoamPath(std::string strWebId)
{
	std::string strData = m_jWebRoamPath.toStyledString();
	ToSendWebCommond("VS_ToWebAllRoamPath", strWebId, strData);
}

void ProjectBasedClient::RequestSubscribeBindIp()
{
	char szBuffer[1024] = { 0 };
	sprintf_s(szBuffer, sizeof(szBuffer), "%s/api/v10/bindUser?port=%d&userToken=%s", m_strApiHost.c_str(),
		APIProvider::GetSystemAPI()->iProtocolAPI->iGetCloudRenderManager()->iGetCloudParam()->nWebPort,
		m_sUserInfo.strUserToken.c_str());

	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_API_GET;
	stParam.AddParam("this", (INT_PTR)this);
	stParam.strID = "requestSubscribeBindIp";
	stParam.strUrl = szBuffer;
	stParam.bRepeat = false;
	stParam.bThread = true;
	stParam.bRunCallbackInMainThread = false;
	AddHttpCommonParam(&stParam);
	ThreadWrapper::GetInstance()->AddTask(stParam);
}

void	ProjectBasedClient::DoOnSceneElementLoadedData(bool bAllComplete)
{
	iOnSceneElementLoadedData(bAllComplete);

	bool bEnableUdp = iSetUdpParam(&m_stUdpParam);
	if (bEnableUdp && !m_pNetCommand)
	{
		//UDP连接
		m_pNetCommand = APIProvider::GetSystemAPI()->iProtocolAPI->iGetNetworkManager()->iCreateNetworkCommand();

		SocketConnectorParam stParam;
		if (m_stUdpParam.nClientPort == -1)
		{
			if (APIProvider::GetSystemAPI()->iProtocolAPI->iGetServerVersion() >= SERVER_VERSION_V10)
			{
				m_stUdpParam.nClientPort = APIProvider::GetSystemAPI()->iProtocolAPI->iGetCloudRenderManager()->iGetCloudParam()->nWebPort + 1 + 4;
			}
		}
		if (m_stUdpParam.nClientPort != -1)
		{
			stParam.nDataClientPort = m_stUdpParam.nClientPort;
		}
		if (!m_stUdpParam.pReceiveFunc)
		{
			m_stUdpParam.pReceiveFunc = CB_DataCome;
		}
		std::string strMsg = "udp port=" + to_string(m_stUdpParam.nClientPort);
		ProjectLogger::GetInstance()->InfoMessage(strMsg);
		stParam.strHeartBeatFindIP = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.strServerIp;
		stParam.pReceiveCallback = m_stUdpParam.pReceiveFunc;
		stParam.pReceiveCallbackUser = this;
		m_pConnector = APIProvider::GetSystemAPI()->iProtocolAPI->iGetNetworkManager()->iRegistSocketConnector(stParam);
		m_pConnector->iListen();
	}
}

void ProjectBasedClient::requestRoamPath()
{
	m_jWebRoamPath.clear();
	m_jWebRoamPath = Json::arrayValue;

	std::string strID = "requestRoamPath";

	ThreadCallbackParam stCallback;
	ThreadRequestParam stParam;
	stParam.strID = "requestRoamPath";
	stParam.eRequestType = THREAD_REQUEST_API_GET;
	stParam.strUrl = m_strRoamUrl;
	AddHttpCommonParam(&stParam);
	ThreadWrapper::GetInstance()->DoNetRequest(stParam, &stCallback);
	if (stCallback.nResult == STATUS_SUCCESS)
	{
		try
		{
			std::vector<Json::Value> vecParamList;
			vecParamList.clear();
			Json::CharReaderBuilder jsonBuilder;
			std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
			Json::Value root;
			std::string jsError;
			if (reader->parse(stCallback.strResponse.c_str(), stCallback.strResponse.c_str() + strlen(stCallback.strResponse.c_str()), &root, &jsError))
			{
				Json::Value child = root["data"];
				for (int i = 0; i < child.size(); i++)
				{
					char szName[512] = { 0 };
					string strRoamName = child[i]["name"].asCString();
					Utf8ToAscii(strRoamName.c_str(), sizeof(szName), szName);
					int nID = child[i]["roamid"].asInt();
					Json::Value jItem;
					jItem["id"] = nID;
					jItem["name"] = strRoamName;
					m_jWebRoamPath.append(jItem);

					std::vector<Vector3> vecPoints;
					std::vector<Quaternion> vecQua;
					Json::Value vPath = child[i]["children"];
					if (vPath.isArray())
					{
						for (Json::Value vjPos : vPath)
						{
							vector<string> v;
							string strPos = vjPos["point"].asCString();
							SplitStringBySpecial(strPos, v, ",");
							if (v.size() == 3)
							{
								vecPoints.push_back(Vector3(atof(v[0].c_str()), atof(v[1].c_str()), atof(v[2].c_str())));
							}

							v.clear();
							string strAngle = vjPos["quaternion"].asCString();
							SplitStringBySpecial(strAngle, v, ",");
							if (v.size() == 4)
							{
								vecQua.push_back(Quaternion(atof(v[3].c_str()), atof(v[0].c_str()), atof(v[1].c_str()), atof(v[2].c_str())));
							}
						}
					}
					//if (vecPoints.size() > 0 && vecQua.size() > 0)
					{
						INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
						ICamera* pCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
						pCamera->iAddAnimationPath(szName, vecPoints, vecQua);
					}
				}
			}
		}
		catch (const std::exception&)
		{
		}
	}
}

CircleStatisticsTool* ProjectBasedClient::iCreateCircleStatisticsTool()
{
	PolygonParam stPolyGonParam;
	stPolyGonParam.vBaseColor = Vector4(24 / 255.0f, 255 / 255.0f, 253 / 255.0f, 0.4);
	stPolyGonParam.vLineColor = Vector4(24 / 255.0f, 255 / 255.0f, 253 / 255.0f, 1);
	stPolyGonParam.fLineWidth = 6.0f;
	stPolyGonParam.bDepthTest = false;
	stPolyGonParam.bWithLine = true;
	stPolyGonParam.bCalculateCenter = true;

	CircleStatisticsTool* pCircleStatisticsTool = new CircleStatisticsTool();
	pCircleStatisticsTool->SetStyleParam(stPolyGonParam);

	return pCircleStatisticsTool;
}

CircleStatisticsTool* ProjectBasedClient::iCreatePOICircleStatisticsTool()
{
	PolygonParam stPolyGonParam;
	stPolyGonParam.vBaseColor = Vector4(24 / 255.0f, 255 / 255.0f, 253 / 255.0f, 0.4);
	stPolyGonParam.vLineColor = Vector4(24 / 255.0f, 255 / 255.0f, 253 / 255.0f, 1);
	stPolyGonParam.fLineWidth = 6.0f;
	stPolyGonParam.bDepthTest = false;
	stPolyGonParam.bWithLine = true;
	stPolyGonParam.bCalculateCenter = true;

	CircleStatisticsTool* pCircleStatisticsTool = new CircleStatisticsTool(true);
	pCircleStatisticsTool->SetStyleParam(stPolyGonParam);

	return pCircleStatisticsTool;
}

LineStatisticsTool* ProjectBasedClient::iCreateLineStatisticsTool()
{
	RoadParam stParam;
	stParam.bAnimation = false;
	stParam.strImage = "resource/extra/statistic_line_bg.png";
	stParam.bDepthTest = false;
	stParam.nCalcHeightType = RoadParam::CalcHeightType::ORIGINAL_HEIGHT;

	LocusLineSegment sSegment;
	sSegment.fWidth = 6.0f;
	sSegment.vColor = Vector4(24 / 255.0f, 255 / 255.0f, 253 / 255.0f, 1);

	LineStatisticsTool* pLineStatisticsTool = new LineStatisticsTool();
	pLineStatisticsTool->SetStyleParam(stParam);
	pLineStatisticsTool->SetLineParam(sSegment);

	BoardParam stPointParam;
	stPointParam.strImage = "resource/extra/statistic_point.png";
	stPointParam.fHeight = 24;
	stPointParam.fWidth = 24;
	stPointParam.bDynamicScaleEnable = true;
	stPointParam.fDynamicScale = 0.001;
	stPointParam.fDynamicMaxScale = 0;
	stPointParam.fDynamicMinScale = 0;
	stPointParam.direction = BoardDirect_e::FaceToCamera;
	stPointParam.bDepthTest = false;
	pLineStatisticsTool->SetPointParam(stPointParam);

	return pLineStatisticsTool;
}

PolygonStatisticsTool* ProjectBasedClient::iCreatePolygonStatisticsTool()
{
	PolygonParam stPolyGonParam;
	stPolyGonParam.vBaseColor = Vector4(24 / 255.0f, 255 / 255.0f, 253 / 255.0f, 0.4);
	stPolyGonParam.vLineColor = Vector4(24 / 255.0f, 255 / 255.0f, 253 / 255.0f, 1);
	stPolyGonParam.fLineWidth = 6.0f;
	stPolyGonParam.bDepthTest = false;
	stPolyGonParam.bWithLine = true;
	stPolyGonParam.bCalculateCenter = true;

	BoardParam stPointParam;
	stPointParam.strImage = "resource/extra/statistic_point.png";
	stPointParam.fHeight = 24;
	stPointParam.fWidth = 24;
	stPointParam.bDynamicScaleEnable = true;
	stPointParam.fDynamicScale = 0.001;
	stPointParam.fDynamicMaxScale = 0;
	stPointParam.fDynamicMinScale = 0;
	stPointParam.direction = BoardDirect_e::FaceToCamera;
	stPointParam.bDepthTest = false;

	PolygonStatisticsTool* pPolygonStatisticsTool = new PolygonStatisticsTool();
	pPolygonStatisticsTool->SetStyleParam(stPolyGonParam);
	pPolygonStatisticsTool->SetPointParam(stPointParam);

	return pPolygonStatisticsTool;
}

PolygonStatisticsTool* ProjectBasedClient::iCreatePolygonStatisticsRectangularTool()
{
	PolygonParam stPolyGonParam;
	stPolyGonParam.vBaseColor = Vector4(24 / 255.0f, 255 / 255.0f, 253 / 255.0f, 0.4);
	stPolyGonParam.vLineColor = Vector4(24 / 255.0f, 255 / 255.0f, 253 / 255.0f, 1);
	stPolyGonParam.fLineWidth = 6.0f;
	stPolyGonParam.bDepthTest = false;
	stPolyGonParam.bWithLine = true;
	stPolyGonParam.bCalculateCenter = true;

	BoardParam stPointParam;
	stPointParam.strImage = "resource/extra/statistic_point.png";
	stPointParam.fHeight = 24;
	stPointParam.fWidth = 24;
	stPointParam.bDynamicScaleEnable = true;
	stPointParam.fDynamicScale = 0.001;
	stPointParam.fDynamicMaxScale = 0;
	stPointParam.fDynamicMinScale = 0;
	stPointParam.direction = BoardDirect_e::FaceToCamera;
	stPointParam.bDepthTest = false;

	PolygonStatisticsTool* pPolygonStatisticsTool = new PolygonStatisticsTool(PolygonStatisticsType::RECTANGULAR);
	pPolygonStatisticsTool->SetStyleParam(stPolyGonParam);
	pPolygonStatisticsTool->SetPointParam(stPointParam);

	return pPolygonStatisticsTool;
}

WebLabelParser* ProjectBasedClient::iCreateWebLabelParser()
{
	return new WebLabelParser();
}

SightAnalysisTool* ProjectBasedClient::iCreateSightAnalysisTool()
{
	return new SightAnalysisTool();
}

DistanceMeasureTool* ProjectBasedClient::iCreateDistanceMeasureTool()
{
	FontBoardParam tParam;
	tParam.strImage = "resource/extra/distance_font_bg.png";
	tParam.fHeight = 90;
	tParam.fWidth = 576;
	tParam.bDynamicScaleEnable = true;
	tParam.fDynamicScale = 0.00035f;
	tParam.fDynamicMaxScale = 0;
	tParam.fDynamicMinScale = 0;
	tParam.direction = BoardDirect_e::FaceToCamera;
	TextShow stTextShow;
	stTextShow.vColor = Vector4(255.0 / 255.0f, 250.0f / 255.0f, 255 / 255.0f);
	int nTextSize = 40;
	stTextShow.vTextSize = Vector2(nTextSize, nTextSize * 1.33f);
	stTextShow.vTextPos = Vector2(116.0f, 20.0f);
	stTextShow.strID = "value";
	stTextShow.bWidthCenter = false;
	stTextShow.bHeightCenter = false;
	stTextShow.bAdjustWidth = true;
	tParam.SetText(stTextShow);

	BoardParam stStartPointBoard;
	stStartPointBoard.strImage = "resource/extra/distance_linepoint_start.png";
	stStartPointBoard.fHeight = 24;
	stStartPointBoard.fWidth = 24;
	stStartPointBoard.bDynamicScaleEnable = true;
	stStartPointBoard.fDynamicScale = 0.0006;
	stStartPointBoard.fDynamicMaxScale = 0;
	stStartPointBoard.fDynamicMinScale = 0;
	stStartPointBoard.bGlowEnable = true;
	stStartPointBoard.fGlowIntensity = 10.0f;
	stStartPointBoard.direction = BoardDirect_e::FaceToCamera;

	DistanceMeasureTool* pDistanceMeasureTool = new DistanceMeasureTool();
	pDistanceMeasureTool->SetFontBoardParam(tParam, stTextShow.strID);
	pDistanceMeasureTool->SetStartPointBoardParam(stStartPointBoard);
	BoardParam stEndPointBoard;
	stEndPointBoard = stStartPointBoard;
	stEndPointBoard.strImage = "resource/extra/distance_linepoint_end.png";
	pDistanceMeasureTool->SetEndPointBoardParam(stEndPointBoard);

	LocusLineSegment sSegment;
	sSegment.vColor = Vector4(250.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f, 1);
	sSegment.fWidth = 6.0f;
	pDistanceMeasureTool->SetStyleParam(sSegment);

	return pDistanceMeasureTool;
}

AreaMeasureTool* ProjectBasedClient::iCreateAreaMeasureTool()
{
	PolygonParam stPolyGonParam;
	stPolyGonParam.vBaseColor = Vector4(217 / 255.0f, 71 / 255.0f, 255 / 255.0f, 0.4f);
	stPolyGonParam.vLineColor = Vector4(214 / 255.0f, 162 / 255.0f, 255 / 255.0f, 1.0f);
	stPolyGonParam.fLineWidth = 6.0f;
	stPolyGonParam.bDepthTest = false;
	stPolyGonParam.bWithLine = true;
	stPolyGonParam.bCalculateCenter = true;

	FontBoardParam tParam;
	tParam.strImage = "resource/extra/area_font_bg.png";
	tParam.fHeight = 90;
	tParam.fWidth = 578;
	tParam.bDynamicScaleEnable = true;
	tParam.fDynamicScale = 0.00035f;
	tParam.fDynamicMaxScale = 0;
	tParam.fDynamicMinScale = 0;
	tParam.direction = BoardDirect_e::FaceToCamera;
	tParam.bDepthTest = false;
	TextShow stTextShow;
	stTextShow.vColor = Vector4(255.0 / 255.0f, 250.0f / 255.0f, 255 / 255.0f);
	int nTextSize = 40;
	stTextShow.vTextSize = Vector2(nTextSize, nTextSize * 1.33f);
	stTextShow.vTextPos = Vector2(116.0f, 20.0f);
	stTextShow.strID = "value";
	stTextShow.bWidthCenter = false;
	stTextShow.bHeightCenter = false;
	stTextShow.bAdjustWidth = true;
	tParam.SetText(stTextShow);

	AreaMeasureTool* pAreaMeasureTool = new AreaMeasureTool();
	pAreaMeasureTool->SetStyleParam(stPolyGonParam);
	pAreaMeasureTool->SetFontBoardParam(tParam, stTextShow.strID);

	BoardParam stPointParam;
	stPointParam.strImage = "resource/extra/anniu_normal.png";
	stPointParam.fHeight = 24;
	stPointParam.fWidth = 24;
	stPointParam.bDynamicScaleEnable = true;
	stPointParam.fDynamicScale = 0.001;
	stPointParam.fDynamicMaxScale = 0;
	stPointParam.fDynamicMinScale = 0;
	stPointParam.direction = BoardDirect_e::FaceToCamera;
	stPointParam.bDepthTest = false;
	pAreaMeasureTool->SetPointParam(stPointParam);

	return pAreaMeasureTool;
}

void ProjectBasedClient::static_callback_statistics(BaseStatisticsTool* pTool, const Json::Value& jResult,
	std::vector<StatisTaskData*> vecFilterStatisTask, const bool bCancel)
{
	ProjectBasedClient* pThis = (ProjectBasedClient*)pTool->GetINTPTR("this");
	pThis->iOnStatisticsComplete(pTool, jResult, vecFilterStatisTask, bCancel);
}

void ProjectBasedClient::iOnStatisticsComplete(BaseStatisticsTool* pTool, const Json::Value& jResult,
	std::vector<StatisTaskData*>& vecFilterStatisTask, const bool bCancel)
{
	std::string strWeb = "";
	Json::Value jValue = Json::arrayValue;
	std::string strWebID = "";
	Json::Value jWebID = pTool->GetJsonExtra("web_commond_id");
	if (jWebID.isString())
	{
		strWebID = jWebID.asString();
	}
	if (!bCancel)
	{
		std::vector<std::string> vecKeys = jResult.getMemberNames();
		for (size_t i = 0; i < vecKeys.size(); i++)
		{
			Json::Value jItem = {};
			int nType = atoi(vecKeys[i].c_str());
			jItem["type"] = atoi(vecKeys[i].c_str());
			ProjectBasedSceneElementTypeInfo* pTypeInfo = m_pSceneElementLoader->GetElementTypeInfoByID(nType);
			std::string strName = "";
			if (pTypeInfo)
			{
				//char sz[1024] = { 0 };
				//AsciiToUtf8(pTypeInfo->strName.c_str(), sizeof(sz), sz);
				strName = pTypeInfo->strName;
			}
			if (jItem["type"] == -100)
			{
				//char sz[1024] = { 0 };
				//AsciiToUtf8("其他", sizeof(sz), sz);
				strName = "其他";
			}

			char sz[1024] = { 0 };
			AsciiToUtf8(strName.c_str(), sizeof(sz), sz);
			jItem["name"] = sz;

			//jItem["name"] = strName;
			jItem["item"] = Json::arrayValue;

			Json::Value list = jResult[vecKeys[i]];
			for (int j = 0; j < list.size(); j++)
			{
				Json::Value jParam = {};
				jParam["id"] = list[j]["id"];
				jParam["name"] = list[j]["name"];
				jParam["extra"] = list[j]["extra"];

				jItem["item"].append(jParam);
			}

			jValue.append(jItem);
		}
	}

	strWeb += jValue.toStyledString();
	strWeb += ",";
	strWeb += bCancel ? "true" : "false";
	ToSendWebCommond("VS_ToWebToolsStaticData", strWebID, strWeb);
}

void ProjectBasedClient::static_area_measure(INode* pPolygon, std::map<string, INT_PTR> mapINTPTR, float fResult)
{
	WebCommandData* pCommond = (WebCommandData*)mapINTPTR["web_commond"];
	ProjectBasedClient* pThis = (ProjectBasedClient*)mapINTPTR["this"];

	std::string str = "";
	str += to_string(fResult);
	pThis->ToSendWebCommond("VS_ToWebAreaMeasure", pCommond->strID, str);
}

void ProjectBasedClient::static_distance_measure(INode* pLocusLine, std::map<string, INT_PTR> mapINTPTR, std::vector<float> vecResult)
{
	WebCommandData* pCommond = (WebCommandData*)mapINTPTR["web_commond"];
	ProjectBasedClient* pThis = (ProjectBasedClient*)mapINTPTR["this"];
	std::string str = "";
	for (size_t i = 0; i < vecResult.size(); i++)
	{
		if (i == 0)
		{
			str += "[" + std::to_string(vecResult[i]) + ",";
			continue;
		}
		if (i == vecResult.size() - 1)
		{
			str += std::to_string(vecResult[i]) + "]";
			continue;
		}
		str += std::to_string(vecResult[i]) + ",";
	}
	pThis->ToSendWebCommond("VS_ToWebDistanceMeasure", pCommond->strID, str);
}

void ProjectBasedClient::static_callback_sight_analysis(INode* pLineNode, SightAnalysisTool* pTool, Json::Value jResult)
{
	ProjectBasedClient* pThis = (ProjectBasedClient*)pTool->GetINTPTR("this");
	std::string strWebId = pTool->GetExtra("web_id").asString();
	std::string strData = jResult.toStyledString();
	pThis->ToSendWebCommond("VS_SightAnalysis", strWebId, strData);
}

void ProjectBasedClient::sendCameraViewport(std::string& strWebId, const bool bForce)
{
	if (!m_strListenCameraViewportWebId.empty() || bForce)
	{
		INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
		ICamera* pCamera = static_cast<ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
		char szPos[LENGTH_1K] = { 0 };
		IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));
		Vector3d vPos = pCameraNode->iGetOrigin();
		Vector3d vAngle = pCameraNode->iGetAngles();
		if (m_vLastCameraViewport.Distance(vPos) > 3.0f || m_vLastCameraAngle.Distance(vAngle) > 3.0f || bForce)
		{
			m_vLastCameraViewport = vPos;
			m_vLastCameraAngle = vAngle;

			sprintf_s(szPos, sizeof(szPos), "%f, %f, %f, %f, %f, %f", vPos.x, vPos.y, vPos.z, vAngle.x, vAngle.y, vAngle.z);

			double dLng, dLat;
			char szWgs84[LENGTH_1K] = { 0 };
			ScenePosToWGS(vPos, dLng, dLat);
			sprintf_s(szWgs84, sizeof(szWgs84), "%f, %f,%f,%f,%f,%f", dLng, dLat, vPos.z, vAngle.x, vAngle.y, vAngle.z);

			std::string str = "";
			str += "'" + std::string(szPos) + "'";
			str += ",";
			str += "'" + std::string(szWgs84) + "'";
			ToSendWebCommond("VS_ToCameraViewport", strWebId, str);
		}
	}
}

void ProjectBasedClient::sendListenViewShedState()
{
	if (m_pViewshedAnalysisTool != nullptr && m_pViewshedAnalysisTool->GetViewshedAnalysis() && !m_strListenViewShedStateWebId.empty())
	{
		Vector3d vOrigin = m_pViewshedAnalysisTool->GetViewshedAnalysis()->iGetOrigin();
		Vector3d vAngles = m_pViewshedAnalysisTool->GetViewshedAnalysis()->iGetAngles();
		double dLng = 0.0f, dLat = 0.0f;
		ScenePosToWGS(vOrigin, dLng, dLat);
		float fClip = m_pViewshedAnalysisTool->GetViewshedAnalysis()->iGetFarClip();
		float fHRangeAngle = m_pViewshedAnalysisTool->GetViewshedAnalysis()->iGetFOV();
		float fVRangeAngle = m_pViewshedAnalysisTool->GetVFov();
		ViewshedState thisViewshedState = ViewshedState(vOrigin, vAngles, fHRangeAngle, fVRangeAngle, fClip);
		if (!m_vPreviousViewshedState.IsChange(thisViewshedState))
		{
			m_vPreviousViewshedState = ViewshedState(vOrigin, vAngles, fHRangeAngle, fVRangeAngle, fClip);
			return;
		}
		m_vPreviousViewshedState = ViewshedState(vOrigin, vAngles, fHRangeAngle, fVRangeAngle, fClip);
		Json::Value jValue = Json::objectValue;
		std::string strOrigin = std::to_string(vOrigin.x) + "," + std::to_string(vOrigin.y) + "," + std::to_string(vOrigin.z);
		std::string strLnglat = std::to_string(dLng) + "," + std::to_string(dLat) + "," + std::to_string(vOrigin.z);
		std::string strAngles = std::to_string(vAngles.x) + "," + std::to_string(vAngles.y) + "," + std::to_string(vAngles.z);
		jValue["origin"] = strOrigin;
		jValue["lnglat"] = strLnglat;
		jValue["angle"] = strAngles;
		jValue["field_horizontal"] = fHRangeAngle;
		jValue["field_vertical"] = fVRangeAngle;
		jValue["far_clip"] = fClip;
		std::string str = jValue.toStyledString();
		ToSendWebCommond("VS_ListenViewShedState", m_strListenViewShedStateWebId, str);
	}
}

void ProjectBasedClient::sendListenMouseWheel(float fValue)
{
	if (!m_strListenMouseWheelId.empty())
	{
		std::string str = "";
		str += to_string(fValue);
		ToSendWebCommond("VS_ListenMouseWheel", m_strListenMouseWheelId, str);
	}
}

void ProjectBasedClient::sendListenMouseLB(int nState, Vector3d vPos)
{
	if (!m_strListenMouseLBWebId.empty())
	{
		IManipulator* pManipulator = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->GetDynamicComponent<IManipulator>();
		Vector3d vPos = pManipulator->iGetMousePoint();
		char szPos[LENGTH_1K] = { 0 };
		sprintf_s(szPos, sizeof(szPos), "%f, %f, %f", vPos.x, vPos.y, vPos.z);

		double dLng, dLat;
		char szWgs84[LENGTH_1K] = { 0 };
		ScenePosToWGS(vPos, dLng, dLat);
		sprintf_s(szWgs84, sizeof(szWgs84), "%f, %f,%f", dLng, dLat, vPos.z);

		std::string str = "";
		str += to_string(nState);
		str += ",";
		str += "'" + std::string(szPos) + "'";
		str += ",";
		str += "'" + std::string(szWgs84) + "'";
		ToSendWebCommond("VS_ListenMouseLB", m_strListenMouseLBWebId, str);
	}
}

void ProjectBasedClient::sendListenMouseRB(int nState, Vector3d vPos)
{
	if (!m_strListenMouseRBWebId.empty())
	{
		IManipulator* pManipulator = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->GetDynamicComponent<IManipulator>();
		Vector3d vPos = pManipulator->iGetMousePoint();
		char szPos[LENGTH_1K] = { 0 };
		sprintf_s(szPos, sizeof(szPos), "%f, %f, %f", vPos.x, vPos.y, vPos.z);

		double dLng, dLat;
		char szWgs84[LENGTH_1K] = { 0 };
		ScenePosToWGS(vPos, dLng, dLat);
		sprintf_s(szWgs84, sizeof(szWgs84), "%f, %f,%f", dLng, dLat, vPos.z);

		std::string str = "";
		str += to_string(nState);
		str += ",";
		str += "'" + std::string(szPos) + "'";
		str += ",";
		str += "'" + std::string(szWgs84) + "'";
		ToSendWebCommond("VS_ListenMouseRB", m_strListenMouseRBWebId, str);
	}
}

void ProjectBasedClient::sendListenMouseMovePos(Vector3d vPos)
{
	if (m_vLastMouseMovePos.Distance2D(vPos) < 2.0f)
	{
		return;
	}
	m_vLastMouseMovePos = vPos;
	if (!m_strListenMouseMovePosWebId.empty())
	{
		IManipulator* pManipulator = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->GetDynamicComponent<IManipulator>();
		Vector3d vPos = pManipulator->iGetMousePoint();
		char szPos[LENGTH_1K] = { 0 };
		sprintf_s(szPos, sizeof(szPos), "%f, %f, %f", vPos.x, vPos.y, vPos.z);

		double dLng, dLat;
		char szWgs84[LENGTH_1K] = { 0 };
		ScenePosToWGS(vPos, dLng, dLat);
		sprintf_s(szWgs84, sizeof(szWgs84), "%f, %f,%f", dLng, dLat, vPos.z);

		std::string str = "";
		str += "'" + std::string(szPos) + "'";
		str += ",";
		str += "'" + std::string(szWgs84) + "'";
		ToSendWebCommond("VS_ListenMouseMovePos", m_strListenMouseMovePosWebId, str);
	}
}

bool ProjectBasedClient::static_request_getElevationPoint(map<string, INT_PTR>& mapParams,
	ThreadCallbackParam* stCallbackParam)
{
	ProjectBasedClient* pThis = (ProjectBasedClient*)mapParams["this"];
	float fCenterSceneX = stCallbackParam->sRequestParam.GetExtraJValue("fCenterSceneX").asFloat();
	float fCenterSceneY = stCallbackParam->sRequestParam.GetExtraJValue("fCenterSceneY").asFloat();
	float fRadius = stCallbackParam->sRequestParam.GetExtraJValue("fRadius").asFloat();
	float fIgnoreRange = stCallbackParam->sRequestParam.GetExtraJValue("fIgnoreRange").asFloat();
	int nCount = stCallbackParam->sRequestParam.GetExtraJValue("nCount").asInt();

	//分割区域取高地图
	std::vector<Vector3d> vecResult;
	Vector2 vLeftTop(fCenterSceneX - fRadius, fCenterSceneY + fRadius);
	Vector2 vRightBottom(fCenterSceneX + fRadius, fCenterSceneY - fRadius);
	Vector3d vCenterPos(fCenterSceneX, fCenterSceneY, 0);
	float fStartX = vLeftTop.x;
	float fStartY = vLeftTop.y;
	while (fStartX < vRightBottom.x)
	{
		float fTempX = fStartX + 5.0f;
		if (fTempX > vRightBottom.x)
		{
			fTempX = vRightBottom.x;
		}
		while (fStartY > vRightBottom.y)
		{
			float fTempY = fStartY - 5.0f;
			if (fTempY < vRightBottom.y)
			{
				fTempY = vRightBottom.y;
			}
			double dLeftTopCoord[2] = { fStartX, fStartY };
			double dRightBottomCoord[2] = { fTempX, fTempY };
			std::vector<Vector3d> vecPoint;
			std::vector<Vector3d> vecTop;
			pThis->LoadTif(vecPoint, dLeftTopCoord, dRightBottomCoord);
			//去重处理,个数处理
			pThis->removeElevationPointDuplicate(vCenterPos, fRadius, fIgnoreRange, vecPoint, vecTop, nCount);
			vecResult.insert(vecResult.end(), vecTop.begin(), vecTop.end());
			vecPoint.clear();
			vecTop.clear();

			fStartY = fTempY;
		}
		fStartX = fTempX;
		fStartY = vLeftTop.y;
	}
	sort(vecResult.begin(), vecResult.end(), [&](const Vector3d& a, const Vector3d& b)->bool {
		return a.z > b.z;
		});
	std::vector<Vector3d> vecOut;
	pThis->removeElevationPointDuplicate(vCenterPos, fRadius, fIgnoreRange, vecResult, vecOut, nCount);
	sort(vecOut.begin(), vecOut.end(), [&](const Vector3d& a, const Vector3d& b)->bool {
		return a.z > b.z;
		});
	//给网页回调
	Json::Value jArray = Json::arrayValue;
	for (size_t i = 0; i < vecOut.size(); ++i)
	{
		Json::Value jItem = {};
		char sz[1024] = { 0 };
		double dLng, dLat = 0.0f;
		pThis->ScenePosToWGS(vecOut[i], dLng, dLat);
		sprintf_s(sz, sizeof(sz), "%f,%f,%f", vecOut[i].x, vecOut[i].y, vecOut[i].z);
		jItem["scene_pos"] = sz;
		sprintf_s(sz, sizeof(sz), "%f,%f,%f", dLng, dLat, vecOut[i].z);
		jItem["lnglat_pos"] = sz;

		jArray.append(jItem);
	}

	std::string strData = jArray.toStyledString();
	pThis->ToSendWebCommond("VS_GetElevationPoint", pThis->m_strGetElevationPointWebId, strData);

	return true;
}

void ProjectBasedClient::removeElevationPointDuplicate(Vector3d& vCenterPos, float& fRadius, float& fIgnoreRange, std::vector<Vector3d>& vecSrc,
	std::vector<Vector3d>& vecOut, int nCount)
{
	bool bNew = true;
	for (size_t i = 0; i < vecSrc.size(); ++i)
	{
		if (vCenterPos.Distance2D(vecSrc[i]) > fRadius)
		{
			continue;
		}
		if (i > 0)
		{
			//距离在2.0f内的则忽略
			bNew = true;
			for (size_t j = 0; j < vecOut.size(); ++j)
			{
				if (vecOut[j].Distance2D(vecSrc[i]) < fIgnoreRange)
				{
					bNew = false;
					break;
				}
			}
		}
		if (bNew)
		{
			vecOut.push_back(vecSrc[i]);
		}
		if (vecOut.size() >= nCount)
		{
			break;
		}
	}
}

void	ProjectBasedClient::iOnHCLabelClickLabel(FactotryAssembleDetail* pAssemble, CommonLabelFactory* pFactory, bool bClick)
{
	//判断互斥组
	if (bClick)
	{
		std::map<std::string, std::vector<int>> mapExclusiveFactory = m_pSceneElementLoader->GetExclusiveFactory();
		int nType = pAssemble->nType;
		for (auto& it : mapExclusiveFactory)
		{
			for (auto nGroupType : it.second)
			{
				if (nType != nGroupType)
				{
					//关闭其他的
					CommonLabelFactory* pFactory = m_pSceneElementLoader->GetOutElementFactoryByType(nGroupType);
					if (pFactory)
					{
						pFactory->GetDetailFactory()->CloseAll();
					}
				}
			}
		}
	}

	std::string strExtra = pAssemble->pData->strExtra;
	if (strExtra.empty())
	{
		strExtra = "''";
	}
	std::string str = "";
	str += "'" + pAssemble->pData->strUuid + "'";
	str += ",";
	str += to_string(pAssemble->pData->nTypeID);
	str += ",";
	str += strExtra;
	str += ",";
	str += bClick ? "1" : "0";
	ToSendWebCommond("VS_ClickHCMapLabel", "", str);

	BCEvent tEvent(PROJECTBASED_EVENT_HC_LABEL_CLICK, (EVENT_PARAM)pAssemble, (EVENT_PARAM)pFactory, (EVENT_PARAM)bClick);
	APIProvider::GetSystemAPI()->iProjectBasedAPI->iGetStateManager()->iExecuteEvent(tEvent);
}

void	ProjectBasedClient::iOnHCDetailClickLabel(FactotryAssembleDetail* pAssemble, CommonLabelFactory* pFactory, std::string strFlagName, bool bClick)
{
	std::string strExtra = pAssemble->pData->strExtra;
	if (strExtra.empty())
	{
		strExtra = "''";
	}
	std::string str = "";
	str += "'" + pAssemble->pData->strUuid + "'";
	str += ",";
	str += to_string(pAssemble->pData->nTypeID);
	str += ",";
	str += strExtra;
	str += ",";
	str += bClick ? "1" : "0";
	str += ",";
	str += "'" + strFlagName + "'";
	ToSendWebCommond("VS_ClickHCMapDetail", "", str);

	BCEvent tEvent(PROJECTBASED_EVENT_HC_DETAIL_CLICK, (EVENT_PARAM)pAssemble, (EVENT_PARAM)pFactory, (EVENT_PARAM)&strFlagName, (EVENT_PARAM)bClick);
	APIProvider::GetSystemAPI()->iProjectBasedAPI->iGetStateManager()->iExecuteEvent(tEvent);
}

void	ProjectBasedClient::iOnWebLabelClickLabel(FactotryAssembleDetail* pAssemble, CommonLabelFactory* pFactory, bool bClick)
{
	std::string strExtra = pAssemble->pData->strExtra;
	std::string str = "";
	str += "'" + pAssemble->pData->strUuid + "'";
	str += ",";
	str += to_string(pAssemble->pData->nTypeID);
	str += ",";
	str += "'" + strExtra + "'";
	str += ",";
	str += bClick ? "1" : "0";

	std::string strData = str;
	ToSendWebCommond("VS_ClickLabel", "", strData);
}

void	ProjectBasedClient::iOnWebDetailClickLabel(FactotryAssembleDetail* pAssemble, CommonLabelFactory* pFactory, std::string strFlagName, bool bClick)
{
	std::string strExtra = pAssemble->pData->strExtra;
	std::string str = "";
	str += "'" + pAssemble->pData->strUuid + "'";
	str += ",";
	str += to_string(pAssemble->pData->nTypeID);
	str += ",";
	str += "'" + strExtra + "'";
	str += ",";
	str += bClick ? "1" : "0";
	str += ",";
	str += "'" + strFlagName + "'";

	ToSendWebCommond("VS_ClickDetail", "", str);
}

void ProjectBasedClient::static_request_CloudLogin_callback(ThreadCallbackParam_s* pParam)
{
	ProjectBasedClient* pThis = (ProjectBasedClient*)pParam->sRequestParam.mapParams["this"];
	std::string strWebID = pParam->sRequestParam.GetExtraJValue("web_commond_id").asString();
	int nCode = -1;
	std::string strToken;
	std::string strProjectName;
	//char sz[1024] = { 0 };
	//AsciiToUtf8("接口访问出错!", sizeof(sz), sz);
	std::string strMsg = "接口访问出错!";
	if (pParam->nResult == STATUS_SUCCESS)
	{
		try
		{
			Json::Value& jResult = pParam->jRoot;
			nCode = jResult["code"].asInt();
			if (nCode == 0)
			{
				//成功
				strToken = jResult["data"]["access_token"].asString();
				std::string strUserName = jResult["data"]["account"].asString();
				strProjectName = jResult["data"]["project_name"].asString();
				pThis->m_sUserInfo.strAccount = pThis->m_sUserInfo.strAccount;
				pThis->m_sUserInfo.strUserToken = strToken;
			}
			strMsg = jResult["msg"].asString();
		}
		catch (const std::exception&)
		{
		}
	}
	Json::Value jObject;
	jObject["code"] = nCode;
	jObject["token"] = strToken;
	jObject["msg"] = strMsg;

	pThis->ToSendWebCommond("VS_CloudLogin", strWebID, jObject.toStyledString());
	//登录成功,启动三维
	if (nCode == 0)
	{
		if (APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iIsSceneLoad())
		{
			std::string strData = "";
			pThis->ToSendWebCommond("VS_ShowAppPage", "", strData);
		}
		else
		{
			std::string strQtVSCommond = "start_game";
			pThis->iExecuteQtVsCommond(strQtVSCommond, nullptr, nullptr);
		}
	}
}

void ProjectBasedClient::AddRefreshFontBoardID(const std::string& strID)
{
	std::vector<std::string>::iterator it = find(m_vecNeedRefreshFontBoard.begin(), m_vecNeedRefreshFontBoard.end(), strID);
	if (it != m_vecNeedRefreshFontBoard.end())
	{
		m_vecNeedRefreshFontBoard.erase(it);
	}
	m_vecNeedRefreshFontBoard.push_back(strID);
}
