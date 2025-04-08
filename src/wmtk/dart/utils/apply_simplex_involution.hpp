#pragma once
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/dart/Dart.hpp>

namespace wmtk::dart::utils {


    // When we map (ssd,s) down to (tsd,t) 
    // let c = canonical_simplex_orientation(ssd,s,tsd.simplex_type)
    // let phi_c a homomorphism such that 
    // -  phi_c(tsd.Id) = c
    // - phi_c commutes for any permutation only affecting t-simplices
    // - phi commutes for any permutation only affecting t-simplices
    // s c^{-1} phi_c(tsd.Id) = s c^{-1} c
    //
    //// Mapping from lower to higher: 
    // phi(t) = s
    // c s^{-1} phi*t = c
    //  phi*(c s^{-1}) t) = c
    //  phi*(c s^{-1}) t) = phi_c I
    //  phi = phi_c I (c s^{-1}) t)^{-1}
    //  phi = t^{-1} s c^{-1}  phi_c I
    //
    //  So we map t^{-1} s c^{-1} = M
    //  given input i in ssd, 
    //  phi_I

// maps (pt,a) to (opt,oa) then (opt,b)


wmtk::dart::Dart apply_simplex_involution(
    PrimitiveType pt,
    PrimitiveType opt,
    const dart::Dart& involution,
    const dart::Dart& source);

int8_t apply_simplex_involution(
    PrimitiveType pt,
    PrimitiveType opt,
    int8_t involution,
    int8_t source);

} // namespace wmtk::dart::utils
