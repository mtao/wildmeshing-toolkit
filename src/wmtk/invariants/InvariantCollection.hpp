#pragma once
#include <map>
#include <memory>
#include <vector>
#include "Invariant.hpp"


namespace wmtk {
namespace simplex {
class Simplex;
}

namespace invariants {

class InvariantCollection : public Invariant
{
public:
    // all children below indicates that every mesh in this invariant collection is below it (so we don't need to map up and then down)
    InvariantCollection(const Mesh& m, bool all_children_below = false);
    InvariantCollection(const InvariantCollection&);
    InvariantCollection(InvariantCollection&&);
    InvariantCollection& operator=(const InvariantCollection&);
    InvariantCollection& operator=(InvariantCollection&&);
    ~InvariantCollection();
    bool before(const simplex::Simplex& t) const override;
    bool after(
        const std::vector<Tuple>& top_dimension_tuples_before,
        const std::vector<Tuple>& top_dimension_tuples_after) const override;


    bool directly_modified_after(
        const std::vector<simplex::Simplex>& simplices_before,
        const std::vector<simplex::Simplex>& simplices_after) const override;

    // pass by value so this can be internally moved
    void add(std::shared_ptr<Invariant> invariant);

    const std::shared_ptr<Invariant>& get(int64_t index) const;
    int64_t size() const;
    bool empty() const;
    const std::vector<std::shared_ptr<Invariant>>& invariants() const;

    // finds all invariants held into itself or invariant collectiosn under it
    std::vector<std::shared_ptr<Invariant>> all_child_invariants() const;

    std::map<Mesh const*, std::vector<std::shared_ptr<Invariant>>> get_map_mesh_to_invariants()
        const;

    std::shared_ptr<InvariantCollection> children_reorganized_by_mesh() const;

    void setName(std::string name) { m_name = std::move(name); }

    std::string name() const override;

private:
    std::string m_name;
    std::vector<std::shared_ptr<Invariant>> m_invariants;
    // set true if every invariant under is a child
    bool m_use_map_to_child = false;
};

} // namespace invariants

class Mesh;
} // namespace wmtk
