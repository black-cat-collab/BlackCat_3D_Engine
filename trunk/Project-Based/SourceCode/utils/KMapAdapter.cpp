#include "ProjectBasedHeader.h"
#include "Project-Based/utils/KMapAdapter.h"
#include <stdio.h>
#include <math.h>

using namespace bc;
using namespace std;



const double a = 6378245.0;              //长半轴
const double ee = 0.00669342162296594323;
const double x_pi = M_PI * 3000.0 / 180.0;


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

const double wgs84r = 6378137;
//wgs84与utm坐标转换

KMapAdapter::KMapAdapter()
{
	m_dx = 0;
	m_dy = 0;
	m_dz = 0;
	m_dRadius = 6378137.0;
	m_dF = 1/298.257223563;


	m_dMinLatitude = -85.05112878;
	m_dMaxLatitude = 85.05112878;

	m_dMinLongitude = -180;
    m_dMaxLongitude = 180;

	m_dScalex = 1;
	m_dScaley = 1;

	m_b3Degree = false;
	m_fCentralLng = -1.0f;
}

KMapAdapter::~KMapAdapter()
{

}

///////////////////
void KMapAdapter::lbxy(double l, double b, double *x, double *y, int l0)
{
	double sa,sb,sep,sn,sy2,st,sm,sx,hb;
	double xx,yy,hd,sd;

	//判断值的范围
	if (l > 360 || l < 0 || b > 360 || b < 0)
	{
		*x = l;
		*y = b;
		return;
	}

//	l  = l-l0;   //
	l = l - 121;   //
	sa = 6378245;   
	sb = 6356863.019;
	sep= 0.006738525415;
	hd = b*M_PI;
	hb = hd/180.0;
	st = tan(hb);

	sn=pow(sa,(double)2)/sqrt(pow(sa,(double)2)*pow(cos(hb),(double)2)
		+pow(sb,(double)2)*pow(sin(hb),(double)2));

	sy2=sep*pow(cos(hb),(double)2);
	sd = cos(hb)*l*M_PI;
	sm = sd/180.0;
	sx = 111134.861*b-(32005.78*sin(hb)+133.924*pow(sin(hb),(double)3)+0.697*pow(sin(hb),(double)5))*cos(hb);
	xx = sx+sn*st*(0.5*pow(sm,(double)2)+1.0/24.0*(5.0-pow(st,(double)2)+9.0*sy2)*pow(sm,(double)4));
	yy = sn*(sm+1.0/6.0*(1.0-pow(st,(double)2)+sy2)*pow(sm,(double)3)+1.0/120.0*(5.0-18.0*pow(st,(double)2)+pow(st,(double)4))*pow(sm,(double)5));
	*x = xx;
	*y = yy+500000;
}

///////////////
void KMapAdapter::CoordConvert(double l,double b,double h,double &x,double &y,double& z)
{
//	double x,y,z;
	double e2;//第一偏心率的平方
	double N;//卯酉圈半径
	e2=2*m_dF-m_dF*m_dF;
	N= m_dRadius/sqrt(1-e2*sin(b)*sin(b));

	x=(N+h)*cos(b)*cos(l);
	y=(N+h)*cos(b)*sin(l);
	z=(N*(1-e2)+h)*sin(b);
}
void KMapAdapter::SetReferencePoint0(double l,double b, double h,const Vector3d& vPoint)
{
	double x,y;
	lbxy(l,b,&x,&y,(int)l);
	m_dx = x - vPoint.y;
	m_dy = y - vPoint.x;
	m_dz = h - vPoint.z;
}

void KMapAdapter::SetReferencePoint1(double l, double b, double h, const Vector3d& vPoint)
{
	double x, y;
	lbxy(l, b, &x, &y, (int)l);
	m_dx = x - vPoint.y;
	m_dy = y - vPoint.x;

	m_dz = h - vPoint.z;
}
void KMapAdapter::SetReferencePoint2(double l, double b, double h, const Vector3d& vPoint)
{
	double x, y;
	lbxy(l, b, &x, &y, (int)l);
	m_dx = x - vPoint.y;
	m_dy = y - vPoint.x;

	m_dz = h - vPoint.z;

}
///////////
void KMapAdapter::GetCoord(double lng, double lat,double h, Vector3d *pPos)
{
	double x, y,z;

	z = h;
	lbxy(lng, lat, &x, &y, (int)lng);

	if (pPos)
	{
		pPos->y = x - m_dx;
		pPos->x = y - m_dy;
		pPos->z = h;
	}


}

void KMapAdapter::GetBaiduCoord(double lng, double lat, double h, Vector3d *pPos)
{
	Vector3d vecWgsPos;
	Vector3d vecPos;
	double x, y;

	BD09_to_GCJ02(lng, lat, &vecPos);
	GCJ02_to_WGS(vecPos.x, vecPos.y, x, y);

	//BD09_to_Mercator(x, y, &vecWgsPos);

	GetCoord(x, y, h, pPos);

}



void KMapAdapter::GetGCJCoord(double lng, double lat, double h,Vector3d *pPos)
{
	Vector3d vecWgsPos;
	double x, y;

	GCJ02_to_WGS(lng, lat, x, y);

	GetCoord(x, y, h, pPos);

}



bool KMapAdapter::outOfChina(double x, double y)
{
	if ((x < 72.004 || x > 137.8347) && (y < 0.8293 || y > 55.8271))
	{
		return true;
	}

	return false;
}



double KMapAdapter::transformLat(double x, double y)
{
	double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(x > 0 ? x : -x);
	ret += (20.0 * sin(6.0 * x * M_PI) + 20.0 *sin(2.0 * x * M_PI)) * 2.0 / 3.0;
	ret += (20.0 * sin(y * M_PI) + 40.0 * sin(y / 3.0 * M_PI)) * 2.0 / 3.0;
	ret += (160.0 * sin(y / 12.0 * M_PI) + 320 * sin(y * M_PI / 30.0)) * 2.0 / 3.0;
	return ret;
}

double KMapAdapter::transformLon(double x, double y)
{
	double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(x > 0 ? x : -x);
	ret += (20.0 * sin(6.0 * x * M_PI) + 20.0 * sin(2.0 * x * M_PI)) * 2.0 / 3.0;
	ret += (20.0 * sin(x * M_PI) + 40.0 * sin(x / 3.0 * M_PI)) * 2.0 / 3.0;
	ret += (150.0 * sin(x / 12.0 * M_PI) + 300.0 * sin(x / 30.0 * M_PI)) * 2.0 / 3.0;
	return ret;
}




void KMapAdapter::WGS_to_Mercator(double x, double y, Vector3d *pPos)
{
	double earthRad = 6378137.0;
	double a = y * M_PI / 180;

	pPos->x = x * M_PI / 180 * earthRad;
	pPos->y = earthRad / 2 * log((1.0 + sin(a)) / (1.0 - sin(a)));

}


void KMapAdapter::Mercator_to_WGS(double x, double y, Vector3d *pPos) 
{
	pPos->x = x / 20037508.34 * 180;
	pPos->y = y / 20037508.34 * 180;
	pPos->y = 180 / M_PI * (2 * atan(exp(pPos->y * M_PI / 180)) - M_PI / 2);

}
		

/*void KMapAdapter::WGS_to_GCJ02(double x,double y, Vector3d *pPos)
{
	if (!pPos)
	{
		return;
	}

	if (outOfChina(x, y))
	{
		pPos->x = x;
		pPos->y = y;
	}

	double dLat = transformLat(x - 105.0, y - 35.0);
	double dLon = transformLon(x - 105.0, y - 35.0);
	double radLat = y / 180.0 * M_PI;
	double magic = sin(radLat);
	magic = 1 - ee * magic * magic;
	double sqrtMagic = sqrt(magic);
	dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * M_PI);
	dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * M_PI);

	pPos->x = x + dLon;
	pPos->y = y + dLat;

}*/


void KMapAdapter::WGS_to_GCJ02(double x,double y, double &ax, double &ay)
{

	if (outOfChina(x, y))
	{
		ax = x;
		ay = y;
	}

	double dLat = transformLat(x - 105.0, y - 35.0);
	double dLon = transformLon(x - 105.0, y - 35.0);
	double radLat = y / 180.0 * M_PI;
	double magic = sin(radLat);
	magic = 1 - ee * magic * magic;
	double sqrtMagic = sqrt(magic);
	dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * M_PI);
	dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * M_PI);

	ax = x + dLon;
	ay = y + dLat;

}


///
///  Transform GCJ-02 to WGS-84
///  Reverse of transformFromWGSToGC() by iteration.
///
///  Created by Fengzee (fengzee@fengzee.me).
///
/*void KMapAdapter::GCJ02_to_WGS(double x,double y, Vector3d *pPos)
{
	Vector3d vecPos;
	Vector3d vecCurrPos;
	double dx, dy;

	vecPos.x = x;
	vecPos.y = y;

	while (1) {
		WGS_to_GCJ02(vecPos.x, vecPos.y,&vecCurrPos);
		dx = x - vecCurrPos.x;
		dy = y - vecCurrPos.y;
		if (fabs(dx) < 1e-7 && fabs(dy) < 1e-7) {  // 1e-7 ~ centimeter level accuracy
															   // Result of experiment:
															   //   Most of the time 2 iterations would be enough for an 1e-8 accuracy (milimeter level).
															   //
			*pPos = vecPos;
			return;
		}
		vecPos.x += dx;
		vecPos.y += dy;
	}

	*pPos = vecPos;

}*/

void KMapAdapter::GCJ02_to_WGS(double x, double y, double &ax, double &ay)
{
	//double dx, dy;
	//double tempx, tempy;
	//tempx = x;
	//tempy = y;

	//while (1) {
	//	WGS_to_GCJ02(tempx, tempy, ax, ay);
	//	dx = x - ax;
	//	dy = y - ay;
	//	if (fabs(dx) < 1e-7 && fabs(dy) < 1e-7) {  // 1e-7 ~ centimeter level accuracy
	//											   // Result of experiment:
	//											   //   Most of the time 2 iterations would be enough for an 1e-8 accuracy (milimeter level).
	//											   //

	//		return;
	//	}
	//	tempx += dx;
	//	tempy += dy;
	//}

	if (outOfChina(x, y)) {
		ax = x;
		ay = y;
	}
	double dLat = transformLat(x - 105.0, y - 35.0);
	double dLon = transformLon(x - 105.0, y - 35.0);
	double radLat = y / 180.0 * M_PI;
	double magic = sin(radLat);
	magic = 1 - ee * magic * magic;
	double sqrtMagic = sqrt(magic);
	dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * M_PI);
	dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * M_PI);
	double mgLat = y + dLat;
	double mgLon = x + dLon;

	double lontitude = x * 2 - mgLon;
	double latitude = y * 2 - mgLat;

	ax = lontitude;
	ay = latitude;

}

void KMapAdapter::WGS_to_UTM(double lng, double lat, double &ax, double &ay)
{
	double lat_rad = lat * M_PI / 180;
	double lat_sin = sin(lat_rad);
	double lat_cos = cos(lat_rad);
	double lat_tan = lat_sin / lat_cos;
	double lat_tan2 = lat_tan * lat_tan; 
	double lat_tan4 = lat_tan2 * lat_tan2;

	int zone_num = GetZoneNum(lng, lat);
	double lng_rad = lng * M_PI / 180;
	double central_lng = GetCentralLngByZoneNum(zone_num);
	double central_lng_rad = central_lng * M_PI / 180;

	double n = wgs84r / sqrt(1 - wgs84e * pow(lat_sin, 2));
	double c = wgs84ep2 * pow(lat_cos, 2);

	double a = lat_cos * (lng_rad - central_lng_rad);
	double a2 = a * a;
	double a3 = a2 * a;
	double a4 = a3 * a;
	double a5 = a4 * a;
	double a6 = a5 * a;

	double m = wgs84r * (wgs84_m1 * lat_rad -
		wgs84_m2 * sin(2 * lat_rad) +
		wgs84_m3 * sin(4 * lat_rad) -
		wgs84_m4 * sin(6 * lat_rad));

	ax = wgs84k0 * n * (a +
		a3 / 6 * (1 - lat_tan2 + c) +
		a5 / 120 * (5 - 18 * lat_tan2 + lat_tan4 + 72 * c - 58 * wgs84ep2)) + 500000;

	ay = wgs84k0 * (m + n*lat_tan*(a2 / 2 +
		a4 / 24 * (5 - lat_tan2 + 9 * c + 4 * pow(c, 2)) +
		a6 / 720 * (61 - 58 * lat_tan2 + lat_tan4 + 600 * c - 330 * wgs84ep2)));
}

void KMapAdapter::UTM_to_WGS(double easting, double northing, int zonenum, double &lng, double &lat)
{
	double x = easting - 500000;
	double y = northing;

	double m = y / wgs84k0;
	double mu = m / (wgs84r * wgs84_m1);

	double p_rad = (mu +
		wgs84_p2 * sin(2 * mu) +
		wgs84_p3 * sin(4 * mu) +
		wgs84_p4 * sin(6 * mu) +
		wgs84_p5 * sin(8 * mu));
	
	double p_sin = sin(p_rad);
	double p_sin2 = p_sin * p_sin;

	double p_cos = cos(p_rad);

	double p_tan = p_sin / p_cos;
	double p_tan2 = p_tan * p_tan;
	double p_tan4 = p_tan2 * p_tan2;

	double ep_sin = 1 - wgs84e * p_sin2;
	double ep_sin_sqrt = sqrt(1 - wgs84e * p_sin2);

	double n = wgs84r / ep_sin_sqrt;
	double r = (1 - wgs84e) / ep_sin;

	double c = wgs84_e * pow(p_cos, 2);
	double c2 = c * c;

	double d = x / (n * wgs84k0);
	double d2 = d * d;
	double d3 = d2 * d;
	double d4 = d3 * d;
	double d5 = d4 * d;
	double d6 = d5 * d;

	lat = (p_rad - (p_tan / r) *
		(d2 / 2 -
		d4 / 24 * (5 + 3 * p_tan2 + 10 * c - 4 * c2 - 9 * wgs84ep2)) +
		d6 / 720 * (61 + 90 * p_tan2 + 298 * c + 45 * p_tan4 - 252 * wgs84ep2 - 3 * c2)) * 180 / M_PI;

	lng = (d -
		d3 / 6 * (1 + 2 * p_tan2 + c) +
		d5 / 120 * (5 - 2 * c + 28 * p_tan2 - 3 * c2 + 8 * wgs84ep2 + 24 * p_tan4)) / p_cos * 180 / M_PI + GetCentralLngByZoneNum(zonenum);
}

int	 KMapAdapter::GetZoneNum(double lng, double lat)
{
	if (m_b3Degree)
	{
		//3度投影带
		int nZone = floor((lng - 1.5f) / 3.0f) + 1;
		return nZone;
	}

	if (56 <= lat && 64 > lat && 3 <= lng && 12 > lat)
	{
		return 32;
	}
	if (72 <= lat && 84 >= lat && lng >= 0)
	{
		if (lng < 9)
		{
			return 31;
		}
		else if (lng < 21)
		{
			return 33;
		}
		else if (lng < 33)
		{
			return 35;
		}
		else if (lng < 42)
		{
			return 37;
		}
	}
	return (int)((lng + 180) / 6 + 1);  
}

double KMapAdapter::GetCentralLngByZoneNum(int ZoneNum)
{
	if (m_fCentralLng != -1.0f)
	{
		return m_fCentralLng;
	}
	else if (m_b3Degree)
	{
		double dResult = 3 * ZoneNum; 
		return dResult;
	}
	return (ZoneNum - 1) * 6 - 180 + 3;
}

void KMapAdapter::SetReferencePoint0_UTM(double l, double b, double h, const Vector3d& vPoint)
{
	double x, y;
	WGS_to_UTM(l, b, x, y);
	m_refer0localx = vPoint.x;
	m_refer0localy = vPoint.y;
	m_refer0utmx = x;
	m_refer0utmy = y;
	m_dx = x - vPoint.x;
	m_dy = y - vPoint.y;
	m_dz = h - vPoint.z;
}

void KMapAdapter::SetReferencePoint1_UTM(double l, double b, double h, const Vector3d& vPoint)
{
	double x, y;
	WGS_to_UTM(l, b, x, y);
	m_dScalex = (vPoint.x - m_refer0localx) / (x - m_refer0utmx);
	m_dScaley = (vPoint.y - m_refer0localy) / (y - m_refer0utmy);
}

void   KMapAdapter::GetPointUTM_LngLa(Vector3d *pPos, int zonenum, double &lng, double &lat)
{
	if (pPos)
	{
		double utmX = (pPos->x - m_refer0localx) / m_dScalex + m_refer0utmx;
		double utmY = (pPos->y - m_refer0localy) / m_dScaley + m_refer0utmy;
		UTM_to_WGS(utmX, utmY, zonenum, lng, lat);
	}
}

void KMapAdapter::GetCoord_UTM(double lng, double lat, double h, Vector3d *pPos)
{
	double x, y, z;

	z = h;
	WGS_to_UTM(lng, lat, x, y);

	if (pPos)
	{
		//pPos->x = (x - m_dx) * m_dScalex;
		//pPos->y = (y - m_dy) * m_dScaley;
		pPos->x = (x - m_refer0utmx) * m_dScalex + m_refer0localx;
		pPos->y = (y - m_refer0utmy) * m_dScaley + m_refer0localy;
		pPos->z = h;
	}
}

void KMapAdapter::GetCoord_Relative(double lng, double lat, double z, Vector3d* pPos)
{
	if (pPos)
	{
		pPos->x = (lng - m_refer0utmx) * m_dScalex + m_refer0localx;
		pPos->y = (lat - m_refer0utmy) * m_dScaley + m_refer0localy;
		pPos->z = z;
	}
}


void KMapAdapter::GCJ02_to_BD09(double x,double y, Vector3d *pPos)
{

	double z = sqrt(x * x + y * y) + 0.00002 * sin(y * x_pi);
	double theta = atan2(y, x) + 0.000003 * cos(x * x_pi);
	if (pPos)
	{
		pPos->x = z * cos(theta) + 0.0065;
		pPos->y = z * sin(theta) + 0.006;
	}

}



void KMapAdapter::BD09_to_GCJ02(double x, double y, Vector3d *pPos)
{
	double x1 = x - 0.0065, y1 = y - 0.006;
	double z = sqrt(x1 * x1 + y1 * y1) - 0.00002 * sin(y1 * x_pi);
	double theta = atan2(y1, x1) - 0.000003 * cos(x1 * x_pi);

	if (pPos)
	{
		pPos->x = z * cos(theta);
		pPos->y = z * sin(theta);
	}

}



void KMapAdapter::GCJ02_to_Mercator(double x, double y, Vector3d *pPos)
{
	//Vector3d vecPos;
	double dx, dy;

	GCJ02_to_WGS(x, y, dx, dy);

	WGS_to_Mercator(dx, dy, pPos);

}

void KMapAdapter::Mercator_to_GCJ02(double x, double y, Vector3d *pPos)
{
	Vector3d vecPos;
	double dx, dy;
	
	Mercator_to_WGS(x, y, &vecPos);

	WGS_to_GCJ02(vecPos.x, vecPos.y, dx, dy);
	pPos->x = dx;
	pPos->y = dy;

}


static double MCBAND[] = { 12890594.86, 8362377.87, 5591021, 3481989.83, 1678043.12, 0 };

static double MC2LL[6][10] = {
	{1.410526172116255e-8, 0.00000898305509648872, -1.9939833816331, 200.9824383106796, -187.2403703815547, 91.6087516669843, -23.38765649603339, 2.57121317296198, -0.03801003308653, 17337981.2},
	{-7.435856389565537e-9, 0.000008983055097726239, -0.78625201886289, 96.32687599759846, -1.85204757529826, -59.36935905485877, 47.40033549296737, -16.50741931063887, 2.28786674699375, 10260144.86},
	{-3.030883460898826e-8, 0.00000898305509983578, 0.30071316287616, 59.74293618442277, 7.357984074871, -25.38371002664745, 13.45380521110908, -3.29883767235584, 0.32710905363475, 6856817.37},
	{-1.981981304930552e-8, 0.000008983055099779535, 0.03278182852591, 40.31678527705744, 0.65659298677277, -4.44255534477492, 0.85341911805263, 0.12923347998204, -0.04625736007561, 4482777.06},
	{3.09191371068437e-9, 0.000008983055096812155, 0.00006995724062, 23.10934304144901, -0.00023663490511, -0.6321817810242, -0.00663494467273, 0.03430082397953, -0.00466043876332, 2555164.4},
	{2.890871144776878e-9, 0.000008983055095805407, -3.068298e-8, 7.47137025468032, -0.00000353937994, -0.02145144861037, -0.00001234426596, 0.00010322952773, -0.00000323890364, 826088.5}
};

void KMapAdapter::Mercator_to_BD09(double x,double y, Vector3d *pPos)
{
	int k=-1;

	for (int i = 0; i<6; i++) 
	{
		if (y >= MCBAND[i]) 
		{
			k = i;
			break;
		}
	}
	if (k >= 0)
	{
		double cB = y / MC2LL[k][9];

		pPos->x = MC2LL[k][0] + MC2LL[k][1] * x;
		pPos->y = MC2LL[k][2] + MC2LL[k][3] * cB + MC2LL[k][4] * cB * cB +
			MC2LL[k][5] * cB * cB * cB + MC2LL[k][6] * cB * cB * cB * cB +
			MC2LL[k][7] * cB * cB * cB * cB * cB +
			MC2LL[k][8] * cB * cB * cB * cB * cB * cB;
	}

}


static double LLBAND[6] = { 75, 60, 45, 30, 15, 0 };

static double LL2MC[6][10] = {
	{-0.0015702102444, 111320.7020616939, 1704480524535203, -10338987376042340, 26112667856603880, -35149669176653700, 26595700718403920, -10725012454188240, 1800819912950474, 82.5},
	{0.0008277824516172526, 111320.7020463578, 647795574.6671607, -4082003173.641316, 10774905663.51142, -15171875531.51559, 12053065338.62167, -5124939663.577472, 913311935.9512032, 67.5},
	{0.00337398766765, 111320.7020202162, 4481351.045890365, -23393751.19931662, 79682215.47186455, -115964993.2797253, 97236711.15602145, -43661946.33752821, 8477230.501135234, 52.5},
	{0.00220636496208, 111320.7020209128, 51751.86112841131, 3796837.749470245, 992013.7397791013, -1221952.21711287, 1340652.697009075, -620943.6990984312, 144416.9293806241, 37.5},
	{-0.0003441963504368392, 111320.7020576856, 278.2353980772752, 2485758.690035394, 6070.750963243378, 54821.18345352118, 9540.606633304236, -2710.55326746645, 1405.483844121726, 22.5},
	{-0.0003218135878613132, 111320.7020701615, 0.00369383431289, 823725.6402795718, 0.46104986909093, 2351.343141331292, 1.58060784298199, 8.77738589078284, 0.37238884252424, 7.45}
};


void KMapAdapter::BD09_to_Mercator(double x, double y, Vector3d *pPos)
{
	int m = -1;
	int t;
	for (int i= 0; i < 6; i++) {
		if (y >= LLBAND[i]) {
			m = i;
			break;
		}
	}

	if (m < 0)
	{
		for (t = 5; t >= 0; t--) {
			if (y <= -LLBAND[t]) {
				m = t;
				break;
			}
		}
	}
	

	if (m >= 0)
	{
		double cB = y / LL2MC[m][9];
		pPos->x = LL2MC[m][0] + LL2MC[m][1] * x;
		pPos->y = LL2MC[m][2] + LL2MC[m][3] * cB + LL2MC[m][4] * cB * cB +
			LL2MC[m][5] * cB * cB * cB + LL2MC[m][6] * cB * cB * cB * cB +
			LL2MC[m][7] * cB * cB * cB * cB * cB +
			LL2MC[m][8] * cB * cB * cB * cB * cB * cB;
	}


}
double KMapAdapter::Clip(double n, double minValue, double maxValue)
{
	return min(max(n, minValue), maxValue);

}
double KMapAdapter::GetGroundResolution(double lat, int nLevel)
{
	double latitude = Clip(lat, m_dMinLatitude, m_dMaxLatitude);

	return cos(latitude * M_PI / 180) * 2 * M_PI * m_dRadius / (256 << nLevel);
}

double KMapAdapter::GetMapScale(double lat, int nLevel, int nScreenDpi)
{
	return GetGroundResolution(lat, nLevel) * nScreenDpi / 0.0254;
}

void KMapAdapter::CGCS2000_to_WGS(double Y,double X, float fCentralMeridian,double& outy, double& outx)
{
	double L0 = fCentralMeridian;//中央子午线需根据实际情况设置
	double lat, lon;
	Y -= 500000;
	double iPI = 0.0174532925199433;//pi/180
	double a = 6378137.0; //长半轴 m
	double b = 6356752.31414; //短半轴 m
	double f = 1 / 298.257222101;//扁率 a-b/a
	double e = 0.0818191910428; //第一偏心率 Math.sqrt(5)
	double ee = sqrt(a * a - b * b) / b; //第二偏心率
	double bf = 0; //底点纬度
	double a0 = 1 + (3 * e * e / 4) + (45 * e * e * e * e / 64) + (175 * e * e * e * e * e * e / 256) + (11025 * e * e * e * e * e * e * e * e / 16384) + (43659 * e * e * e * e * e * e * e * e * e * e / 65536);
	double b0 = X / (a * (1 - e * e) * a0);
	double c1 = 3 * e * e / 8 + 3 * e * e * e * e / 16 + 213 * e * e * e * e * e * e / 2048 + 255 * e * e * e * e * e * e * e * e / 4096;
	double c2 = 21 * e * e * e * e / 256 + 21 * e * e * e * e * e * e / 256 + 533 * e * e * e * e * e * e * e * e / 8192;
	double c3 = 151 * e * e * e * e * e * e * e * e / 6144 + 151 * e * e * e * e * e * e * e * e / 4096;
	double c4 = 1097 * e * e * e * e * e * e * e * e / 131072;
	bf = b0 + c1 * sin(2 * b0) + c2 * sin(4 * b0) + c3 * sin(6 * b0) + c4 * sin(8 * b0); // bf =b0+c1*sin2b0 + c2*sin4b0 + c3*sin6b0 +c4*sin8b0 +...
	double tf = tan(bf);
	double n2 = ee * ee * cos(bf) * cos(bf); //第二偏心率平方成bf余弦平方
	double c = a * a / b;
	double v = sqrt(1 + ee * ee * cos(bf) * cos(bf));
	double mf = c / (v * v * v); //子午圈半径
	double nf = c / v;//卯酉圈半径

	//纬度计算
	lat = bf - (tf / (2 * mf) * Y) * (Y / nf) * (1 - 1 / 12 * (5 + 3 * tf * tf + n2 - 9 * n2 * tf * tf) * (Y * Y / (nf * nf)) + 1 / 360 * (61 + 90 * tf * tf + 45 * tf * tf * tf * tf) * (Y * Y * Y * Y / (nf * nf * nf * nf)));
	//经度偏差
	lon = 1 / (nf * cos(bf)) * Y - (1 / (6 * nf * nf * nf * cos(bf))) * (1 + 2 * tf * tf + n2) * Y * Y * Y + (1 / (120 * nf * nf * nf * nf * nf * cos(bf))) * (5 + 28 * tf * tf + 24 * tf * tf * tf * tf) * Y * Y * Y * Y * Y;
	outy = lat / iPI;
	outx = L0 + lon / iPI;
}

void KMapAdapter::WGS_to_CGCS2000(double longitude, double latitude, float fCentralMeridian, double& outx, double& outy)
{
	double longitude1, latitude1, longitude0, X0, Y0, xval, yval;
	//NN曲率半径，测量学里面用N表示
	//M为子午线弧长，测量学里用大X表示
	//fai为底点纬度，由子午弧长反算公式得到，测量学里用Bf表示
	//R为底点所对的曲率半径，测量学里用Nf表示
	double a, f, e2, ee, NN, T, C, A, M, iPI;
	iPI = 0.0174532925199433; //3.1415926535898/180.0;
	a = 6378137.0; f = 1 / 298.257222101; //CGCS2000坐标系参数
	//a=6378137.0; f=1/298.2572236; //wgs84坐标系参数
	longitude0 = fCentralMeridian;//中央子午线 根据实际进行配置
	longitude0 = longitude0 * iPI;//中央子午线转换为弧度
	longitude1 = longitude * iPI; //经度转换为弧度
	latitude1 = latitude * iPI; //纬度转换为弧度
	e2 = 2 * f - f * f;
	ee = e2 * (1.0 - e2);
	NN = a / sqrt(1.0 - e2 * sin(latitude1) * sin(latitude1));
	T = tan(latitude1) * tan(latitude1);
	C = ee * cos(latitude1) * cos(latitude1);
	A = (longitude1 - longitude0) * cos(latitude1);
	M = a * ((1 - e2 / 4 - 3 * e2 * e2 / 64 - 5 * e2 * e2 * e2 / 256) * latitude1 - (3 * e2 / 8 + 3 * e2 * e2 / 32 + 45 * e2 * e2
		* e2 / 1024) * sin(2 * latitude1)
		+ (15 * e2 * e2 / 256 + 45 * e2 * e2 * e2 / 1024) * sin(4 * latitude1) - (35 * e2 * e2 * e2 / 3072) * sin(6 * latitude1));
	xval = NN * (A + (1 - T + C) * A * A * A / 6 + (5 - 18 * T + T * T + 72 * C - 58 * ee) * A * A * A * A * A / 120);
	yval = M + NN * tan(latitude1) * (A * A / 2 + (5 - T + 9 * C + 4 * C * C) * A * A * A * A / 24
		+ (61 - 58 * T + T * T + 600 * C - 330 * ee) * A * A * A * A * A * A / 720);
	X0 = 500000L;
	Y0 = 0;
	xval = xval + X0; yval = yval + Y0;

	//转换为投影
	outx = xval;
	outy = yval;
}

