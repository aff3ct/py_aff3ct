#!/usr/bin/env python3

import sys
sys.path.insert(0, '../build/lib')

import numpy as np
import py_aff3ct as aff3ct
import math
import time
import matplotlib.pyplot as plt
from datetime import timedelta

K = 512
N = 1024
ebn0_min = 0
ebn0_max = 4.0
ebn0_step = 0.25

ebn0 = np.arange(ebn0_min,ebn0_max,ebn0_step)
esn0 = ebn0 + 10 * math.log10(K/N)
sigma_vals = 1/(math.sqrt(2) * 10 ** (esn0 / 20))


fbgen = aff3ct.tools.frozenbits_generator.Frozenbits_generator_GA_Arikan(K, N)
noise = aff3ct.tools.noise.Sigma(sigma_vals[0])
fbgen.set_noise(noise)
frozen_bits = fbgen.generate()

src = aff3ct.module.source.Source_random_fast(K, 12)
enc = aff3ct.module.encoder.Encoder_polar_sys(K,N,frozen_bits)
dec = aff3ct.module.decoder.Decoder_polar_SC_fast_sys(K,N,frozen_bits)
mdm = aff3ct.module.modem.Modem_BPSK_fast(N)
gen = aff3ct.tools.Gaussian_noise_generator_implem.FAST
chn = aff3ct.module.channel.Channel_AWGN_LLR(N, gen)
mnt = aff3ct.module.monitor.Monitor_BFER_AR(K, 1000)

sigma = np.ndarray(shape = (1,1),  dtype = np.float32)
enc["encode       ::U_K "].bind(src["generate   ::U_K "])
mdm["modulate     ::X_N1"].bind(enc["encode     ::X_N "])
chn["add_noise    ::X_N "].bind(mdm["modulate   ::X_N2"])
mdm["demodulate   ::Y_N1"].bind(chn["add_noise  ::Y_N "])
dec["decode_siho  ::Y_N "].bind(mdm["demodulate ::Y_N2"])
mnt["check_errors ::U   "].bind(src["generate   ::U_K "])
mnt["check_errors ::V   "].bind(dec["decode_siho::V_K "])
chn["add_noise    ::CP  "].bind(                 sigma  )
mdm["demodulate   ::CP  "].bind(                 sigma  )

#src("generate"    ).stats = True
#enc("encode"      ).stats = True
#mdm("modulate"    ).stats = True
#chn("add_noise"   ).stats = True
#mdm("demodulate"  ).stats = True
#dec("decode_siho" ).stats = True
#mnt("check_errors").stats = True

seq  = aff3ct.tools.sequence.Sequence(src("generate"), mnt("check_errors"), 4)

fer = np.zeros(len(ebn0))
ber = np.zeros(len(ebn0))

fig = plt.figure()
ax = fig.add_subplot(111)
line1,line2, = ax.semilogy(ebn0, fer, 'r-', ebn0, ber, 'b--') # Returns a tuple of line objects, thus the comma
plt.ylim((1e-6, 1))

print("Eb/NO | FRA | BER | FER | Tpt ")
for i in range(len(sigma_vals)):
	sigma[:] = sigma_vals[i]
	noise = aff3ct.tools.noise.Sigma(sigma_vals[i])
	fbgen.set_noise(noise)
	frozen_bits = fbgen.generate()
	enc.set_frozen_bits(frozen_bits)
	dec.set_frozen_bits(frozen_bits)

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

