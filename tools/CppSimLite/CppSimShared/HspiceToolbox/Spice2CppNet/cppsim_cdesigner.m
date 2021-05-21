function [] = cppsim_cdesigner(parfile_name,no_netlist_flag)

synopsys_lib_file_dir = getenv('SYNOPSYS_LIB_FILE_DIR');
if length(synopsys_lib_file_dir) == 0
   disp('Error running cppsim_cdesigner Matlab script:  environment variable');
   disp('   SYNOPSYS_LIB_FILE_DIR is undefined');
   return
end
synopsys_lib_file_dir = strrep(synopsys_lib_file_dir,'\','/'); 
synopsys_lib_file_dir = strrep(synopsys_lib_file_dir,'C:','c:'); 
synopsys_lib_file_dir = strrep(synopsys_lib_file_dir,' ',''); 

cppsim_home = getenv('CppSimHome');
if length(cppsim_home) == 0
   cppsim_home = getenv('CPPSIMHOME');
end
if length(cppsim_home) == 0
   disp('Error running cppsim_cdesigner Matlab script:  environment variable');
   disp('   CPPSIMHOME is undefined');
   return
end
cppsim_home = strrep(cppsim_home,'\','/'); 
cppsim_home = strrep(cppsim_home,'C:','c:'); 
cppsim_home = strrep(cppsim_home,' ',''); 

cppsimshared_home = getenv('CppsimSharedHome');
if length(cppsimshared_home) == 0
   cppsimshared_home = getenv('CPPSIMSHAREDHOME');
end
if length(cppsimshared_home) == 0
   disp('Error running cppsim_cdesigner Matlab script:  environment variable');
   disp('   CPPSIMSHAREDHOME is undefined');
   return
end
cppsimshared_home = strrep(cppsimshared_home,'\','/'); 
cppsimshared_home = strrep(cppsimshared_home,'C:','c:'); 
cppsimshared_home = strrep(cppsimshared_home,' ',''); 

%% directory where sue.lib is placed
suelib_dir = sprintf('%s/Sue2',cppsim_home);

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
   disp('Error:  you need to run the cppsim_cdesigner.m script in a directory');
   disp('    of form '' ...../SimRuns/Library_name/Cell_name''');
   disp('    -> in this case, you ran in directory:');
   disp(cur_dir); 
   return
end
library_cell = cur_dir(i+9:length(cur_dir));
[library_name,r] = strtok(library_cell,'/');
cell_name = r(2:length(r));

if nargin == 0
   sim_file = 'test.par';
else
   i = findstr(parfile_name,'.par');
   if length(i) < 1
      sim_file = sprintf('%s.par',parfile_name);
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
   create_netlist_tcl_script(cppsim_home,cell_name,library_name);
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

disp('... running spice2cppnet ...');
if operating_system == 'Windows'
   run_program = sprintf('%s/HspiceToolbox/Spice2CppNet/bin/win32/spice2cppnet netlist.hsp %s/lib.defs %s/Netlist/netlist.cppsim',cppsimshared_home,synopsys_lib_file_dir,cppsim_home);
else
   run_program = sprintf('%s/HspiceToolbox/Spice2CppNet/bin/spice2cppnet netlist.hsp %s/lib.defs %s/Netlist/netlist.cppsim',cppsimshared_home,synopsys_lib_file_dir,cppsim_home);
end
[status] = system(run_program);

if status ~= 0
   disp('************** ERROR:  spice2cppnet program failed! ****************');
   return
end

if nargin == 0
   sim_file = 'test.par';
else
   i = findstr(parfile_name,'.par');
   if length(i) < 1
      sim_file = sprintf('%s.par',parfile_name);
   else
      sim_file = parfile_name;
   end
end

disp('... running net2code ...');

fid = fopen('./Makefile');
if fid >= 0
   fclose(fid);
   system('rm Makefile');
   system('rm *.o');
end

if operating_system == 'Windows'
   run_program = sprintf('%s/bin/win32/net2code -cpp %s',cppsimshared_home, sim_file);
else
   run_program = sprintf('%s/bin/net2code -cpp %s',cppsimshared_home, sim_file);
end
[status] = system(run_program);

if status ~= 0
   disp('************** ERROR:  net2code program failed! ****************');
   return
end

fid = fopen('./Makefile');
if fid < 0
   disp('************** ERROR:  net2code program failed! ****************');
else
   fclose(fid);
   disp('... compiling ...');
   system('make');
end



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [] = create_netlist_tcl_script(cppsimhome,top_cell,top_library)

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

fprintf(fid,'set netlist_dir %s/SimRuns/$TOP_LIBRARY/$TOP_CELL\n',cppsimhome);
fprintf(fid,'set netlist_obj [nl::getNetlisters HSPICE]\n');
fprintf(fid,'db::setAttr netlistEndProc -of $netlist_obj -value "hspc_netlistEndProc"\n\n');

fprintf(fid,'set cv [dm::findCellView "schematic" -cellName $TOP_CELL -libName $TOP_LIBRARY]\n');
fprintf(fid,'set netlist_result [nl::runNetlister $netlist_obj -cellView $cv -filePath "$netlist_dir/netlist.hsp" \\\n');
fprintf(fid,'			-viewSearchList "schematic symbol" -viewStopList "symbol"]\n');

fclose(fid);
