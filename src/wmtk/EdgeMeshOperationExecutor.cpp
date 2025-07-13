
#include "EdgeMeshOperationExecutor.hpp"
#include <fmt/ranges.h>
#include <wmtk/operations/internal/SplitAlternateFacetData.hpp>
#include <wmtk/simplex/faces.hpp>
#include <wmtk/simplex/top_dimension_cofaces.hpp>
#include <wmtk/simplex/top_dimension_cofaces_iterable.hpp>

namespace wmtk {
// constructor
EdgeMesh::EdgeMeshOperationExecutor::EdgeMeshOperationExecutor(
    EdgeMesh& m,
    const Tuple& operating_tuple)
    : operations::edge_mesh::EdgeOperationData(m, operating_tuple)
    , flag_accessors{{m.get_flag_accessor(PrimitiveType::Vertex), m.get_flag_accessor(PrimitiveType::Edge)}}
    , ee_accessor(m.create_accessor<int64_t>(m.m_ee_handle))
    , ev_accessor(m.create_accessor<int64_t>(m.m_ev_handle))
    , ve_accessor(m.create_accessor<int64_t>(m.m_ve_handle))
{
    Tuple operating_tuple_switch_vertex = mesh().switch_vertex(operating_tuple);
    // store ids of incident vertices

    // update hash on neighborhood
    if (!mesh().is_boundary_vertex(m_operating_tuple)) {
        m_neighbor_eids[0] = mesh().id_edge(mesh().switch_edge(m_operating_tuple));
    }
    if (!mesh().is_boundary_vertex(operating_tuple_switch_vertex)) {
        m_neighbor_eids[1] = mesh().id_edge(mesh().switch_edge(operating_tuple_switch_vertex));
    }


    if (m_neighbor_eids[0] == m_neighbor_eids[1] && m_neighbor_eids[0] == operating_edge_id()) {
        m_is_self_loop = true;
    }

    global_ids_to_update.resize(2);
    if (m.has_child_mesh_in_dimension(0)) {
        auto add = [&](const Tuple& t) {
            simplex::Simplex s(PrimitiveType::Vertex, t);
            global_ids_to_update.at(0).emplace_back(mesh().id(s));
        };
        add(m_operating_tuple);
        add(mesh().switch_vertex(m_operating_tuple));
    }

    // if (m.has_child_mesh_in_dimension(1)) {
    //     global_ids_to_potential_tuples.at(1).emplace_back(
    //         mesh().id(simplex::Simplex::edge(m_operating_tuple)),
    //         wmtk::simplex::top_dimension_cofaces_tuples(
    //             mesh(),
    //             simplex::Simplex::edge(m_operating_tuple)));
    // }
}



const std::array<std::vector<int64_t>, 4>
EdgeMesh::EdgeMeshOperationExecutor::get_split_simplices_to_delete(
    const Tuple& tuple,
    const EdgeMesh& m)
{
    std::array<std::vector<int64_t>, 4> ids;
    ids[1].emplace_back(m.id_edge(tuple));
    return ids;
}

const std::array<std::vector<int64_t>, 4>
EdgeMesh::EdgeMeshOperationExecutor::get_collapse_simplices_to_delete(
    const Tuple& tuple,
    const EdgeMesh& m)
{
    std::array<std::vector<int64_t>, 4> ids;
    ids[0].emplace_back(m.id_vertex(tuple));
    ids[1].emplace_back(m.id_edge(tuple));
    if (m.is_free()) {
        ids[0].emplace_back(m.id_vertex(m.switch_tuple(tuple, PrimitiveType::Vertex)));
    }
    return ids;
}


void EdgeMesh::EdgeMeshOperationExecutor::split_edge()
{
    set_split();
    m_output_tuple = split_edge_single_mesh();
    // TODO: Implement for multi_mesh in the future
}

Tuple EdgeMesh::EdgeMeshOperationExecutor::split_edge_single_mesh()
{
    simplex_ids_to_delete = get_split_simplices_to_delete(m_operating_tuple, mesh());

    // create new edges (facets)
    // m_split_e[i] is connect to m_neighbor_eids[i] and m_spine_vids[i]
    const auto& data = split_facet_data().add_facet(mesh(), m_operating_tuple);
    m_split_e = data.new_facet_indices;

    if (mesh().is_free()) {
        const std::vector<int64_t> new_vids =
            this->request_simplex_indices(PrimitiveType::Vertex, 2);
        assert(new_vids.size() == 2);
        std::copy(new_vids.begin(), new_vids.end(), m_free_split_v.begin());
        const int64_t v_new = new_vids[0];
        m_split_v = -1;
    } else {
        // create new vertex
        const std::vector<int64_t> new_vids =
            this->request_simplex_indices(PrimitiveType::Vertex, 1);
        assert(new_vids.size() == 1);
        const int64_t v_new = new_vids[0];
        m_split_v = v_new;
    }
    const int64_t local_vid = mesh().is_ccw(m_operating_tuple) ? 0 : 1;

    // update ee
    if (mesh().is_free()) {
    } else {
        // for 2 new edges
        auto ee_new_0 = ee_accessor.vector_attribute(m_split_e[0]);
        auto ee_new_1 = ee_accessor.vector_attribute(m_split_e[1]);
        ee_new_0[local_vid ^ 1] = m_split_e[1];
        ee_new_1[local_vid] = m_split_e[0];
        if (m_is_self_loop) {
            ee_new_0[local_vid] = m_split_e[1];
            ee_new_1[local_vid ^ 1] = m_split_e[0];
        } else {
            ee_new_0[local_vid] = m_neighbor_eids[0];
            ee_new_1[local_vid ^ 1] = m_neighbor_eids[1];
            // for neighbor edges
            for (int64_t i = 0; i < 2; i++) {
                if (m_neighbor_eids[i] != -1) {
                    auto ee_neighbor = ee_accessor.vector_attribute(m_neighbor_eids[i]);
                    auto ev_neighbor = ev_accessor.vector_attribute(m_neighbor_eids[i]);
                    for (int64_t j = 0; j < 2; j++) {
                        if (ee_neighbor[j] == operating_edge_id() &&
                            ev_neighbor[j] == m_spine_vids[i]) {
                            ee_neighbor[j] = m_split_e[i];
                            break;
                        }
                    }
                }
            }
        }
    }

    // update ev
    {
        // for new edges
        auto ev_new_0 = ev_accessor.vector_attribute(m_split_e[0]);
        auto ev_new_1 = ev_accessor.vector_attribute(m_split_e[1]);
        ev_new_0[local_vid] = m_spine_vids[0];
        if (mesh().is_free()) {
            ev_new_0[local_vid ^ 1] = m_free_split_v[0];
            ev_new_1[local_vid] = m_free_split_v[1];
        } else {
            ev_new_0[local_vid ^ 1] = m_split_v;
            ev_new_1[local_vid] = m_split_v;
        }
        ev_new_1[local_vid ^ 1] = m_spine_vids[1];
    }

    // update ve
    {
        // for new vertex
        if (mesh().is_free()) {
            ve_accessor.scalar_attribute(m_free_split_v[0]) = m_split_e[0];
            ve_accessor.scalar_attribute(m_free_split_v[1]) = m_split_e[1];
        } else {
            ve_accessor.scalar_attribute(m_split_v) = m_split_e[0];
        }

        // for spine vertices
        ve_accessor.scalar_attribute(m_spine_vids[0]) = m_split_e[0];
        ve_accessor.scalar_attribute(m_spine_vids[1]) = m_split_e[1];
    }
    delete_simplices();

    // prepare return Tuple
    auto ret_edge = mesh().edge_tuple_from_id(m_split_e[1]);

    // if the mesh is free we don't care about which edge is returned
    if (!mesh().is_free()) {
        if (mesh().id_vertex(ret_edge) != m_split_v) {
            ret_edge = mesh().switch_vertex(ret_edge);

            assert(mesh().id_edge(ret_edge) == m_split_e[1]);
            assert(mesh().id_vertex(ret_edge) == m_split_v);
            assert(mesh().id_vertex(mesh().switch_vertex(ret_edge)) == m_spine_vids[1]);
        }
    }

    return ret_edge;
}


void EdgeMesh::EdgeMeshOperationExecutor::collapse_edge()
{
    set_collapse();
    m_output_tuple = collapse_edge_single_mesh();
    // TODO: Implement for multi_mesh in the future
}


Tuple EdgeMesh::EdgeMeshOperationExecutor::collapse_edge_single_mesh()
{
    if (mesh().is_free()) {
        simplex_ids_to_delete = get_collapse_simplices_to_delete(m_operating_tuple, mesh());
        delete_simplices();
        return Tuple();
        ;
    }
    // check if the collapse is valid
    if (m_is_self_loop || (mesh().is_boundary_vertex(m_operating_tuple) &&
                           mesh().is_boundary_vertex(mesh().switch_vertex(m_operating_tuple)))) {
        return Tuple();
    }
    simplex_ids_to_delete = get_collapse_simplices_to_delete(m_operating_tuple, mesh());

    std::cout << std::endl << std::endl;
    spdlog::info(
        "Edge collapse on [{}]: {} with neighbors {}, vertices {}",
        fmt::join(mesh().absolute_multi_mesh_id(), ","),
        operating_edge_id(),
        fmt::join(m_neighbor_eids, ","),
        fmt::join(m_spine_vids, ","));
    //  update ee
    {
        // for neighbor edges
        for (int64_t i = 0; i < 2; i++) {
            if (m_neighbor_eids[i] != -1) {
                auto ee_neighbor = ee_accessor.vector_attribute(m_neighbor_eids[i]);
                spdlog::info(
                    "neighbor {} with gid {} had old ee of {}",
                    i,
                    m_neighbor_eids[i],
                    fmt::join(ee_neighbor, ","));
                for (int64_t j = 0; j < 2; j++) {
                    if (ee_neighbor[j] == operating_edge_id()) {
                        ee_neighbor[j] = m_neighbor_eids[i ^ 1];
                        break;
                    }
                }
                spdlog::warn(
                    "neighbor {} with gid {} has new ee of {}",
                    i,
                    m_neighbor_eids[i],
                    fmt::join(ee_neighbor, ","));
            }
        }
    }

    // update ev
    {
        if (m_neighbor_eids[0] != -1) {
            auto ev_neighbor = ev_accessor.vector_attribute(m_neighbor_eids[0]);
            spdlog::info(
                "neighbor {} with gid {} had old ee of {}",
                0,
                m_neighbor_eids[0],
                fmt::join(ev_neighbor, ","));
            for (int64_t j = 0; j < 2; j++) {
                if (ev_neighbor[j] == m_spine_vids[0]) {
                    ev_neighbor[j] = m_spine_vids[1];
                }
            }
            spdlog::warn(
                "neighbor {} with gid {} had old ee of {}",
                0,
                m_neighbor_eids[0],
                fmt::join(ev_neighbor, ","));
        }
    }


    // update ve
    {
        ve_accessor.scalar_attribute(m_spine_vids[1]) =
            (m_neighbor_eids[1] != -1) ? m_neighbor_eids[1] : m_neighbor_eids[0];

        spdlog::info(
            "Vertex {} set to ve {}",
            m_spine_vids[1],
            (m_neighbor_eids[1] != -1) ? m_neighbor_eids[1] : m_neighbor_eids[0]);
    }

    delete_simplices();

    const int64_t ret_eid = m_neighbor_eids[0] == -1 ? m_neighbor_eids[1] : m_neighbor_eids[0];
    Tuple ret_tuple = mesh().edge_tuple_from_id(ret_eid);

    if (mesh().id_vertex(ret_tuple) != m_spine_vids[1]) {
        ret_tuple = mesh().switch_vertex(ret_tuple);
    }
    return ret_tuple;
}

std::vector<int64_t> EdgeMesh::EdgeMeshOperationExecutor::request_simplex_indices(
    const PrimitiveType type,
    int64_t count)
{
    mesh().guarantee_more_attributes(type, count);
    return mesh().request_simplex_indices(type, count);
}

} // namespace wmtk
