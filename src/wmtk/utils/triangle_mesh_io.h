#pragma once
#include <Eigen/Core>
#include <filesystem>
#include <wmtk/utils/ManifoldUtils.hpp>


namespace wmtk {

bool read_triangle_mesh(const std::filesystem::path& path, Eigen::MatrixXd& V, Eigen::MatrixXi& F);
bool read_triangle_mesh(const std::filesystem::path& path, Eigen::MatrixXf& V, Eigen::MatrixXi& F);
bool write_triangle_mesh(const std::filesystem::path& path, const Eigen::MatrixXd& V, const Eigen::MatrixXi& F);
bool write_triangle_mesh(const std::filesystem::path& path, const Eigen::MatrixXf& V, const Eigen::MatrixXi& F);

bool read_triangle_mesh(
    const std::filesystem::path& path,
    manifold_internal::Vertices& V,
    manifold_internal::Facets& F);

bool read_triangle_mesh(
    const std::filesystem::path& path,
    Eigen::Matrix<double, Eigen::Dynamic, 3>& V,
    Eigen::Matrix<int, Eigen::Dynamic, 3>& F);
bool read_triangle_mesh(
    const std::filesystem::path& path,
    Eigen::Matrix<float, Eigen::Dynamic, 3>& V,
    Eigen::Matrix<int, Eigen::Dynamic, 3>& F);
bool read_triangle_mesh(
    const std::filesystem::path& path,
    Eigen::Matrix<double, Eigen::Dynamic, 2>& V,
    Eigen::Matrix<int, Eigen::Dynamic, 3>& F);
bool read_triangle_mesh(
    const std::filesystem::path& path,
    Eigen::Matrix<float, Eigen::Dynamic, 2>& V,
    Eigen::Matrix<int, Eigen::Dynamic, 3>& F);

bool read_triangle_mesh(
    const std::filesystem::path& path,
    Eigen::Matrix<double, Eigen::Dynamic, 3, Eigen::RowMajor>& V,
    Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor>& F);
bool read_triangle_mesh(
    const std::filesystem::path& path,
    Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>& V,
    Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor>& F);
bool read_triangle_mesh(
    const std::filesystem::path& path,
    Eigen::Matrix<double, Eigen::Dynamic, 2, Eigen::RowMajor>& V,
    Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor>& F);
bool read_triangle_mesh(
    const std::filesystem::path& path,
    Eigen::Matrix<float, Eigen::Dynamic, 2, Eigen::RowMajor>& V,
    Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor>& F);
} // namespace wmtk
