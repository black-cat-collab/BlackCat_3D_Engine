#pragma once

#include "Project-Based/utils/pLocal.h"

namespace bc
{
#pragma pack(push,1)	

#define EFFECT_TYPE_CONE_TRIANGLE std::string("effect_cone_triangle")  //三角锥形上下浮动效果
#define EFFECT_TYPE_CONE_ROUND std::string("effect_cone_round")  //圆形锥形上下浮动效果

	typedef enum EPosition
	{
		POSITION_IN_ROOM,			//室内
		POSITION_OUT_ROOM,			//室外
	}EPosition_e;

	typedef enum EJumpType
	{
		JUMP_OVER_LOOK,			//鸟瞰方式
		JUMP_NONE,				//无跳转
	}EJumpType_e;

	typedef enum EPoliceType
	{
		POLICE_MAN = 1,			//警员
		POLICE_CAR,				//警车
	}EPoliceType_e;
	//对外发送事件
	typedef enum EEventMessageType
	{
		PROJECTBASED_EVENT_SCENE_ELEMENT_DATA_REFRESHED = EVENT_TYPE_END + 1,		//上图工具数据刷新完
		PROJECTBASED_EVENT_PROJECT_CONFIG_REFRESH,		//配置数据刷新完
		PROJECTBASED_EVENT_PROJECT_WEB_COMMOND,		//web端的信息
		PROJECTBASED_EVENT_WEB_LOADED,				//网页加载成功
		PROJECTBASED_EVENT_UDP_DATA,					//UDP数据
		PROJECTBASED_EVENT_MOUSE_CLICK,				//鼠标点击
		PROJECTBASED_EVENT_APP_MODE_CHANGED,			//app模式改变
		PROJECTBASED_EVENT_APPID_CHANGED,			//appid 改变
		PROJECTBASED_EVENT_ONLY_LAYER_LOADED,		//仅仅图层数据加载完，可以处理一些跟账号无关的事情
		PROJECTBASED_EVENT_HC_LABEL_CLICK,		//自身标签点击
		PROJECTBASED_EVENT_HC_DETAIL_CLICK,		//自身标签详情面板按钮点击
		PROJECTBASED_EVENT_SCENE_MODE_SWITCH,		//场景模式变化，GIS球和平面

		PROJECTBASED_EVENT_END,

	}EventMessageType_e;

	typedef enum EElementCoordinateType
	{
		COOR_WGS84,
		COOR_GCJ02,
		COOR_SCENE_LOCAL,
		COOR_CGCS2000,
		COOR_BD09,
		COOR_WGS84_SELF,		//自身坐标
	}ElementCoordinateType_e;

	class WebCommandData
	{
	public:
		std::string strCommand;
		std::vector<Json::Value> vecValues;
		std::string strID;

		WebCommandData()
		{
			strCommand = "";
			vecValues.resize(0);
			strID = "";
		}

		class WebCommandData& operator = (const class WebCommandData& src)
		{
			strCommand = src.strCommand;
			vecValues = src.vecValues;
			strID = src.strID;

			return *this;
		}

	};

//设备信息基类
typedef struct ProjectBasedDeviceInfo
{
	int nFloorIndex;
	int nType;
	char szViewPoint[128];
	char szPosition[128];
	Vector3d vOriginPos;
	Vector3d vOriginAngle;
	Vector3d vCurrentPos;
	Vector3d vCurrentAngle;
	char szCameraID[80];

	ProjectBasedDeviceInfo()
	{
		nFloorIndex = -1;
		nType = 0;
		memset(szViewPoint, 0, sizeof(szViewPoint));
		memset(szPosition, 0, sizeof(szPosition));
		memset(szCameraID, 0, sizeof(szCameraID));
		vOriginPos = Vector3d(0, 0, 0);
		vOriginAngle = Vector3d(0, 0, 0);
		vCurrentPos = Vector3d(0, 0, 0);
		vCurrentAngle = Vector3d(0, 0, 0);
	}

	struct ProjectBasedDeviceInfo& operator = (const struct ProjectBasedDeviceInfo &src)
	{
		nFloorIndex = src.nFloorIndex;
		nType = src.nType;
		memcpy(szViewPoint, src.szViewPoint, sizeof(szViewPoint));
		memcpy(szPosition, src.szPosition, sizeof(szPosition));
		memcpy(szCameraID, src.szCameraID, sizeof(szCameraID));

		vOriginPos = src.vCurrentPos;
		vOriginAngle = src.vOriginAngle;
		vCurrentPos = src.vCurrentPos;
		vCurrentAngle = src.vCurrentAngle;

		return *this;
	}
}IProjectBasedDeviceInfo;

//	摄像头信息
typedef struct ProjectBasedCameraInfo : IProjectBasedDeviceInfo
{

	int nId;
	int nType;					// 摄像头类型，根据具体项目自定义
	char szName[256];
	char szIP[80];
	char szPort[10];
	char szUser[80];
	char szPassword[80];
	char szCameraID[80];
	std::string strVideoPath;
	int nSip;
	double dLng;
	double dLat;
	int nIsShow;
	char szArea[120];
	char szFloorNodeID[40];		//	楼层节点ID
	int nOnline;				//	是否在线  1:在线   0：离线
	int nAreaID;				//	区域id

	ProjectBasedCameraInfo()
	{
		IProjectBasedDeviceInfo();
		nId = 0;
		nType = 0;
		memset(szName, 0, sizeof(szName));
		memset(szIP, 0, sizeof(szIP));
		memset(szPort, 0, sizeof(szPort));
		memset(szUser, 0, sizeof(szUser));
		memset(szPassword, 0, sizeof(szPassword));
		memset(szCameraID, 0, sizeof(szCameraID));
		memset(szArea, 0, sizeof(szArea));
		memset(szFloorNodeID, 0, sizeof(szFloorNodeID));
		nSip = 0;
		strVideoPath = "";
		nIsShow = 1;
		nOnline = 1;
		nAreaID = 0;
	}

	struct ProjectBasedCameraInfo& operator = (const struct ProjectBasedCameraInfo &src)
	{
		IProjectBasedDeviceInfo::operator=(src);
		nId = src.nId;
		nType = src.nType;
		memcpy(szName, src.szName, sizeof(szName));
		memcpy(szIP, src.szIP, sizeof(szIP));
		memcpy(szPort, src.szPort, sizeof(szPort));
		memcpy(szUser, src.szUser, sizeof(szUser));
		memcpy(szPassword, src.szPassword, sizeof(szPassword));
		memcpy(szCameraID, src.szCameraID, sizeof(szCameraID));
		memcpy(szArea, src.szArea, sizeof(szArea));
		memcpy(szFloorNodeID, src.szFloorNodeID, sizeof(szFloorNodeID));
		nSip = src.nSip;
		strVideoPath = src.strVideoPath; 
		dLng = src.dLng;
		dLat = src.dLat;
		nOnline = src.nOnline;
		nIsShow = src.nIsShow;
		nAreaID = src.nAreaID;

		return *this;
	}
}IProjectBasedCameraInfo;

// 标签基类
typedef struct ProjectBasedLabel
{
	int					nType;			//标签类型
	double				dJd;			//经度
	double				dWd;			//纬度
	std::string				strID;			//唯一标识
	std::string				strTime;		//时间
	Vector3d				vOrigin;		//标签位置
	Vector3d				vLastOrigin;	//上一次的位置

	ProjectBasedLabel()
	{
		vOrigin = Vector3d(0, 0, 0);
		vLastOrigin = Vector3d(0, 0, 0);
		dJd = 0;
		dWd = 0;
		strTime.clear();
	}

	struct ProjectBasedLabel& operator = (const struct ProjectBasedLabel &src)
	{
		nType = src.nType;
		vOrigin = src.vOrigin;
		vLastOrigin = src.vLastOrigin;
		dJd = src.dJd;
		dWd = src.dWd;
		strTime = src.strTime;
		return *this;
	}

}IProjectBasedLabel;

// 警员
typedef struct ProjectBasedPoliceInfo : IProjectBasedLabel
{
	//nType: 1--警员    2--警车
	std::string strSerial;		//类型（比如车类型）
	std::string strName;			//姓名
	std::string strSex;			//性别
	std::string strMz;			//名族
	std::string strNumber;		//警号
	std::string strPost;			//职务
	std::string strIdPhoto;		//证件照
	std::string strPlace;		//地点
	std::string strDeviceId;		//设备编号
	std::string strUnit;			//分局
	std::string strDepartment;	//部门
	std::string IDCardNumber;	//身份证号码
	std::string strTelNum;		//电话
	std::string strCarNum;		//车牌
	std::string strUID;		//唯一标识
	std::string strState;    //目前状态 
	long lLastTime;		//最后一次在线时间
	bool bOnline;		//是否在线

	ProjectBasedPoliceInfo()
	{
		IProjectBasedDeviceInfo();
		nType = 1;		//默认警员
		lLastTime = time(NULL);
		bOnline = true;
	}

	struct ProjectBasedPoliceInfo& operator = (const struct ProjectBasedPoliceInfo &src)
	{
		ProjectBasedLabel::operator=(src);

		strSerial = src.strSerial;
		strName = src.strName;
		strNumber = src.strNumber;
		strUnit = src.strUnit;
		strDepartment = src.strDepartment;
		IDCardNumber = src.IDCardNumber;
		strTelNum = src.strTelNum;
		strCarNum = src.strCarNum;
		strSex = src.strSex;
		strMz = src.strMz;
		strDeviceId = src.strDeviceId;
		lLastTime = src.lLastTime;
		bOnline = src.bOnline;

		return *this;
	}

}IProjectBasedPoliceInfo;

// 警情
typedef struct ProjectBasedWarningInfo : IProjectBasedLabel
{
	bool bStaue;				//告警状态
	std::string strWaringTime;		//告警时间
	std::string strWaringType;		//告警类型
	std::string strWaringText;		//告警内容
	std::string strName;				//告警人姓名
	std::string strCall;				//告警人电话
	std::string strPlace;			//告警地点

	ProjectBasedWarningInfo()
	{
		IProjectBasedDeviceInfo();
		bStaue = false;
	}

	struct ProjectBasedWarningInfo& operator = (const struct ProjectBasedWarningInfo &src)
	{
		nType = src.nType;
		strWaringTime = src.strWaringTime;
		bStaue = src.bStaue;
		strWaringType = src.strWaringType;
		strWaringText = src.strWaringText;
		strName = src.strName;
		strCall = src.strCall;
		strPlace = src.strPlace;

		return *this;
	}

}IProjectBasedWarningInfo;

//移动警力信息
//typedef struct ProjectBasedPoliceInfo : IProjectBasedDeviceInfo
//{
//	int nId;
//	int nAge;
//	int nSex;
//	char szName[128];			//姓名或负责人
//	char szCallNo[30];			//对讲机
//	int  nType;					//警种
//	char szPic[256];			//图片
//	char szLocalPicPath[256];	//本地下载图片
//	char szDeviceCode[20];		//设备编号
//	char szReceiveTime[20];		//接收时间
//	char szUnit[128];			//单位
//	char szType[20];			//终端类型
//	char szDepartment[60];		//部门
//	char szStation[60];			//岗位
//	char szTerminal[20];		//终端号	
//	char szPoliceNo[20];		//警号
//	char szMobile[30];			//手机号
//	char szCarNumber[20];		//车牌号
//	double dLng;				//经度
//	double dLat;				//纬度
//	Vector3d vCurrentOrigin;
//	Vector3d vCurrentAngle;
//	int nSeraialType;			// 1:警员    2：警车
//	INode *pNode;
//	long lPreTime;				//上次更新数据的时间
//	bool bOnline;				//是否在线
//
//	ProjectBasedPoliceInfo()
//	{
//		IProjectBasedDeviceInfo();
//		nId = 0;
//		nAge = 0;
//		nSex = 0;
//		memset(szName, 0, sizeof(szName));
//		memset(szUnit, 0, sizeof(szUnit));
//		memset(szMobile, 0, sizeof(szMobile));
//		memset(szPoliceNo, 0, sizeof(szPoliceNo));
//		memset(szCallNo, 0, sizeof(szCallNo));
//		memset(szPic, 0, sizeof(szPic));
//		memset(szLocalPicPath, 0, sizeof(szLocalPicPath));
//		memset(szCarNumber, 0, sizeof(szCarNumber));
//		memset(szDeviceCode, 0, sizeof(szDeviceCode));
//		memset(szReceiveTime, 0, sizeof(szReceiveTime));
//		memset(szDepartment, 0, sizeof(szDepartment));
//		memset(szStation, 0, sizeof(szStation));
//		memset(szTerminal, 0, sizeof(szTerminal));
//		vCurrentOrigin = Vector3d(0, 0, 0);
//		vCurrentAngle = Vector3d(0, 0, 0);
//
//		nType = 0;
//		dLng = 0;
//		dLat = 0;
//		nSeraialType = 1;
//		pNode = NULL;
//		lPreTime = time(NULL);
//		bOnline = true;
//	}
//
//	struct ProjectBasedPoliceInfo& operator = (const struct ProjectBasedPoliceInfo &src)
//	{
//		IProjectBasedDeviceInfo::operator=(src);
//		nId = src.nId;
//		nId = src.nAge;
//		nId = src.nSex;
//		memcpy(szName, src.szName, sizeof(szName));
//		memcpy(szUnit, src.szUnit, sizeof(szUnit));
//		memcpy(szMobile, src.szMobile, sizeof(szMobile));
//		memcpy(szPoliceNo, src.szPoliceNo, sizeof(szPoliceNo));
//		memcpy(szCallNo, src.szCallNo, sizeof(szCallNo));
//		memcpy(szPic, src.szPic, sizeof(szPic));
//		memcpy(szLocalPicPath, src.szLocalPicPath, sizeof(szLocalPicPath));
//		memcpy(szCarNumber, src.szCarNumber, sizeof(szCarNumber));
//		memcpy(szDeviceCode, src.szDeviceCode, sizeof(szDeviceCode));
//		memcpy(szReceiveTime, src.szReceiveTime, sizeof(szReceiveTime));
//		memcpy(szDepartment, src.szDepartment, sizeof(szDepartment));
//		memcpy(szStation, src.szStation, sizeof(szStation));
//		memcpy(szTerminal, src.szTerminal, sizeof(szTerminal));
//		nType = src.nType;
//		dLng = src.dLng;
//		dLat = src.dLat;
//		vCurrentOrigin = src.vCurrentOrigin;
//		vCurrentAngle = src.vCurrentAngle;
//		nSeraialType = src.nSeraialType;
//		pNode = src.pNode;
//		lPreTime = src.lPreTime;
//		bOnline = src.bOnline;
//
//		return *this;
//	}
//}IProjectBasedPoliceInfo;


// 视域分析工具显示
typedef struct ProjectBasedViewShedAnalysisTool
{

	bool bActive;
	bool bEnable;

	ProjectBasedViewShedAnalysisTool()
	{
		bActive = false;
		bEnable = false;
	}

	struct ProjectBasedViewShedAnalysisTool& operator = (const struct ProjectBasedViewShedAnalysisTool &src)
	{
		bEnable = src.bEnable;

		return *this;
	}
}IProjectBasedViewShedAnalysisTool;

typedef struct ProjectBasedSceneClusterLevel
{
	int nIndex;
	int nRow;
	int nColumn;
	float fDistance;
}IProjectBasedSceneClusterLevel;

// 工具上图统一类型信息结构体
typedef struct ProjectBasedSceneElementTypeInfo
{
	int nTypeID;			//类型id
	std::string strName;			//名称
	std::string strInnerName;	//名称标识
	int nUseType;			//使用类型
	int nLayerID;			//图层id
	int nElemType;			//类型 1：点   2：线   3：面/区域
	std::vector<std::string> vecStrLabelStyle;		//风格（节点的属相json）
	std::string strDetailStyle;	//详情风格（节点的属相json）
	std::string strClusterStyle;	//聚合风格（节点的属相json）
	std::string strOverTapStyle;	//重叠风格（节点的属相json）
	float fOffsetDetailX;	//详情面板距标签的X偏移
	float fOffsetDetailY;	//详情面板距标签的Y偏移
	std::map<std::string,std::string> mapLabelDataMatch;	//标签三维和数据的匹配（三维字段=数据字段_数据类型;三维字段=数据字段,子字段_数据类型）,例如：title=name_string;address=addr,name_string
	std::map<std::string, std::string> mapDetailDataMatch;	//详情三维和数据的匹配（三维字段=数据字段_数据类型;三维字段=数据字段,子字段_数据类型）,例如：title=name_string;address=addr,name_string
	ProjectBasedSceneElementTypeInfo* pBaseParent;		//若是子类，则指向基础类
	int nIndoor;			//是否室内   1:室内    0:室外
	Json::Value j3dOption;	//3d配置信息

	ProjectBasedSceneElementTypeInfo()
	{
		nTypeID = -1;
		nUseType = -1;
		nLayerID = -1;
		nElemType = -1;
		fOffsetDetailX = 0.0f;
		fOffsetDetailY = 0.0f;
		pBaseParent = nullptr;
		nIndoor = 0;
		j3dOption = Json::objectValue;
	}

	struct ProjectBasedSceneElementTypeInfo& operator = (const struct ProjectBasedSceneElementTypeInfo &src)
	{
		nTypeID = src.nTypeID;
		strName = src.strName;
		strInnerName = src.strInnerName;
		nUseType = src.nUseType;
		nLayerID = src.nLayerID;
		nElemType = src.nElemType;
		vecStrLabelStyle = src.vecStrLabelStyle;
		strDetailStyle = src.strDetailStyle;
		strClusterStyle = src.strClusterStyle;
		strOverTapStyle = src.strOverTapStyle; 
		fOffsetDetailX = src.fOffsetDetailX;
		fOffsetDetailY = src.fOffsetDetailY;
		mapLabelDataMatch = src.mapLabelDataMatch;
		mapDetailDataMatch = src.mapDetailDataMatch;
		pBaseParent = src.pBaseParent;
		nIndoor = src.nIndoor;
		j3dOption = src.j3dOption;

		return *this;
	}

	Json::Value Get3DOptionAttr(std::string strKey)
	{
		try
		{
 			if (j3dOption.isObject() && j3dOption.isMember(strKey))
			{
				return j3dOption[strKey];
			}
		}
		catch (const std::exception&)
		{

		}
		return Json::nullValue;
	}

	std::string Get3DOption_detailOffset()
	{
		try
		{
			Json::Value jValue = Get3DOptionAttr("pos_offset");
			if (jValue.isString())
			{
				return jValue.asString();
			}
		}
		catch (const std::exception&)
		{

		}
		return "";
	}

	//是否平滑移动
	bool Get3DOption_Smooth_Move()
	{
		try
		{
			Json::Value jValue = Get3DOptionAttr("smooth_move");
			if (jValue.isBool())
			{
				return jValue.asBool();
			}
		}
		catch (const std::exception&)
		{

		}
		return false;
	}

	//当两点平滑移动所需时间超过这个值则直接跳过（秒），只有bSmoothMove=true生效
	int Get3DOption_SeondToIgnorePoint()
	{
		try
		{
			Json::Value jValue = Get3DOptionAttr("smooth_ignore_time");
			if (jValue.isInt())
			{
				return jValue.asInt();
			}
		}
		catch (const std::exception&)
		{

		}
		return 2;
	}

	float Get3DOption_Smooth_Speed()
	{
		try
		{
			Json::Value jValue = Get3DOptionAttr("smooth_speed");
			if (jValue.isDouble())
			{
				return jValue.asFloat();
			}
		}
		catch (const std::exception&)
		{

		}
		return 60.0f;
	}

	float Get3DOption_OverTap_JudgeDis()
	{
		try
		{
			Json::Value jValue = Get3DOptionAttr("overtap_judge_dis");
			if (jValue.isDouble())
			{
				return jValue.asFloat();
			}
		}
		catch (const std::exception&)
		{

		}
		return 2.0f;
	}

	bool Is3DOption_OverTap_Enable()
	{
		try
		{
			Json::Value jValue = Get3DOptionAttr("is_overtap_enable");
			if (jValue.isBool())
			{
				return jValue.asBool();
			}
		}
		catch (const std::exception&)
		{

		}
		return true;
	}

	//是否分摊读取高地图
	bool Is3DOption_ReadHeightTif_Frame()
	{
		try
		{
			Json::Value jValue = Get3DOptionAttr("is_readTif");
			if (jValue.isBool())
			{
				return jValue.asBool();
			}
		}
		catch (const std::exception&)
		{

		}
		return true;
	}

	//聚合参数
	Json::Value Get3DOption_Cluster()
	{
		try
		{
			Json::Value jValue = Get3DOptionAttr("cluster");
			return jValue;
		}
		catch (const std::exception&)
		{

		}
		return Json::nullValue;
	}

	//聚合参数
	bool Is3DOption_Cluster_Enable()
	{
		try
		{
			Json::Value jValue = Get3DOptionAttr("cluster");
			if (jValue.isObject())
			{
				return jValue["enable"].asBool();
			}
		}
		catch (const std::exception&)
		{

		}
		return true;
	}

	//聚合参数
	bool Is3DOption_Cluster_WGS84()
	{
		try
		{
			Json::Value jValue = Get3DOptionAttr("cluster");
			if (jValue.isObject())
			{
				return jValue["is_wgs84"].asBool();
			}
		}
		catch (const std::exception&)
		{

		}
		return false;
	}

	//解析聚合参数
	bool Parse3DOption_Cluster(std::vector<ProjectBasedSceneClusterLevel> &vecLevel,Vector3d &vLeftTop,Vector3d &vRightBottom)
	{
		try
		{
			Json::Value jCluster = Get3DOption_Cluster();
			if (jCluster.isObject())
			{
				bool bEnable = jCluster["enable"].asBool();
				if (!bEnable)
				{
					return false;
				}
				std::string strLeftTop = jCluster["left_top"].asString();
				std::string strRightBottom = jCluster["right_bottom"].asString();
				sscanf_s(strLeftTop.c_str(), "%lf,%lf", &vLeftTop.x, &vLeftTop.y);
				sscanf_s(strRightBottom.c_str(), "%lf,%lf", &vRightBottom.x, &vRightBottom.y);
				Json::Value& jLevel = jCluster["level"];
				for (int i = 0; i < jLevel.size(); ++i)
				{
					ProjectBasedSceneClusterLevel stLevel;
					stLevel.fDistance = jLevel[i]["distance"].asFloat();
					stLevel.nIndex = jLevel[i]["index"].asInt();
					stLevel.nRow = jLevel[i]["row"].asInt();
					stLevel.nColumn = jLevel[i]["column"].asInt();

					vecLevel.push_back(stLevel);
				}

				return true;
			}
		}
		catch (const std::exception&)
		{
			return false;
		}
		return true;
	}

	//是否显示详情面板
	bool Is3DOption_Detail_Label()
	{
		try
		{
			Json::Value jValue = Get3DOptionAttr("is_detail_label");
			if (jValue.isBool())
			{
				return jValue.asBool();
			}
		}
		catch (const std::exception&)
		{

		}
		return true;
	}

	//是否支持反选
	bool Is3DOption_Revert_Click()
	{
		try
		{
			Json::Value jValue = Get3DOptionAttr("is_revert_click");
			if (jValue.isBool())
			{
				return jValue.asBool();
			}
		}
		catch (const std::exception&)
		{

		}
		return true;
	}

	std::string  PrintOption3DTemplate()
	{
		Json::Value jObject;
		jObject["pos_offset"] = "0,50.0";
		jObject["smooth_move"] = true;
		jObject["smooth_ignore_time"] = 2;
		jObject["smooth_speed"] = 60.0f;
		jObject["overtap_judge_dis"] = 1.0f;
		jObject["is_overtap_enable"] = true;
		jObject["is_readTif"] = true;
		jObject["is_revert_click"] = true;		//是否反选
		{
			Json::Value jCluster = Json::objectValue;
			jCluster["left_top"] = "2450.5,14520.6";
			jCluster["right_bottom"] = "-5214.5.5,-24520.6";
			jCluster["enable"] = true;
			jCluster["is_wgs84"] = false;
			Json::Value jLevelArray = Json::arrayValue;
			Json::Value jLevel_1 = Json::objectValue;
			jLevel_1["index"] = 1;
			jLevel_1["row"] = 20;
			jLevel_1["column"] = 20;
			jLevel_1["distance"] = 5000.0f;
			jLevelArray.append(jLevel_1);
			Json::Value jLevel_2 = Json::objectValue;
			jLevel_2["index"] = 2;
			jLevel_2["row"] = 10;
			jLevel_2["column"] = 10;
			jLevel_2["distance"] = 10000.0f;
			jLevelArray.append(jLevel_2);
			jCluster["level"] = jLevelArray;
			jObject["cluster"] = jCluster;
		}
		jObject["is_detail_label"] = true;

		return jObject.toStyledString();
	}

}IProjectBasedSceneElementTypeInfo;

//
enum EReadTifState
{
	READ_TIF_NONE = -1,     //-1：无需读取
	READ_TIF_NEED_NOT_YET_ADD ,     //0：需要读取还未添加
	READ_TIF_NEED_ALREADY_ADD,     //1：需要读取已经添加
	READ_TIF_NEED_DONE,     //2：需要读取已经读取
};

// 工具上图单个元素结构体
typedef struct ProjectBasedSceneElement
{
	std::string strPlanID;				//活动id（用于预案类型）
	int nID;						//序列id
	int nTypeID;					//类型id
	int nMapID;						//地图id
	int nSiteID;					//场所id
	int nElemType;					//使用id（圆或多边形） 1:点 2：线  3：面 4:圆
	int nLayerID;					//关联的其他id（归属关系）
	int nUserid;					//用户id（权限）
	std::string strUuid;					//UUID
	std::string strTitle;				//title
	std::string strDescription;			//详情面板
	std::string strStyle;				//风格（节点的属相json）
	Vector3d vLabelPos;				//标签当前真实坐标
	Vector3d vAngle;				//标签角度
	Vector3d vSmoothPos;			//平滑移动场景坐标
	Vector3d vSmoothAngle;			//平滑移动场景角度
	double dLng;					//经度
	double dLat;					//纬度
	double dZ;						//Z值
	std::vector<Vector2>	vecOriginLngLat;		//经纬度集合（使用多值）
	std::vector<Vector3d>	vecPoint;		//经纬度集合（使用多值）
	std::string strNodePath;				//模型路径或者id(外加载模型或者tw4加载)
	Vector3d vInitNodePos;			//模型初始位置(外加载模型或者tw4加载，可用于分层移动)
	std::vector<INode*> vecExtraNode;	//其他模型
	std::string strFloorNodeID;			//室内关联模型名称
	INode* pFloorNode;				//室内关联模型
	std::string strPos;
	std::string strLngLats;
	std::string	 strExtra;				//V10版本后的额外数据
	std::string strDataType;		//V10版本后的数据类型
	bool   bDoPositonPost;			//是否后置处理的位置
	Json::Value	jProjectExtra;      //项目的额外数据
	bool bIndoor;					//是否在室内
	EReadTifState eReadTifStatus;				//读取高地图的状态，默认不读取，状态由外部设置，-1：无需读取，0：需要读取还未读取，1：已读取完成
	
	Vector3d vLastRealLabelPos;	//标签最后一次的真实位置,用来发送当前详情面板对应的屏幕坐标给二维
	bool  bCanSmooth;				//是否自身平滑

	ProjectBasedSceneElement()
	{
		nID = -1;
		nTypeID = -1;
		nMapID = -1;
		nSiteID = -1;
		nLayerID = -1;
		nUserid = -1;
		nElemType = -1;
		dLng = 0;
		dLat = -1;
		dZ = 0;
		pFloorNode = NULL;
		bDoPositonPost = false;
		bIndoor = false;
		Json::Value value(Json::objectValue);
		strExtra = value.toStyledString();
		eReadTifStatus = READ_TIF_NONE;
		Guid::CreateGuId(strUuid, "ProjectBasedSceneElement_");
		bCanSmooth = false;
	}

	struct ProjectBasedSceneElement& operator = (const struct ProjectBasedSceneElement &src)
	{
		strPlanID = src.strPlanID;
		nID = src.nID;
		nTypeID = src.nTypeID;
		nMapID = src.nMapID;
		nSiteID = src.nSiteID;
		nElemType = src.nElemType;
		nLayerID = src.nLayerID;
		nUserid = src.nUserid;
		strUuid = src.strUuid;
		strTitle = src.strTitle;
		strDescription = src.strDescription;
		strStyle = src.strStyle;
		vLabelPos = src.vLabelPos;
		dLng = src.dLng;
		dLat = src.dLat;
		dZ = src.dZ;
		vecOriginLngLat = src.vecOriginLngLat;
		vecPoint = src.vecPoint;
		strNodePath = src.strNodePath;
		vInitNodePos = src.vInitNodePos;
		vecExtraNode = src.vecExtraNode;
		strFloorNodeID = src.strFloorNodeID;
		pFloorNode = src.pFloorNode;
		strPos = src.strPos;
		strLngLats = src.strLngLats;
		strExtra = src.strExtra;
		strDataType = src.strDataType;
		bDoPositonPost = src.bDoPositonPost;
		jProjectExtra = src.jProjectExtra;
		bIndoor = src.bIndoor;
		vSmoothPos = src.vSmoothPos;
		vSmoothAngle = src.vSmoothAngle;
		eReadTifStatus = src.eReadTifStatus;
		vLastRealLabelPos = src.vLastRealLabelPos;
		bCanSmooth = src.bCanSmooth;

		return *this;
	}

	void SetInfo(ProjectBasedSceneElement *src)
	{
		strPlanID = src->strPlanID;
		nTypeID = src->nTypeID;
		nMapID = src->nMapID;
		strUuid = src->strUuid;
		strTitle = src->strTitle;
		strDescription = src->strDescription;
		strStyle = src->strStyle;
		vLabelPos = src->vLabelPos;
		dLng = src->dLng;
		dLat = src->dLat;
		vecOriginLngLat = src->vecOriginLngLat;
		vecPoint = src->vecPoint;
		strNodePath = src->strNodePath;
		vInitNodePos = src->vInitNodePos;
		strExtra = src->strExtra;
		bDoPositonPost = src->bDoPositonPost;
		jProjectExtra = src->jProjectExtra;
	}

	std::string GetExtraAttrTitle(std::string strKey)
	{
		try
		{
			Json::Value jExtra;
			ParseJsonByString(strExtra,jExtra);
			if (jExtra.isObject())
			{
				if (jExtra.isMember(strKey))
				{
					return jExtra[strKey]["title"].asString();
				}
			}
		}
		catch (const std::exception&)
		{

		}
		return "";
	}

	Json::Value GetExtraAttrValue(std::string strKey)
	{
		try
		{
			Json::Value jExtra;
			ParseJsonByString(strExtra, jExtra);
			if (jExtra.isObject())
			{
				if (jExtra.isMember(strKey))
				{
					return jExtra[strKey]["value"];
				}
			}
		}
		catch (const std::exception&)
		{

		}
		return Json::nullValue;
	}

}IProjectBasedSceneElement;

// 工具上图地图结构体
typedef struct ProjectBasedSceneElementMapInfo
{
	int nMapID;			//类型id
	std::string strName;			//名称
	int nPid;			//父类pid
	std::string strUrl;			//url
	std::string strNodeName;		//关联的节点
	int nStatus;	//状态
	EElementCoordinateType eCoorType;		//坐标系

	ProjectBasedSceneElementMapInfo()
	{
		nMapID = -1;
		nPid = 0;
		nStatus = 1;
		eCoorType = COOR_SCENE_LOCAL;
	}

	struct ProjectBasedSceneElementMapInfo& operator = (const struct ProjectBasedSceneElementMapInfo &src)
	{
		nMapID = src.nMapID;
		strName = src.strName;
		nPid = src.nPid;
		strUrl = src.strUrl;
		strNodeName = src.strNodeName;
		nStatus = src.nStatus;
		eCoorType = src.eCoorType;

		return *this;
	}
}IProjectBasedSceneElementMapInfo;


// web js 上图数据
typedef struct ProjectBasedWebLabelData : public ProjectBasedSceneElement
{
	float		fSize;			//BoardInstance的实例大小
	bool		bWGS84;			//是否WGS84经纬度
	bool     bUseHeightTif;		//是否读取高地图
	bool		bLivegbs;		//是否是livegbs
	std::string	 strLiveUrl;		//bLivegbs--true,livegbs的服务器地址  false:视频流地址
	std::string	 strDeviceID;		//bLivegbs--true 有效，国标id
	std::string	 strChannelID;		//bLivegbs--true 有效，渠道id
	Json::Value	j3DTextMatchObject;	//三维文字数据匹配Object,都需要是字串串格式，如：{"title":"我的名字","num":"4"}
	NodeParamBase* pLabelParam;		//标签样式
	NodeParamBase* pDetailParam;	//详情面板样式
	Json::Value	jPoiOption;			//非图层元素的配置（仅非图层模式有效）
	std::string strLabelStyleMD5;		//检验labelStyle是否一致
	std::string strDetailStylrMD5;		//检验detailStyle是否一致

	ProjectBasedWebLabelData():
		ProjectBasedSceneElement()
	{
		fSize = 1;
		bWGS84 = true;
		bUseHeightTif = false;
		bLivegbs = false;
		j3DTextMatchObject = Json::nullValue;
		pLabelParam = nullptr;
		pDetailParam = nullptr;
		jPoiOption = Json::nullValue;
	}

	struct ProjectBasedWebLabelData& operator = (const struct ProjectBasedWebLabelData& src)
	{
		ProjectBasedSceneElement::operator=(src);

		fSize = src.fSize;
		bWGS84 = src.bWGS84;
		bLivegbs = src.bLivegbs;
		strLiveUrl = src.strLiveUrl;
		strDeviceID = src.strDeviceID;
		strChannelID = src.strChannelID;
		j3DTextMatchObject = src.j3DTextMatchObject;
		pLabelParam = src.pLabelParam;
		pDetailParam = src.pDetailParam;
		jPoiOption = src.jPoiOption;
		strLabelStyleMD5 = src.strLabelStyleMD5;
		strDetailStylrMD5 = src.strDetailStylrMD5;
		return *this;
	}

	~ProjectBasedWebLabelData()
	{
		if (pLabelParam)
		{
			DELETE_PTR(pLabelParam);
		}
		if (pDetailParam)
		{
			DELETE_PTR(pDetailParam);
		}
	}
}IProjectBasedWebLabelData;

// user用户信息
typedef struct ProjectBasedUserInfo
{
	std::string	strUserToken;			//token
	std::string strAccount;				//账号
	std::string strLastAccount;			//用来判断上次和当前是否相同
	std::string	strAppId;				//账号下的应用id
	std::string	strLastAppId;			//用来判断上次和当前是否相同

	ProjectBasedUserInfo()
	{
		strAppId = "0";
		strUserToken = "default";
		strLastAppId = "";
		strLastAccount = "";
	}

	struct ProjectBasedUserInfo& operator = (const struct ProjectBasedUserInfo& src)
	{
		strUserToken = src.strUserToken;
		strAccount = src.strAccount;
		strLastAccount = src.strLastAccount;
		strAppId = src.strAppId;
		strLastAppId = src.strLastAppId;

		return *this;
	}

	bool IsSame()
	{
		bool bSame = (strAccount == strLastAccount);
		bSame = bSame && (strAppId == strLastAppId);

		return bSame;
	}
}IProjectBasedUserInfo;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * 重叠状态
 */
enum EOverTapState
{
	OVERTAP_NONE = 0,		//不支持
	OVERTAP_SHOW,			//重叠--自身显示	
	OVERTAP_HIDE,				//重叠--自身隐藏	
};

struct FactoryAssemble
{
	int nType;															//图层id
	INode* pLabelNode;										//标签节点
	ProjectBasedSceneElement* pData;					//数据
	BoardInstanceData* pLabelInstance;				//实例的
	bool bInstance;												//是否是实例
	bool bAlreadyCluste;										//是否已经聚合
	EOverTapState eOverTapState;						//重叠状态
	bool bSmoothMove;										//是否平滑处理
	bool bInstanceSelected;									//实例的当前是否被选中（用Board替换，控制显隐）

	bool bLastVisible;											//上一次的显隐（主要用于和当前判断，状态相同可避免重复调用显隐方法）
	std::atomic<bool> bModify;									//是否修改了

	FactoryAssemble()
	{
		nType = -1;
		pLabelNode = nullptr;
		pData = nullptr;
		pLabelInstance = nullptr;
		bInstance = false;
		bAlreadyCluste = false;
		eOverTapState = OVERTAP_NONE;
		bSmoothMove = false;
		bInstanceSelected = false;

		bLastVisible = false;
		bModify.store(false);
	}

	struct FactoryAssemble& operator = (const struct FactoryAssemble& src)
	{
		nType = src.nType;
		pLabelNode = src.pLabelNode;
		pData = src.pData;
		pLabelInstance = src.pLabelInstance;
		bInstance = src.bInstance;
		bAlreadyCluste = src.bAlreadyCluste;
		eOverTapState = src.eOverTapState;
		bSmoothMove = src.bSmoothMove;
		bInstanceSelected = src.bInstanceSelected;

		bLastVisible = src.bLastVisible;
		bModify.store(src.bModify);

		return *this;
	}

	bool IsVisible() 
	{
		bool bVisible = !bAlreadyCluste;
		bVisible = bVisible && (eOverTapState == OVERTAP_NONE || eOverTapState == OVERTAP_SHOW);
		if (bInstance)
		{
			bVisible = bVisible && !bInstanceSelected;
		}

		return bVisible;
	}
};

struct FactotryAssembleDetail
{
	int nType;					//图层id
	INode* pLabelNode;			//标签的节点Node
	INode* pSelfNode;			//自身节点Node
	ProjectBasedSceneElement* pData;	//绑定的数据
	BoardInstanceData* pBoardInstanceData;		//实例的
	bool bInstance;				//是否标签是实例模式
	Vector2 vDetailToLabelOffset;	//详情面板到标签的距离
	bool bAdjustPosWithLabel;		//是否根据标签位置进行位置调整

	FactotryAssembleDetail()
	{
		nType = -1;
		pLabelNode = nullptr;
		pSelfNode = nullptr;
		pData = nullptr;
		pBoardInstanceData = nullptr;
		bInstance = false;
		vDetailToLabelOffset = Vector2(0,30.0f);
		bAdjustPosWithLabel = true;
	}

	struct FactotryAssembleDetail& operator = (const struct FactotryAssembleDetail& src)
	{
		nType = src.nType;
		pLabelNode = src.pLabelNode;
		pSelfNode = src.pSelfNode;
		pData = src.pData;
		pBoardInstanceData = src.pBoardInstanceData;
		bInstance = src.bInstance;
		vDetailToLabelOffset = src.vDetailToLabelOffset;
		bAdjustPosWithLabel = src.bAdjustPosWithLabel;

		return *this;
	}
};

enum ELabelFunc
{
	FUNC_CLUSTER = 0,						//聚合
	FUNC_OVERTAP,								//重叠
	FUNC_SMOOTH_MOVE,					//平滑移动
	FUNC_READ_HRIGHT_TIF,				//帧分摊读取高地图
	FUNC_SCALE_DETAIL,						//详情面板缩放（比如全屏功能）
};

#pragma pack(pop)
}

