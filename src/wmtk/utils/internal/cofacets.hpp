
#pragma once

#include <map>
#include <set>
#include "wmtk/dart/Dart.hpp"
namespace wmtk::utils::internal {


class IndexSimplexMapper;


std::map<int64_t, std::set<dart::Dart>> coboundary(
    const IndexSimplexMapper& ism,
    int8_t dim);

template <int8_t SimplexDim, int8_t FaceDim>
std::map<int64_t, std::set<dart::Dart>> cofacets(const IndexSimplexMapper& ism);

template <int8_t SimplexDim>
std::map<int64_t, std::set<dart::Dart>> coboundary(const IndexSimplexMapper& ism);
} // namespace wmtk::utils::internal
