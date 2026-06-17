#include "bcwebenginepage.h"
#include <QDebug>

#ifdef BCQT6
BCWebEnginePage::BCWebEnginePage(QObject *pParent):QWebEnginePage (pParent)
{
    connect(this, &BCWebEnginePage::featurePermissionRequested, this, &BCWebEnginePage::slotOnFeaturePermissionRequested);
    connect(this, &BCWebEnginePage::certificateError, this, &BCWebEnginePage::slotCertificateError);
}

BCWebEnginePage::BCWebEnginePage(QWebEngineProfile *profile, QObject *parent):
    QWebEnginePage (profile,parent)
{
    connect(this, &BCWebEnginePage::featurePermissionRequested, this, &BCWebEnginePage::slotOnFeaturePermissionRequested);
    connect(this, &BCWebEnginePage::certificateError, this, &BCWebEnginePage::slotCertificateError);
}

bool BCWebEnginePage::slotCertificateError(const QWebEngineCertificateError &certificateError)
{
    return true;
}

void BCWebEnginePage::slotOnFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature)
{
    //    if(feature == QWebEnginePage::MediaAudioCapture
    //                    || feature == QWebEnginePage::MediaVideoCapture
    //                    || feature == QWebEnginePage::MediaAudioVideoCapture)
    //    {
    //        setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionGrantedByUser);
    //    }
    //    else
    //    {
    //        setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionDeniedByUser);
    //    }

    setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionGrantedByUser);
}
#else
BCWebEnginePage::BCWebEnginePage(QObject *pParent,bool bSsl):QWebEnginePage (pParent)
{
    m_bSSl = bSsl;
    connect(this, &BCWebEnginePage::featurePermissionRequested, this, &BCWebEnginePage::slotOnFeaturePermissionRequested);
}

bool BCWebEnginePage::certificateError(const QWebEngineCertificateError &certificateError)
{
    if(m_bSSl)
    {
        return true;
    }
    else
    {
        return QWebEnginePage::certificateError(certificateError);
    }
}

void BCWebEnginePage::slotOnFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature)
{
    //    if(feature == QWebEnginePage::MediaAudioCapture
    //                    || feature == QWebEnginePage::MediaVideoCapture
    //                    || feature == QWebEnginePage::MediaAudioVideoCapture)
    //    {
    //        setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionGrantedByUser);
    //    }
    //    else
    //    {
    //        setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionDeniedByUser);
    //    }

    setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionGrantedByUser);
}
#endif
