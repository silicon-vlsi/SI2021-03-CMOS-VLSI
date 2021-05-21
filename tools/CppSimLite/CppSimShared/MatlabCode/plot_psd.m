function [] = plot_psd(x,f_low,f_high,node)

in = evalsig(x,node);
t = evalsig(x,'TIME');
Ts = t(200)-t(199);
num_segments = 20;
window_length = floor(length(t)/num_segments);

[Pxx,f] = pwelch(in,window_length,[],[],1/Ts,'onesided');

semilogx(f,Pxx);
axis([f_low f_high min(Pxx) max(Pxx)]);

temp_string = sprintf('One-sided PSD of signal ''%s''',node);
temp_string = strrep(temp_string,'_','\_');
title(temp_string);
xlabel('Frequency (Hz)');
ylabel('V^2/Hz');
grid on;
