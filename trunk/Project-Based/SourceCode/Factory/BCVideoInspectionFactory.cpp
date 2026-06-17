#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/base/VideoInspectionFactory.h" 
#include "Project-Based/Factory/BCVideoInspectionFactory.h"

using namespace bc;

BCVideoInspectionFactory::BCVideoInspectionFactory(ISystemAPI* pSystemAPI)
	: VideoInspectionFactory(pSystemAPI)
{
}

BCVideoInspectionFactory::~BCVideoInspectionFactory()
{
}

void BCVideoInspectionFactory::iModifyPointInfo(const std::string& strKey, std::vector<BCVideoRouterPointInfo*>& vecPointInfo)
{
	for (size_t i = 0; i < vecPointInfo.size(); i++)
	{
		if (vecPointInfo[i]->bWithVideo)
		{
			BCVideoRouterPointInfo* pInfo = vecPointInfo[i];
			iSetVideoBoardParam(strKey, vecPointInfo[i], &vecPointInfo[i]->stVideoParam);
			VideoBoardWithButtonParam* pParam = &vecPointInfo[i]->stVideoParam;
			pParam->direction = NormalDirect;
			//pParam->bDynamicScaleEnable = false;
			//pParam->fScale = 1.0f;
			//pParam->bDepthTest = false;
			VideoBoardParam* pVideoParam = nullptr;
			if (pParam->vecVideoParam.size() > 0)
			{
				pVideoParam = &pParam->vecVideoParam[0];
			}
			if (pVideoParam)
			{
				if (!pInfo->strUrl.empty())
				{
					pVideoParam->strLiveUrl = pInfo->strUrl;
					pVideoParam->ePlugin = MEDIA_PLUGIN_FFMPEG;
				}
				else
				{
					//livegbs
					pVideoParam->strDeviceID = pInfo->strCameraID;
					pVideoParam->ePlugin = MEDIA_PLUGIN_LIVEGBS;
				}
				pVideoParam->bTimeStamp = true;
				pVideoParam->bPlayVideo = false;
				pVideoParam->bAutoReconnect = false;
			}
		}
	}
}



