//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#include "Windows.h"
#include <direct.h>
#include <io.h>
#include <Tlhelp32.h>
#include "Iphlpapi.h"
#include <psapi.h>

#endif


#include <sys/timeb.h>
#include <sys/types.h>

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <random>
#include <codecvt>


#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ProjectLogger.h"
#include "Project-Based/base/ProjectBasedClient.h"

#ifdef __GNUC__

#define strncpy_s	strncpy

#else
	
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib,"psapi.lib")
#pragma comment(lib,"version.lib")
#endif
 
using namespace std;

#define Localtime_r(tm, ti)  localtime_s(tm, ti)
double Random(const double& start, const double& end)
{
	std::random_device rd;
	std::default_random_engine random(rd());
	std::uniform_real_distribution<double> dis1(start, end);
	double dResult = dis1(random);
	return dResult;
}
/**
 * 字符串转时间戳
 */
long StrTotimestamp(const char* str_time)
{
	struct tm stm;
	int iY, iM, iD, iH, iMin, iS;

	memset(&stm, 0, sizeof(stm));
	iY = atoi(str_time);
	iM = atoi(str_time + 5);
	iD = atoi(str_time + 8);
	iH = atoi(str_time + 11);
	iMin = atoi(str_time + 14);
	iS = atoi(str_time + 17);

	stm.tm_year = iY - 1900;
	stm.tm_mon = iM - 1;
	stm.tm_mday = iD;
	stm.tm_hour = iH;
	stm.tm_min = iMin;
	stm.tm_sec = iS;

	/*printf("%d-%0d-%0d %0d:%0d:%0d\n", iY, iM, iD, iH, iMin, iS);*/
	//标准时间格式例如：2016:08:02 12:12:30
	return (long)mktime(&stm);
}

tm StrToTimeTm(char* str_time)
{
	struct tm stm;
	int iY, iM, iD, iH, iMin, iS;

	memset(&stm, 0, sizeof(stm));
	iY = atoi(str_time);
	iM = atoi(str_time + 5);
	iD = atoi(str_time + 8);
	iH = atoi(str_time + 11);
	iMin = atoi(str_time + 14);
	iS = atoi(str_time + 17);

	stm.tm_year = iY - 1900;
	stm.tm_mon = iM - 1;
	stm.tm_mday = iD;
	stm.tm_hour = iH;
	stm.tm_min = iMin;
	stm.tm_sec = iS;

	return stm;
}

tm TimestampToTimeTm(const long& lTime)
{
	time_t     theTime = 0;
	struct tm  theDate;
	char       szBuffer[32] = { 0 };

	if (!lTime)
	{
		theTime = time(0);
	}
	else
	{
		theTime = lTime;
	}
	Localtime_r(&theDate, &theTime);

	theDate.tm_year += 1900;
	theDate.tm_mon++;

	return theDate;
}

std::string TimeTmToStr(tm& timeTm)
{
	timeTm.tm_year -= 1900;
	timeTm.tm_mon -= 1;
	long lTime = (long)mktime(&timeTm);
	return TimestampToStr(lTime);
}

long TimeTmTotimestamp(tm& timeTm)
{
	long lTime = (long)mktime(&timeTm);
	return lTime;
}

/**
 * 时间戳转字符串
 */
std::string TimestampToStr(const long& lTime)
{
	time_t     theTime = 0;
	struct tm  theDate;
	char       szBuffer[32] = { 0 };

	if (!lTime)
	{
		theTime = time(0);
	}
	else
	{
		theTime = lTime;
	}
	Localtime_r(&theDate, &theTime);

	theDate.tm_year += 1900;
	theDate.tm_mon++;

	sprintf_s(szBuffer, sizeof(szBuffer), "%4d-%02d-%02d %02d:%02d:%02d", theDate.tm_year, theDate.tm_mon, theDate.tm_mday,
		theDate.tm_hour, theDate.tm_min, theDate.tm_sec);
	std::string str = szBuffer;
	return str;
}

/**
 * 替换字符串
 */
std::string Replace_all(const std::string& str, const std::string& old_value, const std::string& new_value)
{
	std::string strResult = str;
	while (true) {
		std::string::size_type   pos(0);
		if ((pos = strResult.find(old_value)) != std::string::npos)
		{
			strResult.replace(pos, old_value.length(), new_value);
		}
		else
		{
			break;
		}
	}
	return strResult;
}

////字符串按某字符分割成列表
//void SplitStringBySpecial(const std::string& s, std::vector<std::string>& v, const std::string& c)
//{
//	string::size_type pos1, pos2;
//	pos2 = s.find(c);
//	pos1 = 0;
//	while (string::npos != pos2)
//	{
//		v.push_back(s.substr(pos1, pos2 - pos1));
//
//		pos1 = pos2 + c.size();
//		pos2 = s.find(c, pos1);
//	}
//	if (pos1 != s.length())
//	{
//		v.push_back(s.substr(pos1));
//	}
//	return true;
//}

// 字符串按长度截取中文字符
std::vector<std::string> CutString(const std::string& strText, const int& nCutLength)
{
	std::string str = strText;
	std::vector<std::string> vecStr;

	int nLength, num, nYuShu, CharCount;

	nLength = str.length();				//字符长度
	num = nLength / nCutLength;			//截取次数
	nYuShu = nLength % nCutLength;		//最后一行截取个数
	CharCount = 0;						//记录每次截取的位数
	std::string Intercept;					//每行截取的数据

	if (nYuShu)
	{
		num += 1;
	}

	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < str.size(); j++)
		{
			if (CharCount <= nCutLength)
			{
				if (str[j] & 0x80)
				{
					CharCount += 2;
					if (CharCount > nCutLength)		//对下一个中文是否超出截取范围做判断，超出即不能继续拼接字符串
					{
						CharCount -= 2;
						str = str.substr(CharCount, (nLength - CharCount));
						break;
					}
					Intercept += str[j];
					Intercept += str[++j];
				}
				else
				{
					CharCount += 1;
					Intercept += str[j];
				}
			}
			else
			{
				str = str.substr(CharCount, (nLength - CharCount));
				break;
			}
		}

		vecStr.push_back(Intercept);
		Intercept = "";
		nLength -= CharCount;
		CharCount = 0;
	}

	return vecStr;
}

/**
 * 获取毫秒时间戳
 */
long long GetSystemTime()
{
	struct timeb t;

	ftime(&t);

	return 1000 * t.time + t.millitm;
}

//int	SaveLog(bc::ISystemAPI *pSystemAPI, bc::LogLevel_E eLevel, char *pData)
//{
//	int nRet = pSystemAPI->iProtocol->WriteLog(eLevel,pData);
//	return nRet;
//}

void WriteToFile(const std::string strFilePath, const char* pData, int mode)
{
	int nIndex = strFilePath.find_last_of("/");
	if (nIndex == std::string::npos)
	{
		nIndex = strFilePath.find_last_of("\\");
	}
	std::string strDir;
	if (nIndex != std::string::npos)
	{
		strDir = strFilePath.substr(0, nIndex);
	}
	else
	{
		strDir = strFilePath;
	}
	Mkdirs(strDir.c_str());
	if (mode == -1)
	{
		mode = std::ios::out;
	}
#ifdef __GNUC__
	ofstream outFile(strFilePath);
#elif _MSC_VER
	std::ofstream outFile(strFilePath, mode);
#endif
	try
	{
		if (!outFile)
		{
			return;
		}
		outFile << pData;
	}
	catch (const std::exception&)
	{
	}
	outFile.flush();
	outFile.close();
}

std::string ReadFile(const std::string& strFilePath, int mode)
{
#ifdef _MSC_VER
	if (mode == -1)
	{
		mode = std::ios::_Nocreate;
	}
	std::ifstream fin(strFilePath, mode);
#elif __GNUC__
	std::ifstream fin;
	fin.open(strFilePath);

#endif
	std::ostringstream buf;
	try
	{
		char ch;
		while (buf && fin.get(ch))
		{
			buf.put(ch);
		}
	}
	catch (const std::exception&)
	{
	}
	//返回与流对象buf关联的字符串
	fin.close();
	return buf.str();
}

size_t GetFileSize(const std::string& strFilePath)
{
	std::ifstream in(strFilePath.c_str());
	in.seekg(0, std::ios::end);
	size_t size = in.tellg();
	in.close();
	return size; //单位是：byte
}

bool StartWith(const std::string& mainstr, const std::string& substr)
{
	return mainstr.find(substr) == 0 ? 1 : 0;
}

void Mkdirs(const char* muldir)
{
	int i, len;
	char str[512];
	
	strncpy_s(str, muldir, 512);
	len = strlen(str);
	for (i = 0; i < len; i++)
	{
		if (str[i] == '/')
		{
			str[i] = '\0';
			if (_access(str, 0) != 0)
			{
				_mkdir(str);
			}
			str[i] = '/';
		}
	}
	if (len > 0 && _access(str, 0) != 0)
	{
		_mkdir(str);
	}
}

int LaunchProcess(const std::string& strCmd)
{
#ifdef _MSC_VER
	STARTUPINFO   si;
	PROCESS_INFORMATION   pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	si.dwFlags = STARTF_USESHOWWINDOW;     //设置隐藏执行窗口
	si.wShowWindow = SW_HIDE;
	if (!CreateProcess(
		NULL,   // No module name (use command line)
		(LPSTR)strCmd.c_str(),        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		char sz[1024] = { 0 };
		sprintf_s(sz, sizeof(sz), "CreateProcess failed (%d).\n", GetLastError());
		return -1;
	}
	else
	{
		char sz[1024] = { 0 };
		sprintf_s(sz, sizeof(sz), "CreateProcess success (%d).\n", pi.dwProcessId);
	}
	return pi.dwProcessId;
#elif __GNUC__
	return 0;
#endif
}

int KillProcess(const int& nProcessID)
{
#ifdef _MSC_VER
	//强制终止进程 
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, nProcessID);
	if (hProcess == NULL)
	{
		return -1;
	}
	if (!TerminateProcess(hProcess, 0))
	{
		return -1;
	}

#elif __GNUC__
	if (kill(nProcessID, 0) != -1)
	{
		return -1;
	}
#endif
	return 0;
}

int KillProcess(std::string& strExeName)
{
#ifdef _MSC_VER
	PROCESSENTRY32 pe;
	DWORD dwProcessId = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapshot, &pe))
	{
		return 0;
	}

	do
	{
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32Next(hSnapshot, &pe) == FALSE)
			break;
		if (lstrcmpi(pe.szExeFile, strExeName.c_str()) == 0)
		{
			dwProcessId = pe.th32ProcessID;
			break;
		}
	} while (TRUE);

	CloseHandle(hSnapshot);
	if (dwProcessId == 0)
	{
		return -1;
	}

	return KillProcess(dwProcessId);
#elif __GNUC__
	return -1;
#endif
}

void GoViewPoint(bc::INode* pCameraNode, std::string& strViewPointData, bool bSKy)
{
	bc::Vector3d vOrigin, vAngle;
	if (!strViewPointData.empty())
	{
		sscanf_s(strViewPointData.c_str(), "%lf,%lf,%lf,%lf,%lf,%lf", &vOrigin.x, &vOrigin.y, &vOrigin.z, &vAngle.x, &vAngle.y, &vAngle.z);
		std::vector<std::string> vec;
		SplitStringBySpecial(strViewPointData, vec, ",");
		bc::Vector3d vTempAngle = pCameraNode->iGetAngles();
		bc::Vector3d vTempOrigin = pCameraNode->iGetOrigin();
		for (int i = vec.size(); i < 6; i++)
		{
			if (i == 0)
			{
				vOrigin.x = vTempOrigin.x;
			}
			else if (i == 1)
			{
				vOrigin.y = vTempOrigin.y;
			}
			else if (i == 2)
			{
				vOrigin.z = vTempOrigin.z;
			}
			else if (i == 3)
			{
				vAngle.x = vTempAngle.x;
			}
			else if (i == 4)
			{
				vAngle.y = vTempAngle.y;
			}
			else if (i == 5)
			{
				vAngle.z = vTempAngle.z;
			}
		}
		if (bSKy)
		{
			bSKy = _isForceNoSky(pCameraNode, vOrigin);
		}
		GoViewPoint(pCameraNode, vOrigin, vAngle, 0,Vector2d(0,0),bSKy);
	}
}

void GoViewPoint(bc::INode* pCameraNode, const bc::Vector3d& vTargetPos, const bc::Vector3d& vCameraAngle, const float fDistance,
	const bc::Vector2& vOffsetPos, bool bSky, bool bSphereLimit)
{
	Vector3d vPos = vTargetPos;
	Vector3d vAngle = vCameraAngle;
	float fOffsetX = vOffsetPos.x;
	float fOffsetY = vOffsetPos.y;
	Axisd axis;
	axis.FromAngles(vAngle);
	Vector3d vDir = axis[0];
	vDir.Normalise();
	vPos = vTargetPos.MoveForward(vDir, -fDistance);
	vDir = axis[1];
	vDir.Normalise();
	vPos = vPos.MoveForward(vDir, fOffsetX);
	vDir = axis[2];
	vDir.Normalise();
	vPos = vPos.MoveForward(vDir, fOffsetY);
	if (bSky)
	{
		bSky = _isForceNoSky(pCameraNode, vPos);
	}

	pCameraNode->GetDynamicComponent<bc::ICamera>()->iGoViewPoint(vPos, vAngle, bSky);
}

//根据场景文件节点跳转视点
void GoViewPoint(bc::INode* pCameraNode, const char* szName, bool bSky)
{
	if (pCameraNode)
	{
		pCameraNode->GetDynamicComponent<bc::ICamera>()->iGoViewPoint(szName, bSky);
	}
}

void GoViewPoint(bc::INode* pCameraNode, const bc::Vector3d& vTargetPos, const Vector2& vCameraAngle, const float fTargetDistance, const float fOffSetX, const float fOffSetY)
{
	bc::IManipulator* pManipulator = pCameraNode->GetDynamicComponent<bc::IManipulator>();
	if (pManipulator)
	{
		if (vCameraAngle.x < 1.0 || vCameraAngle.x > 85.0)
		{
			return;
		}
		Vector3d vTargetCameraAngle;
		Vector3d vTargetCameraPos;
		vTargetCameraPos.x = vTargetPos.x - fTargetDistance * cos(vCameraAngle.x) * cos(vCameraAngle.y);
		vTargetCameraPos.y = vTargetPos.y - fTargetDistance * cos(vCameraAngle.x) * sin(vCameraAngle.y);
		vTargetCameraPos.z = vTargetPos.z + fTargetDistance * sin(vCameraAngle.x);
		bc::Axisd axis;
		axis.LookAt(vTargetCameraPos, vTargetPos, bc::Vector3d(0, 0, 1));
		vTargetCameraAngle = axis.ToAngles();
		pCameraNode->GetDynamicComponent<bc::ICamera>()->iGoViewPoint(vTargetCameraPos, vTargetCameraAngle, false);
	}
}

void GoViewPoint(bc::INode* pCameraNode, const bc::Vector3d& vTargetPos, const bc::Vector3d& vCameraAngle, bool bSky)
{
	GoViewPoint(pCameraNode,vTargetPos,vCameraAngle,0,bc::Vector2d(0,0),bSky);
}

void GoViewPoint(bc::INode* pCameraNode, const bc::Vector3d& vPos, const float fYDis, const float fZDis, bool bSky)
{
	bc::IManipulator* pManipulator = pCameraNode->GetDynamicComponent<bc::IManipulator>();
	if (pManipulator)
	{
		bc::Vector3d vOrigin = vPos.MoveForward(bc::Vector3d(0, -1, 0), fYDis);
		vOrigin = vOrigin.MoveForward(bc::Vector3d(0, 0, 1), fZDis);
		bc::Axisd axis;
		axis.LookAt(vOrigin, vPos, bc::Vector3d(0, 0, 1));
		bc::Vector3d vAngle = axis.ToAngles();

		GoViewPoint(pCameraNode, vOrigin, vAngle, bSky);
	}
}

bool _isForceNoSky(bc::INode* pCameraNode, const Vector3d& vTargetPos)
{
	Vector3d vCameraPos = pCameraNode->iGetOrigin();
	if (vCameraPos.Distance(vTargetPos) < S_FORCE_NO_SKY_DIS)
	{
		return false;
	}
	return true;
}

//使面板充满全屏的比例
bc::Vector3d CalacuatePostion(bc::INode* pCameraNode, const bc::Vector3d& vPos, const bc::Vector3d& vAngles,
	const float& fOffset, const float& fOriginWidth)
{
	bc::Vector3d vOrigin;
	bc::ICamera* pCamera = pCameraNode->GetDynamicComponent<bc::ICamera>();
	if (pCamera)
	{
		float fFov = pCamera->iGetFOV() / 2.0f;
		float fDis = (fOriginWidth / 2.0f) / tan(fFov * D_TO_R);
		fDis += fOffset;

		bc::Axisd axis;
		axis.FromAngles(vAngles);
		vOrigin = vPos.MoveForward(axis[0], fDis);
	}
	return vOrigin;
}
bc::Vector3d StringToVector3(const std::string& str, const std::string c)
{
	bc::Vector3d v;
	std::vector<std::string> vec;
	SplitStringBySpecial(str, vec, c);
	if (vec.size() >= 3)
	{
		float fX = atof(vec[0].c_str());
		float fY = atof(vec[1].c_str());
		float fZ = atof(vec[2].c_str());
		v = bc::Vector3d(fX, fY, fZ);
	}
	return v;
}

std::string Vector3ToString(const bc::Vector3d& v, const std::string c)
{
	char szBuffer[1024] = { 0 };
	sprintf_s(szBuffer, sizeof(szBuffer), "%f%s%f%s%f", v.x, c.c_str(), v.y, c.c_str(), v.z);
	return std::string(szBuffer);
}

bc::Vector2 StringToVector2(const std::string& str, const std::string c)
{
	bc::Vector2 v;
	std::vector<std::string> vec;
	SplitStringBySpecial(str, vec, c);
	if (vec.size() >= 2)
	{
		float fX = atof(vec[0].c_str());
		float fY = atof(vec[1].c_str());
		v.x = fX;
		v.y = fY;
	}
	return v;
}

std::string Vector2ToString(const bc::Vector2& v, const std::string c)
{
	char szBuffer[1024] = { 0 };
	sprintf_s(szBuffer, sizeof(szBuffer), "%f%s%f", v.x, c.c_str(), v.y);
	return std::string(szBuffer);
}

bc::Vector4 StringToVector4(const std::string& str, const std::string c)
{
	bc::Vector4 v;
	v.w = 1.0f;
	std::vector<std::string> vec;
	SplitStringBySpecial(str, vec, c);
	if (vec.size() >= 4)
	{
		float fX = atof(vec[0].c_str());
		float fY = atof(vec[1].c_str());
		float fZ = atof(vec[2].c_str());
		float fW = atof(vec[3].c_str());
		v.x = fX;
		v.y = fY;
		v.z = fZ;
		v.w = fW;
	}
	return v;
}

std::string Vector4ToString(const bc::Vector4& v, const std::string c)
{
	char szBuffer[1024] = { 0 };
	sprintf_s(szBuffer, sizeof(szBuffer), "%f%s%f%s%f%s%f", v.x, c.c_str(), v.y, c.c_str(), v.z, c.c_str(), v.w);
	return std::string(szBuffer);
}

void GetFilesFromDir(const std::string path, std::vector<FileInfo>& vecFile, const bool& bRecursion, const std::string strFileType)
{
#ifdef _MSC_VER
	intptr_t           hFile = 0;
	struct _finddata_t fileinfo;
	std::string pathName, p;

	//代表要遍历所有的类型
	if ((hFile = _findfirst(pathName.assign(path).append("/*.*").c_str(), &fileinfo)) == -1)
	{
		return;
	}
	do
	{
		if (bRecursion && (fileinfo.attrib & _A_SUBDIR))
		{
			if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)  //进入文件夹查找
			{
				GetFilesFromDir(p.assign(path).append("\\").append(fileinfo.name), vecFile, true, strFileType);
			}
		}
		else if (!(fileinfo.attrib & _A_SUBDIR))
		{
			std::string strName = fileinfo.name;
			//检测为文件就添加
			if (strFileType == "*")
			{
				FileInfo stInfo;
				stInfo.tInfo = fileinfo;
				stInfo.strAbsoluteDir = path;
				vecFile.push_back(stInfo);
			}
			else if (strName.find("." + strFileType) != std::string::npos)
			{
				FileInfo stInfo;
				stInfo.tInfo = fileinfo;
				stInfo.strAbsoluteDir = path;
				vecFile.push_back(stInfo);  //将文件路径保存，也可以只保存文件名:    p.assign(path).append("\\").append(fileinfo.name)
			}
		}
	} while (_findnext(hFile, &fileinfo) == 0);

	_findclose(hFile);
#elif __GNUC__
	DIR* pDir;    //  是头文件<dirent.h>的类型
	struct _finddata_t ptr;  // opendir、readdir这些都是头文件dirent.h
	if (!(pDir = opendir(path.c_str())))
		return;
	while ((ptr = readdir(pDir)) != 0)
	{
		if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
		{
			int nPos = strlen(ptr->d_name) - strFileType.length();
			if (nPos >= 0)
			{
				string strFileExt = ptr->d_name + nPos;
				if (strFileExt == strFileType)
				{
					FileInfo stInfo;
				    stInfo.tInfo = ptr;
				    stInfo.strAbsoluteDir = path;
				
					vecFile.push_back(stInfo);
				}
			}
		}
	}
	closedir(pDir);

#endif
}

int	GetUTMZoneNum(const double& lng, const double& lat)
{
	if (56 <= lat && 64 > lat && 3 <= lng && 12 > lat)
	{
		return 32;
	}
	if (72 <= lat && 84 >= lat && lng >= 0)
	{
		if (lng < 9)
		{
			return 31;
		}
		else if (lng < 21)
		{
			return 33;
		}
		else if (lng < 33)
		{
			return 35;
		}
		else if (lng < 42)
		{
			return 37;
		}
	}
	return (int)((lng + 180) / 6 + 1);
}

bool IsFileExist(const std::string& strFilePath)
{
	return (_access(strFilePath.c_str(), 0) == 0);
}

int	ConvertTimeCountFromStr(const std::string& strTimeCount)
{
	int nCount = 0;
	std::vector<std::string> vec;
	SplitStringBySpecial(strTimeCount, vec, ":");
	if (vec.size() == 3)
	{
		int nn = atoi(vec[0].c_str());
		nCount += nn * 3600;

		nn = atoi(vec[1].c_str());
		nCount += nn * 60;

		nn = atoi(vec[2].c_str());
		nCount += nn;
	}
	else if (vec.size() == 2)
	{
		int nn = atoi(vec[0].c_str());
		nCount += nn * 60;

		nn = atoi(vec[1].c_str());
		nCount += nn;
	}
	else if (vec.size() == 1)
	{
		int nn = atoi(vec[0].c_str());
		nCount += nn;
	}

	return nCount;
}

std::string	ConvertTimeCountToStr(const int& nTimeCount)
{
	int nHour = nTimeCount / 3600;
	int nMin = nTimeCount - nHour * 3600;
	int nSecond = nTimeCount - nHour * 3600 - nMin * 60;
	if (nSecond < 0)
	{
		nSecond = 0;
	}
	char szHour[20] = { 0 };
	char szMin[20] = { 0 };
	char szSecond[20] = { 0 };
	if (nHour > 10)
	{
		sprintf_s(szHour, sizeof(szHour), "%d", nHour);
	}
	else if (nHour >= 0)
	{
		sprintf_s(szHour, sizeof(szHour), "0%d", nHour);
	}

	if (nMin > 10)
	{
		sprintf_s(szMin, sizeof(szMin), "%d", nMin);
	}
	else if (nMin >= 0)
	{
		sprintf_s(szMin, sizeof(szMin), "0%d", nMin);
	}

	if (nSecond > 10)
	{
		sprintf_s(szSecond, sizeof(szSecond), "%d", nSecond);
	}
	else if (nSecond >= 0)
	{
		sprintf_s(szSecond, sizeof(szSecond), "0%d", nSecond);
	}

	std::string str = std::string(szHour) + std::string(":") + std::string(szMin) + std::string(":") + std::string(szSecond) + std::string(":");
	return str;
}

bc::Vector3d	 CalcAngle(const bc::Vector3d& vPrePos, const bc::Vector3d& vCurPos)
{
	//bc::Vector3d vAngle;
	//std::vector<bc::Vector3d> vecDirs;
	//vecDirs.push_back(bc::Vector3d(0, 0, 1));
	//vecDirs.push_back(bc::Vector3d(1, 0, 0));
	//vecDirs.push_back(bc::Vector3d(0, 1, 0));
	//for (int j = 0; j < vecDirs.size(); j++)
	//{
	//	bc::Vector3d v1 = vPrePos - vCurPos;
	//	bc::Vector3d v2 = vecDirs[j];
	//	float radian = v1.AngleBetween(v2);
		//bool bResert = false;
		//if (v2.CrossProduct(v1).z < 0)
		//{
		//	//radian = -radian;
		//	bResert = true;
		//}
		//radian = abs(radian);
		//float r = RAD_TO_DEG(radian);
		//r += 180.0f;
	//	if (j == 0)
	//	{
	//		vAngle.z = r;
	//	}
	//	else if (j == 1)
	//	{
	//		vAngle.x = r;
	//	}
	//	else if (j == 2)
	//	{
	//		//vAngle.y = r + 180.0f;
	//		if (bResert)
	//		{
	//			vAngle.y = 360 - r;
	//		}
	//		else
	//		{
	//			vAngle.y = r;
	//		}
	//	}
	//}
	//return vAngle;

	bc::Vector3d vAngle;
	Vector3d vDir = vPrePos - vCurPos;
	//计算Y的角度
	{
		Vector2d vTarget(vDir.x,vDir.y);
		vTarget.Normalise();
		Vector2d vBase(-1, 0);		//从原点出发
		float radian = vTarget.AngleBetween(vBase);
		bool bResert = false;
		if (vBase.CrossProduct(vTarget) < 0)
		{
			bResert = true;
		}
		radian = abs(radian);
		float r = RAD_TO_DEG(radian);
		if (bResert)
		{
			r = 360 - r;
		}
		vAngle.y = r;
	}
	////计算X的角度   先不处理
	//{
	//	Vector2d vTarget(vDir.x, vDir.z);
	//	vTarget.Normalise();
	//	Vector2d vBase(-1, 0);		//从原点出发
	//	float radian = vTarget.AngleBetween(vBase);
	//	bool bResert = false;
	//	if (vBase.CrossProduct(vTarget) < 0)
	//	{
	//		bResert = true;
	//	}
	//	radian = abs(radian);
	//	float r = RAD_TO_DEG(radian);
	//	if (bResert)
	//	{
	//		r = 360 - r;
	//	}
	//	vAngle.x = -r;
	//}
	return vAngle;
}

bc::Vector4 ConvertColorFormat255(const bc::Vector4& vColor)
{
	bc::Vector4 vOut;
	bool bFloat = false;
	if (vColor.x >= 0 && vColor.x <= 1
		&& vColor.y >= 0 && vColor.y <= 1
		&& vColor.z >= 0 && vColor.z <= 1
		&& vColor.w >= 0 && vColor.w <= 1)
	{
		bFloat = true;
	}
	if (bFloat)
	{
		vOut = vColor;
	}
	else
	{
		vOut.x = vColor.x / 255.0f;
		vOut.y = vColor.y / 255.0f;
		vOut.z = vColor.z / 255.0f;
		vOut.w = vColor.w;
	}
	return vOut;
}

bc::Vector3d ConvertColorFormat255(const bc::Vector3d& vColor)
{
	bc::Vector3d vOut;
	bool bFloat = false;
	if (vColor.x >= 0 && vColor.x <= 1
		&& vColor.y >= 0 && vColor.y <= 1
		&& vColor.z >= 0 && vColor.z <= 1)
	{
		bFloat = true;
	}
	if (bFloat)
	{
		vOut = vColor;
	}
	else
	{
		vOut.x = vColor.x / 255.0f;
		vOut.y = vColor.y / 255.0f;
		vOut.z = vColor.z / 255.0f;
	}
	return vOut;
}

bc::Vector4 ColorToFormat255(const bc::Vector4& vColor)
{
	bc::Vector4 vOut;
	vOut.x = (int)(vColor.x * 255.0f);
	vOut.y = (int)(vColor.y * 255.0f);
	vOut.z = (int)(vColor.z * 255.0f);
	vOut.w = vColor.w;
	return vOut;
}

bc::Vector3d ColorToFormat255(const bc::Vector3d& vColor)
{
	bc::Vector3d vOut;
	vOut.x = (int)(vColor.x * 255.0f);
	vOut.y = (int)(vColor.y * 255.0f);
	vOut.z = (int)(vColor.z * 255.0f);
	return vOut;
}

bc::TextShow* SetTextShowByStrKey(bc::INode* pNode, const std::string& strKey, const std::string& strText, bool bForce)
{
	bc::IBoard* pBoard = static_cast<bc::IBoard*>(pNode->iGetComponent(bc::COMPONENT_TYPE_BOARD));
	if (!pBoard)
	{
		return NULL;
	}

	bc::BoardParam* pParam = dynamic_cast<bc::BoardParam*>(pBoard->iGetBoardParam());
	if (!pParam)
	{
		return NULL;
	}
	std::vector<bc::INode*> vecBoard;
	vecBoard.push_back(pNode);
	if (pParam->nNodeType == bc::NODE_FONTBOARD_WITH_BUTTON)
	{
		std::vector<bc::INode*> vec;
		pBoard->iGetButtonBoard(vec);
		vecBoard.insert(vecBoard.end(), vec.begin(), vec.end());
	}
	else if (pParam->nNodeType == bc::NODE_BOARD_WITH_FONTBOARD)
	{
		bc::IBoard* pBtn = pBoard;
		if (pBtn->iGetFontBoard())
		{
			vecBoard.push_back(pBtn->iGetFontBoard());
		}
		if (pBtn->iGetDetailFontBoard())
		{
			vecBoard.push_back(pBtn->iGetDetailFontBoard());
		}
	}
	else if (pParam->nNodeType == bc::NODE_VIDEO_BOARD_WITH_BUTTON)
	{
		std::vector<bc::INode*> vecBtn;
		pBoard->iGetButtonBoard(vecBtn);
		vecBoard.insert(vecBoard.end(), vecBtn.begin(), vecBtn.end());
	}
	for (int i = 0; i < vecBoard.size(); i++)
	{
		bc::IBoard* pFontBoard = vecBoard[i]->GetDynamicComponent<IBoard>();
		if (pFontBoard)
		{
			bc::BoardParam* pTempParam = (bc::BoardParam*)pFontBoard->iGetBoardParam();
			bc::FontBoardParam* pParam = dynamic_cast<bc::FontBoardParam*>(pTempParam);
			if (pParam)
			{
				for (int j = 0; j < pParam->vecText.size(); j++)
				{
					if (pParam->vecText[j].strID == strKey)
					{
						if (pParam->vecText[j].strText == strText && !bForce && !pParam->vecText[j].strFlagName.empty())
						{
							continue;
						}
						pParam->vecText[j].strFlagName = "1";	//用于标识已经设置过了
						std::string str = strText;
						std::vector<std::string> vecText;
						SplitStringBySpecial(str, vecText, "\n");
						std::string strLongText;
						for (size_t m = 0; m < vecText.size(); m++)
						{
							if (m == 0)
							{
								strLongText = vecText[m];
							}
							else if (vecText[m].length() > strLongText.length())
							{
								strLongText = vecText[m];
							}
						}
						if (pParam->vecText[j].bAdjustWidth)
						{
							//适配长度
							if (strText.empty())
							{
								str = pParam->vecText[j].strText;
								strLongText = str;
							}
							if (pParam->fMinWidth == 0.0f)
							{
								pParam->fMinWidth = pParam->fWidth;
							}
							float fLen = bc::AsciiNum(strLongText.data()) / 2.0f;
							float fWidth = fLen * pParam->vecText[j].vTextSize[0] + pParam->vecText[j].vTextPos[0] + pParam->vecText[j].vTextPos[0];
							if (fWidth < pParam->fMinWidth)
							{
								fWidth = pParam->fMinWidth;
							}
							pParam->fWidth = fWidth;
						}
						if (pParam->vecText[j].bWidthCenter)
						{
							if (strText.empty())
							{
								str = pParam->vecText[j].strText;
								strLongText = str;
							}
							float fLen = bc::AsciiNum(strLongText.data()) / 2.0f;
							float fTextWidth = fLen * pParam->vecText[j].vTextSize[0];
							float fStartX = (pParam->fWidth - fTextWidth) / 2;
							if (fTextWidth > pParam->fWidth)
							{
								fStartX = 0.0f;
							}
							pParam->vecText[j].vTextPos.x = fStartX;
						}
						if (vecText.size() >= 2 && pParam->vecText[j].nLineMaxLength <= 0)
						{
							float fHeight = vecText.size() * pParam->vecText[j].vTextSize[1] + pParam->vecText[j].vTextPos[1] + pParam->vecText[j].vTextSize[1];
							//if (fHeight < pParam->fMinWidth)
							//{
							//	fWidth = pParam->fMinWidth;
							//}
							pParam->fHeight = fHeight;
						}

						if (pParam->vecText[j].nLineMaxLength > 0 && strText.length() > pParam->vecText[j].nLineMaxLength)
						{
							std::string strTemp = "";
							std::vector<std::string> vecStr = CutString(strText, pParam->vecText[j].nLineMaxLength);
							int nLine = 1;
							for (int k = 0; k < vecStr.size(); k++)
							{
								strTemp += vecStr[k];
								if (k != vecStr.size() - 1)
								{
									strTemp += "\n";
									nLine++;
									if (pParam->vecText[j].nMaxLine > 0 && nLine > pParam->vecText[j].nMaxLine)
									{
										strTemp = strTemp.substr(0, strTemp.length() - 3);
										strTemp += "...";
										break;
									}
								}
							}

							pParam->vecText[j].strText = strTemp;
						}
						else
						{
							pParam->vecText[j].strText = str;
						}
						if (pParam->vecText[j].bHeightCenter)
						{
							float fTextHeight = pParam->vecText[j].GetTextHeight();
							float fStartY = (pParam->fHeight - fTextHeight) / 2;
							pParam->vecText[j].vTextPos.y = fStartY;
						}

						//不直接执行，数据量一大 RenderTime就会很大会卡顿
						//pFontBoard->iUpdateText();		
						ProjectBasedClient* pClient = (ProjectBasedClient*)APIProvider::GetSystemAPI()->iProjectClientAPI;
						pClient->AddRefreshFontBoardID(pFontBoard->iGetOriginNode()->iGetID());
						return &pParam->vecText[j];
					}
				}
			}
		}
	}
	return NULL;
}

void FindAllTextShow(bc::INode* pNode, std::vector<bc::TextShow*>& vec, const std::string& strKey)
{
	bc::IBoard* pBoard = pNode->GetDynamicComponent<bc::IBoard>();
	if (!pBoard)
	{
		return;
	}

	bc::BoardParam* pParam = dynamic_cast<bc::BoardParam*>(pBoard->iGetBoardParam());
	std::vector<bc::INode*> vecBoard;
	vecBoard.push_back(pNode);
	if (pParam->nNodeType == bc::NODE_FONTBOARD_WITH_BUTTON)
	{
		std::vector<bc::INode*> vec;
		pBoard->iGetButtonBoard(vec);
		vecBoard.insert(vecBoard.end(), vec.begin(), vec.end());
	}
	if (pParam->nNodeType == bc::NODE_BOARD_WITH_FONTBOARD)
	{
		bc::IBoard* pBtn = pBoard;
		if (pBtn->iGetFontBoard())
		{
			vecBoard.push_back(pBtn->iGetFontBoard());
		}
		if (pBtn->iGetDetailFontBoard())
		{
			vecBoard.push_back(pBtn->iGetDetailFontBoard());
		}
	}
	else if (pParam->nNodeType == bc::NODE_VIDEO_BOARD_WITH_BUTTON)
	{
		std::vector<bc::INode*> vecBtn;
		pBoard->iGetButtonBoard(vecBtn);
		vecBoard.insert(vecBoard.end(), vecBtn.begin(), vecBtn.end());
	}
	for (int i = 0; i < vecBoard.size(); i++)
	{
		bc::IBoard* pFontBoard = vecBoard[i]->GetDynamicComponent<IBoard>();
		if (pFontBoard)
		{
			bc::BoardParam* pTempParam = (bc::BoardParam*)pFontBoard->iGetBoardParam();
			bc::FontBoardParam* pParam = dynamic_cast<bc::FontBoardParam*>(pTempParam);
			if (pParam)
			{
				for (int j = 0; j < pParam->vecText.size(); j++)
				{
					if (strKey.empty() || pParam->vecText[j].strID.find(strKey) != std::string::npos)
					{
						vec.push_back(&pParam->vecText[j]);
					}
				}
			}
		}
	}
}

bc::INode* FindBoardContainTextShow(bc::INode* pNode, const std::string& strKey)
{
	bc::IBoard* pBoard = static_cast<bc::IBoard*>(pNode->iGetComponent(bc::COMPONENT_TYPE_BOARD));
	if (!pBoard)
	{
		return NULL;
	}

	bc::BoardParam* pParam = dynamic_cast<bc::BoardParam*>(pBoard->iGetBoardParam());
	std::vector<bc::INode*> vecBoard;
	vecBoard.push_back(pNode);
	if (pParam->nNodeType == bc::NODE_FONTBOARD_WITH_BUTTON)
	{
		std::vector<bc::INode*> vec;
		pBoard->iGetButtonBoard(vec);
		vecBoard.insert(vecBoard.end(), vec.begin(), vec.end());
	}
	else if (pParam->nNodeType == bc::NODE_BOARD_WITH_FONTBOARD)
	{
		bc::IBoard* pBtn = pBoard;
		if (pBtn->iGetFontBoard())
		{
			vecBoard.push_back(pBtn->iGetFontBoard());
		}
		if (pBtn->iGetDetailFontBoard())
		{
			vecBoard.push_back(pBtn->iGetDetailFontBoard());
		}
	}
	else if (pParam->nNodeType == bc::NODE_VIDEO_BOARD_WITH_BUTTON)
	{
		std::vector<bc::INode*> vecBtn;
		pBoard->iGetButtonBoard(vecBtn);
		vecBoard.insert(vecBoard.end(), vecBtn.begin(), vecBtn.end());
	}
	for (int i = 0; i < vecBoard.size(); i++)
	{
		bc::IBoard* pFontBoard = vecBoard[i]->GetDynamicComponent<IBoard>();
		if (pFontBoard)
		{
			bc::BoardParam* pTempParam = (bc::BoardParam*)pFontBoard->iGetBoardParam();
			bc::FontBoardParam* pParam = dynamic_cast<bc::FontBoardParam*>(pTempParam);
			if (pParam)
			{
				for (int j = 0; j < pParam->vecText.size(); j++)
				{
					if (strKey.empty() || pParam->vecText[j].strID.find(strKey) != std::string::npos)
					{
						return vecBoard[i];
					}
				}
			}
		}
	}

	return NULL;
}

bool IsContainerNodeFromNode(bc::INode* pContainerNode, bc::INode* pTargetNode)
{
	if (!pContainerNode)
	{
		return false;
	}
	std::vector<bc::INode*> vecBoard;
	vecBoard.push_back(pContainerNode);
	bc::IBoard* pBoard = static_cast<bc::IBoard*>(pContainerNode->iGetComponent(bc::COMPONENT_TYPE_BOARD));
	if (pBoard)
	{
		bc::BoardParam* pParam = dynamic_cast<bc::BoardParam*>(pBoard->iGetBoardParam());
		if (pParam->nNodeType == bc::NODE_FONTBOARD_WITH_BUTTON)
		{
			std::vector<bc::INode*> vec;
			pBoard->iGetButtonBoard(vec);
			vecBoard.insert(vecBoard.end(), vec.begin(), vec.end());
		}
		else if (pParam->nNodeType == bc::NODE_BOARD_WITH_FONTBOARD)
		{
			bc::IBoard* pBtn = pBoard;
			if (pBtn->iGetFontBoard())
			{
				vecBoard.push_back(pBtn->iGetFontBoard());
			}
			if (pBtn->iGetDetailFontBoard())
			{
				vecBoard.push_back(pBtn->iGetDetailFontBoard());
			}
		}
		else if (pParam->nNodeType == bc::NODE_VIDEO_BOARD_WITH_BUTTON)
		{
			std::vector<bc::INode*> vecBtn;
			pBoard->iGetButtonBoard(vecBtn);
			vecBoard.insert(vecBoard.end(), vecBtn.begin(), vecBtn.end());
		}
	}
	for (int i = 0; i < vecBoard.size(); i++)
	{
		if (vecBoard[i] == pTargetNode)
		{
			return true;
		}
	}
	return false;
}

std::string StrToSubNumStr(const std::string& strOrigin)
{
	//const char* pOrigin = strOrigin.c_str();
	//int cnt_index = 0, cnt_int = 0;
	//for (int i = 0; pOrigin[i] != '\0'; ++i) //当a数组元素不为结束符时.遍历字符串a.
	//{
	//	if (pOrigin[i] >= '0' && pOrigin[i] <= '9') //如果是数字.
	//	{
	//		cnt_int *= 10;//先乘以10保证先检测到的数字存在高位，后检测的存在低位
	//		cnt_int += pOrigin[i] - '0'; //数字字符的ascii-字符'0'的ascii码就等于该数字.
	//	}

	//	else if ((pOrigin[i] >= 'a' && pOrigin[i] <= 'z') || (pOrigin[i] >= 'A' && pOrigin[i] <= 'Z')) //如果是字母.
	//	{
	//		b[cnt_index++] = a[i]; //如果是字符,则增加到b数组中.
	//	}
	//}

	const char* pOrigin = strOrigin.c_str();
	std::string strResult = "";
	for (int i = 0; pOrigin[i] != '\0'; ++i) //当a数组元素不为结束符时.遍历字符串a.
	{
		if (pOrigin[i] >= '0' && pOrigin[i] <= '9') //如果是数字.
		{
			strResult += pOrigin[i];
		}
	}
	return strResult;
}

std::string EncodeBase64(const std::string& str)
{
	//编码表
	const char EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//返回值
	std::string strEncode;
	unsigned char Tmp[4] = { 0 };
	int LineLength = 0;
	int DataByte = str.length();
	const char* Data = str.c_str();
	for (int i = 0; i < (int)(DataByte / 3); i++)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		Tmp[3] = *Data++;
		strEncode += EncodeTable[Tmp[1] >> 2];
		strEncode += EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		strEncode += EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		strEncode += EncodeTable[Tmp[3] & 0x3F];
		if (LineLength += 4, LineLength == 76) { strEncode += "\r\n"; LineLength = 0; }
	}
	//对剩余数据进行编码
	int Mod = DataByte % 3;
	if (Mod == 1)
	{
		Tmp[1] = *Data++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4)];
		strEncode += "==";
	}
	else if (Mod == 2)
	{
		Tmp[1] = *Data++;
		Tmp[2] = *Data++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		strEncode += EncodeTable[((Tmp[2] & 0x0F) << 2)];
		strEncode += "=";
	}

	return strEncode;
}

std::string DecodeBase64(const std::string& str)
{
	//, const char* Data, int DataByte, int& OutByte
	int DataByte = str.length();
	const char* Data = str.c_str();
	//解码表
	const char DecodeTable[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		62, // '+'
		0, 0, 0,
		63, // '/'
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
		0, 0, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
		0, 0, 0, 0, 0, 0,
		26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
	};
	//返回值
	std::string strDecode;
	int nValue;
	int i = 0;
	while (i < DataByte)
	{
		if (*Data != '\r' && *Data != '\n')
		{
			nValue = DecodeTable[*Data++] << 18;
			nValue += DecodeTable[*Data++] << 12;
			strDecode += (nValue & 0x00FF0000) >> 16;
			//OutByte++;
			if (*Data != '=')
			{
				nValue += DecodeTable[*Data++] << 6;
				strDecode += (nValue & 0x0000FF00) >> 8;
				//OutByte++;
				if (*Data != '=')
				{
					nValue += DecodeTable[*Data++];
					strDecode += nValue & 0x000000FF;
					//OutByte++;
				}
			}
			i += 4;
		}
		else// 回车换行,跳过
		{
			Data++;
			i++;
		}
	}
	return strDecode;
}

std::string GetComputeMacAddress(std::vector<MacInfo>& vecMacAddress)
{
#ifdef _MSC_VER
	PIP_ADAPTER_INFO pIpInfo = NULL;

	//PIP_ADAPTER_INFO结构体指针存储本机网卡信息
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//得到结构体大小,用于GetAdaptersInfo参数
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//如果函数返回的是ERROR_BUFFER_OVERFLOW
		//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
		//这也是说明为什么stSize既是一个输入量也是一个输出量
		//释放原来的内存空间

		delete pIpAdapterInfo;
		//重新申请内存空间用来存储所有网卡信息
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}
	if (ERROR_SUCCESS == nRel)
	{
		pIpInfo = pIpAdapterInfo;
		char szIP[20] = { 0 };

		while (pIpInfo)
		{
			if (strcmp(pIpInfo->IpAddressList.IpAddress.String, "0.0.0.0") && (!strstr(pIpInfo->Description, "VMware")))
			{
				MacInfo stMacInfo;
				char sz[1024] = { 0 };
				sprintf_s(sz, sizeof(sz), "%02X:%02X:%02X:%02X:%02X:%02X",
					pIpInfo->Address[0], pIpInfo->Address[1],
					pIpInfo->Address[2], pIpInfo->Address[3],
					pIpInfo->Address[4], pIpInfo->Address[5]);
				stMacInfo.strMac = sz;
				stMacInfo.strIP = pIpInfo->IpAddressList.IpAddress.String;
				stMacInfo.strMask = pIpInfo->IpAddressList.IpMask.String;
				stMacInfo.strName = pIpInfo->AdapterName;
				stMacInfo.strDescription = pIpInfo->Description;

				vecMacAddress.push_back(stMacInfo);
			}

			pIpInfo = pIpInfo->Next;
		}while (pIpInfo);
	}

	//释放内存空间
	if (pIpAdapterInfo)
	{
		DELETE_PTR(pIpAdapterInfo);
	}
#elif __GNUC__
	int fd = 0, nNum = 0;
	struct ifreq ifNet[MAXINTERFACES];
	struct ifconf ifc;
	struct ifreq ifrcopy;
	char szMac[32] = { 0 };
	char szIp[32] = { 0 };
	char szMask[32] = { 0 };

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
	{
		ifc.ifc_len = sizeof(ifNet);
		ifc.ifc_buf = (caddr_t)ifNet;
		if (!ioctl(fd, SIOCGIFCONF, (char*)&ifc))
		{
			nNum = ifc.ifc_len / sizeof(struct ifreq);    //网卡总数
			while (nNum-- > 0)
			{
				MacInfo stMacInfo;
				memset(szMac, 0, sizeof(szMac));
				memset(szIp, 0, sizeof(szIp));
				memset(szMask, 0, sizeof(szMask));

				//ignore the interface that not up or not runing  
				ifrcopy = ifNet[nNum];
				if (ioctl(fd, SIOCGIFFLAGS, &ifrcopy))
				{
					continue;
				}

				//get the mac of this interface
				if (!ioctl(fd, SIOCGIFHWADDR, (char*)(&ifNet[nNum])))
				{
					snprintf(szMac, sizeof(szMac), "%02X:%02X:%02X:%02X:%02X:%02X",
						(unsigned char)ifNet[nNum].ifr_hwaddr.sa_data[0],
						(unsigned char)ifNet[nNum].ifr_hwaddr.sa_data[1],
						(unsigned char)ifNet[nNum].ifr_hwaddr.sa_data[2],
						(unsigned char)ifNet[nNum].ifr_hwaddr.sa_data[3],
						(unsigned char)ifNet[nNum].ifr_hwaddr.sa_data[4],
						(unsigned char)ifNet[nNum].ifr_hwaddr.sa_data[5]);
				}

				//get the IP of this interface  
				if (!ioctl(fd, SIOCGIFADDR, (char*)&ifNet[nNum]))
				{
					snprintf(szIp, sizeof(szIp), "%s",
						(char*)inet_ntoa(((struct sockaddr_in*)&(ifNet[nNum].ifr_addr))->sin_addr));
				}

				//get the subnet mask of this interface  
				if (!ioctl(fd, SIOCGIFNETMASK, &ifNet[nNum]))
				{
					snprintf(szMask, sizeof(szMask), "%s",
						(char*)inet_ntoa(((struct sockaddr_in*)&(ifNet[nNum].ifr_netmask))->sin_addr));
				}

				stMacInfo.strMac = szMac;
				stMacInfo.strIP = szIp;
				stMacInfo.strMask = szMask;
				stMacInfo.strName = ifNet[nNum].ifr_name;
				stMacInfo.strDescription = "";

				vecMacAddress.push_back(stMacInfo);
			}
		}

		close(fd);
	}
#endif

	std::string strMacList = "";
	for (int i = 0; i < vecMacAddress.size(); i++)
	{
		if (i == 0)
		{
			strMacList = vecMacAddress[i].strMac;
		}
		else
		{
			strMacList = strMacList + std::string(",") + std::string(vecMacAddress[i].strMac);
		}
	}
	return strMacList;
}

std::string GetComputeName()
{
	//获取计算机名
	//初始化:如果不初始化，以下代码将无法执行
	std::string pcName = "";
#ifdef _MSC_VER
	WSAData data;
	if (WSAStartup(MAKEWORD(1, 1), &data) != 0)
	{
		bc::ProjectLogger::GetInstance()->ErrorMessage("初始化错误,无法获取主机信息");
	}
#endif

	char host[255];
	//获取主机名:也可以使用GetComputerName()这个函数
	if (gethostname(host, sizeof(host)) == SOCKET_ERROR)
	{
		bc::ProjectLogger::GetInstance()->ErrorMessage("无法获取主机名");
	}
	else
	{
		pcName = host;
	}

	return pcName;
}

Json::Value GetJsonItemRecursion(Json::Value root, std::string strRecursionKeys)
{
	std::vector<std::string> vecTree;
	SplitStringBySpecial(strRecursionKeys, vecTree, ",");
	Json::Value value(Json::nullValue);
	Json::Value item = root;
	try
	{
		if (vecTree.size() >= 1)
		{
			for (int i = 0; i < vecTree.size(); i++)
			{
				if (item.isNull())
				{
					break;
				}
				if (item.isObject() && item.isMember(vecTree[i]))
				{
					item = item[vecTree[i]];
				}
				else
				{
					break;
				}
				if (i == (vecTree.size() - 1))
				{
					value = item;
					return value;
				}
			}
		}
	}
	catch (const std::exception&)
	{
	}
	return value;
}

void ParseJsonByString(std::string& str, Json::Value& jResult)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	std::string jsError;
	if (!reader->parse(str.c_str(), str.c_str() + str.length(), &jResult, &jsError))
	{
		jResult = Json::nullValue;
	}
}

void SplitToCircle(bc::Vector3d vCenterPos, float fRadius, std::vector<bc::Vector3d>& vecPoints)
{
	//分割
	int nCount = 100;
	float fDeltaAngle = 360.0f / nCount;
	for (int zz = 0; zz < nCount; zz++)
	{
		bc::Vector3d vScenePos;
		float fAngle = zz * fDeltaAngle;
		double fX = vCenterPos.x + fRadius * sin(DEG_TO_RAD(fAngle));
		double fY = vCenterPos.y + fRadius * cos(DEG_TO_RAD(fAngle));
		vScenePos.x = fX;
		vScenePos.y = fY;
		vScenePos.z = vCenterPos.z;

		vecPoints.push_back(vScenePos);
	}
}

void SetINTForNodeWithDetail(bc::INode* pNode)
{
	if (!pNode)
	{
		return;
	}
	bc::IBoard* pBoard = static_cast<bc::IBoard*>(pNode->iGetComponent(bc::COMPONENT_TYPE_BOARD));
	if (!pBoard)
	{
		return;
	}
	bc::BoardParam* pParam = dynamic_cast<bc::BoardParam*>(pBoard->iGetBoardParam());
	if (!pParam)
	{
		return;
	}
	std::vector<bc::INode*> vecBoard;
	std::vector<bc::INode*> vecVideo;
	if (pParam->nNodeType == bc::NODE_FONTBOARD_WITH_BUTTON)
	{
		std::vector<bc::INode*> vec;
		pBoard->iGetButtonBoard(vec);
		vecBoard.insert(vecBoard.end(), vec.begin(), vec.end());
	}
	else if (pParam->nNodeType == bc::NODE_VIDEO_BOARD_WITH_BUTTON)
	{
		std::vector<bc::INode*> vecBtn;
		pBoard->iGetButtonBoard(vecBtn);
		vecBoard.insert(vecBoard.end(), vecBtn.begin(), vecBtn.end());
		std::vector<bc::INode*> vecVi;
		pBoard->iGetVideoBoard(vecVi);
		vecVideo.insert(vecVideo.end(), vecVi.begin(), vecVi.end());
	}

	for (int i = 0; i < vecBoard.size(); i++)
	{
		if (vecBoard[i])
		{
			vecBoard[i]->iSetExternParam((INT_PTR)pNode);
		}
	}
	for (int i = 0; i < vecVideo.size(); i++)
	{
		if (vecVideo[i])
		{
			vecVideo[i]->iSetExternParam((INT_PTR)pNode);
		}
	}
}

void SetINTForNodeWithLabel(bc::INode* pNode)
{
	if (!pNode)
	{
		return;
	}
	bc::IBoard* pBoard = static_cast<bc::IBoard*>(pNode->iGetComponent(bc::COMPONENT_TYPE_BOARD));
	if (!pBoard)
	{
		return;
	}
	bc::BoardParam* pParam = dynamic_cast<bc::BoardParam*>(pBoard->iGetBoardParam());
	if (!pParam)
	{
		return;
	}
	std::vector<bc::INode*> vecBoard;
	std::vector<bc::INode*> vecVideo;
	if (pParam->nNodeType == bc::NODE_FONTBOARD_WITH_BUTTON)
	{
		std::vector<bc::INode*> vec;
		pBoard->iGetButtonBoard(vec);
		vecBoard.insert(vecBoard.end(), vec.begin(), vec.end());
	}
	else if (pParam->nNodeType == bc::NODE_BOARD_WITH_FONTBOARD)
	{
		vecBoard.push_back(pBoard->iGetFontBoard());
	}
	else if (pParam->nNodeType == bc::NODE_VIDEO_BOARD_WITH_BUTTON)
	{
		std::vector<bc::INode*> vecBtn;
		pBoard->iGetButtonBoard(vecBtn);
		vecBoard.insert(vecBoard.end(), vecBtn.begin(), vecBtn.end());
		std::vector<bc::INode*> vecVideo;
		pBoard->iGetVideoBoard(vecVideo);
		vecVideo.insert(vecVideo.end(), vecVideo.begin(), vecVideo.end());
	}

	for (int i = 0; i < vecBoard.size(); i++)
	{
		if (vecBoard[i])
		{
			vecBoard[i]->iSetExternParam(pNode->iGetExternParam());
		}
	}
	for (int i = 0; i < vecVideo.size(); i++)
	{
		if (vecVideo[i])
		{
			vecVideo[i]->iSetExternParam(pNode->iGetExternParam());
		}
	}
}

float _getCross(bc::Vector2& v1, bc::Vector2& v2, bc::Vector2& vPos) {
	return (v2.x - v1.x) * (vPos.y - v1.y) - (vPos.x - v1.x) * (v2.y - v1.y);
}

bool IsPointInRect(bc::Vector2& vPoint, bc::Vector2& vLeftBottom, bc::Vector2& vLeftTop,
	bc::Vector2& vRightTop, bc::Vector2& vRightBottom)
{
	bool isPointIn = _getCross(vLeftBottom, vLeftTop, vPoint) * _getCross(vRightTop, vRightBottom, vPoint) >= 0 &&
		_getCross(vLeftTop, vRightTop, vPoint) * _getCross(vRightBottom, vLeftBottom, vPoint) >= 0;
	return isPointIn;
}

float GetZFromOpenglCamera(INode* pCameraNode, Vector3d& vPos)
{
	if (!pCameraNode)
	{
		return 0.0f;
	}
	IManipulator* pManipulator = pCameraNode->GetDynamicComponent<IManipulator>();
	ICamera* pCamera = pCameraNode->GetDynamicComponent<ICamera>();
	if (pCamera->iCullPoint(vPos))
	{
		//不在相机范围内
		return 0.0f;
	}
	Vector3d vScreenPos;
	//转到屏幕坐标
	pCamera->iProject(vPos, vScreenPos);
	//转到场景坐标，包含高度Z
	Vector3d vResult;
	pCamera->iUnProject(vScreenPos.x, vScreenPos.y, vResult, true);
	return vResult.z;
}

bool IsUTF8String(const std::string& string)
{
	int c, i, ix, n, j;
	for (i = 0, ix = string.length(); i < ix; i++)
	{
		c = (unsigned char)string[i];
		//if (c==0x09 || c==0x0a || c==0x0d || (0x20 <= c && c <= 0x7e) ) n = 0; // is_printable_ascii
		if (0x00 <= c && c <= 0x7f) n = 0; // 0bbbbbbb
		else if ((c & 0xE0) == 0xC0) n = 1; // 110bbbbb
		else if (c == 0xed && i < (ix - 1) && ((unsigned char)string[i + 1] & 0xa0) == 0xa0) return false; //U+d800 to U+dfff
		else if ((c & 0xF0) == 0xE0) n = 2; // 1110bbbb
		else if ((c & 0xF8) == 0xF0) n = 3; // 11110bbb
		//else if (($c & 0xFC) == 0xF8) n=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
		//else if (($c & 0xFE) == 0xFC) n=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
		else return false;
		for (j = 0; j < n && i < ix; j++) { // n bytes matching 10bbbbbb follow ?
			if ((++i == ix) || (((unsigned char)string[i] & 0xC0) != 0x80))
				return false;
		}
	}
	return true;
}

bool IsDir(const std::string& strPath)
{
	bool bDir = false;
	const char* folderPath = strPath.c_str();
	struct stat info;
	if (stat(folderPath, &info) != 0) 
	{
		//printf("%s does not exist.\n", folderPath);
	} 
	else if (info.st_mode & S_IFDIR) 
	{
		bDir = true;
		//printf("%s exists and is a directory.\n", folderPath);
	} 
	
    return bDir;

/*	
	//定义文件信息的结构体
	_finddata_t FileInfo;

	const char* c_path = strPath.c_str();
	long Handle = _findfirst(c_path, &FileInfo);
	bool bDir = false;
	do
	{
		//返回值为-1，表示未找到，返回
		if (Handle == -1)
		{
			bDir = false;
		}
		if (FileInfo.attrib == _A_SUBDIR)
		{
			bDir = true;
		}
		break;
	} while (true);
	//_findclose(Handle);

	return bDir;
*/
}

std::vector<Vector3> ConvertVecVector3dTo(std::vector<Vector3d>& vecVector3d)
{
	std::vector<Vector3> vecPoint;
	for (size_t i = 0; i < vecVector3d.size(); ++i)
	{
		vecPoint.push_back(vecVector3d[i]);
	}
	return vecPoint;
}

std::vector<Vector3d> ConvertVecVector3ToD(std::vector<Vector3>& vecVector3)
{
	std::vector<Vector3d> vecPoint;
	for (size_t i = 0; i < vecVector3.size(); ++i)
	{
		vecPoint.push_back(Vector3d(vecVector3[i]));
	}
	return vecPoint;
}


bool GetProjectVersion(const std::string& strFilePath, std::string& strVersion)
{
#ifdef _WIN32
	// 获取DLL文件的版本信息
	DWORD handle;
	DWORD size = GetFileVersionInfoSize(strFilePath.data(), &handle);
	if (size == 0)
	{
		std::cout << "Failed to get DLL version info size." << std::endl;
		return false;
	}
	std::vector<char> buffer(size);
	if (!GetFileVersionInfo(strFilePath.data(), handle, size, buffer.data()))
	{
		std::cout << "Failed to get DLL version info." << std::endl;
		return false;
	}
	// 获取版本信息
	DWORD* versionInfo;
	UINT versionInfoSize;
	LPVOID lpBuff = NULL;
	if (!VerQueryValue(buffer.data(), "\\VarFileInfo\\Translation", (void**)&versionInfo, &versionInfoSize))
	{
		std::cout << "Failed to get DLL version info." << std::endl;
		return false;
	}
	DWORD m_dwLangCharset = MAKELONG(HIWORD(versionInfo[0]), LOWORD(versionInfo[0]));
	char Block[1024] = { 0 };
	sprintf_s(Block, 1024, "\\StringFileInfo\\%08lx\\FileDescription", m_dwLangCharset);
	VerQueryValueA(buffer.data(), Block, &lpBuff, &versionInfoSize);
	if (versionInfoSize)
	{
		strVersion = (char*)lpBuff;
		return true;
	}
#endif
	return false;
}

PROJECT_BASED_API std::string GetProjectExecuteName() 
{
	std::string strResult;
#ifdef _WIN32
	
	char exePath[MAX_PATH + 1] = { 0 };
	GetModuleFileNameA(NULL, exePath, MAX_PATH);
	std::string strExePath(exePath);
	strResult = Replace_all(strExePath, "\\", "/");
	strResult = strResult.substr(strResult.find_last_of("/") + 1, strResult.find_last_of(".") - strResult.find_last_of("/") - 1);
#endif
	std::transform(strResult.begin(), strResult.end(),
		strResult.begin(), [](unsigned char c)
		{
			return std::tolower(c);
		});
	return strResult;
}
