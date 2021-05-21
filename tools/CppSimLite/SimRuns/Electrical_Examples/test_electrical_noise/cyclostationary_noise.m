%% used to plot noise variance of sampler at different points in time
%% relative to the clock pulse

x = loadsig_cppsim('test_sampler.tr0');

t = evalsig(x,'TIME');
num_samples = 150;

sig_mat = zeros(length(t),num_samples);
sample_offset = 0:(num_samples-1);

for i = 1:num_samples
   sig_name = sprintf('out_vec_%d',i-1);
   sig_mat(:,i) = evalsig(x,sig_name);   
end

% plot(t,sig_mat);
overall = sig_mat(end,:);
plot(sample_offset,overall,'-o')
xlabel('Sample offset from clock edge');
ylabel('Variance (V^2)')
title('Plot of sample variance versus offset from clock edge');
