#pragma once

#include <vector>

namespace bc
{
	struct TriangleFaced
	{
		Vector3d  a;
		Vector3d  b;
		Vector3d  c;

		TriangleFaced()
		{

		}

		TriangleFaced(const double& a1, const double& a2, const double& a3,
			const double& b1, const double& b2, const double& b3,
			const double& c1, const double& c2, const double& c3)
		{
			Set(a1, a2, a3, b1, b2, b3, c1, c2, c3);
		}

		void Set(const double& a1, const double& a2, const double& a3,
			const double& b1, const double& b2, const double& b3,
			const double& c1, const double& c2, const double& c3)
		{
			a.x = a1;
			a.y = a2;
			a.z = a3;

			b.x = b1;
			b.y = b2;
			b.z = b3;

			c.x = c1;
			c.y = c2;
			c.z = c3;
		}
	};

	class Planed;
	class BoundingSphered;
	class Triangle;
	class BoundingBoxd
	{
	public:
		Vector3d _vMins;
		Vector3d _vMaxs;
	public:
		BoundingBoxd()
		{
			Clear();
		}


		BoundingBoxd(const BoundingBox& bb)
		{
			*this = bb;
		}

		BoundingBoxd(const Vector3d& vMins, const Vector3d& vMaxs) :
			_vMins(vMins), _vMaxs(vMaxs)
		{

		}

		inline void Set(const Vector3d& vMins, const Vector3d& vMaxs)
		{
			_vMins = vMins;
			_vMaxs = vMaxs;
		}

		inline BoundingBoxd& operator = (const BoundingBoxd& bb)
		{
			_vMins = bb._vMins;
			_vMaxs = bb._vMaxs;
			return *this;
		}

		inline BoundingBoxd& operator = (const BoundingBox& bb)
		{
			_vMins = bb._vMins;
			_vMaxs = bb._vMaxs;
			return *this;
		}

		inline BoundingBox Getf()
		{
			BoundingBox bb;
			bb._vMins = _vMins;
			bb._vMaxs = _vMaxs;
			return bb;
		}

		inline bool IsClear() const
		{
			if ((_vMins[0] == 999999999.0f && _vMins[1] == 999999999.0f && _vMins[2] == 999999999.0f)
				&& (_vMaxs[0] == -999999999.0f && _vMaxs[1] == -999999999.0f && _vMaxs[2] == -999999999.0f))
			{
				return true;
			}
			return false;
		}

		// 判断box相交
		inline bool Intersect(const BoundingBoxd& bb) const
		{
			return MAX(_vMins.x, bb._vMins.x) <= MIN(_vMaxs.x, bb._vMaxs.x) &&
				MAX(_vMins.y, bb._vMins.y) <= MIN(_vMaxs.y, bb._vMaxs.y) &&
				MAX(_vMins.z, bb._vMins.z) <= MIN(_vMaxs.z, bb._vMaxs.z);
		}
		// 判断线段相交
		inline bool Intersect(const Vector3d& p0, const Vector3d& p1) const
		{
			Vector3d m = (p0 + p1) / 2.0;
			Vector3d w = m - p0;
			double X = fabs(w.x);
			double Y = fabs(w.y);
			double Z = fabs(w.z);
			Vector3d vHalf = (_vMaxs - _vMins) / 2.0;
			if (fabs(m.x) > X + vHalf.x)
				return false;
			if (fabs(m.y) > Y + vHalf.y)
				return false;
			if (fabs(m.z) > Z + vHalf.z)
				return false;
			if (fabs(m.y * w.z - m.z * w.y) > vHalf.y * Z + vHalf.z * Y)
				return false;
			if (fabs(m.x * w.z - m.z * w.x) > vHalf.x * Z + vHalf.z * X)
				return false;
			if (fabs(m.x * w.y - m.y * w.x) > vHalf.x * Y + vHalf.y * X)
				return false;
			return true;
		}
		inline bool Valid()
		{
			return _vMaxs.x >= _vMins.x && _vMaxs.y >= _vMins.y && _vMaxs.z >= _vMins.z;
		}

		inline bool Valid() const
		{
			return _vMaxs.x >= _vMins.x && _vMaxs.y >= _vMins.y && _vMaxs.z >= _vMins.z;
		}

		inline void ExpandBy(const BoundingBoxd& bb)
		{
			if (!bb.Valid()) return;

			if (bb._vMins.x < _vMins.x) _vMins.x = bb._vMins.x;
			if (bb._vMaxs.x > _vMaxs.x) _vMaxs.x = bb._vMaxs.x;

			if (bb._vMins.y < _vMins.y) _vMins.y = bb._vMins.y;
			if (bb._vMaxs.y > _vMaxs.y) _vMaxs.y = bb._vMaxs.y;

			if (bb._vMins.z < _vMins.z) _vMins.z = bb._vMins.z;
			if (bb._vMaxs.z > _vMaxs.z) _vMaxs.z = bb._vMaxs.z;
		}

		/** Expands the bounding box to include the given coordinate.
		* If the box is uninitialized, set its min and max extents to v. */
		inline void ExpandBy(const Vector3d& v)
		{
			if (v.x < _vMins.x) _vMins.x = v.x;
			if (v.x > _vMaxs.x) _vMaxs.x = v.x;

			if (v.y < _vMins.y) _vMins.y = v.y;
			if (v.y > _vMaxs.y) _vMaxs.y = v.y;

			if (v.z < _vMins.z) _vMins.z = v.z;
			if (v.z > _vMaxs.z) _vMaxs.z = v.z;
		}

		inline double Height() const
		{
			return fabs((double)_vMaxs.z - _vMins.z);
		}

		inline double Length() const
		{
			return fabs((double)_vMaxs.y - _vMins.y);
		}

		inline double Width() const
		{
			return fabs((double)_vMaxs.x - _vMins.x);
		}

		inline double MaxEdge() const
		{
			double fSelectedMaxValue = 0.0f;
			fSelectedMaxValue = _vMaxs.x - _vMins.x;
			double fLength = _vMaxs.y - _vMins.y;
			if (fLength > fSelectedMaxValue)
			{
				fSelectedMaxValue = fLength;
			}
			fLength = _vMaxs.z - _vMins.z;
			if (fLength > fSelectedMaxValue)
			{
				fSelectedMaxValue = fLength;
			}

			return fSelectedMaxValue;
		}

		inline const Vector3d GetCorner(unsigned int pos) const
		{
			return Vector3d(pos & 1 ? _vMaxs.x : _vMins.x, pos & 2 ? _vMaxs.y : _vMins.y, pos & 4 ? _vMaxs.z : _vMins.z);
		}

		inline Vector3d GetCorner(unsigned int pos)
		{
			return Vector3d(pos & 1 ? _vMaxs.x : _vMins.x, pos & 2 ? _vMaxs.y : _vMins.y, pos & 4 ? _vMaxs.z : _vMins.z);
		}

		inline double GetRadius() const
		{
			return sqrt(GetRadius2());
		}

		/** Calculates and returns the squared length of the bounding box radius.
		* Note, radius2() is faster to calculate than radius(). */
		inline double GetRadius2() const
		{
			return 0.25f * ((_vMaxs - _vMins).SquaredLength());
		}

		Vector3d GetCenter()		const
		{
			return  (_vMins + _vMaxs) / 2.0f;
		}

		inline void Clear()
		{
			_vMins = Vector3d(999999999.0f, 999999999.0f, 999999999.0f);
			_vMaxs = Vector3d(-999999999.0f, -999999999.0f, -999999999.0f);
		}

		inline bool Inside(const Vector2& v) const
		{
			if (v[0] >= _vMins[0] &&
				v[1] >= _vMins[1] &&
				v[0] <= _vMaxs[0] &&
				v[1] <= _vMaxs[1])
			{
				return true;
			}
			return false;
		}

		inline bool Inside(const Vector3d& v) const
		{
			if (v[0] >= _vMins[0] &&
				v[1] >= _vMins[1] &&
				v[2] >= _vMins[2] &&
				v[0] <= _vMaxs[0] &&
				v[1] <= _vMaxs[1] &&
				v[2] <= _vMaxs[2])
			{
				return true;
			}

			return false;
		}

		inline bool Inside(const bc::BoundingBoxd& v) const
		{
			if (v._vMins.x >= _vMins.x &&
				v._vMins.y >= _vMins.y &&
				v._vMins.z >= _vMins.z &&
				v._vMaxs.x <= _vMaxs.x &&
				v._vMaxs.y <= _vMaxs.y &&
				v._vMaxs.z <= _vMaxs.z)
			{
				return true;
			}

			return false;
		}

		inline bool InsideOffset(const Vector2& vC, const Vector2& vOff) const
		{
			if (vC[0] >= (_vMins[0] - vOff[0]) &&
				vC[1] >= (_vMins[1] - vOff[1]) &&
				vC[0] <= (_vMaxs[0] + vOff[0]) &&
				vC[1] <= (_vMaxs[1] + vOff[1]))
			{
				return true;
			}
			return false;
		}

		inline bool InsideOffset(const Vector3d& vC, const Vector3d& vOff)
		{
			if (vC[0] >= (_vMins[0] - vOff[0]) &&
				vC[1] >= (_vMins[1] - vOff[1]) &&
				vC[2] >= (_vMins[2] - vOff[2]) &&
				vC[0] <= (_vMaxs[0] + vOff[0]) &&
				vC[1] <= (_vMaxs[1] + vOff[1]) &&
				vC[2] <= (_vMaxs[2] + vOff[2]))
			{
				return true;
			}
			return false;
		}
		void GetTriangles(std::vector<Triangle>& outTris) const;
		void GetLines(std::vector<Vector3>& outLines) const;
		void GetLines(std::vector<Vector3d>& outLines) const;
		void Set(const BoundingSphered& bs);

		void Set(const Vector3d& vCenter, const double& fL, const double& fW, const double& fH);

		void Add(const BoundingBoxd& cBBox, const Matrix4d& m);

		void Add(const BoundingBoxd& cBBox);

		void Add(BoundingBoxd& box);

		void Add(const Vector3d& v);

		void Add(const Vector3& v);

		BoundingBoxd Transform(const Matrix4d& m) const;

		void BuildCollisionFace(std::vector<TriangleFaced>& vecBS) const;

		void GetPlane(std::vector<Planed>& vecPlane) const;
	};
}

