#pragma once

namespace bc
{
	//shp数据
	class ShpData
	{
	public:
		enum EShpType
		{
			SHP_POINT = 0,		//点
			SHP_LINE,			//线
			SHP_POLYGON,		//多边形
			SHP_MULTI_POINT,	//点集合
			SHP_MULTI_LINE,		//线集合
			SHP_MULTI_POLYGON,	//面集合
			SHP_GEOMETRY_COLLECTION,	//几何体集合
			SHP_NONE,	//NONE
		};

	public:
		std::map<std::string, std::string> mapAttr;			//属性
		EShpType eType;		//类型
		//SHP_POINT
		Vector3 vPoint;	
		//SHP_LINE
		std::vector<Vector3> vecLine;
		//SHP_POLYGON
		std::vector<Vector3> vecOuterRing;
		std::vector<std::vector<Vector3>> vecInteriorRing;
		//SHP_MULTI_POINT
		std::vector<Vector3> vecMultiPoint;
		//SHP_MULTI_LINE
		std::vector<std::vector<Vector3>> vecMultiLine;
		//SHP_MULTI_POLYGON
		std::vector<std::vector<Vector3>> vecMultiOuterRing;
		std::vector<std::vector<std::vector<Vector3>>> vecMultiInteriorRing;

		ShpData():
			eType(SHP_NONE)
		{

		}
	};

	class IMapAdapter
	{
	public:
		virtual void	iSetReferencePoint0(double lng, double lat, double height, const Vector3& vPoint) = 0;
		virtual void	iSetReferencePoint1(double lng, double lat, double height, const Vector3& vPoint) = 0;

		virtual void	iGetCoord(double lng, double lat, double height, Vector3& vPoint) = 0;
		virtual void	iGetCoordGJC(double lng, double lat, double height, Vector3& vPoint) = 0;
		virtual void	iGetCoordBD(double lng, double lat, double height, Vector3& vPoint) = 0;

		//UTM
		virtual void	iSetReferencePoint0_UTM(double lng, double lat, double height, const Vector3& vPoint) = 0;
		virtual void	iSetReferencePoint1_UTM(double lng, double lat, double height, const Vector3& vPoint) = 0;
		//根据世界坐标转经纬度（前提得设置参考点）
		virtual void	iGetCoord_UTM(double lng, double lat, double height, Vector3& vPos) = 0;
		virtual void	iGetLngLat_UTM(const Vector3& vPos, int zonenum, double& lng, double& lat) = 0;		
		virtual void	iGetCoordGCJ02_UTM(double lng, double lat, double height, Vector3& vPos) = 0;
		virtual void	iGetLngLatGCJ02_UTM(const Vector3& vPos, int zonenum, double& lng, double& lat) = 0;
	};

	class ILocationManager
	{
	public:
		virtual IMapAdapter*	iRegistMapAdapter(const std::string& strName) = 0;
	};
}
