#pragma once

#include "MathHeader.h"

namespace bc
{
    enum class ContainmentType
    {
        /// <summary>
        /// The two bounding volumes don't intersect at all.
        /// </summary>
        Disjoint,

        /// <summary>
        /// One bounding volume completely contains another.
        /// </summary>
        Contains,

        /// <summary>
        /// The two bounding volumes overlap.
        /// </summary>
        Intersects
    };

	class CollisionsHelper
	{
	public:
		static ContainmentType SphereContainsSphere(const BoundingSphere& sphere1, const BoundingSphere& sphere2);
        static ContainmentType BoxContainsPoint(const BoundingBox& box, const Vector3& point);
        static ContainmentType BoxContainsBox(const BoundingBox& lBox, const BoundingBox& rBox);

        static bool BoxIntersectsTriangle(const BoundingBox& box, const Vector3& v0, const Vector3& v1, const Vector3& v2);
	};
}
