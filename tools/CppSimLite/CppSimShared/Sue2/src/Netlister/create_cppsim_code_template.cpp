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

int main(int argc,char *argv[])
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
char base_node_name[MAX_ARG];
DIR *dir;
int verilog_code_flag, min_bit, max_bit;

if (argc != 4)
  {
   printf("Error: need three arguments!\n");
   printf("  create_cppsim_code_template sue_module sue_module_library cppsim_or_verilog\n");
   exit(1);
  }

if (strcmp(argv[3],"Verilog") == 0)
   verilog_code_flag = 1;
else
   verilog_code_flag = 0;

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

///////////// CPPSIMSHAREDHOME environment variable /////////

cppsimshared_home_dir_const = getenv("CPPSIMSHAREDHOME");
if (cppsimshared_home_dir_const != NULL)
  {
  strcpy(cppsimshared_home_dir,cppsimshared_home_dir_const);
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

if (verilog_code_flag == 0)
   {
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
   fprintf(fp,"timing_sensitivity: \n");

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
       if (strcmp(cur_mod_node->node_dir,"input") == 0 || strcmp(cur_mod_node->node_dir,"inout") == 0)
          {
	    if (check_if_arrayed_signal_name(cur_mod_node->name,base_node_name) == 1)
	      {
	      max_bit = determine_max_bit(cur_mod_node->name);
	      min_bit = determine_min_bit(cur_mod_node->name);
	      if (max_bit == min_bit)
		{
                 printf("Error: in running 'create_cppsim_code_template':\n");
                 printf("   Arrayed signals with bus width of 1 are not supported\n");
                 printf("   (example:  node<0>, node<3>, are not OK)\n");
                 printf("  -> signal '%s' in module '%s' (lib '%s')\n",
                       cur_mod_node->name,sue_module_name,dirname);
                 printf("     must be changed to either '%s'\n",base_node_name);
                 printf("     or to an arrayed signal with bus > 1 (such as '%s<1:0>')\n",
                              base_node_name);
                 exit(1);
		}
	      if (min_bit != 0)
		{
                 printf("Error: in running 'create_cppsim_code_template':\n");
                 printf("   Arrayed signals without lowest index of 0 are not supported\n");
                 printf("   (example:  node<2:0> is OK, but node<3:1> is not)\n");
                 printf("  -> signal '%s' in module '%s' (lib '%s')\n",
                       cur_mod_node->name,sue_module_name,dirname);
                 printf("     must be changed to '%s<%d:0>'\n",base_node_name,max_bit-min_bit);
                 exit(1);
		}
              if (count == 0)
		 fprintf(fp,"bool %s[%d:%d]",base_node_name,max_bit,min_bit);
              else if (count % 2 == 0)
                 fprintf(fp,"\n             bool %s[%d:%d]",base_node_name,max_bit,min_bit);
              else
                 fprintf(fp,", bool %s[%d:%d]",base_node_name,max_bit,min_bit);
	      }
	    else
	      {
              if (count == 0)
                 fprintf(fp,"double %s",cur_mod_node->name);
              else if (count % 2 == 0)
                 fprintf(fp,"\n             double %s",cur_mod_node->name);
              else
                 fprintf(fp,", double %s",cur_mod_node->name);
	      }
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
	    if (check_if_arrayed_signal_name(cur_mod_node->name,base_node_name) == 1)
	      {
	      max_bit = determine_max_bit(cur_mod_node->name);
	      min_bit = determine_min_bit(cur_mod_node->name);
	      if (max_bit == min_bit)
		{
                 printf("Error: in running 'create_cppsim_code_template':\n");
                 printf("   Arrayed signals with bus width of 1 are not supported\n");
                 printf("   (example:  node<0>, node<3>, are not OK)\n");
                 printf("  -> signal '%s' in module '%s' (lib '%s')\n",
                       cur_mod_node->name,sue_module_name,dirname);
                 printf("     must be changed to either '%s'\n",base_node_name);
                 printf("     or to an arrayed signal with bus > 1 (such as '%s<1:0>')\n",
                              base_node_name);
                 exit(1);
		}
	      if (min_bit != 0)
		{
                 printf("Error: in running 'create_cppsim_code_template':\n");
                 printf("   Arrayed signals without lowest index of 0 are not supported\n");
                 printf("   (example:  node<2:0> is OK, but node<3:1> is not)\n");
                 printf("  -> signal '%s' in module '%s' (lib '%s')\n",
                       cur_mod_node->name,sue_module_name,dirname);
                 printf("     must be changed to '%s<%d:0>'\n",base_node_name,max_bit-min_bit);
                 exit(1);
		}
              if (count == 0)
		 fprintf(fp,"bool %s[%d:%d]",base_node_name,max_bit,min_bit);
              else if (count % 2 == 0)
                 fprintf(fp,"\n             bool %s[%d:%d]",base_node_name,max_bit,min_bit);
              else
                 fprintf(fp,", bool %s[%d:%d]",base_node_name,max_bit,min_bit);
	      }
	    else
	      {
              if (count == 0)
                 fprintf(fp,"double %s",cur_mod_node->name);
              else if (count % 2 == 0)
                 fprintf(fp,"\n             double %s",cur_mod_node->name);
              else
                 fprintf(fp,", double %s",cur_mod_node->name);
	      }
            count++;
          }
       cur_mod_node = cur_mod_node->next;
      }
   fprintf(fp,"\n");

   fprintf(fp,"classes:  \n");
   fprintf(fp,"static_variables:  \n");
   fprintf(fp,"init:  \n\n\n");
   fprintf(fp,"end:  \n");
   fprintf(fp,"code:  \n\n\n\n");
   fprintf(fp,"electrical_element:  \n");
   fprintf(fp,"functions:  \n");
   fprintf(fp,"custom_classes_definition:  \n");
   fprintf(fp,"custom_classes_code:  \n");

   fclose(fp);
   }
else
   {
   sprintf(full_dir,"%s/CadenceLib/%s/%s/verilog",base_dir,dirname,sue_module_name);
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

   sprintf(output_filename,"%s/verilog.v",full_dir);

   if ((fp = fopen(output_filename,"r")) != NULL)
     {
      printf("Error: in running 'create_cppsim_code_template':\n");
      printf("   Verilog file already exists!\n");
      printf("  -> you must delete the file 'verilog.v' within directory:\n");
      printf("     %s\n", full_dir);
      printf("     if you want to create this Verilog code template\n");
      exit(1);
     }


   if ((fp = fopen(output_filename,"w")) == NULL)
     {
      printf("Error: in running 'create_cppsim_code_template':\n   Can't open Verilog code file:\n   '%s'\n\n",output_filename);
      exit(1);
     }

   fprintf(fp,"// To include other Verilog files within a specified directory:\n");
   fprintf(fp,"// cpp_verilator_include_dir:\n\n");
   fprintf(fp,"// To specify Verilator command line options:\n");
   fprintf(fp,"// cpp_verilator_options:\n\n");
   fprintf(fp,"// To run a specified shell command before Verilator is run on this module:\n");
   fprintf(fp,"// cpp_verilator_pre_command:\n\n");
   fprintf(fp,"// -> Note that the above statements must be preceeded with '//' as shown\n\n");


   fprintf(fp,"module %s(",cur_module->name);

   // inputs
   cur_mod_node = cur_module->first_ext_node;
   count = 0;
   while (cur_mod_node != NULL)
      {
       if (strcmp(cur_mod_node->node_dir,"input") == 0)
          {
	    if (check_if_arrayed_signal_name(cur_mod_node->name,base_node_name) == 1)
	      {
	      max_bit = determine_max_bit(cur_mod_node->name);
	      min_bit = determine_min_bit(cur_mod_node->name);
	      if (max_bit == min_bit)
		{
                 printf("Error: in running 'create_cppsim_code_template':\n");
                 printf("   Arrayed signals with bus width of 1 are not supported\n");
                 printf("   (example:  node<0>, node<3>, are not OK)\n");
                 printf("  -> signal '%s' in module '%s' (lib '%s')\n",
                       cur_mod_node->name,sue_module_name,dirname);
                 printf("     must be changed to either '%s'\n",base_node_name);
                 printf("     or to an arrayed signal with bus > 1 (such as '%s<1:0>')\n",
                              base_node_name);
                 exit(1);
		}
	      if (min_bit != 0)
		{
                 printf("Error: in running 'create_cppsim_code_template':\n");
                 printf("   Arrayed signals without lowest index of 0 are not supported\n");
                 printf("   (example:  node<2:0> is OK, but node<3:1> is not)\n");
                 printf("  -> signal '%s' in module '%s' (lib '%s')\n",
                       cur_mod_node->name,sue_module_name,dirname);
                 printf("     must be changed to '%s<%d:0>'\n",base_node_name,max_bit-min_bit);
                 exit(1);
		}
              if (count == 0)
		fprintf(fp,"%s",base_node_name);
              else if (count % 5 == 0)
                 fprintf(fp,", \n             %s",base_node_name);
              else
                 fprintf(fp,", %s",base_node_name);
	      }
	    else
	      {
              if (count == 0)
                 fprintf(fp,"%s",cur_mod_node->name);
              else if (count % 5 == 0)
                 fprintf(fp,", \n             %s",cur_mod_node->name);
              else
                 fprintf(fp,", %s",cur_mod_node->name);
	      }
           count++;
          }
       cur_mod_node = cur_mod_node->next;
      }

   // outputs
   cur_mod_node = cur_module->first_ext_node;
   while (cur_mod_node != NULL)
      {
       if (strcmp(cur_mod_node->node_dir,"output") == 0)
          {
	    if (check_if_arrayed_signal_name(cur_mod_node->name,base_node_name) == 1)
	      {
	      max_bit = determine_max_bit(cur_mod_node->name);
	      min_bit = determine_min_bit(cur_mod_node->name);
	      if (max_bit == min_bit)
		{
                 printf("Error: in running 'create_cppsim_code_template':\n");
                 printf("   Arrayed signals with bus width of 1 are not supported\n");
                 printf("   (example:  node<0>, node<3>, are not OK)\n");
                 printf("  -> signal '%s' in module '%s' (lib '%s')\n",
                       cur_mod_node->name,sue_module_name,dirname);
                 printf("     must be changed to either '%s'\n",base_node_name);
                 printf("     or to an arrayed signal with bus > 1 (such as '%s<1:0>')\n",
                              base_node_name);
                 exit(1);
		}
	      if (min_bit != 0)
		{
                 printf("Error: in running 'create_cppsim_code_template':\n");
                 printf("   Arrayed signals without lowest index of 0 are not supported\n");
                 printf("   (example:  node<2:0> is OK, but node<3:1> is not)\n");
                 printf("  -> signal '%s' in module '%s' (lib '%s')\n",
                       cur_mod_node->name,sue_module_name,dirname);
                 printf("     must be changed to '%s<%d:0>'\n",base_node_name,max_bit-min_bit);
                 exit(1);
		}
              if (count == 0)
		fprintf(fp,"%s",base_node_name);
              else if (count % 5 == 0)
                 fprintf(fp,", \n             %s",base_node_name);
              else
                 fprintf(fp,", %s",base_node_name);
	      }
	    else
	      {
              if (count == 0)
                 fprintf(fp,"%s",cur_mod_node->name);
              else if (count % 5 == 0)
                 fprintf(fp,", \n             %s",cur_mod_node->name);
              else
                 fprintf(fp,", %s",cur_mod_node->name);
	      }
           count++;
          }
       cur_mod_node = cur_mod_node->next;
      }
   fprintf(fp,");\n\n");


   cur_mod_parameter = cur_module->first_mod_parameter;
   while (cur_mod_parameter != NULL)
      {
       fprintf(fp,"parameter %s = 0;\n",cur_mod_parameter->name);
       cur_mod_parameter = cur_mod_parameter->next;
      }
   fprintf(fp,"\n");

   // inputs
   cur_mod_node = cur_module->first_ext_node;
   while (cur_mod_node != NULL)
      {
       if (strcmp(cur_mod_node->node_dir,"input") == 0)
          {
	    if (check_if_arrayed_signal_name(cur_mod_node->name,base_node_name) == 1)
	      {
	      max_bit = determine_max_bit(cur_mod_node->name);
	      min_bit = determine_min_bit(cur_mod_node->name);
	      if (max_bit == min_bit)
		{
                 printf("Error: in running 'create_cppsim_code_template':\n");
                 printf("   Arrayed signals with bus width of 1 are not supported\n");
                 printf("   (example:  node<0>, node<3>, are not OK)\n");
                 printf("  -> signal '%s' in module '%s' (lib '%s')\n",
                       cur_mod_node->name,sue_module_name,dirname);
                 printf("     must be changed to either '%s'\n",base_node_name);
                 printf("     or to an arrayed signal with bus > 1 (such as '%s<1:0>')\n",
                              base_node_name);
                 exit(1);
		}
	      if (min_bit != 0)
		{
                 printf("Error: in running 'create_cppsim_code_template':\n");
                 printf("   Arrayed signals without lowest index of 0 are not supported\n");
                 printf("   (example:  node<2:0> is OK, but node<3:1> is not)\n");
                 printf("  -> signal '%s' in module '%s' (lib '%s')\n",
                       cur_mod_node->name,sue_module_name,dirname);
                 printf("     must be changed to '%s<%d:0>'\n",base_node_name,max_bit-min_bit);
                 exit(1);
		}
	      fprintf(fp,"input [%d:%d] %s;\n",max_bit,min_bit,base_node_name);
	      }
	    else
	      {
              fprintf(fp,"input %s;\n",cur_mod_node->name);
	      }
           count++;
          }
       cur_mod_node = cur_mod_node->next;
      }

   // outputs
   cur_mod_node = cur_module->first_ext_node;
   while (cur_mod_node != NULL)
      {
       if (strcmp(cur_mod_node->node_dir,"output") == 0)
          {
	    if (check_if_arrayed_signal_name(cur_mod_node->name,base_node_name) == 1)
	      {
	      max_bit = determine_max_bit(cur_mod_node->name);
	      min_bit = determine_min_bit(cur_mod_node->name);
	      if (max_bit == min_bit)
		{
                 printf("Error: in running 'create_cppsim_code_template':\n");
                 printf("   Arrayed signals with bus width of 1 are not supported\n");
                 printf("   (example:  node<0>, node<3>, are not OK)\n");
                 printf("  -> signal '%s' in module '%s' (lib '%s')\n",
                       cur_mod_node->name,sue_module_name,dirname);
                 printf("     must be changed to either '%s'\n",base_node_name);
                 printf("     or to an arrayed signal with bus > 1 (such as '%s<1:0>')\n",
                              base_node_name);
                 exit(1);
		}
	      if (min_bit != 0)
		{
                 printf("Error: in running 'create_cppsim_code_template':\n");
                 printf("   Arrayed signals without lowest index of 0 are not supported\n");
                 printf("   (example:  node<2:0> is OK, but node<3:1> is not)\n");
                 printf("  -> signal '%s' in module '%s' (lib '%s')\n",
                       cur_mod_node->name,sue_module_name,dirname);
                 printf("     must be changed to '%s<%d:0>'\n",base_node_name,max_bit-min_bit);
                 exit(1);
		}
	      fprintf(fp,"output [%d:%d] %s;\n",max_bit,min_bit,base_node_name);
	      }
	    else
	      {
              fprintf(fp,"output %s;\n",cur_mod_node->name);
	      }
           count++;
          }
       cur_mod_node = cur_mod_node->next;
      }
   fprintf(fp,"\n\nendmodule\n");

   fclose(fp);
   }
printf("%s\n",output_filename);
}

