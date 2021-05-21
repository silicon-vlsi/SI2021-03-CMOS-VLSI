***** Spice Testbench for Cell 'invX1' *****


************** Include the model files
.lib '../../../../ngspice-32/models/scn4m_subm/scmos_bsim4.lib' nom

************** Include the subcircuit generated from Sue2 **************
.include ../../../Netlist/invX1.sp

************** Instantiate the Subcircuit
Xinv in out invX1

.temp 25
.param vsupply=5.0
.global vdd gnd
Cload	out 	0	10f
Vsup vdd 0 vsupply
Vclk in  0 PULSE(0 vsupply 5n 100p 100p 10n 20n)
.options delmax=5p relv=1e-6 reli=1e-6 relmos=1e-6 method=gear
.tran 100p 20n
.op

.end

