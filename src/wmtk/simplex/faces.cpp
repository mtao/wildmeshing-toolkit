#include "faces.hpp"
#include "boundary.hpp"

#include <wmtk/utils/primitive_range_iter.hpp>
#include "faces_single_dimension.hpp"


namespace wmtk::simplex {
SimplexCollection faces(const Mesh& mesh, const Simplex& simplex, const bool sort_and_clean)
{
    SimplexCollection collection(mesh);

    faces(collection, simplex, sort_and_clean);

    return collection;
}

void faces(SimplexCollection& simplex_collection, const Simplex& simplex, const bool sort_and_clean)
{
    for (auto pt : wmtk::utils::primitive_below_iter(simplex.primitive_type() - 1, false)) {
        faces_single_dimension(simplex_collection, simplex, pt);
    }


    if (sort_and_clean) {
        simplex_collection.sort_and_clean();
    }
}

} // namespace wmtk::simplex
