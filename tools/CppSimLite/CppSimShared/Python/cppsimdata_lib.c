/***************
cppsimdata_lib.c 
written by Michael H. Perrott
available at www.cppsim.com as part of the CppSim package

This file is compiled in order to create a shared library file that
is used to support the CppSim data module for Python.
 
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
/* #include "mat.h" */

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

struct NAME_LIST
  {
  unsigned char name[256];
  struct NAME_LIST *next;
};
typedef struct NAME_LIST NAME_LIST;

struct HEADER_FRAME
  {
  unsigned char buf[8192];
  int length;
  struct HEADER_FRAME *next;
};
typedef struct HEADER_FRAME HEADER_FRAME;

struct DATA_FRAME
  {
  float *fbuf;
  double *dbuf;
  int length, data_type;
  struct DATA_FRAME *next;
};
typedef struct DATA_FRAME DATA_FRAME;

struct NAMES_AND_DATA_STORAGE
  {
   struct NAME_LIST *first_sig_name;
   struct DATA_FRAME *first_data_frame;
   char filename[256];
   int num_sigs;
   int num_samples;
   int cur_sig_count;
};
typedef struct NAMES_AND_DATA_STORAGE NAMES_AND_DATA_STORAGE;

struct STORAGE_INFO
  {
   char *filename;
   int num_sigs;
   int num_samples;
};
typedef struct STORAGE_INFO STORAGE_INFO;

#ifdef _WIN32
static struct NAMES_AND_DATA_STORAGE names_and_data_file_info = {NULL, NULL, "NULL", 0, 0, 0};
#else
static struct NAMES_AND_DATA_STORAGE names_and_data_file_info = {.first_sig_name = NULL, .first_data_frame = NULL, .filename = "NULL", .num_sigs = 0, .num_samples = 0, .cur_sig_count = 0};
#endif

int CPPSIM_OLD_FILE_FLAG;

int load_data_from_file(char *filename,HEADER_FRAME *first_header_frame,
                        DATA_FRAME *first_data_frame,int *num_signals,
                        int *num_samples, char *error_msg);
int fread_int_values(FILE *fp, unsigned int *ibuf, int num_of_bytes, char *filename, char *error_msg);
int fread_char_values(FILE *fp, unsigned char *cbuf, int num_of_bytes);
int fread_float_values(FILE *fp, float *fbuf, int num_of_floats);
int fread_double_values(FILE *fp, double *dbuf, int num_of_doubles);
void print_header_frames(HEADER_FRAME *first_header_frame);
HEADER_FRAME *init_header_frame();
DATA_FRAME *init_data_frame();
int create_data_frame_buf(DATA_FRAME *data_frame);
NAME_LIST *init_name_list();
void print_name_list(NAME_LIST *first_sig_name);
int extract_signal_names(HEADER_FRAME *first_header_frame,int num_sigs,
                         NAME_LIST *first_sig_name, char *filename, char *error_msg);
void free_header_frames(HEADER_FRAME *first_header_frame);
void free_data_frames(DATA_FRAME *first_data_frame);
void free_name_list(NAME_LIST *first_name_list);


DLLEXPORT int evalsig(STORAGE_INFO *storage_info, char *sig_name, double *data, char *error_msg);
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
   for (i = 0; i < names_and_data_file_info.num_sigs; i++)
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
	   if (cur_data_frame->data_type == 32)
              data[j] = cur_data_frame->fbuf[count];
           else
              data[j] = cur_data_frame->dbuf[count];
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
HEADER_FRAME *first_header_frame;

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

first_header_frame = init_header_frame();
if (first_header_frame == NULL)
   {
     sprintf(error_msg,"Error using loadsig('%s'):\n  -> unable to allocate memory for header frame\n  ---> Insufficient memory to load in file",storage_info->filename);
     return(1);
   }
names_and_data_file_info.first_sig_name = init_name_list();
if (names_and_data_file_info.first_sig_name == NULL)
   {
     sprintf(error_msg,"Error using loadsig('%s'):\n  -> unable to allocate memory for signal list\n  ---> Insufficient memory to load in file",storage_info->filename);
     return(1);
   }

strcpy(error_msg,"NULL");
if (load_data_from_file(storage_info->filename, first_header_frame,
                        names_and_data_file_info.first_data_frame,
                        &num_sigs, &num_samples, error_msg) == 1)
  {
    return(1);
  }

 if (extract_signal_names(first_header_frame,num_sigs,names_and_data_file_info.first_sig_name, storage_info->filename, error_msg) == 1)
  {
    return(1);
  }

names_and_data_file_info.num_sigs = num_sigs;
names_and_data_file_info.num_samples = num_samples;

storage_info->num_sigs = num_sigs;
storage_info->num_samples = num_samples;

free_header_frames(first_header_frame);
first_header_frame = NULL;

return(0);
}


int load_data_from_file(char *filename,HEADER_FRAME *first_header_frame,
                        DATA_FRAME *first_data_frame,int *num_signals,
                        int *num_samples, char *error_msg)
{
int i,j, num_sigs;
char num_sigs_text[5],output_version[5];
int status, data_type, buf_length, buf_length_confirm, buf_length_actual;
unsigned int ibuf[20];
HEADER_FRAME *cur_header_frame;
DATA_FRAME *cur_data_frame;
FILE *fid;
int data_frame_count, buf_length_float, buf_length_double;
int num_sample_points, remainder, last_buf_length;
unsigned char temp_buf[1000];
int found_header_flag, found_data_flag, data_64_flag;

cur_data_frame = NULL;
cur_header_frame = NULL;
data_64_flag = 0;

if ((fid = fopen(filename,"rb")) == NULL)   
  {
   sprintf(error_msg,"Error using loadsig('%s'):\n  -> file can't be opened", filename);
   return(1);
  }

/*****************  read in header ******************/

CPPSIM_OLD_FILE_FLAG = 0;
data_frame_count = 0;
found_header_flag = 0;
found_data_flag = 0;

if (fread_int_values(fid, ibuf, 4, filename, error_msg) != 4) 
   {
    if (strcmp(error_msg,"NULL") == 0)
        sprintf(error_msg, "\nError using loadsig('%s'):\n  -> file appears to be empty!", filename);
    return(1);
   }
if (ibuf[0] != 4 && ibuf[0] != 8)
   {
    CPPSIM_OLD_FILE_FLAG = 1;
    //    printf("...reading in old version of CppSim simulation file...\n");
    rewind(fid);
    if (fread_int_values(fid, ibuf, 4, filename, error_msg) != 4) 
       {
       if (strcmp(error_msg,"NULL") == 0)
           sprintf(error_msg, "Error using loadsig('%s'):\n  -> encountered a read error for file", filename);
       return(1);
       }
    if (ibuf[0] != 4 && ibuf[0] != 8)
      {
       sprintf(error_msg, "Error using loadsig('%s'):\n  -> file is corrupted", filename);
       return(1);
      }
   }
rewind(fid);

while(1)
   {
   /*   read in whether header or data (header:  112, data: 128)   */
   /*   and buffer length of next data segment   */

   if (fread_int_values(fid, ibuf, 16, filename, error_msg) != 16) 
       {
       if (strcmp(error_msg,"NULL") != 0)
	  return(1);
       status = -1;
       break;
       }
   data_type = ibuf[1]; 
   buf_length = ibuf[3]; 

   /* printf("data_type = %d, buf_length = %d\n",data_type,buf_length); */
   if (data_type == 112)
     {
      /*    read current header segment   */
      if (cur_header_frame == NULL)
	  {
          cur_header_frame = first_header_frame;
	  }
      else
          {
	  cur_header_frame->next = init_header_frame();
          if (cur_header_frame->next == NULL)
              {
               sprintf(error_msg,"Error using loadsig('%s'):\n  ->  unable to allocate memory for header frame\n  ---> Insufficient memory to load in file",filename);
               return(1);
              }

	  cur_header_frame = cur_header_frame->next;
          }

      buf_length_actual = fread_char_values(fid, cur_header_frame->buf, 
	      buf_length);

      cur_header_frame->length = buf_length_actual;
      if (buf_length_actual != buf_length) 
          {
	  status = -1;
  	  break;
          }

      if (found_header_flag == 0)
	{
         for (i = 16; i < 20; i++)
            output_version[i-16] = first_header_frame->buf[i];
         output_version[4] = '\0';

         /* printf("num_sigs = %d, output_version = %s\n",num_sigs,output_version); */

         if (strcmp(output_version,"9601") != 0)
            {
             if (strcmp(output_version,"9602") == 0)
                data_64_flag = 1;
             else
               {
		 sprintf(error_msg, "Error using loadsig('%s'):\n  -> file is not Hspice version 9601 (or version 9602 for CppSim 64-bit data)", filename);
                return(1);
               }
            }
         found_header_flag = 1;
	}
     }
   else if (data_type == 128)
     {
       if (found_header_flag == 0)
	 {
	  sprintf(error_msg, "Error using loadsig('%s'):\n  -> did not find header in file", filename);
          return(1);
	 }
      found_data_flag = 1;

      data_frame_count++;
      /*    read current data segment   */
      if (cur_data_frame == NULL)
          cur_data_frame = first_data_frame;
      else
          {
	  cur_data_frame->next = init_data_frame();
          if (cur_data_frame->next == NULL)
              {
               sprintf(error_msg,"Error using loadsig('%s'):\n  -> unable to allocate memory for data frame\n  ---> Insufficient memory to load in file",filename);
               return(1);
              }
	  cur_data_frame = cur_data_frame->next;
          }

      if (data_64_flag == 0) /* 32-bit data */
	{
	 cur_data_frame->data_type = 32;
         if (create_data_frame_buf(cur_data_frame) == 0)
              {
               sprintf(error_msg,"Error using loadsig('%s'):\n  ->  unable to allocate memory for data frame buffer\n  ---> Insufficient memory to load in file",filename);
               return(1);
              }
         buf_length_float = buf_length/4;
         /*  printf("buf_length_float = %d\n",buf_length_float);  */
         buf_length_actual = fread_float_values(fid, cur_data_frame->fbuf, 
                                                buf_length_float);
         cur_data_frame->length = buf_length_actual;
         /*      printf("buf_length_actual = %d\n",buf_length_actual);   */
         if (buf_length_actual != buf_length_float) 
           {
	    /*	    printf("data_frame_count = %d, buf_length = %d\n",
		    data_frame_count, buf_length_actual); */
	    status = -1;
  	    break;
           }
	}
      else /* 64-bit data */
	{
	 cur_data_frame->data_type = 64;
         if (create_data_frame_buf(cur_data_frame) == 0)
              {
               sprintf(error_msg,"Error using loadsig('%s'):\n  ->  unable to allocate memory for data frame buffer\n  ---> Insufficient memory to load in file",filename);
               return(1);
              }
         buf_length_double = buf_length/8;
         /*  printf("buf_length_double = %d\n",buf_length_double);  */
         buf_length_actual = fread_double_values(fid, cur_data_frame->dbuf, 
                                                buf_length_double);
         cur_data_frame->length = buf_length_actual;
         /*      printf("buf_length_actual = %d\n",buf_length_actual);   */
         if (buf_length_actual != buf_length_double) 
           {
	    /*	    printf("data_frame_count = %d, buf_length = %d\n",
		    data_frame_count, buf_length_actual); */
	    status = -1;
  	    break;
           }
	}
     }
   else
     {
       sprintf(error_msg, "Error using loadsig('%s'):\n  -> unrecognized data type segment", filename);
       return(1);
     }
/* grab end of current buffer segment and check that data is OK */

   if (fread_int_values(fid,ibuf, 4, filename, error_msg) != 4) 
       {
	if (strcmp(error_msg,"NULL") != 0)
	   return(1);
	status = -1;
	break;
       } 
   buf_length_confirm = ibuf[0]; 
   /*   printf("buf_length_confirm = %d\n",buf_length_confirm); */

   if (buf_length != buf_length_confirm)
     {
      sprintf(error_msg, "Error using loadsig('%s'):\n  ->  buf_length not equal to buf_length_confirm", filename);
      return(1);
     } 
   }
if (found_header_flag == 0 || found_data_flag == 0)
   {
     sprintf(error_msg, "Error using loadsig('%s'):\n  ->  attempt to read file was unsuccessful", filename);
     return(1);
   }

last_buf_length = buf_length_actual;

for (i = 0; i < 4; i++)
    num_sigs_text[i] = first_header_frame->buf[i];
num_sigs_text[4] = '\0';

num_sigs = atoi(num_sigs_text);




num_sample_points = (data_frame_count-1)*2048 + last_buf_length;
remainder = num_sample_points % num_sigs;
num_sample_points = (num_sample_points - remainder)/num_sigs;

/* printf("num_sample_points = %d\n",num_sample_points);  */

/* print_header_frames(first_header_frame); */

*num_samples = num_sample_points;
*num_signals = num_sigs;
fclose(fid);
return(0);
}

int extract_signal_names(HEADER_FRAME *first_header_frame,int num_sigs,
                         NAME_LIST *first_sig_name, char *filename, char *error_msg)
{
int i, j, k, count;
HEADER_FRAME *cur_header_frame;
NAME_LIST *cur_sig_name;

cur_sig_name = NULL;

cur_header_frame = first_header_frame;
count = 0;
while(cur_header_frame != NULL)
  {
   if (cur_header_frame == first_header_frame)
      i = 264;
   else
      i = 0;

   for (   ; i < cur_header_frame->length; i++)
      {
      if (count == num_sigs)
          break;
      if (cur_header_frame->buf[i] == '1')
          count++;
      }
   if (count == num_sigs)
     {
       i++;
       break;
     }
   cur_header_frame = cur_header_frame->next;
  }
if (count != num_sigs)
   {
    sprintf(error_msg, "Error using loadsig('%s'):\n  -> in 'extract_signal_names':  not enough 1 entries for file", filename);
    return(1);
   }

for (j = 0; j < num_sigs; j++)
  {
   k = i;
   while(1)
     {
      if (k == cur_header_frame->length)
	{
	  if (cur_header_frame->next != NULL)
	     {
	      cur_header_frame = cur_header_frame->next;
              k = 0;
	     }
          else
	      break;
	}
      if (cur_header_frame->buf[k] != ' ')
           break;
      k++;
     }  

   i = k;
   if (cur_sig_name == NULL)
       cur_sig_name = first_sig_name;
   else
     {
       cur_sig_name->next = init_name_list();
       if (cur_sig_name->next == NULL)
          {
           sprintf(error_msg,"Error using loadsig('%s'):\n  -> unable to allocate memory for signal list\n  ---> Insufficient memory to load in file",filename);
          return(1);
          }
       cur_sig_name = cur_sig_name->next;
     }
   k = i;
   while(1)
     {
      if (k == cur_header_frame->length)
	{
	 if (cur_header_frame->next != NULL)
	   {
	     cur_header_frame = cur_header_frame->next;
             i = i-k;
             k = 0;
	   }
         else
	     break;
	}

      if (cur_header_frame->buf[k] == ' ')
         break;

      /***
      if (cur_header_frame->buf[k] != '.')
         cur_sig_name->name[k-i] = cur_header_frame->buf[k];
      else
         cur_sig_name->name[k-i] = '_';
      ***/
      cur_sig_name->name[k-i] = cur_header_frame->buf[k];
      k++;
     }
   cur_sig_name->name[k-i] = '\0';
   i = k;
  }
return(0);
}

void print_header_frames(HEADER_FRAME *first_header_frame)
{
HEADER_FRAME *cur_header_frame;
int i;

cur_header_frame = first_header_frame;

while (cur_header_frame != NULL)
   {
     for (i = 0; i < cur_header_frame->length; i++)
        printf("%c",(char) cur_header_frame->buf[i]);
     printf("\n");
     cur_header_frame = cur_header_frame->next;
   }   
}

void print_name_list(NAME_LIST *first_sig_name)
{
NAME_LIST *cur_sig_name;

cur_sig_name = first_sig_name;

while (cur_sig_name != NULL)
   {
     printf("%s\n",cur_sig_name->name);
     cur_sig_name = cur_sig_name->next;
   }   
}


int fread_int_values(FILE *fp, unsigned int *ibuf, int num_of_bytes, char *filename, char *error_msg)
  {
   unsigned char *charbuf;
   int i, num_int_values;
   size_t bytes_read;

   num_int_values = num_of_bytes/4;

   if ((charbuf = (unsigned char *) calloc(num_of_bytes,sizeof(unsigned char))) == NULL)   
         {   
	  sprintf(error_msg,"Error using loadsig('%s'):\n  -> in 'fread_int_values':  calloc call failed\n   ->  not enough memory to read file",
               filename);   
          return(-1);   
        }   

   bytes_read = fread(charbuf, sizeof(unsigned char),num_of_bytes, fp);

   for (i = 0; i < num_int_values; i++)
      {
	if (CPPSIM_OLD_FILE_FLAG == 1) 	/**** Former Endian method ****/
	  {
          ibuf[i] = charbuf[i*4]*(1<<24) + charbuf[i*4+1]*(1<<16) 
                     + charbuf[i*4+2]*(1<<8) + charbuf[i*4+3];
	  }
	else /**** New Endian method to match Hspice ****/
	  {
          ibuf[i] = charbuf[i*4+3]*(1<<24) + charbuf[i*4+2]*(1<<16) 
                     + charbuf[i*4+1]*(1<<8) + charbuf[i*4];
	  }
      }
   free(charbuf);
   return((int) bytes_read);
  }

int fread_char_values(FILE *fp, unsigned char *cbuf, int num_of_bytes)
  {
   size_t bytes_read;

   bytes_read = fread(cbuf, sizeof(unsigned char),num_of_bytes, fp);

   return((int) bytes_read);
  }

int fread_float_values(FILE *fp, float *fbuf, int num_of_floats)
  {
   size_t floats_read;

   floats_read = fread(fbuf, sizeof(float),num_of_floats, fp);

   return((int) floats_read);
  }

int fread_double_values(FILE *fp, double *dbuf, int num_of_doubles)
  {
   size_t double_read;

   double_read = fread(dbuf, sizeof(double), num_of_doubles, fp);

   return((int) double_read);
  }


HEADER_FRAME *init_header_frame()
{
HEADER_FRAME *A; 
   
if ((A = (HEADER_FRAME *) calloc(1,sizeof(HEADER_FRAME))) == NULL)   
   {   
   /*
   printf("error in 'init_header_frame':  calloc call failed\n");   
   printf("out of memory!\n");   
   */
   return(NULL);   
  }   
A->length = 8192;
A->next = NULL;
return(A);
}

DATA_FRAME *init_data_frame()
{
DATA_FRAME *A; 
   
if ((A = (DATA_FRAME *) calloc(1,sizeof(DATA_FRAME))) == NULL)   
   {   
   /*
   printf("error in 'init_data_frame':  calloc call failed\n");   
   printf("out of memory!\n");   
   */
   return(NULL);   
  }
A->data_type = 32;
A->fbuf = NULL;
A->dbuf = NULL;   
A->length = 2048;
A->next = NULL;
return(A);
}

int create_data_frame_buf(DATA_FRAME *data_frame)
{
if (data_frame->data_type == 32)   
  {
   if ((data_frame->fbuf = (float *) calloc(2048,sizeof(float))) == NULL)   
      {   
      /**
      printf("error in 'create_data_frame_buf':  calloc call failed\n");   
      printf("out of memory!\n");   
      **/
      return(0);   
      }
  }   
else
  {
   if ((data_frame->dbuf = (double *) calloc(2048,sizeof(double))) == NULL)   
      { 
      /**  
      printf("error in 'create_data_frame_buf':  calloc call failed\n");   
      printf("out of memory!\n");   
      **/
      return(0);   
      }
  }   
return(2048);
}

NAME_LIST *init_name_list()
{
NAME_LIST *A; 
   
if ((A = (NAME_LIST *) calloc(1,sizeof(NAME_LIST))) == NULL)   
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

void free_header_frames(HEADER_FRAME *first_header_frame)
{
HEADER_FRAME *cur_header_frame, *prev_header_frame;

cur_header_frame = first_header_frame;

while(cur_header_frame != NULL)
   {
    prev_header_frame = cur_header_frame;
    cur_header_frame = cur_header_frame->next;
    free(prev_header_frame);
   }
}

void free_data_frames(DATA_FRAME *first_data_frame)
{
DATA_FRAME *cur_data_frame, *prev_data_frame;

cur_data_frame = first_data_frame;

while(cur_data_frame != NULL)
   {
    prev_data_frame = cur_data_frame;
    cur_data_frame = cur_data_frame->next;
    if (prev_data_frame->fbuf != NULL)
        free(prev_data_frame->fbuf);
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
