#pragma once

#include "BasicDef.h"
#include "Utility/Math/MathHeader.h"

#include "Utility/String/StringProcess.h"
#include "Utility/Color/ColorProcess.h"
#include "Utility/BCFile/BCFile.h"
#include "Utility/Lock/BCAutoLock.h"
#include "Utility/Lock/BCMutexLock.h"
#include "Utility/ThreadPool/BCThreadPool.h"
#include "Utility/ThreadPool/BCThread.h"
#include "Utility/Guid/Guid.h"
#include "Utility/Unicode/UniConversion.h"
#include "Utility/Unicode/Utf8_16.h"
#include "Utility/Xml/RapidXml.hpp"
#include "Utility/Xml/RapidxmlPrint.hpp"
#include "Utility/Xml/BCXmlFile.h"
#include "Utility/MD5/MD5.h"
#include "Utility/Json/json.h"

#include "Utility/Mesh/MeshUtility.h"

#include "Engine/iEngineAPI.h"
#include "Engine/iProjectCore.h"
#include "Engine/iProjectPlugin.h"

#include "Protocol/iNetwork.h"
#include "Protocol/iMonitor.h"
#include "Protocol/iLocation.h"
#include "Protocol/iCloudRender.h"
#include "Protocol/iSoftwareLock.h"
#include "Protocol/iProtocolAPI.h"

 


#ifdef _MSC_VER
#include "Render/GL/glew.h"

#elif __GNUC__
#include <GL/glew.h>

#endif

#include <GL/glu.h>

#include "Render/iTexture.h"
#include "Render/iGLShader.h"
#include "Render/iEffectStage.h"
#include "Render/iParticleObject.h"
#include "Render/iMaterial.h"
#include "Render/iVertexBuffer.h"
#include "Render/iBufferObject.h"
#include "Render/iMesh.h"
#include "Render/iFrameBuffer.h"

#include "Core/iComponent.h"
#include "Core/iNode.h"
#include "Core/iView.h"
#include "Core/iCamera.h"
#include "Core/iManipulator.h"
#include "Core/iInertiaManipulator.h"
#include "Core/iOrthoManipulator.h"
#include "Core/iSky.h"
#include "Core/iGroup.h"
#include "Core/iRenderNode.h"
#include "Core/iCubeReflect.h"
#include "Core/iLight.h"
#include "Core/iGeo.h"
#include "Core/iParticle.h"
#include "Core/iBoard.h"
#include "Core/iSpotLightBoard.h"
#include "Core/iVideo.h"
#include "Core/iBoardRoute.h"
#include "Core/iVideoRoute.h"
#include "Core/iLightBeam.h"
#include "Core/iMeasure.h"
#include "Core/iLocusLine.h"
#include "Core/iFlyLine.h"
#include "Core/iVideoFusionNode.h"
#include "Core/iUserInterface.h"
#include "Core/iUIWidget.h"
#include "Core/iUIVideo.h"
#include "Core/iInstance.h"
#include "Core/iUIBoard.h"
#include "Core/iUIBoardInstance.h"
#include "Core/iDrawLine.h"
#include "Core/iShell.h"
#include "Core/iPolygon.h"
#include "Core/iRoad.h"
#include "Core/iPlotting.h"
#include "Core/iFusion.h"
#include "Core/iFusionCamera.h"
#include "Core/iAnimation.h"
#include "Core/IBuffer.h"
#include "Core/IEnergy.h"
#include "Core/iPolygonSelector.h"

#include "Core/iBIMRoot.h"
#include "Core/iTIF.h"
#include "Core/iBlankModel.h"

#include "Core/iObject.h"
#include "Core/iNodeClusterer.h"
#include "Core/iTimeLine.h"

#include "Core/iSkyOcclusion.h"
#include "Core/iCubeReflectView.h"
#include "Core/iHiZView.h"
#include "Core/iShadowView.h"
#include "Core/iSSAOView.h"
#include "Core/iGTAOView.h"
#include "Core/iDefferedLighting.h"
#include "Core/iHeatMap.h"
#include "Core/iViewshedAnalysis.h"
#include "Core/iVideoFusion.h"
#include "Core/iScreenSpaceReflect.h"
#include "Core/iRGBSplitGlitch.h"
#include "Core/iColorTransition.h"
#include "Core/iCoordinateAxisView.h"
#include "Core/iPOIView.h"
#include "Core/iShellRoot.h"
#include "Core/iPagedRoot.h"

#include "Core/iUndoCommand.h"
#include "Core/iViewManager.h"
#include "Core/iCoreAPI.h"
#include "Core/iVirtualRelationship.h"

#include "Render/iRenderAPI.h"

#include "Model/iModel.h"
#include "Model/iModelAPI.h"

#include "Project-Based/IProjectBasedAPI.h"
#include "Project-Based/iStateMachine.h"
#include "Project-Based/iStateManager.h"
#include "Project-Based/iProjectClientAPI.h"

