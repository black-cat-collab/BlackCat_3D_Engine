#pragma once

#include <assert.h>
#include <cmath>
#include <string>

#ifdef __GNUC__
#define		__forceinline	__inline__
#endif
#if (defined(__aarch64__) || defined(_M_ARM64)) || defined(__arm__)
#include "sse2neon.h"
#else
#include <xmmintrin.h>
#endif


#ifdef _MSC_VER
#include <intrin.h>
#endif


#include "TVector3.h"
#include "TVector4.h"
#include "Utility/Math/Axis.h"


namespace bc
{
	class Matrix4
	{
	private:

#ifdef __GNUC__

#pragma pack (16)
		float _m[16], __m[4][4];
		__m128 _sse[4];
#pragma pack ()

#else

		union __declspec(align(16))
		{
			float _m[16], __m[4][4];
			__m128 _sse[4];
		};
#endif

	public:
		inline Matrix4()
		{
			memset(_m, 0, 16 * sizeof(float));
		}

		inline Matrix4(const float fData[4][4])
		{
			memcpy(_m, fData, 16 * sizeof(float));
		}

		inline Matrix4(const Matrix4& rMatrix4)
		{
			memcpy(_m, rMatrix4._m, 16 * sizeof(float));
		}

		inline Matrix4(const Axis& vAxis, const Vector3& vPos)
		{
			SetAxis(vAxis);
			SetOrigin(vPos);
		}

		/*	inline Matrix4(const float& f00, const float& f01, const float& f02, const float& f03,
				const float& f10, const float& f11, const float& f12, const float& f13,
				const float& f20, const float& f21, const float& f22, const float& f23,
				const float& f30, const float& f31, const float& f32, const float& f33)
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
			*/

		inline Matrix4(const float f00, const float f01, const float f02, const float f03,
			const float f10, const float f11, const float f12, const float f13,
			const float f20, const float f21, const float f22, const float f23,
			const float f30, const float f31, const float f32, const float f33)
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




		inline const float& operator () (const size_t& i, const size_t& j) const
		{
			assert(i < 4);
			assert(j < 4);

			return _m[i * 4 + j];

		}

		inline float& operator () (const size_t& i, const size_t& j)
		{
			assert(i < 4);
			assert(j < 4);

			return _m[i * 4 + j];
		}

		inline const float& operator [] (const size_t& i) const
		{
			assert(i < 16);
			return _m[i];
		}

		inline float& operator [] (const size_t& i)
		{
			assert(i < 16);
			return _m[i];
		}

		inline bool operator == (const Matrix4& rMatrix4) const
		{
			if (
				_m[0] != rMatrix4._m[0] || _m[4] != rMatrix4._m[4] || _m[8] != rMatrix4._m[8] || _m[12] != rMatrix4._m[12] ||
				_m[1] != rMatrix4._m[1] || _m[5] != rMatrix4._m[5] || _m[9] != rMatrix4._m[9] || _m[13] != rMatrix4._m[13] ||
				_m[2] != rMatrix4._m[2] || _m[6] != rMatrix4._m[6] || _m[10] != rMatrix4._m[10] || _m[14] != rMatrix4._m[14] ||
				_m[3] != rMatrix4._m[3] || _m[7] != rMatrix4._m[7] || _m[11] != rMatrix4._m[11] || _m[15] != rMatrix4._m[15])
				return false;
			return true;
		}

		inline bool operator != (const Matrix4& rMatrix4) const
		{
			return !((*this) == rMatrix4);
		}

		inline Matrix4& operator = (const Matrix4& rMatrix4)
		{
			memcpy(_m, rMatrix4._m, 16 * sizeof(float));
			return *this;
		}

		inline Matrix4 operator * (const float& fValue) const
		{
			Matrix4 m4;
			for (int i = 0; i < 16; ++i)
			{
				m4._m[i] = _m[i] * fValue;
			}

			return m4;
		}

		inline Vector3 operator * (const Vector3& v) const
		{
			float w = _m[3] * v.x + _m[7] * v.y + _m[11] * v.z + _m[15];
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

		inline Matrix4 operator * (const Matrix4& b) const
		{
			Matrix4 r;

			for (size_t i = 0; i < 4; i++)
			{
				__m128 brod1 = _mm_set1_ps(b[4 * i + 0]);
				__m128 brod2 = _mm_set1_ps(b[4 * i + 1]);
				__m128 brod3 = _mm_set1_ps(b[4 * i + 2]);
				__m128 brod4 = _mm_set1_ps(b[4 * i + 3]);
				__m128 row = _mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(brod1, _sse[0]),
						_mm_mul_ps(brod2, _sse[1])),
					_mm_add_ps(
						_mm_mul_ps(brod3, _sse[2]),
						_mm_mul_ps(brod4, _sse[3])));
				_mm_store_ps(&r._m[4 * i], row);
			}

			return r;
		}

		inline Matrix4 Transpose() const
		{
			Matrix4 r(*this);

			_MM_TRANSPOSE4_PS(r._sse[0], r._sse[1], r._sse[2], r._sse[3]);

			return r;
		}

		static Matrix4 SetRotation(Vector3 axis, float angleInRadians)
		{
			axis.Normalise();

			auto sinAngle = std::sin(angleInRadians);
			auto cosAngle = std::cos(angleInRadians);
			auto oneMinusCosAngle = 1 - cosAngle;

			Matrix4 mat = Matrix4::IDENTITY;

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

		inline void MakeFrustum(float l, float r, float b, float t, float n, float f)
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
		inline void MakeTranslate(const Vector3& v)
		{
			_m[12] = v.x;
			_m[13] = v.y;
			_m[14] = v.z;
		}

		inline Vector3 GetScale() const
		{
			Vector3 vScale;
			vScale[0] = (float)sqrt((double)_m[0] * _m[0] + (double)_m[1] * _m[1] + (double)_m[2] * _m[2]);
			vScale[1] = (float)sqrt((double)_m[4] * _m[4] + (double)_m[5] * _m[5] + (double)_m[6] * _m[6]);
			vScale[2] = (float)sqrt((double)_m[8] * _m[8] + (double)_m[9] * _m[9] + (double)_m[10] * _m[10]);

			return vScale;
		}


		inline void SetScale(const Vector3& v)
		{
			Matrix4 m = Matrix4::IDENTITY;
			m[0] = v.x;
			m[5] = v.y;
			m[10] = v.z;

			*this = (*this) * m;
		}

		inline void SetOrigin(const Vector3& vOrigin)
		{
			_m[12] = vOrigin.x, _m[13] = vOrigin.y, _m[14] = vOrigin.z, _m[15] = 1;
		}

		inline Vector3 GetOrigin() const
		{
			return Vector3(_m[12], _m[13], _m[14]);
		}

		inline void SetAxis(const size_t& i, const Vector3& Axis)
		{
			_m[i * 4] = Axis.x;
			_m[i * 4 + 1] = Axis.y;
			_m[i * 4 + 2] = Axis.z;
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

		inline Axis GetAxis() const
		{
			Axis Axis(_m[0], _m[1], _m[2],
				_m[4], _m[5], _m[6],
				_m[8], _m[9], _m[10]);

			return Axis;
		}

		Matrix4 Inverse() const;
		void MakeAngles(const Vector3& vAngles);
		void SetAngles(const Vector3& v);
		Vector3 GetAngles() const;

		inline float* Ptr()
		{
			return _m;
		}

		inline const float* Ptr() const
		{
			return _m;
		}

	public:
		static const Matrix4 BIAS;
		static const Matrix4 F_TRANSFORM;
		static const Matrix4 F_TRANSFORM_INVERSE;
		static const Matrix4 IDENTITY;

	};

#ifdef _MSC_VER
	static_assert(sizeof(Matrix4) == (16 * sizeof(float)), "Error size of Matrix4");
#endif

}
