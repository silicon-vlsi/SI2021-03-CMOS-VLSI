from __future__ import division

# import cppsimdata module
import os
import sys

cppsimsharedhome = os.getenv("CPPSIMSHAREDHOME")
if cppsimsharedhome == None:
   home_loc = os.getenv("HOME")
   path_name = '%s/CppSim/CppSimShared/Python' % home_loc
   sys.path.append(path_name)
else:
   sys.path.append(cppsimsharedhome + '/Python')
from cppsimdata import *

def verilog_net():
   sim_file = 'test_ver_netlist.par'
   file = open(sim_file,'w')
   file.write('num_sim_steps: 10\n')
   file.write('Ts: 1e-9\n')
   file.close()
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
   rp_arg1 = '-vpp'
   rp_arg2 = sim_file
   rp = [rp_base, rp_arg1, rp_arg2]
   status = sp.Popen(rp, stdout=sp.PIPE, stderr=sp.STDOUT, universal_newlines=True) 
   for line in cppsim_unbuffer_for_print(status):
       print(line)

   if status.returncode != 0:
       print('************** ERROR:  exited CppSim run prematurely! ****************')
       sys.exit()

def modify_and_save_verilog_net():
   line_count = 0
   filename_in = 'test_ver_netlist_lib.v'
   filename_out = 'netlist.v'
   file_in = open(filename_in,'r')
   file_out = open(filename_out,'w')
   for line in file_in:
       line_count = line_count + 1
       if line_count > 182:
          file_out.write(line)
   file_in.close()
   file_out.close()

verilog_net()
modify_and_save_verilog_net()
