/**************************************************************************
 * FILE : sue_general_netlister.cpp
 * 
 * AUTHOR : Mike Perrott
 * 
 * DESCRIPTION : creates Hspice and CppSim netlists from SUE files.
 ************************************************************************/

/**************************************************************************
Copyright (c) 2004 Michael H Perrott
All rights reserved.


Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in
all copies of this software.

IN NO EVENT SHALL MICHAEL H PERROTT BE LIABLE TO ANY PARTY FOR DIRECT,
INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF MICHAEL H
PERROTT HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MICHAEL H PERROTT SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THE SOFTWARE PROVIDED
HEREUNDER IS ON AN "AS IS" BASIS, AND MICHAEL H PERROTT HAS NO
OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
MODIFICATIONS.
**************************************************************************/

#include "sue2_common_code.h"

void determine_simruns_directory(char *simruns_dir_name);
void extract_sue_library_name(char *sue_file_name, char *sue_library_name);
void extract_cadencelib_dir_name(char *sue_file_name, char *cppsim_library_name);
MODULE *mark_active_modules(MODULE *first_module, char *top_module_name);
void determine_active_modules(MODULE *first_module, char *top_sue_module_name,
                              char *control_string, int run_in_lite_mode_flag);
void create_archive_directory(char *simruns_base_dir_name,
	    char *suelib_base_dir_name, char *cadencelib_base_dir_name);
void recursively_delete_directory(char *cur_dir_name);
void copy_file(char *infile_name, char *outfile_name);
void recursively_copy_directory(char *cur_dir_name, char *cur_archive_dir_name);
void recursively_copy_simruns_directory(char *cur_dir_name, char *cur_archive_dir_name);
int check_for_noncopy_simruns_file(char *file_name);
void copy_modules_to_cadence_directories(MODULE *first_module,
       char *code_filename, char *base_cadence_dir);
int extract_code_arguments(LINE *line_set, char *filename, int linecount, ARGUMENT *args);


int main(int argc,char *argv[])
{
char top_sue_module_name[MAX_CHAR_LENGTH];
char netlist_filename[MAX_CHAR_LENGTH];
PARAMETER *param_list;
MODULE *first_module, *top_module, *cur_module;
DIR *dir;
FILE *fp;
int i, j, old_version_flag;

char simruns_dir_name[MAX_CHAR_LENGTH];
char cadencelib_dir_name[MAX_CHAR_LENGTH];
char cur_file_name[MAX_CHAR_LENGTH];
char archive_simruns_base_dir_name[MAX_CHAR_LENGTH];
char archive_suelib_base_dir_name[MAX_CHAR_LENGTH];
char archive_cadencelib_base_dir_name[MAX_CHAR_LENGTH];
char cur_archive_dir_name[MAX_CHAR_LENGTH];
char cur_archive_file_name[MAX_CHAR_LENGTH];
char cur_dir_name[MAX_CHAR_LENGTH];
char cur_base_dir_name[MAX_CHAR_LENGTH];
char base_dir[MAX_CHAR_LENGTH];
char sue_lib_filename[MAX_CHAR_LENGTH];
char commoncode_dir_name[MAX_CHAR_LENGTH];
char modules_par_filename[MAX_CHAR_LENGTH];
int run_in_lite_mode_flag;

if (argc < 4)
  {
   printf("error: need at least three arguments!\n");
   printf("  export_cppsim_library sue_top_module sue_lib_file all/single (lite)\n");
   exit(1);
  }
if (argc == 4)
   run_in_lite_mode_flag = 0;
else
   run_in_lite_mode_flag = 1;


strcpy(top_sue_module_name,argv[1]);
for (i = 0; top_sue_module_name[i] != '\0'; i++)
  {
   if (strncmp(&top_sue_module_name[i],".sue",4) == 0)
      {
	top_sue_module_name[i] = '\0';
        break;
      }
  }

param_list = init_parameter();


//// extract modules
printf("\n... Extracting modules for export ...\n\n");
fflush(stdout);

first_module = extract_sue_modules_oldversion(argv[2],param_list);
if (first_module == NULL)
  {
   old_version_flag = 0;
   first_module = extract_sue_modules(argv[2],param_list);
   warn_about_duplicate_modules(first_module);
  }
 else
   {
   old_version_flag = 1;
   warn_about_duplicate_modules_no_exit(first_module);
   }
// print_keyinfo_modules(first_module); exit(1);



//// process modules
if (run_in_lite_mode_flag == 0)
   {
    connect_sue_instances_to_modules(first_module);
    trim_node_naming_instances_from_modules(first_module);
    first_module = trim_node_naming_modules(first_module);
   }
//print_keyinfo_modules(first_module);

determine_active_modules(first_module, top_sue_module_name, argv[3],
                         run_in_lite_mode_flag);

printf("\n... Clearing out Import_Export directory ...\n\n");
fflush(stdout);
create_archive_directory(archive_simruns_base_dir_name,
       archive_suelib_base_dir_name, archive_cadencelib_base_dir_name);

printf("\n... Copying files to Import_Export directory ...\n\n");
fflush(stdout);
if (old_version_flag == 1)
   {
   sprintf(sue_lib_filename,"%s",argv[2]);
   for (i = 0; sue_lib_filename[i] != '\0'; i++);
   for (    ; sue_lib_filename[i] != '/' && sue_lib_filename[i] != '\\'; i--);
   sue_lib_filename[i] = '\0';
   for (    ; sue_lib_filename[i] != '/' && sue_lib_filename[i] != '\\'; i--);
   for (j = 0; j < i; j++)
      {
       base_dir[j] = sue_lib_filename[j];
      }
   base_dir[j] = '\0';

   sprintf(simruns_dir_name,"%s/SimRuns",base_dir);
   if ((dir = opendir(simruns_dir_name)) == NULL) 
      {
	printf("error in 'cppsim exporter':\n");
        printf("   cannot open SimRuns directory:\n");
        printf("   %s\n",simruns_dir_name);
        exit(1);
      }
   else
      closedir(dir);

   sprintf(commoncode_dir_name,"%s/CommonCode",base_dir);
   if ((dir = opendir(commoncode_dir_name)) == NULL) 
      {
	printf("error in 'cppsim exporter':\n");
        printf("   cannot open CommonCode directory:\n");
        printf("   %s\n",commoncode_dir_name);
        exit(1);
      }
   else
      closedir(dir);

   sprintf(modules_par_filename,"%s/modules.par",commoncode_dir_name);

   cur_module = first_module;
   while (cur_module != NULL)
      {
       if (cur_module->active_sue_module_flag == 1)
         {
         // copy SueLib files
         sprintf(cur_archive_dir_name,"%s/%s",
		archive_suelib_base_dir_name,cur_module->library_path);
         if ((dir = opendir(cur_archive_dir_name)) == NULL) 
            {
            if (makedir(cur_archive_dir_name) != 0)
              {
               printf("error in 'cppsim exporter':\n");
               printf("     cannot create Import_Export subdirectory:\n");
               printf("     '%s'\n",cur_archive_dir_name);
               exit(1);
              }
	    }
         else
	    closedir(dir);

         sprintf(cur_archive_file_name,"%s/%s.sue",
	       cur_archive_dir_name,cur_module->name);
         copy_file(cur_module->full_filename,cur_archive_file_name);

         //////  copy SimRun files /////

         if ((dir = opendir(archive_simruns_base_dir_name)) == NULL) 
            {
	    printf("error in 'cppsim exporter':  can't open Import_Export directory for SimRuns directory:\n");
            printf("   '%s'\n",archive_simruns_base_dir_name);
            exit(1);
	    }
         else
	    closedir(dir);

         sprintf(cur_dir_name,"%s/%s",simruns_dir_name,
                 cur_module->library_path);
         sprintf(cur_archive_dir_name,"%s/%s",archive_simruns_base_dir_name,
	         cur_module->library_path);

         if ((dir = opendir(cur_dir_name)) != NULL) 
            {
	    closedir(dir);

            if ((dir = opendir(cur_archive_dir_name)) == NULL) 
               {
               if (makedir(cur_archive_dir_name) != 0)
                  {
                  printf("error in 'cppsim exporter':\n");
                  printf("     can't create Import_Export subdirectory:\n");
                  printf("     '%s'\n",cur_archive_dir_name);
                  exit(1);
                  }
	       }
            else
	       closedir(dir);

            sprintf(cur_dir_name,"%s/%s/%s",simruns_dir_name,
                  cur_module->library_path,cur_module->name);

            if ((dir = opendir(cur_dir_name)) != NULL) 
               {
	       closedir(dir);
               sprintf(cur_archive_dir_name,"%s/%s/%s",
                     archive_simruns_base_dir_name,
	             cur_module->library_path,cur_module->name);
               recursively_copy_simruns_directory(cur_dir_name, cur_archive_dir_name);
	       }
	    }
	 }
       cur_module = cur_module->next;
      }
      //////  copy CadenceLib files /////
      copy_modules_to_cadence_directories(first_module,
          modules_par_filename, archive_cadencelib_base_dir_name);
   }
else
   {
   determine_simruns_directory(simruns_dir_name);
   cur_module = first_module;
   while (cur_module != NULL)
      {
       if (cur_module->active_sue_module_flag == 1)
         {
         // copy SueLib files
         sprintf(cur_archive_dir_name,"%s/%s",
		archive_suelib_base_dir_name,cur_module->library_path);
         if ((dir = opendir(cur_archive_dir_name)) == NULL) 
            {
            if (makedir(cur_archive_dir_name) != 0)
              {
               printf("error in 'cppsim exporter':\n");
               printf("     cannot create Import_Export subdirectory:\n");
               printf("     '%s'\n",cur_archive_dir_name);
               exit(1);
              }
	    }
         else
	    closedir(dir);

         sprintf(cur_archive_file_name,"%s/%s.sue",
	       cur_archive_dir_name,cur_module->name);
         copy_file(cur_module->full_filename,cur_archive_file_name);

         //////  copy CadenceLib files /////

         extract_cadencelib_dir_name(cur_module->full_filename,cur_base_dir_name);
       
         if ((dir = opendir(cur_base_dir_name)) == NULL) 
            {
	    printf("Error in 'cppsim exporter':  can't open CadenceLib directory:\n");
            printf("   '%s'\n",cur_base_dir_name);
            exit(1);            
	    }
         else
	    closedir(dir);

         if ((dir = opendir(archive_cadencelib_base_dir_name)) == NULL) 
            {
	    printf("error in 'cppsim exporter':  can't open Import_Export directory for CadenceLib directory:\n");
            printf("   '%s'\n",archive_cadencelib_base_dir_name);
            exit(1);
	    }
         else
	    closedir(dir);

         sprintf(cur_dir_name,"%s/%s",cur_base_dir_name,
                 cur_module->library_path);
         sprintf(cur_archive_dir_name,"%s/%s",archive_cadencelib_base_dir_name,
	         cur_module->library_path);

         if ((dir = opendir(cur_dir_name)) != NULL) 
            {
	    closedir(dir);

            if ((dir = opendir(cur_archive_dir_name)) == NULL) 
               {
               if (makedir(cur_archive_dir_name) != 0)
                  {
                  printf("error in 'cppsim exporter':\n");
                  printf("     can't create Import_Export subdirectory:\n");
                  printf("     '%s'\n",cur_archive_dir_name);
                  exit(1);
                  }
	       }
            else
	       closedir(dir);

            sprintf(cur_dir_name,"%s/%s/%s",cur_base_dir_name,
                  cur_module->library_path,cur_module->name);

            if ((dir = opendir(cur_dir_name)) != NULL) 
               {
	       closedir(dir);
               sprintf(cur_archive_dir_name,"%s/%s/%s",
                     archive_cadencelib_base_dir_name,
	             cur_module->library_path,cur_module->name);
               recursively_copy_directory(cur_dir_name, cur_archive_dir_name);
	       }
	    }


         //////  copy SimRun files /////

         if ((dir = opendir(simruns_dir_name)) == NULL) 
            {
	    printf("error in 'cppsim exporter':  can't open SimRuns directory:\n");
            printf("   '%s'\n",simruns_dir_name);
            exit(1);
	    }
         else
	    closedir(dir);

         if ((dir = opendir(archive_simruns_base_dir_name)) == NULL) 
            {
	    printf("error in 'cppsim exporter':  can't open Import_Export directory for SimRuns directory:\n");
            printf("   '%s'\n",archive_simruns_base_dir_name);
            exit(1);
	    }
         else
	    closedir(dir);

         sprintf(cur_dir_name,"%s/%s",simruns_dir_name,
                 cur_module->library_path);
         sprintf(cur_archive_dir_name,"%s/%s",archive_simruns_base_dir_name,
	         cur_module->library_path);

         if ((dir = opendir(cur_dir_name)) != NULL) 
            {
	    closedir(dir);

            if ((dir = opendir(cur_archive_dir_name)) == NULL) 
               {
               if (makedir(cur_archive_dir_name) != 0)
                  {
                  printf("error in 'cppsim exporter':\n");
                  printf("     can't create Import_Export subdirectory:\n");
                  printf("     '%s'\n",cur_archive_dir_name);
                  exit(1);
                  }
	       }
            else
	       closedir(dir);

            sprintf(cur_dir_name,"%s/%s/%s",simruns_dir_name,
                  cur_module->library_path,cur_module->name);

            if ((dir = opendir(cur_dir_name)) != NULL) 
               {
	       closedir(dir);
               sprintf(cur_archive_dir_name,"%s/%s/%s",
                     archive_simruns_base_dir_name,
	             cur_module->library_path,cur_module->name);
               recursively_copy_simruns_directory(cur_dir_name, cur_archive_dir_name);
	       }
	    }
       ///////////////////////////////////////////////////////////

         /*
         printf("code_dir: %s/%s/%s\n",cadencelib_dir_name,
	      cur_module->library_path,cur_module->name);
         printf("sim_runs_dir: %s/%s/%s\n",simruns_dir_name,
              cur_module->library_path,cur_module->name);
         */
	 }
       cur_module = cur_module->next;
      }
   }
printf("\n... Exporting of files complete ...\n\n");
}

int extract_code_arguments(LINE *line_set, char *filename, int linecount, ARGUMENT *args)
{
char *inpline;
int i,j,line_set_count;

if (line_set->num_of_lines == 0)
   {
   strcpy(args->arg_array[0],"\n");
   args->num_of_args = 0;
   return(0);
   }

line_set_count = 0;
inpline = line_set->line_array[line_set_count];
i = 0;


for (args->num_of_args = 0; args->num_of_args < MAX_NUM_ARGS; args->num_of_args++)
  {
   if (inpline[i] == '\0' || inpline[i] == '\n' || 
       inpline[i] == ('\n' + 3) || inpline[i] == 0x0d)
     {
      args->arg_array[args->num_of_args][0] = '\n';
      args->arg_array[args->num_of_args][1] = '\0';
      args->num_of_args++;

      if (++line_set_count < line_set->num_of_lines)
	{
         inpline = line_set->line_array[line_set_count];
	 i = 0;
	}
      else
         break;
     }
   
   if (inpline[i] == ' ' || inpline[i] == '\t')
     {
     for (j = 0; inpline[i] == ' ' || inpline[i] == '\t'; i++,j++)
       {
       if (j-1 >= MAX_ARG)
         {
         printf("error in 'extract_code_arguments':  max argument size exceeded\n");
         printf("     in file '%s' on line %d\n",filename,linecount);
         printf("      to fix error, change MAX_ARG in source code\n");
         exit(1);
        }
       args->arg_array[args->num_of_args][j] = ' ';
       }
     }
   else if (inpline[i] == '%' || inpline[i] == ',' ||
            inpline[i] == '(' || inpline[i] == ')' ||
	    //            inpline[i] == '[' || inpline[i] == ']' ||
            inpline[i] == ',' || inpline[i] == '=' ||
            inpline[i] == ';' || (inpline[i] == ':' && args->num_of_args > 0) )
     {
       args->arg_array[args->num_of_args][0] = inpline[i];
       i++; j=1;
     }
   else
     {
     for (j = 0; inpline[i] != ' ' && inpline[i] != ',' &&
                 inpline[i] != '(' && inpline[i] != ')' &&
	    //                 inpline[i] != '[' && inpline[i] != ']' &&
                 inpline[i] != ',' && inpline[i] != '\n' &&
                 inpline[i] != ';' && inpline[i] != '=' &&
	         inpline[i] != '%' && inpline[i] != '\t' &&
                 (inpline[i] != ':' || args->num_of_args == 0) &&
                 inpline[i] != ('\n' + 3)  && inpline[i] != '\0' &&
                 inpline[i] != 0x0d
               ; i++,j++)
       {
       if (j-1 >= MAX_ARG)
         {
         printf("error in 'extract_code_arguments':  max argument size exceeded\n");
         printf("     in file '%s' on line %d\n",filename,linecount);
         printf("      to fix error, change MAX_ARG in source code\n");
         exit(1);
        }
       args->arg_array[args->num_of_args][j] = inpline[i];
       }
     }
   args->arg_array[args->num_of_args][j] = '\0';
  }

if (args->num_of_args > MAX_NUM_ARGS)
  {
   printf("error in 'extract_code_arguments':  max number of arguments exceeded\n");
   printf("error occurred in spice file '%s' on line %d\n",filename,linecount);
   printf("   MAX_NUM_ARGS specifed in source code as '%d'\n",MAX_NUM_ARGS);
   exit(1);
  }

return(args->num_of_args);
}


void copy_modules_to_cadence_directories(MODULE *first_module,
          char *code_filename, char *base_cadence_dir)
{
FILE *in,*cppsim_file,*master_tag_file;
int i,j,linecount,end_of_file_flag,word_length;
LINE *line_set;
ARGUMENT *args;
int module_comment_flag;
char cadence_dir[MAX_ARG], cppsim_filename[MAX_ARG];
char master_tag_filename[MAX_ARG];
char temp1[MAX_ARG], temp2[MAX_ARG];
DIR *dir;
struct dirent *entry;
MODULE *cur_module;


/////////////////  Read Code File ///////////////////////
//////////////////////////////////////////////////////////

if ((in = fopen(code_filename,"r")) == NULL)
  {
   printf("error in 'copy_modules_to_cadence_directories':\n");
   printf("   can't open file:  '%s'\n", code_filename);
   exit(1);
  }


cppsim_file = NULL;
line_set = init_line();
args = init_argument();
linecount = 0;
end_of_file_flag = 0;
module_comment_flag = 0;


// check existence of base_cadence_dir
if ((dir = opendir(base_cadence_dir)) == NULL) 
   {
    printf("error in 'copy_modules_to_cadence_directories':\n");
    printf("    Could not open base Cadence directory %s\n",base_cadence_dir);
    exit(1);
   }
closedir(dir);


while(end_of_file_flag == 0)
  {
   end_of_file_flag = extract_code_lineset(in,line_set,code_filename,&linecount);
   //send_line_set_to_std_output(line_set);
   // printf("-----------------\n");
   extract_code_arguments(line_set,code_filename,linecount,args);

   //   for (i = 0; i < args->num_of_args; i++)
   //  printf("arg[%d] = [%s]\n",i,args->arg_array[i]);
   //////////////////////////////////////////////////

   for (i = 0; i < args->num_of_args; i++)
     {
       if (args->arg_array[i][0] != ' ' && args->arg_array[i][0] != '\n')
	  break;
     }
   if (i >= args->num_of_args)
      continue;
   for (j = 0; args->arg_array[i][j] != '\0'; j++)
     {
     if (args->arg_array[i][j] == ':')
        break;
     if (args->arg_array[i][j] == '*' || args->arg_array[i][j] == '%' ||
	 (args->arg_array[i][j] == '/' && args->arg_array[i][j+1] == '/'))
       break;
     }
   if (args->arg_array[i][j] == '*' || args->arg_array[i][j] == '%' ||
       (args->arg_array[i][j] == '/' && args->arg_array[i][j+1] == '/'))
     {
     for (   ; args->arg_array[i][j] != '\0'; j++)
       if (args->arg_array[i][j] != '*' && 
           args->arg_array[i][j] != '%' &&
           args->arg_array[i][j] != '/' &&
           args->arg_array[i][j] != ' ')
	 break;
     if (strcmp(&(args->arg_array[i][j]),"module:") == 0)
       {
        module_comment_flag = 1;
        continue;
       }
     }
   if (module_comment_flag == 1 && strcmp(args->arg_array[i],"module:") != 0)
     continue; // 
   if (strcmp(args->arg_array[i],"module:") == 0)
     {
     module_comment_flag = 0;
     strip_args(args);
     if (args->num_of_args < 2)
       {
	printf("error in 'extract_module_code': need an argument\n");
        printf("  with 'module:' command\n");
        exit(1);
       }
     for (i = 0; args->arg_array[1][i] != '\0'; i++)
	 if (isupper(args->arg_array[1][i]))
	    {
 	     printf("error in 'copy_modules_to_cadence_directories': must have all\n");
	     printf("  lowercase letters when defining module names in\n");
	     printf("  modules parameter file!\n");
	     printf("  in this case, the module name '%s' has uppercase letters\n",args->arg_array[1]);
	     printf("  line reads:\n");
             send_line_set_to_std_output(line_set);
	     exit(1);
	    }

     cur_module = first_module;
     while (cur_module != NULL)
        {
        if (cur_module->active_sue_module_flag == 1)
	   {
	   strcpy(temp1,cur_module->name);
           strcpy(temp2,args->arg_array[1]);
	   if (strcmp(lowercase(temp1),lowercase(temp2)) == 0)
	      break;
	   }
        cur_module = cur_module->next;
        }

     if (cur_module == NULL)
       {
         module_comment_flag = 1;
         continue;
       }
     if (strcmp(cur_module->name,args->arg_array[1]) != 0)
       {
	 printf("cur_module: '%s', arg: '%s'\n",cur_module->name,
               args->arg_array[1]);
         printf("   line: '%s'",line_set->line_array[0]);
	 for (word_length = 0; cur_module->name[word_length] != '\0'; 
              word_length++);
         
	 for (i = 0; line_set->line_array[0][i] != '\0'; i++)
	   if (strncmp(&line_set->line_array[0][i],args->arg_array[1],word_length-1) == 0)
	     {
	       for (j = 0; j < word_length; j++,i++)
		  line_set->line_array[0][i] = cur_module->name[j];
               break;
	     }
       }
     // check existance of library directory 
     sprintf(cadence_dir,"%s/%s",base_cadence_dir,cur_module->library_path);
     if ((dir = opendir(cadence_dir)) == NULL) 
        {
         if (makedir(cadence_dir) != 0)
	    {
	     printf("error in 'copy_modules_to_cadence_directories':\n");
	     printf("  can't create directory '%s'\n",cadence_dir);
	     exit(1);
	    }
        }
     else
         closedir(dir);

     // check existance of cell directory
     sprintf(cadence_dir,"%s/%s/%s",base_cadence_dir,cur_module->library_path,
              args->arg_array[1]);
     if ((dir = opendir(cadence_dir)) == NULL) 
        {
         if (makedir(cadence_dir) != 0)
	    {
	     printf("error in 'copy_modules_to_cadence_directories':\n");
	     printf("  can't create directory '%s'\n",cadence_dir);
	     exit(1);
	    }
        }
     else
        closedir(dir);

     // check existance of cppsim directory
     sprintf(cadence_dir,"%s/%s/%s/cppsim",base_cadence_dir,
                         cur_module->library_path,args->arg_array[1]);
     if ((dir = opendir(cadence_dir)) == NULL) 
        {
         if (makedir(cadence_dir) != 0)
	    {
	     printf("error in 'copy_modules_to_cadence_directories':\n");
	     printf("  can't create directory '%s'\n",cadence_dir);
	     exit(1);
	    }
        }
     else
        closedir(dir);

     // check existance of master.tag file and create if necessary
     sprintf(master_tag_filename,"%s/master.tag",cadence_dir);
     if ((master_tag_file = fopen(master_tag_filename,"r")) == NULL)
        {
          if ((master_tag_file = fopen(master_tag_filename,"w")) == NULL)
	    {
	     printf("error in 'copy_modules_to_cadence_directories':\n");
	     printf("  can't open file '%s'\n",master_tag_filename);
	     exit(1);
	    }
          fprintf(master_tag_file,"-- Master.tag File, Rev:1.0\n");
          fprintf(master_tag_file,"text.txt\n");
          fclose(master_tag_file);
        }

     // check existance of cppsim file
     sprintf(cppsim_filename,"%s/text.txt",cadence_dir);
     if ((cppsim_file = fopen(cppsim_filename,"r")) == NULL)
        {
          if ((cppsim_file = fopen(cppsim_filename,"w")) == NULL)
	    {
	     printf("error in 'copy_modules_to_cadence_directories':\n");
	     printf("  can't open file '%s'\n",cppsim_filename);
	     exit(1);
	    }
        }
     else
       {
	 fclose(cppsim_file);

	 module_comment_flag = 1;
  	 printf("    -> Warning:  duplicate module '%s' in modules.par file...\n",
                 args->arg_array[1]);
         printf("                 --- using the first '%s' code found ---\n",
		args->arg_array[1]);
	 continue;
       }
     }
   if (cppsim_file == NULL)
        continue;
   send_line_set_to_output(line_set,cppsim_file);

  }
fclose(in);
}

void copy_file(char *infile_name, char *outfile_name)
{
FILE *infile, *outfile;
static int cur_char;
char buffer[2000];
size_t result, result_out;

if ((infile = fopen(infile_name,"rb")) == NULL)
  {
   printf("error in 'copy_file':  can't open file:  '%s'\n",infile_name);
   exit(1);
  }
if ((outfile = fopen(outfile_name,"wb")) == NULL)
  {
   printf("error in 'copy_file':  can't open file:  '%s'\n",outfile_name);
   exit(1);
  }
// obtain file size
 
while (1)
   {
    result = fread(buffer,sizeof(char),2000,infile);
    result_out = fwrite(buffer,sizeof(char),result,outfile);
    if (result_out != result)
      {
	printf("error in 'copy_file':  copy operation not complete\n");
        printf("  input_file:   %s\n",infile_name);
        printf("  output file:  %s\n",outfile_name);
        exit(1);
      }
    if (result != 2000)
       break;
   }

fclose(infile);
fclose(outfile);
}

/**
void copy_file(char *infile_name, char *outfile_name)
{
FILE *infile, *outfile;
static int cur_char;


if ((infile = fopen(infile_name,"rb")) == NULL)
  {
   printf("error in 'copy_file':  can't open file:  '%s'\n",infile_name);
   exit(1);
  }
if ((outfile = fopen(outfile_name,"wb")) == NULL)
  {
   printf("error in 'copy_file':  can't open file:  '%s'\n",outfile_name);
   exit(1);
  }
while ((cur_char = fgetc(infile)) != EOF)
   {
    fputc(cur_char,outfile);
   }

fclose(infile);
fclose(outfile);
}
**/

void determine_active_modules(MODULE *first_module, char *top_sue_module_name,
                              char *control_string, int run_in_lite_mode_flag)
{
MODULE *cur_module;
char cur_sue_dir_name[MAX_CHAR_LENGTH];
DIR *dir;
struct dirent *entry;
char cur_sue_module_name[MAX_CHAR_LENGTH];
int i;

if (run_in_lite_mode_flag == 1 && strcmp(control_string,"all") == 0)
   {
     cur_module = first_module;
     while (cur_module != NULL)
       {
         if (strcmp(cur_module->name,top_sue_module_name) == 0)
	    break;
	 cur_module = cur_module->next;
       }
     if (cur_module == NULL)
       {
	 printf("error in 'determine_active_modules':\n");
         printf("   can't find module '%s'\n",top_sue_module_name);
         printf("   -> this should never happen!?\n");
         exit(1);
       }
     strcpy(cur_sue_dir_name,cur_module->library_path);
     cur_module = first_module;
     while (cur_module != NULL)
       {
         if (strcmp(cur_module->library_path,cur_sue_dir_name) == 0)
            cur_module->active_sue_module_flag = 1;
         else
            cur_module->active_sue_module_flag = 0;
	 cur_module = cur_module->next;
       }
   }
else if (run_in_lite_mode_flag == 1 && strcmp(control_string,"all") != 0)
   {
     cur_module = first_module;
     while (cur_module != NULL)
       {
         if (strcmp(cur_module->name,top_sue_module_name) == 0)
	    break;
	 cur_module = cur_module->next;
       }
     if (cur_module == NULL)
       {
	 printf("error in 'determine_active_modules':\n");
         printf("   can't find module '%s'\n",top_sue_module_name);
         printf("   -> this should never happen!?\n");
         exit(1);
       }
     cur_module->active_sue_module_flag = 1;
   }
else
   {
   cur_module = mark_active_modules(first_module, top_sue_module_name);
   if (cur_module == NULL)
     {
       printf("error in 'determine_active_modules':\n");
       printf("  there is a problem with the file '%s.sue'\n",top_sue_module_name);
       printf("  -> perhaps the module name inside '%s.sue' is not '%s'?\n",
	      top_sue_module_name,top_sue_module_name);
       printf("  ****** export operation aborted *****\n");
       exit(1);
     }
   if (strcmp(control_string,"all") == 0)
      {
      extract_sue_library_name(cur_module->full_filename,cur_sue_dir_name);

      if ((dir = opendir(cur_sue_dir_name)) == NULL) 
         {
          printf("error in 'determine_active_modules':\n");
          printf("  cannot open library directory:\n");
          printf("      %s\n",cur_sue_dir_name);
          exit(1);     
         }

      chdir(cur_sue_dir_name);
      while ((entry = readdir(dir)) != NULL) 
         {
          if (is_sue_file(entry->d_name))
	    {
	     sprintf(cur_sue_module_name,"%s",entry->d_name);
             for (i = 0; cur_sue_module_name[i] != '\0'; i++);
             for (     ; i >= 0; i--)
	        if (cur_sue_module_name[i] == '.')
	           break;
	     if (cur_sue_module_name[i] != '.')
	        {
	        printf("error in 'determine_active_modules':\n");
                printf("  sue module name appears to be messed up!\n");
                printf("   -> name: '%s'\n",cur_sue_module_name);
                exit(1);
	        }
             cur_sue_module_name[i] = '\0';
             mark_active_modules(first_module, cur_sue_module_name);
	    }
         }
      closedir(dir);
     }
   }
}

void create_archive_directory(char *simruns_base_dir_name,
             char *suelib_base_dir_name, char *cadencelib_base_dir_name)
{
int i;
DIR *dir, *dir_temp;
struct dirent *entry;
const char *cppsim_home_dir_const = NULL;
char cppsim_home_dir[MAX_CHAR_LENGTH];
char archive_dir_name[MAX_CHAR_LENGTH];
char next_dir_name[MAX_CHAR_LENGTH];
char cur_file_name[MAX_CHAR_LENGTH];

///////////// CPPSIMHOME environment variable /////////

cppsim_home_dir_const = getenv("CPPSIMHOME");
if (cppsim_home_dir_const != NULL)
  {
  strcpy(cppsim_home_dir,cppsim_home_dir_const);
  for (i = 0; cppsim_home_dir[i] != '\0'; i++)
    if (cppsim_home_dir[i] == '\\')
       cppsim_home_dir[i] = '/';
  }
else
  {
    printf("Error:  you must define environment variable CPPSIMHOME\n");
    printf("  before running CppSim\n");
    printf("  -> include the following statement in your .cshrc[.mine] file:\n");
    printf("     setenv CPPSIMHOME $HOME/CppSim\n");
    exit(1);
  }

if ((dir = opendir(cppsim_home_dir)) == NULL) 
   {
    printf("Error:  cannot open CppSim home directory specified\n");
    printf("        by the UNIX environment variable CPPSIMHOME = '%s'\n",
                    cppsim_home_dir);
    printf("  -> perhaps you need to redefine UNIX environment variable CPPSIMHOME\n");
    printf("     within your .cshrc[.mine] file:\n");
    exit(1);
   } 
else
   closedir(dir);

// make fresh Import_Export directory or clear out the previously made one //

sprintf(archive_dir_name,"%s/Import_Export",cppsim_home_dir);

if ((dir = opendir(archive_dir_name)) == NULL) 
   {
    if (makedir(archive_dir_name) != 0)
       {
        printf("error in 'create_archive_directory':\n");
        printf("     cannot create Import_Export directory:\n");
        printf("     '%s'\n",archive_dir_name);
        exit(1);
       }
   } 
else
   {
    chdir(archive_dir_name);
    while ((entry = readdir(dir)) != NULL) 
        {
	 if (strcmp(entry->d_name,".") == 0 ||
	     strcmp(entry->d_name,"..") == 0)
             continue;

	 if ((dir_temp = opendir(entry->d_name)) != NULL) // directory
	    {
	    closedir(dir_temp);
	    sprintf(next_dir_name,"%s/%s",archive_dir_name,entry->d_name);
	    recursively_delete_directory(next_dir_name);
	    }
	 /******************* don't delete files in Import_Export for now ...
         else // file
    	   {
	     sprintf(cur_file_name,"%s/%s",archive_dir_name,entry->d_name);
             if (remove(cur_file_name) != 0)
	       {
               printf("error in 'create_archive_directory':\n");
               printf("     cannot clean out the Import_Export directory\n");
               printf("     in particular, deletion of the following file failed:\n");
               printf("     '%s'\n",cur_file_name);
               printf("     -> try changing the permissions of the directory/file\n");
               printf("        and make sure that no process is currently accessing that file\n");
               exit(1);
	       }
	   }
	 *******************/
	}
    closedir(dir);    
   }

//////////////////////////////////////////

sprintf(simruns_base_dir_name,"%s/SimRuns",archive_dir_name);
if (makedir(simruns_base_dir_name) != 0)
    {
     printf("error in 'create_archive_directory':\n");
     printf("     cannot create Import_Export subdirectory:\n");
     printf("     '%s'\n",simruns_base_dir_name);
     printf("     -> try changing the permissions of the directory\n");
     printf("        '%s'\n",archive_dir_name);
     exit(1);
    }

sprintf(suelib_base_dir_name,"%s/SueLib",archive_dir_name);
if (makedir(suelib_base_dir_name) != 0)
    {
     printf("error in 'create_archive_directory':\n");
     printf("     cannot create Import_Export subdirectory:\n");
     printf("     '%s'\n",suelib_base_dir_name);
     printf("     -> try changing the permissions of the directory\n");
     printf("        '%s'\n",archive_dir_name);
     exit(1);
    }

sprintf(cadencelib_base_dir_name,"%s/CadenceLib",archive_dir_name);
if (makedir(cadencelib_base_dir_name) != 0)
    {
     printf("error in 'create_archive_directory':\n");
     printf("     cannot create Import_Export subdirectory:\n");
     printf("     '%s'\n",cadencelib_base_dir_name);
     printf("     -> try changing the permissions of the directory\n");
     printf("        '%s'\n",archive_dir_name);
     exit(1);
    }

}

void recursively_delete_directory(char *cur_dir_name)
{
DIR *dir, *dir_temp;
struct dirent *entry;
char next_dir_name[MAX_CHAR_LENGTH];
char cur_file_name[MAX_CHAR_LENGTH];

if ((dir = opendir(cur_dir_name)) == NULL) 
   {
     printf("error in 'recursively_delete_directory':\n");
     printf("  can't open directory:\n");
     printf("  '%s'\n",cur_dir_name);
     exit(1);
   } 
else
   {
    chdir(cur_dir_name);
    while ((entry = readdir(dir)) != NULL) 
        {
	 if (strcmp(entry->d_name,".") == 0 ||
	     strcmp(entry->d_name,"..") == 0)
             continue;

	 if ((dir_temp = opendir(entry->d_name)) != NULL) // directory
	    {
	    closedir(dir_temp);
	    sprintf(next_dir_name,"%s/%s",cur_dir_name,entry->d_name);
	    recursively_delete_directory(next_dir_name);
	    }
         else // file
    	   {
	     sprintf(cur_file_name,"%s/%s",cur_dir_name,entry->d_name);
             if (remove(cur_file_name) != 0)
	       {
               printf("error in 'recursively_delete_directory':\n");
               printf("     cannot clean out the Import_Export directory\n");
               printf("     in particular, deletion of the following file failed:\n");
               printf("     '%s'\n",cur_file_name);
               printf("     -> try changing the permissions of the directory/file\n");
               printf("        and make sure that no process is currently accessing that file\n");
               exit(1);
	       }
	   }
	}
    closedir(dir);    
    chdir("..");
    if (rmdir(cur_dir_name) != 0)
      {
       printf("error in 'recursively_delete_directory':\n");
       printf("     cannot clean out the Import_Export directory\n");
       printf("     in particular, deletion of the following directory failed:\n");
       printf("     '%s'\n",cur_dir_name);
       printf("     -> try changing the permissions of the directory\n");
       printf("        and make sure that no process is currently accessing\n");
       printf("        the contents of that directory\n");
       exit(1);
      }
   }
}


void recursively_copy_directory(char *cur_dir_name, char *cur_archive_dir_name)
{
DIR *dir,*dir_temp;
struct dirent *entry;
char next_dir_name[MAX_CHAR_LENGTH];
char next_archive_dir_name[MAX_CHAR_LENGTH];
char cur_file_name[MAX_CHAR_LENGTH];
char cur_archive_file_name[MAX_CHAR_LENGTH]; 

if ((dir = opendir(cur_dir_name)) == NULL) 
   {
     printf("error in 'recursively_copy_directory':\n");
     printf("  can't open directory:\n");
     printf("  '%s'\n",cur_dir_name);
     exit(1);
   } 
else
   {
    if ((dir_temp = opendir(cur_archive_dir_name)) == NULL) 
        {
        if (makedir(cur_archive_dir_name) != 0)
            {
             printf("error in 'recursively_copy_directory':\n");
             printf("     cannot create Import_Export subdirectory:\n");
             printf("     '%s'\n",cur_archive_dir_name);
             exit(1);
            }
        } 
    else
	closedir(dir_temp);

    chdir(cur_dir_name);
    while ((entry = readdir(dir)) != NULL) 
        {
	 if (strcmp(entry->d_name,".") == 0 ||
	     strcmp(entry->d_name,"..") == 0)
             continue;

	 if ((dir_temp = opendir(entry->d_name)) != NULL) 
	    {
	    closedir(dir_temp);
	    sprintf(next_dir_name,"%s/%s",cur_dir_name,entry->d_name);
 	    sprintf(next_archive_dir_name,"%s/%s",cur_archive_dir_name,
                                             entry->d_name);
	    recursively_copy_directory(next_dir_name, next_archive_dir_name);
	    }
         else
    	   {
	     sprintf(cur_file_name,"%s/%s",cur_dir_name,entry->d_name);
             sprintf(cur_archive_file_name,"%s/%s",cur_archive_dir_name,
		     entry->d_name);
             copy_file(cur_file_name,cur_archive_file_name);
	   }
	}
    closedir(dir);    
    chdir("..");
   }
}


void recursively_copy_simruns_directory(char *cur_dir_name, char *cur_archive_dir_name)
{
DIR *dir,*dir_temp;
struct dirent *entry;
char next_dir_name[MAX_CHAR_LENGTH];
char next_archive_dir_name[MAX_CHAR_LENGTH];
char cur_file_name[MAX_CHAR_LENGTH];
char cur_archive_file_name[MAX_CHAR_LENGTH]; 

if ((dir = opendir(cur_dir_name)) == NULL) 
   {
     printf("error in 'recursively_copy_simruns_directory':\n");
     printf("  can't open directory:\n");
     printf("  '%s'\n",cur_dir_name);
     exit(1);
   } 
else
   {
    if ((dir_temp = opendir(cur_archive_dir_name)) == NULL) 
        {
        if (makedir(cur_archive_dir_name) != 0)
            {
             printf("error in 'recursively_copy_simruns_directory':\n");
             printf("     cannot create Import_Export subdirectory:\n");
             printf("     '%s'\n",cur_archive_dir_name);
             exit(1);
            }
        } 
    else
	closedir(dir_temp);

    chdir(cur_dir_name);
    while ((entry = readdir(dir)) != NULL) 
        {
	 if (strcmp(entry->d_name,".") == 0 ||
	     strcmp(entry->d_name,"..") == 0)
             continue;

	 if ((dir_temp = opendir(entry->d_name)) != NULL) 
	    {
	    closedir(dir_temp);
	    if (strncmp(entry->d_name,"verilator_",10) != 0)
	      {
	       sprintf(next_dir_name,"%s/%s",cur_dir_name,entry->d_name);
 	       sprintf(next_archive_dir_name,"%s/%s",cur_archive_dir_name,
                                             entry->d_name);
	       recursively_copy_simruns_directory(next_dir_name, next_archive_dir_name);
	      }
	    }
         else
    	   {
	     if (check_for_noncopy_simruns_file(entry->d_name) == 1)
	        continue;
	     sprintf(cur_file_name,"%s/%s",cur_dir_name,entry->d_name);
             sprintf(cur_archive_file_name,"%s/%s",cur_archive_dir_name,
		     entry->d_name);
             copy_file(cur_file_name,cur_archive_file_name);
	   }
	}
    closedir(dir);    
    chdir("..");
   }
}

int check_for_noncopy_simruns_file(char *file_name)
{
int i;

for (i = 0; file_name[i] != '\0'; i++)
  if (file_name[i] == '.')
     break;

if (file_name[i] != '.' ||
    strcmp(&file_name[i],".exe") == 0 ||
    strcmp(&file_name[i],".o") == 0 ||
    strcmp(&file_name[i],".cpp") == 0 ||
    strcmp(&file_name[i],".h") == 0 ||
    strcmp(&file_name[i],".cppsim") == 0 ||
    strcmp(&file_name[i],".outfile_list") == 0 ||
    strcmp(&file_name[i],".v") == 0 ||
    strcmp(&file_name[i],".out") == 0 ||
    strcmp(&file_name[i],".counter") == 0 ||
    (strcmp(&file_name[i],".m") == 0 &&
     strncmp(file_name,"compile_",8) == 0) ||
    strcmp(file_name,".cppsim_author_info") == 0 ||
    strcmp(file_name,"CppSimView_history.mat") == 0 ||
    strcmp(file_name,"Makefile") == 0 ||
    strncmp(&file_name[i],".lxt",4) == 0 ||
    strncmp(&file_name[i],".fst",4) == 0 ||
    strncmp(&file_name[i],".raw",4) == 0 ||
    strcmp(file_name,"netlist.ngsim") == 0 ||
    strcmp(file_name,"simrun.sp") == 0 ||
    strcmp(file_name,"simrun.log") == 0 ||
    strncmp(&file_name[i],".tr",3) == 0)
   {
   return(1);
   }
else
   {
   for (i = 0; file_name[i] != '\0'; i++);
   if (i > 0)
     {
      if (file_name[i-1] == '~')
	 return(1);
     }
   return(0);
   }
}

void determine_simruns_directory(char *simruns_dir_name)
{
int i;
DIR *dir;
const char *cppsim_home_dir_const = NULL;
char cppsim_home_dir[MAX_CHAR_LENGTH];

///////////// CPPSIMHOME environment variable /////////

cppsim_home_dir_const = getenv("CPPSIMHOME");
if (cppsim_home_dir_const != NULL)
  {
  strcpy(cppsim_home_dir,cppsim_home_dir_const);
  for (i = 0; cppsim_home_dir[i] != '\0'; i++)
    if (cppsim_home_dir[i] == '\\')
       cppsim_home_dir[i] = '/';
  }
else
  {
    printf("Error:  you must define environment variable CPPSIMHOME\n");
    printf("  before running CppSim\n");
    printf("  -> include the following statement in your .cshrc[.mine] file:\n");
    printf("     setenv CPPSIMHOME $HOME/CppSim\n");
    exit(1);
  }

if ((dir = opendir(cppsim_home_dir)) == NULL) 
   {
    printf("Error:  cannot open CppSim home directory specified\n");
    printf("        by the UNIX environment variable CPPSIMHOME = '%s'\n",
                    cppsim_home_dir);
    printf("  -> perhaps you need to redefine UNIX environment variable CPPSIMHOME\n");
    printf("     within your .cshrc[.mine] file:\n");
    exit(1);
   } 
else
   closedir(dir);

sprintf(simruns_dir_name,"%s/SimRuns",cppsim_home_dir);

if ((dir = opendir(simruns_dir_name)) == NULL) 
   {
    printf("error in 'determine_simruns_directory':\n");
    printf("     cannot open SimRuns directory:\n");
    printf("     '%s'\n",simruns_dir_name);
    exit(1);
   } 
else
   closedir(dir);

}

void extract_sue_library_name(char *sue_file_name, char *sue_library_name)
{
int i;

strcpy(sue_library_name,sue_file_name);
for (i = 0; sue_library_name[i] != '\0'; i++);
for (    ; i >= 0; i--)
  {
   if (sue_library_name[i] == '/')
      break;
  }

if (sue_library_name[i] != '/')
   {
    printf("error in 'extract_sue_library_name':\n");
       printf("  sue file name seems to be wrong ...\n");
       printf("  file name: '%s'\n",sue_file_name);
       printf("  -> there should be at least one '/' character\n");
       exit(1);
   }
sue_library_name[i] = '\0';
}

void extract_cadencelib_dir_name(char *sue_file_name, char *cppsim_library_name)
{
int i,count;
char temp[MAX_CHAR_LENGTH];
DIR *dir;

strcpy(cppsim_library_name,sue_file_name);
for (i = 0; cppsim_library_name[i] != '\0'; i++);
count = 0;
for (    ; i >= 0; i--)
 {
   if (cppsim_library_name[i] == '/')
      count++;
   if (count == 3)
      break;
 }
if (i < 0)
   {
    printf("error in 'extract_cadencelib_dir_name':\n");
    printf("  sue file name seems to be wrong ...\n");
    printf("  file name: '%s'\n",sue_file_name);
    printf("  -> there should be at least three '/' characters\n");
    exit(1);
   }
if (strncmp(&cppsim_library_name[i],"/SueLib/",8) != 0)
   {
    printf("error in 'extract_cadencelib_dir_name':\n");
    printf("  sue file name seems to be wrong ...\n");
    printf("  file name: '%s'\n",sue_file_name);
    printf("  -> there should be a '/SueLib/' string within the file name\n");
    exit(1);
   }

cppsim_library_name[i] = '\0';
strcat(cppsim_library_name,"/CadenceLib");

if ((dir = opendir(cppsim_library_name)) == NULL) 
   {
    printf("error in 'extract_cadencelib_dir_name':\n");
    printf("     cannot open CadenceLib directory:\n");
    printf("     '%s'\n",cppsim_library_name);
    exit(1);
   } 
else
   closedir(dir);

}




MODULE *mark_active_modules(MODULE *first_module, char *top_module_name)
{
MODULE *cur_module;

if (is_node_naming_object(top_module_name))
   {
     printf("Error in 'mark_active_modules':\n");
     printf("      Export of 'devices' library is not supported!\n");
     printf("\n      ******  aborting the export operation  ******\n");
     printf("        -> top_module is '%s'\n", top_module_name);
     exit(1);
   }

cur_module = first_module;
while (cur_module != NULL)
   {
    if (strcmp(cur_module->name,top_module_name) == 0)
      {
       cur_module->active_sue_module_flag = 1;
       break;       
      }
    cur_module = cur_module->next;
   }

if (cur_module == NULL)
   {
    printf("\nError in 'mark_active_modules':  can't find module '%s'\n",
	    top_module_name);
    printf("   -> This may be caused by a mismatch between the module name and its .sue file\n");
    printf("      consider looking at the contents of file '%s.sue'\n",
	   top_module_name);
    printf("      to make sure it contains:\n");
    printf("      proc SCHEMATIC_%s ...\n",top_module_name);
    printf("      AND/OR\n");
    printf("      proc ICON_%s ...\n",top_module_name);
    printf("\n      ******  aborting the export operation  ******\n");
    printf("        -> top module is '%s'\n", top_module_name);
    exit(1);
   }

determine_sue_module_dependencies(cur_module);
return(cur_module);
}
