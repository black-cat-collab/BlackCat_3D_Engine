#ifndef __GLWIDGET_H__
#define __GLWIDGET_H__

#include "corecommon.h"
#include <QOpenGLWidget>
#include <QtWidgets>
#include <QOpenGLFunctions>
#include <QColor>
#include <QUdpSocket>
#include <QTcpServer>
#include <QThread>
#include "blackcat_global.h"
#include "ipluginmanager.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>

#ifdef __GNUC__
#define		sprintf_s	snprintf
namespace bc
{
class IMonitorManager;
}
#elif WIN32
class bc::IMonitorManager;
#endif

class BLACKCATSHARED_EXPORT GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit GLWidget(IPluginManager *pPluginManager, QWidget *parent = NULL);
    virtual ~GLWidget();

    virtual void                SetLoad(bool bLoad);
    virtual void                SetOpen(bool bOpen);
    virtual bool                IsOpen() { return m_bOpen; }
    virtual bool                IsLoad() { return m_bLoad; }

    virtual void                MouseDoubleClick(const int& button, const QPoint& pos);
    virtual void                MousePress(const int& button, const QPoint& pos);
    virtual void                MouseRelease(const int& button, const QPoint& pos);
    virtual void                MouseMove(const int&, const QPoint& pos);
    virtual void                Wheel(const QPoint& pos, const QPoint& angleDelta);
    virtual void                KeyPress(const int& QtKey);
    virtual void                KeyRelease(const int& QtKey);

    virtual void                ResizeGLWidget(int nWidth,int nHeight);

    virtual void                watchDog();
protected:
    virtual void                init();
    virtual void                initFormat();
    virtual void                frameUpdate();
    virtual void                frameUpdateEngine();
    virtual void                encodeFrame();
    virtual bool                startServer();
    virtual void                initializeGL();
    virtual void                paintGL();
    virtual void                resizeGL(int width, int height);
    virtual void                resizeEvent(QResizeEvent *e);
    virtual void                contextMenuEvent(QContextMenuEvent *event);
    virtual void                mouseDoubleClickEvent(QMouseEvent *pEvent);
    virtual void                mousePressEvent(QMouseEvent *event);
    virtual void                mouseMoveEvent(QMouseEvent *pEvent);
    virtual void                mouseReleaseEvent(QMouseEvent *pEvent);
    virtual void                keyPressEvent(QKeyEvent *);
    virtual void                keyReleaseEvent(QKeyEvent *pEvent);
    virtual void                wheelEvent(QWheelEvent * pEvent) ;
    virtual void                timerEvent(QTimerEvent *event);
    virtual void                changeEvent(QEvent *event);
    virtual void                showEvent(QShowEvent * event);

    virtual void                createLoadTexture();
    virtual void                createSleepTexture();




private:
    static void static_bcclouder_error_callback();
    static void static_Cloud_log_callback(const std::string& strMsg);
    static void static_Cloud_handle_func(const std::string& strCode,QList<QJsonValue> list);

    qreal getScreenRatio();     //获取屏幕的缩放

protected:
    IPluginManager*             m_pPluginManager;
    bc::IMonitorManager*        m_pMonitorManager;
    long                        m_lHeartBeatCounter;
    QMap<int, char>             m_mKeyMap; //键盘映射表
    bool                        m_bLoad;
    bool                        m_bOpen;
    int                         m_nSamples;
    int                         m_nSwapInterval;
    int                         m_nUpdateFPS;

    int                         m_nTimerID;
    bool                        m_bSendEncodeData;
    bool                        m_bEncodeRun;
    bool                        m_bHardEncode;


    QSize                       m_FrameBufferSize;
    QOpenGLShaderProgram        m_ShaderProgram;
    QOpenGLVertexArrayObject    m_VAO;
    QOpenGLBuffer               m_VBO;
    QOpenGLFramebufferObject*   m_pFBO;
    QOpenGLTexture*             m_pLoadTexture;
    QOpenGLTexture*             m_pSleepTexture;

    QTimer*                     m_pCloudeRenderTimerEvent;
    QTimer*                     m_pReStartCloudTime;
    static GLWidget*            s_pThis;

Q_SIGNALS:
    void sigReStartCloud();
    void sigReEncodeVideo();

public slots:
    void slotReStartCloud();
    void slotCloudTimeout();
    void slotReEncodeVideo();
};

#endif   //__GLWIDGET_H__
