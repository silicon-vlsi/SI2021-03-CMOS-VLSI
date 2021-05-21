/////////////////////////// SerialProgClass ///////////////////////////
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

#include "cppsim_classes.h"
#include "serial_prog_class.h"

SerialProgClass::SerialProgClass(const char *chip_version)
   : eval_formula()
{
first_definition = NULL;
cur_definition = NULL;

reg_only_mode = 0; // address only refers to reg space
nvm_mode = 1;  // nvmreg_v1
addr_bitwidth = 8;
addr_nvm_reg_size = 256; // should be 2^addr_bitwidth

if (reg_only_mode == 0)
   {
    addr_nvm_size = addr_nvm_reg_size/2;
    addr_reg_size = addr_nvm_reg_size/2;
   }
else
   {
    addr_nvm_size = 0;
    addr_reg_size = addr_nvm_reg_size;
   }
header_bitwidth = 16;
nvm_reg_bitwidth = 16;

if (strcmp(chip_version,"example_ic_version") == 0)
  {
// NVM0
  add_definition("enable_something","nvm0[11]");
     add_tokens("0,1",
                "off,on");
  add_definition("circuit_config","nvm0[10:8]");
     add_tokens("000,001,010,011,100,101,110,111",
                "0,1,2,3,4,5,6,7");
  add_definition("out_signals","nvm0[7:4]");
  add_tokens("0000,0001,0010,0011,0100,0101,0110,0111,    1000,1001,1010,1011,    1100,1101,1110,1111",
             "off, sig1,sig2, sig3,  sig4, sig5,sig6,sig7, \
              sig8,sig9,unused,unused,unused,unused,unused,unused");
// NVM1
  add_definition("gain_setting","nvm1[15:3]");
     add_formula("x");
     add_inv_formula("x");

// NVM2
  add_definition("gain_setting2","nvm2[9:4]");
     add_formula("(x-16)*(2^5-1)");
     add_inv_formula("x/(2^5-1)+2");
  add_definition("gain_setting3","nvm2[3:0],nvm3[15:14]");
     add_formula("(x-16)*(2^5-1)");
     add_inv_formula("x/(2^5-1)+2");

// NVM4
  add_definition("gain_setting4","nvm4[6:0],nvm5[15:0],nvm6[15:14]");
     add_formula("x*(2^24-1)/(2^(-3))");
     add_inv_formula("x/((2^24-1)/(2^(-3)))","signed_val");

// NVM7
  add_definition("gain_setting5","nvm7[14:0],nvm8[15:0]");
     add_formula("x*2^(31-4)");
     add_inv_formula("x/(2^(31-4))");
     add_xor("0010110000000000000000000000000");

// REG0
  add_definition("enable_something_else","reg0[15]");
     add_tokens("0,1","off,on");
  }
else
  {
   printf("error in 'SerialProgClass' constructor\n");
   printf("  -> chip_version '%s' is not supported\n",chip_version);
   exit(1);
  }

nvmreg_v1 = new IntMatrix("SerialProgClass","nvmreg_v1");
nvmreg_v2 = new IntMatrix("SerialProgClass","nvmreg_v2");
cur_packet = new IntVector("SerialProgClass","cur_packet");


nvmreg_v1->set_size(addr_nvm_reg_size,nvm_reg_bitwidth);
nvmreg_v2->set_size(addr_nvm_reg_size,nvm_reg_bitwidth);

cur_packet_counter = 0;
num_packet_bits = header_bitwidth + addr_bitwidth + nvm_reg_bitwidth;
cur_packet->set_length(num_packet_bits);

// header is 0x0C0C 
cur_packet->set_elem(0,0);
cur_packet->set_elem(1,0);
cur_packet->set_elem(2,0);
cur_packet->set_elem(3,0);

cur_packet->set_elem(4,1);
cur_packet->set_elem(5,1);
cur_packet->set_elem(6,0);
cur_packet->set_elem(7,0);

cur_packet->set_elem(8,0);
cur_packet->set_elem(9,0);
cur_packet->set_elem(10,0);
cur_packet->set_elem(11,0);

cur_packet->set_elem(12,1);
cur_packet->set_elem(13,1);
cur_packet->set_elem(14,0);
cur_packet->set_elem(15,0);


first_command = NULL;
prog_send_active = 0;
prog_send_active_internal = 0;
force_high = 0;
force_low = 0;
force_high_internal = 0;
force_low_internal = 0;
}

SerialProgClass::~SerialProgClass()
{
WriteCommand *cur_command,*next_command;
TokenList *cur_token, *next_token;
DefinitionList *next_definition;

cur_definition = first_definition;
while (cur_definition != NULL)
  {
   cur_token = cur_definition->first_token;
   while (cur_token != NULL)
      {
       next_token = cur_token->next;
       delete cur_token;
       cur_token = next_token;
      }
   next_definition = cur_definition->next;
   delete cur_definition;
   cur_definition = next_definition;
  }

cur_command = first_command;
while (cur_command != NULL)
   {
     next_command = cur_command->next;
     delete cur_command;
     cur_command = next_command;
   }
delete nvmreg_v1;
delete nvmreg_v2;
delete cur_packet;
}

int SerialProgClass::get_addr_bitwidth()
 {
   return(addr_bitwidth);
 }

int SerialProgClass::get_addr_size()
 {
   return(addr_nvm_reg_size);
 }

int SerialProgClass::get_addr_nvm_size()
 {
   return(addr_nvm_size);
 }

int SerialProgClass::get_addr_reg_size()
 {
   return(addr_reg_size);
 }

int SerialProgClass::get_nvm_reg_bitwidth()
 {
   return(nvm_reg_bitwidth);
 }

void SerialProgClass::remove_start_and_end_spaces_from_string(const char *name_in, char *name_out)
{
int i;

for (i = 0; name_in[i] == ' ' || name_in[i] == '\t'; i++);
strncpy(name_out,&name_in[i],MAX_DEFINITION_STRING-1);
for (i = 0; name_out[i] != '\0'; i++);
for (i--; name_out[i] == ' ' || name_out[i] == '\t'; i--);
name_out[i+1] = '\0';
}


void SerialProgClass::add_definition(const char *name_in, const char *nvmreg_name_in)
{
int i,count;
char name[MAX_DEFINITION_STRING];
char nvmreg_name[MAX_DEFINITION_STRING];
char nvmreg_string[MAX_DEFINITION_STRING];
WriteCommand *cur_command;
char *p;

// remove spaces at beginning and end of input strings
remove_start_and_end_spaces_from_string(name_in,name);
remove_start_and_end_spaces_from_string(nvmreg_name_in,nvmreg_name);

for (i = 0; name[i] == ' ' || name[i] == '\t'; i++);
 
if (strncmp(name,"nvm",3) == 0)
   {
    if (name[3] <= '9' && name[3] >= '0')
       {
        printf("error in 'SerialProgClass.add_definition':  definition name\n");
        printf("  must not begin with 'nvmx', where 'x' is an integer value\n");
        printf("  -> please change definition name '%s'\n",name);
        exit(1);
       }
   }
else if (strncmp(name,"reg",3) == 0)
   {
    if (name[3] <= '9' && name[3] >= '0')
       {
        printf("error in 'SerialProgClass.add_definition':  definition name\n");
        printf("  must not begin with 'regx', where 'x' is an integer value\n");
        printf("  -> please change definition name '%s'\n",name);
        exit(1);
       }
   }

cur_definition = first_definition;
if (cur_definition == NULL)
  {
   first_definition = new DefinitionList;
   cur_definition = first_definition;
  }
else
  {
   while (cur_definition->next != NULL)
     {
      if (strcmp(cur_definition->name,name) == 0)
	{
         printf("error in 'SerialProgClass.add_definition':  definition name\n");
         printf("  must not be repeated!\n");
         printf("  -> please change definition name '%s'\n",name);
         exit(1);
        }
      cur_definition = cur_definition->next;
     }
  cur_definition->next = new DefinitionList;
  cur_definition = cur_definition->next; 
  }

strncpy(cur_definition->name,name,MAX_DEFINITION_STRING-1);
strncpy(cur_definition->nvmreg_name,nvmreg_name,MAX_DEFINITION_STRING-1);
strcpy(cur_definition->xor_pattern,"NULL");
strcpy(cur_definition->formula,"NULL");
strcpy(cur_definition->inv_formula,"NULL");
cur_definition->first_token = NULL;
cur_definition->xor_pattern_exists_flag = 0;
cur_definition->formula_exists_flag = 0;
cur_definition->inv_formula_exists_flag = 0;
cur_definition->inv_formula_sign_flag = 0;
cur_definition->clear_internal_nvmreg_block_flag = 0;
cur_definition->clear_internal_nvmreg_block_first_addr = -1;
cur_definition->clear_internal_nvmreg_block_last_addr = -1;
cur_definition->command_one = NULL;
cur_definition->command_two = NULL;
cur_definition->command_three = NULL;
cur_definition->command_four = NULL;
cur_definition->overall_sequence_length = 0;
cur_definition->next = NULL;

////  Evaluate nvmreg_name, create and evaluate associated commands
strncpy(nvmreg_string,nvmreg_name,MAX_DEFINITION_STRING-1);
p = strtok(nvmreg_string,"\t ,;");
count = 0;
while(p != NULL)
  {
   p = strtok(NULL,"\t ,;");
   count++;
  }
if (count > 4)
  {
   printf("error in 'SerialProgClass.add_definition':\n");
   printf("  definition name '%s'\n",name);
   printf("  is associated with more than 4 nvm/reg values\n");
   printf("  -> in this case, nvm/reg specification is:\n");
   printf("     '%s'   (count = %d)\n",nvmreg_name,count);
   printf("  -> please change definition '%s' description\n",name);
   exit(1);
  }

strncpy(nvmreg_string,nvmreg_name,MAX_DEFINITION_STRING-1);
p = strtok(nvmreg_string,"\t ,;");
i = 1;
cur_command = NULL;
while(p != NULL)
  {
   if (i == count)
      {
       cur_definition->command_one = new WriteCommand;
       cur_command = cur_definition->command_one;
      }
   else if (i == (count-1))
      {
       cur_definition->command_two = new WriteCommand;
       cur_command = cur_definition->command_two;
      }
   else if (i == (count-2))
      {
       cur_definition->command_three = new WriteCommand;
       cur_command = cur_definition->command_three;
      }
   else if (i == (count-3))
      {
       cur_definition->command_four = new WriteCommand;
       cur_command = cur_definition->command_four;
      }
   else
      {
       printf("error in 'SerialProgClass.add_definition':\n");
       printf("  definition name '%s'\n",name);
       printf("  is associated with more than 4 nvm/reg values\n");
       printf("  -> in this case, nvm/reg specification is:\n");
       printf("     '%s'   (count = %d)\n",nvmreg_name,count);
       printf("  -> please change definition '%s' description\n",name);
       exit(1);
     }

   //printf("p = '%s'\n",p);
   strcpy(cur_command->address,p);
   cur_command->value = 0; // set to 0 for now
   cur_command->address_val = -1;
   cur_command->msb_val = -1;
   cur_command->lsb_val = -1;
   cur_command->bit_width = -1;
   cur_command->clear_internal_nvmreg_block_flag = 0;
   cur_command->clear_internal_nvmreg_block_first_addr = -1;
   cur_command->clear_internal_nvmreg_block_last_addr = -1;
   cur_command->next = NULL;

   // evaluate_command computes each of the above values (except 'value')
   if (evaluate_command(cur_command))
      {
        printf("  -> please change definition '%s' description\n",name);
        exit(1);
      }
   cur_definition->overall_sequence_length += (cur_command->msb_val -
            cur_command->lsb_val + 1);
   p = strtok(NULL,"\t ,;");
   i++;
  }

}

int SerialProgClass::set(char *definition_name_in, char *value_in)
{
DefinitionList *cur_definition;
TokenList *cur_token;
WriteCommand *cur_command,*next_command;
int i,j,value_int,token_index, temp_int, found_token_flag;
double value_dbl, temp_dbl;
int value_int_flag, value_dbl_flag;
char nvmreg_subname[MAX_DEFINITION_STRING];
char token_string[MAX_TOKEN_STRING], bit_pattern_string[MAX_TOKEN_STRING];
char *p;
char value[MAX_TOKEN_STRING];
char definition_name[MAX_DEFINITION_STRING];
int bit_pattern_val,count;

//printf("set: '%s' to '%s'\n",definition_name_in, value_in);

// remove spaces at beginning and end of input strings
remove_start_and_end_spaces_from_string(definition_name_in,definition_name);
remove_start_and_end_spaces_from_string(value_in,value);

if (strcmp(definition_name,"prog") == 0)
   {
    if (strcmp(value,"send") == 0)
      {
       force_high_internal = 0;
       force_low_internal = 0;
       prog_send();
       return(0);
      }
    else if (strcmp(value,"force_high") == 0)
      {
       force_high_internal = 1;
       force_low_internal = 0;
       return(0);
      }
    else if (strcmp(value,"force_low") == 0)
      {
       force_high_internal = 0;
       force_low_internal = 1;
       return(0);
      }
    else if (strcmp(value,"release") == 0)
      {
       force_high_internal = 0;
       force_low_internal = 0;
       return(0);
      }
   }

cur_definition = first_definition;
while (cur_definition != NULL)
   {
    //printf("cur_definition = '%s'\n",cur_definition->name);
   if (strcmp(definition_name,cur_definition->name) == 0)
       break;
   cur_definition = cur_definition->next;
   }

if (cur_definition == NULL)
   {
    if ((strncmp(definition_name,"nvm",3) == 0 ||
         strncmp(definition_name,"reg",3) == 0) && 
         definition_name[3] <= '9' && definition_name[3] >= '0')
       {
        if (is_integer(value))
           {
            for (i = 0; value[i] != '\0'; i++);
            bit_pattern_val = 0;
            for (i--,j = 0; i >= 0; i--,j++)
              {
               if (value[i] == '1')
                   bit_pattern_val += 1<<j;
               else if (value[i] != '0')
                  {
                   printf("error in 'SerialProgClass.set':  incorrect bit_pattern specification\n");
                   printf("   for nvm/reg definition '%s', which\n",definition_name);
                   printf("   is incorrectly specified (i.e., has non '0' or '1' characters)\n");
                   printf("   -> bit_pattern reads '%s'\n",value);
		   return(1);
                   // exit(1);
                  }
               }
            set_reg(definition_name,bit_pattern_val);
           }
        else
           {
            printf("error in 'SerialProgClass.set':  invalid specification for\n");
            printf("  nvm/reg definition '%s'\n",definition_name);
            printf("  -> must specify binary sequence instead of '%s'\n",value);
	    return(1);
            // exit(1);
           }
       }
    else  
       {
        printf("error in 'SerialProgClass.set':  unrecognized definition name\n");
        printf("   -> definition '%s' is unknown\n",definition_name);
	return(1);
        // exit(1);
       }
   return(0);
   }

value_int_flag = 0;
value_dbl_flag = 0;
if (is_integer(value))
    {
    value_int_flag = 1;
    value_int = atoi(value);
    }
else if (is_double(value))
   {
    value_dbl_flag = 1;
    value_dbl = atof(value);
   }
// printf("value_int = %d flag = %d, value_dbl = %5.3e flag = %d\n",value_int,value_int_flag, value_dbl, value_dbl_flag);

if (cur_definition->formula_exists_flag == 1)
   {
    if (value_dbl_flag == 1)
        eval_formula.eval(cur_definition->formula,"x",value_dbl);
    else if (value_int_flag == 1)
        eval_formula.eval(cur_definition->formula,"x",(double) value_int);
    else
       {
          printf("error in 'SerialProgClass.set':  inputs for formulas must be numerical in value\n");
          printf("   input of x = '%s' was specified for\n",value);
          printf("   formula '%s'\n",cur_definition->formula);
          printf("   in definition '%s'\n",definition_name);
          printf("   -> please correct the set command applied to definition '%s'\n",definition_name);
	  return(1);
          // exit(1);
       }
    /*****
    if (eval_formula.out < 0.0)
       {
          printf("error in 'SerialProgClass.set':  formula results must be >= 0\n");
          printf("   input of x = '%s' was specified for\n",value);
          printf("   formula '%s'\n",cur_definition->formula);
          printf("   resulting in a formula output of '%15.0f'\n",eval_formula.out);
          printf("   in definition '%s'\n",definition_name);
          printf("   -> please correct the formula for definition '%s'\n",definition_name);
          return(1);
          // exit(1);
       }
    ******/
    if (fabs(eval_formula.out) > 2147483647.0)
       {
          printf("error in 'SerialProgClass.set':  formula results must be < (2^31-1)\n");
          printf("   so that they can be represented by a 31-bit integer value\n");
          printf("   input of x = '%s' was specified for\n",value);
          printf("   formula '%s'\n",cur_definition->formula);
          printf("   resulting in a formula output of '%15.0f'\n",eval_formula.out);
          printf("   in definition '%s'\n",definition_name);
          printf("   -> please correct the formula for definition '%s'\n",definition_name);
	  return(1);
          // exit(1);
       }
    temp_int = ((int) floor(eval_formula.out + 0.5));
    //       printf("formula out = %12.1f, int = %d\n",eval_formula.out,temp_int);


    count = 1;
    cur_command = cur_definition->command_one;
    i = 0;

    while (cur_command != NULL)
      {
       bit_pattern_val = 0;
       if (cur_definition->xor_pattern_exists_flag == 1)
	 {
          for (j = 0; j < cur_command->bit_width; j++,i++)
            {
	      if (cur_definition->xor_pattern[i] == '\0')
		{
                 printf("error in 'SerialProgClass.set':  xor_pattern length is mismatched\n");
		 printf("   to bit_pattern length\n");
		 printf("   -> this should never happen!\n");
		 exit(1);
		}
	     if (cur_definition->xor_pattern[i] == '1')
                 bit_pattern_val += (((1<<i) & temp_int) == 0 ? (1<<j) : 0);
	     else
                 bit_pattern_val += (((1<<i) & temp_int) == 0 ? 0 : (1<<j));
            }
	 }
       else
	 {
          for (j = 0; j < cur_command->bit_width; j++,i++)
            {
             bit_pattern_val += (((1<<i) & temp_int) == 0 ? 0 : (1<<j));
            }
	 }

        if (count == 1)
           next_command = cur_definition->command_two;
        else if (count == 2)
           next_command = cur_definition->command_three;
        else if (count == 3)
           next_command = cur_definition->command_four;
        else
           next_command = NULL;

        if (next_command == NULL)
	  {
            cur_command->clear_internal_nvmreg_block_flag = 
                  cur_definition->clear_internal_nvmreg_block_flag;
            cur_command->clear_internal_nvmreg_block_first_addr = 
                  cur_definition->clear_internal_nvmreg_block_first_addr;
            cur_command->clear_internal_nvmreg_block_last_addr = 
                  cur_definition->clear_internal_nvmreg_block_last_addr;
	  }

        set_reg(cur_command,bit_pattern_val);
	cur_command = next_command;
        count++;
      }

   }
else
   {
   found_token_flag = 0;
   cur_token = cur_definition->first_token;
   while (cur_token != NULL)
     {
      strncpy(token_string,cur_token->token_val,MAX_TOKEN_STRING-1);
      p = strtok(token_string,"\t ,;");
      token_index = 0;
      while(p != NULL)
        {
         if (value_int_flag == 1)
           {
            if (is_integer(p))
               {
                temp_int = atoi(p);
                if (value_int == temp_int)
                   found_token_flag = 1;
               }
            else if (is_double(p))
               {
                temp_dbl = atof(p);
                temp_int = (int) floor(temp_dbl + 0.5);
                if (fabs(temp_dbl - temp_int) < 1e-6)
                   {
                    if (value_int == temp_int)
                       found_token_flag = 1;
                   }
               }
           }
         else if (value_dbl_flag == 1)
           {
            if (is_double(p))
               {
                temp_dbl = atof(p);
                if (fabs(value_dbl - temp_dbl) < 1e-6)
                   {
                    found_token_flag = 1;
                   }
               }
           }
         else if (strcmp(p,value) == 0)
             found_token_flag = 1;

         if (found_token_flag == 1)
             break;
         token_index++;
         p = strtok(NULL,"\t ,;");
        }
      if (found_token_flag == 1)
          break;
      cur_token = cur_token->next;
     }
   if (found_token_flag == 0)
         {
          printf("error in 'SerialProgClass.set':  can't find token!\n");
          printf("  -> looking for token '%s' for definition '%s'\n",
               value, definition_name);
          printf("     but it doesn't seem to exist\n");
          printf("     valid tokens for definition '%s':\n",definition_name);
          cur_token = cur_definition->first_token;
          while (cur_token != NULL)
             {
              strncpy(token_string,cur_token->token_val,MAX_TOKEN_STRING-1);
              p = strtok(token_string,"\t ,;");
              while(p != NULL)
                {
                 printf("    '%s'\n",p);
                 p = strtok(NULL,"\t ,;");
                }
              cur_token = cur_token->next;
             }
	  return(1);
          // exit(1);
         }
    strncpy(bit_pattern_string,cur_token->bit_pattern,MAX_TOKEN_STRING-1);
    p = strtok(bit_pattern_string,"\t ,;");
    i = 0;
    while(p != NULL)
       {
        if (i == token_index)
            break;
        p = strtok(NULL,"\t ,;");
        i++;
       }
    if (p == NULL)
       {
        printf("error in 'SerialProgClass.set':  no bit pattern found");
        printf("   for token '%s'\n",value);
        printf("   for definition '%s'\n",definition_name);
        printf("   -> bit pattern for token '%s' is missing!\n",value);
        printf("   list of tokens/bit patterns for definition '%s':\n",definition_name);
        cur_token = cur_definition->first_token;
        while (cur_token != NULL)
           {
            printf("    tokens '%s'\n     have bit patterns '%s'\n",
                   cur_token->token_val,cur_token->bit_pattern);
            cur_token = cur_token->next;
           }
	return(1);
        // exit(1);
       }
    for (i = 0; p[i] != '\0'; i++);
    if (i != cur_definition->overall_sequence_length)
       {
          printf("error in 'SerialProgClass.set':  incorrect bit_pattern specification\n");
          printf("   bit pattern for token '%s'\n",value);
          printf("   in definition '%s'\n",definition_name);
          printf("   has incorrect number of bits = '%d' (should be '%d')\n",i,
               cur_definition->overall_sequence_length);
          printf("   -> bit_pattern reads '%s'\n",p);
	  return(1);
          // exit(1);
       }
    if (cur_definition->xor_pattern_exists_flag == 1)
	 {
          for (j = 0; p[j] != '\0'; j++)
            {
	      if (cur_definition->xor_pattern[j] == '\0')
		{
                 printf("error in 'SerialProgClass.set':  xor_pattern length is mismatched\n");
		 printf("   to bit_pattern length\n");
		 printf("   -> this should never happen!\n");
		 exit(1);
		}
	      if (cur_definition->xor_pattern[j] == '1')
		 p[j] = (p[j] == '0') ? '1' : '0';
	    }
	 }
//zippy
    count = 1;
    cur_command = cur_definition->command_one;

    while (cur_command != NULL)
      {
       bit_pattern_val = 0;
       for (i--,j = 0; i >= 0; i--,j++)
         {
         if (p[i] == '1')
            bit_pattern_val += 1<<j;
         else if (p[i] != '0')
           {
            printf("error in 'SerialProgClass.set':  incorrect bit_pattern specification\n");
            printf("   bit pattern for token '%s'\n",value);
            printf("   in definition '%s'\n",definition_name);
            printf("   is incorrectly specified (i.e., has non '0' or '1' characters)\n");
            printf("   -> bit_pattern reads '%s'\n",p);
	    return(1);
            // exit(1);
           }
         if (j >= (cur_command->bit_width-1))
            break;
         }

        if (count == 1)
           next_command = cur_definition->command_two;
        else if (count == 2)
           next_command = cur_definition->command_three;
        else if (count == 3)
           next_command = cur_definition->command_four;
        else
           next_command = NULL;

        if (next_command == NULL)
	  {
            cur_command->clear_internal_nvmreg_block_flag = 
                  cur_definition->clear_internal_nvmreg_block_flag;
            cur_command->clear_internal_nvmreg_block_first_addr = 
                  cur_definition->clear_internal_nvmreg_block_first_addr;
            cur_command->clear_internal_nvmreg_block_last_addr = 
                  cur_definition->clear_internal_nvmreg_block_last_addr;
	  }

        set_reg(cur_command,bit_pattern_val);
	cur_command = next_command;
        count++;
      }
//    printf("Set for definition: '%s' is token '%s'\n",definition_name, value);
//    printf("   -> corresponding bit_pattern '%s' (val = %d)\n",p,bit_pattern_val);
//exit(1);
   }
return(0);
}


int SerialProgClass::is_integer(char *value)
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


int SerialProgClass::is_double(char *value)
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


void SerialProgClass::add_tokens(const char *bit_pattern_in, const char *token_val_in)
{
TokenList *cur_token;
char bit_pattern[MAX_TOKEN_STRING];
char temp_bit_pattern[MAX_TOKEN_STRING];
char token_val[MAX_TOKEN_STRING];
char temp_token_val[MAX_TOKEN_STRING];
char *p_pattern, *p_token;
int i,j,count;

if (cur_definition == NULL)
  {
   printf("error in 'SerialProgClass::add_tokens':\n");
   printf("   -> you must call an 'add_definition()' function before\n");
   printf("      calling 'add_tokens()'\n");
   exit(1);
  }

// remove spaces at beginning and end of input strings
remove_start_and_end_spaces_from_string(bit_pattern_in,bit_pattern);
remove_start_and_end_spaces_from_string(token_val_in,token_val);

strncpy(temp_bit_pattern,bit_pattern,MAX_TOKEN_STRING-1);
p_pattern = strtok(temp_bit_pattern,"\t ,;");
count = 0;
while(p_pattern != NULL)
  {
   for (i = 0; p_pattern[i] != '\0'; i++)
   {
    if (p_pattern[i] != '0' && p_pattern[i]!= '1')
       {
        printf("error in 'SerialProgClass.add_tokens':\n");
        printf("  invalid bit_pattern '%s' (i.e. non-binary sequence)\n",
               p_pattern);

        strncpy(temp_token_val,token_val,MAX_TOKEN_STRING-1);
        p_token = strtok(temp_token_val,"\t ,;");
        j = 0;
        while(p_token != NULL)
           {
           if (j == count)
              break;
           p_token = strtok(NULL,"\t ,;");
           j++;
           }
        if (p_token == NULL)
           printf("  associated with token string '%s'\n",token_val);
        else
           {
           printf("  associated with token '%s'\n",p_token);
           printf("  within token string '%s'\n",token_val);
           }

        printf("  for definition '%s'\n",cur_definition->name);
        printf("  -> please fix 'add_tokens' statement associated with\n");
        printf("     definition '%s' so that it has binary sequences\n",
             cur_definition->name);
        printf("     instead of '%s'\n",bit_pattern);
        exit(1);
       }
   }
   if (i != cur_definition->overall_sequence_length)
      {
        printf("error in 'SerialProgClass.add_tokens':\n");
        printf("  invalid length of '%d' for bit_pattern '%s'\n",i,p_pattern);

        strncpy(temp_token_val,token_val,MAX_TOKEN_STRING-1);
        p_token = strtok(temp_token_val,"\t ,;");
        j = 0;
        while(p_token != NULL)
           {
           if (j == count)
              break;
           p_token = strtok(NULL,"\t ,;");
           j++;
           }
        if (p_token == NULL)
           printf("  associated with token string '%s'\n",token_val);
        else
           {
           printf("  associated with token '%s'\n",p_token);
           printf("  within token string '%s'\n",token_val);
           }
        printf("  for definition '%s'\n",cur_definition->name);
        printf("  -> please fix 'add_tokens' statement associated with\n");
        printf("     definition '%s' so that bit patterns have length '%d'\n",
                    cur_definition->name,cur_definition->overall_sequence_length);
        printf("     note that full bit pattern is currently '%s'\n",bit_pattern);
        exit(1);
      }
   p_pattern = strtok(NULL,"\t ,;");
   count++;
  }

strncpy(temp_token_val,token_val,MAX_TOKEN_STRING-1);
p_token = strtok(temp_token_val,"\t ,;");
i = 0;
while(p_token != NULL)
  {
   p_token = strtok(NULL,"\t ,;");
   i++;
  }
if (i != count)
  {
   printf("error in 'SerialProgClass.add_tokens':\n");
   printf("  mismatch between number of tokens = '%d'\n",i);
   printf("  versus number of bit patterns = '%d'\n",count);
   printf("  for token '%s'\n",token_val);
   printf("  and bit pattern '%s'\n",bit_pattern);
   printf("  within definition '%s'\n",cur_definition->name);
   printf("  -> please fix 'add_tokens' statement associated with\n");
   printf("     definition '%s' so that the number of tokens\n",
                    cur_definition->name);
   printf("     is the same as the number of bit patterns\n");
   exit(1);
  }

cur_token = cur_definition->first_token;
if (cur_token == NULL)
  {
   cur_definition->first_token = new TokenList;
   cur_token = cur_definition->first_token;
  }
else
  {
   while (cur_token->next != NULL)
      cur_token = cur_token->next;
  cur_token->next = new TokenList;
  cur_token = cur_token->next; 
  }
strncpy(cur_token->bit_pattern,bit_pattern,MAX_TOKEN_STRING-1);
strncpy(cur_token->token_val,token_val,MAX_TOKEN_STRING-1);
cur_token->next = NULL;

}


void SerialProgClass::add_xor(const char *xor_pattern)
{
int i,j;

if (cur_definition == NULL)
  {
   printf("error in 'SerialProgClass::add_xor':\n");
   printf("   -> you must call an 'add_definition()' function before\n");
   printf("      calling 'add_xor()'\n");
   exit(1);
  }

for (i = 0; xor_pattern[i] != '\0'; i++);
if (i >= MAX_CHAR_LENGTH)
  {
   printf("error in 'SerialProgClass::add_xor':\n");
   printf("   xor_pattern string is too long!\n");
   printf("   (must be less than %d as set by MAX_CHAR_LENGTH)\n",MAX_CHAR_LENGTH);
   printf("   -> either make xor_pattern shorter, or increase MAX_CHAR_LENGTH in source code\n");
   printf("      for xor_pattern: '%s'\n",xor_pattern);
   printf("      associated with definition '%s'\n",cur_definition->name);
   exit(1);
  }
if (i != cur_definition->overall_sequence_length)
  {
   printf("error in 'SerialProgClass.add_xor':\n");
   printf("  invalid length of '%d' for xor_pattern '%s'\n",i,xor_pattern);
   printf("  for definition '%s'\n",cur_definition->name);
   printf("  -> please fix 'add_xor' statement associated with\n");
   printf("     definition '%s' so that xor pattern has length '%d'\n",
               cur_definition->name,cur_definition->overall_sequence_length);
   exit(1);
  }

// reverse order of bits for correct operation later
for (j = 0,i--; i >= 0; j++,i--)
  cur_definition->xor_pattern[j] = xor_pattern[i];
cur_definition->xor_pattern[j] = '\0';

// printf("input pattern '%s'\n",xor_pattern);
// printf("->out pattern '%s'\n",cur_definition->xor_pattern);
cur_definition->xor_pattern_exists_flag = 1;
}

void SerialProgClass::add_clear_internal_nvmreg_block(int first_addr, int last_addr)
{
int i,j;

if (cur_definition == NULL)
  {
   printf("error in 'SerialProgClass::add_clear_internal_nvmreg_block':\n");
   printf("   -> you must call an 'add_definition()' function before\n");
   printf("      calling 'add_clear_internal_nvmreg_block()'\n");
   exit(1);
  }

if (first_addr > last_addr)
   {
   printf("error in 'SerialProgClass::add_clear_internal_nvmreg_block':\n");
   printf("   first_addr must be <= last_addr\n");
   printf("   in this case, first_addr = %d, last_addr = %d\n",first_addr, last_addr);
   exit(1);
   }
if (first_addr < 0)
   {
   printf("error in 'SerialProgClass::add_clear_internal_nvmreg_block':\n");
   printf("   first_addr must be >= 0\n");
   printf("   in this case, first_addr = %d\n",first_addr);
   exit(1);
   }
cur_definition->clear_internal_nvmreg_block_flag = 1;
cur_definition->clear_internal_nvmreg_block_first_addr = first_addr;
cur_definition->clear_internal_nvmreg_block_last_addr = last_addr;
}

void SerialProgClass::add_formula(const char *formula)
{
int i;

if (cur_definition == NULL)
  {
   printf("error in 'SerialProgClass::add_formula':\n");
   printf("   -> you must call an 'add_definition()' function before\n");
   printf("      calling 'add_formula()'\n");
   exit(1);
  }

for (i = 0; formula[i] != '\0'; i++);
if (i >= MAX_CHAR_LENGTH)
  {
   printf("error in 'SerialProgClass::add_formula':\n");
   printf("   formula string is too long!\n");
   printf("   (must be less than %d as set by MAX_CHAR_LENGTH)\n",MAX_CHAR_LENGTH);
   printf("   -> either make formula shorter, or increase MAX_CHAR_LENGTH in source code\n");
   printf("      for formula: '%s'\n",formula);
   printf("      associated with definition '%s'\n",cur_definition->name);
   exit(1);
  }
strncpy(cur_definition->formula,formula,MAX_CHAR_LENGTH-1);
cur_definition->formula_exists_flag = 1;
}

void SerialProgClass::add_inv_formula(const char *inv_formula, const char *signed_val)
{
add_inv_formula(inv_formula);
cur_definition->inv_formula_sign_flag = 1;
}

void SerialProgClass::add_inv_formula(const char *inv_formula)
{
int i;

if (cur_definition == NULL)
  {
   printf("error in 'SerialProgClass::add_inv_formula':\n");
   printf("   -> you must call an 'add_definition()' function before\n");
   printf("      calling 'add_inv_formula()'\n");
   exit(1);
  }

if (cur_definition->formula_exists_flag != 1)
  {
   printf("error in 'SerialProgClass::add_inv_formula':\n");
   printf("   you must set a 'formula' BEFORE setting 'inv_formula'!\n");
   printf("   -> add a 'add_formula' statement before\n");
   printf("      inv_formula: '%s'\n",inv_formula);
   printf("      associated with definition '%s'\n",cur_definition->name);
   exit(1);
  }

for (i = 0; inv_formula[i] != '\0'; i++);
if (i >= MAX_CHAR_LENGTH)
  {
   printf("error in 'SerialProgClass::add_inv_formula':\n");
   printf("   inv_formula string is too long!\n");
   printf("   (must be less than %d as set by MAX_CHAR_LENGTH)\n",MAX_CHAR_LENGTH);
   printf("   -> either make formula shorter, or increase MAX_CHAR_LENGTH in source code\n");
   printf("      for formula: '%s'\n",inv_formula);
   printf("      associated with definition '%s'\n",cur_definition->name);
   exit(1);
  }
strncpy(cur_definition->inv_formula,inv_formula,MAX_CHAR_LENGTH-1);
cur_definition->inv_formula_exists_flag = 1;
}


// combines all following commands with same address
void SerialProgClass::load_command_into_packet(WriteCommand *cur_command,IntVector *cur_packet)
{
int i,j;
int new_val, lsb_val, msb_val, address_val;
int mask;
int first_addr, last_addr;
WriteCommand *same_address_command,*prev_command;

if (cur_command == NULL)
   {
     printf("error in 'load_command_into_packet': cur_command == NULL!\n");
     exit(1);
   }
if (cur_packet == NULL)
   {
     printf("error in 'load_command_into_packet': cur_packet == NULL!\n");
     exit(1);
   }

address_val = cur_command->address_val;


same_address_command = cur_command;
prev_command = cur_command;

while (same_address_command != NULL)
   {
     if (same_address_command->address_val != address_val)
       {
	 prev_command = same_address_command;
	 same_address_command = same_address_command->next;
         continue;
       }
    new_val = same_address_command->value;
    lsb_val = same_address_command->lsb_val;
    msb_val = same_address_command->msb_val;

    // set memory based on this command
    for (i = 0, j = lsb_val; j <= msb_val; i++, j++)
       {
        mask = 1<<i;
        if (nvm_mode == 1)
           nvmreg_v1->set_elem(address_val,j,(mask & new_val) == 0 ? 0 : 1);
        else if (nvm_mode == 2)
           nvmreg_v2->set_elem(address_val,j,(mask & new_val) == 0 ? 0 : 1);
        }
    if (same_address_command->clear_internal_nvmreg_block_flag == 1)
      {
        first_addr = same_address_command->clear_internal_nvmreg_block_first_addr;
        last_addr = same_address_command->clear_internal_nvmreg_block_last_addr;
        for (i = first_addr; i <= last_addr; i++)
	  {
	    for (j = 0; j < nvm_reg_bitwidth; j++)
	      {
              if (nvm_mode == 1)
                 nvmreg_v1->set_elem(i,j,0);
              else if (nvm_mode == 2)
                 nvmreg_v2->set_elem(i,j,0);
	      }
	  }
      }

    if (same_address_command != cur_command)
      {
	// remove commands which have same address once their information is obtained
       prev_command->next = same_address_command->next;
       delete same_address_command;
       same_address_command = prev_command->next;       
      }
    else
      {
       prev_command = same_address_command;
       same_address_command = same_address_command->next;
      }
   }

// fill in the address bits to packet
for (i = 0; i < addr_bitwidth ; i++)
  {
  mask = 1<<i;
  cur_packet->set_elem(header_bitwidth+addr_bitwidth-i-1,(mask & address_val) == 0 ? 0 : 1);
  }

// fill in nvm/reg bit values to packet
for (i = 0; i < nvm_reg_bitwidth ; i++)
  {
  if (nvm_mode == 1)
      cur_packet->set_elem(header_bitwidth+addr_bitwidth+nvm_reg_bitwidth-i-1,nvmreg_v1->get_elem(address_val,i));
  else if (nvm_mode == 2)
      cur_packet->set_elem(header_bitwidth+addr_bitwidth+nvm_reg_bitwidth-i-1,nvmreg_v2->get_elem(address_val,i));
  }
}


int SerialProgClass::evaluate_command(WriteCommand *cur_command)
{
int i,j;
char address[50];
char temp[50];
int addr_msb, addr_size;

if (cur_command == NULL)
   {
     printf("error in 'evaluate_command': cur_command == NULL!\n");
     exit(1);
   }
strcpy(address,cur_command->address);

for (i = 0; address[i] == ' ' || address[i] == '\t'; i++);
if (strncmp(&address[i],"nvm",3) == 0)
   {
     addr_msb = 1;
     addr_size = addr_nvm_size;
   }
else if (strncmp(&address[i],"reg",3) == 0)
   {
     addr_msb = 0;
     addr_size = addr_reg_size;
   }
else
   {
    printf("error in 'evaluate_command':  invalid address!\n");
    printf("   -> address must be of form nvmx[m:n] or regx[m:n]\n");
    printf("      (such as nvm4[3] or nvm28[7:5] or reg4[9:3])\n");
    printf("      in this case, address is '%s'\n",address);
    return(1);
   }

for (j = 0, i += 3; address[i] != '[' && address[i] != '\0'; j++,i++)
     temp[j] = address[i];
if (address[i] == '\0')
   {
    ////  removed error below:  MHP 8/12/2015 ////
    // printf("error in 'evaluate_command':  invalid address!\n");
    // printf("   -> address must be of form nvmx[m:n] or regx[m:n]\n");
    // printf("      (such as nvm4[3] or nvm28[7:5] or reg4[9:3])\n");
    // printf("      in this case, address is '%s'\n",address);
    // return(1);
    i--;
   }
temp[j] = '\0';
//printf("temp = '%s'\n",temp);
cur_command->address_val = atoi(temp);
if (cur_command->address_val >= addr_size)
   {
    printf("error in 'evaluate_command':  invalid address!\n");
    printf("   -> address number must be < %d\n", addr_size);
    printf("      in this case, address is '%s'\n",address);
    return(1);
   }

if (addr_msb == 1 && reg_only_mode == 0)
  {
   //cur_command->address_val += 1<<5;
   cur_command->address_val += addr_reg_size;
  }
//printf("address_val = %d\n",cur_command->address_val);

for (j = 0, i += 1; address[i] != ':' && address[i] != ']' && address[i] != '\0'; j++,i++)
   temp[j] = address[i];
if (address[i] == '\0')
   {
    ////  removed error below:  MHP 8/11/2015 ////
    //printf("error in 'evaluate_command':  invalid address!\n");
    //printf("   -> address must be of form nvmx[m:n] or regx[m:n]\n");
    //printf("      (such as nvm4[3] or nvm28[7:5] or reg4[9:3])\n");
    //printf("      in this case, address is '%s'\n",address);
    //return(1);
    cur_command->msb_val = nvm_reg_bitwidth-1;
    cur_command->lsb_val = 0;
   }
else
   {
    temp[j] = '\0';     
    //printf("temp = '%s'\n",temp);
    cur_command->msb_val = atoi(temp);
    if (address[i] == ']')
       {
        cur_command->lsb_val = cur_command->msb_val;
       }
    else
       {
        for (j = 0, i += 1; address[i] != ']' && address[i] != '\0'; j++,i++)
            temp[j] = address[i];
        if (address[i] == '\0')
           {
            printf("error in 'load_command_into_packet':  invalid address!\n");
            printf("   -> address must be of form nvmx[m:n] or regx[m:n]\n");
            printf("      (such as nvm4[3] or nvm28[7:5] or reg4[9:3])\n");
            printf("      in this case, address is '%s'\n",address);
            return(1);
           }
        temp[j] = '\0';     
//      printf("temp = '%s'\n",temp);
        cur_command->lsb_val = atoi(temp);
       }
   }
if (cur_command->msb_val < cur_command->lsb_val)
   {
   j = cur_command->msb_val;
   cur_command->msb_val = cur_command->lsb_val;
   cur_command->lsb_val = j;
  }
if (cur_command->msb_val >= nvm_reg_bitwidth || cur_command->lsb_val >= nvm_reg_bitwidth)
   {
    printf("error in 'load_command_into_packet':  invalid data bit designation!\n");
    printf("   -> address must be of form nvmx[m:n] or regx[m:n]\n");
    printf("      where m and n are less than %d\n", nvm_reg_bitwidth);
    printf("      in this case, m is '%d' and n is '%d'\n",cur_command->msb_val, cur_command->lsb_val);
    printf("      for address '%s'\n",address);
    return(1);
   }

cur_command->bit_width = cur_command->msb_val - cur_command->lsb_val + 1;
return(0);
}


int SerialProgClass::get(char *definition_name_in, char *value)
{
int i,j;
DefinitionList *cur_definition;
char bit_pattern_string[MAX_TOKEN_STRING],temp_string[MAX_TOKEN_STRING];
char temp_string2[MAX_TOKEN_STRING];
char definition_name[MAX_DEFINITION_STRING];
char nvmreg_subname[MAX_DEFINITION_STRING];
int token_index;
TokenList *cur_token;
char token_string[MAX_TOKEN_STRING];
int bit_pattern_value, found_token_flag;
char *p;

//printf("definition_name = %s\n",definition_name_in);
// remove spaces at beginning and end of input strings
remove_start_and_end_spaces_from_string(definition_name_in,definition_name);

cur_definition = first_definition;
while (cur_definition != NULL)
   {
//printf("cur_definition = '%s'\n",cur_definition->name);
   if (strcmp(definition_name,cur_definition->name) == 0)
       break;
   cur_definition = cur_definition->next;
   }

if (cur_definition == NULL)
   {
    if ((strncmp(definition_name,"nvm",3) == 0 ||
         strncmp(definition_name,"reg",3) == 0) && 
         definition_name[3] <= '9' && definition_name[3] >= '0')
       {
        if (get_reg(definition_name,value))
           {
            printf("  -> error occurred in 'SerialProgClass.get'\n");
            printf("     nvm/reg definition '%s'\n",definition_name);
	    return(1);
	    // exit(1);
           }
       }
    else  
       {
        printf("error in 'SerialProgClass.get':  unrecognized definition name\n");
        printf("   -> definition '%s' is unknown\n",definition_name);
	return(1);
	// exit(1);
       }
    return(0);
   }

strncpy(nvmreg_subname,cur_definition->nvmreg_name,MAX_DEFINITION_STRING-1);
p = strtok(nvmreg_subname,"\t ,;");
i = 0;
bit_pattern_string[i] = '\0';

while(p != NULL)
   {
    if (i > 3)
      {
        printf("error in 'SerialProgClass.get':  maximum bitwidth exceeded\n");
        printf("   for definition '%s'\n",definition_name);
        printf("   -> please fix definition statement for '%s'\n",definition_name);
	return(1);
        // exit(1);
      }
    get_reg(p,temp_string);
//printf("p = '%s', pattern = '%s'\n",p,bit_pattern_string);
    strcpy(temp_string2,bit_pattern_string);
    sprintf(bit_pattern_string,"%s%s",temp_string2,temp_string);
    p = strtok(NULL,"\t ,;");
    i++;
   }

//printf("value = '%s'\n",bit_pattern_string);
if (cur_definition->xor_pattern_exists_flag == 1)
   {
     for (j = 0; cur_definition->xor_pattern[j] != '\0'; j++);
     j--;
     for (i = 0; j >= 0; i++,j--)
       {
	 if (bit_pattern_string[i] == '\0')
	   {
	     printf("error in ''SerialProgClass.get':  xor_pattern length is\n");
	     printf("    mismatched to bit_pattern length\n");
	     printf("    -> this should never happen! ??\n");
	     return(1);
	     // exit(1);
	   }
	 if (cur_definition->xor_pattern[j] == '1')
 	    bit_pattern_string[i] = (bit_pattern_string[i] == '0') ? '1' : '0';
       }
   }
//printf("value = '%s'\n",bit_pattern_string);

if (cur_definition->inv_formula_exists_flag == 1)
   {
    for (i = 0; bit_pattern_string[i] != '\0'; i++);
    if (i > 31)
       {
        printf("error in 'SerialProgClass.get':  maximum bitwidth exceeded\n");
        printf("   for definition '%s'\n",definition_name);
        printf("   -> please fix definition statement for '%s'\n",definition_name);
	return(1);
        // exit(1);
       }
    if (strcmp(value,"min") == 0)
      {
       if (cur_definition->inv_formula_sign_flag == 1)
	 {
	  bit_pattern_value = (1<<(i-1))-1;
          eval_formula.eval(cur_definition->inv_formula,"x",(double) -bit_pattern_value);
          sprintf(value,"%17.12e",eval_formula.out);
	 }
       else
	 {
	  bit_pattern_value = 0;
          eval_formula.eval(cur_definition->inv_formula,"x",(double) bit_pattern_value);
          sprintf(value,"%17.12e",eval_formula.out);
	 }
      }
    else if (strcmp(value,"max") == 0)
      {
       if (cur_definition->inv_formula_sign_flag == 1)
	 {
	  bit_pattern_value = (1<<(i-1))-1;
          eval_formula.eval(cur_definition->inv_formula,"x",(double) bit_pattern_value);
          sprintf(value,"%17.12e",eval_formula.out);
	 }
       else
	 {
	  bit_pattern_value = (1<<i)-1;
          eval_formula.eval(cur_definition->inv_formula,"x",(double) bit_pattern_value);
          sprintf(value,"%17.12e",eval_formula.out);
	 }
      }
    else
      {
       bit_pattern_value = 0;
       for (i--,j = 0; i >= 0; i--,j++)
          {
           bit_pattern_value += ((bit_pattern_string[i] == '0') ? 0 : 1<<j);
          } 
       // printf("original val = %d\n",bit_pattern_value);
       if (cur_definition->inv_formula_sign_flag == 1)
         {
	  if (bit_pattern_string[0] == '1')
	    {
	    // printf("original val = %d\n",bit_pattern_value);
	    for (    ; j < 32 ; j++)
	       bit_pattern_value += (1<<j);
	    // printf("negative val = %d\n",bit_pattern_value);
	    }
         }
       eval_formula.eval(cur_definition->inv_formula,"x",(double) bit_pattern_value);
       sprintf(value,"%17.12e",eval_formula.out);
      }
   }
else
   {
   found_token_flag = 0;
   cur_token = cur_definition->first_token;
   while (cur_token != NULL)
     {
      strncpy(temp_string,cur_token->bit_pattern,MAX_TOKEN_STRING-1);
      p = strtok(temp_string,"\t ,;");
      token_index = 0;
      while(p != NULL)
        {
         if (strcmp(p,bit_pattern_string) == 0)
            {
             found_token_flag = 1;
             break;
            }
         token_index++;
         p = strtok(NULL,"\t ,;");
        }
      if (found_token_flag == 1)
          break;
      cur_token = cur_token->next;
     }

   if (found_token_flag == 0)
         {
          if (cur_definition->formula_exists_flag == 1)
	    {
	     printf("*** Since you use 'add_formula' for definition '%s' ***\n",definition_name);
	     printf("*** then you must also include an 'add_inv_formula' statement ***\n");
	    }
	  else
	    {
             printf("error in 'SerialProgClass.get':  can't find token!\n");
             printf("  -> looking for bit pattern '%s' for definition '%s'\n",
                  bit_pattern_string, definition_name);
             printf("     but it doesn't seem to exist\n");
             printf("     valid tokens for definition '%s':\n",definition_name);
             cur_token = cur_definition->first_token;
             while (cur_token != NULL)
                {
                 strncpy(temp_string,cur_token->bit_pattern,MAX_TOKEN_STRING-1);
                 p = strtok(temp_string,"\t ,;");
                 while(p != NULL)
                   {
                    printf("    '%s'\n",p);
                    p = strtok(NULL,"\t ,;");
                   }
                 cur_token = cur_token->next;
                }
	    }
	  return(1);
          // exit(1);
         }
    strncpy(token_string,cur_token->token_val,MAX_TOKEN_STRING-1);
    p = strtok(token_string,"\t ,;");
    i = 0;

    while(p != NULL)
       {
        if (i == token_index)
            break;
        p = strtok(NULL,"\t ,;");
        i++;
       }
    if (p == NULL)
       {
        printf("error in 'SerialProgClass.set':  no token found");
        printf("   for bit pattern '%s'\n",bit_pattern_string);
        printf("   for definition '%s'\n",definition_name);
        printf("   -> token for bit pattern '%s' is missing!\n",bit_pattern_string);
        printf("   list of tokens/bit patterns for definition '%s':\n",definition_name);
        cur_token = cur_definition->first_token;
        while (cur_token != NULL)
           {
            printf("    tokens '%s'\n     have bit patterns '%s'\n",
                   cur_token->token_val,cur_token->bit_pattern);
            cur_token = cur_token->next;
           }
	return(1);
        // exit(1);
       }
    strcpy(value,p);
   }
return(0);
}

int SerialProgClass::get_reg(char *address_in, char *value)
{
int i,j,msb_val,lsb_val,nvmreg_val;
char address[50];
char temp[50];
 int addr_msb, addr_size;
int address_val;

strcpy(address,address_in);

for (i = 0; address[i] == ' ' || address[i] == '\t'; i++);
if (strncmp(&address[i],"nvm",3) == 0)
   {
     addr_msb = 1;
     addr_size = addr_nvm_size;
   }
else if (strncmp(&address[i],"reg",3) == 0)
   {
     addr_msb = 0;
     addr_size = addr_reg_size;
   }
else
   {
    printf("error in 'get_reg':  invalid address!\n");
    printf("   -> address must be of form nvmx[m:n] or regx[m:n]\n");
    printf("      (such as nvm4[3] or nvm28[7:5] or reg4[9:3])\n");
    printf("      in this case, address is '%s'\n",address);
    return(1);
   }

for (j = 0, i += 3; address[i] != '[' && address[i] != '\0'; j++,i++)
     temp[j] = address[i];
if (address[i] == '\0')
   {
    ////  removed error below:  MHP 8/12/2015 ////
    // printf("error in 'get_reg':  invalid address!\n");
    // printf("   -> address must be of form nvmx[m:n] or regx[m:n]\n");
    // printf("      (such as nvm4[3] or nvm28[7:5] or reg4[9:3])\n");
    // printf("      in this case, address is '%s'\n",address);
    // return(1);
    i--;
   }
temp[j] = '\0';
//printf("temp = '%s'\n",temp);		//debug	
address_val = atoi(temp);
if (address_val >= addr_reg_size)
   {
    printf("error in 'get_reg':  invalid address!\n");
    printf("   -> address number must be < %d\n",addr_reg_size);
    printf("      in this case, address is '%s'\n",address);
    return(1);
   }

if (addr_msb == 1)
  {
    //address_val += 1<<5;
    address_val += addr_reg_size;
  }
//printf("address_val = %d\n",address_val);

for (j = 0, i += 1; address[i] != ':' && address[i] != ']' && address[i] != '\0'; j++,i++)
   temp[j] = address[i];
if (address[i] == '\0')
   {
     ////  removed error below:  MHP 8/11/2015 ////
     // printf("error in 'get_reg':  invalid address!\n");
     // printf("   -> address must be of form nvmx[m:n] or regx[m:n]\n");
     // printf("      (such as nvm4[3] or nvm28[7:5] or reg4[9:3])\n");
     // printf("      in this case, address is '%s'\n",address);
     // return(1);
     msb_val = nvm_reg_bitwidth-1;
     lsb_val = 0;
   }
 else
   {
    temp[j] = '\0';     
    //printf("temp = '%s'\n",temp);
    msb_val = atoi(temp);
    if (address[i] == ']')
       {
        lsb_val = msb_val;
       }
    else
       {
        for (j = 0, i += 1; address[i] != ']' && address[i] != '\0'; j++,i++)
            temp[j] = address[i];
        if (address[i] == '\0')
           {
            printf("error in 'get_reg':  invalid address!\n");
            printf("   -> address must be of form nvmx[m:n] or regx[m:n]\n");
            printf("      (such as nvm4[3] or nvm28[7:5] or reg4[9:3])\n");
            printf("      in this case, address is '%s'\n",address);
            return(1);
           }
        temp[j] = '\0';     
        //printf("temp = '%s'\n",temp);			//debug
        lsb_val = atoi(temp);
       }
    if (msb_val < lsb_val)
       {
        j = msb_val;
        msb_val = lsb_val;
        lsb_val = j;
       }
   }
//printf("msb_val:%d lsb_val:%d\n", msb_val, lsb_val);	//debug
//printf("nvm_mode:%d \n", nvm_mode);

// get memory value
for (i = 0, j = msb_val; j >= lsb_val; i++, j--)
       {
        if (nvm_mode == 1)
           nvmreg_val = nvmreg_v1->get_elem(address_val,j);
        else if (nvm_mode == 2)
           nvmreg_val = nvmreg_v2->get_elem(address_val,j);
        if (i >= nvm_reg_bitwidth)
           {
            printf("error in 'get_reg':  address bitwidth is too long!\n");
            printf("   -> address must be of form nvmx[m:n] or regx[m:n]\n");
	    sprintf(temp,"      where (m - n) < %d\n", nvm_reg_bitwidth);
            printf("%s",temp);
            printf("      in this case, address is '%s'\n",address);
            return(1);
           }
        value[i] = (nvmreg_val == 0) ? '0' : '1';
        }
value[i] = '\0';

return(0);
}



void SerialProgClass::print_cur_packet()
{
int i;
printf("cur_packet:   header:");
for (i = 0; i < header_bitwidth; i++)
  {
   if (i % 4 == 0)
      printf(" ");
   printf("%d",cur_packet->get_elem(i));
  }
printf("\n");
printf("              address:");
for (i = 0; i < addr_bitwidth; i++)
  {
    if (i % 4 == 0)
      printf(" ");
    printf("%d",cur_packet->get_elem(header_bitwidth+i));
  }
 printf("\n");
printf("              value:");
for (i = 0; i < nvm_reg_bitwidth; i++)
  {
    if (i % 4 == 0)
      printf(" ");
    printf("%d",cur_packet->get_elem(header_bitwidth+addr_bitwidth+i));
  }
 printf("\n");
}

void SerialProgClass::prog_send()
{
prog_send_active_internal = 1;
}

int SerialProgClass::read_next_prog_val()
{
int addr, val;
WriteCommand *cur_command;

if (first_command == NULL && cur_packet_counter == 0 
    && prog_send_active_internal == 1)
    {
      prog_send_active = 0;
      prog_send_active_internal = 0;
      return(0);
    }

if (prog_send_active_internal == 1)
   {
     force_low = 0;
     force_high = 0;
     prog_send_active = 1;
     cur_command = first_command;
     if (cur_packet_counter == 0)
        {
	 load_command_into_packet(cur_command,cur_packet);
         first_command = cur_command->next;
         delete cur_command;
        }
     val = cur_packet->get_elem(cur_packet_counter);
     cur_packet_counter++;
     if (cur_packet_counter == cur_packet->get_length())
        cur_packet_counter = 0;
     return(val);
   }
else
   {
     if (force_high_internal == 1)
       force_high = 1;
     else
       force_high = 0;
     if (force_low_internal == 1)
       force_low = 1;
     else
       force_low = 0;
     return(0);
   }
}

void SerialProgClass::set_reg(char *address,int value)
{
WriteCommand *cur_command;

if (first_command == NULL)
   {
     first_command = new WriteCommand;
     cur_command = first_command;
   }
else
   {
     cur_command = first_command;
     while (cur_command->next != NULL)
	 cur_command = cur_command->next;
     cur_command->next = new WriteCommand;
     cur_command = cur_command->next;
   }

strcpy(cur_command->address,address);
cur_command->value = value;
cur_command->address_val = -1;
cur_command->msb_val = -1;
cur_command->lsb_val = -1;
cur_command->bit_width = -1;
cur_command->clear_internal_nvmreg_block_flag = 0;
cur_command->clear_internal_nvmreg_block_first_addr = -1;
cur_command->clear_internal_nvmreg_block_last_addr = -1;
cur_command->next = NULL;

//printf("updating '%s' with '%d'\n",address,value);
if (evaluate_command(cur_command))
   {
    printf("error occurred with function 'set_reg' for\n");
    printf("   address '%s' and value '%d'\n",address,value);
    exit(1);
   }
}


void SerialProgClass::set_reg(WriteCommand *input_command, int value)
{
WriteCommand *cur_command;

if (first_command == NULL)
   {
     first_command = new WriteCommand;
     cur_command = first_command;
   }
else
   {
     cur_command = first_command;
     while (cur_command->next != NULL)
	 cur_command = cur_command->next;
     cur_command->next = new WriteCommand;
     cur_command = cur_command->next;
   }
//printf("updating '%s' with '%d'\n",input_command->address,value);
strcpy(cur_command->address,input_command->address);
cur_command->value = value;
cur_command->address_val = input_command->address_val;
cur_command->msb_val = input_command->msb_val;
cur_command->lsb_val = input_command->lsb_val;
cur_command->bit_width = input_command->bit_width;
cur_command->clear_internal_nvmreg_block_flag = input_command->clear_internal_nvmreg_block_flag;
cur_command->clear_internal_nvmreg_block_first_addr = input_command->clear_internal_nvmreg_block_first_addr;
cur_command->clear_internal_nvmreg_block_last_addr = input_command->clear_internal_nvmreg_block_last_addr;
cur_command->next = NULL;
}

// added functions below after Dec 18, 2010  - Michael Perrott
void SerialProgClass::set_chip_address(int address_value)
{
  int i, bit_value;
  if (address_value < 0 || address_value > 15)
    {
      printf("error in 'SerialProgClass::set_chip_address':  chip_address must be between 0 and 15\n");
      printf("  -> in this case, chip_address = %d\n",address_value);
      exit(1);
    }
// change header value in bit positions 11:8 of total headers bits 15:0
 
  for (i = 0; i < 4; i++)
    {
      bit_value = (address_value & (1<<i)) == 0 ? 0 : 1;
      cur_packet->set_elem(11-i,bit_value);
    }
}
