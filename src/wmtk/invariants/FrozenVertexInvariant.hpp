#pragma once

#include <optional>
#include <wmtk/attribute/TypedAttributeHandle.hpp>
#include <wmtk/attribute/MeshAttributeHandle.hpp>
#include "Invariant.hpp"

namespace wmtk::invariants {
class FrozenVertexInvariant : public Invariant
{
public:
    // opposite vertex
    FrozenVertexInvariant(const Mesh& m, const attribute::TypedAttributeHandle<int64_t>& frozen_vertex_handle, bool opposite_vertex = false);
    FrozenVertexInvariant(const attribute::MeshAttributeHandle& frozen_vertex_handle, bool opposite_vertex = true);
    bool before(const simplex::Simplex& t) const override;
    std::string name() const final override;

private:
    const TypedAttributeHandle<int64_t> m_frozen_vertex_handle;
};



} // namespace wmtk::invariants
