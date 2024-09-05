import pickle

import sympy as sp

a, b = sp.symbols("a b")
r, z = sp.symbols("r z")

eq_1 = 100 * sp.cos(a) + 102.2 * sp.cos(b) - r
eq_2 = 100 * sp.sin(a) - 102.2 * sp.sin(b) - z

rst = sp.solve((eq_1, eq_2), a, b)
with open('tempsolve', "wb") as w:
    pickle.dump(rst, w)
