#pragma once
#include <array>
#include <cstdint>
#include <tuple>
#include "Dart.hpp"


namespace wmtk::autogen {
template <int64_t Dim>
class SimplexAdjacency
{
public:
    // returns a tie reference type
    DartWrap operator[](int64_t index) { return dart_as_tuple(index); }
    Dart operator[](int64_t index) const { return dart_as_tuple(index); }

    template <typename IT, typename OT>
    DartWrap& operator=(const _Dart<IT, OT>& d)
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
    operator Dart() const
        requires(Dim == 1)
    {
        return dart_as_tuple(0);
    }

private:
    Dart dart_as_tuple(int64_t index) const
    {
        return Dart(m_global_ids[index], m_local_orientations[index]);
    }
    DartWrap dart_as_tuple(int64_t index)
    {
        return DartWrap(m_global_ids[index], m_local_orientations[index]);
    }
    std::array<int64_t, Dim> m_global_ids;
    std::array<int8_t, Dim> m_local_orientations;
};
} // namespace wmtk::autogen
