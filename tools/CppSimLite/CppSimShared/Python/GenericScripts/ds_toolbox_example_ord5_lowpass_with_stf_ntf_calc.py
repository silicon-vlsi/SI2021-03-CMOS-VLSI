from __future__ import division
from pylab import *
import numpy as np
from IPython.core.display import Image
from deltasigma import *
from scipy.signal import zpk2tf,freqz
import warnings
import sys
warnings.filterwarnings('ignore')

np.set_printoptions(suppress=True, precision=3)

## This example is taken directly from Delta Sigma toolbox example for 5th order lowpass modulator:
## http://nbviewer.jupyter.org/github/ggventurini/python-deltasigma/blob/master/examples/dsexample1.ipynb
## with some extra additions such as plotting of STF and NTF

order = 5
osr = 32
nlev = 2
f0 = 0.
Hinf = 1.5
form = 'CRFB'

plot_stf = 0

ntf = synthesizeNTF(order, osr, 2, Hinf, f0)            # Optimized zero placement
# ntf_zeros = ntf[0], ntf_poles = ntf[1]
[b_raw,a_raw] = ntf_tf = zpk2tf(ntf[0],ntf[1],1)
b = real(b_raw)
a = real(a_raw)

fclk = 1e6
Tclk = 1/fclk
f_norm = linspace(0,0.5,fclk/10)
f = f_norm*fclk
s = 1j*2*pi*f
# look at frequencies from 20Hz to bw=20kHz
fstart = 20
fend = 20e3
i_start = argwhere(f >= fstart)[0]
i_end = argwhere(f >= fend)[0]
f_ind_bw = arange(i_start,i_end+1)
f_bw = f[f_ind_bw]

[w,ntf_freq_resp] = freqz(b,a,2*pi*f_norm)
noise_scale = 2*sqrt(2) # for 1,0 output of Delta-Sigma, normalize by this to have max 0dB signal
quant_var = 2/12 # single sided variance of quantization noise assuming 0,1 output
ntf_pxx = Tclk/(0.5*len(f_norm))*noise_scale**2*quant_var*abs(ntf_freq_resp)**2  # single-sided noise spectrum

ntf_pxx_bw = ntf_pxx[f_ind_bw]
noise_power = (fend-fstart)*sum(ntf_pxx_bw)
print("noise_power = %5.3e" % (noise_power))
noise_power_db = 10*log10(noise_power)

fig_ntf_pxx = figure(1,figsize=(12,6))
fig_ntf_pxx.clf()
rcParams['lines.linewidth'] = 2  # set line thickness in plots to 2

semilogx(f,10*log10(abs(ntf_pxx)),'m--',label='unweighted (full)')
semilogx(f_bw,10*log10(abs(ntf_pxx_bw)),'r',label='unweighted (bw): %5.1f dB SNR at 0dB signal' % noise_power_db)

xlabel("Normalized Frequency (Hz)")
ylabel("dB")
title("Single-Ended Noise Spectral Density Calculation (OSR = %5.1f)" % osr)
grid(True,which='both')
legend(loc='lower right',prop={'size':12})
fig_ntf_pxx.show()

print("Synthesized a %d-order NTF, with roots:\n" % order)
print(" Zeros:\t\t\t Poles:")
for z, p in zip(ntf[0], ntf[1]):
    print("(%f, %fj)\t(%f, %fj)" % (np.real(z), np.imag(z), np.real(p), np.imag(p)))
print("")
print("The NTF transfer function has the following expression:\n")
print(pretty_lti(ntf))
print("")
#sys.exit()

fig_pz = figure(2,figsize=(12,6))
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

if plot_stf == 1:
   ## Calculate and plot STF
   ntf_zpg,stf_zpg = calculateTF(ABCD)

   [b_stf,a_stf] = zpk2tf(stf_zpg[0],stf_zpg[1],stf_zpg[2])
   [w,stf_freq_resp] = freqz(b_stf,a_stf,2*pi*f_norm)

   fig_stf = figure(3,figsize=(12,6))
   fig_stf.clf()
   rcParams['lines.linewidth'] = 2  # set line thickness in plots to 2

   semilogx(w/(2*pi)*fclk,20*log10(abs(stf_freq_resp)),'m',label='STF')
   xlabel("Frequency (Hz)")
   ylabel("dB")
   title("STF Transfer Function Plot")
   grid(True,which='both')
   axis([20,fclk/2,-1.5,0.5])
   legend(loc='lower right',prop={'size':12})
   fig_stf.show()
   sys.exit()

#DocumentNTF(ABCD, osr, f0)
#f = gcf()
#f.set_size_inches((15, 6))

fig_spec = figure(3,figsize=(12,6))
fig_spec.clf()
rcParams['lines.linewidth'] = 2  # set line thickness in plots to 2

PlotExampleSpectrum(ntf, M=1, osr=osr, f0=f0)
fig_spec.show()

fig_snr = figure(4,figsize=(12,6))
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
figureMagic([-100, 0], 10, None, [0, 120], 10, None, [12, 6], 'Time-Domain Simulations')
legend(loc=2);
fig_snr.show()

ABCD0 = ABCD.copy()

# Option 1:  Utilize dynamic range scaling
print("Doing dynamic range scaling... ")
ABCD, umax, S = scaleABCD(ABCD0, nlev, f0)
# Option 2:  Do not utilize dynamict range scaling, but use scaleABCD to determine maximum input amplitude for stable operation
# print("Calculating max input amplitude...")
# ABCD_scratch, umax, S = scaleABCD(ABCD0, nlev, f0)

print("Done.")
print("Maximum input magnitude: %.3f" % umax)

print("Check dynamic range scaling... ")
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

fig_state = figure(5,figsize=(12,6))
fig_state.clf()  
rcParams['lines.linewidth'] = 2  # set line thickness in plots to 2

colors = get_cmap('jet')(np.linspace(0, 1.0, order))

hold(True)
for i in range(order):
    plot(u,maxima[i,:], 'o-', color=colors[i], label='State %d' % (i+1))
grid(True)
#text(umax/2, 0.05, 'DC input', horizontalalignment='center', verticalalignment='center')
figureMagic([0, umax], None, None, [0, 5] , 0.25, 2, [12, 6], 'State Maxima')
#title('State Maxima')
xlabel('DC input')
ylabel('Maxima')
legend(loc='best')
fig_state.show()

## Scaled coefficients

a_raw, g, b_raw, c = mapABCD(ABCD, form)
scale_val = 1.0/b_raw[0]
print('scale_val = %5.3e' % scale_val)
a = scale_val*a_raw
b = scale_val*b_raw
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
if len(a) == 4:
   t_datarow4 = ['#5','       '            ,'      ',             '{:.4f}'.format(b[4]),'      ']
else:
   t_datarow4 = ['#5','{:.4f}'.format(a[4]),'      ',             '{:.4f}'.format(b[4]),'{:.4f}'.format(c[4])]
   t_datarow5 = ['#6','       '            ,'      ',             '{:.4f}'.format(b[5]),'      ']

print('\t'.join(t_labelrow1))
print('\t'.join(t_labelrow2))
print('\t'.join(t_datarow0))
print('\t'.join(t_datarow1))
print('\t'.join(t_datarow2))
print('\t'.join(t_datarow3))
print('\t'.join(t_datarow4))
if len(a) == 5:
   print('\t'.join(t_datarow5))
