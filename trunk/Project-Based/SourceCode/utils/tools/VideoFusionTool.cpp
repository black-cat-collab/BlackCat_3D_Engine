#include "ProjectBasedHeader.h"
#include "Project-Based/utils/tools/VideoFusionTool.h"
#include "Project-Based/base/ProjectBasedClient.h"

using namespace bc;

VideoFusionTool* VideoFusionTool::s_pThis = nullptr;
VideoFusionTool::VideoFusionTool(INode* pParentGroup):
    m_pFusionCameraNode(nullptr),
    m_pCurFusionNode(nullptr),
    m_bStartRender(false)
{
    s_pThis = this;
    m_pParentGroup = pParentGroup;
    if (!m_pParentGroup || m_pParentGroup->iGetNodeType() != NODE_GROUP)
    {
        m_pParentGroup = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode();
    }

    createFusionCameraNode();
}

VideoFusionTool::~VideoFusionTool()
{
}

void VideoFusionTool::createFusionCameraNode()
{
    FusionCameraParam stParam;
    stParam.bDepthTest = false;
    m_pFusionCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iCreateAndInitializeNode(stParam);
    m_pFusionCameraNode->iLoadModel();
    m_pFusionCameraNode->iSetVisible(false);
    m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iSetTopGroupNode(m_pParentGroup);
    m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iSetCreatedCallback(static_fusion_create_callback);
    m_pParentGroup->iAddChild(m_pFusionCameraNode);
    
}

void VideoFusionTool::SetPosition(const Vector3d& vOrg)
{
    m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iSetPosition(vOrg);
}

void VideoFusionTool::SetAngle(const Vector3d& vAngle)
{
    m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iSetAngle(vAngle);
}

void VideoFusionTool::SetFOV(float fFOV)
{
    m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iSetFOV(fFOV);
}

void VideoFusionTool::SetAspect(float fRatio)
{
    m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iSetAspect(fRatio);
}

void VideoFusionTool::SetDistance(float fDistane)
{
    m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iSetDistance(fDistane);
}

void VideoFusionTool::SetFuseImage(const std::string& strImage)
{
    m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iSetFuseImage(strImage);
}

void VideoFusionTool::Render()
{
    INode* pCameraNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetCurrentView()->iGetMainCamera();
    ICamera* pICamera = pCameraNode->GetDynamicComponent<ICamera>();
    m_pFusionCameraNode->iSetVisible(true);
    IFusionCamera* pIFusionCamera = m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>();
    GoViewPoint(pCameraNode, pIFusionCamera->iGetPosition(), pIFusionCamera->iGetAngle(),false);
    pIFusionCamera->iStart();

    m_bStartRender = true;
}

void VideoFusionTool::Stop()
{
    IFusionCamera* pIFusionCamera = m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>();
    m_pFusionCameraNode->iSetVisible(false);
    pIFusionCamera->iStop();

    m_bStartRender = false;
}

void VideoFusionTool::SetCutEnable(bool bEnable)
{
    m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iSetCutEnable(bEnable);
}

void VideoFusionTool::SetFuseParam(const FusionParam& tFusionParam)
{
    m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iSetFuseParam(tFusionParam);
}

void VideoFusionTool::MoveLeft(float fDis)
{
    Vector3d pos = m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iGetPosition();
    pos.x += fDis;
    m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iSetCameraPosition(pos);
}

void VideoFusionTool::MoveZ(float fDis)
{
    Vector3d pos = m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iGetPosition();
    pos.y += fDis;
    m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iSetCameraPosition(pos);
}

void VideoFusionTool::MoveForward(float fDis)
{
    Vector3d pos = m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iGetPosition();
    ProjectBasedClient* pClient = dynamic_cast<ProjectBasedClient*>(APIProvider::GetSystemAPI()->iProjectClientAPI);
    pClient->AdjustZ(pos, fDis,true);
    m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iSetCameraPosition(pos);
}

void VideoFusionTool::RotateX(float fAngle)
{

    //Vector3d angle = m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iGetAngle();
    Vector3d angle;
    angle.x = fAngle;
    m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iSetCameraAngle(angle);
}

void VideoFusionTool::RotateY(float fAngle)
{
    Vector3d angle;
    angle.y = fAngle;
    m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iSetCameraAngle(angle);
}

void VideoFusionTool::SetModelSavePath(std::string strSavePath)
{
    m_pFusionCameraNode->GetDynamicComponent<IFusionCamera>()->iSetSaveDir(strSavePath);
}

void VideoFusionTool::static_fusion_create_callback(IFusion* pFusionNode)
{
    INode* pNode = pFusionNode->iGetOriginNode();
    pNode->GetDynamicComponent<IRenderNode>()->iSetSelectEnable(true);
    pNode->GetDynamicComponent<IRenderNode>()->iSetCheckEnable(true);
    pNode->GetDynamicComponent<IRenderNode>()->iSetClickEnable(true);
    s_pThis->m_pCurFusionNode = pNode;
}

void VideoFusionTool::Save()
{
    s_pThis->m_pCurFusionNode = nullptr;
}

void VideoFusionTool::Reset()
{
    if (m_bStartRender)
    {
        if (m_pCurFusionNode)
        {
            FusionParam stParam;
            m_pCurFusionNode->GetDynamicComponent<IFusion>()->iGetFusionParam(stParam);
            std::string strPath = stParam.strTexturePath;
            std::string strTemp = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strProjectDir;
            strTemp += "/" + strPath;
            remove(strTemp.c_str());
            APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iDeleteNode(&m_pCurFusionNode, false);
        }
        m_pCurFusionNode = nullptr;
        Render();
    }
}

