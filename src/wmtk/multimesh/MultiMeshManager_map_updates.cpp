
#include <cassert>
#include <functional>
#include <set>
#include <wmtk/Mesh.hpp>
#include <wmtk/dart/Dart.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/utils/apply_simplex_involution.hpp>
#include <wmtk/operations/EdgeOperationData.hpp>
#include <wmtk/operations/internal/CollapseAlternateFacetData.hpp>
#include <wmtk/operations/internal/SplitAlternateFacetData.hpp>
#include <wmtk/utils/Logger.hpp>
#include "MultiMeshManager.hpp"
#include "utils/local_switch_tuple.hpp"
#include "utils/transport_tuple.hpp"
#include "utils/tuple_map_attribute_io.hpp"

namespace wmtk::multimesh {

namespace {


Tuple find_valid_tuple(
    const Mesh& my_mesh,
    const wmtk::Tuple& tuple,
    wmtk::PrimitiveType primitive_type,
    const wmtk::operations::EdgeOperationData& data)
{
    return std::visit(
        [&](const auto& facet_data) -> wmtk::Tuple {
            return facet_data->get_alternative(my_mesh.top_simplex_type(), tuple, primitive_type);
        },
        data.m_op_data);
}

} // namespace

#if defined(WMTK_ENABLED_MULTIMESH_DART)
std::tuple<dart::Dart, dart::Dart>
MultiMeshManager::mapped_darts(const Mesh& my_mesh, const Mesh& child_mesh, int64_t index) const
{
    assert(&my_mesh.m_multi_mesh_manager == this);
    auto [parent_to_child_accessor, child_to_parent_accessor] =
        get_map_const_accessors(my_mesh, child_mesh);
    const PrimitiveType parent_pt = my_mesh.top_simplex_type();
    const PrimitiveType child_pt = child_mesh.top_simplex_type();
    wmtk::dart::Dart parent_to_child_dart =
        parent_to_child_accessor.IndexBaseType::operator[](index);
    if (parent_to_child_dart.is_null()) {
        return {};
    }
    // the child to parent is always the global id
    wmtk::dart::Dart child_to_parent_dart =
        child_to_parent_accessor.IndexBaseType::operator[](parent_to_child_dart.global_id());

    const dart::SimplexDart& child_sd = dart::SimplexDart::get_singleton(child_pt);
    int8_t child_permutation = child_sd.identity();
    int8_t parent_permutation = parent_to_child_dart.permutation();
    if (parent_pt == child_pt) {
        std::swap(child_permutation, parent_permutation);
    }

    dart::Dart parent_dart = dart::Dart(child_to_parent_dart.global_id(), parent_permutation);
    dart::Dart child_dart = dart::Dart(parent_to_child_dart.global_id(), child_permutation);
    return {parent_dart, child_dart};

#if !defined(NDEBUG)
    const dart::SimplexDart& parent_sd = dart::SimplexDart::get_singleton(parent_pt);
    assert(
        wmtk::dart::utils::apply_simplex_involution(
            parent_pt,
            child_pt,
            parent_to_child_dart,
            parent_dart) == child_dart);

#endif
}
#endif

std::tuple<Tuple, Tuple>
MultiMeshManager::mapped_tuples(const Mesh& my_mesh, const Mesh& child_mesh, int64_t index) const
{
#if defined(WMTK_ENABLED_MULTIMESH_DART)
    auto [parent_dart, child_dart] = mapped_darts(my_mesh, child_mesh, index);

    const PrimitiveType parent_pt = my_mesh.top_simplex_type();
    const PrimitiveType child_pt = child_mesh.top_simplex_type();
    const dart::SimplexDart& parent_sd = dart::SimplexDart::get_singleton(parent_pt);
    const dart::SimplexDart& child_sd = dart::SimplexDart::get_singleton(child_pt);
    Tuple parent_tuple = parent_sd.tuple_from_dart(parent_dart);
    Tuple child_tuple = child_sd.tuple_from_dart(child_dart);

    return {parent_tuple, child_tuple};

#else
    assert(&my_mesh.m_multi_mesh_manager == this);
    auto [parent_to_child_accessor, child_to_parent_accessor] =
        get_map_const_accessors(my_mesh, child_mesh);
    auto parent_to_child_data =
        Mesh::get_index_access(parent_to_child_accessor).const_vector_attribute(index);

    return wmtk::multimesh::utils::vectors_to_tuples(parent_to_child_data);
#endif
}

// TODO: verify why these names are necessary
void MultiMeshManager::update_child_handles(Mesh& my_mesh)
{
    MultiMeshManager& parent_manager = my_mesh.m_multi_mesh_manager;
    auto& children = parent_manager.m_children;
    // update handles for all children
    for (size_t i = 0; i < children.size(); ++i) {
        auto new_handle = my_mesh.get_attribute_handle_typed<int64_t>(
            parent_to_child_map_attribute_name(children[i].mesh->m_multi_mesh_manager.m_child_id),
            children[i].mesh->top_simplex_type());
        children[i] = ChildData{children[i].mesh, new_handle};
    }

    //  update child ids for all children
    for (size_t i = 0; i < children.size(); ++i) {
        children[i].mesh->m_multi_mesh_manager.m_child_id = i;
        my_mesh.m_attribute_manager.set_name<int64_t>(
            children[i].map_handle,
            parent_to_child_map_attribute_name(i));
    }
}

void MultiMeshManager::update_maps_from_edge_operation(
    Mesh& my_mesh,
    const operations::EdgeOperationData& operation_data)
{
    for (int8_t j = 0; j < m_has_child_mesh_in_dimension.size(); ++j) {
        if (m_has_child_mesh_in_dimension[j]) {
            update_maps_from_edge_operation(my_mesh, get_primitive_type_from_id(j), operation_data);
        }
    }
}

void MultiMeshManager::update_maps_from_edge_operation(
    Mesh& my_mesh,
    PrimitiveType primitive_type,
    const operations::EdgeOperationData& operation_data)
{
    // Assumes that the child mesh does not have a simplex affected by the operation (such cases
    // are evaluated by a  multimesh topological guarantee or handled by operation specific
    // code. This just makes sure that the tuple used to map to a child mesh still exists after
    // an operation

    if (children().empty()) {
        return;
    }
    // auto parent_flag_accessor = my_mesh.get_const_flag_accessor(primitive_type);
    //  auto& update_tuple = [&](const auto& flag_accessor, Tuple& t) -> bool {
    //      if(acc.index_access().
    //  };

    const std::vector<int64_t>& gids = operation_data.global_ids_to_update[get_primitive_type_id(
        primitive_type)]; // get facet gids(primitive_type);
    //spdlog::info("Updating map for {}-simplices from edge split on [{}]:{}, ids are {}", primitive_type,my_mesh.absolute_multi_mesh_id(), operation_data.m_input_edge_gid, gids);

    // go over every child mesh and try to update their hashes
    for (auto& child_data : children()) {
        auto& child_mesh = *child_data.mesh;
        // ignore ones whos map are the wrong dimension
        if (child_mesh.top_simplex_type() != primitive_type) {
            continue;
        }
        //spdlog::info(
        //     "[{}->{}] Doing a child mesh ({})",
        //     fmt::join(my_mesh.absolute_multi_mesh_id(), ","),
        //     fmt::join(child_mesh.absolute_multi_mesh_id(), ","),child_mesh.top_simplex_type());
        //  get accessors to the maps
        auto maps = get_map_accessors(my_mesh, child_data);
        auto& [parent_to_child_accessor, child_to_parent_accessor] = maps;

        // auto child_flag_accessor = child_mesh.get_const_flag_accessor(primitive_type);


        for (const auto& gid : gids) {
            const bool parent_exists = !my_mesh.is_removed(gid, primitive_type);
            if (!parent_exists) {
                logger().debug("parent doesnt exist, skip!");
                continue;
            }

            auto [parent_tuple, child_tuple] = mapped_tuples(my_mesh, *child_data.mesh, gid);


            // If the parent tuple is valid, it means this parent-child pair has already been
            // handled, so we can skip it
            // If the parent tuple is invalid then there was no map so we can try the next cell
            if (parent_tuple.is_null()) {
                logger().debug("parent in map null, skip!");
                continue;
            }


            // it is not this function's responsibility to handle cases where
            // teh child mesh's topology has changed on the mapped simplex, so we must make sure
            // it's still there
            const bool child_exists = !child_mesh.is_removed(child_tuple);
            if (!child_exists) {
                logger().debug("child doesnt exist, skip!");
                continue;
            }


            
            parent_tuple = wmtk::multimesh::find_valid_tuple(
                my_mesh,
                parent_tuple,
                primitive_type,
                operation_data);

            if (parent_tuple.is_null()) {
                continue;
            }

            assert(!parent_tuple.is_null());
            wmtk::multimesh::utils::symmetric_write_tuple_map_attributes(
                parent_to_child_accessor,
                child_to_parent_accessor,
                parent_tuple,
                child_tuple);
        }
    }
}

void MultiMeshManager::update_map_tuple_hashes(
    Mesh& my_mesh,
    PrimitiveType primitive_type,
    const std::vector<std::tuple<int64_t, std::vector<Tuple>>>& simplices_to_update,
    const std::vector<std::tuple<int64_t, std::array<int64_t, 2>>>& split_cell_maps)
{
    const PrimitiveType parent_primitive_type = my_mesh.top_simplex_type();
#if defined(WMTK_ENABLED_MULTIMESH_DART)
    const auto& parent_sd = dart::SimplexDart::get_singleton(parent_primitive_type);
#endif
    // spdlog::info(
    //     "Updating tuple hashes {}: {}",
    //     primitive_type_name(primitive_type),
    //     simplices_to_update.size());


    // go over every child mesh and try to update their hashes
    for (auto& child_data : children()) {
        auto& child_mesh = *child_data.mesh;
        // spdlog::info(
        //     "{} {}",
        //     primitive_type_name(child_mesh.top_simplex_type()),
        //     primitive_type_name(primitive_type));
        //  ignore ones whos map are the wrong dimension
        if (child_mesh.top_simplex_type() != primitive_type) {
            continue;
        }
        //  get accessors to the maps
        auto maps = get_map_accessors(my_mesh, child_data);
        auto& [parent_to_child_accessor, child_to_parent_accessor] = maps;

        // auto child_flag_accessor = child_mesh.get_const_flag_accessor(primitive_type);


        for (const auto& [original_parent_gid, equivalent_parent_tuples] : simplices_to_update) {
#if defined(WMTK_ENABLED_MULTIMESH_DART)

            // spdlog::info("{}-{}", primitive_type_name(primitive_type), original_parent_gid);

            const dart::SimplexDart& child_sd = dart::SimplexDart::get_singleton(primitive_type);
            //  read off the original map's data
            // auto [parent_tuple, child_tuple] =
            //    mapped_tuples(my_mesh, *child_data.mesh, original_parent_gid);
            dart::Dart child_map_dart = parent_to_child_accessor[original_parent_gid];
            if (child_map_dart.is_null()) {
                continue;
            }
            dart::Dart parent_map_dart = child_to_parent_accessor[child_map_dart.global_id()];
            spdlog::info(
                "gid {} found map {} {}",
                original_parent_gid,
                std::string(parent_map_dart),
                std::string(child_map_dart));

            // If the parent tuple is valid, it means this parent-child pair has already been
            // handled, so we can skip it
            // If the parent tuple is invalid then there was no map so we can try the next cell
            if (parent_map_dart.is_null()) {
                logger().debug("parent dart was null");
                continue;
            }


            // TODO: this covers the case of updating the spine simplices, but we should have caught
            // that alraedy check if the map is handled in the ear case if the child simplex is
            // deleted then we can skip it
            if (child_mesh.is_removed(child_map_dart.global_id())) {
                logger().debug("child simplex was removed, moving on");
                continue;
            }


            Tuple old_tuple;
            // find a tuple using hte original gid
            std::optional<Tuple> old_tuple_opt = find_tuple_from_gid(
                my_mesh,
                my_mesh.top_simplex_type(),
                equivalent_parent_tuples,
                parent_map_dart.global_id());
            // assert(old_tuple_opt.has_value());
            if (!old_tuple_opt.has_value()) {
                continue;
            }
            old_tuple = old_tuple_opt.value();

            simplex::Simplex old_simplex(primitive_type, old_tuple_opt.value());
            std::optional<Tuple> new_parent_shared_opt;
            if (split_cell_maps.empty()) {
                new_parent_shared_opt = find_valid_tuple_from_alternatives(
                    my_mesh,
                    my_mesh.top_simplex_type(),
                    equivalent_parent_tuples);
                int8_t old_facet_permutation =
                    parent_sd.dart_from_tuple(old_simplex.tuple()).permutation();
                // old_simplex,
                Tuple new_parent_tuple_shared = new_parent_shared_opt.value();
                assert(my_mesh.is_valid(new_parent_tuple_shared));
                // assert(child_mesh.is_valid(child_tuple));

                int8_t new_facet_permutation =
                    parent_sd.dart_from_tuple(new_parent_tuple_shared).permutation();

                int8_t new_map_permutation = parent_sd.product(
                    parent_sd.product(
                        old_facet_permutation,
                        parent_sd.inverse(old_facet_permutation)),
                    parent_map_dart.permutation());

                dart::Dart new_parent_dart(
                    new_parent_tuple_shared.global_cid(),
                    new_map_permutation);
                dart::Dart new_child_dart(child_map_dart.global_id(), child_sd.identity());

                Tuple parent_tuple = parent_sd.tuple_from_dart(new_parent_dart);

                assert(my_mesh.is_valid(parent_tuple));
                Tuple child_tuple = child_sd.tuple_from_dart(new_child_dart);
                assert(child_mesh.is_valid(child_tuple));


                child_to_parent_accessor[child_map_dart.global_id()] = parent_map_dart;
                // parent_to_child_accessor[original_parent_gid] = new_child_dart;

                spdlog::info(
                    "Updating {} to {} at gids {} {}",
                    std::string(parent_map_dart),
                    std::string(child_map_dart),
                    original_parent_gid,
                    child_map_dart.global_id());

                wmtk::multimesh::utils::symmetric_write_tuple_map_attributes(
                    parent_to_child_accessor,
                    child_to_parent_accessor,
                    parent_tuple,
                    child_tuple);


            } else {
                new_parent_shared_opt = find_valid_tuple_from_split(
                    my_mesh,
                    old_simplex,
                    original_parent_gid,
                    equivalent_parent_tuples,
                    split_cell_maps);
                if (!new_parent_shared_opt.has_value()) {
                    continue;
                }
                int64_t new_parent_gid = new_parent_shared_opt.value().global_cid();

                parent_map_dart = dart::Dart(new_parent_gid, parent_map_dart.permutation());
                spdlog::info(
                    "Updating {} to {} at gids {} {}",
                    std::string(parent_map_dart),
                    std::string(child_map_dart),
                    original_parent_gid,
                    child_map_dart.global_id());
                parent_to_child_accessor[original_parent_gid] = child_map_dart;
                child_to_parent_accessor[child_map_dart.global_id()] = parent_map_dart;
            }


#else
            //  read off the original map's data
            auto [parent_tuple, child_tuple] =
                mapped_tuples(my_mesh, *child_data.mesh, original_parent_gid);

            // If the parent tuple is valid, it means this parent-child pair has already been
            // handled, so we can skip it
            // If the parent tuple is invalid then there was no map so we can try the next cell
            if (parent_tuple.is_null()) {
                continue;
            }


            // TODO: this covers the case of updating the spine simplices, but we should have caught
            // that alraedy check if the map is handled in the ear case if the child simplex is
            // deleted then we can skip it
            if (child_mesh.is_removed(child_tuple)) {
                continue;
            }
            // assert(!child_mesh.is_removed(child_tuple));

            // Find a valid representation of this simplex representation of the original tupl
            Tuple old_tuple;
            std::optional<Tuple> old_tuple_opt = find_tuple_from_gid(
                my_mesh,
                my_mesh.top_simplex_type(),
                equivalent_parent_tuples,
                parent_tuple.global_cid());
            // assert(old_tuple_opt.has_value());
            if (!old_tuple_opt.has_value()) {
                continue;
            }
            simplex::Simplex old_simplex(primitive_type, old_tuple_opt.value());

            std::optional<Tuple> new_parent_shared_opt = find_valid_tuple(
                my_mesh,
                old_simplex,
                original_parent_gid,
                equivalent_parent_tuples,
                split_cell_maps);


            if (!new_parent_shared_opt.has_value()) {
                std::cout << "get skipped, someting is wrong?" << std::endl;
                continue;
            }
            // assert(new_parent_shared_opt.has_value());


            Tuple new_parent_tuple_shared = new_parent_shared_opt.value();
            parent_tuple = wmtk::multimesh::utils::transport_tuple(
                old_simplex.tuple(),
                parent_tuple,
                my_mesh.top_simplex_type(),
                new_parent_tuple_shared,
                my_mesh.top_simplex_type());
            assert(my_mesh.is_valid(parent_tuple));
            assert(child_mesh.is_valid(child_tuple));


            wmtk::multimesh::utils::symmetric_write_tuple_map_attributes(
                parent_to_child_accessor,
                child_to_parent_accessor,
                parent_tuple,
                child_tuple);
#endif
        }
    }
}
std::optional<Tuple> MultiMeshManager::find_valid_tuple(
    Mesh& my_mesh,
    const simplex::Simplex& old_simplex,
    const int64_t old_gid,
    const std::vector<Tuple>& equivalent_parent_tuples,
    const std::vector<std::tuple<int64_t, std::array<int64_t, 2>>>& split_cell_maps) const
{
    // if old gid was one of the originals then do tuple
    // otherwise just find some random tuple that still exists

    // when we're splitting we don't need to look at alternatives / will only update if the map used the right simplex
    if (split_cell_maps.empty()) {
        return find_valid_tuple_from_alternatives(
            my_mesh,
            my_mesh.top_simplex_type(),
            equivalent_parent_tuples);
    } else {
        return find_valid_tuple_from_split(
            my_mesh,
            old_simplex,
            old_gid,
            equivalent_parent_tuples,
            split_cell_maps);
    }
}


std::optional<Tuple> MultiMeshManager::find_valid_tuple_from_alternatives(
    Mesh& my_mesh,
    PrimitiveType primitive_type,
    const std::vector<Tuple>& tuple_alternatives) const
{
    // auto parent_flag_accessor = my_mesh.get_const_flag_accessor(primitive_type);
    //  find a new sharer by finding a tuple that exists
    auto it = std::find_if(
        tuple_alternatives.begin(),
        tuple_alternatives.end(),
        [&](const Tuple& t) -> bool { return !my_mesh.is_removed(t); });
    if (it != tuple_alternatives.end()) {
        return *it;
    } else {
        return std::optional<Tuple>{};
    }
}

std::optional<Tuple> MultiMeshManager::find_valid_tuple_from_split(
    Mesh& my_mesh,
    const simplex::Simplex& old_simplex,
    const int64_t old_simplex_gid,
    const std::vector<Tuple>& tuple_alternatives,
    const std::vector<std::tuple<int64_t, std::array<int64_t, 2>>>& split_cell_maps) const
{
    const Tuple& old_tuple = old_simplex.tuple();
    const PrimitiveType primitive_type = old_simplex.primitive_type();

    for (const auto& [old_cid, new_cids] : split_cell_maps) {
        if (old_cid != old_tuple.global_cid()) {
            continue;
        }

        auto old_tuple_opt =
            find_tuple_from_gid(my_mesh, my_mesh.top_simplex_type(), tuple_alternatives, old_cid);
        // assert(old_tuple_opt.has_value());
        if (!(old_tuple_opt.has_value())) {
            return std::optional<Tuple>{};
        }

        const Tuple& old_cid_tuple = old_tuple_opt.value();
        for (const int64_t new_cid : new_cids) {
            Tuple tuple(
                old_cid_tuple.local_vid(),
                old_cid_tuple.local_eid(),
                old_cid_tuple.local_fid(),
                new_cid);


            if (my_mesh.is_valid(tuple) && !my_mesh.is_removed(tuple) &&
                old_simplex_gid == my_mesh.id(tuple, primitive_type)) {
                return tuple;
            }
        }
    }
    return std::optional<Tuple>{};
}

std::optional<Tuple> MultiMeshManager::find_tuple_from_gid(
    const Mesh& my_mesh,
    PrimitiveType primitive_type,
    const std::vector<Tuple>& tuples,
    int64_t gid)
{
    logger().trace("Finding gid {} {}", gid, primitive_type_name(primitive_type));

    auto it = std::find_if(tuples.begin(), tuples.end(), [&](const Tuple& t) -> bool {
        return (gid == my_mesh.id(t, primitive_type));
    });
    if (it == tuples.end()) {
        return std::optional<Tuple>{};
    } else {
        return *it;
    }
}
int64_t MultiMeshManager::child_global_cid(const AccessorType& parent_to_child, int64_t parent_gid)
{
#if defined(WMTK_ENABLED_MULTIMESH_DART)
    return parent_to_child.IndexBaseType::operator[](parent_gid)[0].global_id();
#else
    // look at src/wmtk/multimesh/utils/tuple_map_attribute_io.cpp to see what index global_cid gets mapped to)
    // 2 is the size of a tuple is 2 longs, global_cid currently gets written to position 3
    // 5 is the size of a tuple is 5 longs, global_cid currently gets written to position 3
    return Mesh::get_index_access(parent_to_child)
        .const_vector_attribute(parent_gid)(
            wmtk::multimesh::utils::TUPLE_SIZE + wmtk::multimesh::utils::GLOBAL_ID_INDEX);
#endif
}
int64_t MultiMeshManager::parent_global_cid(const AccessorType& child_to_parent, int64_t child_gid)
{
#if defined(WMTK_ENABLED_MULTIMESH_DART)
    return child_to_parent.IndexBaseType::operator[](child_gid)[0].global_id();
#else
    // look at src/wmtk/multimesh/utils/tuple_map_attribute_io.cpp to see what index global_cid gets mapped to)
    // 2 is the size of a tuple is 2 longs, global_cid currently gets written to position 3
    // 5 is the size of a tuple is 5 longs, global_cid currently gets written to position 2
    return Mesh::get_index_access(child_to_parent)
        .const_vector_attribute(child_gid)(
            wmtk::multimesh::utils::TUPLE_SIZE + wmtk::multimesh::utils::GLOBAL_ID_INDEX);
#endif
}

int64_t MultiMeshManager::parent_local_fid(const AccessorType& child_to_parent, int64_t child_gid)
{
#if defined(WMTK_ENABLED_MULTIMESH_DART)
    // only works on tets
    auto o = child_to_parent.IndexBaseType::operator[](child_gid)[0].permutation();
    return wmtk::dart::SimplexDart::get_singleton(PrimitiveType::Tetrahedron)
        .simplex_index(o, PrimitiveType::Triangle);
#else
    // look at src/wmtk/multimesh/utils/tuple_map_attribute_io.cpp to see what index global_cid gets mapped to)
#if defined(WMTK_DISABLE_COMPRESSED_MULTIMESH_TUPLE)
    // 5 is the size of a tuple is 5 longs, global_cid currently gets written to position 3
    return Mesh::get_index_access(child_to_parent)
        .const_vector_attribute(child_gid)(wmtk::multimesh::utils::TUPLE_SIZE + 2);
#else
    // pick hte index that isn't teh global id index
    const int64_t v =
        Mesh::get_index_access(child_to_parent)
            .const_vector_attribute(child_gid)(
                wmtk::multimesh::utils::TUPLE_SIZE + (1 - wmtk::multimesh::utils::GLOBAL_ID_INDEX));
    auto vptr = reinterpret_cast<const int8_t*>(&v);
    return vptr[2];
#endif
#endif
}


} // namespace wmtk::multimesh
