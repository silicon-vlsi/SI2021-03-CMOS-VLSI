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

data = CppSimData('test.tr0')

#sig_names = data.lssig()
sig_names = data.lssig('print')
#num_samples = data.get_num_samples()
#num_sigs = data.get_num_sigs()
#filename = data.get_filename()
#print 'file: ',filename, 'num_samples: ',num_samples, 'num_sigs: ', num_sigs

t = data.evalsig('TIME')
vin = data.evalsig('vin')
sd_in = data.evalsig('sd_in')

fig = figure(1)
fig.clf()
subplot(2,1,1)
plot(t,vin,'r-')
grid(True)
subplot(2,1,2)
plot(t,sd_in,'b-')
grid(True)
fig.show()
