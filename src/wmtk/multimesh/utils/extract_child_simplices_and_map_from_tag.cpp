
#include "extract_child_simplices_and_map_from_tag.hpp"
#include <wmtk/Mesh.hpp>
#include <wmtk/dart/utils/permute.hpp>
#include <wmtk/utils/EigenMatrixWriter.hpp>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/internal/compactify_eigen_indices.hpp>
#include <wmtk/utils/internal/identity_tuple.hpp>
#include <wmtk/utils/internal/manifold_decomposition.hpp>

namespace wmtk::multimesh::utils {

template <typename T>
std::pair<MatrixXl, std::vector<Tuple>> extract_child_simplices_and_map_from_tag(
    const Mesh& m,
    const wmtk::attribute::TypedAttributeHandle<T>& tag_handle,
    const T& tag_value,
    bool decompose_nonmanifold)
{
    PrimitiveType pt = tag_handle.primitive_type();
    std::vector<Tuple> tagged_tuples = utils::tagged_tuples<T>(m, tag_handle, tag_value);
    if (tagged_tuples.empty()) {
        return {};
    }
    MatrixXl S;
    wmtk::utils::EigenMatrixWriter em;
    m.serialize(em);
    MatrixXl F = em.get_simplex_vertex_matrix();
    const dart::SimplexDart& mesh_sd = dart::SimplexDart::get_singleton(m.top_simplex_type());
    S.resize(tagged_tuples.size(), get_primitive_type_id(pt) + 1);

    for (size_t j = 0; j < tagged_tuples.size(); ++j) {
        const dart::Dart d = mesh_sd.dart_from_tuple(tagged_tuples[j]);
        auto f = F.row(d.global_id());
        auto perm = dart::utils::permute(f, d.permutation());
        S.row(j) = perm.transpose().head(S.cols());
    }

    S = wmtk::utils::internal::compactify_eigen_indices(S);
    if (decompose_nonmanifold) {
        switch (S.cols()) {
        case 1: S = wmtk::utils::internal::manifold_decomposition<1>(S); break;
        case 2: S = wmtk::utils::internal::manifold_decomposition<2>(S); break;
        case 3: S = wmtk::utils::internal::manifold_decomposition<3>(S); break;
        case 4: S = wmtk::utils::internal::manifold_decomposition<4>(S); break;
        default: break;
        }
    }

    return std::make_pair(S, tagged_tuples);
}

std::pair<MatrixXl, std::vector<Tuple>> extract_child_simplices_and_map_from_tag(
    const wmtk::attribute::MeshAttributeHandle& tag_handle,
    const wmtk::attribute::MeshAttributeHandle::ValueVariant& tag_value,
    bool decompose_nonmanifold)
{
    return std::visit(
        [&](auto&& handle, auto&& value) noexcept -> std::pair<MatrixXl, std::vector<Tuple>> {
            using HandleType = typename std::decay_t<decltype(handle)>;
            using T = typename HandleType::Type;
            if constexpr (wmtk::attribute::MeshAttributeHandle::template handle_type_is_basic<
                              HandleType>()) {
                if constexpr (std::is_convertible_v<std::decay_t<decltype(value)>, T>) {
                    return extract_child_simplices_and_map_from_tag<T>(
                        tag_handle.mesh(),
                        handle,
                        T(value),
                        decompose_nonmanifold);
                } else {
                    log_and_throw_error("Tried to use a tag value that was not convertible to "
                                        "the tag attribute type");
                    return {};
                }
            } else {
                log_and_throw_error("Tried to use a non-primitive meshattributehandle when "
                                    "extracting a child mesh fro ma tag");
            }
            return {};
        },
        tag_handle.handle(),
        tag_value);
}


template <typename T>
std::vector<Tuple> tagged_tuples(
    const Mesh& m,
    const wmtk::attribute::TypedAttributeHandle<T>& tag_handle,
    const T& tag_value)
{
    auto tags = m.create_const_accessor(tag_handle);
    const PrimitiveType pt = tag_handle.primitive_type();

    std::vector<Tuple> tagged_tuples;
    for (const Tuple& t : m.get_all(pt)) {
        if (tags.const_scalar_attribute(t) == tag_value) {
            tagged_tuples.emplace_back(t);
        }
    }
    return tagged_tuples;
}

template std::vector<Tuple> tagged_tuples<char>(
    const Mesh& m,
    const wmtk::attribute::TypedAttributeHandle<char>& tag_handle,
    const char&);
template std::vector<Tuple> tagged_tuples<int64_t>(
    const Mesh& m,
    const wmtk::attribute::TypedAttributeHandle<int64_t>& tag_handle,
    const int64_t&);
template std::vector<Tuple> tagged_tuples<double>(
    const Mesh& m,
    const wmtk::attribute::TypedAttributeHandle<double>& tag_handle,
    const double&);
template std::vector<Tuple> tagged_tuples<wmtk::Rational>(
    const Mesh& m,
    const wmtk::attribute::TypedAttributeHandle<wmtk::Rational>& tag_handle,
    const wmtk::Rational&);
} // namespace wmtk::multimesh::utils
