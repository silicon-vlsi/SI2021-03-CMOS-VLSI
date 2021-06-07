#!/usr/bin/env python

#Import required python packages
import sys
import os
import numpy as np
# import pylab package
from pylab import *

ngspiceRawFile = 'vgs-id-temp.raw'

cppsimsharedhome = os.getenv("CPPSIMSHAREDHOME")
sys.path.append(cppsimsharedhome + '/Python')
## Import the Python package for ngspice 
from ngspicedata import *


##FUNCTIONS
def slope(x, y, x1, x2):
    """calculates dy/dx: y(x2)-y(x1)/x1-x2"""
    index = np.where(x >= x1)[0][0]
    y1 = y[index]
    index = np.where(x >= x2)[0][0]
    y2 = y[index]

    return y1,y2,(y2-y1)/(x2-x1)

def vt_kp(v, Id, v1, v2, woverl):
    """Calculates Vt & Kp"""
    rtId = np.sqrt(2*Id)
    rtId1,rtId2,m = slope(v, rtId, v1, v2)
    c = rtId1 - m*v1
    vt = -c/m
    ##Kp Calculation
    #FIXME: calculate Kp
    kp = 0.0 

    return vt,kp

# Read the RAW ngspice data
data = NgspiceData(ngspiceRawFile)
## Listing all the available signals
print data.lssig()
## Reading the signal into variable 
id1 = data.evalsig('i_vid1')
v = data.evalsig('VOLTAGE')

## Calculate Vt
##FIXME Provide the v1,v2 and W/L 
v1=0.0
v2=0.0
woverl=0.0
vt,kp = vt_kp(v, id1, v1, v2, woverl)
print "Vt=%3.2f V,  Kp=%3.2f A/V^2" % (vt,kp)

# Plot the figure
fig = figure(1)
fig.clf()
rtId = np.sqrt(2*id1)
plot(v, rtId*1e3)
xlabel('Vgs(V)')
ylabel('Ids(mA)')
title('Vgs vs Ids')
grid(True,which='both')
#fig.show()

#input("Press any key to quite")
