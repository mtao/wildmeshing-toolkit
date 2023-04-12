#pragma once
#include <wmtk/TriMeshOperation.h>
#include <vector>

namespace extremeopt {
class ExtremeOpt;
namespace tests {

double compute_energy(const ExtremeOpt& opt);
void check_flips(const ExtremeOpt& opt);
void check_flips(const ExtremeOpt& opt, wmtk::TriMeshOperation::ExecuteReturnData);
void check_flips(const ExtremeOpt& opt, const std::vector<wmtk::TriMeshTuple>& tuple);

void perform_checks(const ExtremeOpt& extremeopt, const double E_in);
} // namespace tests
} // namespace extremeopt
