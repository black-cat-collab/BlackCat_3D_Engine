#pragma once

namespace bc
{
	class HeatmapFactory;
	class PROJECT_BASED_API TencentHeatMapFactory : public HeatmapFactory
	{
	public:
		TencentHeatMapFactory(ISystemAPI *pSystemAPI);
		virtual ~TencentHeatMapFactory();

		//解析边界数据
		virtual void iParseBoundaryData(HeatmapItemData *pHeatmapItemData, std::string strResponce, std::vector<Vector3d> &outVecData);
		//解析边界数据
		virtual void iParseHeatmapData(HeatmapItemData *pHeatmapItemData, std::string strResponce, std::vector<ExtraHeatmapData> &outVecData);

	protected:
		virtual Vector3d iBoundaryLnglatToScenePos(std::string strKey,const double dlng,
			const double dlat) = 0;
		virtual Vector3d iHeatmapLnglatToScenePos(std::string strKey, const double dlng,
			const double dlat) = 0;

	};
}


