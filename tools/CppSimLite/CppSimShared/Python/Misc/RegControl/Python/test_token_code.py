import sys
import os
reg_control_home = os.getenv("REG_CONTROL_HOME")
sys.path.append(reg_control_home + '/Python')

from token_code import *

filename = '%s/RegMap/reg_control_tokens.xlsx' % reg_control_home
#regmap = TokenMap(filename)
regmap = TokenMap()
regmap.load_file(filename,'no')
#regmap.print_registers()

out_set = regmap.set_token_map('temp_trim','register','-40000')
out_get = regmap.get_token_map('temp_trim','register',out_set)

print('\ntemp_trim:\n')
print('out_set = %s, out_get = %s' % (out_set, out_get))

out_set = regmap.set_token_map('DelayVal','register','63')
out_get = regmap.get_token_map('DelayVal','register',out_set)
out_min = regmap.get_token_map('DelayVal','register','min')
out_max = regmap.get_token_map('DelayVal','register','max')

print('\nDelayVal:\n')
print('out_set = %s, out_get = %s, out_min = %s, out_max = %s' % (out_set, out_get,out_min,out_max))


out_set = regmap.set_token_map('bias_test','test_block','set_ref_cur')
out_get = regmap.get_token_map('bias_test','test_block',out_set)

print('\nbias_test:\n')
print('out_set = %s, out_get = %s' % (out_set, out_get))

reg_name = 'BiasTrim'
token_name_list = regmap.get_token_names(reg_name,'register')
print('\nBiasTrim:\n')
for token_name in token_name_list:
    print('token_name = %s for reg %s' % (token_name,reg_name))

reg_name = 'bias_test'
token_name_list = regmap.get_token_names(reg_name,'test_block')
print('\nbias_test:\n')
for token_name in token_name_list:
    print('token_name = %s for reg %s' % (token_name,reg_name))

reg_name_list = regmap.get_reg_names('register')
for reg_name in reg_name_list:
    print('reg_name = %s' % (reg_name))


reg_name_list = regmap.get_reg_names('test_block')
for reg_name in reg_name_list:
    print('test_block_name = %s' % (reg_name))
    
regmap.print_reg('')
regmap.print_reg('trim')
regmap.print_reg_tokens('del')
regmap.print_reg_tokens('temp')
regmap.print_reg_tokens('bias')
regmap.print_reg_tokens('')

pad_info_0 = regmap.get_pad_info('bias_test', 'meas_voltage', 0)
print('\npad_info_0 = %s' % pad_info_0)

