import sys
sys.path.insert(0, '../../build/lib')

import numpy as np
import py_aff3ct as aff3ct
import math
from py_loop_counter import loop_counter

K = 10
src = aff3ct.module.source.Source_random(K, 12)
swi = aff3ct.module.switcher.Switcher(2,K,np.int32)
cnt = loop_counter(5)

swi["select::data1"].bind(src["generate::U_K  "])
swi["select::data0"].bind(swi["commute::data0 "])
swi["commute::data"].bind(swi["select::data   "])
cnt("increment")    .bind(swi["select::status "])
swi["commute::ctrl"].bind(cnt["increment::ctrl"])


seq = aff3ct.tools.sequence.Sequence(src("generate"), swi("commute"))
swi("commute").debug = True
seq.export_dot("test.dot")
seq.exec()
