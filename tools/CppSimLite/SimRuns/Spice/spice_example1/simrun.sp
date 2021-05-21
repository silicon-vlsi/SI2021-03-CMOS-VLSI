***** Spice Netlist for Cell 'spice_example1' *****

************** Module spice_example1 **************
m0 n1 n0 gnd gnd
+    cmosn l='0.13u' w='1.0u' m='1'
+    ad='hdout*(1.0u)' as='hdout*(1.0u)'
+    pd='2*hdout+2*(1.0u)' ps='2*hdout+2*(1.0u)'
m1 n1 n0 vdd vdd
+    cmosp l='0.13u' w='1.0u' m='1'
+    ad='hdout*(1.0u)' as='hdout*(1.0u)'
+    pd='2*hdout+2*(1.0u)' ps='2*hdout+2*(1.0u)'
r2 n1 n2 10k noisy=0
r0 n0 n1 100k noisy=0
r1 in n0 50k noisy=0
c0 gnd n2 2p
xi0 n2 out inverter
c1 gnd out 2p

************** Module inverter **************
.subckt inverter in out m=1 w_p=2.4u l_p=0.13u w_n=1.2u l_n=0.13u
m1 out in vdd vdd
+    cmosp l='l_p' w='w_p' m='m'
+    ad='hdout*(w_p)' as='hdout*(w_p)'
+    pd='2*hdout+2*(w_p)' ps='2*hdout+2*(w_p)'
m0 out in gnd gnd
+    cmosn l='l_n' w='w_n' m='m'
+    ad='hdout*(w_n)' as='hdout*(w_n)'
+    pd='2*hdout+2*(w_n)' ps='2*hdout+2*(w_n)'
.ends inverter



.param hdout=2.000e-07
.param hdin=3.000e-07
.include '../../../SpiceModels/cmos_013_bsim4.mod'
.include '../../../SpiceModels/bipolar.mod'
.temp 25
.param vsupply=1.3
.global vdd gnd
Vsup vdd 0 vsupply
.options delmax=5p relv=1e-6 reli=1e-6 relmos=1e-6 method=gear
.tran 10p 1u
.probe v(in) v(n0) v(n1) v(n2) v(out) i(Vsup)

v_in in 0 pwl '0*(1/10e6)' '0'
+, '0*(1/10e6)+.2n' '0', '1*(1/10e6)' '0'
+, '1*(1/10e6)+.2n' '0', '2*(1/10e6)' '0'
+, '2*(1/10e6)+.2n' 'vsupply', '3*(1/10e6)' 'vsupply'
+, '3*(1/10e6)+.2n' 'vsupply', '4*(1/10e6)' 'vsupply'
+, '4*(1/10e6)+.2n' 'vsupply', '5*(1/10e6)' 'vsupply'
+, '5*(1/10e6)+.2n' '0', '6*(1/10e6)' '0'
+, '6*(1/10e6)+.2n' 'vsupply', '7*(1/10e6)' 'vsupply'
+, '7*(1/10e6)+.2n' 'vsupply', '8*(1/10e6)' 'vsupply'
+, '8*(1/10e6)+.2n' '0', '9*(1/10e6)' '0'
+, '9*(1/10e6)+.2n' '0', '10*(1/10e6)' '0'
+, '10*(1/10e6)+.2n' '0', '11*(1/10e6)' '0'
+, '11*(1/10e6)+.2n' 'vsupply', '12*(1/10e6)' 'vsupply'
+, '12*(1/10e6)+.2n' '0', '13*(1/10e6)' '0'
+, R, TD='0.0n'

.end

