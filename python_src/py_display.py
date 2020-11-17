#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import numpy as np
import matplotlib.pyplot as plt
from py_aff3ct.module.py_module import Py_Module

class Display(Py_Module):
	def plot(self, x):
		if  self.i_plt % 50 == 0:
			self.line.set_data(x[::2], x[1::2])
			self.fig.canvas.draw()
			self.fig.canvas.flush_events()
			plt.pause(0.000000000001)

		self.i_plt = self.i_plt + 1
		return 0

	def __init__(self, N):
		Py_Module.__init__(self)
		t_plot = self.create_task("plot")
		self.create_socket_in(t_plot, "x", N, np.float32)
		self.create_codelet  (t_plot, lambda m,t,f: m.plot(t.sockets[0]))

		self.fig   = plt.figure()
		self.ax    = self.fig.add_subplot(1, 1, 1)
		self.line, = self.ax.plot([], '.b')
		self.i_plt = 0
		plt.xlabel("Real part")
		plt.ylabel("Imaginary part")
		plt.ylim(-2,2)
		plt.xlim(-2,2)

	def codelet(self, task, py_code):
		np_args = map(lambda arg:np.array(arg, copy = False), task.sockets)
		return py_code(*np_args)