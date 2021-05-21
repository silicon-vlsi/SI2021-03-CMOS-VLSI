/**************************************************************************
 * FILE : create_cppsim_code_template.cpp
 * 
 * AUTHOR : Mike Perrott
 * 
 * DESCRIPTION : creates cppsim template for a given Sue2 cell.
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

main(int argc,char *argv[])
{
char sue_module_name[MAX_CHAR_LENGTH];
PARAMETER *param_list;
MODULE *cur_module;
int i, count, private_library_flag;
char sue_module_filename[1000], output_filename[1000];
FILE *fp;
MOD_PARAMETER *cur_mod_parameter;
NODES *cur_mod_node;
const char *cppsim_home_dir_const = NULL;
const char *cppsimshared_home_dir_const = NULL;
char cppsim_home_dir[MAX_CHAR_LENGTH], cppsimshared_home_dir[MAX_CHAR_LENGTH];
char dirname[1000],base_dir[1000],full_dir[1000];
DIR *dir;

if (argc != 3)
  {
   printf("Error: need two arguments!\n");
   printf("  create_cppsim_code_template sue_module sue_module_library\n");
   exit(1);
  }


///////////// CPPSIMHOME environment variable /////////

cppsim_home_dir_const = getenv("CPPSIMHOME");
if (cppsim_home_dir_const != NULL)
  {
  sprintf(cppsim_home_dir,cppsim_home_dir_const);
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

///////////// CPPSIMSHAREDHOME environment variable /////////

cppsimshared_home_dir_const = getenv("CPPSIMSHAREDHOME");
if (cppsimshared_home_dir_const != NULL)
  {
  sprintf(cppsimshared_home_dir,cppsimshared_home_dir_const);
  for (i = 0; cppsimshared_home_dir[i] != '\0'; i++)
    if (cppsimshared_home_dir[i] == '\\')
       cppsimshared_home_dir[i] = '/';
  }
else
  {
    printf("Error:  you must define UNIX environment variable CPPSIMSHAREDHOME\n");
    printf("  before running VppSim\n");
    printf("  -> include the following statement in your .cshrc[.mine] file:\n");
    printf("     setenv CPPSIMSHAREDHOME <Dir_Location>/CppSimShared\n");
    exit(1);
  }

if ((dir = opendir(cppsimshared_home_dir)) == NULL) 
   {
    printf("Error:  cannot open CppSimShared directory specified\n");
    printf("        by the UNIX environment variable CPPSIMSHAREDHOME = '%s'\n",
                    cppsimshared_home_dir);
    printf("  -> perhaps you need to redefine UNIX environment variable CPPSIMSHAREDHOME\n");
    printf("     within your .cshrc[.mine] file:\n");
    exit(1);
   } 
else
   {
   closedir(dir);
   }

/***
for (i = 0; argv[1][i] != '\0'; i++)
   sue_module_name[i] = tolower(argv[1][i]);
sue_module_name[i] = '\0';
***/
strcpy(sue_module_name,argv[1]);

strcpy(dirname,argv[2]);
private_library_flag = 0;

for (i = 0; dirname[i] != '\0'; i++);
for (    ; dirname[i] != ':' && i > 0; i--);
if (dirname[i] == ':')
   {
    if (strcmp(&dirname[i],":Private") == 0)
       {
         private_library_flag = 1;
         dirname[i] = '\0';
       }
    else
       {
	 printf("Error: in running 'create_cppsim_code_template':\n");
         printf("  unrecognized ':' character in library name\n");
         printf("  -> library:  %s\n",dirname);
         exit(1);
       }
   }

// Find the Sue2 file
sprintf(base_dir,"%s",cppsim_home_dir);
sprintf(full_dir,"%s/SueLib/%s",base_dir,dirname);
if ((dir = opendir(full_dir)) == NULL) 
   {
    sprintf(base_dir,"%s",cppsimshared_home_dir);
    sprintf(full_dir,"%s/SueLib/%s",base_dir,dirname);
    if ((dir = opendir(full_dir)) == NULL) 
       {
        printf("Error: in running 'create_cppsim_code_template':\n");
        printf("    Could not open directory '%s'\n",full_dir);

        sprintf(base_dir,"%s",cppsim_home_dir);
        sprintf(full_dir,"%s/SueLib/%s",base_dir,dirname);

	printf("    OR directory '%s'\n",full_dir);
        exit(1);
       }
    else if (private_library_flag == 1)
       {
	 printf("Error: in running 'create_cppsim_code_template':\n");
         printf("   input library '%s' can only be found\n",
                dirname);
         printf("   within the shared directory:\n");
         printf("   '%s'\n",full_dir);
         printf("   but was designated as being in the private directory:\n");
         sprintf(base_dir,"%s",cppsim_home_dir);
         sprintf(full_dir,"%s/SueLib/%s",base_dir,dirname);
	 printf("   %s\n",full_dir);
         exit(1);
       }
   }


sprintf(sue_module_filename,"%s/%s.sue",full_dir,sue_module_name);


param_list = init_parameter();
cur_module = NULL;

cur_module = add_sue_module(cur_module, sue_module_filename, 
			    argv[2], param_list,0);

///// Now create CppSim code template in associated CadenceLib //////

sprintf(full_dir,"%s/CadenceLib",base_dir);
if ((dir = opendir(full_dir)) == NULL) 
   {
    printf("Error:  cannot open CadenceLib directory associated with\n");
    printf("        CppSim(Shared) directory '%s'\n",base_dir);
    printf("  -> it looks like you need to place directory 'CadenceLib' in the above directory\n");
    exit(1);
   } 
else
   {
   closedir(dir);
   }

sprintf(full_dir,"%s/CadenceLib/%s",base_dir,dirname);
if ((dir = opendir(full_dir)) == NULL) 
   {
   if (makedir(full_dir) != 0)
       {
	printf("Error: in running 'create_cppsim_code_template':\n");
        printf("  Having problems with directory '%s'\n",full_dir);
        printf("  Either it exists and is not readable and executable\n");
        printf("  or it doesn't exist and cannot be created\n");
	exit(1);
       }
   }
else
   {
   closedir(dir);
   }

sprintf(full_dir,"%s/CadenceLib/%s/%s",base_dir,dirname,sue_module_name);
if ((dir = opendir(full_dir)) == NULL) 
   {
   if (makedir(full_dir) != 0)
       {
	printf("Error: in running 'create_cppsim_code_template':\n");
        printf("  Having problems with directory '%s'\n",full_dir);
        printf("  Either it exists and is not readable and executable\n");
        printf("  or it doesn't exist and cannot be created\n");
	exit(1);
       }
   }
else
   {
   closedir(dir);
   }

sprintf(full_dir,"%s/CadenceLib/%s/%s/cppsim",base_dir,dirname,sue_module_name);
if ((dir = opendir(full_dir)) == NULL) 
   {
   if (makedir(full_dir) != 0)
       {
	printf("Error: in running 'create_cppsim_code_template':\n");
        printf("  Having problems with directory '%s'\n",full_dir);
        printf("  Either it exists and is not readable and executable\n");
        printf("  or it doesn't exist and cannot be created\n");
	exit(1);
       }
   }
else
   {
   closedir(dir);
   }

sprintf(output_filename,"%s/text.txt",full_dir);

if ((fp = fopen(output_filename,"r")) != NULL)
  {
   printf("Error: in running 'create_cppsim_code_template':\n");
   printf("   CppSim file already exists!\n");
   printf("  -> you must delete the file 'text.txt' within directory:\n");
   printf("     %s\n", full_dir);
   printf("     if you want to create this CppSim code template\n");
   exit(1);
  }


if ((fp = fopen(output_filename,"w")) == NULL)
  {
   printf("Error: in running 'create_cppsim_code_template':\n   Can't open CppSim code file:\n   '%s'\n\n",output_filename);
   exit(1);
 }


fprintf(fp,"module: %s\n",cur_module->name);
fprintf(fp,"description: \n");

fprintf(fp,"parameters:  ");
cur_mod_parameter = cur_module->first_mod_parameter;
count = 0;
while (cur_mod_parameter != NULL)
   {
    if (count == 0)
        fprintf(fp,"double %s",cur_mod_parameter->name);
    else if (count % 2 == 0)
        fprintf(fp,"\n             double %s",cur_mod_parameter->name);
    else
        fprintf(fp,", double %s",cur_mod_parameter->name);
    count++;
    cur_mod_parameter = cur_mod_parameter->next;
   }
fprintf(fp,"\n");

fprintf(fp,"inputs:  ");
cur_mod_node = cur_module->first_ext_node;
count = 0;
while (cur_mod_node != NULL)
   {
    if (strcmp(cur_mod_node->node_dir,"input") == 0)
       {
        if (count == 0)
           fprintf(fp,"double %s",cur_mod_node->name);
        else if (count % 2 == 0)
           fprintf(fp,"\n             double %s",cur_mod_node->name);
        else
           fprintf(fp,", double %s",cur_mod_node->name);
        count++;
       }
    cur_mod_node = cur_mod_node->next;
   }
fprintf(fp,"\n");

fprintf(fp,"outputs:  ");
cur_mod_node = cur_module->first_ext_node;
count = 0;
while (cur_mod_node != NULL)
   {
    if (strcmp(cur_mod_node->node_dir,"output") == 0)
       {
        if (count == 0)
           fprintf(fp,"double %s",cur_mod_node->name);
        else if (count % 2 == 0)
           fprintf(fp,"\n             double %s",cur_mod_node->name);
        else
           fprintf(fp,", double %s",cur_mod_node->name);
        count++;
       }
    cur_mod_node = cur_mod_node->next;
   }
fprintf(fp,"\n");

fprintf(fp,"classes:  \n");
fprintf(fp,"static_variables:  \n");
fprintf(fp,"init:  \n");
fprintf(fp,"end:  \n");
fprintf(fp,"code:  \n\n\n");
fprintf(fp,"functions:  \n");
fprintf(fp,"custom_classes_definition:  \n");
fprintf(fp,"custom_classes_code:  \n");

fclose(fp);

printf("%s\n",output_filename);
}

