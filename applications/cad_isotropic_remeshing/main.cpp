
#include <CLI/App.hpp>
#include <CLI/CLI.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/applications/utils/element_count_report.hpp>
#include <wmtk/applications/utils/get_integration_test_data_root.hpp>
#include <wmtk/applications/utils/parse_jse.hpp>
#include <wmtk/components/input/InputOptions.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/components/multimesh/MultimeshOptions.hpp>
#include <wmtk/components/multimesh/multimesh.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/components/output/parse_output.hpp>
#include <wmtk/multimesh/same_simplex_dimension_bijection.hpp>
#include <wmtk/multimesh/utils/check_map_valid.hpp>
#include <wmtk/utils/cast_attribute.hpp>
#include <wmtk/utils/internal/manifold_decomposition.hpp>
#include "wmtk/components/utils/PathResolver.hpp"


#include <wmtk/components/mesh_info/transfer/edge_length.hpp>
#include <wmtk/components/mesh_info/transfer/mean_ratio_measure.hpp>
#include <wmtk/components/mesh_info/transfer/min_neighbor.hpp>

#include <wmtk/Mesh.hpp>
#include <wmtk/utils/Logger.hpp>

#include <wmtk/components/input/input.hpp>
#include <wmtk/components/isotropic_remeshing/IsotropicRemeshingOptions.hpp>
#include <wmtk/components/isotropic_remeshing/isotropic_remeshing.hpp>
#include <wmtk/components/multimesh/from_manifold_decomposition.hpp>
#include <wmtk/components/output/OutputOptions.hpp>
#include <wmtk/components/output/output.hpp>
#include <wmtk/components/utils/resolve_path.hpp>

#include <h5pp/h5pp.h>
#include <wmtk/components/output/output.hpp>
#include "EigenMeshes.hpp"
#include "Mesh.hpp"
#include "wmtk/invariants/Swap2dEdgeLengthInvariant.hpp"

using namespace wmtk::components;
using namespace wmtk;
namespace fs = std::filesystem;

constexpr static std::string root_attribute_name = "root";

int main(int argc, char* argv[])
{
    h5pp::File file(argv[1], h5pp::FileAccess::READONLY);

    FusedOutput fo;
    fo.load(file, "/");
    auto trimesh = fo.mesh.create();


    // auto edge_MD = wmtk::components::multimesh::from_manifold_decomposition(*trimesh);
    // kstd::vector<std::array<std::shared_ptr<wmtk::Mesh>,2>> manifold_decomposition =
    // wmtk::components::multimesh::from_manifold_decomposition(*trimesh);

    /*
    auto& F = fo.mesh.F;
    F = std::get<0>(wmtk::utils::internal::boundary_manifold_decomposition<3>(F));
    auto manifold = std::make_shared<wmtk::TriMesh>();
    manifold->initialize(F);
    auto map = wmtk::multimesh::same_simplex_dimension_bijection(*trimesh, *manifold);

    trimesh->register_child_mesh(manifold, map);

    auto mah = fo.topology.add_feature_edge_mesh_tag(*trimesh, "edge_tag");

    auto pos = trimesh->get_attribute_handle<double>("vertices", wmtk::PrimitiveType::Vertex);
    auto edge_tag = wmtk::utils::cast_attribute<char>(mah, *manifold, "edge_tag");
    pos = wmtk::utils::cast_attribute<char>(pos, *manifold, "vertices");
    {
        auto p = std::dynamic_pointer_cast<wmtk::EdgeMesh>(
            wmtk::components::multimesh::from_tag(edge_tag, 1, {pos}));
        // p->delete_attribute(p->get_attribute_handle<char>("edge_tag",
        // wmtk::PrimitiveType::Edge));
        manifold->delete_attribute(edge_tag);

        trimesh->deregister_child_mesh(manifold);
        trimesh = manifold;
    }
    */


    wmtk::attribute::MeshAttributeHandle position_attr =
        trimesh->get_attribute_handle<double>("vertices", wmtk::PrimitiveType::Vertex);

    wmtk::components::multimesh::MeshCollection mc;
    auto& nmm = mc.emplace_mesh(*trimesh, std::string("fused"));
    auto subcomplexes = fo.topology.feature_subcomplexes(*trimesh);

    // assert(wmtk::multimesh::utils::check_maps_valid(*trimesh));
    // for (const auto& m : subcomplexes) {
    //     assert(wmtk::multimesh::utils::check_maps_valid(*m));
    // }
    // return 0;

    {
        wmtk::components::isotropic_remeshing::IsotropicRemeshingOptions opts;
        opts.position_attribute = position_attr;


        int64_t iterations = 10;
        double length_relative = 1e-2;

        if (subcomplexes.size() >= 1) {
            nmm.set_name(*subcomplexes[0], "feature_edges");
            auto epos_attr =
                wmtk::utils::cast_attribute<double>(position_attr, *subcomplexes[0], "vertices");
            opts.copied_attributes.emplace_back(epos_attr, position_attr);
            opts.other_position_attributes.emplace_back(epos_attr);

            if (subcomplexes.size() >= 2) {
                nmm.set_name(*subcomplexes[1], "critical_points");
                // auto vpos_attr = wmtk::utils::cast_attribute<double>(
                //     position_attr,
                //     *subcomplexes[1],
                //     "vertices");
                // opts.copied_attributes.emplace_back(vpos_attr, position_attr);
                // opts.other_position_attributes.emplace_back(vpos_attr);
                opts.static_meshes.emplace_back("fused.feature_edges.critical_points");
            }
        }

        opts.iterations = iterations;
        opts.length_rel = length_relative;

        opts.swap.mode = components::isotropic_remeshing::EdgeSwapMode::Valence;


        {
            auto el = std::make_shared<wmtk::components::mesh_info::transfer::EdgeLength>();
            el->type = "edge_length";
            el->attribute_path = "fused/edge_length";
            el->base_attribute_path = "fused/vertices";

            auto mrm = std::make_shared<wmtk::components::mesh_info::transfer::MeanRatioMeasure>();
            mrm->type = "mean_ratio_transfer";
            mrm->attribute_path = "fused/mean_area_measure";
            mrm->base_attribute_path = "fused/vertices";
            auto min_mrm = std::make_shared<wmtk::components::mesh_info::transfer::MinNeighbor>();
            min_mrm->type = "min";
            min_mrm->attribute_path = "fused/mean_area_measure";
            min_mrm->parameters["simplex_dimension"] = 1;
            min_mrm->base_attribute_path = "fused/mean_area_measure";
            opts.utility_attributes.emplace_back(el);
            opts.utility_attributes.emplace_back(mrm);
            opts.utility_attributes.emplace_back(min_mrm);
        }
        opts.mesh_collection = &mc;

        wmtk::components::isotropic_remeshing::isotropic_remeshing(opts);
    }


    {
        wmtk::components::output::OutputOptions opts;
        opts.path = argv[2];
        opts.type = ".hdf5";
        spdlog::info("end making intermediate opts");
        opts.position_attribute = position_attr;
        wmtk::components::output::output(*trimesh, opts);
    }
}
