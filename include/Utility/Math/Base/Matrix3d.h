#ifndef __TW_Matrix3d_D_H__
#define __TW_Matrix3d_D_H__
#pragma once

namespace bc
{
	class Matrix3d
	{
	public:
		union
		{
			double _m[9], __m[3][3];
		};
	public:
		Matrix3d()
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

		inline Matrix3d(const double fData[9])
		{
			memcpy(_m, fData, 9 * sizeof(double));
		}

		inline Matrix3d(const Matrix3d& rMatrix3d)
		{
			memcpy(_m, rMatrix3d._m, 9 * sizeof(double));
		}

		inline Matrix3d(const double f00, const double f01, const double f02,
			const double f10, const double f11, const double f12,
			const double f20, const double f21, const double f22)
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

		inline const double& operator () (const size_t& i, const size_t& j) const
		{
			assert(i < 3);
			assert(j < 3);

			return _m[i * 3 + j];

		}

		inline double& operator () (const size_t& i, const size_t& j)
		{
			assert(i < 3);
			assert(j < 3);

			return _m[i * 3 + j];
		}

		inline const double& operator [] (const size_t& i) const
		{
			assert(i < 9);

			return _m[i];
		}

		inline double& operator [] (const size_t& i)
		{
			assert(i < 9);

			return _m[i];
		}

		inline bool operator == (const Matrix3d& rMatrix3d) const
		{
			for (int i = 0; i < 9; ++i)
			{
				if (_m[i] != rMatrix3d._m[i])
					return false;
			}

			return true;
		}

		inline bool operator!= (const Matrix3d& rMatrix) const
		{
			return !operator==(rMatrix);
		}

		inline Matrix3d& operator = (const Matrix3d& rMatrix3d)
		{
			for (int i = 0; i < 9; ++i)
			{
				_m[i] = rMatrix3d._m[i];
			}
			return *this;
		}

		inline Matrix3d operator + (const Matrix3d& rMatrix3d)
		{
			Matrix3d m3;
			for (int i = 0; i < 9; ++i)
			{
				m3._m[i] = _m[i] + rMatrix3d._m[i];
			}

			return m3;
		}

		inline Matrix3d operator - (const Matrix3d& rMatrix3d)
		{
			Matrix3d m3;
			for (int i = 0; i < 9; ++i)
			{
				m3._m[i] = _m[i] - rMatrix3d._m[i];
			}

			return m3;
		}

		inline Matrix3d operator * (const Matrix3d& rMatrix3d)
		{
			Matrix3d m3;
			for (int i = 0; i < 9; ++i)
			{
				m3._m[i] = _m[i] * rMatrix3d._m[i];
			}

			return m3;
		}

		inline Vector3d operator * (const Vector3d& v) const
		{
			return Vector3d(
				_m[0] * v.x + _m[3] * v.y + _m[6] * v.z,
				_m[1] * v.x + _m[4] * v.y + _m[7] * v.z,
				_m[2] * v.x + _m[5] * v.y + _m[8] * v.z
			);
		}
		inline Vector3 operator * (const Vector3& v) const
		{
			return Vector3(
				_m[0] * v.x + _m[3] * v.y + _m[6] * v.z,
				_m[1] * v.x + _m[4] * v.y + _m[7] * v.z,
				_m[2] * v.x + _m[5] * v.y + _m[8] * v.z
			);
		}
		inline Matrix3d operator / (const Matrix3d& rMatrix3d)
		{
			Matrix3d m3;
			for (int i = 0; i < 9; ++i)
			{
				assert(_m[i] != 0.0f);
				assert(rMatrix3d._m[i] != 0.0f);
				m3._m[i] = _m[i] / rMatrix3d._m[i];
			}

			return m3;
		}

		inline Matrix3d Transpose() const
		{
			Matrix3d m3;
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

		Matrix3d Inverse() const;

		inline double* Ptr()
		{
			return _m;
		}

		inline Matrix3 Getf()
		{
			Matrix3 m3;
			for (int i = 0; i < 9; ++i)
			{
				m3._m[i] = _m[i];
			}
			return m3;
		}

		inline Matrix3 Getf() const
		{
			Matrix3 m3;
			for (int i = 0; i < 9; ++i)
			{
				m3._m[i] = _m[i];
			}
			return m3;
		}

		inline void SetScale(const Vector3d& v)
		{
			Matrix3d m = Matrix3d::IDENTITY;
			m._m[0] = v.x;
			m._m[4] = v.y;
			m._m[8] = v.z;

			*this = *this * m;
		}
		static Matrix3d RotationZ(const double& angleInRadians)
		{
			double s = std::sin(angleInRadians);
			double c = std::cos(angleInRadians);

			return Matrix3d // clang-format off
			{
					c,  s,  0,
					-s,  c,  0,
					0,  0,  1 // clang-format on
			};
		}

		static Matrix3d Mul(const Matrix3d& m1, const Matrix3d& m2)
		{
			Matrix3d mOut;
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
		inline const double* Ptr() const
		{
			return _m;
		}
	public:
		static const Matrix3d IDENTITY;
	};
}
#endif
