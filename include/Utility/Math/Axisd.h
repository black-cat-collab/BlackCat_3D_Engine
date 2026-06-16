#ifndef __HC_AXIS_D_H__
#define __HC_AXIS_D_H__
#pragma once

#include <cassert>

#include "Axis.h"
namespace bc
{
	class Axisd
	{
	private:
		Vector3d m_vAxis[3];
	public:
		static const Axisd IDENTITY;
	public:
		inline Axisd()
		{
			memset(m_vAxis, 0, 9 * sizeof(double));
		}

		inline Axisd(const double fData[3][3])
		{
			memcpy(m_vAxis, fData, 9 * sizeof(double));
		}

		inline Axisd(const Axis& rAxis)
		{
			m_vAxis[0][0] = rAxis[0][0];
			m_vAxis[0][1] = rAxis[0][1];
			m_vAxis[0][2] = rAxis[0][2];

			m_vAxis[1][0] = rAxis[1][0];
			m_vAxis[1][1] = rAxis[1][1];
			m_vAxis[1][2] = rAxis[1][2];

			m_vAxis[2][0] = rAxis[2][0];
			m_vAxis[2][1] = rAxis[2][1];
			m_vAxis[2][2] = rAxis[2][2];
		}

		inline Axisd(const Axisd& rAxis)
		{
			memcpy(m_vAxis, rAxis.m_vAxis, 9 * sizeof(double));
		}

		inline Axisd(const Vector3d& vFront, const Vector3d& vLeft, const Vector3d& vUp)
		{
			m_vAxis[AxisDirection_e::FRONT] = vFront;
			m_vAxis[AxisDirection_e::LEFT] = vLeft;
			m_vAxis[AxisDirection_e::UP] = vUp;
		}

		inline Axisd(const double& x0, const double& x1, const double& x2,
			const double& y0, const double& y1, const double& y2,
			const double& z0, const double& z1, const double& z2)
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

		inline Axis Getf()
		{
			Axis a;
			a[0][0] = m_vAxis[0][0];
			a[0][1] = m_vAxis[0][1];
			a[0][2] = m_vAxis[0][2];

			a[1][0] = m_vAxis[1][0];
			a[1][1] = m_vAxis[1][1];
			a[1][2] = m_vAxis[1][2];

			a[2][0] = m_vAxis[2][0];
			a[2][1] = m_vAxis[2][1];
			a[2][2] = m_vAxis[2][2];

			return a;
		}

		inline Axis Getf() const
		{
			Axis a;
			a[0][0] = m_vAxis[0][0];
			a[0][1] = m_vAxis[0][1];
			a[0][2] = m_vAxis[0][2];

			a[1][0] = m_vAxis[1][0];
			a[1][1] = m_vAxis[1][1];
			a[1][2] = m_vAxis[1][2];

			a[2][0] = m_vAxis[2][0];
			a[2][1] = m_vAxis[2][1];
			a[2][2] = m_vAxis[2][2];

			return a;
		}
		inline const Vector3d& operator [] (const size_t& i) const
		{
			assert(i < 3);

			return m_vAxis[i];
		}

		inline Vector3d& operator [] (const size_t& i)
		{
			assert(i < 3);

			return m_vAxis[i];
		}

		inline bool operator == (const Axisd& rAxis) const
		{
			if (m_vAxis[0] == rAxis.m_vAxis[0] &&
				m_vAxis[1] == rAxis.m_vAxis[1] &&
				m_vAxis[2] == rAxis.m_vAxis[2])
			{
				return true;
			}

			return false;
		}

		inline bool operator != (const Axisd& rAxis) const
		{
			return !((*this) == rAxis);
		}

		inline Axisd& operator = (const Axisd& rAxis)
		{
			memcpy(m_vAxis, rAxis.m_vAxis, 9 * sizeof(double));

			return *this;
		}

		inline void SetUp(const Vector3d& v)
		{
			m_vAxis[2] = v;
		}

		inline void SetRight(const Vector3d& v)
		{
			m_vAxis[1] = -v;
		}

		inline void SetForward(const Vector3d& v)
		{
			m_vAxis[0] = v;
		}

		inline double* Ptr()
		{
			return &(m_vAxis[0].x);
		}

		inline const double* Ptr() const
		{
			return &(m_vAxis[0].x);
		}

		void FromVector(const Vector3d& vFront);
		void FromAngles(const Vector3d& vAngles);
		Vector3d ToAngles();
		Vector3d GetScale();
		void Multiply(const Axisd& vAxis);
		void LookAt(const Vector3d& vEye, const Vector3d& vCenter, const Vector3d& vUp);
		Axisd Transpose() const;

	};
}
#endif
