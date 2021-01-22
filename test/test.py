#!/usr/bin/env python3

import sys
sys.path.insert(0, '../build/lib')

from math import sqrt
from py_aff3ct.module.py_module import Py_Module
import numpy as np

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


m = Modulator(16)
m.tasks[0].debug = True
sh = np.shape(m["modulate::b"][:])
b = np.random.randint(0,2,sh, np.int32)
m["modulate::b"].bind(b)
m("modulate").exec()