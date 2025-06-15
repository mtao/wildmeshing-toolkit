

#include "canonical_darts.hpp"
#include <array>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/utils/from_local_vertex_permutation.hpp>
namespace wmtk::tests::dart::utils {
namespace {
template <typename... Args>
int8_t v(Args&&... args)
{
    std::array<int8_t, sizeof...(args)> a{{int8_t(args)...}};

    const PrimitiveType pt = get_primitive_type_from_id(a.size() - 1);
    return wmtk::dart::utils::from_local_vertex_permutation(
        pt,
        VectorX<int8_t>::ConstMapType(a.data(), a.size()));
}
} // namespace
  //
const wmtk::dart::Dart d0 = {0, v(0)};

const wmtk::dart::Dart d01 = {0, v(0, 1)};
const wmtk::dart::Dart d10 = {0, v(1, 0)};

const wmtk::dart::Dart d012 = {0, v(0, 1, 2)};
const wmtk::dart::Dart d021 = {0, v(0, 2, 1)};
const wmtk::dart::Dart d102 = {0, v(1, 0, 2)};
const wmtk::dart::Dart d120 = {0, v(1, 2, 0)};
const wmtk::dart::Dart d201 = {0, v(2, 0, 1)};
const wmtk::dart::Dart d210 = {0, v(2, 1, 0)};

const wmtk::dart::Dart d0123 = {0, v(0, 1, 2, 3)};
const wmtk::dart::Dart d0213 = {0, v(0, 2, 1, 3)};
const wmtk::dart::Dart d1023 = {0, v(1, 0, 2, 3)};
const wmtk::dart::Dart d1203 = {0, v(1, 2, 0, 3)};
const wmtk::dart::Dart d2013 = {0, v(2, 0, 1, 3)};
const wmtk::dart::Dart d2103 = {0, v(2, 1, 0, 3)};

const wmtk::dart::Dart d0132 = {0, v(0, 1, 3, 2)};
const wmtk::dart::Dart d0231 = {0, v(0, 2, 3, 1)};
const wmtk::dart::Dart d1032 = {0, v(1, 0, 3, 2)};
const wmtk::dart::Dart d1230 = {0, v(1, 2, 3, 0)};
const wmtk::dart::Dart d2031 = {0, v(2, 0, 3, 1)};
const wmtk::dart::Dart d2130 = {0, v(2, 1, 3, 0)};

const wmtk::dart::Dart d0312 = {0, v(0, 3, 1, 2)};
const wmtk::dart::Dart d0321 = {0, v(0, 3, 2, 1)};
const wmtk::dart::Dart d1302 = {0, v(1, 3, 0, 2)};
const wmtk::dart::Dart d1320 = {0, v(1, 3, 2, 0)};
const wmtk::dart::Dart d2301 = {0, v(2, 3, 0, 1)};
const wmtk::dart::Dart d2310 = {0, v(2, 3, 1, 0)};

const wmtk::dart::Dart d3012 = {0, v(3, 0, 1, 2)};
const wmtk::dart::Dart d3021 = {0, v(3, 0, 2, 1)};
const wmtk::dart::Dart d3102 = {0, v(3, 1, 0, 2)};
const wmtk::dart::Dart d3120 = {0, v(3, 1, 2, 0)};
const wmtk::dart::Dart d3201 = {0, v(3, 2, 0, 1)};
const wmtk::dart::Dart d3210 = {0, v(3, 2, 1, 0)};

const std::array<wmtk::dart::Dart, 1> D0 = {{d0}};

const std::array<wmtk::dart::Dart, 2> D1 = {{d01, d10}};

const std::array<wmtk::dart::Dart, 6> D2 = {{d012, d021, d102, d120, d201, d210}};

const std::array<wmtk::dart::Dart, 24> D3 = {
    {d1230, d1320, d2130, d2310, d3120, d3210, d0231, d0321, d2031, d2301, d3021, d3201,
     d0132, d0312, d1032, d1302, d3012, d3102, d0123, d0213, d1023, d1203, d2013, d2103}};

#if defined(__cpp_lib_span)
std::span<const wmtk::dart::Dart>
darts(int8_t dimension)
{
    switch (dimension) {
    case 0: return D0;
    case 1: return D1;
    case 2: return D2;
    case 3: return D3;
    default: assert(false); return {};
    }
}
#else
std::vector<wmtk::dart::Dart>
darts(int8_t dimension)
{
    switch (dimension) {
    case 0: return {D0.begin(),D0.end()};
    case 1: return {D1.begin(),D1.end()};
    case 2: return {D2.begin(),D2.end()};
    case 3: return {D3.begin(),D3.end()};
    default: assert(false); return {};
    }
}
#endif

} // namespace wmtk::tests::dart::utils
