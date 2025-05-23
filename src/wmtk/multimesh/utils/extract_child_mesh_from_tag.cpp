#include "extract_child_mesh_from_tag.hpp"
#include <map>
#include <numeric>
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/multimesh/MultiMeshManager.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/Primitive.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/attribute/MeshAttributeHandle.hpp>
#include <wmtk/attribute/TypedAttributeHandle.hpp>
#include <wmtk/utils/Logger.hpp>
#include "internal/TupleTag.hpp"

namespace wmtk::multimesh::utils {


template <typename T>

std::shared_ptr<Mesh> internal::TupleTag::extract_and_register_child_mesh_from_tag_handle(
    Mesh& m,
    const wmtk::attribute::TypedAttributeHandle<T>& tag_handle,
    const T& tag_value,
    bool child_is_free)
{
    auto tags = m.create_const_accessor(tag_handle);
    const PrimitiveType pt = tag_handle.primitive_type();

    std::vector<Tuple> tagged_tuples;
    for (const Tuple& t : m.get_all(pt)) {
        if (tags.const_scalar_attribute(t) == tag_value) {
            tagged_tuples.emplace_back(t);
        }
    }

    auto run_edge = [&]() {
        std::map<int64_t, int64_t> parent_to_child_vertex_map;
        // int64_t child_vertex_count = 0;

        RowVectors2l edge_mesh_matrix;
        edge_mesh_matrix.resize(tagged_tuples.size(), 2);

        for (size_t i = 0; i < tagged_tuples.size(); ++i) {
            const std::array<int64_t, 2> vs = {
                {m.id(tagged_tuples[i], PrimitiveType::Vertex),
                 m.id(
                     m.switch_tuple(tagged_tuples[i], PrimitiveType::Vertex),
                     PrimitiveType::Vertex)}};


            // check and add v0, v1 to the vertex map
            for (int k = 0; k < 2; k++) {
                size_t size = parent_to_child_vertex_map.size();
                parent_to_child_vertex_map.try_emplace(vs[k], size);
                edge_mesh_matrix(i, k) = parent_to_child_vertex_map[vs[k]];
            }
        }

        std::shared_ptr<EdgeMesh> child_ptr = std::make_shared<EdgeMesh>();
        auto& child = *child_ptr;
        child.initialize(edge_mesh_matrix);
        return child_ptr;
    };

    auto run_face = [&]() {
        std::map<int64_t, int64_t> parent_to_child_vertex_map;
        int64_t child_vertex_count = 0;

        RowVectors3l tri_mesh_matrix;
        tri_mesh_matrix.resize(tagged_tuples.size(), 3);
        for (int64_t i = 0; i < tagged_tuples.size(); ++i) {
            // TODO: check if this break the orientation of the map
            const std::array<int64_t, 3> vs = {
                {m.id(tagged_tuples[i], PrimitiveType::Vertex),
                 m.id(
                     m.switch_tuple(tagged_tuples[i], PrimitiveType::Vertex),
                     PrimitiveType::Vertex),
                 m.id(
                     m.switch_tuples(
                         tagged_tuples[i],
                         {PrimitiveType::Edge, PrimitiveType::Vertex}),
                     PrimitiveType::Vertex)}};

            // check and add v0, v1, v2 to the vertex map
            for (int k = 0; k < 3; k++) {
                size_t size = parent_to_child_vertex_map.size();
                parent_to_child_vertex_map.try_emplace(vs[k], size);
                tri_mesh_matrix(i, k) = parent_to_child_vertex_map.at(vs[k]);
            }
        }
        std::shared_ptr<TriMesh> child_ptr = std::make_shared<TriMesh>();
        auto& child = *child_ptr;
        child.initialize(tri_mesh_matrix);

        return child_ptr;
    };

    auto run_tet = [&]() {
        std::map<int64_t, int64_t> parent_to_child_vertex_map;
        int64_t child_vertex_count = 0;

        RowVectors4l tet_mesh_matrix;

        tet_mesh_matrix.resize(tagged_tuples.size(), 4);
        for (int64_t i = 0; i < tagged_tuples.size(); ++i) {
            const std::array<int64_t, 4> vs = {
                {m.id(tagged_tuples[i], PrimitiveType::Vertex),
                 m.id(
                     m.switch_tuple(tagged_tuples[i], PrimitiveType::Vertex),
                     PrimitiveType::Vertex),
                 m.id(
                     m.switch_tuples(
                         tagged_tuples[i],
                         {PrimitiveType::Triangle, PrimitiveType::Edge, PrimitiveType::Vertex}),
                     PrimitiveType::Vertex),
                 m.id(
                     m.switch_tuples(
                         tagged_tuples[i],
                         {PrimitiveType::Edge, PrimitiveType::Vertex}),
                     PrimitiveType::Vertex)}};

            for (int k = 0; k < 4; ++k) {
                size_t size = parent_to_child_vertex_map.size();
                parent_to_child_vertex_map.try_emplace(vs[k], size);
                tet_mesh_matrix(i, k) = parent_to_child_vertex_map[vs[k]];
            }
        }

        // TODO: use TV matrix instead of switches


        std::shared_ptr<TetMesh> child_ptr = std::make_shared<TetMesh>();
        auto& child = *child_ptr;
        child.initialize(tet_mesh_matrix);
        return child_ptr;
    };


    std::shared_ptr<Mesh> child_mesh_ptr;

    if (child_is_free) {
        switch (pt) {
        case PrimitiveType::Vertex: throw("not implemented");
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
        default: throw("invalid child mesh type");
        }

    } else {
        switch (pt) {
        case PrimitiveType::Vertex: throw("not implemented");
        case PrimitiveType::Edge: {
            child_mesh_ptr = run_edge();
            break;
        }
        case PrimitiveType::Triangle: {
            child_mesh_ptr = run_face();
            break;
        }
        case PrimitiveType::Tetrahedron: {
            child_mesh_ptr = run_tet();
            break;
        }
        default: throw("invalid child mesh type");
        }
    }

    assert(bool(child_mesh_ptr));
    auto& child = *child_mesh_ptr;

    std::vector<std::array<Tuple, 2>> child_to_parent_map(tagged_tuples.size());
    assert(tagged_tuples.size() == child.capacity(pt));

    const auto child_top_dimension_tuples = child.get_all(pt);

    for (size_t i = 0; i < tagged_tuples.size(); ++i) {
        child_to_parent_map[i] = {{child_top_dimension_tuples[i], tagged_tuples[i]}};
    }

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
