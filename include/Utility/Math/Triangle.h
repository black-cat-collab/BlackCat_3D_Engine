#pragma once

namespace bc
{
	const  unsigned int TRIANGLE_INTERSECT = -1;
	const  unsigned int INSIDE_ONE = 0x000001;
	const  unsigned int INSIDE_TWO = 0x000002;
	const  unsigned int INSIDE_THREE = 0x000004;

	const  unsigned int INSIDE_ONE_TWO = INSIDE_ONE | INSIDE_TWO;
	const  unsigned int INSIDE_ONE_THREE = INSIDE_ONE | INSIDE_THREE;
	const  unsigned int INSIDE_TWO_THREE = INSIDE_TWO | INSIDE_THREE;

	const  unsigned int TRI_POSITIVE = INSIDE_ONE;
	const  unsigned int TRI_NEGATIVE = INSIDE_TWO;
	const  unsigned int TRI_INTERSECT = INSIDE_THREE;


	const  unsigned int ONE_POSITIVE = 0x000001;
	const  unsigned int ONE_NEGATIVE = 0x000002;
	const  unsigned int ONE_INTERSECT = 0x000004;

	const  unsigned int TWO_POSITIVE = 0x0000010;
	const  unsigned int TWO_NEGATIVE = 0x0000020;
	const  unsigned int TWO_INTERSECT = 0x0000040;


	const  unsigned int THREE_POSITIVE = 0x00000100;
	const  unsigned int THREE_NEGATIVE = 0x00000200;
	const  unsigned int THREE_INTERSECT = 0x00000400;

	////////////////////////////////////
	//POSITIVE
	const  unsigned int TRIANGLE_POSITIVE = ONE_POSITIVE | TWO_POSITIVE | THREE_POSITIVE;

	const  unsigned int TRI_ONE_P_TWO_P_THREE_I = ONE_POSITIVE | TWO_POSITIVE | THREE_INTERSECT;
	const  unsigned int TRI_ONE_P_TWO_I_THREE_P = ONE_POSITIVE | TWO_INTERSECT | THREE_POSITIVE;
	const  unsigned int TRI_ONE_I_TWO_P_THREE_P = ONE_INTERSECT | TWO_POSITIVE | THREE_POSITIVE;

	const  unsigned int TRI_ONE_P_TWO_I_THREE_I = ONE_POSITIVE | TWO_INTERSECT | THREE_INTERSECT;
	const  unsigned int TRI_ONE_I_TWO_P_THREE_I = ONE_INTERSECT | TWO_POSITIVE | THREE_INTERSECT;
	const  unsigned int TRI_ONE_I_TWO_I_THREE_P = ONE_INTERSECT | TWO_INTERSECT | THREE_POSITIVE;


	////////////////////////////////////
	//NEGATIVE
	const  unsigned int TRIANGLE_NEGATIVE = ONE_NEGATIVE | TWO_NEGATIVE | THREE_NEGATIVE;
	const  unsigned int TRI_ONE_N_TWO_N_THREE_I = ONE_NEGATIVE | TWO_NEGATIVE | THREE_INTERSECT;
	const  unsigned int TRI_ONE_N_TWO_I_THREE_N = ONE_NEGATIVE | TWO_INTERSECT | THREE_NEGATIVE;
	const  unsigned int TRI_ONE_I_TWO_N_THREE_N = ONE_INTERSECT | TWO_NEGATIVE | THREE_NEGATIVE;

	const  unsigned int TRI_ONE_N_TWO_I_THREE_I = ONE_NEGATIVE | TWO_INTERSECT | THREE_INTERSECT;
	const  unsigned int TRI_ONE_I_TWO_N_THREE_I = ONE_INTERSECT | TWO_NEGATIVE | THREE_INTERSECT;
	const  unsigned int TRI_ONE_I_TWO_I_THREE_N = ONE_INTERSECT | TWO_INTERSECT | THREE_NEGATIVE;

	////////////////////////////////////
	// INTERSECT TWO POINT AT POSITIVE
	const  unsigned int TRI_ONE_P_TWO_P_THREE_N = ONE_POSITIVE | TWO_POSITIVE | THREE_NEGATIVE;
	const  unsigned int TRI_ONE_P_TWO_N_THREE_P = ONE_POSITIVE | TWO_NEGATIVE | THREE_POSITIVE;
	const  unsigned int TRI_ONE_N_TWO_P_THREE_P = ONE_NEGATIVE | TWO_POSITIVE | THREE_POSITIVE;

	////////////////////////////////////
	// INTERSECT TWO POINT AT NEGATIVE
	const  unsigned int TRI_ONE_N_TWO_N_THREE_P = ONE_NEGATIVE | TWO_NEGATIVE | THREE_POSITIVE;
	const  unsigned int TRI_ONE_N_TWO_P_THREE_N = ONE_NEGATIVE | TWO_POSITIVE | THREE_NEGATIVE;
	const  unsigned int TRI_ONE_P_TWO_N_THREE_N = ONE_POSITIVE | TWO_NEGATIVE | THREE_NEGATIVE;

	////////////////////////////////////////////////
	// POSITIVE-ON-NEGATIVE
	const  unsigned int TRI_ONE_P_TWO_I_THREE_N = ONE_POSITIVE | TWO_INTERSECT | THREE_NEGATIVE;
	const  unsigned int TRI_ONE_P_TWO_N_THREE_I = ONE_POSITIVE | TWO_NEGATIVE | THREE_INTERSECT;

	const  unsigned int TRI_ONE_N_TWO_P_THREE_I = ONE_NEGATIVE | TWO_POSITIVE | THREE_INTERSECT;
	const  unsigned int TRI_ONE_N_TWO_I_THREE_P = ONE_NEGATIVE | TWO_INTERSECT | THREE_POSITIVE;

	const  unsigned int TRI_ONE_I_TWO_P_THREE_N = ONE_INTERSECT | TWO_POSITIVE | THREE_NEGATIVE;
	const  unsigned int TRI_ONE_I_TWO_N_THREE_P = ONE_INTERSECT | TWO_NEGATIVE | THREE_POSITIVE;

	class TriangleIndex
	{
	public:
		unsigned  a, b, c;
	public:
		inline TriangleIndex& operator = (const TriangleIndex& v)
		{
			a = v.a;
			b = v.b;
			c = v.c;
			return *this;
		}
	};
	class TriangleList
	{
	public:
		std::vector< TriangleIndex> vecTri;
	};
	class TriangleVertex
	{
	public:
		Vector3					vertex;
		bc::Vector2             p2D;
		Vector3					normal;
		std::vector<Vector2>	vecUV;
		int						nUVCount;
	public:
		TriangleVertex()
		{
			for (int i = 0; i < 4; ++i)
			{
				vecUV.push_back(Vector2(0.0));
			}
			nUVCount = 1;
		}
		inline TriangleVertex& operator = (const TriangleVertex& rhs)
		{
			vertex = rhs.vertex;
			normal = rhs.normal;
			p2D    = rhs.p2D;
			vecUV.clear();
			vecUV.assign(rhs.vecUV.begin(), rhs.vecUV.end());
			nUVCount = rhs.nUVCount;
			return *this;

		}
		~TriangleVertex() 
		{

		}
		
		void SetVertex(const Vector3& v)
		{
			vertex = v;
		}
		const Vector3& GetVertex()
		{
			return vertex;
		}

		void SetUV(const Vector2& uv, const int& nIndex) 
		{
			if (nIndex >= vecUV.size())
			{
				vecUV.push_back(uv);
			}
			else
			{
				vecUV[nIndex] = uv;
			}

		}
		Vector2 GetUV(const int& nIndex)
		{
			return vecUV[nIndex];
		}
		const Vector3& GetNormal()
		{
			return normal;
		}
		void SetNormal(const Vector3& n)
		{
			normal = n;
		}
	};

	typedef int (*PFNNMCOMPARED)(const TriangleVertex* v0, const TriangleVertex* v1);


	class IMesh;
	class Triangle
	{
	public:
		TriangleVertex vertex[3];
		const Vector3& GetVertex(const int& nIndex) const
		{
			return vertex[nIndex].vertex;
		}
		const Vector2& GetVertexUV(const int& nIndex, const int& nUVIndex) const
		{
			return vertex[nIndex].vecUV[nUVIndex];
		}
		const int& GetVertexUVCount(const int& nIndex) const
		{
			return vertex[nIndex].nUVCount;
		}
		const Vector3& GetVertexNormal(const int& nIndex) const
		{
			return vertex[nIndex].normal;
		}
		TriangleVertex& operator [] (const int& nIndex)
		{
			return vertex[nIndex];
		}
		Vector3 GetNormal();
		bool LinearIntersectTriangle(const Vector3& base, const Vector3& direction, Vector3& result) const;
		unsigned int Side(Plane& pln);
		bool Inside(const Vector3& v) const;
		bool Intersect(const BoundingBox& tBox) const;
		bool Intersect(const Vector3& point1, const Vector3& point2) const;

	/*	bool SplitFace(Triangle& tri, Planed& pln, unsigned int dFlag, std::vector<Triangle>& posi,
			std::vector<Triangle>& nega);*/

		static bool SplitFace(Triangle& tri, Plane& pln, unsigned int dFlag, std::vector<Triangle>& posi,
			std::vector<Triangle>& nega);

		static bool SplitFaceInterSet(Triangle& tri, Plane& pln, unsigned int dFlag, std::vector<Triangle>& posi,
			std::vector<Triangle>& nega, std::vector<Vector3>& vecInterSet);

		static bool SplitTriangle(std::vector<bc::Triangle>& vecTriangle, Plane& pln, std::vector<Triangle>& cPositiveSide,
			std::vector<Triangle>& cNegativeSide);

		static bool SplitTriangleInterSet(std::vector<Triangle>& vecTriangle, Plane& pln, std::vector<Triangle>& cPositiveSide,
			std::vector<Triangle>& cNegativeSide, std::vector<Vector3>& vecInterSet);
	
	};
	

	
}

