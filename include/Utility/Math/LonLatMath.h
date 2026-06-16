#pragma once

#define EARTH_RADIUS  6378.137     //地球赤道半径(km)
#define EARTH_ARC     111.199      //地球每度的弧长(km)

namespace bc
{
	class LonLatMath
	{
	public:
		struct LonLatParam
		{
			Vector3 vKeyPoint1;
			Vector2 vLonLat1;

			Vector3 vKeyPoint2;
			Vector2 vLonLat2;

			double fCentralLng = -1.0;
			bool bDregree = false;

			LonLatParam()
			{
				vKeyPoint1 = { -1490.953857,-15566.719727, 0.0 };
				vLonLat1 = { 121.3569746097,28.6891950399 };

				vKeyPoint2 = { 8991.649414,-20762.507812, 0.0 };
				vLonLat2 = { 121.4641919708,28.6421974746 };

				//vKeyPoint1 = { -21185.679688, 6582.646484, 0.0 };
				//vLonLat1 = { 121.15128837974237,28.885435814516526 };

				//vKeyPoint2 = { -17927.441406, 4041.124512, 0.0 };
				//vLonLat2 = { 121.18508166796433, 28.862952371527605 };

				fCentralLng = 121.35;;
				bDregree = false;
			}
		};

		//wgs84与utm坐标转换
		const double wgs84k0 = 0.9996;

		const double wgs84e = 0.00669438;
		const double wgs84e2 = wgs84e * wgs84e;
		const double wgs84e3 = wgs84e2 * wgs84e;
		const double wgs84ep2 = wgs84e / (1 - wgs84e);

		const double wgs84se = sqrt(1 - wgs84e);
		const double wgs84_e = (1 - wgs84se) / (1 + wgs84se);
		const double wgs84_e2 = wgs84_e * wgs84_e;
		const double wgs84_e3 = wgs84_e2 * wgs84_e;
		const double wgs84_e4 = wgs84_e3 * wgs84_e;
		const double wgs84_e5 = wgs84_e4 * wgs84_e;

		const double wgs84_m1 = 1 - wgs84e / 4 - 3 * wgs84e2 / 64 - 5 * wgs84e3 / 256;
		const double wgs84_m2 = 3 * wgs84e / 8 + 3 * wgs84e2 / 32 + 45 * wgs84e3 / 1024;
		const double wgs84_m3 = 15 * wgs84e2 / 256 + 45 * wgs84e3 / 1024;
		const double wgs84_m4 = 35 * wgs84e3 / 3072;

		const double wgs84_p2 = 3.0 / 2 * wgs84_e - 27.0 / 32 * wgs84_e3 + 269.0 / 512 * wgs84_e5;
		const double wgs84_p3 = 21.0 / 16 * wgs84_e2 - 55.0 / 32 * wgs84_e4;
		const double wgs84_p4 = 151.0 / 96 * wgs84_e3 - 417.0 / 128 * wgs84_e5;
		const double wgs84_p5 = 1097.0 / 512 * wgs84_e4;

		double wgs84r = 6378137;

		void GetLatLng(const Vector3& input, Vector2& output);

		int32_t GetZoneNum(double lng, double lat, bool bDegree = false);

		double GetCentralLngByZoneNum(int ZoneNum, double fCentralLng = -1.0, bool bDregree = false);

		void WGS_TO_UTM(double lng, double lat, double& ax, double& ay, double fCentralLng = -1.0, bool bDregree = false);

		void GetCoord_UTM(double lng, double lat, double h, Vector3d* pPos, const LonLatParam& param = LonLatParam());
		
		void GetLngLat_UTM(Vector3d& pos, Vector3d* pOut);
	};
}

