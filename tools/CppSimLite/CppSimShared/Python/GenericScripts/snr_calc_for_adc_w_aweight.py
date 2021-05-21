from __future__ import division

import numpy as np
from pylab import *
import csv
from collections import defaultdict
from scipy.signal import lfilter,welch,hann
from scipy import fft
import sys

import os
cppsimsharedhome = os.getenv("CPPSIMSHAREDHOME")
if cppsimsharedhome != None:
   CPPSIMSHARED_PATH = '%s' % cppsimsharedhome
else:
   HOME = os.getenv("HOME")
   CPPSIMSHARED_PATH = '%s/CppSim/CppSimShared' % HOME

sys.path.append(CPPSIMSHARED_PATH + '/Python')
from cppsimdata import *

def calc_win_fft(sd_out,fs):
   N = len(sd_out)
   hwin = hann(N)

   # assume sd_out is Delta-Sigma sequence alternating between 0 and 1
   sd_out_win = sd_out*hwin
   # remove low frequency spectral bleeding
   sd_out_minus_mean = sd_out - mean(sd_out_win)/mean(hwin)
   sd_out_win = sd_out_minus_mean*hwin

   s_out = (4/sum(hwin)*absolute(fft(sd_out_win,N)))**2
   freq = arange(N)*1/N*fs

   ##### A-weight Calculations - https://en.wikipedia.org/wiki/A-weighting
   s = 1j*2*pi*freq;
   Ha_num = 7.39705e9*s**4
   Ha_den = (s+129.4)**2*(s+676.7)*(s+4636)*(s+76655)**2
   Ha = Ha_num/Ha_den
 
   s_out_aweight = s_out*abs(Ha)**2

   return freq,s_out,s_out_aweight

def calc_sig_noise_over_freq_range(s_out,freq,fstart,fend): 
   i_start = argwhere(freq >= fstart)[0]
   i_end = argwhere(freq >= fend)[0]
   f_ind_bw = arange(i_start,i_end+1)

   freq_bw = freq[f_ind_bw]
   s_out_bw = s_out[f_ind_bw]

   s_ind_max = argmax(s_out_bw)
   num_bw_samples = len(s_out_bw)
   fs = freq[-1]
   print("    BW for SNR calc: %3.1fHz to %3.1fkHz for fs = %5.3fMHz (%d samples)" % (fstart,fend*1e-3,fs*1e-6,num_bw_samples)) 

   # calculate indices corresponding to signal (assume 3 bins)
   signal_indices = arange(-1,1+1) + s_ind_max

   # computation of noise power is slightly off since the values in the 3 signal bins
   # are assumed to be 0
   # -> need 'num_bw_samples' above to be reasonable large for this to be a small error   
   signal_power_raw = sum(s_out_bw[signal_indices])
   noise_power_raw = sum(s_out_bw) - signal_power_raw

   # scale signal and noise power by 2/3 due to Hann window
   signal_power = 2/3*signal_power_raw
   noise_power = 2/3*noise_power_raw

   return signal_power,noise_power,s_out_bw,freq_bw,signal_indices

close('all')

# look at frequencies from 20Hz to 20kHz
fstart = 20
fend = 20e3

fig_snr = figure(1,figsize=(14,8))
fig_snr.clf()
# set line thickness in plots to 2
rcParams['lines.linewidth'] = 2

dir_name = os.getcwd()
file_basename = 'test'
sig_name = 'out'

#file_suffix = ['tr0','tr1','tr2','tr3','tr4']
file_suffix = ['tr0']
for file_suffix_val in file_suffix:
   file_name= '%s/%s.%s' % (dir_name,file_basename,file_suffix_val)
   if os.path.isfile(file_name):
      print("... Opening file '%s'..." % file_name)
   else:
      print("... Stop:  file '%s' does not exist..." % file_name)
      sys.exit()

   data = CppSimData(file_name)
   sd_out = data.evalsig(sig_name) 
   t = data.evalsig('TIME')
   t_delta = lfilter([1,-1],[1],t)
   Ts = mean(t_delta[1:-1])
   fs = 1/Ts

   # assume sd_out corresponds to 1-bit Delta-Sigma sequence alternating between 0 and 1
   sd_out_mean = mean(sd_out)
   sd_out[sd_out < sd_out_mean] = 0
   sd_out[sd_out > sd_out_mean] = 1

   freq,s_out,s_out_aweight = calc_win_fft(sd_out,fs)
   
   signal_power,noise_power,s_out_bw,freq_bw,signal_indices = calc_sig_noise_over_freq_range(s_out,freq,fstart,fend)
   sqnr_log = 10*log10(signal_power/noise_power)
   print("    --> Standard:  Signal Power:  %5.2f dB, Noise Power: %5.2f dB, SNR = %5.2f dB" % (10*log10(signal_power),10*log10(noise_power),sqnr_log))

   semilogx(freq,10*log10(s_out+1e-30),label='non-weighted')
   # mark signal bins
   semilogx(freq_bw[signal_indices],10*log10(s_out_bw[signal_indices]),'ko')

   # A-weight version
   signal_power,noise_power,s_out_bw,freq_bw,signal_indices = calc_sig_noise_over_freq_range(s_out_aweight,freq,fstart,fend)
   sqnr_log = 10*log10(signal_power/noise_power)
   print("    --> A-weight:  Signal Power:  %5.2f dB, Noise Power: %5.2f dB, SNR = %5.2f dB" % (10*log10(signal_power),10*log10(noise_power),sqnr_log))

   semilogx(freq,10*log10(s_out_aweight+1e-30),label='A-weighted')
   # mark signal bins
   semilogx(freq_bw[signal_indices],10*log10(s_out_bw[signal_indices]),'ko')


   # mark high end of BW used for SNR calc
   bw_marker_f = np.array([freq_bw[-1],freq_bw[-1]+1e-3])
   bw_marker = np.array([-200,0])
   semilogx(bw_marker_f, bw_marker,'r--')
   # mark low end of BW used for SNR calc
   bw_marker_f = np.array([freq_bw[0],freq_bw[0]+1e-3])
   bw_marker = np.array([-200,0])
   semilogx(bw_marker_f, bw_marker,'r--')


axis([10, 0.5*fs, -220, 3])
xlabel('Frequency (Hz)')
ylabel('Spectral Density')
title('Simulated Spectral Density')
grid(True,which='both')
legend(loc='lower right',prop={'size':14})
fig_snr.show()
