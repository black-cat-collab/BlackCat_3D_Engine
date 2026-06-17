
//#include "EncodeAPI.h"
#include "BlackCat/glwidget.h"

//#include "HCClouderender.h"
#include <QOpenGLWidget>
#include "../proxy/servermanager.h"
#include "BlackCat/imainframeplugin.h"
#include <QOpenGLFramebufferObject>
#include <iomanip>

using namespace bc;


//typedef struct SoftEncodeUser_s
//{
//    IVideoEncodeListen* pVideoEncodeListen;
//    int                 nWidth;
//    int                 nHeight;
//}SoftEncodeUser;



const GLchar* vertexShaderSource = "#version 330 core\n"
                                   "layout (location = 0) in vec2 position;\n"
                                   "layout (location = 1) in vec2 texCoords;\n"
                                   "out vec2 TexCoords;\n"
                                   "void main()\n"
                                   "{\n"
                                   "gl_Position = vec4(position.x, position.y, 0.0, 1.0);\n"
                                   "TexCoords = texCoords;\n"
                                   "}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
                                     "out vec4 color;\n"
                                     "in vec2 TexCoords;\n"
                                     "uniform sampler2D screenTexture;\n"
                                     "void main()\n"
                                     "{\n"
                                     "color = texture(screenTexture, TexCoords);\n"
                                     "}\n\0";

float screenVertices[]{
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f,  -1.0f,  0.0f, 0.0f,
    1.0f,  -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f,  -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
};


GLWidget* GLWidget::s_pThis = nullptr;
GLWidget::GLWidget(IPluginManager *pPluginManager, QWidget *parent) :
    QOpenGLWidget(parent),
    m_pPluginManager(pPluginManager),
    m_pFBO(nullptr),
    m_lHeartBeatCounter(0),
    m_bOpen(false),
    m_bLoad(false),
    m_nSamples(0),
    m_nTimerID(0),
    m_nUpdateFPS(0),
    m_nSwapInterval(0),
    m_bEncodeRun(false),
    m_bSendEncodeData(false),
    m_pLoadTexture(nullptr),
    m_pSleepTexture(nullptr),
    m_pCloudeRenderTimerEvent(nullptr),
    m_pReStartCloudTime(nullptr)
{
    s_pThis = this;
    init();
}

GLWidget::~GLWidget()
{
    ISystemAPI* pSystemAPI = m_pPluginManager->iGetSystemAPI();
    if(m_pCloudeRenderTimerEvent)
    {
        m_pCloudeRenderTimerEvent->stop();
    }
    if(pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
    {
#ifdef USE_NVIDIA_HARD_ENCODE
        HCEncode_EncodeEnd();
#endif

    }

    //HCCloudeRender::Destroy();
    qDebug()<<"delete GLWidget";
}

void GLWidget::SetOpen(bool bOpen)
{
    if(!m_bOpen)
    {
        if(m_pPluginManager->iGetSystemAPI()->iProtocolAPI->iGetServerVersion() >= SERVER_VERSION_V10)
        {
            ServerManager::GetInstance()->SetSystemAPI(m_pPluginManager->iGetSystemAPI());
            IProtocolAPI* pProtocolAPI = ServerManager::GetInstance()->GetSystemAPI()->iProtocolAPI;
            if(pProtocolAPI->iGetCloudRenderManager()->iGetRenderHost() !=STATUS_SUCCESS)
            {
                ServerManager::GetInstance()->GetMainParam()->nWebPort= ServerManager::GetInstance()->GetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nQtCmdPort;
            }
            QString strMsg = QString("websocket port=") + QString::number(ServerManager::GetInstance()->GetMainParam()->nWebPort);
            if(m_pPluginManager->iGetSystemAPI()->iProjectClientAPI)
            {
                m_pPluginManager->iGetSystemAPI()->iProjectClientAPI->iSaveLog(LOG_QT,strMsg.toLocal8Bit().toStdString());
            }

            if(m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bDataSubscribeServer)
            {
                ServerManager::GetInstance()->CreateSubscribeWebsocketServer();
            }
        }
        else if(m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
        {
            ServerManager::GetInstance()->SetSystemAPI(m_pPluginManager->iGetSystemAPI());
            IProtocolAPI* pProtocolAPI = ServerManager::GetInstance()->GetSystemAPI()->iProtocolAPI;
            if(pProtocolAPI->iGetCloudRenderManager()->iGetRenderHost() !=STATUS_SUCCESS)
            {
                ServerManager::GetInstance()->GetMainParam()->nWebPort= DEFAULT_REMOTERENDER_PORT;
            }
        }

        m_lHeartBeatCounter = 0;
        m_bHardEncode = m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bHardEncode;
        if(m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
        {
            if(m_nTimerID != 0)
            {
                killTimer(m_nTimerID);
                m_nTimerID = 0;
            }

            float fInterval = 1000.0f / m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nBackgroundLoadFPS + 0.5;

            m_nTimerID = startTimer((int)fInterval, Qt::PreciseTimer);
        }
    }
        m_bOpen = bOpen;
}

void GLWidget::SetLoad(bool bLoad)
{
    m_bLoad = bLoad;

    if(m_bLoad)
    {
        m_lHeartBeatCounter = 0;
        if(m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bFixedFPS ||
                m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
        {
            if(m_nTimerID != 0)
            {
                killTimer(m_nTimerID);
                m_nTimerID = 0;
            }

            float fInterval = 1000.0f / m_nUpdateFPS + 0.5f;

            m_nTimerID = startTimer((int)fInterval, Qt::PreciseTimer);
        }

        if(!m_bSendEncodeData && m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
        {
            m_bSendEncodeData = startServer();
        }
    }
}

void GLWidget::init()
{  
    initFormat();

    m_mKeyMap.insert(Qt::Key_0, 0x30);
    m_mKeyMap.insert(Qt::Key_ParenRight, 0x30);
    m_mKeyMap.insert(Qt::Key_1, 0x31);
    m_mKeyMap.insert(Qt::Key_Exclam, 0x31);
    m_mKeyMap.insert(Qt::Key_2, 0x32);
    m_mKeyMap.insert(Qt::Key_At, 0x32);
    m_mKeyMap.insert(Qt::Key_3, 0x33);
    m_mKeyMap.insert(Qt::Key_NumberSign, 0x33);
    m_mKeyMap.insert(Qt::Key_4, 0x34);
    m_mKeyMap.insert(Qt::Key_Dollar, 0x34);
    m_mKeyMap.insert(Qt::Key_5, 0x35);
    m_mKeyMap.insert(Qt::Key_Percent, 0x35);
    m_mKeyMap.insert(Qt::Key_6, 0x36);
    m_mKeyMap.insert(Qt::Key_AsciiCircum, 0x36);
    m_mKeyMap.insert(Qt::Key_7, 0x37);
    m_mKeyMap.insert(Qt::Key_Ampersand, 0x37);
    m_mKeyMap.insert(Qt::Key_8, 0x38);
    m_mKeyMap.insert(Qt::Key_Asterisk, 0x38);
    m_mKeyMap.insert(Qt::Key_9, 0x39);
    m_mKeyMap.insert(Qt::Key_ParenLeft, 0x39);
    m_mKeyMap.insert(Qt::Key_A, 0x41);
    m_mKeyMap.insert(Qt::Key_B, 0x42);
    m_mKeyMap.insert(Qt::Key_C, 0x43);
    m_mKeyMap.insert(Qt::Key_D, 0x44);
    m_mKeyMap.insert(Qt::Key_E, 0x45);
    m_mKeyMap.insert(Qt::Key_F, 0x46);
    m_mKeyMap.insert(Qt::Key_G, 0x47);
    m_mKeyMap.insert(Qt::Key_H, 0x48);
    m_mKeyMap.insert(Qt::Key_I, 0x49);
    m_mKeyMap.insert(Qt::Key_J, 0x4A);
    m_mKeyMap.insert(Qt::Key_K, 0x4B);
    m_mKeyMap.insert(Qt::Key_L, 0x4C);
    m_mKeyMap.insert(Qt::Key_M, 0x4D);
    m_mKeyMap.insert(Qt::Key_N, 0x4E);
    m_mKeyMap.insert(Qt::Key_O, 0x4F);
    m_mKeyMap.insert(Qt::Key_P, 0x50);
    m_mKeyMap.insert(Qt::Key_Q, 0x51);
    m_mKeyMap.insert(Qt::Key_R, 0x52);
    m_mKeyMap.insert(Qt::Key_S, 0x53);
    m_mKeyMap.insert(Qt::Key_T, 0x54);
    m_mKeyMap.insert(Qt::Key_U, 0x55);
    m_mKeyMap.insert(Qt::Key_V, 0x56);
    m_mKeyMap.insert(Qt::Key_W, 0x57);
    m_mKeyMap.insert(Qt::Key_X, 0x58);
    m_mKeyMap.insert(Qt::Key_Y, 0x59);
    m_mKeyMap.insert(Qt::Key_Z, 0x5A);
    m_mKeyMap.insert(Qt::Key_Equal, '=');
    //m_mKeyMap.insert(Qt::Key_Plus, '+');
    m_mKeyMap.insert(Qt::Key_Minus, '-');

#ifdef _WIN32
#if BUILD_PLAT == BUILD_PLAT_WIN32
    m_mKeyMap.insert(Qt::Key_Left,VK_LEFT);
    m_mKeyMap.insert(Qt::Key_Up, VK_UP);
    m_mKeyMap.insert(Qt::Key_Right, VK_RIGHT);
    m_mKeyMap.insert(Qt::Key_Down, VK_DOWN);
    m_mKeyMap.insert(Qt::Key_Backspace, VK_BACK);
    m_mKeyMap.insert(Qt::Key_Tab, VK_TAB);
    m_mKeyMap.insert(Qt::Key_Clear, VK_CLEAR);
    m_mKeyMap.insert(Qt::Key_Return, VK_RETURN);
    m_mKeyMap.insert(Qt::Key_Shift, VK_SHIFT);
    m_mKeyMap.insert(Qt::Key_Control, VK_CONTROL);
    m_mKeyMap.insert(Qt::Key_Alt, VK_MENU);
    m_mKeyMap.insert(Qt::Key_Pause, VK_PAUSE);
    m_mKeyMap.insert(Qt::Key_CapsLock, VK_CAPITAL);
    m_mKeyMap.insert(Qt::Key_Escape, VK_ESCAPE);
    m_mKeyMap.insert(Qt::Key_Space, VK_SPACE);
    m_mKeyMap.insert(Qt::Key_PageUp, VK_PRIOR);
    m_mKeyMap.insert(Qt::Key_PageDown, VK_NEXT);
    m_mKeyMap.insert(Qt::Key_End, VK_END);
    m_mKeyMap.insert(Qt::Key_Home, VK_HOME);
    m_mKeyMap.insert(Qt::Key_Select, VK_SELECT);
    m_mKeyMap.insert(Qt::Key_Print, VK_PRINT);
    m_mKeyMap.insert(Qt::Key_Execute, VK_EXECUTE);
    m_mKeyMap.insert(Qt::Key_Printer, VK_SNAPSHOT);
    m_mKeyMap.insert(Qt::Key_Insert, VK_INSERT);
    m_mKeyMap.insert(Qt::Key_Delete, VK_DELETE);
    m_mKeyMap.insert(Qt::Key_Help, VK_HELP);
    m_mKeyMap.insert(Qt::Key_multiply, VK_MULTIPLY);
    m_mKeyMap.insert(Qt::Key_Enter, VK_SEPARATOR);
    m_mKeyMap.insert(Qt::Key_F1, VK_F1);
    m_mKeyMap.insert(Qt::Key_F2, VK_F2);
    m_mKeyMap.insert(Qt::Key_F3, VK_F3);
    m_mKeyMap.insert(Qt::Key_F4, VK_F4);
    m_mKeyMap.insert(Qt::Key_F5, VK_F5);
    m_mKeyMap.insert(Qt::Key_F6, VK_F6);
    m_mKeyMap.insert(Qt::Key_F7, VK_F7);
    m_mKeyMap.insert(Qt::Key_F8, VK_F8);
    m_mKeyMap.insert(Qt::Key_F9, VK_F9);
    m_mKeyMap.insert(Qt::Key_F10, VK_F10);
    m_mKeyMap.insert(Qt::Key_F11, VK_F11);
    m_mKeyMap.insert(Qt::Key_F12, VK_F12);
    m_mKeyMap.insert(Qt::Key_F13, VK_F13);
    m_mKeyMap.insert(Qt::Key_F14, VK_F14);
    m_mKeyMap.insert(Qt::Key_F15, VK_F15);
    m_mKeyMap.insert(Qt::Key_F16, VK_F16);
#endif
#endif

    ServerManager::GetInstance()->SetGLWidget(this);
}

bool GLWidget::startServer()
{
    IProtocolAPI* pProtocolAPI = ServerManager::GetInstance()->GetSystemAPI()->iProtocolAPI;

    //char* host= m_pSystemAPI->iProtocol->GetStringValue(STR_VALUE_WEBRTC_HOST);
    //int   port= m_pSystemAPI->iProtocol->GetIntValue(INT_VALUE_WEBRTC_PORT);
    //QUrl url =
    BootConfig  sBootConfig= m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig;
    QString strIP = QString::fromLocal8Bit(sBootConfig.strWebRTCServerHost.c_str());

    if(strIP.size() == 0)
    {
        return false;
    }
    QUrl url;
    if(sBootConfig.strServerIp.find("https") != std::string::npos)
    {
        url = QUrl(QString("wss://%1").arg(strIP));
    }
    else
    {
        url = QUrl(QString("ws://%1").arg(strIP));
    }


    ICloudServer *pCloudParam = ServerManager::GetInstance()->GetMainParam();
    ServerManager::GetInstance()->StartServer();

    if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI)
    {
        std::string strMsg = "webrtc ip=" + strIP.toLocal8Bit().toStdString();
        m_pPluginManager->iGetSystemAPI()->iProjectClientAPI->iSaveLog(LOG_QT,strMsg);
    }
    //HCCloudeRender::GetInstance()->SetWebRTCErrorCallback(static_hcclouder_error_callback);
    //HCCloudeRender::GetInstance()->SetLogCallback(static_Cloud_log_callback);
    //HCCloudeRender::GetInstance()->AddQtCoreHandleFunc(static_Cloud_handle_func);
    connect(this,&GLWidget::sigReStartCloud,this,&GLWidget::slotReStartCloud);
    connect(this,&GLWidget::sigReEncodeVideo,this,&GLWidget::slotReEncodeVideo);

    //HCCloudeRender::GetInstance()->StartCloudRend(url,sBootConfig.strServerIp,sBootConfig.strApplicationName,pCloudParam->nID);

    m_pMonitorManager=pProtocolAPI->iGetMonitorManager();

//    watchDog();

    return true;
}




void GLWidget::watchDog()
{
#ifdef _WIN32
        m_pCloudeRenderTimerEvent = new QTimer(this);
        connect(m_pCloudeRenderTimerEvent,&QTimer::timeout,this,[this]()
        {
            bool bClose = true;  //HCCloudeRender::GetInstance()->IsNeedReboot(30);
            QString str = "HCCloudeRender IsNeedReboot()," + QString(bClose?"true":"false");
            qDebug()<<str;
            if(m_pPluginManager->iGetSystemAPI()->iProjectClientAPI && bClose)
            {
                m_pPluginManager->iGetSystemAPI()->iProjectClientAPI->iSaveLog(LOG_QT,str.toStdString());
            }
            if(bClose)
            {
                //自杀
                qint64 pid = QCoreApplication::applicationPid();//获取当前进程的PID
                QString cmd = QString("TASKKILL /PID %1 /F").arg(pid);
                WinExec(cmd.toLocal8Bit().data(), SW_HIDE);//SW_HIDE：控制运行cmd时，不弹出cmd运行窗口
            }



        });
        m_pCloudeRenderTimerEvent->start(1000);

#endif


}








void GLWidget::initFormat()
{
    m_nUpdateFPS = m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nUpdateFPS;

    auto format = QSurfaceFormat::defaultFormat();

    if(m_nSamples > 0)
    {
        format.setSamples(m_nSamples);
    }

    format.setSwapInterval(0);
    format.setVersion(4, 4);
    QSurfaceFormat::setDefaultFormat(format);
}

void GLWidget::frameUpdate()
{
    QSize viewportSize;
    qreal ratio = getScreenRatio();
    if(m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
    {
        m_pPluginManager->iGetSystemAPI()->iCoreAPI->iGetViewManager()->iSetSceneSize(m_FrameBufferSize.width(), m_FrameBufferSize.height());
        viewportSize = QSize(m_FrameBufferSize.width(), m_FrameBufferSize.height());
    }
    else
    {
        int w = size().width() * ratio;
        int h = size().height() * ratio;
        if(m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bScaleOutputSize)
        {
            int nSceneWidth = m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nSceneWidth;
            int nSceneHeight = m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nSceneHeight;
            m_pPluginManager->iGetSystemAPI()->iCoreAPI->iGetViewManager()->iSetSceneSize(nSceneWidth, nSceneHeight);
        }
        else
        {           
            m_pPluginManager->iGetSystemAPI()->iCoreAPI->iGetViewManager()->iSetSceneSize(w, h);
        }

        m_pPluginManager->iGetSystemAPI()->iCoreAPI->iGetViewManager()->iSetOutputSize(w, h);

        viewportSize = QSize(w, h);
    }

    // 加载页面
    {
        glViewport(0, 0, viewportSize.width(), viewportSize.height());

        glClearColor(0.0, 0.0, 0.0, 0.0);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        if(m_pLoadTexture)
        {
            m_ShaderProgram.bind();

            glBindTexture(GL_TEXTURE_2D, m_pLoadTexture->textureId());

            m_ShaderProgram.setUniformValue(m_ShaderProgram.uniformLocation("screenTexture"), 0);

            QOpenGLVertexArrayObject::Binder vaoBind(&m_VAO);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindTexture(GL_TEXTURE_2D, 0);

            m_ShaderProgram.release();
        }
    }

    if(m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bAutoSleep)
    {
        if(IsLoad())
        {
            long lCounterTime = m_lHeartBeatCounter * m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetRenderInfo().fFrameTime;
            if(lCounterTime > m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.lAutoSleepWaitTime)
            {
                if(m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
                {
                    glViewport(0, 0, m_FrameBufferSize.width(), m_FrameBufferSize.height());
                }
                else
                {
                    glViewport(0, 0, size().width(), size().height());
                }

                glClearColor(0.0, 0.0, 0.0, 0.0);

                glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

                if(m_pSleepTexture)
                {
                    m_ShaderProgram.bind();

                    glBindTexture(GL_TEXTURE_2D, m_pSleepTexture->textureId());

                    m_ShaderProgram.setUniformValue(m_ShaderProgram.uniformLocation("screenTexture"), 0);

                    QOpenGLVertexArrayObject::Binder vaoBind(&m_VAO);

                    glDrawArrays(GL_TRIANGLES, 0, 6);

                    glBindTexture(GL_TEXTURE_2D, 0);

                    m_ShaderProgram.release();
                }

                return;
            }
            else
            {
                ++m_lHeartBeatCounter;
            }
        }
    }

    if(m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bFixedFPS ||
            m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
    {
        if(m_nUpdateFPS != m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nUpdateFPS)
        {
            m_nUpdateFPS = m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nUpdateFPS;

            if(m_nTimerID != 0)
            {
                killTimer(m_nTimerID);
                m_nTimerID = 0;
            }

            float fInterval = 1000.0f / m_nUpdateFPS + 0.5f;

            m_nTimerID = startTimer((int)fInterval, Qt::PreciseTimer);
        }
    }
    else
    {
        if(m_nTimerID != 0)
        {
            killTimer(m_nTimerID);
            m_nTimerID = 0;
        }
        m_nUpdateFPS = 0;
    }

    frameUpdateEngine();

    if(m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
    {
         QFrame *pFrame = ((IMainFramePlugin*)m_pPluginManager->iGetPlugin(MainFramePluginType))->GetMainFrame();
         if(pFrame->isVisible() && !m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bShowMainWindow)
         {
            pFrame->hide();
         }
    }
}

void GLWidget::frameUpdateEngine()
{
    m_pPluginManager->iMouseMove();

    ISystemAPI* pSystemAPI = m_pPluginManager->iGetSystemAPI();
    if(pSystemAPI->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
    {
        // TODO
        //        QPoint pt(mapFromGlobal(QCursor::pos()));
        //        pSystemAPI->iEngineAPI->iMouseEvent(MouseState_e::MOUSE_MOVE, pt.x(), pt.y());

        int x = ServerManager::GetInstance()->GetIntPtrValue(INT_VALUE_CURSOR_X);
        int y = ServerManager::GetInstance()->GetIntPtrValue(INT_VALUE_CURSOR_Y);
        pSystemAPI->iEngineAPI->iMouseEvent(MOUSE_MOVE, x, y );
    }
    else
    {
        QPoint pt(mapFromGlobal(QCursor::pos()));

        QPoint rPoint = pt;
        qreal ratio = getScreenRatio();
        rPoint.setX(rPoint.x() * ratio);
        rPoint.setY(rPoint.y() * ratio);

        pSystemAPI->iEngineAPI->iMouseEvent(MouseState_e::MOUSE_MOVE, rPoint.x(), rPoint.y());
    }

    pSystemAPI->iEngineAPI->iRunFrame();
}

void GLWidget::createLoadTexture()
{
    m_pLoadTexture = new QOpenGLTexture(QImage(":/resource/image/loadfile.png").mirrored(), QOpenGLTexture::GenerateMipMaps);
    m_pLoadTexture->create();
}

void GLWidget::createSleepTexture()
{
    m_pSleepTexture = new QOpenGLTexture(QImage(":/resource/image/sleep.png").mirrored(), QOpenGLTexture::GenerateMipMaps);
    m_pSleepTexture->create();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    m_ShaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_ShaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_ShaderProgram.link();

    m_VAO.create();
    m_VBO.create();
    m_VAO.bind();
    m_VBO.bind();

    glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), &screenVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    m_VAO.release();
    m_VBO.release();

    createLoadTexture();

    createSleepTexture();

    if(m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
    {
        m_FrameBufferSize = QSize(m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nOutputWidth,
                                  m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nOutputHeight);
        //m_FrameBufferSize = QSize(1920, 1080);
        m_pFBO = new QOpenGLFramebufferObject(m_FrameBufferSize, QOpenGLFramebufferObject::Depth, GL_TEXTURE_2D, GL_RGBA16F_ARB);
    }
}

void GLWidget::paintGL()
{
    if(m_bOpen)
    {
        if (!m_pPluginManager->iGetSystemAPI())
        {
            return;
        }

        if(!m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
        {
            frameUpdate();

            if(!m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bFixedFPS || !m_bLoad)
            {
                update();
            }
        }
        else
        {
            glViewport(0, 0, m_FrameBufferSize.width(), m_FrameBufferSize.height());

            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            m_ShaderProgram.bind();

            m_ShaderProgram.setUniformValue(m_ShaderProgram.uniformLocation("screenTexture"), 0);

            glBindTexture(GL_TEXTURE_2D, m_pFBO->texture());

            QOpenGLVertexArrayObject::Binder vaoBind(&m_VAO);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindTexture(GL_TEXTURE_2D, 0);

            m_ShaderProgram.release();
        }
    }

    QOpenGLWidget::paintGL();
}

void GLWidget::resizeGL(int width, int height)
{
    qreal ratio = getScreenRatio();
    width *= ratio;
    height *= ratio;

    int temp = width / 4;
    if(width % 4 > 0)
    {
        width = (temp + 1) * 4;
    }
    temp = height / 4;
    if(height % 4 > 0)
    {
        height = (temp + 1) * 4;
    }

    qDebug()<<width;
    qDebug()<<height;
    m_FrameBufferSize.setWidth(width);
    m_FrameBufferSize.setHeight(height);

    if(!m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bScaleOutputSize)
    {
        m_pPluginManager->iGetSystemAPI()->iCoreAPI->iGetViewManager()->iSetSceneSize(width, height);
    }

    m_pPluginManager->iGetSystemAPI()->iCoreAPI->iGetViewManager()->iSetOutputSize(width, height);

    if(m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
    {
        if(width != m_pFBO->width() || height != m_pFBO->height())
        {
            m_pFBO->release();
            delete m_pFBO;
            m_pFBO = new QOpenGLFramebufferObject(m_FrameBufferSize, QOpenGLFramebufferObject::Depth, GL_TEXTURE_2D, GL_RGBA16F_ARB);
        }
    }
}

void GLWidget::resizeEvent(QResizeEvent *e)
{
    QOpenGLWidget::resizeEvent(e);
}

int SendDataToWebRTC(INT_PTR wParam, INT_PTR pUser)
{
    BCDataBuffer     *pData=(BCDataBuffer*)wParam;
    //SoftEncodeUser* pUser2 = (SoftEncodeUser*)pUser;
    //pUser2->pVideoEncodeListen->IVideoEncodeCallBack((uint8_t*)pData->pBuffer, pData->nLength, pUser2->nWidth,pUser2->nHeight);

    return 0;
}

void GLWidget::encodeFrame()
{
    /*
    IVideoEncodeListen* pVideoEncodeListen = nullptr;
    pVideoEncodeListen = HCCloudeRender::GetInstance()->GetVideoEncodeListen();
    static BCSoftEncodeHandle* hSoftEncode=NULL;
    if(pVideoEncodeListen == nullptr)
    {


        return;
    }
    bool bClose = HCCloudeRender::GetInstance()->IsPeerConnectionClosed();
    if(bClose)
    {
        if(m_bEncodeRun)
        {
            m_bEncodeRun = false;

            if(m_bHardEncode)
            {
#ifdef USE_NVIDIA_HARD_ENCODE
                HCEncode_EncodeEnd();
#endif

            }
            else
            {
                m_pMonitorManager->iSoftEncodeClose(hSoftEncode);
                hSoftEncode=NULL;
            }

        }
        return;
    }

    if(m_bSendEncodeData)
    {
        if(!m_bEncodeRun)
        {
            m_bEncodeRun = true;
            if(m_bHardEncode)
            {
                int nCodecType=m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nCodecType;
                int nCodecLevel=m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nCodecLevel;

                if(nCodecType==CODEC_TYPE_H264)
                {
#ifdef USE_NVIDIA_HARD_ENCODE
                    HCEncode_Init(m_pFBO->texture(), GL_TEXTURE_2D, m_FrameBufferSize.width(), m_FrameBufferSize.height(), ENC_CODEC_H264, (eEncResset)nCodecLevel,
                               ENC_TUNING_INFO_UNDEFINED, m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nEncodeVideoGPU);
#endif

                }
                else if(nCodecType==CODEC_TYPE_H265)
                {
#ifdef USE_NVIDIA_HARD_ENCODE
                    HCEncode_Init(m_pFBO->texture(), GL_TEXTURE_2D, m_FrameBufferSize.width(), m_FrameBufferSize.height(), ENC_CODEC_HEVC, (eEncResset)nCodecLevel,
                                ENC_TUNING_INFO_ULTRA_LOW_LATENCY, m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nEncodeVideoGPU);
#endif

                }

            }
            else
            {
                hSoftEncode=m_pMonitorManager->iSoftEncodeInit(false, m_pFBO->toImage().bits(), m_FrameBufferSize.width(),m_FrameBufferSize.height(),m_FrameBufferSize.width(),m_FrameBufferSize.height());
            }

        }
        if(m_bHardEncode)
        {
#ifdef USE_NVIDIA_HARD_ENCODE
            std::vector<std::vector<uint8_t>>& vPacket = HCEncode_EncodeFrame();
            if (vPacket.size() > 0)
            {
                for (std::vector<uint8_t> &packet : vPacket)
                {
                    pVideoEncodeListen->IVideoEncodeCallBack(packet.data(), packet.size(), m_FrameBufferSize.width(), m_FrameBufferSize.height());
                }
            }
#endif

        }
        else
        {
            SoftEncodeUser tUser;
            tUser.pVideoEncodeListen=pVideoEncodeListen;
            tUser.nWidth=m_FrameBufferSize.width();
            tUser.nHeight=m_FrameBufferSize.height();

            m_pMonitorManager->iSoftEncodeRun(hSoftEncode, (uint8*)m_pFBO->toImage().bits(),(fnCallBack)SendDataToWebRTC,(INT_PTR)&tUser);
        }

    }
    else if(m_bEncodeRun)
    {
        m_bEncodeRun = false;

        if(m_bHardEncode)
        {
#ifdef USE_NVIDIA_HARD_ENCODE
            HCEncode_EncodeEnd();
#endif

        }
        else
        {
            m_pMonitorManager->iSoftEncodeClose(hSoftEncode);
            hSoftEncode=NULL;
        }

    }
    */
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent *pEvent)
{
    QPoint sPoint = pEvent->pos();
    if(!m_pPluginManager->iCheckWebCanHandleScene(sPoint))
    {
        return;
    }
    MouseDoubleClick(pEvent->button(), pEvent->pos());
    pEvent->accept();
}

void GLWidget::mousePressEvent(QMouseEvent *pEvent)
{
    QPoint sPoint = pEvent->pos();
    if(!m_pPluginManager->iCheckWebCanHandleScene(sPoint))
    {
        return;
    }
    MousePress(pEvent->button(), pEvent->pos());

    pEvent->accept();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *pEvent)
{
    MouseRelease(pEvent->button(), pEvent->pos());
    pEvent->accept();
}

void GLWidget::mouseMoveEvent(QMouseEvent *pEvent)
{
    QPoint sPoint = pEvent->pos();
    if(!m_pPluginManager->iCheckWebCanHandleScene(sPoint))
    {
        return;
    }
    MouseMove(pEvent->button(), pEvent->pos());
    pEvent->accept();
}

void GLWidget::wheelEvent(QWheelEvent * pEvent)
{
#ifdef BCQT6
    QPoint sPoint = pEvent->position().toPoint();
    if(!m_pPluginManager->iCheckWebCanHandleScene(sPoint))
    {
        return;
    }
    Wheel(pEvent->position().toPoint(), pEvent->angleDelta());
    pEvent->accept();
#else
    QPoint sPoint = pEvent->pos();
    if(!m_pPluginManager->iCheckWebCanHandleScene(sPoint))
    {
        return;
    }
    Wheel(pEvent->pos(), pEvent->angleDelta());
    pEvent->accept();
#endif
}

void GLWidget::keyPressEvent(QKeyEvent* pEvent)
{
    if(!m_pPluginManager->iGetCanKeyEvent())
    {
        return;
    }
    KeyPress(pEvent->key());
    pEvent->accept();
}

void GLWidget::keyReleaseEvent(QKeyEvent *pEvent)
{
    KeyRelease(pEvent->key());
    pEvent->accept();
}

void GLWidget::MouseDoubleClick(const int& button, const QPoint& pos)
{
    m_lHeartBeatCounter = 0;

    if(!m_bLoad)
        return;
    QPoint sPoint = pos;
    if(!m_pPluginManager->iCheckWebCanHandleScene(sPoint))
    {
        return;
    }
    if( Qt::LeftButton ==  button )
    {
        m_pPluginManager->iGetSystemAPI()->iEngineAPI->iMouseEvent(MouseState_e::MOUSE_LBUTTONDBLCLK, pos.x(), pos.y());
    }
}

void GLWidget::MousePress(const int& button, const QPoint& pos)
{
    m_lHeartBeatCounter = 0;

    if(!m_bLoad)
        return;

    QPoint sPoint = pos;
    if(!m_pPluginManager->iCheckWebCanHandleScene(sPoint))
    {
        return;
    }
    QPoint rPoint = sPoint;
    qreal ratio = getScreenRatio();
    rPoint.setX(rPoint.x() * ratio);
    rPoint.setY(rPoint.y() * ratio);

    if(Qt::LeftButton == button)
    {
        m_pPluginManager->iGetSystemAPI()->iEngineAPI->iMouseEvent(MouseState_e::MOUSE_LBUTTONDOWN, rPoint.x(), rPoint.y());
    }

    if(Qt::RightButton == button)
    {
        m_pPluginManager->iGetSystemAPI()->iEngineAPI->iMouseEvent(MouseState_e::MOUSE_RBUTTONDOWN, rPoint.x(), rPoint.y());
    }
#ifdef BCQT6
    if(Qt::MiddleButton == button)
    {
        m_pPluginManager->iGetSystemAPI()->iEngineAPI->iMouseEvent(MouseState_e::MOUSE_MBUTTONDOWN, rPoint.x(), rPoint.y());
    }
#else
    if(Qt::MidButton == button)
    {
        m_pPluginManager->iGetSystemAPI()->iEngineAPI->iMouseEvent(MouseState_e::MOUSE_MBUTTONDOWN, rPoint.x(), rPoint.y());
    }
#endif
}

void GLWidget::MouseRelease(const int& button, const QPoint& pos)
{
    m_lHeartBeatCounter = 0;

    if(!m_bLoad)
        return;

    QPoint rPoint = pos;
    qreal ratio = getScreenRatio();
    rPoint.setX(rPoint.x() * ratio);
    rPoint.setY(rPoint.y() * ratio);

    if(Qt::LeftButton == button)
    {
        m_pPluginManager->iGetSystemAPI()->iEngineAPI->iMouseEvent(MouseState_e::MOUSE_LBUTTONUP, rPoint.x(), rPoint.y());
    }
    else if (Qt::RightButton == button)
    {
        m_pPluginManager->iGetSystemAPI()->iEngineAPI->iMouseEvent(MouseState_e::MOUSE_RBUTTONUP,  rPoint.x(), rPoint.y());
    }
#ifdef BCQT6
    if(Qt::MiddleButton == button)
    {
        m_pPluginManager->iGetSystemAPI()->iEngineAPI->iMouseEvent(MouseState_e::MOUSE_MBUTTONUP, rPoint.x(), rPoint.y() );
    }
#else
    if(Qt::MidButton == button)
    {
        m_pPluginManager->iGetSystemAPI()->iEngineAPI->iMouseEvent(MouseState_e::MOUSE_MBUTTONUP, rPoint.x(), rPoint.y() );
    }
#endif

}

void GLWidget::MouseMove(const int&, const QPoint& pos)
{
    m_lHeartBeatCounter = 0;

    if(!m_bLoad)
        return;
}

void GLWidget::Wheel(const QPoint& pos, const QPoint& angleDelta)
{
    m_lHeartBeatCounter = 0;

    if(!m_bLoad)
        return;

    QPoint sPoint = pos;
    if(!m_pPluginManager->iCheckWebCanHandleScene(sPoint))
    {
        return;
    }
    if (angleDelta.isNull() || angleDelta.y() == 0)
    {
        return;
    }

    QPoint rPoint = pos;
    qreal ratio = getScreenRatio();
    rPoint.setX(rPoint.x() * ratio);
    rPoint.setY(rPoint.y() * ratio);

    m_pPluginManager->iGetSystemAPI()->iEngineAPI->iMouseEvent(MouseState_e::MOUSE_WHEEL, rPoint.x()*(angleDelta.y()/qAbs(angleDelta.y())), rPoint.y());
}

void GLWidget::KeyPress(const int& QtKey)
{
    m_lHeartBeatCounter = 0;

    if(!m_bLoad)
        return;


    if(!m_pPluginManager->iGetCanKeyEvent())
    {
        return;
    }
    char vkMask = m_mKeyMap.value(QtKey);

    if(QGuiApplication::keyboardModifiers() == Qt::NoModifier)
    {
        switch(QtKey)
        {
        case Qt::Key_Delete:

            break;
        case Qt::Key_Z:
        {
        }
            break;
        case Qt::Key_F3:
        {

        }
            break;
        case Qt::Key_F4:
        {

        }
            break;
        case Qt::Key_F5:
        {
            qDebug()<<"GlWidget F5";
            ICommand _tCommand(COMMAND_RELOAD_WEB);
            m_pPluginManager->iSendCommand(nullptr, &_tCommand);
        }
            break;
        case Qt::Key_F11:
        {
            //全屏切换
            ICommand tCommand(COMMAND_APP_SHOW_STATUSBAR);
            m_pPluginManager->iSendCommand(nullptr, &tCommand);
        }
            break;
        case Qt::Key_F10:
        {
            m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bShowFrameTime = !m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bShowFrameTime;
        }
            break;
        default:
            m_pPluginManager->iGetSystemAPI()->iEngineAPI->iKeyEvent(vkMask, true);
            break;
        }
    }
    else if(QGuiApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        switch(QtKey)
        {
        case Qt::Key_Y:

            break;
        case Qt::Key_Z:

            break;
        case Qt::Key_N:
        {
            bc::INode* pCameraNode = m_pPluginManager->iGetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
            if(pCameraNode)
            {
                QClipboard *pBoard = QApplication::clipboard();
                Vector3d vPos = pCameraNode->iGetOrigin();
                Vector3d vAngle = pCameraNode->iGetAngles();
                std::string str = "";
                {
                    std::stringstream stream;
                    stream << std::fixed << std::setprecision(6) << vPos.x;
                    std::string result = stream.str();
                    str += result;
                    str += ",";
                }
                {
                    std::stringstream stream;
                    stream << std::fixed << std::setprecision(6) << vPos.y;
                    std::string result = stream.str();
                    str += result;
                    str += ",";
                }
                {
                    std::stringstream stream;
                    stream << std::fixed << std::setprecision(6) << vPos.z;
                    std::string result = stream.str();
                    str += result;
                    str += ",";
                }
                {
                    std::stringstream stream;
                    stream << std::fixed << std::setprecision(6) << vAngle.x;
                    std::string result = stream.str();
                    str += result;
                    str += ",";
                }
                {
                    std::stringstream stream;
                    stream << std::fixed << std::setprecision(6) << vAngle.y;
                    std::string result = stream.str();
                    str += result;
                    str += ",";
                }
                {
                    std::stringstream stream;
                    stream << std::fixed << std::setprecision(6) << vAngle.z;
                    std::string result = stream.str();
                    str += result;
                }
                pBoard->setText(str.c_str());
            }
        }
            break;
        case Qt::Key_M:
        {
            Vector3d vPos;
            bc::INode* pCameraNode = m_pPluginManager->iGetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
            bc::ICamera* pCamera = static_cast<bc::ICamera*>(pCameraNode->iGetComponent(COMPONENT_TYPE_CAMERA));
            IManipulator* pManipulator = static_cast<IManipulator*>(pCameraNode->iGetComponent(COMPONENT_TYPE_MANIPULATOR));

            Vector3 vDc;
            vPos = pManipulator->iGetMousePoint();
            //            IntersectSet vSet;
            //            pCamera->iIntersectObject(vDc.x,vDc.y,vSet);
            //            if(vSet.pMesh)
            //            {
            //                vPos = vSet.vCrossPoint;
            //            }
            std::string str = "";
            {
                std::stringstream stream;
                stream << std::fixed << std::setprecision(6) << vPos.x;
                std::string result = stream.str();
                str += result;
                str += ",";
            }
            {
                std::stringstream stream;
                stream << std::fixed << std::setprecision(6) << vPos.y;
                std::string result = stream.str();
                str += result;
                str += ",";
            }
            {
                std::stringstream stream;
                stream << std::fixed << std::setprecision(6) << vPos.z;
                std::string result = stream.str();
                str += result;
            }
            if(pCamera)
            {
                QClipboard *pBoard = QApplication::clipboard();
                pBoard->setText(str.c_str());
            }
        }
            break;
        case Qt::Key_T:
        {

        }
            break;
        default:
            m_pPluginManager->iGetSystemAPI()->iEngineAPI->iKeyEvent(vkMask, true);
            break;
        }
    }
}

void GLWidget::KeyRelease(const int& QtKey)
{
    m_lHeartBeatCounter = 0;

    if(!m_bLoad)
        return;

    char vkMask  = m_mKeyMap.value(QtKey);

    m_pPluginManager->iGetSystemAPI()->iEngineAPI->iKeyEvent(vkMask, false);
}

void GLWidget::contextMenuEvent(QContextMenuEvent *event)
{

}

void GLWidget::timerEvent(QTimerEvent *event)
{

    if(m_pPluginManager->iGetSystemAPI() && m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
    {
        makeCurrent();

        m_pFBO->bind();

        frameUpdate();

        if(m_bSendEncodeData)
        {
            encodeFrame();
        }

        m_pFBO->release();

        doneCurrent();
    }

    update();
}

void GLWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        if (!isMinimized())
        {
            setAttribute(Qt::WA_Mapped);
        }
    }
    QOpenGLWidget::changeEvent(event);
}

void GLWidget::showEvent(QShowEvent * event)
{
    this->setAttribute(Qt::WA_Mapped);
    QOpenGLWidget::showEvent(event);
}

void GLWidget::ResizeGLWidget(int nWidth,int nHeight)
{
    if(nWidth < 200 || nHeight < 200)
    {
        //目前低于100英伟达编码失败
        return;
    }
    if(m_bEncodeRun)
    {

#ifdef USE_NVIDIA_HARD_ENCODE
        HCEncode_StopEncode();
        HCEncode_EncodeEnd();
#endif

    }
    m_bEncodeRun = false;

    //告知网页真实宽高
    int nScaleWidth = nWidth;
    int nScaleHeight = nHeight;
    //最大支持3840*2160
//    int nMaxWidth = 3840;
//    int nMaxHeight = 2160;
    int nMaxWidth = m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nSceneWidth;
    int nMaxHeight = m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.nSceneHeight;
    if(nMaxWidth <= 0)
    {
        nMaxWidth = 3840;
    }
    if(nMaxHeight <= 0)
    {
        nMaxHeight = 2160;
    }
    float f1 = (1.0f * nWidth) / nHeight;
    float fBase = (1.0f * nMaxWidth)/nMaxHeight;
    if(f1 > fBase)
    {
        // 宽度过大， 以宽度为基准
        if(nWidth > nMaxWidth)
        {
            nScaleWidth = nMaxWidth;
            nScaleHeight = (int)(nScaleWidth / f1);
        }
    }
    else
    {
        // 高度过大， 以高度为基准
        if(nHeight > nMaxHeight)
        {
            nScaleHeight = nMaxHeight;
            nScaleWidth = (int)(nScaleHeight * f1);
        }
    }

    char szBuffer[LENGTH_1K] = { 0 };
    sprintf_s(szBuffer, sizeof(szBuffer), "VS_AppWindowSize(%d,%d)",nScaleWidth,nScaleHeight);
    bc::BCEvent _tEvent(EVENT_RUN_WEB_JS, 0, 0, 0);
    _tEvent.strWebJsParam = std::string(szBuffer);
    m_pPluginManager->iGetSystemAPI()->iEngineAPI->iExecuteEvent(_tEvent);

    QFrame *pFrame = ((IMainFramePlugin*)m_pPluginManager->iGetPlugin(MainFramePluginType))->GetMainFrame();
    pFrame->resize(QSize(nScaleWidth,nScaleHeight));
    pFrame->move(0, 0);
    pFrame->show();
}

void GLWidget::static_bcclouder_error_callback()
{
    emit s_pThis->sigReStartCloud();
}

void GLWidget::slotReStartCloud()
{
    //HCCloudeRender::GetInstance()->ReStartCloudRend();
    {
        if(m_pReStartCloudTime)
        {
            m_pReStartCloudTime->stop();
            m_pReStartCloudTime->deleteLater();
        }
        m_pReStartCloudTime = new QTimer();
        m_pReStartCloudTime->singleShot(1000,this,&GLWidget::slotCloudTimeout);
    }
}

void GLWidget::slotCloudTimeout()
{
    //发送给网页重新连接云渲染
    std::string str = "";
    str += "VS_ToCommonWebCommandV2(";
    str += "'" + std::string("VS_ReStartScene") + "'";
    str += ",";
    str += "'" + std::string("") + "'";
    str += ")";

    bc::BCEvent _tEvent(EVENT_RUN_WEB_JS, 0, 0, 0);
    _tEvent.strWebJsParam = str;
    m_pPluginManager->iGetSystemAPI()->iEngineAPI->iExecuteEvent(_tEvent);

    if(m_pPluginManager->iGetSystemAPI()->iProjectClientAPI)
    {
        m_pPluginManager->iGetSystemAPI()->iProjectClientAPI->iSaveLog(LOG_QT,"to web restart cloud connect!!!");
    }
}

void GLWidget::static_Cloud_log_callback(const std::string& strMsg)
{
    if(s_pThis->m_pPluginManager->iGetSystemAPI()->iProjectClientAPI)
    {
        s_pThis->m_pPluginManager->iGetSystemAPI()->iProjectClientAPI->iSaveLog(LOG_CLOUD,strMsg);
    }
}

void GLWidget::static_Cloud_handle_func(const std::string& strCode,QList<QJsonValue> list)
{
    if(strCode == "webrtc_connected")
    {
        emit s_pThis->sigReEncodeVideo();
    }
    else if(strCode == "webrtc_closed")
    {
        if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI)
        {
            std::string strMsg = "webrtc_closed";
            ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT,strMsg);
        }
        emit s_pThis->sigReStartCloud();
    }
}

void GLWidget::slotReEncodeVideo()
{
    //重新编码
    if(m_bEncodeRun)
    {
#ifdef USE_NVIDIA_HARD_ENCODE
        HCEncode_StopEncode();
        HCEncode_EncodeEnd();
#endif

    }
    m_bEncodeRun = false;
    if(ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI)
    {
        std::string strMsg = "glwidget restart encode video";
        ServerManager::GetInstance()->GetSystemAPI()->iProjectClientAPI->iSaveLog(bc::LOG_QT,strMsg);
    }
}

qreal GLWidget::getScreenRatio()
{
    if(m_pPluginManager->iGetSystemAPI() && m_pPluginManager->iGetSystemAPI()->iEngineAPI->iGetProjectConfig().sBootConfig.bBackgroundRender)
    {
        return 1.0f;
    }
    qreal ratio = 1.0f;
    if(QT_VERSION < QT_VERSION_CHECK(6,0,0))
    {
        ratio = 1.0f;
        return ratio;
    }

    QScreen *pScreen = nullptr;
    QList<QScreen*> listScreen = QGuiApplication::screens();
    for(auto *p : listScreen)
    {
        QRect screenGeometry = p->geometry();
        if(screenGeometry.contains(this->geometry().center()))
        {
            pScreen = p;
            break;
        }
    }
    if(pScreen)
    {
        ratio = pScreen->devicePixelRatio();
    }
    return ratio;
}
