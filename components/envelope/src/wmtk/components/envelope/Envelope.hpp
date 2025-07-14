#pragma once

#include <wmtk/PrimitiveType.hpp>
#include <wmtk/Tuple.hpp>
#include <wmtk/Types.hpp>
namespace wmtk {
class Mesh;
}
namespace wmtk::components::envelope {


class Envelope
{
public:
    virtual ~Envelope();
    virtual bool is_inside(const Vector3d& p) const = 0;
    virtual bool is_inside(const Vector3r& p) const = 0;

    virtual bool is_inside(const Mesh& m, const Tuple& tuple, const PrimitiveType pt) const = 0;
    bool is_inside(const Mesh& m, const std::vector<Tuple>& simplices, const PrimitiveType pt)
        const;
    bool is_inside(const Mesh& m, const std::vector<Tuple>& simplices) const;
};
} // namespace wmtk::components::envelope
