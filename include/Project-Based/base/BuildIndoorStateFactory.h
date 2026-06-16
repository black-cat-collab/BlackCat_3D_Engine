#ifndef _BUILD_INDOOR_STATE_FACTORY_H_
#define _BUILD_INDOOR_STATE_FACTORY_H_


#include "Project-Based/utils/ThreadWrapper.h"

/**
 * 建组室内Factory,包括分层，进入，离开等逻辑
 */


#define FLOOR_ALL_CLOSE_INDEX  -100		//楼层全关闭
#define FLOOR_ALL_OPEN_INDEX  -101		//楼层全打开
#define FLOOR_ALL_CLOSE_ID  string("ALL_FLOOR_CLOSE")	
#define FLOOR_ALL_OPEN_ID  string("ALL_FLOOR_OPEN")

using namespace std;

namespace bc
{
	class AreaSceneData;
	class BuildingGroupData;
	class BuildingData;
	class FloorData;
	class RoomData;
	class SceneElementLoader;

	enum EVersion {
		VERSION_V10_LESS = 0,
		VERSION_V10
	};

	// 房间数据
	class RoomData
	{
		friend FloorData;
	public:
		string			strRoomNodeID;		// id，用来区分，不可重复
		INode*			pNode;				// 节点
		FloorData*		pFloorData;			// 楼层
		std::string		strRoomNum;			//房间号
		int				nLiveAllPerson;			//房间入住人员总数量（包括告警数量）
		int				nLiveWarningPerson;		//房间入住人员告警数量

		RoomData()
		{
			pNode = NULL;
			pFloorData = NULL;
			nLiveAllPerson = 0;
			nLiveWarningPerson = 0;
		}

		RoomData& operator = (const class RoomData &src)
		{
			strRoomNodeID = src.strRoomNodeID;
			pNode = src.pNode;
			pFloorData = src.pFloorData;
			strRoomNum = src.strRoomNum;
			nLiveAllPerson = src.nLiveAllPerson;
			nLiveWarningPerson = src.nLiveWarningPerson;

			return *this;
		}

		~RoomData()
		{

		}
	};

	// 楼层数据
	class FloorData
	{
		friend BuildingData;
	public:

		int					nFloorIndex;				// 楼层序数
		float				fAllOpenSpace;				// 全部打开的间距
		float				fFlyHeight;					// 单层打开的间距

		string				strFloorNodeID;				
		string				strFloorName;				
		INode*				pNode;						// 节点
		BuildingData*		pBuildingData;				// 楼栋
		vector<RoomData*>	vecRoomData;
		Vector3d				vInitOrigin;				// 初始的位置
		float				fLabelOffsetZ;				// 标签相对偏移值
		bool				bNodeLoaded;
		bool				bOldSortNum;				//老版本解析时最顶层改名字
		FloorData()
		{
			pNode = NULL;
			pBuildingData = NULL;
			nFloorIndex = 0;
			fAllOpenSpace = 15.0f;
			fFlyHeight = fAllOpenSpace;
			fLabelOffsetZ = 1.5f;
			bNodeLoaded = false;
			bOldSortNum = false;
		}

		FloorData& operator = (const class FloorData &src)
		{
			strFloorNodeID = src.strFloorNodeID;
			strFloorName = src.strFloorName;
			pNode = src.pNode;
			pBuildingData = src.pBuildingData;
			vecRoomData = src.vecRoomData;
			nFloorIndex = src.nFloorIndex;
			fAllOpenSpace = src.fAllOpenSpace;
			fFlyHeight = src.fFlyHeight;
			vInitOrigin = src.vInitOrigin;
			fLabelOffsetZ = src.fLabelOffsetZ;
			bNodeLoaded = src.bNodeLoaded;
			bOldSortNum = src.bOldSortNum;
			return *this;
		}

		~FloorData()
		{
			for (size_t i = 0; i < vecRoomData.size(); i++)
			{
				DELETE_PTR(vecRoomData[i]);
			}
			vecRoomData.resize(0);
		}

		int GetLiveAllPerson()
		{
			int nCount = 0;
			for (size_t i = 0; i < vecRoomData.size(); i++)
			{
				nCount += vecRoomData[i]->nLiveAllPerson;
			}
			return nCount;
		}

		int GetLiveWarningPerson()
		{
			int nCount = 0;
			for (size_t i = 0; i < vecRoomData.size(); i++)
			{
				nCount += vecRoomData[i]->nLiveWarningPerson;
			}
			return nCount;
		}
	};

	// 楼栋数据
	class BuildingData
	{
		friend BuildingGroupData;
	public:
		string				strBuildingNodeID;			// id，用来区分，不可重复
		INode*				pNode;						// 节点
		BuildingGroupData*	pBuildingGroupData;			// 楼群
		vector<FloorData*>	vecFloorData;
		std::string			strName;					// 名称
		Vector3d				vLabelPos;					// 标签放置位置
		std::string			strInnerViewport;			// 室内视点

		BuildingData()
		{
			pNode = NULL;
			pBuildingGroupData = NULL;
		}

		BuildingData& operator = (const class BuildingData &src)
		{
			strBuildingNodeID = src.strBuildingNodeID;
			pNode = src.pNode;
			pBuildingGroupData = src.pBuildingGroupData;
			vecFloorData = src.vecFloorData;
			strName = src.strName;
			vLabelPos = src.vLabelPos;
			strInnerViewport = src.strInnerViewport;

			return *this;
		}

		~BuildingData()
		{
			for (size_t i = 0; i < vecFloorData.size(); i++)
			{
				DELETE_PTR(vecFloorData[i]);
			}
			vecFloorData.resize(0);
		}

		int GetLiveAllPerson()
		{
			int nCount = 0;
			for (size_t i = 0; i < vecFloorData.size(); i++)
			{
				nCount += vecFloorData[i]->GetLiveAllPerson();
			}
			return nCount;
		}

		int GetLiveWarningPerson()
		{
			int nCount = 0;
			for (size_t i = 0; i < vecFloorData.size(); i++)
			{
				nCount += vecFloorData[i]->GetLiveWarningPerson();
			}
			return nCount;
		}
	};

	//建筑群体数据
	class BuildingGroupData {
		friend AreaSceneData;
	public:
		std::string				strRealID;					//真实id（比如洲际酒店的编号）
		std::string				strName;					//名称（比如洲际酒店，包括很多栋）
		std::string				strNodeID;					//节点名称，不一定有
		INode*					pNode;						//模型，不一定有
		std::vector<BuildingData*> vecBuilding;				//下级楼栋
		AreaSceneData*			pAreaSceneData;
		int						nType;						// 建筑类型 500--酒店  501--场馆  502--重点建筑  503--风景区

		BuildingGroupData()
		{
			pNode = nullptr;
			pAreaSceneData = nullptr;
			nType = -1;
		}

		BuildingGroupData& operator = (const class BuildingGroupData& src)
		{
			strRealID = src.strRealID;
			strName = src.strName;
			strNodeID = src.strNodeID;
			pNode = src.pNode;
			vecBuilding = src.vecBuilding;
			pAreaSceneData = src.pAreaSceneData;
			nType = src.nType;

			return *this;
		}

		~BuildingGroupData()
		{
			for (size_t i = 0; i < vecBuilding.size(); i++)
			{
				DELETE_PTR(vecBuilding[i]);
			}
			vecBuilding.resize(0);
		}

		int GetLiveAllPerson()
		{
			int nCount = 0;
			for (size_t i = 0; i < vecBuilding.size(); i++)
			{
				nCount += vecBuilding[i]->GetLiveAllPerson();
			}
			return nCount;
		}

		int GetLiveWarningPerson()
		{
			int nCount = 0;
			for (size_t i = 0; i < vecBuilding.size(); i++)
			{
				nCount += vecBuilding[i]->GetLiveWarningPerson();
			}
			return nCount;
		}
	};

	// 区域场景数据
	class AreaSceneData
	{
	public:
		string					strAreaSceneNodeID;			// 区域场景的id，用来区分，不可重复
		INode*					pNode;						// 节点
		vector<BuildingGroupData*>	vecBuildingGroup;				// 栋群

	public:
		AreaSceneData()
		{
			pNode = NULL;
		}

		AreaSceneData& operator = (const class AreaSceneData &src)
		{
			strAreaSceneNodeID = src.strAreaSceneNodeID;
			pNode = src.pNode;
			vecBuildingGroup = src.vecBuildingGroup;

			return *this;
		}

		~AreaSceneData()
		{
			for (size_t i = 0; i < vecBuildingGroup.size(); i++)
			{
				DELETE_PTR(vecBuildingGroup[i]);
			}
			vecBuildingGroup.resize(0);
		}

		int GetLiveAllPerson()
		{
			int nCount = 0;
			for (size_t i = 0; i < vecBuildingGroup.size(); i++)
			{
				nCount += vecBuildingGroup[i]->GetLiveAllPerson();
			}
			return nCount;
		}

		int GetLiveWarningPerson()
		{
			int nCount = 0;
			for (size_t i = 0; i < vecBuildingGroup.size(); i++)
			{
				nCount += vecBuildingGroup[i]->GetLiveWarningPerson();
			}
			return nCount;
		}
	};

	class PROJECT_BASED_API BuildIndoorStateFactory
	{
	public:
		BuildIndoorStateFactory();
		virtual ~BuildIndoorStateFactory();

	public:
		virtual void Create();
		AreaSceneData* ChangeAreaScene(string strID,bool bDynamicLoaded = true);
		BuildingData* ChangeBuilding(string strID, bool bDynamicLoaded = true);
		FloorData* ChangeFloor(string strID);

		void ChangeAlpha(float fAlpha);				// 设置透明
		void OpenEffectShader(bool bOpen);			// 场景渲染
		void ChangeFog(bool bIndoor);				// 设置雾化
		void CloseAreaScene(bool bEnterAreaScene, bool bDynamicLoaded = true);
		virtual void FrameUpdate();
		virtual EventReturnType_e iProcessEvent(const BCEvent& tEvent) = 0;
		virtual EventReturnType_e	iExecuteEvent(const BCEvent& tEvent) { return EventReturnType_e::NONE; }

		AreaSceneData* GetCurAreaSceneData() { return m_pCurAreaSceneData; }
		BuildingGroupData* GetCurBuildingGroupData() { return m_pCurBuildingGroupData; }
		BuildingData* GetCurBuildingData() { return m_pCurBuildingData; }
		FloorData* GetCurFloorData() { return m_pCurFloorData; }
		RoomData* GetCurRoomData() { return m_pCurRoomData; }
		std::string GetCurFloorID() { return m_strCurFloorID; }

		void GetAllAreaSceneData(vector<AreaSceneData*> &vecData);
		AreaSceneData* GetAreaSceneData(string strID);
		void GetAllBuildingGroupData(vector<BuildingGroupData*>& vecData, AreaSceneData* pData = NULL);
		BuildingGroupData* GetBuildingGroupDataByNodeID(string strID, AreaSceneData* pData = NULL);
		BuildingGroupData* GetBuildingGroupDataByRealID(string strID, AreaSceneData* pData = NULL);
		void GetAllBuildingData(vector<BuildingData*> &vecData, BuildingGroupData*pData = NULL);
		BuildingData* GetBuildingData(string strID, BuildingGroupData*pData = NULL);
		void GetAllFloorData(vector<FloorData*> &vecData, BuildingData *pData = NULL);
		FloorData* GetFloorData(string strID, BuildingData *pData = NULL);
		void GetAllRoomData(vector<RoomData*> &vecData, FloorData *pData = NULL);
		RoomData* GetRoomData(string strID, FloorData *pData = NULL);

		void SetServerVersion(EVersion eVersion) { m_eServerVersion = eVersion; }
		//刷新上图工具数据
		virtual void RefreshSceneElement(std::string& strFloorID);

	protected:
		virtual AreaSceneData* iCreateAreaSceneData();
		virtual BuildingGroupData* iCreateBuildingGroupData();
		virtual BuildingData* iCreateBuildingData();
		virtual FloorData* iCreateFloorData();
		virtual RoomData* iCreateRoomData();


		//返回区域场景列表
		virtual vector<AreaSceneData*> iGetAreaSceneNode() = 0;
		//返回区域场景的楼栋
		virtual vector<BuildingGroupData*> iGetBuildingGroupPerAreaScene(string strAreaSceneID, AreaSceneData* pData) = 0;
		//返回区域场景的楼栋
		virtual vector<BuildingData*> iGetBuildingPerBuildingGroup(string strAreaSceneID, BuildingGroupData *pData) = 0;
		//返回楼栋的楼层
		virtual vector<FloorData*> iGetFloorPerBuilding(string strBuildingID, BuildingData* pData) = 0;
		//返回楼层的房间
		virtual vector<RoomData*> iGetRoomPerFloor(string strFloorID, FloorData* pData) = 0;
		//进入大场景
		virtual void iEnterAreaScene(AreaSceneData *pData) {}
		//切换楼栋
		virtual void iChangeBuilding(BuildingData *pData) {}
		//离开
		virtual void iLeaveAreaScene(AreaSceneData *pData) {}
		//切换楼层
		virtual void iChangeFloor(std::string strFloorID, FloorData *pData) {}
		//iUpdate
		virtual void iFrameUpdate() {}
		//设置需要透明的节点
		virtual vector<INode*> iGetAlphaNode(BuildingData* pData);

		//网络请求的拦截处理
		virtual void iInterceptHttpRequest(ThreadRequestParam* pRequest) {};

		//上图工具改变位置
		virtual void changeSceneElementPos();

	private:
		//楼层移动
		void floorMove(string strNodeID);
		//清空数据
		void clearData();


	protected:
		ISystemAPI*							m_pISystemAPI;
		vector<AreaSceneData*>				m_vecAreaSceneData;
		AreaSceneData*						m_pCurAreaSceneData;
		BuildingGroupData*					m_pCurBuildingGroupData;
		BuildingData*						m_pCurBuildingData;
		FloorData*							m_pCurFloorData;
		RoomData*							m_pCurRoomData;


		SceneElementLoader*					m_pSceneElementLoader;

		std::string							m_strFCSuffix;			//分层后缀
		std::string							m_strSNSuffix;			
		std::string							m_strSWSuffix;			
		std::string							m_strWKSuffix;			
		std::string							m_strLABELSuffix;		
		std::string							m_strBIGScene;		
		EVersion							m_eServerVersion;
		std::string							m_strCurFloorID;		//当前的

	};
}
/////////////////
#endif

