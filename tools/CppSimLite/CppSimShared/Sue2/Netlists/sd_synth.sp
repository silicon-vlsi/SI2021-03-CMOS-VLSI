***** Hspice Netlist for Cell 'sd_synth' *****

************** Module and2 **************
.subckt and2 a b y
.ends and2

************** Module ch_pump **************
.subckt ch_pump in out ival=1
.ends ch_pump

************** Module constant **************
.subckt constant out consval=1
.ends constant

************** Module dffreset **************
.subckt dffreset d clk q qb r
.ends dffreset

************** Module dffset **************
.subckt dffset d clk q qb s
.ends dffset

************** Module divider_ideal **************
.subckt divider_ideal in div_val out
.ends divider_ideal

************** Module gated_noise **************
.subckt gated_noise mux_in out varpos=1 varneg=1
.ends gated_noise

************** Module inv **************
.subckt inv a y
.ends inv

************** Module latch1 **************
.subckt latch1 d latch q
.ends latch1

************** Module leadlagfilter **************
.subckt leadlagfilter in out fp=1 fz=1 gain=1
.ends leadlagfilter

************** Module noise **************
.subckt noise out var=1
.ends noise

************** Module sd_modulator **************
.subckt sd_modulator in clk out order=1
.ends sd_modulator

************** Module step_in **************
.subckt step_in step vend=1 vstart=1 tstep=1
.ends step_in

************** Module sum_junct **************
.subckt sum_junct a y b
.ends sum_junct

************** Module vco **************
.subckt vco vctrl squareout sineout freq=1 kvco=1
.ends vco

************** Module xor2 **************
.subckt xor2 a b y
.ends xor2

************** Module dff2 **************
.subckt dff2 d clk q qb
xi0 clk n0 inv
xi1 q qb inv
xi2 n1 n0 q latch1
xi3 d clk n1 latch1
.ends dff2

************** Module nand2 **************
.subckt nand2 a b y
xi0 a b n0 and2
xi1 n0 y inv
.ends nand2

************** Module xorpfd **************
.subckt xorpfd ref div out
xi0 n0 ref n5 n0 dff2
xi1 xor_out ref n7 n2 n8 dffreset
xi2 xor_out div n4 n8 n7 dffset
xi3 xor_out n4 n3 nand2
xi4 n2 n3 out nand2
xi5 n5 n6 xor_out xor2
xi6 n1 div n6 n1 dff2
.ends xorpfd

************** Module sd_synth **************
.subckt sd_synth out
xi0 n3 constant consval=0
xi1 n3 ref n6 vco freq=20e6 kvco=1
xi12 ref div pfdout xorpfd
xi2 pfdout n0 ch_pump ival=1.5e-6
xi3 n1 n2 leadlagfilter fp=127.2e3 fz=11.8e3 gain=1/(30e-12)
xi4 vin out n7 vco freq=1.84e9 kvco=30e6
xi5 n4 noise var=3.25e-16
xi6 pfdout n5 gated_noise varpos=1.85e-25 varneg=1.2e-24
xi7 out div_val div divider_ideal
xi8 sd_in div div_val sd_modulator order=2
xi9 sd_in step_in vend=in_gl+delta_gl vstart=in_gl tstep=step_time_gl
xi10 n2 vin n4 sum_junct
xi11 n0 n1 n5 sum_junct
.ends sd_synth


.end

