#include "tetmesh_topology_initialization.h"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <wmtk/autogen/tet_mesh/autogenerated_tables.hpp>
#include <wmtk/utils/internal/IndexSimplexMapper.hpp>

namespace wmtk {

std::tuple<RowVectors6l, RowVectors4l, RowVectors4l, VectorXl, VectorXl, VectorXl>
tetmesh_topology_initialization(Eigen::Ref<const RowVectors4l> T)
{
    RowVectors6l TE;
    RowVectors4l TF;
    RowVectors4l TT;
    VectorXl FT;
    VectorXl ET;
    VectorXl VT;

    //spdlog::info("Start");

    //wmtk::utils::internal::IndexSimplexMapper ism(T);
    //TE.resize(T.rows(),4);
    //TF.resize(T.rows(),4);
    //TT.resize(T.rows(),4);
    

    //spdlog::info("Start2");
    // First pass is identifying faces, and filling VT
    std::vector<std::vector<int64_t>> TTT;

    char iv0 = 0;
    char iv1 = 1;
    char iv2 = 2;
    char it = 3;
    char ii = 4;


    int64_t vertex_count = T.maxCoeff() + 1;

    // Build a table for finding Faces and populate the corresponding
    // topology relations
    {
        TTT.resize(T.rows() * 4);
        for (int64_t t = 0; t < T.rows(); ++t) {
            for (int64_t i = 0; i < 4; ++i) {
                // v1 v2 v3 f ei
                int64_t x = T(t, static_cast<int64_t>(autogen::tet_mesh::auto_3d_faces[i][0]));
                int64_t y = T(t, static_cast<int64_t>(autogen::tet_mesh::auto_3d_faces[i][1]));
                int64_t z = T(t, static_cast<int64_t>(autogen::tet_mesh::auto_3d_faces[i][2]));
                if (x > y) std::swap(x, y);
                if (y > z) std::swap(y, z);
                if (x > y) std::swap(x, y);

                std::vector<int64_t> r(5);
                r[iv0] = x;
                r[iv1] = y;
                r[iv2] = z;
                r[it] = t;
                r[ii] = i;
                TTT[t * 4 + i] = r;
            }
        }
        std::sort(TTT.begin(), TTT.end());

        // std::ofstream file("TTT_F_debug.txt");
        // for (const auto& row : TTT) {
        //     for (const auto& element : row) {
        //         file << element << " ";
        //     }
        //     file << std::endl;
        // }
        // file.close();

        // VT
        VT.resize(vertex_count, 1);
        for (int64_t i = 0; i < T.rows(); ++i) {
            for (int64_t j = 0; j < T.cols(); ++j) {
                VT[T(i, j)] = i;
            }
        }

        // Compute TF, TT, and FT
        TF.resize(T.rows(), 4);
        TT.resize(T.rows(), 4);
        std::vector<int64_t> FT_temp;

        // iterate over TTT to find faces
        // for every entry check if the next is the same, and update the connectivity accordingly

        for (int64_t i = 0; i < TTT.size(); ++i) {
            if ((i == TTT.size() - 1) || (TTT[i][0] != TTT[i + 1][0]) ||
                (TTT[i][1] != TTT[i + 1][1]) || (TTT[i][2] != TTT[i + 1][2])) {
                // If the next tuple is empty, then this is a boundary face
                FT_temp.push_back(TTT[i][it]);

                TT(TTT[i][it], TTT[i][ii]) = -1;
                TF(TTT[i][it], TTT[i][ii]) = FT_temp.size() - 1;
            } else {
                // this is an internal face, update both sides
                // If the next tuple is empty, then this is a boundary face

                FT_temp.push_back(TTT[i][it]);

                TT(TTT[i][it], TTT[i][ii]) = TTT[i + 1][it];
                TF(TTT[i][it], TTT[i][ii]) = FT_temp.size() - 1;

                TT(TTT[i + 1][it], TTT[i + 1][ii]) = TTT[i][it];
                TF(TTT[i + 1][it], TTT[i + 1][ii]) = FT_temp.size() - 1;

                ++i; // skip the other entry
            }
        }

        // copy FT
        FT.resize(FT_temp.size());
        for (int64_t i = 0; i < FT_temp.size(); ++i) FT(i) = FT_temp[i];
    }

    // Build a table for finding edges and populate the corresponding
    // topology relations
    {
        TTT.resize(T.rows() * 6);
        for (int64_t t = 0; t < T.rows(); ++t) {
            for (int64_t i = 0; i < 6; ++i) {
                // v1 v2 f ei
                int64_t x = T(t, static_cast<int64_t>(autogen::tet_mesh::auto_3d_edges[i][0]));
                int64_t y = T(t, static_cast<int64_t>(autogen::tet_mesh::auto_3d_edges[i][1]));
                if (x > y) std::swap(x, y);

                std::vector<int64_t> r(5);
                r[iv0] = x;
                r[iv1] = y;
                r[iv2] = 0; // unused
                r[it] = t;
                r[ii] = i;
                TTT[t * 6 + i] = r;
            }
        }
        std::sort(TTT.begin(), TTT.end());

        // std::ofstream file("TTT_E_debug.txt");
        // for (const auto& row : TTT) {
        //     for (const auto& element : row) {
        //         file << element << " ";
        //     }
        //     file << std::endl;
        // }
        // file.close();


        // Compute TE, ET
        TE.resize(T.rows(), 6);
        std::vector<int64_t> ET_temp;


        // iterate over TTT to find edges
        // for every entry check if the next is the same, and update the connectivity accordingly
        for (int64_t i = 0; i < TTT.size(); ++i) {
            if ((i == TTT.size() - 1) || (TTT[i][0] != TTT[i + 1][0]) ||
                (TTT[i][1] != TTT[i + 1][1])) {
                // new edge found
                ET_temp.push_back(TTT[i][it]);

                // update first copy
                TE(TTT[i][it], TTT[i][ii]) = ET_temp.size() - 1;
            } else {
                // new edge found
                ET_temp.push_back(TTT[i][it]);

                // update first copy
                TE(TTT[i][it], TTT[i][ii]) = ET_temp.size() - 1;

                // loop over all the copies
                int64_t j = 1;
                while ((i + j < TTT.size()) && (TTT[i][0] == TTT[i + j][0]) &&
                       (TTT[i][1] == TTT[i + j][1])) {
                    TE(TTT[i + j][it], TTT[i + j][ii]) = ET_temp.size() - 1;
                    ++j;
                }
                assert(j >= 2);

                i = i + j - 1; // skip the visited entries
            }
        }

        // copy ET
        ET.resize(ET_temp.size());
        for (int64_t i = 0; i < ET_temp.size(); ++i) ET(i) = ET_temp[i];
    }
    //spdlog::info("End");

    return {TE, TF, TT, VT, ET, FT};
}

} // namespace wmtk
