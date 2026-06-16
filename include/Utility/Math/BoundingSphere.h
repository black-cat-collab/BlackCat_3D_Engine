#pragma once
namespace bc
{
	class BoundingSphere
	{
	public:
		Vector3 _vCenter;
		float    _fRadius;
		BoundingSphere() :
			_vCenter(0.0f, 0.0f, 0.0f), _fRadius(1.0f)
		{

		}
		BoundingSphere(const Vector3& vCenter, const float& fRadius) :
			_vCenter(vCenter), _fRadius(fRadius)
		{

		}

		inline void Set(const Vector3& vCenter, const float& fRadius)
		{
			_vCenter = vCenter;
			_fRadius = fRadius;
		}

		void Set(const BoundingBox& tBBox);

		inline void Set(const Vector3& vMins, const Vector3& vMaxs)
		{
			_vCenter = (vMins + vMaxs) / 2.0f;
			_fRadius = sqrt(0.25 * (vMaxs - vMins).SquaredLength());
		}

		inline const Vector3& GetCenter()  const
		{
			return _vCenter;
		}

		inline const Vector3& GetCenter()
		{
			return _vCenter;
		}

		inline float GetRadius()
		{
			return _fRadius;
		}

		inline float GetRadius() const
		{
			return _fRadius;
		}

		inline bool Valid() const { return _fRadius >= 0.0; }

		inline bool Inside(const Vector3& v)
		{
			return _fRadius > _vCenter.Distance(v);
		}

		void ExpandBy(const Vector3& v)
		{
			if (Valid())
			{
				Vector3 dv = Vector3(v) - _vCenter;
				double r = dv.Length();
				if (r > _fRadius)
				{
					double dr = (r - _fRadius) * 0.5;
					_vCenter += dv * (dr / r);
					_fRadius += dr;
				} // else do nothing as vertex is within sphere.
			}
			else
			{
				_vCenter = v;
				_fRadius = 0.0;
			}
		}

		void ExpandRadiusBy(const Vector3& v)
		{
			if (Valid())
			{
				double r = (Vector3(v) - _vCenter).Length();
				if (r > _fRadius) _fRadius = r;
				// else do nothing as vertex is within sphere.
			}
			else
			{
				_vCenter = v;
				_fRadius = 0.0;
			}
		}

		void ExpandBy(const BoundingSphere& sh)
		{
			// ignore operation if incoming BoundingSphere is invalid.
			if (!sh.Valid()) return;

			// This sphere is not set so use the inbound sphere
			if (!Valid())
			{
				_vCenter = sh._vCenter;
				_fRadius = sh._fRadius;

				return;
			}


			// Calculate d == The distance between the sphere centers
			double d = (_vCenter - sh.GetCenter()).Length();

			// New sphere is already inside this one
			if (d + sh.GetRadius() <= _fRadius)
			{
				return;
			}

			//  New sphere completely contains this one
			if (d + _fRadius <= sh.GetRadius())
			{
				_vCenter = sh._vCenter;
				_fRadius = sh._fRadius;
				return;
			}


			// Build a new sphere that completely contains the other two:
			//
			// The GetCenter point lies halfway along the line between the furthest
			// points on the edges of the two spheres.
			//
			// Computing those two points is ugly - so we'll use similar triangles
			double new_radius = (_fRadius + d + sh.GetRadius()) * 0.5;
			double ratio = (new_radius - _fRadius) / d;

			_vCenter[0] += (sh.GetCenter()[0] - _vCenter[0]) * ratio;
			_vCenter[1] += (sh.GetCenter()[1] - _vCenter[1]) * ratio;
			_vCenter[2] += (sh.GetCenter()[2] - _vCenter[2]) * ratio;

			_fRadius = new_radius;

		}

		void ExpandRadiusBy(const BoundingSphere& sh)
		{
			if (sh.Valid())
			{
				if (Valid())
				{
					double r = (sh._vCenter - _vCenter).Length() + sh._fRadius;
					if (r > _fRadius) _fRadius = r;
					// else do nothing as vertex is within sphere.
				}
				else
				{
					_vCenter = sh._vCenter;
					_fRadius = sh._fRadius;
				}
			}
		}


		void ExpandBy(const BoundingBox& bb);

		void ExpandRadiusBy(const BoundingBox& bb);

	};

}

