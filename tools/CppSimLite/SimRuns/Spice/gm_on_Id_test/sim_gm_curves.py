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

# choose w_nmos values for parameter sweep
w_nmos_vals = arange(1,6,1)*1e-6

# choose hspc sim file to modify
hspc_filename = 'test.hspc'

# create figure or clear existing one
fig = figure(1)
fig.clf()
color_vals = ['b','g','r','m','k']

# perform multiple ngspice simulations and plot results
count = 0
for w_nmos in w_nmos_vals:
     print 'w_nmos: ', w_nmos
     hspc_set_param('w_nmos',w_nmos,hspc_filename)
     # when adding lines, must always start with 'hspc_addline'
     # followed by 'hspc_addline_continued' statements
     hspc_addline('.param new_param = 1',hspc_filename)
     hspc_addline_continued('.param new_param2 = 10',hspc_filename)
     hspc_addline_continued('.param new_param3 = 15',hspc_filename)
     ngsim(hspc_filename)

     data = NgspiceData('simrun.raw')

     current = data.evalsig('CURRENT')*1e6;
     m1_gm = data.evalsig('m1_gm');
     plot(current,m1_gm,color_vals[count])
     count = count + 1

# add grid and label plot axis
grid(True)
xlabel('Current (uA)')
ylabel('gm of M1')
fig.show()


