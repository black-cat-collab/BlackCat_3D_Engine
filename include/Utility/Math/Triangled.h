#pragma once

namespace bc
{
	
	class TriangleVertexd
	{
	public:
		Vector3d					vertex;
		bc::Vector2d             p2D;
		Vector3d					normal;
		std::vector<Vector2d>	vecUV;
		int						nUVCount;
	public:
		TriangleVertexd()
		{
			for (int i = 0; i < 4; ++i)
			{
				vecUV.push_back(Vector2(0.0));
			}
			nUVCount = 1;
		}
		inline TriangleVertexd& operator = (const TriangleVertexd& rhs)
		{
			vertex = rhs.vertex;
			normal = rhs.normal;
			p2D    = rhs.p2D;
			vecUV.clear();
			vecUV.assign(rhs.vecUV.begin(), rhs.vecUV.end());
			nUVCount = rhs.nUVCount;
			return *this;

		}
		~TriangleVertexd()
		{

		}

		TriangleVertex Getf()
		{
			TriangleVertex tvf;
			tvf.vertex = vertex;
			tvf.normal = normal; 
			tvf.p2D = p2D;
			tvf.nUVCount = nUVCount;
			tvf.vecUV.clear();
			for (int i = 0; i < nUVCount; i++)
			{
				tvf.vecUV.push_back(vecUV[i]);

			}
			return tvf;
		}
		
		void SetVertex(const Vector3d& v)
		{
			vertex = v;
		}
		void SetVertex(const Vector3& v)
		{
			vertex = v;
		}
		const Vector3d& GetVertex()
		{
			return vertex;
		}

		void SetUV(const Vector2d& uv, const int& nIndex) 
		{
			 vecUV[nIndex] = uv;
		}
		Vector2d GetUV(const int& nIndex)
		{
			return vecUV[nIndex];
		}
		const Vector3d& GetNormal()
		{
			return normal;
		}
		void SetNormal(const Vector3d& n)
		{
			normal = n;
		}
	};

	typedef int (*PFNNMCOMPAREDd)(const TriangleVertexd* v0, const TriangleVertexd* v1);
	class IMesh;
	class Triangled
	{
	public:
		TriangleVertexd vertex[3];
		const Vector3d& GetVertex(const int& nIndex) const
		{
			return vertex[nIndex].vertex;
		}
		const Vector2d& GetVertexUV(const int& nIndex, const int& nUVIndex) const
		{
			return vertex[nIndex].vecUV[nUVIndex];
		}
		const int& GetVertexUVCount(const int& nIndex) const
		{
			return vertex[nIndex].nUVCount;
		}
		const Vector3d& GetVertexNormal(const int& nIndex) const
		{
			return vertex[nIndex].normal;
		}
		TriangleVertexd& operator [] (const int& nIndex)
		{
			return vertex[nIndex];
		}

		const TriangleVertexd& operator [] (const int& nIndex) const
		{
			return vertex[nIndex];
		}
		Triangle Getf() {
			Triangle t;
			t.vertex[0] = vertex[0].Getf();
			t.vertex[1] = vertex[1].Getf();
			t.vertex[2] = vertex[2].Getf();
			return t;
		}
		Vector3d GetNormal();
		bool LinearIntersectTriangle(const Vector3d& base, const Vector3d& direction, Vector3d& result) const;
		unsigned int Side(Planed& pln);
		bool Inside(const Vector3d& v) const;
		bool Intersect(const BoundingBoxd& tBox) const;
		bool Intersect(const Vector3d& point1, const Vector3d& point2) const;


		static bool SplitFace(Triangled& tri, Planed& pln, unsigned int dFlag, std::vector<Triangled>& posi,
			std::vector<Triangled>& nega);

		//static bool SplitFace(Triangled& tri, Planed& pln, unsigned int dFlag, std::vector<Triangled>& posi,
		//	std::vector<Triangled>& nega);

		static bool SplitFaceInterSet(Triangled& tri, Planed& pln, unsigned int dFlag, std::vector<Triangled>& posi,
			std::vector<Triangled>& nega, std::vector<Vector3d>& vecInterSet);

		static bool SplitTriangle(std::vector<bc::Triangled>& vecTriangle, Planed& pln, std::vector<Triangled>& cPositiveSide,
			std::vector<Triangled>& cNegativeSide);

		static bool SplitTriangleInterSet(std::vector<Triangled>& vecTriangle, Planed& pln, std::vector<Triangled>& cPositiveSide,
			std::vector<Triangled>& cNegativeSide, std::vector<Vector3d>& vecInterSet);
	
	};
	

	
}

