#pragma once

#include <assert.h>
#include <math.h>
#include <string>

namespace bc
{
	class Matrix3
	{
	public:
		union
		{
			float _m[9], __m[3][3];
		};
	public:
		inline Matrix3()
		{
			_m[0] = 0.0f;
			_m[1] = 0.0f;
			_m[2] = 0.0f;
			_m[3] = 0.0f;
			_m[4] = 0.0f;
			_m[5] = 0.0f;
			_m[6] = 0.0f;
			_m[7] = 0.0f;
			_m[8] = 0.0f;
		}

		inline Matrix3(const float fData[9])
		{
			memcpy(_m, fData, 9 * sizeof(float));
		}

		inline Matrix3(const Matrix3& rMatrix3)
		{
			memcpy(_m, rMatrix3._m, 9 * sizeof(float));
		}

		inline Matrix3(const float f00, const float f01, const float f02,
			const float f10, const float f11, const float f12,
			const float f20, const float f21, const float f22)
		{
			_m[0] = f00;
			_m[1] = f01;
			_m[2] = f02;
			_m[3] = f10;
			_m[4] = f11;
			_m[5] = f12;
			_m[6] = f20;
			_m[7] = f21;
			_m[8] = f22;
		}

		inline const float& operator () (const size_t& i, const size_t& j) const
		{
			assert(i < 3);
			assert(j < 3);

			return _m[i * 3 + j];

		}

		inline float& operator () (const size_t& i, const size_t& j)
		{
			assert(i < 3);
			assert(j < 3);

			return _m[i * 3 + j];
		}

		inline const float& operator [] (const size_t& i) const
		{
			assert(i < 9);

			return _m[i];
		}

		inline float& operator [] (const size_t& i)
		{
			assert(i < 9);

			return _m[i];
		}

		inline bool operator == (const Matrix3& rMatrix3) const
		{
			for (int i = 0; i < 9; ++i)
			{
				if (_m[i] != rMatrix3._m[i])
					return false;
			}

			return true;
		}

		inline bool operator!= (const Matrix3& rMatrix) const
		{
			return !operator==(rMatrix);
		}

		inline Matrix3& operator = (const Matrix3& rMatrix3)
		{
			for (int i = 0; i < 9; ++i)
			{
				_m[i] = rMatrix3._m[i];
			}
			return *this;
		}

		inline Matrix3 operator + (const Matrix3& rMatrix3)
		{
			Matrix3 m3;
			for (int i = 0; i < 9; ++i)
			{
				m3._m[i] = _m[i] + rMatrix3._m[i];
			}

			return m3;
		}

		inline Matrix3 operator - (const Matrix3& rMatrix3)
		{
			Matrix3 m3;
			for (int i = 0; i < 9; ++i)
			{
				m3._m[i] = _m[i] - rMatrix3._m[i];
			}

			return m3;
		}

		Matrix3& operator*(const Matrix3& right)
		{
			*this = Mul(*this, right);
			return *this;
		}

		Matrix3& operator*=(const Matrix3& right)
		{
			*this = Mul(*this, right);
			return *this;
		}

		inline Matrix3 Transpose() const
		{
			Matrix3 m3;
			m3._m[0] = _m[0];
			m3._m[1] = _m[3];
			m3._m[2] = _m[6];

			m3._m[3] = _m[1];
			m3._m[4] = _m[4];
			m3._m[5] = _m[7];

			m3._m[6] = _m[2];
			m3._m[7] = _m[5];
			m3._m[8] = _m[8];

			return m3;
		}

		Matrix3 Inverse() const;

		inline float* Ptr()
		{
			return _m;
		}

		inline const float* Ptr() const
		{
			return _m;
		}

		inline Vector3 operator * (const Vector3& v) const
		{
			return Vector3(
				_m[0] * v.x + _m[3] * v.y + _m[6] * v.z,
				_m[1] * v.x + _m[4] * v.y + _m[7] * v.z,
				_m[2] * v.x + _m[5] * v.y + _m[8] * v.z
			);
		}

		inline void SetScale(const Vector3& v)
		{
			Matrix3 m = Matrix3::IDENTITY;
			m[0] = v.x;
			m[4] = v.y;
			m[8] = v.z;

			*this = Mul(*this, m);
		}

		static Matrix3 RotationZ(float angleInRadians)
		{
			auto s = std::sin(angleInRadians);
			auto c = std::cos(angleInRadians);

			return Matrix3 // clang-format off
			{
					c,  s,  0,
					-s,  c,  0,
					0,  0,  1 // clang-format on
			};
		}

		static Matrix3 Mul(const Matrix3& m1, const Matrix3& m2)
		{
			Matrix3 mOut;
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					for (int k = 0; k < 3; k++)
					{
						mOut.__m[i][j] += m1.__m[k][j] * m2.__m[i][k];
					}
				}
			}

			return mOut;
		}

	public:
		static const Matrix3 IDENTITY;
	};

	static_assert(sizeof(Matrix3) == (9 * sizeof(float)), "Error size of Matrix3");
}
