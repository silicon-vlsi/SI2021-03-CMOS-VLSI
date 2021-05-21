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

#include "token_code.h"


RegTokenClass::RegTokenClass()
   : eval_formula()
{
first_reg_field = NULL;
}

RegTokenClass::~RegTokenClass()
{
TokenList *cur_token, *next_token;
RegFieldList *cur_reg_field, *next_reg_field;

cur_reg_field = first_reg_field;
while (cur_reg_field != NULL)
  {
   cur_token = cur_reg_field->first_token;
   while (cur_token != NULL)
      {
       next_token = cur_token->next;
       delete cur_token;
       cur_token = next_token;
      }
   next_reg_field = cur_reg_field->next;
   delete cur_reg_field;
   cur_reg_field = next_reg_field;
  }

first_reg_field = NULL;
}

void RegTokenClass::remove_start_and_end_spaces_from_string(const char *name_in, char *name_out)
{
int i;

for (i = 0; name_in[i] == ' ' || name_in[i] == '\t'; i++);
strncpy(name_out,&name_in[i],MAX_CHAR_LENGTH-1);
for (i = 0; name_out[i] != '\0'; i++);
for (i--; name_out[i] == ' ' || name_out[i] == '\t'; i--);
name_out[i+1] = '\0';
}


RegFieldList *RegTokenClass::obtain_reg_field(const char *name_in, const char *type_in)
{
int i,count;
char name[MAX_CHAR_LENGTH];
char type[MAX_CHAR_LENGTH];
RegFieldList *cur_reg_field;

// remove spaces at beginning and end of input strings
remove_start_and_end_spaces_from_string(name_in,name);
remove_start_and_end_spaces_from_string(type_in,type);

cur_reg_field = first_reg_field;
if (cur_reg_field == NULL)
  {
   first_reg_field = new RegFieldList;
   cur_reg_field = first_reg_field;
  }
else
  {
   while (cur_reg_field != NULL)
      {
       if (strcmp(cur_reg_field->name,name) == 0 && strcmp(cur_reg_field->type,type) == 0)
          return(cur_reg_field);
       cur_reg_field = cur_reg_field->next;
      }
   cur_reg_field = first_reg_field;
   while (cur_reg_field->next != NULL)
      cur_reg_field = cur_reg_field->next;

  cur_reg_field->next = new RegFieldList;
  cur_reg_field = cur_reg_field->next; 
  }

if (cur_reg_field == NULL)
   return(cur_reg_field);

strncpy(cur_reg_field->name,name,MAX_CHAR_LENGTH-1);
strncpy(cur_reg_field->type,type,MAX_CHAR_LENGTH-1);
strcpy(cur_reg_field->formula,"NULL");
strcpy(cur_reg_field->inv_formula,"NULL");
strcpy(cur_reg_field->default_str,"NULL");
strcpy(cur_reg_field->description,"NULL");
cur_reg_field->first_token = NULL;
cur_reg_field->formula_exists_flag = 0;
cur_reg_field->inv_formula_exists_flag = 0;
cur_reg_field->signed_val = 0;
cur_reg_field->num_bits = 0;
cur_reg_field->next = NULL;

return(cur_reg_field);
}




int RegTokenClass::is_integer(const char *value)
{
int i;


for (i = 0; value[i] != '\0'; i++)
   {
    if ((value[i] < '0' || value[i] > '9') && value[i] != '-')
       {
       return(0);  
       }
   }
return(1);
}


int RegTokenClass::is_double(const char *value)
{
int i;

for (i = 0; value[i] != '\0'; i++)
   {
    if ((value[i] < '0' || value[i] > '9') && value[i] != '.' && value[i] != 'e' &&
         value[i] != 'E' && value[i] != '-')
       {
        return(0);  
       }
   }

return(1);
}



int RegTokenClass::add_token(const char *reg_field_name_in, const char *type_in, const char *token_name_in, const char *bit_pattern_in, int num_bits, const char *pad0, const char *pad1, const char *pad2, const char *pad3, const char *default_str_in, const char *description, char *error_msg)
{
int i,j,bit_pattern_value;
RegFieldList *cur_reg_field;
TokenList *cur_token;
char reg_field_name[MAX_CHAR_LENGTH];
char token_name[MAX_CHAR_LENGTH];
char bit_pattern[MAX_CHAR_LENGTH];
char type[MAX_CHAR_LENGTH];
char default_str[MAX_CHAR_LENGTH];

char temp_string[MAX_CHAR_LENGTH];
error_msg[0] = '\0';

// remove spaces at beginning and end of input strings
remove_start_and_end_spaces_from_string(reg_field_name_in,reg_field_name);
remove_start_and_end_spaces_from_string(token_name_in,token_name);
remove_start_and_end_spaces_from_string(bit_pattern_in,bit_pattern);
remove_start_and_end_spaces_from_string(type_in,type);
remove_start_and_end_spaces_from_string(default_str_in,default_str);

cur_reg_field = obtain_reg_field(reg_field_name,type);

if (cur_reg_field == NULL)
  {
   sprintf(temp_string,"Error in 'RegTokenClass::add_token':\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"   cur_reg_field is NULL -> this should never happen!\n");
   strcat(error_msg,temp_string);   
   return(1);
  }

if (cur_reg_field->num_bits == 0)
   cur_reg_field->num_bits = num_bits;
else if (num_bits != cur_reg_field->num_bits)
   {
   sprintf(temp_string,"Error in 'RegTokenClass::add_token':\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"   num_bits does not agree with previous num_bits provided to reg_field\n");
   strcat(error_msg,temp_string);   
   sprintf(temp_string,"   -> num_bits = %d, previously provided num_bits = %d\n", num_bits, cur_reg_field->num_bits);
   strcat(error_msg,temp_string);   
   return(1);
   }
cur_token = cur_reg_field->first_token;
if (cur_token == NULL)
  {
   cur_reg_field->first_token = new TokenList;
   cur_token = cur_reg_field->first_token;
  }
else
  {
   while (cur_token->next != NULL)
      cur_token = cur_token->next;
  cur_token->next = new TokenList;
  cur_token = cur_token->next; 
  }

for (i = 0; bit_pattern[i] != '\0'; i++);
if (i != num_bits)
   {
   sprintf(temp_string,"Error in 'RegTokenClass::add_token':\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"   num_bits does not agree with input bit_pattern\n");
   strcat(error_msg,temp_string);   
   sprintf(temp_string,"   -> num_bits = %d, bit_pattern = '%s'\n", num_bits, bit_pattern_in);
   strcat(error_msg,temp_string);   
   return(1);
   }
bit_pattern_value = 0;
for (i--,j = 0; i >= 0; i--,j++)
    bit_pattern_value += ((bit_pattern[i] == '0') ? 0 : 1<<j);

strncpy(cur_token->name,token_name,MAX_CHAR_LENGTH-1);
cur_token->value = bit_pattern_value;
strcpy(cur_token->pad0,pad0);
strcpy(cur_token->pad1,pad1);
strcpy(cur_token->pad2,pad2);
strcpy(cur_token->pad3,pad3);
strncpy(cur_token->default_str,default_str,MAX_CHAR_LENGTH-1);
strncpy(cur_token->description,description,MAX_CHAR_LENGTH-1);
cur_token->next = NULL;

return(0);
}


int RegTokenClass::add_formula(const char *reg_field_name_in, const char *type_in, const char *formula, int num_bits, int signed_val, const char *default_str_in, const char *description, char *error_msg)
{
int i;
RegFieldList *cur_reg_field;
char reg_field_name[MAX_CHAR_LENGTH];
char type[MAX_CHAR_LENGTH];
char default_str[MAX_CHAR_LENGTH];

char temp_string[MAX_CHAR_LENGTH];
error_msg[0] = '\0';

if (num_bits < 1)
  {
   sprintf(temp_string,"Error in 'RegTokenClass::add_formula':\n");
   strcat(error_msg, temp_string);
   sprintf(temp_string,"  num_bits must be >= 1\n");
   strcat(error_msg, temp_string);
   sprintf(temp_string,"  -> in this case, num_bits = %d\n", num_bits);
   strcat(error_msg, temp_string);
   return(1);
  }
if (num_bits > 32)
  {
   sprintf(temp_string,"Error in 'RegTokenClass::add_formula':\n");
   strcat(error_msg, temp_string);
   sprintf(temp_string,"  num_bits must be <= 32\n");
   strcat(error_msg, temp_string);
   sprintf(temp_string,"  -> in this case, num_bits = %d\n", num_bits);
   strcat(error_msg, temp_string);
   return(1);
  }
if (signed_val != 0 && signed_val != 1)
  {
   sprintf(temp_string,"Error in 'RegTokenClass::add_formula':\n");
   strcat(error_msg, temp_string);
   sprintf(temp_string,"  signed_val must be either 0 or 1\n");
   strcat(error_msg, temp_string);
   sprintf(temp_string,"  -> in this case, signed_val = %d\n", signed_val);
   strcat(error_msg, temp_string);
   return(1);
  }

// remove spaces at beginning and end of input strings
remove_start_and_end_spaces_from_string(reg_field_name_in,reg_field_name);
remove_start_and_end_spaces_from_string(type_in,type);
remove_start_and_end_spaces_from_string(default_str_in,default_str);

cur_reg_field = obtain_reg_field(reg_field_name,type);

if (cur_reg_field == NULL)
  {
   sprintf(temp_string,"Error in 'RegTokenClass::add_formula':\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"   cur_reg_field is NULL -> this should never happen!\n");
   strcat(error_msg,temp_string);
   return(1);
  }

strncpy(cur_reg_field->default_str,default_str,MAX_CHAR_LENGTH-1);
strncpy(cur_reg_field->description,description,MAX_CHAR_LENGTH-1);

for (i = 0; formula[i] != '\0'; i++);
if (i >= MAX_CHAR_LENGTH)
  {
   sprintf(temp_string,"Error in 'RegTokenClass::add_formula':\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"   formula string is too long!\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"   (must be less than %d as set by MAX_CHAR_LENGTH)\n",MAX_CHAR_LENGTH);
   strcat(error_msg,temp_string);
   sprintf(temp_string,"   -> either make formula shorter, or increase MAX_CHAR_LENGTH in source code\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"      for formula: '%s'\n",formula);
   strcat(error_msg,temp_string);
   sprintf(temp_string,"      associated with reg_field '%s'\n",cur_reg_field->name);
   strcat(error_msg,temp_string);
   return(1);
  }
strncpy(cur_reg_field->formula,formula,MAX_CHAR_LENGTH-1);
cur_reg_field->formula_exists_flag = 1;

if (cur_reg_field->inv_formula_exists_flag == 1)
   {
     if (cur_reg_field->signed_val != signed_val)
       {
        sprintf(temp_string,"Error in 'RegTokenClass::add_formula':\n");
        strcat(error_msg,temp_string);
        sprintf(temp_string,"  signed_val disagrees with previously specified signed_val from add_inv_formula\n");
        strcat(error_msg,temp_string);
        sprintf(temp_string,"  -> in this case, signed_val = %d, previously specified signed_val = %d\n", signed_val,
	       cur_reg_field->signed_val);
        strcat(error_msg,temp_string);
        return(1);
       }
     if (cur_reg_field->num_bits != num_bits)
       {
        sprintf(temp_string,"Error in 'RegTokenClass::add_formula':\n");
        strcat(error_msg,temp_string);
        sprintf(temp_string,"  num_bits disagrees with previously specified num_bits from add_inv_formula\n");
        strcat(error_msg,temp_string);
        sprintf(temp_string,"  -> in this case, num_bits = %d, previously specified num_bits = %d\n", num_bits,
	       cur_reg_field->num_bits);
        strcat(error_msg,temp_string);
        return(1);
       }
   }
 else
   {
   cur_reg_field->signed_val = signed_val;
   cur_reg_field->num_bits = num_bits;
   }

return(0);
}


int RegTokenClass::add_inv_formula(const char *reg_field_name_in, const char *type_in, const char *inv_formula, 
				   int num_bits, int signed_val, char *error_msg)
{
int i;
RegFieldList *cur_reg_field;
char reg_field_name[MAX_CHAR_LENGTH];
char type[MAX_CHAR_LENGTH];

char temp_string[MAX_CHAR_LENGTH];
error_msg[0] = '\0';

if (num_bits < 1)
  {
   sprintf(temp_string,"Error in 'RegTokenClass::add_formula':\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"  num_bits must be >= 1\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"  -> in this case, num_bits = %d\n", num_bits);
   strcat(error_msg,temp_string);
   return(1);
  }
if (num_bits > 32)
  {
   sprintf(temp_string,"Error in 'RegTokenClass::add_formula':\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"  num_bits must be <= 32\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"  -> in this case, num_bits = %d\n", num_bits);
   strcat(error_msg,temp_string);
   return(1);
  }
if (signed_val != 0 && signed_val != 1)
  {
   sprintf(temp_string,"Error in 'RegTokenClass::add_formula':\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"  signed_val must be either 0 or 1\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"  -> in this case, signed_val = %d\n", signed_val);
   strcat(error_msg,temp_string);
   return(1);
  }

// remove spaces at beginning and end of input strings
remove_start_and_end_spaces_from_string(reg_field_name_in,reg_field_name);
remove_start_and_end_spaces_from_string(type_in,type);

cur_reg_field = obtain_reg_field(reg_field_name,type);

if (cur_reg_field == NULL)
  {
   sprintf(temp_string,"Error in 'RegTokenClass::add_inv_formula':\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"   cur_reg_field is NULL -> this should never happen!\n");
   strcat(error_msg,temp_string);
   return(1);
  }

cur_reg_field->signed_val = signed_val;

for (i = 0; inv_formula[i] != '\0'; i++);
if (i >= MAX_CHAR_LENGTH)
  {
   sprintf(temp_string,"Error in 'RegTokenClass::add_inv_formula':\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"   inv_formula string is too long!\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"   (must be less than %d as set by MAX_CHAR_LENGTH)\n",MAX_CHAR_LENGTH);
   strcat(error_msg,temp_string);
   sprintf(temp_string,"   -> either make formula shorter, or increase MAX_CHAR_LENGTH in source code\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"      for formula: '%s'\n",inv_formula);
   strcat(error_msg,temp_string);
   sprintf(temp_string,"      associated with reg_field '%s'\n",cur_reg_field->name);
   strcat(error_msg,temp_string);
   return(1);
  }
strncpy(cur_reg_field->inv_formula,inv_formula,MAX_CHAR_LENGTH-1);
cur_reg_field->inv_formula_exists_flag = 1;


if (cur_reg_field->formula_exists_flag == 1)
   {
     if (cur_reg_field->signed_val != signed_val)
       {
        sprintf(temp_string,"Error in 'RegTokenClass::add_inv_formula':\n");
        strcat(error_msg,temp_string);
        sprintf(temp_string,"  signed_val disagrees with previously specified signed_val from add_formula\n");
        strcat(error_msg,temp_string);
        sprintf(temp_string,"  -> in this case, signed_val = %d, previously specified signed_val = %d\n", signed_val,
	       cur_reg_field->signed_val);
        strcat(error_msg,temp_string);
        return(1);
       }
     if (cur_reg_field->num_bits != num_bits)
       {
        sprintf(temp_string,"Error in 'RegTokenClass::add_inv_formula':\n");
        strcat(error_msg,temp_string);
        sprintf(temp_string,"  num_bits disagrees with previously specified num_bits from add_formula\n");
        strcat(error_msg,temp_string);
        sprintf(temp_string,"  -> in this case, num_bits = %d, previously specified num_bits = %d\n", num_bits,
	       cur_reg_field->num_bits);
        strcat(error_msg,temp_string);
        return(1);
       }
   }
 else
   {
   cur_reg_field->signed_val = signed_val;
   cur_reg_field->num_bits = num_bits;
   }

return(0);
}

int RegTokenClass::get_reg_name(const char *type_in, int reg_num, char *reg_out, char *error_msg)
{
RegFieldList *cur_reg_field;
int i;
char type[MAX_CHAR_LENGTH];

error_msg[0] = '\0';
remove_start_and_end_spaces_from_string(type_in,type);

i = 0;
cur_reg_field = first_reg_field;
while (cur_reg_field != NULL)
   {
    if (strcmp(type,cur_reg_field->type) == 0)
       {
        if (i == reg_num)
           break;
        i++;
       }
    cur_reg_field = cur_reg_field->next;
   }

if (cur_reg_field == NULL)
    return(2);

strcpy(reg_out,cur_reg_field->name);
return(0);
}

int RegTokenClass::get_token_name(const char *reg_field_name_in, const char *type_in, int token_num, char *token_out, char *error_msg)
{
RegFieldList *cur_reg_field;
TokenList *cur_token;
int i;
char reg_field_name[MAX_CHAR_LENGTH];
char type[MAX_CHAR_LENGTH];

char temp_string[MAX_CHAR_LENGTH];
error_msg[0] = '\0';

remove_start_and_end_spaces_from_string(reg_field_name_in,reg_field_name);
remove_start_and_end_spaces_from_string(type_in,type);

cur_reg_field = first_reg_field;
while (cur_reg_field != NULL)
   {
    //sprintf(temp_string,"cur_reg_field = '%s'\n",cur_reg_field->name);
   if (strcmp(reg_field_name,cur_reg_field->name) == 0 && strcmp(type,cur_reg_field->type) == 0)
       break;
   cur_reg_field = cur_reg_field->next;
   }

if (cur_reg_field == NULL)
   {
    sprintf(temp_string,"Error in 'RegTokenClass.get_token_name':  unrecognized reg_field name\n");
    strcat(error_msg,temp_string);
    sprintf(temp_string,"   -> reg_field '%s' (type '%s') is unknown\n",reg_field_name,type);
    strcat(error_msg,temp_string);
    return(1);
   }

i = 0;
cur_token = cur_reg_field->first_token;
while (cur_token != NULL)
   {
    if (i == token_num)
       break;
    i++;
    cur_token = cur_token->next;
   }

if (cur_token == NULL)
    return(2);

strcpy(token_out,cur_token->name);
return(0);
}


int RegTokenClass::get_formula(const char *reg_field_name_in, const char *type_in, char *formula_out, char *error_msg)
{
RegFieldList *cur_reg_field;
char reg_field_name[MAX_CHAR_LENGTH];
char type[MAX_CHAR_LENGTH];

char temp_string[MAX_CHAR_LENGTH];
error_msg[0] = '\0';

remove_start_and_end_spaces_from_string(reg_field_name_in,reg_field_name);
remove_start_and_end_spaces_from_string(type_in,type);

cur_reg_field = first_reg_field;
while (cur_reg_field != NULL)
   {
    //sprintf(temp_string,"cur_reg_field = '%s'\n",cur_reg_field->name);
   if (strcmp(reg_field_name,cur_reg_field->name) == 0 && strcmp(type,cur_reg_field->type) == 0)
       break;
   cur_reg_field = cur_reg_field->next;
   }

if (cur_reg_field == NULL)
   {
    sprintf(temp_string,"Error in 'RegTokenClass.get_formula':  unrecognized reg_field name\n");
    strcat(error_msg,temp_string);
    sprintf(temp_string,"   -> reg_field '%s' (type '%s') is unknown\n",reg_field_name,type);
    strcat(error_msg,temp_string);
    return(1);
   }

if (cur_reg_field->formula_exists_flag != 1)
   {
    // sprintf(temp_string,"Error in 'RegTokenClass.get_formula':  formula is undefined\n");
    // strcat(error_msg,temp_string);
    // sprintf(temp_string,"   -> reg_field '%s' (type '%s') has no formula\n",reg_field_name,type);
    // strcat(error_msg,temp_string);
    return(2);
   }

strcpy(formula_out,cur_reg_field->formula);
return(0);
}


int RegTokenClass::get_inv_formula(const char *reg_field_name_in, const char *type_in, char *inv_formula_out, char *error_msg)
{
RegFieldList *cur_reg_field;
char reg_field_name[MAX_CHAR_LENGTH];
char type[MAX_CHAR_LENGTH];

char temp_string[MAX_CHAR_LENGTH];
error_msg[0] = '\0';

remove_start_and_end_spaces_from_string(reg_field_name_in,reg_field_name);
remove_start_and_end_spaces_from_string(type_in,type);

cur_reg_field = first_reg_field;
while (cur_reg_field != NULL)
   {
    //sprintf(temp_string,"cur_reg_field = '%s'\n",cur_reg_field->name);
   if (strcmp(reg_field_name,cur_reg_field->name) == 0 && strcmp(type,cur_reg_field->type) == 0)
       break;
   cur_reg_field = cur_reg_field->next;
   }

if (cur_reg_field == NULL)
   {
    sprintf(temp_string,"Error in 'RegTokenClass.get_inv_formula':  unrecognized reg_field name\n");
    strcat(error_msg,temp_string);
    sprintf(temp_string,"   -> reg_field '%s' (type '%s') is unknown\n",reg_field_name,type);
    strcat(error_msg,temp_string);
    return(1);
   }

if (cur_reg_field->inv_formula_exists_flag != 1)
   {
    // sprintf(temp_string,"Error in 'RegTokenClass.get_inv_formula':  inv_formula is undefined\n");
    // strcat(error_msg,temp_string);
    // sprintf(temp_string,"   -> reg_field '%s' (type '%s') has no inv_formula\n",reg_field_name,type);
    // strcat(error_msg,temp_string);
    return(2);
   }

strcpy(inv_formula_out,cur_reg_field->inv_formula);
return(0);
}


int RegTokenClass::set_token_map(const char *reg_field_name_in, const char *type_in, const char *token_name_in, char *value_out, char *error_msg)
{
RegFieldList *cur_reg_field;
TokenList *cur_token;
int temp_int, found_token_flag, mask_val;
int token_int, token_int_flag, token_dbl_flag;
double token_dbl, temp_dbl;
char token_name[MAX_CHAR_LENGTH];
char reg_field_name[MAX_CHAR_LENGTH];
char type[MAX_CHAR_LENGTH];
int min_value, max_value, min_max_flag;

char temp_string[MAX_CHAR_LENGTH];
error_msg[0] = '\0';

//sprintf(temp_string,"set: '%s' to '%s'\n",reg_field_name_in, token_name_in);

// remove spaces at beginning and end of input strings
remove_start_and_end_spaces_from_string(reg_field_name_in,reg_field_name);
remove_start_and_end_spaces_from_string(token_name_in,token_name);
remove_start_and_end_spaces_from_string(type_in,type);

cur_reg_field = first_reg_field;
while (cur_reg_field != NULL)
   {
    //sprintf(temp_string,"cur_reg_field = '%s'\n",cur_reg_field->name);
   if (strcmp(reg_field_name,cur_reg_field->name) == 0 && strcmp(type,cur_reg_field->type) == 0)
       break;
   cur_reg_field = cur_reg_field->next;
   }

if (cur_reg_field == NULL)
   {
    sprintf(temp_string,"Error in 'RegTokenClass.set_token_map':  unrecognized reg_field name\n");
    strcat(error_msg,temp_string);
    sprintf(temp_string,"   -> reg_field '%s' (type '%s') is unknown\n",reg_field_name,type);
    strcat(error_msg,temp_string);
    return(1);
   }
mask_val = (1<<(cur_reg_field->num_bits))-1;

token_int_flag = 0;
token_dbl_flag = 0;
if (is_integer(token_name))
    {
    token_int_flag = 1;
    token_int = atoi(token_name);
    }
else if (is_double(token_name))
   {
    token_dbl_flag = 1;
    token_dbl = atof(token_name);
   }

if (cur_reg_field->formula_exists_flag == 1)
   {
    if (cur_reg_field->signed_val == 1)
       {
	max_value = (1<<(cur_reg_field->num_bits-1))-1;
        min_value = -(max_value + 1);
       }
    else
       {
	min_value = 0;
	max_value = (1<<(cur_reg_field->num_bits))-1;
       }

    if (token_dbl_flag == 1)
      {
	if (eval_formula.eval(error_msg,cur_reg_field->formula,"x",token_dbl) == 1)
	  {
           strcat(error_msg,"--> Error in 'RegTokenClass.set_token_map' using function eval_formula.eval() with formula\n");
           return(1);
	  }
      }
    else if (token_int_flag == 1)
      {
	if (eval_formula.eval(error_msg,cur_reg_field->formula,"x",(double) token_int) == 1)
	  {
           strcat(error_msg,"--> Error in 'RegTokenClass.set_token_map' using function eval_formula.eval() with formula\n");
           return(1);
	  }
      }
    else
       {
          sprintf(temp_string,"Error in 'RegTokenClass.set_token_map':  inputs for formulas must be numerical in value\n");
          strcat(error_msg,temp_string);
          sprintf(temp_string,"   input of x = '%s' was specified for\n",token_name);
          strcat(error_msg,temp_string);
          sprintf(temp_string,"   formula '%s'\n",cur_reg_field->formula);
          strcat(error_msg,temp_string);
          sprintf(temp_string,"   in reg_field '%s'\n",reg_field_name);
          strcat(error_msg,temp_string);
          sprintf(temp_string,"   -> please correct the set command applied to reg_field '%s'\n",reg_field_name);
          strcat(error_msg,temp_string);
	  return(1);
       }

    temp_int = ((int) floor(eval_formula.out + 0.5));
    if (cur_reg_field->signed_val == 1 && (temp_int > max_value || temp_int < min_value))
       {
	sprintf(temp_string,"Warning in 'RegTokenClass.set_token_map':\n");
        strcat(error_msg,temp_string);
        sprintf(temp_string,"   for reg_field '%s'\n",reg_field_name);
        strcat(error_msg,temp_string);
        sprintf(temp_string,"   signed formula results must be >= %d and <= %d\n",min_value,max_value);
        strcat(error_msg,temp_string);
        sprintf(temp_string,"   so that they can be represented by a signed %d-bit value\n", cur_reg_field->num_bits);
        strcat(error_msg,temp_string);
        sprintf(temp_string,"   input of x = '%s' was specified for\n",token_name);
        strcat(error_msg,temp_string);
        sprintf(temp_string,"   formula '%s'\n",cur_reg_field->formula);
        strcat(error_msg,temp_string);
        sprintf(temp_string,"   resulting in a formula output of '%d'\n",temp_int);
        strcat(error_msg,temp_string);
        sprintf(temp_string,"   -> please correct input '%s' for reg_field '%s'\n",token_name,reg_field_name);
        strcat(error_msg,temp_string);

	min_max_flag = 0;
	if (temp_int > max_value)
	  {
	    min_max_flag = 1;
	   temp_int = max_value;
           sprintf(temp_string,"   --> for now, setting formula output to max_value of '%d'\n",temp_int);
           strcat(error_msg,temp_string);
	  }
	if (temp_int < min_value)
	  {
	   min_max_flag = 1;
	   temp_int = min_value;
           sprintf(temp_string,"   --> for now, setting formula output to min_value of '%d'\n",temp_int);
           strcat(error_msg,temp_string);
	  }
	if (min_max_flag == 1)
	  {
           if (cur_reg_field->inv_formula_exists_flag == 1)
	     {
	      if (eval_formula.eval(error_msg,cur_reg_field->inv_formula,"x",(double) temp_int) == 1)
	         {
                  strcat(error_msg,"--> Error in 'RegTokenClass.set_token_map' using function eval_formula.eval() with inv_formula\n");
                  return(1);
	         }
              if (token_dbl_flag == 1)
		   sprintf(temp_string,"       which corresponds to a formula input of '%5.3e'\n",eval_formula.out);
              else
		   sprintf(temp_string,"       which corresponds to a formula input of '%d'\n",(int) floor(eval_formula.out + 0.5));
              strcat(error_msg,temp_string);
	     }
	  }
       }
    else if (cur_reg_field->signed_val == 0 && (temp_int > max_value || temp_int < min_value))
       {
	sprintf(temp_string,"Warning in 'RegTokenClass.set_token_map':\n");  
        strcat(error_msg,temp_string);
        sprintf(temp_string,"   for reg_field '%s'\n",reg_field_name);
        strcat(error_msg,temp_string);
	sprintf(temp_string,"   unsigned formula results must be >= %d and <= %d\n",min_value,max_value);
        strcat(error_msg,temp_string);
        sprintf(temp_string,"   so that they can be represented by an unsigned %d-bit value\n", cur_reg_field->num_bits);
        strcat(error_msg,temp_string);
        sprintf(temp_string,"   input of x = '%s' was specified for\n",token_name);
        strcat(error_msg,temp_string);
        sprintf(temp_string,"   formula '%s'\n",cur_reg_field->formula);
        strcat(error_msg,temp_string);
        sprintf(temp_string,"   resulting in a formula output of '%d'\n",temp_int);
        strcat(error_msg,temp_string);
        sprintf(temp_string,"   -> please correct input '%s' for reg_field '%s'\n",token_name,reg_field_name);
        strcat(error_msg,temp_string);
	if (temp_int > max_value)
	  {
	   temp_int = max_value;
           sprintf(temp_string,"   --> for now, setting formula output to max_value of '%d'\n",temp_int);
           strcat(error_msg,temp_string);
	  }
	if (temp_int < min_value)
	  {
	   temp_int = min_value;
           sprintf(temp_string,"   --> for now, setting formula output to min_value of '%d'\n",temp_int);
           strcat(error_msg,temp_string);
	  }
       }

    //       sprintf(temp_string,"formula out = %12.1f, int = %d\n",eval_formula.out,temp_int);
    sprintf(value_out,"%d",temp_int&mask_val);
   }
else
   {
   found_token_flag = 0;
   cur_token = cur_reg_field->first_token;
   while (cur_token != NULL)
     {
      if (token_int_flag == 1)
        {
         if (is_integer(cur_token->name))
            {
            temp_int = atoi(cur_token->name);
            if (token_int == temp_int)
                found_token_flag = 1;
            }
         else if (is_double(cur_token->name))
            {
             temp_dbl = atof(cur_token->name);
             temp_int = (int) floor(temp_dbl + 0.5);
             if (fabs(temp_dbl - temp_int) < 1e-6)
                {
                 if (token_int == temp_int)
                     found_token_flag = 1;
                }
            }
	}
      else if (token_dbl_flag == 1)
        {
         if (is_double(cur_token->name))
            {
             temp_dbl = atof(cur_token->name);
             if (fabs(token_dbl - temp_dbl) < 1e-6)
                 {
                  found_token_flag = 1;
                 }
            }
        }
      else if (strcmp(cur_token->name,token_name) == 0)
         found_token_flag = 1;

      if (found_token_flag == 1)
          break;

      cur_token = cur_token->next;
     }
   if (found_token_flag == 0)
         {
          sprintf(temp_string,"Error in 'RegTokenClass.set_token_map':  can't find token!\n");
          strcat(error_msg,temp_string);
          sprintf(temp_string,"  -> looking for token '%s' for reg_field '%s'\n",
               token_name, reg_field_name);
          strcat(error_msg,temp_string);
          sprintf(temp_string,"     but it doesn't seem to exist\n");
          strcat(error_msg,temp_string);
          sprintf(temp_string,"     valid tokens for reg_field '%s':\n",reg_field_name);
          strcat(error_msg,temp_string);
          cur_token = cur_reg_field->first_token;
          while (cur_token != NULL)
             {
	      sprintf(temp_string,"       '%s'   (value = '%d')\n",cur_token->name, cur_token->value);
              strcat(error_msg,temp_string);
              cur_token = cur_token->next;
             }
	  return(1);
         }
    sprintf(value_out,"%d",cur_token->value);
   }
return(0);
}


int RegTokenClass::get_token_pad_info(const char *reg_field_name_in, const char *type_in, const char *token_name_in, int pad_num, char *value_out, char *error_msg)
{
RegFieldList *cur_reg_field;
TokenList *cur_token;
char token_name[MAX_CHAR_LENGTH];
char reg_field_name[MAX_CHAR_LENGTH];
char type[MAX_CHAR_LENGTH];


char temp_string[MAX_CHAR_LENGTH];
error_msg[0] = '\0';

//sprintf(temp_string,"set: '%s' to '%s'\n",reg_field_name_in, token_name_in);

// remove spaces at beginning and end of input strings
remove_start_and_end_spaces_from_string(reg_field_name_in,reg_field_name);
remove_start_and_end_spaces_from_string(token_name_in,token_name);
remove_start_and_end_spaces_from_string(type_in,type);

if (pad_num < 0 || pad_num > 3)
   {
    sprintf(temp_string,"Error in 'RegTokenClass.get_token_pad_info':  input pad_num must be 0, 1, 2, or 3\n");
    strcat(error_msg,temp_string);
    sprintf(temp_string,"   -> pad_num = %d is invalid for token '%s' of reg_field '%s' (type '%s')\n",pad_num,token_name,reg_field_name,type);
    strcat(error_msg,temp_string);
    return(1);
   }

cur_reg_field = first_reg_field;
while (cur_reg_field != NULL)
   {
    //sprintf(temp_string,"cur_reg_field = '%s'\n",cur_reg_field->name);
   if (strcmp(reg_field_name,cur_reg_field->name) == 0 && strcmp(type,cur_reg_field->type) == 0)
       break;
   cur_reg_field = cur_reg_field->next;
   }

if (cur_reg_field == NULL)
   {
    sprintf(temp_string,"Error in 'RegTokenClass.get_token_pad_info':  unrecognized reg_field name\n");
    strcat(error_msg,temp_string);
    sprintf(temp_string,"   -> reg_field '%s' (type '%s') is unknown\n",reg_field_name,type);
    strcat(error_msg,temp_string);
    return(1);
   }


cur_token = cur_reg_field->first_token;
while (cur_token != NULL)
  {
   if (strcmp(cur_token->name,token_name) == 0)
      break;
   cur_token = cur_token->next;
  }
if (cur_token == NULL)
  {
   sprintf(temp_string,"Error in 'RegTokenClass.get_token_pad_info':  can't find token!\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"  -> looking for token '%s' for reg_field '%s'\n",
               token_name, reg_field_name);
   strcat(error_msg,temp_string);
   sprintf(temp_string,"     but it doesn't seem to exist\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"     valid tokens for reg_field '%s':\n",reg_field_name);
   strcat(error_msg,temp_string);
   cur_token = cur_reg_field->first_token;
   while (cur_token != NULL)
     {
      sprintf(temp_string,"    '%s'\n",cur_token->name);
      strcat(error_msg,temp_string);
      cur_token = cur_token->next;
     }
   return(1);
  }

if (pad_num == 0)
   sprintf(value_out,"%s",cur_token->pad0);
else if (pad_num == 1)
   sprintf(value_out,"%s",cur_token->pad1);
else if (pad_num == 2)
   sprintf(value_out,"%s",cur_token->pad2);
else if (pad_num == 3)
   sprintf(value_out,"%s",cur_token->pad3);

return(0);
}

int RegTokenClass::get_token_info(const char *reg_field_name_in, const char *type_in, const char *token_name_in, const char *info_str, char *value_out, char *error_msg)
{
RegFieldList *cur_reg_field;
TokenList *cur_token;
char token_name[MAX_CHAR_LENGTH];
char reg_field_name[MAX_CHAR_LENGTH];
char type[MAX_CHAR_LENGTH];


char temp_string[MAX_CHAR_LENGTH];
error_msg[0] = '\0';

//sprintf(temp_string,"set: '%s' to '%s'\n",reg_field_name_in, token_name_in);

// remove spaces at beginning and end of input strings
remove_start_and_end_spaces_from_string(reg_field_name_in,reg_field_name);
remove_start_and_end_spaces_from_string(token_name_in,token_name);
remove_start_and_end_spaces_from_string(type_in,type);

if (strcmp(info_str,"default_str") != 0 && strcmp(info_str,"description") != 0)
   {
    sprintf(temp_string,"Error in 'RegTokenClass.get_token_info':  info_str must be 'default_str' or 'description'\n");
    strcat(error_msg,temp_string);
    sprintf(temp_string,"   -> info_str = '%s' is invalid for token '%s' of reg_field '%s' (type '%s')\n",info_str,token_name,reg_field_name,type);
    strcat(error_msg,temp_string);
    return(1);
   }

cur_reg_field = first_reg_field;
while (cur_reg_field != NULL)
   {
    //sprintf(temp_string,"cur_reg_field = '%s'\n",cur_reg_field->name);
   if (strcmp(reg_field_name,cur_reg_field->name) == 0 && strcmp(type,cur_reg_field->type) == 0)
       break;
   cur_reg_field = cur_reg_field->next;
   }

if (cur_reg_field == NULL)
   {
    sprintf(temp_string,"Error in 'RegTokenClass.get_token_info':  unrecognized reg_field name\n");
    strcat(error_msg,temp_string);
    sprintf(temp_string,"   -> reg_field '%s' (type '%s') is unknown\n",reg_field_name,type);
    strcat(error_msg,temp_string);
    return(1);
   }


cur_token = cur_reg_field->first_token;
while (cur_token != NULL)
  {
   if (strcmp(cur_token->name,token_name) == 0)
      break;
   cur_token = cur_token->next;
  }
if (cur_token == NULL)
  {
   sprintf(temp_string,"Error in 'RegTokenClass.get_token_info':  can't find token!\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"  -> looking for token '%s' for reg_field '%s'\n",
               token_name, reg_field_name);
   strcat(error_msg,temp_string);
   sprintf(temp_string,"     but it doesn't seem to exist\n");
   strcat(error_msg,temp_string);
   sprintf(temp_string,"     valid tokens for reg_field '%s':\n",reg_field_name);
   strcat(error_msg,temp_string);
   cur_token = cur_reg_field->first_token;
   while (cur_token != NULL)
     {
      sprintf(temp_string,"    '%s'\n",cur_token->name);
      strcat(error_msg,temp_string);
      cur_token = cur_token->next;
     }
   return(1);
  }

if (strcmp(info_str,"default_str") == 0)
   sprintf(value_out,"%s",cur_token->default_str);
else if(strcmp(info_str,"description") == 0)
   sprintf(value_out,"%s",cur_token->description);
 
return(0);
}

int RegTokenClass::get_reg_info(const char *reg_field_name_in, const char *type_in, const char *info_str, char *value_out, char *error_msg)
{
RegFieldList *cur_reg_field;
char reg_field_name[MAX_CHAR_LENGTH];
char type[MAX_CHAR_LENGTH];


char temp_string[MAX_CHAR_LENGTH];
error_msg[0] = '\0';

//sprintf(temp_string,"set: '%s' to '%s'\n",reg_field_name_in, token_name_in);

// remove spaces at beginning and end of input strings
remove_start_and_end_spaces_from_string(reg_field_name_in,reg_field_name);
remove_start_and_end_spaces_from_string(type_in,type);

if (strcmp(info_str,"default_str") != 0 && strcmp(info_str,"description") != 0 && strcmp(info_str,"num_bits") != 0 && strcmp(info_str,"signed_val") != 0)
   {
    sprintf(temp_string,"Error in 'RegTokenClass.get_reg_info':  info_str must be 'default_str', 'description', 'num_bits', or 'signed_val'\n");
    strcat(error_msg,temp_string);
    sprintf(temp_string,"   -> info_str = '%s' is invalid for reg_field '%s' (type '%s')\n",info_str,reg_field_name,type);
    strcat(error_msg,temp_string);
    return(1);
   }

cur_reg_field = first_reg_field;
while (cur_reg_field != NULL)
   {
    //sprintf(temp_string,"cur_reg_field = '%s'\n",cur_reg_field->name);
   if (strcmp(reg_field_name,cur_reg_field->name) == 0 && strcmp(type,cur_reg_field->type) == 0)
       break;
   cur_reg_field = cur_reg_field->next;
   }

if (cur_reg_field == NULL)
   {
    sprintf(temp_string,"Error in 'RegTokenClass.get_reg_info':  unrecognized reg_field name\n");
    strcat(error_msg,temp_string);
    sprintf(temp_string,"   -> reg_field '%s' (type '%s') is unknown\n",reg_field_name,type);
    strcat(error_msg,temp_string);
    return(1);
   }

if (strcmp(info_str,"default_str") == 0)
   sprintf(value_out,"%s",cur_reg_field->default_str);
else if(strcmp(info_str,"description") == 0)
   sprintf(value_out,"%s",cur_reg_field->description);
else if(strcmp(info_str,"num_bits") == 0)
   sprintf(value_out,"%d",cur_reg_field->num_bits);
else if(strcmp(info_str,"signed_val") == 0)
   sprintf(value_out,"%d",cur_reg_field->signed_val);
 
return(0);
}


int RegTokenClass::get_token_map(const char *reg_field_name_in, const char *type_in, const char *value_in, char *token_name_out, char *error_msg)
{
RegFieldList *cur_reg_field;
TokenList *cur_token;
int temp_int, found_token_flag, mask_sign_ext, mask_check_sign;
double token_dbl, temp_dbl;
int token_int, token_int_flag, token_dbl_flag;
char value_str[MAX_CHAR_LENGTH];
int value, temp_value, min_value, max_value;
char reg_field_name[MAX_CHAR_LENGTH];
char type[MAX_CHAR_LENGTH];

char temp_string[MAX_CHAR_LENGTH];
error_msg[0] = '\0';

//sprintf(temp_string,"set: '%s' to '%s'\n",reg_field_name_in, token_name_in);

// remove spaces at beginning and end of input strings
remove_start_and_end_spaces_from_string(reg_field_name_in,reg_field_name);
remove_start_and_end_spaces_from_string(value_in,value_str);
remove_start_and_end_spaces_from_string(type_in,type);

cur_reg_field = first_reg_field;
while (cur_reg_field != NULL)
   {
   //sprintf(temp_string,"cur_reg_field = '%s'\n",cur_reg_field->name);
   if (strcmp(reg_field_name,cur_reg_field->name) == 0 && strcmp(type,cur_reg_field->type) == 0)
       break;
   cur_reg_field = cur_reg_field->next;
   }

if (cur_reg_field == NULL)
   {
    sprintf(temp_string,"Error in 'RegTokenClass.get_token_map':  unrecognized reg_field name\n");
    strcat(error_msg,temp_string);
    sprintf(temp_string,"   -> reg_field '%s' (type '%s') is unknown\n",reg_field_name,type);
    strcat(error_msg,temp_string);
    return(1);
   }

if (cur_reg_field->inv_formula_exists_flag == 1)
   {
    if (cur_reg_field->signed_val == 1)
       {
	max_value = (1<<(cur_reg_field->num_bits-1))-1;
        min_value = -(max_value + 1);
       }
    else
       {
	min_value = 0;
	max_value = (1<<(cur_reg_field->num_bits))-1;
       }

    if (strcmp(value_str,"min") == 0)
      {
       if (eval_formula.eval(error_msg,cur_reg_field->inv_formula,"x",(double) min_value) == 1)
	  {
           strcat(error_msg,"--> Error in 'RegTokenClass.get_token_map' using function eval_formula.eval()\n");
           return(1);
	  }
       sprintf(token_name_out,"%d",(int) floor(eval_formula.out + 0.5));
      }
    else if (strcmp(value_str,"max") == 0)
      {
       if (eval_formula.eval(error_msg,cur_reg_field->inv_formula,"x",(double) max_value) == 1)
	  {
           strcat(error_msg,"--> Error in 'RegTokenClass.get_token_map' using function eval_formula.eval()\n");
           return(1);
	  }
       sprintf(token_name_out,"%d",(int) floor(eval_formula.out + 0.5));
      }
    else
      {
       mask_sign_ext = ~((1<<(cur_reg_field->num_bits))-1);
       mask_check_sign = 1<<((cur_reg_field->num_bits)-1);

       if (is_integer(value_str) != 1)
          {
           sprintf(temp_string,"Error in 'RegTokenClass.get_token_map':  value_in must correspond to an integer\n");
           strcat(error_msg,temp_string);
           sprintf(temp_string,"   -> in this case, value_in = '%s'\n", value_in);
           strcat(error_msg,temp_string);
           return(1);
          }

       value = atoi(value_str);

       if (cur_reg_field->signed_val == 1)
          {
           if ((value & mask_check_sign) != 0)
              {
               value = value | mask_sign_ext;
              }
          }
       if (eval_formula.eval(error_msg,cur_reg_field->inv_formula,"x",(double) value) == 1)
	  {
           strcat(error_msg,"--> Error in 'RegTokenClass.get_token_map' using function eval_formula.eval()\n");
           return(1);
	  }
       sprintf(token_name_out,"%d",(int) floor(eval_formula.out + 0.5));
      }
   }
else
   {
    if (is_integer(value_str) != 1)
      {
       sprintf(temp_string,"Error in 'RegTokenClass.get_token_map':  value_in must correspond to an integer\n");
       strcat(error_msg,temp_string);
       sprintf(temp_string,"   -> in this case, value_in = '%s'\n", value_in);
       strcat(error_msg,temp_string);
       return(1);
      }

   value = atoi(value_str);

   found_token_flag = 0;
   cur_token = cur_reg_field->first_token;
   while (cur_token != NULL)
     {
      if (cur_token->value == value)
	{
          found_token_flag = 1;
          break;
	}
      cur_token = cur_token->next;
     }

   if (found_token_flag == 0)
         {
          if (cur_reg_field->formula_exists_flag == 1)
	    {
             sprintf(temp_string,"Error in 'RegTokenClass.get_token_map':  no inverse formula!\n");
             strcat(error_msg,temp_string);
	     sprintf(temp_string,"    -> Since you use 'add_formula' for reg_field '%s' ***\n",reg_field_name);
             strcat(error_msg,temp_string);
	     sprintf(temp_string,"       then you must also include an 'add_inv_formula' statement ***\n");
             strcat(error_msg,temp_string);
	    }
	  else
	    {
             sprintf(temp_string,"Error in 'RegTokenClass.get_token_map':  can't find token!\n");
             strcat(error_msg,temp_string);
             sprintf(temp_string,"  -> looking for value = '%s'\n", value_in);
             strcat(error_msg,temp_string);
             sprintf(temp_string,"     but it doesn't seem to exist\n");
             strcat(error_msg,temp_string);
             sprintf(temp_string,"     valid tokens for reg_field '%s':\n",reg_field_name);
             strcat(error_msg,temp_string);
             cur_token = cur_reg_field->first_token;
             while (cur_token != NULL)
                {
	         sprintf(temp_string,"         token '%s':  value = '%d'\n", cur_token->name, cur_token->value);
                 strcat(error_msg,temp_string);
                 cur_token = cur_token->next;
                }
	    }
	  return(1);
         }

   strcpy(token_name_out,cur_token->name);
   }
return(0);
}

void RegTokenClass::print_registers()
{
RegFieldList *cur_reg_field;
TokenList *cur_token;

char name[MAX_CHAR_LENGTH];
TokenList *first_token;
int formula_exists_flag, inv_formula_exists_flag, signed_val, num_bits;
char formula[MAX_CHAR_LENGTH], inv_formula[MAX_CHAR_LENGTH];
struct RegFieldList *next;

printf("\n-------------------------- REGISTERS ----------------------------\n\n");
cur_reg_field = first_reg_field;
while (cur_reg_field != NULL)
   {
     if (strcmp(cur_reg_field->type,"register") == 0)
       {
        printf("----- Register Field '%s' -----\n",cur_reg_field->name);
        printf("    formula_exists_flag = %d\n", cur_reg_field->formula_exists_flag);
        printf("    inv_formula_exists_flag = %d\n", cur_reg_field->inv_formula_exists_flag);
        printf("    signed_val = %d\n", cur_reg_field->signed_val);
        printf("    num_bits = %d\n", cur_reg_field->num_bits);
        if (cur_reg_field->formula_exists_flag == 1)
           printf("    -> formula = '%s'\n", cur_reg_field->formula);
        if (cur_reg_field->inv_formula_exists_flag == 1)
           printf("    -> inv_formula = '%s'\n", cur_reg_field->inv_formula);
        if (cur_reg_field->first_token != NULL)
           {
	   printf("    -> list of tokens:\n");
	   cur_token = cur_reg_field->first_token;
	   while (cur_token != NULL)
	      {
	       printf("       token '%s':  value = %d\n", cur_token->name, cur_token->value);  
	       cur_token = cur_token->next;
	      }
           }
       }
     cur_reg_field = cur_reg_field->next;
   }

printf("\n-------------------------- TEST BLOCKS ----------------------------\n\n");

cur_reg_field = first_reg_field;
while (cur_reg_field != NULL)
   {
     if (strcmp(cur_reg_field->type,"test_block") == 0)
       {
        printf("----- Test Block '%s' -----\n",cur_reg_field->name);
        printf("    formula_exists_flag = %d\n", cur_reg_field->formula_exists_flag);
        printf("    inv_formula_exists_flag = %d\n", cur_reg_field->inv_formula_exists_flag);
        printf("    signed_val = %d\n", cur_reg_field->signed_val);
        printf("    num_bits = %d\n", cur_reg_field->num_bits);
        if (cur_reg_field->formula_exists_flag == 1)
           printf("    -> formula = '%s'\n", cur_reg_field->formula);
        if (cur_reg_field->inv_formula_exists_flag == 1)
           printf("    -> inv_formula = '%s'\n", cur_reg_field->inv_formula);
        if (cur_reg_field->first_token != NULL)
           {
	   printf("    -> list of tokens:\n");
	   cur_token = cur_reg_field->first_token;
	   while (cur_token != NULL)
	      {
	       printf("       token '%s':  value = %d\n", cur_token->name, cur_token->value);  
	       cur_token = cur_token->next;
	      }
           }
       }
     cur_reg_field = cur_reg_field->next;
   }
}

/////////////////////// EvalExpression //////////////////////////

EvalExpression::EvalExpression()
{
first_func = NULL;
first_arg = NULL;
first_var = NULL;
if (create_func_list() == 1)
     exit(1);
}

EvalExpression::~EvalExpression()
{
free_all();
}


VarList *EvalExpression::init_varlist()
{
 VarList *A;

 A = new VarList;
 strcpy(A->name,"NULL");
 A->value = 0;
 A->is_number_flag = 0;
 A->is_function_flag = 0;
 A->is_valid_var_flag = 0;
 A->next = NULL;

 return(A);
}

void EvalExpression::free_all()
{
VarList *cur_func, *next_func, *cur_arg, *next_arg, *cur_var, *next_var;
cur_func = first_func;
cur_arg = first_arg;
cur_var = first_var;

while (cur_func != NULL)
   {
     next_func = cur_func->next;
     delete cur_func;
     cur_func = next_func;
   }
while (cur_arg != NULL)
   {
     next_arg = cur_arg->next;
     delete cur_arg;
     cur_arg = next_arg;
   }
while (cur_var != NULL)
   {
     next_var = cur_var->next;
     delete cur_var;
     cur_var = next_var;
   }
first_func = NULL;
first_arg = NULL;
first_var = NULL;
}

void EvalExpression::free_arg_and_var()
{
VarList *cur_arg, *next_arg, *cur_var, *next_var;
cur_arg = first_arg;
cur_var = first_var;

while (cur_arg != NULL)
   {
     next_arg = cur_arg->next;
     delete cur_arg;
     cur_arg = next_arg;
   }
while (cur_var != NULL)
   {
     next_var = cur_var->next;
     delete cur_var;
     cur_var = next_var;
   }
first_arg = NULL;
first_var = NULL;
}


int EvalExpression::eval(char *error_msg, const char *expr_in,const char *vars, double var0, double var1, double var2, double var3, double var4, double var5, double var6, double var7, double var8, double var9)
{
char vars2[MAX_CHAR_LENGTH];
char *p;
int i, count;
VarList *cur_arg,*cur_var;
char temp_string[MAX_CHAR_LENGTH];

free_arg_and_var();
//free_all();


for (i = 0; i < MAX_CHAR_LENGTH; i++)
   if (expr_in[i] == '\0')
       break;
if (expr_in[i] != '\0')
  {
    error_msg[0] = '\0';
    sprintf(temp_string,"Error in EvalExpression.eval:  input expression is too long\n");
    strcat(error_msg, temp_string);
    sprintf(temp_string,"  string is limited to MAX_CHAR_LENGTH = %d\n",MAX_CHAR_LENGTH);
    strcat(error_msg, temp_string);
    sprintf(temp_string,"   must increase MAX_CHAR_LENGTH in source code\n");
    strcat(error_msg, temp_string);
    sprintf(temp_string,"   if you want to use the following input expression:\n");
    strcat(error_msg, temp_string);
    sprintf(temp_string,"   expr_in = %s\n",expr_in);
    strcat(error_msg, temp_string);
    return(1);
  }

// create first_arg list
if (parse_into_args(error_msg,expr_in) == 1)
   {
    strcat(error_msg,"--> Error in EvalExpression.eval() using parse_into_args() function\n");
    return(1);
   }
//printf("args:\n");
//print_arg_list();

// create first_var list
if (determine_variables(error_msg) == 1)
  {
    strcat(error_msg,"--> Error in EvalExpression.eval() using determine_variables() function\n");
    return(1);
  }
//printf("args:\n");
//print_arg_list();

if (vars != NULL)
  {
   strncpy(vars2,vars,MAX_CHAR_LENGTH-1);
   p = strtok(vars2," ,;");
  }
else
  p = NULL;

i = 0;
while(p != NULL)
   {
    if (i >= 10)
      {
        error_msg[0] = '\0';
	sprintf(temp_string,"Error in EvalExpression.eval:  number of parameters exceeded\n");
        strcat(error_msg,temp_string);
	sprintf(temp_string,"  (it's limited to 10 parameters");
        strcat(error_msg,temp_string);
	sprintf(temp_string,"  arg string: %s\n",vars);
        strcat(error_msg,temp_string);
	sprintf(temp_string,"  associated expression: %s\n",expr_in);
        strcat(error_msg,temp_string);
	return(1);
      }
     if (i == 0)
       {
       if (var0 == NaN)
	 {
	   error_msg[0] = '\0';
	   sprintf(temp_string,"Error in EvalExpression.eval:  value of variable '%s' not set\n",p);
           strcat(error_msg,temp_string);
	   sprintf(temp_string,"   overall expression: %s\n",expr_in);
           strcat(error_msg,temp_string);
	   return(1);
	 }
       if (set_var_value(error_msg,p,var0) == 1)
	 {
           sprintf(temp_string,"--> Error in EvalExpression.eval() using set_var_value() function\n");
           strcat(error_msg,temp_string); 
	   return(1);
	 }
       }
     else if (i == 1)
       {
       if (var1 == NaN)
	 {
	   error_msg[0] = '\0';
	   sprintf(temp_string,"Error in EvalExpression.eval:  value of variable '%s' not set\n",p);
           strcat(error_msg,temp_string);
	   sprintf(temp_string,"   overall expression: %s\n",expr_in);
           strcat(error_msg,temp_string);
	   return(1);
	 }
       if (set_var_value(error_msg,p,var1) == 1)
	 {
           sprintf(temp_string,"--> Error in EvalExpression.eval() using set_var_value() function\n");
           strcat(error_msg,temp_string); 
	   return(1);
	 }
       }
     else if (i == 2)
       {
       if (var2 == NaN)
	 {
	   error_msg[0] = '\0';
	   sprintf(temp_string,"Error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
           strcat(error_msg,temp_string);
	   sprintf(temp_string,"   overall expression: %s\n",expr_in);
           strcat(error_msg,temp_string);
	   return(1);
	 }
       if (set_var_value(error_msg,p,var2) == 1)
	 {
           sprintf(temp_string,"--> Error in EvalExpression.eval() using set_var_value() function\n");
           strcat(error_msg,temp_string); 
	   return(1);
	 }
       }
     else if (i == 3)
       {
       if (var3 == NaN)
	 {
	   error_msg[0] = '\0';
	   sprintf(temp_string,"Error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
           strcat(error_msg,temp_string);
	   sprintf(temp_string,"   overall expression: %s\n",expr_in);
           strcat(error_msg,temp_string);
	   return(1);
	 }
       if (set_var_value(error_msg,p,var3) == 1)
	 {
           sprintf(temp_string,"--> Error in EvalExpression.eval() using set_var_value() function\n");
           strcat(error_msg,temp_string); 
	   return(1);
	 }
       }
     else if (i == 4)
       {
       if (var4 == NaN)
	 {
	   error_msg[0] = '\0';
	   sprintf(temp_string,"Error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
           strcat(error_msg,temp_string);
	   sprintf(temp_string,"   overall expression: %s\n",expr_in);
           strcat(error_msg,temp_string);
	   return(1);
	 }
       if (set_var_value(error_msg,p,var4) == 1)
	 {
           sprintf(temp_string,"--> Error in EvalExpression.eval() using set_var_value() function\n");
           strcat(error_msg,temp_string); 
	   return(1);
	 }
       }
     else if (i == 5)
       {
       if (var5 == NaN)
	 {
	   error_msg[0] = '\0';
	   sprintf(temp_string,"Error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
           strcat(error_msg,temp_string);
	   sprintf(temp_string,"   overall expression: %s\n",expr_in);
           strcat(error_msg,temp_string);
	   return(1);
	 }
       if (set_var_value(error_msg,p,var5) == 1)
	 {
           sprintf(temp_string,"--> Error in EvalExpression.eval() using set_var_value() function\n");
           strcat(error_msg,temp_string); 
	   return(1);
	 }
       }
     else if (i == 6)
       {
       if (var6 == NaN)
	 {
	   error_msg[0] = '\0';
	   sprintf(temp_string,"Error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
           strcat(error_msg,temp_string);
	   sprintf(temp_string,"   overall expression: %s\n",expr_in);
           strcat(error_msg,temp_string);
	   return(1);
	 }
       if (set_var_value(error_msg,p,var6) == 1)
	 {
           sprintf(temp_string,"--> Error in EvalExpression.eval() using set_var_value() function\n");
           strcat(error_msg,temp_string); 
	   return(1);
	 }
       }
     else if (i == 7)
       {
       if (var7 == NaN)
	 {
	   error_msg[0] = '\0';
	   sprintf(temp_string,"Error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
           strcat(error_msg,temp_string);
	   sprintf(temp_string,"   overall expression: %s\n",expr_in);
           strcat(error_msg,temp_string);
	   return(1);
	 }
       if (set_var_value(error_msg,p,var7) == 1)
	 {
           sprintf(temp_string,"--> Error in EvalExpression.eval() using set_var_value() function\n");
           strcat(error_msg,temp_string); 
	   return(1);
	 }
       }
     else if (i == 8)
       {
       if (var8 == NaN)
	 {
	   error_msg[0] = '\0';
	   sprintf(temp_string,"Error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
           strcat(error_msg,temp_string);
	   sprintf(temp_string,"   overall expression: %s\n",expr_in);
           strcat(error_msg,temp_string);
	   return(1);
	 }
       if (set_var_value(error_msg,p,var8) == 1)
	 {
           sprintf(temp_string,"--> Error in EvalExpression.eval() using set_var_value() function\n");
           strcat(error_msg,temp_string); 
	   return(1);
	 }
       }
     else if (i == 9)
       {
       if (var9 == NaN)
	 {
	   error_msg[0] = '\0';
	   sprintf(temp_string,"Error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
           strcat(error_msg,temp_string);
	   sprintf(temp_string,"   overall expression: %s\n",expr_in);
           strcat(error_msg,temp_string);
	   return(1);
	 }
       if (set_var_value(error_msg,p,var9) == 1)
	 {
           sprintf(temp_string,"--> Error in EvalExpression.eval() using set_var_value() function\n");
           strcat(error_msg,temp_string); 
	   return(1);
	 }
       }
     i++;
     p = strtok(NULL," ,;");
   }

 cur_var = first_var;
 while(cur_var != NULL)
   {
     if (cur_var->is_valid_var_flag == 0)
       {
         error_msg[0] = '\0';
	 sprintf(temp_string,"Error in 'EvalExpression.eval':  variable '%s' is undefined\n",cur_var->name);
         strcat(error_msg,temp_string);
         sprintf(temp_string,"   for expr: '%s'\n",expr_in);
         strcat(error_msg,temp_string);
	 return(1);
       }
     cur_var = cur_var->next;
   }
/*
printf("variables:\n");
print_var_list();
printf("args:\n");
print_arg_list();
return(1);

*/

set_arg_variable_vals();

/*
printf("args:\n");
print_arg_list();
printf("variables:\n");
print_var_list();
*/

for (i = 0; i < 50; i++)
   {
    eval_inner_paren_expr();
    count = count_num_of_args();
    if (count < 2)
      break;
    //    printf("****** new args:   count = %d ********\n",count);
    //  print_VarList(first_arg);
   }
i = 0;
cur_arg = first_arg;
while (cur_arg != NULL)
   {
     i++;
     cur_arg = cur_arg->next;
   }
if (i != 1)
   {
     print_arg_list();
     error_msg[0] = '\0';
     sprintf(temp_string,"Error in 'EvalExpression.eval':  expression not evaluated properly!\n");
     strcat(error_msg,temp_string);
     sprintf(temp_string,"  -> expr = '%s'\n",expr_in);
     strcat(error_msg,temp_string);
     return(1);
   }
out = first_arg->value;
return(0);
}


int EvalExpression::set_var_value(char *error_msg, const char *name, double val)
{
VarList *cur_var;
int flag;
char temp_string[MAX_CHAR_LENGTH];

cur_var = first_var;
flag = 0;
while (cur_var != NULL)
   {
     if (strcmp(cur_var->name,name) == 0)
       {
	 cur_var->value = val;
	 cur_var->is_number_flag = 1;
         cur_var->is_valid_var_flag = 1;
         flag = 1;
       }
     cur_var = cur_var->next;
   }
if (flag == 0)
   {
     error_msg[0] = '\0';
     sprintf(temp_string,"Error in 'EvalExpression.set_var_value':\n");
     strcat(error_msg,temp_string);
     sprintf(temp_string,"  can't find variable '%s'\n",name);
     strcat(error_msg,temp_string);
     return(1);   
   }
return(0);
}

double EvalExpression::get_var_value(const char *name)
{
VarList *cur_var;

cur_var = first_var;
while (cur_var != NULL)
   {
     if (strcmp(cur_var->name,name) == 0)
       {
	 if (cur_var->is_number_flag == 1)
	   return(cur_var->value);
       }
     cur_var = cur_var->next;
   }
printf("Error in 'EvalExpression.get_var_value':\n");
printf("  can't find variable '%s'\n",name);
exit(1);
return(0);  // never gets here, but gcc complains without this statement
}


int EvalExpression::count_num_of_args()
{
VarList *cur_arg;
int i;

i = 0;
cur_arg = first_arg;
while (cur_arg != NULL)
   {
     i++;
     cur_arg = cur_arg->next;
   }
return(i);
}

void EvalExpression::eval_inner_paren_expr()
{
VarList *cur_arg,*first_paren_arg,*last_paren_arg,*prev_arg,*temp_arg;
VarList *prev_paren_arg;

cur_arg = first_arg;
prev_paren_arg = NULL;
first_paren_arg = NULL;
last_paren_arg = NULL;
temp_arg = NULL;

// find last '('
while (cur_arg != NULL)
   {
     if (strcmp(cur_arg->name,"(") == 0)
       {
        first_paren_arg = cur_arg;
        prev_paren_arg = temp_arg;
       }
     temp_arg = cur_arg;
     cur_arg = cur_arg->next;
   }
if (first_paren_arg != NULL)
   {
     if (prev_paren_arg != NULL)
       {
	 prev_paren_arg->next = first_paren_arg->next;
         delete first_paren_arg;
         first_paren_arg = prev_paren_arg->next;
       }
     else
       {
         temp_arg = first_paren_arg->next;
         delete first_arg;
         first_arg = temp_arg;
         first_paren_arg = first_arg;
       }
   }
cur_arg = first_paren_arg;
while (cur_arg != NULL)
   {
     if (strcmp(cur_arg->name,")") == 0)
       {
	 last_paren_arg = cur_arg;
	 break;
       }
     temp_arg = cur_arg;
     cur_arg = cur_arg->next;
   }

if (last_paren_arg != NULL)
   {
     if (temp_arg != NULL)
       {
	temp_arg->next = last_paren_arg->next;
        delete last_paren_arg;
        last_paren_arg = temp_arg->next;
       }
     else
       {
	 printf("error:  mismatched parenthesis to ')'\n");
	 first_arg = NULL;
         return;
       }
   }

if (first_paren_arg == NULL)
  {
   first_paren_arg = first_arg;
  }

//if (strcmp(first_paren_arg->name,"-") == 0 || 
//    strcmp(first_paren_arg->name,"+") == 0)
//   {
//    if (first_paren_arg->next == NULL)
//       {
//	printf("error: - or + unary operator is misplaced\n");
//	return(NULL);
//       }
//    if (first_paren_arg->next->is_number_flag == 0)
//       {
//	printf("error: - or + unary operator is misplaced\n");
//	return(NULL);
//       }
//    if (strcmp(first_paren_arg->name,"-") == 0)
//	first_paren_arg->value = -first_paren_arg->next->value;
//    else
//	first_paren_arg->value = first_paren_arg->next->value;
//    first_paren_arg->is_number_flag = 1;
//    sprintf(first_paren_arg->name,"%5.3e",first_paren_arg->value);

//    temp_arg = first_paren_arg->next;
//    first_paren_arg->next = temp_arg->next;
//    delete temp_arg;
//   }

cur_arg = first_paren_arg;
prev_arg = NULL;
while (cur_arg != last_paren_arg)
  {
    if (strcmp(cur_arg->name,"^") == 0)
      {
	if (prev_arg == NULL)
	  {
	    printf("error:  expression for '^' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (cur_arg->next == NULL)
	  {
	    printf("error:  expression for '^' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (prev_arg->is_number_flag == 0)
	  {
	    printf("error:  expression for '^' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
        if (cur_arg->next->is_number_flag == 0)
	  {
	    printf("error:  expression for '^' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
        prev_arg->value = pow(prev_arg->value,cur_arg->next->value);
	sprintf(prev_arg->name,"%17.12e",prev_arg->value);
        prev_arg->next = cur_arg->next->next;
        delete cur_arg->next;
        delete cur_arg;
        cur_arg = prev_arg;
      }
    prev_arg = cur_arg;
    cur_arg = cur_arg->next;
  }

// unary +
cur_arg = first_paren_arg;
prev_arg = NULL;
while (cur_arg != last_paren_arg)
  {
    if (strcmp(cur_arg->name,"+") == 0)
      {
	if (prev_arg == NULL)
	  {
   	    if (cur_arg->next == NULL)
	      {
	       printf("error:  expression for unary '+' is invalid\n");
	       first_arg = NULL;
	       return;
	      }
            if (cur_arg->next->is_number_flag == 0)
	      {
	       printf("error:  expression for unary '+' is invalid\n");
	       first_arg = NULL;
	       return;
	      }
	    cur_arg->value = cur_arg->next->value;
	    cur_arg->is_number_flag = 1;
	    sprintf(cur_arg->name,"%17.12e",cur_arg->value);
	    temp_arg = cur_arg->next;
	    cur_arg->next = cur_arg->next->next;
	    delete temp_arg;
	  }
      }
    prev_arg = cur_arg;
    cur_arg = cur_arg->next;
  }

// unary -
cur_arg = first_paren_arg;
prev_arg = NULL;
while (cur_arg != last_paren_arg)
  {
    if (strcmp(cur_arg->name,"-") == 0)
      {
	if (prev_arg == NULL)
	  {
	    if (cur_arg->next == NULL)
	      {
	       printf("error:  expression for unary '-' is invalid\n");
	       first_arg = NULL;
	       return;
	      }
            if (cur_arg->next->is_number_flag == 0)
	      {
	       printf("error:  expression for unary '-' is invalid\n");
	       first_arg = NULL;
	       return;
	      }
	    cur_arg->value = -(cur_arg->next->value);
	    cur_arg->is_number_flag = 1;
	    sprintf(cur_arg->name,"%17.12e",cur_arg->value);
	    temp_arg = cur_arg->next;
	    cur_arg->next = cur_arg->next->next;
	    delete temp_arg;
	  }
      }
    prev_arg = cur_arg;
    cur_arg = cur_arg->next;
  }


cur_arg = first_paren_arg;
prev_arg = NULL;
while (cur_arg != last_paren_arg)
  {
    if (strcmp(cur_arg->name,"/") == 0)
      {
	if (prev_arg == NULL)
	  {
	    printf("error:  expression for '/' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (cur_arg->next == NULL)
	  {
	    printf("error:  expression for '/' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (prev_arg->is_number_flag == 0)
	  {
	    printf("error:  expression for '/' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
        if (cur_arg->next->is_number_flag == 0)
	  {
	    printf("error:  expression for '/' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (fabs(cur_arg->next->value) < 1e-300)
	  {
	    printf("error:  expression for '/' is invalid (divide by 0)\n");
	    first_arg = NULL;
	    return;
	  }
        prev_arg->value = prev_arg->value/cur_arg->next->value;
	sprintf(prev_arg->name,"%17.12e",prev_arg->value);
        prev_arg->next = cur_arg->next->next;
        delete cur_arg->next;
        delete cur_arg;
        cur_arg = prev_arg;
      }
    prev_arg = cur_arg;
    cur_arg = cur_arg->next;
  }

cur_arg = first_paren_arg;
prev_arg = NULL;
while (cur_arg != last_paren_arg)
  {
    if (strcmp(cur_arg->name,"*") == 0)
      {
	if (prev_arg == NULL)
	  {
	    printf("error:  expression for '*' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (cur_arg->next == NULL)
	  {
	    printf("error:  expression for '*' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (prev_arg->is_number_flag == 0)
	  {
	    printf("error:  expression for '*' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
        if (cur_arg->next->is_number_flag == 0)
	  {
	    printf("error:  expression for '*' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
        prev_arg->value = prev_arg->value*cur_arg->next->value;
	sprintf(prev_arg->name,"%17.12e",prev_arg->value);
        prev_arg->next = cur_arg->next->next;
        delete cur_arg->next;
        delete cur_arg;
        cur_arg = prev_arg;
      }
    prev_arg = cur_arg;
    cur_arg = cur_arg->next;
  }

cur_arg = first_paren_arg;
prev_arg = NULL;
while (cur_arg != last_paren_arg)
  {
    if (strcmp(cur_arg->name,"+") == 0)
      {
	if (cur_arg->next == NULL)
	  {
	    printf("error:  expression for '+' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
        if (cur_arg->next->is_number_flag == 0)
	  {
	    printf("error:  expression for '+' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (prev_arg == NULL)
	  {
	    printf("error:  expression for '+' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	else if (prev_arg->is_number_flag == 0)
	  {
	    printf("error:  expression for '+' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
        else
	  {
           prev_arg->value = prev_arg->value+cur_arg->next->value;
	   sprintf(prev_arg->name,"%17.12e",prev_arg->value);
           prev_arg->next = cur_arg->next->next;
           delete cur_arg->next;
           delete cur_arg;
           cur_arg = prev_arg;
	  }
      }
    prev_arg = cur_arg;
    cur_arg = cur_arg->next;
  }

cur_arg = first_paren_arg;
prev_arg = NULL;
while (cur_arg != last_paren_arg)
  {
    if (strcmp(cur_arg->name,"-") == 0)
      {
	if (cur_arg->next == NULL)
	  {
	    printf("error:  expression for '-' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
        if (cur_arg->next->is_number_flag == 0)
	  {
	    printf("error:  expression for '-' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (prev_arg == NULL)
	  {
	    printf("error:  expression for '-' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	else if (prev_arg->is_number_flag == 0)
	  {
	    printf("error:  expression for '-' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	else
	  {
           prev_arg->value = prev_arg->value-cur_arg->next->value;
	   sprintf(prev_arg->name,"%17.12e",prev_arg->value);
           prev_arg->next = cur_arg->next->next;
           delete cur_arg->next;
           delete cur_arg;
           cur_arg = prev_arg;
	  }
      }
    prev_arg = cur_arg;
    cur_arg = cur_arg->next;
  }


if (prev_paren_arg != NULL)
   {
     if (prev_paren_arg->is_function_flag == 1)
       {
         prev_paren_arg->value = eval_func(prev_paren_arg->name,first_paren_arg->value);
         prev_paren_arg->is_number_flag = 1;
	 sprintf(prev_paren_arg->name,"%17.12e",prev_paren_arg->value);
         prev_paren_arg->next = first_paren_arg->next;
         delete first_paren_arg;
         first_paren_arg = NULL;
       }
   }

}

double EvalExpression::eval_func(const char *name, double val)
{

if (strcmp(name,"log") == 0)
   return(log10(val));
else if (strcmp(name,"log10") == 0)
   return(log10(val));
else if (strcmp(name,"ln") == 0)
   return(log(val));
else if (strcmp(name,"sin") == 0)
   return(sin(val));
else if (strcmp(name,"cos") == 0)
   return(cos(val));
else if (strcmp(name,"exp") == 0)
   return(exp(val));
else if (strcmp(name,"abs") == 0)
   return(fabs(val));
else if (strcmp(name,"tan") == 0)
   return(tan(val));
else if (strcmp(name,"atan") == 0)
   return(atan(val));
else if (strcmp(name,"sqrt") == 0)
   return(sqrt(val));
else if (strcmp(name,"sign") == 0)
  return(val >= 0.0 ? 1.0 : -1.0);
else if (strcmp(name,"round") == 0)
   return(floor(val+0.5));
else if (strcmp(name,"floor") == 0)
   return(floor(val));
else if (strcmp(name,"ceil") == 0)
   return(ceil(val));
else if (strcmp(name,"db") == 0)
   return(20.0*log10(val));
else
   {
     printf("error:  unknown function '%s'\n",name);
     return(1.0);
   }
}

void EvalExpression::set_arg_variable_vals()
{
VarList *cur_arg, *cur_var;

cur_arg = first_arg;
while (cur_arg != NULL)
   {
     cur_var = first_var;
     while (cur_var != NULL)
       {
	 if (strcmp(cur_arg->name,cur_var->name) == 0)
	   break;
	 cur_var = cur_var->next;
       }
     if (cur_var != NULL)
        {
         cur_arg->value = cur_var->value;
	 cur_arg->is_number_flag = 1;
	 cur_arg->is_function_flag = 0;
        }
     cur_arg = cur_arg->next;
   } 
}

void EvalExpression::print_arg_list()
{
VarList *cur_arg;
int i;

cur_arg = first_arg;
i = 0;
while (cur_arg != NULL)
   {
     if (cur_arg->is_number_flag == 1)
        printf("arg[%d] = '%s' --> val = %17.12e\n",i++,cur_arg->name,cur_arg->value);
     else if (cur_arg->is_function_flag == 1)
        printf("arg[%d] = '%s' --> ***function***\n",i++,cur_arg->name);
     else
        printf("arg[%d] = '%s'\n",i++,cur_arg->name);

     cur_arg = cur_arg->next;
   }
}

void EvalExpression::print_var_list()
{
VarList *cur_var;
int i;

cur_var = first_var;
i = 0;
while (cur_var != NULL)
   {
     if (cur_var->is_number_flag == 1)
        printf("arg[%d] = '%s' --> val = %17.12e\n",i++,cur_var->name,cur_var->value);
     else if (cur_var->is_function_flag == 1)
        printf("arg[%d] = '%s' --> ***function***\n",i++,cur_var->name);
     else
        printf("arg[%d] = '%s'\n",i++,cur_var->name);

     cur_var = cur_var->next;
   }
}

int EvalExpression::determine_variables(char *error_msg)
{
VarList *cur_var, *cur_arg, *cur_func;
char delim[100];
char r[MAX_CHAR_LENGTH], t[MAX_CHAR_LENGTH];


sprintf(delim,"+-/*^()0123456789.");

if (first_var != NULL)
   {
     sprintf(error_msg,"Error in 'EvalExpression.determine_variables':  first_var is != NULL!\n");
     return(1);
   }
cur_var = first_var;
cur_arg = first_arg;
cur_func = first_func;

while (cur_arg != NULL)
   {
     strtok_mp(cur_arg->name,t,r,delim);
     if (strcmp(t,"") != 0)
       {
	 cur_func = NULL;
	 if (cur_arg->next != NULL)
	   {
	     if (strcmp(cur_arg->next->name,"(") == 0)
	       {
		 cur_func = first_func;
		 while (cur_func != NULL)
		   {
		     if (strcmp(cur_arg->name,cur_func->name) == 0)
		       {
	                 cur_arg->is_function_flag = 1;
			 break;
		       }
		     cur_func = cur_func->next;
		   }
	       }
	   }
	 if (cur_func == NULL)
	   {
	     if (cur_var == NULL)
		{
		 first_var = init_varlist();
		 cur_var = first_var;
		}
	     else
		{
		 cur_var->next = init_varlist();
		 cur_var = cur_var->next;
		}
	     strcpy(cur_var->name,cur_arg->name);
	   }
       }
     cur_arg = cur_arg->next;
   }
return(0);
}

int EvalExpression::parse_into_args(char *error_msg, const char *expr)
{
int j,k,paren_count;
char expr_in[MAX_CHAR_LENGTH];
char delim[100], delim_m[100];
char r[MAX_CHAR_LENGTH], t[MAX_CHAR_LENGTH];
VarList *cur_arg;

sprintf(delim,"+-/*^()&:, ");
sprintf(delim_m,"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789.@");

if (first_arg != NULL)
   {
     sprintf(error_msg,"Error in 'EvalExpression.parse_into_args': first_arg != NULL!\n");
     return(1);
   }
cur_arg = first_arg;

strncpy(expr_in,expr,MAX_CHAR_LENGTH-1);


for (k = 0; expr_in[k] != '\0'; k++)
   {
     if (expr_in[k] == 'e' && k > 0)
       {
	 if ((expr_in[k-1] >= '0' && expr_in[k-1] <= '9') || expr_in[k-1] == '.')
	   {
	     if (expr_in[k+1] == '-')
	       expr_in[k+1] = '$';
             else if (expr_in[k+1] == '+')
               expr_in[k+1] = '%';
	   }
       }
   }



for (j = 0; expr_in[j] != '\0'; j++)
  {
    if (expr_in[j] != ' ' && expr_in[j] != '-' && expr_in[j] != '(' && expr_in[j] != '+')
    break;
  }

if (j > 0)
   {
    while(1)
       {
	 for (k = 0; expr_in[k] != '\0'; k++)
	   if (expr_in[k] != ' ')
	      break;
         if (k == j)
	   break;
         if (expr_in[k] == '(')
	   {
	     if (cur_arg == NULL)
	       {
		 first_arg = init_varlist();
		 cur_arg = first_arg;
	       }
             else
	       {
		 cur_arg->next = init_varlist();
		 cur_arg = cur_arg->next;
	       }
	     sprintf(cur_arg->name,"(");
             expr_in[k] = ' ';            
	   }        
         else if (expr_in[k] == '-')
	   {
	     if (cur_arg == NULL)
	       {
		 first_arg = init_varlist();
		 cur_arg = first_arg;
	       }
             else
	       {
		 cur_arg->next = init_varlist();
		 cur_arg = cur_arg->next;
	       }
	     sprintf(cur_arg->name,"-");
             expr_in[k] = ' ';
	   }
         else if (expr_in[k] == '+')
	   {
	     if (cur_arg == NULL)
	       {
		 first_arg = init_varlist();
		 cur_arg = first_arg;
	       }
             else
	       {
		 cur_arg->next = init_varlist();
		 cur_arg = cur_arg->next;
	       }
	     sprintf(cur_arg->name,"+");
             expr_in[k] = ' ';
	   }
       }
   }


strcpy(r,&expr_in[j]);
strtok_mp(r,t,r,delim);

while(strcmp(t,"") != 0)
  {
   if (cur_arg == NULL)
      {
       first_arg = init_varlist();
       cur_arg = first_arg;
      }
   else
      {
       cur_arg->next = init_varlist();
       cur_arg = cur_arg->next;
      }
   sprintf(cur_arg->name,"%s",t);
   strtok_mp(r,t,r,delim_m);

   if (strcmp(t,"") == 0)
     break;
   for (k = 0; t[k] != '\0'; k++)
     {
      if (cur_arg == NULL)
         {
          first_arg = init_varlist();
          cur_arg = first_arg;
         }
      else
         {
          cur_arg->next = init_varlist();
          cur_arg = cur_arg->next;
         }
      sprintf(cur_arg->name,"%c",t[k]);
     }
   strtok_mp(r,t,r,delim);
  }

// substitute numerical value for pi

cur_arg = first_arg;
while (cur_arg != NULL)
  {
  if (strcmp(cur_arg->name,"pi") == 0)
    {
    cur_arg->value = 3.14159265358979;
    cur_arg->is_number_flag = 1;
    cur_arg->is_function_flag = 0;
    }
  else
    {
    for (j = 0; cur_arg->name[j] != '\0'; j++)
       {
       if (cur_arg->name[j] == '$')
          cur_arg->name[j] = '-';
       if (cur_arg->name[j] == '%')
          cur_arg->name[j] = '+';
       }
    }
  cur_arg = cur_arg->next;
  }

paren_count = 0;
cur_arg = first_arg;
while (cur_arg != NULL)
   {
    if (strcmp(cur_arg->name,"(") == 0)
      paren_count = paren_count + 1;
    else if (strcmp(cur_arg->name,")") == 0)
      paren_count = paren_count - 1;
    cur_arg = cur_arg->next;
   }
if (paren_count != 0)
  {
   sprintf(error_msg,"Error in 'EvalExpression.parse_into_args': parenthesis mismatch\n  -> expr = '%s'\n",expr);
   first_arg = NULL;
   return(1);
  }

sprintf(delim,"0123456789.e-+");

cur_arg = first_arg;
while (cur_arg != NULL)
   {
     strtok_mp(cur_arg->name,t,r,delim);
     if (strcmp(t,"") == 0)
       {
	 if (strcmp(cur_arg->name,"+") != 0 && strcmp(cur_arg->name,"-") != 0 &&
             strcmp(cur_arg->name,"e") != 0)
	   {
	     cur_arg->value = atof(cur_arg->name);
	     cur_arg->is_number_flag = 1;
	     cur_arg->is_function_flag = 0;
	   }
       }
    cur_arg = cur_arg->next;
   }
return(0);
}


void EvalExpression::strtok_mp(char *str_in, char *t, char *r, char *delim)
{
int i,j,k;
char temp[MAX_CHAR_LENGTH];

k = 0;
for (i = 0; str_in[i] != '\0'; i++)
   {
     for (j = 0; delim[j] != '\0'; j++)
       {
	 if (str_in[i] == delim[j])
	   break;
       }
     if (delim[j] != '\0')
         break;
     if (str_in[i] != ' ')
        t[k++] = str_in[i];
   }
strcpy(temp,&str_in[i]);
strcpy(r,temp);
t[k] = '\0';
}


int EvalExpression::create_func_list()
{
VarList *cur_func;

if (first_func != NULL)
   {
     printf("Error in 'EvalExpression.create_func_list':  first_func is != NULL!\n");
     return(1);
   }
first_func = init_varlist();
cur_func = first_func;

strcpy(cur_func->name,"log");
cur_func->next = init_varlist();
cur_func = cur_func->next;
strcpy(cur_func->name,"log10");
cur_func->next = init_varlist();
cur_func = cur_func->next;
strcpy(cur_func->name,"ln");
cur_func->next = init_varlist();
cur_func = cur_func->next;
strcpy(cur_func->name,"sin");
cur_func->next = init_varlist();
cur_func = cur_func->next;
strcpy(cur_func->name,"cos");
cur_func->next = init_varlist();
cur_func = cur_func->next;
strcpy(cur_func->name,"exp");
cur_func->next = init_varlist();
cur_func = cur_func->next;
strcpy(cur_func->name,"abs");
cur_func->next = init_varlist();
cur_func = cur_func->next;
strcpy(cur_func->name,"tan");
cur_func->next = init_varlist();
cur_func = cur_func->next;
strcpy(cur_func->name,"atan");
cur_func->next = init_varlist();
cur_func = cur_func->next;
strcpy(cur_func->name,"sqrt");
cur_func->next = init_varlist();
cur_func = cur_func->next;
strcpy(cur_func->name,"sign");
cur_func->next = init_varlist();
cur_func = cur_func->next;
strcpy(cur_func->name,"round");
cur_func->next = init_varlist();
cur_func = cur_func->next;
strcpy(cur_func->name,"floor");
cur_func->next = init_varlist();
cur_func = cur_func->next;
strcpy(cur_func->name,"ceil");
cur_func->next = init_varlist();
cur_func = cur_func->next;
strcpy(cur_func->name,"db");

return(0);
} 




