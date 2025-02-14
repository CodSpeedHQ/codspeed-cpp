#ifndef CODSPEED_PICOBENCH_COMPAT_HPP_INCLUDED
#define CODSPEED_PICOBENCH_COMPAT_HPP_INCLUDED

#include "picobench/picobench.hpp"

// Undefine the original PICOBENCH macro
// #undef PICOBENCH

// Define a new PICOBENCH macro that wraps the original functionality
#define PICOBENCH2(func) \
    do { \
        std::cout << "Registering benchmark for " << #func << std::endl; \
        static auto& I_PICOBENCH_PP_CAT(picobench, PICOBENCH_UNIQUE_SYM_SUFFIX) = \
            PICOBENCH_NAMESPACE::global_registry::new_benchmark(#func, func); \
    } while (0)

#endif // CODSPEED_PICOBENCH_COMPAT_HPP_INCLUDED
