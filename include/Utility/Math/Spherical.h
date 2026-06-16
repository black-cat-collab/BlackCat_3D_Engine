#pragma once
#include "Utility/Math/Base/BaseType.h"
namespace bc
{
	class Spherical
	{
	public:
		Spherical(float radius_ = 1.0, float phi_ = 0, float theta_ = 0) : radius(radius_), phi(phi_), theta(theta_) {}

		void SetFromFloat3(const Vector3& vec);
		void SetFromCartesianCoords(float x = 1, float y = 0, float z = 0);

		void MakeSafe();

		void GetSphericalCoords(Vector3& output);
		void GetSphericalCoords2(Vector3& output);
		float radius = 1;
		float phi = 0;
		float theta = 0;
	};
}


