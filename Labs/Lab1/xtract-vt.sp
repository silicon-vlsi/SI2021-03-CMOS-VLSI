Parameter extraction of nmos

.LIB "/project2020/eda/ngspice-32/models/scn4m_subm/scmos_bsim4.lib" nom

vds out 0 dc 5
vid out D dc 0
vsb 0 B dc 0
M0 D D 0 B scmosn w=5u l=1u m=1

.DC vds 0 3 0.001 vsb 0 2 1

.control
run
plot (vid#branch*2)^0.5 vs v(D)
.endc

.end
