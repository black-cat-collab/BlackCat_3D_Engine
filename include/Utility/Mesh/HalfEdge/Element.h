#pragma once

#include <limits>
#include <tuple>

#ifdef __GNUC__
#include "linuxDef.h"

#if (defined(__aarch64__) || !defined(_M_ARM64)) || (__ARM_ARCH == 8)
#include <functional>
#else
#include <tr1/functional>	
#endif

#endif



namespace std
{

	// Combinie hash values in a not-completely-evil way
	// (I think this is strategy boost uses)
	namespace
	{
		template <class T>
		inline void hash_combine(std::size_t& seed, T const& v)
		{
			seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}

		// Recursive template code derived from Matthieu M.
		template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
		struct HashValueImpl {
			static void apply(size_t& seed, Tuple const& tuple) {
				HashValueImpl<Tuple, Index - 1>::apply(seed, tuple);
				hash_combine(seed, std::get<Index>(tuple));
			}
		};

		template <class Tuple>
		struct HashValueImpl<Tuple, 0>
		{
			static void apply(size_t& seed, Tuple const& tuple) { hash_combine(seed, std::get<0>(tuple)); }
		};
	} // namespace

	// Hash for tuples
	template <typename... TT>
	struct hash<tuple<TT...>>
	{
		size_t operator()(std::tuple<TT...> const& tt) const {
			size_t seed = 0;
			HashValueImpl<std::tuple<TT...>>::apply(seed, tt);
			return seed;
		}
	};
}

namespace bc
{
	namespace Geometry
	{
		template <typename T, typename M>
		class Element
		{
		public:
			using ParentMeshT = M;
		public:

			Element();                              // construct an empty (null) element
			Element(ParentMeshT* mesh, size_t nidex); // construct pointing to the i'th element of that type on a mesh.

			inline bool operator==(const Element<T, M>& other) const;
			inline bool operator!=(const Element<T, M>& other) const;
			inline bool operator>(const Element<T, M>& other) const;
			inline bool operator>=(const Element<T, M>& other) const;
			inline bool operator<(const Element<T, M>& other) const;
			inline bool operator<=(const Element<T, M>& other) const;

			size_t GetIndex() const;

			// Get the parent mesh on which the element is defined.
			ParentMeshT* GetMesh() const;

			//bool IsDead() const;

		protected:
			ParentMeshT* m_pMesh = nullptr;
			size_t m_nIndex = (std::numeric_limits<size_t>::max)();
		};
	}
}

#include "Element.inl"

