#ifndef BLACKCAT_GLOBAL_H
#define BLACKCAT_GLOBAL_H

#include <QtCore/qglobal.h>

#if (defined(__aarch64__)  || (__ARM_ARCH == 8))


#if defined(BLACKCAT_LIBRARY)
#  define BLACKCATSHARED_EXPORT  __attribute__((visibility("default")))
#else
#  define BLACKCATSHARED_EXPORT
#endif




#else
#if defined(BLACKCAT_LIBRARY)
#  define BLACKCATSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BLACKCATSHARED_EXPORT Q_DECL_IMPORT
#endif


#endif




#endif // BLACKCAT_GLOBAL_H
