#pragma once
#include "wmtk/attribute/Accessor.hpp"
#include <Eigen/Geometry>

namespace wmtk::utils {



template <
    typename T,
    typename MeshType,
    typename AttributeType,
    int Dim>
    auto bounding_box(const wmtk::attribute::Accessor<T,MeshType,AttributeType,Dim>& acc) -> Eigen::AlignedBox<T,Dim>{

        Eigen::AlignedBox<T,Dim> box(acc.dimension());
        
        for(const Tuple& t: acc.mesh().get_all(acc.primitive_type())) {
            
            box.extend(acc.const_vector_attribute(t));
        }



        return box;
    }
}
