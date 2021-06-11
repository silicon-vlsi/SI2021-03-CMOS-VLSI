################ Inverter Static Characteristics #################

*include model file
.LIB /project2020/eda/ngspice-32/models/scn4m_subm/scmos_bsim4.lib nom
.TEMP 27
.GLOBAL vdd gnd

*define sources
vsup vdd gnd  DC 2.5
vin  in  gnd  DC 2.5

*define MOSFETs
MP1 out in vdd vdd scmosp w=3u l=1u m=1
MN1 out in gnd gnd scmosn w=1u l=1u m=1

*sweep input voltage
.DC vin 0 2.5 0.001

*control statements
.CONTROL
RUN
PLOT v(out) v(in)
.ENDC
.END
