# -*- coding: utf-8 -*-
from __future__ import division

# import cppsimdata module
import os
import sys

cppsimsharedhome = os.getenv("CPPSIMSHAREDHOME")
if cppsimsharedhome != None:
   CPPSIMSHARED_PATH = '%s' % cppsimsharedhome
else:
   home_dir = os.getenv("HOME")
   CPPSIMSHARED_PATH = '%s/CppSim/CppSimShared' % home_dir
sys.path.append(CPPSIMSHARED_PATH + '/Python')
from cppsimdata import *

# import pylab package
from pylab import *

data = CppSimData('test_noise.tr0')
noiseout = data.evalsig('noiseout')
t = data.evalsig('TIME')
Ts = t[20]-t[19]
f_low = 10e3
f_high = 30e6
f, Pxx_db = calc_pll_phasenoise(noiseout,Ts)

fig = figure(1)
fig.clf()
semilogx(f,Pxx_db)
axis([f_low, f_high, min(Pxx_db), max(Pxx_db)])
xlabel('Frequency Offset from Carrier (Hz)')
ylabel('L(f) (dBc/Hz)')
title('Simulated Phase Noise')
grid(True,which='both')
fig.show()

