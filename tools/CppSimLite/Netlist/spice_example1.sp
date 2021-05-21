***** Spice Netlist for Cell 'spice_example1' *****

************** Module spice_example1 **************
.subckt spice_example1 in out
m0 n1 n0 gnd gnd cmosn w='1.0u' l='0.13u' m='1' geo='0' 
m1 n1 n0 vdd vdd cmosp w='1.0u' l='0.13u' m='1' geo='0' 
r2 n1 n2 10k noisy=0 
r0 n0 n1 100k noisy=0 
r1 in n0 50k noisy=0 
c0 gnd n2 2p 
xi0 n2 out inverter
c1 gnd out 2p 
.ends spice_example1

************** Module inverter **************
.subckt inverter in out m=1 w_p=2.4u l_p=0.13u w_n=1.2u l_n=0.13u
m1 out in vdd vdd cmosp w='w_p' l='l_p' m='m' geo='0' 
m0 out in gnd gnd cmosn w='w_n' l='l_n' m='m' geo='0' 
.ends inverter


.end

