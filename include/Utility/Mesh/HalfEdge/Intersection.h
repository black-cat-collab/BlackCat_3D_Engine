#pragma once

#include <vector>
#include <Utility/Math/MathHeader.h>
namespace bc
{
	namespace Geometry
	{
		class HalfEdgeMesh;
		struct SurfaceIntersectionResult 
		{
			std::vector<Vector3> points;
			std::vector<std::array<size_t, 2>> edges;
			bool hasIntersections;
		};

		SurfaceIntersectionResult SelfIntersections(HalfEdgeMesh* pMesh, std::vector<Vector3>& vecPoint);
	}
}

