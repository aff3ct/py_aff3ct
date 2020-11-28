import sys
sys.path.insert(0, '../build')

import numpy as np
import py_aff3ct as aff3ct
import math
import time
import matplotlib.pyplot as plt
from datetime import timedelta

K = 64
N = 128
ebn0_min = 0
ebn0_max = 10.6
ebn0_step = 0.5

ebn0 = np.arange(ebn0_min,ebn0_max,ebn0_step)
esn0 = ebn0 + 10 * math.log10(K/N)
sigma_vals = 1/(math.sqrt(2) * 10 ** (esn0 / 20))

src = aff3ct.module.source.Source_random_fast(K, 12)
enc = aff3ct.module.encoder.Encoder_repetition_sys(K, N)
mdm = aff3ct.module.modem.Modem_BPSK_fast(N)

gen = aff3ct.tools.Gaussian_noise_generator_implem.FAST
chn = aff3ct.module.channel.Channel_AWGN_LLR(N, gen)

dec = aff3ct.module.decoder.Decoder_repetition_fast(K, N)
mnt = aff3ct.module.monitor.Monitor_BFER(K, 1000)

sigma = np.ndarray(shape = (1,1),  dtype = np.float32)
enc["encode       ::U_K "].bind(src["generate   ::U_K "])
mdm["modulate     ::X_N1"].bind(enc["encode     ::X_N "])
chn["add_noise    ::X_N "].bind(mdm["modulate   ::X_N2"])
mdm["demodulate   ::Y_N1"].bind(chn["add_noise  ::Y_N "])
dec["decode_siho  ::Y_N "].bind(mdm["demodulate ::Y_N2"])
mnt["check_errors2::U   "].bind(src["generate   ::U_K "])
mnt["check_errors2::V   "].bind(dec["decode_siho::V_K "])
chn["add_noise    ::CP  "].bind(                 sigma  )
mdm["demodulate   ::CP  "].bind(                 sigma  )

#src("generate"     ).debug = True
#enc("encode"       ).debug = True
#mdm("modulate"     ).debug = True
#chn("add_noise"    ).debug = True
#mdm("demodulate"   ).debug = True
#dec("decode_siho"  ).debug = True
#mnt("check_errors2").debug = True

seq  = aff3ct.tools.sequence.Sequence(src("generate"), mnt("check_errors2"), 4)
seq_mnts = seq.get_BFER_monitors()
mnt_red = aff3ct.module.monitor.Monitor_reduction_BFER(seq_mnts)
mnt_red.set_reduce_frequency(timedelta(microseconds=100))
# print(v)
#seq.set_n_frames(1)

fer = np.zeros(len(ebn0))
ber = np.zeros(len(ebn0))

fig = plt.figure()
ax = fig.add_subplot(111)
line1,line2, = ax.semilogy(ebn0, fer, 'r-', ebn0, ber, 'b--') # Returns a tuple of line objects, thus the comma
plt.ylim((1e-6, 1))

print("Eb/NO | FRA | BER | FER | Tpt ")
for i in range(len(sigma_vals)):
	sigma[:] = sigma_vals[i]

	t = time.time()
	seq.exec(lambda: mnt_red.is_done()) # GIL, mnt_red -> slow
	# seq.exec(lambda: all([s.is_done() for s in seq_mnts])) # GIL, no mnt_red -> slow
	# seq.exec_auto() # no GIL -> fast
	elapsed = time.time() - t

	# Working
	total_fra = 0.
	total_fe  = 0.
	total_be  = 0.
	for m in seq_mnts:
		total_fra += m.get_n_analyzed_fra()
		total_fe  += m.get_n_fe()
		total_be  += m.get_n_be()

	# Not working, use mnt reduce
	#mnt_red.reduce(True)
	#total_fra = mnt_red.get_n_analyzed_fra()
	#total_fe  = mnt_red.get_n_fe()
	#total_be  = mnt_red.get_n_be()

	ber[i] = total_be / (total_fra*K)
	fer[i] = total_fe / total_fra

	tpt = total_fra * K * 1e-6/elapsed
	print( ebn0[i] , "|",  total_fra, "|", ber[i] ,"|", fer[i] , "|", tpt)

	for m in seq_mnts:
		m.reset()
	# mnt_red.reset() # seems to work

	line1.set_ydata(fer)
	line2.set_ydata(ber)
	fig.canvas.draw()
	fig.canvas.flush_events()
	plt.pause(1e-6)

seq.show_stats()
plt.show()

