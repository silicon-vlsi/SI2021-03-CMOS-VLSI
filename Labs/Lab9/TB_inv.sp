Testbench for inverter sub-circuit
.GLOBAL vdd gnd
.PARAM vsupply=5.0
**Library model file
.LIB "/project2020/eda/ngspice-32/models/scn4m_subm/scmos_bsim4.lib" nom 

**Include the Inverter netlist
.INCLUDE Lab9_inv.sp

**Instatiate inverter
Xinv1 in out Lab9_inv
 
**Sources
Vsup	vdd	0	DC vsupply
Vin	in	0	PULSE(0 vsupply 5n 10p 10p 10n 20)
Cload	out	0	50f

**Analyses
.TRAN 10p 20n

.CONTROL
RUN
PLOT v(out)
.ENDC

.END
