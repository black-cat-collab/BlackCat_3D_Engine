#pragma once

#include "MathHeader.h"

namespace bc
{
	class Curves
	{
	public:
		static Vector3 Bezier(const std::vector<Vector3>& points, float progress)
		{
            if (points.empty())
            {
                return Vector3(0.0f);
            }
            else if (points.size() == 1)
            {
                return points[0];
            }
            else if (points.size() == 2)
            {
                //一阶
                return MathUtils::Mix(points[0], points[1], Vector3(progress));
            }
            else if (points.size() == 3)
            {
                //二阶
                return MathUtils::Mix(MathUtils::Mix(points[0], points[1], Vector3(progress)),
                    MathUtils::Mix(points[1], points[2], Vector3(progress)), Vector3(progress));
            }
            else if (points.size() == 4)
            {
                //三阶
                Vector3 p0 = MathUtils::Mix(points[0], points[1], Vector3(progress));
                Vector3 p1 = MathUtils::Mix(points[1], points[2], Vector3(progress));
                Vector3 p2 = MathUtils::Mix(points[2], points[3], Vector3(progress));

                return MathUtils::Mix(MathUtils::Mix(p0, p1, Vector3(progress)),
                    MathUtils::Mix(p1, p2, Vector3(progress)), Vector3(progress));
            }
            else
            {
                //四阶以及往上
                std::vector<Vector3> middlePoints(points.size() - 1);
                for (int i = 0; i < points.size() - 1; i++)
                {
                    middlePoints[i] = MathUtils::Mix(points[i], points[i + 1], Vector3(progress));
                }
                return Bezier(middlePoints, progress);
            }
		}

        static std::vector<Vector3> GenerateBezierByStep(const std::vector<Vector3>& points, int32_t max)
        {
            std::vector<Vector3> output;
            float reverse= 1.0f / max;

            for (size_t i = 0; i < max; i++)
            {
                output.push_back(Bezier(points, i * reverse));
            }

            output.push_back(points.back());
            return output;
        }

        static float Length(const std::vector<Vector3>& points)
        {
            float length = 0;

            for (int32_t i = 0; i < points.size() - 1; i++)
            {
                length += points[i].Distance2D(points[i + 1]);
            }

            return length;
        }

        static float Length(const std::vector<Vector2>& points)
        {
            float length = 0;

            for (int32_t i = 0; i < points.size() - 1; i++)
            {
                length += points[i].Distance(points[i + 1]);
            }

            return length;
        }

        static float BaseLength(const std::vector<Vector3>& points)
        {
            return (std::pow)(Length(points), 0.99);
        }

        static float BaseLength(const std::vector<Vector2>& points)
        {
            return (std::pow)(Length(points), 0.99);
        }
	};

}




