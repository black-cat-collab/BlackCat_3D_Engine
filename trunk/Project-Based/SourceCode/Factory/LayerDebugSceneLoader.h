#pragma once

namespace bc
{
	class SceneElementLoader;
	class LayerDebugSceneLoader : public SceneElementLoader
	{
	protected:
		class InnerRequestEndData
		{
		public:
			std::vector<ProjectBasedSceneElementTypeInfo*> vecTempSceneTypeInfo;
			std::vector<ProjectBasedSceneElementMapInfo*> vecTempMapInfo;
			std::vector<ProjectBasedSceneElement*> vecSceneElement;

		};

		class  InnerPageData
		{
		public:
			bool	bClear;
			bool	bComplete;
			std::vector<ProjectBasedSceneElement*> vecData;

			InnerPageData()
			{
				bClear = false;
				bComplete = false;
			}

		};

		class InnerLayerDebugParam
		{
		public:
			int nType;
			int nPointID;
			bool bGetPos;
			bool	bGetManyPos;					// 是否获取多个三维点位
			std::string strWebCommondID;
			INode* pLocusLine;

			InnerLayerDebugParam()
			{
				nType = -1;
				nPointID = -1;
				bGetPos = false;
				bGetManyPos = false;
				pLocusLine = nullptr;
			}

		};

	public:
		LayerDebugSceneLoader();
		virtual ~LayerDebugSceneLoader();

		void FrameUpdate();
		virtual void iDoWebCommond(WebCommandData* pWebData);
		void SetShow(bool bShow) { m_bShow = bShow; }
		virtual void ChangeShowLabel();
		virtual EventReturnType_e	iProcessEvent(const BCEvent& tEvent);

	private:
		static bool static_request_ElementShow(std::map<std::string, INT_PTR>& mapParams, ThreadCallbackParam* stCallbackParam);	//线程执行函数	return false;不往下执行  return true;继续执行
		static void static_callback_ElementShow(ThreadCallbackParam* pParam);

		virtual void loadDataComplete(bool bAllComplete, bool bToWebCommond);
		void requestElementShow();
		void copyMapAndTypeInfo(InnerRequestEndData* pData);
		void requestSceneElement(InnerRequestEndData* pRequestEndData, ThreadCallbackParam* stCallbackParam);				//获取场景上图的所有元素
		void addPageTaskToMain(InnerPageData* pPageData, ThreadCallbackParam* stCallbackParam);
		void parseElement(ProjectBasedSceneElement* pInfo, Json::Value root, InnerRequestEndData* pEndData);
		void clearFactory();
		virtual void iOnCreateFactory(CommonLabelFactory* pFactory);

		// 获取单个三维点位坐标
		void getPointPos();
		// 添加三维点位坐标
		void addPointPos(bool bIsOver);

	private:
		BCMutexLock		m_ParseElementShowLock;
		std::string		m_strLayerDebuggToken;
		bool m_bShow;
		InnerLayerDebugParam	    m_stLayerDebugParam;					//标签调试参数
		bool										m_bToolsSceneFactoryShow;		//调试工具是否可见
		std::vector<std::string>		m_vecCurToolShowType;				// 标签调试工具显示图层
		std::vector<Vector3>				m_vecPos;						// 点位坐标

	};
};


