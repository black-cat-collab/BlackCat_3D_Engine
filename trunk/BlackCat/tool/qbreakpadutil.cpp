#include "BlackCat/qBreakpad/qbreakpadutil.h"
#include <QCoreApplication>

QBreakpadUtil::QBreakpadUtil()
{
    QString strPath = QCoreApplication::applicationDirPath() + "/crash";
    SetDumpPath(strPath);
}

QBreakpadUtil::~QBreakpadUtil()
{

}

void QBreakpadUtil::SetDumpPath(const QString &strPtah)
{
    QBreakpadInstance.setDumpPath(strPtah);
}
