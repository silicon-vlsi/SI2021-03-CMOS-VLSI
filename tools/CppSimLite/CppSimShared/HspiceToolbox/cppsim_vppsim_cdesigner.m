function [] = cppsim_vppsim_cdesigner(cppsim_vppsim_mode, parfile_name, netlist_mode)

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

if nargin ~= 3
   disp('Error:  you need to run the cppsim_vppsim_cdesigner.m script with 3 arguments');
   disp('        > cppsim_vppsim_cdesigner(cppsim_vppsim_mode, sim_file, netlist_mode);');
   return
end

i = findstr(parfile_name,'.par');
if length(i) < 1
   sim_file = sprintf('%s.par',parfile_name);
else
   sim_file = parfile_name;
end


disp_msg = sprintf('Library: %s,  Cell: %s,  Parameter file: %s',library_name,cell_name,sim_file);
disp(disp_msg)

if strcmp(netlist_mode,'no_net') == 1
   disp(' ');
   disp('**********************************************************************');
   disp('*** Note: NOT running SAE Netlister - must netlist from schematic  ***');
   disp('**********************************************************************');
   disp(' ');
else
   % Add check to the netlist in the future
   disp('... running SAE Netlister  ...');
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


disp('... running net2code ...');

fid = fopen('./Makefile');
if fid >= 0
   fclose(fid);
   system('rm Makefile');
   system('rm *.o');
end

if strcmp(cppsim_vppsim_mode,'cpp') == 1
   if operating_system == 'Windows'
      run_program = sprintf('%s/bin/win32/net2code -cpp %s',cppsimshared_home, sim_file);
   else
      run_program = sprintf('%s/bin/net2code -cpp %s',cppsimshared_home, sim_file);
   end
else
   if operating_system == 'Windows'
      run_program = sprintf('%s/bin/win32/net2code -vpp %s',cppsimshared_home, sim_file);
   else
      run_program = sprintf('%s/bin/net2code -vpp %s',cppsimshared_home, sim_file);
   end
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

fprintf(fid,'proc cppsim_add_conversion_module_to_netlist {libName cellName fid_netlist} {\n\n');

fprintf(fid,'set dmCellView [dm::getCellViews schematic -cellName $cellName -libName $libName]\n');
fprintf(fid,'if {[db::isEmpty $dmCellView]} {\n');
fprintf(fid,'    de::sendMessage "Error: conversion module ''$cellName'' (lib ''$libName'') schematic doesn''t exist" -severity error\n');
fprintf(fid,'    return 1\n');
fprintf(fid,'}\n');
fprintf(fid,'if { ![catch {set context [de::open $dmCellView -headless true -readOnly true]} result ]} {\n');
fprintf(fid,'    set conversion_module [db::getAttr topDesign -of $context]\n');
fprintf(fid,'    # puts "---> descend into module [db::getAttr cellName -of $conversion_module]"\n');
fprintf(fid,'    set return_val [cppsim_descend_into_module $conversion_module $fid_netlist]\n');
fprintf(fid,'    # puts "--------> returned! --------"\n');
fprintf(fid,'    if {$return_val == 1} {\n');
fprintf(fid,'        puts "Error: cannot descend into conversion module ''$cellName'' (lib ''$libName'')"\n');
fprintf(fid,'        return 1\n');
fprintf(fid,'    }\n');
fprintf(fid,'} else {\n');
fprintf(fid,'    puts "Error: cannot open context of conversion module ''$cellName'' (lib ''$libName'')"\n');
fprintf(fid,'    return 1\n');
fprintf(fid,'}\n');
fprintf(fid,'return 0\n');
fprintf(fid,'}\n\n');

fprintf(fid,'proc cppsim_descend_into_module {cur_module fid_netlist} {\n\n');
fprintf(fid,'   set module_name [db::getAttr cellName -of $cur_module]\n');
fprintf(fid,'   set lib_name [db::getAttr libName -of $cur_module]\n\n');

fprintf(fid,'   set cur_lib [db::getAttr lib -of $cur_module]\n');
fprintf(fid,'   set cur_lib_path [db::getAttr fullPath -of $cur_lib]\n');
fprintf(fid,'   set cur_module_path "$cur_lib_path/$module_name/schematic/sch.oa"\n');

fprintf(fid,'   #   puts "Entered module ''$module_name''"\n');
fprintf(fid,'   puts $fid_netlist "************** Module $module_name *****************"\n');
fprintf(fid,'   puts $fid_netlist "module: $module_name $lib_name"\n');
fprintf(fid,'   puts $fid_netlist "path: $cur_module_path"\n\n');

fprintf(fid,'   db::foreach cur_term [db::getTerms -of $cur_module] {\n');
fprintf(fid,'      set term_name [db::getAttr name -of $cur_term]\n');
fprintf(fid,'      set term_net [db::getAttr net -of $cur_term]\n');
fprintf(fid,'      set term_net_name [db::getAttr name -of $term_net]\n');
fprintf(fid,'      set term_type [db::getAttr termType -of $cur_term]\n');
fprintf(fid,'      if {$term_name != $term_net_name} {\n');
fprintf(fid,'           puts "Error in netlisting module ''$module_name'' (lib ''$lib_name'')"\n');
fprintf(fid,'           puts "   module terminal ''$term_name'' is connected a net with"\n');
fprintf(fid,'           puts "   a different name of ''$term_net_name''"\n');
fprintf(fid,'           puts "   -> Please correct your schematic for this module"\n');
fprintf(fid,'           hspc_error_dialog "Error: netlisting not completed due to terminal/net name mismatch" [gi::getActiveWindow]\n');
fprintf(fid,'           return 1\n');
fprintf(fid,'      }\n');
fprintf(fid,'      puts $fid_netlist "   module_terminal: $term_name $term_type"\n');
fprintf(fid,'   }\n\n');

fprintf(fid,'   db::foreach cur_inst [db::getInsts -of $cur_module] {\n');
fprintf(fid,'      if {[db::viewExists schematic -of $cur_inst] || [db::viewExists cppsim -of $cur_inst] || [db::viewExists verilog -of $cur_inst] || [db::viewExists functional -of $cur_inst] || [db::viewExists hspice -of $cur_inst] || [db::viewExists spectre -of $cur_inst]} {\n');
fprintf(fid,'	        set inst_name [db::getAttr name -of $cur_inst]\n');
fprintf(fid,'           set inst_module_name [db::getAttr cellName -of $cur_inst]\n');
fprintf(fid,'           set inst_module_lib_name [db::getAttr libName -of $cur_inst]\n');
fprintf(fid,'           puts $fid_netlist "instance: $inst_name $inst_module_name $inst_module_lib_name"\n');
fprintf(fid,'           db::foreach cur_term [db::getInstTerms -of $cur_inst] {\n');
fprintf(fid,'              set term_name [db::getAttr name -of $cur_term]\n');
fprintf(fid,'              set term_net [db::getAttr net -of $cur_term]\n');
fprintf(fid,'              set term_net_name [db::getAttr name -of $term_net]\n');
fprintf(fid,'              puts $fid_netlist "   instance_terminal: $term_name $term_net_name"\n');
fprintf(fid,'           }\n');
fprintf(fid,'           db::foreach cur_param [db::getParams -of $cur_inst] {\n');
fprintf(fid,'              set param_def [db::getAttr paramDef -of $cur_param]\n');
fprintf(fid,'	           set param_name [db::getAttr name -of $param_def]\n');
fprintf(fid,'	           set param_value [db::getAttr value -of $cur_param]\n');
fprintf(fid,'#	           set param_def_value [db::getAttr defValue -of $param_def]\n');
fprintf(fid,'#             puts "   instance_parameter: $param_name $param_value $param_def_value"\n');
fprintf(fid,'	           if {$param_name != "lxComponentType"} {\n');
fprintf(fid,'                 puts $fid_netlist "   instance_parameter: $param_name $param_value"\n');
fprintf(fid,'              }\n');
fprintf(fid,'           }\n');
fprintf(fid,'      }\n');
fprintf(fid,'   }\n\n');

fprintf(fid,'   db::foreach cur_inst [db::getInsts -of $cur_module] {\n');
fprintf(fid,'      set inst_module_name [db::getAttr cellName -of $cur_inst]\n');
fprintf(fid,'      set inst_module_lib_name [db::getAttr libName -of $cur_inst]\n');
fprintf(fid,'      if {[db::viewExists schematic -of $cur_inst]} {\n');
fprintf(fid,'           set dmCellView [dm::getCellViews schematic -cellName $inst_module_name -libName $inst_module_lib_name]\n');
fprintf(fid,'           if {[db::isEmpty $dmCellView]} {\n');
fprintf(fid,'              de::sendMessage "The $libName/$cellName/$viewName does not exist" -severity error\n');
fprintf(fid,'              return 1\n');
fprintf(fid,'           }\n');
fprintf(fid,'           if { ![catch {set context [de::open $dmCellView -headless true -readOnly true]} result ]} {\n');
fprintf(fid,'              set inst_module [db::getAttr topDesign -of $context]\n');
fprintf(fid,'	           # puts "---> descend into module [db::getAttr cellName -of $inst_module]"\n');
fprintf(fid,'	           set return_val [cppsim_descend_into_module $inst_module $fid_netlist]\n');
fprintf(fid,'              # puts "--------> returned! --------"\n');
fprintf(fid,'              if {$return_val == 1} {\n');
fprintf(fid,'                  return 1\n');
fprintf(fid,'	           }\n');
fprintf(fid,'           } else {\n');
fprintf(fid,'              return 1\n');
fprintf(fid,'           }\n');
fprintf(fid,'      } elseif {[db::viewExists cppsim -of $cur_inst] || [db::viewExists verilog -of $cur_inst] || [db::viewExists functional -of $cur_inst] || [db::viewExists hspice -of $cur_inst] || [db::viewExists spectre -of $cur_inst]} {\n');
fprintf(fid,'           set inst_module [db::getAttr master -of $cur_inst]\n');
fprintf(fid,'	        # puts "---> descend into module [db::getAttr cellName -of $inst_module]"\n');
fprintf(fid,'	        set return_val [cppsim_descend_into_module $inst_module $fid_netlist]\n');
fprintf(fid,'           # puts "--------> returned! --------"\n');
fprintf(fid,'           if {$return_val == 1} {\n');
fprintf(fid,'              return 1\n');
fprintf(fid,'	        }\n');
fprintf(fid,'      }\n');
fprintf(fid,'   }\n');
fprintf(fid,'   return 0\n');
fprintf(fid,'}\n\n');

fprintf(fid,'set CPPSIM_HOME %s\n',cppsimhome);
fprintf(fid,'set TOP_CELL %s\n',top_cell);
fprintf(fid,'set TOP_LIBRARY %s\n\n',top_library);

fprintf(fid,'set dmCellView [dm::getCellViews schematic -cellName $TOP_CELL -libName $TOP_LIBRARY]\n');
fprintf(fid,'if {[db::isEmpty $dmCellView]} {\n');
fprintf(fid,'    de::sendMessage "Schematic for $TOP_CELL (lib ''$TOP_LIBRARY'') does not exist" -severity error\n');
fprintf(fid,'    return\n');
fprintf(fid,'}\n');
fprintf(fid,'if { ![catch {set context [de::open $dmCellView -headless true -readOnly true]} result ]} {\n');
fprintf(fid,'    set top_module [db::getAttr topDesign -of $context]\n\n');
fprintf(fid,'} else {\n');
fprintf(fid,'    puts "Error:  can not open top cell ''$TOP_CELL'' (lib ''$TOP_LIBRARY'')"\n');
fprintf(fid,'    puts "--- Netlisting Exited with Errors ---"\n');
fprintf(fid,'    return\n');
fprintf(fid,'}\n\n');

fprintf(fid,'puts "--------- running CppSim netlister for ''$TOP_CELL'' (lib ''$TOP_LIBRARY'')  --------"\n\n');

fprintf(fid,'set netlist_dir "$CPPSIM_HOME/Netlist"\n');
fprintf(fid,'set netlist_filename "$netlist_dir/netlist.cppsim"\n\n');

fprintf(fid,'if {[file isdirectory $netlist_dir] == 0} {\n');
fprintf(fid,'    puts "Error:  You must create CppSim netlist directory: $netlist_dir"\n');
fprintf(fid,'    puts "--- Netlisting Exited with Errors ---"\n');
fprintf(fid,'    return\n');
fprintf(fid,'}\n\n');

fprintf(fid,'if {[file isfile $netlist_filename] != 0} {\n');
fprintf(fid,'    if {[catch {file delete $netlist_filename} mssg]} {\n');
fprintf(fid,'        puts "Cannot delete netlist file: $netlist_filename"\n');
fprintf(fid,'        puts "--- Netlisting Exited with Errors ---"\n');
fprintf(fid,'        return\n');
fprintf(fid,'    }\n');
fprintf(fid,'}\n\n');

fprintf(fid,'####### Open netlist file\n');
fprintf(fid,'if {[catch "set fid_netlist \\[open $netlist_filename a\\]" err]} {\n');
fprintf(fid,'    puts "Error: cannot create netlist file: $netlist_filename"\n');
fprintf(fid,'    return\n');
fprintf(fid,'}\n\n');

fprintf(fid,'puts $fid_netlist "***** CppSim Netlist for Cell ''$TOP_CELL'' *****"\n');
fprintf(fid,'puts $fid_netlist " "\n');
fprintf(fid,'set return_val [cppsim_descend_into_module $top_module $fid_netlist]\n\n');

fprintf(fid,'################### Add conversion modules to netlist ###################\n');
fprintf(fid,'set return_val [cppsim_add_conversion_module_to_netlist "CppSimModules" "cpp_internal_double_interp_to_bool_convert" $fid_netlist]\n');
fprintf(fid,'if {$return_val == 1} {\n');
fprintf(fid,'    return\n');
fprintf(fid,'}\n');
fprintf(fid,'set return_val [cppsim_add_conversion_module_to_netlist "CppSimModules" "cpp_internal_double_to_int_convert" $fid_netlist]\n');
fprintf(fid,'if {$return_val == 1} {\n');
fprintf(fid,'    return\n');
fprintf(fid,'}\n');
fprintf(fid,'set return_val [cppsim_add_conversion_module_to_netlist "CppSimModules" "cpp_internal_int_to_double_convert" $fid_netlist]\n');
fprintf(fid,'if {$return_val == 1} {\n');
fprintf(fid,'    return\n');
fprintf(fid,'}\n');
fprintf(fid,'set return_val [cppsim_add_conversion_module_to_netlist "CppSimModules" "cpp_internal_bool_to_double_interp_convert" $fid_netlist]\n');
fprintf(fid,'if {$return_val == 1} {\n');
fprintf(fid,'    return\n');
fprintf(fid,'}\n');
fprintf(fid,'################ End of Adding Conversion Modules ################\n\n');

fprintf(fid,'if {[catch {close $fid_netlist} err]} {\n');
fprintf(fid,'    puts "Error: cannot close netlist file: $netlist_filename"\n');
fprintf(fid,'    return\n');
fprintf(fid,'}\n\n');

fprintf(fid,'if {$return_val == 1} {\n');
fprintf(fid,'    puts "Error:  CppSim netlisting failed"\n');
fprintf(fid,'    puts "--- Netlisting Exited with Errors ---"\n');
fprintf(fid,'    return\n');
fprintf(fid,'}\n\n');

fprintf(fid,'puts "--- CppSim netlisting successfully completed ---"\n');


fclose(fid);
