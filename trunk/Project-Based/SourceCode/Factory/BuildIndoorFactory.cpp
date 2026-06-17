#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ProjectLogger.h"
#include "Project-Based/base/BuildIndoorStateFactory.h"
#include "Project-Based/base/SceneElementLoader.h"
#include "Project-Based/Factory/BuildIndoorFactory.h"

using namespace bc;

// 自定义排序"小于"
bool BuildIndoorFactory::compare(const FloorData* t1, const FloorData* t2)
{
	return t1->nFloorIndex < t2->nFloorIndex;
}

BuildIndoorFactory::BuildIndoorFactory()
	:BuildIndoorStateFactory()
{
	m_pBigSceneNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(m_strBIGScene);
	m_bEnterAreaScene = true;
}

BuildIndoorFactory::~BuildIndoorFactory()
{
}

EventReturnType_e BuildIndoorFactory::iProcessEvent(const BCEvent& tEvent)
{
	if (tEvent.eMessage == EVENT_NODE_CLICK)
	{
		INode* pNode = (INode*)(tEvent.nParam1);
                if(!pNode)
                {
                    return EventReturnType_e::NONE;
                }
		if (pNode->iGetParent())
		{
			string strParentID = pNode->iGetParent()->iGetID();
			if (strParentID.find(m_strLABELSuffix) != string::npos)
			{
				RoomData* pRoomData = (RoomData*)pNode->iGetExternParam();
				if (m_pCurRoomData)
				{
					GetComponent<IRenderNode>(m_pCurRoomData->pNode)->iChangeColor(false);
				}
				m_pCurRoomData = pRoomData;
				if (m_pCurRoomData)
				{
					GetComponent<IRenderNode>(m_pCurRoomData->pNode)->iChangeColor(true, iGetRoomClickColor());
				}

				iOnRoomClick(pNode, pRoomData);
			}
		}
	}
	return EventReturnType_e::NONE;
}

void BuildIndoorFactory::Create()
{
	//BuildIndoorStateFactory::Create();
}

void BuildIndoorFactory::RequestData(const std::string& strUrl, bool bThread)
{
	ThreadRequestParam stParam;
	stParam.eRequestType = THREAD_REQUEST_API_GET;
	stParam.pThreadCompleteFunc = Callback_Request_Callback;
	stParam.AddParam("this", (INT_PTR)this);
	stParam.strUrl = strUrl;
	iInterceptHttpRequest(&stParam);
	stParam.strID = "HCBuildIndoorFactory_RequestData";
	stParam.bThread = bThread;
	stParam.bRunCallbackInMainThread = true;
	ThreadWrapper::GetInstance()->AddTask(stParam);
}

// 设置是否切换区域场景值
void BuildIndoorFactory::SetEnterAreaScene(bool bEnterAreaScene)
{
	m_bEnterAreaScene = bEnterAreaScene;
}

void BuildIndoorFactory::SetV10LessData(const std::string& strJsonData)
{
	//默认的后台解析流程
	vector<Json::Value> vecHotelValue;
	vector<Json::Value> vecBuildingValue;
	vector<Json::Value> vecFloorValue;
	vector<Json::Value> vecRoomValue;
	char szBuffer[1024] = { 0 };
	try
	{
		//清空
		clearData();

		Json::CharReaderBuilder jsonBuilder;
		std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
		Json::Value root;
		std::string jsError;
		if (reader->parse(strJsonData.c_str(), strJsonData.c_str() + strJsonData.length(), &root, &jsError))
		{
			if (root.isObject() && root.isMember("code"))
			{
				root = root["data"];
			}
			INode* pRootNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode();
			for (auto hotel : root)
			{
				vecHotelValue.push_back(hotel);
				Json::Value item = hotel["builds"];
				for (auto building : item)
				{
					vecBuildingValue.push_back(building);
					Json::Value floors = building["floors"];
					for (auto floor : floors)
					{
						vecFloorValue.push_back(floor);
						Json::Value rooms = floor["rooms"];
						for (auto room : rooms)
						{
							vecRoomValue.push_back(room);
						}
					}
				}
			}

			//解析每一层级
			for (auto hotel : vecHotelValue)
			{
				std::string strAreaNodeID = hotel["area_id"].asString();
				INode* pAreaNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(strAreaNodeID);
				if (!pAreaNode)
				{
					continue;
				}
				AreaSceneData* pAreaSceneData = GetAreaSceneData(strAreaNodeID);
				if (!pAreaSceneData)
				{
					pAreaSceneData = iCreateAreaSceneData();
					pAreaSceneData->strAreaSceneNodeID = strAreaNodeID;
					pAreaSceneData->pNode = pAreaNode;
					m_vecAreaSceneData.push_back(pAreaSceneData);
				}

				BuildingGroupData* pBuild = iCreateBuildingGroupData();

				pBuild->nType = hotel["type"].asInt();
				Utf8ToAscii(hotel["hotel_name"].asCString(), sizeof(szBuffer), szBuffer);
				pBuild->strName = szBuffer;
				pBuild->strRealID = hotel["hotel_real_id"].asString();
				pBuild->strNodeID = hotel["hotel_node_id"].asString();
				pBuild->pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(pBuild->strNodeID);
				if (pBuild->pNode == NULL)
				{
					pBuild->pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(pBuild->strNodeID + m_strFCSuffix);
				}
				pBuild->pAreaSceneData = pAreaSceneData;
				pAreaSceneData->vecBuildingGroup.push_back(pBuild);
			}

			for (auto building : vecBuildingValue)
			{
				string str = building["hotel_node_id"].asString();
				BuildingGroupData* pHotel = GetBuildingGroupDataByNodeID(str);
				if (!pHotel)
				{
					continue;
				}
				BuildingData* pBuilding = iCreateBuildingData();
				string strNodes = building["building_node_id"].asString();
				pBuilding->strBuildingNodeID = strNodes;
				if (!building["label_pos"].asString().empty())
				{
					std::string strLabelPos = building["label_pos"].asString();
					sscanf_s(strLabelPos.c_str(), "%lf,%lf,%lf", &pBuilding->vLabelPos.x, &pBuilding->vLabelPos.y, &pBuilding->vLabelPos.z);
				}
				else
				{
					Vector3d vPos;
					if (pHotel->pNode)
					{
						vPos = pHotel->pNode->iGetOrigin();
						vPos.z += 1820.0f;
					}

					pBuilding->vLabelPos = vPos;
				}
				pBuilding->pBuildingGroupData = pHotel;
				pBuilding->pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(pBuilding->strBuildingNodeID);
				if (pBuilding->pNode == NULL)
				{
					pBuilding->pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(pBuilding->strBuildingNodeID + m_strFCSuffix);
				}
				pHotel->vecBuilding.push_back(pBuilding);
			}

			map<string, Vector2> mapBuildFloorSpaceHeight;
			for (auto floor : vecFloorValue)
			{
				string str = floor["building_node_id"].asString();
				BuildingData* pBuilding = GetBuildingData(str);
				if (!pBuilding)
				{
					continue;
				}
				string strFloorNodeID = floor["floor_node_id"].asString();
				if (strFloorNodeID == str)
				{
					string strFloorName = floor["floor_name"].asString();
					vector<string> vecAll;
					SplitStringBySpecial(strFloorName, vecAll, "(");
					string stNext = "";
					if (vecAll.size() > 1)
					{
						stNext = "(" + vecAll[1];
						strFloorName = vecAll[0];
					}
					stNext = Replace_all(stNext, "(", "");
					stNext = Replace_all(stNext, ")", "");
					//(100:20）
					//a:b
					//a--单层展开的间距
					//b--全部展开的间距
					vector<string> vec;
					SplitStringBySpecial(stNext, vec, ":");
					float fHeight = 200.0f;
					float fAllFloorOpenSpace = 20.0f;
					if (vec.size() >= 1)
					{
						fHeight = atof(vec[0].c_str());
					}
					if (vec.size() >= 2)
					{
						fAllFloorOpenSpace = atof(vec[1].c_str());
					}
					mapBuildFloorSpaceHeight[str] = Vector2(fHeight, fAllFloorOpenSpace);
				}
			}
			for (auto floor : vecFloorValue)
			{
				string str = floor["building_node_id"].asString();
				BuildingData* pBuilding = GetBuildingData(str);
				if (!pBuilding)
				{
					continue;
				}
				string strFloorNodeID = floor["floor_node_id"].asString();
				if (strFloorNodeID == str)
				{
					continue;
				}

				string strFloorName = floor["floor_name"].asString();
				vector<string> vecAll;
				SplitStringBySpecial(strFloorName, vecAll, "(");
				string stNext = "";
				if (vecAll.size() > 1)
				{
					stNext = "(" + vecAll[1];
					strFloorName = vecAll[0];
				}
				stNext = Replace_all(stNext, "(", "");
				stNext = Replace_all(stNext, ")", "");
				//(100:20）
				//a:b
				//a--单层展开的间距
				//b--全部展开的间距
				vector<string> vec;
				SplitStringBySpecial(stNext, vec, ":");
				float fHeight = 200.0f;
				float fAllFloorOpenSpace = 20.0f;
				if (vec.size() >= 1)
				{
					fHeight = atof(vec[0].c_str());
				}
				if (vec.size() >= 2)
				{
					fAllFloorOpenSpace = atof(vec[1].c_str());
				}

				if (strFloorNodeID.find(":") != string::npos)
				{
					//新的规则
					vector<string> vec;
					SplitStringBySpecial(strFloorNodeID, vec, ":");
					if (vec.size() < 3)
					{
						continue;
					}
					string strPre = vec[0];
					vector<string> vecFloors;
					SplitStringBySpecial(vec[1], vecFloors, "-");
					if (vecFloors.size() < 2)
					{
						continue;
					}
					int nStart = atoi(vecFloors[0].c_str());
					int nEnd = atoi(vecFloors[1].c_str());
					for (int i = nStart; i <= nEnd; i++)
					{
						FloorData* pFloor = iCreateFloorData();
						pFloor->pBuildingData = pBuilding;
						pFloor->nFloorIndex = i;
						string strNodeID = strPre + to_string(i) + vec[2];
						string strFloorName = to_string(i) + vec[2];
						pFloor->strFloorNodeID = strNodeID;
						//if (i == nEnd)
						//{
							//char sz[1024] = { 0 };
							//ascii_to_utf8("屋顶", sizeof(sz),sz );
						//	strFloorName = string(sz);
						//}
						pFloor->strFloorName = strFloorName;
						pFloor->pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(pFloor->strFloorNodeID);
						if (pFloor->pNode)
						{
							pFloor->pNode->iSetExternParam((INT_PTR)pFloor);
							pFloor->vInitOrigin = pFloor->pNode->iGetOrigin();
							if (stNext == "" && mapBuildFloorSpaceHeight.find(str) != mapBuildFloorSpaceHeight.end())
							{
								fHeight = mapBuildFloorSpaceHeight[str][0];
								fAllFloorOpenSpace = mapBuildFloorSpaceHeight[str][1];
							}
							pFloor->fFlyHeight = fHeight;
							pFloor->fAllOpenSpace = fAllFloorOpenSpace;
						}
						pBuilding->vecFloorData.push_back(pFloor);
					}
				}
				else
				{
					//老的解析
					FloorData* pFloor = iCreateFloorData();
					pFloor->pBuildingData = pBuilding;
					pFloor->nFloorIndex = floor["floor_index"].asInt();
					string strNodeID = floor["floor_node_id"].asString();
					pFloor->strFloorNodeID = strNodeID;
					pFloor->strFloorName = strFloorName;
					pFloor->pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(strNodeID);
					if (pFloor->pNode)
					{
						pFloor->pNode->iSetExternParam((INT_PTR)pFloor);
						pFloor->vInitOrigin = pFloor->pNode->iGetOrigin();
						if (stNext == "" && mapBuildFloorSpaceHeight.find(str) != mapBuildFloorSpaceHeight.end())
						{
							fHeight = mapBuildFloorSpaceHeight[str][0];
							fAllFloorOpenSpace = mapBuildFloorSpaceHeight[str][1];
						}
						pFloor->fFlyHeight = fHeight;
						pFloor->fAllOpenSpace = fAllFloorOpenSpace;
					}
					pBuilding->vecFloorData.push_back(pFloor);
				}
				// 根据楼层信息排序
				sort(pBuilding->vecFloorData.begin(), pBuilding->vecFloorData.end(), compare);
			}

			for (auto room : vecRoomValue)
			{
				string str = room["floor_node_id"].asString();
				BuildingData* pBuilding = GetBuildingData(str);
				FloorData* pFloor = GetFloorData(str);
				if (!pFloor && !pBuilding)
				{
					continue;
				}
				string strMatchKey = room["room_match_key"].asString();
				if (strMatchKey.empty())
				{
					continue;
				}
				vector<string> vec;
				SplitStringBySpecial(strMatchKey, vec, ",");
				if (pBuilding)
				{
					//获取每一层
					for (auto pBFloor : pBuilding->vecFloorData)
					{
						findRoomFromFloor(pBFloor, vec);
					}
				}
				else
				{
					findRoomFromFloor(pFloor, vec);
				}
			}

			std::vector<BuildingData*> vecBuilding;
			GetAllBuildingData(vecBuilding);

			//设置屋顶
			for (int i = 0; i < vecBuilding.size(); i++)
			{
				BuildingData* pBuilding = vecBuilding[i];
				for (int n = 0; n < pBuilding->vecFloorData.size(); n++)
				{
					if (n == pBuilding->vecFloorData.size() - 1)
					{
						string strFloorName = pBuilding->vecFloorData[n]->strFloorName;
						char sz[1024] = { 0 };
						AsciiToUtf8("屋顶", sizeof(sz), sz);
						if (strFloorName != sz)
						{
							pBuilding->vecFloorData[n]->strFloorName = "屋顶";
						}
					}
				}
			}
		}
	}
	catch (const std::exception&)
	{
	}
}

void BuildIndoorFactory::SetV10Data(const std::string& strJsonData)
{
	//默认的后台解析流程
	vector<Json::Value> vecHotelValue;
	vector<Json::Value> vecBuildingValue;
	vector<Json::Value> vecFloorValue;
	vector<Json::Value> vecRoomValue;
	char szBuffer[1024] = { 0 };
	try
	{
		//清空
		clearData();

		Json::CharReaderBuilder jsonBuilder;
		std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
		Json::Value root;
		std::string jsError;
		if (reader->parse(strJsonData.c_str(), strJsonData.c_str() + strJsonData.length(), &root, &jsError))
		{
			if (root["data"].isNull() || root["data"].size() == 0)
			{
				return;
			}
			root = root["data"];
			INode* pRootNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iGetRootNode();
			for (auto hotel : root)
			{
				hotel["parent"] = Json::nullValue;
				vecHotelValue.push_back(hotel);
				Json::Value item = hotel["children"];
				for (auto building : item)
				{
					building["parent"] = hotel;
					vecBuildingValue.push_back(building);
					Json::Value floors = building["children"];
					for (auto floor : floors)
					{
						floor["parent"] = building;
						vecFloorValue.push_back(floor);
						Json::Value rooms = floor["children"];
						for (auto room : rooms)
						{
							room["parent"] = floor;
							vecRoomValue.push_back(room);
						}
					}
				}
			}

			//解析每一层级
			for (auto hotel : vecHotelValue)
			{
				std::string strAreaNodeID = hotel["area_id"].asString();
				INode* pAreaNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(strAreaNodeID);
				if (!pAreaNode)
				{
					continue;
				}
				AreaSceneData* pAreaSceneData = GetAreaSceneData(strAreaNodeID);
				if (!pAreaSceneData)
				{
					pAreaSceneData = iCreateAreaSceneData();
					pAreaSceneData->strAreaSceneNodeID = strAreaNodeID;
					//将节点从根节点移除，移入到业务节点，这样可以通过业务节点做到统一隐藏和显示
					pAreaNode->iGetParent()->iRemoveChild(pAreaNode);
					APIProvider::GetSystemAPI()->iProjectClientAPI->iGetProjectGroupNode()->iAddChild(pAreaNode);
					pAreaSceneData->pNode = pAreaNode;
					m_vecAreaSceneData.push_back(pAreaSceneData);
				}

				BuildingGroupData* pBuild = iCreateBuildingGroupData();

				pBuild->nType = hotel["type"].asInt();
				Utf8ToAscii(hotel["name"].asCString(), sizeof(szBuffer), szBuffer);
				pBuild->strName = szBuffer;
				pBuild->strRealID = hotel["data_id"].asString();
				pBuild->strNodeID = hotel["nodeid"].asString();
				pBuild->pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(pBuild->strNodeID);
				if (pBuild->pNode == NULL)
				{
					pBuild->pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(pBuild->strNodeID + m_strFCSuffix);
				}
				pBuild->pAreaSceneData = pAreaSceneData;
				pAreaSceneData->vecBuildingGroup.push_back(pBuild);
			}

			for (auto building : vecBuildingValue)
			{
				string str = building["parent"]["nodeid"].asString();
				BuildingGroupData* pHotel = GetBuildingGroupDataByNodeID(str);
				if (!pHotel)
				{
					continue;
				}
				BuildingData* pBuilding = iCreateBuildingData();
				string strNodes = building["nodeid"].asString();
				pBuilding->strBuildingNodeID = strNodes;
				if (building["parent"].isMember("label_pos") && !building["parent"]["label_pos"].asString().empty())
				{
					std::string strLabelPos = building["parent"]["label_pos"].asString();
					sscanf_s(strLabelPos.c_str(), "%lf,%lf,%lf", &pBuilding->vLabelPos.x, &pBuilding->vLabelPos.y, &pBuilding->vLabelPos.z);
				}
				else
				{
					Vector3d vPos(0, 0, 0);
					/*if (pHotel->pNode)
					{
						vPos = pHotel->pNode->iGetOrigin();
						vPos.z += 1820.0f;
					}*/
					pBuilding->vLabelPos = vPos;
				}
				pBuilding->pBuildingGroupData = pHotel;
				pBuilding->pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(pBuilding->strBuildingNodeID);
				if (pBuilding->pNode == NULL)
				{
					pBuilding->pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(pBuilding->strBuildingNodeID + m_strFCSuffix);
				}
				if (building.isMember("viewport"))
				{
					std::string strViewport = building["viewport"].asString();
					pBuilding->strInnerViewport = strViewport;
				}
				Utf8ToAscii(building["parent"]["name"].asCString(), sizeof(szBuffer), szBuffer);
				pBuilding->strName = szBuffer;
				pHotel->vecBuilding.push_back(pBuilding);
			}

			map<string, Vector2> mapBuildFloorSpaceHeight;
			for (auto floor : vecFloorValue)
			{
				string str = floor["parent"]["nodeid"].asString();
				BuildingData* pBuilding = GetBuildingData(str);
				if (!pBuilding)
				{
					continue;
				}
				string strFloorNodeID = floor["nodeid"].asString();
				if (strFloorNodeID == str)
				{
					string strFloorName = floor["name"].asString();
					vector<string> vecAll;
					SplitStringBySpecial(strFloorName, vecAll, "(");
					string stNext = "";
					if (vecAll.size() > 1)
					{
						stNext = "(" + vecAll[1];
						strFloorName = vecAll[0];
					}
					stNext = Replace_all(stNext, "(", "");
					stNext = Replace_all(stNext, ")", "");
					//(100:20）
					//a:b
					//a--单层展开的间距
					//b--全部展开的间距
					vector<string> vec;
					SplitStringBySpecial(stNext, vec, ":");
					float fHeight = 200.0f;
					float fAllFloorOpenSpace = 20.0f;
					if (vec.size() >= 1)
					{
						fHeight = atof(vec[0].c_str());
					}
					if (vec.size() >= 2)
					{
						fAllFloorOpenSpace = atof(vec[1].c_str());
					}
					mapBuildFloorSpaceHeight[str] = Vector2(fHeight, fAllFloorOpenSpace);
				}
			}
			for (auto floor : vecFloorValue)
			{
				string str = floor["parent"]["nodeid"].asString();
				BuildingData* pBuilding = GetBuildingData(str);
				if (!pBuilding)
				{
					continue;
				}
				string strFloorNodeID = floor["nodeid"].asString();
				if (strFloorNodeID == str)
				{
					continue;
				}

				string strFloorName = floor["name"].asString();
				vector<string> vecAll;
				SplitStringBySpecial(strFloorName, vecAll, "(");
				string stNext = "";
				if (vecAll.size() > 1)
				{
					stNext = "(" + vecAll[1];
					strFloorName = vecAll[0];
				}
				stNext = Replace_all(stNext, "(", "");
				stNext = Replace_all(stNext, ")", "");
				//(100:20）
				//a:b
				//a--单层展开的间距
				//b--全部展开的间距
				vector<string> vec;
				SplitStringBySpecial(stNext, vec, ":");
				float fHeight = 200.0f;
				float fAllFloorOpenSpace = 20.0f;
				if (vec.size() >= 1)
				{
					fHeight = atof(vec[0].c_str());
				}
				if (vec.size() >= 2)
				{
					fAllFloorOpenSpace = atof(vec[1].c_str());
				}

				if (strFloorNodeID.find(":") != string::npos)
				{
					//新的规则
					vector<string> vec;
					SplitStringBySpecial(strFloorNodeID, vec, ":");
					if (vec.size() < 3)
					{
						continue;
					}
					string strPre = vec[0];
					vector<string> vecFloors;
					SplitStringBySpecial(vec[1], vecFloors, "-");
					if (vecFloors.size() < 2)
					{
						continue;
					}
					int nStart = atoi(vecFloors[0].c_str());
					int nEnd = atoi(vecFloors[1].c_str());
					for (int i = nStart; i <= nEnd; i++)
					{
						FloorData* pFloor = iCreateFloorData();
						pFloor->pBuildingData = pBuilding;
						pFloor->nFloorIndex = i;
						string strNodeID = strPre + to_string(i) + vec[2];
						string strFloorName = to_string(i) + vec[2];
						pFloor->strFloorNodeID = strNodeID;
						//if (i == nEnd)
						//{
							//char sz[1024] = { 0 };
							//ascii_to_utf8("屋顶", sizeof(sz),sz );
						//	strFloorName = string(sz);
						//}
						pFloor->strFloorName = strFloorName;
						pFloor->pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(pFloor->strFloorNodeID);
						if (pFloor->pNode)
						{
							pFloor->pNode->iSetExternParam((INT_PTR)pFloor);
							pFloor->vInitOrigin = pFloor->pNode->iGetOrigin();
							if (stNext == "" && mapBuildFloorSpaceHeight.find(str) != mapBuildFloorSpaceHeight.end())
							{
								fHeight = mapBuildFloorSpaceHeight[str][0];
								fAllFloorOpenSpace = mapBuildFloorSpaceHeight[str][1];
							}
							pFloor->fFlyHeight = fHeight;
							pFloor->fAllOpenSpace = fAllFloorOpenSpace;
						}
						pBuilding->vecFloorData.push_back(pFloor);
					}
					if (vec.size() >= 4)
					{
						vector<string> vecOffsetZ;
						SplitStringBySpecial(vec[3], vecOffsetZ, ",");
						for (int n1 = 0; n1 < vecOffsetZ.size(); n1++)
						{
							vector<string> vecPerZ;
							SplitStringBySpecial(vecOffsetZ[n1], vecPerZ, "-");
							if (vecPerZ.size() >= 2)
							{
								int nIndex = atoi(vecPerZ[0].c_str());
								float fZ = atof(vecPerZ[1].c_str());
								for (int n2 = 0; n2 < pBuilding->vecFloorData.size(); n2++)
								{
									if (pBuilding->vecFloorData[n2]->nFloorIndex == nIndex)
									{
										pBuilding->vecFloorData[n2]->fLabelOffsetZ = fZ;
										break;
									}
								}
							}
						}
					}
				}
				else
				{
					//老的解析
					FloorData* pFloor = iCreateFloorData();
					pFloor->pBuildingData = pBuilding;
					pFloor->nFloorIndex = floor["sort_num"].asInt();
					if (pFloor->nFloorIndex > 0)
					{
						pFloor->bOldSortNum = true;
					}
					string strNodeID = floor["nodeid"].asString();
					pFloor->strFloorNodeID = strNodeID;
					pFloor->strFloorName = strFloorName;
					pFloor->pNode = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager()->iFindNodeByID(strNodeID);
					if (pFloor->pNode)
					{
						pFloor->pNode->iSetExternParam((INT_PTR)pFloor);
						pFloor->vInitOrigin = pFloor->pNode->iGetOrigin();
						if (stNext == "" && mapBuildFloorSpaceHeight.find(str) != mapBuildFloorSpaceHeight.end())
						{
							fHeight = mapBuildFloorSpaceHeight[str][0];
							fAllFloorOpenSpace = mapBuildFloorSpaceHeight[str][1];
						}
						pFloor->fFlyHeight = fHeight;
						pFloor->fAllOpenSpace = fAllFloorOpenSpace;
					}
					pBuilding->vecFloorData.push_back(pFloor);
				}
				// 根据楼层信息排序
				sort(pBuilding->vecFloorData.begin(), pBuilding->vecFloorData.end(), compare);
			}

			for (auto room : vecRoomValue)
			{
				string str = room["parent"]["nodeid"].asString();
				BuildingData* pBuilding = GetBuildingData(str);
				FloorData* pFloor = GetFloorData(str);
				if (!pFloor && !pBuilding)
				{
					continue;
				}
				string strMatchKey = room["nodeid"].asString();
				if (strMatchKey.empty())
				{
					continue;
				}
				vector<string> vec;
				SplitStringBySpecial(strMatchKey, vec, ",");
				if (pBuilding)
				{
					//获取每一层
					for (auto pBFloor : pBuilding->vecFloorData)
					{
						findRoomFromFloor(pBFloor, vec);
					}
				}
				else
				{
					findRoomFromFloor(pFloor, vec);
				}
			}

			std::vector<BuildingData*> vecBuilding;
			GetAllBuildingData(vecBuilding);

			//设置屋顶
			for (int i = 0; i < vecBuilding.size(); i++)
			{
				BuildingData* pBuilding = vecBuilding[i];
				for (int n = 0; n < pBuilding->vecFloorData.size(); n++)
				{
					if (n == pBuilding->vecFloorData.size() - 1)
					{
						string strFloorName = pBuilding->vecFloorData[n]->strFloorName;
						//char sz[1024] = { 0 };
						//AsciiToUtf8("屋顶", sizeof(sz), sz);
						std::string sz = "屋顶";
						if ((strFloorName != sz) && !pBuilding->vecFloorData[n]->bOldSortNum)
						{
							pBuilding->vecFloorData[n]->strFloorName = sz;
						}
					}
				}
			}
		}
	}
	catch (const std::exception&)
	{
	}
}

void BuildIndoorFactory::Callback_Request_Callback(ThreadCallbackParam* pParam)
{
	BuildIndoorFactory* pThis = (BuildIndoorFactory*)pParam->sRequestParam.mapParams["this"];
	char szBuffer[1024] = { 0 };
	if (pParam->nResult == STATUS_SUCCESS)
	{
		if (pThis->m_eServerVersion == EVersion::VERSION_V10_LESS)
		{
			pThis->SetV10LessData(pParam->strResponse);
		}
		else if (pThis->m_eServerVersion == EVersion::VERSION_V10)
		{
			pThis->SetV10Data(pParam->strResponse);
		}
	}
	pThis->iLoadedData();
}

void BuildIndoorFactory::findRoomFromFloor(FloorData* pFloor, vector<string> vecKeys)
{
	if (!pFloor || !pFloor->pNode)
	{
		return;
	}
	INode* pGroup = pFloor->pNode;
	vector<INode*> vecGeo;
	pGroup->iGetAllChildren(vecGeo);
	for (int z1 = 0; z1 < vecGeo.size(); z1++)
	{
		for (int z2 = 0; z2 < vecKeys.size(); z2++)
		{
			string strNodeID = vecGeo[z1]->iGetID();
			if (strstr(strNodeID.c_str(), vecKeys[z2].c_str()) != NULL)
			{
				// 具体房间信息解析
				RoomData* pRoom = iCreateRoomData();
				pRoom->pFloorData = pFloor;
				pRoom->pNode = vecGeo[z1];

				//截取房间号
				string strOld = strNodeID;
				string roomNum = Replace_all(strNodeID, vecKeys[z2], "");
				roomNum = Replace_all(roomNum, "_", "");
				//string strNum = StrToSubNumStr(roomNum);
				string strNum = iFilterRoomNum(roomNum);
				pRoom->strRoomNum = strNum;
				pRoom->strRoomNodeID = strOld;

				pFloor->vecRoomData.push_back(pRoom);
				IRenderNode* pIRender = pRoom->pNode->GetDynamicComponent<IRenderNode>();
				if (pIRender)
				{
					pRoom->pNode->GetDynamicComponent<IRenderNode>()->iSetCheckEnable(true);
					pRoom->pNode->GetDynamicComponent<IRenderNode>()->iSetHoverEnable(true);
					pRoom->pNode->GetDynamicComponent<IRenderNode>()->iSetClickEnable(true);
				}

				vecGeo[z1]->iSetExternParam((INT_PTR)pRoom);
			}
		}
	}
}

void BuildIndoorFactory::clearData()
{
	for (size_t i = 0; i < m_vecAreaSceneData.size(); i++)
	{
		DELETE_PTR(m_vecAreaSceneData[i]);
	}
	m_vecAreaSceneData.clear();

	m_pCurAreaSceneData = NULL;
	m_pCurBuildingGroupData = NULL;
	m_pCurBuildingData = NULL;
	m_pCurFloorData = NULL;
	m_strCurFloorID = "";
}

vector<AreaSceneData*> BuildIndoorFactory::iGetAreaSceneNode()
{
	vector<AreaSceneData*> vecBigSceneData;
	return vecBigSceneData;
}

vector<BuildingGroupData*> BuildIndoorFactory::iGetBuildingGroupPerAreaScene(string strAreaSceneID, AreaSceneData* pData)
{
	vector<BuildingGroupData*> vec;
	return vec;
}

vector<BuildingData*> BuildIndoorFactory::iGetBuildingPerBuildingGroup(string strBigSceneID, BuildingGroupData* pData)
{
	vector<BuildingData*> vec;
	return vec;
}

vector<FloorData*> BuildIndoorFactory::iGetFloorPerBuilding(string strBuildingID, BuildingData* pData)
{
	vector<FloorData*> vec;
	return vec;
}

vector<RoomData*> BuildIndoorFactory::iGetRoomPerFloor(string strFloorID, FloorData* pData)
{
	vector<RoomData*> vec;
	return vec;
}

void BuildIndoorFactory::iEnterAreaScene(AreaSceneData* pData)
{
	if (m_pBigSceneNode && m_bEnterAreaScene)
	{
		m_pBigSceneNode->iSetVisible(false);
	}
}

void BuildIndoorFactory::iChangeBuilding(BuildingData* pData)
{
}

void BuildIndoorFactory::iLeaveAreaScene(AreaSceneData* pData)
{
	if (m_pCurRoomData)
	{
		GetComponent<IRenderNode>(m_pCurRoomData->pNode)->iChangeColor(false);
	}
	m_pCurRoomData = nullptr;

	if (m_pBigSceneNode && m_bEnterAreaScene)
	{
		m_pBigSceneNode->iSetVisible(true);
	}
}

void BuildIndoorFactory::iChangeFloor(std::string strFloorID, FloorData* pData)
{
}

void BuildIndoorFactory::iFrameUpdate()
{
}

vector<INode*> BuildIndoorFactory::iGetAlphaNode(BuildingData* pData)
{
	vector<INode*> vecNode;
	vector<INode*> vecGeo;
	pData->pNode->iGetAllChildren(vecGeo);
	for (int i = 0; i < vecGeo.size(); i++)
	{
		INode* pGeo = vecGeo[i];
		string strID = pGeo->iGetID();
		string strSW = pData->strBuildingNodeID + m_strSWSuffix;
		string strSN = pData->strBuildingNodeID + m_strSNSuffix;
		if (strID.find(strSW) != string::npos ||
			strID.find(strSN) != string::npos)
		{
			vecNode.push_back(pGeo);
		}
	}
	return vecNode;
}

Vector4 BuildIndoorFactory::iGetRoomNormalColor()
{
	return Vector4(65.0f / 255.0f, 150.0f / 255.0f, 255.0f / 255.0f, 0.6);
}

Vector4 BuildIndoorFactory::iGetRoomWarningColor()
{
	return Vector4(255.0f / 255.0f, 86.0f / 255.0f, 86.0f / 255.0f, 0.8);
}

Vector4 BuildIndoorFactory::iGetRoomClickColor()
{
	return Vector4(110.0f / 255.0f, 247.0f / 255.0f, 247.0f / 255.0f, 0.8);
}

void BuildIndoorFactory::iOnRoomClick(INode* pRoomNode, RoomData* pRoomData)
{
}

