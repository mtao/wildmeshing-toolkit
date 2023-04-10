#include "Parameters.h"
#include <nlohmann/json.hpp>
namespace extremeopt {

    void Parameters::load(const nlohmann::json& config) {
    this->max_iters = config["max_iters"]; // iterations
    this->E_target = config["E_target"]; // Energy target
    this->ls_iters = config["ls_iters"]; // param for linesearch in smoothing operation
    this->do_newton = config["do_newton"]; // do newton/gd steps for smoothing operation
    this->do_collapse = config["do_collapse"]; // do edge_collapse or not
    this->do_swap = config["do_swap"]; // do edge_swap or not
    this->do_split = config["do_split"]; // do edge_split or not
    this->global_upsample = config["global_upsample"];
    // do global/local smooth (local smooth does not optimize boundary vertices)
    this->local_smooth = config["local_smooth"];
    this->global_smooth = config["global_smooth"];
    this->use_envelope = config["use_envelope"];
    this->elen_alpha = config["elen_alpha"];
    this->do_projection = config["do_projection"];
    this->with_cons = config["with_cons"];

    this->save_meshes = config["save_meshes"];
    }
}
