#pragma once

namespace bc
{
	namespace Geometry
	{
		// == Base range iterator
		// All range iterators have the form "advance through indices, skipping invalid elements". The two classes below
		// encapsulate that functionality, allowing us to just specify the element type and "valid" function for each.
		template <typename F>
		class RangeIteratorBase
		{

		public:
			RangeIteratorBase(typename F::ParentMeshT* mesh_, size_t iStart_, size_t iEnd_);
			const RangeIteratorBase& operator++();
			bool operator==(const RangeIteratorBase& other) const;
			bool operator!=(const RangeIteratorBase& other) const;
			typename F::Etype operator*() const;

		private:
			typename F::ParentMeshT* mesh;
			size_t iCurr;
			size_t iEnd;
		};

		template <typename F>
		class RangeSetBase
		{
		public:
			RangeSetBase(typename F::ParentMeshT* mesh_, size_t iStart_, size_t iEnd_);
			RangeIteratorBase<F> begin() const;
			RangeIteratorBase<F> end() const;

		private:
			typename F::ParentMeshT* mesh;
			size_t iStart, iEnd;
		};

		// == Base range iterator
		// All navigation iterators have the form "advance through elements, stopping when we get back where we started". The
		// two classes below encapsulate that functionality, allowing us to just specify the element type, advancer, and "valid"
		// function for each. The remaining boilerplates are generated. The template argument should be a class <N> with the
		// following functionality:
		//
		//    - type Rtype: the type of elements returned by the iterator (eg Vertex)
		//    - type Etype: the type of elements tracked by the iterator (eg Halfedge)
		//    - member Etype currE: the current element pointed to by the iterator
		//    - advance(): advance the iterator once (updating currE). Should not skip element, isValid will be used for that.
		//    - isValid(): reports true if currE should be returned by the iterator (not needed for most: always True)
		//
		// The helper classes will construct an instance of N, call advance() until isValid(), return currE. When iterator++ is
		// invoked, the wrapper will call advance() until isValid() is satisfied. iterator== is implemented by comparing
		// N::currE, in addition to a justStarted member which is set to false after the first increment. Probably best
		// understood by reading the examples below.
		template <typename N>
		class NavigationIteratorBase
		{

		public:
			NavigationIteratorBase(typename N::Etype firstE_, bool justStarted_);
			const NavigationIteratorBase& operator++();
			bool operator==(const NavigationIteratorBase& other) const;
			bool operator!=(const NavigationIteratorBase& other) const;
			typename N::Rtype operator*() const;

		private:
			N state;
			bool justStarted; // our iterators are generally best expressed as "do-while" loops, so this is useful to distinguish
							  // begin() from end()
		};

		template <typename N>
		class NavigationSetBase 
		{
		public:
			NavigationSetBase(typename N::Etype firstE_);
			NavigationIteratorBase<N> begin() const;
			NavigationIteratorBase<N> end() const;

		private:
			typename N::Etype firstE;
			NavigationIteratorBase<N> cachedEndIter; // avoid advancing to a new ending iterator for each end()
		};
	}
}

#include "ElementIterators.inl"

