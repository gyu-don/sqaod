#include <sqaodc/pyglue/pyglue.h>

namespace {

template<class real>
using DGFormulas = sq::cuda::DenseGraphFormulas<real>;

template<class real>
using BGFormulas = sq::cuda::BipartiteGraphFormulas<real>;

}

#define modname "cuda_formulas"
#define INIT_MODULE INITFUNCNAME(cuda_formulas)

#include <sqaodc/pyglue/formulas.inc>
