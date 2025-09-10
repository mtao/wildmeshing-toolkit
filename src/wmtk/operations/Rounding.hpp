#pragma once

#include <wmtk/utils/Rational.hpp>
#include "AttributeUpdate.hpp"


namespace wmtk::operations {

class Rounding : public AttributeUpdate
{
public:
    Rounding(Mesh& m, TypedAttributeHandle<Rational>& coordinate);

    std::vector<simplex::Simplex> execute(const simplex::Simplex& simplex) override;

private:
    TypedAttributeHandle<Rational>& m_coordinate_handle;
};

} // namespace wmtk::operations