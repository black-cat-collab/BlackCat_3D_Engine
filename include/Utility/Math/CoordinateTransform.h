#pragma once

namespace bc
{
	class CoordinateTransform
	{
	public:
		static void WGS84_To_CGCS2000(const double& lng, const double& lat, const double& fCentralMeridian, double& x, double& y);
		static void CGCS2000_To_WGS84(double Y, double X, float fCentralMeridian, double& outy, double& outx);
		static void WGS84_To_WebMercator(double lng, double lat, double& x, double& y);
		static void WebMercator_To_WGS84(double x, double y, double& lng, double& lat);

		static double GetDistanceByLatLng(float lat1, float lng1, float lat2, float lng2);
	};
}
