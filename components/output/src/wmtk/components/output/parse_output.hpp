#pragma once

#include "OutputOptions.hpp"

#include <nlohmann/json_fwd.hpp>
namespace wmtk::components::output {

using OutputOptionsCollection = std::vector<std::pair<std::string, OutputOptions>>;
//  These two types of paths will be prioritized
//  "file.hdf5"
//  {"path": "file.hdf5", ...}
//
// {"mesh_name": {"path": "file.hdf5", ...} }
// {"mesh_name": {"path": "file.hdf5", ...}, "mesh_name2": {"path": "file2.hdf5" } }
// {"mesh_name": [{"path": "file.hdf5", ...}, {"path": "file2.hdf5"} ],
//                 "mesh_name2: {"path": "file3.hdf5"}}
//
// note: arrays internally result in recusrive calls to parse_output.
//  ["mesh_name": {"path": "file.hdf5", ...}, "mesh_name2": {"path": "file2.hdf5"} ]
// This allows multiple outputs of a nameless mesh:
//  [{"path": "file.hdf5", ...}, {"path": "file2.hdf5"} ]
//
// you CANNOT have a mesh name and a single file format
// - this results in an ambiguous parse a mesh name?)
//  {"mesh_name": "file.hdf5" } // not allowed
OutputOptionsCollection parse_output(const nlohmann::json& js);
} // namespace wmtk::components::output


namespace nlohmann {

template <>
struct adl_serializer<wmtk::components::output::OutputOptionsCollection>
{
    using OutputOptions = wmtk::components::output::OutputOptions;
    static void to_json(json& j, const std::vector<std::pair<std::string,OutputOptions>>& p);

    static void from_json(const json& j, std::vector<std::pair<std::string,OutputOptions>>& p);
};

}
