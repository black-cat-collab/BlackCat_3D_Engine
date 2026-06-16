#pragma once

namespace bc
{
	enum class PlaneType_e
	{
		PLANE_X = 0,
		PLANE_Y,
		PLANE_Z,
		PLANE_NON_AXIAL
	};

	enum class PlaneSide_e
	{
		NO_SIDE = 0,
		POSITIVE_SIDE,
		NEGATIVE_SIDE,
		INTERSECT
	};

	class Plane
	{
	private:
		Vector3			_vNormal;
		float			_fDist;
		PlaneType_e		_eType;
		unsigned char	_bySignbits;

		unsigned int	_upperBBCorner;
		unsigned int	_lowerBBCorner;
	public:
		Plane();
		~Plane();
		Plane(const float x, const float y, const float z, const float fDis);
		Plane(const Vector3& vNormal, const Vector3& vPoint);
		Plane(const Vector3& vPoint0, const Vector3& vPoint1, const Vector3& vPoint2);
		inline void Set(const float& x, const float& y, const float& z, const float& fDis)
		{
			_vNormal.Set(x, y, z);
			_fDist = fDis;
			CalculateUpperLowerBBCorners();
		}

		inline void CalculateUpperLowerBBCorners()
		{
			_upperBBCorner = (_vNormal[0] >= 0.0 ? 1 : 0) |
				(_vNormal[1] >= 0.0 ? 2 : 0) |
				(_vNormal[2] >= 0.0 ? 4 : 0);
			_lowerBBCorner = (~_upperBBCorner) & 7;
		}

		bool Equal(const Vector3& vNormal, float fDist);
		bool Equal(const Plane& other);
		bool Valid();
		
		void Redefine(const Vector3& vNormal, const Vector3& vPoint);
		void Redefine(const Vector3& vPoint0, const Vector3& vPoint1, const Vector3& vPoint2);
		
		const Vector3& GetNormal() const
		{
			return _vNormal;
		}

		void SetNormal(const Vector3& v)
		{
			_vNormal = v;
		}

		void SetDistance(const float& fDist)
		{
			_fDist = fDist;
		}

		float GetDistance() const
		{
			return _fDist;
		}

		float GetDistance(const Vector3& rPoint) const
		{
			return _vNormal.DotProduct(rPoint) + _fDist;
		}

		void SetType(const PlaneType_e& eType)
		{
			_eType = eType;
		}

		const PlaneType_e& GetType() const
		{
			return _eType;
		}

		void SetSignbits(const unsigned char& bSign)
		{
			_bySignbits = bSign;
		}

		const unsigned char& GetSignbits() const
		{
			return _bySignbits;
		}

		void CalculateSignbits();

		inline void Flip()
		{
			_vNormal[0] = -_vNormal[0];
			_vNormal[1] = -_vNormal[1];
			_vNormal[2] = -_vNormal[2];
			_fDist = -_fDist;
			CalculateUpperLowerBBCorners();
		}

		inline void MakeUnitLength()
		{
			float inv_length = 1.0 / sqrt(_vNormal[0] * _vNormal[0] + _vNormal[1] * _vNormal[1] + _vNormal[2] * _vNormal[2]);
			_vNormal[0] *= inv_length;
			_vNormal[1] *= inv_length;
			_vNormal[2] *= inv_length;
			_fDist *= inv_length;
		}

		inline void TransformProvidingInverse(const Matrix4& matrix)
		{
			// note pre multiplications, which effectively transposes matrix.
			Vector4 vec(_vNormal[0], _vNormal[1], _vNormal[2], _fDist);
			vec = matrix * vec;
			Set(vec.x, vec.y, vec.z, vec.w);
			MakeUnitLength();
		}

		inline float DotProductNormal(const Vector3& v) const
		{
			return _vNormal[0] * v.x +
				_vNormal[1] * v.y +
				_vNormal[2] * v.z;
		}

		inline Vector3 Mirror(const Vector3& vPoint) const
		{
			float fDis = GetDistance(vPoint);

			Vector3 v3 = vPoint - _vNormal * fDis * 2;

			return v3;
		}

		PlaneSide_e Intersect(const BoundingBox& bb) const;
		PlaneSide_e Intersect(const BoundingSphere& bs) const;
		PlaneSide_e Intersect(const std::vector<Vector3>& vertices) const;
		bool Intersect(const Vector3& vStart, const Vector3& vEnd, Vector3& vIntersectPoint);
		PlaneType_e GetTypeFromNormal(const Vector3& vNormal);
		PlaneSide_e GetSide(const Vector3& vPoint);
		PlaneSide_e GetSide(const BoundingBox& tBBox);
		void ToTextureAxis(Vector3& vX, Vector3& vY);
	};
}

