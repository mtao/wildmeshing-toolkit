#pragma once
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wswitch-enum"
#elif (defined(__GNUC__) || defined(__GNUG__)) && !(defined(__clang__) || defined(__INTEL_COMPILER))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#endif
#include <highfive/H5DataType.hpp>
#if defined(__clang__)
#pragma clang diagnostic pop
#elif (defined(__GNUC__) || defined(__GNUG__)) && !(defined(__clang__) || defined(__INTEL_COMPILER))
#pragma GCC diagnostic pop
#endif

namespace wmtk::serialization {
struct Range
{
    size_t begin = 0;
    size_t end = 0;
    static HighFive::CompoundType datatype();
};
}
//WMTK_HIGHFIVE_DECLARE_TYPE(wmtk::serialization::Range)
    template <>                          
    HighFive::DataType HighFive::create_datatype<wmtk::serialization::Range>();

