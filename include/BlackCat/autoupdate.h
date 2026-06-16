#ifndef AUTOUPDATE_H
#define AUTOUPDATE_H

#include <QApplication>
#include <QtXml>
#include <QProcess>
#include "blackcat_global.h"

class BLACKCATSHARED_EXPORT AutoUpdate
{
public:
    explicit AutoUpdate();

    //strDesktopName快捷方式
    static bool StartAutoUpdate(const QString& strConfName,QString strDesktopName = "");
    static void WriteLog(const char *pData);
};

#endif // AUTOUPDATE_H
