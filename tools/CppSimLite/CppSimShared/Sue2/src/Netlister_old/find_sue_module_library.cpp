/**************************************************************************
 * FILE : find_sue_module_library.cpp
 * 
 * AUTHOR : Mike Perrott
 * 
 * DESCRIPTION : finds library associated with a given Sue2 cell.
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
MODULE *first_module, *cur_module;
int i, count;
char sue_module_library[1000];
char sue_module_path[2000];
char temp[1000];

if (argc != 3)
  {
   printf("Error: need two arguments!\n");
   printf("  find_sue_module_library sue_module sue_lib_file\n");
   exit(1);
  }

/****
for (i = 0; argv[1][i] != '\0'; i++)
   sue_module_name[i] = tolower(argv[1][i]);
sue_module_name[i] = '\0';
****/

strcpy(sue_module_name,argv[1]);


param_list = init_parameter();

//// extract modules
first_module = extract_sue_modules(argv[2],param_list,"lite");

cur_module = first_module;
count = 0;
while (cur_module != NULL)
   {
     //printf("cur_module->name = %s\n",cur_module->name);
     if (strcmp(sue_module_name,cur_module->name) == 0)
       {
	count++;
        if (count == 1)
	  {
	   if (cur_module->shared_library_flag == 2)
	      sprintf(sue_module_library,"%s:Private",cur_module->library_path);
           else
	      sprintf(sue_module_library,"%s",cur_module->library_path);
           sprintf(sue_module_path,"%s",cur_module->full_filename);
	  }
        else if (count == 2)
	  {
	   strcpy(temp,sue_module_library);
	   if (cur_module->shared_library_flag == 2)
               sprintf(sue_module_library,"Error:  *** Multiple Libraries ***\n             %s,\n             %s:Private",
                   temp,cur_module->library_path);
           else
               sprintf(sue_module_library,"Error:  *** Multiple Libraries ***\n             %s,\n             %s",
                   temp,cur_module->library_path);
	  }
        else
	  {
	   strcpy(temp,sue_module_library);
	   if (cur_module->shared_library_flag == 2)
               sprintf(sue_module_library,"%s,\n             %s:Private",
                     temp,cur_module->library_path);
           else
               sprintf(sue_module_library,"%s,\n             %s",
                     temp,cur_module->library_path);
	  }
       }
     cur_module = cur_module->next;
   }
 if (count == 0)
   {
     sprintf(sue_module_library,"Error: - can't determine library for cell '%s'",sue_module_name);
   }

 printf("%s\nfile: %s",sue_module_library,sue_module_path);
}

