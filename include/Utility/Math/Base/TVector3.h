#pragma once

#include <cstdint>
#include <cmath>

#include "Utility/Math/MathUtils.h"

namespace bc
{
	template <typename T>
	struct TVector3
	{
		union
		{
			struct { T x, y, z; };
			struct { T r, g, b; };
			struct { T s, t, p; };
		};

		constexpr TVector3(const TVector3&) = default;
		constexpr TVector3(TVector3&&) = default;


		constexpr TVector3() noexcept : x{ 0 }, y{ 0 }, z{ 0 } {}
		constexpr explicit TVector3(T v) noexcept : x{ v }, y{ v }, z{ v } {}
		constexpr TVector3(T fx, T fy, T fz) noexcept : x{ fx }, y{ fy }, z{ fz } {}

		template <typename Y>
		constexpr TVector3(Y fx, Y fy, Y fz) noexcept : x{ T(fx) }, y{ T(fy) }, z{ T(fz) } {}

		template <typename Y>
		constexpr TVector3(const TVector3<Y>& right)
		{
			x = static_cast<T>(right.x);
			y = static_cast<T>(right.y);
			z = static_cast<T>(right.z);
		}

		T* Data() { return reinterpret_cast<T*>(this); }

		const T* Data() const { return reinterpret_cast<const T*>(this); }

		T* Ptr() { return reinterpret_cast<T*>(this); }

		const T* Ptr() const { return reinterpret_cast<const T*>(this); }

		T& operator[](size_t index)
		{
			return Data()[index];
		}

		const T& operator[](size_t index) const
		{
			return Data()[index];
		}

		constexpr TVector3& operator=(const TVector3& right)
		{
			x = right.x;
			y = right.y;
			z = right.z;
			return *this;
		};

		constexpr TVector3& operator=(TVector3&& right)
		{
			x = right.x;
			y = right.y;
			z = right.z;
			return *this;
		}

		TVector3& operator=(const T& right)
		{
			x = right;
			y = right;
			z = right;
			return *this;
		};

		constexpr bool operator==(const TVector3& right) const
		{
			return x == right.x && y == right.y && z == right.z;
		}

		constexpr bool operator!=(const TVector3& right) const
		{
			return !(*this == right);
		}

		constexpr TVector3 operator+(const TVector3<T>& right) const
		{
			return TVector3{ x + right.x, y + right.y, z + right.z };
		}

		TVector3& operator+=(const TVector3<T>& right)
		{
			x += right.x;
			y += right.y;
			z += right.z;
			return *this;
		}

		TVector3& operator+=(const T& right)
		{
			x += right;
			y += right;
			z += right;
			return *this;
		}

		constexpr TVector3 operator-(const TVector3<T>& right) const
		{
			return TVector3{ x - right.x, y - right.y, z - right.z };
		}

		TVector3& operator-=(const TVector3<T>& right)
		{
			x -= right.x;
			y -= right.y;
			z -= right.z;
			return *this;
		}

		TVector3& operator-=(const T& right)
		{
			x -= right;
			y -= right;
			z -= right;
			return *this;
		}

		constexpr TVector3 operator-() const
		{
			return TVector3{ -x, -y, -z };
		}

		constexpr TVector3 operator*(T s) const
		{
			return TVector3{ x * s, y * s, z * s };
		}

		constexpr TVector3 operator*(const TVector3& right) const
		{
			return TVector3{ x * right.x, y * right.y, z * right.z };
		}

		TVector3& operator*=(const TVector3& right)
		{
			x *= right.x;
			y *= right.y;
			z *= right.z;
			return *this;
		}

		TVector3& operator*=(T s)
		{
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}

		friend TVector3 operator * (const T fScalar, const TVector3& rkVector)
		{
			return TVector3(
				fScalar * rkVector.x,
				fScalar * rkVector.y,
				fScalar * rkVector.z);
		}

		constexpr TVector3 operator/(const TVector3& right) const
		{
			return TVector3{ x / right.x, y / right.y, z / right.z };
		}

		constexpr TVector3& operator/=(const TVector3& right)
		{
			x /= right.x;
			y /= right.y;
			z /= right.z;
			return *this;
		}

		constexpr TVector3 operator^(const TVector3& right) const
		{
			return TVector3(
				y * right.z - z * right.y,
				z * right.x - x * right.z,
				x * right.y - y * right.x);
		}

		constexpr TVector3& operator^=(const TVector3& right)
		{
			return TVector3(
				y * right.z - z * right.y,
				z * right.x - x * right.z,
				x * right.y - y * right.x);
		}

		constexpr TVector3 operator/(T s) const
		{
			return TVector3{ x / s, y / s, z / s };
		}

		TVector3& operator/=(T s)
		{
			x /= s;
			y /= s;
			z /= s;
			return *this;
		}

		void Set(const float& fx, const float& fy, const float& fz)
		{
			x = fx;
			y = fy;
			z = fz;
		}


		constexpr T Length() const
		{
			return std::sqrt(x * x + y * y + z * z);
		}

		constexpr T Length2D() const
		{
			return std::sqrt(x * x + y * y);
		}

		constexpr T SquaredLength() const
		{
			return x * x + y * y + z * z;
		}

		constexpr bool Empty() const
		{
			T sqlen = SquaredLength();
			return (sqlen < (1e-06));
		}

		constexpr bool Valid() const { return !(isnan(x) != 0 || isnan(y) != 0 || isnan(z) != 0); }

		constexpr T Normalise()
		{
			T fLength = Length();

			// Will also work for zero-sized vectors, but will change nothing
			if (fLength > 1e-08)
			{
				T fInvLength = 1.0 / fLength;
				x *= fInvLength;
				y *= fInvLength;
				z *= fInvLength;
			}

			return fLength;
		}

		constexpr TVector3 Normalise() const
		{
			TVector3 result = *this;
			result.Normalise();
			return result;
		}

		constexpr T Distance(const TVector3& rhs) const
		{
			return (*this - rhs).Length();
		}

		constexpr T  Distance2D(const TVector3& rhs) const
		{
			return (*this - rhs).Length2D();
		}

		constexpr T DotProduct(const TVector3& rVector) const
		{
			return x * rVector.x + y * rVector.y + z * rVector.z;
		}

		constexpr TVector3 CrossProduct(const TVector3& rVector) const
		{
			return TVector3(
				y * rVector.z - z * rVector.y,
				z * rVector.x - x * rVector.z,
				x * rVector.y - y * rVector.x);
		}

		constexpr float AngleBetween(const TVector3& rVector) const
		{
			T dotProduct = DotProduct(rVector);

			T vectorsMagnitude = Length() * rVector.Length();

			double angle = acos(dotProduct / vectorsMagnitude);

			if (isnan(angle))
			{
				return 0;
			}

			return angle;
		}

		TVector3 MoveForward(const TVector3& v, const T& fDis)
		{
			return  (*this + v * fDis);
		}

		constexpr TVector3 MoveForward(const TVector3& v, const T& fDis) const
		{
			return  (*this + v * fDis);
		}

		void FromAngles(const TVector3& vForward, const TVector3& vRight, const TVector3& vUp)
		{
			T fAngle;
			T sp, sy, cr, cp;

			sp = -vForward[2];
			fAngle = (T)asin(sp);
			(*this)[0] = fAngle * 360.0f / (M_PI * 2.0f);

			cp = (float)cos(fAngle);
			sy = vForward[1] / cp;
			fAngle = (T)asin(sy);
			(*this)[1] = fAngle * 360.0f / (M_PI * 2);

			cr = vUp[2] / cp;
			fAngle = (T)acos(cr);
			(*this)[2] = fAngle * 360 / (M_PI * 2);
		}

		void ToAngles(TVector3& vForward, TVector3& vRight, TVector3& vUp)
		{
			T fAngle;
			fAngle = (*this)[1] * (M_PI * 2 / 360);
			T sy = (T)sin(fAngle);
			T cy = (T)cos(fAngle);

			fAngle = (*this)[0] * (M_PI * 2 / 360);
			T sp = (T)sin(fAngle);
			T cp = (T)cos(fAngle);

			fAngle = (*this)[2] * (M_PI * 2 / 360);
			T sr = (T)sin(fAngle);
			T cr = (T)cos(fAngle);

			vForward[0] = cp * cy;
			vForward[1] = cp * sy;
			vForward[2] = -sp;

			vRight[0] = (-1 * sr * sp * cy + -1 * cr * -sy);
			vRight[1] = (-1 * sr * sp * sy + -1 * cr * cy);
			vRight[2] = -1 * sr * cp;

			vUp[0] = (cr * sp * cy + -sr * -sy);
			vUp[1] = (cr * sp * sy + -sr * cy);
			vUp[2] = cr * cp;
		}

		T DegreeAngleBetween(const TVector3& v1)
		{
			TVector3 vCorss = CrossProduct(v1);
			vCorss.Normalise();
			T radian_angle = atan2(vCorss.Length(), DotProduct(v1));
			if (vCorss.z < 0)
			{
				radian_angle = 2 * M_PI - radian_angle;
			}
			return radian_angle * 180 / M_PI;
		}

		static const TVector3<T> ZERO;
		static const TVector3<T> UNIT_X;
		static const TVector3<T> UNIT_Y;
		static const TVector3<T> UNIT_Z;
		static const TVector3<T> NEGATIVE_UNIT_X;
		static const TVector3<T> NEGATIVE_UNIT_Y;
		static const TVector3<T> NEGATIVE_UNIT_Z;
		static const TVector3<T> UNIT_SCALE;
	};

	using Vector3 = TVector3<float>;
	using Vector3d = TVector3<double>;
	using Vector3i = TVector3<int32_t>;
	using int3 = TVector3<int32_t>;
	using U8int3 = TVector3<uint8_t>;

	template <typename T>
	const TVector3<T> TVector3<T>::ZERO = TVector3<T>(0);
	template <typename T>
	const TVector3<T> TVector3<T>::UNIT_X = TVector3<T>(1, 0, 0);
	template <typename T>
	const TVector3<T> TVector3<T>::UNIT_Y = TVector3<T>(0, 1, 0);
	template <typename T>
	const TVector3<T> TVector3<T>::UNIT_Z = TVector3<T>(0, 0, 1);
	template <typename T>
	const TVector3<T> TVector3<T>::NEGATIVE_UNIT_X = TVector3<T>(-1, 0, 0);
	template <typename T>
	const TVector3<T> TVector3<T>::NEGATIVE_UNIT_Y = TVector3<T>(0, -1, 0);
	template <typename T>
	const TVector3<T> TVector3<T>::NEGATIVE_UNIT_Z = TVector3<T>(0, 0, -1);
	template <typename T>
	const TVector3<T> TVector3<T>::UNIT_SCALE = TVector3<T>(1, 1, 1);
}
