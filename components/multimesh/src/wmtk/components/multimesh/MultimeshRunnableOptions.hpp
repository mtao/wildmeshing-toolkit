#pragma once
#include <nlohmann/json_fwd.hpp>
namespace wmtk::components::multimesh {
class MeshCollection;
class MultimeshRunnableOptions
{
public:
    MultimeshRunnableOptions();
    bool operator<=>(const MultimeshRunnableOptions&) const = default;
    virtual ~MultimeshRunnableOptions();
    virtual void run(MeshCollection&) const = 0;
    virtual void to_json(nlohmann::json& j) const = 0;
    virtual void from_json(const nlohmann::json&) = 0;
};
} // namespace wmtk::components::multimesh
