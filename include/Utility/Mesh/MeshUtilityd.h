#pragma once

namespace bc
{
	extern int CompareOnlyVertexd(const bc::TriangleVertexd* v0, const bc::TriangleVertexd* v1);
	extern int CompareVertexUVd(const bc::TriangleVertexd* v0, const  bc::TriangleVertexd* v1);
	class IMaterial;
	class MeshUtilityd
	{
	public:


		static bool Split(Planed& pln, std::vector<Triangled>& vecTriangle, std::vector<Triangled>& vecPosi, std::vector<Triangled>& vecNega) ;
		static void BuildIndex(const std::vector<Triangled>& vecTri, std::vector<uint>& vecElem, std::vector<TriangleVertexd>& vecVertices, PFNNMCOMPAREDd compare = NULL);
		static uint InsertVertexPtr(bc::TriangleVertexd*pPt, int& nCount, uint* pSortIndex, bc::TriangleVertexd** pVetex, PFNNMCOMPAREDd compare);
		
		static IMesh* BuildMesh(const std::string& strName, const std::vector<Triangled>& vecTri,
			IMaterial* pMat , PFNNMCOMPAREDd compare = NULL);
		
		static IMesh* BuildMesh(const std::string& strName, const std::vector<Triangled>& vecTri, bool hasUV1 = false);

		static uint InsertVertex(const bc::TriangleVertexd& pPt, int& nCount, uint* pSortIndex, bc::TriangleVertexd* pVetex, PFNNMCOMPAREDd compare);
	
	
		static IMesh* CreatePlaneMesh(const std::string&strName, const std::vector<Vector3>& vecVertex);


		static int IndexBinaryTraversePtr(bc::TriangleVertexd* value, // The reference element
			bc::TriangleVertexd** data,             // pointer to the indexed data
			uint* indices,               // pointer index
			int count,                  // number of items in the array
			int& result,                // The result of the last compare
			PFNNMCOMPAREDd compare);     // Compare function. 

		static int IndexBinaryTraverse(const bc::TriangleVertexd& value, // The reference element
			bc::TriangleVertexd* data,             // pointer to the indexed data
			uint* indices,               // pointer index
			int count,                  // number of items in the array
			int& result,                // The result of the last compare
			PFNNMCOMPAREDd compare);     // Compare function. 

			//static bool SplitFace(Triangled& tri, Planed& pln, unsigned int dFlag, std::vector<Triangled>& posi, std::vector<Triangled>& nega);
	
	};

}

