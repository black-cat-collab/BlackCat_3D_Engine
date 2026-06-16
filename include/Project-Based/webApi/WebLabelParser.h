#pragma once

namespace bc
{
	class CommonLabelFactory;
	class InstanceLabelFactory;
	class WebPoiLabelFactory;
	class WebLayerInsLabelFactory;
	class ProjectBasedClient;
	class IWebLabelFactory;
	class PROJECT_BASED_API WebLabelParser
	{
	private:
		enum ETaskType
		{
			TASK_LAYER_ADD = 1,	//图层添加元素
			TASK_LAYER_UPDATE,	   //图层更新元素
			TASK_POI_ADD,	   //poi添加元素
			TASK_POI_UPDATE,	   //poi更新元素
		};

		class TaskInfo
		{
		public:
			ETaskType eTaskType;
			int nCurDataIndex;
			WebCommandData tWebCommandData;

			TaskInfo()
			{
				nCurDataIndex = 0;
			}
		};

	public:
		WebLabelParser();
		virtual ~WebLabelParser();

		//解析NodeParam
		NodeParamBase* ParseNodeParam(std::string strJson);
		NodeParamBase* ParseNodeParam(Json::Value& jValue);

		virtual void   iDoWebCommond(WebCommandData* pWebData);
		virtual void   iFrameUpdate();
		virtual EventReturnType_e	iProcessEvent(const BCEvent& tEvent);
		void GetAllFactory(std::vector<CommonLabelFactory*> &vecFactory);
		CommonLabelFactory* GetFactoryByType(int nLabelType);
		//点击Node回调
		virtual void iOnBeforeClickLabel(INode* pNode, ProjectBasedWebLabelData* pInfo, bool bInstance, bool bClick) {}
		virtual void iOnLabelClickNode(INode* pNode, ProjectBasedWebLabelData* pInfo, bool bInstance,bool bClick) {}
		virtual void iOnDetailClickNode(INode* pNode, ProjectBasedWebLabelData* pInfo, bool bInstance, bool bClick) {}

		Vector2 ParseVector2(Json::Value& jValue);
		Vector3d ParseVector3(Json::Value& jValue);
		Vector4 ParseColor(Json::Value& jValue);
		void SetEnable(bool bEnable);
		void ChangeShowLabel();
		bool IsEnable() { return m_bEnable; }
		std::vector<std::string> GetCurShowType() { return m_vecCurShowType; }
		WebPoiLabelFactory* GetPoiFactory() { return m_pWebSinglePoiFactory; }
		void SetProjectPath(std::string strPath) { m_strProjectPath = strPath; }


	protected:
		virtual IWebLabelFactory* iCreateFactory(int nLabelType, NodeType_e eNodeType,bool bDetail);

		virtual void parseBaseParam(Json::Value &jValue, NodeParamBase* pParam);
		virtual void parseBoardParam(Json::Value& jValue, BoardParam* pParam);
		virtual void parseTextShow(Json::Value& jValue, FontBoardParam* pParam);
		virtual void parseFontBoardParam(Json::Value& jValue, FontBoardParam* pParam);
		virtual void parseVideoBoardParam(Json::Value& jValue, VideoBoardParam* pParam);
		virtual void parseButtonBoardParam(Json::Value& jValue, ButtonBoardParam* pParam);
		virtual void parseFontBoardWithButtonParam(Json::Value& jValue, FontBoardWithButtonParam* pParam);
		virtual void parseVideoBoardWithButtonParam(Json::Value& jValue, VideoBoardWithButtonParam* pParam);
		virtual void parseBoardWithFontBoardParam(Json::Value& jValue, BoardWithFontBoardParam* pParam);
		virtual void parsePolygonParam(Json::Value& jValue, PolygonParam* pParam);
		virtual void parseRoadParam(Json::Value& jValue, RoadParam* pParam);
		virtual void parseBoardInstanceParam(Json::Value& jValue, BoardInstanceParam* pParam);
		virtual void parseLocusLineParam(Json::Value& jValue, LocusLineParam* pParam);
		virtual void parseFlyLineParam(Json::Value& jValue, FlyLineParam* pParam);
		virtual void parseEffectParam(Json::Value& jValue, BoardParam* pParam);

		void clearFactory(CommonLabelFactory *pFactory);
		void parseData(int nType, Json::Value &jValue, ProjectBasedWebLabelData* pInfo, NodeParamBase* pParam);
		void deleteLayer(int nType, const std::string& strLabelStyleMD5, const std::string& strDetailStyleMD5, const std::string& strClusterStyleMD5);		//删除图层
		void clearAllLayerElement(int nType);	//清空图层所有的元素

		void setSmoothMoveParam(ProjectBasedWebLabelData* pInfo);	//设置平滑移动的相关参数

		bool addPoiElement(TaskInfo* pTask, WebCommandData * pWebData,int &nCount,const int &nMaxCount);
		bool updatePoiElement(TaskInfo* pTask, WebCommandData* pWebData, int& nCount, const  int& nMaxCount);
		bool addLayerElement(TaskInfo* pTask, WebCommandData* pWebData, int& nCount, const int& nMaxCount);
		bool updateLayerElement(TaskInfo* pTask, WebCommandData* pWebData, int& nCount, const int& nMaxCount);

	private:
		static void	static_callback_downshpFile(ThreadCallbackParam_s* pParam);
		static void	static_callback_cartographicOutput(ThreadCallbackParam_s* pParam);


	protected:
		ProjectBasedClient*				m_pClient;
		std::map<int, IWebLabelFactory*> m_mapLabelFactory;
		std::vector<std::string>		 m_vecCurShowType;
		bool				m_bEnable;			//是否可用
		WebPoiLabelFactory*				m_pWebSinglePoiFactory;
		int								 m_nCurPoiIndex;			//当前poi点的序号，累加为防止重复
		std::string						 m_strProjectPath;				//项目resource路径，从resource开头	
		//传入的id对应的真实shp生成的polygon id，因为一个shp可以有多个多边形
		std::map<std::string, std::vector<std::string>> m_mapShp;
		std::queue<TaskInfo*> m_queueTaskInfo;

	};
}


