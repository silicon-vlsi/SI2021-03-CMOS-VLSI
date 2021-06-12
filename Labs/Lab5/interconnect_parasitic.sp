##################### Interconnect Parasitics #####################

*Define model file
.LIB /project2020/eda/ngspice-32/models/scn4m_subm/scmos_bsim4.lib nom
.TEMP 27
.GLOBAL vdd gnd 

*source definition
vin in gnd pulse 0 2.5 2n 10p 10p 15n 30n 
vsup vdd gnd DC 2.5

*Lumped-model definition
RLumped in outL 15k
CLumped outL gnd 192f

*T-Model definition
RT1 in n1 7.5k
RT2 n1 outT 7.5k
CT n1 gnd 192f

*PI-Model definition 
RP in outP 15k
CP1 in gnd 96f
CP2 outp gnd 96f

*Distributted Model definition
*Resistor
RD1 in nd1 1.5k
RD2 nd1 nd2 1.5k
RD3 nd2 nd3 1.5k
RD4 nd3 nd4 1.5k
RD5 nd4 nd5 1.5k
RD6 nd5 nd6 1.5k
RD7 nd6 nd7 1.5k
RD8 nd7 nd8 1.5k
RD9 nd8 nd9 1.5k
RD10 nd9 outD 1.5k

*Capacitor
CD1 nd1 gnd 19.2f
CD2 nd2 gnd 19.2f
CD3 nd3 gnd 19.2f
CD4 nd4 nd4 19.2f
CD5 nd5 gnd 19.2f
CD6 nd6 gnd 19.2f
CD7 nd7 gnd 19.2f
CD8 nd8 gnd 19.2f
CD9 nd9 gnd 19.2f
CD10 outD gnd 19.2f

.TRAN 5p 35n

*control statements
.CONTROL
RUN
PLOT v(in) v(outL) v(outT) v(voutP) v(outD)
.ENDC

*Measuement statements
*for Lumped model
.MEASURE TRAN TPLH TRIG v(in) val='vsup*0.5' FALL=1 TARG v(outL) val='vsup*0.5' RISE=1
.MEASURE TRAN TPHL TRIG v(in) val='vsup*0.5' RISE=1 TARG v(outL) val='vsup*0.5' FALL=1
.MEASURE TRAN TRISE TRIG v(outL) val='vsup*0.1' RISE=1 TARG v(outL) val='vsup*0.9' RISE=1
.MEASURE TRAN TFALL TRIG v(outL) val='vsup*0.9' FALL=1 TARG v(outL) val='vsup*0.1' FALL=1

*for T-model
.MEASURE TRAN TPLH TRIG v(in) val='vsup*0.5' FALL=1 TARG v(outT) val='vsup*0.5' RISE=1
.MEASURE TRAN TPHL TRIG v(in) val='vsup*0.5' RISE=1 TARG v(outT) val='vsup*0.5' FALL=1
.MEASURE TRAN TRISE TRIG v(outT) val='vsup*0.1' RISE=1 TARG v(outT) val='vsup*0.9' RISE=1
.MEASURE TRAN TFALL TRIG v(outT) val='vsup*0.9' FALL=1 TARG v(outT) val='vsup*0.1' FALL=1

*for PI-model
.MEASURE TRAN TPLH TRIG v(in) val='vsup*0.5' FALL=1 TARG v(outP) val='vsup*0.5' RISE=1
.MEASURE TRAN TPHL TRIG v(in) val='vsup*0.5' RISE=1 TARG v(outP) val='vsup*0.5' FALL=1
.MEASURE TRAN TRISE TRIG v(outP) val='vsup*0.1' RISE=1 TARG v(outP) val='vsup*0.9' RISE=1
.MEASURE TRAN TFALL TRIG v(outP) val='vsup*0.9' FALL=1 TARG v(outP) val='vsup*0.1' FALL=1

*for Distributted-model
.MEASURE TRAN TPLH TRIG v(in) val='vsup*0.5' FALL=1 TARG v(outD) val='vsup*0.5' RISE=1
.MEASURE TRAN TPHL TRIG v(in) val='vsup*0.5' RISE=1 TARG v(outD) val='vsup*0.5' FALL=1
.MEASURE TRAN TRISE TRIG v(outD) val='vsup*0.1' RISE=1 TARG v(outD) val='vsup*0.9' RISE=1
.MEASURE TRAN TFALL TRIG v(outD) val='vsup*0.9' FALL=1 TARG v(outD) val='vsup*0.1' FALL=1
.END
