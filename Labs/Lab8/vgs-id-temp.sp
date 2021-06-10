Extracting Vt, Kp, gamma for nmos from Vgs-Id plots

* Include the SCMOS library
.LIB "/project2020/eda/ngspice-32/models/scn4m_subm/scmos_bsim4.lib" nom 


* Netlist 
M1	D1	D1	0	B	scmosn W=5u L=1u
Vds	D	0	DC	5
Vid1	D	D1	DC	0
Vsb	0	B	DC	1

* Set the device temperature
.TEMP 27 
* Analyses
.DC	Vds	0.1	2	0.001  
*TEMP  0 100 20

* Script
.CONTROL
RUN
PLOT LOG(Vid1#branch) vs v(D)
*PLOT (2*Vid1#branch)^0.5 vs v(D)
.ENDC

.END
