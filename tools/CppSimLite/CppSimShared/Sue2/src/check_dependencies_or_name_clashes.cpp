#include "sue2_common_code.h"
int mark_dependent_modules(MODULE *first_module, char *module_name,
                           char *library_name, int check_library_flag);
int mark_name_clash_modules(MODULE *first_module, char *module_name,
			    char *library_name, int check_library_flag);


main(int argc,char *argv[])
{
PARAMETER *param_list;
MODULE *first_module, *cur_module;
char cur_module_name[MAX_CHAR_LENGTH], cur_library_name[MAX_CHAR_LENGTH];
int i,count,check_library_flag,overall_count;

if (argc != 6)
   {
     printf("error:  need five arguments:\n");
     printf("   check_dependencies_or_name_clashes sue_lib_file module_name all/single dependencies/name_clashes verbose/nonverbose\n");
     exit(1);
   }
param_list = init_parameter();
strcpy(cur_module_name,argv[2]);
for (i = 0; cur_module_name[i] != '\0'; i++)
  if (strncmp(&cur_module_name[i],".sue",4) == 0)
     break;
cur_module_name[i] = '\0';


if (strcmp(argv[4],"dependencies") == 0)
  {
  printf("\n... Extracting Sue2 modules ...\n");
  fflush(stdout);
  first_module = extract_sue_modules(argv[1],param_list,"normal");
  connect_sue_instances_to_modules(first_module);
  // print_keyinfo_modules(first_module); exit(1);

  if (strcmp(argv[3],"all") == 0)
     {
       cur_module = first_module;
       while (cur_module != NULL)
	 {
           if (strcmp(cur_module->name,cur_module_name) == 0)
	      break;
	   cur_module = cur_module->next;
	 }
       if (cur_module == NULL)
	 {
	   printf("error in 'check_dependencies_or_name_clashes':\n");
           printf("  can't find module '%s'\n",cur_module_name);
           printf("  -> this should never happen!?\n");
           exit(1);
	 }
       strcpy(cur_library_name,cur_module->library_path);
       printf("\n... Checking for module dependencies on library '%s' ...\n",
               cur_library_name);
       fflush(stdout);
       check_library_flag = 1;
       overall_count = 0;
       cur_module = first_module;
       while (cur_module != NULL)
	 {
           if (strcmp(cur_module->library_path,cur_library_name) == 0)
	     {
	      strcpy(cur_module_name,cur_module->name);
              count = mark_dependent_modules(first_module,cur_module_name,
                                  cur_library_name,check_library_flag);
	      overall_count += count;
	     }
	   cur_module = cur_module->next;
	 }
     if (overall_count == 0)
       {
	 printf("\n --- No other libraries depend on library '%s' ---\n",
		cur_library_name);
         exit(0);
       }
     else
       {
         if (overall_count == 1)
	    printf("\n --- %d other module depends on library '%s': ---\n\n",
                overall_count, cur_library_name);
         else
	    printf("\n --- %d other modules depend on library '%s': ---\n\n",
                overall_count, cur_library_name);
	 if (strcmp(argv[5],"verbose") == 0)
	   {
            cur_module = first_module;
            while (cur_module != NULL)
	      {
              if (cur_module->active_sue_module_flag == 1)
	         printf("   '%s' (library: '%s')\n",
		      cur_module->name,cur_module->library_path);
	      cur_module = cur_module->next;
	      }
            if (overall_count == 1)
	      printf("\n --- %d other module depends on library '%s': ---\n\n",
                 overall_count, cur_library_name);
            else
	      printf("\n --- %d other modules depend on library '%s': ---\n\n",
                 overall_count, cur_library_name);
	   }
         else
	   {
	     printf("     (For more details, push 'Dependencies' button of 'Library Operations:')\n");
	   }
	 exit(1);
       }
     }
  else
     {
     printf("\n... Checking for module dependencies on module '%s' ...\n",
             cur_module_name);
     fflush(stdout);
     check_library_flag = 0;
     count = mark_dependent_modules(first_module,cur_module_name,
                  cur_library_name,check_library_flag);
     if (count == 0)
       {
	 printf("\n --- No other modules depend on module '%s' ---\n",
		cur_module_name);
         exit(0);
       }
     else
       {
	 if (count == 1)
	    printf("\n --- %d other module depends on module '%s': ---\n\n",
                count, cur_module_name);
	 else
	    printf("\n --- %d other modules depend on module '%s': ---\n\n",
                count, cur_module_name);
	 if (strcmp(argv[5],"verbose") == 0)
	   {
            cur_module = first_module;
            while (cur_module != NULL)
	      {
              if (cur_module->active_sue_module_flag == 1)
	        printf("   '%s' (library: '%s')\n",
		      cur_module->name,cur_module->library_path);
	      cur_module = cur_module->next;
	      }
	    if (count == 1)
	      printf("\n --- %d other module depends on module '%s': ---\n\n",
                count, cur_module_name);
	    else
	      printf("\n --- %d other modules depend on module '%s': ---\n\n",
                count, cur_module_name);
	   }
         else
	   {
	     printf("     (For more details, push 'Dependencies' button of 'Module Operations:')\n");
	   }
         exit(1);
       }
     }
    }
 else // look for name clashes
    {
    printf("\n... Extracting Sue2 modules ...\n");
    fflush(stdout);
    first_module = extract_sue_modules(argv[1],param_list,"lite");

    if (strcmp(argv[3],"all") == 0)
      {
       cur_module = first_module;
       while (cur_module != NULL)
	 {
           if (strcmp(cur_module->name,cur_module_name) == 0)
	      break;
	   cur_module = cur_module->next;
	 }
       if (cur_module == NULL)
	 {
	   printf("error in 'check_dependencies_or_name_clashes':\n");
           printf("  can't find module '%s'\n",cur_module_name);
           printf("  -> this should never happen!?\n");
           exit(1);
	 }
       strcpy(cur_library_name,cur_module->library_path);
       printf("\n... Checking for module name clashes with library '%s' ...\n",
               cur_library_name);
       fflush(stdout);
       check_library_flag = 1;
       overall_count = 0;
       cur_module = first_module;
       while (cur_module != NULL)
	 {
           if (strcmp(cur_module->library_path,cur_library_name) == 0)
	     {
	      strcpy(cur_module_name,cur_module->name);
              count = mark_name_clash_modules(first_module,cur_module_name,
                                  cur_library_name,check_library_flag);
	      overall_count += count;
	     }
	   cur_module = cur_module->next;
	 }
       if (overall_count == 0)
         {
	 printf("\n --- No module name clashes with library '%s' ---\n",
                 cur_library_name);
         exit(0);
         }
       else
         {
	 if (overall_count == 1)
	    printf("\n --- %d module name clashes with library '%s': ---\n\n",
                overall_count, cur_library_name);
         else
	    printf("\n --- %d module names clash with library '%s': ---\n\n",
                overall_count, cur_library_name);
	 if (strcmp(argv[5],"verbose") == 0)
	   {
           cur_module = first_module;
           while (cur_module != NULL)
	     {
             if (cur_module->active_sue_module_flag == 1)
	        printf("   '%s' (library: '%s')\n",
		      cur_module->name,cur_module->library_path);
	     cur_module = cur_module->next;
	     }
	   if (overall_count == 1)
	     printf("\n --- %d module name clashes with library '%s': ---\n\n",
                overall_count, cur_library_name);
           else
	     printf("\n --- %d module names clash with library '%s': ---\n\n",
                overall_count, cur_library_name);
	   }
         else
	   {
	     printf("     (For more details, push 'Dependencies' button of 'Library Operations:')\n");
	   }

	 exit(1);
         }
      }
  else
     {
     printf("\n... Checking for module name clashes with module '%s' ...\n",
             cur_module_name);
     fflush(stdout);
     check_library_flag = 0;
     count = mark_name_clash_modules(first_module,cur_module_name,
                  cur_library_name,check_library_flag);
     if (count == 0)
       {
	 printf("\nerror in 'check_dependencies_or_name_clashes':\n");
	 printf("   can't find module '%s'\n",cur_module_name);
         printf("   -> this should never happen?!\n");
         exit(1);
       }
     else if (count == 1)
       {
	 printf("\n --- No other modules name clash with module '%s' ---\n",
		cur_module_name);
         exit(0);
       }
     else
       {
	 if (count == 2)
	    printf("\n --- %d other module name clashes with module '%s': ---\n\n",
                count-1, cur_module_name);
         else
	    printf("\n --- %d other modules name clash with module '%s': ---\n\n",
                count-1, cur_module_name);
	 if (strcmp(argv[5],"verbose") == 0)
	   {
           cur_module = first_module;
           while (cur_module != NULL)
	     {
             if (cur_module->active_sue_module_flag == 1)
	        printf("   '%s' (library: '%s')\n",
		      cur_module->name,cur_module->library_path);
	     cur_module = cur_module->next;
	     }
	   if (count == 2)
	     printf("\n --- %d other module name clashes with module '%s': ---\n\n",
                count-1, cur_module_name);
           else
	     printf("\n --- %d other modules name clash with module '%s': ---\n\n",
                count-1, cur_module_name);
	   }
         else
	   {
	     printf("     (For more details, push 'Dependencies' button of 'Module Operations:')\n");
	   }

         exit(1);
       }
     }
    }
}

int mark_dependent_modules(MODULE *first_module, char *module_name,
                           char *library_name, int check_library_flag)
{
int count;
MODULE *cur_module;
INSTANCE *cur_instance;

     count = 0;
     cur_module = first_module;
     while (cur_module != NULL)
        {
	 if (strcmp(cur_module->name,module_name) == 0)
	    {
	      cur_module = cur_module->next;
              continue;
	    }
	 cur_instance = cur_module->first_instance;
         while (cur_instance != NULL)
	   {
             if (strcmp(cur_instance->mod_name,module_name) == 0)
	        break;
	     cur_instance = cur_instance->next;
	   }
	 if (cur_instance != NULL)
	   {
	   if (check_library_flag == 1)
	      {
	      if (strcmp(cur_module->library_path,library_name) != 0)
		 {
		 if (cur_module->active_sue_module_flag == 0)
                      count++;
                 cur_module->active_sue_module_flag = 1;
		 }
	      }     
           else
	      {
	      if (cur_module->active_sue_module_flag == 0)
                 count++;
              cur_module->active_sue_module_flag = 1;
	      }
	   }
	 cur_module = cur_module->next;
	}
return(count);
}


int mark_name_clash_modules(MODULE *first_module, char *module_name,
                           char *library_name, int check_library_flag)
{
int count;
MODULE *cur_module;

     count = 0;
     cur_module = first_module;
     while (cur_module != NULL)
        {
	if (strcmp(cur_module->name,module_name) == 0)
	   {
	   if (check_library_flag == 1)
	      {
	      if (strcmp(cur_module->library_path,library_name) != 0)
		 {
		 if (cur_module->active_sue_module_flag == 0)
                      count++;
                 cur_module->active_sue_module_flag = 1;
		 }
	      }     
           else
	      {
	      if (cur_module->active_sue_module_flag == 0)
                 count++;
              cur_module->active_sue_module_flag = 1;
	      }
	   }
	 cur_module = cur_module->next;
	}
return(count);
}
