#ifndef _MATRIX4_D_H_
#define _MATRIX4_D_H_

#pragma  once

#include <assert.h>
#include <math.h>

#if (defined(__aarch64__) || defined(_M_ARM64)) || defined(__arm__)
#include "sse2neon.h"
#else
#include <xmmintrin.h>
#endif

#include "TVector3.h"
#include "TVector4.h"
#include "Matrix4.h"
#include "Utility/Math/Axisd.h"

namespace bc
{
	class Matrix4d
	{
		friend class Matrix4;
	private:

#ifdef __GNUC__
		double _m[16];

#else

		union __declspec(align(32))
		{
			double _m[16], __m[4][4];
			__m256d _sse[4];
		};
#endif

	public:
		inline Matrix4d()
		{
			memset(_m, 0, 16 * sizeof(double));
		}

		inline Matrix4d(const double fData[4][4])
		{
			memcpy(_m, fData, 16 * sizeof(double));
		}

		inline Matrix4d(const Matrix4d& rMatrix4)
		{
			memcpy(_m, rMatrix4._m, 16 * sizeof(double));
		}

		inline Matrix4d(const Matrix4& rMatrix4)
		{
			*this = rMatrix4;
		}
		inline Matrix4 Getf()
		{
#ifdef __GNUC__
			return Matrix4(_m[0],
				_m[1],
				_m[2],
				_m[3],

				_m[4],
				_m[5],
				_m[6],
				_m[7],

				_m[8],
				_m[9],
				_m[10],
				_m[11],

				_m[12],
				_m[13],
				_m[14],
				_m[15]);

#else

			return Matrix4(__m[0][0],
				__m[0][1],
				__m[0][2],
				__m[0][3],

				__m[1][0],
				__m[1][1],
				__m[1][2],
				__m[1][3],

				__m[2][0],
				__m[2][1],
				__m[2][2],
				__m[2][3],

				__m[3][0],
				__m[3][1],
				__m[3][2],
				__m[3][3]);
#endif

		}
		Matrix4d InverseMMX4();
		inline const Matrix4 Getf() const
		{
#ifdef __GNUC__
			return Matrix4(_m[0],
				_m[1],
				_m[2],
				_m[3],

				_m[4],
				_m[5],
				_m[6],
				_m[7],

				_m[8],
				_m[9],
				_m[10],
				_m[11],

				_m[12],
				_m[13],
				_m[14],
				_m[15]);

#else			
			return Matrix4(__m[0][0],
				__m[0][1],
				__m[0][2],
				__m[0][3],

				__m[1][0],
				__m[1][1],
				__m[1][2],
				__m[1][3],

				__m[2][0],
				__m[2][1],
				__m[2][2],
				__m[2][3],

				__m[3][0],
				__m[3][1],
				__m[3][2],
				__m[3][3]);
#endif				
		}

		inline Matrix4d(const Axisd& vAxis, const Vector3d& vPos)
		{
			SetAxis(vAxis);
			SetOrigin(vPos);
		}

		inline Matrix4d(const double f00, const double f01, const double f02, const double f03,
			const double f10, const double f11, const double f12, const double f13,
			const double f20, const double f21, const double f22, const double f23,
			const double f30, const double f31, const double f32, const double f33)
		{
			_m[0] = f00;
			_m[1] = f01;
			_m[2] = f02;
			_m[3] = f03;
			_m[4] = f10;
			_m[5] = f11;
			_m[6] = f12;
			_m[7] = f13;
			_m[8] = f20;
			_m[9] = f21;
			_m[10] = f22;
			_m[11] = f23;
			_m[12] = f30;
			_m[13] = f31;
			_m[14] = f32;
			_m[15] = f33;
		}


		static Matrix4d SetRotation(Vector3d axis, const double& angleInRadians)
		{
			axis.Normalise();

			auto sinAngle = std::sin(angleInRadians);
			auto cosAngle = std::cos(angleInRadians);
			auto oneMinusCosAngle = 1 - cosAngle;

			Matrix4d mat = Matrix4d::IDENTITY;

			mat._m[0] = 1 + oneMinusCosAngle * (axis.x * axis.x - 1);
			mat._m[1] = axis.z * sinAngle + oneMinusCosAngle * axis.x * axis.y;
			mat._m[2] = -axis.y * sinAngle + oneMinusCosAngle * axis.x * axis.z;
			mat._m[3] = 0;

			mat._m[4] = -axis.z * sinAngle + oneMinusCosAngle * axis.y * axis.x;
			mat._m[5] = 1 + oneMinusCosAngle * (axis.y * axis.y - 1);
			mat._m[6] = axis.x * sinAngle + oneMinusCosAngle * axis.y * axis.z;
			mat._m[7] = 0;

			mat._m[8] = axis.y * sinAngle + oneMinusCosAngle * axis.z * axis.x;
			mat._m[9] = -axis.x * sinAngle + oneMinusCosAngle * axis.z * axis.y;
			mat._m[10] = 1 + oneMinusCosAngle * (axis.z * axis.z - 1);
			mat._m[11] = 0;

			mat._m[12] = 0;
			mat._m[13] = 0;
			mat._m[14] = 0;
			mat._m[15] = 1;

			return mat;
		}

		inline const double& operator () (const size_t& i, const size_t& j) const
		{
			assert(i < 4);
			assert(j < 4);

			return _m[i * 4 + j];

		}

		inline double& operator () (const size_t& i, const size_t& j)
		{

			assert(i < 4);
			assert(j < 4);

			int index = i * 4 + j;


			return (double&)_m[index];
		}

		inline const double& operator [] (const size_t& i) const
		{
			assert(i < 16);
			return _m[i];
		}

		inline double& operator [] (const size_t& i)
		{
			assert(i < 16);

			return (double&)_m[i];
		}


		inline bool operator == (const Matrix4d& rMatrix4) const
		{
			if (
				_m[0] != rMatrix4._m[0] || _m[4] != rMatrix4._m[4] || _m[8] != rMatrix4._m[8] || _m[12] != rMatrix4._m[12] ||
				_m[1] != rMatrix4._m[1] || _m[5] != rMatrix4._m[5] || _m[9] != rMatrix4._m[9] || _m[13] != rMatrix4._m[13] ||
				_m[2] != rMatrix4._m[2] || _m[6] != rMatrix4._m[6] || _m[10] != rMatrix4._m[10] || _m[14] != rMatrix4._m[14] ||
				_m[3] != rMatrix4._m[3] || _m[7] != rMatrix4._m[7] || _m[11] != rMatrix4._m[11] || _m[15] != rMatrix4._m[15])
				return false;
			return true;
		}

		inline bool operator != (const Matrix4d& rMatrix4) const
		{
			return !((*this) == rMatrix4);
		}

		inline Matrix4d& operator = (const Matrix4d& rMatrix4)
		{
			memcpy(_m, rMatrix4._m, 16 * sizeof(double));
			return *this;
		}

		inline Matrix4d& operator = (const Matrix4& rMatrix4)
		{
			//memcpy(_m, rMatrix4._m, 16 * sizeof(double));
			_m[0] = rMatrix4[0];
			_m[1] = rMatrix4[1];
			_m[2] = rMatrix4[2];
			_m[3] = rMatrix4[3];
			_m[4] = rMatrix4[4];
			_m[5] = rMatrix4[5];
			_m[6] = rMatrix4[6];
			_m[7] = rMatrix4[7];
			_m[8] = rMatrix4[8];
			_m[9] = rMatrix4[9];
			_m[10] = rMatrix4[10];
			_m[11] = rMatrix4[11];
			_m[12] = rMatrix4[12];
			_m[13] = rMatrix4[13];
			_m[14] = rMatrix4[14];
			_m[15] = rMatrix4[15];
			return *this;
		}

		inline Matrix4d operator * (const double& fValue) const
		{
			Matrix4d m4;
			for (int i = 0; i < 16; ++i)
			{
				m4._m[i] = _m[i] * fValue;
			}

			return m4;
		}

		inline Vector3d operator * (const Vector3d& v) const
		{
			double w = _m[3] * v.x + _m[7] * v.y + _m[11] * v.z + _m[15];
			return Vector3d((v.x * _m[0] + v.y * _m[4] + v.z * _m[8] + _m[12]) / w,
				(v.x * _m[1] + v.y * _m[5] + v.z * _m[9] + _m[13]) / w,
				(v.x * _m[2] + v.y * _m[6] + v.z * _m[10] + _m[14]) / w);
		}

		inline Vector3 operator * (const Vector3& v) const
		{
			double w = _m[3] * v.x + _m[7] * v.y + _m[11] * v.z + _m[15];
			return Vector3((v.x * _m[0] + v.y * _m[4] + v.z * _m[8] + _m[12]) / w,
				(v.x * _m[1] + v.y * _m[5] + v.z * _m[9] + _m[13]) / w,
				(v.x * _m[2] + v.y * _m[6] + v.z * _m[10] + _m[14]) / w);
		}

		inline Vector4 operator * (const Vector4& v) const
		{
			return Vector4(
				_m[0] * v.x + _m[4] * v.y + _m[8] * v.z + _m[12] * v.w,
				_m[1] * v.x + _m[5] * v.y + _m[9] * v.z + _m[13] * v.w,
				_m[2] * v.x + _m[6] * v.y + _m[10] * v.z + _m[14] * v.w,
				_m[3] * v.x + _m[7] * v.y + _m[11] * v.z + _m[15] * v.w
			);
		}

		inline Vector4d operator * (const Vector4d& v) const
		{
			return Vector4d(
				_m[0] * v.x + _m[4] * v.y + _m[8] * v.z + _m[12] * v.w,
				_m[1] * v.x + _m[5] * v.y + _m[9] * v.z + _m[13] * v.w,
				_m[2] * v.x + _m[6] * v.y + _m[10] * v.z + _m[14] * v.w,
				_m[3] * v.x + _m[7] * v.y + _m[11] * v.z + _m[15] * v.w
			);
		}

		inline Matrix4d operator * (const Matrix4d& b) const
		{
			Matrix4d r;

#ifdef __GNUC__

			r._m[0] = _m[0] * b._m[0] + _m[4] * b._m[1] + _m[8] * b._m[2] + _m[12] * b._m[3];
			r._m[1] = _m[1] * b._m[0] + _m[5] * b._m[1] + _m[9] * b._m[2] + _m[13] * b._m[3];
			r._m[2] = _m[2] * b._m[0] + _m[6] * b._m[1] + _m[10] * b._m[2] + _m[14] * b._m[3];
			r._m[3] = _m[3] * b._m[0] + _m[7] * b._m[1] + _m[11] * b._m[2] + _m[15] * b._m[3];

			r._m[4] = _m[0] * b._m[4] + _m[4] * b._m[5] + _m[8] * b._m[6] + _m[12] * b._m[7];
			r._m[5] = _m[1] * b._m[4] + _m[5] * b._m[5] + _m[9] * b._m[6] + _m[13] * b._m[7];
			r._m[6] = _m[2] * b._m[4] + _m[6] * b._m[5] + _m[10] * b._m[6] + _m[14] * b._m[7];
			r._m[7] = _m[3] * b._m[4] + _m[7] * b._m[5] + _m[11] * b._m[6] + _m[15] * b._m[7];

			r._m[8] = _m[0] * b._m[8] + _m[4] * b._m[9] + _m[8] * b._m[10] + _m[12] * b._m[11];
			r._m[9] = _m[1] * b._m[8] + _m[5] * b._m[9] + _m[9] * b._m[10] + _m[13] * b._m[11];
			r._m[10] = _m[2] * b._m[8] + _m[6] * b._m[9] + _m[10] * b._m[10] + _m[14] * b._m[11];
			r._m[11] = _m[3] * b._m[8] + _m[7] * b._m[9] + _m[11] * b._m[10] + _m[15] * b._m[11];

			r._m[12] = _m[0] * b._m[12] + _m[4] * b._m[13] + _m[8] * b._m[14] + _m[12] * b._m[15];
			r._m[13] = _m[1] * b._m[12] + _m[5] * b._m[13] + _m[9] * b._m[14] + _m[13] * b._m[15];
			r._m[14] = _m[2] * b._m[12] + _m[6] * b._m[13] + _m[10] * b._m[14] + _m[14] * b._m[15];
			r._m[15] = _m[3] * b._m[12] + _m[7] * b._m[13] + _m[11] * b._m[14] + _m[15] * b._m[15];


#else


			for (size_t i = 0; i < 4; i++)
			{
				__m256d brod1 = _mm256_set1_pd(b[4 * i + 0]);
				__m256d brod2 = _mm256_set1_pd(b[4 * i + 1]);
				__m256d brod3 = _mm256_set1_pd(b[4 * i + 2]);
				__m256d brod4 = _mm256_set1_pd(b[4 * i + 3]);
				__m256d row = _mm256_add_pd(
					_mm256_add_pd(
						_mm256_mul_pd(brod1, _sse[0]),
						_mm256_mul_pd(brod2, _sse[1])),
					_mm256_add_pd(
						_mm256_mul_pd(brod3, _sse[2]),
						_mm256_mul_pd(brod4, _sse[3])));
				_mm256_store_pd(&r._m[4 * i], row);
			}
#endif

			return r;
		}

		inline Matrix4d Transpose() const
		{
			Matrix4d r;
			r._m[0] = _m[0];
			r._m[4] = _m[1];
			r._m[8] = _m[2];
			r._m[12] = _m[3];

			r._m[1] = _m[4];
			r._m[5] = _m[5];
			r._m[9] = _m[6];
			r._m[13] = _m[7];

			r._m[2] = _m[8];
			r._m[6] = _m[9];
			r._m[10] = _m[10];
			r._m[14] = _m[11];

			r._m[3] = _m[12];
			r._m[7] = _m[13];
			r._m[11] = _m[14];
			r._m[15] = _m[15];

			return r;
		}

		inline void MakeFrustum(double l, double r, double b, double t, double n, double f)
		{
			_m[0] = 2.0f * n / (r - l);
			_m[1] = 0.0;
			_m[2] = 0.0;
			_m[3] = 0.0;

			_m[4] = 0.0;
			_m[5] = 2.0f * n / (t - b);
			_m[6] = 0.0;
			_m[7] = 0.0;

			_m[8] = (r + l) / (r - l);
			_m[9] = (t + b) / (t - b);
			_m[10] = -(f + n) / (f - n);
			_m[11] = -1.0;


			_m[12] = 0.0;
			_m[13] = 0.0;
			_m[14] = -2.0 * f * n / (f - n);
			_m[15] = 0.0;
		}

		inline void MakeOrtho(double left, double right, double bottom, double top, double zNear, double zFar)
		{
			double tx = -(right + left) / (right - left);
			double ty = -(top + bottom) / (top - bottom);
			double tz = -(zFar + zNear) / (zFar - zNear);

			_m[0] = 2.0 / (right - left);
			_m[1] = 0;
			_m[2] = 0;
			_m[3] = 0;

			_m[4] = 0;
			_m[5] = 2.0 / (top - bottom);
			_m[6] = 0;
			_m[7] = 0;

			_m[8] = 0;
			_m[9] = 0;
			_m[10] = -2.0 / (zFar - zNear);
			_m[11] = 0;

			_m[12] = tx;
			_m[13] = ty;
			_m[14] = tz;
			_m[15] = 1;

		}
		inline void MakeTranslate(const Vector3d& v)
		{
			_m[12] = v.x;
			_m[13] = v.y;
			_m[14] = v.z;
		}

		inline void MakeTranslate(const Vector3& v)
		{
			_m[12] = v.x;
			_m[13] = v.y;
			_m[14] = v.z;
		}

		inline Vector3d GetScale() const
		{
			Vector3d vScale;
			vScale[0] = (double)sqrt((double)_m[0] * _m[0] + (double)_m[1] * _m[1] + (double)_m[2] * _m[2]);
			vScale[1] = (double)sqrt((double)_m[4] * _m[4] + (double)_m[5] * _m[5] + (double)_m[6] * _m[6]);
			vScale[2] = (double)sqrt((double)_m[8] * _m[8] + (double)_m[9] * _m[9] + (double)_m[10] * _m[10]);

			return vScale;
		}

		/*	inline Vector3d GetScaled() const
			{
				Vector3d vScale;
				vScale[0] = (double)sqrt((double)_m[0] * _m[0] + (double)_m[1] * _m[1] + (double)_m[2] * _m[2]);
				vScale[1] = (double)sqrt((double)_m[4] * _m[4] + (double)_m[5] * _m[5] + (double)_m[6] * _m[6]);
				vScale[2] = (double)sqrt((double)_m[8] * _m[8] + (double)_m[9] * _m[9] + (double)_m[10] * _m[10]);

				return vScale;
			}*/

		inline void SetScale(const Vector3& v)
		{
			Matrix4d m = Matrix4d::IDENTITY;
			m[0] = v.x;
			m[5] = v.y;
			m[10] = v.z;

			*this = (*this) * m;
		}

		inline void SetScale(const Vector3d& v)
		{
			Matrix4d m = Matrix4d::IDENTITY;
			m[0] = v.x;
			m[5] = v.y;
			m[10] = v.z;

			*this = (*this) * m;
		}
		inline void SetOrigin(const Vector3& vOrigin)
		{
			_m[12] = vOrigin.x, _m[13] = vOrigin.y, _m[14] = vOrigin.z, _m[15] = 1;
		}
		inline void SetOrigin(const Vector3d& vOrigin)
		{
			_m[12] = vOrigin.x, _m[13] = vOrigin.y, _m[14] = vOrigin.z, _m[15] = 1;
		}

		inline Vector3d GetOrigin() const
		{
			return Vector3d(_m[12], _m[13], _m[14]);
		}

		//inline Vector3d GetOrigind() const
		//{
		//	return Vector3d(_m[12], _m[13], _m[14]);
		//}

		//inline Vector3 GetOrigin() const
		//{
		//	return Vector3(_m[12], _m[13], _m[14]);
		//}

		inline void SetAxis(const size_t& i, const Vector3d& Axisd)
		{
			_m[i * 4] = Axisd.x;
			_m[i * 4 + 1] = Axisd.y;
			_m[i * 4 + 2] = Axisd.z;
		}
		inline void SetAxis(const size_t& i, const Vector3& Axis)
		{
			_m[i * 4] = Axis.x;
			_m[i * 4 + 1] = Axis.y;
			_m[i * 4 + 2] = Axis.z;
		}
		inline void SetAxis(const Axisd& vAxis)
		{
			for (int i = 0; i < 3; i++)
			{
				_m[i * 4] = vAxis[i].x;
				_m[i * 4 + 1] = vAxis[i].y;
				_m[i * 4 + 2] = vAxis[i].z;
				_m[i * 4 + 3] = 0;
			}
		}

		inline void SetAxis(const Axis& vAxis)
		{
			for (int i = 0; i < 3; i++)
			{
				_m[i * 4] = vAxis[i].x;
				_m[i * 4 + 1] = vAxis[i].y;
				_m[i * 4 + 2] = vAxis[i].z;
				_m[i * 4 + 3] = 0;
			}
		}

		inline Axisd GetAxis() const
		{
			Axisd Axis(_m[0], _m[1], _m[2],
				_m[4], _m[5], _m[6],
				_m[8], _m[9], _m[10]);

			return Axis;
		}

		//inline Axisd GetAxisd() const
		//{
		//	Axisd Axisd(_m[0], _m[1], _m[2],
		//		_m[4], _m[5], _m[6],
		//		_m[8], _m[9], _m[10]);

		//	return Axisd;
		//}

		Matrix4d Inverse() const;

		Matrix4d Inverse0()
		{

			Matrix4d r;

			double fDeterminant =
				_m[0] * (_m[5] * (_m[10] * _m[15] - _m[11] * _m[14]) -
					_m[9] * (_m[6] * _m[15] - _m[7] * _m[14]) +
					_m[13] * (_m[6] * _m[11] - _m[7] * _m[10])) -
				_m[4] * (_m[1] * (_m[10] * _m[15] - _m[11] * _m[14]) -
					_m[9] * (_m[2] * _m[15] - _m[3] * _m[14]) +
					_m[13] * (_m[2] * _m[11] - _m[3] * _m[10])) +
				_m[8] * (_m[1] * (_m[6] * _m[15] - _m[7] * _m[14]) -
					_m[5] * (_m[2] * _m[15] - _m[3] * _m[14]) +
					_m[13] * (_m[2] * _m[7] - _m[3] * _m[6])) -
				_m[12] * (_m[1] * (_m[6] * _m[11] - _m[7] * _m[10]) -
					_m[5] * (_m[2] * _m[11] - _m[3] * _m[10]) +
					_m[9] * (_m[2] * _m[7] - _m[3] * _m[6]));

			if (fDeterminant == 0.0f)
			{
				r._m[0] = 1; r._m[1] = 0;  r._m[2] = 0;  r._m[3] = 0;
				r._m[4] = 0; r._m[5] = 1;  r._m[6] = 0;  r._m[7] = 0;
				r._m[8] = 0; r._m[9] = 0;  r._m[10] = 1;  r._m[11] = 0;
				r._m[12] = 0; r._m[13] = 0;  r._m[14] = 0;  r._m[15] = 1;
				return r;
			}

			double rd = 1.0f / fDeterminant;

			r._m[0] = rd * (_m[5] * (_m[10] * _m[15] - _m[11] * _m[14]) -
				_m[9] * (_m[6] * _m[15] - _m[7] * _m[14]) +
				_m[13] * (_m[6] * _m[11] - _m[7] * _m[10]));
			r._m[1] = -rd * (_m[1] * (_m[10] * _m[15] - _m[11] * _m[14]) -
				_m[9] * (_m[2] * _m[15] - _m[3] * _m[14]) +
				_m[13] * (_m[2] * _m[11] - _m[3] * _m[10]));
			r._m[2] = rd * (_m[1] * (_m[6] * _m[15] - _m[7] * _m[14]) -
				_m[5] * (_m[2] * _m[15] - _m[3] * _m[14]) +
				_m[13] * (_m[2] * _m[7] - _m[3] * _m[6]));
			r._m[3] = -rd * (_m[1] * (_m[6] * _m[11] - _m[7] * _m[10]) -
				_m[5] * (_m[2] * _m[11] - _m[3] * _m[10]) +
				_m[9] * (_m[2] * _m[7] - _m[3] * _m[6]));

			r._m[4] = -rd * (_m[4] * (_m[10] * _m[15] - _m[11] * _m[14]) -
				_m[8] * (_m[6] * _m[15] - _m[7] * _m[14]) +
				_m[12] * (_m[6] * _m[11] - _m[7] * _m[10]));
			r._m[5] = rd * (_m[0] * (_m[10] * _m[15] - _m[11] * _m[14]) -
				_m[8] * (_m[2] * _m[15] - _m[3] * _m[14]) +
				_m[12] * (_m[2] * _m[11] - _m[3] * _m[10]));
			r._m[6] = -rd * (_m[0] * (_m[6] * _m[15] - _m[7] * _m[14]) -
				_m[4] * (_m[2] * _m[15] - _m[3] * _m[14]) +
				_m[12] * (_m[2] * _m[7] - _m[3] * _m[6]));
			r._m[7] = rd * (_m[0] * (_m[6] * _m[11] - _m[7] * _m[10]) -
				_m[4] * (_m[2] * _m[11] - _m[3] * _m[10]) +
				_m[8] * (_m[2] * _m[7] - _m[3] * _m[6]));

			r._m[8] = rd * (_m[4] * (_m[9] * _m[15] - _m[11] * _m[13]) -
				_m[8] * (_m[5] * _m[15] - _m[7] * _m[13]) +
				_m[12] * (_m[5] * _m[11] - _m[7] * _m[9]));
			r._m[9] = -rd * (_m[0] * (_m[9] * _m[15] - _m[11] * _m[13]) -
				_m[8] * (_m[1] * _m[15] - _m[3] * _m[13]) +
				_m[12] * (_m[1] * _m[11] - _m[3] * _m[9]));
			r._m[10] = rd * (_m[0] * (_m[5] * _m[15] - _m[7] * _m[13]) -
				_m[4] * (_m[1] * _m[15] - _m[3] * _m[13]) +
				_m[12] * (_m[1] * _m[7] - _m[3] * _m[5]));
			r._m[11] = -rd * (_m[0] * (_m[5] * _m[11] - _m[7] * _m[9]) -
				_m[4] * (_m[1] * _m[11] - _m[3] * _m[9]) +
				_m[8] * (_m[1] * _m[7] - _m[3] * _m[5]));

			r._m[12] = -rd * (_m[4] * (_m[9] * _m[14] - _m[10] * _m[13]) -
				_m[8] * (_m[5] * _m[14] - _m[6] * _m[13]) +
				_m[12] * (_m[5] * _m[10] - _m[6] * _m[9]));
			r._m[13] = rd * (_m[0] * (_m[9] * _m[14] - _m[10] * _m[13]) -
				_m[8] * (_m[1] * _m[14] - _m[2] * _m[13]) +
				_m[12] * (_m[1] * _m[10] - _m[2] * _m[9]));
			r._m[14] = -rd * (_m[0] * (_m[5] * _m[14] - _m[6] * _m[13]) -
				_m[4] * (_m[1] * _m[14] - _m[2] * _m[13]) +
				_m[12] * (_m[1] * _m[6] - _m[2] * _m[5]));
			r._m[15] = rd * (_m[0] * (_m[5] * _m[10] - _m[6] * _m[9]) -
				_m[4] * (_m[1] * _m[10] - _m[2] * _m[9]) +
				_m[8] * (_m[1] * _m[6] - _m[2] * _m[5]));

			return r;
		}

		void MakeAngles(const Vector3d& vAngles);
		void MakeAngles(const Vector3& vAngles);
		void SetAngles(const Vector3d& v);
		void SetAngles(const Vector3& v);
		Vector3d GetAngles() const;
		//Vector3d GetAnglesd() const;
		inline double* Ptr()
		{
			return (double*)_m;
		}

		inline const double* Ptr() const
		{
			return (double*)_m;
		}

	public:
		static const Matrix4d BIAS;
		static const Matrix4d F_TRANSFORM;
		static const Matrix4d F_TRANSFORM_INVERSE;
		static const Matrix4d IDENTITY;

	};
}
#endif
