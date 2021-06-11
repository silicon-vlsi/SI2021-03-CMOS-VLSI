################# Dynamic Characteristics of Inverter #####################

.LIB /project2020/eda/ngspice-32/models/scn4m_subm/scmos_bsim4.lib nom
.TEMP 27
.GLOBAL vdd gnd 

* Circuit definition
vsup vdd gnd DC 2.5
vin vdd gnd pulse 0 2.5 10n 100p 100p 5n 10n
MP1 out in vdd vdd scmosp w=1.8u l=0.4u m=1
MN1 out in gnd gnd scmosn w=0.6u l=0.4u m=1

*Fan out gate
MP2 out2 in vdd vdd scmosp w=1.8u l=0.4u m=4
MN2 out2 in gnd gnd scmosn w=0.6u l=0.4u m=4

*transient analysis
.TRAN 5p 50n

*control statements
.CONTROL
RUN
PLOT v(in)+3 v(out)
.ENDC

*mesure statements
.measure TRAN tplh TRIG v(in) val='vsup*0.5' FALL=1 TARG v(out) val='vsup*0.5' RISE=1
.measure TRAN tphl TRIG v(in) val='vsup*0.5' RISE=1 TARG v(out) val='vsup*0.5' FALL=1
.measure TRAN trise TRIG v(out) val='vsup*0.1' RISE=1 TARG v(out) val='vsup*0.9' RISE=1
.measure TRAN tfall TRIG v(out) val='vsup*0.9' FALL=1 TARG v(out) val='vsup*0.1' FALL=1
.END
