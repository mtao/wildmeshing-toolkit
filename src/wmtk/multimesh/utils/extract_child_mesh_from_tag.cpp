#include "extract_child_mesh_from_tag.hpp"
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/multimesh/from_facet_orientations.hpp>
#include <wmtk/utils/Logger.hpp>
#include "extract_child_simplices_and_map_from_tag.hpp"
#include "internal/TupleTag.hpp"
#include "wmtk/utils/internal/is_manifold.hpp"

namespace wmtk::multimesh::utils {


template <typename T>

std::shared_ptr<Mesh> internal::TupleTag::extract_and_register_child_mesh_from_tag_handle(
    Mesh& m,
    const wmtk::attribute::TypedAttributeHandle<T>& tag_handle,
    const T& tag_value,
    bool child_is_free)
{
    std::vector<Tuple> tagged_tuples = utils::tagged_tuples<T>(m, tag_handle, tag_value);


    std::shared_ptr<Mesh> child_mesh_ptr;

    PrimitiveType pt = tag_handle.primitive_type();
    if (child_is_free) {
        switch (pt) {
        case PrimitiveType::Vertex: {
            child_mesh_ptr = std::make_shared<PointMesh>(tagged_tuples.size());
            break;
            // throw std::runtime_error("not implemented for vertex");
        }
        case PrimitiveType::Edge: {
            std::shared_ptr<EdgeMesh> meshptr = std::make_shared<EdgeMesh>();
            meshptr->initialize_free(tagged_tuples.size());
            child_mesh_ptr = meshptr;
            break;
        }
        case PrimitiveType::Triangle: {
            std::shared_ptr<TriMesh> meshptr = std::make_shared<TriMesh>();
            meshptr->initialize_free(tagged_tuples.size());
            child_mesh_ptr = meshptr;
            break;
        }
        case PrimitiveType::Tetrahedron: {
            std::shared_ptr<TetMesh> meshptr = std::make_shared<TetMesh>();
            meshptr->initialize_free(tagged_tuples.size());
            child_mesh_ptr = meshptr;
            break;
        }
        default: throw std::runtime_error("invalid child mesh type");
        }

    } else {
        MatrixXl S;
        std::tie(S, tagged_tuples) =
            extract_child_simplices_and_map_from_tag<T>(m, tag_handle, tag_value);
        child_mesh_ptr = Mesh::from_vertex_indices(S);
    }

    assert(bool(child_mesh_ptr));
    auto& child = *child_mesh_ptr;
    try {
        if (!wmtk::utils::internal::is_manifold(child)) {
            spdlog::warn("Mesh created by extract_child_mesh_from_tag is not manifold");
        }
    } catch (const std::exception& e) {
        spdlog::warn("Mesh created by extract_child_mesh_from_tag is not manifold");
    }

    std::vector<std::array<Tuple, 2>> child_to_parent_map =
        wmtk::multimesh::from_facet_orientations(m, child, tagged_tuples);
    assert(tagged_tuples.size() == child.capacity(pt));

    m.register_child_mesh(child_mesh_ptr, child_to_parent_map);
    return child_mesh_ptr;
}

std::shared_ptr<Mesh> extract_and_register_child_mesh_from_tag(
    Mesh& m,
    const std::string& tag,
    const int64_t tag_value,
    const PrimitiveType pt,
    bool child_is_free)
{
    assert(m.top_simplex_type() >= pt);
    auto tag_handle = m.get_attribute_handle<int64_t>(tag, pt).as<int64_t>();
    return extract_and_register_child_mesh_from_tag_handle(m, tag_handle, tag_value, child_is_free);
}

std::shared_ptr<Mesh> extract_and_register_child_mesh_from_tag_handle(
    Mesh& m,
    const wmtk::attribute::TypedAttributeHandle<int64_t>& tag_handle,
    const int64_t tag_value,
    bool child_is_free)
{
    return internal::TupleTag::extract_and_register_child_mesh_from_tag_handle(
        m,
        tag_handle,
        tag_value,
        child_is_free);
}

std::shared_ptr<Mesh> extract_and_register_child_mesh_from_tag(
    wmtk::attribute::MeshAttributeHandle& tag_handle,
    const wmtk::attribute::MeshAttributeHandle::ValueVariant& tag_value,
    bool child_is_free)
{
    return std::visit(
        [&](auto&& handle) noexcept -> std::shared_ptr<Mesh> {
            using HandleType = typename std::decay_t<decltype(handle)>;
            using T = typename HandleType::Type;
            if constexpr (wmtk::attribute::MeshAttributeHandle::template handle_type_is_basic<
                              HandleType>()) {
                return std::visit(
                    [&](auto&& value) noexcept -> std::shared_ptr<Mesh> {
                        if constexpr (std::is_convertible_v<std::decay_t<decltype(value)>, T>) {
                            return internal::TupleTag::
                                extract_and_register_child_mesh_from_tag_handle(
                                    tag_handle.mesh(),
                                    handle,
                                    T(value),
                                    child_is_free);
                        } else {
                            log_and_throw_error(
                                "Tried to use a tag value that was not convertible to "
                                "the tag attribute type");
                            return {};
                        }
                    },
                    tag_value);
            } else {
                log_and_throw_error("Tried to use a non-primitive meshattributehandle when "
                                    "extracting a child mesh fro ma tag");
            }
            return nullptr;
        },
        tag_handle.handle());
}

} // namespace wmtk::multimesh::utils
