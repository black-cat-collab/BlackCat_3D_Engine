#pragma once

namespace bc
{
	class BuildIndoorStateFactory;
	class PROJECT_BASED_API BuildIndoorFactory : public BuildIndoorStateFactory
	{
	public:
		BuildIndoorFactory();
		virtual ~BuildIndoorFactory();

		virtual EventReturnType_e iProcessEvent(const BCEvent& tEvent);

		//请求数据
		void RequestData(const std::string& strUrl, bool bThread);
		//设置数据，直接传入数据
		void SetV10LessData(const std::string &strJsonData);		//V10之前服务版本
		void SetV10Data(const std::string& strJsonData);			//V10服务版本

		// 设置是否切换区域场景值
		void SetEnterAreaScene(bool bEnterAreaScene);

	protected:
		virtual void Create();
		//房间入住正常颜色
		virtual Vector4 iGetRoomNormalColor();
		//房间入住告警颜色
		virtual Vector4 iGetRoomWarningColor();
		//房间入住点击颜色
		virtual Vector4 iGetRoomClickColor();
		//房间点击
		virtual void iOnRoomClick(INode* pRoomNode, RoomData* pRoomData);

		//返回大场景列表
		virtual vector<AreaSceneData*> iGetAreaSceneNode();
		//返回楼栋群
		virtual vector<BuildingGroupData*> iGetBuildingGroupPerAreaScene(string strAreaSceneID, AreaSceneData* pData);
		//返回大场景的楼栋
		virtual vector<BuildingData*> iGetBuildingPerBuildingGroup(string strBigSceneID, BuildingGroupData* pData);
		//返回楼栋的楼层
		virtual vector<FloorData*> iGetFloorPerBuilding(string strBuildingID, BuildingData* pData);
		//返回楼层的房间
		virtual vector<RoomData*> iGetRoomPerFloor(string strFloorID, FloorData* pData);
		//进入大场景
		virtual void iEnterAreaScene(AreaSceneData *pData);
		virtual void iChangeBuilding(BuildingData *pData);
		//离开
		virtual void iLeaveAreaScene(AreaSceneData *pData);
		//切换楼层
		virtual void iChangeFloor(std::string strFloorID,FloorData *pData);
		//iUpdate
		virtual void iFrameUpdate();
		virtual vector<INode*> iGetAlphaNode(BuildingData* pData);
		//加载完成
		virtual void iLoadedData() {};
		virtual std::string iFilterRoomNum(std::string strRoomNum) { return strRoomNum; }

	private:
		static void	 Callback_Request_Callback(ThreadCallbackParam* pParam);
		// 自定义排序"小于"
		static bool compare(const FloorData* t1, const FloorData* t2);

		void	findRoomFromFloor(FloorData* pFloor, vector<string> vecKeys);
		void	clearData();

	protected:
		INode* m_pBigSceneNode;

		bool m_bEnterAreaScene;			// 室内分层是否进入单独的区域场景

	};
}


