#include "attribute_missing_error.hpp"
#include "named_error_text.hpp"
#include <fmt/format.h>
namespace wmtk::components::multimesh::utils::detail {

std::string attribute_missing_error::make_message(const AttributeDescription& ad)
{
    return fmt::format("Could not find attribute {}", make_named_error_string(ad));
}
} // namespace wmtk::components::multimesh::utils::detail
