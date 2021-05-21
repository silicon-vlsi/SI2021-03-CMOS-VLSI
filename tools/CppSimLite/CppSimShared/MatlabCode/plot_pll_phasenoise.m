function [] = plot_pll_phasenoise(x,f_low,f_high,node)

vin = evalsig(x,node);
t = evalsig(x,'TIME');
Ts = t(200)-t(199);
num_segments = 20;
window_length = floor(length(t)/num_segments);

Kv = 1; % VCO gain (Hz/V) - this is one for noiseout from CppSim
phase = filter(Ts*2*pi*Kv,[1 -1],vin-mean(vin));
%  calculate L(f)
[Pxx,f] = pwelch(phase,window_length,[],[],1/Ts,'twosided');
%[Pxx,f] = psd(sqrt(Ts)*phase,2^16,1/Ts,2^16,'mean');

Pxx_db = 10*log10(Pxx);
semilogx(f,Pxx_db);
axis([f_low f_high min(Pxx_db) max(Pxx_db)]);

temp_string = sprintf('Phase noise computed from signal ''%s''',node);
temp_string = strrep(temp_string,'_','\_');
title(temp_string);
xlabel('Frequency Offset from Carrier (Hz)');
ylabel('L(f) (dBc/Hz)');
grid on;
