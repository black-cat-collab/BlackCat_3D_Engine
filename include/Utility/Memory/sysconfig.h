#ifndef  _SYSTEM_CONFIG_
#define  _SYSTEM_CONFIG_

#define  BUILD_PLAT_WIN32 0
#define  BUILD_PLAT_GCC 1

#if defined WIN32
#define BUILD_PLAT BUILD_PLAT_WIN32
#elif defined ANDROID_NDK
#define BUILD_PLAT  BUILD_PLAT_GCC
#else 
#define BUILD_PLAT  BUILD_PLAT_GCC
#endif


#if BUILD_PLAT == BUILD_PLAT_GCC
#define PROJECT_KEEBUILDER
#else
//#define PROJECT_KEESHOW
//#define PRJOECT_KEE3D
//#define PROJECT_KEEBUILDER
#endif

#endif

