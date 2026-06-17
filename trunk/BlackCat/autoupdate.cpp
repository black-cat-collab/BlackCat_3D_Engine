#include "BlackCat/autoupdate.h"
#ifdef __GNUC__
#else
#include <iostream>
#include <Windows.h>
#pragma comment(lib,"version.lib")
#endif

AutoUpdate::AutoUpdate()
{

}

void AutoUpdate::WriteLog(const char *pData)
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss");
    str = "[" + str;
    str += "]";

    str += " " + QCoreApplication::applicationName();
    str += "## ";

    QString strFileString = str + "  " + pData;
    QString strPath = QCoreApplication::applicationDirPath() + "/project_logs/AutoUpdate";
    QDir dir(strPath);
    if(!dir.exists())
    {
        dir.mkdir(strPath);
    }
    QString strFileName = qAppName() + ".txt";
    QFile file(strPath + "/" + strFileName);
    bool isOK = false;
    if(file.size() >= 20*1024*1024)
    {
        isOK = file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    }
    else
    {
        isOK = file.open(QIODevice::WriteOnly | QIODevice::Append);
    }
    if(isOK)
    {

        QTextStream textStream(&file);
        textStream << strFileString << "\n";
        file.flush();
        file.close();
    }
}

bool AutoUpdate::StartAutoUpdate(const QString& strConfName,QString strDesktopName)
{
#ifdef __GNUC__
    return false;
#else
    /**获取文件的版本信息**/
    std::string str_data_dir;//数据目录
    std::string str_client_dll;//加载客户端动态库名称
    std::string str_application_name;//应用程序名称
    bool b_background_render = false;//云渲染
    std::string str_server_ip = "http://127.0.0.1:3333";//服务器地址
    QDomDocument docment;
    QFile file(strConfName + ".conf");
    QString strMsg = QString("conf file=%1").arg(strConfName + ".conf");
    WriteLog(strMsg.toStdString().c_str());
    if(file.open(QIODevice::ReadOnly) && docment.setContent(&file))
    {
        QString strMsg = QString("conf file open success!");
        WriteLog(strMsg.toStdString().c_str());
        QDomElement root = docment.documentElement();
        if(!root.isNull())
        {
            QDomNodeList nodeList = root.childNodes();
            for(int i = 0; i < nodeList.size(); i++)
            {
                if(nodeList.item(i).nodeName() == "data_dir" && nodeList.item(i).isElement())
                {
                    str_data_dir = QString(nodeList.item(i).toElement().text()).toStdString();
                }
                if(nodeList.item(i).nodeName() == "client_dll" && nodeList.item(i).isElement())
                {
                    str_client_dll = QString(nodeList.item(i).toElement().text()).toStdString();
                }
                if(nodeList.item(i).nodeName() == "application_name" && nodeList.item(i).isElement())
                {
                    str_application_name = QString(nodeList.item(i).toElement().text()).toStdString();
                }
                if(nodeList.item(i).nodeName() == "background_render" && nodeList.item(i).isElement())
                {
                    std::string str_background_render = QString(nodeList.item(i).toElement().text()).toStdString();
                    b_background_render = (str_background_render == "true") ? true : false;
                }
                if(nodeList.item(i).nodeName() == "server_ip" && nodeList.item(i).isElement())
                {
                    str_server_ip = QString(nodeList.item(i).toElement().text()).toStdString();
                }
            }
        }
    }
    file.close();
    if(!b_background_render && !str_client_dll.empty())
    {
        str_client_dll = QString(QApplication::applicationDirPath()+"/").toStdString() + str_client_dll + ".dll";
        //qDebug()<<QString::fromLocal8Bit(str_client_dll.c_str());
        char* pString = (char*)str_client_dll.c_str();
        int nSize = MultiByteToWideChar(CP_OEMCP, 0, pString, strlen(pString) + 1, NULL, 0);
        LPWSTR pFilePath = new wchar_t[nSize];
        MultiByteToWideChar(CP_OEMCP, 0, pString, strlen(pString) + 1, pFilePath, nSize);
        DWORD dwSize = GetFileVersionInfoSize(pFilePath, NULL);

        {
            QString strMsg = QString("client name=%1").arg(str_client_dll.c_str());
            WriteLog(strMsg.toStdString().c_str());
        }

        if (dwSize > 0)
        {
            char* pBuff = new char[dwSize + 1];
            memset(pBuff, 0, dwSize + 1);
            DWORD dwSuccess = GetFileVersionInfo(pFilePath, NULL, dwSize, pBuff);
            {
                QString strMsg = QString("GetFileVersionInfo result=%1").arg(dwSuccess?"true":"false");
                WriteLog(strMsg.toStdString().c_str());
            }
            if (dwSuccess != 0)
            {
                LPVOID lpBuff = NULL;
                UINT uLen = 0;
                DWORD* pTransTable;
                dwSuccess = VerQueryValue(pBuff, L"\\VarFileInfo\\Translation", (void**)&pTransTable, &uLen);
                DWORD m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
                char Block[1024] = { 0 };
                sprintf_s(Block, 1024, "\\StringFileInfo\\%08lx\\FileDescription", m_dwLangCharset);
                VerQueryValueA(pBuff, Block, &lpBuff, &uLen);
                if (uLen)
                {
                    std::string strVersion = (char*)lpBuff;
                    {
                        QString strMsg = QString("dll version=%1").arg(strVersion.c_str());
                        WriteLog(strMsg.toStdString().c_str());
                    }
                    //qDebug()<<QString::fromLocal8Bit(strVersion.c_str());
                    //启动更新程序
                    QDir dir(QApplication::applicationDirPath());
                    QString strMsg = QString("self exe path=%1").arg(dir.path());
                    WriteLog(strMsg.toStdString().c_str());

                    QStringList nameList;
                    nameList << "*.exe";
                    QFileInfoList fileList = dir.entryInfoList(nameList, QDir::Filter::Files|QDir::Filter::NoDotAndDotDot);
                    for(auto& fileInfo : fileList)
                    {
                        if(fileInfo.filePath().indexOf("AutoUpdate") > 0)
                        {
                            QProcess programUpdate;
                            QString exePath(fileInfo.filePath());
                            QStringList argsList;
                            //"1.0.2001.20231122"
                            argsList << QString::fromLocal8Bit(strVersion.c_str());
                            //"TZSJPT"
                            argsList << QString::fromLocal8Bit(str_application_name.c_str());
                            //"F:/HCTECH_Engine/HCProject/Project_Double/HCTZPlatform_Test/Release/HCTZPlatform.exe"
                            argsList << QApplication::applicationDirPath() + "/" + QApplication::applicationName() + ".exe";
                            //"F:\\HCTECH_Engine\\data"
                            argsList << QString::fromLocal8Bit(str_data_dir.c_str());
                            //"http://192.168.30.24:12144"
                            argsList << QString::fromLocal8Bit(str_server_ip.c_str());
                            //qDebug() << argsList;
                            argsList << "1";
                            argsList << "1";
                            argsList << strDesktopName.toUtf8();
                            programUpdate.setProgram(exePath);
                            programUpdate.setArguments(argsList);

                            QString strMsg = QString("will start AutoUpdate.exe");
                            WriteLog(strMsg.toStdString().c_str());

                            if(programUpdate.startDetached())
                            {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
#endif

}
