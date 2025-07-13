#pragma once
#include <fmt/format.h>
#include <cinttypes>
#include <string>
#include <tuple>

namespace wmtk::dart {
class Dart;
class DartWrap;
class ConstDartWrap;

template <typename IndexType, typename OrientType>
class _Dart : public std::tuple<IndexType, OrientType>
{
public:
    using tuple_type = std::tuple<IndexType, OrientType>;
    using tuple_type::tuple_type;

    IndexType& global_id() { return std::get<0>(*this); }
    IndexType global_id() const { return std::get<0>(*this); }

    OrientType& permutation() { return std::get<1>(*this); }
    OrientType permutation() const { return std::get<1>(*this); }

    bool is_null() const { return global_id() == -1; }

    operator std::string() const
    {
        return fmt::format("Dart[{}:{}]", global_id(), permutation());
    }

    const tuple_type& as_tuple() const { return static_cast<const tuple_type&>(*this); }

    template <typename A, typename B>
    _Dart& operator=(const _Dart<A, B>& o)
    {
        tuple_type::operator=(o.as_tuple());
        return *this;
    }

};
template <typename IndexType, typename OrientType>
std::string format_as(const _Dart<IndexType, OrientType>& d) {
    return fmt::format("Dart[{} {}]", d.global_id(), d.permutation());
}


template <int64_t Dim>
class SimplexAdjacency;

class Dart : public _Dart<int64_t, int8_t>
{
public:
    template <int64_t Dim>
    friend class SimplexAdjacency;
    using _DartType = _Dart<int64_t, int8_t>;
    using _DartType::_DartType;
    using _DartType::global_id;
    using _DartType::is_null;
    using _DartType::permutation;
    using _DartType::operator=;
    Dart()
        : _Dart(-1, -1)
    {}
    template <typename A, typename B>
    Dart(const _Dart<A, B>& o)
        : _DartType(o.global_id(), o.permutation())
    {}
};

class DartWrap : public _Dart<int64_t&, int8_t&>
{
public:
    using _DartType = _Dart<int64_t&, int8_t&>;
    using _DartType::_DartType;
    using _DartType::global_id;
    using _DartType::is_null;
    using _DartType::permutation;
    using _DartType::operator=;
};

class ConstDartWrap : public _Dart<const int64_t&, const int8_t&>
{
public:
    using _DartType = _Dart<const int64_t&, const int8_t&>;
    using _DartType::_DartType;
    using _DartType::global_id;
    using _DartType::is_null;
    using _DartType::permutation;
    using _DartType::operator=;
};

template <typename IndexType, typename OrientType, typename IndexType2, typename OrientType2>
auto operator==(
        const _Dart<IndexType,OrientType>& a,
        const _Dart<IndexType2,OrientType2>& b) -> bool {
    return a.global_id() == b.global_id() && a.permutation() == b.permutation();
}
} // namespace wmtk::autogen
