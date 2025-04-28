#pragma once
//#include <fmt/ranges.h>
//#include <spdlog/spdlog.h>
#include <map>
#include <set>
#if defined(_cpp_lib_span)
#include <span>
#endif
#include <wmtk/Tuple.hpp>
#include <wmtk/Types.hpp>
#include <wmtk/dart/Dart.hpp>
#include <wmtk/utils/DisjointSet.hpp>


#include "IndexSimplexMapper.hpp"


namespace wmtk::utils::internal {


// uses indices to find a manifold decomposition of an input mesh

// returns the indices of non-manifold regions in the original input's index sset
template <int Dim>
std::tuple<RowVectors<int64_t, Dim>, RowVectors<int64_t, Dim - 1>> boundary_manifold_decomposition(
    Eigen::Ref<const RowVectors<int64_t, Dim>> S)
{
    IndexSimplexMapper ism(S);


    std::map<int64_t, std::set<int64_t>> coboundary;
    for (int j = 0; j < S.rows(); ++j) {
        std::array<int64_t, Dim - 1> R;
        using MT = typename Vector<int64_t, Dim - 1>::MapType;
        MT(R.data()) = S.row(j).transpose().template head<Dim - 1>();
        int64_t last = S(j, Dim - 1);

        auto s = S.row(j);
        std::array<int64_t, Dim> ss;
        std::copy(s.begin(), s.end(), ss.begin());

        coboundary[ism.get_index(R)].emplace(j);

        for (int k = 0; k < Dim - 1; ++k) {
            int64_t old = R[k];
            R[k] = last;
            coboundary[ism.get_index(R)].emplace(j);
            R[k] = old;
        }
    }

    std::vector<std::array<int64_t, 2>> B;

    RowVectors<int64_t, Dim> R; // returned simplices
    RowVectors<int64_t, Dim - 1> NMF; // nonmanifold faces
    R.resizeLike(S);
    {
        int64_t index = 0;
        for (int j = 0; j < R.rows(); ++j) {
            for (int k = 0; k < R.cols(); ++k) {
                R(j, k) = index++;
            }
        }
    }


    std::set<int64_t> nonmanifold_faces;
    wmtk::utils::DisjointSet ds(R.size());
    for (const auto& [face, simplices] : coboundary) {
        if (simplices.size() == 2) {
            std::array<int64_t, 2>& r = B.emplace_back();
            std::copy(simplices.begin(), simplices.end(), r.begin());

            auto oa = S.row(r[0]);
            auto a = R.row(r[0]);

            auto ob = S.row(r[1]);
            auto b = R.row(r[1]);
            // lazy fuse - would be better if i had some orientation data
            for (int j = 0; j < Dim; ++j) {
                for (int k = 0; k < Dim; ++k) {
                    if (oa(j) == ob(k)) {
                        ds.merge(a(j), b(k));
                    }
                }
            }
        } else if (simplices.size() > 2) {
            nonmanifold_faces.emplace(face);
            // spdlog::info(
            //     "Face {} ({}) got Facets {}",
            //     face,
            //     fmt::join(ism.simplices<Dim - 2>()[face], ","),
            //     fmt::join(simplices, ","));
            // for (const auto& j : simplices) {
            //     std::cout << j << ": " << S.row(j) << std::endl;
            // }
        }
    }
    std::map<size_t, size_t> unindexer;
    const auto roots = ds.roots();
    for (size_t j = 0; j < roots.size(); ++j) {
        unindexer[roots[j]] = j;
    }
    R.noalias() = R.unaryExpr([&](int64_t i) -> int64_t { return unindexer.at(ds.get_root(i)); });
    NMF.resize(nonmanifold_faces.size(), Dim - 1);
    {
        int index = 0;
        const auto& F = ism.simplices<Dim - 2>();
        for (int64_t i : nonmanifold_faces) {
            auto r = NMF.row(index++);
            const auto& s = F[i];
            assert(s.size() == r.size());
            std::copy(s.begin(), s.end(), r.begin());
        }
    }
    return std::make_tuple(R, NMF);
}


template <int Dim>
RowVectors<int64_t, Dim> manifold_decomposition(Eigen::Ref<const RowVectors<int64_t, Dim>> S)
{
    // currently only supports face fusing
    return std::get<0>(boundary_manifold_decomposition<Dim>(S));
}
} // namespace wmtk::utils::internal
