import sys
sys.path.insert(0, '../../build/lib')

import numpy as np
from py_aff3ct.module.py_module import Py_Module


# Step 1
class loop_counter(Py_Module):

    # Step 2
    def count(self, ctrl): # Your original code (independently from aff3ct)
        if self.cnt > self.loop_max-2:
            self.cnt = 0
            ctrl[0,:] = 1
        else:
            self.cnt += 1
            ctrl[0,:] = 0
        return 0

    # Step 3
    def __init__(self, loop_max):
        Py_Module.__init__(self) # Call the aff3ct Py_Module __init__
        self.name = "loop_cnt"   # Set your module's name
        self.loop_max = loop_max
        self.cnt      = 0
        tsk = self.create_task("increment") # create a task for your module

        self.create_socket_out(tsk, "ctrl", 1, np.int8  ) # create an input socket for the task t_mod

        self.create_codelet(tsk, lambda slf, lsk, fid: slf.count(lsk[0])) # create codelet