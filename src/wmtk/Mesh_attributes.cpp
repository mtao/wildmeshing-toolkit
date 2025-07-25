#include <numeric>
#include "Mesh.hpp"

#include <wmtk/multimesh/utils/tuple_map_attribute_io.hpp>
#include <wmtk/utils/Logger.hpp>

#include "Primitive.hpp"

namespace wmtk {
template <typename T>
attribute::MeshAttributeHandle Mesh::register_attribute(
    const std::string& name,
    PrimitiveType ptype,
    int64_t size,
    bool replace,
    T default_value)
{
    attribute::MeshAttributeHandle attr(
        *this,
        register_attribute_typed<T>(name, ptype, size, replace, default_value));
    return attr;
}

template <typename T>
attribute::TypedAttributeHandle<T> Mesh::register_attribute_typed(
    const std::string& name,
    PrimitiveType ptype,
    int64_t size,
    bool replace,
    T default_value)
{
    auto attr =
        m_attribute_manager.register_attribute<T>(name, ptype, size, replace, default_value);
    return attr;
}

template <typename T>
const T& Mesh::get_attribute_default_value(const TypedAttributeHandle<T>& handle) const
{
    return m_attribute_manager.get_attribute_default_value(handle);
}

std::vector<int64_t> Mesh::request_simplex_indices(PrimitiveType type, int64_t count)
{
    // passses back a set of new consecutive ids. in hte future this could do
    // something smarter for re-use but that's probably too much work
    int64_t current_capacity = capacity(type);

    // enable newly requested simplices
    attribute::FlagAccessor<Mesh> flag_accessor = get_flag_accessor(type);
    int64_t max_size = flag_accessor.base_accessor().reserved_size();

    if (current_capacity + count > max_size) {
        logger().warn(
            "Requested more {} simplices than available (have {}, wanted {}, can only have at most "
            "{}",
            primitive_type_name(type),
            current_capacity,
            count,
            max_size);
        return {};
    }

    std::vector<int64_t> ret(count);
    std::iota(ret.begin(), ret.end(), current_capacity);


    int64_t new_capacity = current_capacity + ret.size();
    assert(ret.back() + 1 == current_capacity + ret.size());
    size_t primitive_id = get_primitive_type_id(type);

    m_attribute_manager.m_capacities[primitive_id] = new_capacity;

    attribute::IndexFlagAccessor<Mesh>& flag_accessor_indices = flag_accessor.index_access();

    for (const int64_t simplex_index : ret) {
        // wmtk::logger().trace("Activating {}-simplex {}", primitive_id, simplex_index);
        flag_accessor_indices.activate(simplex_index);
    }

    return ret;
}

int64_t Mesh::capacity(PrimitiveType type) const
{
    return m_attribute_manager.m_capacities.at(get_primitive_type_id(type));
}

void Mesh::reserve_attributes_to_fit()
{
    m_attribute_manager.reserve_to_fit();
}
void Mesh::reserve_attributes(PrimitiveType type, int64_t size)
{
    m_attribute_manager.reserve_attributes(get_primitive_type_id(type), size);
}
void Mesh::set_capacities(std::vector<int64_t> capacities)
{
    m_attribute_manager.set_capacities(std::move(capacities));
}

// reserves extra attributes than necessary right now
void Mesh::reserve_more_attributes(PrimitiveType type, int64_t size)
{
    m_attribute_manager.reserve_more_attributes(get_primitive_type_id(type), size);
}
// reserves extra attributes than necessary right now
void Mesh::reserve_more_attributes(const std::vector<int64_t>& sizes)
{
    assert(top_cell_dimension() + 1 == sizes.size());
    for (int64_t j = 0; j < sizes.size(); ++j) {
        m_attribute_manager.reserve_more_attributes(j, sizes[j]);
    }
}
void Mesh::guarantee_at_least_attributes(PrimitiveType type, int64_t size)
{
    m_attribute_manager.guarantee_at_least_attributes(get_primitive_type_id(type), size);
}
void Mesh::guarantee_at_least_attributes(const std::vector<int64_t>& sizes)
{
    assert(top_cell_dimension() + 1 == sizes.size());
    for (int64_t j = 0; j < sizes.size(); ++j) {
        m_attribute_manager.guarantee_at_least_attributes(j, sizes[j]);
    }
}
void Mesh::guarantee_more_attributes(PrimitiveType type, int64_t size)
{
    m_attribute_manager.guarantee_more_attributes(get_primitive_type_id(type), size);
}
void Mesh::guarantee_more_attributes(const std::vector<int64_t>& sizes)
{
    assert(top_cell_dimension() + 1 == sizes.size());
    for (int64_t j = 0; j < sizes.size(); ++j) {
        m_attribute_manager.guarantee_more_attributes(j, sizes[j]);
    }
}

namespace {
std::vector<attribute::MeshAttributeHandle::HandleVariant> variant_diff(
    std::vector<attribute::MeshAttributeHandle::HandleVariant>& a,
    std::vector<attribute::MeshAttributeHandle::HandleVariant>& b)
{
    std::vector<attribute::MeshAttributeHandle::HandleVariant> ret;
    std::sort(a.begin(), a.end());

    std::sort(b.begin(), b.end());

    std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(ret, ret.begin()));
    return ret;
}
} // namespace
std::vector<attribute::MeshAttributeHandle::HandleVariant> Mesh::custom_attributes() const
{
    auto all = m_attribute_manager.get_all_attributes();
    auto builtins = builtin_attributes();

    return variant_diff(all, builtins);
}

std::string Mesh::get_attribute_name(
    const attribute::MeshAttributeHandle::HandleVariant& handle) const
{
    return m_attribute_manager.get_name(handle);
}
void Mesh::clear_attributes()
{
    m_attribute_manager.clear_attributes(custom_attributes());
}

void Mesh::clear_attributes(
    const std::vector<attribute::MeshAttributeHandle::HandleVariant>& keep_attributes)
{
    auto a = this->custom_attributes();
    auto b = keep_attributes;
    m_attribute_manager.clear_attributes(variant_diff(a, b));
    update_child_handles();
}
void Mesh::clear_attributes(const std::vector<attribute::MeshAttributeHandle>& keep_attributes)
{
    std::map<Mesh*, std::vector<attribute::MeshAttributeHandle::HandleVariant>> keeps_t;
    for (const auto& attr : keep_attributes) {
        keeps_t[const_cast<Mesh*>(&attr.mesh())].emplace_back(attr.handle());
    }
    for (auto& [mptr, handles] : keeps_t) {
        mptr->clear_attributes(handles);
    }
}
void Mesh::delete_attribute(const attribute::MeshAttributeHandle& to_delete)
{
    assert(this == &to_delete.mesh());

    delete_attribute(to_delete.handle());
}

void Mesh::delete_attribute(const attribute::MeshAttributeHandle::HandleVariant& to_delete)
{
    m_attribute_manager.delete_attribute(to_delete);
}
multimesh::attribute::AttributeScopeHandle Mesh::create_scope()
{
    return multimesh::attribute::AttributeScopeHandle(*this);
}

attribute::AttributeScopeHandle Mesh::create_single_mesh_scope()
{
    return m_attribute_manager.create_scope(*this);
}

std::tuple<std::vector<std::vector<int64_t>>, std::vector<std::vector<int64_t>>>
Mesh::consolidate_update_data() const
{
    // Number of dimensions
    int64_t tcp = top_cell_dimension() + 1;

    // Store the map from new indices to old. First index is dimensions, second simplex id
    std::vector<std::vector<int64_t>> new2old(tcp);
    // Store the map from old indices to new. First index is dimensions, second simplex id
    std::vector<std::vector<int64_t>> old2new(tcp);

    // Initialize both maps
    for (int64_t d = 0; d < tcp; d++) {
        size_t capacity = this->capacity(wmtk::get_primitive_type_from_id(d));
        old2new[d].reserve(capacity);
        new2old[d].reserve(capacity);
        attribute::FlagAccessor<Mesh> flag_accessor =
            get_flag_accessor(wmtk::get_primitive_type_from_id(d));
        for (int64_t i = 0; i < capacity; ++i) {
            if (flag_accessor.index_access().is_active(i)) {
                old2new[d].push_back(new2old[d].size());
                new2old[d].push_back(old2new[d].size() - 1); // -1 since we just pushed into it
            } else {
                old2new[d].push_back(-1);
            }
        }
    }
    /*
    for (int64_t d = 0; d < tcp; d++) {
        size_t capacity = this->capacity(wmtk::get_primitive_type_from_id(d));
        old2new[d].resize(capacity, -1);
        new2old[d].reserve(capacity);
        attribute::FlagAccessor<Mesh> flag_accessor =
            get_flag_accessor(wmtk::get_primitive_type_from_id(d));
        for (int64_t i = 0; i < capacity; ++i) {
            if (flag_accessor.index_access().is_active(i)) {
                size_t n2o = new2old[d].size();
                size_t o2n = old2new[d].size();
                old2new[d][i] = (n2o);
                new2old[d].emplace_back(o2n);
            }
        }
    }
    */
    return {new2old, old2new};
}
std::tuple<std::vector<std::vector<int64_t>>, std::vector<std::vector<int64_t>>> Mesh::consolidate()
{
    auto pr = consolidate_update_data();
    {
        for (const auto& child_data : m_multi_mesh_manager.m_children) {
            auto& child_mesh = *child_data.mesh;
            auto [me_to_child, child_to_me] =
                m_multi_mesh_manager.get_map_accessors(*this, child_mesh);

            const auto& child_sd = dart::SimplexDart::get_singleton(child_mesh.top_simplex_type());
            for (const auto& t : child_mesh.get_all(child_to_me.handle().primitive_type())) {
                dart::DartWrap d = child_to_me[child_sd.dart_from_tuple(t)];
                int64_t old_gid = d.global_id();
                if (is_removed(old_gid)) {
                    spdlog::warn("map wasnt valid for {}", old_gid);
                }
            }
        }
        if (auto parent_ptr = m_multi_mesh_manager.m_parent; parent_ptr != nullptr) {
            auto [parent_to_me, me_to_parent] =
                parent_ptr->m_multi_mesh_manager.get_map_accessors(*parent_ptr, *this);


            const auto& p_sd = dart::SimplexDart::get_singleton(parent_ptr->top_simplex_type());
            for (const auto& t : parent_ptr->get_all(parent_to_me.handle().primitive_type())) {
                dart::DartWrap d = parent_to_me[p_sd.dart_from_tuple(t)];
                if (!d.is_null()) {
                    int64_t old_gid = d.global_id();
                    if (is_removed(old_gid)) {
                        spdlog::warn("map wasnt valid for {}", old_gid);
                    }
                }
            }
        }
    }

    consolidate_maps(pr);
    consolidate_attributes(pr);
    consolidate_connectivity(pr);
    return pr;
}
void Mesh::consolidate_attributes(
    const std::tuple<std::vector<std::vector<int64_t>>, std::vector<std::vector<int64_t>>>&
        new2old_old2new)
{
    // Number of dimensions
    int64_t tcp = top_cell_dimension() + 1;
    const auto& [new2old, old2new] = new2old_old2new;
    size_t dim = get_primitive_type_id(top_simplex_type());
    const auto& top_map = old2new[dim];


    // Use new2oldmap to compact all attributes
    auto run = [&](auto&& mesh_attrs) {
        for (int64_t d = 0; d < mesh_attrs.size(); ++d) {
            auto& ma = mesh_attrs[d];
            const auto& n2o = new2old[d];
            ma.reserve(n2o.size());

            for (auto& h : ma.active_attributes()) {
                ma.attribute(h).consolidate(n2o);
            }
        }
    };
    run(m_attribute_manager.m_char_attributes);
    run(m_attribute_manager.m_long_attributes);

    run(m_attribute_manager.m_double_attributes);

    run(m_attribute_manager.m_rational_attributes);
}
void Mesh::consolidate_connectivity(
    const std::tuple<std::vector<std::vector<int64_t>>, std::vector<std::vector<int64_t>>>&
        new2old_old2new)
{
    // Number of dimensions
    int64_t tcp = top_cell_dimension() + 1;
    const auto& [new2old, old2new] = new2old_old2new;
    size_t dim = get_primitive_type_id(top_simplex_type());

    // Update the attribute size in the manager
    for (int64_t d = 0; d < tcp; d++) {
        m_attribute_manager.m_capacities[d] = new2old[d].size();
    }

    // Apply old2new to attributes containing indices
    std::vector<std::vector<TypedAttributeHandle<int64_t>>> handle_indices =
        connectivity_attributes();

    for (int64_t d = 0; d < tcp; d++) {
        for (int64_t i = 0; i < handle_indices[d].size(); ++i) {
            attribute::Accessor<int64_t> accessor = create_accessor<int64_t>(handle_indices[d][i]);
            accessor.attribute().index_remap(old2new[d]);
        }
    }
}

void Mesh::consolidate_maps(
    const std::tuple<std::vector<std::vector<int64_t>>, std::vector<std::vector<int64_t>>>&
        new2old_old2new)
{
    // Number of dimensions
    int64_t tcp = top_cell_dimension() + 1;
    const auto& [new2old, old2new] = new2old_old2new;
    size_t dim = get_primitive_type_id(top_simplex_type());
    const auto& top_map = old2new[dim];

    for (const auto& child_data : m_multi_mesh_manager.m_children) {
        auto& child_mesh = *child_data.mesh;
        auto [me_to_child, child_to_me] = m_multi_mesh_manager.get_map_accessors(*this, child_mesh);

        const auto& child_sd = dart::SimplexDart::get_singleton(child_mesh.top_simplex_type());
        for (const auto& t : child_mesh.get_all(child_to_me.handle().primitive_type())) {
            dart::DartWrap d = child_to_me[child_sd.dart_from_tuple(t)];
            int64_t old_gid = d.global_id();
            if (is_removed(old_gid)) {
                spdlog::warn("map wasnt valid for {}", old_gid);
            }
            int64_t new_gid = top_map[old_gid];
            if (new_gid == -1) {
                spdlog::warn("Bad gid");
            }
            continue;
            d = dart::Dart{new_gid, d.permutation()};
            simplex::Simplex v =
                child_mesh.map_to_parent(simplex::Simplex(child_mesh.top_simplex_type(), t));
            // spdlog::info(
            //     "{} 2c[{}]: {} {} {}",

            //    child_mesh.id(t, child_mesh.top_simplex_type()),
            //    fmt::join(absolute_multi_mesh_id(), ","),
            //    d,
            //    old_gid,
            //    top_map[old_gid]);
            if (v.tuple().is_null()) {
                spdlog::warn("Error");
            }
        }
    }
    if (auto parent_ptr = m_multi_mesh_manager.m_parent; parent_ptr != nullptr) {
        auto [parent_to_me, me_to_parent] =
            parent_ptr->m_multi_mesh_manager.get_map_accessors(*parent_ptr, *this);


        const auto& p_sd = dart::SimplexDart::get_singleton(parent_ptr->top_simplex_type());
        for (const auto& t : parent_ptr->get_all(parent_to_me.handle().primitive_type())) {
            dart::DartWrap d = parent_to_me[p_sd.dart_from_tuple(t)];
            if (!d.is_null()) {
                int64_t old_gid = d.global_id();
                if (is_removed(old_gid)) {
                    spdlog::warn("map wasnt valid for {}", old_gid);
                }
                int64_t new_gid = top_map[old_gid];
                if (new_gid == -1) {
                    spdlog::warn("Bad gid");
                }
                continue;
                d = dart::Dart{new_gid, d.permutation()};
                // spdlog::info(
                //     "{} 2p[{}]: {} {} {}",
                //     parent_ptr->id(t, top_simplex_type()),
                //     fmt::join(absolute_multi_mesh_id(), ","),
                //     d,
                //     old_gid,
                //     top_map[old_gid]);
                std::vector<simplex::Simplex> vs =
                    parent_ptr->map_to_child(*this, simplex::Simplex(top_simplex_type(), t));
                for (const auto& v : vs) {
                    if (v.tuple().is_null()) {
                        spdlog::warn("Error");
                    }
                }
            }
        }
    }
}
std::vector<attribute::MeshAttributeHandle::HandleVariant> Mesh::builtin_attributes() const
{
    std::vector<attribute::MeshAttributeHandle::HandleVariant> data;
    for (const auto& vec : connectivity_attributes()) {
        std::copy(vec.begin(), vec.end(), std::back_inserter(data));
    }

    std::copy(m_flag_handles.begin(), m_flag_handles.end(), std::back_inserter(data));

    auto mm_handles = m_multi_mesh_manager.map_handles();
    std::copy(mm_handles.begin(), mm_handles.end(), std::back_inserter(data));
    return data;
}


template wmtk::attribute::MeshAttributeHandle
Mesh::register_attribute<char>(const std::string&, PrimitiveType, int64_t, bool, char);
template wmtk::attribute::MeshAttributeHandle
Mesh::register_attribute<int64_t>(const std::string&, PrimitiveType, int64_t, bool, int64_t);
template wmtk::attribute::MeshAttributeHandle
Mesh::register_attribute<double>(const std::string&, PrimitiveType, int64_t, bool, double);
template wmtk::attribute::MeshAttributeHandle
Mesh::register_attribute<Rational>(const std::string&, PrimitiveType, int64_t, bool, Rational);


template TypedAttributeHandle<char>
Mesh::register_attribute_typed(const std::string&, PrimitiveType, int64_t, bool, char);
template TypedAttributeHandle<int64_t>
Mesh::register_attribute_typed(const std::string&, PrimitiveType, int64_t, bool, int64_t);
template TypedAttributeHandle<double>
Mesh::register_attribute_typed(const std::string&, PrimitiveType, int64_t, bool, double);
template TypedAttributeHandle<Rational>
Mesh::register_attribute_typed(const std::string&, PrimitiveType, int64_t, bool, Rational);

template const int64_t& Mesh::get_attribute_default_value(
    const TypedAttributeHandle<int64_t>& handle) const;
template const char& Mesh::get_attribute_default_value(
    const TypedAttributeHandle<char>& handle) const;
template const double& Mesh::get_attribute_default_value(
    const TypedAttributeHandle<double>& handle) const;
template const wmtk::Rational& Mesh::get_attribute_default_value(
    const TypedAttributeHandle<wmtk::Rational>& handle) const;
} // namespace wmtk
