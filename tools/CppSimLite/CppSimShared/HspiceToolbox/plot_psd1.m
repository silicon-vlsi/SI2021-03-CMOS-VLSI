%  written by Mike Perrott with modifications by Scott Willingham
function [] = plot_psd1(x,s,log)

global sweep;

title_str = s(1);
sig = evalsig(x, char(s(1)));
for i = 2:length(s)
  title_str = strcat(title_str,', ',s(i));
  sig = [sig, evalsig(x, char(s(i)))];
end

t = x(1).data(:,1);
if length(t) < 20
    return
end

%  set parameters
Ts_vec = filter([1 -1],1,t);
if length(Ts_vec) > 10010
   Ts = mean(Ts_vec(10:10010));
else
   Ts = mean(Ts_vec(10:length(Ts_vec)));
end

num_segments = 20;
window_length = floor(length(t)/num_segments);

for i = 1:size(sig,2)
   [Pxx(:,i),f] = pwelch(sig(:,i),window_length,[],[],1/Ts,'onesided');
end

ivar = f(2:end);
Pxx = Pxx(2:end,:);

if nargin > 2
    switch log
	case 'logx'
	    semilogx(ivar, Pxx)
	case 'logy'
	    semilogy(ivar, Pxx)
	case {'logxy','loglog'}
	    loglog(ivar, Pxx)
	otherwise
	    fprintf('Warning (plot_psd1): unrecognized plot_args\n')
	    plot(ivar, Pxx)
    end
else
    plot(ivar, Pxx)
end

title_str = strrep(char(title_str), '_', '\_');
ylabel(title_str);
grid on;
