#pragma once

namespace extremeopt{
    struct Parameters
    {
        int max_iters = 500;
        bool do_newton = false;
        bool do_swap = false;
        bool do_collapse = false;
        int ls_iters = 200;
        double E_target = 10.0;
        double split_thresh = 1.0;
        /* data */
    };
    
} // namespace extremeopt

//TODO:
// one full step of swap after opt   DONE.
// need to recompute the G matrix after one iteration 
// changed the 3d geometry
// edge len instead?





// put timer to profile

// to store per-t
// add split                         

// mpfr lower prior

// things to measure
// max step size for each tri around the vertice

// edge-len, areas, 