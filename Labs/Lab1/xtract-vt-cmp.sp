Parameter extraction of nmos

.LIB "/project2020/eda/ngspice-32/models/scn4m_subm/scmos_bsim4.lib" nom

* Level-1 MOdel
.MODEL sitn NMOS (LEVEL=1 VT0=0.508 PHI=0.846 KP=121E-6 LAMBDA=0.028 GAMMA=0.576)

* Testbench
vds 	out 	0 	dc 5
vid 	out 	D 	dc 0
vsb 	0 	B 	dc 0
* Test MOS (SCMOS)
M0 	D 	D 	0 	B scmosn w=5u l=1u m=1

* SIT MOS
vid2 	out 	D2 	DC 0
M1 	D2 	D2 	0 	B sitn w=5u l=1u m=1


* Simulation
.DC vds 0 3 0.001 vsb 0 2 1

.control

run
*plot (vid#branch*2)^0.5 vs v(D)
plot (vid#branch*2)^0.5 (vid2#branch*2)^0.5 vs v(D)
*plot vid#branch vid2#branch vs v(D)

.endc

.end
