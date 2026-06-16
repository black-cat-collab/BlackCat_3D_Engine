#pragma once

#include <vector>

namespace bc
{
	class BoundingBox2D;
	class BoundingBox2Dd
	{
	public:
		Vector2d _vMins;
		Vector2d _vMaxs;
	public:
		BoundingBox2Dd()
		{
			Clear();
		}

		BoundingBox2Dd(const Vector2d& vMins, const Vector2d& vMaxs) :
			_vMins(vMins), _vMaxs(vMaxs)
		{

		}

		inline BoundingBox2D Getf()
		{
			BoundingBox2D bb ;
			bb._vMins = Vector2(_vMins.x, _vMins.y);
			bb._vMaxs = Vector2(_vMaxs.x, _vMaxs.y);

			return bb;
		}

		inline void Set(const Vector2d& vMins, const Vector2d& vMaxs)
		{
			_vMins = vMins;
			_vMaxs = vMaxs;
		}

		inline BoundingBox2Dd& operator = (const BoundingBox2Dd& bb)
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

		/** Return true if this bounding box intersects the specified bounding box. */
		inline bool Intersect(const BoundingBox2Dd& bb) const
		{
			return MAX(_vMins.x, bb._vMins.x) <= MIN(_vMaxs.x, bb._vMaxs.x) &&
				MAX(_vMins.y, bb._vMins.y) <= MIN(_vMaxs.y, bb._vMaxs.y);
		}

		// 判断线段相交
		inline bool Intersect(const Vector2d& p0, const Vector2d& p1)
		{
			Vector2d m = (p0 + p1) / 2.0;
			Vector2d w = m - p0;
			double X = fabs(w.x);
			double Y = fabs(w.y);
			Vector2d vHalf = (_vMaxs - _vMins) / 2.0;
			if (fabs(m.x) > X + vHalf.x)
				return false;
			if (fabs(m.y) > Y + vHalf.y)
				return false;
			if (fabs(m.x * w.y - m.y * w.x) > vHalf.x * Y + vHalf.y * X)
				return false;
			return true;
		}

		inline bool Valid()
		{
			return _vMaxs.x >= _vMins.x && _vMaxs.y >= _vMins.y ;
		}

		inline bool Valid() const
		{
			return _vMaxs.x >= _vMins.x && _vMaxs.y >= _vMins.y;
		}

		inline void ExpandBy(const BoundingBox2Dd& bb)
		{
			if (!bb.Valid()) return;

			if (bb._vMins.x < _vMins.x) _vMins.x = bb._vMins.x;
			if (bb._vMaxs.x > _vMaxs.x) _vMaxs.x = bb._vMaxs.x;

			if (bb._vMins.y < _vMins.y) _vMins.y = bb._vMins.y;
			if (bb._vMaxs.y > _vMaxs.y) _vMaxs.y = bb._vMaxs.y;

		}

		/** Expands the bounding box to include the given coordinate.
		* If the box is uninitialized, set its min and max extents to v. */
		inline void ExpandBy(const Vector2d& v)
		{
			if (v.x < _vMins.x) _vMins.x = v.x;
			if (v.x > _vMaxs.x) _vMaxs.x = v.x;

			if (v.y < _vMins.y) _vMins.y = v.y;
			if (v.y > _vMaxs.y) _vMaxs.y = v.y;

		}


		inline double Length() const
		{
			return fabs(_vMaxs.y - _vMins.y);
		}

		inline double Width() const
		{
			return fabs(_vMaxs.x - _vMins.x);
		}

		inline double MaxEdge() const
		{
			double fSelectedMaxValue = 0.0;
			fSelectedMaxValue = _vMaxs.x - _vMins.x;
			double fLength = _vMaxs.y - _vMins.y;
			if (fLength > fSelectedMaxValue)
			{
				fSelectedMaxValue = fLength;
			}
		
			return fSelectedMaxValue;
		}

		inline double GetRadius() const
		{
			return sqrt(GetRadius2());
		}

		/** Calculates and returns the squared length of the bounding box radius.
		* Note, radius2() is faster to calculate than radius(). */
		inline double GetRadius2() const
		{
			return 0.25 * ((_vMaxs - _vMins).SquaredLength());
		}

		Vector2d GetCenter()		const
		{
			return  (_vMins + _vMaxs) / 2.0;
		}

		inline void Clear()
		{
			_vMins = Vector2d(999999999.0, 999999999.0);
			_vMaxs = Vector2d(-999999999.0, -999999999.0);
		}

		inline bool Inside(const Vector2d& v) const
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


		inline bool Inside(const bc::BoundingBox2Dd& v) const
		{
			if (v._vMins.x >= _vMins.x &&
				v._vMins.y >= _vMins.y &&
				v._vMaxs.x <= _vMaxs.x &&
				v._vMaxs.y <= _vMaxs.y )
			{
				return true;
			}

			return false;
		}

		inline bool InsideOffset(const Vector2d& vC, const Vector2d& vOff) const
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

		inline bool InsideOffset(const Vector2d& vC, const Vector2d& vOff)
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

		inline const Vector2d GetCorner(unsigned int pos) const
		{
			return Vector2d(pos & 1 ? _vMaxs.x : _vMins.x, pos & 2 ? _vMaxs.y : _vMins.y);
		}

		inline Vector2d GetCorner(unsigned int pos)
		{
			return Vector2d(pos & 1 ? _vMaxs.x : _vMins.x, pos & 2 ? _vMaxs.y : _vMins.y);
		}

		void Set(const Vector2d& vCenter, const double& fL, const double& fW);

		void Add(const BoundingBox2Dd& cBBox);

		void Add(BoundingBox2Dd& box);

		void Add(const Vector2d& v);
	};
}

