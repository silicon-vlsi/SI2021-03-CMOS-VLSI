/**************************************************************************
 * FILE : spice2cppnet.cpp
 * 
 * AUTHOR : Michael H. Perrott
 * 
 * DESCRIPTION : converts and Hspice netlist into a CppSim netlist
 *   file, allowing for system level simulation in the Cadence environment.
 ************************************************************************/
//#include <ctype.h>
//#include <stdio.h>
#include <dirent.h>
//#include <unistd.h>

#include "cppsim_classes.h"
#undef MAX_CHAR_LENGTH

#define LINESIZE 1000
//#define LENGTH_PAR_LIST 70  /* don't make this more than 100 */
#define LENGTH_PAR 200
#define MAX_CHAR_LENGTH 202
#define MAX_ARG 1000
#define MAX_NUM_ARGS 1000
#define MAX_NUM_LINES 1000
#define MAX_VARIABLES 100
#define MAX_NUM_VALUES 1000
#define MAX_MODULE_NAME_LENGTH 256
#define MAX_INSTANCE_NAME_LENGTH 256
//#define MAX_NODE_NAME_LENGTH 256
#define MAX_TRANS_NAME_LENGTH 30
#define MAX_NUM_PASSIVE_MODULES 20
#define MAX_NUM_GLOBAL_VARS 20
#define MAX_DIFF_NODE_SUFFIX 10
#define NaN 1.2345e307

typedef struct  
  {  
   int main_par_length, global_par_length, nest_length;
   char main_par_list[LENGTH_PAR_LIST][LENGTH_PAR];
   double main_par_val[LENGTH_PAR_LIST];
   double nest_val[LENGTH_PAR_LIST];
   int param_not_found;
} PARAMETER;

typedef struct
  {
  int num_of_args;
  char arg_array[MAX_NUM_ARGS][MAX_ARG];
} ARGUMENT;

typedef struct
  {
  int num_of_lines;
  char line_array[MAX_NUM_LINES][LINESIZE];
} LINE;

struct MOD_PARAMETER
  {
  char name[MAX_ARG];
  double value;
  char value_text[MAX_ARG];
  char type[MAX_ARG];
  int use_text_flag;
  struct MOD_PARAMETER *next;
};
typedef struct MOD_PARAMETER MOD_PARAMETER;

struct INSTANCE
  {
  char name[MAX_INSTANCE_NAME_LENGTH];
  char mod_name[MAX_INSTANCE_NAME_LENGTH];
  struct MODULE *mod;
  int node_arg_number;
  int num_of_node_args;
  MOD_PARAMETER *first_mod_parameter;
  double sim_order;
  int already_visited_flag;
  struct NODES *first_node;
  struct INSTANCE *next;
};
typedef struct INSTANCE INSTANCE;


struct NODES
  {
  char name[MAX_NODE_NAME_LENGTH];
  INSTANCE *out_instance,*first_instance;
  char node_type[MAX_ARG];
  char node_dir[MAX_ARG];
  int is_global_flag;
  double global_value;
  unsigned long int sub_arg_number; // used for sorting nodes alphabetically
  struct NODES *next;
};
typedef struct NODES NODES;

typedef struct
  {
  char node_type[MAX_ARG];
  char node_dir[MAX_ARG];
} NODE_INFO;

struct MODULE
  {
  char name[MAX_MODULE_NAME_LENGTH];
  NODES *first_ext_node;
  NODES *first_int_node;
  INSTANCE *first_instance,*first_unused_instance;
  MOD_PARAMETER *first_mod_parameter;
  int top_module_flag, top_module_not_subckt_flag;
  int sim_mod_order,*mod_dependency_list;
  int code_for_module_flag,missing_parameters_flag;
  int module_belongs_in_netlist_flag;
  char full_filename[MAX_ARG];
  char true_module_name[MAX_ARG];
  char library_path[MAX_ARG];
  int spice_text_flag;
  struct MODULE *next;
};
typedef struct MODULE MODULE;


void check_for_recursive_module_instantiations(MODULE *first_module);

void connect_instance_parameters_to_modules(MODULE *first_module);
MODULE *sort_modules(MODULE *first_module);
MODULE *determine_module_order(MODULE *first_module);
void determine_module_dependencies(MODULE *cur_module, MODULE *ref_module);
char *lowercase(char *arg);
void strip_args(ARGUMENT *args);
void add_instance_node(INSTANCE *cur_instance, char *name);
MODULE *grab_top_module(MODULE *first_module);
void add_param_value(const char *name, double value, PARAMETER *par_list);
void zero_node_info(NODE_INFO *node_info);
NODE_INFO *init_node_info();
MODULE *extract_modules(char *deck_filename, PARAMETER *param_list);
void connect_instances_to_modules(MODULE *first_module);
MODULE *grab_module(MODULE *first_module, char *name);
int extract_lineset(FILE *in, LINE *line_set, char *filename, int *linecount);
MODULE *add_top_module(MODULE *first_module);
void add_mod_parameter(MODULE *cur_mod, char *name, double value,
		       const char *value_text, int use_text_flag);
void add_instance_parameter(INSTANCE *cur_instance, char *name, double value,
			    char *value_text, int use_text_flag);
MOD_PARAMETER *init_mod_parameter(char *name, double value, const char *value_text,
				  int use_text_flag);
void add_instance_to_mod(MODULE *cur_mod, INSTANCE *inst);
INSTANCE *init_instance(const char *name, const char *mod_name, int node_arg_number,
		 MOD_PARAMETER *first_mod_parameter, NODES *first_node);
MODULE *init_module(const char *name);
NODES *init_node(const char *name);
void add_instance_to_node(NODES *cur_node, INSTANCE *inst);
MODULE *add_module(MODULE *first_module, char *filename, LINE *line_set,
                   int linecount, PARAMETER *param_list, ARGUMENT *args);
void add_ext_node(MODULE *cur_module, const char *name);
void add_int_node(MODULE *cur_module, char *name, INSTANCE *instance);
NODES *grab_node(MODULE *cur_module, char *name);

int extract_arguments_lc(LINE *line_set, char *filename, int linecount, ARGUMENT *args);
void extract_instance_parameters(ARGUMENT *args, char *filename, 
   LINE *line_set, int linecount, PARAMETER *param_list, INSTANCE *instance);
void send_line_set_to_output(LINE *line_set, FILE *out);
void send_line_set_to_std_output(LINE *line_set);
int extract_arguments(LINE *line_set, char *filename, int linecount, ARGUMENT *args);
int extract_par_arguments(LINE *line_set, int line_number, char *filename, 
			  ARGUMENT *args);
int extract_descriptor(ARGUMENT *args, int arg_num, char *desc_name, 
     char *filename, LINE *line_set,PARAMETER *param_list,double *desc_value);
int extract_text_descriptor(ARGUMENT *args, int arg_num, char *desc_name, 
			    char *desc_value);
double eval_paren_expr(char *input_in, const char *file_name,LINE *line_set, PARAMETER *par_list);
double eval_no_paren_expr(char *input, const char *file_name, LINE *line_set, PARAMETER *par_list);
double convert_string(char *input, const char *filename, LINE *line_set, 
                      int *error_flag);
void convert_spice_unit_expressions(char *param_expr);
double convert_exponent(char *input, const char *filename, LINE *line_set, const char *full_string);
LINE *init_line();
ARGUMENT *init_argument();
PARAMETER *init_parameter();       
void print_cppsim_netlist(FILE *fp, MODULE *first_module, 
			  int print_top_subckt_flag);
int get_first_bus_val(const char *node_name);
int get_last_bus_val(const char *node_name);
int get_bus_length(const char *node_name);
void determine_file_pathnames(MODULE *first_module, char *lib_defs_filename);

main(int argc,char *argv[])
{
char deck_filename[MAX_CHAR_LENGTH], lib_defs_filename[MAX_CHAR_LENGTH];
char temp[30];
PARAMETER *param_list;
MODULE *first_module, *top_module, *cur_module;
NODES *cur_node;
int print_top_subckt_flag = 0;
FILE *outfile;

if (argc != 4)
  {
   printf("error: need 3 arguments!\n");
   printf("  spice2cppnet hspice_netlist lib.defs outfile\n");
   exit(1);
  }
sprintf(deck_filename,"%s",argv[1]);
sprintf(lib_defs_filename,"%s",argv[2]);

if ((outfile = fopen(argv[3],"w")) == NULL)
  {
   printf("error in 'spice2cppnet':  can't open file:  '%s'\n",argv[3]);
   exit(1);
 }

param_list = init_parameter();
add_param_value("pi",PI,param_list);

//// extract netlist info
first_module = extract_modules(deck_filename, param_list);
determine_file_pathnames(first_module, lib_defs_filename);

// make sure that you don't have recursive module definitions
check_for_recursive_module_instantiations(first_module);

top_module = determine_module_order(first_module);
top_module->top_module_flag = 1;
first_module = sort_modules(first_module);

//// transfer instance parameters to their corresponding modules
connect_instance_parameters_to_modules(first_module);
print_cppsim_netlist(outfile, first_module, print_top_subckt_flag);
fclose(outfile);

//////////////////////////////////////////////////////////////////////
return(0);
}


void determine_file_pathnames(MODULE *first_module, char *lib_defs_filename)
{
MODULE *cur_module;
FILE *fp;
int i,linecount,end_of_file_flag;
LINE *line_set;
ARGUMENT *args;
char full_filename[MAX_ARG], module_dir_path[MAX_ARG];
DIR *module_dir_path_check;
const char *home_dir_const;
char home_dir[MAX_CHAR_LENGTH];


line_set = init_line();
args = init_argument();
linecount = 0;
end_of_file_flag = 0;



if ((fp = fopen(lib_defs_filename,"r")) == NULL)
  {
   printf("error in 'determine_file_pathnames':  can't open lib.defs file:  '%s'\n",
          lib_defs_filename);
   exit(1);
  }

home_dir_const = getenv("HOME");
if (home_dir_const != NULL)
   {
     sprintf(home_dir,"%s",home_dir_const);
     for (i = 0; home_dir[i] != '\0'; i++)
       {
	 if (home_dir[i] == '\\')
	   home_dir[i] = '/';
       }
   }
else
   {
     printf("error in 'determine_file_pathnames':  the shell variable HOME is undefined!\n");
     printf("   -> you must define the HOME environment variable\n");
     exit(1);
   }

// check for NULL values of true_module_names
cur_module = first_module;
while (cur_module != NULL)
   {
     if (strcmp(cur_module->true_module_name,"NULL") == 0)
       {
	 printf("error in 'determine_file_pathnames':  cur_module->true_module_name is NULL\n");
	 printf("  for module '%s', (lib '%s')\n",cur_module->name, cur_module->library_path);
	 exit(1);
       }
     cur_module = cur_module->next;
   }

while(end_of_file_flag == 0)
  {
   end_of_file_flag = extract_lineset(fp,line_set,lib_defs_filename,&linecount);
   //   send_line_set_to_std_output(line_set);
   //printf("-----------------\n");
   extract_arguments(line_set,lib_defs_filename,linecount,args);

   //   for (i = 0; i < args->num_of_args; i++)
   //     printf("arg[%d] = '%s'\n",i,args->arg_array[i]);
   //   printf("\n");
   if (args->num_of_args > 0)
     {
      if (strcmp(args->arg_array[0],"DEFINE") == 0)
        {
         if (args->num_of_args != 3)
	   {
	   printf("error in 'determine_file_pathnames':  lib.defs file has incorrect\n");
	   printf("   DEFINE statement which does not have three items in its line\n");
	   printf("   lib.defs line reads:\n");
           send_line_set_to_std_output(line_set);
	   exit(1);
	   }
         cur_module = first_module;
         while (cur_module != NULL)
	   {
	   if (strcmp(cur_module->library_path,args->arg_array[1]) == 0)
	     {
	       if (args->arg_array[2][0] == '~')
		 {
		  sprintf(module_dir_path,"%s%s/%s",home_dir,&(args->arg_array[2][1]),
                       cur_module->true_module_name);
		 }
               else
		 {
  	          sprintf(module_dir_path,"%s/%s",args->arg_array[2],cur_module->true_module_name);
		 }
	       if ((module_dir_path_check = opendir(module_dir_path)) == NULL)
		 {
  	          printf("error in 'determine_file_pathnames':  lib.defs file has incorrect\n");
  	          printf("   pathname for library '%s' (needed for cell '%s')\n",
			 cur_module->library_path, cur_module->true_module_name);
		  printf("   which is specified as '%s'\n",module_dir_path);
	          printf("   lib.defs line reads:\n");
                  send_line_set_to_std_output(line_set);
	          exit(1);
		 }
	       else
		 closedir(module_dir_path_check);

	       sprintf(full_filename,"%s/schematic/sch.oa",module_dir_path);

	       strcpy(cur_module->full_filename,full_filename);
	     }
	   cur_module = cur_module->next;
	   }
        }
     }
  }

free(line_set);
free(args);
fclose(fp);
}


void check_for_recursive_module_instantiations(MODULE *first_module)
{
MODULE *cur_module;
INSTANCE *cur_instance;

cur_module = first_module;
while (cur_module != NULL)
   {
    cur_instance = cur_module->first_instance; 
    while (cur_instance != NULL)
      {
	if (strcmp(cur_module->name,cur_instance->mod->name) == 0)
	  {
	   printf("error in 'check_for_recursive_module_instantiations':\n");
	   printf("  cannot have instances whose associated module is the\n");
	   printf("  is the same as module that the instance is contained in\n");
	   printf("  i.e., you cannot have recursively instantiated modules.\n\n");
	   printf("  in this case, module '%s' contains instance '%s'\n",
		  cur_module->name,cur_instance->name);
	   printf("  which is also an instantiation of module '%s'\n",
                  cur_module->name);
	   exit(1);
	  }
	cur_instance = cur_instance->next;
      }
    cur_module = cur_module->next;
   }
}



void connect_instance_parameters_to_modules(MODULE *first_module)
{
MODULE *cur_module;
INSTANCE *cur_instance;
MOD_PARAMETER *cur_inst_parameter,*cur_mod_parameter;

cur_module = first_module;
while(cur_module != NULL)
   {
    cur_instance = cur_module->first_instance;
    while (cur_instance != NULL)
      {
      cur_inst_parameter = cur_instance->first_mod_parameter;
      while (cur_inst_parameter != NULL)
	{
	cur_mod_parameter = cur_instance->mod->first_mod_parameter;
	while(cur_mod_parameter != NULL)
	  {
          if (strcmp(cur_mod_parameter->name,cur_inst_parameter->name) == 0)
	     break;
	  cur_mod_parameter = cur_mod_parameter->next;
	  }
        if (cur_mod_parameter == NULL)
	  {
           cur_instance->mod->missing_parameters_flag = 1;
           add_mod_parameter(cur_instance->mod,cur_inst_parameter->name, 
                             -1.0,"0.0",1);
	  }
	cur_inst_parameter = cur_inst_parameter->next;
	}
      cur_instance = cur_instance->next;
      }
    cur_module = cur_module->next;
   }
}




MODULE *sort_modules(MODULE *first_module)
{
MODULE *cur_module, *prev_module, *prev_prev_module;
MODULE *ret_first_module;
double prev_sim_mod_order;

if (first_module == NULL)
   {
     printf("error in 'sort_modules':  first_module is NULL!\n");
     exit(1);
   }
cur_module = first_module;
ret_first_module = first_module;

prev_sim_mod_order = cur_module->sim_mod_order;
while (cur_module != NULL)
   {
    if (cur_module->sim_mod_order < prev_sim_mod_order)
       {
        if (prev_module == ret_first_module)
	   {
            prev_module->next = cur_module->next;
	    ret_first_module = cur_module;
            cur_module->next = prev_module;
	   }
        else
	   {
            prev_module->next = cur_module->next;
            prev_prev_module->next = cur_module;
            cur_module->next = prev_module;
	   }
        cur_module = ret_first_module;
       }
    prev_sim_mod_order = cur_module->sim_mod_order;
    prev_prev_module = prev_module;
    prev_module = cur_module; 
    cur_module = cur_module->next;
   }

/////////////////////////////////////
//cur_module = ret_first_module;
// while (cur_module != NULL)
//  {
//     printf(">>>>> module '%s'\n",cur_module->name);
//     cur_module = cur_module->next;
//   }
/////////////////////////////////////
return(ret_first_module);
}



MODULE *grab_top_module(MODULE *first_module)
{
MODULE *cur_module,*loop_module,*top_module;
INSTANCE *cur_instance;
char top_name[MAX_ARG];

if (first_module == NULL)
   {
     printf("error in 'grab_top_module':  first_module = NULL!\n");
     exit(1);
   }
strcpy(top_name,"NULL");
cur_module = first_module;
while(cur_module != NULL)
  {
    loop_module = first_module;
    while (loop_module != NULL)
      {
        if (strcmp(loop_module->name,cur_module->name) == 0)
	  {
	  loop_module = loop_module->next;
	  continue;
	  }
        cur_instance = loop_module->first_instance;
        while (cur_instance != NULL)
	  {
	   if (strcmp(cur_instance->mod->name,cur_module->name) == 0)
              break;
	   cur_instance = cur_instance->next;
	  }
        if (cur_instance != NULL)
	   break;
	loop_module = loop_module->next;
      }
    if (loop_module == NULL)
      {
       if (strcmp(top_name,"NULL") != 0)
	  {
	   printf("error in 'grab_top_module':  there appear to be\n");
           printf("  at least two top modules!  (i.e. two modules that\n");
           printf("  do not appear in any other modules in the netlist)\n");
           printf("  Two modules that fit this category are\n");
           printf("  ---> '%s' and '%s'\n",top_name,cur_module->name);
           exit(1);
	  }
       else
	  {
	   strcpy(top_name,cur_module->name);
           top_module = cur_module;
	  }
      }
    cur_module = cur_module->next;
  }
if (strcmp(top_name,"NULL") == 0)
  {
    printf("error in 'grab_top_module':  there appears to be no\n");
    printf("  'top' module in the netlist (i.e., all the modules\n");
    printf("  appear in other modules in the netlist)\n");
    exit(1);
  }
return(top_module);
}

MODULE *determine_module_order(MODULE *first_module)
{
MODULE *cur_module, *top_module;
int count,i,j,max;

if (first_module == NULL)
   {
     printf("error in 'determine_module_order':  first_module is NULL!\n");
     exit(1);
   }
// count modules, and give each a number
cur_module = first_module;
count = 0;
while (cur_module != NULL)
  {
   cur_module->sim_mod_order = count++;
   cur_module = cur_module->next;
  }
// allocate memory for mod_dependency_list (an int array) in each module
cur_module = first_module;
while (cur_module != NULL)
  {
   if ((cur_module->mod_dependency_list = 
            (int *) calloc(count,sizeof(int))) == NULL)
      {   
       printf("error in 'determine_module_order':  calloc call failed\n");   
       printf("out of memory!\n");   
       exit(1);   
      }   
   for (i = 0; i < count; i++)
      cur_module->mod_dependency_list[i] = 0;  // zero out array
   cur_module = cur_module->next;
  }
// set entries in mod_dependency_list according to module dependency
cur_module = first_module;
while (cur_module != NULL)
  {
   determine_module_dependencies(cur_module,cur_module);
   //   printf("module '%s': ",cur_module->name);
   //   for (i = 0; i < count; i++)
   //      printf("%d ",cur_module->mod_dependency_list[i]);
   //   printf("\n");
   cur_module = cur_module->next;
  }
// redo sim_mod_order based on the results in the mod_dependency_list
cur_module = first_module;
top_module = first_module;
max = 0;
while (cur_module != NULL)
  {
   j = 0;
   for (i = 0; i < count; i++)
     if (cur_module->mod_dependency_list[i] == 1)
       j++;
   cur_module->sim_mod_order = j;
   if (j > max)
     {
       max = j;
       top_module = cur_module;
     }
   //   printf("module '%s': %d\n",cur_module->name,cur_module->sim_mod_order);
   cur_module = cur_module->next;
  }
// make sure that there is only one 'top' module
cur_module = first_module;
while (cur_module != NULL)
   {
     if (cur_module->sim_mod_order == max)
       {
	 if (cur_module != top_module)
	   {
	   printf("error in 'determine_module_order':  there appear to be\n");
           printf("  at least two top modules!  (i.e. two modules that\n");
           printf("  are at the top level of the module hierarchy)\n");
           printf("  Two modules that fit this category are\n");
           printf("  ---> '%s' and '%s'\n",top_module->name,cur_module->name);
           exit(1);
	   }
       }
    cur_module = cur_module->next;
   }
return(top_module);
}

void determine_module_dependencies(MODULE *cur_module, MODULE *ref_module)
{
INSTANCE *cur_instance;

if (cur_module == NULL)
   {
    printf("error in 'determine_module_dependencies':  cur_module = NULL!\n");
    exit(1);
   }
if (ref_module == NULL)
   {
    printf("error in 'determine_module_dependencies':  ref_module = NULL!\n");
    exit(1);
   }
cur_instance = cur_module->first_instance;
while (cur_instance != NULL)
   {
    if (cur_instance->mod->code_for_module_flag == 0) // no code, so not a primitive
       determine_module_dependencies(cur_instance->mod,ref_module);
    ref_module->mod_dependency_list[cur_instance->mod->sim_mod_order] = 1;
    cur_instance = cur_instance->next;
   }
}





MODULE *extract_modules(char *deck_filename, PARAMETER *param_list)
{
FILE *in;
int i,j,k,linecount,end_of_file_flag,notfound_global_flag=1;
LINE *line_set;
ARGUMENT *args;
MODULE *first_module, *cur_module;
INSTANCE *dummy_instance;
char module_name[MAX_ARG], module_name_lc[MAX_ARG], library_name[MAX_ARG];

/////////////////  Read Spice Deck ///////////////////////
//////////////////////////////////////////////////////////

if ((in = fopen(deck_filename,"r")) == NULL)
  {
   printf("error in 'extract_modules':  can't open spice file:  '%s'\n",deck_filename);
   exit(1);
 }


dummy_instance = init_instance(" "," ",0,NULL,NULL);
line_set = init_line();
args = init_argument();
linecount = 0;
end_of_file_flag = 0;
first_module = NULL;
cur_module = NULL;

strcpy(library_name,"NULL");
strcpy(module_name,"NULL");

while(end_of_file_flag == 0)
  {
   end_of_file_flag = extract_lineset(in,line_set,deck_filename,&linecount);
   //send_line_set_to_std_output(line_set);
   //printf("-----------------\n");
   extract_arguments_lc(line_set,deck_filename,linecount,args);

   if (strcmp(args->arg_array[0],"null") == 0)
     {
       printf("error in 'extract_modules':  there is an instantiation called '%s'\n",args->arg_array[0]);
       printf("   in the netlist file: '%s'\n",deck_filename);
       printf("   line reads:\n");
       send_line_set_to_std_output(line_set);
       exit(1);      
     }
   for (i = 1; i < args->num_of_args; i++)
     {
       if (strcmp(args->arg_array[i],"null") == 0)
	 {
         printf("error in 'extract_modules':  there is a node, parameter, or parameter value called '%s'\n",args->arg_array[i]);
         printf("   in the netlist file: '%s'\n",deck_filename);
         printf("   line reads:\n");
         send_line_set_to_std_output(line_set);
         exit(1);      
	 }
     }

   if (strncmp(args->arg_array[0],".subckt",7) == 0)
     {
       cur_module = add_module(first_module,deck_filename,line_set,
                 linecount,param_list,args);
       if (first_module == NULL)
          first_module = cur_module;

       strcpy(module_name_lc,module_name);
       lowercase(module_name_lc);
       if (strcmp(cur_module->name,module_name_lc) != 0 &&
           strcmp(module_name,"NULL") != 0)
	 {
	  if (module_name[0] >= '0' && module_name[0] <= '9')
	    {
	     printf("\nWarning in 'extract_modules': cell is inappropriately named with a number as its\n");
	     printf("   first letter, which leads to substitution with a '_' character\n");
	     printf("   -> original cell name '%s' changed to '%s'\n",module_name,cur_module->name);
             printf("   subckt line from netlist reads:\n");
             send_line_set_to_std_output(line_set);
	    }
          else
            {
	     printf("error in 'extract_modules': cellname mismatch for subckt line versus\n");
	     printf("  name of cell '%s' given by previous comment line in hspice netlist\n",
                     module_name);
             printf("   subckt line reads:\n");
             send_line_set_to_std_output(line_set);
             exit(1);
            }
	 }
       strcpy(cur_module->true_module_name,module_name);
       strcpy(cur_module->library_path,library_name);
       strcpy(library_name,"NULL");
       strcpy(module_name,"NULL");
     }
   //   printf("arg = _%s_\n",args->arg_array[0]);
   if (strncmp(args->arg_array[0],".ends",5) == 0)
       cur_module = NULL;


   switch(args->arg_array[0][0])
     {
     case '*':
       for (i = 1; i < args->num_of_args; i++)
	 {
	 if (strcmp(args->arg_array[i],"library") == 0 ||
             strcmp(args->arg_array[i],"cell") == 0)
	   {
	     if (args->num_of_args < i+3)
	       {
		 printf("error in 'extract_modules': not enough args for comment line\n");
		 printf("  specifying %s name within hspice netlist\n",
                     args->arg_array[i]);
                 printf("   line reads:\n");
                 send_line_set_to_std_output(line_set);
                 exit(1);      
	       }
	     if (strcmp(args->arg_array[i],"library") == 0)
	       {
                extract_arguments(line_set,deck_filename,linecount,args);
	        strcpy(library_name,args->arg_array[i+2]);
	       }
	     else
	       {
                extract_arguments(line_set,deck_filename,linecount,args);
	        strcpy(module_name,args->arg_array[i+2]);
	       }
	   }
	 break;
	 }
       break;
     case 'm':
     case 'r':
     case 'c':
     case 'l':
       break;
     case 'x':   
        if (cur_module == NULL)
	  {
	  cur_module = add_top_module(first_module);
          cur_module->top_module_not_subckt_flag = 1;
          if (first_module == NULL)
             first_module = cur_module;

          strcpy(cur_module->name,module_name);
          strcpy(cur_module->true_module_name,module_name);
          strcpy(cur_module->library_path,library_name);
          strcpy(library_name,"NULL");
          strcpy(module_name,"NULL");
	  }
	break;
     }

   switch(args->arg_array[0][0])
        {
        case 'x':
          extract_instance_parameters(args,deck_filename,line_set,linecount,
                param_list,dummy_instance);
          add_instance_to_mod(cur_module,dummy_instance);
          for (j = 0; j < dummy_instance->num_of_node_args; j++)
	    {
            dummy_instance->node_arg_number = j;
            add_int_node(cur_module,args->arg_array[j+1],dummy_instance);
	    }
          break;
        } 
  }

connect_instances_to_modules(first_module);


free(dummy_instance);
free(line_set);
free(args);
fclose(in);

return(first_module);
}

void strip_args(ARGUMENT *args)
{
int i,j,count;

count = args->num_of_args;

for (i = 0; i < count; i++)
  {
    if (args->arg_array[i][0] == ' ' || 
        args->arg_array[i][0] == ';' ||
        args->arg_array[i][0] == ',' ||
        args->arg_array[i][0] == '\n')
      {
	for (j = i+1; j < count; j++)
	  {
	   strcpy(args->arg_array[j-1],args->arg_array[j]);
	  }
        i--;
        count--;
      }
  }
args->num_of_args = count;
}




void connect_instances_to_modules(MODULE *first_module)
{
MODULE *cur_module;
INSTANCE *cur_instance;
NODES *cur_node;
int count;

if (first_module == NULL)
   {
     printf("error in 'connect_instances_to_module':  first_module is NULL\n");
     exit(1);
   }
cur_module = first_module;
while(cur_module != NULL)
   {
     // connect module instances
     cur_instance = cur_module->first_instance;
     while(cur_instance != NULL)
       {
	 cur_instance->mod = grab_module(first_module,cur_instance->mod_name);
         if (cur_instance->mod == NULL)
	   {
	     printf("error in 'connect_instances_to_modules':\n");
             printf("  can't find module '%s'\n",cur_instance->mod_name);
             printf("  --> look in module '%s'\n",cur_module->name); 
             exit(1);
	   }
         // check that instance and its associated module have the same
	 // number of nodes
         count = 0;
         cur_node = cur_instance->first_node;
         while(cur_node != NULL)
	   {
	    count++;
	    cur_node = cur_node->next;
	   }
         cur_node = cur_instance->mod->first_ext_node;
         while(cur_node != NULL)
	   {
	    count--;
	    cur_node = cur_node->next;
	   }
         if (count != 0)
	   {
	    printf("error in 'connect_instances_to_modules':\n");
            printf("  the number of nodes in instance '%s'\n",
		   cur_instance->name);
            printf("  does not match up with the number of nodes in\n");
            printf("  its associated module '%s'\n",cur_instance->mod->name);
            printf("  ---> look in module '%s'\n",cur_module->name);
            exit(1);  
	   }
         cur_instance = cur_instance->next;
       }
     
     /*
     // connect external node instances
     cur_node = cur_module->first_ext_node;
     while(cur_node != NULL)
       {
       cur_instance = cur_node->first_instance;
       while(cur_instance != NULL)
	  {
 	  cur_instance->mod = grab_module(first_module,cur_instance->mod_name);
          if (cur_instance->mod == NULL)
	       {
	        printf("error in 'connect_instances_to_modules':\n");
                printf("  can't find module '%s'\n",cur_instance->mod_name);
                printf("  ---> look in module '%s'\n",cur_module->name);
                exit(1);
	       }
          cur_instance = cur_instance->next;
	  }
       cur_node = cur_node->next;
       }
     // connect internal node instances
     cur_node = cur_module->first_int_node;
     while(cur_node != NULL)
       {
       cur_instance = cur_node->first_instance;
       while(cur_instance != NULL)
	  {
 	  cur_instance->mod = grab_module(first_module,cur_instance->mod_name);
          if (cur_instance->mod == NULL)
	       {
	        printf("error in 'connect_instances_to_modules':\n");
                printf("  can't find module '%s'\n",cur_instance->mod_name);
                printf("  ---> look in module '%s'\n",cur_module->name);
                exit(1);
	       }
          cur_instance = cur_instance->next;
	  }
       cur_node = cur_node->next;
       }
     */
     cur_module = cur_module->next;
   }
}


MODULE *grab_module(MODULE *first_module, char *name)
{
MODULE *cur_module;

if (first_module == NULL)
   {
     printf("error in 'grab_module':  first_module is NULL\n");
     exit(1);
   }
cur_module = first_module;
while(cur_module != NULL)
   {
     if (strncmp(cur_module->name,name,MAX_MODULE_NAME_LENGTH-1) == 0)
       break;
     cur_module = cur_module->next;
   }
if (cur_module == NULL)
   return(NULL);
else
   return(cur_module);
}


int extract_lineset(FILE *in, LINE *line_set, char *filename, int *linecount)
{
int i;
static int first_char;
static int first_line_flag = 1;
static char stored_filename[MAX_CHAR_LENGTH];

if (strcmp(filename,stored_filename) != 0)
   {
    strcpy(stored_filename,filename);
    first_line_flag = 1;
   }

if (first_line_flag == 1)
   {
     first_line_flag = 0;
     if ((first_char = fgetc(in)) == EOF)
        {
         printf("error in '%s':  input file '%s' is empty!\n",
	   "hspc",filename);
         exit(1);
        }
   }
line_set->num_of_lines = 0;
while(1)
     {
     (*linecount)++;
     if (first_char == (int) '\n')
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
       if (line_set->line_array[line_set->num_of_lines][i] == '&')
          if (line_set->line_array[line_set->num_of_lines][i+1] == '\n' ||
	      line_set->line_array[line_set->num_of_lines][i+1] == '\0' ||
	      line_set->line_array[line_set->num_of_lines][i+1] == ' ')
              break;

     if (line_set->line_array[line_set->num_of_lines][i] == '&')
       {
	 line_set->line_array[line_set->num_of_lines][i] = ' ';
	 line_set->num_of_lines++;
	 if (line_set->num_of_lines >= MAX_NUM_LINES)
	   {
	     printf(" error in '%s':  too many '& lines' encountered\n","hspc");
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


void print_cppsim_netlist(FILE *fp, MODULE *first_module, 
                       int print_top_subckt_flag)
{
MODULE *cur_module;
INSTANCE *cur_instance;
NODES *cur_mod_node,*cur_inst_node;
MOD_PARAMETER *cur_mod_parameter;
char temp[MAX_ARG],primitive_string[MAX_ARG];
int inst_array_length, inst_bus_length;
int i;

// determine if one should print out subckt defintion for top module
// to print:   print_top_subckt_flag = 1;

cur_module = first_module;
while (cur_module != NULL)
   {
     if (cur_module->top_module_flag == 1)
        break;
     cur_module = cur_module->next;
   }
if (cur_module == NULL)
   {
     printf("error in 'print_cppsim_netlist': no top module!\n");
     exit(1);
   }
fprintf(fp,"***** CppSim Netlist for Cell '%s' *****\n\n",cur_module->name);

cur_module = first_module;
while (cur_module != NULL)
  {
    // print header for module
    fprintf(fp,"************** Module %s **************\n",cur_module->name);

    fprintf(fp,"module: %s %s\n",cur_module->name,cur_module->library_path);
    fprintf(fp,"path: %s\n",cur_module->full_filename);
    if (cur_module->spice_text_flag != 0)
	fprintf(fp,"electrical_object: %s\n",cur_module->name);

    // print terminals
    cur_mod_node = cur_module->first_ext_node;
    while (cur_mod_node != NULL)
       {
       fprintf(fp,"   module_terminal: %s\n",cur_mod_node->name);
       cur_mod_node = cur_mod_node->next;
       }
    // print parameters
    cur_mod_parameter = cur_module->first_mod_parameter;
    while (cur_mod_parameter != NULL)
       {
       if (strcmp(cur_mod_parameter->value_text,"NULL") != 0)
           fprintf(fp,"   module_parameter: %s %s\n",cur_mod_parameter->name,
                   cur_mod_parameter->value_text);
       else
           fprintf(fp,"   module_parameter: %s\n",cur_mod_parameter->name);
                   cur_mod_parameter = cur_mod_parameter->next;
       }

    //////////////// netlist instances /////////////////////////
    cur_instance = cur_module->first_instance;
    while (cur_instance != NULL)
      {
       if (cur_instance->mod == NULL)
	  {
	   printf("error in 'print_cppsim_netlist':\n");
	   printf("   cur_instance->mod is NULL!\n");
	   printf("   cur_instance:  %s\n",cur_instance->name);
	  }

       // print instance name
       fprintf(fp,"instance: %s %s %s\n",cur_instance->name,
                   cur_instance->mod->name, cur_instance->mod->library_path);
       // print terminals
       cur_inst_node = cur_instance->first_node;
       cur_mod_node = cur_instance->mod->first_ext_node;
       while (cur_inst_node != NULL)
	 {
	  if (cur_mod_node == NULL)
	     {
	      printf("error in 'print_cppsim_netlist':  cur_mod_node is NULL!\n");
	      printf("   this should never happen.... ?!\n");
	      exit(1);
	     }
          inst_array_length = get_bus_length(cur_instance->name);
	  inst_bus_length = get_bus_length(cur_inst_node->name);
          if (inst_bus_length == 0 && inst_array_length > 0)
	     {
	     fprintf(fp,"   instance_terminal: %s <*%d>%s\n",
                    cur_mod_node->name, inst_array_length, cur_inst_node->name);
	     }
          else
             {
	     fprintf(fp,"   instance_terminal: %s %s\n",
                    cur_mod_node->name, cur_inst_node->name);
	     }
	  cur_inst_node = cur_inst_node->next;
	  cur_mod_node = cur_mod_node->next;
	 }
       // print parameters
       cur_mod_parameter = cur_instance->first_mod_parameter;
       while (cur_mod_parameter != NULL)
	 {
	   fprintf(fp,"   instance_parameter: %s %s\n",cur_mod_parameter->name,
                               cur_mod_parameter->value_text);
	   cur_mod_parameter = cur_mod_parameter->next;
	 }
       cur_instance = cur_instance->next;
      }

    /////////// netlist input, output, and inout pins (utilize first_unused_instance) //////////
    cur_instance = cur_module->first_unused_instance;
    while (cur_instance != NULL)
      {
       if (cur_instance->mod == NULL)
	  {
	   printf("error in 'print_cppsim_netlist':\n");
	   printf("   cur_instance->mod is NULL!\n");
	   printf("   cur_instance:  %s\n",cur_instance->name);
	  }
       // print out only pins and global
       if (strcmp(cur_instance->mod->name,"input") == 0 ||
           strcmp(cur_instance->mod->name,"inout") == 0 ||
           strcmp(cur_instance->mod->name,"output") == 0 ||
           strcmp(cur_instance->mod->name,"global") == 0)
	 {
	  // print instance name
	  fprintf(fp,"instance: %s %s %s\n",cur_instance->name,
                   cur_instance->mod->name, cur_instance->mod->library_path);
	  // print terminals
	  cur_inst_node = cur_instance->first_node;
          cur_mod_node = cur_instance->mod->first_ext_node;
	  while (cur_inst_node != NULL)
	    {
	     if (cur_mod_node == NULL)
		{
		  printf("error in 'print_cppsim_netlist':  cur_mod_node is NULL!\n");
		  printf("   this should never happen.... ?!\n");
		  exit(1);
		}
             inst_array_length = get_bus_length(cur_instance->name);
	     inst_bus_length = get_bus_length(cur_inst_node->name);
             if (inst_bus_length == 0 && inst_array_length > 0)
	        {
	        fprintf(fp,"   instance_terminal: %s <*%d>%s\n",
                    cur_mod_node->name, inst_array_length, cur_inst_node->name);
		}
             else
               {
	        fprintf(fp,"   instance_terminal: %s %s\n",
                    cur_mod_node->name, cur_inst_node->name);
	       }
	     cur_inst_node = cur_inst_node->next;
	     cur_mod_node = cur_mod_node->next;
	    }
          // print parameters
	  cur_mod_parameter = cur_instance->first_mod_parameter;
	  while (cur_mod_parameter != NULL)
	    {
	      fprintf(fp,"   instance_parameter: %s %s\n",cur_mod_parameter->name,
                               cur_mod_parameter->value_text);
	      cur_mod_parameter = cur_mod_parameter->next;
	    }
	 }
       else // turn primitive instance into a primitive
	 {
	   //	  extract_primitive_string(cur_instance,primitive_string);
	   //	  fprintf(fp,"%s\n",primitive_string);
	 } 
       cur_instance = cur_instance->next;
      }

    cur_module = cur_module->next;
  }
}

int get_first_bus_val(const char *node_name)
{
int i,k,flag;
char temp_name[MAX_NODE_NAME_LENGTH];

flag = 0;
k = 0;
for (i = 0; node_name[i] != '\0'; i++)
   {
     if (node_name[i] == '<' && node_name[i+1] == '*')
       {
	 for (  ; node_name[i] != '>' && node_name[i] != '\0'; i++);
	 if (node_name[i] != '>')
	   {
            printf("error in 'get_first_bus_val':  bus name is invalid\n");
            printf("   -> in this case, bus name = '%s'\n",node_name);
            exit(1);
	   }
       }
     else if (node_name[i] == '<')
	flag = 1;
     else if (node_name[i] == ':' || node_name[i] == '>')
        break;
     else if (flag == 1 && node_name[i] != ' ')
       temp_name[k++] = node_name[i];
   }
if (k == 0)
   {
     printf("error in 'get_first_bus_val':  bus name is invalid\n");
     printf("   -> in this case, bus name = '%s'\n",node_name);
     exit(1);
   }

temp_name[k] = '\0';

for (i = 0; temp_name[i] != '\0'; i++)
   {
     if (temp_name[i] < '0' || temp_name[i] > '9')
       {
       printf("error in 'get_first_bus_val':  bus name is invalid\n");
       printf("   -> in this case, bus name = '%s'\n",node_name);
       exit(1);
       }
   }
return(atoi(temp_name));
}

int get_last_bus_val(const char *node_name)
{
int i,k,flag;
char temp_name[MAX_NODE_NAME_LENGTH];

flag = 0;
k = 0;
for (i = 0; node_name[i] != '\0'; i++)
   {
     if (node_name[i] == '<' && node_name[i+1] == '*')
       {
	 for (  ; node_name[i] != '>' && node_name[i] != '\0'; i++);
	 if (node_name[i] != '>')
	   {
            printf("error in 'get_first_bus_val':  bus name is invalid\n");
            printf("   -> in this case, bus name = '%s'\n",node_name);
            exit(1);
	   }
       }
     else if (node_name[i] == '<')
	flag = 1;
     else if (node_name[i] == ':')
       {
	if (flag != 1)
	   {
            printf("error in 'get_last_bus_val':  bus name is invalid\n");
            printf("   -> in this case, bus name = '%s'\n",node_name);
            exit(1);
	   }
        k = 0;
       }
     else if (node_name[i] == '>')
        break;
     else if (flag == 1 && node_name[i] != ' ')
       temp_name[k++] = node_name[i];
   }
if (k == 0)
   {
     printf("error in 'get_last_bus_val':  bus name is invalid\n");
     printf("   -> in this case, bus name = '%s'\n",node_name);
     exit(1);
   }
temp_name[k] = '\0';

for (i = 0; temp_name[i] != '\0'; i++)
   {
     if (temp_name[i] < '0' || temp_name[i] > '9')
       {
       printf("error in 'get_last_bus_val':  bus name is invalid\n");
       printf("   -> in this case, bus name = '%s'\n",node_name);
       exit(1);
       }
   }
return(atoi(temp_name));
}

int get_bus_length(const char *node_name)
{
int i, first, last;

for (i = 0; node_name[i] != '\0'; i++)
  {
  if (node_name[i] == '<' && node_name[i+1] != '*')
     break;
  }

if (node_name[i] == '\0')
   return(0);

first = get_first_bus_val(node_name);
last = get_last_bus_val(node_name);

if (first == last)
  return(0);
else if (first > last)
   return(first-last+1);
else if (first < last)
   return(last-first+1);
}



void extract_instance_parameters(ARGUMENT *args, char *filename,
   LINE *line_set,int linecount, PARAMETER *param_list, INSTANCE *instance)
{
int i,j,k;
char desc_name[MAX_ARG];
double desc_value;
int desc_flag;

if (instance == NULL)
   {
   printf("error in 'extract_instance_parameters':  input instance is NULL\n");
   exit(1);
   }
strncpy(instance->name,args->arg_array[0],MAX_INSTANCE_NAME_LENGTH-1);

instance->num_of_node_args = args->num_of_args-2; 
instance->node_arg_number = 0;
instance->first_node = NULL; // note: the current node
 // chain should not be cleared out since other node and modules are using
 // it in their respective instances

for (i = 0; i < args->num_of_args; i++)
  {
  for (j = 0; args->arg_array[i][j] != '\0'; j++)
    if (args->arg_array[i][j] == '=')
      {
       instance->num_of_node_args = i-2;
       break;
      }
  if (args->arg_array[i][j] == '=')
    break;
  }
for (j = 1; j < i-1; j++)
  add_instance_node(instance,args->arg_array[j]);

instance->first_mod_parameter = NULL; // note: the current mod_parameter
 // chain should not be cleared out since other node and modules are using
 // it in their respective instances

param_list->param_not_found = 0; // don't abort on missing parameters

for (  ; i < args->num_of_args; i++)
  {
    desc_flag = extract_descriptor(args,i,desc_name,filename,line_set,
				   param_list,&desc_value);
    if (param_list->param_not_found == 1)
      {
       param_list->param_not_found = 0;
       if (desc_flag != 0)
         {
	   for (j = 0; args->arg_array[i][j] != '\0'; j++)
	     if (args->arg_array[i][j] == '=')
	       break;
           j++;
           for (   ; args->arg_array[i][j] == ' '; j++);
           for (k=j; args->arg_array[i][k] != '\0'; k++);
           for (   ; args->arg_array[i][k] == ' ' || 
                     args->arg_array[i][k] == '\0'; k--); 
	   args->arg_array[i][k+1] = '\0';
	   //	   printf("test case param '%s': '%s'\n",desc_name,
	   //		  &(args->arg_array[i][j]));
           add_instance_parameter(instance,desc_name,0.0,
                   &(args->arg_array[i][j]),1);
         }
      }
   else
      {
       if (desc_flag != 0)
         {
	   // old method: evaluate instance parameter if possible
	   //  add_instance_parameter(instance,desc_name,desc_value," ",0);

           // new method: keep instance parameters in text form
           //  -> use same routine as above (I'm only doing this to allow
           //         an easy way to go back to the old method if desired)
	   for (j = 0; args->arg_array[i][j] != '\0'; j++)
	     if (args->arg_array[i][j] == '=')
	       break;
           j++;
           for (   ; args->arg_array[i][j] == ' '; j++);
           for (k=j; args->arg_array[i][k] != '\0'; k++);
           for (   ; args->arg_array[i][k] == ' ' || 
                     args->arg_array[i][k] == '\0'; k--); 
	   args->arg_array[i][k+1] = '\0';
	   //	   printf("test case param '%s': '%s'\n",desc_name,
	   //		  &(args->arg_array[i][j]));
           add_instance_parameter(instance,desc_name,desc_value,
                   &(args->arg_array[i][j]),1);
         }
      }
  }

param_list->param_not_found = -1; // default back to abort on missing parameters

strncpy(instance->mod_name,args->arg_array[instance->num_of_node_args+1],
          MAX_INSTANCE_NAME_LENGTH-1);
instance->next = NULL;
}




PARAMETER *init_parameter()
{   
PARAMETER *A;   
   
if ((A = (PARAMETER *) malloc(sizeof(PARAMETER))) == NULL)   
   {   
   printf("error in 'init_parameter':  malloc call failed\n");   
   printf("out of memory!\n");   
   exit(1);   
  }   
A->main_par_length = 0;
A->nest_length = 0;
A->param_not_found = -1;  // error checked - see eval_no_paren_expr()
return(A);   
}   




MODULE *add_top_module(MODULE *first_module)
{
MODULE *cur_module;

if (first_module == NULL)
  {
  cur_module = init_module("top");
  }
else
  {
  cur_module = first_module;
  while(cur_module->next != NULL)
    cur_module = cur_module->next;
  cur_module->next = init_module("top");
  cur_module = cur_module->next;
  }
return(cur_module);
}

MODULE *add_module(MODULE *first_module, char *filename, LINE *line_set,
                   int linecount, PARAMETER *param_list, ARGUMENT *args)
{
int i,j,k;
int desc_flag;
char desc_name[MAX_ARG];
double desc_value;
MODULE *cur_module;

if (args->num_of_args < 2)
     {
      printf("error in 'add_module':  subckt doesn't have enough args\n");
      exit(1);
     }


if (first_module == NULL)
  {
  cur_module = init_module(args->arg_array[1]);
  }
else
  {
  cur_module = first_module;
  while(cur_module->next != NULL)
    cur_module = cur_module->next;
  cur_module->next = init_module(args->arg_array[1]);
  cur_module = cur_module->next;
  }

for (j = 2; j < args->num_of_args; j++)
   {
    for (k = 0; args->arg_array[j][k] != '\0'; k++)
       if (args->arg_array[j][k] == '=')
           break;
    if (args->arg_array[j][k] == '=')
       break;
    add_ext_node(cur_module,args->arg_array[j]);
   }


param_list->param_not_found = 0; // don't abort on missing parameters

for (i = j; i < args->num_of_args; i++)
  {
    desc_flag = extract_descriptor(args,i,desc_name,filename,line_set,
				   param_list,&desc_value);
    if (param_list->param_not_found == 1)
      {
       param_list->param_not_found = 0;
       if (desc_flag != 0)
         {
	   for (j = 0; args->arg_array[i][j] != '\0'; j++)
	     if (args->arg_array[i][j] == '=')
	       break;
           j++;
           for (   ; args->arg_array[i][j] == ' '; j++);
           for (k=j; args->arg_array[i][k] != '\0'; k++);
           for (   ; args->arg_array[i][k] == ' ' || 
                     args->arg_array[i][k] == '\0'; k--); 
	   args->arg_array[i][k+1] = '\0';
	   //	   printf("test case param '%s': '%s'\n",desc_name,
	   //		  &(args->arg_array[i][j]));
           add_mod_parameter(cur_module,desc_name,0.0,
                   &(args->arg_array[i][j]),1);
         }
      }
   else
      {
       if (desc_flag != 0)
         {
           // new method: keep module parameters in text form
	   for (j = 0; args->arg_array[i][j] != '\0'; j++)
	     if (args->arg_array[i][j] == '=')
	       break;
           j++;
           for (   ; args->arg_array[i][j] == ' '; j++);
           for (k=j; args->arg_array[i][k] != '\0'; k++);
           for (   ; args->arg_array[i][k] == ' ' || 
                     args->arg_array[i][k] == '\0'; k--); 
	   args->arg_array[i][k+1] = '\0';
	   //	   printf("test case param '%s': '%s'\n",desc_name,
	   //		  &(args->arg_array[i][j]));
           add_mod_parameter(cur_module,desc_name,desc_value,
                   &(args->arg_array[i][j]),1);
         }
      }
  }

param_list->param_not_found = -1; // default back to abort on missing parameters


return(cur_module);
}




MODULE *init_module(const char *name)
{   
MODULE *A;   
   
if ((A = (MODULE *) malloc(sizeof(MODULE))) == NULL)   
   {   
   printf("error in 'init_module':  malloc call failed\n");   
   printf("out of memory!\n");   
   exit(1);   
  }   
strncpy(A->name,name,MAX_MODULE_NAME_LENGTH-1);   
strcpy(A->full_filename,"NULL");
strcpy(A->library_path,"NULL");
strcpy(A->true_module_name,"NULL");
A->spice_text_flag = 0;

A->first_ext_node = NULL;
A->first_int_node = NULL;
A->first_instance = NULL;
A->first_unused_instance = NULL;
A->first_mod_parameter = NULL;
A->sim_mod_order = 0;
A->code_for_module_flag = 0;
A->mod_dependency_list = NULL;
A->next = NULL;
A->top_module_flag = 0;
A->top_module_not_subckt_flag = 0;
A->missing_parameters_flag = 0;
A->module_belongs_in_netlist_flag = 0;
return(A);   
}


NODES *init_node(const char *name)
{   
NODES *A;   
   
if ((A = (NODES *) malloc(sizeof(NODES))) == NULL)   
   {   
   printf("error in 'init_node':  malloc call failed\n");   
   printf("out of memory!\n");   
   exit(1);   
  }   
strncpy(A->name,name,MAX_NODE_NAME_LENGTH-1);
A->first_instance = NULL; // this is probably obsolete
strcpy(A->node_type,"NULL");
strcpy(A->node_dir,"NULL");
A->is_global_flag = 0;
A->global_value = 0.0;
A->out_instance = NULL;
A->next = NULL;
return(A);   
}



void add_instance_parameter(INSTANCE *cur_instance, char *name, double value,
                     char *value_text, int use_text_flag)
{
MOD_PARAMETER *cur_mod_parameter;

if (cur_instance->first_mod_parameter == NULL)
   cur_instance->first_mod_parameter = 
         init_mod_parameter(name,value,value_text,use_text_flag);
else
  {
  cur_mod_parameter = cur_instance->first_mod_parameter;
  while(cur_mod_parameter->next != NULL)
     cur_mod_parameter = cur_mod_parameter->next;
  cur_mod_parameter->next = 
         init_mod_parameter(name,value,value_text,use_text_flag);
  }
}


void add_mod_parameter(MODULE *cur_mod, char *name, double value,
                            const char *value_text, int use_text_flag)
{
MOD_PARAMETER *cur_mod_parameter;

if (cur_mod->first_mod_parameter == NULL)
   cur_mod->first_mod_parameter = init_mod_parameter(name,value,
                value_text,use_text_flag);
else
  {
  cur_mod_parameter = cur_mod->first_mod_parameter;
  while(cur_mod_parameter->next != NULL)
     cur_mod_parameter = cur_mod_parameter->next;
  cur_mod_parameter->next = init_mod_parameter(name,value,value_text,
                  use_text_flag);
  }
}


MOD_PARAMETER *init_mod_parameter(char *name, double value, const char *value_text,
                    int use_text_flag)
{   
MOD_PARAMETER *A;   
   
if ((A = (MOD_PARAMETER *) malloc(sizeof(MOD_PARAMETER))) == NULL)   
   {   
   printf("error in 'init_mod_parameter':  malloc call failed\n");   
   printf("out of memory!\n");   
   exit(1);   
  }   
strncpy(A->name,name,MAX_ARG-1);
A->use_text_flag = use_text_flag;
strncpy(A->value_text,value_text,MAX_ARG-1);
strcpy(A->type,"NULL");
A->value = value;
A->next = NULL;  
return(A);   
}   





INSTANCE *init_instance(const char *name, const char *mod_name, int node_arg_number,
           MOD_PARAMETER *first_mod_parameter, NODES *first_node)
{   
INSTANCE *A;   
   
if ((A = (INSTANCE *) malloc(sizeof(INSTANCE))) == NULL)   
   {   
   printf("error in 'init_instance':  malloc call failed\n");   
   printf("out of memory!\n");   
   exit(1);   
  }   
strncpy(A->name,name,MAX_INSTANCE_NAME_LENGTH-1);   
strncpy(A->mod_name,mod_name,MAX_MODULE_NAME_LENGTH-1);   
A->mod = NULL;
A->first_node = first_node;
A->node_arg_number = node_arg_number;
A->num_of_node_args = 0;
A->first_mod_parameter = first_mod_parameter;
A->sim_order = 0.0;
A->already_visited_flag = 0;

A->next = NULL;  
return(A);   
}   

void add_instance_to_node(NODES *cur_node, INSTANCE *inst)
{
INSTANCE *cur_instance;

if (cur_node->first_instance == NULL)
   cur_node->first_instance = 
        init_instance(inst->name,inst->mod_name,inst->node_arg_number,
                      inst->first_mod_parameter,inst->first_node);
else
  {
  cur_instance = cur_node->first_instance;
  while(cur_instance->next != NULL)
     cur_instance = cur_instance->next;
  cur_instance->next = 
        init_instance(inst->name,inst->mod_name,inst->node_arg_number,
                      inst->first_mod_parameter,inst->first_node);
  }
}

void add_instance_to_mod(MODULE *cur_mod, INSTANCE *inst)
{
INSTANCE *cur_instance;
int node_arg_number;

node_arg_number = 0;

if (cur_mod->first_instance == NULL)
   cur_mod->first_instance = 
        init_instance(inst->name,inst->mod_name,inst->node_arg_number,
             inst->first_mod_parameter,inst->first_node);
else
  {
  cur_instance = cur_mod->first_instance;
  while(cur_instance->next != NULL)
     cur_instance = cur_instance->next;
  cur_instance->next = 
        init_instance(inst->name,inst->mod_name,inst->node_arg_number,
             inst->first_mod_parameter,inst->first_node);
  }
}

void add_int_node(MODULE *cur_module, char *name, INSTANCE *instance)
{
NODES *cur_node;
register int i;
int int_node_flag;

 if (cur_module == NULL)
   {
     printf("error in 'add_int_node':  cur_module is NULL!\n");
     exit(1);
   }
 // printf("module = '%s', node = '%s'\n",cur_module->name,name);
int_node_flag = 1;

cur_node = cur_module->first_ext_node;
while(cur_node != NULL)
   {
   if (strncmp(name,cur_node->name,MAX_NODE_NAME_LENGTH-1) == 0)
      {
      int_node_flag = 0;
      break;
      }
   cur_node = cur_node->next;
   }

if (int_node_flag == 1) // not external, must be internal node
  {
   if (cur_module->first_int_node == NULL)
     {
      cur_module->first_int_node = init_node(name);
      cur_node = cur_module->first_int_node;
     }
   else
     {
      cur_node = cur_module->first_int_node;
      while(cur_node->next != NULL)
         {
         if (strncmp(name,cur_node->name,MAX_NODE_NAME_LENGTH-1) == 0)
            break;
         cur_node = cur_node->next;
         }
      if (strcmp(name,cur_node->name) != 0)
	{
         cur_node->next = init_node(name);
         cur_node = cur_node->next;
	}
     }
  }
 
if (instance != NULL) // node is in an instance declaration
   add_instance_to_node(cur_node,instance);

//cur_node = cur_module->first_int_node;
// while (cur_node != NULL)
//   {
//     printf(">>>> int node: '%s'\n",cur_node->name);
//     cur_node = cur_node->next;
//   }

}


NODE_INFO *init_node_info()
{   
NODE_INFO *A;   
   
if ((A = (NODE_INFO *) malloc(sizeof(NODE_INFO))) == NULL)   
   {   
   printf("error in 'init_node_info':  malloc call failed\n");   
   printf("out of memory!\n");   
   exit(1);   
  }   
strcpy(A->node_type,"NULL");
strcpy(A->node_dir,"NULL");

return(A);   
}

void zero_node_info(NODE_INFO *node_info)
{   
strcpy(node_info->node_type,"NULL");
}


void add_param_value(const char *name, double value, PARAMETER *par_list)
{
if (par_list->main_par_length >= (LENGTH_PAR_LIST-1))
   {
   printf("error in 'add_param_value':  maximum allowable number of\n");
   printf("  parameters exceeded\n");
   printf("   to fix, change LENGTH_PAR_LIST in source code\n");
   exit(1);
   }
sprintf(par_list->main_par_list[par_list->main_par_length],"%s",name);

par_list->main_par_val[par_list->main_par_length] = value;
par_list->main_par_length++;	   
}





void add_instance_node(INSTANCE *cur_instance, char *name)
{
NODES *cur_node;

if (cur_instance->first_node == NULL)
   cur_instance->first_node = init_node(name);
else
   {
   cur_node = cur_instance->first_node;
   while(cur_node->next != NULL)
      cur_node = cur_node->next;
   cur_node->next = init_node(name);
   }
}

// note:  must pass in valid pointer for name
void add_ext_node(MODULE *cur_module, const char *name)
{
NODES *cur_node;
register int i;

if (cur_module == NULL)
  {
    printf("error in 'add_ext_node':  cur_module is NULL!\n");
    exit(1);
  }
if (cur_module->first_ext_node == NULL)
   cur_module->first_ext_node = init_node(name);
else
   {
   cur_node = cur_module->first_ext_node;
   while(cur_node->next != NULL)
      cur_node = cur_node->next;
   cur_node->next = init_node(name);
   }
}   


NODES *grab_node(MODULE *cur_module, char *name)
{
NODES *cur_node;

cur_node = cur_module->first_ext_node;
while(cur_node != NULL) // check if it's an ext. node
   {
   if (strcmp(cur_node->name,name) == 0)
       break;
   cur_node = cur_node->next;
   }
if (cur_node == NULL) // now check if it's an int. node
   {
   cur_node = cur_module->first_int_node;
   while(cur_node != NULL)
     {
     if (strcmp(cur_node->name,name) == 0)
        break;
     cur_node = cur_node->next;
     }
   }
if (cur_node == NULL)
   {
     printf("error in 'extract_int_node_info':  can't find node '%s'!\n",
          name);
     exit(1);
   }
return(cur_node);
}

   

void send_line_set_to_output(LINE *line_set, FILE *out)
{
int i;

if (line_set != NULL)
   {
    for (i = 0; i < line_set->num_of_lines; i++)
       fprintf(out,"%s",line_set->line_array[i]);
   }
else
   fprintf(out," ---> line is unknown - sorry!\n");
}

void send_line_set_to_std_output(LINE *line_set)
{
int i;

if (line_set != NULL)
   {
    for (i = 0; i < line_set->num_of_lines; i++)
       printf("-> %s",line_set->line_array[i]);
   printf("\n");
   }
else
   printf(" ---> line is unknown - sorry!\n");
}

char *lowercase(char *arg)
{
int i;
for (i = 0; arg[i] != '\0'; i++)
   if (isupper(arg[i]))
      arg[i] = tolower(arg[i]);
return(arg);
}

int extract_arguments_lc(LINE *line_set, char *filename, int linecount, ARGUMENT *args)
{
char *inpline;
int i,j,line_set_count;

if (line_set->num_of_lines == 0)
  {
   strcpy(args->arg_array[0],"NULL");
   args->num_of_args = 0;
   return(args->num_of_args);
  }

// printf("line_set = _%s_\n",line_set->line_array[0]);
line_set_count = 0;
inpline = line_set->line_array[line_set_count];
/* in SunOS gcc, blank lines (i.e. lines only containing '\n')
   are appended to the beginning of nonempty lines */
for (i = 0 ; inpline[i] == ' ' || inpline[i] == '\n'; i++);
for (args->num_of_args = 0; args->num_of_args < MAX_NUM_ARGS; args->num_of_args++)
  {   
   for (    ; inpline[i] == ' '; i++);
   for (j = 0; inpline[i] != ' ' && inpline[i] != '\n' && 
               inpline[i] != '\0' && inpline[i] != 0x0d &&
               inpline[i] != '\t'; i++,j++)
     {
      if (j-1 >= MAX_ARG)
        {
         printf("error in 'extract_arguments_lc':  max argument size exceeded\n");
         printf("     in spice file '%s' on line %d\n",filename,linecount);
         printf("      to fix error, change MAX_ARG in source code\n");
         exit(1);
       }
      if (isupper(inpline[i]))
        args->arg_array[args->num_of_args][j] = tolower(inpline[i]);
      else
        args->arg_array[args->num_of_args][j] = inpline[i];
      //      printf("inpline[%d] = %x\n",i,inpline[i]);
      //      if (inpline[i] == 0x0d)
      //	printf("found it!\n");
    }
   args->arg_array[args->num_of_args][j] = '\0';
   if (inpline[i] == '\t' || inpline[i] == ' ')
      for (    ; inpline[i] == '\t' || inpline[i] == ' '; i++);
   if (inpline[i] == '\0' || inpline[i] == '\n' || inpline[i] == 0x0d)
     {
      if (++line_set_count < line_set->num_of_lines)
	{
         inpline = line_set->line_array[line_set_count];
         if (line_set->line_array[line_set_count][0] == '+')
            i = 1;
         else
            i = 0;
         continue;
	}
      else
         break;
     }
  }
args->num_of_args++;
if (args->num_of_args > MAX_NUM_ARGS)
  {
   printf("error in 'extract_arguments_lc':  max number of arguments exceeded\n");
   printf("error occurred in spice file '%s' on line %d\n",filename,linecount);
   printf("   MAX_NUM_ARGS specifed in source code as '%d'\n",MAX_NUM_ARGS);
   exit(1);
  }
//  for (i = 0; i < args->num_of_args; i++)
//    printf("extract_args: arg[%d] = _%s_\n",i,args->arg_array[i]);

return(args->num_of_args);
}

int extract_arguments(LINE *line_set, char *filename, int linecount, ARGUMENT *args)
{
char *inpline;
int i,j,line_set_count;

if (line_set->num_of_lines == 0)
  {
   strcpy(args->arg_array[0],"NULL");
   args->num_of_args = 0;
   return(args->num_of_args);
  }

line_set_count = 0;
inpline = line_set->line_array[line_set_count];
/* in SunOS gcc, blank lines (i.e. lines only containing '\n')
   are appended to the beginning of nonempty lines */
for (i = 0 ; inpline[i] == ' ' || inpline[i] == '\n'; i++);
for (args->num_of_args = 0; args->num_of_args < MAX_NUM_ARGS; args->num_of_args++)
  {   
   for (    ; inpline[i] == ' '; i++);
   for (j = 0; inpline[i] != ' ' && inpline[i] != '\n' && inpline[i] != '\0'; i++,j++)
     {
      if (j-1 >= MAX_ARG)
        {
         printf("error in 'extract_arguments':  max argument size exceeded\n");
         printf("     in spice file '%s' on line %d\n",filename,linecount);
         printf("      to fix error, change MAX_ARG in source code\n");
         exit(1);
       }
      args->arg_array[args->num_of_args][j] = inpline[i];
    }
   args->arg_array[args->num_of_args][j] = '\0';
   if (inpline[i] == ' ')
      for (    ; inpline[i] == ' '; i++);
   if (inpline[i] == '\0' || inpline[i] == '\n')
     {
      if (++line_set_count < line_set->num_of_lines)
	{
         inpline = line_set->line_array[line_set_count];
         if (line_set->line_array[line_set_count][0] == '+')
            i = 1;
         else
            i = 0;
         continue;
	}
      else
         break;
     }
  }
args->num_of_args++;
if (args->num_of_args > MAX_NUM_ARGS)
  {
   printf("error in 'extract_arguments':  max number of arguments exceeded\n");
   printf("error occurred in spice file '%s' on line %d\n",filename,linecount);
   printf("   MAX_NUM_ARGS specifed in source code as '%d'\n",MAX_NUM_ARGS);
   exit(1);
  }

return(args->num_of_args);
}

/* note that `extract_par_arguments' allows the use of '[' and
   ']' to group arguments */

int extract_par_arguments(LINE *line_set, int line_number, char *filename, 
			  ARGUMENT *args)
{
char *inpline;
int i,j,line_set_count;

inpline = line_set->line_array[line_number];
/* in SunOS gcc, blank lines (i.e. lines only containing '\n')
   are appended to the beginning of nonempty lines */
for (i = 0 ; inpline[i] == ' ' || inpline[i] == '\n'; i++);
for (args->num_of_args = 0; args->num_of_args < MAX_NUM_ARGS; args->num_of_args++)
  {   
   if (inpline[i] == '[')
     {
       for (i++; inpline[i] == ' '; i++);
       for (j = 0; inpline[i] != ']' && inpline[i] != '\n' && inpline[i] != '\0';
                   i++,j++)
	 {
	   if (j-1 >= MAX_ARG)
	     {
	       printf("error in 'extract_par_arguments':  max argument size exceeded\n");
	       printf("     in par file '%s' on %% line %d\n",filename,
		      line_number+1);
	       printf("      to fix error, change MAX_ARG in source code\n");
	       exit(1);
	     }
	   args->arg_array[args->num_of_args][j] = inpline[i];
	 }
       if (inpline[i] != ']')
	 {
         printf("error in 'extract_par_arguments':  a closing ']' was not found!\n");
	 printf("     in par file '%s' on %% line %d\n",filename,
		      line_number+1);
         exit(1);
         }
       i++;
     }
   else
     {
       for (j = 0; inpline[i] != ' ' && inpline[i] != '\n' && inpline[i] != '\0'
                                    && inpline[i] != '('  && inpline[i] != ')'; 
                                    i++,j++)
	 {
	   if (j-1 >= MAX_ARG)
	     {
	       printf("error in 'extract_par_arguments':  max argument size exceeded\n");
	       printf("     in par file '%s' on %% line %d\n",filename,
		      line_number+1);
	       printf("      to fix error, change MAX_ARG in source code\n");
	       exit(1);
	     }
	   args->arg_array[args->num_of_args][j] = inpline[i];
	 }
     }
   args->arg_array[args->num_of_args][j] = '\0';
   if (inpline[i] == ')' || inpline[i] == '(')
      {
       if (j > 0)
          args->num_of_args++;
       args->arg_array[args->num_of_args][0] = inpline[i];
       args->arg_array[args->num_of_args][1] = '\0';
       i++;
      }
   for (    ; inpline[i] == ' '; i++);
   if (inpline[i] == '\n' || inpline[i] == '\0')
      break;
  }
args->num_of_args++;
if (args->num_of_args > MAX_NUM_ARGS)
  {
   printf("error in 'extract_par_arguments':  max number of arguments exceeded\n");
   printf("error occurred in par file '%s' on %% line %d\n",filename,
	  line_number+1);
   printf("   MAX_NUM_ARGS specifed in source code as '%d'\n",MAX_NUM_ARGS);
   exit(1);
  }

return(args->num_of_args);
}

/* note:  par_name should be a string of the same size as
          arg->arg_array[i] array since no error checking on 
          writing operations to this variable is done  */
int extract_descriptor(ARGUMENT *args, int arg_num, char *desc_name, 
      char *filename,LINE *line_set,PARAMETER *param_list,double *desc_value)
{
int i,k;

if (arg_num >= args->num_of_args)
  {
    printf("error in 'extract_descriptor':  cannot extract descriptor\n");
    printf("from argument number %d because the total number of\n",arg_num);
    printf("  arguments is only %d\n",args->num_of_args);
    printf("  error occurred in file '%s'\n",filename);
    printf("  the line reads:\n");
    send_line_set_to_std_output(line_set);
    exit(1);
  }
for (i = 0; args->arg_array[arg_num][i] != '='  && 
            args->arg_array[arg_num][i] != '\0' && 
            args->arg_array[arg_num][i] != ' '      ; i++)
  desc_name[i] = args->arg_array[arg_num][i];
desc_name[i] = '\0';
for (   ; args->arg_array[arg_num][i] == ' '; i++);
if (args->arg_array[arg_num][i] != '=')
   return(0);
k = i;
for (   ; args->arg_array[arg_num][i] != '\0'; i++)
   if (args->arg_array[arg_num][i] == '\'')
      args->arg_array[arg_num][i] = ' ';

/* note:  contents of args->arg_array[arg_num] are 
          destroyed by eval_paren_expr */

// printf("arg = %s\n",&(args->arg_array[arg_num][k+1]));
*desc_value = eval_paren_expr(&(args->arg_array[arg_num][k+1]),filename,
			    line_set, param_list); 

return(1);
}


/* note:  par_name should be a string of the same size as
          arg->arg_array[i] array since no error checking on 
          writing operations to this variable is done  */
int extract_text_descriptor(ARGUMENT *args, int arg_num, char *desc_name, 
          char *desc_value)
{
int i,quote_flag;

quote_flag = 1;
if (arg_num >= args->num_of_args)
  {
    printf("error in 'extract_text_descriptor':  cannot extract descriptor\n");
    printf("from argument number %d because the total number of\n",arg_num);
    printf("  arguments is only %d\n",args->num_of_args);
    exit(1);
  }
for (i = 0; args->arg_array[arg_num][i] != '='  && 
            args->arg_array[arg_num][i] != '\0' && 
            args->arg_array[arg_num][i] != ' '      ; i++)
  desc_name[i] = args->arg_array[arg_num][i];
desc_name[i] = '\0';
for (   ; args->arg_array[arg_num][i] == ' '; i++);
if (args->arg_array[arg_num][i] != '=')
   return(0);
/* note:  contents of args->arg_array[arg_num] are 
          destroyed by eval_paren_expr */
i++;
for (   ; args->arg_array[arg_num][i] == ' '; i++);
if (args->arg_array[arg_num][i] == '\'')
  {
  quote_flag = 2;
  i++;
  }
strcpy(desc_value,&(args->arg_array[arg_num][i]));
for (i = 0; desc_value[i] != '\0'; i++)
   if (desc_value[i] == '\'' || desc_value[i] == ' ')
     {
      desc_value[i] = '\0';
      break;
     }
return(quote_flag);
}



LINE *init_line()
{   
LINE *A;   
   
if ((A = (LINE *) malloc(sizeof(LINE))) == NULL)   
   {   
   printf("error in 'init_line':  malloc call failed\n");   
   printf("out of memory!\n");   
   exit(1);   
  }   
A->num_of_lines = 0;
return(A);   
}   

ARGUMENT *init_argument()
{   
ARGUMENT *A;   
   
if ((A = (ARGUMENT *) malloc(sizeof(ARGUMENT))) == NULL)   
   {   
   printf("error in 'init_argument':  malloc call failed\n");   
   printf("out of memory!\n");   
   exit(1);   
  }   
A->num_of_args = 0;
return(A);   
}   


double eval_paren_expr(char *input_in, const char *file_name, LINE *line_set, 
                       PARAMETER *par_list)
{
int i,j,left_paren_count, right_paren_count;
int left_paren[LENGTH_PAR_LIST];
int curr_right_paren,curr_left_paren,nest_count;
char nest_val_string[3];
double return_value;
char input[MAX_ARG];

strncpy(input,input_in,MAX_ARG-1);
/* replace any negative signs following 'e' or 'E' by '!' */
/* this simplifies parsing when dealing with exponential notation */
for (i = 0; input[i] != '\0'; i++)
  {
   if (input[i] == 'e' || input[i] == 'E')
     {
       if (i > 0)
	 {
          if (input[i+1] == '-' && 
	      ((input[i-1] <= '9' && input[i-1] >= '0') || input[i-1] == '.'))
              input[i+1] = '!';
          else if (input[i+1] == '+' && 
	      ((input[i-1] <= '9' && input[i-1] >= '0') || input[i-1] == '.'))
              input[i+1] = '0';
	 }
     }
  }
left_paren_count = -1;
right_paren_count = -1;

for (i=0; input[i] != '\0'; i++)
  {
   switch(input[i])
     {
     case '(':
       left_paren_count++;
       if (left_paren_count >= LENGTH_PAR_LIST)
	 {
          printf("error in 'eval_paren_expr':  parenthesis nested too deep\n");
          printf("   in file '%s'\n",file_name);
          printf("   expression:  %s\n",input);
	  printf("   line reads:\n");
          send_line_set_to_std_output(line_set);
          exit(1);
	}
       left_paren[left_paren_count] = i;
       break;
     case ')':
       right_paren_count++;
     }
 }
if (right_paren_count != left_paren_count)
  {
    printf("error in 'eval_paren_expr':  parenthesis mismatch\n");
    printf("   in file '%s'\n",file_name);
    printf("   expression:  %s\n",input);
    printf("   line reads:\n");
    send_line_set_to_std_output(line_set);
    exit(1);
  }
par_list->nest_length = 0;

for (nest_count = 0; left_paren_count >= 0; left_paren_count--)
  {
    if (nest_count > 99)
      {
	printf("error in 'eval_paren_expr':  parenthesis nested too deep\n");
	printf("   in input file '%s'\n",file_name);
	printf("   expression:  %s\n",input);
	printf("  note:  LENGTH_PAR_LIST should not be set above 100!\n");
        printf("   line reads:\n");
        send_line_set_to_std_output(line_set);
	exit(1);
      }
    sprintf(nest_val_string,"%d",nest_count);
    curr_left_paren = left_paren[left_paren_count];
    for (i = curr_left_paren; input[i] != ')' && input[i] != '\0'; i++);
    if (input[i] == '\0')
      {
	printf("error in 'eval_paren_expr':  right and left parenthesis mismatch\n");
	printf("   in file '%s'\n",file_name);
	printf("   expression:  %s\n",input);
        printf("   line reads:\n");
        send_line_set_to_std_output(line_set);
	exit(1);
      }
    curr_right_paren = i;
    input[curr_right_paren] = '\0';
    
    par_list->nest_val[nest_count++] = eval_no_paren_expr(&input[curr_left_paren+1],
							  file_name,line_set,par_list);
    
    par_list->nest_length++;
    for (j = 0, i = curr_left_paren+1; j < 2; i++,j++)
      input[i] = nest_val_string[j];
    for (j = 0, i = curr_left_paren + 1; input[i] != '\0' && j < 2; i++,j++);
    for (   ; i <= curr_right_paren; i++)
      input[i] = ' ';
  }

return_value = eval_no_paren_expr(input,file_name,line_set,par_list);
return(return_value);
}


double eval_no_paren_expr(char *input, const char *file_name, LINE *line_set,
                          PARAMETER *par_list)
{
int i,j,arg_num,oper_num,arg_flag,found_in_list_flag;
char arg_list[LENGTH_PAR_LIST][LENGTH_PAR];
double arg_value[LENGTH_PAR_LIST];
char oper_list[LENGTH_PAR_LIST];
int error_flag,negative_found_at_begin;
double exp_value;

arg_num = 0;
oper_num = 0;
negative_found_at_begin = 0;
for (i = 0; input[i] == ' '; i++);

if (input[i] == '-')
  {
   negative_found_at_begin = 1;
   for (i++; input[i] == ' '; i++);
 }

/*  parse expression */
while(1)
  {
    if (arg_num > LENGTH_PAR_LIST)
      {
	printf("error in 'eval_no_paren_expr':  too many strings \n");
	printf("   in file `%s'\n",file_name);
        printf("   expression = %s\n",input);
        printf("   line reads:\n");
        send_line_set_to_std_output(line_set);
	exit(1);
      }
    arg_flag = 0;
    for (j = 0; input[i] != ' ' && input[i] != '\0' && input[i] != '*' &&
	 input[i] != '+' && input[i] != '\n' && input[i] != '/' &&
	 input[i] != '-'; j++,i++)
      {
        arg_flag = 1;
	if (j >= LENGTH_PAR-1)
	  {
	    printf("error in 'eval_no_paren_expr':  string too long\n");
	    printf("   in file `%s'\n",file_name);
	    printf("   expression = %s\n",input);
            printf("   line reads:\n");
            send_line_set_to_std_output(line_set);
	    exit(1);
	  }
	arg_list[arg_num][j] = input[i];
      }
    if (arg_flag == 1)
      {
       arg_list[arg_num][j] = '\0';
       arg_num++;
      }
    if (input[i] == ' ')
       for (   ; input[i] == ' '; i++);
    if (input[i] == '\n' || input[i] == '\0')
        break;
    else if (input[i] == '+' || input[i] == '-' || input[i] == '*' ||
             input[i] == '/')
      {
       if (oper_num > LENGTH_PAR_LIST)
	 {
	   printf("error in 'eval_no_paren_expr':  too many operators \n");
	   printf("   in file `%s'\n",file_name);
	   printf("   expression = %s\n",input);
           printf("   line reads:\n");
           send_line_set_to_std_output(line_set);
	   exit(1);
	 }
       oper_list[oper_num] = input[i];
       oper_num++;
       for (i++; input[i] == ' '; i++);
      }
    else
      {
	printf("error in 'eval_no_paren_expr':  two arguments in a row\n");
	printf("   in file `%s'\n",file_name);
	printf("   expression = %s\n",input);
        printf("   line reads:\n");
        send_line_set_to_std_output(line_set);
	exit(1);
      }
  }
if (arg_num != oper_num + 1)
  {
    printf("error in 'eval_no_paren_expr':  operator and argument mismatch \n");
    printf("   in file `%s'\n",file_name);
    printf("   expression = %s\n",input);
    printf("   note:  if expression is blank, you probably have empty\n");
    printf("          set of parenthesis somewhere\n");
    printf("   line reads:\n");
    send_line_set_to_std_output(line_set);
    exit(1);
  }


/*  evaluate expression */


/*  first, convert all arguments to numbers */

/* exp_value = convert_string("4582.0",file_name,linecount,&error_flag);
 printf("exp_value = %12.8e, error flag = %d \n",exp_value,error_flag); 
  printf("decimal = %d\n",atoi("9"));
  exit(1);
par_list->nest_length = 12;
par_list->main_par_length = 3;
par_list->main_par_val[0] = 14.2;
par_list->main_par_val[1] = 7.45;
par_list->main_par_val[2] = -193e-3;
par_list->nest_val[0] = 23.9;
par_list->nest_val[11] = -67.5;
sprintf(par_list->main_par_list[0],"Weff");
sprintf(par_list->main_par_list[1],"Leff");
sprintf(par_list->main_par_list[2],"error");  */

for (i = 0; i < arg_num; i++)
  {
    //    printf("arg[%d] = %s\n",i,arg_list[i]);
   convert_spice_unit_expressions(arg_list[i]);
   /* replace any negative signs following 'e' or 'E' by '!' */
   /* this simplifies parsing when dealing with exponential notation */
   for (j = 0; arg_list[i][j] != '\0'; j++)
      {
       if (arg_list[i][j] == 'e' || arg_list[i][j] == 'E')
          {
          if (j > 0)
	     {
              if (arg_list[i][j+1] == '-' && 
	         ((arg_list[i][j-1] <= '9' && arg_list[i][j-1] >= '0') || arg_list[i][j-1] == '.'))
                 arg_list[i][j+1] = '!';
              else if (arg_list[i][j+1] == '+' && 
	         ((arg_list[i][j-1] <= '9' && arg_list[i][j-1] >= '0') || arg_list[i][j-1] == '.'))
                 arg_list[i][j+1] = '0';
	     }
          }
      }

   exp_value = convert_string(arg_list[i],file_name,line_set,&error_flag);
   if (error_flag == 0)
      arg_value[i] = exp_value;
   else
     {
       /* check if parameter corresponds to a previously parsed expression
	  within parenthesis  */
      if (arg_list[i][0] == '(')
	{
         j = atoi(&arg_list[i][1]);
         if (j >= par_list->nest_length)
	   {
           printf("error in 'eval_no_paren_expr':  nested_arg_list exceeded\n");
           printf("   (this error shouldn't happen:  something wrong\n");
           printf("   with the net2cpp source code\n");
	   printf("   expression = %s\n",input);
           printf("   line reads:\n");
           send_line_set_to_std_output(line_set);
           exit(1);
	   }
         arg_value[i] = par_list->nest_val[j];
	}
      /* otherwise:
         parameter must correspond to variables defined within spice netlist */
      else
	{
          found_in_list_flag = 0;
	  for (j = 0; j < par_list->main_par_length; j++)
	    {
             if (strcmp(arg_list[i],par_list->main_par_list[j]) == 0)
	       {
                found_in_list_flag = 1;
                arg_value[i] = par_list->main_par_val[j];
                break;
	      }
	   }
          if (found_in_list_flag == 0)
	    {
            if (par_list->param_not_found == -1)
	       {
               printf("error in 'eval_no_paren_expr':  argument not defined\n");
               printf("   in file `%s'\n",file_name);
               printf("    arg = `%s'\n",arg_list[i]);
	       printf("   expression = %s\n",input);
               printf("   line reads:\n");
               send_line_set_to_std_output(line_set);
               exit(1);
	       }
             else
	       {
               par_list->param_not_found = 1;
               arg_value[i] = 0.0; // insert arbitrary value
	       }
	   }
	}
    }
 }

/*  now, perform operations and return value of expression */


/*   change sign of any arguments with - operating on them */

if (negative_found_at_begin == 1)
   arg_value[0] = -arg_value[0];
for (i = 0; i < oper_num; i++)
   if (oper_list[i] == '-')
      arg_value[i+1] = -arg_value[i+1];

/*   do multiplication and division from left to right */
for (i = 0; i < oper_num; i++)
  {
    switch(oper_list[i])
      {
      case '*':
	arg_value[i+1] *= arg_value[i];
        arg_value[i] = 0.0;
	break;
      case '/':
	arg_value[i+1] = arg_value[i] / arg_value[i+1];
        arg_value[i] = 0.0;
      }
  }

/*   add up the resulting arguments */
for (i = 0; i < oper_num; i++)
   arg_value[i+1] += arg_value[i];


return(arg_value[oper_num]);
}

void convert_spice_unit_expressions(char *param_expr)
{
int i,j,char_flag,operator_flag,num_flag,found_unit_expr_flag;
int prev_char_flag, prev_operator_flag, prev_num_flag;
char new_expr[MAX_ARG], temp_string[MAX_ARG];

num_flag = 0;
char_flag = 0;
operator_flag = 0;
prev_num_flag = 0;
prev_char_flag = 0;
prev_operator_flag = 0;
found_unit_expr_flag = 0;
for (i = 0,j = 0; param_expr[i] != '\0'; i++)
  {
    if ((param_expr[i] >= 'a' && param_expr[i] <= 'z') ||
        (param_expr[i] >= 'A' && param_expr[i] <= 'Z'))
      {
       char_flag = 1;
       num_flag = 0;
       operator_flag = 0;
      }
    if (param_expr[i] >= '0' && param_expr[i] <= '9')
      {
       char_flag = 0;
       num_flag = 1;
       operator_flag = 0;
       found_unit_expr_flag = 0;
      }
    if (param_expr[i] == '+' || param_expr[i] == '-' ||
        param_expr[i] == '*' || param_expr[i] == '/')
      {
       char_flag = 0;
       num_flag = 0;
       operator_flag = 1;
       found_unit_expr_flag = 0;
      }
    if (prev_num_flag == 1 && char_flag == 1 && param_expr[i] != 'e' && param_expr[i] != 'E')
      {
       found_unit_expr_flag = 1;
       new_expr[j] = '\0';
       switch(param_expr[i])
	 {
	  case 'A':
	  case 'a':
	    strcat(new_expr,"e-18");
	    break;
	  case 'F':
	  case 'f':
	    strcat(new_expr,"e-15");
	    break;
	  case 'P':
	  case 'p':
	    strcat(new_expr,"e-12");
	    break;
	  case 'N':
	  case 'n':
	    strcat(new_expr,"e-9");
	    break;
	  case 'U':
	  case 'u':
	    strcat(new_expr,"e-6");
	    break;
	  case 'M':
	  case 'm':
	    if (param_expr[i+1] == 'E' || param_expr[i+1] == 'e')
	      {
	       if (param_expr[i+2] == 'G' || param_expr[i+2] == 'g')
		 {
	          strcat(new_expr,"e6");
		 }
	       else
		  {
		   printf("error in 'convert_spice_unit_expressions':  units value '%s'\n",
                             &param_expr[i]);
		   printf("   not recognized\n");
		   printf("   overall parameter expression is '%s'\n",param_expr);
		   exit(1);
		  }
	       }
	    else
	      {
	       strcat(new_expr,"e-3");
	      }
	    break;
	  case 'K':
	  case 'k':
	    strcat(new_expr,"e3");
	    break;
	  case 'G':
	  case 'g':
	    strcat(new_expr,"e9");
	    break;
	  case 'T':
	  case 't':
	    strcat(new_expr,"e12");
	    break;
	  default:
	    printf("error in 'convert_spice_unit_expressions':  units value '%s'\n",&param_expr[i]);
	    printf("   not recognized\n");
	    printf("   overall parameter expression is '%s'\n",param_expr);
	    exit(1);

	 }
       for (j = 0; new_expr[j] != '\0'; j++);
      }
    else if (param_expr[i] != ' ' && found_unit_expr_flag == 0)
       new_expr[j++] = param_expr[i];

    prev_char_flag = char_flag;
    prev_num_flag = num_flag;
    prev_operator_flag = operator_flag;
  }
new_expr[j] = '\0';
/**
printf("old expr =  '%s'\n",param_expr);
printf("new expr =  '%s'\n",new_expr);
**/
strcpy(param_expr,new_expr);
}

double convert_string(char *input, const char *filename, LINE *line_set, 
                      int *error_flag)
{
int i,j,j_max,dec_place;
double string_value,char_value,power_of_ten,units_value;
int int_value;

// printf("input (in convert_string) = _%s_\n",input);

for (i = 0; input[i] != '\0'; i++)
  if (input[i] == '\n')
     input[i] = '\0';
units_value = 1.0;
*error_flag = 0;
for (i = 0; input[i] != '\0'; i++)
  {
    //   printf("raw input[i] = _%c_\n",input[i]);
   if ((input[i] > '9' || input[i] < '0') && input[i] != '.')
     {
       if (i > 0)
	 {
	   switch(input[i])
	     {
	     case 'E':
             case 'e':
               units_value = convert_exponent(&input[i+1],filename,line_set,input);
               break;
/**************  don't allow units conversion for now **********
	     case 'F':
	     case 'f':
	       units_value = 1.0e-15;
	       break;
	     case 'P':
	     case 'p':
	       units_value = 1.0e-12;
	       break;
	     case 'N':
	     case 'n':
	       units_value = 1.0e-9;
	       break;
	     case 'U':
	     case 'u':
	       units_value = 1.0e-6;
	       break;
	     case 'M':
	     case 'm':
	       if (input[i+1] == 'E' || input[i+1] == 'e')
		 {
		   if (input[i+2] == 'G' || input[i+2] == 'g')
		     units_value = 1.0e6;
		   else
		     {
		       printf("error in 'convert_string':  units value '%s'\n",
                             &input[i]);
		       printf("   not recognized in file '%s'\n",filename);
                       printf("   line reads:\n");
                       send_line_set_to_std_output(line_set);
		       exit(1);
		     }
		 }
	       else
		 units_value = 1.0e-3;
	       break;
	     case 'K':
	     case 'k':
	       units_value = 1.0e3;
	       break;
	     case 'G':
	     case 'g':
	       units_value = 1.0e9;
	       break;
	     case 'T':
	     case 't':
	       units_value = 1.0e12;
	       break;
***************************************************************/
	     default:
	           printf("error in 'convert_string':  units value '%s'\n",&input[i]);
	           printf("   not recognized in file '%s'\n",filename);
                   printf("       notation must be with 'e' or 'E'\n");
                   printf("       example:  1e9, 3.7e-9, 7.1E16, 2.3e-30\n");
	           //               printf("       OR common units such as f,p,n,u,m,k,meg,g,t\n");
                   printf("   line reads:\n");
                   send_line_set_to_std_output(line_set);
	           exit(1);
	     }
	   input[i] = '\0';
	 }
       else
	 {
	   *error_flag = 1;
	   return(0.0);
	 }
       break;
     }
 }
for (dec_place = 0; input[dec_place] != '.' && input[dec_place] != '\0';
         dec_place++);



i = dec_place;
string_value = 0.0;
power_of_ten = 1.0;
while(--i >= 0)
  {
   switch(input[i])
     {
      case '0':
        char_value = 0.0;
        break;
      case '1':
        char_value = 1.0;
        break;
      case '2':
        char_value = 2.0;
        break;
      case '3':
        char_value = 3.0;
        break;
      case '4':
        char_value = 4.0;
        break;
      case '5':
        char_value = 5.0;
        break;
      case '6':
        char_value = 6.0;
        break;
      case '7':
        char_value = 7.0;
        break;
      case '8':
        char_value = 8.0;
        break;
      case '9':
        char_value = 9.0;
      }
     string_value += char_value*power_of_ten;
     power_of_ten *= 10.0;
 }

if (input[dec_place] == '.')
  {
    power_of_ten = 1/10.0;
    for (i = dec_place + 1; input[i] != '\0'; i++)
      {
	switch(input[i])
	  {
	  case '0':
	    char_value = 0.0;
	    break;
	  case '1':
	    char_value = 1.0;
	    break;
	  case '2':
	    char_value = 2.0;
	    break;
	  case '3':
	    char_value = 3.0;
	    break;
	  case '4':
	    char_value = 4.0;
	    break;
	  case '5':
	    char_value = 5.0;
	    break;
	  case '6':
	    char_value = 6.0;
	    break;
	  case '7':
	    char_value = 7.0;
	    break;
	  case '8':
	    char_value = 8.0;
	    break;
	  case '9':
	    char_value = 9.0;
	  }
	string_value += char_value*power_of_ten;
	power_of_ten /= 10.0;
      }
  }
string_value *= units_value;

return(string_value);
}

/* note:  a negative sign in front of exponent is assumed to
          have been replaced with '!' in order to simplify parsing
          in other programs */
double convert_exponent(char *input, const char *filename, LINE *line_set, const char *full_string)
{
int i;
int string_value,char_value,power_of_ten,sign;
double output;

sign = 1;
for (i = 0; input[i] != '\0'; i++)
  {
    if (input[i] > '9' || input[i] < '0')
     {
       if (input[i] == '!' && i == 0)
	 sign = -1;
       else
	 {
	 printf("error in 'convert_exponent':  exponent term '%s'\n",input);
         printf("  within the string '%s'\n",full_string);
	 printf("   has unallowed characters in file '%s'\n",filename);
         printf("   line reads:\n");
         send_line_set_to_std_output(line_set);
	 exit(1);
	 }
     }
  }
if (i == 0)
  {
    printf("error in 'convert_exponent':  exponent term\n");
    printf("  within the string '%s'\n",full_string);
    printf("   has no value in file '%s'\n",filename);
    printf("   line reads:\n");
    send_line_set_to_std_output(line_set);
    exit(1);
  }


string_value = 0;
power_of_ten = 1;
while(--i >= 0 && input[i] != '!')
  {
   switch(input[i])
     {
      case '0':
        char_value = 0;
        break;
      case '1':
        char_value = 1;
        break;
      case '2':
        char_value = 2;
        break;
      case '3':
        char_value = 3;
        break;
      case '4':
        char_value = 4;
        break;
      case '5':
        char_value = 5;
        break;
      case '6':
        char_value = 6;
        break;
      case '7':
        char_value = 7;
        break;
      case '8':
        char_value = 8;
        break;
      case '9':
        char_value = 9;
      }
     string_value += char_value*power_of_ten;
     power_of_ten *= 10;
 }

output = 1.0;
if (sign > 0)
  {
   for (i = 0; i < string_value; i++)
      output *= 10.0;
  }
else
  {
   for (i = 0; i < string_value; i++)
      output *= 0.1;
  }

return(output);
}
