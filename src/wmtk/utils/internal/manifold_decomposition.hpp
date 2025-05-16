#pragma once
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <map>
#include <set>
#if defined(_cpp_lib_span)
#include <span>
#endif
#include <wmtk/Tuple.hpp>
#include <wmtk/Types.hpp>
#include <wmtk/dart/Dart.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/utils/from_local_vertex_permutation.hpp>
#include <wmtk/dart/utils/get_canonical_subdart_permutation.hpp>
#include <wmtk/dart/utils/get_local_vertex_permutation.hpp>
#include <wmtk/utils/DisjointSet.hpp>
#include "compactify_eigen_indices.hpp"


#include "IndexSimplexMapper.hpp"


namespace wmtk::utils::internal {


template <size_t Dim>
struct ManifoldDecomposition
{
    std::map<std::array<int64_t, Dim - 1>, std::set<dart::Dart>> face_map;
    wmtk::RowVectors<int64_t, Dim> manifold_decomposition;

    // face matrix and mm_map should have a consistent ordering because iteration through std::map
    // is stable. Indexing is based on the input's indexing scheme
    wmtk::RowVectors<int64_t, Dim - 1> face_matrix(bool compactify=false) const;
    std::vector<std::array<Tuple, 2>> mm_map() const;
};
// uses indices to find a manifold decomposition of an input mesh

// returns the indices of non-manifold regions in the original input's index sset
// here we use "boundary"-manifoldness. That is, a mesh is manifold if every k-1 face is shared by
// at most two simplices. This only detects k-1 non-manifoldness and fails to generate a manifold
// decomposition if, for (j < k-1) the open star of a j-simplex is not a k-ball (i.e lower
// non-manifoldness). This _should_ resolve non-manifoldness where the subcomplex of k-simplices
// fused by k-1 faces forms distinct k-balls.
template <int Dim>
ManifoldDecomposition<Dim> boundary_manifold_decomposition(
    Eigen::Ref<const RowVectors<int64_t, Dim>> S)
{
    ManifoldDecomposition<Dim> MD;
    RowVectors<int64_t, Dim>& R = MD.manifold_decomposition; // returned simplices
    auto& face_map = MD.face_map;
    if (Dim == 1) {
        R = S;
        return MD;
    }
    IndexSimplexMapper ism(S);

    const PrimitiveType pt = get_primitive_type_from_id(Dim - 1);
    const PrimitiveType face_pt = get_primitive_type_from_id(Dim - 2);

    const auto& face_sd = dart::SimplexDart::get_singleton(face_pt);
    const auto& sd = dart::SimplexDart::get_singleton(pt);

    std::map<int64_t, std::set<dart::Dart>> coboundary;
    for (int j = 0; j < S.rows(); ++j) {
        std::array<int64_t, Dim - 1> r;
        using MT = typename Vector<int64_t, Dim - 1>::MapType;
        MT(r.data()) = S.row(j).transpose().template head<Dim - 1>();
        int64_t last = S(j, Dim - 1);

        auto s = S.row(j);
        std::array<int64_t, Dim> ss;
        std::array<int8_t, Dim> local_permutation;
        std::iota(local_permutation.begin(), local_permutation.end(), 0);
        std::copy(s.begin(), s.end(), ss.begin());

        auto add = [&]() {
            int64_t index = ism.get_index(r);

            std::array<int64_t, Dim - 1> sorted = r;
            std::sort(sorted.begin(), sorted.end());
            int8_t p = wmtk::dart::utils::get_canonical_subdart_permutation(ss, sorted);
            auto myd = wmtk::dart::Dart(j, p);
            coboundary[index].emplace(myd);
        };

        add();

        for (int k = 0; k < Dim - 1; ++k) {
            int64_t old = r[k];
            r[k] = last;
            add();
            r[k] = old;
        }
    }
    if constexpr (Dim > 1) {
        const auto& S = ism.simplices<Dim - 1>();
        auto F = ism.simplices<Dim - 2>();
        // spdlog::warn("{} {} {}", Dim, S.size(), F.size());
        //  static_assert(std::decay_t<decltype(F)>::value_type::size() == Dim - 2);
        for (const auto& [k, v] : coboundary) {
            std::vector<std::array<int64_t, Dim>> faces;
            for (const auto& a : v) {
                faces.emplace_back(S[a.global_id()]);
            }
            // spdlog::warn(
            //    "{}/{}: {}  => {}",
            //    k,
            //    F.size(),
            //    fmt::join(F[k], ","),
            //    fmt::join(faces, ","));
            // spdlog::warn("{}  {} {}=> {}", k, fmt::join(S, ","), S.size(), v);
        }
    }

    std::vector<std::array<int64_t, 2>> B;

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


    // if a face was manifold then fuse pairs of vertices on those two faces
    std::set<int64_t> nonmanifold_faces;
    wmtk::utils::DisjointSet ds(R.size());
    auto F = ism.simplices<Dim - 2>();
    for (const auto& [face, simplices] : coboundary) {
        if (simplices.size() == 2) {
            std::array<int64_t, 2>& r = B.emplace_back();
            std::transform(
                simplices.begin(),
                simplices.end(),
                r.begin(),
                [](const dart::Dart& d) -> int64_t { return d.global_id(); });

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
            face_map.emplace(F.at(face), simplices);
            // spdlog::info("Found a nonmanifold face");
            // nonmanifold_faces.emplace(face);
            //  spdlog::info(
            //      "Face {} ({}) got Facets {}",
            //      face,
            //      fmt::join(ism.simplices<Dim - 2>()[face], ","),
            //      fmt::join(simplices, ","));
            //  for (const auto& j : simplices) {
            //      std::cout << j << ": " << S.row(j) << std::endl;
            //  }
        }
    }
    std::map<size_t, size_t> unindexer;
    const auto roots = ds.roots();
    for (size_t j = 0; j < roots.size(); ++j) {
        unindexer[roots[j]] = j;
    }
    R.noalias() = R.unaryExpr([&](int64_t i) -> int64_t { return unindexer.at(ds.get_root(i)); });
    /*
    NMF.resize(nonmanifold_faces.size(), Dim - 1);
    {
        int index = 0;
        const auto& F = ism.simplices<Dim - 2>();
        for (int64_t i : nonmanifold_faces) {
            auto r = NMF.row(index++);
            const auto& s = F[i];
            assert(s.size() == r.size());
            std::copy(s.begin(), s.end(), r.begin());

            face_map.emplace(s, coboundary.at(i));
        }
    }
    */
    return MD;
}


template <int Dim>
RowVectors<int64_t, Dim> manifold_decomposition(Eigen::Ref<const RowVectors<int64_t, Dim>> S)
{
    // currently only supports face fusing
    return std::get<0>(boundary_manifold_decomposition<Dim>(S));
}


template <size_t Dim>
wmtk::RowVectors<int64_t, Dim - 1> ManifoldDecomposition<Dim>::face_matrix(bool compactify) const
{
    wmtk::RowVectors<int64_t, Dim - 1> R(face_map.size(), Dim - 1);
    Eigen::Index i = 0;
    for (const auto& [key, value] : face_map) {
        using MT = typename wmtk::Vector<int64_t, Dim - 1>::ConstMapType;


        R.row(i++) = MT(key.data()).transpose();
    }
    if(compactify) {
        R = compactify_eigen_indices(R);
    }
    return R;
}

template <size_t Dim>
std::vector<std::array<Tuple, 2>> ManifoldDecomposition<Dim>::mm_map() const
{
    std::vector<std::array<Tuple, 2>> tups;
    tups.reserve(face_map.size());
    Eigen::Index i = 0;
    const auto& sd = dart::SimplexDart::get_singleton(get_primitive_type_from_id(Dim - 2));
    int8_t id = sd.identity();
    for (const auto& [key, value] : face_map) {
        Tuple t = sd.tuple_from_dart(wmtk::dart::Dart(i++, id));
        tups.emplace_back(std::array<Tuple, 2>{{t, value}});
    }
    return tups;
}
} // namespace wmtk::utils::internal
