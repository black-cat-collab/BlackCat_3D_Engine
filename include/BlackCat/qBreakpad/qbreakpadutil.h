#ifndef QBREAKPADUTIL_H
#define QBREAKPADUTIL_H

#include "QBreakpadHandler.h"
#include "../blackcat_global.h"

class BLACKCATSHARED_EXPORT QBreakpadUtil
{
public:
    QBreakpadUtil();
    virtual ~QBreakpadUtil();

    void SetDumpPath(const QString &strPtah);
};

#endif // QBREAKPADUTIL_H
