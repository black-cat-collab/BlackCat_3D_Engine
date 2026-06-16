#pragma once

#include "MathHeader.h"

#include <type_traits>
#include <vector>
namespace bc
{
	namespace DelaunayUtils
	{
		struct Edge
		{
			Vector3 p0, p1;

			Edge() {}
			Edge(Vector3 const& _p0, Vector3 const& _p1) : p0{ _p0 }, p1{ _p1 } {}


			bool operator==(const Edge& other) const
			{
				return ((other.p0 == p0 && other.p1 == p1) ||
					(other.p0 == p1 && other.p1 == p0));
			}
		};

		struct Circle
		{
			float x, y, radius;
			Circle() = default;
		};

		struct DelaunayTriangle
		{
			Vector3 p0, p1, p2;
			Edge e0, e1, e2;
			Circle circle;

			DelaunayTriangle(const Vector3& _p0, const Vector3& _p1, const Vector3& _p2)
				: p0{ _p0 }, p1{ _p1 }, p2{ _p2 }, circle{}
			{
				Vector3 tempEdge1 = p1 - p0;
				Vector3 tempEdge2 = p2 - p1;
				
				Vector3 crossValue = Vector3(tempEdge1.x, tempEdge1.y, 1.0).CrossProduct(Vector3(tempEdge2.x, tempEdge2.y, 1.0));

				if (crossValue.z < 0)
				{
					std::swap(p1, p2);
				}

				e0 = { p0, p1 };
				e1 = { p1, p2 };
				e2 = { p0, p2 };

				const auto ax = p1.x - p0.x;
				const auto ay = p1.y - p0.y;
				const auto bx = p2.x - p0.x;
				const auto by = p2.y - p0.y;

				const float m = p1.x * p1.x - p0.x * p0.x + p1.y * p1.y - p0.y * p0.y;
				const float u = p2.x * p2.x - p0.x * p0.x + p2.y * p2.y - p0.y * p0.y;
				const float s = 1. / (2. * (ax * by - ay * bx));


				circle.x = ((p2.y - p0.y) * m + (p0.y - p1.y) * u) * s;
				circle.y = ((p0.x - p2.x) * m + (p1.x - p0.x) * u) * s;

				const float dx = p0.x - circle.x;
				const float dy = p0.y - circle.y;
				circle.radius = dx * dx + dy * dy;
			}
		};

		struct Delaunay
		{
			std::vector<DelaunayTriangle> triangles;
		};

		//template <typename T, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
		Delaunay triangulate(const std::vector<Vector3>& points, float height = 10.0f);
	}
}


