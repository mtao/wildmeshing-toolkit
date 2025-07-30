#include "InvariantCollection.hpp"
#include <memory>
#include <type_traits>
#include <wmtk/Mesh.hpp>
#include <wmtk/simplex/Simplex.hpp>
#include <wmtk/utils/Logger.hpp>

namespace wmtk::invariants {

InvariantCollection::InvariantCollection(
    const Mesh& m,
    bool all_children_below,
    bool use_before,
    bool use_old_state_in_after,
    bool use_new_state_in_after)
    : Invariant(m, use_before, use_old_state_in_after, use_new_state_in_after)
    , m_use_map_to_child(all_children_below)
{}
InvariantCollection::~InvariantCollection() = default;
InvariantCollection::InvariantCollection(const InvariantCollection&) = default;
InvariantCollection::InvariantCollection(InvariantCollection&&) = default;
InvariantCollection& InvariantCollection::operator=(const InvariantCollection& o)
{
    assert(o.mesh() == mesh());
    m_invariants = o.m_invariants;
    return *this;
}
InvariantCollection& InvariantCollection::operator=(InvariantCollection&& o)
{
    assert(o.mesh() == mesh());
    m_invariants = std::move(o.m_invariants);
    return *this;
}

void InvariantCollection::add(std::shared_ptr<Invariant> invariant)
{
    m_invariants.emplace_back(std::move(invariant));
}
bool InvariantCollection::before(const simplex::Simplex& t) const
{
    for (const auto& invariant : m_invariants) {
        if (&mesh() != &invariant->mesh()) {
            for (const Tuple& ct : m_use_map_to_child ? mesh().lub_map_tuples(invariant->mesh(), t)
                                                      : mesh().map_tuples(invariant->mesh(), t)) {
                if (!invariant->before(
                        simplex::Simplex(invariant->mesh(), t.primitive_type(), ct))) {
#if defined(WMTK_ENABLED_DEV_MODE)
                    wmtk::logger().debug(
                        "{}::before false because {} was false",
                        name(),
                        invariant->name());
#endif
                    return false;
                }
            }
        } else {
            if (!invariant->before(t)) {
#if defined(WMTK_ENABLED_DEV_MODE)
                wmtk::logger().debug(
                    "{}::before false because {} was false",
                    name(),
                    invariant->name());
#endif
                return false;
            }
        }
    }
    return true;
}
bool InvariantCollection::after(
    const std::vector<Tuple>& top_dimension_tuples_before,
    const std::vector<Tuple>& top_dimension_tuples_after) const
{
    for (const auto& invariant : m_invariants) {
        if (&mesh() != &invariant->mesh()) {
            const bool invariant_uses_old_state = invariant->use_old_state_in_after();
            const bool invariant_uses_new_state = invariant->use_new_state_in_after();
            if (!(invariant_uses_old_state || invariant_uses_new_state)) {
                continue;
            }
            auto map = [&](const auto& tuples) -> std::vector<Tuple> {
                return m_use_map_to_child ? mesh().lub_map_tuples(
                                                invariant->mesh(),
                                                mesh().top_simplex_type(),
                                                tuples)
                                          : mesh().map_tuples(
                                                invariant->mesh(),
                                                mesh().top_simplex_type(),
                                                tuples);
            };
            const std::vector<Tuple> mapped_tuples_after =
                invariant_uses_new_state ? map(top_dimension_tuples_after) : std::vector<Tuple>{};
            const std::vector<Tuple> mapped_tuples_before =
                invariant_uses_old_state ? mesh().parent_scope(map, top_dimension_tuples_before)
                                         : std::vector<Tuple>{};
            if (!invariant->after(mapped_tuples_before, mapped_tuples_after)) {
#if defined(WMTK_ENABLED_DEV_MODE)
                wmtk::logger().debug(
                    "{}::after false because {} was false",
                    name(),
                    invariant->name());
#endif
                return false;
            }
        } else {
            if (!invariant->after(top_dimension_tuples_before, top_dimension_tuples_after)) {
#if defined(WMTK_ENABLED_DEV_MODE)
                wmtk::logger().debug(
                    "{}::after false because {} was false",
                    name(),
                    invariant->name());
#endif
                return false;
            }
        }
        // assert(&mesh() != &invariant->mesh());
        // if (!invariant->after(type, tuples)) {
        //     return false;
        // }
    }
    return true;
}

bool InvariantCollection::directly_modified_after(
    const std::vector<simplex::Simplex>& simplices_before,
    const std::vector<simplex::Simplex>& simplices_after) const
{
#ifndef NDEBUG
    for (const auto& s : simplices_before) {
        mesh().parent_scope([&]() { assert(mesh().is_valid(s.tuple())); });
    }
    for (const auto& s : simplices_after) {
        assert(mesh().is_valid(s.tuple()));
    }
#endif

    for (const auto& invariant : m_invariants) {
        if (&mesh() != &invariant->mesh()) {
            auto mapped_simplices_after = mesh().map(invariant->mesh(), simplices_after);
            auto mapped_simplices_before = mesh().parent_scope(
                [&]() { return mesh().map(invariant->mesh(), simplices_before); });
#ifndef NDEBUG
            for (const auto& s : mapped_simplices_before) {
                mesh().parent_scope([&]() { assert(invariant->mesh().is_valid(s.tuple())); });
            }
            for (const auto& s : mapped_simplices_after) {
                assert(invariant->mesh().is_valid(s.tuple()));
            }
            assert(mesh().is_from_same_multi_mesh_structure(invariant->mesh()));
#endif
            if (!invariant->directly_modified_after(
                    mapped_simplices_before,
                    mapped_simplices_after)) {
                return false;
            }
        } else {
            if (!invariant->directly_modified_after(simplices_before, simplices_after)) {
                return false;
            }
        }
    }
    return true;
}

const std::shared_ptr<Invariant>& InvariantCollection::get(int64_t index) const
{
    return m_invariants.at(index);
}
int64_t InvariantCollection::size() const
{
    return m_invariants.size();
}
bool InvariantCollection::empty() const
{
    return m_invariants.empty();
}
const std::vector<std::shared_ptr<Invariant>>& InvariantCollection::invariants() const
{
    return m_invariants;
}

std::map<Mesh const*, std::vector<std::shared_ptr<Invariant>>>
InvariantCollection::get_map_mesh_to_invariants() const
{
    std::map<Mesh const*, std::vector<std::shared_ptr<Invariant>>> mesh_invariants_map;


    for (std::shared_ptr<Invariant> inv : m_invariants) {
        // TODO check if that if statement is correct
        auto invc = std::dynamic_pointer_cast<InvariantCollection>(inv);
        if (bool(invc)) {
            auto sub_map = invc->get_map_mesh_to_invariants();

            for (const auto& [mptr, i] : sub_map) {
                auto& vec = mesh_invariants_map[mptr];
                vec.insert(vec.end(), i.begin(), i.end());
            }
        } else {
            mesh_invariants_map[&(inv->mesh())].push_back(inv);
        }
    }
    return mesh_invariants_map;
}
std::shared_ptr<InvariantCollection> InvariantCollection::children_reorganized_by_mesh(
    bool use_before,
    bool use_old_state_in_after,
    bool use_new_state_in_after) const
{
    std::map<Mesh const*, std::vector<std::shared_ptr<Invariant>>> mesh_invariants_map =
        get_map_mesh_to_invariants();

    std::map<Mesh const*, std::shared_ptr<InvariantCollection>> collections;
    std::map<std::vector<int64_t>, Mesh const*> ids;
    for (const auto& [key, value] : mesh_invariants_map) {
        auto ic = std::make_shared<InvariantCollection>(
            *key,
            true,
            use_before,
            use_old_state_in_after,
            use_new_state_in_after);
        for (const auto& i : value) {
            if ((i->use_before() && use_before) ||
                (i->use_old_state_in_after() && use_old_state_in_after) ||
                (i->use_new_state_in_after() && use_new_state_in_after)) {
                ic->add(i);
            }
        }

        collections[key] = ic;
        ids[key->absolute_multi_mesh_id()] = key;
    }

    if (ids.find(std::vector<int64_t>{}) == ids.end()) {
        const auto& root = mesh().get_multi_mesh_root();
        ids[root.absolute_multi_mesh_id()] = &root;

        collections[&root] = std::make_shared<InvariantCollection>(root, true);
    }


    for (const auto& [id, meshptr] : ids) {
        const auto& ic = collections[meshptr];
        std::vector<int64_t> parent_id = id;
        while (!parent_id.empty()) {
            parent_id.pop_back();
            if (auto it = ids.find(parent_id); it != ids.end()) {
                collections.at(it->second)->add(ic);
                break;
            }
        }
    }
    for (auto& [id, ic] : collections) {
        auto& v = ic->m_invariants;
        std::sort(v.begin(), v.end());
        v.erase(std::unique(v.begin(), v.end()), v.end());
    }
    // return the root
    return collections.at(ids.at({}));
}

std::string InvariantCollection::name() const
{
    if (m_name.empty()) {
        std::vector<std::string> names;
        for (const auto& i : m_invariants) {
            names.emplace_back(i->name());
        }
        return fmt::format("InvariantCollection[{}]", fmt::join(names, ","));
    } else {
        return m_name;
    }
}

} // namespace wmtk::invariants
