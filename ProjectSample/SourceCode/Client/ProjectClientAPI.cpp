#include "UserHeader.h"
#include "ProjectClientAPI.h"
#include "./statemachine/GlobalState.h"
#include "./test/TestFactory.h"
#include "./test/TestInstanceFactory.h"
#include "./test/TestClusterFunc.h"
#include "data/ProjectSceneElementLoader.h"

using namespace bc;

ProjectClientAPI* ProjectClientAPI::m_pProjectClientAPI = nullptr;
ISystemAPI* APIProvider::m_pSystemAPI = nullptr;

wstring Ascii_To_Unicode(const string& str)
{
	wstring ret_str = L"";
	//预算-缓冲区中宽字节的长度
	int unicodeLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
	//给指向缓冲区的指针变量分配内存
	wchar_t* pUnicode = (wchar_t*)malloc(sizeof(wchar_t) * unicodeLen);
	//开始向缓冲区转换字节
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, pUnicode, unicodeLen);
	if (pUnicode)
	{
		ret_str = pUnicode;
	}
	free(pUnicode);

	return ret_str;
}

string Unicode_To_Utf8(const wstring& wstr)
{
	string ret_str = "";
	//预算 - 缓冲区中多字节的长度
	int ansiiLen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	//给指向缓冲区的指针变量分配内存
	char* pAssii = (char*)malloc(sizeof(char) * ansiiLen);
	//开始向缓冲区转换字节
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, pAssii, ansiiLen, nullptr, nullptr);
	if (pAssii)
	{
		ret_str = pAssii;
	}
	free(pAssii);

	return ret_str;
}

string Ascii_To_Utf8(const string& str)
{
	return Unicode_To_Utf8(Ascii_To_Unicode(str));
}

ProjectClientAPI::ProjectClientAPI() : ProjectBasedClient()
{
	ProjectLogger::GetInstance()->Create(5);

	m_vLngLatLeftTop = Vector2(115.2808794121, 22.8044941777);				//左上角经纬度
	m_vLngLatRightBottom = Vector2(115.2949213829, 22.7953468689);			//右下角经纬度
	m_vScenePosLeftTop = Vector2(-920.38, 899.27);						//左上角场景坐标
	m_vScenePosRightBottom = Vector2(528.457, -103.596);					//右下角场景坐标

	m_vSelfLngLatLeftTop = m_vLngLatLeftTop;				//左上角经纬度
	m_vSelfLngLatRightBottom = m_vLngLatRightBottom;				//右下角经纬度
	m_vSelfScenePosLeftTop = m_vScenePosLeftTop;						//左上角场景坐标
	m_vSelfScenePosRightBottom = m_vScenePosRightBottom;  		//右下角场景坐标

	m_strTifFilePath = "resource/project/TestDemo/result.tif";
}

ProjectClientAPI::~ProjectClientAPI()
{
	DELETE_PTR(m_pAPIProvider);
}

void ProjectClientAPI::iInitialize(ISystemAPI* pSystemAPI)
{
	m_pAPIProvider = new APIProvider(pSystemAPI);

	ProjectBasedClient::iInitialize(pSystemAPI);
}

void ProjectClientAPI::iInitializeScene()
{
	ProjectBasedClient::iInitializeScene();


	APIProvider::GetSystemAPI()->iProjectBasedAPI->iGetStateManager()->iAddStateMachine(new GlobalState());
}

void ProjectClientAPI::iSceneLoad()
{
	ProjectBasedClient::iSceneLoad();

	IManipulator* pManipulator = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera()->GetDynamicComponent<IManipulator>();
	pManipulator->iSetInertialEnable(true);
	pManipulator->iSetInertialSpeedScale(2.0f);

	//标记社区治理中心位置
#if 1
	{
		FontBoardParam tParam;
		tParam.fWidth = 40;
		tParam.fHeight = 60;
		tParam.strImage = string(PROJECT_RESOURCE_PATH) + "/scene/2@2x.png";
		//tParam.bClickEnable = true;
		//tParam.bGlowEnable = true;
		tParam.fScale = 0.5;
		tParam.direction = 2;

		INode* pFontBoard = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(tParam);
		pFontBoard->iSetID("shequzhilizhongxin");
		pFontBoard->iSetOrigin(Vector3d(-149.030293, 68.862379, 50.0));
		pFontBoard->iSetVisible(true);
		pFontBoard->iLoadModel();
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode()->iAddChild(pFontBoard);
	}
#endif

	//ThreadRequestParam stParam;
	//stParam.eRequestType = THREAD_REQUEST_CUSTOM;
	//stParam.pThreadRuningFunc = static_callback_test;
	//stParam.bRunCallbackInMainThread = true;
	//stParam.bThread = true;
	//stParam.bInQueue = true;
	//stParam.bRepeat = true;
	//stParam.lInterval = 50000;
	//stParam.AddParam("this", (INT_PTR)this);
	//ThreadWrapper::GetInstance()->AddTask(stParam);

#if 1
	//创建定位标签
	{
		std::string strLabelKey = "FontBoardParam";
		std::string strFilePath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir;
		strFilePath += "/" + PROJECT_RESOURCE_PATH + "/style/style_test_fontboard.json";
		if (IsFileExist(strFilePath))
		{
			std::string strData = ReadFile(strFilePath);
			ParamParseStream::GetInstance()->ReadParamFromString(strData, strLabelKey);
			ReadParam* pReadParam = ParamParseStream::GetInstance()->GetReadParamByKey(strLabelKey);
			FontBoardParam* pParam = (FontBoardParam*)pReadParam->pParam;

			m_pLocationBoard = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(*pParam);
			m_pLocationBoard->iSetID("locationboard");
			m_pLocationBoard->iSetOrigin(Vector3d(30.657349, 219.726562, 5));
			m_pLocationBoard->iSetVisible(true);
			m_pLocationBoard->iLoadModel();
			APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode()->iAddChild(m_pLocationBoard);
		}
	}
#endif

#if 0
	// 创建视频监控
	{
		std::vector<ProjectBasedSceneElement*> vecElement;
		{
			ProjectBasedSceneElement* pElement = new ProjectBasedSceneElement();
			pElement->nTypeID = 11;
			pElement->strUuid = "44150257001312000005";
			pElement->vLabelPos = Vector3(-159.619064, -224.765335, 9.451541);

			string strData = "{\"Camera_id\":{\"title\":\"" + Ascii_To_Utf8("监控点共享编码") + "\",";
			strData += "\"value\":\"44150257001312000005\"},";
			strData += "\"Camera_name\":{\"title\":\"" + Ascii_To_Utf8("监控点名称") + "\",";
			strData += "\"value\":\"" + Ascii_To_Utf8("天面_通道1") + "\"},";
			strData += "\"Region\":{\"title\":\"" + Ascii_To_Utf8("所在区域") + "\",";
			strData += "\"value\":\"" + Ascii_To_Utf8("中国/广东省/深圳市/监控测试") + "\"}}";

			Json::Value jItem = {};
			jItem["title"] = Ascii_To_Utf8("扩展属性");
			jItem["value"] = strData;

			Json::Value jExtra = {};
			jExtra["description"] = jItem;
			pElement->strExtra = jExtra.toStyledString();
			vecElement.push_back(pElement);
		}

		{
			ProjectBasedSceneElement* pElement = new ProjectBasedSceneElement();
			pElement->nTypeID = 11;
			pElement->strUuid = "44150257001312000006";
			pElement->vLabelPos = Vector3(-175.591171, -224.758789, 9.219153);

			string strData = "{\"Camera_id\":{\"title\":\"" + Ascii_To_Utf8("监控点共享编码") + "\",";
			strData += "\"value\":\"44150257001312000006\"},";
			strData += "\"Camera_name\":{\"title\":\"" + Ascii_To_Utf8("监控点名称") + "\",";
			strData += "\"value\":\"" + Ascii_To_Utf8("天面_通道2") + "\"},";
			strData += "\"Region\":{\"title\":\"" + Ascii_To_Utf8("所在区域") + "\",";
			strData += "\"value\":\"" + Ascii_To_Utf8("中国/广东省/深圳市/监控测试") + "\"}}";

			Json::Value jItem = {};
			jItem["title"] = Ascii_To_Utf8("扩展属性");
			jItem["value"] = strData;

			Json::Value jExtra = {};
			jExtra["description"] = jItem;
			pElement->strExtra = jExtra.toStyledString();
			vecElement.push_back(pElement);
		}

		m_pSceneElementLoader->AddElement(vecElement, false);
	}
#endif

#if 0
	INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
	std::string strViewPort = "291.909454, 261.652954, 55.0, -184.707395, 22.798709, 180.0";
	//std::string strViewPort = "1116.793579, 68.055550, 100.0, -184.699442, 22.796999, 180.0";
	bool bSky = false;
	GoViewPoint(pCameraNode, strViewPort, bSky);
#endif

#if 0
	// 创建走私点
	std::vector<ProjectBasedSceneElement*> vecElement;
	ProjectBasedSceneElement* pElement = new ProjectBasedSceneElement();
	pElement->nTypeID = 11;
	pElement->strUuid = "zousidian_1";
	pElement->vLabelPos = Vector3(252.660843, -238.108978, -50.076714);
	vecElement.push_back(pElement);
	
	m_pSceneElementLoader->AddElement(vecElement, false);
#endif

#if 0
	{
		vector<Vector3> vPos;
		vPos.push_back(Vector3(76.003281, -26.306555, -55.0));
		vPos.push_back(Vector3(208.619919, 12.432613, -55.0));
		vPos.push_back(Vector3(367.437805, -27.428209, -55.0));

		LocusLineParam stParam;
		stParam.nNodeType = NODE_LOCUS_LINE;
		stParam.stDefaultSegment.nLineStyle = LineStyle_e::LINE_STYLE_DOTTED;
		INode* pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(stParam);
		pNode->iLoadModel();
		APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode()->iAddChild(pNode);

		vector<LocusLineSegment> vecSeg;
		LocusLineSegment sSegment;
		sSegment.vColor = Vector3(255 / 255.0f, 210.0f / 255.0f, 85.0f / 255.0f);
		sSegment.fWidth = 10.0f;
		sSegment.vecVertex = vPos;

		vecSeg.push_back(sSegment);

		ILocusLine* pLocusLine = GetComponent<ILocusLine>(pNode);
		pLocusLine->iUpdateLocusLineSegment(vecSeg);
		pNode->iSetVisible(true);
	}
#endif
}

void ProjectClientAPI::iFrameUpdate()
{
	ProjectBasedClient::iFrameUpdate();
	//do something
}

void ProjectClientAPI::iPostFrameUpdate()
{
	ProjectBasedClient::iPostFrameUpdate();
}

void ProjectClientAPI::iPostSceneLoad()
{
	ProjectBasedClient::iPostSceneLoad();
}

std::string ProjectClientAPI::iPreLoadWebPage(const std::string& strOriUrl, const std::string& strConcatUrl)
{
	return ProjectBasedClient::iPreLoadWebPage(strOriUrl, strConcatUrl);
}

EventReturnType_e ProjectClientAPI::iExecuteEvent(const BCEvent& tEvent)
{
	ProjectBasedClient::iExecuteEvent(tEvent);

	return EventReturnType_e::NONE;
}


EventReturnType_e ProjectClientAPI::iProcessEvent(const BCEvent& tEvent)
{
	ProjectBasedClient::iProcessEvent(tEvent);

	return EventReturnType_e::NONE;
}

void ProjectClientAPI::iDoUdpData(int nCommond, char* pData)
{

	WebCommandData tWebData;
	tWebData.strCommand = "inner_doUdp";
	std::string strData = pData;
	tWebData.vecValues.push_back(nCommond);
	tWebData.vecValues.push_back(strData);
	AddWebCommond(tWebData);
}

void ProjectClientAPI::iDoWebCommond(WebCommandData* pWebData)
{
	ProjectBasedClient::iDoWebCommond(pWebData);
}

void ProjectClientAPI::iOnHCLabelClickLabel(FactotryAssembleDetail* pAssemble, CommonLabelFactory* pFactory, bool bClick)
{
	ProjectBasedClient::iOnHCLabelClickLabel(pAssemble, pFactory, bClick);
}

ProjectClientAPI* ProjectClientAPI::GetProjectClientAPI()
{
	if (m_pProjectClientAPI == nullptr)
	{
		m_pProjectClientAPI = new ProjectClientAPI;
	}

	return m_pProjectClientAPI;
}

void ProjectClientAPI::Destroy()
{
	DELETE_PTR(m_pProjectClientAPI);
}

void ProjectClientAPI::iOnSceneElementLoadedData(bool bAllComplete)
{

}

SceneElementLoader* ProjectClientAPI::iCreateSceneElementLoader()
{
	return new ProjectSceneElementLoader(this);
}

void ProjectClientAPI::iRefreshData()
{
	
}
