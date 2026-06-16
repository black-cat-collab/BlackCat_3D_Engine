#pragma once

#ifdef __GNUC__

#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>	

typedef	unsigned long long				SOCKET;

#define	_In_
#define _MAX_PATH  						260 
#define MAXINTERFACES 					16
#define SOCKET_ERROR           			(-1)

#define	INVALID_SOCKET					(SOCKET)(~0)

#define WSAETIMEDOUT              		10060L
#define VK_SHIFT          				0x10
#define	VK_CONTROL						0x11
#define VK_MENU							0x12
#define	VK_SPACE						0x20
#define	VK_LEFT							0x25
#define	VK_UP							0x26
#define	VK_RIGHT						0x27
#define	VK_DOWN							0x28

#define VK_LSHIFT         				0xA0
#define VK_RSHIFT         				0xA1
#define VK_LCONTROL       				0xA2
#define VK_RCONTROL       				0xA3
#define VK_LMENU          				0xA4
#define VK_RMENU          				0xA5


//////////////render
#define	HRESULT_FROM_WIN32	
#define	ERROR_INVALID_DATA				13L
#define	ERROR_NOT_SUPPORTED				50L


//typedef unsigned long		DWORD;
typedef	void*							HINSTANCE;
typedef	unsigned short					USHORT;
typedef	unsigned long					ULONG;
typedef	unsigned char					UCHAR;
typedef	unsigned int					UINT;

typedef	void							*HANDLE;
typedef	char							TCHAR, * PTCHAR;
//typedef	unsigned long					DWORD;



#define	strncpy_s(dest, m, src, n)		strncpy(dest, src, n)
#define	strcpy_s(dest, n, src)		   	strncpy(dest, src, n)


#define	OutputDebugString(str)			printf(str)

#define	_stricmp						strcasecmp
#define	_strnicmp						strncasecmp
#define	_mkdir(dir)						mkdir(dir, 0755)

#define	sprintf_s						snprintf
#define	sscanf_s			    		sscanf
#define fscanf_s 						fscanf        
#define	localtime_s(tm, ti)				localtime_r(ti, tm)
#define	Localtime_r(tm, ti)				localtime_r(ti, tm)
#define	_access							access


#define	Sleep(x)		  				sleep((x)/1000)

#define	IN_ADDR							in_addr
#define	HOSTENT							hostent
#define	_finddata_t						dirent*






#define	CORE_API			__attribute__((visibility("default")))
#define ENGINE_API			__attribute__((visibility("default")))
#define PROTOCOL_API		__attribute__((visibility("default")))
#define	MODEL_API			__attribute__((visibility("default")))
#define	RENDER_API			__attribute__((visibility("default")))
#define	PROJECT_BASED_API	__attribute__((visibility("default")))



 
typedef struct _FILETIME
{
    unsigned long dwLowDateTime;
    unsigned long dwHighDateTime;
} FILETIME, * PFILETIME, * LPFILETIME;
 



#define	DECLARE_HANDLE(name)	struct name##__{int unused;}; typedef struct name##__ *name

#define USE_GDAL     
#define USE_FFMPEG   

#else
#define USE_GDAL     
#define USE_FFMPEG     
#define USE_HIKVISION  

#endif

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

