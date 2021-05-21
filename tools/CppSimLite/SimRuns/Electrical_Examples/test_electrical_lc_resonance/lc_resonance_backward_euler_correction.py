from __future__ import division

# import cppsimdata module
import os
import sys

# import pylab package
from pylab import *

# Time step of CppSim simulation
Ts = 1/200e9

l_val = 4e-9
c_val = 1.583e-12
r_val = 10

fres = 1/(2*pi*sqrt(l_val*c_val))

fmin = fres/2
fmax = fres*2
f = linspace(fmin,fmax,100e3)
s = 1j*2*pi*f

# Calculation of mapping from Backward Euler to S-domain
zm = exp(1j*2*pi*f*Ts)
s_be = 1/Ts*(1 - zm**(-1))

z_res = s*l_val + r_val + 1/(s*c_val)
z_res_be = s_be*l_val + r_val + 1/(s_be*c_val) 

zcor = exp(1j*2*pi*fres*Ts)
s_cor = 1/Ts*(1 - zcor**(-1))
r_cor = -real(s_cor*l_val + 1/(s_cor*c_val))
print("fres = %5.3e,  r_cor = %5.3e" % (fres,r_cor))

z_res_be_cor = s_be*l_val + r_val + r_cor + 1/(s_be*c_val) 

fig = figure(1,figsize=(14,8))
fig.clf()
rcParams['lines.linewidth'] = 2

subplot(211)   
plot(f,-20*log10(abs(z_res)),'k',label='Actual')
plot(f,-20*log10(abs(z_res_be)),'r',label='Backward Euler')
plot(f,-20*log10(abs(z_res_be_cor)),'b--',label='Corrected Backward Euler')

title('Magnitude of Drive Admittance')
ylabel('dB')
grid(True,which='both')
legend(loc='upper right',prop={'size':10})
#axis([fmin, fmax, 0, 80])

subplot(212)
plot(f,180/pi*np.unwrap(np.angle(z_res)),'k',label='Actual')
plot(f,180/pi*np.unwrap(np.angle(z_res_be)),'r',label='Backward Euler')
plot(f,180/pi*np.unwrap(np.angle(z_res_be_cor)),'b--',label='Corrected Backward Euler')
title('Phase of Drive Impedance');
ylabel('Degrees')
xlabel('Frequency (Hz)')
grid(True,which='both')
legend(loc='lower right',prop={'size':10})
axis([fmin, fmax, -100, 100])

fig.show()

