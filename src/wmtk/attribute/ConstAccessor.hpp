#pragma once

#include "TupleAccessor.hpp"

namespace wmtk::attribute {
template <typename T>
using ConstAccessor = const TupleAccessor<T>;
} // namespace wmtk::attribute
