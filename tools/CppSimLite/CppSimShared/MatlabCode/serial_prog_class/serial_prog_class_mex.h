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

#define MAX_TOKEN_STRING 1000
#define MAX_DEFINITION_STRING 1000


struct WriteCommand
{
char address[30];
int address_val, msb_val, lsb_val, bit_width, value;
int clear_internal_nvmreg_block_flag;
int clear_internal_nvmreg_block_first_addr, clear_internal_nvmreg_block_last_addr;
struct WriteCommand *next;
};
typedef struct WriteCommand WriteCommand;

struct TokenList
{
char bit_pattern[MAX_TOKEN_STRING];
char token_val[MAX_TOKEN_STRING];
struct TokenList *next;
};
typedef struct TokenList TokenList;

struct DefinitionList
{
char name[MAX_DEFINITION_STRING], nvmreg_name[MAX_DEFINITION_STRING];
TokenList *first_token;
WriteCommand *command_one, *command_two, *command_three, *command_four;
int formula_exists_flag, inv_formula_exists_flag, inv_formula_sign_flag, xor_pattern_exists_flag;
int clear_internal_nvmreg_block_flag;
int clear_internal_nvmreg_block_first_addr, clear_internal_nvmreg_block_last_addr;
char formula[MAX_CHAR_LENGTH], inv_formula[MAX_CHAR_LENGTH];
char xor_pattern[MAX_CHAR_LENGTH];
int overall_sequence_length;
struct DefinitionList *next;
};
typedef struct DefinitionList DefinitionList;



class SerialProgClass
{
public:
SerialProgClass(const char *chip_version);
~SerialProgClass();
int prog_send_active, force_high, force_low;
int set(char *definition_name, char *value);
int read_next_prog_val();
int get(char *definition_name_in, char *value);
void set_reg(char *address, int value);
DefinitionList *first_definition;
// added functions below after Dec 18, 2010  - Michael Perrott
void set_chip_address(int address_value);
int get_addr_bitwidth();
int get_addr_size();
int get_addr_nvm_size();
int get_addr_reg_size();
int get_nvm_reg_bitwidth();

private:

EvalExpression eval_formula;
int reg_only_mode; // 0: reg + nvm, 1: reg only
int nvm_mode; // 0: diff, 1: sngl1, 2: sngl2
int cur_packet_counter, num_packet_bits;
int addr_nvm_reg_size;
int addr_nvm_size;
int addr_reg_size;
int header_bitwidth;
int addr_bitwidth;
int nvm_reg_bitwidth;
int prog_send_active_internal, force_high_internal, force_low_internal;
WriteCommand *first_command;
DefinitionList *cur_definition;
IntMatrix *nvmreg_v1;
IntMatrix *nvmreg_v2;
IntVector *cur_packet;
int evaluate_command(WriteCommand *cur_command);
void load_command_into_packet(WriteCommand *cur_command,IntVector *cur_packet);
void add_definition(const char *name, const char *nvmreg_name);
void add_tokens(const char *bit_pattern, const char *token_val);
void add_formula(const char *formula);
void add_xor(const char *xor_pattern);
void add_clear_internal_nvmreg_block(int first_addr, int last_addr);
void add_inv_formula(const char *inv_formula);
void add_inv_formula(const char *inv_formula, const char *signed_val);
int is_integer(char *value);
int is_double(char *value);
void clear_all();
void print_cur_packet();
void prog_send();
void remove_start_and_end_spaces_from_string(const char *bit_pattern_in,char *bit_pattern);
// address is nvm8[5:4] or reg4[7:2], etc...
//void set_reg(char *address, int value);		//moved to public
void set_reg(WriteCommand *input_command, int value);
int get_reg(char *address_in, char *value);
};

