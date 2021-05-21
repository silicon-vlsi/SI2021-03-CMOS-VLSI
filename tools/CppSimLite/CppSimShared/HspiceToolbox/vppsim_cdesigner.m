function [] = vppsim_cdesigner(parfile_name)
if nargin == 0
    cppsim_vppsim_cdesigner('vpp','test.hspc','netlist');
else
    cppsim_vppsim_cdesigner('vpp',parfile_name,'netlist');
end
