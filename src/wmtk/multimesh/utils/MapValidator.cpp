
#include "MapValidator.hpp"
#include <wmtk/Mesh.hpp>
#include <wmtk/multimesh/utils/internal/print_all_mapped_tuples.hpp>
#include <wmtk/multimesh/utils/tuple_map_attribute_io.hpp>
#include <wmtk/simplex/top_dimension_cofaces.hpp>
#include <wmtk/simplex/utils/SimplexComparisons.hpp>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/primitive_range.hpp>
#include "internal/print_all_mapped_tuples.hpp"
namespace wmtk::multimesh::utils {

MapValidator::MapValidator(const Mesh& m)
    : m_mesh(m)
{}
bool MapValidator::check_all() const
{
    logger().info("Checking all map validitiy checks");
    bool ok = true;
    {
        bool r = check_parent_map_attribute_valid();
        if (!r) {
            logger().warn(
                "Testing [{}] parent attributes failed",
                fmt::join(m_mesh.absolute_multi_mesh_id(), ","));
        }
        ok &= r;
    }
    {
        bool r = check_child_map_attributes_valid();
        if (!r) {
            logger().warn(
                "Testing [{}] child attributes failed",
                fmt::join(m_mesh.absolute_multi_mesh_id(), ","));
        }
        ok &= r;
    }
    if (!ok) {
        return ok;
    }
    ok &= check_switch_homomorphism();
    ok &= check_facet_uniqueness();

    return ok;
}

bool MapValidator::check_child_map_attributes_valid() const
{
    logger().debug("Checking child map attributes are valid");
    bool ok = true;
    for (const auto& [cptr, attr] : m_mesh.m_multi_mesh_manager.m_children) {
        const auto& child = *cptr;

        logger().warn(
            "Testing [{}] to [{}]",
            fmt::join(m_mesh.absolute_multi_mesh_id(), ","),
            fmt::join(child.absolute_multi_mesh_id(), ","));
        // auto [me_to_child, child_to_me] =
        // m_mesh.m_multi_mesh_manager.get_map_const_accessors(m_mesh, child);
        // wmtk::multimesh::utils::internal::print_all_mapped_tuples(me_to_child, child_to_me);

        for (int64_t j = 0; j < child.top_cell_dimension(); ++j) {
            wmtk::PrimitiveType prim_type = wmtk::PrimitiveType(j);

            for (const auto& pt : m_mesh.get_all(prim_type)) {
                wmtk::simplex::Simplex s(prim_type, pt);

                auto tups = simplex::top_dimension_cofaces_tuples(m_mesh, s);

                for (const auto& source_tuple : tups) {
                }
            }
        }
    }
    return ok;
}

bool MapValidator::check_parent_map_attribute_valid() const
{
    logger().debug(
        "Checking parent map attribute validity for {}",
        fmt::join(m_mesh.absolute_multi_mesh_id(), ","));
    bool ok = true;
    const auto& parent_ptr = m_mesh.m_multi_mesh_manager.m_parent;
    if (parent_ptr == nullptr) {
        return true;
    }
    const auto& attr = m_mesh.m_multi_mesh_manager.map_to_parent_handle;
    const auto& parent = *parent_ptr;

    wmtk::PrimitiveType prim_type = m_mesh.top_simplex_type();

    auto [parent_to_me, me_to_parent] =
        parent.m_multi_mesh_manager.get_map_const_accessors(parent, m_mesh);

    logger().warn(
        "Testing [{}] to [{}]",
        fmt::join(parent.absolute_multi_mesh_id(), ","),
        fmt::join(m_mesh.absolute_multi_mesh_id(), ","));
    // wmtk::multimesh::utils::internal::print_all_mapped_tuples(parent_to_me, me_to_parent);
    const auto& sd = dart::SimplexDart::get_singleton(m_mesh.top_simplex_type());
    for (const auto& source_tuple : m_mesh.get_all(prim_type)) {
        assert(m_mesh.is_valid(source_tuple)); // paranoia check
        dart::Dart d = me_to_parent[sd.dart_from_tuple(source_tuple)];
        if (d.is_null()) {
            ok = false;
            wmtk::logger().error(
                "Map from child [{}] to parent [{}] on tuple {} (dim {}, id {}, simplex_index {}) "
                "has null entry {}",
                fmt::join(m_mesh.absolute_multi_mesh_id(), ","),
                fmt::join(parent.absolute_multi_mesh_id(), ","),
                std::string(source_tuple),
                m_mesh.top_cell_dimension(),
                m_mesh.id(source_tuple, prim_type),
                sd.simplex_index(d, prim_type),
                d);
        }
        if (parent.is_removed(d, prim_type)) {
            ok = false;
            wmtk::logger().error(
                "Map from child [{}] to parent [{}] on tuple {} (dim {}, id {}, simplex_index {}) "
                "has removed facet "
                "{}",
                fmt::join(m_mesh.absolute_multi_mesh_id(), ","),
                fmt::join(parent.absolute_multi_mesh_id(), ","),
                std::string(source_tuple),
                m_mesh.top_cell_dimension(),
                m_mesh.id(source_tuple, prim_type),
                sd.simplex_index(d, prim_type),
                d);
        }
        dart::ConstDartWrap dw = parent_to_me[d];

        if (dw.is_null()) {
            ok = false;
            wmtk::logger().error(
                "Map from child [{}] to parent [{}] on tuple {} (dim {}, id {}, simplex_index {}) "
                "has null entry {}",
                fmt::join(m_mesh.absolute_multi_mesh_id(), ","),
                fmt::join(parent.absolute_multi_mesh_id(), ","),
                std::string(source_tuple),
                m_mesh.top_cell_dimension(),
                m_mesh.id(source_tuple, prim_type),
                sd.simplex_index(d, prim_type),
                d);
        }
        if (m_mesh.is_removed(dw.global_id(), prim_type)) {
            ok = false;
            wmtk::logger().error(
                "Map from child [{}] to parent [{}] on tuple {} (dim {}, id {}, simplex_index {}) "
                "has removed {}-simplex "
                "{} (id = {})",
                fmt::join(m_mesh.absolute_multi_mesh_id(), ","),
                fmt::join(parent.absolute_multi_mesh_id(), ","),
                std::string(source_tuple),
                m_mesh.top_cell_dimension(),
                m_mesh.id(source_tuple, prim_type),
                sd.simplex_index(d, prim_type),
                prim_type,
                dw, m_mesh.id(dw,prim_type));
        }
    }
    return ok;
}


bool MapValidator::check_switch_homomorphism() const
{
    bool ok = true;
    for (const auto& cptr : m_mesh.get_child_meshes()) {
        ok &= check_child_switch_homomorphism(*cptr);
    }
    return ok;
}
bool MapValidator::check_facet_uniqueness() const
{
    bool ok = true;
    for (const auto& cptr : m_mesh.get_child_meshes()) {
        ok &= check_child_facet_uniqueness(*cptr);
    }
    return ok;
}
bool MapValidator::check_child_facet_uniqueness(const Mesh& child) const
{
    bool ok = true;
    assert(child.m_multi_mesh_manager.m_parent == &m_mesh);
    const auto pt = child.top_simplex_type();

    auto child_facets = child.get_all(pt);


    std::vector<std::vector<simplex::Simplex>> children_mapped(child_facets.size());


    for (const auto& parent_t : m_mesh.get_all(pt)) {
        wmtk::simplex::Simplex s(pt, parent_t);
        auto child_ss = m_mesh.map_to_child(child, s);
        for (const auto& child_s : child_ss) {
            auto child_id = child.id(child_s);
            if (child_id >= children_mapped.size()) {
                children_mapped.resize(child_id + 1);
            }
            children_mapped.at(child_id).emplace_back(child_s);
        }
    }
    for (size_t j = 0; j < children_mapped.size(); ++j) {
        const auto& children = children_mapped.at(j);
        if (children.size() > 1) {
            std::vector<std::string> tups;
            ok = false;
            std::transform(
                children.begin(),
                children.end(),
                std::back_inserter(tups),
                [](const auto& x) { return std::string(x.tuple()); });
            wmtk::logger().error(
                "Single child facet maps to multiple parent facets: [{}]:{} maps to [{}]:{}",
                fmt::join(m_mesh.absolute_multi_mesh_id(), ","),
                j,
                fmt::join(child.absolute_multi_mesh_id(), ","),
                fmt::join(tups, ","));
        }
    }
    return ok;
}

bool MapValidator::check_child_switch_homomorphism(const Mesh& child) const
{
    assert(child.m_multi_mesh_manager.m_parent == &m_mesh);
    bool ok = true;
    for (PrimitiveType pt : wmtk::utils::primitive_below(child.top_simplex_type())) {
        auto tups = child.get_all(pt);
        for (const wmtk::Tuple& t : tups) {
            assert(!t.is_null());
            wmtk::simplex::Simplex s(pt, t);

            wmtk::Tuple parent_tuple = child.map_to_parent_tuple(s);
            assert(!parent_tuple.is_null());
            assert(m_mesh.is_valid(parent_tuple));

            // spt is the primitive type we will switch
            for (PrimitiveType spt : wmtk::utils::primitive_below(child.top_simplex_type())) {
                if (pt == PrimitiveType::Vertex) {
                    continue;
                }
                // skip switches over boundaries
                if (spt == child.top_simplex_type()) {
                    continue;
                }
                if (spt == child.top_simplex_type() &&
                    child.is_boundary(child.top_simplex_type() - 1, t)) {
                    continue;
                }

                wmtk::simplex::Simplex switched_simplex(pt, child.switch_tuple(t, spt));
                wmtk::Tuple switch_map = child.map_to_parent_tuple(switched_simplex);
                wmtk::Tuple map_switch = m_mesh.switch_tuple(parent_tuple, spt);
                for (PrimitiveType my_pt : wmtk::utils::primitive_below(child.top_simplex_type())) {
                    bool worked = wmtk::simplex::utils::SimplexComparisons::equal(
                        m_mesh,
                        my_pt,
                        switch_map,
                        map_switch);
                    if (!worked) {
                        wmtk::logger().error(
                            "Map from child [{0}] to parent [{1}] on tuple {2} (dim {3}) fails on  "
                            "switch(map({2}),{4}) = {5} !=  "
                            "map(switch({2},{4})) = {6} (dim {7} id {8} != {9})",
                            fmt::join(child.absolute_multi_mesh_id(), ","), // 0
                            fmt::join(m_mesh.absolute_multi_mesh_id(), ","), // 1
                            t.as_string(), // 2
                            child.top_cell_dimension(), // 3
                            primitive_type_name(pt), // 4
                            map_switch.as_string(), // 5
                            switch_map.as_string(), // 6
                            primitive_type_name(my_pt), // 7
                            m_mesh.id(map_switch, my_pt), // 8
                            m_mesh.id(switch_map, my_pt) // 9
                        );
                        ok = false;
                    }
                }
            }
        }
    }
    return ok;
}


void MapValidator::print()
{
    for (const auto& [cptr, attr] : m_mesh.m_multi_mesh_manager.m_children) {
        const auto& child = *cptr;
        auto [me_to_child, child_to_me] =
            m_mesh.m_multi_mesh_manager.get_map_const_accessors(m_mesh, child);

        internal::print_all_mapped_tuples(me_to_child, child_to_me);
    }
}

} // namespace wmtk::multimesh::utils
