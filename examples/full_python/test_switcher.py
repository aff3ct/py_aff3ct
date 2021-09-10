import sys
sys.path.insert(0, '../../build/lib')

import numpy as np
import py_aff3ct as aff3ct
import math
from py_loop_counter import loop_counter

K = 10
src = aff3ct.module.source.Source_random(K, 12)
mdm = aff3ct.module.modem.Modem_BPSK_fast(K)
swi = aff3ct.module.switcher.Switcher(2,K,np.int32)
cnt = loop_counter(5)

swi["select::data1"] = src["generate::U_K  "]
swi["select::data0"] = swi["commute::data0 "]
swi["commute::data"] = swi["select::data   "]
cnt["increment" ]    = swi["select::status "]
swi["commute::ctrl"] = cnt["increment::ctrl"]
#swi["commute::data1 "] = mdm["modulate::X_N1"]

seq = aff3ct.tools.sequence.Sequence(src["generate"], 1)
seq.export_dot("test.dot")
nb = 0
#swi["commute" ].debug = True
#seq.exec()
while not seq.is_done():
    t = seq.exec_step()
    while  t :
        if t == src['generate']:
            nb += 1
        print("Stopped in", t.name, '\t ' , nb , 'times!')
        t = seq.exec_step()

