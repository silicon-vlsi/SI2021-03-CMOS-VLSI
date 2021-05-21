function [] = bdasim(parfile_name)

cppsimshared_home = getenv('CppsimSharedHome');
if length(cppsimshared_home) == 0
   cppsimshared_home = getenv('CPPSIMSHAREDHOME');
end
if length(cppsimshared_home) == 0
   disp('Error running bdasim_cdesigner Matlab script:  environment variable');
   disp('   CPPSIMSHAREDHOME is undefined');
   return
end
cppsimshared_home = strrep(cppsimshared_home,'\','/'); 
cppsimshared_home = strrep(cppsimshared_home,'C:','c:'); 
cppsimshared_home = strrep(cppsimshared_home,' ',''); 


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%% no changes required for stuff below  %%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% note:  directory name must correspond to cell name
%%        parent directory of cell directory must correspond to library name
cur_dir = lower(pwd);
cur_dir = strrep(cur_dir,'\','/'); 
i = findstr(cur_dir,'/simulation/');
cur_dir = pwd;
cur_dir = strrep(cur_dir,'\','/'); 
if length(i) < 1
   disp('Error:  you need to run the bdasim.m script in a directory');
   disp('    of form '' ...../simulation/Library_name/Cell_name''');
   disp('    -> in this case, you ran in directory:');
   disp(cur_dir); 
   return
end
library_cell = cur_dir(i+12:length(cur_dir));
[library_name,r] = strtok(library_cell,'/');
[cell_name,r] = strtok(r,'/');
% cell_name = r(2:length(r));

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

disp('**********************************************************************');
disp('***  Note: NOT running netlister - assuming netlist is up-to-date  ***');
disp('**********************************************************************');

hspc_file = sprintf('%s.hspc',strtok(sim_file,'.'));
if exist(hspc_file) == 0
   fid = fopen(hspc_file,'w');
   if fid < 0
      disp_msg = sprintf('Error:  can''t create new sim file ''%s''',hspc_file);
      disp(disp_msg);
      return;
   end

   fprintf(fid,'** BDA Simulation of ''%s'' (Lib: ''%s'')\n\n',cell_name,library_name);

   fprintf(fid,'***** Note:   to understand non-BDA commands below,\n');
   fprintf(fid,'*****         look at HSPC manual at http://www.cppsim.com/manuals\n\n');

   fprintf(fid,'***** Parameters for Calculation of Diffusion Regions for CMOS Process\n');
   fprintf(fid,'> set_mode_diff geo\n');
   fprintf(fid,'> set_hdout .66u\n');
   fprintf(fid,'> set_hdin .74u\n');
   fprintf(fid,'> use_four_sided_perimeter\n\n');

   fprintf(fid,'***** Transistor Model File\n');
   fprintf(fid,'***** Specify appropriate CMOS and Bipolar model files below\n');
   fprintf(fid,'.include ''/tool/cad/models/tsmc55gp/physical/spectre/sl_tsmc55gp.scs''\n\n');
%   fprintf(fid,'.include ''../../../SpiceModels/bipolar.mod''\n');

   fprintf(fid,'***** Temperature\n');
   fprintf(fid,'.temp 25\n\n');

   fprintf(fid,'***** Parameters\n');
   fprintf(fid,'.param vsupply=1.3\n\n');

   fprintf(fid,'***** DC Voltage Sources\n');
   fprintf(fid,'Vsup vdd 0 ''vsupply''\n');
   fprintf(fid,'.global vdd gnd\n\n');

   fprintf(fid,'***** Simulation Options\n');
   fprintf(fid,'.options post=1 delmax=5p relv=1e-6 reli=1e-6 relmos=1e-6 method=gear\n\n');

   fprintf(fid,'***** Simulation Parameters (.tran or .ac or .dc statements)\n');
   fprintf(fid,'.tran 5p 40n\n');
   fprintf(fid,'** .dc Vsup 0 ''vsupply'' 0.1\n');
   fprintf(fid,'** .ac lin 1000 100k 10Meg Vsup\n\n');

   fprintf(fid,'***** Have operating point information sent to output file\n');
   fprintf(fid,'.op\n\n');

   fprintf(fid,'****** Digital Input Stimulus (remove lines below if you don''t want this)\n');
   fprintf(fid,'*** Timing Statement:  % timing delay rise/fall_time period vlow vhigh\n');
   fprintf(fid,'* > timing 0.0n .2n [1/1e9] 0 vsupply\n');
   fprintf(fid,'*** Digital Input:  % input nodename [set 0 1 1 0 1 ... R]\n');
   fprintf(fid,'* > input in [set 0 0 1 1 1 0 1 1 0 0 0 1 0 R]\n\n');

   fprintf(fid,'****** Analog Input Stimulus\n');
   fprintf(fid,'*** Simply add voltage or current sources as appropriate\n\n');

   fprintf(fid,'****** Selectively Probe Signals (uncomment below if you want this)\n');
   fprintf(fid,'*.probe in out\n');

   fclose(fid);

   disp_msg = sprintf('New simulation file ''%s'' created - type ''edit %s''\n',hspc_file,hspc_file);   
   disp(disp_msg);
   return
end

disp('... running hspc ...');

run_program = sprintf('%s/HspiceToolbox/HSPC/bin/hspc %s.scs simrun.sp %s.hspc',cppsimshared_home,cell_name,strtok(sim_file,'.'));

[status] = system(run_program);

if status ~= 0
   disp('************** ERROR:  HSPC program failed! ****************');
   return
end

% bdasim_fix_tilda('simrun.sp');

disp('... running BDA Analog Fast Spice ...');

run_program = sprintf('bsub afs -i simrun.sp -o simrun -l simrun.log -f hspicebin');
[status] = system(run_program);

if status ~= 0
   disp('************** ERROR:  BDA Analog Fast Spice program failed! ****************');
   return
end
