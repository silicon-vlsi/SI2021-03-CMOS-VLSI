from __future__ import division

# import ngspicedata module
import os
import sys
if sys.platform == 'darwin':
   home_dir = os.getenv("HOME")
   sys.path.append(home_dir + '/CppSim/CppSimShared/Python')
else:
   cppsimsharedhome = os.getenv("CPPSIMSHAREDHOME")
   sys.path.append(cppsimsharedhome + '/Python')
from ngspicedata import *

# import pylab package
from pylab import *


# choose hspc sim file to modify
hspc_filename = 'test.hspc'

color_vals = ['b','g','r','k']

##########################################################

### plot transfer function

# create figure for transfer function
fig = figure(1)
fig.clf()
rcParams['lines.linewidth'] = 2

hspc_addline('.ac dec 1e3 10 100e3',hspc_filename)
hspc_addline_continued('.probe out',hspc_filename)

hspc_set_param('r1_val',100e6,hspc_filename)
hspc_set_param('r2_val',100e3,hspc_filename)
hspc_set_param('r3_val',100e3,hspc_filename)

ngsim(hspc_filename)

data = NgspiceData('simrun.raw')

f = data.evalsig('FREQUENCY')
tf = data.evalsig('out')

semilogx(abs(f),20*log10(abs(tf)),color_vals[0],label='transfer func.')

xlabel('Frequency (Hz)')
ylabel('dB')
title('Transfer Function from Input to Output')
grid(True,which='both')
#legend(loc='lower center',prop={'size':12})
fig.show()

# create figure for noise curves
fig = figure(2)
fig.clf()
rcParams['lines.linewidth'] = 2

fig = figure(3)
fig.clf()
rcParams['lines.linewidth'] = 2

hspc_addline('.noise v(out) vin dec 1e3 10 100e3',hspc_filename)
hspc_addline_continued('.probe inoise_spectrum onoise_spectrum',hspc_filename)

# perform multiple ngspice simulations and plot results
noise_setting_list = range(0,4) # creates list of 0 through 3

count = 0
for noise_setting in noise_setting_list:
     if noise_setting == 0:
        plot_label = 'r1'
        hspc_set_param('r1_en',1,hspc_filename)
        hspc_set_param('r2_en',0,hspc_filename)
        hspc_set_param('r3_en',0,hspc_filename)
     elif noise_setting == 1:
        plot_label = 'r2'
        hspc_set_param('r1_en',0,hspc_filename)
        hspc_set_param('r2_en',1,hspc_filename)
        hspc_set_param('r3_en',0,hspc_filename)
     elif noise_setting == 2:
        plot_label = 'r3'
        hspc_set_param('r1_en',0,hspc_filename)
        hspc_set_param('r2_en',0,hspc_filename)
        hspc_set_param('r3_en',1,hspc_filename)
     elif noise_setting == 3:
        plot_label = 'overall'
        hspc_set_param('r1_en',1,hspc_filename)
        hspc_set_param('r2_en',1,hspc_filename)
        hspc_set_param('r3_en',1,hspc_filename)

     ngsim(hspc_filename)

     data = NgspiceData('simrun.raw')

     #f = data.evalsig('FREQUENCY')
     
     f = data.evalsig('FREQUENCY')
     s_out = data.evalsig('onoise_spectrum')
     
     ##### A-weight Calculations - https://en.wikipedia.org/wiki/A-weighting
     s = 1j*2*pi*f;
     Ha_num = 7.39705e9*s**4
     Ha_den = (s+129.4)**2*(s+676.7)*(s+4636)*(s+76655)**2
     Ha = Ha_num/Ha_den
     s_out_aweight = s_out*abs(Ha)**2
     
     fig = figure(2)
     loglog(f,sqrt(s_out),color_vals[count],label=plot_label)
     fig = figure(3)     
     loglog(f,sqrt(s_out_aweight),color_vals[count],label=plot_label)
     
     count = count + 1

# add grid and label plot axis
fig = figure(2)
xlabel('Frequency (Hz)')
ylabel('V/rHz')
title('Raw Noise Spectral Density at Output')
grid(True,which='both')
legend(loc='lower center',prop={'size':12})
fig.show()

fig = figure(3)
xlabel('Frequency (Hz)')
ylabel('V/rHz')
title('A-Weighted Noise Spectral Density at Output')
grid(True,which='both')
legend(loc='lower center',prop={'size':12})
fig.show()
#sys.exit()




