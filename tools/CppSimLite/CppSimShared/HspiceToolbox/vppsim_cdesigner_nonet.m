function [] = vppsim_cdesigner_nonet(parfile_name)
if nargin == 0
    cppsim_vppsim_cdesigner('vpp','test.hspc','no_net');
else
    cppsim_vppsim_cdesigner('vpp',parfile_name,'no_net');
end
