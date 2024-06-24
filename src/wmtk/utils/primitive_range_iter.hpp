
#pragma once

#include <vector>
#include <wmtk/Primitive.hpp>
namespace wmtk::utils {


namespace detail {
template <
    std::underlying_type_t<PrimitiveType> Start,
    std::underlying_type_t<PrimitiveType> End,
    bool Inverted = (Start > End)>
class PrimitiveTypeRange
{
    public:
    using integral_type = std::underlying_type_t<PrimitiveType>;
    class iterator
    {
    public:
        using difference_type = integral_type;
        using value_type = PrimitiveType;
        using pointer = nullptr_t;
        using iterator_category = std::bidirectional_iterator_tag;
        using reference= value_type&;

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

        bool operator==(const iterator& o) const {
            return m_value == o.m_value;
        }
        bool operator!=(const iterator& o) const {
            return m_value != o.m_value;
        }
        bool operator<(const iterator& o) const {
            return m_value < o.m_value;
        }
        auto operator*() const -> PrimitiveType { return static_cast<PrimitiveType>(m_value); }
        //auto operator*() const -> PrimitiveType { return static_cast<PrimitiveType>(m_value); }

    private:
        integral_type m_value;
    };
    // using iterator = PrimitiveType;
    using const_iterator = iterator;

    auto begin() const -> iterator { return iterator(Start); }
    auto end() const -> iterator { return iterator(End); }
    auto cbegin() const -> const_iterator { return const_iterator(Start); }
    auto cend() const -> const_iterator { return const_iterator(End); }
};
} // namespace detail


// returns a vector of primitives including the endpoints of the range
template <PrimitiveType Start, PrimitiveType End>
auto primitive_range_iter()
{
    using integral_type = std::underlying_type_t<PrimitiveType>;
    constexpr static auto StartI = static_cast<integral_type>(Start);
    constexpr static auto EndI = static_cast<integral_type>(End);

    return detail::PrimitiveTypeRange<StartI, 
           EndI

           + (StartI < EndI ? 1 : -1)
               >{};
}
// returns a vector of primitives including the endpoint
template <PrimitiveType Start, bool LowerToUpper = true>
auto primitive_above_iter()
{
    constexpr static PrimitiveType End = PrimitiveType::Tetrahedron;
    using integral_type = std::underlying_type_t<PrimitiveType>;
    constexpr static auto StartI = static_cast<integral_type>(Start);
    constexpr static auto EndI = static_cast<integral_type>(End);
    if constexpr (LowerToUpper) {
        return detail::PrimitiveTypeRange<StartI, EndI + 1>{};
    } else {
        return detail::PrimitiveTypeRange<EndI, StartI - 1>{};
    }
}
// returns a vector of primitives including the endpoint
template <PrimitiveType End, bool LowerToUpper = true>
auto primitive_below_iter()
{
    constexpr static PrimitiveType Start = PrimitiveType::Vertex;
    using integral_type = std::underlying_type_t<PrimitiveType>;
    constexpr static auto StartI = static_cast<integral_type>(Start);
    constexpr static auto EndI = static_cast<integral_type>(End);
    if constexpr (LowerToUpper) {
        return detail::PrimitiveTypeRange<StartI, EndI + 1>{};
    } else {
        return detail::PrimitiveTypeRange<EndI, StartI - 1>{};
    }
}

} // namespace wmtk::utils
