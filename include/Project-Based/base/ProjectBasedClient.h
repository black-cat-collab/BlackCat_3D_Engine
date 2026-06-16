#pragma once

#include "Project-Based/label/func/IFunc.h"
#include "Project-Based/label/func/ClusterFunc.h"
#include "Project-Based/base/BuildIndoorStateFactory.h"
#include "Project-Based/Factory/BuildIndoorFactory.h"
#include "Project-Based/base/VideoInspectionFactory.h"
#include "Project-Based/Factory/BCVideoInspectionFactory.h"
#include "Project-Based/base/HeatmapFactory.h"
#include "Project-Based/Factory/TencentHeatMapFactory.h"
#include "Project-Based/base/SceneElementLoader.h"
#include "Project-Based/webApi/WebLabelParser.h"
#include "Project-Based/utils/tools/ViewshedAnalysisTool.h"
#include "Project-Based/label/CommonLabelFactory.h"

using namespace std;

namespace bc
{
	class CommonLabelFactory;
	class WebCommonApi;
	class KMapAdapter;
	class SceneElementLoader;
	class HeatmapFactory;
	class WebLabelParser;
	class SightAnalysisTool;
	class CircleStatisticsTool;
	class LineStatisticsTool;
	class PolygonStatisticsTool;
	class AreaMeasureTool;
	class DistanceMeasureTool;
	class ProjectBasedClient;
	class BaseStatisticsTool;
	class StatisTaskData;
	class LayerDebugSceneLoader;

	class UdpParam
	{
	public:
		int nClientPort;
		SocketReceiveFunc pReceiveFunc;

		UdpParam()
		{
			nClientPort = -1;
			pReceiveFunc = nullptr;
		}

		class UdpParam& operator = (const class UdpParam& src)
		{
			nClientPort = src.nClientPort;
			pReceiveFunc = src.pReceiveFunc;

			return *this;
		}
	};

	//高低图层级，支持多张细小高地图，提高效率
	class TifLevel
	{
	public:
		Vector2d vLeftTop;
		Vector2d vRightBottom;
		float fArea;		//根据面积做排序，先从小的开始找找不到再往后（因为高地图会重叠渲染）
		std::string strFilePath;

		TifLevel()
		{
			fArea = 0.0f;
			vLeftTop = Vector2d(0.0f);
			vRightBottom = Vector2d(0.0f);
		}

		class TifLevel& operator = (const class TifLevel& src)
		{
			vLeftTop = src.vLeftTop;
			vRightBottom = src.vRightBottom;
			fArea = src.fArea;
			strFilePath = src.strFilePath;

			return *this;
		}

		bool IsMatch(Vector2& vPoint)
		{
			return ((vLeftTop.x - vPoint.x) * (vRightBottom.x - vPoint.x) <= 0) && ((vLeftTop.y - vPoint.y) * (vRightBottom.y - vPoint.y) <= 0);
			//Vector2 vLeftBottom = Vector2(vLeftTop.x, vRightBottom.y);
			//Vector2 vLeftTop = Vector2(vLeftTop.x, vLeftTop.y);
			//Vector2 vRightTop = Vector2(vRightBottom.x, vLeftTop.y);
			//Vector2 vRightBottom = Vector2(vRightBottom.x, vRightBottom.y);
			//return IsPointInRect(vPoint, vLeftBottom, vLeftTop, vRightTop, vRightBottom);
		}
	};
	enum EAppMode
	{
		APP_MODE_EXHIBITION = 1 << 1,	//演示模式
		APP_MODE_YBSS_EDIT = 1 << 2,			//一标三实模式
		APP_MODE_VIDEO_FUSE_EDIT = 1 << 3,	//视频融合
		APP_MODE_ROUTE_EDIT = 1 << 4,		//漫游路线
		APP_MODE_VIEW_EDIT = 1 << 5,			//视点抓取
		APP_MODE_DYNAMIC_EDIT = 1 << 6,		//动态视域
		APP_MODE_PLOTTING = 1 << 7,			//动态标绘
		APP_MODE_GIS_EARTH = 1 << 8,			//gis地球(可能是临时的，后续可能用不到)
		APP_MODE_LAYER_EDIT = 1 << 9,			//场景要素上图

		APP_MODE_NONE = 0,		//都没有
	};

	class YBSSModule;
	class VideoFusionModule;
	class BCModuleManager
	{
	public:
		YBSSModule* pYBSSModule;		//一标三实module
		VideoFusionModule* pVideoFuseModule;

	public:
		BCModuleManager()
		{
			pYBSSModule = nullptr;
			pVideoFuseModule = nullptr;
		}

	};

	//元素监听param
	class ListenElementParam
	{
	public:
		int nType;
		std::string strId;
		Vector3d vLastPos;

		ListenElementParam()
		{
			nType = -1;
		}
	};

	//默认的视频巡控类
	class PROJECT_BASED_API DefaultVideoInspectionFactory : public BCVideoInspectionFactory
	{
	public:
		DefaultVideoInspectionFactory(ProjectBasedClient* pClient);
		virtual ~DefaultVideoInspectionFactory();

		//面板的样式,设置视频的livegbs的liveUrl和ChannelID
		virtual void iSetVideoBoardParam(const std::string& strKey, BCVideoRouterPointInfo* pInfo, VideoBoardWithButtonParam* pParam);
		virtual void iInterceptHttpRequest(ThreadRequestParam* pRequest);
		virtual void iLnglatToScenePos(const double& dLng, const double& dLat, const double& dZ, Vector3d& vOutPos, Vector3d& vOutAngle);
		virtual void iOnLoaded(Json::Value jRetData);
		virtual bool iChangeVideoPoint(BCVideoRouterPointInfo* pInfo, bool bOpen);

	protected:
		ProjectBasedClient* m_pClient;
	};

	//默认的热力图类
	class PROJECT_BASED_API DefaultHeatMapFactory : public TencentHeatMapFactory
	{
	public:
		DefaultHeatMapFactory(ISystemAPI* pSystemAPI, ProjectBasedClient* pClient);
		~DefaultHeatMapFactory();

	protected:
		virtual Vector3d iBoundaryLnglatToScenePos(std::string strKey, const double dlng,
			const double dlat);
		virtual Vector3d iHeatmapLnglatToScenePos(std::string strKey, const double dlng,
			const double dlat);
		virtual void iLoadedData();
	protected:
		ProjectBasedClient*			m_pClient;
	};

	//室内分层
	class PROJECT_BASED_API DefaultIndoorFactory : public BuildIndoorFactory
	{
	public:
		DefaultIndoorFactory(ProjectBasedClient* pClient);
		virtual ~DefaultIndoorFactory();

		virtual void iChangeBuilding(BuildingData* pData);
		virtual void iLeaveAreaScene(AreaSceneData* pData);
		virtual void iLoadedData();

	protected:
		ProjectBasedClient* m_pClient;

		Vector3d	m_vLastCameraOrigin;
		Vector3d	m_vLastCameraAngle;
	};

	//订阅类
	class PROJECT_BASED_API DefaultProjectElementLoader : public SceneElementLoader
	{
	public:
		DefaultProjectElementLoader(ProjectBasedClient* pClient);
		virtual ~DefaultProjectElementLoader();

		void iOnLoadedData(bool bAllComplete);

	protected:
		ProjectBasedClient*			m_pClient;
	};

	//////////////////////////////////////////////////////////

	class PROJECT_BASED_API ProjectBasedClient : public IProjectClientAPI
	{
	public:
		ProjectBasedClient();
		virtual	~ProjectBasedClient();

		//虚函数
		virtual void				iInitialize(ISystemAPI* pSystemAPI);
		virtual void				iInitializeScene();
		virtual void				iSceneLoad();
		virtual void				iFrameUpdate();
		virtual void				iPostFrameUpdate();
		virtual void				iPostSceneLoad();
		virtual EventReturnType_e	iExecuteEvent(const BCEvent& tEvent);
		virtual EventReturnType_e	iProcessEvent(const BCEvent& tEvent);
		virtual std::string		iPreLoadWebPage(const std::string& strOriUrl, const std::string& strConcatUrl);
		virtual void				iSetExtraParam(const std::string& strKey, INT_PTR pValue);
		virtual INT_PTR		iGetExtraParam(const std::string& strKey);
		virtual void				iAddQtVsConnectFunc(QtVsConnectFunc fn);
		virtual void				iRemoveQtVsConnectFunc(QtVsConnectFunc fn);
		virtual void				iExecuteQtVsCommond(const std::string& strCommond, void* pInData, void* pOutData);
		virtual bool				iCanHandleScene(const Vector2& vScreenPos);
		virtual bool				iCanRespondKey();
		virtual void				iSaveLog(ELogType eType, const std::string& strMsg);
		virtual INode*			iGetProjectGroupNode();

		virtual void				iRefreshData();																		//网页刷新
		virtual void				iOnLayerLoaded() {}																//图层获取成功

	public:
		void							AddWebCommond(WebCommandData& stWebCommond,bool bNextFrame = true);
		virtual void				RequestHeartbeat();		//心跳
		virtual std::string		GetVersion();			//获取版本
		std::string					m_strApiHost;			//服务器地址

		//wgs84 经纬度转换
		virtual void				ConvertLngToVector3(const double dlng, const double dlat, Vector3d* vecPos, bool bUseTif = true);
		//多种 经纬度转换
		virtual void				ConvertToVector3WithCoorSystem(const double dlng, const double dlat, Vector3d* vecPos,
			EElementCoordinateType eType, bool bUseTif = true);
		//通过自身渲染地图（建模渲染）来转换坐标
		virtual Vector3d			ConvertLngLatFromSelfMap(double dLng, double dLat, double dZ, bool bUserTif = true);			
		//场景坐标转WGS
		virtual void				ScenePosToWGS(Vector3d& vPos, double& dlng, double& dlat);			
		virtual void				WGS_TO_GCJ02(double dlng, double dlat, double& outX, double& outY);	
		virtual void				GCJ02_to_WGS(double dlng, double dlat, double& outX, double& outY);		
		virtual void				WGS_TO_CGCS2000(double dlng, double dlat, double& outX, double& outY);		
		virtual void				CGCS2000_to_WGS(double dlng, double dlat, double& outX, double& outY);		
		//经纬度获取高地图
		virtual float				GetSceneZFromLngLat(double dLng, double dLat);													
		virtual float				GetZFromTif(Vector3d& vPos);			
		virtual void				GetZFromTif(std::vector<Vector3d> &vecPos,std::vector<double> &vecHeight);
		virtual void				AdjustZ(Vector3d &vPos,float fZ,bool bAdd);	//调整Z值
		virtual int					LoadTif(vector<Vector3d>& vecHeight, double* dLeftTopCoord, double* dRightBtmCoord);
		//增加统一的网络请求参数
		virtual void				AddHttpCommonParam(ThreadRequestParam* pRequestParam);			
		//向网页发送数据和命令
		virtual void				ToSendWebCommond(std::string strFunName, std::string strWebId, std::string strRealData);			
		//设置是否显示操作轴（Builder里的移动之类的轴）
		void							SetAxisToolEnable(bool bEnable) { m_bAxisToolEnable = bEnable; }
		void						    RequestSubscribeBindIp();			//调用后台订阅绑定接口

		INode*						iGetApiGroupNode() { return m_pApiGroupNode; }
		void							AddRefreshFontBoardID(const std::string &strID);

	private:
		static void					CB_DataCome(INT_PTR wParam, INT_PTR lParam);
		static void					static_pre_request_heartbeat(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam_s* pParam);
		static void					static_request_heartbeat_callback(ThreadCallbackParam_s* pParam);
		static void					static_request_CloudLogin_callback(ThreadCallbackParam_s* pParam);

	protected:
		virtual void				iDoWebCommond(WebCommandData* pWebData);
		//获取udp
		virtual bool				iSetUdpParam(UdpParam* pUdpParam);
		//处理udp数据
		virtual void				iDoUdpData(int nCommond, char* pData) {}
		//处理订阅websocket数据
		virtual void				iDoSubscribeWebsocketData(const char* pData) {}
		//鼠标单击点击事件，做了位置偏差判断
		virtual void				iMouseClick(Vector3d vPos) {}

		void							parseCommand(const char* strJsonData);	//解析web端json命令
		void							getConfigInfoFromNet();                //从云端获取配置信息，一般在开机之初调用
		virtual void				iReset();
		virtual void				requestRoamPath();						//请求漫游路径

	public:
		//点击
		//自身
		virtual void				iOnHCLabelClickLabel(FactotryAssembleDetail* pAssemble, CommonLabelFactory* pFactory, bool bClick);
		virtual void				iOnHCDetailClickLabel(FactotryAssembleDetail* pAssemble, CommonLabelFactory* pFactory, std::string strFlagName,bool bClick);
		//web api上图的
		virtual void				iOnWebLabelClickLabel(FactotryAssembleDetail* pAssemble, CommonLabelFactory* pFactory, bool bClick);
		virtual void				iOnWebDetailClickLabel(FactotryAssembleDetail* pAssemble, CommonLabelFactory* pFactory, std::string strFlagName, bool bClick);

		//视频巡控
		virtual DefaultVideoInspectionFactory* iCreateVideoInspectionFactory();
		virtual void				iOnSetVideoInspectionParam(const std::string& strKey, BCVideoRouterPointInfo* pInfo, VideoBoardWithButtonParam* pParam) {}
		virtual void				iOnVideoInspectionToScenePos(const double& dLng, const double& dLat, const double& dZ, Vector3d& vOutPos, Vector3d& vOutAngle);
		virtual void				iOnVideoInspectionLoadedData(Json::Value jRetData);
		virtual bool				iOnChangeVideoPoint(BCVideoRouterPointInfo* pInfo, bool bOpen) { return true; }
		void	ToWebAllVideoInspection(std::string strWebId);			//发送所有视频巡控

		//热力图
		virtual HeatmapFactory* iCreateHeatMapFactory();

		//室内分层
		virtual BuildIndoorFactory* iCreateBuildingIndoorFactory();
		virtual void	iOnBuildingDataLoadedData();		//建筑分层数据请求完成
		void	ToWebAllBuilding(std::string strWebId);		//发送所有的楼栋信息
		static bool	static_compareFloorIndex(const FloorData* t1, const FloorData* t2);
		Json::Value	dataToIndoorJsonValue(FloorData* pFloorData);							// 组装JSON信息

		//订阅类
		virtual SceneElementLoader* iCreateSceneElementLoader();
		void ToWebAllLayer(std::string strWebId);			//发送所有的图层
		void	DoOnSceneElementLoadedData(bool bAllComplete);
		virtual void	iOnSceneElementLoadedData(bool bAllComplete) {}

		//漫游
		void ToWebAllRoamPath(std::string strWebId);			//发送所有漫游路径

		//工具
		virtual CircleStatisticsTool* iCreateCircleStatisticsTool();
		virtual CircleStatisticsTool* iCreatePOICircleStatisticsTool();
		virtual LineStatisticsTool* iCreateLineStatisticsTool();
		virtual PolygonStatisticsTool* iCreatePolygonStatisticsTool();
		virtual PolygonStatisticsTool* iCreatePolygonStatisticsRectangularTool();
		virtual AreaMeasureTool* iCreateAreaMeasureTool();
		virtual DistanceMeasureTool* iCreateDistanceMeasureTool();
		virtual SightAnalysisTool* iCreateSightAnalysisTool();
		static void static_callback_statistics(BaseStatisticsTool* pTool, const Json::Value& jResult,
			std::vector<StatisTaskData*> vecFilterStatisTask, const bool bCancel);	//统计回调
		static void static_area_measure(INode* pPolygon, std::map<string, INT_PTR> mapINTPTR, float fResult);//面积测量回调
		static void static_distance_measure(INode* pLocusLine, std::map<string, INT_PTR> mapINTPTR, std::vector<float> vecResult);//距离测量回调
		static void static_callback_sight_analysis(INode* pLineNode, SightAnalysisTool* pTool, Json::Value jResult);
		
		virtual void	iOnStatisticsComplete(BaseStatisticsTool* pTool, const Json::Value& jResult,
			std::vector<StatisTaskData*>& vecFilterStatisTask, const bool bCancel);

		//Web API
		WebCommonApi* m_pWebCommonApi;				// API接口
		virtual WebLabelParser* iCreateWebLabelParser();
		void sendCameraViewport(std::string& strWebId, const bool bForce);
		std::string	 m_strListenCameraViewportWebId;
		Vector3d m_vLastCameraViewport;			// 相机上次的位置
		Vector3d m_vLastCameraAngle;				// 相机上次的角度

		void sendListenViewShedState();
		std::string					m_strListenViewShedStateWebId;
		ViewshedState			m_vPreviousViewshedState;		// 记录上一次的视域分析状态
		void sendListenMouseWheel(float fValue);
		void sendListenMouseLB(int nState, Vector3d vPos);
		void sendListenMouseRB(int nState, Vector3d vPos);
		void sendListenMouseMovePos(Vector3d vPos);
		std::string					m_strListenMouseMovePosWebId;
		std::string					m_strListenMouseWheelId;
		std::string					m_strListenMouseRBWebId;
		std::string					m_strListenMouseLBWebId;
		Vector3d					m_vLastMouseMovePos;			// 鼠标上次移动位置
		Vector3d					m_vLastMouseDownPos;			// 鼠标左键按下的位置
		Vector3d					m_vCurMouseDownPos;				// 鼠标当前的位置
		std::string					m_strListenMouseClickPosWebId;
		std::string					m_strGetElevationPointWebId;	//制高点网页回调id
		std::string					m_strListenMouseLDBWebId;		//鼠标左键双击
		static bool	static_request_getElevationPoint(map<string, INT_PTR>& mapParams, ThreadCallbackParam* stCallbackParam);//制高点分析
		void removeElevationPointDuplicate(Vector3d& vCenterPos, float& fRadius, float& fIgnoreRange, std::vector<Vector3d>& vecSrc,
			std::vector<Vector3d>& vecOut, int nCount);  //制高点去重
		std::vector<ListenElementParam> m_vecListenElement;			//当前监听的元素
		std::string					m_strListenElementWebId;


	public:
		ProjectBasedUserInfo			m_sUserInfo;									//用户信息
		std::string								m_strProjectPath;							//项目resource路径，从resource开头
		int										m_nAppMode;									//app模式					

		Vector3d								m_vClusterSceneTopLeft;				//聚合左上角场景坐标
		Vector3d								m_vClusterSceneBottomRight;		//聚合右下角场景坐标
		std::vector<ClusterFunc::ClusterLevel> m_vecClusterLevel;		//聚合分层level
		SceneElementLoader* m_pSceneElementLoader;			//订阅上图
		std::string								m_strLivegbsChannel;						//livegbs 渠道号
		std::string								m_strLivegbsHost;							//livegbs host
		HeatmapFactory*				m_pHeatMapFactory;						//热力图
		BCModuleManager				m_stBCModuleManager;				//公共模块管理
		WebLabelParser*					m_pWebLabelParser;						//JS API类
		Json::Value							m_jStatisticTemplate;						// 统计模板
		std::vector<std::string>		m_vecStatisticLayer;						// 需要统计的图层
		CircleStatisticsTool*				m_pPOICircleStatisticsTool;				//POI点周边资源拾取
		CircleStatisticsTool*				m_pCircleStatisticsTool;					//圆形统计
		LineStatisticsTool*				m_pLineStatisticsTool;						//线性统计
		PolygonStatisticsTool*		m_pPolygonStatisticsTool;				//多边形统计
		PolygonStatisticsTool*		m_pPolygonStatisticsRectangularTool;	//多边形统计 特例 矩形统计
		AreaMeasureTool*				m_pAreaMeasureTool;					//面积测量
		DistanceMeasureTool*		m_pDistanceMeasureTool;				//距离测量
		ViewshedAnalysisTool*		m_pViewshedAnalysisTool;				//视域分析
		BuildIndoorFactory*		m_pBuildingIndoorFactory;				//建筑分层
		DefaultVideoInspectionFactory* m_pVideoInspectionFactory;	//视频巡控
		SightAnalysisTool*				m_pSightAnalysisTool;					//通视分析
		bool										m_bEnterAreaScene;						// 室内分层是否进入单独的区域场景
		LayerDebugSceneLoader*  m_pLabelRegulator;							//标签调试工具				
		std::vector<BaseStatisticsTool*> m_vecStatisticsTool;

	protected:
		bool										m_bLabelDebug;							//是否启动标签调试工具
		BCMutexLock						m_MutexLock;
		std::queue<WebCommandData>  m_queueWebCommand;				//web端的命令
		BCMutexLock						m_MutexSubcribeLock;
		std::queue<std::string*>		m_queueSubscribeData;							//订阅的消息
		ISocketConnector*				m_pConnector;
		INetworkCommand*			m_pNetCommand;
		UdpParam							m_stUdpParam;						//udp的数据
		bool										m_bRequestHeartbeat;			//是否心跳接口请求
		bool										m_bNeedKillProcess;				//是否需要认证失败后杀掉进程
		std::string								m_strVersion;							//版本
		KMapAdapter*					m_pMapAdapter;
		std::string								m_strHeartbeatUrl;					//心跳地址
		std::string								m_strRoamUrl;							//漫游接口地址
		std::string								m_strBuildingUrl;						//建筑分层接口地址
		std::string								m_strVideoInspectionUrl;		//视频巡控接口地址

		//坐标转换
		Vector2						m_vLngLatLeftTop;			//左上角经纬度
		Vector2						m_vLngLatRightBottom;		//右下角经纬度
		Vector2						m_vScenePosLeftTop;			//左上角场景坐标
		Vector2						m_vScenePosRightBottom;		//右下角场景坐标

		Vector2						m_vSelfLngLatLeftTop;		//自身正射影像地图左上角经纬度
		Vector2						m_vSelfLngLatRightBottom;	//自身正射影像地图右下角经纬度
		Vector2						m_vSelfScenePosLeftTop;		//自身正射影像地图左上角场景坐标
		Vector2						m_vSelfScenePosRightBottom;	//自身正射影像地图右下角场景坐标
		int								m_nUTM_project_zone;		//投影带
		float								m_fCGCS2000CentralMeridian; //地区中央子午线
		std::string						m_strTifFilePath;				//高地图路径(绝对路径)
		std::string						m_strProjectConfigFilePath;			//项目配置文件(config.json)路径，从resource开始
		bool								m_bAxisToolEnable;			//设置模型移动，旋转等工具轴的显隐
		Json::Value					m_jWebRoamPath;				// 给到网页漫游路径
		std::string						m_strConcatWebUrl;


		void loadTifLevel(const std::string &strPath);
	private:
		std::string												m_strEngineVersion;							//引擎版本
		std::vector<QtVsConnectFunc>			m_vecQtVsConnectFunc;
		std::map<std::string, INT_PTR>			m_mapExtraParam;
		bool														m_bKillProcess;					//杀掉进程
		bool														m_bCameraInertialEnable;		//相机是否可惯性
		std::vector<TifLevel>							m_vecTifLevel;					//多张高地图
		INode*													m_pProjectGroupNode;			//业务的所有节点放到此Group里
		std::vector<WebCommandData>		m_vecNextFrameWebComondData;	//下一帧需要添加到队列的网页命令
		INode*													m_pApiGroupNode;			//api第三方
		std::vector<std::string>						m_vecNeedRefreshFontBoard;		//需要更新文字的FontBaord id
	};
}

