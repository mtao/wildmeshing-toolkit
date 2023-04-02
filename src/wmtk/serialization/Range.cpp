
#include <wmtk/serialization/Hdf5Utils.hpp>
#include <wmtk/serialization/Range.h>
HIGHFIVE_REGISTER_TYPE(wmtk::serialization::Range, wmtk::serialization::Range::datatype);

using namespace wmtk;
HighFive::CompoundType serialization::Range::datatype()
{
    return HighFive::CompoundType{
        {"begin", HighFive::create_datatype<size_t>()},
        {"end", HighFive::create_datatype<size_t>()}};
}
