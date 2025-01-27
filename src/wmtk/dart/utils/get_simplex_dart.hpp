#pragma once
#include <wmtk/autogen/point_mesh/SimplexDart.hpp>
#include <wmtk/autogen/edge_mesh/SimplexDart.hpp>
#include <wmtk/autogen/tri_mesh/SimplexDart.hpp>
#include <wmtk/autogen/tet_mesh/SimplexDart.hpp>
#include <wmtk/PrimitiveType.hpp>

namespace wmtk::dart::utils {
    namespace detail {
        template <int8_t Dim>
            struct simplex_dart_type_from_dimension {};
        template <>struct simplex_dart_type_from_dimension<0> {using type = wmtk::autogen::point_mesh::SimplexDart;};
        template <>struct simplex_dart_type_from_dimension<1> {using type = wmtk::autogen::edge_mesh::SimplexDart;};
        template <>struct simplex_dart_type_from_dimension<2> {using type = wmtk::autogen::tri_mesh::SimplexDart;};
        template <>struct simplex_dart_type_from_dimension<3> {using type = wmtk::autogen::tet_mesh::SimplexDart;};
        template <int8_t Dim>
        using simplex_dart_type_from_dimension_t = typename simplex_dart_type_from_dimension<Dim>::type;
    }


    template <int8_t Dim>
    auto get_simplex_dart() {
        return detail::simplex_dart_type_from_dimension_t<Dim>{};
    }
    template <PrimitiveType PT>
    auto get_simplex_dart_from_primitive_type() {

        return get_simplex_dart<get_primitive_type_id(PT)>();
    }
    inline const dart::SimplexDart& get_simplex_dart(PrimitiveType pt) {

        return dart::SimplexDart::get_singleton(pt);
    }
   inline  const dart::SimplexDart& get_simplex_dart(int8_t dim) {
        return get_simplex_dart(get_primitive_type_from_id(dim));
    }

}
