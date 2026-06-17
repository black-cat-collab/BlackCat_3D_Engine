
#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ThreadWrapper.h"
#include "Project-Based/base/HeatmapFactory.h"
#include "Project-Based/Factory/TencentHeatMapFactory.h"

using namespace bc;

TencentHeatMapFactory::TencentHeatMapFactory(ISystemAPI *pSystemAPI) : HeatmapFactory(pSystemAPI)
{

}


TencentHeatMapFactory::~TencentHeatMapFactory()
{
	UninitializeThread();
}

//解析边界数据
void TencentHeatMapFactory::iParseBoundaryData(HeatmapItemData *pHeatmapItemData, std::string strResponce,
	std::vector<Vector3d> &outVecData)
{
	try
	{
		Json::CharReaderBuilder jsonBuilder;
		std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
		Json::Value root;
		std::string jsError;
		if (reader->parse(strResponce.c_str(),strResponce.c_str() + strResponce.length(), &root, &jsError))
		{
			std::string strBoundary = root["data"]["boundary"].asString();
			std::vector<std::string> vecLnglat;
			SplitStringBySpecial(strBoundary, vecLnglat, "|");
			for (int i = 0; i < vecLnglat.size(); i++)
			{
				std::vector<std::string> vec;
				SplitStringBySpecial(vecLnglat[i], vec, ",");
				if (vec.size() > 1)
				{
					double dlng, dlat;
					dlat = atof(vec[0].c_str());
					dlng = atof(vec[1].c_str());
					Vector3d vPos = iBoundaryLnglatToScenePos(pHeatmapItemData->strKey, dlng,dlat);
					outVecData.push_back(vPos);
				}
			}

			if (outVecData.size() > 1)
			{
				outVecData.push_back(outVecData[0]);
			}
		}
	}
	catch (const std::exception&)
	{

	}
}
//解析数据
void TencentHeatMapFactory::iParseHeatmapData(HeatmapItemData *pHeatmapItemData, std::string strResponce,
	std::vector<ExtraHeatmapData> &outVecData)
{
	
	try
	{
		do
		{
			Json::CharReaderBuilder jsonBuilder;
			std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
			Json::Value root;
			std::string jsError;
			if (reader->parse(strResponce.c_str(), strResponce.c_str() + strResponce.length(), &root,&jsError))
			{
				if (root.isMember("data"))
				{
					Json::Value val_items = root["data"];
					if (val_items.isNull())
					{
						break;
					}
	
					for (int i = 0; i < val_items.size(); i++)
					{
						//取最后一个
						if (i < val_items.size() - 1)
						{
							continue;
						}

						Json::Value points = val_items[i]["points"];
						for (int j = 0; j < points.size(); j++)
						{
							ExtraHeatmapData stData;
							stData.strKey = pHeatmapItemData->strKey;
							stData.strTime = val_items[i]["time"].asString();
							stData.lTime = StrTotimestamp((char*)stData.strTime.c_str());

							double dLng = points[j]["lng"].asFloat();
							double dLat = points[j]["lat"].asFloat();
							stData.vPos = iHeatmapLnglatToScenePos(pHeatmapItemData->strKey, dLng, dLat);
							stData.nWeight = points[j]["weight"].asFloat();
							outVecData.push_back(stData);
						}
					}
				}
			}
		} while (0);
	}
	catch (const std::exception&)
	{

	}
}

