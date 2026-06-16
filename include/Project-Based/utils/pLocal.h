// gLocal.h
//
//////////////////////////////////////////////////////////////////////

#ifndef _P_LOCAL_H_
#define _P_LOCAL_H_

#ifdef _WIN32
#include <io.h>
#endif


#include "../IProjectBasedAPI.h"

#define TEXT_SIZE_HW_SCALE  (1.33)			//文字高宽比

using namespace bc;

//距离内强制不跳天，0则无效
static float S_FORCE_NO_SKY_DIS = 0.0f;

class MacInfo
{
public:
	std::string     strName;                      ///< 网卡名称
	std::string     strDescription;               ///< 网卡描述
	std::string     strMac;                        ///< mac地址
	std::string     strIP;
	std::string     strMask;

	MacInfo()
	{
	}

	class MacInfo& operator = (const struct MacInfo& src)
	{
		strName = src.strName;
		strDescription = src.strDescription;
		strMac = src.strMac;
		strIP = src.strIP;
		strMask = src.strMask;

		return *this;
	}
};

class FileInfo
{
public:
	_finddata_t tInfo;
	std::string strAbsoluteDir;	//决定路径
};

//////////////////////////////////////////////////////////////////////
PROJECT_BASED_API double Random(const double& start, const double& end);
PROJECT_BASED_API long StrTotimestamp(const char* str_time);
PROJECT_BASED_API tm StrToTimeTm(const char* str_time);
PROJECT_BASED_API tm TimestampToTimeTm(const long& lTime);
PROJECT_BASED_API std::string TimeTmToStr(tm& timeTm);
PROJECT_BASED_API long TimeTmTotimestamp(tm& timeTm);
PROJECT_BASED_API std::string TimestampToStr(const long& lTime);
PROJECT_BASED_API std::string Replace_all(const std::string& str, const std::string& old_value, const std::string& new_value);
//PROJECT_BASED_API void SplitStringBySpecial(const std::string& s, std::vector<string>& v, const std::string& c);
PROJECT_BASED_API std::vector<std::string> CutString(const std::string& strText, const int& nCutLength);
PROJECT_BASED_API long long GetSystemTime();
//int	SaveLog(bc::ISystemAPI* pSystemAPI, bc::LogLevel_E eLevel, char *pData);
PROJECT_BASED_API void	WriteToFile(const std::string strFilePath, const char* pData, int mode = -1);	//C++的方式
PROJECT_BASED_API std::string ReadFile(const std::string& strFilePath, int mode = -1); //C++的方式
PROJECT_BASED_API size_t GetFileSize(const std::string& strFilePath); //C++的方式,获取文件大小
PROJECT_BASED_API bool StartWith(const std::string& mainstr, const std::string& substr);
PROJECT_BASED_API void Mkdirs(const char* muldir);		//创建多级文件夹
PROJECT_BASED_API int LaunchProcess(const std::string& strCmd);		//启动进程
PROJECT_BASED_API int KillProcess(const std::string& strExeName);			//杀掉进程
PROJECT_BASED_API int KillProcess(const int& nProcessID);			//杀掉进程
PROJECT_BASED_API void GetFilesFromDir(const std::string path, std::vector<FileInfo>& vecFile, const bool& bRecursion, const std::string strFileType = "*");	//获取文件夹内的所有文件信息
PROJECT_BASED_API bool IsFileExist(const std::string& strFilePath);		//判断文件是否存在
PROJECT_BASED_API int GetUTMZoneNum(const double& lng, const double& lat);	//获取UTM的投影带
PROJECT_BASED_API int ConvertTimeCountFromStr(const std::string& strTimeCount);	//"05:10:03" （秒）
PROJECT_BASED_API std::string	 ConvertTimeCountToStr(const int& nTimeCount);	//返回"05:10:03"  输入35（秒）
PROJECT_BASED_API bc::Vector3d	 CalcAngle(const bc::Vector3d& vPrePos, const bc::Vector3d& vCurPos);	//通过两个点计算角度

//根据tw4的名称进行跳转
PROJECT_BASED_API void GoViewPoint(bc::INode* pCameraNode, const char* szName, bool bSky);
//直接设置位置和角度进行跳转
PROJECT_BASED_API void GoViewPoint(bc::INode* pCameraNode, const bc::Vector3d& vTargetPos, const bc::Vector3d& vCameraAngle, bool bSky);
//使用LookAt求出角度跳转
PROJECT_BASED_API void GoViewPoint(bc::INode* pCameraNode, const bc::Vector3d& vPos, const float fYDis, const float fZDis, bool bSky = false);
//直接视角的字符串进行跳转
PROJECT_BASED_API void GoViewPoint(bc::INode* pCameraNode, std::string& strViewPointData, bool bSKy);
//传入相机角度，目标位置，偏移值
PROJECT_BASED_API void GoViewPoint(bc::INode* pCameraNode, const bc::Vector3d& vTargetPos, const bc::Vector3d& vCameraAngle, const float fDistance, const bc::Vector2& vOffsetPos, bool bSky, bool bSphereLimit = true);
//屏幕中心为(0,0)
PROJECT_BASED_API void GoViewPoint(bc::INode* pCameraNode, const bc::Vector3d& vTargetPos, const Vector2& vCameraAngle, const float fTargetDistance, const float fOffSetX, const float fOffSetY);
bool _isForceNoSky(bc::INode* pCameraNode,const Vector3d& vTargetPos);

PROJECT_BASED_API bc::Vector3d CalacuatePostion(bc::INode* pCameraNode, const bc::Vector3d& vPos, const bc::Vector3d& vAngles, const float& fOffset, const float& fOriginWidth);
PROJECT_BASED_API bc::Vector3d StringToVector3(const std::string& str, const std::string c = ",");
PROJECT_BASED_API std::string Vector3ToString(const bc::Vector3d& v, const std::string c = ",");
PROJECT_BASED_API bc::Vector2 StringToVector2(const std::string& str, const std::string c = ",");
PROJECT_BASED_API std::string Vector2ToString(const bc::Vector2& v, const std::string c = ",");
PROJECT_BASED_API bc::Vector4 StringToVector4(const std::string& str, const std::string c = ",");
PROJECT_BASED_API std::string Vector4ToString(const bc::Vector4& v, const std::string c = ",");
PROJECT_BASED_API bc::Vector4 ConvertColorFormat255(const bc::Vector4& vColor);	//检测是255格式解析的不
PROJECT_BASED_API bc::Vector3d ConvertColorFormat255(const bc::Vector3d& vColor);	//检测是255格式解析的不
PROJECT_BASED_API bc::Vector4 ColorToFormat255(const bc::Vector4& vColor);	//转换到255格式的
PROJECT_BASED_API bc::Vector3d ColorToFormat255(const bc::Vector3d& vColor);	//转换到255格式的
PROJECT_BASED_API bc::TextShow* SetTextShowByStrKey(bc::INode* pBoard, const std::string& strKey, const std::string& strText, bool bForce = false);
PROJECT_BASED_API void FindAllTextShow(bc::INode* pBoard, std::vector<bc::TextShow*>& vec, const std::string& strKey = "");
PROJECT_BASED_API bc::INode* FindBoardContainTextShow(bc::INode* pBoard, const std::string& strKey);
PROJECT_BASED_API bool IsContainerNodeFromNode(bc::INode* pContainerNode, bc::INode* pTargetNode);	//判断是否包含INode
PROJECT_BASED_API std::string StrToSubNumStr(const std::string& strOrigin);	//筛选字符串中的数字
PROJECT_BASED_API std::string EncodeBase64(const std::string& str);
PROJECT_BASED_API std::string DecodeBase64(const std::string& str);
PROJECT_BASED_API std::string GetComputeMacAddress(std::vector<MacInfo>& vecMacAddress);		//获取计算机的mac地址
PROJECT_BASED_API std::string GetComputeName();				//获取计算机名
PROJECT_BASED_API Json::Value GetJsonItemRecursion(Json::Value root, std::string strRecursionKeys);		//获取json,可递归 如:data,camera,list
PROJECT_BASED_API void ParseJsonByString(std::string& str, Json::Value &jResult);		//解析json
PROJECT_BASED_API void SplitToCircle(bc::Vector3d vCenterPos, float fRadius, std::vector<bc::Vector3d>& vecPoints);
PROJECT_BASED_API void SetINTForNodeWithDetail(bc::INode* pNode);
PROJECT_BASED_API void SetINTForNodeWithLabel(bc::INode* pNode);
PROJECT_BASED_API bool IsPointInRect(bc::Vector2& vPoint, bc::Vector2& vLeftBottom, bc::Vector2& vLeftTop,
	bc::Vector2& vRightTop, bc::Vector2& vRightBottom);
PROJECT_BASED_API float GetZFromOpenglCamera(INode* pCameraNode, Vector3d& vPos);//通过相机获取点位高度
PROJECT_BASED_API bool IsUTF8String(const std::string& string);
PROJECT_BASED_API bool IsDir(const std::string& strPath);
PROJECT_BASED_API std::vector<Vector3> ConvertVecVector3dTo(std::vector<Vector3d> &vecVector3d);
PROJECT_BASED_API std::vector<Vector3d> ConvertVecVector3ToD(std::vector<Vector3>& vecVector3);


PROJECT_BASED_API bool GetProjectVersion(const std::string& strFilePath, std::string& strVersion);
PROJECT_BASED_API std::string GetProjectExecuteName();



//获取IComponent
template <typename T>
T* GetComponent(bc::INode* pNode)
{
	if (!pNode)
	{
		return NULL;
	}
	if (typeid(T) == typeid(bc::IBoard))
	{
		return (T*)pNode->iGetComponent(bc::COMPONENT_TYPE_BOARD);
	}
	else if (typeid(T) == typeid(bc::IRenderNode))
	{
		return (T*)pNode->iGetComponent(bc::COMPONENT_TYPE_RENDER_NODE);
	}
	else if (typeid(T) == typeid(bc::IGeo))
	{
		return (T*)pNode->iGetComponent(bc::COMPONENT_TYPE_GEO);
	}
	else if (typeid(T) == typeid(bc::IVideo))
	{
		return (T*)pNode->iGetComponent(bc::COMPONENT_TYPE_VIDEO);
	}
	else if (typeid(T) == typeid(bc::IVideoRoute))
	{
		return (T*)pNode->iGetComponent(bc::COMPONENT_TYPE_VIDEO_ROUTE);
	}
	else if (typeid(T) == typeid(bc::IInertiaManipulator))
	{
		return (T*)pNode->iGetComponent(bc::COMPONENT_TYPE_INERTIA_MANIPULATOR);
	}
	else if (typeid(T) == typeid(bc::IMeasureArea))
	{
		return (T*)pNode->iGetComponent(bc::COMPONENT_TYPE_MEASURE_AREA);
	}
	else if (typeid(T) == typeid(bc::IMeasureDistance))
	{
		return (T*)pNode->iGetComponent(bc::COMPONENT_TYPE_MEASURE_DISTANCE);
	}
	else if (typeid(T) == typeid(bc::ILocusLine))
	{
		return (T*)pNode->iGetComponent(bc::COMPONENT_TYPE_LOCUS_LINE);
	}
	else if (typeid(T) == typeid(bc::IInstance))
	{
		return (T*)pNode->iGetComponent(bc::COMPONENT_TYPE_INSTANCE);
	}
	else if (typeid(T) == typeid(bc::IManipulator))
	{
		return (T*)pNode->iGetComponent(bc::COMPONENT_TYPE_MANIPULATOR);
	}
	else if (typeid(T) == typeid(bc::ICamera))
	{
		return (T*)pNode->iGetComponent(bc::COMPONENT_TYPE_CAMERA);
	}
	return NULL;
}

#endif

