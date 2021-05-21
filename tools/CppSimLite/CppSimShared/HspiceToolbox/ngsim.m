function [] = ngsim(parfile_name)

cppsim_home = getenv('CppSimHome');
if length(cppsim_home) == 0
   cppsim_home = getenv('CPPSIMHOME');
end
if length(cppsim_home) == 0
   home = getenv('HOME');
   default_cppsim_home = sprintf('%s/CppSim',home);
   if exist(default_cppsim_home,'dir') ~= 0
      cppsim_home = default_cppsim_home;
   else
      disp('Error running cppsim Matlab script:  environment variable');
      disp('   CPPSIMHOME is undefined');
      return
   end
end
cppsim_home = strrep(cppsim_home,'\','/'); 
cppsim_home = strrep(cppsim_home,'C:','c:'); 
cppsim_home = strrep(cppsim_home,' ',''); 

cppsimshared_home = getenv('CppSimSharedHome');
if length(cppsimshared_home) == 0
   cppsimshared_home = getenv('CPPSIMSHAREDHOME');
end
if length(cppsimshared_home) == 0
   default_cppsimshared_home = sprintf('%s/CppSimShared',cppsim_home);
   if exist(default_cppsimshared_home,'dir') ~= 0
      cppsimshared_home = default_cppsimshared_home;
   else
      disp('Error running cppsim Matlab script:  environment variable');
      disp('   CPPSIMSHAREDHOME is undefined');
      return
   end
end
cppsimshared_home = strrep(cppsimshared_home,'\','/'); 
cppsimshared_home = strrep(cppsimshared_home,'C:','c:'); 
cppsimshared_home = strrep(cppsimshared_home,' ',''); 

%% directory where sue.lib is placed
suelib_dir = sprintf('%s/Sue2',cppsim_home);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%% no changes required for stuff below  %%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% note:  directory name must correspond to cell name
%%        parent directory of cell directory must correspond to library name
cur_dir = pwd;
if ispc == 1
   i = findstr(lower(cur_dir),'\simruns\');
else
   i = findstr(lower(cur_dir),'/simruns/');
end
if length(i) < 1
   disp('Error:  you need to run the ngsim.m script in a directory');
   disp('    of form '' ...../SimRuns/Library_name/Cell_name''');
   disp('    -> in this case, you ran in directory:');
   disp(cur_dir); 
   return
end

library_cell = cur_dir(i+9:length(cur_dir));
if isempty(library_cell)
   disp('Error:  you need to run the ngsim.m script in a directory');
   disp('    of form '' ...../SimRuns/Library_name/Cell_name''');
   disp('    -> in this case, you ran in directory:');
   disp(cur_dir); 
   return
end

if ispc == 1
   [library_name,r] = strtok(library_cell,'\');
else
   [library_name,r] = strtok(library_cell,'/');
end
if isempty(r)
   disp('Error:  you need to run the ngsim.m script in a directory');
   disp('    of form '' ...../SimRuns/Library_name/Cell_name''');
   disp('    -> in this case, you ran in directory:');
   disp(cur_dir); 
   return
end

cell_name = r(2:length(r));

if nargin == 0
   sim_file = 'test.hspc';
else
   i = findstr(parfile_name,'.hspc');
   if length(i) < 1
      sim_file = sprintf('%s.hspc',parfile_name);
   else
      sim_file = parfile_name;
   end
end

disp_msg = sprintf('Library: %s,  Cell: %s,  Parameter file: %s',library_name,cell_name,sim_file);
disp(disp_msg)


disp('... netlisting ...');
if ispc == 1
   run_program = sprintf('%s/Sue2/bin/win32/sue_spice_netlister %s %s/sue.lib netlist.ngsim no_top',cppsimshared_home,cell_name,suelib_dir);
   [status] = dos(run_program);
else
   run_program = sprintf('%s/Sue2/bin/sue_spice_netlister %s %s/sue.lib netlist.ngsim no_top',cppsimshared_home,cell_name,suelib_dir);
   [status] = system(run_program);
end

if status ~= 0
   disp('************** ERROR:  exited NGspice run prematurely! ****************');
   return
end

disp('... running hspc ...');
if ispc == 1
   run_program = sprintf('%s/HspiceToolbox/HSPC/bin/win32/hspc netlist.ngsim simrun.sp %s.hspc ngspice',cppsimshared_home,strtok(sim_file,'.'));
   [status] = dos(run_program);
else
   run_program = sprintf('%s/HspiceToolbox/HSPC/bin/hspc netlist.ngsim simrun.sp %s.hspc ngspice',cppsimshared_home,strtok(sim_file,'.'));
   [status] = system(run_program);
end

if status ~= 0
   disp('************** ERROR:  exited NGspice run prematurely! ****************');
   return
end

disp('... running NGspice ...');
if ispc == 1
   ngspice_install_dir = sprintf('%s/NGspice/NGspice_win32',cppsimshared_home);
else
   if ismac == 1
      ngspice_install_dir = sprintf('%s/NGspice/NGspice_macosx',cppsimshared_home);
   else
      ngspice_install_dir = sprintf('%s/NGspice/NGspice_glnxa64',cppsimshared_home);
   end
end

run_program = sprintf('%s/bin/ngspice -b -r simrun.raw -o simrun.log simrun.sp',ngspice_install_dir);

if ispc == 1
   [status] = dos(run_program);
else
   [status] = system(run_program);
end

if ispc == 1
   run_program = sprintf('%s/Sue2/bin/win32/check_ngspice_logfile simrun.log',cppsimshared_home);
   [status_scratch] = dos(run_program);
else
   run_program = sprintf('%s/Sue2/bin/check_ngspice_logfile simrun.log',cppsimshared_home);
   [status_scratch] = system(run_program);
end

if ispc == 1
   run_program = sprintf('%s/Sue2/bin/win32/ngspice_add_op_to_log',cppsimshared_home);
   [status_scratch] = dos(run_program);
else
   run_program = sprintf('%s/Sue2/bin/ngspice_add_op_to_log',cppsimshared_home);
   [status_scratch] = system(run_program);
end

if status == 0
   disp(' ');
   disp('*************** NGspice run has completed ****************');
   return
end




