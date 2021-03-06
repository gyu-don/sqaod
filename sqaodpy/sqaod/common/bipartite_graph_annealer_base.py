from __future__ import print_function
import numpy as np
from . import checkers
from . import preference as pref
from . import common


class BipartiteGraphAnnealerBase :
    
    def __init__(self, cext, dtype, b0, b1, W, optimize, prefdict) :
        self._cext = cext
        self.dtype = dtype
        if not W is None :
            self.set_qubo(b0, b1, W, optimize)
        self.set_preferences(prefdict)

    def __del__(self) :
        if hasattr(self, '_cobj') :
            self._cext.delete(self._cobj, self.dtype)
        
    def seed(self, seed) :
        self._cext.seed(self._cobj, seed, self.dtype)
            
    def set_qubo(self, b0, b1, W, optimize = pref.minimize) :
        checkers.bipartite_graph.qubo(b0, b1, W)
        b0, b1, W = common.clone_as_ndarray_from_vars([b0, b1, W], self.dtype)
        self._cext.set_qubo(self._cobj, b0, b1, W, optimize, self.dtype);
        self._optimize = optimize

    def get_optimize_dir(self) :
        return self._optimize

    def get_problem_size(self) :
        return self._cext.get_problem_size(self._cobj, self.dtype)

    def set_preferences(self, prefdict=None, **prefs) :
        if not prefdict is None :
            self._cext.set_preferences(self._cobj, prefdict, self.dtype)
        self._cext.set_preferences(self._cobj, prefs, self.dtype)

    def get_preferences(self) :
        return self._cext.get_preferences(self._cobj, self.dtype)
        
    def get_E(self) :
        return self._cext.get_E(self._cobj, self.dtype)

    def get_x(self) :
        return self._cext.get_x(self._cobj, self.dtype)

    def set_x(self, x0, x1) :
        self._cext.set_x(self._cobj, x0, x1, self.dtype)

    # Ising model / spins
    
    def get_hamiltonian(self) :
        N0, N1 = self.get_problem_size()
        h0 = np.ndarray((N0), self.dtype);
        h1 = np.ndarray((N1), self.dtype);
        J = np.ndarray((N1, N0), self.dtype);
        c = np.ndarray((1), self.dtype)
        self._cext.get_hamiltonian(self._cobj, h0, h1, J, c, self.dtype)
        return h0, h1, J, c[0]
            
    def get_q(self) :
        return self._cext.get_q(self._cobj, self.dtype)

    def randomize_spin(self) :
        self._cext.randomize_spin(self._cobj, self.dtype)

    def calculate_E(self) :
        self._cext.calculate_E(self._cobj, self.dtype)

    def prepare(self) :
        self._cext.prepare(self._cobj, self.dtype)

    def make_solution(self) :
        self._cext.make_solution(self._cobj, self.dtype)
        
    def anneal_one_step(self, G, beta) :
        self._cext.anneal_one_step(self._cobj, G, beta, self.dtype)
