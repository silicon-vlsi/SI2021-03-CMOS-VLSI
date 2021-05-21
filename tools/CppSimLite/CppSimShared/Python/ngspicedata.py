# ngspicedata.py
# written by Michael H. Perrott
# available at www.cppsim.com as part of the CppSim package
# Copyright (c) 2013 by Michael H. Perrott
# This file is distributed under the MIT license (see Copying file)

import ctypes as ct
import numpy as np
import sys
import os
import platform
import subprocess as sp
import contextlib
from scipy.signal import lfilter,welch
import pylab


class NGSPICE_STORAGE_INFO(ct.Structure):
  _fields_ = [
   ('filename',ct.c_char_p),
   ('num_sigs',ct.c_int),
   ('num_samples',ct.c_int),
   ('num_sig_vals',ct.c_int),
   ('analysis_type',ct.c_int),
   ('complex_data_flag',ct.c_int),
   ('operating_point_flag',ct.c_int),
 ]

class NgspiceData(object):
    def __init__(self, filename=None):
       if filename != None:
          self.storage_info = NGSPICE_STORAGE_INFO(filename.encode('UTF-8'),0,0)
       else:
          self.storage_info = NGSPICE_STORAGE_INFO('None'.encode('UTF-8'),0,0)
       self.err_msg = ct.create_string_buffer(1000)
       self.cur_sig_name = ct.create_string_buffer(1000)
       if sys.platform == 'darwin':
          home_dir = os.getenv("HOME")
          arch_val = platform.architecture()[0]
          ngspicedata_lib_file = home_dir + '/CppSim/CppSimShared/Python/macosx/ngspicedata_lib.so'
       elif sys.platform == 'win32':
          cppsimsharedhome = os.getenv("CPPSIMSHAREDHOME")
          if sys.maxsize == 2147483647:
             ngspicedata_lib_file = cppsimsharedhome + '/Python/win32/ngspicedata_lib.dll'
          else:
             ngspicedata_lib_file = cppsimsharedhome + '/Python/win64/ngspicedata_lib.dll'
       else:
          cppsimsharedhome = os.getenv("CPPSIMSHAREDHOME")
          arch_val = platform.architecture()[0]
          if arch_val == '64bit':
             ngspicedata_lib_file = cppsimsharedhome + '/Python/glnxa64/ngspicedata_lib.so'
          else:
             ngspicedata_lib_file = cppsimsharedhome + '/Python/glnx86/ngspicedata_lib.so'
       self.ngspicedata_lib = ct.CDLL(ngspicedata_lib_file)
       self.ngspicedata_lib.loadsig.argtypes = [ct.POINTER(NGSPICE_STORAGE_INFO), ct.c_char_p]
       self.ngspicedata_lib.lssig.argtypes = [ct.POINTER(NGSPICE_STORAGE_INFO), ct.c_char_p, ct.c_char_p]
       self.ngspicedata_lib.evalsig.argtypes = [ct.POINTER(NGSPICE_STORAGE_INFO), ct.c_char_p, ct.POINTER(ct.c_double), ct.c_char_p]
       self.ngspicedata_lib.evalsig_complex.argtypes = [ct.POINTER(NGSPICE_STORAGE_INFO), ct.c_char_p, ct.POINTER(ct.c_double), ct.POINTER(ct.c_double), ct.c_char_p]
       self.ngspicedata_lib.initialize()
       if filename != None:
           error_flag = self.ngspicedata_lib.loadsig(ct.byref(self.storage_info),self.err_msg)
           if error_flag == 1:
              print(self.err_msg.value.decode('UTF-8'))
              sys.exit()

    def __repr__(self):
       return "File: '%s', num_samples = %d, num_sigs = %d"%(self.storage_info.filename, self.storage_info.num_samples, self.storage_info.num_sigs)
    def loadsig(self,filename):
       self.storage_info.filename = filename
       error_flag = self.ngspicedata_lib.loadsig(ct.byref(self.storage_info),self.err_msg)
       if error_flag == 1:
           print(self.err_msg.value.decode('UTF-8'))
           sys.exit()
    def get_num_samples(self):
       return self.storage_info.num_samples
    def get_num_sigs(self):
       return self.storage_info.num_sigs
    def get_filename(self):
       return self.storage_info.filename
    def lssig(self,print_str_flag=None):
       sig_list = []
       self.ngspicedata_lib.reset_cur_sig_count()
       for i in list(range(self.storage_info.num_sigs)):
           error_flag = self.ngspicedata_lib.lssig(ct.byref(self.storage_info),self.cur_sig_name, self.err_msg)
           if error_flag == 1:
               print(self.err_msg.value.decode('UTF-8'))
               sys.exit()
           if print_str_flag == 'print':
               print('%d: %s' % (i,self.cur_sig_name.value.decode('UTF-8')))
           sig_list.append(self.cur_sig_name.value.decode('UTF-8'))
       return sig_list
    def evalsig(self,sig_name):
       # If the signal name is a string, convert it to a byte array for the interface
       if (type(sig_name) is str):
         sig_name = str.encode(sig_name)
       sig_data = np.zeros(self.storage_info.num_samples)
       if self.storage_info.complex_data_flag == 1 and sig_name != 'FREQUENCY':
           sig_data_imag = np.zeros(self.storage_info.num_samples)
           error_flag = self.ngspicedata_lib.evalsig_complex(ct.byref(self.storage_info), sig_name, sig_data.ctypes.data_as(ct.POINTER(ct.c_double)),sig_data_imag.ctypes.data_as(ct.POINTER(ct.c_double)),self.err_msg)
       else:
           error_flag = self.ngspicedata_lib.evalsig(ct.byref(self.storage_info), sig_name, sig_data.ctypes.data_as(ct.POINTER(ct.c_double)),self.err_msg)

       if error_flag == 1:
            print(self.err_msg.value.decode('UTF-8'))
            sys.exit()

       if self.storage_info.complex_data_flag == 1 and sig_name != 'FREQUENCY':
          return sig_data + 1j*sig_data_imag
       else:
          return sig_data

def ngspice_unbuffer_for_print(status, stream='stdout'):
    newline_chars = ['\r', '\n', '\r\n']
    stream = getattr(status, stream)
    with contextlib.closing(stream):
        while True:
            out = []
            last = stream.read(1)
            if last == '' and status.poll() is not None:
                break
            while last not in newline_chars:
                if last == '' and status.poll() is not None:
                    break
                out.append(last)
                last = stream.read(1)
            out = ''.join(out)
            yield out

def ngsim(sim_file="test.hspc"):
   if sim_file.endswith('.hspc') == False:
      sim_file = sim_file + '.hspc'

   cppsim_home = os.getenv('CppSimHome')
   if cppsim_home == None:
       cppsim_home = os.getenv('CPPSIMHOME')
   if cppsim_home == None:
       home = os.getenv('HOME')
       if sys.platform == 'win32':
          default_cppsim_home = "%s\\CppSim" % (home)
       else:
          default_cppsim_home = "%s/CppSim" % (home)
       if os.path.isdir(default_cppsim_home):
          cppsim_home = default_cppsim_home
       else:
          print('Error running cppsim from Python:  environment variable')
          print('    CPPSIMHOME is undefined')

   cppsimshared_home = os.getenv('CppSimSharedHome')
   if cppsimshared_home == None:
       cppsimshared_home = os.getenv('CPPSIMSHAREDHOME')
   if cppsimshared_home == None:
       if sys.platform == 'win32':
          default_cppsimshared_home = "%s\\CppSimShared" % (cppsim_home)
       else:
          default_cppsimshared_home = "%s/CppSimShared" % (cppsim_home)
       if os.path.isdir(default_cppsimshared_home):
          cppsimshared_home = default_cppsimshared_home
       else:
          print('Error running cppsim:  environment variable')
          print('    CPPSIMSHAREDHOME is undefined')

   # print('cppsimhome: %s' % cppsim_home)
   # print('cppsimsharedhome: %s' % cppsimshared_home)
   cur_dir = os.getcwd()
   if sys.platform == 'win32':
      i = cur_dir.lower().find('\\simruns\\')
   else:
      i = cur_dir.lower().find('/simruns/')

   if i < 0:
       print('Error running ngspice: you need to run this Python script')
       print('  in a directory of form:')
       if sys.platform == 'win32':
          print('  .....\\SimRuns\\Library_name\\Module_name')
       else:
          print('  ...../SimRuns/Library_name/Module_name')
       print('  -> in this case, you ran in directory:')
       print('    %s' % cur_dir)
       sys.exit()
   library_cell = cur_dir[i+9:1000]
   if sys.platform == 'win32':
      i = library_cell.find('\\')
   else:
      i = library_cell.find('/')
   if i < 0:
       print('Error running ngspice: you need to run this Python script')
       print('  in a directory of form:')
       print('  ...../SimRuns/Library_name/Module_name')
       print('  -> in this case, you ran in directory:')
       print('    %s' % cur_dir)
       sys.exit()
   library_name = library_cell[0:i]
   cell_name = library_cell[i+1:1000]
   print("Running Ngspice on module '%s' (Lib: '%s'):" % (cell_name, library_name))

   print("\n... netlisting ...\n")

   if sys.platform == 'win32':
       rp_base = '%s/Sue2/bin/win32/sue_spice_netlister' % (cppsimshared_home)
   else:
       rp_base = '%s/Sue2/bin/sue_spice_netlister' % (cppsimshared_home)
   rp_arg1 = cell_name
   rp_arg2 = '%s/Sue2/sue.lib' % (cppsim_home)
   rp_arg3 = 'netlist.ngsim' 
   rp_arg4 = 'no_top'
   rp = [rp_base, rp_arg1, rp_arg2, rp_arg3, rp_arg4]
   status = sp.Popen(rp, stdout=sp.PIPE, stderr=sp.STDOUT, universal_newlines=True) 
   for line in ngspice_unbuffer_for_print(status):
       print(line)

   if status.returncode != 0:
       print('************** ERROR:  exited Ngspice run prematurely! ****************')
       sys.exit()

   print("\n... running hspc ...\n")

   if sys.platform == 'win32':
       rp_base = '%s/HspiceToolbox/HSPC/bin/win32/hspc' % (cppsimshared_home)
   else:
       rp_base = '%s/HspiceToolbox/HSPC/bin/hspc' % (cppsimshared_home)
   rp_arg1 = 'netlist.ngsim'
   rp_arg2 = 'simrun.sp'
   rp_arg3 = sim_file
   rp_arg4 = 'ngspice'
   rp = [rp_base, rp_arg1, rp_arg2, rp_arg3, rp_arg4]
   status = sp.Popen(rp, stdout=sp.PIPE, stderr=sp.STDOUT, universal_newlines=True) 
   for line in ngspice_unbuffer_for_print(status):
       print(line)

   if status.returncode != 0:
       print('************** ERROR:  exited Ngspice run prematurely! ****************')
       sys.exit()

   print('... running ngspice ...\n')

   if sys.platform == 'win32':
       rp_base = '%s/NGspice/NGspice_win32/bin/ngspice' % (cppsimshared_home)
   elif sys.platform == 'darwin':
       rp_base = '%s/NGspice/NGspice_macosx/bin/ngspice' % (cppsimshared_home)
   else:
       rp_base = '%s/NGspice/NGspice_glnxa64/bin/ngspice' % (cppsimshared_home)

   rp_arg1 = '-b'
   rp_arg2 = '-r'
   rp_arg3 = 'simrun.raw'
   rp_arg4 = '-o'
   rp_arg5 = 'simrun.log'
   rp_arg6 = 'simrun.sp'
   rp = [rp_base, rp_arg1, rp_arg2, rp_arg3, rp_arg4, rp_arg5, rp_arg6]

   status = sp.Popen(rp, stdout=sp.PIPE, stderr=sp.STDOUT, universal_newlines=True) 
   for line in ngspice_unbuffer_for_print(status):
       print(line)

   #########

   if sys.platform == 'win32':
       rp_base = '%s/Sue2/bin/win32/check_ngspice_logfile' % (cppsimshared_home)
   else:
       rp_base = '%s/Sue2/bin/check_ngspice_logfile' % (cppsimshared_home)

   rp_arg1 = 'simrun.log'
   rp = [rp_base, rp_arg1]

   status_scatch = sp.Popen(rp, stdout=sp.PIPE, stderr=sp.STDOUT, universal_newlines=True) 

   #########

   if sys.platform == 'win32':
       rp_base = '%s/Sue2/bin/win32/ngspice_add_op_to_log' % (cppsimshared_home)
   else:
       rp_base = '%s/Sue2/bin/ngspice_add_op_to_log' % (cppsimshared_home)

   rp = [rp_base]

   status_scratch = sp.Popen(rp, stdout=sp.PIPE, stderr=sp.STDOUT, universal_newlines=True) 

   if status.returncode == 0:
       print('************** Ngspice run has completed ****************')




def hspc_set_param(param_name, param_value_raw, sim_file='test.hspc'):

   if sim_file.endswith('.hspc') == False:
      sim_file = sim_file + '.hspc'

   param_value = str(param_value_raw)
      
   flag = 0
   with open(sim_file,'r') as f:
      line_set = f.readlines()
      f.close()
      flag = 1

   if flag == 0:
      print("Error running 'hspc_set_param':  can't open file '%s'" % (sim_file))
      sys.exit()

   flag = 0
   count = 0
   for line in line_set:
      words = line.split()
      if len(words) > 0:
          if words[0] == '.param':
             # print('%d: %s' % (count, line))
             r = line
             line_words = []
             while True:
                strtokens = r.partition('=')
                if strtokens[0] == '' or strtokens[2] == '':
                   break
                s = strtokens[0].split()
                line_words.extend(s)
                line_words.extend(strtokens[1])
                var = s[-1]
                if var == param_name:
                    s = strtokens[2].split()
                    s[0] = param_value   
                    line_words.extend(s)
                    line_words.extend('\n')
                    flag = 1
                    print("-> Changing parameter '%s' to value '%s' in file '%s'" % (param_name, param_value, sim_file))
                    break
                else:
                    r = strtokens[2]
             if flag == 1:
                line_set[count] = ' '.join(line_words)
                break
      count = count + 1

   if flag == 1:
       flag = 0
       with open(sim_file,'w') as f:
           f.writelines(line_set)
           f.close()
           flag = 1
       if flag == 0:
          print("Error running 'hspc_set_param':  can't write to file '%s'" % (sim_file))
          sys.exit()
   else:
       print("Error running 'hspce_set_param':  could not find parameter '%s'" % (param_name))

   return(flag)

def hspc_addline(new_line, sim_file='test.hspc'):

   if sim_file.endswith('.hspc') == False:
      sim_file = sim_file + '.hspc'

   flag = 0
   with open(sim_file,'r') as f:
      line_set = f.readlines()
      f.close()
      flag = 1

   if flag == 0:
      print("Error running 'hspc_addline':  can't open file '%s'" % (sim_file))
      sys.exit()

   found_flag = 0
   flag = 0
   out_line_set = []
   for line in line_set:
      if line.startswith("**** added line below using 'hspc_addline' function ****") or line.startswith("**** added line below using 'hspc_addline_continued' function ****"):
          found_flag = 1
          continue
      if found_flag == 1:
          found_flag = 2
          continue
      if found_flag == 2:
          found_flag = 0
          if len(line) == 1:
             continue

      if flag == 0:
          words = line.split()
          if len(words) > 0 and found_flag == 0:
             if words[0] == '.param' or words[0] == '.temp' or words[0] == '.options' or words[0] == '.tran' or words[0] == '.dc' or words[0] == '.ac' or words[0] == '.noise' or words[0] == '.probe':
                 flag = 1
                 out_line_set.append(line + '\n')
                 print("-> Adding line '%s' in file '%s'" % (new_line, sim_file))
                 out_line_set.append("**** added line below using 'hspc_addline' function ****\n")
                 out_line_set.append(new_line + '\n')
                 continue

      out_line_set.append(line)

   if flag == 1:
       flag = 0
       with open(sim_file,'w') as f:
           f.writelines(out_line_set)
           f.close()
           flag = 1
       if flag == 0:
          print("Error running 'hspc_addline':  can't write to file '%s'" % (sim_file))
          sys.exit()
   else:
       print("Error: did not add line '%s' in file '%s'" % (new_line, sim_file))
   return(flag)


def hspc_addline_continued(new_line, sim_file='test.hspc'):

   if sim_file.endswith('.hspc') == False:
      sim_file = sim_file + '.hspc'

   flag = 0
   with open(sim_file,'r') as f:
      line_set = f.readlines()
      f.close()
      flag = 1

   if flag == 0:
      print("Error running 'hspc_addline_continued':  can't open file '%s'" % (sim_file))
      sys.exit()

   found_flag = 0
   flag = 0
   out_line_set = []
   for line in line_set:
      if line.startswith("**** added line below using 'hspc_addline' function ****") or line.startswith("**** added line below using 'hspc_addline_continued' function ****"):
          flag = 1
          found_flag = 1
          out_line_set.append(line)
          continue
      if found_flag == 1:
          found_flag = 2
          out_line_set.append(line)
          continue
      if found_flag == 2:
          found_flag = 0
          if len(line) == 1:
             out_line_set.append(line)
             continue

      if flag == 1:
          flag = 2
          print("-> Adding line '%s' in file '%s'" % (new_line, sim_file))
          out_line_set.append("**** added line below using 'hspc_addline_continued' function ****\n")
          out_line_set.append(new_line + '\n' + '\n')
          out_line_set.append(line)
          continue

      out_line_set.append(line)

   if flag == 2:
       flag = 0
       with open(sim_file,'w') as f:
           f.writelines(out_line_set)
           f.close()
           flag = 1
       if flag == 0:
          print("Error running 'hspc_addline_continued':  can't write to file '%s'" % (sim_file))
          sys.exit()
   else:
       print("Error: did not add line '%s' in file '%s'" % (new_line, sim_file))
       print("-> be sure that you run 'hspc_addline' before 'hpsc_addline_continued'")
   return(flag)


# eyesig() developed with the help of Sanquan Song
def eyesig(period,start_off,time,data):

    ind = 0
    while time[ind] < start_off:
        ind = ind + 1
    
    fig = pylab.figure(1)
    fig.clf()
    
    ind_end = ind
    ind_start = ind
    i = 0         
    while ind_end+1 < len(data):
        while ind_end+1 < len(data): 
            if time[ind_end+1] < time[ind_start]+i*period:
                ind_end = ind_end + 1
            else:
                pylab.plot(time[ind:ind_end]-time[ind_start]-(i-1)*period,data[ind:ind_end],'b')
                i = i + 1
                ind = ind_end+1
                
                break
        
    pylab.grid(True)
    pylab.xlabel('Time (seconds)');
    pylab.title('Eye Diagram');
    figa = fig.gca()
    figa.set_xlim(0,period)
    fig.show()
