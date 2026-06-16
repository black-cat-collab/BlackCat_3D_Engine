#pragma once

#include <limits>
#include <math.h>

// 取最大值
#ifndef MAX
#define MAX(a, b)		(((a) > (b)) ? (a) : (b))
#endif

// 取最小值
#ifndef MIN
#define MIN(a, b)		(((a) < (b)) ? (a) : (b))
#endif

#define EARTH_RADIUS 6378137.0

#ifndef M_PI
#define M_PI			3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2			1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4			0.785398163397448309616
#endif

#ifndef M_2_PI
#define M_2_PI			6.283185307179586476925
#endif

#ifndef D_TO_R
#define D_TO_R			0.0174532925199432957692
#endif

#ifndef R_TO_D
#define R_TO_D			57.295779513082320876798
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD(x)	( (x) * D_TO_R )
#endif

#ifndef RAD_TO_DEG
#define RAD_TO_DEG(x)	( (x) * R_TO_D )
#endif

#ifndef ANGLE_TO_SHORT
#define	ANGLE_TO_SHORT(x)	((int)((x)*65536.0/360.0) & 65535)
#endif

#ifndef SHORT_TO_ANGLE
#define	SHORT_TO_ANGLE(x)	((x)*(360.0/65536.0))
#endif

#ifndef FLIP_BYTE
#define	FLIP_BYTE(x)	x = (byte)(((byte)((byte)(x & 0x55) << 1)) | ((byte)((byte)(x & 0xAA) >> 1))); \
x = (byte)(((byte)((byte)(x & 0x33) << 2)) | ((byte)((byte)(x & 0xCC) >> 2))); \
x = (byte)(((byte)((byte)(x & 0x0F) << 4)) | ((byte)((byte)(x & 0xF0) >> 4))); 
#endif

#ifndef FLIP_USHORT
#define	FLIP_USHORT(x)	x = (ushort)(((ushort)((ushort)(x & 0x5555) << 1)) | ((ushort)((ushort)(x & 0xAAAA) >> 1))); \
x = (ushort)(((ushort)((ushort)(x & 0x3333) << 2)) | ((ushort)((ushort)(x & 0xCCCC) >> 2))); \
x = (ushort)(((ushort)((ushort)(x & 0x0F0F) << 4)) | ((ushort)((ushort)(x & 0xF0F0) >> 4))); \
x = (ushort)(((ushort)((ushort)(x & 0x00FF) << 8)) | ((ushort)((ushort)(x & 0xFF00) >> 8)));
#endif
#ifndef XYZ
#define  XYZ(xx) (xx.x,xx.y,xx.z)
#endif


#if defined(_MSC_VER)
inline bool isNaN(double v) { return _isnan(v) != 0; }
#elif defined(__ANDROID__)
inline bool isNaN(float v) { return isnan(v); }
inline bool isNaN(double v) { float vv = v;  return isnan(vv); }
#else
inline bool isNaN(float v) { return isnan(v); }
inline bool isNaN(double v) { return isnan(v); }
#endif

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

namespace MathUtils
{
	static bool FloatEuqal(float a, float b)
	{
		return fabs(a - b) < std::numeric_limits<float>::epsilon();
	}

	template<typename T>
	static T AlignUp(T value, T alignment)
	{
		T mask = alignment - 1;
		return (T)(value + mask & ~mask);
	}

	template<typename T>
	inline constexpr T Mix(T x, T y, T a) noexcept
	{
		return x * (T(1) - a) + y * a;
	}

	template <class T>
	constexpr T Clamp(const T& a, const T& _min, const T& _max)
	{
		return a < _min ? _min : a < _max ? a : _max;
	}
}

