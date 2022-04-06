import sys
sys.path.insert(0, '../../build/lib')

import numpy as np
import signal
from py_aff3ct.module.py_module import Py_Module



class signal_catcher(Py_Module):

    def signal_handler(self, sig, frame):
        self.toggle_done()

    # Step 3
    def __init__(self):
        Py_Module.__init__(self) # Call the aff3ct Py_Module __init__

        self.name = "s_catcher"   # Set your module's name

        tsk = self.create_task("do_nothing") # create a task for your module

        self.create_fake_codelet(tsk) # create codelet

        signal.signal(signal.SIGINT, self.signal_handler)