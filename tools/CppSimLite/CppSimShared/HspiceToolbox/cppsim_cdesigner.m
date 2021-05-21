function [] = vppsim_cdesigner(parfile_name)
if nargin == 0
    cppsim_vppsim_cdesigner('cpp','test.hspc','netlist');
else
    cppsim_vppsim_cdesigner('cpp',parfile_name,'netlist');
end
