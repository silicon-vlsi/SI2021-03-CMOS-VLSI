//////// Mex Interface for SerialProgClass /////////
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

#include "mex.h"
#include "serial_prog_class_mex.cpp"

extern "C" void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
static int first_entry_flag = 0;
static SerialProgClass *serial_state;
char end_string_check[11];
char definition_string[101], token_string[101], temp_string[101], temp_string2[101];
char *get_string;
int prog_out_val,rows,cols,i,j,count;
// mex defined outputs
mxArray *plhs_0_mx;
double *plhs_0_dp, *prhs_1_dp, *temp_array;
MexListStream plhs_0_mexlist;
DefinitionList *cur_definition;
TokenList *cur_token;
char token_string_raw[MAX_TOKEN_STRING];
char *p;
int prhs_1_length, error_flag;
static int addr_reg_size, addr_nvm_size, nvm_reg_bitwidth, addr_size;

// delete serial_state object if user calls 'end'
if (nrhs == 1)
   {
   if (mxIsChar(prhs[0]))
      {
      mxGetString(prhs[0],end_string_check,10);
      if (strcmp(end_string_check,"end") == 0)
         {
          if (first_entry_flag != 0)
             {
             delete serial_state;
             first_entry_flag = 0;
             }
	  if (nlhs == 1)
             plhs[0] = mxCreateString("NULL");
	  mexPrintf("***** Serial Prog Class contents have now been erased! ******\n");
          return;
         }
      }
   }

if (nrhs != 2 && nrhs != 3)
   {
   mexErrMsgTxt("Error: need at least 2 inputs ('definition' and 'token')\n   mex function prototype:\n     out = serial_state('definition','token')\n           serial_state('definition','token')\nOR   out = serial_state('prog','send')\nOR   out = serial_state('prog','send',previous_sequence)\nOR   out = serial_state('definition','get')\nOR   out = serial_state('definition','get_tokens')\nOR   out = serial_state('nvm',nvm_matrix)\nOR   out = serial_state('nvm','get_bin')\nOR   out = serial_state('nvm','get_bin_verbose')\nOR         serial_state('end');");
   }

if (!mxIsChar(prhs[0]))
   mexErrMsgTxt("Error:  input argument for mex parameter 'definition' is invalid");
else
   mxGetString(prhs[0],definition_string,100);

if (first_entry_flag == 0)
   {
   first_entry_flag = 1;
   serial_state = new SerialProgClass("example_ic_version");
   addr_reg_size = serial_state->get_addr_reg_size();
   addr_nvm_size = serial_state->get_addr_nvm_size();
   nvm_reg_bitwidth = serial_state->get_nvm_reg_bitwidth();
   }

if (!mxIsChar(prhs[1]))
  {
    if (strcmp(definition_string,"nvm") == 0 || strcmp(definition_string,"reg") == 0)
      {
	if (!mxIsDouble(prhs[1]))
            mexErrMsgTxt("Error:  input argument for mex parameter 'token' is invalid");
        if (strcmp(definition_string,"nvm") == 0)
	   addr_size = addr_nvm_size;
        else
	   addr_size = addr_reg_size;
	prhs_1_dp = (double *) mxGetPr(prhs[1]);
	prhs_1_length = mxGetNumberOfElements(prhs[1]);
	if (prhs_1_length != addr_size*nvm_reg_bitwidth)
	  {
	    sprintf(temp_string,"Error:  invalid size for input matrix (must be %d rows by %d columns)", addr_size, nvm_reg_bitwidth);
            mexErrMsgTxt(temp_string);
	  }
        for (i = 0; i < addr_size; i++)
	  {
	   for (j = 0; j < nvm_reg_bitwidth; j++)
	     {
              if (strcmp(definition_string,"nvm") == 0)
	         sprintf(temp_string,"nvm%d[%d]",i,j);
              else
	         sprintf(temp_string,"reg%d[%d]",i,j);
	      if (prhs_1_dp[j*addr_size + i] == 0.0)
		strcpy(temp_string2,"0");
	      else
		strcpy(temp_string2,"1");
	      error_flag = serial_state->set(temp_string,temp_string2);
	      if (error_flag == 1)
	         {
                  mexErrMsgTxt("Error: set command of Serial Class has failed");
	         }
	     }
          }
	/******* This was only done for verification earlier ******
       // update contents of nvm or reg space by doing "prog" "send"
       error_flag = serial_state->set("prog","send");
       if (error_flag == 1)
	  {
           mexErrMsgTxt("Error: set command of Serial Class has failed");
	  }

       while(1)
          {
           prog_out_val = serial_state->read_next_prog_val();
	   if (serial_state->prog_send_active == 0)
              break;
          }
	***********************************************************/
	if (nlhs == 1)
           strcpy(token_string,"get_bin");
        else
	   return;
      }
    else
       mexErrMsgTxt("Error:  input argument for mex parameter 'token' is invalid");
  }
else
   mxGetString(prhs[1],token_string,100);

if (nlhs != 1)
   {
   if (strcmp(definition_string,"prog") != 0 && strncmp(token_string,"get",3) != 0)
     {
       if (nlhs != 0)
	 {
          mexPrintf("Error: no more than one output is allowed\n   mex function prototype:\n     out = serial_state('%s','%s')\nOR   serial_state('%s','%s')\n",
		       definition_string,token_string,definition_string,token_string);
          mexErrMsgTxt("Error:  please supply no more than one output\n");
	 }
     }
   else
     {
      mexPrintf("Error: one output is required\n   mex function prototype:\n     out = serial_state('%s','%s')\n",definition_string,token_string);
      mexErrMsgTxt("Error:  please supply exactly one output\n");
     }
   }

// added section for dealing with new address header - Dec 19, 2010 - Michael Perrott
if (nrhs == 2)
   {
     if (strcmp(definition_string,"header_address_for_programmer") == 0)
       {
        serial_state->set_chip_address(atoi(token_string));
	return;
       }
   }

if (nrhs == 3)
   {
     if (strcmp(definition_string,"prog") != 0 ||
	 strcmp(token_string,"send") != 0)
       {
	 mexPrintf("Error:  can only include third argument of 'previous_sequence'\n");
	 mexPrintf("   when the first two arguments are 'prog','send'\n");
         mexErrMsgTxt("Error:  input argument for mex parameter 'previous_sequence' is invalid");
       }
   if (!mxIsDouble(prhs[2]))
      mexErrMsgTxt("Error:  input argument for mex parameter 'previous_sequence' is invalid");
   else
     {
       rows = mxGetM(prhs[2]);
       cols = mxGetN(prhs[2]);
       if ((rows != 1 && cols != 1) || (rows == 1 && cols == 1))
	 {
	   mexPrintf("Error:  'previous_sequence' input must be a vector\n");
	   mexPrintf("   -> instead it has %d rows and %d cols\n",rows,cols);
           mexErrMsgTxt("Error:  input argument for mex parameter 'previous_sequence' is invalid");
	 }
       temp_array = mxGetPr(prhs[2]);
       if (cols == 1)
	 {
	   for (i = 0; i < rows; i++)
              plhs_0_mexlist.inp(temp_array[i]);
	 }
       else
	 {
	   for (i = 0; i < cols; i++)
              plhs_0_mexlist.inp(temp_array[i]);
	 }
     }
   }


if (strncmp(token_string,"get",3) != 0)
   {
   error_flag = serial_state->set(definition_string,token_string);
   if (error_flag == 1)
      {
       if (nlhs == 1)
          plhs[0] = mxCreateString("NULL");
       return;
       // mexErrMsgTxt("Error: set command of Serial Class has failed");
      }


   if (strcmp(definition_string,"prog") == 0 && strcmp(token_string,"send") == 0)
      {
       //  mexPrintf("...creating output bit stream...\n");
       while(1)
          {
           prog_out_val = serial_state->read_next_prog_val();
	   if (serial_state->prog_send_active == 0)
              break;
           plhs_0_mexlist.inp((double) prog_out_val);
          }
      }

   // Create output signal 'out'
   if (plhs_0_mexlist.length > 0)
      {
      plhs_0_mx = mxCreateDoubleMatrix(1,plhs_0_mexlist.length,mxREAL);
      if (plhs_0_mx == NULL)
         mexErrMsgTxt("Error: out of memory when allocating for signal 'out'");
      plhs[0] = plhs_0_mx;
      plhs_0_dp = (double *) mxGetPr(plhs_0_mx);
      plhs_0_mexlist.copy_to_mex_array(plhs_0_dp);
      }
   else
     if (nlhs == 1)
         plhs[0] = mxCreateString("OK");
   }
else
   {
     if (strcmp(definition_string,"nvm") == 0 || 
         strcmp(definition_string,"reg") == 0)
       {
        if (strcmp(definition_string,"nvm") == 0)
	    addr_size = addr_nvm_size;
        else
	    addr_size = addr_reg_size;

	if (strncmp(token_string,"get_bin",7) == 0)
	  {
	   // whole of shadow NVM is addr_size by nvm_reg_bitwidth registers
	    mexPrintf("allocating memory.... %d by %d", addr_size, nvm_reg_bitwidth);

	   plhs_0_mx = mxCreateDoubleMatrix(addr_size,nvm_reg_bitwidth,mxREAL);
           if (plhs_0_mx == NULL)
              mexErrMsgTxt("Error: out of memory when allocating for signal 'out'");
           plhs[0] = plhs_0_mx;
           plhs_0_dp = (double *) mxGetPr(plhs_0_mx);
           for (i = 0; i < addr_size; i++)
	     {
              if (strcmp(definition_string,"nvm") == 0)
		 sprintf(temp_string,"nvm%d[%d:0]",i,nvm_reg_bitwidth-1);
              else
		 sprintf(temp_string,"reg%d[%d:0]",i,nvm_reg_bitwidth-1);
	      error_flag = serial_state->get(temp_string,temp_string2);
	      if (error_flag == 1)
		{
                 mexErrMsgTxt("Error: get command of Serial Class has failed");
		}
	      if (strcmp(token_string,"get_bin_verbose") == 0)	    
   	         mexPrintf("%s: '%s'\n",temp_string,temp_string2);
	      for (j = 0; j < nvm_reg_bitwidth; j++)
		  plhs_0_dp[(nvm_reg_bitwidth-1-j)*addr_size + i] = (temp_string2[j] == '0') ? 0.0 : 1.0; 
	     }
	  }
	else
	  {
            if (strcmp(definition_string,"nvm") == 0)
	      {
	       count = 0;
	       cur_definition = serial_state->first_definition;
	       while (cur_definition != NULL)
	         {
		  if (strncmp(cur_definition->nvmreg_name,"nvm",3) == 0)
		     count++;
		  cur_definition = cur_definition->next;
		 }
               plhs_0_mx = mxCreateCellMatrix((mwSize) count,2);
               plhs[0] = plhs_0_mx;
	       i = 0;
	       cur_definition = serial_state->first_definition;
	       while (cur_definition != NULL)
	         {
		  if (strncmp(cur_definition->nvmreg_name,"nvm",3) == 0)
		     {
		     error_flag = serial_state->get(cur_definition->name,temp_string);
	             if (error_flag == 1)
		       { 
                       mexErrMsgTxt("Error: get command of Serial Class has failed");
		       }
		     mxSetCell(plhs_0_mx,i,mxCreateString(cur_definition->name));
		     mxSetCell(plhs_0_mx,count+i,mxCreateString(temp_string));
		     i++;
		     }
		  cur_definition = cur_definition->next;
		 }	       
	      }
            else
	      {
	       count = 0;
	       cur_definition = serial_state->first_definition;
	       while (cur_definition != NULL)
	         {
		  if (strncmp(cur_definition->nvmreg_name,"reg",3) == 0)
		     count++;
		  cur_definition = cur_definition->next;
		 }
               plhs_0_mx = mxCreateCellMatrix((mwSize) count,2);
               plhs[0] = plhs_0_mx;
	       i = 0;
	       cur_definition = serial_state->first_definition;
	       while (cur_definition != NULL)
	         {
		  if (strncmp(cur_definition->nvmreg_name,"reg",3) == 0)
		     {
		     error_flag = serial_state->get(cur_definition->name,temp_string);
	             if (error_flag == 1)
		       {
                        mexErrMsgTxt("Error: get command of Serial Class has failed");
		       }
		     mxSetCell(plhs_0_mx,i,mxCreateString(cur_definition->name));
		     mxSetCell(plhs_0_mx,count+i,mxCreateString(temp_string));
		     i++;
		     }
		  cur_definition = cur_definition->next;
		 }	       
	      }
	  }
       }
     else if (strncmp(definition_string,"nvm",3) == 0 || 
              strncmp(definition_string,"reg",3) == 0)
       {
        get_string = (char *) mxCalloc(100,sizeof(char));
        error_flag = serial_state->get(definition_string,get_string);
	if (error_flag == 1)
	   {
            mexErrMsgTxt("Error: get command of Serial Class has failed");
	   }

        plhs[0] = mxCreateString(get_string);
        mxFree(get_string);
       }
     else if (strcmp(token_string,"get_tokens") == 0)
       {
	cur_definition = serial_state->first_definition;
	while (cur_definition != NULL)
	  {
	   if (strcmp(cur_definition->name,definition_string) == 0)
	      break;
	   cur_definition = cur_definition->next;
	  }
	if (cur_definition == NULL)
	  {
	   mexPrintf("Error:  can't find definition '%s'\n",definition_string);
	   if (nlhs == 1)
              plhs[0] = mxCreateString("NULL");
	   return;
	   // mexErrMsgTxt("Error:  input argument for mex parameter 'definition' is invalid");
	  }
	if (cur_definition->formula_exists_flag == 1)
	  {
	    if (cur_definition->inv_formula_exists_flag == 0)
	      {
	      mexPrintf("Error:  no 'add_inv_formula' for definition '%s'\n",definition_string);
              mexErrMsgTxt("Error:  input argument for mex parameter 'definition' is invalid");
	      }
            plhs_0_mx = mxCreateCellMatrix((mwSize) 3,1);
            plhs[0] = plhs_0_mx;
	    mxSetCell(plhs_0_mx,0,mxCreateString("formula"));
	    sprintf(temp_string,"min");
	    error_flag = serial_state->get(cur_definition->name,temp_string);
	    if (error_flag == 1)
	       {
                mexErrMsgTxt("Error: get command of Serial Class has failed");
	       }

	    mxSetCell(plhs_0_mx,1,mxCreateString(temp_string));
	    sprintf(temp_string,"max");
	    error_flag = serial_state->get(cur_definition->name,temp_string);
	    if (error_flag == 1)
	       {
                mexErrMsgTxt("Error: get command of Serial Class has failed");
	       }

	    mxSetCell(plhs_0_mx,2,mxCreateString(temp_string));
	  }
	else
	  {
	   count = 0;
	   cur_token = cur_definition->first_token;
	   while (cur_token != NULL)
	     {
              strncpy(token_string_raw,cur_token->token_val,MAX_TOKEN_STRING-1);
              p = strtok(token_string_raw,"\t ,;");
              while(p != NULL)
                 {
	          count++;
                  p = strtok(NULL,"\t ,;");
		 }
	      cur_token = cur_token->next;
	     }
           plhs_0_mx = mxCreateCellMatrix((mwSize) count,1);
           plhs[0] = plhs_0_mx;
	   i = 0;
	   cur_token = cur_definition->first_token;
	   while (cur_token != NULL)
	     {
              strncpy(token_string_raw,cur_token->token_val,MAX_TOKEN_STRING-1);
              p = strtok(token_string_raw,"\t ,;");
              while(p != NULL)
                 {
	          mxSetCell(plhs_0_mx,i,mxCreateString(p));
	          i++;
                  p = strtok(NULL,"\t ,;");
		 }
	      cur_token = cur_token->next;
	     }
	  }	       
       }
     else
       {
        get_string = (char *) mxCalloc(100,sizeof(char));
        error_flag = serial_state->get(definition_string,get_string);
	if (error_flag == 1)
	   {
            if (nlhs == 1)
               plhs[0] = mxCreateString("NULL");
	    return;
            // mexErrMsgTxt("Error: get command of Serial Class has failed");
	   }
        plhs[0] = mxCreateString(get_string);
        mxFree(get_string);
       }
   }

}
