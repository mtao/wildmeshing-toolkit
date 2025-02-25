#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/utils/simplex_index_from_permutation_index.hpp>
#include "darts_using_faces.hpp"


namespace wmtk::tests::tools {

int8_t darts_from_permutation(PrimitiveType mesh_type, const std::vector<int8_t>& indices)
{
    const auto& sd = wmtk::dart::SimplexDart::get_singleton(mesh_type);

    std::vector<int8_t> darts;
    auto matches = [mesh_type, &indices](int8_t index) -> bool {
        for (size_t j = 0; j < 3; ++j) {
            const auto& ind_opt = indices[j];
            if (ind_opt.has_value()) {
                const int64_t ind = ind_opt.value();
                const PrimitiveType pt = get_primitive_type_from_id(ind);
                const int8_t got_ind =
                    wmtk::dart::utils::simplex_index_from_permutation_index(mesh_type, index, pt);
                if (ind != got_ind) {
                    return false;
                }
            }
        }
        return true;
    };
    for (int8_t i = 0; i < sd.size(); ++i) {
        if (matches(i)) {
            return i;
        }
    }
    return -1;
}

int8_t darts_using_faces(PrimitiveType mesh_type, const std::vector<std::optional<int8_t>>& indices)
{
    const auto& sd = wmtk::dart::SimplexDart::get_singleton(mesh_type);

    std::vector<int8_t> darts;
    auto matches = [mesh_type, &indices](int8_t index) -> bool {
        for (size_t j = 0; j < 3; ++j) {
            const auto& ind_opt = indices[j];
            if (ind_opt.has_value()) {
                const int64_t ind = ind_opt.value();
                const PrimitiveType pt = get_primitive_type_from_id(ind);
                const int8_t got_ind =
                    wmtk::dart::utils::simplex_index_from_permutation_index(mesh_type, index, pt);
                if (ind != got_ind) {
                    return false;
                }
            }
        }
        return true;
    };
    for (int8_t i = 0; i < sd.size(); ++i) {
        if (matches(i)) {
            return i;
        }
    }
    return -1;
}

} // namespace wmtk::tests::tools
