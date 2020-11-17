import numpy as np
from math import sqrt
from py_aff3ct.module.py_module import Py_Module

class Modulator(Py_Module):
	def modulate(self, b, x):
		x[:] = 1/sqrt(2) - sqrt(2) * b[:]
		return 0

	def __init__(self, N):
		Py_Module.__init__(self)
		self.name = "py_Modem"
		t_mod = self.create_task("modulate")
		sb = self.create_socket_in (t_mod, "b", N, np.int32  )
		sx = self.create_socket_out(t_mod, "x", N, np.float32)
		self.create_codelet(t_mod, lambda m,l,f: m.modulate(l[0], l[1]))
