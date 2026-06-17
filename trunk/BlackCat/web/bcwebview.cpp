#include "BlackCat/bcwebview.h"
#include <QWebEngineSettings>
#include <QWebEngineCookieStore>
#include <QWebEngineProfile>
#include <QNetworkProxy>
#include <QWebEnginePage>
#include <QUrl>
#include <QAuthenticator>
#include <QDir>
#include <QTimer>
#include <QPalette>
#include "bcwebenginepage.h"

BCWebView::BCWebView(QWidget *parent, QString strLoadMovie, QSize loadMovieSize)
    :QWebEngineView(parent)
{
    //setContextMenuPolicy(Qt::CustomContextMenu);

    m_bEmitInputSignal = true;

    setAutoFillBackground(true);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    setStyleSheet("background:transparent;");
    setAcceptDrops(false);

    m_pWebChannelObject = NULL;
    m_pWebChannel   = NULL;
    m_pLoadMovie = NULL;
    m_pLoadLabel = NULL;
    m_bRunJs    = false;
    m_bService  = false;
    m_bLoad = false;

    //    m_pReloadAct  = new QAction(QStringLiteral("重新加载"), this);
    //    m_pReloadAct->setShortcut(Qt::Key_F5);

    //    connect(m_pReloadAct, &QAction::triggered, [this]()
    //    {
    //        ReloadPage();
    //    });

    //    connect(this, &QWidget::customContextMenuRequested, this, [this]()
    //    {
    //        QMenu menu(this);
    //        menu.addAction(m_pReloadAct);
    //        menu.exec(QCursor::pos());
    //    });

#ifdef BCQT6
    QWebEngineProfile *profile = new QWebEngineProfile(QCoreApplication::applicationName());
    profile->setHttpAcceptLanguage("Access-Control-Allow-Origin");
    m_pWebPage = new HCWebEnginePage(profile,this);
    m_pWebPage->setBackgroundColor(Qt::transparent);
    setPage(m_pWebPage);
#else
    m_pWebPage  = new BCWebEnginePage(this);
    m_pWebPage->setBackgroundColor(Qt::transparent);
    setPage(m_pWebPage);
#endif
    clearCookie();

    QWebEngineSettings *websetting = QWebEngineProfile::defaultProfile()->settings();
    websetting->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    websetting->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    websetting->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);

    m_pLoadLabel = new QLabel(this);
    m_pLoadLabel->resize(loadMovieSize);
    m_pLoadLabel->move(0, 0);
    m_pLoadLabel->setStyleSheet("background:transparent");
    m_pLoadLabel->show();

    m_pLoadMovie = new QMovie(strLoadMovie, QByteArray(), this);
    m_pLoadMovie->setScaledSize(QSize(m_pLoadLabel->size().width() * 1/*MainWindow::ResolutionXScale()*/,
                                      m_pLoadLabel->size().height() * 1/*MainWindow::ResolutionYScale()*/));
    m_pLoadLabel->setMovie(m_pLoadMovie);
    m_pLoadMovie->stop();

    m_pLoadLabel->raise();
    m_pLoadLabel->show();
    m_pLoadMovie->start();

    connect(this, SIGNAL(loadStarted()), this, SLOT(slotLoadStarted()));
//    connect(this, SIGNAL(loadProgress(int)), this, SLOT(slotLoadProgress(int)));
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadFinished(bool)));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(slotSelectionText()));
//    connect(m_pWebPage, SIGNAL(featurePermissionRequested(QUrl,QWebEnginePage::Feature)),SLOT(slotPermissionRequested(QUrl,QWebEnginePage::Feature)));


    //    connect(this, &QWebEngineView::renderProcessTerminated,
    //            [this](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {
    //        QString status;
    //        switch (termStatus) {
    //        case QWebEnginePage::NormalTerminationStatus:
    //            status = tr("Render process normal exit");
    //            break;
    //        case QWebEnginePage::AbnormalTerminationStatus:
    //            status = tr("Render process abnormal exit");
    //            break;
    //        case QWebEnginePage::CrashedTerminationStatus:
    //            status = tr("Render process crashed");
    //            break;
    //        case QWebEnginePage::KilledTerminationStatus:
    //            status = tr("Render process killed");
    //            break;
    //        }
    //        QTimer::singleShot(0, [this] { reload(); });
    //    });
}

void BCWebView::ReloadPage()
{
    //reload();
    QUrl s_url = url();
    if (m_pWebPage)
    {
        m_pWebPage->load(s_url);
    }
}

void BCWebView::slotProxyAuthenticationRequired(const QUrl & requestUrl, QAuthenticator * authenticator, const QString & proxyHost)
{
    authenticator->setUser("username");
    authenticator->setPassword("password");
}

BCWebView::~BCWebView()
{
    qDebug()<<"delete BCWebView";
    DeregisterWebChannel();
}

void BCWebView::clearCookie()
{
    if(page())
    {

        QWebEngineProfile * engineProfile = page()->profile();
        if(engineProfile)
        {
            QString cachePath = engineProfile->cachePath();

            QDir dir(cachePath);
            if(!dir.exists())
            {
                return;
            }

//            engineProfile->clearHttpCache();
            QWebEngineCookieStore *cookie = engineProfile->cookieStore();
            if(cookie)
            {
//                cookie->deleteAllCookies();
            }

            DelDir(cachePath);
        }
    }
}

bool BCWebView::DelDir(QString strDir)
{
    if (strDir.isEmpty()){
        return false;
    }
    QDir dir(strDir);
    if(!dir.exists()){
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo file, fileList){
        if (file.isFile()){
            file.dir().remove(file.fileName());
        }else{
            DelDir(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath());
}

void BCWebView::SetWebChannelObject(QObject *pObj, QString name)
{
    m_pWebChannelObject = pObj;
    m_WebChannelName    = name;
}

void BCWebView::DeregisterWebChannel()
{
    if(m_pWebChannel && m_bLoad)
    {
        m_pWebChannel->deregisterObject(m_pWebChannelObject);
        delete m_pWebChannel;
        m_pWebChannel   = NULL;
    }
}

void BCWebView::SetUrl(QString url)
{
    m_bLoad = false;

    setUrl(QUrl(url));
}

void BCWebView::SetServiceUrl(QString url)
{
    m_strServiceUrl = url;
    m_bService  = true;
}

void BCWebView::SaveWebtoPDF(QString path)
{
    m_pWebPage->printToPdf(path);
}

void BCWebView::RunJavaScript(QString js)
{
    if(m_bLoad)
    {
        m_pWebPage->runJavaScript(js);
    }
    else
    {
        m_bRunJs    = true;
        m_strJS = js;
    }
}

void BCWebView::showEvent(QShowEvent *pEvent)
{
    if(m_pLoadLabel)
    {
        m_pLoadLabel->move((size().width() - m_pLoadLabel->size().width()) / 2,
                           (size().height() - m_pLoadLabel->size().height()) / 2);
    }

    this->setAttribute(Qt::WA_Mapped);
    QWebEngineView::showEvent(pEvent);
}

void BCWebView::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
       if (!isMinimized())
       {
           setAttribute(Qt::WA_Mapped);
       }
    }
    QWebEngineView::changeEvent(event);
}

void BCWebView::wheelEvent(QWheelEvent *pEvent)
{
    pEvent->accept();
}

void BCWebView::contextMenuEvent(QContextMenuEvent *event)
{
    //    QMenu menu(this);

    //    menu.addAction(m_pReloadAct);

    //    menu.exec(event->globalPos());
}

bool BCWebView::event(QEvent* evt)
{
    if(m_bEmitInputSignal)
    {
        if (evt->type() == QEvent::ChildPolished)
        {
            QChildEvent *pChild_ev = static_cast<QChildEvent*>(evt);
            QObject *pChildObj = pChild_ev->child();
            if (pChildObj)
            {
                pChildObj->installEventFilter(this);
            }
        }
    }

    return QWebEngineView::event(evt);
}

bool BCWebView::eventFilter(QObject *obj, QEvent *ev)
{
    switch(ev->type())
    {
    case QEvent::MouseButtonPress:
    {
        QMouseEvent *pMouseEvent = static_cast<QMouseEvent*>(ev);

        emit sigMousePress(pMouseEvent->button(), pMouseEvent->pos());
    }
        break;
    case QEvent::MouseButtonDblClick:
    {
        QMouseEvent *pMouseEvent = static_cast<QMouseEvent*>(ev);

        emit sigMouseDbClick(pMouseEvent->button(), pMouseEvent->pos());
    }
        break;
    case QEvent::MouseButtonRelease:
    {
        QMouseEvent *pMouseEvent = static_cast<QMouseEvent*>(ev);

        emit sigMouseRelease(pMouseEvent->button(), pMouseEvent->pos());
    }
        break;
    case QEvent::Wheel:
    {
        QWheelEvent *pWheelEvent = static_cast<QWheelEvent*>(ev);
#ifdef BCQT6
    emit sigWheel(pWheelEvent->position().toPoint(), pWheelEvent->angleDelta());
#else
    emit sigWheel(pWheelEvent->pos(), pWheelEvent->angleDelta());
#endif
    }
        break;
    case QEvent::KeyPress:
    {
        QKeyEvent *pKeyEvent = static_cast<QKeyEvent*>(ev);
//        emit sigKeyPress(pKeyEvent->key());
    }
        break;
    case QEvent::KeyRelease:
    {
        QKeyEvent *pKeyEvent = static_cast<QKeyEvent*>(ev);

//        emit sigKeyRelease(pKeyEvent->key());
    }
        break;
    default:
        break;
    }

    return QWebEngineView::eventFilter(obj, ev);
}

void BCWebView::slotLoadStarted()
{
    qDebug()<<"slotLoadStarted";
    if(m_pWebChannelObject && (m_pWebChannel == NULL))
    {
        m_pWebChannel = new QWebChannel(this);
        m_pWebChannel->registerObject(m_WebChannelName, m_pWebChannelObject);
        m_pWebPage->setWebChannel(m_pWebChannel);
    }

    if(m_pLoadLabel)
    {
//        m_pLoadLabel->raise();
//        m_pLoadLabel->show();
//        m_pLoadMovie->start();
    }
}

void BCWebView::slotLoadProgress(int value)
{

}

void BCWebView::slotLoadFinished(bool bOK)
{
    qDebug()<<"slotLoadFinished";
    if(m_pLoadLabel)
    {
        m_pLoadMovie->stop();
        m_pLoadLabel->hide();
        qDebug()<<"slotLoadFinished__stop";
    }
    if(bOK && m_bRunJs)
    {
        m_pWebPage->runJavaScript(m_strJS);
        m_bRunJs    = false;
    }

    if(bOK && m_bService)
    {
        m_pWebPage->runJavaScript(QString("SetService('%1')").arg(m_strServiceUrl));

        m_bService  = false;
    }

//    clearCookie();

    m_bLoad = bOK;
}

void BCWebView::slotSelectionText()
{
    findText("");
}

void BCWebView::slotPermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature)
{
    m_pWebPage->setFeaturePermission(securityOrigin,feature, QWebEnginePage::PermissionGrantedByUser);
}

void BCWebView::slotReload()
{
    ReloadPage();
}
