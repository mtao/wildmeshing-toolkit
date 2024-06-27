#pragma once

#include <vector>
#include <wmtk/Primitive.hpp>
namespace wmtk::utils {


namespace detail {
// bool m_inverted = (Start > End)>
class PrimitiveTypeRange
{
public:
    using integral_type = std::underlying_type_t<PrimitiveType>;
    PrimitiveTypeRange(const integral_type start, const integral_type end)
        : m_start(start)
        , m_end(end)
          , m_inverted(start > end)
    {}
    class iterator
    {
    public:
        using difference_type = integral_type;
        using value_type = PrimitiveType;
        using pointer = nullptr_t;
        using iterator_category = std::bidirectional_iterator_tag;
        using reference = value_type&;

        iterator(const integral_type pt, bool inverted)
            : m_value(pt), m_inverted(inverted)
        {}
        iterator(const PrimitiveType pt)
            : m_value(static_cast<integral_type>(pt))
        {}
        auto increment(integral_type pt) const -> integral_type
        {
            if (m_inverted) {
                return pt - 1;
            } else {
                return pt + 1;
            }
        }
        auto decrement(integral_type pt) const -> integral_type
        {
            if (m_inverted) {
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
            return iterator(pt,m_inverted);
        }
        auto operator--(int) -> iterator
        {
            integral_type pt = m_value;
            m_value = decrement(m_value);
            return iterator(pt,m_inverted);
        }

        bool operator==(const iterator& o) const { return m_value == o.m_value; }
        bool operator!=(const iterator& o) const { return m_value != o.m_value; }
        bool operator<(const iterator& o) const
        {
            if (m_inverted) {
                return m_value > o.m_value;
            } else {
                return m_value < o.m_value;
            }
        }
        auto operator*() const -> PrimitiveType { return static_cast<PrimitiveType>(m_value); }
        // auto operator*() const -> PrimitiveType { return static_cast<PrimitiveType>(m_value); }

    private:
        integral_type m_value;
        bool m_inverted;
    };
    // using iterator = PrimitiveType;
    using const_iterator = iterator;

    auto begin() const -> iterator { return iterator(m_start,m_inverted); }
    auto end() const -> iterator { return iterator(m_end,m_inverted); }
    auto cbegin() const -> const_iterator { return const_iterator(m_start,m_inverted); }
    auto cend() const -> const_iterator { return const_iterator(m_end,m_inverted); }


private:
    integral_type m_start;
    integral_type m_end;
    bool m_inverted;
};
} // namespace detail


// returns a vector of primitives including the endpoints of the range
inline auto primitive_range_iter(PrimitiveType start, PrimitiveType end)
{
    using integral_type = std::underlying_type_t<PrimitiveType>;
    auto startI = static_cast<integral_type>(start);
    auto endI = static_cast<integral_type>(end);
    const bool LowerToUpper = startI <= endI;
    //assert(startI == endI || (LowerToUpper == (startI < endI)));

    if (LowerToUpper) {
        return detail::PrimitiveTypeRange{startI, integral_type(endI + 1)};
    } else {
        return detail::PrimitiveTypeRange{startI, integral_type(endI - 1)};
    }
}
// returns a vector of primitives including the endpoint
inline auto primitive_above_iter(PrimitiveType start, bool LowerToUpper = true)
{
    constexpr static PrimitiveType End = PrimitiveType::Tetrahedron;
    using integral_type = std::underlying_type_t<PrimitiveType>;
    auto startI = static_cast<integral_type>(start);
    constexpr static auto EndI = static_cast<integral_type>(End);
    //const bool LowerToUpper = startI <= EndI;
    if (LowerToUpper) {
        return detail::PrimitiveTypeRange{startI, integral_type(EndI + integral_type(1))};
    } else {
        return detail::PrimitiveTypeRange{EndI, integral_type(startI - 1)};
    }
}
// returns a vector of primitives including the endpoint
inline auto primitive_below_iter(PrimitiveType end, bool LowerToUpper = true)
{
    constexpr static PrimitiveType Start = PrimitiveType::Vertex;
    using integral_type = std::underlying_type_t<PrimitiveType>;
    constexpr static auto StartI = static_cast<integral_type>(Start);
    auto endI = static_cast<integral_type>(end);
    //assert(LowerToUpper == StartI <= endI);
    if (LowerToUpper) {
        return detail::PrimitiveTypeRange{StartI, integral_type(endI + 1)};
    } else {
        return detail::PrimitiveTypeRange{endI, StartI - integral_type(1)};
    }
}

} // namespace wmtk::utils
