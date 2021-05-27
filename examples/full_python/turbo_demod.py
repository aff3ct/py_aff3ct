#!/usr/bin/env python3
import sys
sys.path.insert(0, '../../build/lib')
import math
import py_aff3ct as aff3ct
from py_loop_counter import loop_counter
import numpy as np
import time
import matplotlib.pyplot as plt

K = 64   # Message length
N = 132  # Codeword length
I = 0    # Turbo-iterations number
bps = 4  # Number of bits per symbols
Ns = N//bps
ebn0_min = 0
ebn0_max = 10.0
ebn0_step = 1.0

ebn0 = np.arange(ebn0_min,ebn0_max,ebn0_step)
esn0 = ebn0 + 10 * math.log10(K/Ns)
sigma_vals = 1/(math.sqrt(2) * 10 ** (esn0 / 20))

src = aff3ct.module.source.Source_random_fast(K, 12)
enc = aff3ct.module.encoder.Encoder_RSC_generic_sys(K,N)

cstl = aff3ct.tools.constellation.Constellation_user("../../lib/aff3ct/conf/mod/16QAM_ANTI_GRAY.mod") # Setup constellation tool
mdm  = aff3ct.module.modem.Modem_generic(N, cstl)

gen = aff3ct.tools.Gaussian_noise_generator_implem.FAST
chn = aff3ct.module.channel.Channel_AWGN_LLR(Ns*2, gen)

itl_core = aff3ct.tools.interleaver_core.Interleaver_core_random(N)
itl_bit  = aff3ct.module.interleaver.Interleaver_int32_t(itl_core)
itl_llr  = aff3ct.module.interleaver.Interleaver_float(itl_core)

trellis = enc.get_trellis()
dec = aff3ct.module.decoder.Decoder_RSC_BCJR_seq_generic_std(K,trellis)
mnt = aff3ct.module.monitor.Monitor_BFER_AR(K, 1000)
sigma = np.ndarray(shape = (1,1),  dtype = np.float32)
swi = aff3ct.module.switcher.Switcher(2, N, np.float32)
Z = np.zeros(swi["select::data1"][:].shape, np.float32)
cnt = loop_counter(I)


enc    ["encode       ::U_K  "].bind(src    ["generate    ::U_K   "])
itl_bit["interleave   ::nat  "].bind(enc    ["encode      ::X_N   "])
mdm    ["modulate     ::X_N1 "].bind(itl_bit["interleave  ::itl   "])
chn    ["add_noise    ::X_N  "].bind(mdm    ["modulate    ::X_N2  "])
chn    ["add_noise    ::CP   "].bind(                       sigma   )
mdm    ["tdemodulate  ::Y_N1 "].bind(chn    ["add_noise   ::Y_N   "])
mdm    ["tdemodulate  ::Y_N2 "].bind(swi    ["select      ::data  "])
mdm    ["tdemodulate  ::CP   "].bind(                       sigma   )
swi    ["select       ::data1"].bind(                       Z       )
itl_llr["deinterleave ::itl  "].bind(mdm    ["tdemodulate ::Y_N3  "])
swi    ["commute      ::data "].bind(itl_llr["deinterleave::nat   "])
dec    ["decode_siso  ::Y_N1 "].bind(swi    ["commute     ::data0 "])
itl_llr["interleave   ::nat  "].bind(dec    ["decode_siso ::Y_N2  "])
swi    ["select       ::data0"].bind(itl_llr["interleave  ::itl   "])
swi    ["commute      ::ctrl "].bind(cnt    ["increment   ::ctrl  "])
cnt    ("increment"           ).bind(swi    ["select      ::status"])
dec    ["decode_siho  ::Y_N  "].bind(swi    ["commute     ::data1 "])
mnt    ["check_errors ::U    "].bind(src    ["generate    ::U_K   "])
mnt    ["check_errors ::V    "].bind(dec    ["decode_siho ::V_K   "])

#dec("decode_siso").debug = True
#dec("decode_siho").debug = True

seq  = aff3ct.tools.sequence.Sequence([src("generate"), swi("select")],8)
seq.export_dot("test.dot")

l_tasks = seq.get_tasks_per_types()
for lt in l_tasks:
    for t in lt:
        t.stats = True
        #t.debug = True
        #t.set_debug_limit(1)


#dec("decode_siso").set_debug_limit(1)

#dec("decode_siho").set_debug_limit(1)

fer = np.zeros(len(ebn0))
ber = np.zeros(len(ebn0))

fig = plt.figure()
ax = fig.add_subplot(111)
line1,line2, = ax.semilogy(ebn0, fer, 'r-', ebn0, ber, 'b--') # Returns a tuple of line objects, thus the comma
plt.ylim((1e-6, 1))
plt.xlabel("Eb/N0")
plt.ylabel("BER/FER")
plt.grid()


print("Eb/NO | FRA | BER | FER | Tpt ")

for i in range(len(sigma_vals)):
	sigma[:] = sigma_vals[i]

	t = time.time()
	seq.exec()
	elapsed = time.time() - t
	total_fra = mnt.get_n_analyzed_fra()

	ber[i] = mnt.get_ber()
	fer[i] = mnt.get_fer()

	tpt = total_fra * K * 1e-6/elapsed
	print( ebn0[i] , "|",  total_fra, "|", ber[i] ,"|", fer[i] , "|", tpt)

	mnt.reset()

	line1.set_ydata(fer)
	line2.set_ydata(ber)
	fig.canvas.draw()
	fig.canvas.flush_events()
	plt.pause(1e-6)

seq.show_stats()
plt.show()

