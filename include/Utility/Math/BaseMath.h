#pragma once

#include <assert.h>
#include <vector>
#include <algorithm>

#include <float.h>
#include <limits.h>



namespace bc
{
	class Degree;
	class Radian
	{
		float mRad;

	public:
		explicit Radian(float r = 0.0f) { mRad = r; }
		Radian(const Degree& d);
		Radian& operator = (const float& f) { mRad = f; return *this; }
		Radian& operator = (const Radian& r) { mRad = r.mRad; return *this; }
		Radian& operator = (const Degree& d);

		float ValueDegrees() const { return RAD_TO_DEG(mRad); }
		float ValueRadians() const { return mRad; }

		const Radian& operator + () const { return *this; }
		Radian operator + (const Radian& r) const { return Radian(mRad + r.mRad); }
		Radian operator + (const Degree& d) const;
		Radian& operator += (const Radian& r) { mRad += r.mRad; return *this; }
		Radian& operator += (const Degree& d);
		Radian operator - () const { return Radian(-mRad); }
		Radian operator - (const Radian& r) const { return Radian(mRad - r.mRad); }
		Radian operator - (const Degree& d) const;
		Radian& operator -= (const Radian& r) { mRad -= r.mRad; return *this; }
		Radian& operator -= (const Degree& d);
		Radian operator * (float f) const { return Radian(mRad * f); }
		Radian operator * (const Radian& f) const { return Radian(mRad * f.mRad); }
		Radian& operator *= (float f) { mRad *= f; return *this; }
		Radian operator / (float f) const { return Radian(mRad / f); }
		Radian& operator /= (float f) { mRad /= f; return *this; }

		bool operator <  (const Radian& r) const { return mRad < r.mRad; }
		bool operator <= (const Radian& r) const { return mRad <= r.mRad; }
		bool operator == (const Radian& r) const { return mRad == r.mRad; }
		bool operator != (const Radian& r) const { return mRad != r.mRad; }
		bool operator >= (const Radian& r) const { return mRad >= r.mRad; }
		bool operator >  (const Radian& r) const { return mRad > r.mRad; }
	};


	class Degree
	{
		float mDeg;

	public:
		explicit Degree(float d = 0) : mDeg(d) {}
		Degree(const Radian& r) : mDeg(r.ValueDegrees()) {}
		Degree& operator = (const float& f) { mDeg = f; return *this; }
		Degree& operator = (const Degree& d) { mDeg = d.mDeg; return *this; }
		Degree& operator = (const Radian& r) { mDeg = r.ValueDegrees(); return *this; }

		float ValueDegrees() const { return mDeg; }
		float ValueRadians() const { return DEG_TO_RAD(mDeg); } // see bottom of this file

		const Degree& operator + () const { return *this; }
		Degree operator + (const Degree& d) const { return Degree(mDeg + d.mDeg); }
		Degree operator + (const Radian& r) const { return Degree(mDeg + r.ValueDegrees()); }
		Degree& operator += (const Degree& d) { mDeg += d.mDeg; return *this; }
		Degree& operator += (const Radian& r) { mDeg += r.ValueDegrees(); return *this; }
		Degree operator - () const { return Degree(-mDeg); }
		Degree operator - (const Degree& d) const { return Degree(mDeg - d.mDeg); }
		Degree operator - (const Radian& r) const { return Degree(mDeg - r.ValueDegrees()); }
		Degree& operator -= (const Degree& d) { mDeg -= d.mDeg; return *this; }
		Degree& operator -= (const Radian& r) { mDeg -= r.ValueDegrees(); return *this; }
		Degree operator * (float f) const { return Degree(mDeg * f); }
		Degree operator * (const Degree& f) const { return Degree(mDeg * f.mDeg); }
		Degree& operator *= (float f) { mDeg *= f; return *this; }
		Degree operator / (float f) const { return Degree(mDeg / f); }
		Degree& operator /= (float f) { mDeg /= f; return *this; }

		bool operator <  (const Degree& d) const { return mDeg < d.mDeg; }
		bool operator <= (const Degree& d) const { return mDeg <= d.mDeg; }
		bool operator == (const Degree& d) const { return mDeg == d.mDeg; }
		bool operator != (const Degree& d) const { return mDeg != d.mDeg; }
		bool operator >= (const Degree& d) const { return mDeg >= d.mDeg; }
		bool operator >  (const Degree& d) const { return mDeg > d.mDeg; }
	};

	struct PolygonPoint
	{
		float	fDistance;
		int		nInsidePoint;
		int		nOutsidePoint;
	};

	struct RayIntersect
	{
		Vector3d		vStart;
		Vector3d		vDir;
		double		fRadio;
		double		fLength;

		RayIntersect(const Vector3d& s, const Vector3d& d)
		{
			Set(s, d);
		}

		void Set(const Vector3d& start, const Vector3d& end, const double& ratio = FLT_MAX)
		{
			vStart = start;
			vDir = end - start;
			fLength = vDir.Length();
			if (fLength > 0.0f)
			{
				vDir /= fLength;
			}

		}

		bool GetClosestPoints(const RayIntersect& line2, Vector3d& ptOnThis, Vector3d& ptOnLine2) const
		{
			Vector3d p1 = vStart;
			Vector3d p2 = line2.vStart;

			Vector3d d1 = vDir;
			Vector3d d2 = line2.vDir;

			Vector3d u = p2 - p1;
			double t = d1.DotProduct(d2);

			const double eps = 1.0e-07;

			if (fabs(fabs(t) - 1.0) < eps)
			{
				// lines are parallel
				return false;
			}
			t = (u.DotProduct(d1) - t * u.DotProduct(d2)) / (1 - t * t);
			ptOnThis = p1 + d1 * t;
			ptOnLine2 = line2.GetClosestPoint(ptOnThis);
			return true;


		}

		Vector3d	GetClosestPoint(const Vector3d& point) const
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
			//                         a穊
			//             comp_b(a) = ---   , a = P-SP, b = D, comp_b(a) = |Q-SP|
			//                         |b|
			//
			//  ==>   Q = SP + comp_b(a)*D
			//                                        19980815 mortene.

			// No use warning about a zero length line here. The user will get a
			// warning when the line is constructed. Also, we don't need to
			// account for the length of the direction vector, since vDir
			// is always normalized (or a null-vector). The result will actually
			// be correct when the line has zero length, since the line starting
			// point will then be the closest point. pederb, 2005-02-24
			return vStart + vDir * (point - vStart).DotProduct(vDir);
		}

	};
	struct SegmentInstrect
	{
		Vector3d vStart;
		Vector3d vEnd;
		SegmentInstrect() :
			vStart(Vector3d(0.0f, 0.0f, 0.0f)),
			vEnd(Vector3d(0.0f, 0.0f, 0.0f))
		{

		}
		SegmentInstrect(const Vector3d& start, const Vector3d& end) :
			vStart(start),
			vEnd(end)
		{

		}
		void Set(const Vector3d& start, const Vector3d& end)
		{
			vStart = start;
			vEnd = end;
		}
	};

	struct IntersectPoint
	{
		double fRatio;
		Vector3d vPoint;
		Vector3d vNormal;
	};



	typedef Vector3 Matrix3x3[3];
	typedef Vector3d Matrix3x3d[3];
	class BoundingBox;
	class BoundingSphere;
	class BaseMath
	{
    public:
		static int		Factorial(int nNum);
		static int		RoundUpPowOfTwo(int nNum);
		static int		WarpSize(int nIndex, int nLength);
		static void		Matrix3x3Multiply(const Matrix3x3 fInMatrix1, const Matrix3x3 fInMatrix2, Matrix3x3 fOutMatrix);
        static void     RotateAroundVector(Vector3 fRot[3], const bc::Vector3& vDir, const float& fDegrees);
        static void     RotateAroundVector(Axis& axis, const Vector3& vDir, const float& fDegrees);
        static Vector3	RotatePointAroundVector(const Vector3& vDir, const Vector3& vPoint, const float& fDegrees);
		static Vector3	PerpendicularVector(const Vector3& vSrc);

		static bool IsNotZerof(const float& number);
		static bool isNotZerod(const double& number);

		static bool IsDoubleEqual(double a, double b);

		static bool IsZerof(const float&  number);
		static bool isZerod(const double& number);
		
		static bool IsVector3Equal(const Vector3& v0, const Vector3& v1);
		static bool IsVector4Equal(const Vector4& v0, const Vector4& v1);
		static bool IsVector4dEqual(const Vector4d& v0, const Vector4d& v1);

		static void ReversePoints(std::vector<Vector3>& points);

		static bool RayIntersectsTriangle(const RayIntersect& ray, const Vector3& vertex1, const Vector3& vertex2, const Vector3& vertex3, float& distance, Vector3& normal);
		static bool RayIntersectsTriangle2(const RayIntersect& ray, const Vector3& vertex1, const Vector3& vertex2, const Vector3& vertex3, float& distance, Vector3& normal);

		static Vector3	ProjectPointOnPlane(const Vector3& vPoint, const Vector3& vNormal);
        static float    Lerp(const float& a, const float& b, const float& f);
		static float    Cerpf(const float& a, const float& b, const float& c, const float& d, const float& x);
		static float	ACos(const float& fValue);
		static float	AngleBetween(const Vector3& source, const Vector3& dest);
		static bool		IntersectAndClip(Vector3& vs, Vector3& vd, const BoundingBox& tBBox);
		static bool		IntersectBoundingSphere(Vector3& vs, Vector3& vd, const BoundingSphere& bs);
		static void		IntersectTriangle(RayIntersect& ri, const Vector3& v1, const Vector3& v2, const Vector3& v3, 
			std::vector<IntersectPoint>& vecIP);
		static void		FlipFace(std::vector<Vector3>& vecPoint);
		static bool		FloatEqual(float a, float b, float tolerance);
		static float	ClampedPixelSize(const Vector4& vPixelSizeVector, const BoundingSphere& tSphere);
		static bool		SegmentPolygon(const std::vector<Vector3>& vecVertexOrg, std::vector<std::vector<Vector3>>& vecConvex);
		template<typename T>
		static bool		CalcBezierCurve(const int& nTimes, const int& nSegment, const std::vector<T>& vecIn, std::vector<T>& vecOut)
		{
			if (nTimes != (vecIn.size() - 1))
			{
				return false;
			}

			for (int i = 0; i <= nSegment; i++)
			{
				float t = float(i) / float(nSegment);

				T v(0.0);

				for (int j = 0; j <= nTimes; j++)
				{
					v += vecIn[j] * float(Factorial(nTimes) / (Factorial(j) * Factorial(nTimes - j)) * pow(t, j) * pow(1 - t, nTimes - j));
				}

				vecOut.push_back(v);
			}

			return true;
		}

		template<typename T>
		static T		CalcBezierCurve(const int& nTimes, const std::vector<T>& vecIn, float fStep)
		{
			if (nTimes != (vecIn.size() - 1))
			{
				return T(0.0f);
			}

			assert(fStep <= 1.0f && fStep >= 0.0f);

			T v(0.0);

			for (int j = 0; j <= nTimes; j++)
			{
				v += vecIn[j] * float(Factorial(nTimes) / (Factorial(j) * Factorial(nTimes - j)) * pow(fStep, j) * pow(1 - fStep, nTimes - j));
			}

			return v;
		}

		template <typename T>
		static T Clamp(T val, T minval, T maxval)
		{
			assert(minval < maxval && "Invalid clamp range");
			return MAX(MIN(val, maxval), minval);
		}
	private:
		static bool		insideConvex(const Vector3& vIntersection, const std::vector<Vector3>& vecPolys);
		static bool		insidePolygon(const  Vector3& vIntersection, const std::vector<Vector3>& vecPolys);
		static bool		insidePolygon(const int& nPos, const std::vector<Vector3>& vecPolys);


    };
}
