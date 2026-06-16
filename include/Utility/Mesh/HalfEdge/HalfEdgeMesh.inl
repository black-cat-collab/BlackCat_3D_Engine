#pragma once

namespace bc
{
	namespace Geometry
	{
		// Methods for getting number of mesh elements
		inline size_t HalfEdgeMesh::nHalfedges()         const { return nHalfedgesCount; }
		inline size_t HalfEdgeMesh::nInteriorHalfedges() const { return nInteriorHalfedgesCount; }
		inline size_t HalfEdgeMesh::nExteriorHalfedges() const { return nHalfedgesCount - nInteriorHalfedgesCount; }
		inline size_t HalfEdgeMesh::nCorners()           const { return nInteriorHalfedgesCount; }
		inline size_t HalfEdgeMesh::nVertices()          const { return nVerticesCount; }
		inline size_t HalfEdgeMesh::nEdges()             const { return nEdgesCount; }
		inline size_t HalfEdgeMesh::nFaces()             const { return nFacesCount; }
		inline size_t HalfEdgeMesh::nBoundaryLoops()     const { return nBoundaryLoopsCount; }

		// Implicit relationships
		inline bool HalfEdgeMesh::usesImplicitTwin() const { return bUseImplicitTwinFlag; }
		inline size_t HalfEdgeMesh::heTwinImplicit(size_t iHe) { return iHe ^ 1; }     // static
		inline size_t HalfEdgeMesh::heEdgeImplicit(size_t iHe) { return iHe / 2; }     // static
		inline size_t HalfEdgeMesh::eHalfedgeImplicit(size_t iE) { return 2 * iE; }      // static

		// Other getters
		inline bool HalfEdgeMesh::heIsInterior(size_t iHe) const { return !faceIsBoundaryLoop(heFaceArr[iHe]); }
		inline bool HalfEdgeMesh::faceIsBoundaryLoop(size_t iF) const { return iF >= nFacesFillCount; }
		inline size_t HalfEdgeMesh::faceIndToBoundaryLoopInd(size_t iF) const { return nFacesCapacityCount - 1 - iF; }
		inline size_t HalfEdgeMesh::boundaryLoopIndToFaceInd(size_t iB) const { return nFacesCapacityCount - 1 - iB; }

		// Detect dead elements
		inline bool HalfEdgeMesh::vertexIsDead(size_t iV)      const { return vHalfedgeArr[iV] == INVALID_IND; }
		inline bool HalfEdgeMesh::halfedgeIsDead(size_t iHe)   const { return heNextArr[iHe] == INVALID_IND; }
		inline bool HalfEdgeMesh::edgeIsDead(size_t iE)        const { return usesImplicitTwin() ? heNextArr[eHalfedgeImplicit(iE)] == INVALID_IND : eHalfedgeArr[iE] == INVALID_IND; }
		inline bool HalfEdgeMesh::faceIsDead(size_t iF)        const { return fHalfedgeArr[iF] == INVALID_IND; }

		// Connectivity
		inline size_t HalfEdgeMesh::heNext(size_t iHe)               const { return heNextArr[iHe]; }
		inline size_t HalfEdgeMesh::heTwin(size_t iHe)               const {
			if (usesImplicitTwin()) return heTwinImplicit(iHe);
			//throw std::runtime_error("called he.twin() on not-necessarily-manifold mesh. Try he.sibling() instead"); 
			return heSiblingArr[iHe];
		}
		inline size_t HalfEdgeMesh::heSibling(size_t iHe)            const { return usesImplicitTwin() ? heTwinImplicit(iHe) : heSiblingArr[iHe]; }
		inline size_t HalfEdgeMesh::heNextIncomingNeighbor(size_t iHe)  const {
			return usesImplicitTwin() ? heTwinImplicit(heNextArr[iHe]) : heVertInNextArr[iHe];
		}
		inline size_t HalfEdgeMesh::heNextOutgoingNeighbor(size_t iHe) const {
			return usesImplicitTwin() ? heNextArr[heTwinImplicit(iHe)] : heVertOutNextArr[iHe];
		}
		inline size_t HalfEdgeMesh::heEdge(size_t iHe)               const { return usesImplicitTwin() ? heEdgeImplicit(iHe) : heEdgeArr[iHe]; }
		inline size_t HalfEdgeMesh::heVertex(size_t iHe)             const { return heVertexArr[iHe]; }
		inline size_t HalfEdgeMesh::heFace(size_t iHe)               const { return heFaceArr[iHe]; }
		inline bool HalfEdgeMesh::heOrientation(size_t iHe)          const { return usesImplicitTwin() ? (iHe % 2) == 0 : heOrientArr[iHe]; }
		inline size_t HalfEdgeMesh::eHalfedge(size_t iE)             const { return usesImplicitTwin() ? eHalfedgeImplicit(iE) : eHalfedgeArr[iE]; }
		inline size_t HalfEdgeMesh::vHalfedge(size_t iV)             const { return vHalfedgeArr[iV]; }
		inline size_t HalfEdgeMesh::fHalfedge(size_t iF)             const { return fHalfedgeArr[iF]; }


		// Methods for iterating over mesh elements w/ range-based for loops ===========

		inline VertexSet HalfEdgeMesh::vertices() { return VertexSet(this, 0, nVerticesFillCount); }
		inline HalfedgeSet HalfEdgeMesh::halfedges() { return HalfedgeSet(this, 0, nHalfedgesFillCount); }
		inline HalfedgeInteriorSet HalfEdgeMesh::interiorHalfedges() { return HalfedgeInteriorSet(this, 0, nHalfedgesFillCount); }
		inline HalfedgeExteriorSet HalfEdgeMesh::exteriorHalfedges() { return HalfedgeExteriorSet(this, 0, nHalfedgesFillCount); }
		inline CornerSet HalfEdgeMesh::corners() { return CornerSet(this, 0, nHalfedgesFillCount); }
		inline EdgeSet HalfEdgeMesh::edges() { return EdgeSet(this, 0, nEdgesFillCount); }
		inline FaceSet HalfEdgeMesh::faces() { return FaceSet(this, 0, nFacesFillCount); }
		inline BoundaryLoopSet HalfEdgeMesh::boundaryLoops() { return BoundaryLoopSet(this, 0, nBoundaryLoopsFillCount); }
	}
}