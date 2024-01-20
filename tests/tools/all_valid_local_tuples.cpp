#include "all_valid_local_tuples.hpp"
#include <wmtk/utils/internal/all_valid_local_tuples.hpp>
using namespace wmtk::autogen;
namespace wmtk::utils::internal {
std::vector<PrimitiveType> primitives_up_to(PrimitiveType pt)
{
    return wmtk::utils::internal::primitives_up_to(pt);
}


int64_t max_tuple_count(PrimitiveType pt)
{
    return wmtk::utils::internal::max_tuple_count(pt);
}

Tuple tuple_from_offset_id(PrimitiveType pt, int offset)
{
}

std::vector<Tuple> all_valid_local_tuples(PrimitiveType pt)
{
    std::vector<Tuple> tups;
    int64_t size = max_tuple_count(pt);
    assert(size > 0);
    tups.reserve(size);
    for (int64_t idx = 0; idx < max_tuple_count(pt); ++idx) {
        tups.emplace_back(tuple_from_offset_id(pt, idx));
    }

    tups.erase(
        std::remove_if(
            tups.begin(),
            tups.end(),
            [](const Tuple& t) -> bool { return t.is_null(); }),
        tups.end());
    return tups;
}
} // namespace wmtk::utils::internal
