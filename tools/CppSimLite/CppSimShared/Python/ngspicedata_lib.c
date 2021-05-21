/***************
ngspicedata_lib.c 
written by Michael H. Perrott
available at www.cppsim.com as part of the CppSim package

This file is compiled in order to create a shared library file that
is used to support the Ngspice data module for Python.
 
Copyright (c) 2013 by Michael H. Perrott

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
****************/

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

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
 
struct NAMES_AND_DATA_STORAGE
  {
   struct NAME_LIST *first_sig_name;
   struct DATA_FRAME *first_data_frame;
   char filename[256];
   int num_sigs;
   int num_sig_vals; /* new */
   int num_samples;
   int complex_data_flag; /* new */
   int analysis_type;  /* i.e., AC, DC, TR, OP */  /* new */
   int operating_point_flag;  /* new */
   int cur_sig_count;
};
typedef struct NAMES_AND_DATA_STORAGE NAMES_AND_DATA_STORAGE;

struct STORAGE_INFO
  {
   char *filename;
   int num_sigs;
   int num_samples;
   int num_sig_vals;
   int analysis_type;
   int complex_data_flag;
   int operating_point_flag;
};
typedef struct STORAGE_INFO STORAGE_INFO;

#ifdef _WIN32
static struct NAMES_AND_DATA_STORAGE names_and_data_file_info = {NULL, NULL, "NULL", 0, 0, 0, 0, 0, 0, 0};
#else
static struct NAMES_AND_DATA_STORAGE names_and_data_file_info = {.first_sig_name = NULL, .first_data_frame = NULL, .filename = "NULL", .num_sigs = 0, .num_sig_vals = 0, .num_samples = 0, .complex_data_flag = 0, .analysis_type = 0, .operating_point_flag = 0, .cur_sig_count = 0};
#endif

NAME_LIST *init_name_list();
DATA_FRAME *init_data_frame();
void free_data_frames(DATA_FRAME *first_data_frame);
void free_name_list(NAME_LIST *first_name_list);


#define BSIZE_SP 512
int ngsp_allocate_sf(FILE *fp, char *filename, char *error_msg);
int ngsp_determine_variables(FILE *fp, char *filename, char *error_msg);
int ngsp_read_data(FILE *fp, char *filename, char *error_msg);
int fread_double_values(double *fval, FILE *fp);

int load_data_from_file(char *filename, char *error_msg);

DLLEXPORT int evalsig(STORAGE_INFO *storage_info, char *sig_name, double *data, char *error_msg);
DLLEXPORT int evalsig_complex(STORAGE_INFO *storage_info, char *sig_name, double *data_real, double *data_imag, char *error_msg);
DLLEXPORT int lssig(STORAGE_INFO *storage_info, char *sig_name, char *error_msg);
DLLEXPORT int loadsig(STORAGE_INFO *storage_info, char *error_msg);
DLLEXPORT int reset_cur_sig_count(void);
DLLEXPORT int initialize(void);


int reset_cur_sig_count(void)
{
names_and_data_file_info.cur_sig_count = 0;
return(0);
}

int evalsig(STORAGE_INFO *storage_info, char *sig_name, double *data, char *error_msg)
{
NAME_LIST *cur_sig_name;
DATA_FRAME *cur_data_frame;
int i, j, count, sig_num;


if (names_and_data_file_info.first_sig_name == NULL)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  -> need to first run loadsig before using evalsig",sig_name);
      return(1);
    }

if (names_and_data_file_info.first_data_frame == NULL)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  -> need to first run loadsig before using evalsig",sig_name);
      return(1);
    }

if (strcmp(storage_info->filename,names_and_data_file_info.filename) != 0)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  ->  internal data storage needs to be updated\n  ---> need to rerun loadsig on file '%s'\n       (Note that file represented in data storage is '%s')",
	      sig_name, storage_info->filename, names_and_data_file_info.filename);
      return(1);
    }

if (storage_info->num_sigs != names_and_data_file_info.num_sigs)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  ->  internal data storage needs to be updated\n  ---> need to rerun loadsig on file '%s'\n       (Note that num_sigs in data storage is %d compared to assumed value of %d)",
	      sig_name, storage_info->filename, names_and_data_file_info.num_sigs, storage_info->num_sigs);
      return(1);
    }

if (storage_info->num_samples != names_and_data_file_info.num_samples)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  ->  internal data storage needs to be updated\n  ---> need to rerun loadsig on file '%s'\n       (Note that num_samples in data storage is %d compared to assumed value of %d)",
	      sig_name, storage_info->filename, names_and_data_file_info.num_samples, storage_info->num_samples);
      return(1);
    }

if (storage_info->num_sig_vals != names_and_data_file_info.num_sig_vals)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  ->  internal data storage needs to be updated\n  ---> need to rerun loadsig on file '%s'\n       (Note that num_sig_vals in data storage is %d compared to assumed value of %d)",
	      sig_name, storage_info->filename, names_and_data_file_info.num_sig_vals, storage_info->num_sig_vals);
      return(1);
    }

if (storage_info->analysis_type != names_and_data_file_info.analysis_type)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  ->  internal data storage needs to be updated\n  ---> need to rerun loadsig on file '%s'\n       (Note that analysis_type in data storage is %d compared to assumed value of %d)",
	      sig_name, storage_info->filename, names_and_data_file_info.analysis_type, storage_info->analysis_type);
      return(1);
    }

if (storage_info->complex_data_flag != names_and_data_file_info.complex_data_flag)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  ->  internal data storage needs to be updated\n  ---> need to rerun loadsig on file '%s'\n       (Note that complex_data_flag in data storage is %d compared to assumed value of %d)",
	      sig_name, storage_info->filename, names_and_data_file_info.complex_data_flag, storage_info->complex_data_flag);
      return(1);
    }

if (storage_info->operating_point_flag != names_and_data_file_info.operating_point_flag)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  ->  internal data storage needs to be updated\n  ---> need to rerun loadsig on file '%s'\n       (Note that operating_point_flag in data storage is %d compared to assumed value of %d)",
	      sig_name, storage_info->filename, names_and_data_file_info.operating_point_flag, storage_info->operating_point_flag);
      return(1);
    }

cur_sig_name = names_and_data_file_info.first_sig_name;
for (sig_num = 0; sig_num < names_and_data_file_info.num_sigs; sig_num++)
    {
      if (strcmp(sig_name,cur_sig_name->name) == 0)
	 break;
     cur_sig_name = cur_sig_name->next;  
    }

if (sig_num == names_and_data_file_info.num_sigs)
   {
     sprintf(error_msg,"Error using evalsig('%s'):\n  -> signal '%s' does not exist as of the last loadsig\n       of file '%s'",
	     sig_name, sig_name, storage_info->filename);
     return(1);  
   }

cur_data_frame = names_and_data_file_info.first_data_frame;

count = 0;

for (j = 0; j < names_and_data_file_info.num_samples; j++)
  {
   for (i = 0; i < names_and_data_file_info.num_sig_vals; i++)
      {
	if (count == cur_data_frame->length)
	  {
	    cur_data_frame = cur_data_frame->next;
	    if (cur_data_frame == NULL)
	      {
		sprintf(error_msg,"error using evalsig('%s'): data frame too short!\n", sig_name);
		return(1);
	      }
            count = 0;
	  }
	if (i == sig_num)
	  {
           data[j] = cur_data_frame->dbuf[count];
	  }
        count++;
      }
  }
return(0);
}


int evalsig_complex(STORAGE_INFO *storage_info, char *sig_name, 
                    double *data_real, double *data_imag, char *error_msg)
{
NAME_LIST *cur_sig_name;
DATA_FRAME *cur_data_frame;
int i, j, count, sig_num;


if (names_and_data_file_info.first_sig_name == NULL)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  -> need to first run loadsig before using evalsig",sig_name);
      return(1);
    }

if (names_and_data_file_info.first_data_frame == NULL)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  -> need to first run loadsig before using evalsig",sig_name);
      return(1);
    }

if (strcmp(storage_info->filename,names_and_data_file_info.filename) != 0)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  ->  internal data storage needs to be updated\n  ---> need to rerun loadsig on file '%s'\n       (Note that file represented in data storage is '%s')",
	      sig_name, storage_info->filename, names_and_data_file_info.filename);
      return(1);
    }

if (storage_info->num_sigs != names_and_data_file_info.num_sigs)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  ->  internal data storage needs to be updated\n  ---> need to rerun loadsig on file '%s'\n       (Note that num_sigs in data storage is %d compared to assumed value of %d)",
	      sig_name, storage_info->filename, names_and_data_file_info.num_sigs, storage_info->num_sigs);
      return(1);
    }

if (storage_info->num_samples != names_and_data_file_info.num_samples)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  ->  internal data storage needs to be updated\n  ---> need to rerun loadsig on file '%s'\n       (Note that num_samples in data storage is %d compared to assumed value of %d)",
	      sig_name, storage_info->filename, names_and_data_file_info.num_samples, storage_info->num_samples);
      return(1);
    }

if (storage_info->num_sig_vals != names_and_data_file_info.num_sig_vals)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  ->  internal data storage needs to be updated\n  ---> need to rerun loadsig on file '%s'\n       (Note that num_sig_vals in data storage is %d compared to assumed value of %d)",
	      sig_name, storage_info->filename, names_and_data_file_info.num_sig_vals, storage_info->num_sig_vals);
      return(1);
    }

if (storage_info->analysis_type != names_and_data_file_info.analysis_type)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  ->  internal data storage needs to be updated\n  ---> need to rerun loadsig on file '%s'\n       (Note that analysis_type in data storage is %d compared to assumed value of %d)",
	      sig_name, storage_info->filename, names_and_data_file_info.analysis_type, storage_info->analysis_type);
      return(1);
    }

if (storage_info->complex_data_flag != names_and_data_file_info.complex_data_flag)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  ->  internal data storage needs to be updated\n  ---> need to rerun loadsig on file '%s'\n       (Note that complex_data_flag in data storage is %d compared to assumed value of %d)",
	      sig_name, storage_info->filename, names_and_data_file_info.complex_data_flag, storage_info->complex_data_flag);
      return(1);
    }

if (storage_info->operating_point_flag != names_and_data_file_info.operating_point_flag)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  ->  internal data storage needs to be updated\n  ---> need to rerun loadsig on file '%s'\n       (Note that operating_point_flag in data storage is %d compared to assumed value of %d)",
	      sig_name, storage_info->filename, names_and_data_file_info.operating_point_flag, storage_info->operating_point_flag);
      return(1);
    }

if (storage_info->complex_data_flag != 1)
    {
      sprintf(error_msg,"Error using evalsig('%s'):\n  ->  an internal call was made to return complex values, but complex_data_flag = %d and not 1\n  --> error occurred for ngspice output file '%s'",
	      sig_name, names_and_data_file_info.complex_data_flag, storage_info->filename);
      return(1);
    }



cur_sig_name = names_and_data_file_info.first_sig_name;
for (sig_num = 0; sig_num < names_and_data_file_info.num_sigs; sig_num++)
    {
      if (strcmp(sig_name,cur_sig_name->name) == 0)
	 break;
     cur_sig_name = cur_sig_name->next;  
    }

if (sig_num == names_and_data_file_info.num_sigs)
   {
     sprintf(error_msg,"Error using evalsig('%s'):\n  -> signal '%s' does not exist as of the last loadsig\n       of file '%s'",
	     sig_name, sig_name, storage_info->filename);
     return(1);  
   }

cur_data_frame = names_and_data_file_info.first_data_frame;

count = 0;
sig_num = sig_num*2 - 1;

for (j = 0; j < names_and_data_file_info.num_samples; j++)
  {
   for (i = 0; i < names_and_data_file_info.num_sig_vals; i++)
      {
	if (count == cur_data_frame->length)
	  {
	    cur_data_frame = cur_data_frame->next;
	    if (cur_data_frame == NULL)
	      {
		sprintf(error_msg,"error using evalsig('%s'): data frame too short!\n", sig_name);
		return(1);
	      }
            count = 0;
	  }
	if (i == sig_num)
	  {
           data_real[j] = cur_data_frame->dbuf[count];
	  }
	else if (i == (sig_num+1))
	  {
           data_imag[j] = cur_data_frame->dbuf[count];
	  }
        count++;
      }
  }
return(0);
}

int lssig(STORAGE_INFO *storage_info, char *sig_name, char *error_msg)
{
NAME_LIST *cur_sig_name;
int i;

if (names_and_data_file_info.first_sig_name == NULL)
    {
      strcpy(error_msg,"Error using lssig: need to first run loadsig before using lssig");
      return(1);
    }

if (names_and_data_file_info.first_data_frame == NULL)
    {
      strcpy(error_msg,"Error using lssig: need to first run loadsig before using lssig");
      return(1);
    }

if (strcmp(storage_info->filename,names_and_data_file_info.filename) != 0)
    {
      sprintf(error_msg,"Error using lssig:  internal data storage needs to be updated\n  ---> need to rerun loadsig on file '%s'\n       (Note that file represented in data storage is '%s')",
	      storage_info->filename, names_and_data_file_info.filename);
      return(1);
    }

if (storage_info->num_sigs != names_and_data_file_info.num_sigs)
    {
      sprintf(error_msg,"Error using lssig:  internal data storage needs to be updated\n  ---> need to rerun loadsig on file '%s'\n       (Note that num_sigs in data storage is %d compared to assumed value of %d)",
	      storage_info->filename, names_and_data_file_info.num_sigs, storage_info->num_sigs);
      return(1);
    }


if (names_and_data_file_info.cur_sig_count >= names_and_data_file_info.num_sigs)
   {
     sprintf(sig_name,"NULL");
     names_and_data_file_info.cur_sig_count = 0;
     return(0);
   }

cur_sig_name = names_and_data_file_info.first_sig_name;
for (i = 0; i < names_and_data_file_info.num_sigs; i++)
    {
      if (i == names_and_data_file_info.cur_sig_count)
	 break;
     cur_sig_name = cur_sig_name->next;  
    }

strcpy(sig_name,cur_sig_name->name);
names_and_data_file_info.cur_sig_count++;

return(0);
}

int initialize(void)
{
if (names_and_data_file_info.first_sig_name != NULL)
  {
     free_name_list(names_and_data_file_info.first_sig_name);
     names_and_data_file_info.first_sig_name = NULL;
  }

if (names_and_data_file_info.first_data_frame != NULL)
  {
     free_data_frames(names_and_data_file_info.first_data_frame);
     names_and_data_file_info.first_data_frame = NULL;
  }

strcpy(names_and_data_file_info.filename,"NULL");
names_and_data_file_info.num_sigs = 0;
names_and_data_file_info.num_samples = 0;
names_and_data_file_info.cur_sig_count = 0;
}

int loadsig(STORAGE_INFO *storage_info, char *error_msg)
{
int num_sigs, num_samples;

if (names_and_data_file_info.first_sig_name != NULL)
  {
     free_name_list(names_and_data_file_info.first_sig_name);
     names_and_data_file_info.first_sig_name = NULL;
  }

if (names_and_data_file_info.first_data_frame != NULL)
  {
     free_data_frames(names_and_data_file_info.first_data_frame);
     names_and_data_file_info.first_data_frame = NULL;
  }

strcpy(names_and_data_file_info.filename,storage_info->filename);

names_and_data_file_info.first_data_frame = init_data_frame();
if (names_and_data_file_info.first_data_frame == NULL)
   {
     sprintf(error_msg,"Error using loadsig('%s'):\n  -> unable to allocate memory for data frame\n  ---> Insufficient memory to load in file",storage_info->filename);
     return(1);
   }

names_and_data_file_info.first_sig_name = init_name_list();
if (names_and_data_file_info.first_sig_name == NULL)
   {
     sprintf(error_msg,"Error using loadsig('%s'):\n  -> unable to allocate memory for signal list\n  ---> Insufficient memory to load in file",storage_info->filename);
     return(1);
   }

strcpy(error_msg,"NULL");
if (load_data_from_file(storage_info->filename, error_msg) == 1)
    return(1);


storage_info->num_sigs = names_and_data_file_info.num_sigs;
storage_info->num_samples = names_and_data_file_info.num_samples;
storage_info->num_sig_vals = names_and_data_file_info.num_sig_vals;
storage_info->analysis_type = names_and_data_file_info.analysis_type;
storage_info->complex_data_flag = names_and_data_file_info.complex_data_flag;
storage_info->operating_point_flag = names_and_data_file_info.operating_point_flag;

return(0);
}


int load_data_from_file(char *filename, char *error_msg)
{
FILE *fp;
int i;
NAME_LIST *cur_sig_name;
DATA_FRAME *cur_data_frame;


if ((fp = fopen(filename,"rb")) == NULL)   
  {
   sprintf(error_msg,"Error using loadsig('%s'):\n  -> file can't be opened", filename);
   return(1);
  }

if (ngsp_allocate_sf(fp, filename, error_msg) == 1)
   {
    /* sprintf(error_msg,"Error using loadsig('%s'):\n  -> file is not in ngspice raw format", filename); */
   return(1);
   }


if (names_and_data_file_info.first_sig_name == NULL)
   names_and_data_file_info.first_sig_name = init_name_list();
if (names_and_data_file_info.first_sig_name == NULL)
   {
     sprintf(error_msg,"Error using loadsig('%s'):\n  -> unable to allocate memory for signal list\n  ---> Insufficient memory to load in file",filename);
     return(1);
   }


if (ngsp_determine_variables(fp,filename,error_msg) == 1)
   return(1);

/******
cur_sig_name = names_and_data_file_info.first_sig_name;
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
******/

if (names_and_data_file_info.first_data_frame == NULL)
     names_and_data_file_info.first_data_frame = init_data_frame();

if (names_and_data_file_info.first_data_frame == NULL)
   {
    sprintf(error_msg, "Error using loadsig('%s'):\n  -> calloc call failed for data frame creation (out of memory)", filename);
    return(1);
   }


if (ngsp_read_data(fp,filename,error_msg) == 1)
   return(1);

fclose(fp);

return(0);
}

int ngsp_determine_variables(FILE *fp, char *filename, char *error_msg)
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
	   names_and_data_file_info.analysis_type = DC_ANALYSIS;
	   data_valid_flag = 1;
	   }
	 else if (strncmp(&buf[10],"AC Analysis",11) == 0)
	   {
	   names_and_data_file_info.analysis_type = AC_ANALYSIS;
	   data_valid_flag = 1;
	   }
	 else if (strncmp(&buf[10],"Noise Spectral Density",22) == 0)
	   {
	   names_and_data_file_info.analysis_type = NOISE_ANALYSIS;
	   data_valid_flag = 1;
	   }
	 else if (strncmp(&buf[10],"Transient Analysis",18) == 0)
	   {
	   names_and_data_file_info.analysis_type = TR_ANALYSIS;
	   data_valid_flag = 1;
	   }
	 else if (strncmp(&buf[10],"Operating Point",15) == 0)
	   {
	   names_and_data_file_info.analysis_type = OP_ANALYSIS;
	   if (names_and_data_file_info.operating_point_flag == 1)
	      data_valid_flag = 1;
	   else
	      data_valid_flag = 0;
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
		if (names_and_data_file_info.analysis_type != AC_ANALYSIS)
		  {
                    sprintf(error_msg, "Error using loadsig('%s'):\n  -> complex data encountered but not AC analysis", filename);
		    return(1);
		  }
		complex_data_flag = 1;
	      }

          }
	else if (record_variables_flag == 1)
	  {
	    if (count < names_and_data_file_info.num_sigs)
	      {
		pch = strtok(buf," \t");
		if (count != atoi(pch))
		  {
                    sprintf(error_msg, "Error using loadsig('%s'):\n  -> variable list in file is not numbered correctly", filename);
		    return(1);
		  }
		pch = strtok(NULL," \t");
		strcpy(signal_name,pch);
		pch = strtok(NULL," \t\n");
		if (strcmp(pch,"voltage") == 0)
		  {
		    if (strcmp(signal_name,"v(i-sweep)") == 0)
		      {
		       if (count == 0)
		           strcpy(signal_name,"CURRENT");
		      }
		    else
		      {
		       if (count == 0)
		           strcpy(signal_name,"VOLTAGE");
		      }

		    strcpy(temp_name,signal_name);
		    if (strncmp(temp_name,"v(",2) == 0)
		      {
		      strcpy(signal_name,&temp_name[2]);
		      for (i = 0; signal_name[i] != '\0'; i++);
		      i--;
		      if (signal_name[i] != ')')
			{
                          sprintf(error_msg, "Error using loadsig('%s'):\n  -> voltage signal '%s' is not labeled correctly", filename, temp_name);
			  return(1);
			}
		      signal_name[i] = '\0';
		      }
		  }
		else if (strcmp(pch,"current") == 0)
		  {
		    if (count == 0)
		        strcpy(signal_name,"CURRENT");

		    if (strncmp(signal_name,"i(",2) == 0)
		      {
		      signal_name[1] = '_';
		      for (i = 0; signal_name[i] != '\0'; i++);
		      i--;
		      if (signal_name[i] != ')')
			{
                          sprintf(error_msg, "Error using loadsig('%s'):\n  -> current signal '%s' is not labeled correctly", filename, temp_name);
			  return(1);
			}
		      signal_name[i] = '\0';
		      }
		  }
		else if (strcmp(pch,"frequency") == 0)
		  {
		    if (strcmp(signal_name,"frequency") != 0)
		      {
                        sprintf(error_msg, "Error using loadsig('%s'):\n  -> frequency independent variable must be 'FREQUENCY' instead of '%s'", 
                              filename, signal_name);
			return(1);
		      }
		    strcpy(signal_name,"FREQUENCY");
		  }
		else if (strcmp(pch,"time") == 0)
		  {
		    if (strcmp(signal_name,"time") != 0)
		      {
                        sprintf(error_msg, "Error using loadsig('%s'):\n  -> time independent variable must be 'TIME' instead of '%s'", 
                              filename, signal_name);
			return(1);
		      }
		    strcpy(signal_name,"TIME");
		  }
		else
		  {
                    if (count == 0)
		        strcpy(signal_name,"UNKNOWN");
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
		    if (names_and_data_file_info.first_sig_name == NULL)
                        names_and_data_file_info.first_sig_name = init_name_list();
                    if (names_and_data_file_info.first_sig_name == NULL)
                       {
                        sprintf(error_msg,"Error using loadsig('%s'):\n  -> unable to allocate memory for signal list\n  ---> Insufficient memory to load in file",filename);
                        return(1);
                       }

                    cur_sig_name = names_and_data_file_info.first_sig_name;
		    strcpy(cur_sig_name->name,signal_name);                    
		  }
                else if (cur_sig_name != NULL)
		  {
		    cur_sig_name->next = init_name_list();
                    cur_sig_name = cur_sig_name->next;
                    if (cur_sig_name == NULL)
                       {
                        sprintf(error_msg,"Error using loadsig('%s'):\n  -> unable to allocate memory for signal list\n  ---> Insufficient memory to load in file",filename);
                        return(1);
                       }

		    strcpy(cur_sig_name->name,signal_name);
		  }
	      }
	    count++;
	    if (count == names_and_data_file_info.num_sigs)
	       record_variables_flag = 0;
	  }
       }
   }

return(0);
}


int ngsp_allocate_sf(FILE *fp, char *filename, char *error_msg)
{
int num_signals,num_sig_vals,num_samples;
int data_valid_flag, complex_data_flag, operating_point_flag;
char buf[BSIZE_SP];

/* probably not necessary to rewind, but just to make sure ... */
rewind(fp);
fgets(buf, BSIZE_SP, fp);
if (strncmp(buf,"Title: ",7) != 0)
  {
   sprintf(error_msg,"Error using loadsig('%s'):\n  -> file is not in Ngspice raw format",filename);
   return(1);
  }

rewind(fp);

 num_signals = -1;
 num_sig_vals = -1;
 num_samples = -1;

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
	   }
	 else if (strncmp(&buf[10],"AC Analysis",11) == 0)
	   {
	   data_valid_flag = 1;
	   }
	 else if (strncmp(&buf[10],"Noise Spectral Density",22) == 0)
	   {
	   data_valid_flag = 1;
	   }
	 else if (strncmp(&buf[10],"Transient Analysis",18) == 0)
	   {
	   data_valid_flag = 1;
	   }
	 else if (strncmp(&buf[10],"Operating Point",15) == 0)
	   {
	   data_valid_flag = 0;
	   }
       }
     else if (data_valid_flag == 1)
       {
        if (strncmp(buf,"No. Variables: ",15) == 0)
          {
           num_signals = atoi(&buf[15]);
          }
        else if (strncmp(buf,"No. Points: ",12) == 0)
          {
           num_samples = atoi(&buf[12]);
          }
        else if (strncmp(buf,"Flags: ",7) == 0)
          {
	    if (strncmp(&buf[7],"complex",7) == 0)
	      {
		complex_data_flag = 1;
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
	   operating_point_flag = 1;
	   }
         }
       else if (data_valid_flag == 1)
         {
          if (strncmp(buf,"No. Variables: ",15) == 0)
            {
             num_signals = atoi(&buf[15]);
            }
          else if (strncmp(buf,"No. Points: ",12) == 0)
            {
             num_samples = atoi(&buf[12]);
            }
          else if (strncmp(buf,"Flags: ",7) == 0)
            {
	     if (strncmp(&buf[7],"complex",7) == 0)
	        {
		complex_data_flag = 1;
	        }
            }
         }
      }
     if (num_signals == -1) 
        {
	 /* not even operating point information is available */
         sprintf(error_msg,"Error using loadsig('%s'):\n  -> num_signals is undefined",filename);
	 return(1);
	}
   }

if (complex_data_flag == 1)
   num_sig_vals = 2*num_signals-1;
else
   num_sig_vals = num_signals;

if (num_samples == -1)
   {
     sprintf(error_msg,"Error using loadsig('%s'):\n  -> num_samples is undefined",filename);
     return(1);
   }

strcpy(names_and_data_file_info.filename, filename);
names_and_data_file_info.analysis_type = -1;
names_and_data_file_info.num_sigs = num_signals;
names_and_data_file_info.num_sig_vals = num_sig_vals;
names_and_data_file_info.num_samples = num_samples;
names_and_data_file_info.complex_data_flag = complex_data_flag;
names_and_data_file_info.operating_point_flag = operating_point_flag;
names_and_data_file_info.cur_sig_count = 0;

return(0);
}


int ngsp_read_data(FILE *fp, char *filename, char *error_msg)
{
int i,j,num_sig_vals;
double val, scratch_val;
DATA_FRAME *cur_data_frame;


if (names_and_data_file_info.first_data_frame == NULL)
   names_and_data_file_info.first_data_frame = init_data_frame();

cur_data_frame = names_and_data_file_info.first_data_frame;
if (cur_data_frame == NULL)
   {
    sprintf(error_msg, "Error using loadsig('%s'):\n  -> calloc call failed for data frame creation (out of memory)", filename);
    return(1);
   }
cur_data_frame->cur_index = 0;

for (j = 0; j < names_and_data_file_info.num_samples; j++)
       {
	num_sig_vals = names_and_data_file_info.num_sig_vals;

	for (i = 0; i < num_sig_vals; i++) 
           {
	    if(fread_double_values(&val, fp) != 1) 
               {
	       sprintf(error_msg, "Error using loadsig('%s'):\n  -> unexpected EOF at field %d", filename, i);
	       return(1);
	       }
	    if (i == 0 && names_and_data_file_info.analysis_type == AC_ANALYSIS) 
	      {
	       if(fread_double_values(&scratch_val, fp) != 1) 
                  {
	           sprintf(error_msg, "Error using loadsig('%s'):\n  -> unexpected EOF at field %d (AC analysis)", filename, i);
	           return(1);
	          }
	      }

	    if (cur_data_frame->dbuf != NULL)
	      {
	       if (cur_data_frame->cur_index >= cur_data_frame->length)
		  {
		   cur_data_frame->next = init_data_frame();
		   cur_data_frame = cur_data_frame->next;
                   if (cur_data_frame == NULL)
                      {
                      sprintf(error_msg, "Error using loadsig('%s'):\n  -> calloc call failed for data frame creation (out of memory)", filename);
                      return(1);
                      }
		  }
	       cur_data_frame->dbuf[cur_data_frame->cur_index] = val;
                    cur_data_frame->cur_index++;
	      }
	   }
       }

return(0);
}
	

/* allow for future fix in case of problem with big endian, little endian issue */
int fread_double_values(double *fval, FILE *fp)
  {
   if (fread(fval,8,1,fp) != 1)
      return(0);

   return(1);
  }


DATA_FRAME *init_data_frame()
{
DATA_FRAME *A; 
   

if ((A = (DATA_FRAME *) calloc(1,sizeof(DATA_FRAME))) == NULL)   
   {   
   return(NULL);   
  }

A->cur_index = 0;  
A->length = 2048;
A->next = NULL;

if ((A->dbuf = (double *) calloc(A->length,sizeof(double))) == NULL)   
      { 
      return(NULL);   
      }

return(A);
}


NAME_LIST *init_name_list()
{
NAME_LIST *A; 
   
if ((A = (NAME_LIST *) calloc(1,sizeof(NAME_LIST))) == NULL)   
   {   
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
        free(prev_data_frame->dbuf);
    free(prev_data_frame);
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
    free(prev_name_list);
   }
}

