#pragma once
#include <Eigen/Core>
#include <set>
#include <wmtk/Tuple.hpp>

#include <map>

namespace wmtk {
class Mesh;
}
template <typename T>
struct EigenMesh
{
    Eigen::Index start() const { return m_start; }
    Eigen::Index end() const { return m_start + M.rows(); }

    Eigen::Index m_start = 0;
    Eigen::MatrixX<T> M;

    template <typename D>
    void assign(Eigen::MatrixBase<D>& m, T offset = 0) const
    {
        m.block(start(), 0, M.rows(), M.cols()) = M.array() + offset;
        // m(Eigen::seq(start(), end()), Eigen::all) = M;
    }
};
struct EigenMeshes
{
    EigenMesh<double> V;
    EigenMesh<int64_t> F;

    std::vector<std::set<int64_t>> VF;

    void compute_vf();

    bool in_v_range(int64_t index) const { return index >= V.start() && index < V.end(); }
    bool in_f_range(int64_t index) const
    {
        return index >= F.M.minCoeff() && index <= F.M.maxCoeff();
    }
    wmtk::Tuple update_to_fused(const wmtk::Tuple& t) const;
};

struct EigenMeshesBuilder
{
    EigenMeshesBuilder(const wmtk::Mesh& m, const std::string_view& pos_name);
    const EigenMeshes& load(const std::vector<std::array<int64_t, 2>>& indices);


    int64_t total_V = 0;
    int64_t total_F = 0;
    const wmtk::Mesh& mesh;
    const std::string position_attribute_name;
    std::vector<EigenMeshes> meshes;
    EigenMeshes merge_meshes() const;
};
