#pragma once
namespace wmtk {
class Mesh;
namespace multimesh {
class MultiMeshManager;
}
} // namespace wmtk

namespace wmtk::multimesh::utils {

class MapValidator
{
public:
    MapValidator(const Mesh& m);
    bool check_child_map_attributes_valid() const;
    bool check_parent_map_attribute_valid() const;
    bool check_all() const;

    bool check_switch_homomorphism() const;
    bool check_child_switch_homomorphism(const Mesh& child) const;

    bool check_facet_uniqueness() const;
    bool check_child_facet_uniqueness(const Mesh& child) const;

    void print();

private:
    const Mesh& m_mesh;
};
} // namespace wmtk::multimesh::utils
