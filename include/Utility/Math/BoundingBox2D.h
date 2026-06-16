#pragma once

#include <vector>

namespace bc
{
	class BoundingBox2D
	{
	public:
		Vector2 _vMins;
		Vector2 _vMaxs;
	public:
		BoundingBox2D()
		{
			Clear();
		}

		BoundingBox2D(const Vector2& vMins, const Vector2& vMaxs) :
			_vMins(vMins), _vMaxs(vMaxs)
		{

		}

		inline void Set(const Vector2& vMins, const Vector2& vMaxs)
		{
			_vMins = vMins;
			_vMaxs = vMaxs;
		}

		inline BoundingBox2D& operator = (const BoundingBox2D& bb)
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
		inline bool Intersect(const BoundingBox2D& bb) const
		{
			return MAX(_vMins.x, bb._vMins.x) <= MIN(_vMaxs.x, bb._vMaxs.x) &&
				MAX(_vMins.y, bb._vMins.y) <= MIN(_vMaxs.y, bb._vMaxs.y);
		}

		// 判断线段相交
		inline bool Intersect(const Vector2& p0, const Vector2& p1)
		{
			Vector2 m = (p0 + p1) / 2.0;
			Vector2 w = m - p0;
			float X = fabs(w.x);
			float Y = fabs(w.y);
			Vector2 vHalf = (_vMaxs - _vMins) / 2.0;
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

		inline void ExpandBy(const BoundingBox2D& bb)
		{
			if (!bb.Valid()) return;

			if (bb._vMins.x < _vMins.x) _vMins.x = bb._vMins.x;
			if (bb._vMaxs.x > _vMaxs.x) _vMaxs.x = bb._vMaxs.x;

			if (bb._vMins.y < _vMins.y) _vMins.y = bb._vMins.y;
			if (bb._vMaxs.y > _vMaxs.y) _vMaxs.y = bb._vMaxs.y;

		}

		/** Expands the bounding box to include the given coordinate.
		* If the box is uninitialized, set its min and max extents to v. */
		inline void ExpandBy(const Vector2& v)
		{
			if (v.x < _vMins.x) _vMins.x = v.x;
			if (v.x > _vMaxs.x) _vMaxs.x = v.x;

			if (v.y < _vMins.y) _vMins.y = v.y;
			if (v.y > _vMaxs.y) _vMaxs.y = v.y;

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
		
			return fSelectedMaxValue;
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

		Vector2 GetCenter()		const
		{
			return  (_vMins + _vMaxs) / 2.0f;
		}

		inline void Clear()
		{
			_vMins = Vector2(999999999.0f, 999999999.0f);
			_vMaxs = Vector2(-999999999.0f, -999999999.0f);
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


		inline bool Inside(const bc::BoundingBox2D& v) const
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

		inline bool InsideOffset(const Vector2& vC, const Vector2& vOff)
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

		inline const Vector2 GetCorner(unsigned int pos) const
		{
			return Vector2(pos & 1 ? _vMaxs.x : _vMins.x, pos & 2 ? _vMaxs.y : _vMins.y);
		}

		inline Vector2 GetCorner(unsigned int pos)
		{
			return Vector2(pos & 1 ? _vMaxs.x : _vMins.x, pos & 2 ? _vMaxs.y : _vMins.y);
		}

		void Set(const Vector2& vCenter, const float& fL, const float& fW);

		void Add(const BoundingBox2D& cBBox);

		void Add(BoundingBox2D& box);

		void Add(const Vector2& v);
	};
}

