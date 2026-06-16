#pragma once

#include "Element.h"
#include "ElementIterators.h"
#include <array>
namespace bc
{
	namespace Geometry
	{
		class HalfEdgeMesh;
		class Vertex;
		class Halfedge;
		class Corner;
		class Edge;
		class Face;
		class BoundaryLoop;

		struct VertexAdjacentVertexNavigator;
		struct VertexIncomingHalfedgeNavigator;
		struct VertexOutgoingHalfedgeNavigator;
		struct VertexAdjacentFaceNavigator;
		struct VertexAdjacentEdgeNavigator;
		struct VertexAdjacentCornerNavigator;
		struct EdgeAdjacentHalfedgeNavigator;
		struct EdgeAdjacentInteriorHalfedgeNavigator;
		struct EdgeAdjacentFaceNavigator;
		struct FaceAdjacentHalfedgeNavigator;
		struct FaceAdjacentVertexNavigator;
		struct FaceAdjacentEdgeNavigator;
		struct FaceAdjacentFaceNavigator;
		struct FaceAdjacentCornerNavigator;
		struct BoundaryLoopAdjacentHalfedgeNavigator;
		struct BoundaryLoopAdjacentVertexNavigator;
		struct BoundaryLoopAdjacentEdgeNavigator;
		struct BoundaryLoopAdjacentFaceNavigator;
		struct BoundaryLoopAdjacentCornerNavigator;


		class Vertex : public Element<Vertex, HalfEdgeMesh>
		{
		public:
			Vertex(); 
			Vertex(HalfEdgeMesh* mesh, size_t ind); 

			// Navigators
			Halfedge halfedge() const;
			Corner corner() const;
			bool isDead() const;

			// Properties
			bool isBoundary() const;
			bool isManifold() const;
			bool isManifoldAndOriented() const;
			size_t degree() const;
			size_t faceDegree() const;


			  // Iterators
			NavigationSetBase<VertexAdjacentVertexNavigator> adjacentVertices() const;
			NavigationSetBase<VertexIncomingHalfedgeNavigator> incomingHalfedges() const;
			NavigationSetBase<VertexOutgoingHalfedgeNavigator> outgoingHalfedges() const;
			NavigationSetBase<VertexAdjacentCornerNavigator> adjacentCorners() const;
			NavigationSetBase<VertexAdjacentEdgeNavigator> adjacentEdges() const;
			NavigationSetBase<VertexAdjacentFaceNavigator> adjacentFaces() const;
		};

		// All vertices
		struct VertexRangeF
		{
			static bool elementOkay(const HalfEdgeMesh& mesh, size_t ind);
			typedef Vertex Etype;
			typedef HalfEdgeMesh ParentMeshT;
		};
		typedef RangeSetBase<VertexRangeF> VertexSet;


		class Halfedge : public Element<Halfedge, HalfEdgeMesh>
		{
		public:
			Halfedge(); 
			Halfedge(HalfEdgeMesh* pMesh, size_t index);

			// Navigators
			Halfedge twin() const;
			Halfedge sibling() const;
			Halfedge nextOutgoingNeighbor() const; // next halfedge which has the same tail vertex as this, form a cycle
			Halfedge nextIncomingNeighbor() const; // next halfedge which has the same tip vertex as this, form a cycle
			Halfedge next() const;
			Corner corner() const;
			Vertex vertex() const;
			Vertex tipVertex() const;
			Vertex tailVertex() const;
			Edge edge() const;
			Face face() const;
			bool isDead() const;

			// Super-navigators
			Halfedge prevOrbitFace() const;
			Halfedge prevOrbitVertex() const; // only meaningful if manifold

			// Properties
			bool isInterior() const;
			bool orientation() const; // True if the halfedge has the same orientation as its edge
									  // Remember that halfedge orientation means "points in direction". If two faces have the
									  // same orientation, their halfedges along a shared edge will have opposite orientations.
		};

		// All halfedges
		struct HalfedgeRangeF 
		{
			static bool elementOkay(const HalfEdgeMesh& mesh, size_t ind);
			typedef Halfedge Etype;
			typedef HalfEdgeMesh ParentMeshT;
		};
		typedef RangeSetBase<HalfedgeRangeF> HalfedgeSet;


		// Interior halfedges
		struct HalfedgeInteriorRangeF {
			static bool elementOkay(const HalfEdgeMesh& mesh, size_t ind);
			typedef Halfedge Etype;
			typedef HalfEdgeMesh ParentMeshT;
		};
		typedef RangeSetBase<HalfedgeInteriorRangeF> HalfedgeInteriorSet;

		// Exterior halfedges
		struct HalfedgeExteriorRangeF {
			static bool elementOkay(const HalfEdgeMesh& mesh, size_t ind);
			typedef Halfedge Etype;
			typedef HalfEdgeMesh ParentMeshT;
		};
		typedef RangeSetBase<HalfedgeExteriorRangeF> HalfedgeExteriorSet;


		class Corner : public Element<Corner, HalfEdgeMesh>
		{
		public:
			Corner(); 
			Corner(HalfEdgeMesh* pMesh, size_t index); 

			// Navigators
			Halfedge halfedge() const;
			Vertex vertex() const;
			Face face() const;
			bool isDead() const;
		};

		// All corners
		struct CornerRangeF {
			static bool elementOkay(const HalfEdgeMesh& mesh, size_t ind);
			typedef Corner Etype;
			typedef HalfEdgeMesh ParentMeshT;
		};
		typedef RangeSetBase<CornerRangeF> CornerSet;


		class Edge : public Element<Edge, HalfEdgeMesh>
		{
		public:
			Edge();                              
			Edge(HalfEdgeMesh* pMesh, size_t index); 

			// Navigators
			Halfedge halfedge() const;
			Vertex otherVertex(Vertex v) const;
			Vertex firstVertex() const;
			Vertex secondVertex() const;
			std::array<Halfedge, 4> diamondBoundary() const;
			bool isDead() const;

			// Properties
			bool isBoundary() const;
			bool isManifold() const;
			bool isOriented() const;
			size_t degree() const;

			// Iterators
			NavigationSetBase<EdgeAdjacentHalfedgeNavigator> adjacentHalfedges() const;
			NavigationSetBase<EdgeAdjacentInteriorHalfedgeNavigator> adjacentInteriorHalfedges() const;
			NavigationSetBase<EdgeAdjacentFaceNavigator> adjacentFaces() const;
			std::array<Vertex, 2> adjacentVertices() const;
		};

		// All edges
		struct EdgeRangeF
		{
			static bool elementOkay(const HalfEdgeMesh& mesh, size_t ind);
			typedef Edge Etype;
			typedef HalfEdgeMesh ParentMeshT;
		};
		typedef RangeSetBase<EdgeRangeF> EdgeSet;

		class Face : public Element<Face, HalfEdgeMesh>
		{
		public:
			Face();
			Face(HalfEdgeMesh* pMesh, size_t index);

			// Navigators
			Halfedge halfedge() const;
			BoundaryLoop asBoundaryLoop() const;
			bool isDead() const;

			// Properties
			bool isBoundaryLoop() const;
			bool isTriangle() const;
			size_t degree() const;

			// Iterators
			NavigationSetBase<FaceAdjacentVertexNavigator> adjacentVertices() const;
			NavigationSetBase<FaceAdjacentHalfedgeNavigator> adjacentHalfedges() const;
			NavigationSetBase<FaceAdjacentCornerNavigator> adjacentCorners() const;
			NavigationSetBase<FaceAdjacentEdgeNavigator> adjacentEdges() const;
			NavigationSetBase<FaceAdjacentFaceNavigator> adjacentFaces() const;
		};

		// All faces
		struct FaceRangeF
		{
			static bool elementOkay(const HalfEdgeMesh& mesh, size_t ind);
			typedef Face Etype;
			typedef HalfEdgeMesh ParentMeshT;
		};
		typedef RangeSetBase<FaceRangeF> FaceSet;

		class BoundaryLoop : public Element<BoundaryLoop, HalfEdgeMesh>
		{
		public:
			BoundaryLoop();
			BoundaryLoop(HalfEdgeMesh* pMesh, size_t index);

			Halfedge halfedge() const;
			Face asFace() const;
			bool isDead() const;

			// Properties
			size_t degree() const;

			// Iterators
			NavigationSetBase<BoundaryLoopAdjacentVertexNavigator> adjacentVertices() const;
			NavigationSetBase<BoundaryLoopAdjacentHalfedgeNavigator> adjacentHalfedges() const;
			NavigationSetBase<BoundaryLoopAdjacentEdgeNavigator> adjacentEdges() const;
		};


		// All boundary loops
		struct BoundaryLoopRangeF
		{
			static bool elementOkay(const HalfEdgeMesh& mesh, size_t ind);
			typedef BoundaryLoop Etype;
			typedef HalfEdgeMesh ParentMeshT;
		};
		typedef RangeSetBase<BoundaryLoopRangeF> BoundaryLoopSet;


		// ==========================================================
		// ===============   Navigation Iterators   =================
		// ==========================================================

		// == Vertex

		// Helper class to store some special extra state for the vertex iterators
		// For implicit-twin meshes, it just does the usual twin.next() and currHe is always an outgoing halfedge. For general
		// meshes, it iterates first through the outgoing, then the incoming halfedges (always stored in currHe).
		struct VertexNeighborIteratorState 
		{
			VertexNeighborIteratorState(Halfedge currHeOutgoing, bool useImplicitTwin);

			const bool useImplicitTwin;
			Halfedge currHe = Halfedge();

			// if useImplicitTwin == false, this is populated
			bool processingIncoming = false;
			Halfedge firstHe = Halfedge();

			void advance();
			bool isHalfedgeCanonical() const; // this currently pointing at the one canonical halfedge along an edge
			bool operator==(const VertexNeighborIteratorState& rhs) const;
		};

		// Adjacent vertices
		struct VertexAdjacentVertexNavigator 
		{
			void advance();
			bool isValid() const;
			typedef VertexNeighborIteratorState Etype;
			Etype currE;
			typedef Vertex Rtype;
			Rtype getCurrent() const;
		};

		// Adjacent incoming halfedges
		struct VertexIncomingHalfedgeNavigator 
		{
			void advance();
			bool isValid() const;
			typedef Halfedge Etype;
			Etype currE;
			typedef Halfedge Rtype;
			Rtype getCurrent() const;
		};

		// Adjacent outgoing halfedges
		struct VertexOutgoingHalfedgeNavigator 
		{
			void advance();
			bool isValid() const;
			typedef Halfedge Etype;
			Etype currE;
			typedef Halfedge Rtype;
			Rtype getCurrent() const;
		};

		// Adjacent corners
		struct VertexAdjacentCornerNavigator
		{
			void advance();
			bool isValid() const;
			typedef Halfedge Etype;
			Etype currE;
			typedef Corner Rtype;
			Rtype getCurrent() const;
		};


		// Adjacent edges
		struct VertexAdjacentEdgeNavigator
		{
			void advance();
			bool isValid() const;
			typedef VertexNeighborIteratorState Etype;
			Etype currE;
			typedef Edge Rtype;
			Rtype getCurrent() const;
		};

		// Adjacent faces
		struct VertexAdjacentFaceNavigator
		{
			void advance();
			bool isValid() const;
			typedef Halfedge Etype;
			Etype currE;
			typedef Face Rtype;
			Rtype getCurrent() const;
		};

		// == Edge


		// Adjacent halfedge
		struct EdgeAdjacentHalfedgeNavigator
		{
			void advance();
			bool isValid() const;
			typedef Halfedge Etype;
			Etype currE;
			typedef Halfedge Rtype;
			Rtype getCurrent() const;
		};

		// Adjacent interior halfedge
		struct EdgeAdjacentInteriorHalfedgeNavigator
		{
			void advance();
			bool isValid() const;
			typedef Halfedge Etype;
			Etype currE;
			typedef Halfedge Rtype;
			Rtype getCurrent() const;
		};

		// Adjacent interior halfedge
		struct EdgeAdjacentFaceNavigator 
		{
			void advance();
			bool isValid() const;
			typedef Halfedge Etype;
			Etype currE;
			typedef Face Rtype;
			Rtype getCurrent() const;
		};

		// == Face

		// Adjacent vertices
		struct FaceAdjacentVertexNavigator 
		{
			void advance();
			bool isValid() const;
			typedef Halfedge Etype;
			Etype currE;
			typedef Vertex Rtype;
			Rtype getCurrent() const;
		};

		// Adjacent halfedge
		struct FaceAdjacentHalfedgeNavigator
		{
			void advance();
			bool isValid() const;
			typedef Halfedge Etype;
			Etype currE;
			typedef Halfedge Rtype;
			Rtype getCurrent() const;
		};

		// Adjacent corner
		struct FaceAdjacentCornerNavigator 
		{
			void advance();
			bool isValid() const;
			typedef Halfedge Etype;
			Etype currE;
			typedef Corner Rtype;
			Rtype getCurrent() const;
		};

		// Adjacent edge
		struct FaceAdjacentEdgeNavigator
		{
			void advance();
			bool isValid() const;
			typedef Halfedge Etype;
			Etype currE;
			typedef Edge Rtype;
			Rtype getCurrent() const;
		};

		// Adjacent face
		struct FaceAdjacentFaceNavigator
		{
			void advance();
			bool isValid() const;
			typedef std::pair<Halfedge, Halfedge>
				Etype; // first is current halfedge of this face, second halfedge adjacent to face to return
			Etype currE;
			typedef Face Rtype;
			Rtype getCurrent() const;
		};


		// == Faces

		// Adjacent vertex
		struct BoundaryLoopAdjacentVertexNavigator 
		{
			void advance();
			bool isValid() const;
			typedef Halfedge Etype;
			Etype currE;
			typedef Vertex Rtype;
			Rtype getCurrent() const;
		};

		// Adjacent halfedge
		struct BoundaryLoopAdjacentHalfedgeNavigator 
		{
			void advance();
			bool isValid() const;
			typedef Halfedge Etype;
			Etype currE;
			typedef Halfedge Rtype;
			Rtype getCurrent() const;
		};

		// Adjacent edge
		struct BoundaryLoopAdjacentEdgeNavigator
		{
			void advance();
			bool isValid() const;
			typedef Halfedge Etype;
			Etype currE;
			typedef Edge Rtype;
			Rtype getCurrent() const;
		};
	}
}

