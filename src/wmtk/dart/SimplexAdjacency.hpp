#pragma once
#include <compare>
#include <array>
#include <cstdint>
#include <tuple>
#include "Dart.hpp"


namespace wmtk::dart {
template <int64_t Dim>
class SimplexAdjacency
{
public:
    // returns a tie reference type
    DartWrap operator[](int64_t index) { return dart_as_tuple(index); }
    ConstDartWrap operator[](int64_t index) const { return dart_as_tuple(index); }

    template <typename IT, typename OT>
    DartWrap operator=(const _Dart<IT, OT>& d)
        requires(Dim == 1)
    {
        (*this)[0] = d;
        return dart_as_tuple(0);
    }
    operator DartWrap()
        requires(Dim == 1)
    {
        return dart_as_tuple(0);
    }
    operator ConstDartWrap() const
        requires(Dim == 1)
    {
        return dart_as_tuple(0);
    }
    operator Dart() const
        requires(Dim == 1)
    {
        return dart_as_tuple(0);
    }

    template <bool IsConst>
    class Iterator {
        using SA = std::conditional_t<IsConst, const SimplexAdjacency, SimplexAdjacency>;
        using DW = std::conditional_t<IsConst, ConstDartWrap, DartWrap>;
        SA& m_adj;
        size_t m_index;
        public:
        Iterator(SA& a, size_t index): m_adj(a), m_index(index) {
        }
        DW operator*() const { 
            return m_adj[m_index];
        }
        DartWrap operator*() requires (!IsConst){ 
            return m_adj[m_index];
        }
        Iterator& operator++() {
            m_index++;
            return *this;
        }
        Iterator operator++(int) {
            Iterator n = *this;
            m_index++;
            return n;
        }
        std::strong_ordering operator<=>(const Iterator & o) const {
            return m_index <=> o.m_index;
        }

    };
    using iterator_type = Iterator<false>;
    using const_iterator_type = Iterator<true>;

    //using iterator_type = 
    const_iterator_type begin() const { return const_iterator_type{*this, 0}; }
    const_iterator_type end() const { return const_iterator_type{*this, Dim}; }
    const_iterator_type cbegin() const { return const_iterator_type{*this, 0}; }
    const_iterator_type cend() const { return const_iterator_type{*this, Dim}; }
    iterator_type begin() { return iterator_type{*this, 0}; }
    iterator_type end() { return iterator_type{*this, Dim}; }


private:
    ConstDartWrap dart_as_tuple(int64_t index) const
    {
        return Dart(m_global_ids[index], m_permutations[index]);
    }
    DartWrap dart_as_tuple(int64_t index)
    {
        return DartWrap(m_global_ids[index], m_permutations[index]);
    }
    std::array<int64_t, Dim> m_global_ids;
    std::array<int8_t, Dim> m_permutations;
};
} // namespace wmtk::autogen
