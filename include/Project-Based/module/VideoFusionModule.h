#pragma once

namespace bc
{
	class VideoFusionInfo
	{
	public:
		std::string strID;
		std::string strName;
		Vector3d vPos;
		double dLng;
		double dLat;
		Vector3d vAngle;
		float fFov;
		float fAspect;
		std::string strVideoUrl;


		VideoFusionInfo()
		{
			vAngle = Vector3d(45.0f, 45.0f, 0);
			fFov = 80.0f;
			fAspect = 1920.0f / 1080.0f;
			dLng = 0.0f;
			dLat = 0.0f;
		}

		class VideoFusionInfo& operator = (const class VideoFusionInfo& src)
		{
			strID = src.strID;
			strName = src.strName;
			vPos = src.vPos;
			dLng = src.dLng;
			dLat = src.dLat;
			vAngle = src.vAngle;
			fFov = src.fFov;
			fAspect = src.fAspect;
			strVideoUrl = src.strVideoUrl;

			return *this;
		}
	};

	class VideoFusionTool;
	class PROJECT_BASED_API VideoFusionModule : public BaseModule
	{
	public:
		VideoFusionModule(const std::string& strModuleName);
		virtual ~VideoFusionModule();

	public:
		virtual void				iSceneLoad(IStateManager* pStateManager);
		virtual void				iRefreshData();
		virtual void				iDoWebCommond(WebCommandData* pWebData);	//处理网页数据
		virtual void				iOnAppModeChanged(int nAppMode);
		virtual void				iOnAppIdChanged(std::string  strAppId);

		virtual void				ToRender(VideoFusionInfo* pInfo);	//开始预览并渲染
		virtual void				Stop();
		virtual void				SetVideoFusionVisible(bool bVisible);

	protected:
		virtual void iOnLoadedXml() {}
		virtual void iOnVideoFusionChangeVisible(INode* pNode,bool bVisible) {}


	private:
		void loadXml();

	protected:
		VideoFusionTool* m_pVideoFusionTool;

		INode* m_pVideoFusionGroup;
		std::string m_strSaveXmlPath;		//子tw4的保存路径
		std::string m_strSaveModelPath;		//模型，贴图保存路径

		std::unordered_map<std::string, INode*> m_mapFusionNode;
		std::vector<VideoFusionInfo*> m_vecVideoFusionInfo;		//数据源
		VideoFusionInfo* m_pCurVideoFusionInfo;
		bool	m_bVideoFusionVisible;

	};
}

