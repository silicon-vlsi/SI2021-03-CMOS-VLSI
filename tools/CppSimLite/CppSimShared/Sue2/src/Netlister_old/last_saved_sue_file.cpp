/**************************************************************************
 * FILE : sue_spice_netlister.cpp
 * 
 * AUTHOR : Mike Perrott
 * 
 * DESCRIPTION : creates Hspice netlist from SUE files.
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

//#include <ctype.h>
//#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include "cppsim_classes.h"
#undef MAX_CHAR_LENGTH

#define LINESIZE 1000
#define LENGTH_PAR_LIST 50  /* don't make this more than 100 */
#define LENGTH_PAR 200
#define MAX_CHAR_LENGTH 202
#define MAX_ARG 1000
#define MAX_TEXT 2000
#define MAX_NUM_ARGS 1000
#define MAX_NUM_LINES 1000
#define MAX_VARIABLES 100
#define MAX_NUM_VALUES 1000
#define MAX_MODULE_NAME_LENGTH 256
#define MAX_INSTANCE_NAME_LENGTH 256
#define MAX_NODE_NAME_LENGTH 256
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
  int origin_x, origin_y, orient_x, orient_y, orient_r;
  struct NODES *first_node;
  struct INSTANCE *next;
};
typedef struct INSTANCE INSTANCE;

struct NODES
  {
  char name[MAX_NODE_NAME_LENGTH];
  INSTANCE *first_instance;
  char node_type[MAX_ARG];
  char node_dir[MAX_ARG];
  int is_global_flag;
  int origin_x, origin_y;
  double global_value;
  struct NODES *next;
};
typedef struct NODES NODES;

typedef struct
  {
  char node_type[MAX_ARG];
  char node_dir[MAX_ARG];
} NODE_INFO;

struct CODE_ENTRY
  {
  char name[MAX_ARG];
  char name_type[MAX_ARG];
  int sub_arg_number;
  struct CODE_ENTRY *next;
};
typedef struct CODE_ENTRY CODE_ENTRY;

struct CODE
  {
  char name[MAX_MODULE_NAME_LENGTH];
  CODE_ENTRY *parameters, *inputs, *outputs, *declaration, *init, *body,
             *order,*static_vars;
  struct CODE *next;
};
typedef struct CODE CODE;

struct TIMING
  {
  double delay, rise_time, period, vlow, vhigh;
  char delay_text[MAX_ARG];
  char rise_time_text[MAX_ARG];
  char period_text[MAX_ARG];
  char vlow_text[MAX_ARG];
  char vhigh_text[MAX_ARG];
  struct TIMING *next;
};
typedef struct TIMING TIMING;

struct INPUT
  {
  char name[MAX_ARG];
  char filename[MAX_ARG];
  char type[MAX_ARG]; // i.e., prbs, square, sine, impulse
  struct TIMING *timing;
  struct INPUT *next;
};
typedef struct INPUT INPUT;

struct SKEW_PARAMETER
  {
  char name[MAX_ARG];
  int skew_group, arg_num;
  char filename[MAX_ARG];
  struct SKEW_PARAMETER *next;
};
typedef struct SKEW_PARAMETER SKEW_PARAMETER;

struct MAIN_PARAMETERS
  {
  double Ts;
  int num_sim_steps;
  char output_file[MAX_CHAR_LENGTH];
  MOD_PARAMETER *first_mod_parameter;
  MOD_PARAMETER *first_global_parameter, *first_global_node;
  TIMING *first_timing;
  INPUT *first_input;
  SKEW_PARAMETER *first_skew_parameter;
  NODES *first_probe_node;
};
typedef struct MAIN_PARAMETERS MAIN_PARAMETERS;

struct WIRE
  {
  int x1, y1, x2, y2;
  char node_name[MAX_CHAR_LENGTH];
  struct WIRE *next;
};
typedef struct WIRE WIRE;

void add_wire_to_module(MODULE *cur_module, int x1, int y1, int x2, int y2);
WIRE *init_wire(int x1, int y1, int x2, int y2);

struct MODULE
  {
  char name[MAX_MODULE_NAME_LENGTH];
  char library_path[MAX_ARG];
  CODE *code;
  WIRE *first_wire;
  NODES *first_ext_node;
  NODES *first_int_node;
  INSTANCE *first_instance,*first_unused_instance;
  MOD_PARAMETER *first_mod_parameter;
  int top_module_flag, top_module_not_subckt_flag;
  int active_sue_module_flag;
  int sim_mod_order,*mod_dependency_list;
  int code_for_module_flag,missing_parameters_flag;
  int icon_exists_flag, schematic_exists_flag;
  char *spice_text;
  struct MODULE *next;
};
typedef struct MODULE MODULE;

void retrieve_instance_node_name(INSTANCE *cur_instance, char *mod_node_name,
                                 char *output);
void retrieve_instance_parameter_value(INSTANCE *cur_instance,
					char *instance_parameter_name,
				       char *output);
void extract_primitive_string(INSTANCE *cur_instance, char *out_string);
void print_hspice_code(FILE *fp, MODULE *first_module,
                       int print_top_subckt_flag);
void connect_instance_parameters_to_modules(MODULE *first_module);
INPUT *init_input(char *name, TIMING *timing);
TIMING *init_timing();
void determine_instance_order_from_code(MODULE *cur_module);
void determine_instance_order(MODULE *cur_module);
int determine_instance_dependencies(MODULE *cur_module, 
                                    INSTANCE *cur_instance, int weight);
MODULE *sort_modules(MODULE *first_module);
MODULE *determine_module_order(MODULE *first_module);
void determine_module_dependencies(MODULE *cur_module, MODULE *ref_module);
void sort_instances(MODULE *cur_module);
char *retrieve_instance_parameter_val(INSTANCE *cur_instance,char *param_name);
void check_instance_parameters(MODULE *first_module, PARAMETER *param_list);
void check_dir_for_ext_mod_nodes(MODULE *first_module);
void extract_int_mod_nodes(MODULE *first_module,MAIN_PARAMETERS *main_parameters);
void pass_mod_info_to_instances(MODULE *first_module);
void extract_ext_mod_nodes(MODULE *cur_module);
char *lowercase(char *arg);
void print_one_code(CODE *cur_code);
void strip_args(ARGUMENT *args);
void add_instance_node(INSTANCE *cur_instance, char *name);
void print_keyinfo_modules(MODULE *mod);
MODULE *grab_top_module(MODULE *first_module);
void print_node_info(NODE_INFO *node_info);
void add_param_value(char *name, double value, PARAMETER *par_list);
void zero_node_info(NODE_INFO *node_info);
NODE_INFO *init_node_info();
void connect_instances_to_modules(MODULE *first_module);
MODULE *grab_module(MODULE *first_module, char *name);
void print_mod_parameter(MOD_PARAMETER *cur_mod_parameter);
int extract_lineset(FILE *in, LINE *line_set, char *filename, int *linecount);
int extract_code_lineset(FILE *in, LINE *line_set, char *filename, 
                         int *linecount);
MOD_PARAMETER *add_mod_parameter(MODULE *cur_mod, char *name, double value,
		       char *value_text, int use_text_flag);
void add_instance_parameter(INSTANCE *cur_instance, char *name, double value,
			    char *value_text, int use_text_flag);
MOD_PARAMETER *init_mod_parameter(char *name, double value, char *value_text,
				  int use_text_flag);
void add_instance_to_mod(MODULE *cur_mod, INSTANCE *inst);
void print_verbose_modules(MODULE *mod);
void print_modules(MODULE *mod);
INSTANCE *init_instance(char *name, char *mod_name, int node_arg_number,
		 MOD_PARAMETER *first_mod_parameter, NODES *first_node);
INSTANCE *init_sue_instance(char *name, char *mod_name, int node_arg_number,
		 MOD_PARAMETER *first_mod_parameter, NODES *first_node,
                 int origin_x, int origin_y);
MODULE *init_sue_module(char *name, char *library_path);
NODES *init_node(char *name);
void add_instance_to_node(NODES *cur_node, INSTANCE *inst);
MODULE *add_sue_module(MODULE *first_module, char *deck_filename, 
                       char *library_path, PARAMETER *param_list);
NODES *add_ext_node_sue(MODULE *cur_module, char *name);
void add_int_node(MODULE *cur_module, char *name, INSTANCE *instance);
NODES *grab_node(MODULE *cur_module, char *name);

int extract_arguments_lc(LINE *line_set, char *filename, int linecount, ARGUMENT *args);
void extract_sue_instance_parameters(ARGUMENT *args,char *filename,
     LINE *line_set,int linecount, PARAMETER *param_list, INSTANCE *instance);
void extract_instance_parameters(ARGUMENT *args,char *filename,
     LINE *line_set,int linecount, PARAMETER *param_list, INSTANCE *instance);
void send_line_set_to_output(LINE *line_set, FILE *out);
void send_line_set_to_std_output(LINE *line_set);
int extract_arguments(LINE *line_set, char *filename, int linecount, ARGUMENT *args);
int extract_par_arguments(LINE *line_set, int line_number, char *filename, 
			  ARGUMENT *args);
int extract_descriptor(ARGUMENT *args, int arg_num, char *desc_name, 
     char *filename, LINE *line_set,PARAMETER *param_list,double *desc_value);
int extract_text_descriptor(ARGUMENT *args, int arg_num, char *desc_name, 
			    char *desc_value);
double eval_paren_expr(char *input_in, char *file_name,LINE *line_set, PARAMETER *par_list);
double eval_no_paren_expr(char *input, char *file_name, LINE *line_set, PARAMETER *par_list);
double convert_string(char *input, char *filename, LINE *line_set, int *error_flag);
double convert_exponent(char *input, char *filename, LINE *line_set, char *full_string);
LINE *init_line();
ARGUMENT *init_argument();
PARAMETER *init_parameter();       
void connect_sue_instances_to_modules(MODULE *first_module);
void connect_wires_to_terminals_and_labels(MODULE *first_module);
void connect_wires_to_themselves(MODULE *first_module);
int count_unnamed_wires(MODULE *cur_module);
void print_wires(MODULE *cur_module);
int label_unnamed_wires(MODULE *cur_module, int count_in);
void determine_connecting_wire_node_name(MODULE *cur_module, int x, int y, 
                                         char *node_name);
void add_sue_instance_node(INSTANCE *cur_instance, NODES *cur_mod_node);
void determine_connecting_terminal_or_global_node_name(MODULE *cur_module, 
						      int x, int y, char *node_name);
int check_if_node_name_is_available(MODULE *cur_module, char *name);
void label_connecting_terminals_or_globals(MODULE *cur_module, int x, int y, 
					   char *name);
void connect_instance_nodes(MODULE *first_module);
int is_node_naming_object(char *name);
void trim_node_naming_instances_from_modules(MODULE *first_module);
void fill_in_missing_module_external_nodenames(MODULE *first_module);
void determine_sue_module_dependencies(MODULE *cur_module);
MODULE *trim_unused_modules(MODULE *first_module, char *top_module_name);
MODULE *trim_node_naming_modules(MODULE *first_module);
MODULE *extract_sue_modules(char *sue_lib_filename, PARAMETER *param_list);
int is_sue_file(char *name);
void prefix_x_to_instance_names(MODULE *first_module);
void recalculate_instance_terminal_origins_due_to_orient(INSTANCE *cur_instance);
void account_for_orient(MODULE *first_module);
void warn_about_duplicate_modules(MODULE *first_module);


main(int argc,char *argv[])
{
char top_sue_module_name[MAX_CHAR_LENGTH];
char hspice_filename[MAX_CHAR_LENGTH];
PARAMETER *param_list;
MODULE *first_module, *top_module;
FILE *outfile;
int i, print_top_subckt_flag;



if (argc < 3)
  {
   printf("error: need at least two arguments!\n");
   printf("  sue_spice_netlister sue_top_module sue_lib_file [outfile] [no_top]\n");
   exit(1);
  }

for (i = 0; argv[1][i] != '\0'; i++)
   top_sue_module_name[i] = tolower(argv[1][i]);
top_sue_module_name[i] = '\0';

print_top_subckt_flag = 1;

if (argc > 4)
   {
   sprintf(hspice_filename,"%s",argv[3]);
   print_top_subckt_flag = 0;
   }
else if (argc > 3)
   {
   sprintf(hspice_filename,"%s",argv[3]);
   }
else
   {
   sprintf(hspice_filename,"%s.sp",argv[1]);
   }

if ((outfile = fopen(hspice_filename,"w")) == NULL)
  {
   printf("error in 'sue_spice_netlister':  can't open file:  '%s'\n",hspice_filename);
   exit(1);
 }

param_list = init_parameter();

//// extract modules
first_module = extract_sue_modules(argv[2],param_list);
// print_keyinfo_modules(first_module); exit(1);
warn_about_duplicate_modules(first_module);

//// process modules
connect_sue_instances_to_modules(first_module);
//print_keyinfo_modules(first_module);

first_module = trim_unused_modules(first_module,top_sue_module_name);
account_for_orient(first_module);

connect_wires_to_terminals_and_labels(first_module);
connect_wires_to_themselves(first_module);
//print_wires(first_module->next);
connect_instance_nodes(first_module);
trim_node_naming_instances_from_modules(first_module);
first_module = trim_node_naming_modules(first_module);
fill_in_missing_module_external_nodenames(first_module);
// prefix_x_to_instance_names(first_module);
// print_keyinfo_modules(first_module); exit(1);


top_module = determine_module_order(first_module);
top_module->top_module_flag = 1;
first_module = sort_modules(first_module);

print_hspice_code(outfile,first_module,print_top_subckt_flag);

printf("Netlisting of cell '%s' completed with no errors\n",
       top_sue_module_name);
}

void warn_about_duplicate_modules(MODULE *first_module)
{
MODULE *cur_module,*cur_module2;
int flag;

flag = 0;
cur_module = first_module;
while (cur_module != NULL)
   {
     cur_module2 = cur_module->next;
     while (cur_module2 != NULL)
       {
         if (strcmp(cur_module->name,cur_module2->name) == 0)
	   {
	     if (flag == 0)
	       {
		 flag = 1;
	         printf("warning:  duplicate modules found:\n");
                 printf("----------------------------------\n");
	       }
	     printf("    %s in '%s'\n        and '%s'\n",cur_module->name,
              cur_module->library_path, cur_module2->library_path);
	   }
	 cur_module2 = cur_module2->next;
       }
     cur_module = cur_module->next;
   }
if (flag == 1)
    printf("----------------------------------\n");
}

void prefix_x_to_instance_names(MODULE *first_module)
{
MODULE *cur_module;
INSTANCE *cur_instance;
char temp[MAX_INSTANCE_NAME_LENGTH];
int prefix_flag;

cur_module = first_module;
while (cur_module != NULL)
   {
     prefix_flag = 0;
     cur_instance = cur_module->first_instance;
     while (cur_instance != NULL)
       {
         if (cur_instance->name[0] != 'x')
	   {
             prefix_flag = 1;
             break;
	   }
	 cur_instance = cur_instance->next;
       }
     if (prefix_flag == 1)
       {
        cur_instance = cur_module->first_instance;
        while (cur_instance != NULL)
          {
	   strcpy(temp,cur_instance->name);
	   sprintf(cur_instance->name,"x%s",temp);
	   cur_instance = cur_instance->next;
	  }
       }
   cur_module = cur_module->next;
   }
}

MODULE *extract_sue_modules(char *sue_lib_filename, PARAMETER *param_list)
{
int i,j;
MODULE *first_module;
FILE *sue_lib_file;
char dirname[300],base_dir[300],full_dir[400];
DIR *dir;
struct dirent *entry;

first_module = NULL;

//// extract modules

if ((sue_lib_file = fopen(sue_lib_filename,"r")) == NULL)
  {
   printf("error in 'extract_sue_modules':  can't open sue.lib file:  '%s'\n",
            sue_lib_filename);
   exit(1);
 }

for (i = 0; sue_lib_filename[i] != '\0'; i++);
for (    ; sue_lib_filename[i] != '/' && sue_lib_filename[i] != '\\'; i--);
for (j = 0; j < i; j++)
   {
     base_dir[j] = sue_lib_filename[j];
   }
base_dir[j] = '\0';

while (fscanf(sue_lib_file,"%s",dirname) == 1)
  {
    // Open the current directory named in the sue_lib_file
    sprintf(full_dir,"%s/SueLib/%s",base_dir,dirname);

    if ((dir = opendir(full_dir)) == NULL) 
       {
        printf("error in 'extract_sue_modules':\n");
        printf("    Could not open directory %s\n",full_dir);
	printf("    contained in the sue.lib file '%s'\n",sue_lib_filename);
        exit(1);
       }

    // change working dir to be able to read file information
    chdir(full_dir);
    while ((entry = readdir(dir)) != NULL) 
       {
       if (is_sue_file(entry->d_name))
          first_module = add_sue_module(first_module,entry->d_name,
					dirname,param_list);
       }
    closedir(dir);
  }

return(first_module);
}


int is_sue_file(char *name)
{
int i;

for (i = 0; name[i] != '\0'; i++)
   {
     if (i > 3)
        if (name[i] == 'e' && name[i-1] == 'u' && 
            name[i-2] == 's' && name[i-3] == '.')
	   if (name[i+1] == '\0')
	      return(1);
   }
return(0);
}


void determine_sue_module_dependencies(MODULE *cur_module)
{
INSTANCE *cur_instance;

if (cur_module == NULL)
   {
    printf("error in 'determine_sue_module_dependencies':  cur_module = NULL!\n");
    exit(1);
   }

cur_instance = cur_module->first_instance;
while (cur_instance != NULL)
   {
     if (cur_instance->mod == NULL)
       {
        printf("error in 'determine_sue_module_dependancies':\n");
        printf("   instance '%s' in module '%s' is not linked to its module\n",
	       cur_instance->name, cur_module->name);
	exit(1);
       }
    if (cur_instance->mod->active_sue_module_flag == 0)
      {
       cur_instance->mod->active_sue_module_flag = 1;
       determine_sue_module_dependencies(cur_instance->mod);
      }
    cur_instance = cur_instance->next;
   }
}


// note:  this routine removes unused modules, but does not delete
//        them from memory
//        fix this in the future?

MODULE *trim_unused_modules(MODULE *first_module, char *top_module_name)
{
MODULE *cur_module;
MODULE *first_retained_module, *cur_retained_module;

cur_module = first_module;
while (cur_module != NULL)
   {
    if (strcmp(cur_module->name,top_module_name) == 0)
       break;
    cur_module = cur_module->next;
   }

if (cur_module == NULL)
   {
    printf("error in 'trim_unused_modules':  can't find top_module '%s'\n",
	    top_module_name);
    exit(1);
   }

cur_module->active_sue_module_flag = 1;

determine_sue_module_dependencies(cur_module);

cur_module = first_module;
cur_retained_module = NULL;
while (cur_module != NULL)
   {
    if (cur_module->active_sue_module_flag == 1)
       {
	 if (cur_retained_module == NULL)
	   {
	     first_retained_module = cur_module;
	     cur_retained_module = cur_module; 
	   }
	 else
	   {
	     cur_retained_module->next = cur_module;
	     cur_retained_module = cur_retained_module->next;
	   }
       }
    cur_module = cur_module->next;
   }
cur_retained_module->next = NULL;

return(first_retained_module);
}


// note:  this routine removes unused modules, but does not delete
//        them from memory
//        fix this in the future?

MODULE *trim_node_naming_modules(MODULE *first_module)
{
MODULE *cur_module;
MODULE *first_retained_module, *cur_retained_module;

cur_module = first_module;
cur_retained_module = NULL;
while (cur_module != NULL)
   {
    if (is_node_naming_object(cur_module->name) == 0)
       {
	 if (cur_retained_module == NULL)
	   {
	     first_retained_module = cur_module;
	     cur_retained_module = cur_module; 
	   }
	 else
	   {
	     cur_retained_module->next = cur_module;
	     cur_retained_module = cur_retained_module->next;
	   }
       }
    cur_module = cur_module->next;
   }
cur_retained_module->next = NULL;

return(first_retained_module);
}


void fill_in_missing_module_external_nodenames(MODULE *first_module)
{
MODULE *cur_module;
INSTANCE *cur_unused_instance;
NODES *cur_ext_node;

cur_module = first_module;
while (cur_module != NULL)
   {
    cur_unused_instance = cur_module->first_unused_instance;
    while (cur_unused_instance != NULL)
      {
        if (strcmp(cur_unused_instance->mod_name,"input") == 0 ||
            strcmp(cur_unused_instance->mod_name,"output") == 0 ||
            strcmp(cur_unused_instance->mod_name,"inout") == 0)
	  {
            cur_ext_node = cur_module->first_ext_node;
	    while (cur_ext_node != NULL)
	      {
                if (strcmp(cur_ext_node->name,cur_unused_instance->name) == 0)
		  break;
		cur_ext_node = cur_ext_node->next;
	      }
	    if (cur_ext_node == NULL) // node doesn't exist yet
	      {
               cur_ext_node = add_ext_node_sue(cur_module,
                                   cur_unused_instance->name);
               strcpy(cur_ext_node->node_dir,cur_unused_instance->mod_name);
	      }
	  }
	cur_unused_instance = cur_unused_instance->next;
      }
    cur_module = cur_module->next;
   }
}

void trim_node_naming_instances_from_modules(MODULE *first_module)
{
MODULE *cur_module;
INSTANCE *cur_instance,*prev_instance,*cur_unused_instance;

cur_module = first_module;
while (cur_module != NULL)
   {
   cur_instance = cur_module->first_instance;
   prev_instance = NULL;
   cur_unused_instance = cur_module->first_unused_instance;
   while (cur_instance != NULL)
      {
      if (is_node_naming_object(cur_instance->mod_name))
	{
	  if (prev_instance == NULL)
	      cur_module->first_instance = cur_instance->next;
	  else
	      prev_instance->next = cur_instance->next;

	  if (cur_unused_instance == NULL)
	     {
	      cur_module->first_unused_instance = cur_instance;
	      cur_unused_instance = cur_instance;
	      cur_instance = cur_instance->next;
	      cur_unused_instance->next = NULL;
	     }
	  else
	     {
              cur_unused_instance->next = cur_instance;
	      cur_unused_instance = cur_unused_instance->next;
	      cur_instance = cur_instance->next;
	      cur_unused_instance->next = NULL;
	     }
	  continue;
	}
      prev_instance = cur_instance;
      cur_instance = cur_instance->next;
      }
   cur_module = cur_module->next;
   }
}


void print_wires(MODULE *cur_module)
{
WIRE *cur_wire;

if (cur_module == NULL)
    {
      printf("error in 'print_wires':  cur_module is NULL\n");
      exit(1);
    }

cur_wire = cur_module->first_wire;
while (cur_wire != NULL)
   {
     printf("wire:  node '%s', %d %d %d %d\n",
	    cur_wire->node_name, cur_wire->x1, cur_wire->y1,
	    cur_wire->x2, cur_wire->y2);
     cur_wire = cur_wire->next;
   }
}


void connect_instance_nodes(MODULE *first_module)
{
MODULE *cur_module;
INSTANCE *cur_instance;
NODES *cur_node;
char node_name[MAX_CHAR_LENGTH];
int count;
int x, y;

cur_module = first_module;
while (cur_module != NULL)
   {
   cur_instance = cur_module->first_instance;
   while (cur_instance != NULL)
     {
       cur_node = cur_instance->first_node;
       while (cur_node != NULL)
	 {
	  if (strcmp(cur_node->name,"NULL") != 0)
	     {
	       cur_node = cur_node->next;
	       continue;
	     }
	  x = cur_instance->origin_x + cur_node->origin_x;
	  y = cur_instance->origin_y + cur_node->origin_y; 
	  determine_connecting_wire_node_name(cur_module,x,y,node_name);
	  //	  printf("cur_instance = '%s', node_name = '%s'\n",
	  //		 cur_instance->name,node_name);
	  if (strcmp(node_name,"NULL") == 0)
	    {
	     determine_connecting_terminal_or_global_node_name(cur_module,x,y,
                                                               node_name);
	    }

	  if (strcmp(node_name,"NULL") == 0 ||
              strcmp(node_name,"FOUND_TERM") == 0) // unconnected or no labels
	    {	
	      // make up a node name        
              count = 0;
	      while(1)
		{
	         sprintf(node_name,"n%d",count++);
		 if (check_if_node_name_is_available(cur_module,node_name))
		   {
	            strcpy(cur_node->name,node_name);
	            break;
		   }
		}
              if (strcmp(node_name,"FOUND_TERM") == 0) // connect other terminals
		{
		  label_connecting_terminals_or_globals(cur_module,x,y,node_name);
		}
	    }
	  else 
	    {
	      label_connecting_terminals_or_globals(cur_module,x,y,node_name);
	    }
	  cur_node = cur_node->next;
	 }
       cur_instance = cur_instance->next;
     }
   cur_module = cur_module->next;
   }
}

void connect_wires_to_terminals_and_labels(MODULE *first_module)
{
MODULE *cur_module;
INSTANCE *cur_instance;
WIRE *cur_wire;
int x1, y1, x2, y2;
int xterm, yterm;

cur_module = first_module;
while (cur_module != NULL)
   {
     cur_wire = cur_module->first_wire;
     while (cur_wire != NULL)    
       {
	 x1 = cur_wire->x1;
	 y1 = cur_wire->y1;
	 x2 = cur_wire->x2;
	 y2 = cur_wire->y2;

         cur_instance = cur_module->first_instance;
	 while (cur_instance != NULL)
	   {
             if (is_node_naming_object(cur_instance->mod_name))
	         {
		  if (cur_instance->first_node == NULL)
		     {
		      printf("error in 'connect_wires_to_terminals_and_labels':\n");
		      printf("   undefined node for terminal!\n");
		      exit(1);
		     }
                   xterm = cur_instance->first_node->origin_x +
		           cur_instance->origin_x;
                   yterm = cur_instance->first_node->origin_y +
		           cur_instance->origin_y;
		   if (x1 == x2) // vertical wire
		      {
			if (xterm == x1 && yterm >= y1 && yterm <= y2)
			  {
			    if (strcmp(cur_wire->node_name,"NULL") != 0)
			      {
				if (strcmp(cur_wire->node_name,cur_instance->name) != 0)
				  {
				   printf("error in 'connect_wires_to_terminals_and_labels':\n");
				   printf("  wire has already been named!\n");
				   printf("  wire name:  %s\n",cur_wire->node_name);
				   printf("  terminal name:  %s\n",cur_instance->name);
				   exit(1);
				  }
			      }
			    else
                                strcpy(cur_wire->node_name,cur_instance->name);
			  }
		      }
		   else if (y1 == y2) // horizontal wire
		      {
			if (yterm == y1 && xterm >= x1 && xterm <= x2)
			  {
			    if (strcmp(cur_wire->node_name,"NULL") != 0)
			      {
				if (strcmp(cur_wire->node_name,cur_instance->name) != 0)
				  {
				   printf("error in 'connect_wires_to_terminals_and_labels':\n");
				   printf("  wire has already been named!\n");
				   printf("  wire name:  %s\n",cur_wire->node_name);
				   printf("  terminal name:  %s\n",cur_instance->name);
				   exit(1);
				  }
			      }
			    else
                                strcpy(cur_wire->node_name,cur_instance->name);
			  }
		      }
                   else // diagonal
		      {
		      printf("error in 'connect_wires_to_terminals_and_labels':\n");
		      printf("  non-manhatten wires not supported right now\n");
		      exit(1);
		      }
	         }
	     cur_instance = cur_instance->next;
	   }
	 cur_wire = cur_wire->next;
       }
     cur_module = cur_module->next;
   }

}


int label_unnamed_wires(MODULE *cur_module, int count_in)
{
WIRE *cur_wire,*cur_wire2;
char cur_node_name[30];
int count, found_name_flag;

if (cur_module == NULL)
   {
     printf("error in 'label_unnamed_wires':  cur_module is NULL\n");
     exit(1);
   }

count = count_in;
cur_wire = cur_module->first_wire;
while (cur_wire != NULL)
   {
    if (strcmp(cur_wire->node_name,"NULL") == 0)
      {
	sprintf(cur_node_name,"n%d",count++);

	found_name_flag = 0;
	cur_wire2 = cur_module->first_wire;
	while (cur_wire2 != NULL)
	  {
	   if (strcmp(cur_wire2->node_name,cur_node_name) == 0)
	     {
	       found_name_flag = 1;
	       break;
	     }
	   cur_wire2 = cur_wire2->next;
	  }
        if (found_name_flag == 0)
	  {
	    strcpy(cur_wire->node_name,cur_node_name);
	    break;
	  }
	else
	    continue;
      }
    cur_wire = cur_wire->next;
   }
if (cur_wire == NULL) // found no unnamed wires
   count = -1;

return(count);
}


int count_unnamed_wires(MODULE *cur_module)
{
WIRE *cur_wire;
int count;

if (cur_module == NULL)
   {
     printf("error in 'count_unnamed_wires':  cur_module is NULL\n");
     exit(1);
   }

cur_wire = cur_module->first_wire;

count = 0;
while (cur_wire != NULL)
   {
    if (strcmp(cur_wire->node_name,"NULL") == 0)
       count++;
    cur_wire = cur_wire->next;
   }
return(count);
}

void connect_wires_to_themselves(MODULE *first_module)
{
MODULE *cur_module;
INSTANCE *cur_instance;
WIRE *cur_wire, *cur_wire2;
int x1, y1, x2, y2;
int wire_match_flag;
int count, prev_count, node_label_count;

prev_count = 0;
node_label_count = 0;
cur_module = first_module;
while (cur_module != NULL)
   {
     count = count_unnamed_wires(cur_module);
     /*      printf("count = %d, module = %s\n",count,cur_module->name); */
     if (count == prev_count)
       {
	 node_label_count = label_unnamed_wires(cur_module,node_label_count);
	 /*
     	 printf("******************** module '%s', count = %d ***********\n",
		cur_module->name,node_label_count);
         print_wires(cur_module);
	 */
         if (node_label_count == -1)
	   {
	    cur_module = cur_module->next;
	    node_label_count = 0;
	    prev_count = 0;
  	    continue;
	   }
	 else
	    prev_count = count;
       }
     else
        prev_count = count;

     cur_wire = cur_module->first_wire;
     while (cur_wire != NULL)    
       {
	 if (strcmp(cur_wire->node_name,"NULL") == 0)
	   {
	     cur_wire = cur_wire->next;
	     continue;
	   }
	 x1 = cur_wire->x1;
	 y1 = cur_wire->y1;
	 x2 = cur_wire->x2;
	 y2 = cur_wire->y2;
         cur_wire2 = cur_module->first_wire;
	 while (cur_wire2 != NULL)
	   {
  	    wire_match_flag = 0;
	    if (x1 == x2) // vertical wire
	       {
		if (cur_wire2->x1 == x1)
		  {
		   if (cur_wire2->y1 >= y1 && cur_wire2->y1 <= y2)
		       wire_match_flag = 1;
		  }
		if (cur_wire2->x2 == x1)
		  {
		   if (cur_wire2->y2 >= y1 && cur_wire2->y2 <= y2)
		       wire_match_flag = 1;
		  }
		if (cur_wire2->x2 >= x1 && cur_wire2->x1 <= x1)
		  {
		   if (cur_wire2->y1 == y1 || cur_wire2->y2 == y1 ||
                       cur_wire2->y1 == y2 || cur_wire2->y2 == y2 )
		       wire_match_flag = 1;
		  }
	       }
	    else if (y1 == y2) // horizontal wire
	       {
		if (cur_wire2->y1 == y1)
		  {
		   if (cur_wire2->x1 >= x1 && cur_wire2->x1 <= x2)
		       wire_match_flag = 1;
		  }
		if (cur_wire2->y2 == y1)
		  {
		   if (cur_wire2->x2 >= x1 && cur_wire2->x2 <= x2)
		       wire_match_flag = 1;
		  }
		if (cur_wire2->y2 >= y1 && cur_wire2->y1 <= y1)
		  {
		   if (cur_wire2->x1 == x1 || cur_wire2->x2 == x1 ||
                       cur_wire2->x1 == x2 || cur_wire2->x2 == x2 )
		       wire_match_flag = 1;
		  }
	       }
            else // diagonal
	       {
		printf("error in 'connect_wires_to_themselves':\n");
		printf("  non-manhatten wires not supported right now\n");
		exit(1);
	       }
	    if (wire_match_flag == 1)
	      {
	       if (strcmp(cur_wire2->node_name,"NULL") != 0)
		  {
		  if (strcmp(cur_wire2->node_name,cur_wire->node_name) != 0)
		      {
   		       printf("error in 'connect_wires_to_themselves':\n");
		       printf("wire has already been associated with a different node!\n");
		       printf("wire node:  %s,  wire2 node:  %s\n",cur_wire->node_name,cur_wire2->node_name);
		       exit(1);
		      }
		  }
	       else
		 {
		  strcpy(cur_wire2->node_name,cur_wire->node_name);
		 }
	      } 
	    cur_wire2 = cur_wire2->next;
	   }
	 cur_wire = cur_wire->next;
       }
   }
}


void determine_connecting_wire_node_name(MODULE *cur_module, int x, int y, 
                                         char *node_name)
{
WIRE *cur_wire;
int x1, y1, x2, y2;

cur_wire = cur_module->first_wire;
strcpy(node_name,"NULL");

while (cur_wire != NULL)    
   {
    x1 = cur_wire->x1;
    y1 = cur_wire->y1;
    x2 = cur_wire->x2;
    y2 = cur_wire->y2;

    if ((x1 == x && y1 == y) || (x2 == x && y2 == y))
      {
	strcpy(node_name,cur_wire->node_name);
	break;
      }

    cur_wire = cur_wire->next;
   }
}

void determine_connecting_terminal_or_global_node_name(MODULE *cur_module, 
                             int x, int y, char *node_name)
{
INSTANCE *cur_instance;
NODES *cur_node;
int x1, y1;
int found_flag;

cur_instance = cur_module->first_instance;

found_flag = 0;
strcpy(node_name,"NULL");
while (cur_instance != NULL)    
   {
    cur_node = cur_instance->first_node;
    while (cur_node != NULL)
      {
       x1 = cur_instance->origin_x + cur_node->origin_x;
       y1 = cur_instance->origin_y + cur_node->origin_y;
       if (x == x1 && y == y1)
	  break;
       cur_node = cur_node->next;
      }
    if (cur_node != NULL)
      {
	found_flag = 1;
	if (strcmp(cur_node->name,"NULL") != 0)
	  {
	    if (strcmp(node_name,"NULL") != 0 &&
		strcmp(node_name,cur_node->name) != 0)
	      {
		printf("error in 'determine_connecting_terminal_or_global_node_name':\n");
		printf("   conflict of node names!\n");
		exit(1);
	      }
	    strcpy(node_name,cur_node->name);
	  }
      }
    cur_instance = cur_instance->next;
   }

if (found_flag == 0)
    strcpy(node_name,"NULL");
else if (strcmp(node_name,"NULL") == 0)
    strcpy(node_name,"FOUND_TERM");
}

void label_connecting_terminals_or_globals(MODULE *cur_module, int x, int y, 
                                            char *name)
{
INSTANCE *cur_instance;
NODES *cur_node;
int x1, y1;

cur_instance = cur_module->first_instance;

while (cur_instance != NULL)    
   {
    cur_node = cur_instance->first_node;
    while (cur_node != NULL)
      {
       x1 = cur_instance->origin_x + cur_node->origin_x;
       y1 = cur_instance->origin_y + cur_node->origin_y;
       if (x == x1 && y == y1)
	 {
 	 if (strcmp(cur_node->name,"NULL") != 0)
	    {
	    if (strcmp(name,cur_node->name) != 0)
	       {
		printf("error in 'label_connecting_terminals_or_globals':\n");
		printf("   conflict of node names!\n");
		exit(1);
	       }
	    }
	 else
	    strcpy(cur_node->name,name);
	 }
       cur_node = cur_node->next;
      }
    cur_instance = cur_instance->next;
   }
}

int check_if_node_name_is_available(MODULE *cur_module, char *name)
{
INSTANCE *cur_instance;
NODES *cur_node;
WIRE *cur_wire;


cur_wire = cur_module->first_wire;
while (cur_wire != NULL)
   {
   if (strcmp(cur_wire->node_name,name) == 0)
      return(0);
   cur_wire = cur_wire->next;
   }

cur_instance = cur_module->first_instance;
while (cur_instance != NULL)    
   {
    cur_node = cur_instance->first_node;
    while (cur_node != NULL)
      {
	if (strcmp(cur_node->name,name) == 0)
	  return(0);
	cur_node = cur_node->next;
      }
    cur_instance = cur_instance->next;
   }
return(1);
}


void print_hspice_code(FILE *fp, MODULE *first_module, 
                       int print_top_subckt_flag)
{
MODULE *cur_module;
INSTANCE *cur_instance;
NODES *cur_mod_node,*cur_inst_node;
MOD_PARAMETER *cur_mod_parameter;
char temp[MAX_ARG],primitive_string[MAX_TEXT];
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
     printf("error in 'print_hspice_code': no top module!\n");
     exit(1);
   }
fprintf(fp,"***** Hspice Netlist for Cell '%s' *****\n\n",cur_module->name);

cur_module = first_module;
while (cur_module != NULL)
  {
    // don't netlist primitives as modules
    if (cur_module->spice_text != NULL)
      {
      cur_module = cur_module->next;
      continue;
      }

    // print header for module
    fprintf(fp,"************** Module %s **************\n",cur_module->name);

    // print out .subckt definition of module unless top module
    //     (print top module if print_top_subckt_flag == 1) 
    if (cur_module->top_module_flag != 1 ||
       (cur_module->top_module_flag == 1 && print_top_subckt_flag == 1))
      {
       fprintf(fp,".subckt %s",cur_module->name);
       // print nodes
       cur_mod_node = cur_module->first_ext_node;
       while (cur_mod_node != NULL)
         {
         fprintf(fp," %s",cur_mod_node->name);
         cur_mod_node = cur_mod_node->next;
         }
       // print parameters
       cur_mod_parameter = cur_module->first_mod_parameter;
       while (cur_mod_parameter != NULL)
         {
	   if (strcmp(cur_mod_parameter->value_text,"NULL") != 0)
              fprintf(fp," %s=%s",cur_mod_parameter->name,
                               cur_mod_parameter->value_text);
	   else
              fprintf(fp," %s=1",cur_mod_parameter->name);
          cur_mod_parameter = cur_mod_parameter->next;
         }
       fprintf(fp,"\n");
      }

    //////////////// netlist instances /////////////////////////
    cur_instance = cur_module->first_instance;
    while (cur_instance != NULL)
      {
       // print out non-primitive instances as is
       if (cur_instance->mod->spice_text == NULL)
	 {
	   // print instance name
	  fprintf(fp,"%s",cur_instance->name);
	  // print nodes
	  cur_inst_node = cur_instance->first_node;
	  while (cur_inst_node != NULL)
	    {
	      fprintf(fp," %s",cur_inst_node->name);
	     cur_inst_node = cur_inst_node->next;
	    }
	  // print module name
	  fprintf(fp," %s",cur_instance->mod->name);
          // print parameters
	  cur_mod_parameter = cur_instance->first_mod_parameter;
	  while (cur_mod_parameter != NULL)
	    {
	      fprintf(fp," %s=%s",cur_mod_parameter->name,
                               cur_mod_parameter->value_text);
	      cur_mod_parameter = cur_mod_parameter->next;
	    }
	  fprintf(fp,"\n");
	 }
       else // turn primitive instance into a primitive
	 {
	  extract_primitive_string(cur_instance,primitive_string);
	  fprintf(fp,"%s\n",primitive_string);
	 } 
       cur_instance = cur_instance->next;
      }
    if (cur_module->top_module_flag != 1 ||
       (cur_module->top_module_flag == 1 && print_top_subckt_flag == 1))
       fprintf(fp,".ends %s\n\n",cur_module->name);
    cur_module = cur_module->next;
  }
fprintf(fp,"\n.end\n\n");
}


void extract_primitive_string(INSTANCE *cur_instance, char *out_string)
{
char *spice_text;
char temp_string[MAX_TEXT],temp_string2[MAX_TEXT];
int i,j,k;

spice_text = cur_instance->mod->spice_text;

if (spice_text == NULL)
   {
     printf("error in 'extract_primitive_string':  spice_text is NULL\n");
     exit(1);
   }

for (i = 0, j = 0; spice_text[i] != '\0' && j < MAX_TEXT; i++)
   {
     if (spice_text[i] == '$')
        {
	 i++;
         k = i;
	 for (    ; (spice_text[i] >= '0' && spice_text[i] <= '9') ||
                       (spice_text[i] >= 'a' && spice_text[i] <= 'z') ||
		       (spice_text[i] >= 'A' && spice_text[i] <= 'Z') ||
		        spice_text[i] == '_'; i++)
	     temp_string[i-k] = spice_text[i];
         temp_string[i-k] = '\0';
         i--;
         retrieve_instance_node_name(cur_instance,temp_string,temp_string2);
         if (strcmp(temp_string2,"NULL") == 0)
             retrieve_instance_parameter_value(cur_instance,temp_string,
                           temp_string2);
         if (strcmp(temp_string2,"NULL") == 0)
	   {
	     printf("error in 'extract_primitive_string':\n");
	     printf("   variable '%s' is not a node or parameter\n",
		    temp_string);
	     printf("   in instance '%s' (module '%s')\n",cur_instance->name,
                      cur_instance->mod->name);
	     exit(1);
	   }
         for (k = 0; temp_string2[k] != '\0'; k++)
	    out_string[j++] = temp_string2[k];
        }
     else
         out_string[j++] = spice_text[i];
   }
out_string[j] = '\0';
}


void retrieve_instance_node_name(INSTANCE *cur_instance, char *mod_node_name,
                                 char *output)
{
NODES *cur_mod_node,*cur_inst_node;

cur_mod_node = cur_instance->mod->first_ext_node;
cur_inst_node = cur_instance->first_node;

while (cur_mod_node != NULL && cur_inst_node != NULL)
   {
    if (strcmp(cur_mod_node->name,mod_node_name) == 0)
	break;
    cur_mod_node = cur_mod_node->next;
    cur_inst_node = cur_inst_node->next;
   }

if (cur_inst_node != NULL)
   strcpy(output,cur_inst_node->name);
else
   strcpy(output,"NULL");
}

void retrieve_instance_parameter_value(INSTANCE *cur_instance,
					char *instance_parameter_name,
                                        char *output)
{
MOD_PARAMETER *cur_inst_parameter, *cur_mod_parameter;

cur_inst_parameter = cur_instance->first_mod_parameter;
cur_mod_parameter = cur_instance->mod->first_mod_parameter;

if (strcmp(instance_parameter_name,"name") == 0)
   {
     strcpy(output,cur_instance->name);
     return;
   }
while (cur_mod_parameter != NULL)
   {
   if (strcmp(cur_mod_parameter->name,instance_parameter_name) == 0)
       break;       
   cur_mod_parameter = cur_mod_parameter->next;
   }
if (cur_mod_parameter != NULL)
   {
    while (cur_inst_parameter != NULL)
       {
	if (strcmp(cur_inst_parameter->name,cur_mod_parameter->name) == 0)
	   break;
	cur_inst_parameter = cur_inst_parameter->next;
       }
    if (cur_inst_parameter != NULL)
	strcpy(output,cur_inst_parameter->value_text);
    else
        strcpy(output,cur_mod_parameter->value_text);
   }
else
   strcpy(output,"NULL");
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

void sort_instances(MODULE *cur_module)
{
INSTANCE *cur_instance, *prev_instance, *prev_prev_instance;
INSTANCE *sim_order_not_found_instance;
MOD_PARAMETER *cur_mod_parameter;
double prev_sim_order;
int sim_order_found_flag, order_from_code_flag;

sim_order_not_found_instance = NULL;
sim_order_found_flag = 0;

if (cur_module == NULL)
   {
     printf("error in 'sort_instances':  cur_module is NULL!\n");
     exit(1);
   }
cur_instance = cur_module->first_instance;
// transfer sim_order parameter to sim_order variable in instance
while (cur_instance != NULL)
   {
     cur_mod_parameter = cur_instance->first_mod_parameter;
     while (cur_mod_parameter != NULL)
       {
         if (strcmp(cur_mod_parameter->name,"sim_order") == 0)
	    break;
	 cur_mod_parameter = cur_mod_parameter->next;
       }
     if (cur_mod_parameter == NULL) // sim_order parameter not defined
       { 
         cur_instance->sim_order = NaN;
         sim_order_not_found_instance = cur_instance;
         cur_instance = cur_instance->next;
         continue;
	 //    printf("error in 'sort_instances':  'sim_order' parameter\n");
	 //    printf("   not defined for instance '%s'\n",cur_instance->name);
	 //    printf("   ---> look in module '%s' in the netlist\n",
	 //		cur_module->name);
	 //    exit(1);
       } 
     if (cur_mod_parameter->use_text_flag == 1)
       {
	 printf("error in 'sort_instances':  'sim_order' parameter\n");
         printf("   can not be an expression!\n");
         printf("   For instance '%s', sim_order = '%s'\n",cur_instance->name,
                 cur_mod_parameter->value_text);
         printf("   ---> look in module '%s' in the netlist\n",
		cur_module->name);
         exit(1);
       } 
     cur_instance->sim_order = cur_mod_parameter->value;
     sim_order_found_flag = 1;
     cur_instance = cur_instance->next;
   }
if (sim_order_found_flag == 1 && sim_order_not_found_instance != NULL)
   {
     printf("Warning:  module '%s' has some instances that are ordered\n",
	    cur_module->name);
     printf("          using the 'sim_order' parameter and at least one\n");
     printf("          that does not have the 'sim_order' parameter defined\n");
     printf("          Specifically, instance '%s' does not have\n",
                            sim_order_not_found_instance->name);
     printf("          the 'sim_order' parameter defined\n");
     printf("          ALL sim_order PARAMETERS WILL BE IGNORED IN THIS MODULE\n");
     printf("          ----> look in module '%s' in the netlist\n",
	                cur_module->name);
   }
/////////////////////////////////////
// cur_instance = cur_module->first_instance;
// while (cur_instance != NULL)
//   {
//     printf("---> instance '%s'\n",cur_instance->name);
//     cur_instance = cur_instance->next;
//   }
/////////////////////////////////////

if (sim_order_not_found_instance != NULL) 
   {
     order_from_code_flag = 0;
     if (cur_module->code != NULL)
        if (cur_module->code->order != NULL)
	  order_from_code_flag = 1;
     if (order_from_code_flag == 1)
        {
	// use order information in module code
	 determine_instance_order_from_code(cur_module);
        }
     else
        {
        // do a complicated algorithm to determine instance order
        determine_instance_order(cur_module);
        }
   }

cur_instance = cur_module->first_instance;
if (cur_instance != NULL)
     prev_sim_order = cur_instance->sim_order;
while (cur_instance != NULL)
    {
      if (cur_instance->sim_order < prev_sim_order)
        {
        if (prev_instance == cur_module->first_instance)
	   {
            prev_instance->next = cur_instance->next;
	    cur_module->first_instance = cur_instance;
            cur_module->first_instance->next = prev_instance;
	   }
        else
	   {
            prev_instance->next = cur_instance->next;
            prev_prev_instance->next = cur_instance;
            prev_prev_instance->next->next = prev_instance;
	   }
        cur_instance = cur_module->first_instance;
        }
      else if (cur_instance->sim_order == prev_sim_order &&
             cur_instance != cur_module->first_instance  &&
             sim_order_not_found_instance == NULL)
        {
	printf("error in 'sort_instances':  sim_order is the same\n");
        printf("  for two instances!\n");
        printf("  specifically, instance '%s' has sim_order = %5.3f\n",
	       cur_instance->name,cur_instance->sim_order);
        printf("            and instance '%s' has sim_order = %5.3f\n",
               prev_instance->name,prev_instance->sim_order);
        exit(1);
       }
      prev_sim_order = cur_instance->sim_order;
      prev_prev_instance = prev_instance;
      prev_instance = cur_instance; 
      cur_instance = cur_instance->next;
    }

/////////////////////////////////////
// cur_instance = cur_module->first_instance;
// while (cur_instance != NULL)
//   {
//     printf(">>>>> instance '%s'\n",cur_instance->name);
//     cur_instance = cur_instance->next;
//   }
// printf("\n");
/////////////////////////////////////

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

void determine_instance_order_from_code(MODULE *cur_module)
{
INSTANCE *cur_instance;
CODE_ENTRY *cur_code_entry;
int count;

if (cur_module == NULL)
   {
     printf("error in 'determine_instance_order_from_code':  cur_module is NULL!\n");
     exit(1);
   }
if (cur_module->code == NULL)
   {
     printf("error in 'determine_instance_order_from_code':  cur_module->code is NULL!\n");
     exit(1);
   }
cur_instance = cur_module->first_instance;
while (cur_instance != NULL)
   {
     cur_instance->sim_order = NaN;
     cur_instance = cur_instance->next;
   }
count = 0;
cur_code_entry = cur_module->code->order;
while (cur_code_entry != NULL)
   {
     cur_instance = cur_module->first_instance;
     while (cur_instance != NULL)
       {
         if (strcmp(cur_code_entry->name,cur_instance->name) == 0)
	   {
	    if (cur_instance->sim_order != NaN)
	       {
		printf("error in 'determine_instance_order_from_code':\n");
		printf("   instance '%s' is repeated in 'sim_order:' list\n",
                        cur_instance->name);
                printf("   ----> look at module '%s' in the code file\n",
		       cur_module->name);
                exit(1);
	       }
            else
	       {
		cur_instance->sim_order = count;
                break;    
	       }
	   }
	 cur_instance = cur_instance->next;
       }
     if (cur_instance == NULL)
       {
        printf("error in 'determine_instance_order_from_code':\n");
	printf("   instance '%s' is included in the 'sim_order:' list\n",
                        cur_code_entry->name);
        printf("   but is not present in the netlist description of the module\n");
        printf("   ----> look at module '%s' in the code file (and netlist)\n",
		       cur_module->name);
        exit(1);
       }
     count++;
     cur_code_entry = cur_code_entry->next;
   }
cur_instance = cur_module->first_instance;
while (cur_instance != NULL)
   {
     if (cur_instance->sim_order == NaN)
       {
        printf("error in 'determine_instance_order_from_code':\n");
	printf("   instance '%s' is not included in 'sim_order:' list\n",
                        cur_instance->name);
        printf("   ----> look at module '%s' in the code file (and netlist)\n",
		       cur_module->name);
        exit(1);
       }
     cur_instance = cur_instance->next;
   }
}

void determine_instance_order(MODULE *cur_module)
{
INSTANCE *cur_instance,*loop_instance;

if (cur_module == NULL)
   {
     printf("error in 'determine_instance_order':  cur_module is NULL!\n");
     exit(1);
   }
cur_instance = cur_module->first_instance;
while (cur_instance != NULL)
   {
    loop_instance = cur_module->first_instance;
    while (loop_instance != NULL)
       {
         loop_instance->already_visited_flag = 0;
	 loop_instance = loop_instance->next;
       }
    cur_instance->already_visited_flag = 1;
    cur_instance->sim_order = 
      (double) determine_instance_dependencies(cur_module,cur_instance,1);
    cur_instance = cur_instance->next;
   }
}

int determine_instance_dependencies(MODULE *cur_module, 
                                    INSTANCE *cur_instance, int weight)
{
INSTANCE *loop_instance;
int count;
NODES *cur_input_node, *cur_output_node;

if (cur_module == NULL)
   {
    printf("error in 'determine_instance_dependencies':  cur_module = NULL!\n");
    exit(1);
   }
if (cur_instance == NULL)
   {
    printf("error in 'determine_instance_dependencies':  cur_instance = NULL!\n");
    exit(1);
   }
count = 0;
cur_input_node = cur_instance->first_node;
while(cur_input_node != NULL)
   {
    if (strcmp(cur_input_node->node_dir,"input") == 0)
      {
       loop_instance = cur_module->first_instance;
       while (loop_instance != NULL)
          {
	   if (loop_instance->already_visited_flag == 1)
	      {
               loop_instance = loop_instance->next;
               continue;
	      }
	   cur_output_node = loop_instance->first_node;
	   while(cur_output_node != NULL)
	     {
	      if (strcmp(cur_output_node->node_dir,"output") == 0)
		 {
		  if (strcmp(cur_output_node->name,cur_input_node->name)
		      == 0)
		    {
		     loop_instance->already_visited_flag = 1;
		     count += weight;
                     count += determine_instance_dependencies(cur_module, 
				   loop_instance, weight+1);
                     break;
		    }
		 }
	      cur_output_node = cur_output_node->next;
	     }
          loop_instance = loop_instance->next;
          }
      }
    cur_input_node = cur_input_node->next;
   }
return(count);
}

void check_instance_parameters(MODULE *first_module, PARAMETER *param_list)
{
MODULE *cur_module;
INSTANCE *cur_instance;
MOD_PARAMETER *cur_mod_parameter,*cur_inst_parameter;
int num_parameters;

cur_module = first_module;
while (cur_module != NULL)
  {
  // save number of current parameters
  num_parameters = param_list->main_par_length;
  // put in default module parameter values into param_list
  cur_mod_parameter = cur_module->first_mod_parameter;
  param_list->param_not_found = 0;  // don't abort on unknown variables
  while(cur_mod_parameter != NULL)
    {
     eval_paren_expr(cur_mod_parameter->value_text,"invalid",NULL,
                          param_list);

     if (param_list->param_not_found == 1)
       {
	 printf("error in 'check_instance_parameters': the default values\n");
         printf("   of module parameters must not have unknown parametersn\n");
         printf("   in this case, parameter '%s' had a default value of '%s'\n",cur_mod_parameter->name,cur_mod_parameter->value_text);
         printf("   ---> look at module '%s' in the netlist file\n",
                cur_module->name);
         exit(1);
       }
     add_param_value(cur_mod_parameter->name,cur_mod_parameter->value,
         param_list);
     cur_mod_parameter = cur_mod_parameter->next;
    }
  param_list->param_not_found = -1; // return to abort behavior

  cur_instance = cur_module->first_instance;
  while(cur_instance != NULL)
     {
     // check that instance parameters coincide to associated module 
     // parameters 
     cur_inst_parameter = cur_instance->first_mod_parameter;
     while(cur_inst_parameter != NULL)
       {
	cur_mod_parameter = cur_instance->mod->first_mod_parameter;
	while (cur_mod_parameter != NULL)
	  {
	   if (strcmp(cur_inst_parameter->name,cur_mod_parameter->name) == 0)
	       break;
	   cur_mod_parameter = cur_mod_parameter->next;
	  }
        if (cur_mod_parameter == NULL &&
            strcmp(cur_inst_parameter->name,"sim_order") != 0)
	  {
	    printf("error in 'check_instance_parameters':  the instance\n");
            printf("   parameter '%s' in instance '%s' is not defined\n",
		   cur_inst_parameter->name,cur_instance->name);
            printf("   in its associated module '%s' in the netlist\n",
		   cur_instance->mod->name);
            printf("   ---> look at module '%s' in the netlist file\n",
                cur_module->name);
            exit(1);
	  }
        cur_inst_parameter = cur_inst_parameter->next;
       }
     // fill in missing instance parameters according to module definition 
     //  ->  if module had all parameters defined, then simply add any
     //        missing parameters in an instance call from the defaults
     //        supplied by the module
     //      if module was missing parameters, then all instances must
     //        have the same list of parameters
     //        -> generate an error if this condition is not met
     cur_mod_parameter = cur_instance->mod->first_mod_parameter;
     while(cur_mod_parameter != NULL)
       {
	cur_inst_parameter = cur_instance->first_mod_parameter;
	while (cur_inst_parameter != NULL)
	  {
	   if (strcmp(cur_inst_parameter->name,cur_mod_parameter->name) == 0)
	       break;
	   cur_inst_parameter = cur_inst_parameter->next;
	  }
        if (cur_inst_parameter == NULL)
	  {
            // Old method: evaluate instance parameter immediately
	    //    add_instance_parameter(cur_instance,cur_mod_parameter->name,
            //        cur_mod_parameter->value," ",0);

            // New method: keep instance parameter in text form
            //  also: generate error if instance parameter not defined
            //    AND there is no default value provided in the module
            //    definition
            //    note:  if no default value provided in module definition
            //           then the mod parameter value will be -1.0 and
            //           its text value "0.0"
	    if (cur_instance->mod->missing_parameters_flag == 1 &&
		((cur_mod_parameter->value == -1.0) && 
                 (strcmp(cur_mod_parameter->value_text,"0.0") == 0)))
	      {
   	       printf("error in 'check_instance_parameters':  the instance\n");
               printf("   parameter '%s' is not defined in instance '%s'\n",
		   cur_mod_parameter->name,cur_instance->name);
               printf("   and does not have a default value provided in\n");
               printf("   the module definition for '%s'\n",
		      cur_instance->mod_name);
               printf("   but is used in other instance calls\n");
               printf("   for module '%s' in the netlist\n",
                      cur_instance->mod->name);
               printf("   ---> look inside module '%s' in the netlist file\n",
                cur_module->name);
               exit(1);
	      }
	    else
	      {
	       add_instance_parameter(cur_instance,cur_mod_parameter->name,
                  cur_mod_parameter->value,cur_mod_parameter->value_text,1);
	      }
	  }
        cur_mod_parameter = cur_mod_parameter->next;
       }
     // now check that all variables in instance parameter expressions
     // are defined as parameters in the module that calls the instance
     cur_inst_parameter = cur_instance->first_mod_parameter;
     while(cur_inst_parameter != NULL)
       {
        if (cur_inst_parameter->use_text_flag == 1)
	 {
          param_list->param_not_found = 0;  // don't abort on unknown variables
          eval_paren_expr(cur_inst_parameter->value_text,"invalid",NULL,
                          param_list);

          if (param_list->param_not_found == 1)
	    {
	     printf("error in 'check_instance_parameters': undefined\n");
             printf("   variable in instance parameter description\n");
             printf("   specifically, parameter '%s' in instance '%s'\n",
                   cur_inst_parameter->name,cur_instance->name);
             printf("   is defined using an unknown variable in the\n");
             printf("   expression: '%s'\n",
		    cur_inst_parameter->value_text);
             printf("   --->  look in module '%s' in the netlist file\n",
                    cur_module->name);
             exit(1);
	    }
	  param_list->param_not_found = -1; // return to abort behavior
	 }
	cur_inst_parameter = cur_inst_parameter->next;
       }
     cur_instance = cur_instance->next;
     }

  // remove default module and passed instance parameters from param_list
  param_list->main_par_length = num_parameters;

  cur_module = cur_module->next;
  }
}


void print_node_info(NODE_INFO *node_info)
{
printf("node_type = %s\n",node_info->node_type);

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



void connect_sue_instances_to_modules(MODULE *first_module)
{
MODULE *cur_module;
INSTANCE *cur_instance;
NODES *cur_node;

if (first_module == NULL)
   {
     printf("error in 'connect_sue_instances_to_module':  first_module is NULL\n");
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
         cur_node = cur_instance->mod->first_ext_node;
	 while (cur_node != NULL)
	   {
             add_sue_instance_node(cur_instance,cur_node);
	     cur_node = cur_node->next;
	   }
         cur_instance = cur_instance->next;
       }
     cur_module = cur_module->next;
   }
}

void connect_instances_to_modules(MODULE *first_module)
{
MODULE *cur_module;
INSTANCE *cur_instance;
NODES *cur_node;
int count;

if (first_module == NULL)
   {
     printf("error in 'connect_instances_to_modules':  first_module is NULL\n");
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
	   "sue_spice_netlister",filename);
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
       if (line_set->line_array[line_set->num_of_lines][i] == '\\')
          if (line_set->line_array[line_set->num_of_lines][i+1] == '\n' ||
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

int extract_code_lineset(FILE *in, LINE *line_set, char *filename, 
                         int *linecount)
{

line_set->num_of_lines = 1;
(*linecount)++;
if (fgets(line_set->line_array[0],LINESIZE-1,in) == NULL)
  {
   line_set->num_of_lines = 0;
   return(1);
  }

return(0);
}

void print_keyinfo_modules(MODULE *mod)
{
NODES *cur_node;
MODULE *cur_module;
INSTANCE *cur_instance;
MOD_PARAMETER *cur_mod_parameter;

cur_module = mod;
while (1)
   {
   printf("----------------------------------------------------------\n");
   printf("module name:  %s\n",cur_module->name);
   if (cur_module->spice_text != NULL)
      printf("    spice_text:  %s\n",cur_module->spice_text);
   cur_mod_parameter = cur_module->first_mod_parameter;
   while(cur_mod_parameter != NULL)
       {
       printf("   parameter '%s' = %s (type '%s')\n",
           cur_mod_parameter->name,cur_mod_parameter->value_text,
           cur_mod_parameter->type);
       cur_mod_parameter = cur_mod_parameter->next;
       if (cur_mod_parameter == NULL)
	   printf("\n");
       }

   if (cur_module->first_instance != NULL)
     {
      cur_instance = cur_module->first_instance;
      printf("  instances:\n");
      while(1)
         {
         printf("   name: %s   module: %s   origin: (%d,%d)\n",
             cur_instance->name,cur_instance->mod_name,
             cur_instance->origin_x, cur_instance->origin_y);

         cur_node = cur_instance->first_node;
         if (cur_node != NULL)
            printf("     instance nodes:\n");
         while(cur_node != NULL)
            {
             printf("       %s (%s, %s)\n",cur_node->name,cur_node->node_type,
                              cur_node->node_dir);
             cur_node = cur_node->next;
            }

         cur_mod_parameter = cur_instance->first_mod_parameter;
         printf("     instance parameters:\n");
	 while(cur_mod_parameter != NULL)
	   {
           print_mod_parameter(cur_mod_parameter);
           cur_mod_parameter = cur_mod_parameter->next;
	   }
         if (cur_instance->next == NULL)
     	    break;
         else
   	    cur_instance = cur_instance->next;
         }
      }

   cur_node = cur_module->first_ext_node;
   printf("  ext. nodes:\n");
   while(cur_node != NULL)
      {
      printf("       %s (%s, %s, %d, %d)\n",cur_node->name,cur_node->node_type,
                  cur_node->node_dir,cur_node->origin_x,cur_node->origin_y);
      cur_node = cur_node->next;
      }

   cur_node = cur_module->first_int_node;
   if (cur_node != NULL)
     {
      printf("  int. nodes:\n");
      while(cur_node != NULL)
        {
        printf("       %s (%s, %s)\n",cur_node->name,cur_node->node_type,
                              cur_node->node_dir);
        cur_node = cur_node->next;
        }
     }
   printf("  spice text:\n");
   if (cur_module->spice_text != NULL)
      printf("  %s\n",cur_module->spice_text);
   print_one_code(cur_module->code);
   printf("----------------------------------------------------------\n");
   if (cur_module->next == NULL)
      break;
   else
      cur_module = cur_module->next; 
   }
}

void print_verbose_modules(MODULE *mod)
{
NODES *cur_node;
MODULE *cur_module;
INSTANCE *cur_instance;
MOD_PARAMETER *cur_mod_parameter;

cur_module = mod;
while (1)
   {
   printf("module name:  %s\n",cur_module->name);

   cur_mod_parameter = cur_module->first_mod_parameter;
   while(cur_mod_parameter != NULL)
       {
       printf("%s=%5.3e ",cur_mod_parameter->name,cur_mod_parameter->value);
       cur_mod_parameter = cur_mod_parameter->next;
       if (cur_mod_parameter == NULL)
	   printf("\n");
       }

   if (cur_module->first_instance != NULL)
     {
      cur_instance = cur_module->first_instance;
      printf("  instances:\n");
      while(1)
         {
         printf("   %s %d %s\n",cur_instance->name,
             cur_instance->node_arg_number,cur_instance->mod_name);
         cur_mod_parameter = cur_instance->first_mod_parameter;
	 while(cur_mod_parameter != NULL)
	   {
          if (cur_mod_parameter == cur_instance->first_mod_parameter)
              printf("     ");
           print_mod_parameter(cur_mod_parameter);
           cur_mod_parameter = cur_mod_parameter->next;
           if (cur_mod_parameter == NULL)
	     printf("\n");
	   }
         if (cur_instance->next == NULL)
     	    break;
         else
   	    cur_instance = cur_instance->next;
         }
      }


   cur_node = cur_module->first_ext_node;
   printf("  ext. nodes:\n");
   while(cur_node != NULL)
     {
      printf("  %s:  node_type = %s",cur_node->name,cur_node->node_type);
      if (cur_node->first_instance != NULL)
	{
          cur_instance = cur_node->first_instance;
	  printf("          instances:\n");
	  while(1)
	    {
	      printf("           %s %d %s\n",cur_instance->name,
                       cur_instance->node_arg_number,cur_instance->mod_name);
             cur_mod_parameter = cur_instance->first_mod_parameter;
	     while(cur_mod_parameter != NULL)
	        {
                 if (cur_mod_parameter == cur_instance->first_mod_parameter)
                    printf("              ");
                 print_mod_parameter(cur_mod_parameter);
                 cur_mod_parameter = cur_mod_parameter->next;
                 if (cur_mod_parameter == NULL)
	             printf("\n");
	        }
              if (cur_instance->next == NULL)
		break;
              else
		cur_instance = cur_instance->next;
	    }
	}
      if (cur_node->next == NULL)
	break;
      else
        cur_node = cur_node->next;
     }

   cur_node = cur_module->first_int_node;
   printf("  int. nodes:\n");
   while(cur_node != NULL)
     {
      printf("  %s:  node_type = %s\n",cur_node->name, cur_node->node_type);

      if (cur_node->first_instance != NULL)
	{
          cur_instance = cur_node->first_instance;
	  printf("          instances:\n");
	  while(1)
	    {
	      printf("           %s %d %s\n",cur_instance->name,
                       cur_instance->node_arg_number,cur_instance->mod_name);
             cur_mod_parameter = cur_instance->first_mod_parameter;
	     while(cur_mod_parameter != NULL)
	        {
                 if (cur_mod_parameter == cur_instance->first_mod_parameter)
                    printf("              ");
                 print_mod_parameter(cur_mod_parameter);
                 cur_mod_parameter = cur_mod_parameter->next;
                 if (cur_mod_parameter == NULL)
	             printf("\n");
	        }
              if (cur_instance->next == NULL)
		break;
              else
		cur_instance = cur_instance->next;
	    }
	}
      if (cur_node->next == NULL)
	break;
      else
        cur_node = cur_node->next;
     }

   if (cur_module->next == NULL)
      break;
   else
      cur_module = cur_module->next; 
   }
}


void print_mod_parameter(MOD_PARAMETER *cur_mod_parameter)
{
if (cur_mod_parameter->use_text_flag == 1)
   printf("       %s=%s\n",cur_mod_parameter->name,
         cur_mod_parameter->value_text);
else
   printf("       %s=%5.3e\n",cur_mod_parameter->name,
          cur_mod_parameter->value);
}

void print_modules(MODULE *mod)
{
NODES *cur_node;
MODULE *cur_module;

cur_module = mod;
while (1)
   {
   printf("module name:  %s\n",cur_module->name);
   cur_node = cur_module->first_ext_node;
   printf("  ext. nodes:  ");
   while(cur_node->next != NULL)
     {
      printf("%s, ",cur_node->name);
      cur_node = cur_node->next;
     }
   printf("%s\n",cur_node->name);

   cur_node = cur_module->first_int_node;
   if (cur_node != NULL)
     {
      printf("  int. nodes:  ");
      while(cur_node->next != NULL)
        {
        printf("%s, ",cur_node->name);
        cur_node = cur_node->next;
        }
      printf("%s\n",cur_node->name);
     }
   if (cur_module->next == NULL)
      break;
   else
      cur_module = cur_module->next; 
   }
}

void account_for_orient(MODULE *first_module)
{
MODULE *cur_module;
INSTANCE *cur_instance;

cur_module = first_module;
while (cur_module != NULL)
   {
     cur_instance = cur_module->first_instance;
     while (cur_instance != NULL)
       {
	recalculate_instance_terminal_origins_due_to_orient(cur_instance);
	cur_instance = cur_instance->next;
       }
     cur_module = cur_module->next;
   }
}

void recalculate_instance_terminal_origins_due_to_orient(INSTANCE *cur_instance)
{
NODES *cur_inst_node, *cur_mod_node;

if (cur_instance == NULL)
   {
     printf("error in 'recalculate_terminal_origins_due_to_orient'\n");
     printf("  cur_instance is NULL\n");
     exit(1);
   }

//printf("orient = %d, %d, %d\n",cur_instance->orient_x,
//	cur_instance->orient_y, cur_instance->orient_r);
cur_inst_node = cur_instance->first_node;
if (cur_instance->mod == NULL)
   {
    printf("error in 'recalculate_instance_terminal_origins_due_to_orient'\n");
    printf("   instances have not been connected to modules yet\n");
    exit(1);
   }
cur_mod_node = cur_instance->mod->first_ext_node;
while (cur_inst_node != NULL && cur_mod_node != NULL)
   {
    if (cur_instance->orient_r == 1)
      {
	cur_inst_node->origin_x = -cur_mod_node->origin_y;
	cur_inst_node->origin_y = cur_mod_node->origin_x;
      }
    else if (cur_instance->orient_r == 2)
      {
	cur_inst_node->origin_x = -cur_mod_node->origin_x;
	cur_inst_node->origin_y = -cur_mod_node->origin_y;
      }
    else if (cur_instance->orient_r == 3)
      {
	cur_inst_node->origin_x = cur_mod_node->origin_y;
	cur_inst_node->origin_y = -cur_mod_node->origin_x;
      }
    else
      {
	cur_inst_node->origin_x = cur_mod_node->origin_x;
	cur_inst_node->origin_y = cur_mod_node->origin_y;
      }
    if (cur_instance->orient_x == 1)
       {
	cur_inst_node->origin_x = -cur_inst_node->origin_x;
       }
    if (cur_instance->orient_y == 1)
       {
	cur_inst_node->origin_y = -cur_inst_node->origin_y;
       }
    cur_inst_node = cur_inst_node->next;
    cur_mod_node = cur_mod_node->next;
   }

}

void extract_sue_instance_parameters(ARGUMENT *args,char *filename,
          LINE *line_set,
          int linecount, PARAMETER *param_list, INSTANCE *instance)
{
int i;

if (instance == NULL)
   {
   printf("error in 'extract_sue_instance_parameters':  input instance is NULL\n");
   exit(1);
   }

strncpy(instance->mod_name,args->arg_array[1],MAX_INSTANCE_NAME_LENGTH-1);

instance->first_mod_parameter = NULL; 
instance->first_node = NULL; 
instance->node_arg_number = -1;
strncpy(instance->name,"NULL",MAX_INSTANCE_NAME_LENGTH-1);
instance->origin_x = 0;
instance->origin_y = 0;
instance->orient_x = 0;
instance->orient_y = 0;
instance->orient_r = 0;
instance->next = NULL;


for (i = 2; i < args->num_of_args; i++)
  {
  if (strcmp(args->arg_array[i],"-name") == 0)
    {
     strncpy(instance->name,args->arg_array[i+1],MAX_INSTANCE_NAME_LENGTH-1);
     i++;
    }
  else if (strcmp(args->arg_array[i],"-origin") == 0)
    {
     instance->origin_x = atoi(args->arg_array[i+1]);
     instance->origin_y = atoi(args->arg_array[i+2]);
     i += 2;
    }
  else if (args->arg_array[i][0] == '-')
    {
      if (strcmp(args->arg_array[i],"-orient") == 0)
	{
	  if (strcmp(args->arg_array[i+1],"rx") == 0)
	     instance->orient_x = 1;
	  else if (strcmp(args->arg_array[i+1],"ry") == 0)
	     instance->orient_y = 1;
	  else if (strcmp(args->arg_array[i+1],"rxy") == 0)
	    {
	     instance->orient_x = 1;
	     instance->orient_y = 1;
	    }
	  else if (strcmp(args->arg_array[i+1],"r90") == 0)
	     instance->orient_r = 1;
	  else if (strcmp(args->arg_array[i+1],"r180") == 0)
	     instance->orient_r = 2;
	  else if (strcmp(args->arg_array[i+1],"r270") == 0)
	     instance->orient_r = 3;
	  else if (strcmp(args->arg_array[i+1],"r90x") == 0)
	    {
	     instance->orient_r = 1;
	     instance->orient_x = 1;
	    }
	  else if (strcmp(args->arg_array[i+1],"r180x") == 0)
	    {
	     instance->orient_r = 2;
	     instance->orient_x = 1;
	    }
	  else if (strcmp(args->arg_array[i+1],"r270x") == 0)
	    {
	     instance->orient_r = 3;
	     instance->orient_x = 1;
	    }
	  else if (strcmp(args->arg_array[i+1],"r90y") == 0)
	    {
	     instance->orient_r = 1;
	     instance->orient_y = 1;
	    }
	  else if (strcmp(args->arg_array[i+1],"r180y") == 0)
	    {
	     instance->orient_r = 2;
	     instance->orient_y = 1;
	    }
	  else if (strcmp(args->arg_array[i+1],"r270y") == 0)
	    {
	     instance->orient_r = 3;
	     instance->orient_y = 1;
	    }
	  else 
	      {
	       printf("error in 'extract_sue_instance_parameters':\n");
	       printf("    unrecognized -orient option\n");
	       printf("    in this case, it reads -orient %s\n",
		       args->arg_array[i+1]);
	       exit(1);
	      }
	}
      else if ((args->arg_array[i][1] < '0' || args->arg_array[i][1] > '9')
              && args->arg_array[i][1] != '.')
         add_instance_parameter(instance,&args->arg_array[i][1],0.0,
                                args->arg_array[i+1],1);
      i++;
    } 
  }
}


void extract_instance_parameters(ARGUMENT *args,char *filename,
          LINE *line_set,
          int linecount, PARAMETER *param_list, INSTANCE *instance)
{
int i,j,k;
char desc_name[MAX_ARG];
double desc_value;
int desc_flag;

if (instance == NULL)
   {
   printf("error in 'extract_sue_instance_parameters':  input instance is NULL\n");
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







MODULE *add_sue_module(MODULE *first_module, char *deck_filename, 
                       char *library_path, PARAMETER *param_list)
{
FILE *in;
int i,j,k,linecount,end_of_file_flag,notfound_global_flag=1;
LINE *line_set;
ARGUMENT *args;
MODULE *cur_module;
INSTANCE *dummy_instance;
int start_icon_description_flag, spice_text_description_flag;
MOD_PARAMETER *cur_mod_parameter;
int x1,x2,y1,y2;
NODES *cur_node;
char text_description[MAX_TEXT];

/////////////////  Read Spice Deck ///////////////////////
//////////////////////////////////////////////////////////

if ((in = fopen(deck_filename,"r")) == NULL)
  {
   printf("error in 'add_sue_module':  can't open sue file:  '%s'\n",deck_filename);
   exit(1);
 }

dummy_instance = init_sue_instance(" "," ",0,NULL,NULL,0,0);
line_set = init_line();
args = init_argument();
linecount = 0;
end_of_file_flag = 0;
cur_module = NULL;

if (first_module == NULL)
    {
     cur_module = init_sue_module("NULL",library_path);
     first_module = cur_module;
    }
else
    {
     cur_module = first_module;
     while(cur_module->next != NULL)
         cur_module = cur_module->next;
     cur_module->next = init_sue_module("NULL",library_path);
     cur_module = cur_module->next;
    }


start_icon_description_flag = 0;
while(end_of_file_flag == 0)
  {
   end_of_file_flag = extract_lineset(in,line_set,deck_filename,&linecount);

   //   send_line_set_to_std_output(line_set);
   //printf("-----------------\n");
   extract_arguments_lc(line_set,deck_filename,linecount,args);

   if (strcmp(args->arg_array[0],"proc") == 0)
     {
      if (args->num_of_args < 2)
          {
           printf("error in 'add_sue_module':  not enough args\n");
           exit(1);
          }
      if ((strncmp(args->arg_array[1],"schematic_",10) == 0 ||
	   strncmp(args->arg_array[1],"icon_",5) == 0))
	{
	  for (i = 0; args->arg_array[1][i] != '\0'; i++)
	    if (args->arg_array[1][i] == '_')
	      break;
          if (args->arg_array[1][i] != '_' || 
              args->arg_array[1][i+1] == '\0')
             {
              printf("error in 'add_sue_module':  name of module is messed up\n");
              printf("  in this case, name is '%s'\n",args->arg_array[1]);
              exit(1);
              }
          i++;
	  if (strcmp(cur_module->name,"NULL") == 0)
 	     strcpy(cur_module->name,&args->arg_array[1][i]);
	  else
	    {
	      if (strcmp(cur_module->name,&args->arg_array[1][i]) != 0)
		{
                cur_module->next = init_sue_module("NULL",library_path);
                cur_module = cur_module->next;
   	        strcpy(cur_module->name,&args->arg_array[1][i]);
	        start_icon_description_flag = 0;
		}
	    }
	}
      if (strncmp(args->arg_array[1],"icon_",5) == 0)
	{
	  cur_module->icon_exists_flag = 1;
	  start_icon_description_flag = 1;
	}
      else if (strncmp(args->arg_array[1],"schematic_",10) == 0)
 	  cur_module->schematic_exists_flag = 1;
     }

   if (start_icon_description_flag == 1)
     {
       if (strcmp(args->arg_array[0],"icon_term") == 0)
	 {
           cur_node = NULL;
	   for (i = 1; i < args->num_of_args; i++)
	     {
	      if (strcmp(args->arg_array[i],"-name") == 0)
		 {
                 cur_node = add_ext_node_sue(cur_module,args->arg_array[i+1]);
                 break;
		 }
	     }
           if (cur_node == NULL)
	     {
	       printf("error in 'add_sue_module':  unnamed terminal node\n");
	       printf("line reads:\n");
               send_line_set_to_std_output(line_set);
	       exit(1);
	     }
	   for (i = 1; i < args->num_of_args; i++)
	     {
	       if (strcmp(args->arg_array[i],"-origin") == 0)
                 {
		   cur_node->origin_x = atoi(args->arg_array[i+1]);
		   cur_node->origin_y = atoi(args->arg_array[i+2]);
		 }
	       else if (strcmp(args->arg_array[i],"-type") == 0)
		  strcpy(cur_node->node_dir,args->arg_array[i+1]);
	     } 
	 }
       else if (strcmp(args->arg_array[0],"icon_property") == 0)
	 {
	   cur_mod_parameter = NULL;
	   spice_text_description_flag = 0;
	   for (i = 1; i < args->num_of_args; i++)
	     {
	       if (strcmp(args->arg_array[i],"-type") == 0)
		 {
		   for (j = 1; j < args->num_of_args; j++)
		     if (strcmp(args->arg_array[j],"-name") == 0)
		       break;
                   if (j < args->num_of_args-1)
		     {
		      if (strcmp(args->arg_array[i+1],"fixed") == 0 &&
                          strcmp(args->arg_array[j+1],"spice") == 0)
			  {
			    spice_text_description_flag = 1;
			  }
		      else if (strcmp(args->arg_array[i+1],"user") == 0 &&
                               strcmp(args->arg_array[j+1],"name") != 0)
                          cur_mod_parameter = 
                           add_mod_parameter(cur_module,"NULL",0.0,"NULL",1);
		     }
                   break;
		 }
	     }
	   if (cur_mod_parameter != NULL)
	     {
 	      for (i = 1; i < args->num_of_args; i++)
	         {
		 if (strcmp(args->arg_array[i],"-name") == 0)
		    strcpy(cur_mod_parameter->name,args->arg_array[i+1]);
		 else if (strcmp(args->arg_array[i],"-default") == 0)
		    strcpy(cur_mod_parameter->value_text,args->arg_array[i+1]);
		 } 
	     }
	   else if (spice_text_description_flag == 1)
	     {
	       for (i = 1; i < args->num_of_args; i++)
		 {
		 if (strcmp(args->arg_array[i],"-text") == 0)
		   {
		     text_description[0] = '\0';
		     for (j = i+1; j < args->num_of_args &&
			    args->arg_array[j][0] != '-'; j++)
		       {
		         strcat(text_description,args->arg_array[j]);
		         strcat(text_description," ");
		       }
                     if ((cur_module->spice_text = 
                         (char *) malloc(sizeof(char[MAX_TEXT]))) == NULL)   
                         {   
                         printf("error in 'add_sue_module':  malloc call failed\n");   
                         printf("out of memory!\n");   
                         exit(1);   
                         }   
		     strncpy(cur_module->spice_text,text_description,MAX_TEXT-1);
		     break;
		   }
		 }
	     }
	 }
     }
   else
     {
       if (strcmp(args->arg_array[0],"make_wire") == 0)
	 {
	   x1 = atoi(args->arg_array[1]);
	   y1 = atoi(args->arg_array[2]);
	   x2 = atoi(args->arg_array[3]);
	   y2 = atoi(args->arg_array[4]);
	   add_wire_to_module(cur_module,x1,y1,x2,y2);
	 }
       else if (strcmp(args->arg_array[0],"make") == 0)
	 {
	   if (strcmp(args->arg_array[1],cur_module->name) != 0)
	     {
              extract_sue_instance_parameters(args,deck_filename,line_set,
                       linecount,param_list,dummy_instance);
              add_instance_to_mod(cur_module,dummy_instance);
	     }
	 }
     }
  }
   /* zip - stuff to add
          for (j = 0; j < dummy_instance->num_of_node_args; j++)
	    {
            dummy_instance->node_arg_number = j;
            add_int_node(cur_module,args->arg_array[j+1],dummy_instance);
	    }

            connect_instances_to_modules(first_module);
   */

free(dummy_instance);
free(line_set);
free(args);
fclose(in);

return(first_module);
}

void add_wire_to_module(MODULE *cur_module, int x1, int y1, int x2, int y2)
{
WIRE *cur_wire;

if (cur_module == NULL)
   {
     printf("error in 'add_wire_to_module':  cur_module is NULL\n");
     exit(1);
   }

if (cur_module->first_wire == NULL)
   {
     cur_module->first_wire = init_wire(x1,y1,x2,y2);
   }
else
   {
     cur_wire = cur_module->first_wire;
     while (cur_wire->next != NULL)
	cur_wire = cur_wire->next;
     cur_wire->next = init_wire(x1,y1,x2,y2);
   }
}

WIRE *init_wire(int x1, int y1, int x2, int y2)
{
WIRE *A;

if ((A = (WIRE *) malloc(sizeof(WIRE))) == NULL)   
   {   
   printf("error in 'init_wire':  malloc call failed\n");   
   printf("out of memory!\n");   
   exit(1);   
   }   

if (x1 < x2)
   {
    A->x1 = x1;
    A->x2 = x2;
   }
else
   {
    A->x1 = x2;
    A->x2 = x1;
   }

if (y1 < y2)
   {
   A->y1 = y1;
   A->y2 = y2;
   }
else
   {
   A->y1 = y2;
   A->y2 = y1;
   }

strcpy(A->node_name,"NULL");
A->next = NULL;

return(A);
}


char *retrieve_instance_parameter_val(INSTANCE *cur_instance, char *param_name)
{
MOD_PARAMETER *cur_mod_parameter,*cur_inst_parameter;
char temp[MAX_ARG];

if (cur_instance == NULL)
    {
      printf("error in 'retrieve_instance_parameter_val':\n");
      printf("  cur_instance is NULL!\n");
      exit(1);
    }

cur_mod_parameter = cur_instance->mod->first_mod_parameter;
while (cur_mod_parameter != NULL)
   {
    if (strcmp(cur_mod_parameter->name,param_name) == 0)
	break;
    cur_mod_parameter = cur_mod_parameter->next;
   }
cur_inst_parameter = cur_instance->first_mod_parameter;
while (cur_inst_parameter != NULL)
   {
    if (strcmp(cur_inst_parameter->name,param_name) == 0)
	break;
    cur_inst_parameter = cur_inst_parameter->next;
   }

if (cur_mod_parameter != NULL && cur_inst_parameter != NULL)
   {
    if (cur_inst_parameter->use_text_flag == 0)
       {
	if (strcmp(cur_mod_parameter->type,"int") == 0)
	   {
	    sprintf(cur_inst_parameter->value_text,"%d",
			 (int) floor(cur_inst_parameter->value + .5));
	   }
        else
	   {
	    sprintf(cur_inst_parameter->value_text,"%5.3e",
			       cur_inst_parameter->value);
           }
        }
    else
	{
	 if (strcmp(cur_mod_parameter->type,"int") == 0)
	   {
	    sprintf(temp,"(int) (%s)",cur_inst_parameter->value_text);
            strcpy(cur_inst_parameter->value_text,temp);
	   }
	}
    return(cur_inst_parameter->value_text);
   }
else
   {
    printf("error in 'retrieve_instance_parameter':  can't find\n");
    printf("  parameter '%s' associated with instance '%s'\n",
	param_name,cur_instance->name);
    exit(1);
   }
}



void print_one_code(CODE *cur_code)
{
CODE_ENTRY *cur_code_entry;

if (cur_code == NULL)
  {
   return;
  }

    printf("************ code for module: %s *************\n",cur_code->name);

    printf("parameters:\n");
    cur_code_entry = cur_code->parameters;
    while(cur_code_entry != NULL)
      {
       printf("   %s, %s\n",cur_code_entry->name,cur_code_entry->name_type);
       cur_code_entry = cur_code_entry->next;
      }

    printf("inputs:\n");
    cur_code_entry = cur_code->inputs;
    while(cur_code_entry != NULL)
      {
       printf("   %s, %s, %d\n",cur_code_entry->name,
	       cur_code_entry->name_type,cur_code_entry->sub_arg_number);
       cur_code_entry = cur_code_entry->next;
      }

    printf("outputs:\n");
    cur_code_entry = cur_code->outputs;
    while(cur_code_entry != NULL)
      {
       printf("   %s, %s, %d\n",cur_code_entry->name,
	       cur_code_entry->name_type,cur_code_entry->sub_arg_number);
       cur_code_entry = cur_code_entry->next;
      }

    printf("classes:\n");
    cur_code_entry = cur_code->declaration;
    while(cur_code_entry != NULL)
      {
       printf("%s",cur_code_entry->name);
       cur_code_entry = cur_code_entry->next;
      }

    printf("init:\n");
    cur_code_entry = cur_code->init;
    while(cur_code_entry != NULL)
      {
       printf("%s",cur_code_entry->name);
       cur_code_entry = cur_code_entry->next;
      }

    printf("code:\n");
    cur_code_entry = cur_code->body;
    while(cur_code_entry != NULL)
      {
       printf("%s",cur_code_entry->name);
       cur_code_entry = cur_code_entry->next;
      }
    printf("**************************************************\n");
}




MODULE *init_sue_module(char *name,char *library_path)
{   
MODULE *A;   
   
if ((A = (MODULE *) malloc(sizeof(MODULE))) == NULL)   
   {   
   printf("error in 'init_sue_module':  malloc call failed\n");   
   printf("out of memory!\n");   
   exit(1);   
  }

   
strncpy(A->name,name,MAX_MODULE_NAME_LENGTH-1);
strcpy(A->library_path,library_path);
A->code = NULL;
A->first_wire = NULL;
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
A->icon_exists_flag = 0;
A->schematic_exists_flag = 0;
A->active_sue_module_flag = 0;
A->spice_text = NULL;
return(A);   
}


NODES *init_node(char *name)
{   
NODES *A;   
   
if ((A = (NODES *) malloc(sizeof(NODES))) == NULL)   
   {   
   printf("error in 'init_node':  malloc call failed\n");   
   printf("out of memory!\n");   
   exit(1);   
  }   
strncpy(A->name,name,MAX_NODE_NAME_LENGTH-1);
A->first_instance = NULL;
strcpy(A->node_type,"NULL");
strcpy(A->node_dir,"NULL");
A->is_global_flag = 0;
A->global_value = 0.0;
A->origin_x = 0;
A->origin_y = 0;
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


MOD_PARAMETER *add_mod_parameter(MODULE *cur_mod, char *name, double value,
                            char *value_text, int use_text_flag)
{
MOD_PARAMETER *cur_mod_parameter;

if (cur_mod->first_mod_parameter == NULL)
  {
   cur_mod->first_mod_parameter = init_mod_parameter(name,value,
                value_text,use_text_flag);
   return(cur_mod->first_mod_parameter);
  }
else
  {
   cur_mod_parameter = cur_mod->first_mod_parameter;
   while(cur_mod_parameter->next != NULL)
     cur_mod_parameter = cur_mod_parameter->next;
   cur_mod_parameter->next = init_mod_parameter(name,value,value_text,
                  use_text_flag);
   return(cur_mod_parameter->next);
  }
}


MOD_PARAMETER *init_mod_parameter(char *name, double value, char *value_text,
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

INPUT *init_input(char *name, TIMING *timing)
{   
INPUT *A;   
   
if ((A = (INPUT *) malloc(sizeof(INPUT))) == NULL)   
   {   
   printf("error in 'init_input':  malloc call failed\n");   
   printf("out of memory!\n");   
   exit(1);   
  }

strncpy(A->name,name,MAX_ARG-1);
strcpy(A->filename,"NULL");
strcpy(A->type,"NULL");
A->timing = timing;
A->next = NULL;
return(A);   
}   




TIMING *init_timing()
{   
TIMING *A;   
   
if ((A = (TIMING *) malloc(sizeof(TIMING))) == NULL)   
   {   
   printf("error in 'init_timing':  malloc call failed\n");   
   printf("out of memory!\n");   
   exit(1);   
  }   
A->delay = 0.0;
A->rise_time = 0.0;
A->period = 0.0;
A->vlow = 0.0;
A->vhigh = 0.0; 
strcpy(A->delay_text,"NULL");
strcpy(A->rise_time_text,"NULL");
strcpy(A->period_text,"NULL");
strcpy(A->vlow_text,"NULL");
strcpy(A->vhigh_text,"NULL"); 
A->next = NULL;
return(A);   
}   


INSTANCE *init_instance(char *name, char *mod_name, int node_arg_number,
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

INSTANCE *init_sue_instance(char *name, char *mod_name, int node_arg_number,
           MOD_PARAMETER *first_mod_parameter, NODES *first_node,
           int origin_x, int origin_y)
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
A->origin_x = origin_x;
A->origin_y = origin_y;
A->orient_x = 0;
A->orient_y = 0;
A->orient_r = 0;
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
  {
   cur_mod->first_instance = 
        init_sue_instance(inst->name,inst->mod_name,inst->node_arg_number,
             inst->first_mod_parameter,inst->first_node,
             inst->origin_x, inst->origin_y);
   cur_mod->first_instance->orient_x = inst->orient_x;
   cur_mod->first_instance->orient_y = inst->orient_y;
   cur_mod->first_instance->orient_r = inst->orient_r;
  }
else
  {
  cur_instance = cur_mod->first_instance;
  while(cur_instance->next != NULL)
     cur_instance = cur_instance->next;
  cur_instance->next = 
        init_sue_instance(inst->name,inst->mod_name,inst->node_arg_number,
             inst->first_mod_parameter,inst->first_node,
             inst->origin_x, inst->origin_y);
   cur_instance = cur_instance->next;
   cur_instance->orient_x = inst->orient_x;
   cur_instance->orient_y = inst->orient_y;
   cur_instance->orient_r = inst->orient_r;
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


void add_param_value(char *name, double value, PARAMETER *par_list)
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

void pass_mod_info_to_instances(MODULE *first_module)
{
MODULE *cur_module;
INSTANCE *cur_instance;
NODES *cur_inst_node, *cur_mod_node;

cur_module = first_module;
while(cur_module != NULL)
  {
   cur_instance = cur_module->first_instance;
   while (cur_instance != NULL)
     {
      cur_inst_node = cur_instance->first_node;
      cur_mod_node = cur_instance->mod->first_ext_node;
      while (cur_inst_node != NULL && cur_mod_node != NULL)
	{
         if (strcmp(cur_mod_node->node_type,"NULL") == 0 ||
             strcmp(cur_mod_node->node_dir,"NULL") == 0)
	   {
	     printf("error in 'pass_mod_info_to_instance': could not\n");
             printf("  determine type and direction of node\n");
             printf("  '%s' in module '%s' in the netlist\n",cur_mod_node,
		    cur_instance->mod->name);
             printf("  this module is instantiated in module '%s'\n",
		    cur_module->name);
             printf("  as instance '%s'\n",cur_instance->name);
             printf("  ---> need to link code to module '%s'\n",
                    cur_instance->mod->name);
             exit(1);
	   }
	 strcpy(cur_inst_node->node_type,cur_mod_node->node_type);
	 strcpy(cur_inst_node->node_dir,cur_mod_node->node_dir);
	 cur_inst_node = cur_inst_node->next;
         cur_mod_node = cur_mod_node->next;
	}
      cur_instance = cur_instance->next;
     }
   cur_module = cur_module->next;
  }
}

void extract_int_mod_nodes(MODULE *first_module, 
                           MAIN_PARAMETERS *main_parameters)
{
MODULE *cur_module;
INSTANCE *cur_instance;
NODES *cur_inst_node, *cur_mod_node, *cur_mod_node2, *prev_mod_node;
NODES *inst_to_mod_node;
MOD_PARAMETER *cur_global_node;
char node_type[MAX_ARG], node_dir[MAX_ARG];
int i,node_count;


if (first_module == NULL)
   {
     printf("error in 'extract_int_mod_nodes':  first_module is NULL!\n");
     exit(1);
   }
if (main_parameters == NULL)
   {
     printf("error in 'extract_int_mod_nodes':  main_parameters is NULL!\n");
     exit(1);
   }

cur_module = first_module;
while(cur_module != NULL)
  {
   cur_mod_node = cur_module->first_int_node;

   prev_mod_node = cur_mod_node;
   strcpy(node_type,"NULL");
   strcpy(node_dir,"NULL");
   while (cur_mod_node != NULL)
     {
       //  printf("node = %s, dir = %s\n",cur_mod_node->name,cur_mod_node->node_dir);
      cur_instance = cur_module->first_instance;
      while (cur_instance != NULL)
        {
        cur_inst_node = cur_instance->first_node;
        while (cur_inst_node != NULL)
 	   {
           if (strcmp(cur_mod_node->name,cur_inst_node->name) == 0)
	     {
	      if ((strcmp(cur_inst_node->node_dir,"NULL") == 0) ||
                  (strcmp(cur_inst_node->node_type,"NULL") == 0))
		 {
		  printf("error in 'extract_int_mod_nodes':  could not\n");
                  printf("  determine type and direction of node\n");
                  printf("  '%s' in instance '%s'\n",cur_inst_node->name,
			     cur_instance->name);
                  exit(1);
		 }
              if (strcmp(cur_mod_node->node_dir,"NULL") == 0 ||
                  strcmp(cur_mod_node->node_dir,"input") == 0)
		 {
                  strcpy(cur_mod_node->node_dir,cur_inst_node->node_dir);
		 }
              else if (strcmp(cur_mod_node->node_dir,"output") == 0 &&
                       strcmp(cur_inst_node->node_dir,"output") == 0)
		{
	         printf("error in 'extract_int_mod_nodes':  all internal\n");
                 printf("  nodes must be connected to no more than one output\n");
                 printf("  of an instance (otherwise two outputs fight)\n");
                 printf("     In this case, in module '%s', internal node '%s'\n",
                             cur_module->name,cur_mod_node->name);
                 printf("     is connected to more than one instance output\n");
		  printf("-->list of instance connections for node '%s'\n",
                            cur_mod_node->name);
                  printf("   in module '%s':\n",cur_module->name);
		  cur_instance = cur_module->first_instance;
		  while (cur_instance != NULL)
		    {
                     cur_inst_node = cur_instance->first_node;
		     node_count = 0;
                     while (cur_inst_node != NULL)
		        {
			 if (strcmp(cur_inst_node->name,cur_mod_node->name)
			      == 0)
	         	  {
                          inst_to_mod_node = cur_instance->mod->first_ext_node;
                          for (i = 0; i < node_count; i++)
		             {
		             if (inst_to_mod_node == NULL)
		                {
		                 printf("error in 'extract_int_mod_nodes': node_count\n");
                                 printf("  exceeded!\n");
                                 exit(1);
		                }
		             inst_to_mod_node = inst_to_mod_node->next;
		             } 
			    printf("   instance '%s':  node '%s' in module '%s' has direction '%s'\n",
			           cur_instance->name,
				   inst_to_mod_node->name,
				   cur_instance->mod->name,
                                   inst_to_mod_node->node_dir);
			   }
		         cur_inst_node = cur_inst_node->next;
			 node_count++;
		        }
		     cur_instance = cur_instance->next;
		    }
                 exit(1);
		}
              if (strcmp(cur_mod_node->node_type,"NULL") == 0)
                  strcpy(cur_mod_node->node_type,cur_inst_node->node_type);
              else if (strcmp(cur_mod_node->node_type,
                              cur_inst_node->node_type) != 0)
                 {
		  printf("error in 'extract_int_mod_nodes':  conflicting\n");
                  printf("  node types between instances.  In this\n");
                  printf("  case, in module '%s', node '%s'\n",
                             cur_module->name,cur_mod_node->name);
                  printf("     has conflicting types '%s' and '%s'\n",
			  cur_mod_node->node_type,cur_inst_node->node_type);
		  printf("-->list of instance connections for node '%s'\n",
                            cur_mod_node->name);
                  printf("   in module '%s':\n",cur_module->name);
		  cur_instance = cur_module->first_instance;
		  while (cur_instance != NULL)
		    {
                     cur_inst_node = cur_instance->first_node;
		     node_count = 0;
                     while (cur_inst_node != NULL)
		        {
			 if (strcmp(cur_inst_node->name,cur_mod_node->name)
			      == 0)
	         	  {
                          inst_to_mod_node = cur_instance->mod->first_ext_node;
                          for (i = 0; i < node_count; i++)
		             {
		             if (inst_to_mod_node == NULL)
		                {
		                 printf("error in 'extract_int_mod_nodes': node_count\n");
                                 printf("  exceeded!\n");
                                 exit(1);
		                }
		             inst_to_mod_node = inst_to_mod_node->next;
		             } 
			    printf("   instance '%s':  node '%s' in module '%s' is type '%s'\n",
			           cur_instance->name,
				   inst_to_mod_node->name,
				   cur_instance->mod->name,
                                   inst_to_mod_node->node_type);
			   }
		         cur_inst_node = cur_inst_node->next;
			 node_count++;
		        }
		     cur_instance = cur_instance->next;
		    }
                  exit(1);
		 }
	     }
	   cur_inst_node = cur_inst_node->next;
	   }
        cur_instance = cur_instance->next;
        }
      if (strcmp(cur_mod_node->node_dir,"output") != 0)
	{
	// determine if the node is global (result is NULL if not global)
	// will use this result below
	cur_global_node = main_parameters->first_global_node;
	while (cur_global_node != NULL)
	  {
	    if (strcmp(cur_global_node->name,cur_mod_node->name) == 0)
	      {
		//  printf("node '%s' is global (value = %5.3f)!\n",
                //     cur_mod_node->name,cur_global_node->value);
	       break;
	      }
	    cur_global_node = cur_global_node->next;
	  }

	//  old method:  switch node from internal input node to an 
        //          external input node 
        //          if top module was not created using a .subckt command
        //          or the node is global
        // new method:  switch node from an internal input node to an
        //          external input node for top module regardless of whether
        //          top module was created using a .subckt command
	//	if (cur_module->top_module_not_subckt_flag == 1 &&
        //    cur_global_node == NULL)
	if (cur_module->top_module_flag == 1 && cur_global_node == NULL)
	   {
	    if (cur_module->first_ext_node == NULL)
	      {
		cur_module->first_ext_node = cur_mod_node;
                cur_mod_node2 = cur_module->first_ext_node;
                if (cur_mod_node == cur_module->first_int_node)
		   cur_module->first_int_node = cur_mod_node->next;
                else
		   prev_mod_node->next = cur_mod_node->next;
                cur_mod_node2->next = NULL;
	      }
	    else
	      {
	        cur_mod_node2->next = cur_mod_node;
                cur_mod_node2 = cur_mod_node2->next;
                if (cur_mod_node == cur_module->first_int_node)
		   cur_module->first_int_node = cur_mod_node->next;
                else
		   prev_mod_node->next = cur_mod_node->next;
                cur_mod_node2->next = NULL;
	      }
	    //	    print_keyinfo_modules(first_module); exit(1);
	    // start the process over again
            cur_mod_node = cur_module->first_int_node;
            while (cur_mod_node != NULL)
	      {
		strcpy(cur_mod_node->node_dir,"NULL");
		strcpy(cur_mod_node->node_type,"NULL");
		cur_mod_node = cur_mod_node->next;
	      }
            cur_mod_node = cur_module->first_int_node;
	    prev_mod_node = cur_mod_node;
            strcpy(node_type,"NULL");
            strcpy(node_dir,"NULL");
            continue;
	   }
        else if (cur_global_node == NULL)
           {
	    printf("error in 'extract_int_mod_nodes':  all internal\n");
            printf("  nodes must be connected to the output of one\n");
            printf("  instance (otherwise the node is floating)\n");
            printf("     In this case, in module '%s', internal node '%s'\n",
                             cur_module->name,cur_mod_node->name);
            printf("     is not connected to any instance output (val = %s)\n",
			  cur_mod_node->node_dir);
	    printf("---> Note:  this check will be ignored for this node\n");
	    printf("     if you declare the node '%s' global using the\n",
                    cur_mod_node->name);
	    printf("     global_nodes: command in the main parameter file\n");
            exit(1);
	    }
        else if (cur_global_node != NULL)
	    {
	     cur_mod_node->is_global_flag = 1;
	     cur_mod_node->global_value = cur_global_node->value;
	    }
	 }
      prev_mod_node = cur_mod_node;
      cur_mod_node = cur_mod_node->next;
     }
   cur_module = cur_module->next;
  }
}

void check_dir_for_ext_mod_nodes(MODULE *first_module)
{
MODULE *cur_module;
INSTANCE *cur_instance;
NODES *cur_mod_node,*cur_inst_node;
char node_dir[MAX_ARG];

cur_module = first_module;
while (cur_module != NULL)
  {
   cur_mod_node = cur_module->first_ext_node;
   while (cur_mod_node != NULL)
      {
      strcpy(node_dir,"input");
      cur_instance = cur_module->first_instance;
      while (cur_instance != NULL)
         {
	   cur_inst_node = cur_instance->first_node;
	   while (cur_inst_node != NULL)
	     {
	      if (strcmp(cur_inst_node->name,cur_mod_node->name) == 0)
		{
		  if (strcmp(cur_inst_node->node_dir,"output") == 0)
		    {
		      if (strcmp(node_dir,"output") == 0)
			 {
			  printf("error in 'check_dir_for_ext_mod_nodes':\n");
                          printf("  nodes must be connected to no more than one output\n");
                          printf("  of an instance (otherwise two outputs fight)\n");
                          printf("     In this case, in module '%s', internal node '%s'\n",
                             cur_module->name,cur_mod_node->name);
                          printf("     is connected to more than one instance output\n");
                          exit(1);
			 }
                      else
			  strcpy(node_dir,"output");
		    }
		}
	      cur_inst_node = cur_inst_node->next;
	     }
          cur_instance = cur_instance->next;
         }
      cur_mod_node = cur_mod_node->next;
      }
   cur_module = cur_module->next;
  }
}

void extract_ext_mod_nodes(MODULE *cur_module)
{
NODES *cur_mod_node, *cur_inst_node, *inst_to_mod_node;
INSTANCE *cur_instance;
char node_type[MAX_ARG], node_dir[MAX_ARG];
int node_count,i;

cur_mod_node = cur_module->first_ext_node;
while (cur_mod_node != NULL)
   {
    strcpy(node_type,"NULL");
    strcpy(node_dir,"NULL");
    cur_instance = cur_module->first_instance;
    while (cur_instance != NULL)
      {
       cur_inst_node = cur_instance->first_node;
       node_count = 0;
       while (cur_inst_node != NULL)
	 {
	   if (strcmp(cur_inst_node->name,cur_mod_node->name) == 0)
	     {
               inst_to_mod_node = cur_instance->mod->first_ext_node;
               for (i = 0; i < node_count; i++)
		 {
		  if (inst_to_mod_node == NULL)
		     {
		       printf("error in 'extract_ext_mod_nodes': node_count\n");
                       printf("  exceeded!\n");
                       exit(1);
		     }
		  inst_to_mod_node = inst_to_mod_node->next;
		 }
	       if ((strcmp(inst_to_mod_node->node_dir,"NULL") == 0) ||
                   (strcmp(inst_to_mod_node->node_type,"NULL") == 0))
		 {
		  extract_ext_mod_nodes(cur_instance->mod);
 	          if ((strcmp(inst_to_mod_node->node_dir,"NULL") == 0) ||
		      (strcmp(inst_to_mod_node->node_type,"NULL") == 0))
		    {
		      printf("error in 'extract_ext_mod_nodes':  could not\n");
                      printf("  determine type and direction of node\n");
                      printf("  '%s' in module '%s' in the netlist\n",
                           inst_to_mod_node->name,cur_instance->mod->name);
                      printf("  this module is instantiated in module '%s'\n",
		           cur_module->name);
                      printf("  as instance '%s'\n",cur_instance->name);
                      printf("  ---> need to link code to module '%s'\n",
                            cur_instance->mod->name);
                      exit(1);
		    }
		 }
               if (strcmp(cur_mod_node->node_dir,"NULL") == 0 ||
                   strcmp(cur_mod_node->node_dir,"input") == 0)
		   {
                   strcpy(cur_mod_node->node_dir,inst_to_mod_node->node_dir);
		   }
               if (strcmp(cur_mod_node->node_type,"NULL") == 0)
                   strcpy(cur_mod_node->node_type,inst_to_mod_node->node_type);
               else if (strcmp(cur_mod_node->node_type,
                                  inst_to_mod_node->node_type) != 0)
                  {
		   printf("error in 'extract_ext_mod_nodes':  conflicting\n");
                   printf("  node types between instances.  In this\n");
                   printf("  case, in module '%s', node '%s'\n",
                             cur_module->name,cur_mod_node->name);
                   printf("     has conflicting types '%s' and '%s'\n",
			  cur_mod_node->node_type,inst_to_mod_node->node_type);
		  printf("-->list of instance connections for node '%s'\n",
                            cur_mod_node->name);
                  printf("   in module '%s':\n",cur_module->name);
		  cur_instance = cur_module->first_instance;
		  while (cur_instance != NULL)
		    {
                     cur_inst_node = cur_instance->first_node;
		     node_count = 0;
                     while (cur_inst_node != NULL)
		        {
			 if (strcmp(cur_inst_node->name,cur_mod_node->name)
			      == 0)
	         	  {
                          inst_to_mod_node = cur_instance->mod->first_ext_node;
                          for (i = 0; i < node_count; i++)
		             {
		             if (inst_to_mod_node == NULL)
		                {
		                 printf("error in 'extract_ext_mod_nodes': node_count\n");
                                 printf("  exceeded!\n");
                                 exit(1);
		                }
		             inst_to_mod_node = inst_to_mod_node->next;
		             } 
			    printf("   instance '%s':  node '%s' in module '%s' is type '%s'\n",
			           cur_instance->name,
				   inst_to_mod_node->name,
				   cur_instance->mod->name,
                                   inst_to_mod_node->node_type);
			   }
		         cur_inst_node = cur_inst_node->next;
			 node_count++;
		        }
		     cur_instance = cur_instance->next;
		    }
                   exit(1);
		  }
	     }
	  cur_inst_node = cur_inst_node->next;
          node_count++;
	 }
       cur_instance = cur_instance->next;
      }
   if (((strcmp(cur_mod_node->node_dir,"NULL") == 0) ||
	(strcmp(cur_mod_node->node_type,"NULL") == 0)) &&
         cur_module->first_instance != NULL)
      {
       printf("error in 'extract_ext_mod_nodes':  could not\n");
       printf("  determine type and direction of node\n");
       printf("  '%s' in module '%s'\n",cur_mod_node->name,
			     cur_module->name);
       exit(1);
      }
    cur_mod_node = cur_mod_node->next;
   }
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

void add_sue_instance_node(INSTANCE *cur_instance, NODES *cur_mod_node)
{
NODES *cur_node;

if (cur_instance->first_node == NULL)
  {
   cur_instance->first_node = init_node("NULL");
   cur_node = cur_instance->first_node;
  }
else
   {
   cur_node = cur_instance->first_node;
   while(cur_node->next != NULL)
      cur_node = cur_node->next;
   cur_node->next = init_node("NULL");
   cur_node = cur_node->next;
   }
strcpy(cur_node->node_type,cur_mod_node->node_type);
strcpy(cur_node->node_dir,cur_mod_node->node_dir);
cur_node->is_global_flag = cur_mod_node->is_global_flag;
cur_node->global_value = cur_mod_node->global_value;
cur_node->origin_x = cur_mod_node->origin_x;
cur_node->origin_y = cur_mod_node->origin_y;


if (is_node_naming_object(cur_instance->mod_name))
     {
      strcpy(cur_node->name,cur_instance->name);
     }
}

int is_node_naming_object(char *name)
{
if (strcmp(name,"input") == 0 || strcmp(name,"output") == 0 ||
    strcmp(name,"inout") == 0 || strcmp(name,"global") == 0 ||
    strcmp(name,"name_net") == 0 || strcmp(name,"name_net_s") == 0)
     {
     return(1);
     }
else
     {
     return(0);
     }
}

// note:  must pass in valid pointer for name
NODES *add_ext_node_sue(MODULE *cur_module, char *name)
{
NODES *cur_node;
register int i;

if (cur_module == NULL)
  {
    printf("error in 'add_ext_node_sue':  cur_module is NULL!\n");
    exit(1);
  }
if (cur_module->first_ext_node == NULL)
  {
   cur_module->first_ext_node = init_node(name);
   return(cur_module->first_ext_node);
  }
else
   {
   cur_node = cur_module->first_ext_node;
   while(cur_node->next != NULL)
      cur_node = cur_node->next;
   cur_node->next = init_node(name);
   return(cur_node->next);
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

line_set_count = 0;
inpline = line_set->line_array[line_set_count];
/* in SunOS gcc, blank lines (i.e. lines only containing '\n')
   are appended to the beginning of nonempty lines */
for (i = 0 ; inpline[i] == ' ' || inpline[i] == '\n'; i++);
for (args->num_of_args = 0; args->num_of_args < MAX_NUM_ARGS; args->num_of_args++)
  {
   for (j = 0; inpline[j] != '\0'; j++)
      if (inpline[j] == '{' || inpline[j] == '}')
	 inpline[j] = ' '; 
   for (    ; inpline[i] == ' '; i++);
   for (j = 0; inpline[i] != ' ' && inpline[i] != '\n' && 
               inpline[i] != '\0' && inpline[i] != '\t' &&
               inpline[i] != 0x0d; i++,j++)
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

return(args->num_of_args);
}



int extract_arguments(LINE *line_set, char *filename, int linecount, ARGUMENT *args)
{
char *inpline;
int i,j,line_set_count;

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
      char *filename, LINE *line_set,PARAMETER *param_list,double *desc_value)
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


double eval_paren_expr(char *input_in, char *file_name, LINE *line_set, 
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
       if (input[i+1] == '-')
           input[i+1] = '!';
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


double eval_no_paren_expr(char *input, char *file_name, LINE *line_set,
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


double convert_string(char *input, char *filename, LINE *line_set, 
                      int *error_flag)
{
int i,dec_place;
double string_value,char_value,power_of_ten,units_value;

units_value = 1.0;
*error_flag = 0;
for (i = 0; input[i] != '\0'; i++)
  {
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
	     case 'A':
	     case 'a':
	       units_value = 1.0e-18;
	       break;
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
	     default:
	       printf("error in 'convert_string':  units value '%s'\n",&input[i]);
	       printf("   not recognized in file '%s'\n",filename);
	       //   printf("       notation must be with 'e' or 'E'\n");
	       //   printf("       example:  1e9, 3.7e-9, 7.1E16, 2.3e-30\n");
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
double convert_exponent(char *input, char *filename, LINE *line_set, char *full_string)
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
