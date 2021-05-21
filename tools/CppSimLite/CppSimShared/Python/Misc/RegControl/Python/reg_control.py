import ctypes
import numpy as np
import sys
import os
from token_code import *
from reg_control_gui_functions import *

reg_control_home = os.getenv("REG_CONTROL_HOME")

class RegControl(object):

    token_filename = '%s/RegMap/reg_control_tokens.xlsx' %  reg_control_home
    # tokenmap = TokenMap(token_filename,'verbose')
    tokenmap = TokenMap(token_filename)

    def __init__(self):
       reg_control_lib_file = '%s/win64/reg_control.dll' % reg_control_home
       self.reg_control_lib = ctypes.CDLL(reg_control_lib_file)
       self.reg_control_lib.SetReg.argtypes = [ctypes.c_char_p, ctypes.c_int]
       self.reg_control_lib.GetReg.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_int)]

    def __repr__(self):
       return "Nothing to report..."
          
    def get_reg_names(self,reg_type='register'):
        return(self.tokenmap.get_reg_names(reg_type))

    def get_reg_info(self, reg_name, info_str):
        return(self.tokenmap.get_reg_info(reg_name, info_str))
               
    def print_reg(self,reg_field_search_name):
        self.tokenmap.print_reg(reg_field_search_name)

    def print_reg_state(self,reg_field_search_name):
        reg_name_list = self.tokenmap.find_reg_name(reg_field_search_name,'register')
        if len(reg_name_list) == 0:
            print("\nError using print_reg_state_gui()")
            print("   -> No register name matches '%s'" % reg_field_search_name)
        reg_name_list.sort()
        reg_name_value_list = [['' for col in range(4)] for row in range(len(reg_name_list))]
        get_bin = lambda x,n: format(x,'b').zfill(n)
        for i in range(len(reg_name_list)):
           num_bits = self.get_reg_info(reg_name_list[i],'num_bits')
           reg_token = self.getreg(reg_name_list[i])
           reg_value = self.getreg_raw(reg_name_list[i])
           reg_name_value_list[i][0] = reg_name_list[i]
           reg_name_value_list[i][1] = num_bits
           reg_name_value_list[i][2] = reg_token
           reg_name_value_list[i][3] = get_bin(int(reg_value),int(num_bits))

        reg_header = ['Register Name', 'Num Bits', 'Token or Inv_Formula', 'Binary Value']
        print("\n%-30s%-10s%-30s%-35s" % (reg_header[0],reg_header[1],reg_header[2],reg_header[3]))
        for i in range(len(reg_name_list)):
            print("%-30s%-10s%-30s%-35s" % (reg_name_value_list[i][0],reg_name_value_list[i][1],reg_name_value_list[i][2],reg_name_value_list[i][3]))
        print("")
    def print_reg_state_gui(self,reg_field_search_name=''):
        reg_name_list = self.tokenmap.find_reg_name(reg_field_search_name,'register')
        if len(reg_name_list) == 0:
            print("\nError using print_reg_state_gui()")
            print("   -> No register name matches '%s'" % reg_field_search_name)
        reg_name_list.sort()
        reg_name_value_list = [['' for col in range(4)] for row in range(len(reg_name_list))]
        get_bin = lambda x,n: format(x,'b').zfill(n)
        for i in range(len(reg_name_list)):
           num_bits = self.get_reg_info(reg_name_list[i],'num_bits')
           reg_token = self.getreg(reg_name_list[i])
           reg_value = self.getreg_raw(reg_name_list[i])
           reg_name_value_list[i][0] = reg_name_list[i]
           reg_name_value_list[i][1] = num_bits
           reg_name_value_list[i][2] = reg_token
           reg_name_value_list[i][3] = get_bin(int(reg_value),int(num_bits))

        reg_header = ['Register Name', 'Num Bits', 'Token or Inv_Formula', 'Binary Value']
        if len(reg_name_list) < 45:
            num_rows = len(reg_name_list)
        else:
            num_rows = 45
        listbox = MultiColumnListBox(reg_header,reg_name_value_list,num_rows)

    def print_test(self,reg_field_search_name):
        self.tokenmap.print_test(reg_field_search_name)

    def print_reg_tokens(self,reg_field_search_name):
        self.tokenmap.print_reg_tokens(reg_field_search_name)

    def print_test_tokens(self,test_block_search_name):
        self.tokenmap.print_test_tokens(test_block_search_name)
    
    def get_pad_info(self,test_block_name, token_name):
        out_pad_info = ''
        for pad_num in range(4):
           out_pad_info = out_pad_info + self.tokenmap.get_pad_info(test_block_name, token_name, pad_num)
        return(out_pad_info)

    def test_mode_check(self,test_block_name,token_name,data_val,function_name):
       test_block_name_confirmed = self.tokenmap.find_reg_name(test_block_name,'test_block')
       if len(test_block_name_confirmed) == 0:
           print("\nError using %s() function: test_block not found" % function_name)
           print("  -> input test_block is '%s' (token = '%s', data = '%d')" % (test_block_name,token_name,data_val))
           sys.exit()
       elif len(test_block_name_confirmed) > 1:
           found_exact_name_match_flag = 0
           for test_block_name_check in test_block_name_confirmed:
               if test_block_name == test_block_name_check:
                   found_exact_name_match_flag = 1
                   break
           if found_exact_name_match_flag == 0:
               print("\nError using %s() function: multiple test_block names found" % function_name)
               print("  -> input test_block is '%s' (token = '%s', data = '%d')" % (test_block_name,token_name,data_val))
               print("     found test_block names:")
               for test_block in test_block_name_confirmed:
                  print("        test_block '%s'" % test_block)
               sys.exit()
       elif test_block_name != test_block_name_confirmed[0]:
           print("\nError using %s() function: test_block name incomplete" % function_name)
           print("  -> input test_block is '%s' (token = '%s', data = '%d')" % (test_block_name,token_name,data_val))
           print("     seems to match test_block '%s'" % test_block_name_confirmed[0])
           sys.exit()

       if token_name != 'skip_token_check':
           token_list = self.get_token_names(test_block_name,'test_block')
           token_name_confirmed = ''
           for token_val in token_list:
               if token_val == token_name:
                  token_name_confirmed = token_name
                  break
           if token_name_confirmed != token_name:
               print("\nError using %s() function: token_name not found" % function_name)
               print("  -> input test_block is '%s', token = '%s' (data = '%d')" % (test_block_name,token_name,data_val))
               print("     valid tokens are:")
               for token_val in token_list:
                  print("        '%s'" % token_val)
               sys.exit()
       if data_val != 1 and data_val != 0:
           print("\nError using %s() function: data_val must be 0 or 1" % function_name)
           print("  -> input test_block is '%s', token = '%s' (data = '%d')" % (test_block_name,token_name,data_val))
           sys.exit()

    def convert_units(self, unit_val_in):
       unit_val = unit_val_in
       unit_val = unit_val.replace('f','e-15')
       unit_val = unit_val.replace('p','e-12')
       unit_val = unit_val.replace('n','e-9')
       unit_val = unit_val.replace('u','e-6')
       unit_val = unit_val.replace('m','e-3')
       unit_val = unit_val.replace('k','e3')
       unit_val = unit_val.replace('M','e6')
       unit_val = unit_val.replace('G','e9')
       return(unit_val)
       
    def get_nearest_token(self, reg_name, input_value):
       # first confirm input_value is a string
       try:
          input_value + ''
       except TypeError:
          print("Error using get_nearest_token():  input value must be a string")
          print("  -> register name is '%s'" % reg_name)
          print("input value:")
          print(input_value)
          print("")
          sys.exit()

       try:
          input_value_float = float(self.convert_units(input_value))
       except ValueError:
          print("Error using get_nearest_token():  input value must represent a number with possible scale value")
          print("   Examples:   4 or 1.3 or 5.7k or 2.9m or 9.4M")
          print("  -> register name is '%s'" % reg_name)
          print("     input value is '%s'" % input_value)
          sys.exit()

       token_list = self.get_token_names(reg_name,'register')
       token_list_unit_conv = token_list[:]
       for i in range(len(token_list_unit_conv)):
          token_list_unit_conv[i] = self.convert_units(token_list_unit_conv[i])
       try:
          token_float_list = np.asarray(token_list_unit_conv,dtype=float)
       except ValueError:
          print("Error using get_nearest_token():  token_list must be all float values")
          print("  -> register name is '%s'" % reg_name)
          print("     input value is '%s'" % input_value)
          print("     token values are:")
          for token_val in token_list:
             print("        '%s'" % token_val)
          sys.exit()       

       index = (np.abs(token_float_list - input_value_float)).argmin()
       return(token_list[index])

    def get_matching_token(self, reg_name, input_value):
       # first confirm input_value is a string
       try:
          input_value + ''
       except TypeError:
          print("Error using get_nearest_token():  input value must be a string")
          print("  -> register name is '%s'" % reg_name)
          print("input value:")
          print(input_value)
          print("")
          sys.exit()

       token_list = self.get_token_names(reg_name,'register')
       for token_raw in token_list:
           token_raw_list = token_raw.split('__or__')
           for token in token_raw_list:
               if token == str(input_value):
                   return(token_raw)
       print("Warning using get_matching_token():  no matching value in token_list")
       print("  -> register name is '%s'" % reg_name)
       print("     input value is '%s'" % input_value)
       print("     token values are:")
       for token_val in token_list:
           print("        '%s'" % token_val)
       out_value = 'error_token_not_found'
       return(out_value)

    def get_token_names(self,reg_name,reg_type='register'):
       token_list = self.tokenmap.get_token_names(reg_name,reg_type)
       return(token_list)
   

    def min_formula_input(self,reg_name):
       reg_name_confirmed = self.tokenmap.find_reg_name(reg_name,'register')
       if len(reg_name_confirmed) == 0:
           print("\nError using min_formula_input() function: reg_name not found")
           print("  -> input reg_name is '%s'" % reg_name)
           sys.exit()
       inv_formula_list = self.tokenmap.get_inv_formula(reg_name,'register')
       if len(inv_formula_list) <= 0:
           print("\nError using min_formula_input() function: inv_formula not found")
           print("  -> input reg_name is '%s'" % reg_name)
           sys.exit()           
       out_min = self.tokenmap.get_token_map(reg_name,'register','min')
       return(out_min)

    def max_formula_input(self,reg_name):
       reg_name_confirmed = self.tokenmap.find_reg_name(reg_name,'register')
       if len(reg_name_confirmed) == 0:
           print("\nError using max_formula_input() function: reg_name not found")
           print("  -> input reg_name is '%s'" % reg_name)
           sys.exit()
       inv_formula_list = self.tokenmap.get_inv_formula(reg_name,'register')
       if len(inv_formula_list) <= 0:
           print("\nError using max_formula_input() function: inv_formula not found")
           print("  -> input reg_name is '%s'" % reg_name)
           sys.exit()           
       out_max = self.tokenmap.get_token_map(reg_name,'register','max')
       return(out_max)

    def setreg(self,reg_name_p, token_val):
       out_set = self.tokenmap.set_token_map(reg_name_p,'register',str(token_val))
       self.setreg_raw(reg_name_p, int(out_set))

    def getreg(self,reg_name_p):
       out_get = self.getreg_raw(reg_name_p)
       out_val = self.tokenmap.get_token_map(reg_name_p,'register',str(out_get))
       return(out_val)

    def setreg_raw(self,reg_name_p, data_val):
       error_code = self.reg_control_lib.SetReg(reg_name_p.encode('utf-8'), ctypes.c_int(data_val))
       if self.check_for_error(error_code) == 1:
           print("--> Error using setreg_raw() function")
           print("    Inputs:  reg_field '%s', data_val '%d'\n" % (reg_name_p, data_val))
           sys.exit()

    def getreg_raw(self,reg_name_p):
       out_int = ctypes.c_int(0)
       error_code = self.reg_control_lib.GetReg(reg_name_p.encode('utf-8'), ctypes.byref(out_int))
       if self.check_for_error(error_code) == 1:
           print("--> Error using getreg_raw() function")
           print("    Input:  reg_field '%s'\n" % (reg_name_p))
           sys.exit()
       return(out_int.value)

    def check_for_error(self,error_code):       
       DLL_OK = 0
       DLL_ERROR = -1

       if error_code == DLL_OK:
           return(0)
       elif error_code == DLL_ERROR:
           print("\nError code 'DLL_ERROR' encountered")
       else:
           print("\nError code unknown")
       return(1)


