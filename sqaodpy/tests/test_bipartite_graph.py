from __future__ import print_function
import unittest
import numpy as np
import sqaod.py as py
import sqaod.common as common


class TestMinEnergy(unittest.TestCase):

    def test_min_energy(self):
        N0 = 200
        N1 = 350
        an = py.bipartite_graph_annealer()
        W = np.random.random((N1, N0)) - 0.5
        b0 = np.random.random((N0)) - 0.5
        b1 = np.random.random((N1)) - 0.5
        an.set_qubo(b0, b1, W)
        an.set_preferences(n_trotters = 1)
        an.prepare()

        x0 = np.ndarray((N0), np.int8)
        x1 = np.ndarray((N1), np.int8)
        x0[:] = 0
        x1[:] = 0
        an.set_x(x0, x1)
        an.calculate_E()
        self.assertTrue(np.allclose(an.get_E(), 0.))
        # print(an.E)


    def test_qubo_energy(self):
        N0 = 8
        N1 = 5
        an = py.bipartite_graph_annealer()
        W = np.random.random((N1, N0)) - 0.5
        b0 = np.random.random((N0)) - 0.5
        b1 = np.random.random((N1)) - 0.5
        an.set_qubo(b0, b1, W)
        an.set_preferences(n_trotters = 1)
        an.prepare()

        iMax = 1 << N0
        jMax = 1 << N1

        for i in range(iMax) :
            x0 = common.create_bitset_sequence((i,), N0)
            for j in range(jMax) :
                x1 = common.create_bitset_sequence((j,), N1)
                Ebf = np.dot(b0, x0.transpose()) + np.dot(b1, x1.transpose()) \
                      + np.dot(x1, np.matmul(W, x0.transpose()))
                an.set_x(x0, x1)
                an.calculate_E()
                Ean = an.get_E()
                if not np.allclose(Ebf, Ean) :
                    print(i, j, Ebf, Ean)
                self.assertTrue(np.allclose(Ebf,  Ean))

    def test_bf_searcher(self):
        N0 = 4
        N1 = 4

        an = py.bipartite_graph_annealer()
        bf = py.bipartite_graph_bf_searcher()
        
        W = np.random.random((N1, N0)) - 0.5
        b0 = np.random.random((N0)) - 0.5
        b1 = np.random.random((N1)) - 0.5

        an.set_qubo(b0, b1, W)
        bf.set_qubo(b0, b1, W)
        
        bf.search();
        common.anneal(an)

        # Assuming problems with small N0 and N1 give the same results
        # for annealer and brute-force solver.
        Ebf = bf.get_E()[0]
        bfx0, bfx1 = bf.get_x()[0]
        Ean = an.get_E()[0]
        anx0, anx1 = an.get_x()[0]
        if not np.allclose(bfx0, anx0) or not np.allclose(bfx1, anx1) :
            print(bfx0, anx0, an.get_E())
            print(bfx1, anx1, bf.get_E())
        self.assertTrue(np.allclose(bfx0, anx0))
        self.assertTrue(np.allclose(bfx1, anx1))

        
if __name__ == '__main__':
    np.random.seed(0)
    unittest.main()
