from __future__ import division
from pylab import *
import numpy as np
from IPython.core.display import Image
from deltasigma import *
import warnings
warnings.filterwarnings('ignore')

np.set_printoptions(suppress=True, precision=3)

## This example is taken directly from Delta Sigma toolbox example for 5th order lowpass modulator:
## http://nbviewer.jupyter.org/github/ggventurini/python-deltasigma/blob/master/examples/dsexample1.ipynb

order = 5
osr = 32
nlev = 2
f0 = 0.
Hinf = 1.5
form = 'CRFB'

ntf = synthesizeNTF(order, osr, 2, Hinf, f0)            # Optimized zero placement
print("Synthesized a %d-order NTF, with roots:\n" % order)
print(" Zeros:\t\t\t Poles:")
for z, p in zip(ntf[0], ntf[1]):
    print("(%f, %fj)\t(%f, %fj)" % (np.real(z), np.imag(z), np.real(p), np.imag(p)))
print("")
print("The NTF transfer function has the following expression:\n")
print(pretty_lti(ntf))
print("")

fig_pz = figure(1,figsize=(12,6))
fig_pz.clf()
rcParams['lines.linewidth'] = 2  # set line thickness in plots to 2

plotPZ(ntf, showlist=True)
fig_pz.show()

a, g, b, c = realizeNTF(ntf, form)
b = np.hstack(( # Use a single feed-in for the input
               np.atleast_1d(b[0]),
               np.zeros((b.shape[0] - 1, ))
             ))
ABCD = stuffABCD(a, g, b, c, form)
print("ABCD Matrix:")
print(ABCD)

#DocumentNTF(ABCD, osr, f0)
#f = gcf()
#f.set_size_inches((15, 6))

fig_spec = figure(2,figsize=(12,6))
fig_spec.clf()
rcParams['lines.linewidth'] = 2  # set line thickness in plots to 2

PlotExampleSpectrum(ntf, M=1, osr=osr, f0=f0)
fig_spec.show()

fig_snr = figure(3,figsize=(12,6))
fig_snr.clf()

snr, amp = simulateSNR(ntf, osr, None, f0, nlev)
if nlev == 2:
    snr_pred, amp_pred, k0, k1, se = predictSNR(ntf, osr)
    plot(amp_pred, snr_pred, '-', label='predicted')
    hold(True)
plot(amp, snr,'o-.g', label='simulated')
xlabel('Input Level (dBFS)')
ylabel('SQNR (dB)')
peak_snr, peak_amp = peakSNR(snr, amp)
msg = 'peak SQNR = %4.1fdB  \n@ amp = %4.1fdB  ' % (peak_snr, peak_amp)
text(peak_amp-10,peak_snr,msg, horizontalalignment='right', verticalalignment='center');
msg = 'OSR = %d ' % osr
text(-2, 5, msg, horizontalalignment='right');
hold(False)
figureMagic([-100, 0], 10, None, [0, 100], 10, None, [12, 6], 'Time-Domain Simulations')
legend(loc=2);
fig_snr.show()

print("Doing dynamic range scaling... ")
ABCD0 = ABCD.copy()
ABCD, umax, S = scaleABCD(ABCD0, nlev, f0)
print("Done.")
print("Maximum input magnitude: %.3f" % umax)

print("Verifying dynamic range scaling... ")
u = np.linspace(0, 0.95*umax, 30)
N = 1e4
N0 = 50
test_tone = np.cos(2*np.pi*f0*np.arange(N))
test_tone[:N0] = test_tone[:N0]*(0.5 - 0.5*np.cos(2*np.pi/N0*np.arange(N0)))
maxima = np.zeros((order, u.shape[0]))
for i in np.arange(u.shape[0]):
    ui = u[i]
    v, xn, xmax, y = simulateDSM(ui*test_tone, ABCD, nlev)
    maxima[:, i] = xmax[:, 0]
    if (xmax > 1e2).any(): 
        print("Warning, umax from scaleABCD was too high.")
        umax = ui
        u = u[:i]
        maxima = maxima[:, :i]
        break
print("Done.")
print("Maximum DC input level: %.3f" % umax)

fig_state = figure(4,figsize=(12,6))
fig_state.clf()  
rcParams['lines.linewidth'] = 2  # set line thickness in plots to 2

colors = get_cmap('jet')(np.linspace(0, 1.0, order))

hold(True)
for i in range(order):
    plot(u,maxima[i,:], 'o-', color=colors[i], label='State %d' % (i+1))
grid(True)
#text(umax/2, 0.05, 'DC input', horizontalalignment='center', verticalalignment='center')
figureMagic([0, umax], None, None, [0, 1] , 0.1, 2, [12, 6], 'State Maxima')
#title('State Maxima')
xlabel('DC input')
ylabel('Maxima')
legend(loc='best')
fig_state.show()

## Scaled coefficients

a, g, b, c = mapABCD(ABCD, form)
adc = {
       'order':order,
       'osr':osr,
       'nlev':nlev,
       'f0':f0,
       'ntf':ntf,
       'ABCD':ABCD,
       'umax':umax,
       'peak_snr':peak_snr,
       'form':form,
       'coefficients':{
                       'a':a,
                       'g':g,
                       'b':b,
                       'c':c
                      }
      }

t_labelrow1 = (['Coeff','DAC FB','Res FB','Feed-in','Interstage'])
t_labelrow2 = (['     ', 'a(n)', 'g(n)', 'b(n)', 'c(n)'])

t_datarow0 = ['#1','{:.4f}'.format(a[0]),'{:.4f}'.format(g[0]),'{:.4f}'.format(b[0]),'{:.4f}'.format(c[0])]
t_datarow1 = ['#2','{:.4f}'.format(a[1]),'{:.4f}'.format(g[1]),'{:.4f}'.format(b[1]),'{:.4f}'.format(c[1])]
t_datarow2 = ['#3','{:.4f}'.format(a[2]),'      ',             '{:.4f}'.format(b[2]),'{:.4f}'.format(c[2])]
t_datarow3 = ['#4','{:.4f}'.format(a[3]),'      ',             '{:.4f}'.format(b[3]),'{:.4f}'.format(c[3])]
t_datarow4 = ['#5','{:.4f}'.format(a[4]),'      ',             '{:.4f}'.format(b[4]),'{:.4f}'.format(c[4])]
t_datarow5 = ['#6','       '            ,'      ',             '{:.4f}'.format(b[5]),'      ']

print('\t'.join(t_labelrow1))
print('\t'.join(t_labelrow2))
print('\t'.join(t_datarow0))
print('\t'.join(t_datarow1))
print('\t'.join(t_datarow2))
print('\t'.join(t_datarow3))
print('\t'.join(t_datarow4))
print('\t'.join(t_datarow5))
