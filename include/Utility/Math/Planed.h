#pragma once

namespace bc
{

	class Planed
	{
	private:
		Vector3d			_vNormal;
		double			_fDist;
		PlaneType_e		_eType;
		unsigned char	_bySignbits;

		unsigned int	_upperBBCorner;
		unsigned int	_lowerBBCorner;
	public:
		Planed();
		~Planed();
		Planed(const double x, const double y, const double z, const double fDis);
		Planed(const Vector3d& vNormal, const Vector3d& vPoint);
		Planed(const Vector3d& vPoint0, const Vector3d& vPoint1, const Vector3d& vPoint2);
		inline void Set(const double& x, const double& y, const double& z, const double& fDis)
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

		bool Equal(const Vector3d& vNormal, double fDist);
		bool Equal(const Planed& other);
		bool Valid();
		
		void Redefine(const Vector3d& vNormal, const Vector3d& vPoint);
		void Redefine(const Vector3d& vPoint0, const Vector3d& vPoint1, const Vector3d& vPoint2);
		
		const Vector3d& GetNormal() const
		{
			return _vNormal;
		}

		void SetNormal(const Vector3d& v)
		{
			_vNormal = v;
		}

		void SetDistance(const double& fDist)
		{
			_fDist = fDist;
		}

		double GetDistance() const
		{
			return _fDist;
		}

		double GetDistance(const Vector3d& rPoint) const
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
			double inv_length = 1.0 / sqrt(_vNormal[0] * _vNormal[0] + _vNormal[1] * _vNormal[1] + _vNormal[2] * _vNormal[2]);
			_vNormal[0] *= inv_length;
			_vNormal[1] *= inv_length;
			_vNormal[2] *= inv_length;
			_fDist *= inv_length;
		}

		inline void TransformProvidingInverse(const Matrix4d& matrix)
		{
			// note pre multiplications, which effectively transposes matrix.
			Vector4 vec(_vNormal[0], _vNormal[1], _vNormal[2], _fDist);
			vec = matrix * vec;
			Set(vec.x, vec.y, vec.z, vec.w);
			MakeUnitLength();
		}

		inline double DotProductNormal(const Vector3d& v) const
		{
			return _vNormal[0] * v.x +
				_vNormal[1] * v.y +
				_vNormal[2] * v.z;
		}

		inline Vector3d Mirror(const Vector3d& vPoint) const
		{
			double fDis = GetDistance(vPoint);

			Vector3d v3 = vPoint - _vNormal * fDis * 2;

			return v3;
		}

		PlaneSide_e Intersect(const BoundingBoxd& bb) const;
		PlaneSide_e Intersect(const BoundingSphered& bs) const;
		PlaneSide_e Intersect(const std::vector<Vector3d>& vertices) const;
		bool Intersect(const Vector3d& vStart, const Vector3d& vEnd, Vector3d& vIntersectPoint);
		bool Intersect(const Vector3d& vStart, const Vector3d& vEnd, Vector3d& vIntersectPoint) const;
		PlaneType_e GetTypeFromNormal(const Vector3d& vNormal);
		PlaneSide_e GetSide(const Vector3d& vPoint);
		PlaneSide_e GetSide(const BoundingBoxd& tBBox);
		void ToTextureAxis(Vector3d& vX, Vector3d& vY);
	};
}

