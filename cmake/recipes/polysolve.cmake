# PolySolve (https://github.com/polyfem/polysolve)
# License: MIT

if(TARGET polysolve::polysolve)
    return()
endif()

# Polysolve options for enabling/disabling optional libraries
option(POLYSOLVE_WITH_ACCELERATE OFF)
option(POLYSOLVE_WITH_CHOLMOD OFF)
option(POLYSOLVE_WITH_UMFPACK OFF)
option(POLYSOLVE_WITH_SUPERLU OFF)
option(POLYSOLVE_WITH_MKL OFF)
option(POLYSOLVE_WITH_CUSOLVER OFF)
option(POLYSOLVE_WITH_PARDISO OFF)
option(POLYSOLVE_WITH_HYPRE OFF)
option(POLYSOLVE_WITH_AMGCL OFF)
option(POLYSOLVE_WITH_SPECTRA OFF)

message(STATUS "Third-party: creating target 'polysolve'")

# TODO: this requires a conflicting version of Eigen. Reenable when Eigen 3.4+ is available.
set(POLYSOLVE_WITH_ACCELERATE OFF CACHE BOOL "Enable Apple Accelerate" FORCE)

include(CPM)
CPMAddPackage("gh:mtao/polysolve#053766abfd6ab101f7a47748bb52b84aae5a09dd")

set_target_properties(polysolve PROPERTIES FOLDER third_party)
set_target_properties(polysolve_linear PROPERTIES FOLDER third_party)
