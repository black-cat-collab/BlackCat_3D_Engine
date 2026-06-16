#pragma once

#include "Utility/Mesh/HalfEdge/HalfEdgeElement.h"

#include <vector>
#include <functional>
#include <list>
#include <unordered_set>
#include <stdexcept>
namespace bc
{
	namespace Geometry
	{
		const size_t INVALID_IND = (std::numeric_limits<size_t>::max)();

		class HalfEdgeMesh
		{
		public:
			HalfEdgeMesh(const std::vector<std::vector<size_t>>& polygons);
			HalfEdgeMesh(const std::vector<std::vector<size_t>>& polygons,
				const std::vector<std::vector<std::tuple<size_t, size_t>>>& twins);

			virtual ~HalfEdgeMesh();

			bool usesImplicitTwin() const;
			// Implicit connectivity relationships
			static size_t heTwinImplicit(size_t iHe);   // he.twin()
			static size_t heEdgeImplicit(size_t iHe);   // he.edge()
			static size_t eHalfedgeImplicit(size_t iE); // e.halfedge()

			  // Number of mesh elements of each type
			size_t nHalfedges() const;
			size_t nInteriorHalfedges() const;
			size_t nCorners() const;
			size_t nVertices() const;
			size_t nInteriorVertices(); // warning: O(n)
			size_t nEdges() const;
			size_t nFaces() const;
			size_t nBoundaryLoops() const;
			size_t nExteriorHalfedges() const;

			// Methods for range-based for loops
			// Example: for(Vertex v : mesh.vertices()) { ... }
			HalfedgeSet halfedges();
			HalfedgeInteriorSet interiorHalfedges();
			HalfedgeExteriorSet exteriorHalfedges();
			CornerSet corners();
			VertexSet vertices();
			EdgeSet edges();
			FaceSet faces();
			BoundaryLoopSet boundaryLoops();

		protected:

			Halfedge getNewHalfedge(bool isInterior);
			Edge getNewEdge();
			Face getNewFace();
			BoundaryLoop getNewBoundaryLoop();
			void expandFaceStorage(); // helper used in getNewFace() and getNewBoundaryLoop()


			  // Detect dead elements
			bool vertexIsDead(size_t iV) const;
			bool halfedgeIsDead(size_t iHe) const;
			bool edgeIsDead(size_t iE) const;
			bool faceIsDead(size_t iF) const;

			// Element connectivity
			size_t heNext(size_t iHe) const;                 // he.vertex()
			size_t heTwin(size_t iHe) const;                 // he.twin()
			size_t heSibling(size_t iHe) const;              // he.sibling()
			size_t heNextIncomingNeighbor(size_t iHe) const; // he.nextIncomingNeighbor()
			size_t heNextOutgoingNeighbor(size_t iHe) const; // he.nextOutgoingNeighbor()
			size_t heEdge(size_t iHe) const;                 // he.edge()
			size_t heVertex(size_t iHe) const;               // he.vertex()
			size_t heFace(size_t iHe) const;                 // he.face()
			bool heOrientation(size_t iHe) const;            // he.face()
			size_t eHalfedge(size_t iE) const;               // e.halfedge()
			size_t vHalfedge(size_t iV) const;               // v.halfedge()
			size_t fHalfedge(size_t iF) const;               // f.halfedge()

			  // Other implicit relationships
			bool heIsInterior(size_t iHe) const;
			bool faceIsBoundaryLoop(size_t iF) const;
			size_t faceIndToBoundaryLoopInd(size_t iF) const;
			size_t boundaryLoopIndToFaceInd(size_t iF) const;

		public:
			// Expansion callbacks
			// Argument is the new size of the element list. Elements up to this index may now be used (but _might_ not be
			// in use immediately).
			std::list<std::function<void(size_t)>> vertexExpandCallbackList;
			std::list<std::function<void(size_t)>> faceExpandCallbackList;
			std::list<std::function<void(size_t)>> edgeExpandCallbackList;
			std::list<std::function<void(size_t)>> halfedgeExpandCallbackList;

			// Compression callbacks
			// Argument is a permutation to a apply, such that d_new[i] = d_old[p[i]]. THe length of the permutation is hte size
			// of the new index space. Any elements with p[i] == INVALID_IND are unused in the new index space.
			std::list<std::function<void(const std::vector<size_t>&)>> vertexPermuteCallbackList;
			std::list<std::function<void(const std::vector<size_t>&)>> facePermuteCallbackList;
			std::list<std::function<void(const std::vector<size_t>&)>> edgePermuteCallbackList;
			std::list<std::function<void(const std::vector<size_t>&)>> halfedgePermuteCallbackList;
			std::list<std::function<void(const std::vector<size_t>&)>> boundaryLoopPermuteCallbackList;
			std::list<std::function<void(void)>> compressCallbackList;

			// Mesh delete callbacks
			// (this unfortunately seems to be necessary; objects which have registered their callbacks above
			// need to know not to try to de-register them if the mesh has been deleted)
			std::list<std::function<void()>> meshDeleteCallbackList;
		protected:
			std::vector<size_t> heNextArr;    // he.next(), forms a circular singly-linked list in each face
			std::vector<size_t> heVertexArr;  // he.vertex()
			std::vector<size_t> heFaceArr;    // he.face()
			std::vector<size_t> vHalfedgeArr; // v.halfedge()
			std::vector<size_t> fHalfedgeArr; // f.halfedge()

			const bool bUseImplicitTwinFlag;


			// (see note above about implicit twin)
			std::vector<size_t> heSiblingArr; // he.sibling() and he.twin(), forms a circular singly-linked list around each edge
			std::vector<size_t> heEdgeArr;    // he.edge()
			std::vector<char> heOrientArr;    // true if the halfedge has the same orientation as its edge
			std::vector<size_t> eHalfedgeArr; // e.halfedge()

			// These form a doubly-linked list of the halfedges around each vertex, providing the richer data needed to iterate
			// around vertices in a nonmanifold mesh. These encode connectivity, but are redundant given the other arrays above,
			// so they don't need to be serialized (etc). Note the removeFromVertexLists() and addToVertexLists() below to
			// simplify maintaining these internally.
			std::vector<size_t> heVertInNextArr;
			std::vector<size_t> heVertInPrevArr;
			std::vector<size_t> vHeInStartArr;
			std::vector<size_t> heVertOutNextArr;
			std::vector<size_t> heVertOutPrevArr;
			std::vector<size_t> vHeOutStartArr;

			// Track element counts (can't rely on rawVertices.size() after deletions have made the list sparse). These are the
			// actual number of valid elements, not the size of the buffer that holds them.
			size_t nHalfedgesCount = 0;
			size_t nInteriorHalfedgesCount = 0;
			size_t nEdgesCount = 0;
			size_t nVerticesCount = 0;
			size_t nFacesCount = 0;
			size_t nBoundaryLoopsCount = 0;

			// == Track the capacity and fill size of our buffers.
			// These give the capacity of the currently allocated buffer.
			// Note that this is _not_ defined to be std::vector::capacity(), it's the largest size such that arr[i] is legal (aka
			// arr.size()).
			size_t nVerticesCapacityCount = 0;
			size_t nHalfedgesCapacityCount = 0; // will always be even if implicit twin
			size_t nEdgesCapacityCount = 0;     // will always be even if implicit twin
			size_t nFacesCapacityCount = 0;     // capacity for faces _and_ boundary loops

			// These give the number of filled elements in the currently allocated buffer. This will also be the maximal index of
			// any element (except the weirdness of boundary loop faces). As elements get marked dead, nVerticesCount decreases
			// but nVertexFillCount does not (etc), so it denotes the end of the region in the buffer where elements have been
			// stored.
			size_t nVerticesFillCount = 0;
			size_t nHalfedgesFillCount = 0; // must always be even if implicit twin
			size_t nEdgesFillCount = 0;
			size_t nFacesFillCount = 0;         // where the real faces stop, and empty/boundary loops begin
			size_t nBoundaryLoopsFillCount = 0; // remember, these fill from the back of the face buffer

			// The mesh is _compressed_ if all of the index spaces are dense. E.g. if thare are |V| vertices, then the vertices
			// are densely indexed from 0 ... |V|-1 (and likewise for the other elements). The mesh can become not-compressed as
			// deletions mark elements with tombstones--this is how we support constant time deletion.
			// Call compress() to re-index and return to usual dense indexing.
			bool isCompressedFlag = true;

			uint64_t modificationTick = 1; // Increment every time the mesh is mutated in any way. Used to track staleness.

			// Hide copy and move constructors, we don't wanna mess with that
			HalfEdgeMesh(const HalfEdgeMesh& other) = delete;
			HalfEdgeMesh& operator=(const HalfEdgeMesh& other) = delete;
			HalfEdgeMesh(HalfEdgeMesh&& other) = delete;
			HalfEdgeMesh& operator=(HalfEdgeMesh&& other) = delete;






			void initializeHalfedgeNeighbors();

			// Build a flat array for iterating around a vertex, before the mesh structure is complete.
			// For vertex iV, vertexIterationCacheHeIndex holds the
			// indices of the halfedges outgoing/incoming to the vertex, in the range from vertexIterationCacheVertexStart[iV] to
			// vertexIterationCacheVertexStart[iV+1]
			void generateVertexIterationCache(std::vector<size_t>& vertexIterationCacheHeIndex,
				std::vector<size_t>& vertexIterationCacheVertexStart, bool incoming,
				bool skipDead = true);


			// Elements need direct access in to members to traverse
			friend class Vertex;
			friend class Halfedge;
			friend class Corner;
			friend class Edge;
			friend class Face;
			friend class BoundaryLoop;

			friend struct VertexRangeF;
			friend struct HalfedgeRangeF;
			friend struct HalfedgeInteriorRangeF;
			friend struct HalfedgeExteriorRangeF;
			friend struct CornerRangeF;
			friend struct EdgeRangeF;
			friend struct FaceRangeF;
			friend struct BoundaryLoopRangeF;
			friend struct VertexNeighborIteratorState;
		};
	}
}

#include "HalfEdgeElement.inl"

#include "HalfEdgeMesh.inl"

