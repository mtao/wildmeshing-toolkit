#include <wmtk/utils/DisableWarnings.hpp>
#include <igl/read_triangle_mesh.h>
#include <wmtk/utils/EnableWarnings.hpp>
#include <wmtk/utils/triangle_mesh_io.h>
#include <igl/write_triangle_mesh.h>
namespace wmtk {

bool read_triangle_mesh(const std::filesystem::path& path, Eigen::MatrixXd& V, Eigen::MatrixXi& F)
{
    return igl::read_triangle_mesh(path.native(), V, F);
}
bool read_triangle_mesh(const std::filesystem::path& path, Eigen::MatrixXf& V, Eigen::MatrixXi& F)
{
    return igl::read_triangle_mesh(path.native(), V, F);
}
bool write_triangle_mesh(const std::filesystem::path& path, const Eigen::MatrixXd& V, const Eigen::MatrixXi& F) {
    return igl::write_triangle_mesh(path.native(), V, F);
}
bool write_triangle_mesh(const std::filesystem::path& path, const Eigen::MatrixXf& V, const Eigen::MatrixXi& F) {
    return igl::write_triangle_mesh(path.native(), V, F);
}
bool read_triangle_mesh(
    const std::filesystem::path& path,
    manifold_internal::Vertices& V,
    manifold_internal::Facets& F)
{
    return igl::read_triangle_mesh(path.native(), V, F);
}

bool read_triangle_mesh(
    const std::filesystem::path& path,
    Eigen::Matrix<double, Eigen::Dynamic, 3>& V,
    Eigen::Matrix<int, Eigen::Dynamic, 3>& F)
{
    return igl::read_triangle_mesh(path.native(), V, F);
}
bool read_triangle_mesh(
    const std::filesystem::path& path,
    Eigen::Matrix<float, Eigen::Dynamic, 3>& V,
    Eigen::Matrix<int, Eigen::Dynamic, 3>& F)
{
    return igl::read_triangle_mesh(path.native(), V, F);
}
bool read_triangle_mesh(
    const std::filesystem::path& path,
    Eigen::Matrix<double, Eigen::Dynamic, 2>& V,
    Eigen::Matrix<int, Eigen::Dynamic, 3>& F)
{
    return igl::read_triangle_mesh(path.native(), V, F);
}
bool read_triangle_mesh(
    const std::filesystem::path& path,
    Eigen::Matrix<float, Eigen::Dynamic, 2>& V,
    Eigen::Matrix<int, Eigen::Dynamic, 3>& F)
{
    return igl::read_triangle_mesh(path.native(), V, F);
}

bool read_triangle_mesh(
    const std::filesystem::path& path,
    Eigen::Matrix<double, Eigen::Dynamic, 3, Eigen::RowMajor>& V,
    Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor>& F)
{
    return igl::read_triangle_mesh(path.native(), V, F);
}
bool read_triangle_mesh(
    const std::filesystem::path& path,
    Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>& V,
    Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor>& F)
{
    return igl::read_triangle_mesh(path.native(), V, F);
}
bool read_triangle_mesh(
    const std::filesystem::path& path,
    Eigen::Matrix<double, Eigen::Dynamic, 2, Eigen::RowMajor>& V,
    Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor>& F)
{
    return igl::read_triangle_mesh(path.native(), V, F);
}
bool read_triangle_mesh(
    const std::filesystem::path& path,
    Eigen::Matrix<float, Eigen::Dynamic, 2, Eigen::RowMajor>& V,
    Eigen::Matrix<int, Eigen::Dynamic, 3, Eigen::RowMajor>& F)
{
    return igl::read_triangle_mesh(path.native(), V, F);
}
} // namespace wmtk
