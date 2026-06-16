#pragma once

#include <vector>

namespace bc
{
	struct TriangleFace
	{
		Vector3  a;
		Vector3  b;
		Vector3  c;

		TriangleFace()
		{

		}

		TriangleFace(const float& a1, const float& a2, const float& a3,
			const float& b1, const float& b2, const float& b3,
			const float& c1, const float& c2, const float& c3)
		{
			Set(a1, a2, a3, b1, b2, b3, c1, c2, c3);
		}

		void Set(const float& a1, const float& a2, const float& a3,
			const float& b1, const float& b2, const float& b3,
			const float& c1, const float& c2, const float& c3)
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

	class Plane;
	class Triangle;
	class BoundingSphere;
	class BoundingBox
	{
	public:
		Vector3 _vMins;
		Vector3 _vMaxs;
	public:
		BoundingBox()
		{
			Clear();
		}

		BoundingBox(const Vector3& vMins, const Vector3& vMaxs) :
			_vMins(vMins), _vMaxs(vMaxs)
		{

		}

		inline void Set(const Vector3& vMins, const Vector3& vMaxs)
		{
			_vMins = vMins;
			_vMaxs = vMaxs;
		}

		inline BoundingBox& operator = (const BoundingBox& bb)
		{
			_vMins = bb._vMins;
			_vMaxs = bb._vMaxs;
			return *this;
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
		inline bool Intersect(const BoundingBox& bb) const
		{
			return MAX(_vMins.x, bb._vMins.x) <= MIN(_vMaxs.x, bb._vMaxs.x) &&
				MAX(_vMins.y, bb._vMins.y) <= MIN(_vMaxs.y, bb._vMaxs.y) &&
				MAX(_vMins.z, bb._vMins.z) <= MIN(_vMaxs.z, bb._vMaxs.z);
		}
		// 判断线段相交
		inline bool Intersect(const Vector3& p0, const Vector3& p1) const
		{
			Vector3 m = (p0 + p1) / 2.0;
			Vector3 w = m - p0;
			float X = fabs(w.x);
			float Y = fabs(w.y);
			float Z = fabs(w.z);
			Vector3 vHalf = (_vMaxs - _vMins) / 2.0;
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

		inline void ExpandBy(const BoundingBox& bb)
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
		inline void ExpandBy(const Vector3& v)
		{
			if (v.x < _vMins.x) _vMins.x = v.x;
			if (v.x > _vMaxs.x) _vMaxs.x = v.x;

			if (v.y < _vMins.y) _vMins.y = v.y;
			if (v.y > _vMaxs.y) _vMaxs.y = v.y;

			if (v.z < _vMins.z) _vMins.z = v.z;
			if (v.z > _vMaxs.z) _vMaxs.z = v.z;
		}

		inline float Height() const
		{
			return fabs((double)_vMaxs.z - _vMins.z);
		}

		inline float Length() const
		{
			return fabs((double)_vMaxs.y - _vMins.y);
		}

		inline float Width() const
		{
			return fabs((double)_vMaxs.x - _vMins.x);
		}

		inline float MaxEdge() const
		{
			float fSelectedMaxValue = 0.0f;
			fSelectedMaxValue = _vMaxs.x - _vMins.x;
			float fLength = _vMaxs.y - _vMins.y;
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

		inline const Vector3 GetCorner(unsigned int pos) const
		{
			return Vector3(pos & 1 ? _vMaxs.x : _vMins.x, pos & 2 ? _vMaxs.y : _vMins.y, pos & 4 ? _vMaxs.z : _vMins.z);
		}

		inline Vector3 GetCorner(unsigned int pos)
		{
			return Vector3(pos & 1 ? _vMaxs.x : _vMins.x, pos & 2 ? _vMaxs.y : _vMins.y, pos & 4 ? _vMaxs.z : _vMins.z);
		}

		inline float GetRadius() const
		{
			return sqrt(GetRadius2());
		}

		/** Calculates and returns the squared length of the bounding box radius.
		* Note, radius2() is faster to calculate than radius(). */
		inline float GetRadius2() const
		{
			return 0.25f * ((_vMaxs - _vMins).SquaredLength());
		}

		Vector3 GetCenter()		const
		{
			return  (_vMins + _vMaxs) / 2.0f;
		}

		inline void Clear()
		{
			_vMins = Vector3(999999999.0f, 999999999.0f, 999999999.0f);
			_vMaxs = Vector3(-999999999.0f, -999999999.0f, -999999999.0f);
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

		inline bool Inside(const Vector3& v) const
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

		inline bool Inside(const bc::BoundingBox& v) const
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

		inline bool InsideOffset(const Vector3& vC, const Vector3& vOff)
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

		void Set(const BoundingSphere& bs);

		void Set(const Vector3& vCenter, const float& fL, const float& fW, const float& fH);

		void Add(const BoundingBox& cBBox, const Matrix4& m);

		void Add(const BoundingBox& cBBox);

		void Add(BoundingBox& box);

		void Add(const Vector3& v);

		BoundingBox Transform(const Matrix4& m) const;

		void BuildCollisionFace(std::vector<TriangleFace>& vecBS) const;

		void GetPlane(std::vector<Plane>& vecPlane) const;

		void GetTriangles(std::vector<Triangle>& outTris) const;

		void GetLines(std::vector<Vector3>& outLines) const;

		void GetLinesElement(std::vector<Vector3>& outLines, std::vector<uint32_t>& vecElem) const;
	};
}

