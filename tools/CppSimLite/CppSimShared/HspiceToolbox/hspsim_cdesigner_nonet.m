function [] = hspsim_cdesigner_nonet(parfile_name)
if nargin == 0
    hspsim_cdesigner('test.hspc','no_net');
else
    hspsim_cdesigner(parfile_name,'no_net');
end
