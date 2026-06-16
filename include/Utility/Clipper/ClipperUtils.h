#pragma once

#include "Utility/Math/MathHeader.h"

#include <vector>

namespace bc
{
	namespace ClipperUtils
	{
		void UnionCirclePoints(std::vector<std::vector<Vector3>>& output, const std::vector<Vector4>& vecPoint, int32_t size = 32);
		void IntersectCirclePoints(std::vector<std::vector<Vector3>>& output, const std::vector<Vector4>& vecPoint, int32_t size = 32);
	}

}
