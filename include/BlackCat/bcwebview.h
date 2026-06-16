#ifndef BCWEBVIEW_H
#define BCWEBVIEW_H

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebChannel>
#include <QWheelEvent>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QMovie>
#include <QShowEvent>
#include "blackcat_global.h"

class BLACKCATSHARED_EXPORT BCWebView : public QWebEngineView
{
    Q_OBJECT
public:
    BCWebView(QWidget *parent = 0, QString strLoadMovie = "", QSize loadMovieSize = QSize(256, 256));
    virtual ~BCWebView();

    void SetWebChannelObject(QObject *pObj, QString name);
    void DeregisterWebChannel();
    void RunJavaScript(QString js);
    void SaveWebtoPDF(QString path);
    void SetUrl(QString url);
    void SetServiceUrl(QString url);
    bool DelDir(QString strDir);
    void clearCookie();
	void ReloadPage();
	QWebEnginePage* getPage() {
		return m_pWebPage;
	}

protected:
    void wheelEvent(QWheelEvent * pEvent);
    void contextMenuEvent(QContextMenuEvent *event);
    void showEvent(QShowEvent *pEvent);
    void changeEvent(QEvent *event);

    bool event(QEvent* evt);
    bool eventFilter(QObject *obj, QEvent *ev);
private:
    QWebEnginePage *m_pWebPage;
    QWebChannel *m_pWebChannel;
    QObject *m_pWebChannelObject;
    QString m_WebChannelName;
    int m_Index;
    QAction *m_pReloadAct;
    QMovie *m_pLoadMovie;
    QLabel *m_pLoadLabel;
    bool m_bTransparentValue;
    bool m_bIsneedLoading;

    bool m_bRunJs;
    QString m_strJS;

    bool m_bService;
    QString m_strServiceUrl;

    bool m_bLoad;
    bool m_bEmitInputSignal;
signals:
    void sigMousePress(int nKey, QPoint point);
    void sigMouseDbClick(int nKey, QPoint point);
    void sigMouseRelease(int nKey, QPoint point);
    void sigWheel(QPoint point, QPoint angleDelta);

    void sigKeyPress(int nKey);
    void sigKeyRelease(int nKey);

public slots:
    void slotReload();
    void slotLoadStarted();
    void slotLoadProgress(int value);
    void slotLoadFinished(bool bOK);
    void slotSelectionText();
    //void slotpdfPrintingFinished(QString& path, bool isSuccess);
    void slotProxyAuthenticationRequired(const QUrl & requestUrl, QAuthenticator * authenticator, const QString & proxyHost);
    void slotPermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature);

};

#endif //BCWEBVIEW_H
