function [] = bdasim_fix_tilda(simrun_name)

%% note:  directory name must correspond to cell name
%%        parent directory of cell directory must correspond to library name
cur_dir = lower(pwd);
cur_dir = strrep(cur_dir,'\','/'); 
i = findstr(cur_dir,'/simruns/');
cur_dir = pwd;
cur_dir = strrep(cur_dir,'\','/'); 
if length(i) < 1
   disp('Error:  you need to run the bdasim_fix_tilda.m script in a directory');
   disp('    of form '' ...../SimRuns/Library_name/Cell_name''');
   disp('    -> in this case, you ran in directory:');
   disp(cur_dir); 
   return
end
library_cell = cur_dir(i+9:length(cur_dir));
[library_name,r] = strtok(library_cell,'/');
cell_name = r(2:length(r));

if nargin == 0
   sim_file = 'simrun.sp';
else
   i = findstr(simrun_name,'.sp');
   if length(i) < 1
      sim_file = sprintf('%s.sp',simrun_name);
   else
      sim_file = simrun_name;
   end
end

fid = fopen(sim_file,'r');
if (fid == -1)
     temp_string = sprintf('Error in ''bdasim_fix_tilda'':  file ''%s'' not found',sim_file);
     disp(temp_string);
     return;
end

temp_filename = sprintf('%s_temp',sim_file);
fod = fopen(temp_filename,'w');

while 1
   tline = fgets(fid);
   if ~ischar(tline)
     break
   end
   new_line = strrep(tline,'~','$HOME');   
   fprintf(fod,'%s',new_line);
end

fclose(fid);
fclose(fod);


copyfile(temp_filename,sim_file);
