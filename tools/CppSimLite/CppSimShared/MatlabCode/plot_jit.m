%%% This Matlab code is used to plot output jitter of a CDR

x = loadsig('test.tr0');

raw_period_vco = evalsig(x,'vco_period');
raw_period_in = evalsig(x,'in_period');

%phase = extract_phase(raw_period_vco);
[phase,avg_period] = extract_phase(raw_period_vco,raw_period_in);

plot(phase,'-k');
grid on;
xlabel('VCO rising edge number');
ylabel('Instantaneous Jitter (U.I.)');
title_str = sprintf('Instantaneous Jitter of VCO in CDR: \n Steady-state RMS jitter = %5.4f mUI',1e3*std(phase(30000:length(phase))));
title(title_str);
%axis([-1e3 5e4 -.2 .05])


