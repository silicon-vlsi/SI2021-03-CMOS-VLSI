import sys
import os
reg_control_home = os.getenv("REG_CONTROL_HOME")
sys.path.append(reg_control_home + '/Python')

from reg_control import *
rtc = RegControl()

def set_get_reg_check(reg_name,set_val):
   rtc.setreg(reg_name,set_val)    
   out_val = rtc.getreg(reg_name)   
   print('%s = %s (should be %s)' % (reg_name,out_val,str(set_val)))

for i in range(10):
    set_get_reg_check('DelayVal',i)
    
for i in range(10):
    set_get_reg_check('temp_trim',i+10)

set_get_reg_check('BiasTrim','0')
set_get_reg_check('BiasTrim','+20')
set_get_reg_check('BiasTrim','+10')
set_get_reg_check('BiasTrim','-10')

# print all registers
rtc.print_reg('')
# print all registers that contain 'del' in them
rtc.print_reg('del')
# print all tokens for register that contains 'bias' in it
rtc.print_reg_tokens('bias')

# print all test blocks
rtc.print_test('')
# print all test blocks that contain 'bias' in them
rtc.print_test('bias')
# print all tokens for test block that contains 'test' in it
rtc.print_test_tokens('test')


out_val = rtc.tokenmap.get_token_map('BiasTrim','register','0')

rtc.print_reg_state('bias')
#rtc.print_reg_state_gui('bias')
#rtc.print_reg_state_gui()
#rtc.print_reg_state_gui('')

