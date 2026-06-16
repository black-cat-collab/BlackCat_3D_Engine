#ifndef __RAY_2D_H_
#define __RAY_2D_H_
#pragma once
// Precompiler options
#include "Prerequisites.h"

#include "Utility/Math/Base/TVector2.h"
namespace bc
{
    /** Representation of a ray in space, i.e. a line with an origin and direction. */
    class  Ray2D
    {
    protected:
        Vector2d mOrigin;
        Vector2d mDirection;
		float   fLength;
    public:
        Ray2D():mOrigin(Vector2d(0, 0)), mDirection(Vector2d(0,0)) {}
        Ray2D(const Vector2d& origin, const Vector2d& direction)
            :mOrigin(origin), mDirection(direction) {}

		Ray2D(const bc::Vector2d& start, const bc::Vector2d& end, float ratio)
		{
			Set(start, end,  ratio );
		}
		void Set(const bc::Vector2d& start, const bc::Vector2d& end, float ratio = FLT_MAX)
		{
			mOrigin = start;
			mDirection = end - start;
			fLength = mDirection.Length();
			if (fLength > 0.0f)
			{
				mDirection /= fLength;
			}

		}

        /** Sets the origin of the ray. */
        void setOrigin(const Vector2d& origin) {mOrigin = origin;} 
        /** Gets the origin of the ray. */
        const Vector2d& getOrigin(void) const {return mOrigin;} 

        /** Sets the direction of the ray. */
		void setDirection(const Vector2d& tDir) { mDirection = tDir; }
        /** Gets the direction of the ray. */
        const Vector2d& getDirection(void) const {return mDirection;} 

		void setLineSegment(const Vector2d& p0, const Vector2d& p1)
		{
			mOrigin = p0;
			mDirection = p1 - p0;
			fLength = mDirection.Length();
			if (fLength > 1e-07)
			{
				mDirection = mDirection / fLength;

			}
		}

		/** Gets the position of a point t units along the ray. */
		Vector2d getPoint(double t) const { 
			return Vector2d(mOrigin + (mDirection * t));
		}
		
		/** Gets the position of a point t units along the ray. */
		Vector2d operator*(double t) const {
			return getPoint(t);
		};

		/////////
		bool getClosestPoints(const Ray2D& line2,Vector2d& ptOnThis, Vector2d& ptOnLine2) 
		{
			// new optimized version based on formulas from from Boyko Bantchev

			// p1 = point on line 1
			// p2 = point on line 2
			// d1 = line 1 direction
			// d2 = line 2 direction
			// q1 = closest point on line 1
			// q2 = closest point on line 2

			// The solution (q1 and q2) must be on their respective 
			// lines:
			//
			// q1 = p1 + t1 * d1                               (0)
			// q2 = p2 + t2 * d2
			//
			// we set u = p2 - p1, and get:
			//
			// q2 - q1 = u + t2*d2 - t1*d1                     (1)
			//
			// the solution line q2 - q1 is orthogonal to d1 and d2 
			// (or a null vector if the lines intersect), which yields:
			//
			// (u + t2*d2 - t1*d1) ?d1 = 0                    (2)
			// (u + t2*d2 - t1*d1) ?d2 = 0
			//
			// we know |d1| and |d2| == 1, and set d1 ?d2 = t
			//
			// t1 - t*t2 = u ?d1
			// t*t1 - t2 = u ?d2
			//
			// Solve for t1, and find q1 using (0):
			//
			// t1 = (u·d1 - t * (u·d2))/ (1 - t^2)
			//
			// just find q2 by using line2.getClosestPoint(q1)

			Vector2d p1 = mOrigin;
			Vector2d p2 = line2.mOrigin;

			Vector2d d1 = mDirection;
			Vector2d d2 = line2.mDirection;

			Vector2d u = p2 - p1;
			double t = d1.DotProduct(d2);

			const double eps = 1.0e-07;
			if (t < -1.0f + eps || t > 1.0f - eps) {
				// lines are parallel
				return false;
			}
			t = (u.DotProduct(d1) - t * u.DotProduct(d2)) / (1 - t*t);
			ptOnThis = p1 + d1 * t;
			ptOnLine2 = line2.getClosestPoint(ptOnThis);
			return true;


		}
		/////////////
		Vector2d	getClosestPoint(const Vector2d& point) const
		{
			//
			//             Q      D
			//    SP x-----x------->
			//        \    |
			//         \   |
			//          \  |
			//           \ |
			//            \|
			//             x P
			//
			// P = argument point, SP = line starting point, D = line direction,
			// Q = point to find.
			//
			// Solved by:
			//                         a·b
			//             comp_b(a) = ---   , a = P-SP, b = D, comp_b(a) = |Q-SP|
			//                         |b|
			//
			//  ==>   Q = SP + comp_b(a)*D
			//                                        19980815 mortene.

			// No use warning about a zero length line here. The user will get a
			// warning when the line is constructed. Also, we don't need to
			// account for the length of the direction vector, since mDirection
			// is always normalized (or a null-vector). The result will actually
			// be correct when the line has zero length, since the line starting
			// point will then be the closest point. pederb, 2005-02-24
			return mOrigin + mDirection * (point - mOrigin).DotProduct(mDirection);
		}
		Vector2d	getClosestPoint(const Vector2d& point) 
		{
			//
			//             Q      D
			//    SP x-----x------->
			//        \    |
			//         \   |
			//          \  |
			//           \ |
			//            \|
			//             x P
			//
			// P = argument point, SP = line starting point, D = line direction,
			// Q = point to find.
			//
			// Solved by:
			//                         a·b
			//             comp_b(a) = ---   , a = P-SP, b = D, comp_b(a) = |Q-SP|
			//                         |b|
			//
			//  ==>   Q = SP + comp_b(a)*D
			//                                        19980815 mortene.

			// No use warning about a zero length line here. The user will get a
			// warning when the line is constructed. Also, we don't need to
			// account for the length of the direction vector, since mDirection
			// is always normalized (or a null-vector). The result will actually
			// be correct when the line has zero length, since the line starting
			// point will then be the closest point. pederb, 2005-02-24
			return mOrigin + mDirection * (point - mOrigin).DotProduct(mDirection);
		}
		/** Tests whether this ray intersects the given plane. 
		@returns A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects. 
			This can be converted to a point in space by calling getPoint().
		*/

    };
}
#endif

