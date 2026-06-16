#pragma once

#include <assert.h>
#include <math.h>
#include <string>

#include "Utility/Math/Base/TVector3.h"

namespace bc
{
	enum AxisDirection_e
	{
		FRONT = 0,
		LEFT,
		UP
	};

	class Axis
	{
	private:
		Vector3 m_vAxis[3];
	public:
		static const Axis IDENTITY;
	public:
		inline Axis()
		{
			memset(m_vAxis, 0, 9 * sizeof(float));
		}

		inline Axis(const float fData[3][3])
		{
			memcpy(m_vAxis, fData, 9 * sizeof(float));
		}

		inline Axis(const Axis& rAxis)
		{
			memcpy(m_vAxis, rAxis.m_vAxis, 9 * sizeof(float));
		}

		inline Axis(const Vector3& vFront, const Vector3& vLeft, const Vector3& vUp)
		{
			m_vAxis[AxisDirection_e::FRONT] = vFront;
			m_vAxis[AxisDirection_e::LEFT] = vLeft;
			m_vAxis[AxisDirection_e::UP] = vUp;
		}

		inline Axis(const float& x0, const float& x1, const float& x2,
			const float& y0, const float& y1, const float& y2,
			const float& z0, const float& z1, const float& z2)
		{
			m_vAxis[0][0] = x0;
			m_vAxis[0][1] = x1;
			m_vAxis[0][2] = x2;

			m_vAxis[1][0] = y0;
			m_vAxis[1][1] = y1;
			m_vAxis[1][2] = y2;

			m_vAxis[2][0] = z0;
			m_vAxis[2][1] = z1;
			m_vAxis[2][2] = z2;
		}

		inline const Vector3& operator [] (const size_t& i) const
		{
			assert(i < 3);

			return m_vAxis[i];
		}

		inline Vector3& operator [] (const size_t& i)
		{
			assert(i < 3);

			return m_vAxis[i];
		}

		inline bool operator == (const Axis& rAxis) const
		{
			if (m_vAxis[0] == rAxis.m_vAxis[0] &&
				m_vAxis[1] == rAxis.m_vAxis[1] &&
				m_vAxis[2] == rAxis.m_vAxis[2])
			{
				return true;
			}

			return false;
		}

		inline bool operator != (const Axis& rAxis) const
		{
			return !((*this) == rAxis);
		}

		inline Axis& operator = (const Axis& rAxis)
		{
			memcpy(m_vAxis, rAxis.m_vAxis, 9 * sizeof(float));

			return *this;
		}

		inline void SetUp(const Vector3& v)
		{
			m_vAxis[2] = v;
		}

		inline void SetRight(const Vector3& v)
		{
			m_vAxis[1] = -v;
		}

		inline void SetForward(const Vector3& v)
		{
			m_vAxis[0] = v;
		}

		inline float* Ptr()
		{
			return &(m_vAxis[0].x);
		}

		inline const float* Ptr() const
		{
			return &(m_vAxis[0].x);
		}

		void FromVector(const Vector3& vFront);
		void FromAngles(const Vector3& vAngles);
		Vector3 ToAngles();
		Vector3 GetScale();
		void Multiply(const Axis& vAxis);
		void LookAt(const Vector3& vEye, const Vector3& vCenter, const Vector3& vUp);
		Axis Transpose() const;

	};

	static_assert(sizeof(Axis) == (9 * sizeof(float)), "Error size of Axis");
}
