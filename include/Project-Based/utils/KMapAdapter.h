#pragma once



#define RADIUS_EARTH      (6378137)
#define MAX_LATITUDE      (85.0511287798)

#ifndef M_PI
#define M_PI	3.14159265358979323846f
#endif



namespace bc
{
	/*typedef struct {
		double lng;
		double lat;
	} Location;*/


	class PROJECT_BASED_API KMapAdapter
	{
	public:
		KMapAdapter();
		~KMapAdapter();

		void GetCoord(double lng, double lat, double z, Vector3d *pPos);
		void GetBaiduCoord(double lng, double lat, double z, Vector3d *pPos);
		void GetGCJCoord(double lng, double lat, double z, Vector3d *pPos);


		void SetReferencePoint0(double l, double b, double h, const Vector3d& vPoint);
		void SetReferencePoint1(double l, double b, double h, const Vector3d& vPoint);
		void SetReferencePoint2(double l, double b, double h, const Vector3d& vPoint);

		void   WGS_to_Mercator(double x, double y, Vector3d *pPos);
		void   Mercator_to_WGS(double x, double y, Vector3d *pPos);


		void   BD09_to_Mercator(double x, double y, Vector3d *pPos);
		void   Mercator_to_BD09(double x, double y, Vector3d *pPos);

		void   GCJ02_to_Mercator(double x, double y, Vector3d *pPos);
		void   Mercator_to_GCJ02(double x, double y, Vector3d *pPos);

		void   BD09_to_GCJ02(double x, double y, Vector3d *pPos);
		void   GCJ02_to_BD09(double x, double y, Vector3d *pPos);
		//void   WGS_to_GCJ02(double x, double y, Vector3d *pPos);
		void   WGS_to_GCJ02(double x, double y, double &ax, double &ay);

		//void   GCJ02_to_WGS(double x, double y, Vector3d *pPos);
		void   GCJ02_to_WGS(double x, double y, double &ax, double &ay);

		void   CGCS2000_to_WGS(double x, double y, float fCentralMeridian, double& outx, double& outy);
		void   WGS_to_CGCS2000(double x, double y, float fCentralMeridian,double& outx, double& outy);


		double GetGroundResolution(double lat, int nLevel);           //地面分辨率
		double GetMapScale(double lat, int nLevel, int nScreenDpi);   //地图比例尺

		//UTM
		void   SetReferencePoint0_UTM(double l, double b, double h, const Vector3d& vPoint);
		void   SetReferencePoint1_UTM(double l, double b, double h, const Vector3d& vPoint);
		void   WGS_to_UTM(double lng, double lat, double &ax, double &ay);
		void   UTM_to_WGS(double easting, double northing, int zonenum, double &lng, double &lat);
		int	   GetZoneNum(double lng, double lat);
		double GetCentralLngByZoneNum(int ZoneNum);
		void   GetCoord_UTM(double lng, double lat, double z, Vector3d *pPos);
		void   GetCoord_Relative(double lng, double lat, double z, Vector3d* pPos);
		void   GetPointUTM_LngLa(Vector3d *pPos, int zonenum, double &lng,double &lat);		//根据世界坐标转经纬度（前提得设置参考点）

		void   SetCentralLng(double dLng) { m_fCentralLng = dLng; } 
		void   SetUtm3Degree(bool b3Degree) { m_b3Degree = b3Degree; }
	protected:

	private:
		


		double transformLat(double x, double y);
		double transformLon(double x, double y);
		bool   outOfChina(double lat, double lon);


		void CoordConvert(double l, double b, double h, double &x, double &y, double& z);
		void lbxy(double l, double b, double *x, double *y, int l0);

		double Clip(double n, double minValue, double maxValue);

		double  m_dRadius;//地球半径
		double  m_dF;//扁率


		double m_dx, m_dy, m_dz;
		double m_dx1, m_dy1, m_dz1;
		double m_refer0localx, m_refer0localy;
		double m_refer0utmx, m_refer0utmy;
		double m_dScalex, m_dScaley;
		float fAngles;

		double m_dMinLatitude;
		double m_dMaxLatitude;
		double m_dMinLongitude;
		double m_dMaxLongitude;
		
		bool   m_b3Degree;		//是否3度投影带，默认是6度投影带
		float  m_fCentralLng;		//自定义中央子午线

	};

}




