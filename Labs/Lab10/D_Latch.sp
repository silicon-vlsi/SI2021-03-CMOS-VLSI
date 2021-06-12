################ Spice netlist for D_latch Test bench ############################

*Library definition
.LIB /project2020/eda/ngspice-32/models/scn4m_subm/scmos_bsim4 nom
.TEMP 27
.GLOBAL vdd gnd

**************** Testbench *********************
Vsup vdd gnd DC 2.5
Vin1 D gnd pulse 0 2.5 1n 100p 100p 3n 6n
Vin2 CK gnd pulse 0 2.5 1.5n 100p 100p 6n 12n
XDLatch D CK Q D_Latch

***************** Module D_Latch ***************
.subckt D_Latch d ck q
Xi0 d n2 Inverter
Xi1 n0 q Inverter
Xi2 q n1 Inverter
Xi3 ck clkb Inverter
Xi4 clkb clk Inverter
Xi5 n2 n0 clkb clk TX_Gate
Xi6 n1 n0 clk clkb TX_Gate
.ENDS D_Latch

*************** Module Inverter ***************
.subckt Inverter input output

