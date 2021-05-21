/////////////////////////// TokenControlClass Wrapper File ///////////////////////////
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

#include "token_code.h"

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

struct CLASS_AND_FILENAME_STORAGE
  {
   RegTokenClass *reg_token_object;
   char filename[256];
};
typedef struct CLASS_AND_FILENAME_STORAGE CLASS_AND_FILENAME_STORAGE;

#ifdef _WIN32
static struct CLASS_AND_FILENAME_STORAGE class_and_file_info = {NULL, "NULL"};
#else
static struct CLASS_AND_FILENAME_STORAGE class_and_file_info = {.reg_token_object = NULL, .filename = "NULL"};
#endif

extern "C" 
{
DLLEXPORT void initialize(const char *filename);
DLLEXPORT int add_token(const char *reg_field_name_in, const char *type_in, const char *token_name_in, const char *bit_pattern_in, int num_bits, const char *default_str, const char *description, char *error_msg);
DLLEXPORT int add_token_with_test_pad_info(const char *reg_field_name_in, const char *type_in, const char *token_name_in, const char *bit_pattern_in, int num_bits, const char *pad0, const char *pad1, const char *pad2, const char *pad3, const char *default_str, const char *description, char *error_msg);
  DLLEXPORT int add_formula(const char *reg_field_name_in, const char *type_in, const char *formula, int num_bits, int signed_val, const char *default_str, const char *description, char *error_msg);
DLLEXPORT int add_inv_formula(const char *reg_field_name_in, const char *type_in, const char *inv_formula, int num_bits, int signed_val, char *error_msg);
DLLEXPORT int set_token_map(const char *reg_field_name_in, const char *type_in, const char *token_name_in, char *value_out, char *error_msg);
DLLEXPORT int get_token_map(const char *reg_field_name_in, const char *type_in, const char *value_in, char *token_name_out, char *error_msg);
DLLEXPORT int get_token_pad_info(const char *reg_field_name_in, const char *type_in, const char *token_name_in, int pad_num, char *value_out, char *error_msg);
DLLEXPORT int get_token_info(const char *reg_field_name_in, const char *type_in, const char *token_name_in, const char *info_str, char *value_out, char *error_msg);
DLLEXPORT int get_reg_info(const char *reg_field_name_in, const char *type_in, const char *info_str, char *value_out, char *error_msg);
DLLEXPORT int get_token_name(const char *reg_field_name_in, const char *type_in, int token_num, char *token_out, char *error_msg);
DLLEXPORT int get_reg_name(const char *type_in, int reg_num, char *reg_name_out, char *error_msg);
DLLEXPORT int get_formula(const char *reg_field_name_in, const char *type_in, char *formula_out, char *error_msg);
DLLEXPORT int get_inv_formula(const char *reg_field_name_in, const char *type_in, char *inv_formula_out, char *error_msg);
DLLEXPORT void print_registers();
}

void initialize(const char *filename)
{
if (class_and_file_info.reg_token_object != NULL)
  {
     delete class_and_file_info.reg_token_object;
     class_and_file_info.reg_token_object = NULL;
  }
class_and_file_info.reg_token_object = new RegTokenClass();
strcpy(class_and_file_info.filename,filename);
}

int add_token(const char *reg_field_name_in, const char *type_in, const char *token_name_in, const char *bit_pattern_in, int num_bits, const char *default_str, const char *description, char *error_msg)
{
char temp_string[MAX_CHAR_LENGTH];

if (class_and_file_info.reg_token_object == NULL)
    {
      sprintf(error_msg, "--> Error using 'add_token':  you must first initialize the RegTokenClass object\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', token_name '%s', bit_pattern '%s', num_bits = %d\n", reg_field_name_in, type_in, token_name_in, bit_pattern_in, num_bits);
      strcat(error_msg, temp_string);
      return(1);
    }
 if (class_and_file_info.reg_token_object->add_token(reg_field_name_in, type_in, token_name_in, bit_pattern_in, num_bits, "NULL", "NULL", "NULL", "NULL", default_str, description, error_msg) != 0)
    {
      strcat(error_msg, "--> Error using 'add_token':  add_token() function failed\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', token_name '%s', bit_pattern '%s', num_bits = %d\n", reg_field_name_in, type_in, token_name_in, bit_pattern_in, num_bits);
      strcat(error_msg, temp_string);
      return(1);
    }
return(0);
}

int add_token_with_test_pad_info(const char *reg_field_name_in, const char *type_in, const char *token_name_in, const char *bit_pattern_in, int num_bits, const char *pad0, const char *pad1, const char *pad2, const char *pad3, const char *default_str, const char *description, char *error_msg)
{
char temp_string[MAX_CHAR_LENGTH];

if (class_and_file_info.reg_token_object == NULL)
    {
      sprintf(error_msg, "--> Error using 'add_token_with_test_pad_info':  you must first initialize the RegTokenClass object\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', token_name '%s', bit_pattern '%s', num_bits = %d\n", reg_field_name_in, type_in, token_name_in, bit_pattern_in, num_bits);
      strcat(error_msg, temp_string);
      return(1);
    }
 if (class_and_file_info.reg_token_object->add_token(reg_field_name_in, type_in, token_name_in, bit_pattern_in, num_bits, pad0, pad1, pad2, pad3, default_str, description, error_msg) != 0)
    {
      strcat(error_msg, "--> Error using 'add_token_with_test_pad_info':  add_token_with_test_pad_info() function failed\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', token_name '%s', bit_pattern '%s', num_bits = %d\n", reg_field_name_in, type_in, token_name_in, bit_pattern_in, num_bits);
      strcat(error_msg, temp_string);
      return(1);
    }
return(0);
}

int add_formula(const char *reg_field_name_in, const char *type_in, const char *formula, int num_bits, int signed_val, const char *default_str, const char *description, char *error_msg)
{
char temp_string[MAX_CHAR_LENGTH];

if (class_and_file_info.reg_token_object == NULL)
    {
      sprintf(error_msg, "--> Error using 'add_formula':  you must first initialize the RegTokenClass object\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', formula '%s', num_bits '%d', signed_val '%d'\n", reg_field_name_in, type_in, formula, num_bits, signed_val);
      strcat(error_msg, temp_string);
      return(1);
    }
 if (class_and_file_info.reg_token_object->add_formula(reg_field_name_in, type_in, formula, num_bits, signed_val, default_str, description, error_msg) != 0)
    {
      strcat(error_msg, "--> Error using 'add_formula':  add_formula() function failed\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', formula '%s', num_bits '%d', signed_val '%d'\n", reg_field_name_in, type_in, formula, num_bits, signed_val);
      strcat(error_msg, temp_string);
      return(1);
    }
return(0);
}

int add_inv_formula(const char *reg_field_name_in, const char *type_in, const char *inv_formula, int num_bits, int signed_val, char *error_msg)
{
char temp_string[MAX_CHAR_LENGTH];

if (class_and_file_info.reg_token_object == NULL)
    {
      sprintf(error_msg, "--> Error using 'add_inv_formula':  you must first initialize the RegTokenClass object\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', inv_formula '%s', num_bits '%d', signed_val '%d'\n", reg_field_name_in, type_in, inv_formula, num_bits, signed_val);
      strcat(error_msg, temp_string);
      return(1);
    }
if (class_and_file_info.reg_token_object->add_inv_formula(reg_field_name_in, type_in, inv_formula, num_bits, signed_val, error_msg) != 0)
    {
      strcat(error_msg, "--> Error using 'add_inv_token':  add_inv_formula() function failed\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', inv_formula '%s', num_bits '%d', signed_val '%d'\n", reg_field_name_in, type_in, inv_formula, num_bits, signed_val);
      strcat(error_msg, temp_string);
      return(1);
    }
return(0);
}


int get_token_pad_info(const char *reg_field_name_in, const char *type_in, const char *token_name_in, int pad_num, char *value_out, char *error_msg)
{
char temp_string[MAX_CHAR_LENGTH];

if (class_and_file_info.reg_token_object == NULL)
    {
      sprintf(error_msg, "--> Error using 'get_token_pad_info':  you must first initialize the RegTokenClass object\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', token_name '%s', pad_num '%d'\n", reg_field_name_in, type_in, token_name_in, pad_num);
      strcat(error_msg, temp_string);
      return(1);
    }

if (class_and_file_info.reg_token_object->get_token_pad_info(reg_field_name_in, type_in, token_name_in, pad_num, value_out, error_msg) != 0)
    {
      strcat(error_msg, "--> Error using 'get_token_pad_info':  get_token_pad_info() function failed\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', token_name '%s', pad_num '%d'\n", reg_field_name_in, type_in, token_name_in, pad_num);
      strcat(error_msg, temp_string);
      return(1);
    }
else if (error_msg[0] != '\0') // this occurs if warning message occurs
    {
      strcat(error_msg, "--> Warning using 'get_token_pad_info':  get_token_pad_info() function warning message\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', token_name '%s', pad_num '%d'\n", reg_field_name_in, type_in, token_name_in, pad_num);
      strcat(error_msg, temp_string);
      return(2);
    }
return(0);
}

int get_token_info(const char *reg_field_name_in, const char *type_in, const char *token_name_in, const char *info_str, char *value_out, char *error_msg)
{
char temp_string[MAX_CHAR_LENGTH];

if (class_and_file_info.reg_token_object == NULL)
    {
      sprintf(error_msg, "--> Error using 'get_token_info':  you must first initialize the RegTokenClass object\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', token_name '%s', info_str '%s'\n", reg_field_name_in, type_in, token_name_in, info_str);
      strcat(error_msg, temp_string);
      return(1);
    }

if (class_and_file_info.reg_token_object->get_token_info(reg_field_name_in, type_in, token_name_in, info_str, value_out, error_msg) != 0)
    {
      strcat(error_msg, "--> Error using 'get_token_info':  get_token_info() function failed\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', token_name '%s', info_str '%s'\n", reg_field_name_in, type_in, token_name_in, info_str);
      strcat(error_msg, temp_string);
      return(1);
    }
else if (error_msg[0] != '\0') // this occurs if warning message occurs
    {
      strcat(error_msg, "--> Warning using 'get_token_info':  get_token_info() function warning message\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', token_name '%s', info_str '%s'\n", reg_field_name_in, type_in, token_name_in, info_str);
      strcat(error_msg, temp_string);
      return(2);
    }
return(0);
}

int get_reg_info(const char *reg_field_name_in, const char *type_in, const char *info_str, char *value_out, char *error_msg)
{
char temp_string[MAX_CHAR_LENGTH];

if (class_and_file_info.reg_token_object == NULL)
    {
      sprintf(error_msg, "--> Error using 'get_reg_info':  you must first initialize the RegTokenClass object\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', info_str '%s'\n", reg_field_name_in, type_in, info_str);
      strcat(error_msg, temp_string);
      return(1);
    }

if (class_and_file_info.reg_token_object->get_reg_info(reg_field_name_in, type_in, info_str, value_out, error_msg) != 0)
    {
      strcat(error_msg, "--> Error using 'get_reg_info':  get_reg_info() function failed\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', info_str '%s'\n", reg_field_name_in, type_in, info_str);
      strcat(error_msg, temp_string);
      return(1);
    }
else if (error_msg[0] != '\0') // this occurs if warning message occurs
    {
      strcat(error_msg, "--> Warning using 'get_reg_info':  get_reg_info() function warning message\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', info_str '%s'\n", reg_field_name_in, type_in, info_str);
      strcat(error_msg, temp_string);
      return(2);
    }
return(0);
}

int set_token_map(const char *reg_field_name_in, const char *type_in, const char *token_name_in, char *value_out, char *error_msg)
{
char temp_string[MAX_CHAR_LENGTH];

if (class_and_file_info.reg_token_object == NULL)
    {
      sprintf(error_msg, "--> Error using 'set_token_map':  you must first initialize the RegTokenClass object\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', token_name '%s'\n", reg_field_name_in, type_in, token_name_in);
      strcat(error_msg, temp_string);
      return(1);
    }

if (class_and_file_info.reg_token_object->set_token_map(reg_field_name_in, type_in, token_name_in, value_out, error_msg) != 0)
    {
      strcat(error_msg, "--> Error using 'set_token_map':  set_token_map() function failed\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', token_name '%s'\n", reg_field_name_in, type_in, token_name_in);
      strcat(error_msg, temp_string);
      return(1);
    }
else if (error_msg[0] != '\0') // this occurs if warning message occurs
    {
      strcat(error_msg, "--> Warning using 'set_token_map':  set_token_map() function warning message\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', token_name '%s'\n", reg_field_name_in, type_in, token_name_in);
      strcat(error_msg, temp_string);
      return(2);
    }
return(0);
}

int get_token_map(const char *reg_field_name_in, const char *type_in, const char *value_in, char *token_name_out, char *error_msg)
{
char temp_string[MAX_CHAR_LENGTH];

if (class_and_file_info.reg_token_object == NULL)
    {
      sprintf(error_msg, "--> Error using 'get_token_map':  you must first initialize the RegTokenClass object\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', value_in '%s'\n", reg_field_name_in, type_in, value_in);
      strcat(error_msg, temp_string);
      return(1);
    }
if (class_and_file_info.reg_token_object->get_token_map(reg_field_name_in, type_in, value_in, token_name_out, error_msg) != 0)
    {
      strcat(error_msg, "--> Error using 'get_token_map':  get_token_map() function failed\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', value_in '%s'\n", reg_field_name_in, type_in, value_in);
      strcat(error_msg, temp_string);
      return(1);
    }
return(0);
}

int get_token_name(const char *reg_field_name_in, const char *type_in, int token_num, char *token_name_out, char *error_msg)
{
char temp_string[MAX_CHAR_LENGTH];

int return_val;
if (class_and_file_info.reg_token_object == NULL)
    {
      sprintf(error_msg, "--> Error using 'get_token_name':  you must first initialize the RegTokenClass object\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', token_num '%d'\n", reg_field_name_in, type_in, token_num);
      strcat(error_msg, temp_string);
      return(1);
    }
return_val = class_and_file_info.reg_token_object->get_token_name(reg_field_name_in, type_in, token_num, token_name_out, error_msg);
if (return_val == 1)
    {
      strcat(error_msg, "--> Error using 'get_token_name':  get_token_name() function failed\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s', token_num '%d'\n", reg_field_name_in, type_in, token_num);
      strcat(error_msg, temp_string);
      return(1);
    }
return(return_val);
}

int get_formula(const char *reg_field_name_in, const char *type_in, char *formula_out, char *error_msg)
{
char temp_string[MAX_CHAR_LENGTH];

int return_val;
if (class_and_file_info.reg_token_object == NULL)
    {
      sprintf(error_msg, "--> Error using 'get_formula':  you must first initialize the RegTokenClass object\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s'\n", reg_field_name_in, type_in);
      strcat(error_msg, temp_string);
      return(1);
    }
return_val = class_and_file_info.reg_token_object->get_formula(reg_field_name_in, type_in, formula_out, error_msg);
if (return_val == 1)
    {
      strcat(error_msg, "--> Error using 'get_formula':  get_formula() function failed\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s'\n", reg_field_name_in, type_in);
      strcat(error_msg, temp_string);
      return(1);
    }
return(return_val);
}

int get_inv_formula(const char *reg_field_name_in, const char *type_in, char *inv_formula_out, char *error_msg)
{
char temp_string[MAX_CHAR_LENGTH];

int return_val;
if (class_and_file_info.reg_token_object == NULL)
    {
      sprintf(error_msg, "--> Error using 'get_inv_formula':  you must first initialize the RegTokenClass object\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s'\n", reg_field_name_in, type_in);
      strcat(error_msg, temp_string);
      return(1);
    }
return_val = class_and_file_info.reg_token_object->get_inv_formula(reg_field_name_in, type_in, inv_formula_out, error_msg);
if (return_val == 1)
    {
      strcat(error_msg, "--> Error using 'get_inv_formula':  get_inv_formula() function failed\n");
      sprintf(temp_string,"    Inputs:  reg_field '%s', type '%s'\n", reg_field_name_in, type_in);
      strcat(error_msg, temp_string);
      return(1);
    }
return(return_val);
}

int get_reg_name(const char *type_in, int reg_num, char *reg_name_out, char *error_msg)
{
char temp_string[MAX_CHAR_LENGTH];

int return_val;
if (class_and_file_info.reg_token_object == NULL)
    {
      sprintf(error_msg, "--> Error using 'get_reg_name':  you must first initialize the RegTokenClass object\n");
      sprintf(temp_string,"    Inputs:  type '%s', reg_num '%d'\n", type_in, reg_num);
      strcat(error_msg, temp_string);
      return(1);
    }
return_val = class_and_file_info.reg_token_object->get_reg_name(type_in, reg_num, reg_name_out, error_msg);
if (return_val == 1)
    {
      strcat(error_msg, "--> Error using 'get_reg_name':  get_reg_name() function failed\n");
      sprintf(temp_string,"    Inputs:  type '%s', reg_num '%d'\n", type_in, reg_num);
      strcat(error_msg, temp_string);
      return(1);
    }
return(return_val);
}


void print_registers()
{
if (class_and_file_info.reg_token_object == NULL)
    {
      printf("--> Error using 'print_registers':  you must first initialize the RegTokenClass object");
    }

class_and_file_info.reg_token_object->print_registers();
}

