#pragma once

#include <cstdint>
#include <cmath>
namespace bc
{
	template <typename T>
	struct TVector2
	{
		union
		{
			struct { T x, y; };
			struct { T r, g; };
			struct { T u, v; };
			struct { T s, t; };
		};

		constexpr TVector2(const TVector2&) = default;
		constexpr TVector2(TVector2&&) = default;


		constexpr TVector2() noexcept : x{ 0 }, y{ 0 } {}
		constexpr explicit TVector2(T v) noexcept : x{ v }, y{ v } {}
		constexpr TVector2(T fx, T fy) noexcept : x{ fx }, y{ fy } {}

		template <typename Y>
		constexpr TVector2(Y fx, Y fy) noexcept : x{ T(fx) }, y{ T(fy) } {}

		template <typename Y>
		constexpr TVector2(const TVector2<Y>& right)
		{
			x = static_cast<T>(right.x);
			y = static_cast<T>(right.y);
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

		constexpr TVector2& operator=(const TVector2& right)
		{
			x = right.x;
			y = right.y;
			return *this;
		};

		constexpr TVector2& operator=(TVector2&& right)
		{
			x = right.x;
			y = right.y;
			return *this;
		}

		TVector2& operator=(const T& right)
		{
			x = right;
			y = right;
			return *this;
		};

		constexpr bool operator==(const TVector2& right) const
		{
			return x == right.x && y == right.y;
		}

		constexpr bool operator!=(const TVector2& right) const
		{
			return !(*this == right);
		}

		constexpr TVector2 operator+(const TVector2<T>& right) const
		{
			return TVector2{ x + right.x, y + right.y };
		}

		TVector2& operator+=(const TVector2<T>& right)
		{
			x += right.x;
			y += right.y;
			return *this;
		}

		TVector2& operator+=(const T& right)
		{
			x += right;
			y += right;
			return *this;
		}

		constexpr TVector2 operator-(const TVector2<T>& right) const
		{
			return TVector2{ x - right.x, y - right.y };
		}

		TVector2& operator-=(const TVector2<T>& right)
		{
			x -= right.x;
			y -= right.y;
			return *this;
		}

		TVector2& operator-=(const T& right)
		{
			x -= right;
			y -= right;
			return *this;
		}

		constexpr TVector2 operator-() const
		{
			return TVector2{ -x, -y };
		}

		constexpr TVector2 operator*(T s) const
		{
			return TVector2{ x * s, y * s };
		}

		constexpr TVector2 operator*(const TVector2& right) const
		{
			return TVector2{ x * right.x, y * right.y };
		}

		TVector2& operator*=(const TVector2& right)
		{
			x *= right.x;
			y *= right.y;
			return *this;
		}

		TVector2& operator*=(T s)
		{
			x *= s;
			y *= s;
			return *this;
		}

		constexpr TVector2 operator/(const TVector2& right) const
		{
			return TVector2{ x / right.x, y / right.y };
		}

		constexpr TVector2& operator/=(const TVector2& right)
		{
			x /= right.x;
			y /= right.y;
			return *this;
		}

		constexpr TVector2 operator/(T s) const
		{
			return TVector2{ x / s, y / s };
		}

		TVector2& operator/=(T s)
		{
			x /= s;
			y /= s;
			return *this;
		}

		void Set(const float& fx, const float& fy)
		{
			x = fx;
			y = fy;
		}


		constexpr T Length() const
		{
			return sqrt(x * x + y * y);
		}

		constexpr T SquaredLength() const
		{
			return x * x + y * y;
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
			}

			return fLength;
		}

		constexpr T Distance(const TVector2& rhs) const
		{
			return (*this - rhs).Length();
		}

		constexpr T DotProduct(const TVector2& rVector) const
		{
			return x * rVector.x + y * rVector.y;
		}

		constexpr T CrossProduct(const TVector2& rVector) const
		{
			return x * rVector.y - y * rVector.x;
		}

		constexpr float AngleBetween(const TVector2& rVector) const
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

		TVector2 MoveForward(const TVector2& v, const T& fDis)
		{
			return  (*this + v * fDis);
		}

		constexpr TVector2 MoveForward(const TVector2& v, const T& fDis) const
		{
			return  (*this + v * fDis);
		}
	};

	using Vector2 = TVector2<float>;
	using Vector2d = TVector2<double>;
	using int2 = TVector2<int32_t>;
}
