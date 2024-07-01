#include "tuples_preserving_primitive_types.hpp"
#include <spdlog/spdlog.h>
#include <wmtk/utils/primitive_range_iter.hpp>

#include <cassert>
#include <wmtk/Mesh.hpp>
#include <wmtk/utils/primitive_range.hpp>

namespace wmtk::simplex {


std::vector<Tuple> tuples_preserving_primitive_types(
    const Mesh& mesh,
    const Tuple& t,
    const PrimitiveType a_ptype,
    const PrimitiveType b_ptype)
{
    PrimitiveType lower_ptype = a_ptype < b_ptype ? a_ptype : b_ptype;
    PrimitiveType higher_ptype = a_ptype == lower_ptype ? b_ptype : a_ptype;
    Tuple t_iter = t;
    std::vector<Tuple> intersection_tuples;

    PrimitiveType start = lower_ptype + 1;
    PrimitiveType end = higher_ptype - 1;
    std::vector<PrimitiveType> switch_tuple_types_old =
        wmtk::utils::primitive_range(lower_ptype, higher_ptype);
    if (start <= end) {
        auto switch_tuple_types = wmtk::utils::primitive_range_iter(start, end);
        do {
            intersection_tuples.emplace_back(t_iter);
            for (const PrimitiveType pt : switch_tuple_types) {
                t_iter = mesh.switch_tuple(t_iter, pt);
            }
        } while (t != t_iter);
    } else {
        intersection_tuples.emplace_back(t_iter);
    }

    return intersection_tuples;
}

} // namespace wmtk::simplex
