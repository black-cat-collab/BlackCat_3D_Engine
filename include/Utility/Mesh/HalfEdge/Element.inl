#pragma once


namespace bc
{
	namespace Geometry
	{
		template<typename T, typename M>
		Element<T, M>::Element() {}
		template<typename T, typename M>
		Element<T, M>::Element(ParentMeshT* pMesh, size_t index) : m_pMesh(pMesh), m_nIndex(index) {}

		// Comparators
		template<typename T, typename M>
		inline bool Element<T, M>::operator==(const Element<T, M>& other) const { return m_nIndex == other.m_nIndex; }
		template<typename T, typename M>
		inline bool Element<T, M>::operator!=(const Element<T, M>& other) const { return !(*this == other); }
		template<typename T, typename M>
		inline bool Element<T, M>::operator>(const Element<T, M>& other) const { return m_nIndex > other.m_nIndex; }
		template<typename T, typename M>
		inline bool Element<T, M>::operator>=(const Element<T, M>& other) const { return m_nIndex >= other.m_nIndex; }
		template<typename T, typename M>
		inline bool Element<T, M>::operator<(const Element<T, M>& other) const { return m_nIndex < other.m_nIndex; }
		template<typename T, typename M>
		inline bool Element<T, M>::operator<=(const Element<T, M>& other) const { return m_nIndex <= other.m_nIndex; }

		template <typename T, typename M>
		size_t Element<T, M>::GetIndex() const { return m_nIndex; }

		template <typename T, typename M>
		M* Element<T, M>::GetMesh() const { return m_pMesh; }
	}
}