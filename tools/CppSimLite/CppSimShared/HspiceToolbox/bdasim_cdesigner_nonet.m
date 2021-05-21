function [] = bdasim_cdesigner_nonet(parfile_name)
if nargin == 0
    bdasim_cdesigner('test.hspc','no_net');
else
    bdasim_cdesigner(parfile_name,'no_net');
end
