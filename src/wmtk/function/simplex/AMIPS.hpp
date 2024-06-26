#pragma once

#include <optional>
#include <wmtk/attribute/MeshAttributeHandle.hpp>
#include <wmtk/function/PerSimplexFunction.hpp>
#include <wmtk/simplex/Simplex.hpp>

namespace wmtk::function {

double Tet_AMIPS_energy(const std::array<double, 12>& T);
double Tri_AMIPS_energy(const std::array<double, 6>& T);

void Tet_AMIPS_jacobian(const std::array<double, 12>& T, Eigen::Vector3d& j);
void Tri_AMIPS_jacobian(const std::array<double, 6>& T, Eigen::Vector2d& j);

void Tet_AMIPS_hessian(const std::array<double, 12>& T, Eigen::Matrix3d& h);
void Tri_AMIPS_hessian(const std::array<double, 6>& T, Eigen::Matrix2d& h);

class AMIPS : public PerSimplexFunction
{
public:
    /**
     * @brief Construct a new AMIPS function
     *
     * @param mesh
     * @param attribute_handle The handle to the attribute that differentiation is with respect to
     */
    AMIPS(const Mesh& mesh, const attribute::MeshAttributeHandle& attribute_handle);

    ~AMIPS(){};

public:
    double get_value(const simplex::Simplex& domain_simplex) const override;

    Eigen::VectorXd get_gradient(
        const simplex::Simplex& domain_simplex,
        const simplex::Simplex& variable_simplex) const override;
    Eigen::MatrixXd get_hessian(
        const simplex::Simplex& domain_simplex,
        const simplex::Simplex& variable_simplex) const override;

public:
    template <int64_t NV, int64_t DIM>
    std::array<double, NV * DIM> get_raw_coordinates(
        const simplex::Simplex& domain_simplex,
        const std::optional<simplex::Simplex>& variable_simplex = {}) const;
};

} // namespace wmtk::function
