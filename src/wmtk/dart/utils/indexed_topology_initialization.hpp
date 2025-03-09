#pragma once
#include <numeric>
#include <wmtk/Types.hpp>
#include <wmtk/utils/DisjointSet.hpp>
#include "../DartTopologyAccessor.hpp"
#include "../SimplexDart.hpp"

namespace wmtk::dart::utils {

namespace internal {
constexpr int factorial(int n)
{
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}
constexpr int nCr(int n, int r)
{
    return factorial(n) / (factorial(r) * factorial(n - r));
}
} // namespace internal

template <int SimplexDim, int Dim, typename MeshType>
RowVectors<int64_t, internal::nCr(Dim + 1, SimplexDim + 1)> indexed_topology_initialization(
    const DartTopologyAccessor<Dim, MeshType>& acc)
{
    constexpr static int SimplexCount = internal::nCr(Dim + 1, SimplexDim + 1);
    using namespace internal;
    spdlog::warn(
        "{}ncr{} ->  {} {} {}",
        Dim,
        SimplexDim,
        factorial(Dim),
        factorial(SimplexDim),
        factorial(Dim - SimplexDim));
    spdlog::warn("Simplex count: {}", SimplexCount);

    constexpr static PrimitiveType SimplexType = get_primitive_type_from_id(SimplexDim);
    constexpr static PrimitiveType FacetType = get_primitive_type_from_id(Dim);
    const int facet_count = acc.size();
    spdlog::info("Sizes {}", acc.size());
    RowVectors<int64_t, SimplexCount> R(facet_count, SimplexCount);
    std::iota(R.data(), R.data() + R.size(), int64_t(0));
    spdlog::info("Call R");
    std::cout << R << std::endl;


    const auto& sd = SimplexDart::get_singleton(get_primitive_type_from_id(Dim));
    auto get_index = [&](const auto& dart) {
        assert(dart.global_id() < facet_count);
        assert(dart.permutation() < sd.size());
        return sd.size() * dart.global_id() + dart.permutation();
    };

    wmtk::utils::DisjointSet dart_ds(facet_count * sd.size());
    spdlog::warn("Facet count: {} DS size: {}", facet_count, dart_ds.size());
    for (int j = 0; j < facet_count; ++j) {
        for (int p = 0; p < sd.size(); ++p) {
            const Dart d(j, p);
            int64_t index = get_index(d);
            // spdlog::warn("{} => {}", std::string(d), index);
            for (int k = SimplexDim + 1; k <= Dim; ++k) {
                const Dart d2 = acc.switch_dart(d, get_primitive_type_from_id(k));
                if (d2.is_null()) {
                    spdlog::info(
                        "{} {} was boundary",
                        std::string(d),
                        sd.simplex_index(d2, SimplexType));
                    continue;
                }
                int64_t index2 = get_index(d2);
                // spdlog::warn("{} => {}", std::string(d2), index2);
                spdlog::info(
                    "Merging {} {} with {} {} from a {}",
                    std::string(d),
                    sd.simplex_index(d, SimplexType),
                    std::string(d2),
                    sd.simplex_index(d2, SimplexType),
                    primitive_type_name(get_primitive_type_from_id(k)));
                dart_ds.merge(index, index2);
            }
        }
    }
    return {};

    auto get_dart = [&](const size_t index) {
        const auto global_id = int64_t(index / sd.size());
        const auto permutation = int8_t(index % sd.size());
        return Dart(global_id, permutation);
    };
    wmtk::utils::DisjointSet ds(R.size());
    for (size_t dart_index = 0; dart_index < dart_ds.size(); ++dart_index) {
        size_t root_index = dart_ds.get_root(dart_index);
        spdlog::info("Root of {} is {}", dart_index, root_index);
        Dart root_dart = get_dart(root_index);
        Dart my_dart = get_dart(dart_index);
        int8_t root_lindex = sd.simplex_index(root_dart, SimplexType);
        int8_t my_lindex = sd.simplex_index(my_dart, SimplexType);
        assert(root_dart.global_id() < R.rows());
        assert(my_dart.global_id() < R.rows());
        assert(root_lindex < R.cols());
        assert(my_lindex < R.cols());

        size_t a = R(root_dart.global_id(), root_lindex);
        size_t b = R(my_dart.global_id(), my_lindex);
        spdlog::info(
            "Mrging {} {} from {}/{} {}/{}",
            a,
            b,
            std::string(root_dart),
            root_lindex,
            std::string(my_dart),
            my_lindex);
        ds.merge(a, b);
    }
    std::map<size_t, size_t> unindexer;
    const auto roots = ds.roots();
    for (size_t j = 0; j < roots.size(); ++j) {
        unindexer[roots[j]] = j;
    }
    R.noalias() = R.unaryExpr([&](int64_t i) -> int64_t { return unindexer.at(ds.get_root(i)); });
    return R;
}
} // namespace wmtk::dart::utils
