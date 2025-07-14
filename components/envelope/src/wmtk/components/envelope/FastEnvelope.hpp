#pragma once
#include "Envelope.hpp"


namespace wmtk::components::envelope {


class FastEnvelope : public Envelope
{
public:
    bool is_inside(const Vector3d& p) const final override;
    bool is_inside(const Vector3r& p) const final override;
};
} // namespace wmtk::components::envelope
