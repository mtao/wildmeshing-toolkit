#pragma once

// DO NOT MODIFY, autogenerated from the /scripts directory

#include <cstdint>
namespace wmtk::autogen::tet_mesh {
// lvids for a specfic leid
extern const int64_t auto_3d_edges[6][2];

// lvids for a specific lfid
extern const int64_t auto_3d_faces[4][3];

// vertex switch's (tuple_index -> [lvid,leid,lfid])
extern const int64_t auto_3d_table_vertex[96][3];

// edge switch's (tuple_index -> [lvid,leid,lfid])
extern const int64_t auto_3d_table_edge[96][3];

// face switch's (tuple_index -> [lvid,leid,lfid])
extern const int64_t auto_3d_table_face[96][3];

// (tuple_index) -> is_ccw
extern const int64_t auto_3d_table_ccw[96];

// lvid -> a ccw [lvid,leid,lfid]
extern const int64_t auto_3d_table_complete_vertex[4][3];

// evid -> a ccw [lvid,leid,lfid]
extern const int64_t auto_3d_table_complete_edge[6][3];

// fvid -> a ccw [lvid,leid,lfid]
extern const int64_t auto_3d_table_complete_face[4][3];

// Valid tuple local indices
extern const int8_t auto_valid_tuples[24][3];

// For each valid tuple encodes the raw tuple index
extern const int8_t auto_valid_tuple_indices[24];

// Index of each tuple according to valid tuple indexing
extern const int8_t auto_all_to_valid_tuple_indices[96];

// Valid tuple indices
extern const int8_t auto_valid_switch_table[24][3];

// Tuple group product using valid tuple indices
extern const int8_t auto_valid_switch_product_table[24][24];

// Tuple group product inverse using valid tuple indices
extern const int8_t auto_valid_switch_inverse_table[24];

// Which tuples are associated with switching. Last two entries are the identity action and opp
// action
extern const int8_t auto_valid_tuple_switch_indices[5];


extern const int8_t subdart_preserving_action_1[24][6];


extern const int8_t max_subdart_preservation_dimension_1[24][6];


extern const int8_t subdart_preserving_action_2[24][4];


extern const int8_t max_subdart_preservation_dimension_2[24][4];


} // namespace wmtk::autogen::tet_mesh
