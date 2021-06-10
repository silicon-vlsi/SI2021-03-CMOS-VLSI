Extracting Vt, Kp, gamma for nmos

* Include the SCMOS library
.LIB "/project2020/eda/ngspice-32/models/scn4m_subm/scmos_bsim4.lib" nom

* SIT Level-1 MODEL
.MODEL sitn NMOS (LEVEL=1 PHI=0.846 VT0=0.514 KP=122U GAMMA=0.55 LAMBDA=0.0)

* Set the device temperature
.TEMP 27

* Netlist 
M1	D1	D1	0	B 	scmosn  W=5u L=1u
M2	D2	D2	0	B 	sitn    W=5u L=1u
Vds	D	0	DC	5
Vid1	D	D1	DC	0
Vid2	D	D2	DC	0
Vsb	0	B	DC	0

* Analyses
.DC	Vds	0	5	0.001  Vsb  0 1 0.5

* Script 
.CONTROL
RUN
PLOT (2*Vid2#branch)^0.5 (2*Vid1#branch)^0.5 vs V(D) 
*PLOT deriv(2*Vid2#branch) deriv(2*Vid1#branch) vs V(D) 
.ENDC

.END
