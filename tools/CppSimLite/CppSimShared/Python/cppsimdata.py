# cppsimdata.py
# written by Michael H. Perrott
# with minor modifications by Doug Pastorello to work with both Python 2.7 and Python 3.4
# available at www.cppsim.com as part of the CppSim package
# Copyright (c) 2013-2017 by Michael H. Perrott
# This file is disributed under the MIT license (see Copying file)



import ctypes as ct
import numpy as np
import sys
import os
import platform
import subprocess as sp
import contextlib
from scipy.signal import lfilter,welch



class CPPSIM_STORAGE_INFO(ct.Structure):
  _fields_ = [
   ('filename',ct.c_char_p),
   ('num_sigs',ct.c_int),
   ('num_samples',ct.c_int)
 ]

class CppSimData(object):
    def __init__(self, filename=None):
       if filename != None:
          self.storage_info = CPPSIM_STORAGE_INFO(filename.encode('UTF-8'),0,0)
       else:
          self.storage_info = CPPSIM_STORAGE_INFO('None'.encode('UTF-8'),0,0)
       self.err_msg = ct.create_string_buffer(1000)
       self.cur_sig_name = ct.create_string_buffer(1000)
       if sys.platform == 'darwin':
          home_dir = os.getenv("HOME")
          arch_val = platform.architecture()[0]
          cppsimdata_lib_file = home_dir + '/CppSim/CppSimShared/Python/macosx/cppsimdata_lib.so'
       elif sys.platform == 'win32':
          cppsimsharedhome = os.getenv("CPPSIMSHAREDHOME")
          if sys.maxsize == 2147483647:
             cppsimdata_lib_file = cppsimsharedhome + '/Python/win32/cppsimdata_lib.dll'
          else:
             cppsimdata_lib_file = cppsimsharedhome + '/Python/win64/cppsimdata_lib.dll'
       else:
          cppsimsharedhome = os.getenv("CPPSIMSHAREDHOME")
          arch_val = platform.architecture()[0]
          if arch_val == '64bit':
             cppsimdata_lib_file = cppsimsharedhome + '/Python/glnxa64/cppsimdata_lib.so'
          else:
             cppsimdata_lib_file = cppsimsharedhome + '/Python/glnx86/cppsimdata_lib.so'
       self.cppsimdata_lib = ct.CDLL(cppsimdata_lib_file)
       self.cppsimdata_lib.loadsig.argtypes = [ct.POINTER(CPPSIM_STORAGE_INFO), ct.c_char_p]
       self.cppsimdata_lib.lssig.argtypes = [ct.POINTER(CPPSIM_STORAGE_INFO), ct.c_char_p, ct.c_char_p]
       self.cppsimdata_lib.evalsig.argtypes = [ct.POINTER(CPPSIM_STORAGE_INFO), ct.c_char_p, ct.POINTER(ct.c_double), ct.c_char_p]
       self.cppsimdata_lib.initialize()
       if filename != None:
           error_flag = self.cppsimdata_lib.loadsig(ct.byref(self.storage_info),self.err_msg)
           if error_flag == 1:
              print(self.err_msg.value.decode('UTF-8'))
              sys.exit()

    def __repr__(self):
       return "File: '%s', num_samples = %d, num_sigs = %d"%(self.storage_info.filename, self.storage_info.num_samples, self.storage_info.num_sigs)
    def loadsig(self,filename):
       self.storage_info.filename = filename
       error_flag = self.cppsimdata_lib.loadsig(ct.byref(self.storage_info),self.err_msg)
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
       self.cppsimdata_lib.reset_cur_sig_count()
       for i in range(self.storage_info.num_sigs):
           error_flag = self.cppsimdata_lib.lssig(ct.byref(self.storage_info),self.cur_sig_name, self.err_msg)
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
       error_flag = self.cppsimdata_lib.evalsig(ct.byref(self.storage_info), sig_name, sig_data.ctypes.data_as(ct.POINTER(ct.c_double)),self.err_msg)
       if error_flag == 1:
            print(self.err_msg.value.decode('UTF-8'))
            sys.exit()
       return sig_data


def cppsim_unbuffer_for_print(status, stream='stdout'):
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

def cppsim(sim_file="test.par"):
   if sim_file.find('.par') < 0:
      sim_file = sim_file + '.par'

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
       print('Error running cppsim: you need to run this Python script')
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
       print('Error running cppsim: you need to run this Python script')
       print('  in a directory of form:')
       print('  ...../SimRuns/Library_name/Module_name')
       print('  -> in this case, you ran in directory:')
       print('    %s' % cur_dir)
       sys.exit()
   library_name = library_cell[0:i]
   cell_name = library_cell[i+1:1000]
   print("Running CppSim on module '%s' (Lib:'%s'):" % (cell_name, library_name))

   print("\n... netlisting ...\n")

   if sys.platform == 'win32':
       rp_base = '%s/Sue2/bin/win32/sue_cppsim_netlister' % (cppsimshared_home)
   else:
       rp_base = '%s/Sue2/bin/sue_cppsim_netlister' % (cppsimshared_home)
   rp_arg1 = cell_name
   rp_arg2 = '%s/Sue2/sue.lib' % (cppsim_home)
   rp_arg3 = '%s/Netlist/netlist.cppsim' % (cppsim_home)
   rp = [rp_base, rp_arg1, rp_arg2, rp_arg3]
   status = sp.Popen(rp, stdout=sp.PIPE, stderr=sp.STDOUT, universal_newlines=True) 
   for line in cppsim_unbuffer_for_print(status):
       print(line)

   if status.returncode != 0:
       print('************** ERROR:  exited CppSim run prematurely! ****************')
       sys.exit()

   print('\n... running net2code ...\n')

   if sys.platform == 'win32':
       rp_base = '%s/bin/win32/net2code' % (cppsimshared_home)
   else:
       rp_base = '%s/bin/net2code' % (cppsimshared_home)
   rp_arg1 = '-cpp'
   rp_arg2 = sim_file
   rp = [rp_base, rp_arg1, rp_arg2]
   status = sp.Popen(rp, stdout=sp.PIPE, stderr=sp.STDOUT, universal_newlines=True) 
   for line in cppsim_unbuffer_for_print(status):
       print(line)

   if status.returncode != 0:
       print('************** ERROR:  exited CppSim run prematurely! ****************')
       sys.exit()

   print('... compiling ...\n')

   if sys.platform == 'win32':
       rp_base = '%s/msys/bin/make' % (cppsimshared_home)
   else:
       rp_base = 'make'
   rp = [rp_base]
   status = sp.Popen(rp, stdout=sp.PIPE, stderr=sp.STDOUT, universal_newlines=True) 
   for line in cppsim_unbuffer_for_print(status):
       print(line)

   if status.returncode != 0:
       print('************** ERROR:  exited CppSim run prematurely! ****************')
       sys.exit()


# calculate phase noise: returns frequency (Hz) and specral density (dBc/Hz)
def calc_pll_phasenoise(noiseout,Ts):
   num_segments = 20;
   window_length = np.floor(noiseout.size/num_segments)
   Kv = 1.0
   phase = lfilter([Ts*2*np.pi*Kv],[1,-1],noiseout-np.mean(noiseout))
   # calculate L(f)
   f, Pxx = welch(phase,1/Ts,'hanning',2**16, None, None, 'constant', False, 'density',-1)

   # In Matlab:
   # [Pxx,f] = pwelch(phase,window_length,[],[],1/Ts,'twosided');
   # [Pxx,f] = psd(sqrt(Ts)*phase,2^16,1/Ts,2^16,'mean');
   Pxx_db = 10.0*np.log10(Pxx)
   return f, Pxx_db
