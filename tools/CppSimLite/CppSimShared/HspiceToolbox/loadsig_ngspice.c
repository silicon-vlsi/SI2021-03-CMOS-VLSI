/*
 * loadsig_ngspice.c: retrieval of NGspice data into MATLAB
 *   written by Michael H. Perrott
 *
 *   To compile this code into a mex file for Matlab, simply
 *   run Matlab in the directory that loadsig_ngspice.c is contained
 *   and then type:
 *   mex loadsig_ngspice.c
 *   You'll then have a loadsig_ngspice mex function for whatever 
 *   computer (i.e., Windows, Mac, Linux) that you're running 
 *   on at the time.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the MxFree
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


#define MAX_NAME_LENGTH 1000
#define TIME 1
#define FREQUENCY 4
#define VOLTAGE 2
#define CURRENT 3
#define UNKNOWN 0
#define TR_ANALYSIS 0
#define AC_ANALYSIS 1
#define DC_ANALYSIS 2
#define OP_ANALYSIS 3
#define NOISE_ANALYSIS 4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
/* #include "mat.h" */
#include "mex.h"

struct NAME_LIST
  {
  unsigned char name[256];
  struct NAME_LIST *next;
};
typedef struct NAME_LIST NAME_LIST;

struct DATA_FRAME
  {
  double *dbuf;
  int cur_index, length;
  struct DATA_FRAME *next;
};
typedef struct DATA_FRAME DATA_FRAME;
 
typedef struct  
  {  
   NAME_LIST *first_sig_name;
   DATA_FRAME *first_data_frame, *cur_data_frame;
   int  *signal_type;  /* i.e. FREQ, TIME, VOLTAGE ... */
   int analysis_type;  /* i.e., AC, DC, TR, OP */
   int operating_point_flag; 
   int num_signals, num_data_rows, num_data_cols;
   int *signal_num_rows;
} SpiceData;  

NAME_LIST *init_name_list();
DATA_FRAME *init_data_frame();
void free_data_frames(DATA_FRAME *first_data_frame);
void free_name_list(NAME_LIST *first_name_list);


SpiceData *init_SpiceData(int num_signals, int num_data_rows, int num_data_cols, int operating_point_flag);
void free_SpiceData(SpiceData *A);

/******************************************************/
#define BSIZE_SP 512
SpiceData *ngsp_allocate_sf(FILE *fp, char *simsource);
int ngsp_determine_variables(FILE *fp, SpiceData *sf, char *simsource);
int ngsp_read_column(FILE *fp, SpiceData *sf, char *simsource);
int fread_double_values(char *simsource,double *fval, FILE *fp);
/******************************************************/



void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
SpiceData *sf;
FILE *fp;
int i,j,k,m,count;
char filename[MAX_NAME_LENGTH], simsource[MAX_NAME_LENGTH];
mxArray *pa1;
double *matData;
const char *field_name[20] = {"name","data"};
static int first_run=0;
NAME_LIST *cur_sig_name;
DATA_FRAME *cur_data_frame;

if (nrhs != 1)
  mexErrMsgTxt("Error: missing filename. Usage:  x = loadsig('filename')");
if (!mxIsChar(prhs[0]))
  mexErrMsgTxt("Error: filename must be a string");
if (nlhs != 1)
  mexErrMsgTxt("Error: missing output variable.  Usage:  x = loadsig('filename')");

mxGetString(prhs[0],filename,MAX_NAME_LENGTH);

if ((fp = fopen(filename,"rb")) == NULL)   
   mexErrMsgTxt("Error:  file can't be opened");


sf = ngsp_allocate_sf(fp,simsource);
if (sf == NULL)
   {
   mexErrMsgTxt("Error:  file is not in ngspice raw format");
   }

if (ngsp_determine_variables(fp,sf,simsource) == 0)
     return;

cur_sig_name = sf->first_sig_name;


while (cur_sig_name != NULL)
   {
   for (i = 0; cur_sig_name->name[i] != '\0'; i++)
      if (cur_sig_name->name[i] == '.' || cur_sig_name->name[i] == '(' ||
          cur_sig_name->name[i] == ')' || cur_sig_name->name[i] == '+' ||
          cur_sig_name->name[i] == '[' || cur_sig_name->name[i] == ']' ||
          cur_sig_name->name[i] == '-' || cur_sig_name->name[i] == '*' ||
          cur_sig_name->name[i] == '#' ||
          cur_sig_name->name[i] == '/' || cur_sig_name->name[i] == ',')
         cur_sig_name->name[i] = '_';
   cur_sig_name = cur_sig_name->next;
  }


if (sf->first_data_frame == NULL)
   {
     sf->first_data_frame = init_data_frame();
   }
sf->cur_data_frame = sf->first_data_frame;

for (i = 0; i < sf->num_data_cols; i++)
   {
    if(ngsp_read_column(fp,sf,simsource) == 1)
       {
        mexPrintf("error:  premature break\n");
        break;
       }
   }

sf->cur_data_frame->length = sf->cur_data_frame->cur_index;

fclose(fp);

plhs[0] = mxCreateStructMatrix(sf->num_signals,1,2,field_name);

cur_sig_name = sf->first_sig_name;
j = 0;
for (k = 0; k < sf->num_signals; k++)
    {
     /* copy over the signal name to the structure */
     pa1 = mxCreateString(cur_sig_name->name);  
     mxSetField(plhs[0],k,field_name[0],pa1);

     if (sf->signal_num_rows[k] == 1)
        pa1 = mxCreateDoubleMatrix(sf->num_data_cols,1,mxREAL);
     else
        pa1 = mxCreateDoubleMatrix(sf->num_data_cols,1,mxCOMPLEX);

     /* copy over the spice data to the matlab matrix */
     matData = mxGetPr(pa1);

     count = 0;
     cur_data_frame = sf->first_data_frame;
     for (i = 0; i < sf->num_data_cols; i++)
        {
         for (m = 0; m < sf->num_data_rows; m++)
             {
	      if (count == cur_data_frame->length)
	         {
	          cur_data_frame = cur_data_frame->next;
	          if (cur_data_frame == NULL)
	             {
		      mexPrintf("error using evalsig: data frame too short!\n");
		      return;
	             }
                  count = 0;
	         }
	     if (m == (k+j))
                 matData[i] = cur_data_frame->dbuf[count];

             count++;
             }
        }

     if (sf->signal_num_rows[k] == 2)
       {
        matData = mxGetPi(pa1);
        j++; 
        count = 0;
        cur_data_frame = sf->first_data_frame;
        for (i = 0; i < sf->num_data_cols; i++)
           {
            for (m = 0; m < sf->num_data_rows; m++)
                {
	         if (count == cur_data_frame->length)
	            {
	             cur_data_frame = cur_data_frame->next;
	             if (cur_data_frame == NULL)
	                {
		         mexPrintf("error using evalsig: data frame too short!\n");
		         return;
	                }
                     count = 0;
	            }
	        if (m == (k+j))
                    matData[i] = cur_data_frame->dbuf[count];

                count++;
                }
           }
       }

     mxSetField(plhs[0],k,field_name[1],pa1);
     cur_sig_name = cur_sig_name->next;
    }

free_SpiceData(sf);
return;
}

int ngsp_determine_variables(FILE *fp, SpiceData *sf, char *simsource)
{
int data_valid_flag, record_variables_flag, count, i;
int complex_data_flag;
char buf[BSIZE_SP], signal_name[BSIZE_SP], temp_name[BSIZE_SP];
char *pch;
NAME_LIST *cur_sig_name;

rewind(fp);

data_valid_flag = 0;
record_variables_flag = 0;
complex_data_flag = 0;

while (fgets(buf, BSIZE_SP, fp)) 
   {
     if (strncmp(buf,"Plotname: ",10) == 0)
       {
	 if (data_valid_flag == 1)
 	   break;

	 if (strncmp(&buf[10],"DC transfer characteristic",26) == 0)
	   {
	   sf->analysis_type = DC_ANALYSIS;
	   data_valid_flag = 1;
	   /*  mexPrintf("found DC transfer (init)\n"); */
	   }
	 else if (strncmp(&buf[10],"AC Analysis",11) == 0)
	   {
	   sf->analysis_type = AC_ANALYSIS;
	   data_valid_flag = 1;
	   /* mexPrintf("found AC transfer (init)\n"); */
	   }
	 else if (strncmp(&buf[10],"Noise Spectral Density",22) == 0)
	   {
	   sf->analysis_type = NOISE_ANALYSIS;
	   data_valid_flag = 1;
	   /* mexPrintf("found NOISE (init)\n"); */
	   }
	 else if (strncmp(&buf[10],"Transient Analysis",18) == 0)
	   {
	   sf->analysis_type = TR_ANALYSIS;
	   data_valid_flag = 1;
	   /* mexPrintf("found TR transfer (init)\n"); */
	   }
	 else if (strncmp(&buf[10],"Operating Point",15) == 0)
	   {
	   sf->analysis_type = OP_ANALYSIS;
	   if (sf->operating_point_flag == 1)
	      data_valid_flag = 1;
	   else
	      data_valid_flag = 0;
	   /* mexPrintf("found Operating Point (init)\n"); */
	   }
       }
     else if (data_valid_flag == 1)
       {
        if (strncmp(buf,"Variables:",10) == 0)
          {
           record_variables_flag = 1;
	   count = 0;
          }
        else if (strncmp(buf,"Binary:",7) == 0)
          {
	    break;
          }
        else if (strncmp(buf,"Flags: ",7) == 0)
          {
	    if (strncmp(&buf[7],"complex",7) == 0)
	      {
		if (sf->analysis_type != AC_ANALYSIS)
		  {
		    mexPrintf("Error:  complex data encountered but not AC analysis\n");
                    mexErrMsgTxt("error trying to read in ngspice raw file");
		  }
		complex_data_flag = 1;
		/* mexPrintf("found complex data!! (init)\n"); */
	      }

          }
	else if (record_variables_flag == 1)
	  {
	    if (count < sf->num_signals)
	      {
		pch = strtok(buf," \t");
		if (count != atoi(pch))
		  {
		    mexPrintf("Error:  variable list in ngspice rawfile is not numbered correctly\n");
                    mexErrMsgTxt("error trying to read in ngspice raw file");
		  }
		pch = strtok(NULL," \t");
		strcpy(signal_name,pch);
		pch = strtok(NULL," \t\n");
		if (strcmp(pch,"voltage") == 0)
		  {
		    /* mexPrintf("complex_data_flag == %d\n",complex_data_flag); */
		    if (strcmp(signal_name,"v(i-sweep)") == 0)
		       sf->signal_type[count] = CURRENT;
		    else
		       sf->signal_type[count] = VOLTAGE;
		    if (complex_data_flag == 1)
                       sf->signal_num_rows[count] = 2;
                    else
                       sf->signal_num_rows[count] = 1;

		    strcpy(temp_name,signal_name);
		    if (strncmp(temp_name,"v(",2) == 0)
		      {
		      strcpy(signal_name,&temp_name[2]);
		      for (i = 0; signal_name[i] != '\0'; i++);
		      i--;
		      if (signal_name[i] != ')')
			{
		          mexPrintf("Error:  voltage_signal '%s' is not labeled correctly\n", temp_name);
                          mexErrMsgTxt("error trying to read in ngspice raw file");
			}
		      signal_name[i] = '\0';
		      }
		    /****
		    else
		      strcpy(signal_name,&temp_name[2]);
		    ****/
		  }
		else if (strcmp(pch,"current") == 0)
		  {
		    sf->signal_type[count] = CURRENT;
		    if (complex_data_flag == 1)
                       sf->signal_num_rows[count] = 2;
                    else
                       sf->signal_num_rows[count] = 1;

		    if (strncmp(signal_name,"i(",2) == 0)
		      {
		      signal_name[1] = '_';
		      for (i = 0; signal_name[i] != '\0'; i++);
		      i--;
		      if (signal_name[i] != ')')
			{
		          mexPrintf("Error:  current_signal '%s' is not labeled correctly\n", temp_name);
                          mexErrMsgTxt("error trying to read in ngspice raw file");
			}
		      signal_name[i] = '\0';
		      }
		    /*****
		    else
		      strcpy(signal_name,&temp_name[2]);
		    ****/
		  }
		else if (strcmp(pch,"frequency") == 0)
		  {
		    if (strcmp(signal_name,"frequency") != 0)
		      {
			mexPrintf("Error:  frequency signal name must be 'frequency'\n");
			mexPrintf("        instead of '%s'\n",signal_name);
                        mexErrMsgTxt("error trying to read in ngspice raw file");
		      }
		    strcpy(signal_name,"FREQUENCY");
		    sf->signal_type[count] = FREQUENCY;
                    sf->signal_num_rows[count] = 1;
		  }
		else if (strcmp(pch,"time") == 0)
		  {
		    if (strcmp(signal_name,"time") != 0)
		      {
			mexPrintf("Error:  time signal name must be 'time'\n");
			mexPrintf("        instead of '%s'\n",signal_name);
                        mexErrMsgTxt("error trying to read in ngspice raw file");
		      }
		    strcpy(signal_name,"TIME");
		    sf->signal_type[count] = TIME;
                    sf->signal_num_rows[count] = 1;
		  }
		else
		  {
		    sf->signal_type[count] = UNKNOWN;
		    if (complex_data_flag == 1)
                       sf->signal_num_rows[count] = 2;
                    else
                       sf->signal_num_rows[count] = 1;
		  }
		if (signal_name[0] == '@')
		  {
		  strcpy(temp_name,&signal_name[1]);
		  for(i = 0; temp_name[i] != '\0'; i++)
		    {
		      if (temp_name[i] == '[')
			temp_name[i] = '_';
		      else if (temp_name[i] == ']')
			temp_name[i] = ' ';
		    }
		  for (i--; temp_name[i] == ' '; i--);
		  temp_name[i+1] = '\0';
		  strcpy(signal_name,temp_name);
		  }
		if (count == 0)
		  {
		    if (sf->signal_type[count] == VOLTAGE)
		      strcpy(signal_name,"VOLTAGE");
		    else if (sf->signal_type[count] == CURRENT)
		      strcpy(signal_name,"CURRENT");
		  }
                if (count == 0)
		  {
		    if (sf->first_sig_name == NULL)
                        sf->first_sig_name = init_name_list();
                    cur_sig_name = sf->first_sig_name;
		    strcpy(cur_sig_name->name,signal_name);                    
		  }
                else if (cur_sig_name != NULL)
		  {
		    cur_sig_name->next = init_name_list();
                    cur_sig_name = cur_sig_name->next;
		    strcpy(cur_sig_name->name,signal_name);
		  }
	      }
	    count++;
	    if (count == sf->num_signals)
	       record_variables_flag = 0;
	  }
       }
   }

return(1);
}


SpiceData *ngsp_allocate_sf(FILE *fp, char *simsource)
{
int num_signals,data_rows,data_cols;
int data_valid_flag, complex_data_flag, operating_point_flag;
char buf[BSIZE_SP];

/* probably not necessary to rewind, but just to make sure ... */
rewind(fp);
fgets(buf, BSIZE_SP, fp);
if (strncmp(buf,"Title: ",7) != 0)
   return(NULL);

rewind(fp);

 num_signals = -1;
 data_rows = -1;
 data_cols = -1;

 data_valid_flag = 0;
 complex_data_flag = 0;
 operating_point_flag = 0;

while (fgets(buf, BSIZE_SP, fp)) 
   {
     if (strncmp(buf,"Plotname: ",10) == 0)
       {
	 if (data_valid_flag == 1)
	    break;

	 if (strncmp(&buf[10],"DC transfer characteristic",26) == 0)
	   {
	   data_valid_flag = 1;
	   /* mexPrintf("found DC transfer\n"); */
	   }
	 else if (strncmp(&buf[10],"AC Analysis",11) == 0)
	   {
	   data_valid_flag = 1;
	   /* mexPrintf("found AC transfer\n"); */
	   }
	 else if (strncmp(&buf[10],"Noise Spectral Density",22) == 0)
	   {
	   data_valid_flag = 1;
	   /* mexPrintf("found NOISE (init)\n"); */
	   }
	 else if (strncmp(&buf[10],"Transient Analysis",18) == 0)
	   {
	   data_valid_flag = 1;
	   /* mexPrintf("found TR transfer\n"); */
	   }
	 else if (strncmp(&buf[10],"Operating Point",15) == 0)
	   {
	   data_valid_flag = 0;
	   /* mexPrintf("found Operating Point\n"); */
	   }
       }
     else if (data_valid_flag == 1)
       {
        if (strncmp(buf,"No. Variables: ",15) == 0)
          {
           num_signals = atoi(&buf[15]);
	   /* mexPrintf("num_signals = %d\n",num_signals); */
          }
        else if (strncmp(buf,"No. Points: ",12) == 0)
          {
           data_cols = atoi(&buf[12]);
	   /* mexPrintf("data_cols = %d\n",data_cols); */
          }
        else if (strncmp(buf,"Flags: ",7) == 0)
          {
	    if (strncmp(&buf[7],"complex",7) == 0)
	      {
		complex_data_flag = 1;
		/* mexPrintf("found complex data!!\n"); */
	      }

          }
       }
   }
 if (num_signals == -1) 
   {
    /* first check if operating point info is there */
    rewind(fp);
    data_valid_flag = 0;
    complex_data_flag = 0;
    while (fgets(buf, BSIZE_SP, fp)) 
      {
       if (strncmp(buf,"Plotname: ",10) == 0)
         {
	 if (strncmp(&buf[10],"Operating Point",15) == 0)
	   {
	   data_valid_flag = 1;
	   /* mexPrintf("found Operating Point\n"); */
	   operating_point_flag = 1;
	   }
         }
       else if (data_valid_flag == 1)
         {
          if (strncmp(buf,"No. Variables: ",15) == 0)
            {
             num_signals = atoi(&buf[15]);
	     /* mexPrintf("num_signals = %d\n",num_signals); */
            }
          else if (strncmp(buf,"No. Points: ",12) == 0)
            {
             data_cols = atoi(&buf[12]);
	     /* mexPrintf("data_cols = %d\n",data_cols); */
            }
          else if (strncmp(buf,"Flags: ",7) == 0)
            {
	     if (strncmp(&buf[7],"complex",7) == 0)
	        {
		complex_data_flag = 1;
		/* mexPrintf("found complex data!!\n"); */
	        }
            }
         }
      }
     if (num_signals == -1) 
        {
	 /* not even operating point information is available */
         mexPrintf("Error:  num_signals is undefined!\n");
         mexErrMsgTxt("error trying to read in ngspice raw file");
	}
   }

if (complex_data_flag == 1)
   data_rows = 2*num_signals-1;
else
   data_rows = num_signals;

if (data_cols == -1)
   {
     mexPrintf("Error:  data_cols is undefined!\n");
     mexErrMsgTxt("error trying to read in ngspice raw file");
   }


return(init_SpiceData(num_signals,data_rows,data_cols,operating_point_flag));
}


int ngsp_read_column(FILE *fp, SpiceData *sf, char *simsource)
{
	int i,num_data_rows;
	double val, scratch_val;

	num_data_rows = 0;
	for (i = 0; i < sf->num_signals; i++)
           num_data_rows += sf->signal_num_rows[i];

	for (i = 0; i < num_data_rows; i++) 
           {
	    if(fread_double_values(simsource, &val, fp) != 1) 
               {
	       mexPrintf("error in ngsp_read_column for data vars: unexpected EOF at field %d\n", i);
	       return(1);
	       }
	    if (i == 0 && sf->analysis_type == AC_ANALYSIS) /* only applies to ngspice */
	      {
	       if(fread_double_values(simsource, &scratch_val, fp) != 1) 
                  {
	           mexPrintf("error in ngsp_read_column for data vars: unexpected EOF at field %d\n", i);
	           return(1);
	          }
	      }

            if (i >= sf->num_data_rows)
	      {
               mexPrintf("error in 'ngsp_read_column': data overflow\n");
               return(1);
	      }

            if (sf->cur_data_frame != NULL)
	      {
		if (sf->cur_data_frame->dbuf != NULL)
		  {
		    if (sf->cur_data_frame->cur_index >= sf->cur_data_frame->length)
		      {
			sf->cur_data_frame->next = init_data_frame();
			sf->cur_data_frame = sf->cur_data_frame->next;
		      }
	            sf->cur_data_frame->dbuf[sf->cur_data_frame->cur_index] = val;
                    sf->cur_data_frame->cur_index++;
		  }
	      }
	   }

        return(0);
}
	




SpiceData *init_SpiceData(int num_signals, int num_data_rows, int num_data_cols, int operating_point_flag)
{
int i;
SpiceData *A; 
   
if ((A = (SpiceData *) mxCalloc(1,sizeof(SpiceData))) == NULL)   
   {   
   mexPrintf("error in 'init_SpiceData':  mxCalloc call failed\n");   
   mexPrintf("out of memory!\n");   
   return(NULL);   
  }   

A->analysis_type = -1; /* this is only used for ngspice */
A->operating_point_flag = operating_point_flag; /* only used for ngspice */
A->num_data_rows = num_data_rows;
A->num_data_cols = num_data_cols;
A->num_signals = num_signals;

A->first_sig_name = NULL;
A->first_data_frame = NULL;
A->cur_data_frame = NULL;

if (((A->signal_num_rows) = (int *) mxCalloc(num_signals,sizeof(int))) == NULL)   
       {   
        mexPrintf("error in 'init_SpiceData':  mxCalloc call failed - \n");   
        mexPrintf("  not enough memory!!\n");   
        return(NULL);   
       }   
if (((A->signal_type) = (int *) mxCalloc(num_signals,sizeof(int))) == NULL)   
       {   
        mexPrintf("error in 'init_SpiceData':  mxCalloc call failed - \n");   
        mexPrintf("  not enough memory!!\n");   
        return(NULL);   
       } 
return(A);
}   


void free_SpiceData(SpiceData *A)
{
int i;  

free_name_list(A->first_sig_name);
free_data_frames(A->first_data_frame);

mxFree(A->signal_num_rows);   
mxFree(A->signal_type);
mxFree(A);
}   




/* allow for future fix in case of problem with big endian, little endian issue */
int fread_double_values(char *simsource,double *fval, FILE *fp)
  {
   if (fread(fval,8,1,fp) != 1)
      return(0);

   return(1);
  }


DATA_FRAME *init_data_frame()
{
DATA_FRAME *A; 
   

if ((A = (DATA_FRAME *) mxCalloc(1,sizeof(DATA_FRAME))) == NULL)   
   {   
   /*
   printf("error in 'init_data_frame':  calloc call failed\n");   
   printf("out of memory!\n");   
   */
   return(NULL);   
  }

A->cur_index = 0;  
A->length = 2048;
A->next = NULL;

if ((A->dbuf = (double *) mxCalloc(A->length,sizeof(double))) == NULL)   
      { 
      /**  
      printf("error in 'init_data_frame':  calloc call failed\n");   
      printf("out of memory!\n");   
      **/
      return(NULL);   
      }

return(A);
}


NAME_LIST *init_name_list()
{
NAME_LIST *A; 
   
if ((A = (NAME_LIST *) mxCalloc(1,sizeof(NAME_LIST))) == NULL)   
   {   
   /*
   printf("error in 'init_name_list':  calloc call failed\n");   
   printf("out of memory!\n");   
   */
   return(NULL);   
  }   
A->next = NULL;
return(A);
}


void free_data_frames(DATA_FRAME *first_data_frame)
{
DATA_FRAME *cur_data_frame, *prev_data_frame;

cur_data_frame = first_data_frame;

while(cur_data_frame != NULL)
   {
    prev_data_frame = cur_data_frame;
    cur_data_frame = cur_data_frame->next;
    if (prev_data_frame->dbuf != NULL)
        mxFree(prev_data_frame->dbuf);
    mxFree(prev_data_frame);
   }
}

void free_name_list(NAME_LIST *first_name_list)
{
NAME_LIST *cur_name_list, *prev_name_list;

cur_name_list = first_name_list;

while(cur_name_list != NULL)
   {
    prev_name_list = cur_name_list;
    cur_name_list = cur_name_list->next;
    mxFree(prev_name_list);
   }
}

