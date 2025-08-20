
#pragma once
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/input/InputOptions.hpp>
#include <wmtk/components/output/OutputOptions.hpp>
#include "PassOptions.hpp"
#include "PassConfiguration.hpp"
#include "invariants/InvariantOptions.hpp"
#include "operations/OperationOptions.hpp"


namespace wmtk::components::configurator {
struct Configuration: public PassConfiguration
{
    wmtk::components::input::InputOptions input;
    // output is optional
    wmtk::components::output::OutputOptions output;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(Configuration);
};
} // namespace wmtk::components::configurator
