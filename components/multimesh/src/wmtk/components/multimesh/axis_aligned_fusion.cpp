#include "axis_aligned_fusion.hpp"
#include <nlohmann/json.hpp>
#include <set>
#include <wmtk/utils/DisjointSet.hpp>
#include "utils//decompose_attribute_path.hpp"

#include <wmtk/Mesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>

#include <wmtk/Types.hpp>
#include <wmtk/utils/EigenMatrixWriter.hpp>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/mesh_utils.hpp>

#include <wmtk/multimesh/same_simplex_dimension_bijection.hpp>
#include "MeshCollection.hpp"
#include "utils/get_attribute.hpp"


namespace wmtk::components::multimesh {
MultimeshAxisAlignedFusionOptions::~MultimeshAxisAlignedFusionOptions() = default;
std::shared_ptr<Mesh>
axis_aligned_fusion(const Mesh& mesh, const std::vector<bool>& operating_axis, double eps)
{
    return axis_aligned_fusion(
        mesh.get_attribute_handle<double>("vertices", PrimitiveType::Vertex),
        operating_axis,
        eps);
}

std::shared_ptr<Mesh> axis_aligned_fusion(
    const attribute::MeshAttributeHandle& position_handle,
    const std::vector<bool>& operating_axis,
    double eps)
{
    spdlog::warn("{}", fmt::join(operating_axis, ","));
    Mesh& mesh = const_cast<Mesh&>(position_handle.mesh());
    // get mesh dimension and checks
    int64_t mesh_dim = mesh.top_cell_dimension();

    if (mesh_dim != position_handle.dimension()) {
        throw std::runtime_error(fmt::format(
            "Can only perform axis aligned fusion on a {0}-dimensional position on a {0}-mesh, got "
            "a {1}-dimensional position",
            mesh_dim,
            position_handle.dimension()));
    }
    if (mesh_dim != operating_axis.size()) {
        throw std::runtime_error(fmt::format(
            "Can only perform axis aligned fusion on a {0} entries in the operating_axis mask on a "
            "{0}-mesh, got a {1} values",
            mesh_dim,
            operating_axis.size()));
    }

    std::map<std::string, std::vector<int64_t>> names;
    wmtk::utils::EigenMatrixWriter writer;
    MatrixX<double> V;
    mesh.serialize(writer);
    writer.get_position_matrix(V);


    // rescale to [0, 1]
    Eigen::MatrixXd V_rescale(V.rows(), V.cols());

    Eigen::VectorXd min_pos = V.colwise().minCoeff();

    // if (abs(min_pos[2] - 0.0) > eps)
    //     throw std::runtime_error("has non-zero z coordinate on 2d mesh");

    for (int64_t i = 0; i < V_rescale.rows(); ++i) {
        V_rescale.row(i) = V.row(i) - min_pos.transpose();
    }

    Eigen::VectorXd max_pos = V_rescale.colwise().maxCoeff();
    V_rescale.array().rowwise() /= max_pos.transpose().array();
    assert(V.rows() == mesh.get_all(PrimitiveType::Vertex).size());


    int dim = std::min<int>(operating_axis.size(), V.cols());
    auto comp = [&V_rescale, &operating_axis, dim, eps](int ai, int bi) {
        auto a = V_rescale.row(ai);
        auto b = V_rescale.row(bi);

        auto d = (b - a).cwiseAbs();

        for (size_t j = 0; j < dim; ++j) {
            if (operating_axis[j]) {
                if (abs(1 - d(j)) > eps) {
                    return false;
                }
            } else {
                if (d(j) > eps) {
                    return false;
                }
            }
        }
        return true;
    };
    std::vector<std::set<int64_t>> fusable_min(dim);
    std::vector<std::set<int64_t>> fusable_max(dim);
    for (int j = 0; j < V.rows(); ++j) {
        auto v = V.row(j);
        for (int d = 0; d < dim; ++d) {
            if (operating_axis[d]) {
                if (v(d) <= eps) {
                    fusable_min[d].emplace(j);
                } else if (v(d) + eps >= 1.0) {
                    fusable_max[d].emplace(j);
                }
            }
        }
    }
    wmtk::utils::DisjointSet ds(V.rows());
    for (int d = 0; d < dim; ++d) {
        for (const auto& m : fusable_min[d]) {
            for (const auto& M : fusable_max[d]) {
                if (comp(m, M)) {
                    ds.merge(m, M);
                }
            }
        }
    }

    std::map<int64_t, int64_t> root_indexer;
    auto roots = ds.roots();
    for (size_t j = 0; j < roots.size(); ++j) {
        root_indexer[roots[j]] = j;
    }

    auto get_index = [&ds, &root_indexer](int64_t i) { return root_indexer.at(ds.get_root(i)); };


    std::shared_ptr<Mesh> parent_ptr;
    // TODO: a lot of matrix copies to remove

    switch (mesh_dim) {
    case (2): {
        MatrixX<int64_t> FV;

        // TODO: make this the mah passed in
        writer.get_FV_matrix(FV);

        assert(FV.rows() == mesh.get_all(PrimitiveType::Triangle).size());

        // create periodic mesh
        RowVectors3l FV_new = FV.NullaryExpr(get_index);

        std::shared_ptr<TriMesh> parent = std::make_shared<TriMesh>();
        parent->initialize(FV_new);
        parent_ptr = parent;
        break;
    }
    case (3): {
        MatrixX<int64_t> TV;

        writer.get_TV_matrix(TV);

        assert(TV.rows() == mesh.get_all(PrimitiveType::Tetrahedron).size());

        // // create periodic mesh
        RowVectors4l TV_new = TV.NullaryExpr(get_index);
        std::shared_ptr<TetMesh> parent = std::make_shared<TetMesh>();
        parent->initialize(TV_new);
        parent_ptr = parent;


        break;
    }
    default: {
        throw std::runtime_error("mesh dimension not supported");
    }
    }
    auto child_map = wmtk::multimesh::same_simplex_dimension_bijection(*parent_ptr, mesh);
    parent_ptr->register_child_mesh(mesh.shared_from_this(), child_map);
    return parent_ptr;
}

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(MultimeshAxisAlignedFusionOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(attribute_path, axes_to_fuse, epsilon, fused_mesh_name);
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(MultimeshAxisAlignedFusionOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(attribute_path, axes_to_fuse, fused_mesh_name);
    if (nlohmann_json_j.contains("epsilon")) {
        WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(epsilon);
    }
}
void MultimeshAxisAlignedFusionOptions::to_json(nlohmann::json& j) const
{
    j = *this;
}
void MultimeshAxisAlignedFusionOptions::from_json(const nlohmann::json& j)
{
    *this = j;
}
void MultimeshAxisAlignedFusionOptions::run(MeshCollection& mc) const
{
    auto attr = utils::get_attribute(mc, attribute_path);
    assert(attr.mesh().is_multi_mesh_root());
    auto [mesh_path, attr_name] = utils::decompose_attribute_path(attribute_path.path);
    const auto& child_nmm = mc.get_named_multimesh(mesh_path);
    spdlog::info("{}", child_nmm.get_names_json()->dump(2));
    auto mesh = axis_aligned_fusion(attr, axes_to_fuse, epsilon);
    assert(!attr.mesh().is_multi_mesh_root());
    assert(mesh->is_multi_mesh_root());
    assert(mesh->is_from_same_multi_mesh_structure(attr.mesh()));
    auto& nmm = mc.emplace_mesh(*mesh, fused_mesh_name);
    spdlog::info("{}", nmm.get_names_json()->dump(2));
    nmm.append_child_mesh_names(*mesh, child_nmm);
}
} // namespace wmtk::components::multimesh
