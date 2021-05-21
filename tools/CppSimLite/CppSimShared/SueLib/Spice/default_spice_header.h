********************** begin header *****************************

.OPTIONS post ACCT OPTS lvltim=2
.OP

.INCLUDE '<your spice models>'

.PARAM vddp=3.0		$ VDD voltage

VDD vdd 0 DC vddp 

.TEMP 105
.TRAN 20p 20n
*********************** end header ******************************
