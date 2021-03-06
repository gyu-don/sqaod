#include <sqaodc/pyglue/pyglue.h>

namespace {

template<class real>
using Annealer = sq::cuda::BipartiteGraphAnnealer<real>;

}

#define modname "cuda_bg_annealer"
#define INIT_MODULE INITFUNCNAME(cuda_bg_annealer)
#define BIPARTITE_GRAPH
#define CUDA_SOLVER

#include <sqaodc/pyglue/annealer.inc>
