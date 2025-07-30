#include "validate.hpp"
#include <fmt/ranges.h>
#include <wmtk/utils/verify_simplex_index_valences.hpp>
#include "wmtk/Mesh.hpp"
#include "wmtk/multimesh/utils/check_map_valid.hpp"
#include "wmtk/utils/Logger.hpp"
namespace wmtk::utils {

// do all valdiationts on this mesh (multimesh) as possible
bool validate(const Mesh& m)
{
    if (!m.is_multi_mesh_root()) {
        return validate(m.get_multi_mesh_root());
    }
    bool ok = true;
    for (const auto& mptr : m.get_all_meshes()) {
        if (!mptr->is_connectivity_valid()) {
            wmtk::logger().error(
                "Mesh {} connectivity was not valid, check env WMTK_LOGGER_LEVEL=debug for "
                "more info",
                fmt::join(mptr->absolute_multi_mesh_id(), ","));
            ok = false;
        }

        if (!wmtk::multimesh::utils::check_maps_valid(*mptr)) {
            wmtk::logger().error(
                "Mesh {} multimesh connectivity was not valid, check env "
                "WMTK_LOGGER_LEVEL=debug for more info",
                fmt::join(mptr->absolute_multi_mesh_id(), ","));
            ok = false;
        }

        if (!wmtk::utils::verify_simplex_index_valences(*mptr)) {
            wmtk::logger().error(
                "Mesh {} was not valid, check env WMTK_LOGGER_LEVEL=debug for more info",
                fmt::join(mptr->absolute_multi_mesh_id(), ","));
            ok = false;
        }
    }
    return ok;
}
} // namespace wmtk::utils
