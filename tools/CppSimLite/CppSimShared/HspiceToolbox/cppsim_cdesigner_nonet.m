function [] = cppsim_cdesigner_nonet(parfile_name)
if nargin == 0
    cppsim_vppsim_cdesigner('cpp','test.hspc','no_net');
else
    cppsim_vppsim_cdesigner('cpp',parfile_name,'no_net');
end
