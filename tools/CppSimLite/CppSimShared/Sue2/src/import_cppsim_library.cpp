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
void copy_file(char *infile_name, char *outfile_name);

void recursively_copy_simruns_directory(char *cur_dir_name, char *cur_archive_dir_name);
int check_for_noncopy_simruns_file(char *file_name);
void copy_modules_to_cadence_directories(MODULE *first_module,
       char *code_filename, char *base_cadence_dir);
int extract_code_arguments(LINE *line_set, char *filename, int linecount, ARGUMENT *args);
int compare_sue_files(MODULE *cur_module, MODULE *cur_module_archive, 
                      int name_change_flag);
void change_module_name_in_archive_sue_modules(MODULE *first_module_archive,
                        MODULE *cur_module_archive, char *new_module_name);
void change_module_name_in_archive(MODULE *cur_module_archive,
                                char *new_module_name);
int compare_code_files(MODULE *cur_module, MODULE *cur_module_archive, const char *code_type,
                       int name_change_flag);
int extract_lineset_install(FILE *in, LINE *line_set, char *filename, 
                             int *linecount);
int extract_lineset_install2(FILE *in, LINE *line_set, char *filename, 
                             int *linecount);
void change_module_name_in_code_file(MODULE *cur_module_archive,
				     char *new_module_name);
void change_module_name_in_verilog_code_file(MODULE *cur_module_archive,
				     char *new_module_name);
void extract_simruns_dir_name(char *sue_file_name, 
                              char *simruns_library_name);
void copy_over_simruns_files(MODULE *cur_module_archive);
void recursively_copy_directory(char *cur_dir_name, 
                                char *cur_archive_dir_name);
void import_cppsim_modules(MODULE *first_module, MODULE *first_module_archive,
			   char *new_cppsim_lib_name);
void add_new_cppsim_library_to_sue_lib_file(char *sue_lib_filename,
                                            char *new_cppsim_library_name);
void process_module_name_collisions(MODULE *first_module, 
                                    MODULE *first_module_archive);
void preview_cppsim_modules(MODULE *first_module, MODULE *first_module_archive,
			    char *new_cppsim_lib_name);

MODULE *verify_module_name_collisions(MODULE *first_module, 
				   MODULE *first_module_archive);
void copy_simruns_and_warn_about_name_collisions(MODULE *first_module,
                                MODULE *first_module_archive);
void delete_lib_dir(char *new_cppsim_library_name, int make_new_lib_dir_flag);


main(int argc,char *argv[])
{
PARAMETER *param_list;
MODULE *first_module, *first_module_archive;
char new_cppsim_library_name[MAX_ARG];
int make_new_lib_dir_flag;

if (argc < 4)
  {
   printf("error: need three arguments!\n");
   printf("  install_cppsim_library sue_lib_file new_library_name import/preview\n");
   exit(1);
  }
sprintf(new_cppsim_library_name,"%s",argv[2]);

param_list = init_parameter();

printf("\n... Adding new library to sue.lib file (if needed) ...\n\n");
fflush(stdout);

add_new_cppsim_library_to_sue_lib_file(argv[1],new_cppsim_library_name);

// extraction of imported modules and current user modules
//    is done in "lite" manner since only the module names
//    are of significance here

printf("\n... Extracting existing Sue2 modules ...\n\n");
fflush(stdout);

first_module = extract_sue_modules(argv[1],param_list,"lite");
warn_about_duplicate_modules(first_module);
// print_keyinfo_modules(first_module); exit(1);

printf("\n... Extracting Sue2 modules to import ...\n\n");
fflush(stdout);

first_module_archive = extract_sue_modules_archive(param_list);


//// trim node naming modules
first_module = trim_node_naming_modules(first_module);
first_module_archive = trim_node_naming_modules(first_module_archive);

// print_keyinfo_modules(first_module);

printf("\n... Check imported modules for SUE and CppSim/Verilog code file clashes ...\n\n");
fflush(stdout);


///// Take care of module name collisions
process_module_name_collisions(first_module, first_module_archive);
first_module_archive = verify_module_name_collisions(first_module, first_module_archive);
copy_simruns_and_warn_about_name_collisions(first_module, first_module_archive);

///// Done with module name collisions

if (strcmp(argv[3],"preview") != 0)
   {
    printf("\n... Now import modules into CppSim package ...\n\n");
    fflush(stdout);

    ///// Now import modules
    import_cppsim_modules(first_module,first_module_archive,
		      new_cppsim_library_name);
   }
else
   {
    printf("\n... Now preview modules that will be imported into CppSim package ...\n\n");
    fflush(stdout);

    ///// Preview of import modules
    preview_cppsim_modules(first_module,first_module_archive,
                           new_cppsim_library_name);
   }
}


void copy_simruns_and_warn_about_name_collisions(MODULE *first_module,
                                MODULE *first_module_archive)
{
MODULE *cur_module, *cur_module_archive;
int compare_code_file_flag, compare_verilog_code_file_flag, compare_sue_file_flag;
int compare_cppsim_code_file_flag;
int allow_module_name_change_flag;

cur_module_archive = first_module_archive;
while (cur_module_archive != NULL)
   {
    if (strcmp(cur_module_archive->name,cur_module_archive->old_name) != 0)
       {
	 /*
	 printf("name: '%s', old_name: '%s'\n",cur_module_archive->name,
		cur_module_archive->old_name);
	 */
       cur_module = first_module;
       while (cur_module != NULL)
	 {
           if (strcmp(cur_module->name,cur_module_archive->old_name) == 0)
	     break;
	   cur_module = cur_module->next;
	 }
       if (cur_module == NULL)
	 {
	   printf("error in 'warn_about_name_collisions'\n");
           printf("  can't determine conflict module name '%s' !?\n",
                   cur_module_archive->old_name);
           printf("  -> this should never happen!?\n");
           exit(1);
	 }
       allow_module_name_change_flag = 1;
       compare_sue_file_flag = compare_sue_files(cur_module,
                                      cur_module_archive,
				      allow_module_name_change_flag);
       compare_cppsim_code_file_flag = compare_code_files(cur_module,
				      cur_module_archive, "cppsim",
				      allow_module_name_change_flag);
       compare_verilog_code_file_flag = compare_code_files(cur_module,
				      cur_module_archive, "verilog",
				      allow_module_name_change_flag);
       if (compare_cppsim_code_file_flag == 0 || compare_verilog_code_file_flag == 0)
	 compare_code_file_flag = 0;
       else
	 compare_code_file_flag = 1;

        printf("Warning:  new module '%s' has the same name\n",
                  cur_module_archive->old_name);
        if (compare_sue_file_flag == 0 && compare_code_file_flag == 1)
               printf("          as an existing module, but their sue files differ\n");
        else if (compare_sue_file_flag == 1 && compare_code_file_flag == 0)
               printf("          as an existing module, but their code files differ\n");
        else if (compare_sue_file_flag == 0 && compare_code_file_flag == 0)
               printf("          as an existing module, but both their sue and code files differ\n");

        printf("          ----- change name of new module -----\n          from: '%s'\n",
                             cur_module_archive->old_name);
        printf("          to:   '%s'\n",cur_module_archive->name);

        // copy over SimRuns files
        copy_over_simruns_files(cur_module_archive);

       }
    cur_module_archive = cur_module_archive->next;
   }
}

void process_module_name_collisions(MODULE *first_module, 
                                    MODULE *first_module_archive)
{
char new_module_name[MAX_MODULE_NAME_LENGTH];
char temp_module_name[MAX_MODULE_NAME_LENGTH];
MODULE *cur_module, *cur_module2, *cur_module_archive;
int i, j, allow_module_name_change_flag;
int compare_code_file_flag, compare_sue_file_flag;
int compare_cppsim_code_file_flag, compare_verilog_code_file_flag;

cur_module_archive = first_module_archive;
while (cur_module_archive != NULL)
   {
   cur_module_archive->active_sue_module_flag = 0;
   cur_module_archive = cur_module_archive->next;
   }

cur_module_archive = first_module_archive;
while (cur_module_archive != NULL)
   {
   cur_module = first_module;
   while (cur_module != NULL)
      {
      if (strcmp(cur_module->name,cur_module_archive->name) == 0)
	 break;
      cur_module = cur_module->next;
      }
   if (cur_module != NULL)
     {
       allow_module_name_change_flag = 0;
       compare_sue_file_flag = compare_sue_files(cur_module,
                                      cur_module_archive,
				      allow_module_name_change_flag);
       compare_cppsim_code_file_flag = compare_code_files(cur_module,
				      cur_module_archive, "cppsim",
				      allow_module_name_change_flag);
       compare_verilog_code_file_flag = compare_code_files(cur_module,
				      cur_module_archive, "verilog",
				      allow_module_name_change_flag);
       if (compare_cppsim_code_file_flag == 0 || compare_verilog_code_file_flag == 0)
	 compare_code_file_flag = 0;
       else
	 compare_code_file_flag = 1;

       if (compare_code_file_flag != 1 || compare_sue_file_flag != 1)
	 {
           cur_module_archive->active_sue_module_flag = 1;
	   sprintf(temp_module_name,"%s",cur_module_archive->name);
           for (j = 0; temp_module_name[j] != '\0'; j++);
           for (     ; temp_module_name[j] != '_' && j > 0; j--);
           if (temp_module_name[j] == '_')
	     {
	       if (temp_module_name[j+1] != '\0')
		 {
 	          if (temp_module_name[j+2] == '\0')
		    {
                     if (temp_module_name[j+1] >= '0' &&
                         temp_module_name[j+1] <= '9')
  		         temp_module_name[j] = '\0';
		    }
		  else if (temp_module_name[j+3] == '\0')
		    {
                     if(temp_module_name[j+1] >= '0' &&
                        temp_module_name[j+1] <= '9' &&
                        temp_module_name[j+2] >= '0' &&
                        temp_module_name[j+2] <= '9') 
  		         temp_module_name[j] = '\0';
		    }
		 }
	     }

           allow_module_name_change_flag = 1;         
	   for (i = 2; i < 100; i++)
	     {
	      sprintf(new_module_name,"%s_%d",temp_module_name,i);
              cur_module = first_module;
              while (cur_module != NULL)
	        {
		 if (strcmp(new_module_name,cur_module->name) == 0)
		    break;
	         cur_module = cur_module->next;
	        }
              cur_module2 = first_module_archive;
              while (cur_module2 != NULL)
	        {
		 if (strcmp(new_module_name,cur_module2->name) == 0)
		    break;
	         cur_module2 = cur_module2->next;
	        }

              if (cur_module == NULL && cur_module2 == NULL)
		{
		 break;
		}
              else if (cur_module != NULL && cur_module2 == NULL)
                {
                 if (compare_sue_files(cur_module,cur_module_archive,
                                         allow_module_name_change_flag) == 1 &&
                     compare_code_files(cur_module,cur_module_archive, "cppsim",
					allow_module_name_change_flag) == 1 &&
                     compare_code_files(cur_module,cur_module_archive, "verilog",
					allow_module_name_change_flag) == 1)
		   {
		    break;
		   }
		}
 	     }
           if (i == 100)
	     {
	       printf("error in 'install_cppsim_library':\n");
               printf("  cannot come up with an alternate module name\n");
               printf("  for imported module '%s'\n",cur_module_archive->name);
               printf("   tried '%s_2', '%s_3', etc.\n",
		      cur_module_archive->name,cur_module_archive->name);
               printf("   up to '%s_99', but all coincided with\n",
                      cur_module_archive->name);
               printf("   other module names already in the package\n");
               printf("   ---- this imported module needs to be renamed ----\n");
               printf("   ---- since it is different than the existing ----\n");
               printf("   ---- module of the same name already in the package ----\n");
               exit(1);
	     }

           // add change in CadenceLib and SimRuns
           // change module name in extracted Import_Export module
           change_module_name_in_archive_sue_modules(first_module_archive,
                              cur_module_archive,new_module_name);
           // change module filename and associated sue2 file

           change_module_name_in_archive(cur_module_archive,new_module_name);
           change_module_name_in_code_file(cur_module_archive,new_module_name);
           change_module_name_in_verilog_code_file(cur_module_archive,new_module_name);

           sprintf(cur_module_archive->name,"%s",new_module_name);

           // must start the process over again since various
           // imported sue modules were affected by the above name change
           cur_module_archive = first_module_archive;
           continue;
	 }
     }
   cur_module_archive = cur_module_archive->next;
   }
}


MODULE *verify_module_name_collisions(MODULE *first_module, 
                                    MODULE *first_module_archive_in)
{
char new_module_name[MAX_MODULE_NAME_LENGTH];
char temp_module_name[MAX_MODULE_NAME_LENGTH];
MODULE *cur_module, *cur_module2, *cur_module_archive, *first_module_archive, *temp_module_archive;
int i, j, allow_module_name_change_flag, new_module_already_exists_flag;
int compare_code_file_flag, compare_sue_file_flag;

first_module_archive = first_module_archive_in;

cur_module_archive = first_module_archive;
while (cur_module_archive != NULL)
   {
   if (cur_module_archive->active_sue_module_flag == 0)
     {
       cur_module_archive = cur_module_archive->next;
       continue;
     }
   cur_module = first_module;
   while (cur_module != NULL)
       {
	if (strcmp(cur_module->name,cur_module_archive->name) == 0)
	      break;
	cur_module = cur_module->next;
      }
   if (cur_module == NULL)
     {
       cur_module_archive = cur_module_archive->next;
       continue;
     }
	   sprintf(temp_module_name,"%s",cur_module_archive->name);
           for (j = 0; temp_module_name[j] != '\0'; j++);
           for (     ; temp_module_name[j] != '_' && j > 0; j--);
	   i = 1;
           if (temp_module_name[j] == '_')
	     {
	       if (temp_module_name[j+1] != '\0')
		 {
 	          if (temp_module_name[j+2] == '\0')
		    {
                     if (temp_module_name[j+1] >= '0' &&
                         temp_module_name[j+1] <= '9')
		       {
  		         temp_module_name[j] = '\0';
	                 i = atoi(&temp_module_name[j+1]);
		       }
		    }
		  else if (temp_module_name[j+3] == '\0')
		    {
                     if(temp_module_name[j+1] >= '0' &&
                        temp_module_name[j+1] <= '9' &&
                        temp_module_name[j+2] >= '0' &&
                        temp_module_name[j+2] <= '9') 
		       {
  		         temp_module_name[j] = '\0';
	                 i = atoi(&temp_module_name[j+1]);
		       }
		    }
		 }
	     }

           allow_module_name_change_flag = 1;
           i--;         
	   for ( ; i > 0; i--)
	     {
	      if (i == 1)
	         sprintf(new_module_name,"%s",temp_module_name,i);
              else
	         sprintf(new_module_name,"%s_%d",temp_module_name,i);

              cur_module = first_module;
              while (cur_module != NULL)
	        {
		 if (strcmp(new_module_name,cur_module->name) == 0)
		    break;
	         cur_module = cur_module->next;
	        }
              cur_module2 = first_module_archive;
              while (cur_module2 != NULL)
	        {
		 if (strcmp(new_module_name,cur_module2->name) == 0)
		    break;
	         cur_module2 = cur_module2->next;
	        }

              if (cur_module == NULL && cur_module2 == NULL)
		{
		 new_module_already_exists_flag = 0;
		 break;
		}
              else if (cur_module != NULL && cur_module2 == NULL)
                {
                 if (compare_sue_files(cur_module,cur_module_archive,
                                         allow_module_name_change_flag) == 1 &&
                     compare_code_files(cur_module,cur_module_archive, "cppsim",
					allow_module_name_change_flag) == 1 &&
                     compare_code_files(cur_module,cur_module_archive, "verilog",
					allow_module_name_change_flag) == 1)
		   {
		    new_module_already_exists_flag = 1;
		    break;
		   }
		}
 	     }
        if (i == 0)
	  {
           cur_module_archive = cur_module_archive->next;
	  }
        else
	  {
           // add change in CadenceLib and SimRuns
           // change module name in extracted Import_Export module
           change_module_name_in_archive_sue_modules(first_module_archive,
                              cur_module_archive,new_module_name);
           // change module filename and associated sue2 file

	   if (new_module_already_exists_flag == 0)
	     {
              change_module_name_in_archive(cur_module_archive,new_module_name);
              change_module_name_in_code_file(cur_module_archive,new_module_name);
              change_module_name_in_verilog_code_file(cur_module_archive,new_module_name);
              sprintf(cur_module_archive->name,"%s",new_module_name);
	     }
	   else
	     {
	       printf("... not including redundant archive module '%s' ...\n\n",
                       cur_module_archive->name);
	       if (cur_module_archive == first_module_archive) // delete first entry in list
		 {
		   first_module_archive = cur_module_archive->next;
		 }
	       else // find and delete entry within list
		 {
		   temp_module_archive = first_module_archive;
		   while (temp_module_archive != NULL)
		     {
		       if (temp_module_archive->next == cur_module_archive)
			   break;
		       temp_module_archive = temp_module_archive->next;
		     }
		   if (temp_module_archive == NULL)
		     {
                       printf("error in 'verify_module_name_collisions':\n");
		       printf("  can't locate module '%s'\n",cur_module_archive->name);
		       printf("  -> this should never happen!\n");
		       exit(1);
		     }
		   temp_module_archive->next = cur_module_archive->next;
		 }
	     }

           // must start the process over again since various
           // imported sue modules were affected by the above name change
           cur_module_archive = first_module_archive;
	  }
   }
return(first_module_archive);
}


void add_new_cppsim_library_to_sue_lib_file(char *sue_lib_filename,
                                            char *new_cppsim_library_name)
{
FILE *sue_lib_file;
LINE *line_set;
ARGUMENT *args;
int i,end_of_file_flag, linecount, make_dir_flag;
char cur_line[LINESIZE];
char new_dir_name[MAX_ARG];
const char *cppsim_home_dir_const = NULL;
char cppsim_home_dir[MAX_CHAR_LENGTH];
DIR *dir;

if ((sue_lib_file = fopen(sue_lib_filename,"r")) == NULL)
  {
   printf("error in 'add_new_cppsim_library_to_sue_lib_file':\n");
   printf("    can't open sue.lib file:  '%s'\n", sue_lib_filename);
   exit(1);
 }

line_set = init_line();
line_set->num_of_lines = 1;
args = init_argument();

end_of_file_flag = 0;
linecount = 0;
make_dir_flag = 0;

while(end_of_file_flag == 0)
  {
    end_of_file_flag = extract_lineset_install(sue_lib_file,
                           line_set,sue_lib_filename,&linecount);
    extract_arguments(line_set,sue_lib_filename,linecount++,args);
    if (args->num_of_args != 1)
      {
	printf("error in 'add_new_cppsim_library_to_sue_lib':\n");
        printf("   sue.lib file '%s'\n",sue_lib_filename);
        printf("   has more than one library name on a line\n");
        printf("   line reads:\n");
        send_line_set_to_std_output(line_set);
        exit(1);
      }
    if (strcmp(args->arg_array[0],new_cppsim_library_name) == 0)
      {
       fclose(sue_lib_file);
       return;
      }
  }
fclose(sue_lib_file);


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

sprintf(new_dir_name,"%s/SueLib/%s",cppsim_home_dir,new_cppsim_library_name);
if ((dir = opendir(new_dir_name)) == NULL) 
   {
    if (makedir(new_dir_name) != 0)
       {
        printf("error in 'add_new_cppsim_library_to_sue_lib':\n");
        printf("     cannot create new CppSim/SueLib directory:\n");
        printf("     '%s'\n",new_dir_name);
        exit(1);
       }
    make_dir_flag = 1;
   } 
else
   closedir(dir);

//////////////////////////////////////////////////////////////////
if (make_dir_flag == 1)
   {
   if ((sue_lib_file = fopen(sue_lib_filename,"a")) == NULL)
     {
      printf("error in 'add_new_cppsim_library_to_sue_lib_file':\n");
      printf("    can't append to sue.lib file:  '%s'\n", sue_lib_filename);
      exit(1);
     }
   sprintf(cur_line,"%s\n",new_cppsim_library_name);
   fputs(cur_line,sue_lib_file);
   fclose(sue_lib_file);
   }
else
   {
    printf("error in 'import_cppsim_modules':\n");
    printf("   the choice of the new library name as\n");
    printf("   '%s' is invalid\n",new_cppsim_library_name);
    printf("   -> the new library name must already be added\n");
    printf("      to the sue.lib file OR it must correspond\n");
    printf("      to a non-existent PRIVATE directory\n");
    printf("      In this case, the following private directory ALREADY exists:\n");
    printf("      '%s'\n",new_dir_name);
    exit(1);
   }
}

void import_cppsim_modules(MODULE *first_module, MODULE *first_module_archive,
                    char *new_cppsim_lib_name)
{
const char *cppsim_home_dir_const = NULL;
char cppsim_home_dir[MAX_CHAR_LENGTH];
MODULE *cur_module, *cur_module_archive;
char new_lib_dir_name[MAX_ARG];
char old_lib_dir_name[MAX_ARG];
char new_file_name[MAX_ARG];
char old_file_name[MAX_ARG];
char archive_simruns_dir_name[MAX_ARG];
char archive_cadencelib_dir_name[MAX_ARG];
int  cppsim_code_exists_flag, simruns_code_exists_flag;
FILE *fp;
DIR *dir;
int i, num_of_imported_modules;

num_of_imported_modules = 0;

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


////////////// mark modules for transfer ////////////

cur_module_archive = first_module_archive;
while (cur_module_archive != NULL)
   {
    cur_module = first_module;
    while(cur_module != NULL)
       {
	if (strcmp(cur_module->name, cur_module_archive->name) == 0)
	   break;
	cur_module = cur_module->next;
       }
    if (cur_module == NULL)
      {
	cur_module_archive->active_sue_module_flag = 1;
        sprintf(new_lib_dir_name,"%s/SimRuns/%s/%s",cppsim_home_dir,
                   new_cppsim_lib_name,cur_module_archive->name);

        if ((dir = opendir(new_lib_dir_name)) != NULL) 
           {
            printf("error in 'import_cppsim_modules':\n");
            printf("     SimRun directory already exists for imported module\n");
            printf("     in library that is being imported into\n");
            printf("     Library: '%s', module: '%s'\n",
                         new_cppsim_lib_name, cur_module_archive->name);
            printf("     Full directory: '%s'\n",new_lib_dir_name);
            printf("     -> You must delete this directory and then\n");
            printf("        re-run this program to import your modules\n");
            exit(1);
           } 
      }
    else
      {
	cur_module_archive->active_sue_module_flag = 0;
        extract_simruns_dir_name(cur_module_archive->full_filename,
                                   archive_simruns_dir_name);
        sprintf(old_lib_dir_name,"%s/%s/%s",
		  archive_simruns_dir_name,
                  cur_module_archive->library_path,
                  cur_module_archive->name);

        if ((dir = opendir(old_lib_dir_name)) != NULL) 
           {
            printf("Warning in 'import_cppsim_modules':\n");
            printf("     SimRun directory exists for an imported module that will\n");
            printf("     NOT be transferred into the CppSim package since its\n");
            printf("     Sue2 file and CppSim/Verilog code are the same as an existing module\n");
            printf("     Library: '%s', module: '%s'\n",
                         cur_module_archive->library_path, 
                         cur_module_archive->name);
            printf("     Full directory: '%s'\n",old_lib_dir_name);
	   }
      }
  
    cur_module_archive = cur_module_archive->next;
   }



///// Create new SueLib Sub-Directory if needed

sprintf(new_lib_dir_name,"%s/SueLib",cppsim_home_dir);
if ((dir = opendir(new_lib_dir_name)) == NULL) 
   {
    printf("Error in 'import_cppsim_modules':\n");
    printf("   -> cannot open CppSim/SueLib directory:\n");
    printf("      '%s'\n",new_lib_dir_name);
    exit(1);
   } 
else
   closedir(dir);

sprintf(new_lib_dir_name,"%s/SueLib/%s",cppsim_home_dir,
        new_cppsim_lib_name);

if ((dir = opendir(new_lib_dir_name)) == NULL) 
   {
    if (makedir(new_lib_dir_name) != 0)
       {
        printf("error in 'import_cppsim_modules':\n");
        printf("     cannot create new CppSim/SueLib sub-directory:\n");
        printf("     '%s'\n",new_lib_dir_name);
        exit(1);
       }
   } 
else
   closedir(dir);

///// Create new CadenceLib Sub-Directory if needed

sprintf(new_lib_dir_name,"%s/CadenceLib",cppsim_home_dir);
if ((dir = opendir(new_lib_dir_name)) == NULL) 
   {
    printf("Error in 'import_cppsim_modules':\n");
    printf("   -> cannot open CppSim/CadenceLib directory:\n");
    printf("      '%s'\n",new_lib_dir_name);
    exit(1);
   } 
else
   closedir(dir);

sprintf(new_lib_dir_name,"%s/CadenceLib/%s",cppsim_home_dir,
        new_cppsim_lib_name);

if ((dir = opendir(new_lib_dir_name)) == NULL) 
   {
    if (makedir(new_lib_dir_name) != 0)
       {
        printf("error in 'import_cppsim_modules':\n");
        printf("     cannot create new CppSim/CadenceLib sub-directory:\n");
        printf("     '%s'\n",new_lib_dir_name);
        exit(1);
       }
   } 
else
   closedir(dir);

///// Create new SimRuns Sub-Directory if needed

sprintf(new_lib_dir_name,"%s/SimRuns",cppsim_home_dir);
if ((dir = opendir(new_lib_dir_name)) == NULL) 
   {
    printf("Error in 'import_cppsim_modules':\n");
    printf("   -> cannot open CppSim/SimRuns directory:\n");
    printf("      '%s'\n",new_lib_dir_name);
    exit(1);
   } 
else
   closedir(dir);

sprintf(new_lib_dir_name,"%s/SimRuns/%s",cppsim_home_dir,
        new_cppsim_lib_name);

if ((dir = opendir(new_lib_dir_name)) == NULL) 
   {
    if (makedir(new_lib_dir_name) != 0)
       {
        printf("error in 'import_cppsim_modules':\n");
        printf("     cannot create new CppSim/SimRuns sub-directory:\n");
        printf("     '%s'\n",new_lib_dir_name);
        exit(1);
       }
   } 
else
   closedir(dir);



cur_module_archive = first_module_archive;
   while (cur_module_archive != NULL)
      {
       if (cur_module_archive->active_sue_module_flag == 1)
         {
	   // Copy sue file
           sprintf(new_file_name,"%s/SueLib/%s/%s.sue",
		   cppsim_home_dir,new_cppsim_lib_name,
                   cur_module_archive->name);
           if ((fp = fopen(new_file_name,"r")) != NULL)
              {
               printf("error in 'import_cppsim_modules':\n");
               printf("   file %s.sue already exists!\n",
		      cur_module_archive->name);
               printf("   file info:  '%s'\n", new_file_name);
               printf("   -> this should never happen!?\n");
               exit(1);
              }

           copy_file(cur_module_archive->full_filename,
                     new_file_name);

           num_of_imported_modules++;
          ////////// Copy CadenceLib files

	  // get Import_Export CadenceLib directory
          extract_cadencelib_dir_name(cur_module_archive->full_filename,
                            archive_cadencelib_dir_name);
          sprintf(old_lib_dir_name,"%s/%s/%s/cppsim",
		  archive_cadencelib_dir_name,
                  cur_module_archive->library_path,
                  cur_module_archive->name);

          if ((dir = opendir(old_lib_dir_name)) != NULL) 
             {
	      cppsim_code_exists_flag = 1;
              sprintf(old_lib_dir_name,"%s/%s/%s",
		  archive_cadencelib_dir_name,
                  cur_module_archive->library_path,
                  cur_module_archive->name);
              closedir(dir);
             } 
          else
	    {
	      //	      printf("module '%s':  no cppsim file\n",cur_module_archive->name);
              sprintf(old_lib_dir_name,"%s/%s/%s/verilog",
		  archive_cadencelib_dir_name,
                  cur_module_archive->library_path,
                  cur_module_archive->name);
              if ((dir = opendir(old_lib_dir_name)) != NULL) 
                 {
		   //	      printf("-----> module '%s':  found a verilog file\n",cur_module_archive->name);
	          cppsim_code_exists_flag = 1;
                  sprintf(old_lib_dir_name,"%s/%s/%s",
		      archive_cadencelib_dir_name,
                      cur_module_archive->library_path,
                      cur_module_archive->name);
                  closedir(dir);
                 } 
              else
		{
	          cppsim_code_exists_flag = 0;
		}
	    }

         if (cppsim_code_exists_flag == 1)
	    {
             sprintf(new_lib_dir_name,"%s/CadenceLib/%s/%s",cppsim_home_dir,
                   new_cppsim_lib_name,cur_module_archive->name);


             if ((dir = opendir(new_lib_dir_name)) == NULL) 
               {
               if (makedir(new_lib_dir_name) != 0)
                 {
                 printf("error in 'import_cppsim_modules':\n");
                 printf("     cannot create new CppSim/CadenceLib sub-directory:\n");
                 printf("     '%s'\n",new_lib_dir_name);
                 exit(1);
                 }
               } 
             else
	       {
                printf("error in 'import_cppsim_modules':\n");
                printf("   CadenceLib subdirectory '%s/%s' already exists!\n",
		      new_cppsim_lib_name,cur_module_archive->name);
                printf("   dir info: '%s'\n",new_lib_dir_name);
                printf("   -> this should never happen!?\n");
                exit(1);
	       }
             recursively_copy_directory(old_lib_dir_name,new_lib_dir_name);
	    }
          ////////// Copy SimRun files

	  // get Import_Export SimRuns directory

          extract_simruns_dir_name(cur_module_archive->full_filename,
                                   archive_simruns_dir_name);
          sprintf(old_lib_dir_name,"%s/%s/%s",
		  archive_simruns_dir_name,
                  cur_module_archive->library_path,
                  cur_module_archive->name);

          if ((dir = opendir(old_lib_dir_name)) != NULL) 
             {
	      simruns_code_exists_flag = 1;
              closedir(dir);
             } 
          else
	    {
	      simruns_code_exists_flag = 0;
	    }

         if (simruns_code_exists_flag == 1)
	    {
             sprintf(new_lib_dir_name,"%s/SimRuns/%s/%s",cppsim_home_dir,
                   new_cppsim_lib_name,cur_module_archive->name);


             if ((dir = opendir(new_lib_dir_name)) == NULL) 
               {
               if (makedir(new_lib_dir_name) != 0)
                 {
                 printf("error in 'import_cppsim_modules':\n");
                 printf("     cannot create new CppSim/SimRuns sub-directory:\n");
                 printf("     '%s'\n",new_lib_dir_name);
                 exit(1);
                 }
               } 
             else
	       {
                printf("error in 'import_cppsim_modules':\n");
                printf("   SimRuns subdirectory '%s/%s' already exists!\n",
		      new_cppsim_lib_name,cur_module_archive->name);
                printf("   dir info: '%s'\n",new_lib_dir_name);
                printf("   -> this should never happen!?\n");
                exit(1);
	       }

            // Now do transfer of all files
            recursively_copy_directory(old_lib_dir_name,
					       new_lib_dir_name);
	    }
	 }
       cur_module_archive = cur_module_archive->next;
      }

if (num_of_imported_modules > 0)
   {
     if (num_of_imported_modules == 1)
        printf("\n\n     ******* Successfully imported %d module: *******\n\n",
              num_of_imported_modules);
     else
        printf("\n\n     ******* Successfully imported %d modules: *******\n\n",
              num_of_imported_modules);
    cur_module_archive = first_module_archive;
    while (cur_module_archive != NULL)
      {
       if (cur_module_archive->active_sue_module_flag == 1)
	 {
	   printf("    '%s'\n",cur_module_archive->name);
	 }
       cur_module_archive = cur_module_archive->next;
      }
     if (num_of_imported_modules == 1)
        printf("\n     ******* Successfully imported %d module: *******\n",
              num_of_imported_modules);
     else
        printf("\n     ******* Successfully imported %d modules: *******\n",
              num_of_imported_modules);
   }
else
   {
    printf("\n\n     ******* No modules need to be imported due to the fact that *******\n");
    printf("     ******* the current installation already has equivalent modules *******\n");
   }
}


void preview_cppsim_modules(MODULE *first_module, MODULE *first_module_archive,
                    char *new_cppsim_lib_name)
{
const char *cppsim_home_dir_const = NULL;
char cppsim_home_dir[MAX_CHAR_LENGTH];
MODULE *cur_module, *cur_module_archive;
char new_lib_dir_name[MAX_ARG];
char old_lib_dir_name[MAX_ARG];
char new_file_name[MAX_ARG];
char old_file_name[MAX_ARG];
char archive_simruns_dir_name[MAX_ARG];
char archive_cadencelib_dir_name[MAX_ARG];
int  cppsim_code_exists_flag, simruns_code_exists_flag;
FILE *fp;
DIR *dir;
int i, num_of_imported_modules;

num_of_imported_modules = 0;

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


////////////// mark modules for transfer ////////////

cur_module_archive = first_module_archive;
while (cur_module_archive != NULL)
   {
    cur_module = first_module;
    while(cur_module != NULL)
       {
	if (strcmp(cur_module->name, cur_module_archive->name) == 0)
	   break;
	cur_module = cur_module->next;
       }
    if (cur_module == NULL)
      {
	cur_module_archive->active_sue_module_flag = 1;
        sprintf(new_lib_dir_name,"%s/SimRuns/%s/%s",cppsim_home_dir,
                   new_cppsim_lib_name,cur_module_archive->name);

        if ((dir = opendir(new_lib_dir_name)) != NULL) 
           {
            printf("error in 'import_cppsim_modules':\n");
            printf("     SimRun directory already exists for imported module\n");
            printf("     in library that is being imported into\n");
            printf("     Library: '%s', module: '%s'\n",
                         new_cppsim_lib_name, cur_module_archive->name);
            printf("     Full directory: '%s'\n",new_lib_dir_name);
            printf("     -> You must delete this directory and then\n");
            printf("        re-run this program to import your modules\n");
            exit(1);
           } 
      }
    else
      {
	cur_module_archive->active_sue_module_flag = 0;
        extract_simruns_dir_name(cur_module_archive->full_filename,
                                   archive_simruns_dir_name);
        sprintf(old_lib_dir_name,"%s/%s/%s",
		  archive_simruns_dir_name,
                  cur_module_archive->library_path,
                  cur_module_archive->name);

        if ((dir = opendir(old_lib_dir_name)) != NULL) 
           {
            printf("Warning in 'import_cppsim_modules':\n");
            printf("     SimRun directory exists for an imported module that will\n");
            printf("     NOT be transferred into the CppSim package since its\n");
            printf("     Sue2 file and CppSim/Verilog code are the same as an existing module\n");
            printf("     Library: '%s', module: '%s'\n",
                         cur_module_archive->library_path, 
                         cur_module_archive->name);
            printf("     Full directory: '%s'\n",old_lib_dir_name);
	   }
      }
  
    cur_module_archive = cur_module_archive->next;
   }



///// Check SueLib Directory

sprintf(new_lib_dir_name,"%s/SueLib",cppsim_home_dir);
if ((dir = opendir(new_lib_dir_name)) == NULL) 
   {
    printf("Error in 'import_cppsim_modules':\n");
    printf("   -> cannot open CppSim/SueLib directory:\n");
    printf("      '%s'\n",new_lib_dir_name);
    exit(1);
   } 
else
   closedir(dir);


///// Check CadenceLib Directory

sprintf(new_lib_dir_name,"%s/CadenceLib",cppsim_home_dir);
if ((dir = opendir(new_lib_dir_name)) == NULL) 
   {
    printf("Error in 'import_cppsim_modules':\n");
    printf("   -> cannot open CppSim/CadenceLib directory:\n");
    printf("      '%s'\n",new_lib_dir_name);
    exit(1);
   } 
else
   closedir(dir);


///// Check SimRuns Directory

sprintf(new_lib_dir_name,"%s/SimRuns",cppsim_home_dir);
if ((dir = opendir(new_lib_dir_name)) == NULL) 
   {
    printf("Error in 'import_cppsim_modules':\n");
    printf("   -> cannot open CppSim/SimRuns directory:\n");
    printf("      '%s'\n",new_lib_dir_name);
    exit(1);
   } 
else
   closedir(dir);


cur_module_archive = first_module_archive;
while (cur_module_archive != NULL)
      {
       if (cur_module_archive->active_sue_module_flag == 1)
         {
           num_of_imported_modules++;
	 }
       cur_module_archive = cur_module_archive->next;
      }

if (num_of_imported_modules > 0)
   {
    printf("\n\n     ******* %d modules will be imported: *******\n\n",
          num_of_imported_modules);
    cur_module_archive = first_module_archive;
    while (cur_module_archive != NULL)
      {
       if (cur_module_archive->active_sue_module_flag == 1)
	 {
	   printf("    '%s'\n",cur_module_archive->name);
	 }
       cur_module_archive = cur_module_archive->next;
      }
    printf("\n     ******* %d modules will be imported: *******\n",
          num_of_imported_modules);
   }
else
   {
    printf("\n\n     ******* No modules need to be imported due to the fact that *******\n");
    printf("     ******* the current installation already has equivalent modules *******\n");
   }
}


void change_module_name_in_archive(MODULE *cur_module_archive,
                                   char *new_module_name)
{
FILE *fp,*fp_temp;
char cur_line[LINESIZE], temp_line[LINESIZE], cur_word[LINESIZE];
char temp_file_name[MAX_ARG], temp_word[LINESIZE];
int i,j,i_end_first,i_begin_second,found_module_name_flag;

if ((fp = fopen(cur_module_archive->full_filename,"r")) == NULL)
   {
   printf("error in 'change_module_name_in_archive':\n");
   printf("   can't open file:  '%s'\n", cur_module_archive->full_filename);
   exit(1);
   }
sprintf(temp_file_name,"%s",cur_module_archive->full_filename);
for (i = 0; temp_file_name[i] != '\0'; i++);
for (    ; temp_file_name[i] != '/'; i--);
i++;
temp_file_name[i] = '\0';
strcat(temp_file_name,new_module_name);
strcat(temp_file_name,".sue");

if ((fp_temp = fopen(temp_file_name,"w")) == NULL)
   {
   printf("error in 'change_module_name_in_archive':\n");
   printf("   can't open file (for write):  '%s'\n", temp_file_name);
   exit(1);
   }

while(1)
     {
      found_module_name_flag = 0;
      if (fgets(cur_line,LINESIZE-1,fp) == NULL)
         break;
      for (i = 0; cur_line[i] == ' '; i++);
      if (cur_line[i] == '\0')
	continue;
      j = 0;
      for (  ; cur_line[i] != '\0' && cur_line[i] != '\n' && cur_line[i] != '\r' && cur_line[i] != ' '; i++)
	 cur_word[j++] = cur_line[i]; 
      cur_word[j] = '\0';
      if (strcmp(cur_word,"proc") == 0)
	 {
          for (   ; cur_line[i] == ' '; i++);
          if (cur_line[i] == '\0')
	     continue;
          i_end_first = i;

          j = 0;
          for (  ; cur_line[i] != '\0' && cur_line[i] != '\n' && cur_line[i] != '\r' && cur_line[i] != ' '; i++)
	     cur_word[j++] = cur_line[i]; 
          cur_word[j] = '\0';
          i_begin_second = i;

          sprintf(temp_word,"SCHEMATIC_%s",cur_module_archive->name);
          if (strcmp(cur_word,temp_word) == 0)
	    {
	     sprintf(temp_word,"SCHEMATIC_%s",new_module_name);
 	     found_module_name_flag = 1;
	    }
          else
	    {
             sprintf(temp_word,"ICON_%s",cur_module_archive->name);
             if (strcmp(cur_word,temp_word) == 0)
	       {
	        sprintf(temp_word,"ICON_%s",new_module_name);
 	        found_module_name_flag = 1;
	       }
	    }
          if (found_module_name_flag == 1)
	    {
              j = 0;
	      for (i = 0; i < i_end_first; i++)
		 temp_line[j++] = cur_line[i];
              for (i = 0; temp_word[i] != '\0'; i++)
		{
		 if (j >= (LINESIZE-1))
		    {
		      printf("error in 'change_module_name_in_archive':\n");
                      printf("   linesize exceeded!?\n");
                      printf("   this should never happen...\n");
                      exit(1);
		    }
		 temp_line[j++] = temp_word[i];
		}
              for (i = i_begin_second; cur_line[i] != '\0'; i++)
		{
		 if (j >= (LINESIZE-1))
		    {
		      printf("error in 'change_module_name_in_archive':\n");
                      printf("   linesize exceeded!?\n");
                      printf("   this should never happen...\n");
                      exit(1);
		    }
		 temp_line[j++] = cur_line[i];
		}
	      temp_line[j] = '\0';
	    }
	  // put in replacement of icon_property line with module name
	 }
      else if (strcmp(cur_word,"icon_property") == 0)
	 {
          for (   ; cur_line[i] != '\0' && cur_line[i] != '\n' && cur_line[i] != '\r'; i++)
	    {
	     if (strncmp(&cur_line[i],"-type",5) == 0)
		break;
	    }
	  if (cur_line[i] != '\0' && cur_line[i] != '\n' && cur_line[i] != '\r')
	    {
             fputs(cur_line,fp_temp);
	     continue;
	    }
	  i--;
          for (   ; cur_line[i] == ' ' && i > 0; i--);
          for (   ; cur_line[i] != ' ' && i > 0; i--);
          if (i == 0)
	     continue;
          i++;
          i_end_first = i;

          j = 0;
          for (  ; cur_line[i] != '\0' && cur_line[i] != ' ' &&
                   cur_line[i] != '\n' && cur_line[i] != '\r'; i++)
	     cur_word[j++] = cur_line[i]; 
          cur_word[j] = '\0';
          if (strcmp(cur_word,cur_module_archive->name) == 0)
	    {
 	      found_module_name_flag = 1;
	      for (i = 0; i < i_end_first; i++)
                temp_line[i] = cur_line[i];
              for (j = 0; new_module_name[j] != '\0'; j++)
		{
		  if (i >= (LINESIZE-1))
		    {
		      printf("error in 'change_module_name_in_archive':\n");
                      printf("   line size exceeded!\n");
                      printf("   -> this should never happen!?\n");
                      exit(1);
		    }
                  temp_line[i++] = new_module_name[j];
		}
	      if (i >= (LINESIZE-3))
		 {
		  printf("error in 'change_module_name_in_archive':\n");
                  printf("   line size exceeded!\n");
                  printf("   -> this should never happen!?\n");
                  exit(1);
		 }
              temp_line[i++] = '\n';
              temp_line[i] = '\0';
	    }
	 }
     if (found_module_name_flag == 0)
        fputs(cur_line,fp_temp);
     else
        fputs(temp_line,fp_temp);
     }


fclose(fp); 
fclose(fp_temp);

remove(cur_module_archive->full_filename);
strcpy(cur_module_archive->full_filename,temp_file_name);
}


void change_module_name_in_code_file(MODULE *cur_module_archive,
                                char *new_module_name)
{
char cadence_lib_dir_archive[MAX_ARG];
char cppsim_code_file_archive[MAX_ARG];
char new_dir_name[MAX_ARG];
FILE *fp,*fp_temp;
char cur_line[LINESIZE], temp_line[LINESIZE], cur_word[LINESIZE];
char temp_file_name[MAX_ARG];
int i,j,i_end_first,i_begin_second,found_module_name_flag;
DIR *dir;

if (cur_module_archive == NULL)
   {
   printf("error in 'change_module_name_in_code_file':\n");
   printf("   cur_module_archive is NULL!\n");
   exit(1);
   }

extract_cadencelib_dir_name(cur_module_archive->full_filename, 
                            cadence_lib_dir_archive);
sprintf(cppsim_code_file_archive,"%s/%s/%s/cppsim/text.txt",
         cadence_lib_dir_archive,
	 cur_module_archive->library_path,cur_module_archive->name);

if ((fp = fopen(cppsim_code_file_archive,"r")) == NULL)
  {
    // this means that there was no code file for the imported module
    return;
    /*
    printf("error in 'change_module_name_in_code_file':\n");
    printf("     cannot open cppsim code for new module '%s'\n",
	   cur_module_archive->name);
    printf("     file name: %s\n",cppsim_code_file_archive);
    exit(1);
    */
  }

sprintf(new_dir_name,"%s/%s/%s",cadence_lib_dir_archive,
          cur_module_archive->library_path,new_module_name);
if ((dir = opendir(new_dir_name)) == NULL) 
   {
    if (makedir(new_dir_name) != 0)
       {
        printf("error in 'change_module_name_in_code_file':\n");
        printf("     cannot create new import_export directory:\n");
        printf("     '%s'\n",new_dir_name);
        exit(1);
       }
   } 
else
   {
   closedir(dir);
   printf("error in 'change_module_name_in_code_file':\n");
   printf("     CadenceLib directory already exists for:\n");
   printf("     cell '%s'.  Directory is:\n",new_module_name);
   printf("     '%s'\n",new_dir_name);
   printf("     ->  this should never happen!?\n");
   exit(1);
   }
strcat(new_dir_name,"/cppsim");
if ((dir = opendir(new_dir_name)) == NULL) 
   {
    if (makedir(new_dir_name) != 0)
       {
        printf("error in 'change_module_name_in_code_file':\n");
        printf("     cannot create new import_export directory:\n");
        printf("     '%s'\n",new_dir_name);
        exit(1);
       }
   } 
else
   {
   closedir(dir);
   printf("error in 'change_module_name_in_code_file':\n");
   printf("     CadenceLib directory already exists for:\n");
   printf("     cell '%s'.  Directory is:\n",new_module_name);
   printf("     '%s'\n",new_dir_name);
   printf("     ->  this should never happen!?\n");
   exit(1);
   }


sprintf(temp_file_name,"%s/text.txt",new_dir_name);
if ((fp_temp = fopen(temp_file_name,"w")) == NULL)
   {
   printf("error in 'change_module_name_in_code_file':\n");
   printf("   can't open file (for write):\n    '%s'\n", 
            temp_file_name);
   exit(1);
   }
   
while(1)
     {
      found_module_name_flag = 0;
      if (fgets(cur_line,LINESIZE-1,fp) == NULL)
         break;
      for (i = 0; cur_line[i] == ' '; i++);
      if (cur_line[i] == '\0')
	continue;
      j = 0;
      for (  ; cur_line[i] != '\0' && cur_line[i] != '\n' && cur_line[i] != '\r' && cur_line[i] != ' '; i++)
	 cur_word[j++] = cur_line[i]; 
      cur_word[j] = '\0';
      if (strcmp(cur_word,"module:") == 0)
	 {
          for (   ; cur_line[i] == ' '; i++);
          if (cur_line[i] == '\0')
	     continue;
          i_end_first = i;

          j = 0;
          for (  ; cur_line[i] != '\0' && cur_line[i] != '\n' && cur_line[i] != '\r' && cur_line[i] != ' '; i++)
	     cur_word[j++] = cur_line[i]; 
          cur_word[j] = '\0';
          i_begin_second = i;

          if (strcmp(cur_word,cur_module_archive->name) == 0)
	    {
	      found_module_name_flag = 1;
              j = 0;
	      for (i = 0; i < i_end_first; i++)
		 temp_line[j++] = cur_line[i];
              for (i = 0; new_module_name[i] != '\0'; i++)
		{
		 if (j >= (LINESIZE-1))
		    {
		      printf("error in 'change_module_name_in_code_file':\n");
                      printf("   linesize exceeded!?\n");
                      printf("   this should never happen...\n");
                      exit(1);
		    }
		 temp_line[j++] = new_module_name[i];
		}
              for (i = i_begin_second; cur_line[i] != '\0'; i++)
		{
		 if (j >= (LINESIZE-1))
		    {
		      printf("error in 'change_module_name_in_code_file':\n");
                      printf("   linesize exceeded!?\n");
                      printf("   this should never happen...\n");
                      exit(1);
		    }
		 temp_line[j++] = cur_line[i];
		}
	      temp_line[j] = '\0';
	    }
	  // put in replacement of icon_property line with module name
	 }

     if (found_module_name_flag == 0)
        fputs(cur_line,fp_temp);
     else
        fputs(temp_line,fp_temp);
     }

fclose(fp); 
fclose(fp_temp);

// copy master.tag file, too
sprintf(cppsim_code_file_archive,"%s/%s/%s/cppsim/master.tag",
         cadence_lib_dir_archive,
	 cur_module_archive->library_path,cur_module_archive->name);
if ((fp = fopen(cppsim_code_file_archive,"r")) == NULL)
   {
    return;
   }
sprintf(temp_file_name,"%s/master.tag",new_dir_name);
if ((fp_temp = fopen(temp_file_name,"w")) == NULL)
   {
   printf("error in 'change_module_name_in_code_file':\n");
   printf("   can't open file (for write):\n    '%s'\n", 
            temp_file_name);
   exit(1);
   }

while(1)
     {
      if (fgets(cur_line,LINESIZE-1,fp) == NULL)
         break;
       fputs(cur_line,fp_temp);
     }
fclose(fp);
fclose(fp_temp);

sprintf(cppsim_code_file_archive,"%s/%s/%s",
         cadence_lib_dir_archive,
	 cur_module_archive->library_path,cur_module_archive->name);

}

void change_module_name_in_verilog_code_file(MODULE *cur_module_archive,
                                char *new_module_name)
{
char cadence_lib_dir_archive[MAX_ARG];
char cppsim_code_file_archive[MAX_ARG];
char new_dir_name[MAX_ARG];
FILE *fp,*fp_temp;
char cur_line[LINESIZE], temp_line[LINESIZE], cur_word[LINESIZE];
char temp_file_name[MAX_ARG];
int i,j,i_end_first,i_begin_second,found_module_name_flag;
DIR *dir;

if (cur_module_archive == NULL)
   {
   printf("error in 'change_module_name_in_verilog_code_file':\n");
   printf("   cur_module_archive is NULL!\n");
   exit(1);
   }

extract_cadencelib_dir_name(cur_module_archive->full_filename, 
                            cadence_lib_dir_archive);
sprintf(cppsim_code_file_archive,"%s/%s/%s/verilog/verilog.v",
         cadence_lib_dir_archive,
	 cur_module_archive->library_path,cur_module_archive->name);

if ((fp = fopen(cppsim_code_file_archive,"r")) == NULL)
  {
    // this means that there was no code file for the imported module
    return;
    /*
    printf("error in 'change_module_name_in_verilog_code_file':\n");
    printf("     cannot open verilog code for new module '%s'\n",
	   cur_module_archive->name);
    printf("     file name: %s\n",cppsim_code_file_archive);
    exit(1);
    */
  }

sprintf(new_dir_name,"%s/%s/%s",cadence_lib_dir_archive,
          cur_module_archive->library_path,new_module_name);
if ((dir = opendir(new_dir_name)) == NULL) 
   {
    if (makedir(new_dir_name) != 0)
       {
        printf("error in 'change_module_name_in_verilog_code_file':\n");
        printf("     cannot create new import_export directory:\n");
        printf("     '%s'\n",new_dir_name);
        exit(1);
       }
   } 
else
   {
   closedir(dir);
   /***********
   **   OK if it already exists since 'change_module_name_in_code_file' may have
   **   created it
   printf("error in 'change_module_name_in_verilog_code_file':\n");
   printf("     CadenceLib directory already exists for:\n");
   printf("     cell '%s'.  Directory is:\n",new_module_name);
   printf("     '%s'\n",new_dir_name);
   printf("     ->  this should never happen!?\n");
   exit(1);
   ************/
   }
strcat(new_dir_name,"/verilog");
if ((dir = opendir(new_dir_name)) == NULL) 
   {
    if (makedir(new_dir_name) != 0)
       {
        printf("error in 'change_module_name_in_verilog_code_file':\n");
        printf("     cannot create new import_export directory:\n");
        printf("     '%s'\n",new_dir_name);
        exit(1);
       }
   } 
else
   {
   closedir(dir);
   printf("error in 'change_module_name_in_verilog_code_file':\n");
   printf("     CadenceLib directory already exists for:\n");
   printf("     cell '%s'.  Directory is:\n",new_module_name);
   printf("     '%s'\n",new_dir_name);
   printf("     ->  this should never happen!?\n");
   exit(1);
   }


sprintf(temp_file_name,"%s/verilog.v",new_dir_name);
if ((fp_temp = fopen(temp_file_name,"w")) == NULL)
   {
   printf("error in 'change_module_name_in_verilog_code_file':\n");
   printf("   can't open file (for write):\n    '%s'\n", 
            temp_file_name);
   exit(1);
   }
   
while(1)
     {
      found_module_name_flag = 0;
      if (fgets(cur_line,LINESIZE-1,fp) == NULL)
         break;
      for (i = 0; cur_line[i] == ' '; i++);
      if (cur_line[i] == '\0')
	continue;
      j = 0;
      for (  ; cur_line[i] != '\0' && cur_line[i] != '\n' && cur_line[i] != '\r' && cur_line[i] != ' '; i++)
	 cur_word[j++] = cur_line[i]; 
      cur_word[j] = '\0';
      if (strcmp(cur_word,"module") == 0 && cur_line[i] == ' ')
	 {
	   //	  printf("found module statement '%s' -> '%s'\n",cur_module_archive->name,new_module_name);
          for (   ; cur_line[i] == ' '; i++);
          if (cur_line[i] == '\0')
	     continue;
          i_end_first = i;

          j = 0;
          for (  ; cur_line[i] != '\0' && cur_line[i] != '\n' && cur_line[i] != '\r' && cur_line[i] != ' ' && cur_line[i] != '('; i++)
	     cur_word[j++] = cur_line[i]; 
          cur_word[j] = '\0';
          i_begin_second = i;
	  //	  printf("---> check: '%s' vs '%s'\n",cur_word, cur_module_archive->name);
          if (strcmp(cur_word,cur_module_archive->name) == 0 && cur_line[i] == '(')
	    {
	      //	      printf("  **** found!!\n");
	      found_module_name_flag = 1;
              j = 0;
	      for (i = 0; i < i_end_first; i++)
		 temp_line[j++] = cur_line[i];
              for (i = 0; new_module_name[i] != '\0'; i++)
		{
		 if (j >= (LINESIZE-1))
		    {
		      printf("error in 'change_module_name_in_verilog_code_file':\n");
                      printf("   linesize exceeded!?\n");
                      printf("   this should never happen...\n");
                      exit(1);
		    }
		 temp_line[j++] = new_module_name[i];
		}
              for (i = i_begin_second; cur_line[i] != '\0'; i++)
		{
		 if (j >= (LINESIZE-1))
		    {
		      printf("error in 'change_module_name_in_verilog_code_file':\n");
                      printf("   linesize exceeded!?\n");
                      printf("   this should never happen...\n");
                      exit(1);
		    }
		 temp_line[j++] = cur_line[i];
		}
	      temp_line[j] = '\0';
	    }
	  // put in replacement of icon_property line with module name
	 }

     if (found_module_name_flag == 0)
        fputs(cur_line,fp_temp);
     else
        fputs(temp_line,fp_temp);
     }

fclose(fp); 
fclose(fp_temp);

// copy master.tag file, too
sprintf(cppsim_code_file_archive,"%s/%s/%s/verilog/master.tag",
         cadence_lib_dir_archive,
	 cur_module_archive->library_path,cur_module_archive->name);
if ((fp = fopen(cppsim_code_file_archive,"r")) == NULL)
   {
    return;
   }
sprintf(temp_file_name,"%s/master.tag",new_dir_name);
if ((fp_temp = fopen(temp_file_name,"w")) == NULL)
   {
   printf("error in 'change_module_name_in_verilog_code_file':\n");
   printf("   can't open file (for write):\n    '%s'\n", 
            temp_file_name);
   exit(1);
   }

while(1)
     {
      if (fgets(cur_line,LINESIZE-1,fp) == NULL)
         break;
       fputs(cur_line,fp_temp);
     }
fclose(fp);
fclose(fp_temp);

sprintf(cppsim_code_file_archive,"%s/%s/%s",
         cadence_lib_dir_archive,
	 cur_module_archive->library_path,cur_module_archive->name);

}


void copy_over_simruns_files(MODULE *cur_module_archive)
{
char simruns_dir_archive[MAX_ARG];
char new_dir_name[MAX_ARG], old_dir_name[MAX_ARG];
DIR *dir;


if (cur_module_archive == NULL)
   {
   printf("error in 'copy_over_simruns_files':\n");
   printf("   cur_module_archive is NULL!\n");
   exit(1);
   }


extract_simruns_dir_name(cur_module_archive->full_filename, 
                            simruns_dir_archive);
sprintf(old_dir_name,"%s/%s/%s",
         simruns_dir_archive,
	 cur_module_archive->library_path,cur_module_archive->old_name);
sprintf(new_dir_name,"%s/%s/%s",
         simruns_dir_archive,
	 cur_module_archive->library_path,cur_module_archive->name);


if ((dir = opendir(old_dir_name)) == NULL) 
   return;
else
   closedir(dir);


if ((dir = opendir(new_dir_name)) == NULL) 
   {
    if (makedir(new_dir_name) != 0)
       {
        printf("error in 'copy_over_simruns_files':\n");
        printf("     cannot create new import_export directory:\n");
        printf("     '%s'\n",new_dir_name);
        exit(1);
       }
   } 
else
   {
   closedir(dir);
   printf("error in 'copy_over_simruns_files':\n");
   printf("     SimRuns directory already exists for:\n");
   printf("     cell '%s'.  Directory is:\n",cur_module_archive->name);
   printf("     '%s'\n",new_dir_name);
   printf("     ->  this should never happen!?\n");
   exit(1);
   }

recursively_copy_directory(old_dir_name, new_dir_name);

}


void change_module_name_in_archive_sue_modules(MODULE *first_module_archive,
                        MODULE *altered_module_archive, char *new_module_name)
{
FILE *fp,*fp_temp;
MODULE *cur_module;
char cur_line[LINESIZE], temp_line[LINESIZE], cur_word[LINESIZE];
char temp_file_name[MAX_ARG];
int i,j,i_end_first,i_begin_second,found_module_name_flag;


cur_module = first_module_archive;
while (cur_module != NULL)
   {
   if ((fp = fopen(cur_module->full_filename,"r")) == NULL)
     {
      printf("error in 'change_module_name_in_archive_sue_modules':\n");
      printf("   can't open file:  '%s'\n", cur_module->full_filename);
      exit(1);
     }
   found_module_name_flag = 0;
   while(1)
     {
      if (fgets(cur_line,LINESIZE-1,fp) == NULL)
         break;
      for (i = 0; cur_line[i] == ' '; i++);
      if (cur_line[i] == '\0')
	continue;
      j = 0;
      for (  ; cur_line[i] != '\0' && cur_line[i] != ' '; i++)
	 cur_word[j++] = cur_line[i]; 
      cur_word[j] = '\0';
      if (strcmp(cur_word,"make") == 0)
	 {
          for (   ; cur_line[i] == ' '; i++);
          if (cur_line[i] == '\0')
	     continue;

          j = 0;
          for (  ; cur_line[i] != '\0' && cur_line[i] != ' '; i++)
	     cur_word[j++] = cur_line[i]; 
          cur_word[j] = '\0';

          if (strcmp(cur_word,altered_module_archive->name) == 0)
	    {
	     found_module_name_flag = 1;
             break;
	    }
	 }
     }
   fclose(fp); 

   // rewrite file with updated module name
   if (found_module_name_flag == 1) 
     {
      cur_module->active_sue_module_flag = 1;
      if ((fp = fopen(cur_module->full_filename,"r")) == NULL)
        {
         printf("error in 'change_module_name_in_archive_sue_modules':\n");
         printf("   can't open file:  '%s'\n", cur_module->full_filename);
         exit(1);
        }
      sprintf(temp_file_name,"%s_temp_",cur_module->full_filename);
      if ((fp_temp = fopen(temp_file_name,"w")) == NULL)
        {
         printf("error in 'change_module_name_in_archive_sue_modules':\n");
         printf("   can't open file (for write):  '%s'\n", temp_file_name);
         exit(1);
        }
      while(1)
        {
	found_module_name_flag = 0;
        if (fgets(cur_line,LINESIZE-1,fp) == NULL)
           break;
        for (i = 0; cur_line[i] == ' '; i++);
        if (cur_line[i] == '\0')
  	   continue;
        j = 0;
        for (  ; cur_line[i] != '\0' && cur_line[i] != ' '; i++)
	   cur_word[j++] = cur_line[i]; 
        cur_word[j] = '\0';
        if (strcmp(cur_word,"make") == 0)
	  {
          for (   ; cur_line[i] == ' '; i++);
          if (cur_line[i] == '\0')
	     continue;
          i_end_first = i;
          j = 0;
          for (  ; cur_line[i] != '\0' && cur_line[i] != ' '; i++)
	     cur_word[j++] = cur_line[i]; 
          cur_word[j] = '\0';
          i_begin_second = i;
          if (strcmp(cur_word,altered_module_archive->name) == 0)
	    {
              found_module_name_flag = 1;
	      for (i = 0; i < i_end_first; i++)
                 temp_line[i] = cur_line[i];
              for (j = 0; new_module_name[j] != '\0'; j++)
		{
		  if (i >= (LINESIZE-1))
		    {
		      printf("error in 'change_module_name_in_archive_sue_modules':\n");
                      printf("    new module name must be too long!\n");
                      printf("    new name: '%s'\n",new_module_name);
                      exit(1);
		    }
		  temp_line[i++] = new_module_name[j]; 
		}
              for (j = i_begin_second; cur_line[j] != '\0'; j++)
		{
		  if (i >= (LINESIZE-1))
		    {
		      printf("error in 'change_module_name_in_archive_sue_modules':\n");
                      printf("    new module name must be too long!\n");
                      printf("    new name: '%s'\n",new_module_name);
                      exit(1);
		    }
                  temp_line[i++] = cur_line[j];
		}
              temp_line[i] = '\0';
	      //   printf("new line (%s): %s\n",cur_module->name,temp_line); 
	    }
	  }
        if (found_module_name_flag == 0)
	    fputs(cur_line,fp_temp);
        else
	    fputs(temp_line,fp_temp);
	}
      fclose(fp); 
      fclose(fp_temp);

      copy_file(temp_file_name,cur_module->full_filename);
      remove(temp_file_name);
     }

   cur_module = cur_module->next;
   }

}

int compare_code_files(MODULE *cur_module, MODULE *cur_module_archive, const char *code_type,
                       int name_change_flag)
{
char cadence_lib_dir[MAX_ARG];
char cadence_lib_dir_archive[MAX_ARG];
char cppsim_code_file[MAX_ARG];
char cppsim_code_file_archive[MAX_ARG];
FILE *fp, *fp_archive;
int end_of_file_flag, end_of_file_flag_archive, diff_flag;
int end_file_compare_flag;
LINE *line_set, *line_set_archive;
ARGUMENT *args, *args_archive;
int i, linecount, linecount_archive;


if (cur_module == NULL)
   {
   printf("error in 'compare_code_files':\n");
   printf("   cur_module is NULL!\n");
   exit(1);
   }
if (cur_module_archive == NULL)
   {
   printf("error in 'compare_code_files':\n");
   printf("   cur_module_archive is NULL!\n");
   exit(1);
   }

extract_cadencelib_dir_name(cur_module->full_filename, 
                            cadence_lib_dir);
extract_cadencelib_dir_name(cur_module_archive->full_filename, 
                            cadence_lib_dir_archive);

if (strcmp(code_type,"cppsim") == 0)
   {
   sprintf(cppsim_code_file,"%s/%s/%s/cppsim/text.txt",cadence_lib_dir,
	 cur_module->library_path,cur_module->name);
   sprintf(cppsim_code_file_archive,"%s/%s/%s/cppsim/text.txt",
         cadence_lib_dir_archive,
	 cur_module_archive->library_path,cur_module_archive->name);
   }
else if (strcmp(code_type,"verilog") == 0)
   {
   sprintf(cppsim_code_file,"%s/%s/%s/verilog/verilog.v",cadence_lib_dir,
	 cur_module->library_path,cur_module->name);
   sprintf(cppsim_code_file_archive,"%s/%s/%s/verilog/verilog.v",
         cadence_lib_dir_archive,
	 cur_module_archive->library_path,cur_module_archive->name);
   }
else
   {
   printf("error in 'compare_code_files':\n");
   printf("   code_type must be either 'cppsim' or 'verilog'\n");
   printf("   in this case, code_type = '%s'\n",code_type);
   exit(1);
   }


fp = fopen(cppsim_code_file,"r");
fp_archive = fopen(cppsim_code_file_archive,"r");

if ((fp == NULL && fp_archive != NULL) ||
    (fp != NULL && fp_archive == NULL))
  {
   return(0);   
  }
if (fp == NULL && fp_archive == NULL)
   return(1);


line_set = init_line();
line_set_archive = init_line();
line_set->num_of_lines = 1;
line_set_archive->num_of_lines = 1;
args = init_argument();
args_archive = init_argument();

end_of_file_flag = 0;
end_of_file_flag_archive = 0;
end_file_compare_flag = 0;
diff_flag = 0;
linecount = 0;
linecount_archive = 0;
   
while(end_file_compare_flag == 0)
   {
    if (end_of_file_flag == 0)
        end_of_file_flag = extract_lineset_install(fp,
                           line_set,cppsim_code_file, 
                           &linecount);
    else
      {
       line_set->num_of_lines = 1;
       line_set->line_array[0][0] = '\0';
       args->arg_array[0][0] = '\0';
      }
    if (end_of_file_flag_archive == 0)
        end_of_file_flag_archive = extract_lineset_install2(fp_archive,
                           line_set_archive,cppsim_code_file_archive, 
                           &linecount_archive);
    else
      {
       line_set_archive->num_of_lines = 1;
       line_set_archive->line_array[0][0] = '\0';
       args_archive->arg_array[0][0] = '\0';
      }

    if (end_of_file_flag == 1 && end_of_file_flag_archive == 1)
	  end_file_compare_flag = 1;

    extract_arguments(line_set,cppsim_code_file,
                          linecount++,args);
    extract_arguments(line_set_archive,cppsim_code_file_archive,
                        linecount_archive++,args_archive);


   if (args->num_of_args != args_archive->num_of_args)
      {
      diff_flag = 1;
      break;
      }

   for (i = 0; i < args->num_of_args; i++)
     {
       if (strcmp(args->arg_array[i],args_archive->arg_array[i]) != 0)
	 {
	  if (name_change_flag == 1)
	     {
	       if (i == 1 && (strcmp(args->arg_array[0],"module:") == 0 ||
                              strcmp(args->arg_array[0],"module") == 0))
		 {
		  continue;
		 }
               else
		 {
		   diff_flag = 1;
                   break;
		 }
	     }
          else
	    {
	     diff_flag = 1;
             break;
	    }
	 }
     }
   if (diff_flag == 1)
      break;
   }


free(line_set);
free(line_set_archive);
free(args);
free(args_archive);
fclose(fp);
fclose(fp_archive);

if (diff_flag == 0)
   return(1);
 else
   return(0);

}

int extract_lineset_install(FILE *in, LINE *line_set, char *filename, int *linecount)
{
int i;
static int first_char;

line_set->line_array[0][0] = '\0';
if ((*linecount) == 0)
   {
     if ((first_char = fgetc(in)) == EOF)
        {
         printf("error in '%s':\n   input file '%s' is empty!\n",
	   "extract_lineset (for 'add_sue_module')",filename);
         exit(1);
        }
   }
line_set->num_of_lines = 0;
while(1)
     {
     (*linecount)++;
     if (first_char == ((int) '\n') || first_char == ((int) '\r'))
       {
        (*linecount)++;
        if ((first_char = fgetc(in)) == EOF)
           return(1);
       }

     line_set->line_array[line_set->num_of_lines][0] = (char) first_char;
     if (fgets(&(line_set->line_array[line_set->num_of_lines][1]),LINESIZE-1,in) == NULL)
       return(1);
     if ((first_char = fgetc(in)) == EOF)
       {
       line_set->num_of_lines++;
       return(1);
       }
     for (i = 0; line_set->line_array[line_set->num_of_lines][i] != '\0'; i++)
       if (line_set->line_array[line_set->num_of_lines][i] == '\\')
          if (line_set->line_array[line_set->num_of_lines][i+1] == '\n' ||
              line_set->line_array[line_set->num_of_lines][i+1] == '\r' ||
	      line_set->line_array[line_set->num_of_lines][i+1] == '\0' ||
	      line_set->line_array[line_set->num_of_lines][i+1] == ' ')
              break;

     if (line_set->line_array[line_set->num_of_lines][i] == '\\')
       {
	 line_set->line_array[line_set->num_of_lines][i] = ' ';
	 line_set->num_of_lines++;
	 if (line_set->num_of_lines >= MAX_NUM_LINES)
	   {
	     printf(" error in '%s':  too many '\\ lines' encountered\n","cdl2hs");
	     printf(" error occurred in input file '%s' on line %d\n",filename,
                    *linecount);
	     printf("   to fix, change MAX_NUM_LINES in source code\n");
	     printf("   (current value of MAX_NUM_LINES is %d)\n",MAX_NUM_LINES);
	     exit(1);
	   }
       }
     else if (first_char == (int) '+')
       {
        line_set->num_of_lines++;
        if (line_set->num_of_lines >= MAX_NUM_LINES)
	  {
           printf(" error in '%s':  too many '+ lines' encountered\n","hsps");
           printf(" error occurred in input file '%s' on line %d\n",filename,
                    *linecount);
           printf("   to fix, change MAX_NUM_LINES in source code\n");
           printf("   (current value of MAX_NUM_LINES is %d)\n",MAX_NUM_LINES);
           exit(1);
	  }
       }
     else
       break;
     }
line_set->num_of_lines++;
return(0);
}

int extract_lineset_install2(FILE *in, LINE *line_set, char *filename, int *linecount)
{
int i;
static int first_char;

line_set->line_array[0][0] = '\0';
if ((*linecount) == 0)
   {
     if ((first_char = fgetc(in)) == EOF)
        {
         printf("error in '%s':\n   input file '%s' is empty!\n",
	   "extract_lineset (for 'add_sue_module')",filename);
         exit(1);
        }
   }
line_set->num_of_lines = 0;
while(1)
     {
     (*linecount)++;
     if (first_char == ((int) '\n') || first_char == ((int) '\r'))
       {
        (*linecount)++;
        if ((first_char = fgetc(in)) == EOF)
           return(1);
       }

     line_set->line_array[line_set->num_of_lines][0] = (char) first_char;
     if (fgets(&(line_set->line_array[line_set->num_of_lines][1]),LINESIZE-1,in) == NULL)
       return(1);
     if ((first_char = fgetc(in)) == EOF)
       {
       line_set->num_of_lines++;
       return(1);
       }
     for (i = 0; line_set->line_array[line_set->num_of_lines][i] != '\0'; i++)
       if (line_set->line_array[line_set->num_of_lines][i] == '\\')
          if (line_set->line_array[line_set->num_of_lines][i+1] == '\n' ||
              line_set->line_array[line_set->num_of_lines][i+1] == '\r' ||
	      line_set->line_array[line_set->num_of_lines][i+1] == '\0' ||
	      line_set->line_array[line_set->num_of_lines][i+1] == ' ')
              break;

     if (line_set->line_array[line_set->num_of_lines][i] == '\\')
       {
	 line_set->line_array[line_set->num_of_lines][i] = ' ';
	 line_set->num_of_lines++;
	 if (line_set->num_of_lines >= MAX_NUM_LINES)
	   {
	     printf(" error in '%s':  too many '\\ lines' encountered\n","cdl2hs");
	     printf(" error occurred in input file '%s' on line %d\n",filename,
                    *linecount);
	     printf("   to fix, change MAX_NUM_LINES in source code\n");
	     printf("   (current value of MAX_NUM_LINES is %d)\n",MAX_NUM_LINES);
	     exit(1);
	   }
       }
     else if (first_char == (int) '+')
       {
        line_set->num_of_lines++;
        if (line_set->num_of_lines >= MAX_NUM_LINES)
	  {
           printf(" error in '%s':  too many '+ lines' encountered\n","hsps");
           printf(" error occurred in input file '%s' on line %d\n",filename,
                    *linecount);
           printf("   to fix, change MAX_NUM_LINES in source code\n");
           printf("   (current value of MAX_NUM_LINES is %d)\n",MAX_NUM_LINES);
           exit(1);
	  }
       }
     else
       break;
     }
line_set->num_of_lines++;
return(0);
}


int compare_sue_files(MODULE *cur_module, MODULE *cur_module_archive, 
                      int name_change_flag)
{
FILE *fp, *fp_archive;
int end_of_file_flag, end_of_file_flag_archive, diff_flag;
LINE *line_set, *line_set_archive;
ARGUMENT *args, *args_archive;
int i, linecount, linecount_archive;
int end_file_compare_flag;

if (cur_module == NULL)
   {
   printf("error in 'compare_sue_files':\n");
   printf("   cur_module is NULL!\n");
   exit(1);
   }
if (cur_module_archive == NULL)
   {
   printf("error in 'compare_sue_files':\n");
   printf("   cur_module_archive is NULL!\n");
   exit(1);
   }

if ((fp = fopen(cur_module->full_filename,"r")) == NULL)
  {
   printf("error in 'compare_sue_files':\n");
   printf("   can't open file:  '%s'\n", cur_module->full_filename);
   exit(1);
  }
if ((fp_archive = fopen(cur_module_archive->full_filename,"r")) == NULL)
  {
   printf("error in 'compare_sue_files':\n");
   printf("   can't open file:  '%s'\n", cur_module_archive->full_filename);
   exit(1);
  }

line_set = init_line();
line_set_archive = init_line();
line_set->num_of_lines = 1;
line_set_archive->num_of_lines = 1;
args = init_argument();
args_archive = init_argument();

end_of_file_flag = 0;
end_of_file_flag_archive = 0;
end_file_compare_flag = 0;
diff_flag = 0;
linecount = 0;
linecount_archive = 0;

while(end_file_compare_flag == 0)
   {
    if (end_of_file_flag == 0)
        end_of_file_flag = extract_lineset_install(fp,
                           line_set,cur_module->full_filename, 
                           &linecount);
    else
      {
       line_set->num_of_lines = 1;
       line_set->line_array[0][0] = '\0';
       args->arg_array[0][0] = '\0';
      }
    if (end_of_file_flag_archive == 0)
        end_of_file_flag_archive = extract_lineset_install2(fp_archive,
                           line_set_archive,cur_module_archive->full_filename, 
                           &linecount_archive);
    else
      {
       line_set_archive->num_of_lines = 1;
       line_set_archive->line_array[0][0] = '\0';
       args_archive->arg_array[0][0] = '\0';
      }

    if (end_of_file_flag == 1 && end_of_file_flag_archive == 1)
	  end_file_compare_flag = 1;

    extract_arguments(line_set,cur_module->full_filename,
                          linecount++,args);
    extract_arguments(line_set_archive,cur_module_archive->full_filename,
                        linecount_archive++,args_archive);
   
   if (args->num_of_args != args_archive->num_of_args)
      {
      diff_flag = 1;
      /*
	      printf("1) line = '%s'\n",line_set->line_array[0]);
	      printf("2) line = '%s'\n",line_set_archive->line_array[0]);
      for (i = 0; i < args->num_of_args; i++)
	printf("arg[%d] = '%s'\n",i,args->arg_array[i]);
      for (i = 0; i < args_archive->num_of_args; i++)
	printf("arg_archive[%d] = '%s'\n",i,args_archive->arg_array[i]);
      */
      break;
      }

   for (i = 0; i < args->num_of_args; i++)
     {
       if (strcmp(args->arg_array[i],args_archive->arg_array[i]) != 0)
	 {
	  if (name_change_flag == 1)
	     {
	       if (i == 1 && strcmp(args->arg_array[0],"proc") == 0)
		 {
		  continue;
		 }
               else if (i == (args->num_of_args-1) &&
                       strcmp(args->arg_array[0],"icon_property") == 0 &&
		       strcmp(args->arg_array[args->num_of_args-1],cur_module->name) == 0)
                  continue;
               else
		 {
		   diff_flag = 1;
                   break;
		 }
	     }
          else
	    {
	      /*
	      printf("-- 1) line = '%s'\n",line_set->line_array[0]);
	      printf("-- 2) line = '%s'\n",line_set_archive->line_array[0]);
	      */
	     diff_flag = 1;
             break;
	    }
	 }
     }
   if (diff_flag == 1)
      break;
   }

free(line_set);
free(line_set_archive);
free(args);
free(args_archive);
fclose(fp);
fclose(fp_archive);
if (diff_flag == 0)
   return(1);
else
   return(0);
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
   if (inpline[i] == '\0' || inpline[i] == '\n' || inpline[i] == '\r' || 
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
                 inpline[i] != ',' && inpline[i] != '\n' && inpline[i] != '\r' &&
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
int i,j,linecount,end_of_file_flag;
LINE *line_set;
ARGUMENT *args;
int module_comment_flag;
char cadence_dir[MAX_ARG], cppsim_filename[MAX_ARG];
char master_tag_filename[MAX_ARG];
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
       if (args->arg_array[i][0] != ' ' && args->arg_array[i][0] != '\n' && args->arg_array[i][0] != '\r')
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
        module_comment_flag = 1;
     continue;
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
	   if (strcmp(cur_module->name,args->arg_array[1]) == 0)
	      break;
	   }
        cur_module = cur_module->next;
        }

     if (cur_module == NULL)
       {
         module_comment_flag = 1;
         continue;
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
free(line_set);
free(args);
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
	    sprintf(next_dir_name,"%s/%s",cur_dir_name,entry->d_name);
 	    sprintf(next_archive_dir_name,"%s/%s",cur_archive_dir_name,
                                             entry->d_name);
	    recursively_copy_simruns_directory(next_dir_name, next_archive_dir_name);
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

if (strcmp(&file_name[i],".exe") == 0 ||
    strcmp(&file_name[i],".o") == 0 ||
    strcmp(&file_name[i],".cpp") == 0 ||
    strcmp(&file_name[i],".h") == 0 ||
    strcmp(&file_name[i],".cppsim") == 0 ||
    strcmp(&file_name[i],".hier_c") == 0 ||
    strcmp(&file_name[i],".hier_v") == 0 ||
    strcmp(&file_name[i],".outfile_list") == 0 ||
    strcmp(&file_name[i],".hier_c") == 0 ||
    strcmp(&file_name[i],".v") == 0 ||
    strcmp(&file_name[i],".out") == 0 ||
    strcmp(&file_name[i],".counter") == 0 ||
    (strcmp(&file_name[i],".m") == 0 &&
     strncmp(file_name,"compile_",8) == 0) ||
    strcmp(file_name,".cppsim_author_info") == 0 ||
    strcmp(file_name,"CppSimView_history.mat") == 0 ||
    strcmp(file_name,"Makefile") == 0 ||
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


void extract_simruns_dir_name(char *sue_file_name, char *simruns_library_name)
{
int i,count;
char temp[MAX_CHAR_LENGTH];
DIR *dir;

strcpy(simruns_library_name,sue_file_name);
for (i = 0; simruns_library_name[i] != '\0'; i++);
count = 0;
for (    ; i >= 0; i--)
 {
   if (simruns_library_name[i] == '/')
      count++;
   if (count == 3)
      break;
 }
if (i < 0)
   {
    printf("error in 'extract_simruns_dir_name':\n");
    printf("  sue file name seems to be wrong ...\n");
    printf("  file name: '%s'\n",sue_file_name);
    printf("  -> there should be at least three '/' characters\n");
    exit(1);
   }
if (strncmp(&simruns_library_name[i],"/SueLib/",8) != 0)
   {
    printf("error in 'extract_simruns_dir_name':\n");
    printf("  sue file name seems to be wrong ...\n");
    printf("  file name: '%s'\n",sue_file_name);
    printf("  -> there should be a '/SueLib/' string within the file name\n");
    exit(1);
   }

simruns_library_name[i] = '\0';
strcat(simruns_library_name,"/SimRuns");

if ((dir = opendir(simruns_library_name)) == NULL) 
   {
    printf("error in 'extract_simruns_dir_name':\n");
    printf("     cannot open SimRuns directory:\n");
    printf("     '%s'\n",simruns_library_name);
    exit(1);
   } 
else
   closedir(dir);

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
             printf("     cannot create new directory:\n");
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



