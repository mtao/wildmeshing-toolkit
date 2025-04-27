#pragma once
#include <nlohmann/json.hpp>
#include <wmtk/attribute/MeshAttributeHandle.hpp>


namespace wmtk::components::mesh_info::transfer {
template <template <typename, int, typename, int> typename Functor>
struct TransferFunctorTraits
{
    static int output_dimension(const attribute::MeshAttributeHandle& mah);
    static attribute::AttributeType output_type(
        const attribute::MeshAttributeHandle& mah,
        const nlohmann::json& js = {});

    static int simplex_dimension(
        const attribute::MeshAttributeHandle& mah,
        const nlohmann::json& js = {});
};
template <template <typename, int, typename, int> typename Functor>
int TransferFunctorTraits<Functor>::output_dimension(const attribute::MeshAttributeHandle& mah)
{
    return mah.dimension();
}
template <template <typename, int, typename, int> typename Functor>
attribute::AttributeType TransferFunctorTraits<Functor>::output_type(
    const attribute::MeshAttributeHandle& mah,
    const nlohmann::json& js)
{
    return mah.held_type();
}

template <template <typename, int, typename, int> typename Functor>
int TransferFunctorTraits<Functor>::simplex_dimension(
    const attribute::MeshAttributeHandle& mah,
    const nlohmann::json& js)
{
    if (js.contains("simplex_dimension")) {
        return js["simplex_dimension"].get<int>();
    } else {
        return get_primitive_type_id(mah.primitive_type());
    }
}
} // namespace wmtk::components::mesh_info::transfer
