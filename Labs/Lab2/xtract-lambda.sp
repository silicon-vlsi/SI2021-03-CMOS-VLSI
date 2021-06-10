Extracting Vt, Kp, gamma for nmos

* Include the SCMOS library
.LIB "/project2020/eda/ngspice-32/models/scn4m_subm/scmos_bsim4.lib" nom

* -- SIT Level-1 MODEL
.MODEL sitn NMOS (LEVEL=1 PHI=0.846 VT0=0.514 KP=122U GAMMA=0.55 LAMBDA=0.0294)

* Set the device temperature
.TEMP 27

* Netlist 
M1	D1	G	0	B 	scmosn  W=5u L=1u
M2	D2	G	0	B 	sitn    W=5u L=1u
Vds	D	0	DC	5
Vid1	D	D1	DC	0
Vid2	D	D2	DC	0
Vsb	0	B	DC	0
Vgs	G	0	DC	1.0

* Analyses
.DC	Vds	0	5	0.001  Vgs 1 1.5 0.5

* Script 
.CONTROL
RUN
PLOT Vid2#branch Vid1#branch vs V(D) 
*PLOT deriv(2*Vid2#branch) deriv(2*Vid1#branch) vs V(D) 
.ENDC

.END
