#pragma once

namespace bc
{
	extern int CompareOnlyVertex(const bc::TriangleVertex* v0, const bc::TriangleVertex* v1);
	extern int CompareVertexUV(const bc::TriangleVertex* v0, const  bc::TriangleVertex* v1);
	class IMaterial;
	class MeshUtility
	{
	public:


		static bool Split(Plane& pln, std::vector<Triangle>& vecTriangle, std::vector<Triangle>& vecPosi, std::vector<Triangle>& vecNega) ;
		static void BuildIndex(const std::vector<Triangle>& vecTri, std::vector<uint>& vecElem, std::vector<TriangleVertex>& vecVertices, PFNNMCOMPARED compare = NULL);
		static uint InsertVertexPtr(bc::TriangleVertex*pPt, int& nCount, uint* pSortIndex, bc::TriangleVertex** pVetex, PFNNMCOMPARED compare);
		
		static IMesh* BuildMesh(const std::string& strName, const std::vector<Triangle>& vecTri,
			IMaterial* pMat , PFNNMCOMPARED compare = NULL);
		
		static IMesh* BuildMesh(const std::string& strName, const std::vector<Triangle>& vecTri, bool hasUV1 = false, Vector3d origin = Vector3d{ 0.0f, 0.0f, 0.0f });

		static uint InsertVertex(const bc::TriangleVertex& pPt, int& nCount, uint* pSortIndex, bc::TriangleVertex* pVetex, PFNNMCOMPARED compare);
	
	
		static IMesh* CreatePlaneMesh(const std::string&strName, const std::vector<Vector3>& vecVertex);


		//static bool SplitFace(Triangle& tri, Plane& pln, unsigned int dFlag, std::vector<Triangle>& posi, std::vector<Triangle>& nega);
	
	};

}

