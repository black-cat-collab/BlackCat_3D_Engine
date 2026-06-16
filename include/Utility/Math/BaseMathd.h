#pragma once

#include <assert.h>
#include <vector>
#include <algorithm>

namespace bc
{
	class Degreed;
	class Radiand
	{
		double mRad;

	public:
		explicit Radiand(double r = 0.0f) { mRad = r; }
		Radiand(const Degreed& d);
		Radiand& operator = (const double& f) { mRad = f; return *this; }
		Radiand& operator = (const Radiand& r) { mRad = r.mRad; return *this; }
		Radiand& operator = (const Degreed& d);

		double ValueDegrees() const { return RAD_TO_DEG(mRad); }
		double ValueRadians() const { return mRad; }

		const Radiand& operator + () const { return *this; }
		Radiand operator + (const Radiand& r) const { return Radiand(mRad + r.mRad); }
		Radiand operator + (const Degreed& d) const;
		Radiand& operator += (const Radiand& r) { mRad += r.mRad; return *this; }
		Radiand& operator += (const Degreed& d);
		Radiand operator - () const { return Radiand(-mRad); }
		Radiand operator - (const Radiand& r) const { return Radiand(mRad - r.mRad); }
		Radiand operator - (const Degreed& d) const;
		Radiand& operator -= (const Radiand& r) { mRad -= r.mRad; return *this; }
		Radiand& operator -= (const Degreed& d);
		Radiand operator * (double f) const { return Radiand(mRad * f); }
		Radiand operator * (const Radiand& f) const { return Radiand(mRad * f.mRad); }
		Radiand& operator *= (double f) { mRad *= f; return *this; }
		Radiand operator / (double f) const { return Radiand(mRad / f); }
		Radiand& operator /= (double f) { mRad /= f; return *this; }

		bool operator <  (const Radiand& r) const { return mRad < r.mRad; }
		bool operator <= (const Radiand& r) const { return mRad <= r.mRad; }
		bool operator == (const Radiand& r) const { return mRad == r.mRad; }
		bool operator != (const Radiand& r) const { return mRad != r.mRad; }
		bool operator >= (const Radiand& r) const { return mRad >= r.mRad; }
		bool operator >  (const Radiand& r) const { return mRad > r.mRad; }
	};


	class Degreed
	{
		double mDeg;

	public:
		explicit Degreed(double d = 0) : mDeg(d) {}
		Degreed(const Radiand& r) : mDeg(r.ValueDegrees()) {}
		Degreed& operator = (const double& f) { mDeg = f; return *this; }
		Degreed& operator = (const Degreed& d) { mDeg = d.mDeg; return *this; }
		Degreed& operator = (const Radiand& r) { mDeg = r.ValueDegrees(); return *this; }

		double ValueDegrees() const { return mDeg; }
		double ValueRadians() const { return DEG_TO_RAD(mDeg); } // see bottom of this file

		const Degreed& operator + () const { return *this; }
		Degreed operator + (const Degreed& d) const { return Degreed(mDeg + d.mDeg); }
		Degreed operator + (const Radiand& r) const { return Degreed(mDeg + r.ValueDegrees()); }
		Degreed& operator += (const Degreed& d) { mDeg += d.mDeg; return *this; }
		Degreed& operator += (const Radiand& r) { mDeg += r.ValueDegrees(); return *this; }
		Degreed operator - () const { return Degreed(-mDeg); }
		Degreed operator - (const Degreed& d) const { return Degreed(mDeg - d.mDeg); }
		Degreed operator - (const Radiand& r) const { return Degreed(mDeg - r.ValueDegrees()); }
		Degreed& operator -= (const Degreed& d) { mDeg -= d.mDeg; return *this; }
		Degreed& operator -= (const Radiand& r) { mDeg -= r.ValueDegrees(); return *this; }
		Degreed operator * (double f) const { return Degreed(mDeg * f); }
		Degreed operator * (const Degreed& f) const { return Degreed(mDeg * f.mDeg); }
		Degreed& operator *= (double f) { mDeg *= f; return *this; }
		Degreed operator / (double f) const { return Degreed(mDeg / f); }
		Degreed& operator /= (double f) { mDeg /= f; return *this; }

		bool operator <  (const Degreed& d) const { return mDeg < d.mDeg; }
		bool operator <= (const Degreed& d) const { return mDeg <= d.mDeg; }
		bool operator == (const Degreed& d) const { return mDeg == d.mDeg; }
		bool operator != (const Degreed& d) const { return mDeg != d.mDeg; }
		bool operator >= (const Degreed& d) const { return mDeg >= d.mDeg; }
		bool operator >  (const Degreed& d) const { return mDeg > d.mDeg; }
	};

	struct PolygonPointd
	{
		double	fDistance;
		int		nInsidePoint;
		int		nOutsidePoint;
	};

	//struct RayIntersectd
	//{
	//	Vector3d		vStart;
	//	Vector3d		vDir;
	//	double		fRadio;
	//	double		fLength;

	//	RayIntersectd(const Vector3d& s, const Vector3d& d)
	//	{
	//		Set(s, d);
	//	}

	//	void Set(const Vector3d& start, const Vector3d& end, double ratio = FLT_MAX)
	//	{
	//		vStart = start;
	//		vDir = end - start;
	//		fLength = vDir.Length();
	//		if (fLength > 0.0f)
	//		{
	//			vDir /= fLength;
	//		}

	//	}

	//	bool GetClosestPoints(const RayIntersectd& line2, Vector3d& ptOnThis, Vector3d& ptOnLine2) const
	//	{
	//		Vector3d p1 = vStart;
	//		Vector3d p2 = line2.vStart;

	//		Vector3d d1 = vDir;
	//		Vector3d d2 = line2.vDir;

	//		Vector3d u = p2 - p1;
	//		double t = d1.DotProduct(d2);

	//		const double eps = 1.0e-07;

	//		if (fabs(fabs(t) - 1.0f) < eps)
	//		{
	//			// lines are parallel
	//			return false;
	//		}
	//		t = (u.DotProduct(d1) - t * u.DotProduct(d2)) / (1 - t * t);
	//		ptOnThis = p1 + d1 * t;
	//		ptOnLine2 = line2.GetClosestPoint(ptOnThis);
	//		return true;


	//	}

	//	Vector3d	GetClosestPoint(const Vector3d& point) const
	//	{
	//		//
	//		//             Q      D
	//		//    SP x-----x------->
	//		//        \    |
	//		//         \   |
	//		//          \  |
	//		//           \ |
	//		//            \|
	//		//             x P
	//		//
	//		// P = argument point, SP = line starting point, D = line direction,
	//		// Q = point to find.
	//		//
	//		// Solved by:
	//		//                         a穊
	//		//             comp_b(a) = ---   , a = P-SP, b = D, comp_b(a) = |Q-SP|
	//		//                         |b|
	//		//
	//		//  ==>   Q = SP + comp_b(a)*D
	//		//                                        19980815 mortene.

	//		// No use warning about a zero length line here. The user will get a
	//		// warning when the line is constructed. Also, we don't need to
	//		// account for the length of the direction vector, since vDir
	//		// is always normalized (or a null-vector). The result will actually
	//		// be correct when the line has zero length, since the line starting
	//		// point will then be the closest point. pederb, 2005-02-24
	//		return vStart + vDir * (point - vStart).DotProduct(vDir);
	//	}

	//};

	/*struct SegmentInstrectd
	{
		Vector3d vStart;
		Vector3d vEnd;
		SegmentInstrectd() :
			vStart(Vector3d(0.0f, 0.0f, 0.0f)),
			vEnd(Vector3d(0.0f, 0.0f, 0.0f))
		{

		}
		SegmentInstrectd(const Vector3d& start, const Vector3d& end) :
			vStart(start),
			vEnd(end)
		{

		}
		void Set(const Vector3d& start, const Vector3d& end)
		{
			vStart = start;
			vEnd = end;
		}
	};*/

	//struct IntersectPointd
	//{
	//	double fRatio;
	//struct IntersectPointd
	//{
	//	float fRatio;
	//	Vector3d vPoint;
	//	Vector3d vNormal;
	//};

	typedef Vector3d Matrix3x3d[3];
	class BoundingBoxd;
	class BoundingSphered;
	class BaseMathd
	{
    public:
		static int		Factorial(int nNum);
		static int		RoundUpPowOfTwo(int nNum);
		static int		WarpSize(int nIndex, int nLength);
		static void		Matrix3x3Multiply(const Matrix3x3d fInMatrix1, const Matrix3x3d fInMatrix2, Matrix3x3d fOutMatrix);
        static void     RotateAroundVector(Vector3d fRot[3], const bc::Vector3d& vDir, const double& fDegrees);
        static void     RotateAroundVector(Axisd& axis, const Vector3d& vDir, const double& fDegrees);
        static Vector3d	RotatePointAroundVector(const Vector3d& vDir, const Vector3d& vPoint, const double& fDegrees);
		static Vector3d	PerpendicularVector(const Vector3d& vSrc);

		/// <summary>
		static bool IsNotZerof(const float& number);
		static bool isNotZerod(const double& number);

		static bool IsZerof(const float& number);
		static bool isZerod(const double& number);

		static bool IsVector3Equal(const Vector3d& v0, const Vector3d& v1);
		static bool IsVector4Equal(const Vector4d& v0, const Vector4d& v1);
		static bool RayIntersectsTriangle(const RayIntersect& ray, const Vector3d& vertex1, const Vector3d& vertex2, const Vector3d& vertex3, double& distance, Vector3d& normal);
		/// ///////////
		//Double
		static Vector3d	ProjectPointOnPlane(const Vector3d& vPoint, const Vector3d& vNormal);
        static double    Lerp(const double& a, const double& b, const double& f);
		static double    Cerpf(const double& a, const double& b, const double& c, const double& d, const double& x);
		static double	ACos(const double& fValue);
		static double	AngleBetween(const Vector3d& source, const Vector3d& dest);
		static bool		IntersectAndClip(Vector3d& vs, Vector3d& vd, const BoundingBoxd& tBBox);
		static bool		IntersectBoundingSphere(Vector3d& vs, Vector3d& vd, const BoundingSphered& bs);
		static void		IntersectTriangle(RayIntersect& ri, const Vector3d& v1, const Vector3d& v2, const Vector3d& v3, 
			std::vector<IntersectPoint>& vecIP);
		static void		FlipFace(std::vector<Vector3d>& vecPoint);
		static bool		FloatEqual(double a, double b, double tolerance);
		static double	ClampedPixelSize(const Vector4d& vPixelSizeVector, const BoundingSphered& tSphere);
		static bool		SegmentPolygon(const std::vector<Vector3d>& vecVertexOrg, std::vector<std::vector<Vector3d>>& vecConvex);
		template<typename T>
		static bool		CalcBezierCurve(const int& nTimes, const int& nSegment, const std::vector<T>& vecIn, std::vector<T>& vecOut)
		{
			if (nTimes != (vecIn.size() - 1))
			{
				return false;
			}

			for (int i = 0; i <= nSegment; i++)
			{
				double t = double(i) / double(nSegment);

				T v(0.0);

				for (int j = 0; j <= nTimes; j++)
				{
					v += vecIn[j] * double(Factorial(nTimes) / (Factorial(j) * Factorial(nTimes - j)) * pow(t, j) * pow(1 - t, nTimes - j));
				}

				vecOut.push_back(v);
			}

			return true;
		}

		template<typename T>
		static T		CalcBezierCurve(const int& nTimes, const std::vector<T>& vecIn, double fStep)
		{
			if (nTimes != (vecIn.size() - 1))
			{
				return T(0.0f);
			}

			assert(fStep <= 1.0f && fStep >= 0.0f);

			T v(0.0);

			for (int j = 0; j <= nTimes; j++)
			{
				v += vecIn[j] * double(Factorial(nTimes) / (Factorial(j) * Factorial(nTimes - j)) * pow(fStep, j) * pow(1 - fStep, nTimes - j));
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
		static bool		insideConvex(const Vector3d& vIntersection, const std::vector<Vector3d>& vecPolys);
		static bool		insidePolygon(const  Vector3d& vIntersection, const std::vector<Vector3d>& vecPolys);
		static bool		insidePolygon(const int& nPos, const std::vector<Vector3d>& vecPolys);


    };
}
