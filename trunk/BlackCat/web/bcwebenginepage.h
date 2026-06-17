#ifndef BCWEBENGINEPAGE_H
#define BCWEBENGINEPAGE_H

#include <QWebEnginePage>

#ifdef BCQT6
class BCWebEnginePage : public QWebEnginePage
{
public:
    BCWebEnginePage(QObject *pParent = NULL);
    BCWebEnginePage(QWebEngineProfile *profile, QObject *parent = nullptr);

public Q_SLOT:
             void slotOnFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature);
    bool slotCertificateError(const QWebEngineCertificateError &certificateError);
};
#else
class BCWebEnginePage : public QWebEnginePage
{
public:
    BCWebEnginePage(QObject *pParent = NULL,bool bSsl = true);

    virtual bool certificateError(const QWebEngineCertificateError &certificateError);

private:
    bool m_bSSl;

public Q_SLOT:
             void slotOnFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature);
};
#endif




#endif // BCWEBENGINEPAGE_H
