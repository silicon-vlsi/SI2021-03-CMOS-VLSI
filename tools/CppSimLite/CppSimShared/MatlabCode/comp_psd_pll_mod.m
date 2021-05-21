%%% This Matlab code is used to plot phase noise of a synthesizer

%  loadsig and evalsig are part of Hspice Toolbox
% x = loadsig('test.tr0'); % load file from C++ sim
 vin = evalsig(x,'vin'); % input to VCO
 t = evalsig(x,'TIME'); % simulation time samples
%  set parameters and cut out initial transient
 Ts = t(200)-t(199); % simulation sample period
 Kv = 30e6; % VCO gain (Hz/V)
 vin = vin(40000:length(vin)); % cut out initial transient
%  create VCO output phase based on integration of vin
 phase = filter(Ts*2*pi*Kv,[1 -1],vin-mean(vin));

 baseband_out = cos(phase)+j*sin(phase);
 baseband_out = baseband_out - mean(baseband_out);
%  calculate output spectrum referred to baseband
 [Pxx,f] = psd(sqrt(Ts)*baseband_out,2^16,1/Ts,2^16,'mean');
 Pxx_cen = fftshift(Pxx);
 f = f - 1.0/(2.0*Ts);
 plot(f,10*log10(Pxx_cen));
 axis([-4e6 4e6 -180 -50]);
 title('Simulated Output Spectrum of Transmitter');
 xlabel('Frequency Offset from Carrier (Hz)');
 ylabel('Output Spectrum (dBc/Hz)');
 grid on;
