#include "PassConfiguration.hpp"
#include <nlohmann/json.hpp>

namespace wmtk::components::configurator {
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(PassConfiguration)
{
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(passes)
    auto& ops = nlohmann_json_j["operations"];
    for (const auto& [name, op] : nlohmann_json_t.operations) {
        ops[name] = op;
    }
    auto& invs = nlohmann_json_j["invariants"];
    for (const auto& [name, inv] : nlohmann_json_t.invariants) {
        invs[name] = inv;
    }
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(PassConfiguration)
{
    if (nlohmann_json_j.contains("passes")) {
        auto passes = nlohmann_json_j["passes"];
        if (passes.is_array()) {
            nlohmann_json_t.passes = passes;
        } else {
            nlohmann_json_t.passes.emplace_back(passes);
        }
    }

    if (nlohmann_json_j.contains("operations")) {
        auto& ops = nlohmann_json_t.operations;
        const auto& opsj = nlohmann_json_j["operations"];
        if (opsj.is_array()) {
            for (const auto& op : opsj) {
                ops[op["name"].get<std::string>()] = op;
            }
        } else {
            for (const auto& [name, op] : opsj.items()) {
                ops[name] = op;
            }
        }
    }

    if (nlohmann_json_j.contains("invariants")) {
        auto& invs = nlohmann_json_t.invariants;
        const auto& invsj = nlohmann_json_j["invariants"];
        if (invsj.is_array()) {
            for (const auto& inv : invsj) {
                invs[inv["name"].get<std::string>()] = inv;
            }
        } else {
            for (const auto& [name, inv] : invsj.items()) {
                invs[name] = inv;
            }
        }
    }

    //
}
} // namespace wmtk::components::configurator
