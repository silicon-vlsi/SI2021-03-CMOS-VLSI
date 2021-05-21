function [] = hspsim_cdesigner(parfile_name,no_netlist_flag)

synopsys_lib_file_dir = getenv('SYNOPSYS_LIB_FILE_DIR');
if length(synopsys_lib_file_dir) == 0
   disp('Error running hspsim_cdesigner Matlab script:  environment variable');
   disp('   SYNOPSYS_LIB_FILE_DIR is undefined');
   return
end
synopsys_lib_file_dir = strrep(synopsys_lib_file_dir,'\','/'); 
synopsys_lib_file_dir = strrep(synopsys_lib_file_dir,'C:','c:'); 
synopsys_lib_file_dir = strrep(synopsys_lib_file_dir,' ',''); 

hspc_home = getenv('Hspchome');
if length(hspc_home) == 0
   hspc_home = getenv('HSPCHOME');
end
if length(hspc_home) == 0
   disp('Error running hspsim_cdesigner Matlab script:  environment variable');
   disp('   HSPCHOME is undefined');
   return
end
hspc_home = strrep(hspc_home,'\','/'); 
hspc_home = strrep(hspc_home,'C:','c:'); 
hspc_home = strrep(hspc_home,' ',''); 

cppsimshared_home = getenv('CppsimSharedHome');
if length(cppsimshared_home) == 0
   cppsimshared_home = getenv('CPPSIMSHAREDHOME');
end
if length(cppsimshared_home) == 0
   disp('Error running hspsim_cdesigner Matlab script:  environment variable');
   disp('   CPPSIMSHAREDHOME is undefined');
   return
end
cppsimshared_home = strrep(cppsimshared_home,'\','/'); 
cppsimshared_home = strrep(cppsimshared_home,'C:','c:'); 
cppsimshared_home = strrep(cppsimshared_home,' ',''); 

%% directory where sue.lib is placed
suelib_dir = sprintf('%s/Sue2',hspc_home);

os_var = getenv('OS');
if length(os_var) == 0
   operating_system = 'Linux  ';
else
   operating_system = 'Windows';
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%% no changes required for stuff below  %%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% note:  directory name must correspond to cell name
%%        parent directory of cell directory must correspond to library name
cur_dir = lower(pwd);
cur_dir = strrep(cur_dir,'\','/'); 
i = findstr(cur_dir,'/simruns/');
cur_dir = pwd;
cur_dir = strrep(cur_dir,'\','/'); 
if length(i) < 1
   disp('Error:  you need to run the hspsim_cdesigner.m script in a directory');
   disp('    of form '' ...../SimRuns/Library_name/Cell_name''');
   disp('    -> in this case, you ran in directory:');
   disp(cur_dir); 
   return
end
library_cell = cur_dir(i+9:length(cur_dir));
[library_name,r] = strtok(library_cell,'/');
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

if nargin == 2
   disp(' ');
   disp('**********************************************************************');
   disp('*** Note: NOT running sae netlister - must netlist from schematic  ***');
   disp('**********************************************************************');
   disp(' ');
else
   % Add check to the netlist in the future
   disp('... running sae netlister  ...');
   create_netlist_tcl_script(hspc_home,cell_name,library_name);
   run_program = sprintf('sae_shell netlist_script.tcl -libDefFile %s/lib.defs -logName sae_info',synopsys_lib_file_dir);
   if exist('sae_info.log')
      delete('sae_info*');
   end
   delete('sae_info*');
   [status] = system(run_program);
   movefile('sae_info.log','simrun.lis');

   if status ~= 0
      disp('************** ERROR:  Netlister program failed! ****************');
      return
   end


   % files=dir(strcat(FullPathToFile,Filename));
   % last_modified=files.date;
end

disp('... running hspc ...');
if operating_system == 'Windows'
   run_program = sprintf('%s/HspiceToolbox/HSPC/bin/win32/hspc netlist.hsp simrun.sp %s.hspc',cppsimshared_home,strtok(sim_file,'.'));
else
   run_program = sprintf('%s/HspiceToolbox/HSPC/bin/hspc netlist.hsp simrun.sp %s.hspc',cppsimshared_home,strtok(sim_file,'.'));
end
[status] = system(run_program);

if status ~= 0
   disp('************** ERROR:  HSPC program failed! ****************');
   return
end

disp('... running hspice ...');

run_program = sprintf('hspice -i simrun.sp -o simrun');
[status] = system(run_program);

if status ~= 0
   disp('************** ERROR:  Hspice program failed! ****************');
   return
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5

function [] = create_netlist_tcl_script(hspc_home, top_cell,top_library)

fid = fopen('netlist_script.tcl','w');

fprintf(fid,'proc hspc_netlistEndProc {netlist_obj} {\n\n');
   
fprintf(fid,'set netlist_filename [db::getAttr fileName -of $netlist_obj]\n');
fprintf(fid,'set i [string last "/" $netlist_filename]\n');
fprintf(fid,'set netlist_dir [string range $netlist_filename 0 [expr $i - 1]]\n');
fprintf(fid,'set i [string last "/" $netlist_dir]\n');
fprintf(fid,'set TOP_CELL [string range $netlist_dir [expr $i + 1] end]\n');
fprintf(fid,'puts "Netlist Dir: $netlist_dir"\n');
fprintf(fid,'puts "Top Cell:  $TOP_CELL"\n\n');

fprintf(fid,'####### Open netlist file\n');
fprintf(fid,'if {[catch "set fid_main \\[open $netlist_filename a\\]" err]} {\n');
fprintf(fid,'   puts "Error: cannot create netlist file: $netlist_filename"\n');
fprintf(fid,'   return\n');
fprintf(fid,'}\n\n');

fprintf(fid,'# main module\n');
fprintf(fid,'foreach subdir [glob -directory "$netlist_dir/cnl" -type d *] {\n');
fprintf(fid,'    set i [string last "/" $subdir]\n');
fprintf(fid,'    set subdir_name [string range $subdir [expr $i + 1] end]\n');
fprintf(fid,'    if {[string compare $subdir_name $TOP_CELL] == 0} {\n');
fprintf(fid,'       if {[file exists "$subdir/netlist"] == 1} {\n');
fprintf(fid,'          if {[catch "set fid_sub \\[open $subdir/netlist r\\]" err]} {\n');
fprintf(fid,'              puts "Error: cannot open sub-netlist file: $subdir/netlist"\n');
fprintf(fid,'              return\n');
fprintf(fid,'          }\n');
fprintf(fid,'          while {[gets $fid_sub line] >= 0} {\n');
fprintf(fid,'              if {[string compare -length 7 $line "* View "] != 0} {\n');
fprintf(fid,'                 puts $fid_main $line\n');
fprintf(fid,'	      }\n');
fprintf(fid,'          }\n');
fprintf(fid,'          if {[catch {close $fid_sub} err]} {\n');
fprintf(fid,'              puts "Error: cannot close sub-netlist file: $subdir/netlist"\n');
fprintf(fid,'              return\n');
fprintf(fid,'          }\n');
fprintf(fid,'       }\n');
fprintf(fid,'    }\n');
fprintf(fid,'}\n\n');

fprintf(fid,'# sub-modules\n');
fprintf(fid,'foreach subdir [glob -directory "$netlist_dir/cnl" -type d *] {\n');
fprintf(fid,'    set i [string last "/" $subdir]\n');
fprintf(fid,'    set subdir_name [string range $subdir [expr $i + 1] end]\n');
fprintf(fid,'    if {[string compare $subdir_name $TOP_CELL] != 0} {\n');
fprintf(fid,'       if {[file exists "$subdir/netlist"] == 1} {\n');
fprintf(fid,'          if {[catch "set fid_sub \\[open $subdir/netlist r\\]" err]} {\n');
fprintf(fid,'              puts "Error: cannot open sub-netlist file: $subdir/netlist"\n');
fprintf(fid,'              return\n');
fprintf(fid,'          }\n');
fprintf(fid,'          while {[gets $fid_sub line] >= 0} {\n');
fprintf(fid,'              if {[string compare -length 7 $line "* View "] != 0} {\n');
fprintf(fid,'                 puts $fid_main $line\n');
fprintf(fid,'	      }\n');
fprintf(fid,'          }\n');
fprintf(fid,'          if {[catch {close $fid_sub} err]} {\n');
fprintf(fid,'              puts "Error: cannot close sub-netlist file: $subdir/netlist"\n');
fprintf(fid,'              return\n');
fprintf(fid,'          }\n');
fprintf(fid,'       }\n');
fprintf(fid,'    }\n');
fprintf(fid,'}\n\n');

fprintf(fid,'puts $fid_main "\n.end"\n');
fprintf(fid,'if {[catch {close $fid_main} err]} {\n');
fprintf(fid,'   puts "Error: cannot close netlist file: $netlist_filename"\n');
fprintf(fid,'   return\n');
fprintf(fid,'}\n');

fprintf(fid,'}\n\n');

fprintf(fid,'set TOP_CELL %s\n',top_cell);
fprintf(fid,'set TOP_LIBRARY %s\n\n',top_library);

fprintf(fid,'set netlist_dir %s/SimRuns/$TOP_LIBRARY/$TOP_CELL\n',hspc_home);
fprintf(fid,'set netlist_obj [nl::getNetlisters HSPICE]\n');
fprintf(fid,'db::setAttr netlistEndProc -of $netlist_obj -value "hspc_netlistEndProc"\n\n');

fprintf(fid,'set cv [dm::findCellView "schematic" -cellName $TOP_CELL -libName $TOP_LIBRARY]\n');
fprintf(fid,'set netlist_result [nl::runNetlister $netlist_obj -cellView $cv -filePath "$netlist_dir/netlist.hsp" \\\n');
fprintf(fid,'			-viewSearchList "schematic symbol" -viewStopList "symbol"]\n');

fclose(fid);
