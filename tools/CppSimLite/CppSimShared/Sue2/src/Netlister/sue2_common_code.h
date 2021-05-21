/**************************************************************************
 * FILE : sue2_common_code.h
 * 
 * AUTHOR : Mike Perrott
 * 
 * DESCRIPTION : routines for doing netlisting, etc. of Sue2 schematics.
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
#define WINDOWS 0
#define LINUX 1
#define OPERATING_SYSTEM WINDOWS // 0: Windows, 1: Linux
//#define OPERATING_SYSTEM LINUX // 0: Windows, 1: Linux
//#include <ctype.h>
//#include <stdio.h>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#if OPERATING_SYSTEM==WINDOWS
   #include <io.h>
#endif

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

struct BUS_NODE
  {
  char node_name[MAX_NODE_NAME_LENGTH];
  struct BUS_NODE *next;
};
typedef struct BUS_NODE BUS_NODE;

struct NODES
  {
  char name[MAX_NODE_NAME_LENGTH];
  char mod_node_name[MAX_NODE_NAME_LENGTH];
  INSTANCE *first_instance;
  char node_type[MAX_ARG];
  char node_dir[MAX_ARG];
  int is_global_flag;
  int origin_x, origin_y;
  double global_value;
  BUS_NODE *first_bus_node;
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
  int wire_group, name_is_valid_flag;
  char node_name[MAX_NODE_NAME_LENGTH];
  char bus_node_name[MAX_NODE_NAME_LENGTH];
  INSTANCE *connecting_instance;
  NODES *connecting_instance_terminal;
  struct WIRE *next;
};
typedef struct WIRE WIRE;

void create_bus_nodes_for_modules_and_instances(MODULE *first_module);
void add_wire_to_module(MODULE *cur_module, int x1, int y1, int x2, int y2);
WIRE *init_wire(int x1, int y1, int x2, int y2);
BUS_NODE *init_bus_node(const char *node_name);
BUS_NODE *evaluate_bus_nodes(const char *node_name);
void print_bus_nodes(BUS_NODE *first_bus_node);
int compare_wire_or_term_name(const char *wire_or_term_name,const char *name);
int check_if_bus_or_bundle(const char *node_name);
int get_first_bus_val(const char *node_name);
int get_last_bus_val(const char *node_name);
int get_norm_first_bus_val(const char *node_name);
int get_norm_last_bus_val(const char *node_name);
int get_bundle_length(const char *node_name);
int get_bus_length(const char *node_name);
void check_wire_groups(MODULE *first_module);
void remove_intermediate_wires(MODULE *first_module);
void remove_unnamed_wire_prefix(MODULE *first_module);
void transfer_bus_node_names(MODULE *first_module);
int check_if_bus_signal(const char *node_name);
void determine_base_name(const char *sig_name, char *base_name);


struct MODULE
  {
  char name[MAX_MODULE_NAME_LENGTH];
  char old_name[MAX_MODULE_NAME_LENGTH];
  char full_filename[MAX_ARG];
  char library_path[MAX_ARG];
  int shared_library_flag;
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
  char *spice_module_include_text;
  char *spice_module_instance_text;
  struct MODULE *next;
};
typedef struct MODULE MODULE;


int makedir(const char *dir_name);
void retrieve_instance_node_name(INSTANCE *cur_instance, const char *mod_node_name,
                                 char *output);
void retrieve_instance_parameter_value(INSTANCE *cur_instance,
					char *instance_parameter_name,
				       char *output);
void extract_primitive_string(INSTANCE *cur_instance, char *out_string);
void print_hspice_netlist(FILE *fp, MODULE *first_module,
                       int print_top_subckt_flag);
void print_cppsim_netlist(FILE *fp, MODULE *first_module,
                       int print_top_subckt_flag);
void connect_instance_parameters_to_modules(MODULE *first_module);
INPUT *init_input(const char *name, TIMING *timing);
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
void add_instance_node(INSTANCE *cur_instance, const char *name);
void print_keyinfo_modules(MODULE *mod);
MODULE *grab_top_module(MODULE *first_module);
void print_node_info(NODE_INFO *node_info);
void add_param_value(const char *name, double value, PARAMETER *par_list);
void zero_node_info(NODE_INFO *node_info);
NODE_INFO *init_node_info();
void connect_instances_to_modules(MODULE *first_module);
MODULE *grab_module(MODULE *first_module, const char *name);
void print_mod_parameter(MOD_PARAMETER *cur_mod_parameter);
int extract_lineset(FILE *in, LINE *line_set, const char *filename, int *linecount);
int extract_code_lineset(FILE *in, LINE *line_set, const char *filename, 
                         int *linecount);
MOD_PARAMETER *add_mod_parameter(MODULE *cur_mod, const char *name, double value,
		       const char *value_text, int use_text_flag);
void add_instance_parameter(INSTANCE *cur_instance, const char *name, double value,
			    char *value_text, int use_text_flag);
MOD_PARAMETER *init_mod_parameter(const char *name, double value, const char *value_text,
				  int use_text_flag);
void add_instance_to_mod(MODULE *cur_mod, INSTANCE *inst);
void print_verbose_modules(MODULE *mod);
void print_modules(MODULE *mod);
INSTANCE *init_instance(const char *name, const char *mod_name, int node_arg_number,
		 MOD_PARAMETER *first_mod_parameter, NODES *first_node);
INSTANCE *init_sue_instance(const char *name, const char *mod_name, int node_arg_number,
		 MOD_PARAMETER *first_mod_parameter, NODES *first_node,
                 int origin_x, int origin_y);
MODULE *init_sue_module(const char *name, const char *library_path);
NODES *init_node(const char *name);
void add_instance_to_node(NODES *cur_node, INSTANCE *inst);
MODULE *add_sue_module(MODULE *first_module, const char *deck_filename, 
                       char *library_path, PARAMETER *param_list,
                       int shared_library_flag, const char *control_string);
MODULE *add_sue_module(MODULE *first_module, const char *deck_filename, 
                       char *library_path, PARAMETER *param_list,
                       int shared_library_flag);
NODES *add_ext_node_sue(MODULE *cur_module, const char *name);
void add_int_node(MODULE *cur_module, const char *name, INSTANCE *instance);
NODES *grab_node(MODULE *cur_module, const char *name);

int extract_arguments_lc(LINE *line_set, const char *filename, int linecount, ARGUMENT *args);
void extract_sue_instance_parameters(ARGUMENT *args,const char *filename,
     LINE *line_set,int linecount, PARAMETER *param_list, INSTANCE *instance);
void extract_instance_parameters(ARGUMENT *args,const char *filename,
     LINE *line_set,int linecount, PARAMETER *param_list, INSTANCE *instance);
void send_line_set_to_output(LINE *line_set, FILE *out);
void send_line_set_to_std_output(LINE *line_set);
int extract_arguments(LINE *line_set, const char *filename, int linecount, ARGUMENT *args);
int extract_par_arguments(LINE *line_set, int line_number, const char *filename, 
			  ARGUMENT *args);
int extract_descriptor(ARGUMENT *args, int arg_num, char *desc_name, 
     const char *filename, LINE *line_set,PARAMETER *param_list,double *desc_value);
int extract_text_descriptor(ARGUMENT *args, int arg_num, char *desc_name, 
			    char *desc_value);
double eval_paren_expr(const char *input_in, const char *file_name,LINE *line_set, PARAMETER *par_list);
double eval_no_paren_expr(const char *input, const char *file_name, LINE *line_set, PARAMETER *par_list);
double convert_string(char *input, const char *filename, LINE *line_set, int *error_flag);
double convert_exponent(const char *input, const char *filename, LINE *line_set, const char *full_string);
LINE *init_line();
ARGUMENT *init_argument();
PARAMETER *init_parameter();       
void connect_sue_instances_to_modules(MODULE *first_module);
void connect_wires_to_terminals_and_labels(MODULE *first_module);
void connect_wires_to_themselves(MODULE *first_module);
int count_unnamed_wires(MODULE *cur_module);
void print_wires(MODULE *cur_module);
int label_unnamed_wires(MODULE *cur_module, int count_in);
WIRE *determine_connecting_wire_node_name(MODULE *cur_module, int x, int y, 
                                         char *node_name);
void add_sue_instance_node(INSTANCE *cur_instance, NODES *cur_mod_node);
void determine_connecting_terminal_or_global_node_name(MODULE *cur_module, 
						      int x, int y, char *node_name);
int check_if_node_name_is_available(MODULE *cur_module, const char *name);
void label_connecting_terminals_or_globals(MODULE *cur_module, int x, int y, 
					   char *name);
void connect_instance_nodes(MODULE *first_module);
int is_node_naming_object(const char *name);
void trim_node_naming_instances_from_modules(MODULE *first_module);
void fill_in_missing_module_external_nodenames(MODULE *first_module);
void determine_sue_module_dependencies(MODULE *cur_module);
MODULE *trim_unused_modules(MODULE *first_module, const char *top_module_name,
                            char *simulator_name);
int is_conversion_module(MODULE *cur_module);
MODULE *trim_node_naming_modules(MODULE *first_module);
MODULE *extract_sue_modules_archive(PARAMETER *param_list);
MODULE *extract_sue_modules(const char *sue_lib_filename, PARAMETER *param_list,
                            const char *control_string);
MODULE *extract_sue_modules(const char *sue_lib_filename, PARAMETER *param_list);
MODULE *extract_sue_modules_oldversion(const char *sue_lib_filename, PARAMETER *param_list);
int is_sue_file(const char *name);
void prefix_x_to_instance_names(MODULE *first_module);
void recalculate_instance_terminal_origins_due_to_orient(INSTANCE *cur_instance);
void account_for_orient(MODULE *first_module);
void warn_about_duplicate_modules(MODULE *first_module);
void warn_about_duplicate_modules_no_exit(MODULE *first_module);
int check_if_arrayed_signal_name(const char *inst_name, char *base_name);
void replace_angle_brackets_with_square_brackets(char *in);
int determine_last_bit(const char *in_raw);
int determine_first_bit(const char *in_raw);
int determine_max_bit(const char *in_raw);
int determine_min_bit(const char *in_raw);
