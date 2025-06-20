
#include <numeric>
#include "EdgeMesh.hpp"
#include "Mesh.hpp"
#include "PointMesh.hpp"
#include "TetMesh.hpp"
#include "TriMesh.hpp"

#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/Rational.hpp>

#include "Primitive.hpp"

namespace wmtk {

Mesh::Mesh(Mesh&& other)
    : m_attribute_manager(std::move(other.m_attribute_manager))
    , m_multi_mesh_manager(std::move(other.m_multi_mesh_manager))
    , m_top_cell_dimension(other.m_top_cell_dimension)
    , m_is_free(other.m_is_free)
{
    m_flag_handles = std::move(other.m_flag_handles);
}


Mesh& Mesh::operator=(Mesh&& other)
{
    m_attribute_manager = std::move(other.m_attribute_manager);
    m_multi_mesh_manager = std::move(other.m_multi_mesh_manager);
    m_flag_handles = std::move(other.m_flag_handles);
    m_top_cell_dimension = other.m_top_cell_dimension;
    m_is_free = other.m_is_free;

    return *this;
}

Mesh::Mesh(const int64_t& dimension)
    : Mesh(dimension, dimension)
{}

Mesh::Mesh(const int64_t& dimension, const int64_t& max_primitive_type_id)
    : m_attribute_manager(max_primitive_type_id + 1)
    , m_multi_mesh_manager(max_primitive_type_id + 1)
    , m_top_cell_dimension(dimension)
{
    m_flag_handles.reserve(max_primitive_type_id + 1);
    for (int64_t j = 0; j <= max_primitive_type_id; ++j) {
        m_flag_handles.emplace_back(
            register_attribute_typed<char>("flags", get_primitive_type_from_id(j), 1, false, 0));
    }
}


Mesh::~Mesh()
{
    m_multi_mesh_manager.detach_children();
}

std::shared_ptr<Mesh> Mesh::from_vertex_indices(Eigen::Ref<const MatrixXl> S)
{
    switch (S.cols()) {
    case 1: {
        auto p = std::make_shared<PointMesh>(S.rows());
        return p;
    }
    case 2: {
        auto p = std::make_shared<EdgeMesh>();
        p->initialize(S);
        return p;
    }
    case 3: {
        auto p = std::make_shared<TriMesh>();
        p->initialize(S);
        return p;
    }
    case 4: {
        auto p = std::make_shared<TetMesh>();
        p->initialize(S);
        return p;
    }
    default:
        throw std::invalid_argument(
            "Cannot make a mesh from vertex indices other than for S.cols() in 1,2,3,4");
    }
}

} // namespace wmtk
