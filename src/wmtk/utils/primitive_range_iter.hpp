
#pragma once

#include <vector>
#include <wmtk/Primitive.hpp>
namespace wmtk::utils {


namespace detail {
template <bool Inverted>
// bool Inverted = (Start > End)>
class PrimitiveTypeRange
{
public:
    using integral_type = std::underlying_type_t<PrimitiveType>;
    PrimitiveTypeRange(const integral_type start, const integral_type end)
        : m_start(start)
        , m_end(end)
    {}
    class iterator
    {
    public:
        using difference_type = integral_type;
        using value_type = PrimitiveType;
        using pointer = nullptr_t;
        using iterator_category = std::bidirectional_iterator_tag;
        using reference = value_type&;

        iterator(const integral_type pt)
            : m_value(pt)
        {}
        iterator(const PrimitiveType pt)
            : m_value(static_cast<integral_type>(pt))
        {}
        static auto increment(integral_type pt) -> integral_type
        {
            if constexpr (Inverted) {
                return pt - 1;
            } else {
                return pt + 1;
            }
        }
        static auto decrement(integral_type pt) -> integral_type
        {
            if constexpr (Inverted) {
                return pt + 1;
            } else {
                return pt - 1;
            }
        }
        auto operator++() -> iterator&
        {
            m_value = increment(m_value);
            return *this;
        }
        auto operator--() -> iterator&
        {
            m_value = decrement(m_value);
            return *this;
        }
        auto operator++(int) -> iterator
        {
            integral_type pt = m_value;
            m_value = increment(m_value);
            return iterator(pt);
        }
        auto operator--(int) -> iterator
        {
            integral_type pt = m_value;
            m_value = decrement(m_value);
            return iterator(pt);
        }

        bool operator==(const iterator& o) const { return m_value == o.m_value; }
        bool operator!=(const iterator& o) const { return m_value != o.m_value; }
        bool operator<(const iterator& o) const { return m_value < o.m_value; }
        auto operator*() const -> PrimitiveType { return static_cast<PrimitiveType>(m_value); }
        // auto operator*() const -> PrimitiveType { return static_cast<PrimitiveType>(m_value); }

    private:
        integral_type m_value;
    };
    // using iterator = PrimitiveType;
    using const_iterator = iterator;

    auto begin() const -> iterator { return iterator(m_start); }
    auto end() const -> iterator { return iterator(m_end); }
    auto cbegin() const -> const_iterator { return const_iterator(m_start); }
    auto cend() const -> const_iterator { return const_iterator(m_end); }


private:
    integral_type m_start;
    integral_type m_end;
};
} // namespace detail


// returns a vector of primitives including the endpoints of the range
template <bool LowerToUpper = true>
auto primitive_range_iter(PrimitiveType start, PrimitiveType end)
{
    using integral_type = std::underlying_type_t<PrimitiveType>;
    auto startI = static_cast<integral_type>(start);
    auto endI = static_cast<integral_type>(end);
    assert(startI == endI || (LowerToUpper == (startI < endI)));

    if constexpr (LowerToUpper) {
        return detail::PrimitiveTypeRange<false>{startI, integral_type(endI + integral_type(1))};
    } else {
        return detail::PrimitiveTypeRange<true>{startI, integral_type(endI - 1)};
    }
}
// returns a vector of primitives including the endpoint
template <bool LowerToUpper = true>
auto primitive_above_iter(PrimitiveType start)
{
    constexpr static PrimitiveType End = PrimitiveType::Tetrahedron;
    using integral_type = std::underlying_type_t<PrimitiveType>;
    auto startI = static_cast<integral_type>(start);
    constexpr static auto EndI = static_cast<integral_type>(End);
    if constexpr (LowerToUpper) {
        return detail::PrimitiveTypeRange<false>{startI, integral_type(EndI + integral_type(1))};
    } else {
        return detail::PrimitiveTypeRange<true>{EndI, integral_type(startI - 1)};
    }
}
// returns a vector of primitives including the endpoint
template <bool LowerToUpper = true>
auto primitive_below_iter(PrimitiveType end)
{
    constexpr static PrimitiveType Start = PrimitiveType::Vertex;
    using integral_type = std::underlying_type_t<PrimitiveType>;
    constexpr static auto StartI = static_cast<integral_type>(Start);
    auto endI = static_cast<integral_type>(end);
    if constexpr (LowerToUpper) {
        return detail::PrimitiveTypeRange<false>{StartI, integral_type(endI + 1)};
    } else {
        return detail::PrimitiveTypeRange<true>{endI, StartI - integral_type(1)};
    }
}

} // namespace wmtk::utils
