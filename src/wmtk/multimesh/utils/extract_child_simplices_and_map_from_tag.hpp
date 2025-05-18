
#pragma once
#include <array>
#include <memory>
#include <vector>
#include <wmtk/Tuple.hpp>
#include <wmtk/Types.hpp>
#include <wmtk/attribute/MeshAttributeHandle.hpp>


namespace wmtk {
class Mesh;
class PointMesh;
class EdgeMesh;
class TriMesh;
class TetMesh;
} // namespace wmtk

namespace wmtk::multimesh::utils {


template <typename T>
std::vector<Tuple> tagged_tuples(
    const Mesh& m,
    const wmtk::attribute::TypedAttributeHandle<T>& tag_handle,
    const T& tag_value);

// returns a set of tuples that can be passed into multimesh::from_facet_orientations to get mapping
// tuples
template <typename T>
std::pair<MatrixXl, std::vector<Tuple>> extract_child_simplices_and_map_from_tag(
    const Mesh& m,
    const wmtk::attribute::TypedAttributeHandle<T>& tag_handle,
    const T& tag_value);

std::pair<MatrixXl, std::vector<Tuple>> extract_child_simplices_and_map_from_tag(
    const wmtk::attribute::MeshAttributeHandle& tag_handle,
    const wmtk::attribute::MeshAttributeHandle::ValueVariant& tag_value);

} // namespace wmtk::multimesh::utils
