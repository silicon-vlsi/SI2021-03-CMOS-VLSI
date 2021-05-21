/////////////////////////// TokenControlClass ///////////////////////////
////// written by Michael H Perrott  (http://www.cppsim.com) //////////
// Copyright (c) 2010-2015 Michael H Perrott
// All rights reserved.


// Permission is hereby granted, without written agreement and without
// license or royalty fees, to use, copy, modify, and distribute this
// software and its documentation for any purpose, provided that the
// above copyright notice and the following two paragraphs appear in
// all copies of this software.

// IN NO EVENT SHALL MICHAEL H PERROTT BE LIABLE TO ANY PARTY FOR DIRECT,
// INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
// THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF MICHAEL H
// PERROTT HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// MICHAEL H PERROTT SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
// BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THE SOFTWARE PROVIDED
// HEREUNDER IS ON AN "AS IS" BASIS, AND MICHAEL H PERROTT HAS NO
// OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
// MODIFICATIONS.
/////////////////////////////////////////////////////////////////////////

#define NaN 1.2345e307
#define MAX_CHAR_LENGTH 1000

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/////////////// EvalExpression /////////////

struct VarList {
char name[MAX_CHAR_LENGTH];
double value;
int is_number_flag, is_function_flag,is_valid_var_flag;
struct VarList *next;
};
typedef struct VarList VarList;


class EvalExpression
{
public:

EvalExpression();
~EvalExpression();
int eval(char *error_msg, const char *expr_in, const char *vars=NULL, double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
double out;

private:
VarList *first_arg, *first_var, *first_func;
int parse_into_args(char *error_msg, const char *expr);
void strtok_mp(char *str_in, char *t, char *r, char *delim);
int create_func_list();
int determine_variables(char *error_msg);
void print_arg_list();
void print_var_list();
void set_arg_variable_vals();
void eval_inner_paren_expr();
double eval_func(const char *name, double val);
int count_num_of_args();
int set_var_value(char *error_msg, const char *name, double val);
double get_var_value(const char *name);
void free_all();
void free_arg_and_var();
VarList *init_varlist();
};

/////////////// TokenList /////////////

struct TokenList
{
char name[MAX_CHAR_LENGTH];
char default_str[MAX_CHAR_LENGTH];
char description[MAX_CHAR_LENGTH];
int value;

// pad descriptions only used for Test Blocks
char pad0[30];
char pad1[30];
char pad2[30];
char pad3[30];

struct TokenList *next;
};
typedef struct TokenList TokenList;

struct RegFieldList
{
char name[MAX_CHAR_LENGTH];
// type options:  register, test_block
char type[MAX_CHAR_LENGTH];
TokenList *first_token;
int formula_exists_flag, inv_formula_exists_flag, signed_val, num_bits;
char formula[MAX_CHAR_LENGTH], inv_formula[MAX_CHAR_LENGTH];
char default_str[MAX_CHAR_LENGTH];
char description[MAX_CHAR_LENGTH];
struct RegFieldList *next;
};
typedef struct RegFieldList RegFieldList;

class RegTokenClass
{
public:
RegTokenClass();
~RegTokenClass();
int set_token_map(const char *reg_field_name_in, const char *type, const char *token_name_in, char *value_out, char *error_msg);
int get_token_map(const char *reg_field_name_in, const char *type, const char *value_in, char *token_name_out, char *error_msg);
int get_token_pad_info(const char *reg_field_name_in, const char *type_in, const char *token_name_in, int pad_num, char *value_out, char *error_msg);
int get_token_info(const char *reg_field_name_in, const char *type_in, const char *token_name_in, const char *info_str, char *value_out, char *error_msg);
int get_reg_info(const char *reg_field_name_in, const char *type_in, const char *info_str, char *value_out, char *error_msg);
int get_token_name(const char *reg_field_name_in, const char *type_in, int token_num, char *token_out, char *error_msg);
int get_reg_name(const char *type_in, int reg_num, char *reg_out, char *error_msg);
int add_token(const char *reg_field_name, const char *type, const char *token_name, const char *bit_pattern, int num_bits, const char *pad0, const char *pad1, const char *pad2, const char *pad3, const char *default_str, const char *description, char *error_msg);
int add_formula(const char *reg_field_name, const char *type, const char *formula, int num_bits, int signed_val, const char *default_str, const char *description, char *error_msg);
int add_inv_formula(const char *reg_field_name, const char *type, const char *inv_formula, int num_bits, int signed_val, char *error_msg);
int get_formula(const char *reg_field_name_in, const char *type_in, char *formula_out, char *error_msg);
int get_inv_formula(const char *reg_field_name_in, const char *type_in, char *inv_formula_out, char *error_msg);
void print_registers();

private:

RegFieldList *first_reg_field;
EvalExpression eval_formula;
// obtain_reg_field creates a new reg_field if needed
RegFieldList *obtain_reg_field(const char *name_in, const char *type_in);
int is_integer(const char *value);
int is_double(const char *value);
void remove_start_and_end_spaces_from_string(const char *bit_pattern_in,char *bit_pattern);
};

