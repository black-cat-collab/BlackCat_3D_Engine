#pragma once

#include <cstdint>
#include "TVector3.h"
#include "TVector2.h"
namespace bc
{

	template <typename T>
	struct TVector4
	{
		union
		{
			struct { T x, y, z, w; };
			struct { T r, g, b, a; };
			struct { T s, t, p, q; };
		};

		constexpr TVector4(const TVector4&) = default;
		constexpr TVector4(TVector4&&) = default;


		constexpr TVector4() noexcept : x{ 0 }, y{ 0 }, z{ 0 }, w{ 0 } {}
		constexpr explicit TVector4(T v) noexcept : x{ v }, y{ v }, z{ v }, w{ v } {}
		constexpr TVector4(T fx, T fy, T fz, T fw) noexcept : x{ fx }, y{ fy }, z{ fz }, w{ fw } {}
		constexpr TVector4(T fx, T fy, T fz) noexcept : x{ fx }, y{ fy }, z{ fz }, w{ 1.0 } {}

		constexpr TVector4(const TVector3<T>& v) noexcept : x{ v.x }, y{ v.y }, z{ v.z }, w{ 1.0 } {}
		constexpr TVector4(const TVector2<T>& v) noexcept : x{ v.x }, y{ v.y }, z{ 0.0 }, w{ 1.0 } {}

		template <typename Y>
		constexpr TVector4(Y fx, Y fy, Y fz, Y fw) noexcept : x{ T(fx) }, y{ T(fy) }, z{ T(fz) }, w{ T(fw) } {}

		template <typename Y>
		constexpr TVector4(const TVector4<Y>& right)
		{
			x = static_cast<T>(right.x);
			y = static_cast<T>(right.y);
			z = static_cast<T>(right.z);
			w = static_cast<T>(right.w);
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

		constexpr TVector4& operator=(const TVector4& right)
		{
			x = right.x;
			y = right.y;
			z = right.z;
			w = right.w;
			return *this;
		};

		constexpr TVector4& operator=(TVector4&& right)
		{
			x = right.x;
			y = right.y;
			z = right.z;
			w = right.w;
			return *this;
		}

		TVector4& operator=(const T& right)
		{
			x = right;
			y = right;
			z = right;
			w = right;
			return *this;
		};

		constexpr bool operator==(const TVector4& right) const
		{
			return x == right.x && y == right.y && z == right.z && w == right.w;
		}

		constexpr bool operator!=(const TVector4& right) const
		{
			return !(*this == right);
		}

		constexpr TVector4 operator+(const TVector4<T>& right) const
		{
			return TVector4{ x + right.x, y + right.y, z + right.z, w + right.w };
		}

		TVector4& operator+=(const TVector4<T>& right)
		{
			x += right.x;
			y += right.y;
			z += right.z;
			w += right.w;
			return *this;
		}

		TVector4& operator+=(const T& right)
		{
			x += right;
			y += right;
			z += right;
			w += right;
			return *this;
		}

		constexpr TVector4 operator-(const TVector4<T>& right) const
		{
			return TVector4{ x - right.x, y - right.y, z - right.z, w - right.w };
		}

		TVector4& operator-=(const TVector4<T>& right)
		{
			x -= right.x;
			y -= right.y;
			z -= right.z;
			w -= right.w;
			return *this;
		}

		TVector4& operator-=(const T& right)
		{
			x -= right;
			y -= right;
			z -= right;
			w -= right;
			return *this;
		}

		constexpr TVector4 operator-() const
		{
			return TVector4{ -x, -y, -z, -w };
		}

		constexpr TVector4 operator*(T s) const
		{
			return TVector4{ x * s, y * s, z * s, w * s };
		}

		constexpr TVector4 operator*(const TVector4& right) const
		{
			return TVector4{ x * right.x, y * right.y, z * right.z, w * right.w };
		}

		TVector4& operator*=(const TVector4& right)
		{
			x *= right.x;
			y *= right.y;
			z *= right.z;
			w *= right.w;
			return *this;
		}

		TVector4& operator*=(T s)
		{
			x *= s;
			y *= s;
			z *= s;
			w *= s;
			return *this;
		}

		constexpr TVector4 operator/(const TVector4& right) const
		{
			return TVector4{ x / right.x, y / right.y, z / right.z, w / right.w };
		}

		constexpr TVector4& operator/=(const TVector4& right)
		{
			x /= right.x;
			y /= right.y;
			z /= right.z;
			w /= right.w;
			return *this;
		}

		constexpr TVector4 operator/(T s) const
		{
			return TVector4{ x / s, y / s, z / s, w / s };
		}

		TVector4& operator/=(T s)
		{
			x /= s;
			y /= s;
			z /= s;
			w /= s;
			return *this;
		}

		constexpr T Length() const
		{
			return sqrt(x * x + y * y + z * z + w * w);
		}

		constexpr T SquaredLength() const
		{
			return x * x + y * y + z * z + w * w;
		}

		constexpr bool Empty() const
		{
			T sqlen = SquaredLength();
			return (sqlen < (1e-06));
		}

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
				w *= fInvLength;
			}

			return fLength;
		}

		constexpr T Distance(const TVector4& rhs) const
		{
			return (*this - rhs).Length();
		}

		constexpr T DotProduct(const TVector4& rVector) const
		{
			return x * rVector.x + y * rVector.y + z * rVector.z + w * rVector.w;
		}

		constexpr T AngleBetween(const TVector4& rVector) const
		{
			T dotProduct = DotProduct(rVector);

			T vectorsMagnitude = Length() * rVector.Length();

			T angle = acos(dotProduct / vectorsMagnitude);

			if (isnan(angle))
			{
				return 0;
			}

			return angle;
		}
	};

	using Vector4 = TVector4<float>;
	using Vector4d = TVector4<double>;
	using int4 = TVector4<int32_t>;
	using U8int4 = TVector4<int8_t>;
}
