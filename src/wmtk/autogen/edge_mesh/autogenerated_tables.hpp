#pragma once

// DO NOT MODIFY, autogenerated from the /scripts directory

#include <cstdint>
namespace wmtk::autogen::edge_mesh {
// Valid tuple local indices
extern const int8_t auto_valid_tuples[2][1];

// For each valid tuple encodes the raw tuple index
extern const int8_t auto_valid_tuple_indices[2];

// Index of each tuple according to valid tuple indexing
extern const int8_t auto_all_to_valid_tuple_indices[2];

// Valid tuple indices
extern const int8_t auto_valid_switch_table[2][1];

// Tuple group product using valid tuple indices
extern const int8_t auto_valid_switch_product_table[2][2];

// Tuple group product inverse using valid tuple indices
extern const int8_t auto_valid_switch_inverse_table[2];

// Which tuples are associated with switching. Last two entries are the identity action and opp
// action
extern const int8_t auto_valid_tuple_switch_indices[3];


} // namespace wmtk::autogen::edge_mesh
