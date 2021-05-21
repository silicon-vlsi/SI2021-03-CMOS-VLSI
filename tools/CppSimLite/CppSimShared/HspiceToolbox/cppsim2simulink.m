function [] = cppsim2simulink(parfile_name)

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
      disp('Error running cppsim2simulink Matlab script:  environment variable');
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
      disp('Error running cppsim2simulink Matlab script:  environment variable');
      disp('   CPPSIMSHAREDHOME is undefined');
      return
   end
end
cppsimshared_home = strrep(cppsimshared_home,'\','/'); 
cppsimshared_home = strrep(cppsimshared_home,'C:','c:'); 
cppsimshared_home = strrep(cppsimshared_home,' ',''); 

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

library_cell = cur_dir(i+9:length(cur_dir));
if isempty(library_cell)
   disp('Error:  you need to run the cppsim2simulink.m script in a directory');
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
   disp('Error:  you need to run the cppsim2simulink.m script in a directory');
   disp('    of form '' ...../SimRuns/Library_name/Cell_name''');
   disp('    -> in this case, you ran in directory:');
   disp(cur_dir); 
   return
end

cell_name = r(2:length(r));

if nargin == 0
   sim_file = 'test.par';
else
   sim_file = parfile_name;
end

disp('... netlisting ...');
if ispc == 1
    run_program = sprintf('%s/Sue2/bin/win32/sue_cppsim_netlister %s %s/Sue2/sue.lib %s/Netlist/netlist.cppsim',cppsimshared_home,cell_name,cppsim_home,cppsim_home);
    [status] = dos(run_program);
else
    run_program = sprintf('%s/Sue2/bin/sue_cppsim_netlister %s %s/Sue2/sue.lib %s/Netlist/netlist.cppsim',cppsimshared_home,cell_name,cppsim_home,cppsim_home);
    [status] = system(run_program);
end

if status ~= 0
   disp('************** ERROR:  exited CppSim run prematurely! ****************');
   return
end

disp('... running net2code ...');
if ispc == 1
   run_program = sprintf('%s/bin/win32/net2code -simulink %s',cppsimshared_home,sim_file);
   [status] = dos(run_program);
else
   run_program = sprintf('%s/bin/net2code -simulink %s',cppsimshared_home,sim_file);
   [status] = system(run_program);
end

if status ~= 0
   disp('************** ERROR:  exited CppSim run prematurely! ****************');
   return
end

disp('... compiling ...');

newpath = sprintf('%s/Matlab',pwd);
run_program = sprintf('%s/compile_%s_s',newpath,cell_name);

out_file = sprintf('%s/%s_s.mexw32',newpath,cell_name);
if exist(out_file)
   delete(out_file);
end
out_file = sprintf('%s/%s_s.dll',newpath,cell_name);
if exist(out_file)
   delete(out_file);
end

run(run_program);
out_file = sprintf('%s/%s_s.mexw32',newpath,cell_name);
out_file2 = sprintf('%s/%s_s.dll',newpath,cell_name);
if exist(out_file) || exist(out_file2)
     disp(' ')
     disp_message = sprintf('... adding path: ''%s'' ...',newpath);
     disp(disp_message);
     addpath(newpath);
     disp(' ')
     disp('****** Note:  to use S-Function within Simulink: ******')
     disp('1) Drag ''S-Function'' block from Simulink Library Browser to your model and double-click on it')
     disp_message = sprintf('2) Enter ''%s_s'' for ''S-function name:''',cell_name);
     disp(disp_message)
     disp('3) Enter appropriate values for ''S-function parameters:''')
     param_file = sprintf('%s/%s_s.txt',newpath,cell_name);
     fid_param = fopen(param_file);
     while 1
        tline = fgetl(fid_param);
        if ~ischar(tline), break, end
	disp(tline)
     end
     fclose(fid_param);
     disp('***********************************  Done!  **********************************')
end



function message = get_output_message(filename);

message = {};
fid = fopen(filename,'r');
if fid < 0
   return
end

while 1
   tline = fgetl(fid);
   if tline ~= -1
      if length(message) == 0
         message = tline;
      else
         message = strvcat(message,tline);
      end 
   end
   if ~ischar(tline)
      break;
   end
end
fclose(fid);

