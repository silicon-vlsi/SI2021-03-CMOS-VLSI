/* $Id$ */

/*********************************************************
      CppSim Classes for Behavioral Simulation of Systems
  written by Michael H. Perrott (http://www.cppsim.com)
            Copyright 2002 by Michael H. Perrott
                 All rights reserved
    This software comes with no warranty or support

NOTE:  PLEASE DO NOT MODIFY THIS CODE - DOING SO WILL
       PREVENT YOU FROM SHARING YOUR SIMULATION CODE
       WITH OTHERS, AND WILL MAKE IT MUCH HARDER FOR
       YOU TO BENEFIT FROM FUTURE UPDATES
    **    IF YOU WANT TO CHANGE A CLASS, THEN COPY THE    **
    **    CODE AND SAVE IT UNDER A DIFFERENT CLASS NAME   **
    **    IN     local_classes_and_functions.cpp             **
    **  SUGGESTION FOR NEW NAME:  CLASSNAME_YOURINITIALS  **
*********************************************************/
/////////////////////////////////////////////////////////////////////////
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
// #include "config.h"
#include "cppsim_classes_mex.h"



int List::inp(const Vector &in)
{
int i;
vector_struct *x;

x = extract_vector_struct(in);

flush();

for (i = 0; i < x->length; i++)
  inp(x->elem[i]);

return(length);
}

int List::inp(Vector &in)
{
int i;
vector_struct *x;

x = extract_vector_struct(in);

flush();

for (i = 0; i < x->length; i++)
  inp(x->elem[i]);

return(length);
}

int List::inp(vector_struct *in)
{
int i;

flush();

for (i = 0; i < in->length; i++)
  inp(in->elem[i]);

return(length);
}

void List::print(const char *name)
{
int i;

reset();
i = 1;
while(notdone)
   mexPrintf("%s[%d] = %5.3e\n",name,i++,read());
mexPrintf("\n");
reset();
}

void List::copy(const List &other)
{
List temp(other);

flush();
temp.reset();
while(temp.notdone)
   inp(temp.read());
}

void List::copy(const Vector &other)
{
flush();
vector_struct *x;
x = extract_vector_struct(other);

int i;
for (i = 0; i < x->length; i++)
  inp(x->elem[i]);
}

void List::copy(const IntVector &other)
{
flush();
int_vector_struct *x;
x = extract_vector_struct(other);

int i;
for (i = 0; i < x->length; i++)
  inp((double) x->elem[i]);
}

List::List(const List &other)
{
length = other.length;
notdone = other.notdone;
out = other.out;
first_entry = other.first_entry;
cur_entry = first_entry;
cur_write_entry = NULL;
temp_flag = 1;
}

void List::set(const List &other)
{
delete_entries(first_entry);
length = other.length;
notdone = other.notdone;
out = other.out;
first_entry = other.first_entry;
cur_entry = first_entry;
cur_write_entry = NULL;
temp_flag = 1;
}



int List::load(const char *filename)   
{   
FILE *fp;   
char word[MAX_FILE_WORD+1];
   
if ((fp = fopen(filename,"r")) == NULL)   
  {   
   mexPrintf("\nerror in List.load: file %s can't be opened\n",filename);   
   mexErrMsgTxt("Exiting CppSim Mex object...");   
 }   
flush();

while(fscanf(fp,"%s",word) == 1)   
  {
   inp(atof(word));   
  }   

fclose(fp);
return(length);   
}   

void List::cat(const List &other)
{
List temp(other);

temp.reset();
while(temp.notdone)
  inp(temp.read());
}

void List::save(const char *filename)   
{   
FILE *fp;   
   
if ((fp = fopen(filename,"w")) == NULL)   
  {   
   mexPrintf("\nerror in List.save: file %s can't be opened\n",filename);   
   mexErrMsgTxt("Exiting CppSim Mex object...");   
 }   
reset();

while(notdone)   
  {
   fprintf(fp,"%12.10e\n",read());
  }   

fclose(fp);   
}   

List::List()
{
first_entry = create_entry();
first_entry->value = NaN;
cur_write_entry = first_entry;
cur_entry = first_entry;
length = 0;
notdone = 0;
out = NaN;
temp_flag = 0;
}


List::~List()
{
if (temp_flag == 0)
   delete_entries(first_entry);
}

void List::flush()
{
delete_entries(first_entry->next);
first_entry->next = NULL;
first_entry->value = NaN;
cur_write_entry = first_entry;
cur_entry = first_entry;
length = 0;
notdone = 0;
out = NaN;
}

int List::inp(double in)
{
notdone = 1;

if (cur_write_entry == NULL)
  {
   mexPrintf("error in List.inp:  cur_write_entry is NULL\n");
   mexPrintf(" odds are, you are trying to write to a passed list\n");
   mexPrintf(" which is not permitted\n");
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (length == 0)
   cur_write_entry->value = in;
else
  {
   cur_write_entry->next = create_entry();
   cur_write_entry = cur_write_entry->next;
   cur_write_entry->value = in;
  }

length++;
return(length);
}

void List::conv(const List &other)
{
double *array1,*array2,*result;
int lengthr, i, j;
List temp(other);

if (length == 0 || other.length == 0)
  {
   if (length == 0)
      mexPrintf("error in List.conv:  length of self list is zero\n");
   else
      mexPrintf("error in List.conv:  length of other list is zero\n");
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
lengthr = length+temp.length-1;

if ((array1 = (double *) calloc(length,sizeof(double))) == NULL)
   {
    mexPrintf("Error in List.conv: out of memory for coeff calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if ((array2 = (double *) calloc(temp.length,sizeof(double))) == NULL)
   {
    mexPrintf("Error in List.conv: out of memory for coeff calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if ((result = (double *) calloc(lengthr,sizeof(double))) == NULL)
   {
    mexPrintf("Error in List.conv: out of memory for coeff calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
reset();
for (i = 0; i < length; i++)
  array1[i] = read();

temp.reset();
for (i = 0; i < temp.length; i++)
  array2[i] = temp.read();

for (i = 0; i < lengthr; i++)
  result[i] = 0.0;

for (i = 0; i < length; i++)
   for (j = 0; j < temp.length; j++)
      result[i+j] += array1[i]*array2[j];

flush();
for (i = 0; i < lengthr; i++)
   inp(result[i]);

free(array1);
free(array2);
free(result);
}

double List::mean()
{
double total;

if (length == 0)
  {
   mexPrintf("error in List.mean:  list has zero length\n");
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
reset();
total = 0.0;

while(notdone)
   total += read();

return(total/((double) length));
}

double List::var()
{
double total,val,bias,meanval,rlength;

if (length == 0)
  {
   mexPrintf("error in List.var:  list has zero length\n");
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
reset();
total = 0.0;
while(notdone)
  {
   val = read();
   total += val*val;
  }
rlength = (double) length;
if (length == 1)
   bias = 0.0;
else
   bias = rlength/(rlength-1.0);
meanval = mean();

return(bias*((total/rlength) - meanval*meanval));
}

void List::add(double in)
{
reset();
while(notdone)
  {
   cur_entry->value += in;
   read();  //increments cur_entry
  }
}

void List::add(const List &other)
{
List temp(other);

if (length != temp.length)
  {
   mexPrintf("error in List.add:  list sequences are of different length\n");
   mexPrintf("  in this case, self list has length %d\n",length);
   mexPrintf("                other list has length %d\n",temp.length);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

reset();
temp.reset();

while(notdone)
  {
   cur_entry->value += temp.read();
   read();  //increments cur_entry
  }
}

void List::mul(double in)
{
reset();
while(notdone)
  {
   cur_entry->value *= in;
   read();  //increments cur_entry
  }
}

void List::mul(const List &other)
{
List temp(other);

if (length != temp.length)
  {
   mexPrintf("error in List.mul:  list sequences are of different length\n");
   mexPrintf("  in this case, self list has length %d\n",length);
   mexPrintf("                other list has length %d\n",temp.length);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

reset();
temp.reset();

while(notdone)
  {
   cur_entry->value *= temp.read();
   read();  //increments cur_entry
  }
}

void List::reset()
{
cur_entry = first_entry;
notdone = 1;
}

double List::read()
{
if (cur_entry == NULL)
  {
    mexPrintf("error in List.read:  cur_entry is NULL\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
out = cur_entry->value;
if (cur_entry->next == NULL)
  {
   cur_entry = first_entry;
   notdone = 0;
  }
else
  {
   cur_entry = cur_entry->next;
   notdone = 1;
  }
return(out);
}

///////////////// New additions to List:  6/5/04 by Mike Perrott //////

double List::read_without_incrementing()
{
if (cur_entry == NULL)
  {
    mexPrintf("error in List.read_without_incrementing:  cur_entry is NULL\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
out = cur_entry->value;

return(out);
}

void List::write(double in)
{
if (cur_entry == NULL)
  {
    mexPrintf("error in List.write:  cur_entry is NULL\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
cur_entry->value = in;
if (cur_entry->next == NULL)
  {
   cur_entry = first_entry;
   notdone = 0;
  }
else
  {
   cur_entry = cur_entry->next;
   notdone = 1;
  }

out = in;
}

void List::write_without_incrementing(double in)
{
if (cur_entry == NULL)
  {
    mexPrintf("error in List.write_without_incrementing:  cur_entry is NULL\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
cur_entry->value = in;

out = in;
}

void List::remove_first_entry()
{
DOUBLE_LIST *temp;


if (cur_write_entry == NULL)
  {
   mexPrintf("error in List.remove_first_entry:  cur_write_entry is NULL\n");
   mexPrintf(" odds are, you are trying to write to a passed list\n");
   mexPrintf(" which is not permitted\n");
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

if (length > 1)
  {
   temp = first_entry;
   first_entry = first_entry->next;
   if (first_entry == NULL)
     {
      mexPrintf("error in List.remove_first_entry:  first_entry->next is NULL\n");
      mexErrMsgTxt("Exiting CppSim Mex object...");
     }
   if (cur_entry == temp)
     {
      cur_entry = first_entry;
      if (cur_entry->next == NULL)
	 notdone = 0;
      else
	 notdone = 1;
     }
   if (cur_write_entry == temp)
     {
      cur_write_entry = first_entry;
     }
   free(temp);
  }
if (length > 0)
   length--;
}

//////////////////////////////////////////////////////////////////////

DOUBLE_LIST *List::create_entry()
{
DOUBLE_LIST *A;

if ((A = (DOUBLE_LIST *) malloc(sizeof(DOUBLE_LIST))) == NULL)
     {
      mexPrintf("error in 'create_entry':  malloc call failed\n");
      mexPrintf("  out of memory!\n");
      mexErrMsgTxt("Exiting CppSim Mex object...");
     }

A->next = NULL;
return(A);
}

void List::delete_entries(DOUBLE_LIST *first_entry)
{

if (first_entry != NULL)
  {
   if (first_entry->next != NULL)
      delete_entries(first_entry->next);
   free(first_entry);
  }
}


Clist::Clist()
{
length = 0;
notdone = 0;
outr = NaN;
outi = NaN;
}

Clist::Clist(const Clist &other)
{
real.set(other.real);
imag.set(other.imag);
length = other.length;
notdone = other.notdone;
outr = other.outr;
outi = other.outi;
}


Clist::~Clist()
{
}

void Clist::cat(const Clist &other)
{
real.cat(other.real);
imag.cat(other.imag);
length = real.length;
notdone = real.notdone;
}

void Clist::read()
{
outr = real.read();
outi = imag.read();
length = real.length;
notdone = real.notdone;
}
void Clist::reset()
{
real.reset();
imag.reset();
length = real.length;
notdone = real.notdone;
}
void Clist::flush()
{
real.flush();
imag.flush();
length = real.length;
notdone = real.notdone;
}
int Clist::inp(double rin, double iin)
{
real.inp(rin);
imag.inp(iin);
length = real.length;
notdone = real.notdone;
return(length);
}
int Clist::inp(const List &rin, const List &iin)
{
if (rin.length != iin.length)
  {
   mexPrintf("error in Clist.inp:  length mismatch in input lists\n");
   mexPrintf("   list rin has length %d, while list iin has length %d\n",
          rin.length, iin.length);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
real.copy(rin);
imag.copy(iin);
length = real.length;
notdone = real.notdone;
return(length);
}

void Clist::add(const Clist &other)
{
if (real.length != other.real.length)
  {
   mexPrintf("error in Clist.add:  length mismatch in Clists\n");
   mexPrintf("   self Clist has length %d, while other Clist has length %d\n",
          real.length, other.real.length);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
else if (imag.length != other.imag.length)
  {
   mexPrintf("error in Clist.add:  length mismatch in Clists\n");
   mexPrintf("   self Clist has length %d, while other Clist has length %d\n",
          imag.length, other.imag.length);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
real.add(other.real);
imag.add(other.imag);
}

void Clist::add(double rin, double iin)
{
real.add(rin);
imag.add(iin);
}
void Clist::mul(const Clist &other)
{
List tempr,tempi;

if (real.length != imag.length)
  {
   mexPrintf("error in Clist.mul:  length mismatch in Clists\n");
   mexPrintf("   self Clist has real length %d, and imag length %d\n",
          real.length, imag.length);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
else if (other.real.length != other.imag.length)
  {
   mexPrintf("error in Clist.mul:  length mismatch in Clists\n");
   mexPrintf("   other Clist has real length %d, and imag length %d\n",
          other.real.length, other.imag.length);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
else if (real.length != other.real.length)
  {
   mexPrintf("error in Clist.mul:  length mismatch in Clists\n");
   mexPrintf("   self Clist has length %d, while other Clist has length %d\n",
          real.length, other.real.length);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
else if (imag.length != other.imag.length)
  {
   mexPrintf("error in Clist.mul:  length mismatch in Clists\n");
   mexPrintf("   self Clist has length %d, while other Clist has length %d\n",
          imag.length, other.imag.length);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
tempr.copy(real);
tempi.copy(imag);

tempr.mul(other.imag);
tempi.mul(other.imag);
tempi.mul(-1.0);

imag.mul(other.real);
real.mul(other.real);

real.add(tempi);
imag.add(tempr);
}
void Clist::mul(double rin, double iin)
{
List tempr,tempi;

if (real.length != imag.length)
  {
   mexPrintf("error in Clist.mul:  length mismatch in Clist\n");
   mexPrintf("   self Clist has real length %d, and imag length %d\n",
          real.length, imag.length);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
tempr.copy(real);
tempi.copy(imag);

tempr.mul(iin);
tempi.mul(iin);
tempi.mul(-1.0);

imag.mul(rin);
real.mul(rin);

real.add(tempi);
imag.add(tempr);
}

void Clist::conv(const Clist &other)
{
List tempr,tempi;

if (real.length != imag.length)
  {
   mexPrintf("error in Clist.conv:  length mismatch in Clists\n");
   mexPrintf("   self Clist has real length %d, and imag length %d\n",
          real.length, imag.length);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
else if (other.real.length != other.imag.length)
  {
   mexPrintf("error in Clist.conv:  length mismatch in Clists\n");
   mexPrintf("   other Clist has real length %d, and imag length %d\n",
          other.real.length, other.imag.length);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

tempr.copy(real);
tempi.copy(imag);

tempr.conv(other.imag);
tempi.conv(other.imag);
tempi.mul(-1.0);

imag.conv(other.real);
real.conv(other.real);

real.add(tempi);
imag.add(tempr);

length = real.length;
notdone = real.notdone;
}

void Clist::conv(const List &other)
{
if (real.length != imag.length)
  {
   mexPrintf("error in Clist.conv:  length mismatch in Clists\n");
   mexPrintf("   self Clist has real length %d, and imag length %d\n",
          real.length, imag.length);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

real.conv(other);
imag.conv(other);
length = real.length;
notdone = real.notdone;
}

void Clist::copy(const Clist &other)
{
real.copy(other.real);
imag.copy(other.imag);
length = real.length;
notdone = real.notdone;
}

void Clist::print(const char *name)
{
int i;
real.reset();
imag.reset();

if (real.length != imag.length)
  {
   mexPrintf("error in Clist.print:  real.length != imag.length\n");
   mexPrintf("   in this case, real.length = %d, imag.length = %d\n",
     real.length, imag.length);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
for (i = 0; i < real.length; i++)
   mexPrintf("%s[%d]: real = %5.3e, imag = %5.3e\n",name,i+1,
              real.read(),imag.read());
mexPrintf("\n");
}

void Clist::fft(const List &data)
{
vector_struct *DATA,*REAL,*IMAG;
DATA = init_vector_struct("Clist::fft","DATA");
REAL = init_vector_struct("Clist::fft","REAL");
IMAG = init_vector_struct("Clist::fft","IMAG");

list_to_vector(data,DATA);
real_fft_vec(DATA,REAL,IMAG);
real.inp(REAL);
imag.inp(IMAG);

length = real.length;
notdone = real.notdone;
free_vector_struct(DATA);
free_vector_struct(REAL);
free_vector_struct(IMAG);
}

void Clist::fft(const List &data_real, const List &data_imag)
{
vector_struct *DATA_REAL,*DATA_IMAG,*FFT_REAL,*FFT_IMAG;

DATA_REAL = init_vector_struct("Clist::fft","DATA_REAL");
DATA_IMAG = init_vector_struct("Clist::fft","DATA_IMAG");
FFT_REAL = init_vector_struct("Clist::fft","FFT_REAL");
FFT_IMAG = init_vector_struct("Clist::fft","FFT_IMAG");

list_to_vector(data_real,DATA_REAL);
list_to_vector(data_imag,DATA_IMAG);
fft_vec(DATA_REAL,DATA_IMAG,FFT_REAL,FFT_IMAG);
real.inp(FFT_REAL);
imag.inp(FFT_IMAG);

length = real.length;
notdone = real.notdone;

free_vector_struct(DATA_REAL);
free_vector_struct(DATA_IMAG);
free_vector_struct(FFT_REAL);
free_vector_struct(FFT_IMAG);
}

void Clist::fft(const Clist &data)
{
vector_struct *DATA_REAL,*DATA_IMAG,*FFT_REAL,*FFT_IMAG;

DATA_REAL = init_vector_struct("Clist::fft","DATA_REAL");
DATA_IMAG = init_vector_struct("Clist::fft","DATA_IMAG");
FFT_REAL = init_vector_struct("Clist::fft","FFT_REAL");
FFT_IMAG = init_vector_struct("Clist::fft","FFT_IMAG");

list_to_vector(data.real,DATA_REAL);
list_to_vector(data.imag,DATA_IMAG);
fft_vec(DATA_REAL,DATA_IMAG,FFT_REAL,FFT_IMAG);
real.inp(FFT_REAL);
imag.inp(FFT_IMAG);

length = real.length;
notdone = real.notdone;

free_vector_struct(DATA_REAL);
free_vector_struct(DATA_IMAG);
free_vector_struct(FFT_REAL);
free_vector_struct(FFT_IMAG);
}

void Clist::ifft(const List &fft_real, const List &fft_imag)
{
vector_struct *DATA_REAL,*DATA_IMAG,*FFT_REAL,*FFT_IMAG;

DATA_REAL = init_vector_struct("Clist::fft","DATA_REAL");
DATA_IMAG = init_vector_struct("Clist::fft","DATA_IMAG");
FFT_REAL = init_vector_struct("Clist::fft","FFT_REAL");
FFT_IMAG = init_vector_struct("Clist::fft","FFT_IMAG");

list_to_vector(fft_real,FFT_REAL);
list_to_vector(fft_imag,FFT_IMAG);
ifft_vec(FFT_REAL,FFT_IMAG,DATA_REAL,DATA_IMAG);

real.inp(DATA_REAL);
imag.inp(DATA_IMAG);

length = real.length;
notdone = real.notdone;

free_vector_struct(DATA_REAL);
free_vector_struct(DATA_IMAG);
free_vector_struct(FFT_REAL);
free_vector_struct(FFT_IMAG);
}

void Clist::ifft(const Clist &fft_in)
{
vector_struct *DATA_REAL,*DATA_IMAG,*FFT_REAL,*FFT_IMAG;

DATA_REAL = init_vector_struct("Clist::fft","DATA_REAL");
DATA_IMAG = init_vector_struct("Clist::fft","DATA_IMAG");
FFT_REAL = init_vector_struct("Clist::fft","FFT_REAL");
FFT_IMAG = init_vector_struct("Clist::fft","FFT_IMAG");

list_to_vector(fft_in.real,FFT_REAL);
list_to_vector(fft_in.imag,FFT_IMAG);
ifft_vec(FFT_REAL,FFT_IMAG,DATA_REAL,DATA_IMAG);

real.inp(DATA_REAL);
imag.inp(DATA_IMAG);

length = real.length;
notdone = real.notdone;

free_vector_struct(DATA_REAL);
free_vector_struct(DATA_IMAG);
free_vector_struct(FFT_REAL);
free_vector_struct(FFT_IMAG);
}

Poly::Poly(const char *expr_in, const char *vars, double var0, double var1, double var2,
           double var3, double var4, double var5, double var6, double var7,
           double var8, double var9)
{
num_coeff = 0;
max = NaN;
min = -NaN;
sample_period = NaN;
set(expr_in,vars,var0,var1,var2,var3,var4,var5,var6,var7,var8,var9);
}

Poly::Poly(const List &other)
{
num_coeff = 0;
max = NaN;
min = -NaN;
sample_period = NaN;
set(other);
}

Poly::Poly(const Vector &other)
{
num_coeff = 0;
max = NaN;
min = -NaN;
sample_period = NaN;
set(other);
}

void Poly::set(const List &other)
{
int i,j;
List temp(other);
double max, cur_val;

if (temp.length < 1)
  {
   mexPrintf("error in Poly.set (using List):  list length is zero\n");
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

if (num_coeff != 0)
   {
     free(coeff);
     free(exp);
   }

ivar = 'z';
/// determine close-to-zero taps and set "zero" threshold ///
max = 0.0;
temp.reset();
while(temp.notdone)
   {
    cur_val = fabs(temp.read());
    max = cur_val > max ? cur_val : max;
   }
if (max < 1e-20)
   {
     mexPrintf("Error in Poly.set (using List):  all elements within\n");
     mexPrintf("  the input list appear to be zero!\n");
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
max *= 1.0e-12;

/// determine number of taps
num_coeff = 0;
temp.reset();
while (temp.notdone)
   {
    if (fabs(temp.read()) > max)
       num_coeff++;
   }

/// allocate memory for taps

if ((coeff = (double *) calloc(num_coeff,sizeof(double))) == NULL)
   {
    mexPrintf("Error in Poly.set (using List): out of memory for coeff calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if ((exp = (double *) calloc(num_coeff,sizeof(double))) == NULL)
   {
    mexPrintf("Error in Poly.set (using List): out of memory for exponent calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

i = 0;
j = 0;
//mexPrintf("num_coeff = %d\n",num_coeff);
temp.reset();
while (temp.notdone)
   {
    cur_val = temp.read();
    if (fabs(cur_val) > max)
      {
       coeff[j] = cur_val;
       exp[j] = -((double) i);
       //       mexPrintf("coeff = %5.3e, exp = %d\n",coeff[j],(int) exp[j]);
       j++;
      }
    i++;
   }
}


void Poly::set(const Vector &other)
{
int i,j,length;
Vector temp(other);
double max, cur_val;

length = temp.get_length();
if (length < 1)
  {
   mexPrintf("error in Poly.set (using Vector):  vector length is zero\n");
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (num_coeff != 0)
   {
     free(coeff);
     free(exp);
   }

ivar = 'z';
/// determine close-to-zero taps and set "zero" threshold ///
max = 0.0;
for (i = 0; i < length; i++)
   {
    cur_val = fabs(temp.get_elem(i));
    max = cur_val > max ? cur_val : max;
   }
if (max < 1e-20)
   {
     mexPrintf("Error in Poly.set (using Vector):  all elements within\n");
     mexPrintf("  the input vector appear to be zero!\n");
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
max *= 1.0e-12;

/// determine number of taps
num_coeff = 0;
for (i = 0; i < length; i++)
   {
    if (fabs(temp.get_elem(i)) > max)
       num_coeff++;
   }

/// allocate memory for taps

if ((coeff = (double *) calloc(num_coeff,sizeof(double))) == NULL)
   {
    mexPrintf("Error in Poly.set (using Vector): out of memory for coeff calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if ((exp = (double *) calloc(num_coeff,sizeof(double))) == NULL)
   {
    mexPrintf("Error in Poly.set (using Vector): out of memory for exponent calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

j = 0;
//mexPrintf("num_coeff = %d\n",num_coeff);
for (i = 0; i < length; i++)
   {
    cur_val = temp.get_elem(i);
    if (fabs(cur_val) > max)
      {
       coeff[j] = cur_val;
       exp[j] = -((double) i);
       //       mexPrintf("coeff = %5.3e, exp = %d\n",coeff[j],(int) exp[j]);
       j++;
      }
   }
}

void Poly::set(const char *expr_in, const char *vars, double var0, double var1, 
                 double var2, double var3, double var4, double var5,
                 double var6, double var7, double var8, double var9)
{
PARAM *par_list;
char expr[MAX_CHAR_LENGTH],p2[MAX_CHAR_LENGTH+1],expr2[MAX_CHAR_LENGTH];
char vars2[MAX_CHAR_LENGTH];
char *p;
int i,j,k=0,count,flag,act_count;
double sign;

if (num_coeff != 0)
   {
     free(coeff);
     free(exp);
   }

for (i = 0; i < MAX_CHAR_LENGTH; i++)
   if (expr_in[i] == '\0')
       break;
if (expr_in[i] != '\0')
  {
   mexPrintf("error in Poly.set:  input expression is too long\n");
   mexPrintf("  string is limited to MAX_CHAR_LENGTH = %d\n",MAX_CHAR_LENGTH);
   mexPrintf("   must increase MAX_CHAR_LENGTH in source code\n");
   mexPrintf("   if you want to use the following input expression:\n");
   mexPrintf("   expr_in = %s\n",expr_in);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

par_list = init_param();
strcpy(par_list->overall_expr,expr_in); // aids debugging in eval_paren_expr
if (vars != NULL)
  {
    strncpy(vars2,vars,MAX_CHAR_LENGTH-1);
   p = strtok(vars2," ,;");
  }
else
  p = NULL;

count = 0;
while(p != NULL)
   {
    if (count >= LENGTH_PAR_LIST || count >= 10)
      {
	mexPrintf("error in Poly construct:  number of parameters exceeded\n");
	mexPrintf("  (it's limited to 10 parameters");
	mexPrintf("  arg string: %s\n",vars);
	mexPrintf("  associated expression: %s\n",expr_in);
	mexErrMsgTxt("Exiting CppSim Mex object...");
      }
    strncpy(par_list->main_par_list[count],p,LENGTH_PAR-1);
    count++;
    p = strtok(NULL," ,;");
   }
par_list->main_par_length = count;
for (i = 0; i < count; i++)
   {
     if (i == 0)
       {
       if (var0 == NaN)
	 {
          mexPrintf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       par_list->main_par_val[i] = var0;
       }
     else if (i == 1)
       {
       if (var1 == NaN)
	 {
          mexPrintf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       par_list->main_par_val[i] = var1;
       }
     else if (i == 2)
       {
       if (var2 == NaN)
	 {
          mexPrintf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       par_list->main_par_val[i] = var2;
       }
     else if (i == 3)
       {
       if (var3 == NaN)
	 {
          mexPrintf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       par_list->main_par_val[i] = var3;
       }
     else if (i == 4)
       {
       if (var4 == NaN)
	 {
          mexPrintf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       par_list->main_par_val[i] = var4;
       }
     else if (i == 5)
       {
       if (var5 == NaN)
	 {
          mexPrintf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       par_list->main_par_val[i] = var5;
       }
     else if (i == 6)
       {
       if (var6 == NaN)
	 {
          mexPrintf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       par_list->main_par_val[i] = var6;
       }
     else if (i == 7)
       {
       if (var7 == NaN)
	 {
          mexPrintf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       par_list->main_par_val[i] = var7;
       }
     else if (i == 8)
       {
       if (var8 == NaN)
	 {
          mexPrintf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       par_list->main_par_val[i] = var8;
       }
     else if (i == 9)
       {
       if (var9 == NaN)
	 {
          mexPrintf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       par_list->main_par_val[i] = var9;
       }
   }

if (count >= LENGTH_PAR_LIST)
    {
     mexPrintf("error in Poly construct:  number of parameters exceeded\n");
     mexErrMsgTxt("Exiting CppSim Mex object...");
    }
strncpy(par_list->main_par_list[count],"pi",LENGTH_PAR-1);
par_list->main_par_val[count++] = PI;
par_list->main_par_length = count;

/*
 for (i = 0; i < count; i++)
   mexPrintf("par = '%s', %5.3f\n",par_list->main_par_list[i],par_list->main_par_val[i]);
*/
// mexPrintf("expr_in = %s\n",expr_in);


ivar = '!'; // nonsense for later check
count = 0;
for (i = 0; expr_in[i] != '\0'; i++)
   {
     if ((expr_in[i] >= 'A' && expr_in[i] <= 'Z') || (expr_in[i] >= 'a'
	 && expr_in[i] <= 'z'))
       {
	 if (i > 0)
	   {
           if ((expr_in[i-1] >= 'A' && expr_in[i-1] <= 'Z') || 
               (expr_in[i-1] >= 'a' && expr_in[i-1] <= 'z'))
	      continue;
	   }
       count = 0;
       for (j = 0; j < par_list->main_par_length; j++)
         {
	   for (k = 0; par_list->main_par_list[j][k] != '\0' &&
                       expr_in[i+k] != '\0'; k++)
	     {
  	      if (expr_in[i+k] != par_list->main_par_list[j][k])
		{
		  count++;
		  break;
		}
	     }
	   if (par_list->main_par_list[j][k] == '\0')
	     {
             if (expr_in[i+k] != ' ' && expr_in[i+k] != '+' && 
                 expr_in[i+k] != '-' && expr_in[i+k] != '^' && 
                 expr_in[i+k] != '/' && expr_in[i+k] != '*' &&
                 expr_in[i+k] != '(' && expr_in[i+k] != ')' &&
                 expr_in[i+k] != '\0')
 	       count++;
	     }
	   else if (expr_in[i+k] == '\0')
	     {
	     if (par_list->main_par_list[j][k] != '\0')
	       count++;
	     }
         }
       if (count == par_list->main_par_length)
	 {
	   if (expr_in[i+1] == '^' || expr_in[i+1] == ' ' ||
               expr_in[i+1] == '+' || expr_in[i+1] == '-' ||
               expr_in[i+1] == '/' || expr_in[i+1] == '*' ||
               expr_in[i+1] == '(' || expr_in[i+1] == ')' ||
               expr_in[i+1] == '\0')
	     {
	      ivar = expr_in[i];
	      break;
	     }
	 }
       }
   }
//mexPrintf("ivar = %c\n",ivar);
/*
 if (ivar == '!')
   {
     mexPrintf("error in Poly construct:  couldn't determine the independent variable\n");
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
*/

count = 0;
for (i = 0,j = 0; expr_in[i] != '\0'; i++)
  {
   if (j >= (MAX_CHAR_LENGTH-1))
      {
	mexPrintf("error in Poly construct:  max char length exceeded\n");
        mexPrintf("  to fix, change MAX_CHAR_LENGTH in source code\n");
	mexErrMsgTxt("Exiting CppSim Mex object...");
      }
   if (expr_in[i] == '(')
      count++;
   else if (expr_in[i] == ')')
      count--;
   if (expr_in[i] == ivar && (expr_in[i+1] == '^' || expr_in[i+1] == ' ' ||
                              expr_in[i+1] == '+' || expr_in[i+1] == '-' ||
			      expr_in[i+1] == '/' || expr_in[i+1] == '*' ||
                              expr_in[i+1] == '(' || expr_in[i+1] == ')' || 
			      expr_in[i+1] == '\0'))
     expr2[j++] = '!'; // substitute '!' for ivar
   else
     expr2[j++] = expr_in[i];
   if (expr_in[i] == '-')
     {
     if (i == 0)
       expr2[j-1] = '@';
     else
       {
       if (expr_in[i-1] == '^')
         expr2[j-1] = '@';
       else if (count == 0)
         expr2[j++] = '@';
       }
     }
  }
expr2[j] = '\0';
//mexPrintf("expr2 = %s\n",expr2);

count = 0;
for (i = 0; expr2[i] != '\0'; i++)
   {
     if (expr2[i] == '(')
       count++;
     else if (expr2[i] == ')')
       count--;
     else if (expr2[i] == '+' || expr2[i] == '-')
       {
	 if (count == 0)
	   expr2[i] = '#';
       }    
   }
// mexPrintf("expr2 = %s\n",expr2);

strcpy(expr,expr2);
p = strtok(expr,"#");
count = 0;
while(p != NULL)
   {
    count++;
    p = strtok(NULL,"#");
   }
strcpy(expr,expr2);

if ((coeff = (double *) calloc(count,sizeof(double))) == NULL)
  {
    mexPrintf("Error in Poly construct: out of memory for coeff calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if ((exp = (double *) calloc(count,sizeof(double))) == NULL)
  {
    mexPrintf("Error in Poly construct: out of memory for exponent calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }

p = strtok(expr,"#");
count = 0;
while(p != NULL)
   {
   for (i = 0; p[i] != '\0'; i++)
     if (p[i] == '@')
       p[i] = '-';
   flag = 0;
   p2[0] = ' ';
   for (i = 0, j = 1; p[i] != '\0'; i++)
     {
       p2[j++] = p[i];
       if (p[i] == '!') // look for ivar ('!' was substituted for ivar)
	 {
	   if (i > 0)
	      for (k = i-1; p[k] == ' '; k--);
	   if (p[k] == '(')
	     {
	       mexPrintf("error in Poly construct:  bad polynomial expression\n");
	       mexPrintf("   expression must be of form a0*x + a1*x^2 ...\n");
	       mexPrintf("   NOT   .... x*a0 .... or .... x/a0 ....\n");
	       mexPrintf("    or   .... a0*(x) .... or .... (x/a1)^2 ....\n");
	       mexPrintf("   overall expression in question: %s\n",expr_in);
	       mexErrMsgTxt("Exiting CppSim Mex object...");
	     }
	   for (k = i+1; p[k] == ' '; k++);
	   if (p[k] == '/' || p[k] == '*' || p[k] == '(' || p[k] == ')')
	     {
	       mexPrintf("error in Poly construct:  bad polynomial expression\n");
	       mexPrintf("   expression must be of form a0*x + a1*x^2 ...\n");
	       mexPrintf("   NOT   .... x*a0 .... or .... x/a0 ....\n");
	       mexPrintf("    or   .... a0*(x) .... or .... (x/a1)^2 ....\n");
	       mexPrintf("   overall expression in question: %s\n",expr_in);
	       mexErrMsgTxt("Exiting CppSim Mex object...");
	     }
           flag = 1;
	   if (p[i+1] != '^')
	     {
	       p2[j++] = '1';
	     }
           else
	      p[i+1] = ' ';
	 }
     }
   if (flag == 0)
     {
       p2[j++] = '!'; // '!' represents ivar
      p2[j++] = '0';
     }
   p2[j] = '\0';

   for (i = 0; p2[i] != '!'; i++);
   p2[i] = '\0';
   for (j = i; p2[j] == ' ' || p2[j] == '\0'; j--)
     if (j == 0)
       break;
   sign = 1.0;
   if (p2[j] == '*')
     p2[j] = ' ';
   else if (p2[j] == '/')
     {
       mexPrintf("error in Poly construct:  cannot use '/' before independent\n");
       mexPrintf("  variable in polynomial expression\n");
       mexPrintf("     offending portion:  %ss^%s\n",p2,&p2[i+1]);
       mexPrintf("     overall expr: %s\n",expr_in);
       mexErrMsgTxt("Exiting CppSim Mex object...");
     }
   else if (p2[j] == '-')
     {
       p2[j] = '1';
       sign = -1.0;
     }
   else if (j == 0 && (p2[j] == ' ' || p2[j] == '\0'))
     p2[j] = '1';

   coeff[count] = sign*eval_paren_expr(p2,par_list);
   exp[count++] = eval_paren_expr(&p2[i+1],par_list);
   p = strtok(NULL,"#");
   }


act_count = count;
 for (i = 0; i < count; i++)
   {
   if (exp[i] == 1e9)
      continue;
   for (j = i+1; j < count; j++)
     {
       if (exp[i] == exp[j])
	 {
           act_count--;
	   coeff[i] += coeff[j];
	   exp[j] = 1e9; // unrealistically high value to serve as marker
	 }
     }
   }

for (i = 0,j = 0; i < count; i++)
  {
    if (exp[i] != 1e9)
      {
       coeff[j] = coeff[i];
       exp[j++] = exp[i];
      }
  }


// set max, min, and sample_period
for (i = 0; i < par_list->main_par_length; i++)
  {
   if (strcmp(par_list->main_par_list[i],"Max") == 0)
       max = par_list->main_par_val[i];
   else if (strcmp(par_list->main_par_list[i],"Min") == 0)
       min = par_list->main_par_val[i];
   else if (strcmp(par_list->main_par_list[i],"Ts") == 0)
       sample_period = par_list->main_par_val[i];
  }
if (max <= min)
  {
   mexPrintf("error in Poly constructor:  max is set <= min\n");
   mexPrintf("  in this case, max = %5.3e, min = %5.3e\n",max,min);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (sample_period <= 0.0)
  {
   mexPrintf("error in Poly constructor:  sample_period is set <= 0.0\n");
   mexPrintf("  in this case, Ts = %5.3e\n",sample_period);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

/*
for (i = 0; i < act_count; i++)
   mexPrintf("coeff = %5.3f, exponent = %5.3f\n",coeff[i],exp[i]);
*/
// mexPrintf("max = %5.3e, min = %5.3e\n",max,min);

num_coeff = act_count;
 free(par_list);
}

Poly::~Poly()
{
  free(coeff);
  free(exp);
}



Delay::Delay()
{
nom_delay = 0.0;
out = -1.0;
input_state = -1;
output_state = -1;
warning_flag = 0;
prev_in = -1.0;
}

void Delay::set_nom_delay(double nom_value)
{
nom_delay = nom_value;
}


Delay::Delay(double nom_value)
{
nom_delay = nom_value;
out = -1.0;
input_state = -1;
output_state = -1;
warning_flag = 0;
prev_in = -1.0;
}


Delay::~Delay()
{
}


double Delay::inp(double in, double delta_delay)
{
double cur_delay_val, buf_val;
int transition_flag, edge_flag;

if ((in < -1.0 || in > 1.0) && warning_flag == 0)
    {
     warning_flag = 1;
     mexPrintf("warning in 'delay.inp':  nonvalid input!\n");
     mexPrintf("     in must -1.0, 1.0, or some value inbetween\n");
     mexPrintf("     in this case, in = %5.3e\n",in);
     mexPrintf("  **** in short, input is not conforming to the double_interp protocol ****\n");
    }

cur_delay_val = nom_delay + delta_delay;

if (cur_delay_val < 0.0)
   {
     mexPrintf("error in 'delay.inp':  delay value must be >= 0.0!\n");
     mexPrintf("  in this case, delay_val = %5.3e time samples\n",cur_delay_val);
     mexPrintf("  --> nom_delay = %5.3e time samples, delta_delay = %5.3e time samples\n",
            nom_delay,delta_delay);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (input_state == -1) // look for rising input edge
   {
    if (in != -1.0)
      {
       if (prev_in == -1.0)
           edges.inp(in + 1.0 - cur_delay_val*2.0 - 2.0);
       else
           edges.inp(10.0);
       input_state = 1;
      }
   }
else if (input_state == 1) // look for falling input edge
   {
    if (in != 1.0)
      {
       if (prev_in == 1.0)
          edges.inp(in - 1.0 + cur_delay_val*2.0 + 2.0);
       else
          edges.inp(-10.0);
       input_state = -1;
      }
   }

prev_in = in;

// don't allow more than 200 edges in buffer, otherwise
// simulation is very slow and the user may think the program
// has crashed
if (edges.length > 200) 
   {
    mexPrintf("error in 'delay.inp':  limit of 200 rising/falling edges has been exceeded!\n");
           mexPrintf("     in edge buffer\n");
           mexPrintf("     -> this is due to having a delay that is too long\n");
           mexPrintf("        relative to the edge frequency of the signal coming in\n");
           mexPrintf("  **** you should reduce the delay time requested of this delay cell ****\n");
           mexPrintf("       nominal_delay = %5.3e time samples, delta_delay = %5.3e time samples\n",
		  nom_delay, delta_delay);
           mexPrintf("       overall delay = %5.3e time samples\n",cur_delay_val);
           mexErrMsgTxt("Exiting CppSim Mex object...");
   }
transition_flag = 0;

if (edges.length != 0)
   {
    edges.reset();

    edge_flag = output_state;
    while (edges.notdone)
       {
        buf_val = edges.read_without_incrementing(); 
        if (edge_flag == -1)  // edge is rising
           edges.write(buf_val + 2.0);
        else // edge is falling
           edges.write(buf_val - 2.0);
        edge_flag = edge_flag == -1 ? 1 : -1;
       }

    edges.reset();
    buf_val = edges.read_without_incrementing(); 

    if (output_state == -1) // rising edge
       {
       if (buf_val > 0.0)
          {
           transition_flag = 1;
           output_state = 1;
           if (buf_val < 2.0)
              out = buf_val - 1.0;
           else
	      out = 1.0;
           edges.remove_first_entry();
          }
       }
    else // falling edge
       {
       if (buf_val < 0.0)
          {
           transition_flag = 1;
           output_state = -1;
           if (buf_val > -2.0)
              out = buf_val + 1.0;
           else
	      out = -1.0;
           edges.remove_first_entry();
          }
       }
   }

if (transition_flag == 0)
   {
    if (output_state == -1)
       out = -1.0;
    else
       out = 1.0;
   }

return(out);
}

double Delay::inp(double in)
{
return(inp(in,0.0));
}

SampleAndHold::SampleAndHold()
{
prev_in = -1.0;
prev_clk = -1.0;
}

SampleAndHold::~SampleAndHold()
{
}

double SampleAndHold::inp(double in, double clk)
{
if (clk < -1.0 || clk > 1.0)
  {
   mexPrintf("error in SampleAndHold.inp:   clk is < -1.0 or > 1.0\n");
   mexPrintf("  in this case, clk = %5.3f\n",clk);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

if (prev_clk == -1.0 && clk != -1.0)
   out = (clk + 1.0)/2.0*prev_in + (-clk + 1.0)/2.0*in;

prev_in = in;
return(out);
}

EdgeDetect::EdgeDetect()
{
prev_in = -1.0;
prev_slope = 2; 
out_of_range_flag = 0;
}

EdgeDetect::~EdgeDetect()
{
}

int EdgeDetect::inp(double in)
{

if ((in < -1.0 || in > 1.0) && out_of_range_flag == 0)
  {
   out_of_range_flag = 1;
   mexPrintf("Warning in EdgeDetect.inp:   in is < -1.0 or > 1.0\n");
   mexPrintf("  in this case, in = %5.3f\n",in);
  }

if (((in-prev_in) > 0.0) && prev_slope == 0)
   {
    prev_slope = 1;
    out = 1;
   } 
else if ((in-prev_in) <= 0.0)
   {
    prev_slope = 0;
    out = 0; 
   }
else
    out = 0;

prev_in = in;
return(out);
}

int EdgeDetect::inp_interp(double in)
{

if ((in < -1.0 || in > 1.0) && out_of_range_flag == 0)
  {
   out_of_range_flag = 1;
   mexPrintf("Warning in EdgeDetect.inp:   in is < -1.0 or > 1.0\n");
   mexPrintf("  in this case, in = %5.3f\n",in);
  }

if (((in-prev_in) > 0.0) && prev_slope == 0)
   {
    prev_slope = 1;
    out = 0;
   } 
else if (prev_slope == 1)
   {
    prev_slope = 2;
    out = 1;
   }
else if ((in-prev_in) <= 0.0)
   {
    prev_slope = 0;
    out = 0; 
   }
else
    out = 0;

prev_in = in;
return(out);
}

int EdgeDetect::inp(int in)
{
return(inp((double) in));
}

EdgeInterpolate::EdgeInterpolate()
{
prev_in = 0.0;
clk_in = -1.0;
out_of_range_flag = 0;
prev_in_array = NULL;
vec_length = 0;
}

EdgeInterpolate::~EdgeInterpolate()
{
if (prev_in_array != NULL)
   free(prev_in_array);
}

double EdgeInterpolate::get_prev_sample()
{
out = prev_in;
return(out);
}

Vector EdgeInterpolate::get_prev_sample_vec()
{
int i;

if (prev_in_array == NULL)
    {
      mexPrintf("Error in 'EdgeInterpolate::get_prev_sample_vec()':\n");
      mexPrintf("   previous vector value is not defined!\n");
      mexPrintf("   -> be sure to call 'EdgeInterpolate::update()' first\n");
      mexPrintf("      before calling this function\n");
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
if (vec_out.get_length() != vec_length)
   vec_out.set_length(vec_length);

for (i = 0; i < vec_length; i++)
   {
   vec_out.set_elem(i,prev_in_array[i]);
   }

return(vec_out);
}

double EdgeInterpolate::inp(double in)
{

if ((clk_in < -1.0 || clk_in > 1.0) && out_of_range_flag == 0)
  {
   out_of_range_flag = 1;
   mexPrintf("Warning in EdgeInterpolate.inp:   clk_in is < -1.0 or > 1.0\n");
   mexPrintf("  in this case, clk_in = %5.3f\n",clk_in);
  }
out = (prev_in*(clk_in+1.0) + in*(-clk_in+1.0))/2.0;

return(out);
}

double EdgeInterpolate::inp(int in_int)
{
double in;

in = ((double) in_int);

if ((clk_in < -1.0 || clk_in > 1.0) && out_of_range_flag == 0)
  {
   out_of_range_flag = 1;
   mexPrintf("Warning in EdgeInterpolate.inp:   clk_in is < -1.0 or > 1.0\n");
   mexPrintf("  in this case, clk_in = %5.3f\n",clk_in);
  }
out = (prev_in*(clk_in+1.0) + in*(-clk_in+1.0))/2.0;

return(out);
}

Vector EdgeInterpolate::inp(Vector vec_in)
{
int i;

if (prev_in_array == NULL)
    {
    vec_length = vec_in.get_length();
    if ((prev_in_array = (double *) calloc(vec_length,sizeof(double))) == NULL)
       { 
        mexPrintf("Error in 'EdgeInterpolate::inp': out of memory for calloc\n");
        mexErrMsgTxt("Exiting CppSim Mex object...");
       }
    }
if (vec_out.get_length() != vec_length)
   vec_out.set_length(vec_length);

if ((clk_in < -1.0 || clk_in > 1.0) && out_of_range_flag == 0)
  {
   out_of_range_flag = 1;
   mexPrintf("Warning in EdgeInterpolate.inp:   clk_in is < -1.0 or > 1.0\n");
   mexPrintf("  in this case, clk_in = %5.3f\n",clk_in);
  }

for (i = 0; i < vec_length; i++)
   {
   vec_out.set_elem(i,(prev_in_array[i]*(clk_in+1.0) + 
                       vec_in.get_elem(i)*(-clk_in+1.0))/2.0);
   }

return(vec_out);
}

Vector EdgeInterpolate::inp(IntVector vec_in)
{
int i;

if (prev_in_array == NULL)
    {
    vec_length = vec_in.get_length();
    if ((prev_in_array = (double *) calloc(vec_length,sizeof(double))) == NULL)
       { 
        mexPrintf("Error in 'EdgeInterpolate::inp': out of memory for calloc\n");
        mexErrMsgTxt("Exiting CppSim Mex object...");
       }
    }
if (vec_out.get_length() != vec_length)
   vec_out.set_length(vec_length);

if ((clk_in < -1.0 || clk_in > 1.0) && out_of_range_flag == 0)
  {
   out_of_range_flag = 1;
   mexPrintf("Warning in EdgeInterpolate.inp:   clk_in is < -1.0 or > 1.0\n");
   mexPrintf("  in this case, clk_in = %5.3f\n",clk_in);
  }

for (i = 0; i < vec_length; i++)
   {
   vec_out.set_elem(i,(prev_in_array[i]*(clk_in+1.0) + 
                       ((double) vec_in.get_elem(i))*(-clk_in+1.0))/2.0);
   }

return(vec_out);
}



void EdgeInterpolate::update(double clk, double in)
{
prev_in = in;
clk_in = clk;
}

void EdgeInterpolate::update(double clk, int in_int)
{
double in;

in = ((double) in_int);

prev_in = in;
clk_in = clk;
}

void EdgeInterpolate::update(double clk, Vector in_vec)
{
int i;

if (prev_in_array == NULL)
    {
    vec_length = in_vec.get_length();
    if ((prev_in_array = (double *) calloc(vec_length,sizeof(double))) == NULL)
       { 
        mexPrintf("Error in 'EdgeInterpolate::update': out of memory for calloc\n");
        mexErrMsgTxt("Exiting CppSim Mex object...");
       }
    }

for (i = 0; i < vec_length; i++)
     prev_in_array[i] = in_vec.get_elem(i);

clk_in = clk;
}

void EdgeInterpolate::update(double clk, IntVector in_vec)
{
int i;

if (prev_in_array == NULL)
    {
    vec_length = in_vec.get_length();
    if ((prev_in_array = (double *) calloc(vec_length,sizeof(double))) == NULL)
       { 
        mexPrintf("Error in 'EdgeInterpolate::update': out of memory for calloc\n");
        mexErrMsgTxt("Exiting CppSim Mex object...");
       }
    }

for (i = 0; i < vec_length; i++)
     prev_in_array[i] = (double) in_vec.get_elem(i);

clk_in = clk;
}


EdgeMeasure::EdgeMeasure()
{
prev_in = -1.0;
time_val = 0.0;
out = 0.0;
out_of_range_flag = 0;
}


EdgeMeasure::~EdgeMeasure()
{
}

double EdgeMeasure::inp(double in)
{
if ((in < -1.0 || in > 1.0) && out_of_range_flag == 0)
  {
   out_of_range_flag = 1;
   mexPrintf("Warning in EdgeMeasure.inp:   in is < -1.0 or > 1.0\n");
   mexPrintf("  in this case, in = %5.3f\n",in);
  }

// rising edge triggered
if (prev_in == -1.0 && in != -1.0)
  {
   out = time_val + 1.0 - (in - prev_in)/2.0;
   time_val = (in - prev_in)/2.0;
  }
else
  {
   time_val += 1.0;
   out = 0.0;
  }

prev_in = in;
return(out);
}




Quantizer::Quantizer(int levels, double step_size, double in_center, 
                     double out_min, double out_max)
{
if (step_size < 1e-30)
  {
    mexPrintf("error in 'Quantizer': step_size is less than 1e-30\n");
    mexPrintf("   in this case, step_size = %5.3e\n",step_size);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
istep_size = step_size;
if (levels < 2)
  {
    mexPrintf("error in 'Quantizer': levels must be >= 2\n");
    mexPrintf("  in this case, levels = %d\n",levels);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
ilevels = levels;
if (levels % 2 == 1)
   odd_levels_flag = 1;
else
   odd_levels_flag = 0;

if (out_max <= out_min)
   {
     mexPrintf("error in 'Quantizer':  out_max must be > out_min\n");
     mexPrintf("  in this case, out_max = %5.3e, out_min = %5.3e\n",
            out_max, out_min);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
iout_max = out_max;
iout_min = out_min;
out_center = (iout_max+iout_min)/2.0;
out_scale = (iout_max-iout_min)/((double) (ilevels-1));
iin_center = in_center;

prev_clk = -1.0;
clk_out_of_range_flag = 0;
out = 0.0;
}

Quantizer::~Quantizer()
{
}

double Quantizer::inp(double in)
{
if (odd_levels_flag == 1)
   out = out_scale*floor((in-iin_center)/istep_size+0.5) + out_center;
else
   out = out_scale*(floor((in-iin_center)/istep_size)+0.5) + out_center;
if (out > iout_max)
   out = iout_max;
if (out < iout_min)
   out = iout_min;
return(out);
}

double Quantizer::inp(double in, double clk)
{
if ((clk < -1.0 || clk > 1.0) && clk_out_of_range_flag == 0)
  {
   clk_out_of_range_flag = 1;
   mexPrintf("Warning in Quantizer.inp:   clk is < -1.0 or > 1.0\n");
   mexPrintf("  in this case, clk = %5.3f\n",clk);
  }
if (prev_clk == -1.0 && clk != -1.0)
  {
   if (odd_levels_flag == 1)
      out = out_scale*floor((in-iin_center)/istep_size+0.5) + out_center;
   else
      out = out_scale*(floor((in-iin_center)/istep_size)+0.5) + out_center;
   if (out > iout_max)
      out = iout_max;
   if (out < iout_min)
      out = iout_min;
  }
prev_clk = clk;
return(out);
}


Probe::Probe(const char *filename, double sample_per, int subsample)
{
if ((fp = fopen(filename,"wb")) == NULL)
   {
    mexPrintf("Error in Probe Construct:  file '%s' can't be opened\n",filename);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if (subsample < 1)
   {
    mexPrintf("Error in Probe Construct:  subsample must be > 0\n");
    mexPrintf("  in this case, subsample = %d\n",subsample);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
sample_period = sample_per; 
subsample_count = subsample;
cur_sample = 0;
buf_length = 0;
first_node = NULL;
nodes_with_time = NULL;
header_flag = 0;
num_signals = 0;
cur_signal = 0;
nodes_with_time = add_node(nodes_with_time,"TIME",0.0);
}


Probe::~Probe()
{
// flush buffer to output file
create_data(nodes_with_time,fp,1,fbuf,&buf_length);
fclose(fp);
}

void Probe::inp(double node_value, const char *node_name, int sim_step)
{

if (header_flag == 0)
  {
   if (first_node == NULL)
     {
      if ((cur_sample % subsample_count) == 0)
	{
	 if (sim_step < 0)
            nodes_with_time->value = ((double) cur_sample)*sample_period;
	 else
            nodes_with_time->value = ((double) sim_step)*sample_period;
	}

      first_node = add_node(first_node,node_name,node_value);
      nodes_with_time->next = first_node;
      num_signals = 1;
     }
   else
     {
     if (strcmp(node_name,first_node->name) == 0)
       {
        if (fp == NULL)
	 {
	   mexPrintf("error in Probe.inp - filename has not been specified!\n");
	   mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       create_header(nodes_with_time,fp);
       create_data(nodes_with_time,fp,0,fbuf,&buf_length);
       header_flag = 1;
       cur_signal = 0;
       cur_node = first_node;
       cur_sample = 1;
       }
     else
       {
       add_node(first_node,node_name,node_value);
       num_signals++;
       }
     }
  }
if (header_flag == 1)
  {
   cur_signal++;
   if (strcmp(node_name,cur_node->name) != 0)
     {
       mexPrintf("error in Probe.inp - the signal probed does not match\n");
       mexPrintf("   with the signal that should be specified\n");
       mexPrintf("      the node that should be probed is: %s\n", cur_node->name);
       mexPrintf("      the node that is being probed is: %s\n", node_name);
       mexPrintf("   this error may be caused by using the same probe object\n");
       mexPrintf("   in two different loops\n");
       mexErrMsgTxt("Exiting CppSim Mex object...");
     }
   cur_node->value = node_value;
   if (cur_signal == num_signals)
     {
      if ((cur_sample % subsample_count) == 0)
	{
	 if (sim_step < 0)
            nodes_with_time->value = ((double) cur_sample)*sample_period;
	 else
            nodes_with_time->value = ((double) sim_step)*sample_period;
         create_data(nodes_with_time,fp,0,fbuf,&buf_length);
	}
      cur_signal = 0;
      cur_node = first_node;
      cur_sample++;
     }
   else
     {
      if (cur_node->next == NULL)
	 {
          mexPrintf("error:  this should never happen\n");
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
      cur_node = cur_node->next;
     }
  }
}

void Probe::inp(double node_value, const char *node_name)
{
inp(node_value,node_name,-1);
}

void Probe::inp(int int_node_value, const char *node_name)
{
double node_value;

node_value = (double) ((signed long int) int_node_value);
inp(node_value,node_name,-1);
}



void Probe::inp(const Vector &vec_node_value, const char *node_name)
{
int i;
char vec_node_name[300];
vector_struct *x;

x = extract_vector_struct(vec_node_value);

for (i = 0; i < x->length; i++)
  {
   sprintf(vec_node_name,"%s_%d",node_name,i);
   inp(x->elem[i],vec_node_name,-1);
  }
}

void Probe::inp(const IntVector &vec_node_value, const char *node_name)
{
int i;
char vec_node_name[300];
double node_value;
int_vector_struct *x;

x = extract_vector_struct(vec_node_value);

for (i = 0; i < x->length; i++)
  {
   sprintf(vec_node_name,"%s_%d",node_name,i);
   node_value = (double) ((signed long int) x->elem[i]);
   inp(node_value,vec_node_name,-1);
  }
}


void Probe::inp(const Vector &vec_node_value, const char *node_name, int sim_step)
{
int i;
char vec_node_name[300];
vector_struct *x;

x = extract_vector_struct(vec_node_value);

for (i = 0; i < x->length; i++)
  {
   sprintf(vec_node_name,"%s_%d",node_name,i);
   inp(x->elem[i],vec_node_name,sim_step);
  }
}

void Probe::inp(const IntVector &vec_node_value, const char *node_name, int sim_step)
{
int i;
char vec_node_name[300];
double node_value;
int_vector_struct *x;

x = extract_vector_struct(vec_node_value);

for (i = 0; i < x->length; i++)
  {
   sprintf(vec_node_name,"%s_%d",node_name,i);
   node_value = (double) ((signed long int) x->elem[i]);
   inp(node_value,vec_node_name,sim_step);
  }
}


void Probe::inp(int int_node_value, const char *node_name, int sim_step)
{
double node_value;

node_value = (double) ((signed long int) int_node_value);
inp(node_value,node_name,sim_step);
}


//////////////////////////////////////////////////////////////////

Probe64::Probe64(const char *filename, double sample_per, int subsample)
{
if ((fp = fopen(filename,"wb")) == NULL)
   {
    mexPrintf("Error in Probe64 Construct:  file '%s' can't be opened\n",filename);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if (subsample < 1)
   {
    mexPrintf("Error in Probe64 Construct:  subsample must be > 0\n");
    mexPrintf("  in this case, subsample = %d\n",subsample);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
sample_period = sample_per; 
subsample_count = subsample;
cur_sample = 0;
buf_length = 0;
first_node = NULL;
nodes_with_time = NULL;
header_flag = 0;
num_signals = 0;
cur_signal = 0;
nodes_with_time = add_node(nodes_with_time,"TIME",0.0);
}


Probe64::~Probe64()
{
// flush buffer to output file
create_data64(nodes_with_time,fp,1,dbuf,&buf_length);
fclose(fp);
}

void Probe64::inp(double node_value, const char *node_name, int sim_step)
{

if (header_flag == 0)
  {
   if (first_node == NULL)
     {
      if ((cur_sample % subsample_count) == 0)
	{
	 if (sim_step < 0)
            nodes_with_time->value = ((double) cur_sample)*sample_period;
	 else
            nodes_with_time->value = ((double) sim_step)*sample_period;
	}

      first_node = add_node(first_node,node_name,node_value);
      nodes_with_time->next = first_node;
      num_signals = 1;
     }
   else
     {
     if (strcmp(node_name,first_node->name) == 0)
       {
        if (fp == NULL)
	 {
	   mexPrintf("error in Probe64.inp - filename has not been specified!\n");
	   mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       create_header64(nodes_with_time,fp);
       create_data64(nodes_with_time,fp,0,dbuf,&buf_length);
       header_flag = 1;
       cur_signal = 0;
       cur_node = first_node;
       cur_sample = 1;
       }
     else
       {
       add_node(first_node,node_name,node_value);
       num_signals++;
       }
     }
  }
if (header_flag == 1)
  {
   cur_signal++;
   if (strcmp(node_name,cur_node->name) != 0)
     {
       mexPrintf("error in Probe64.inp - the signal probed does not match\n");
       mexPrintf("   with the signal that should be specified\n");
       mexPrintf("      the node that should be probed is: %s\n", cur_node->name);
       mexPrintf("      the node that is being probed is: %s\n", node_name);
       mexPrintf("   this error may be caused by using the same probe object\n");
       mexPrintf("   in two different loops\n");
       mexErrMsgTxt("Exiting CppSim Mex object...");
     }
   cur_node->value = node_value;
   if (cur_signal == num_signals)
     {
      if ((cur_sample % subsample_count) == 0)
	{
	 if (sim_step < 0)
            nodes_with_time->value = ((double) cur_sample)*sample_period;
	 else
            nodes_with_time->value = ((double) sim_step)*sample_period;
         create_data64(nodes_with_time,fp,0,dbuf,&buf_length);
	}
      cur_signal = 0;
      cur_node = first_node;
      cur_sample++;
     }
   else
     {
      if (cur_node->next == NULL)
	 {
          mexPrintf("error:  this should never happen\n");
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
      cur_node = cur_node->next;
     }
  }
}

void Probe64::inp(double node_value, const char *node_name)
{
inp(node_value,node_name,-1);
}

void Probe64::inp(int int_node_value, const char *node_name)
{
double node_value;

node_value = (double) ((signed long int) int_node_value);
inp(node_value,node_name,-1);
}



void Probe64::inp(const Vector &vec_node_value, const char *node_name)
{
int i;
char vec_node_name[300];
vector_struct *x;

x = extract_vector_struct(vec_node_value);

for (i = 0; i < x->length; i++)
  {
   sprintf(vec_node_name,"%s_%d",node_name,i);
   inp(x->elem[i],vec_node_name,-1);
  }
}

void Probe64::inp(const IntVector &vec_node_value, const char *node_name)
{
int i;
char vec_node_name[300];
double node_value;
int_vector_struct *x;

x = extract_vector_struct(vec_node_value);

for (i = 0; i < x->length; i++)
  {
   sprintf(vec_node_name,"%s_%d",node_name,i);
   node_value = (double) ((signed long int) x->elem[i]);
   inp(node_value,vec_node_name,-1);
  }
}


void Probe64::inp(const Vector &vec_node_value, const char *node_name, int sim_step)
{
int i;
char vec_node_name[300];
vector_struct *x;

x = extract_vector_struct(vec_node_value);

for (i = 0; i < x->length; i++)
  {
   sprintf(vec_node_name,"%s_%d",node_name,i);
   inp(x->elem[i],vec_node_name,sim_step);
  }
}

void Probe64::inp(const IntVector &vec_node_value, const char *node_name, int sim_step)
{
int i;
char vec_node_name[300];
double node_value;
int_vector_struct *x;

x = extract_vector_struct(vec_node_value);

for (i = 0; i < x->length; i++)
  {
   sprintf(vec_node_name,"%s_%d",node_name,i);
   node_value = (double) ((signed long int) x->elem[i]);
   inp(node_value,vec_node_name,sim_step);
  }
}


void Probe64::inp(int int_node_value, const char *node_name, int sim_step)
{
double node_value;

node_value = (double) ((signed long int) int_node_value);
inp(node_value,node_name,sim_step);
}


//////////////////////////////////////////////////////////////////

Divider::Divider()
{
out = 0.0;
cycle_count = 0;
state = 0;
prev_in = 0.0;
high_count = 1;
low_count = 1;
prev_divide_val = 0;
divide_trans_count = -1;
trans_warning_flag = 0;
divide_val_flag = 0;
}

double Divider::inp(double in, double divide_value)
{
if (fabs(floor(divide_value)-divide_value) > 1e-2)
  {
    mexPrintf("error in Divider.inp:  divide value is far from integer\n");
    mexPrintf(" in this case, divide value = %5.3f\n",divide_value);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
return(inp_base(in,(int) floor(divide_value)));
}

double Divider::inp(double in, int divide_value)
{
return(inp_base(in,divide_value));
}

double Divider::inp_base(double in,int divide_value)
{
if (prev_in == -1.0 && in != -1.0)
   cycle_count++;

if (divide_value != prev_divide_val)
   divide_trans_count++;
prev_divide_val = divide_value;

if (divide_trans_count > 1 && trans_warning_flag == 0)
  {
   mexPrintf("warning: in Divider.inp  divide value transitioned more than once\n");
   mexPrintf("  during one cycle of divider output\n");
   trans_warning_flag = 1;
  }

if (state == 0) // low state
   {
    if (cycle_count == low_count)
      {
      if (divide_value < 2)
        {
	 if (divide_val_flag == 0)
	    {
	     divide_val_flag = 1;
             mexPrintf("Warning in Divider.inp:  divide_value must be > 1\n");
             mexPrintf("  in this case, divide_value = %d\n",divide_value);
	     mexPrintf("  setting divide value to 2 whenever it drops below 2\n");
	    }
	 divide_value = 2;
        }
      high_count = divide_value/2;
      low_count = divide_value-high_count;
      divide_trans_count = 0;

      cycle_count = 0;
      state = 1;
      out = in;
      }
    else
      out = -1.0;
   }
else
   {
    if (cycle_count == high_count)
      {
      cycle_count = 0;
      state = 0;
      out = -in;
      }
    else
      out = 1.0;
   }
prev_in = in;
return(out);
}

Divider::~Divider()
{
} 


Amp::Amp(const char *poly_expr, const char *vars, double var0, double var1, 
       double var2, double var3, double var4, double var5, 
       double var6, double var7, double var8, double var9)
  : poly_gain(poly_expr,vars,var0,var1,var2,var3,var4,var5,var6,var7,
              var8,var9)
{
}

Amp::~Amp()
{
}

void Amp::set(const char *poly_expr, const char *vars, 
       double var0, double var1, 
       double var2, double var3, double var4, double var5, 
       double var6, double var7, double var8, double var9)
{
poly_gain.set(poly_expr,vars,var0,var1,var2,var3,var4,var5,var6,var7,var8,
   var9);
}

double Amp::inp(double in)
{
int i,num_coeff;
num_coeff = poly_gain.num_coeff;

out = 0.0;
for (i = 0; i < num_coeff; i++)
   out += poly_gain.coeff[i]*pow(in,poly_gain.exp[i]);
if (out > poly_gain.max)
   out = poly_gain.max;
else if (out < poly_gain.min)
   out = poly_gain.min;
return(out);
}


Vco::Vco(const char *poly_expr, const char *vars, 
       double var0, double var1, 
       double var2, double var3, double var4, double var5, 
       double var6, double var7, double var8, double var9)
  : Kv_poly(poly_expr,vars,var0,var1,var2,var3,var4,var5,var6,var7,
            var8,var9)
{
out = 0.0;
phase = 0.0;
prev_phase = 0.0;
clk_state = 1;
divide_scale = -1.0;
sample_period = Kv_poly.poly_gain.sample_period;
prev_divide_val = 0;
divide_trans_count = -1;
trans_warning_flag = 0;
divide_val_warning_flag = 0;
out_of_range_flag = 0;

if (sample_period == NaN)
  {
   mexPrintf("Error in Vco Constructor:  sample_period has not been set\n");
   mexPrintf("   in variable list, you must specify 'Ts'\n");
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
else if (sample_period < 1e-30)
{
  mexPrintf("error in Vco Constructor:  sample_period can't be < 1e-30\n");
  mexPrintf("  in this case, sample_period = %5.3e\n",sample_period);
  mexErrMsgTxt("Exiting CppSim Mex object...");
}
}

void Vco::set(const char *poly_expr, const char *vars, 
       double var0, double var1, 
       double var2, double var3, double var4, double var5, 
       double var6, double var7, double var8, double var9)
{
Kv_poly.set(poly_expr,vars,var0,var1,var2,var3,var4,var5,var6,var7,
            var8,var9);
sample_period = Kv_poly.poly_gain.sample_period;

if (sample_period == NaN)
  {
   mexPrintf("Error in Vco Constructor:  sample_period has not been set\n");
   mexPrintf("   in variable list, you must specify 'Ts'\n");
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
else if (sample_period < 1e-30)
  {
  mexPrintf("error in Vco Constructor:  sample_period can't be < 1e-30\n");
  mexPrintf("  in this case, sample_period = %5.3e\n",sample_period);
  mexErrMsgTxt("Exiting CppSim Mex object...");
  }
}


void Vco::set_phase(double in)
{
if (in < 0)
    {
      while(in < 0)
	in += 1.0;
    }
  
else if (in >= 1.0)
    {
      while(in >= 1.0)
	in -= 1.0;
    }

if (in >= 0 && in < 0.5)
    { 
      clk_state = 1;
      out=1;
    }
else
    {
      clk_state = 0;
      out=-1; 
    }

phase = in*2*PI;
prev_phase = in*2*PI;
}

double Vco::inp(double in)
{

phase = prev_phase + sample_period*2.0*PI*Kv_poly.inp(in);
if (phase >= 2*PI)
   phase -= 2*PI;

//  state = 0:  clk phase between PI and 2*PI
//  state = 1:  clk phase between 0 and PI


if (prev_phase >= phase)
     prev_phase -= 2*PI;

if (phase >= 0.0 && phase < PI) /* state = 1 */
  {
    if (clk_state == 1) /* stay in state 1 */
       out = 1.0;
    else  /* transition from state=0 to state=1 */
       out = (phase+prev_phase)/(phase-prev_phase);
    clk_state = 1;
  }
else /* state = 0 */
  {
    if (clk_state == 0) /* stay in state 0 */
       out = -1.0;
    else /* transition from state=1 to state=0 */
       out = (2*PI-(phase+prev_phase))/(phase-prev_phase);
    clk_state = 0;
  }
prev_phase = phase;
if ((out < -1.0 || out > 1.0) && out_of_range_flag == 0)
  {
   out_of_range_flag = 1;
   mexPrintf("Warning in Vco.inp:  interpolated output has value beyond -1 to 1 range\n");
   mexPrintf("  in this case, out = %5.3f\n",out);
   mexPrintf("  probable cause:  input inappropriate\n");
   mexPrintf("  in this case, in = %5.3f\n",in);
  }
return(out);
}

double Vco::inp(double in, int divide_val)
{
return(inp_div(in, divide_val));
}

double Vco::inp(double in, double divide_val)
{
if (fabs(floor(divide_val)-divide_val) > 1e-2)
  {
    mexPrintf("error in Vco.inp:  divide value is far from integer\n");
    mexPrintf(" in this case, divide value = %5.3f\n",divide_val);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
return(inp_div(in,(int) floor(divide_val)));
}

double Vco::inp_div(double in, int divide_val)
{

if (divide_val != prev_divide_val)
   divide_trans_count++;
prev_divide_val = divide_val;
if (divide_trans_count > 1 && trans_warning_flag == 0)
  {
   trans_warning_flag = 1;
   mexPrintf("warning in Vco.inp:  divide value transitioned more than once\n");
   mexPrintf("  during one cycle of Vco output\n");
  }

if (divide_scale < 1.0)
   divide_scale = (double) divide_val;

if (sample_period*Kv_poly.inp(in) > divide_scale/2.0)
  {
   if (divide_val_warning_flag == 0)
     {
     divide_val_warning_flag = 1;
     mexPrintf("Warning in Vco.inp:  divide_val is too small for the given sample rate!\n");
     mexPrintf("  in this case, divide_val = '%d', and should be >= '%d'\n",
          divide_val,(int) (4*sample_period*Kv_poly.inp(in)));
     mexPrintf("  -> setting divide val to '%d' whenever it is too small\n",
             (int) (4*sample_period*Kv_poly.inp(in)));
     }
   divide_val = (int) (4*sample_period*Kv_poly.inp(in));
  }

phase = prev_phase + sample_period*2.0*PI*Kv_poly.inp(in);
if (phase >= 2*PI*divide_scale)
  {
   phase -= 2*PI*divide_scale;
   if (prev_phase >= phase)
       prev_phase -= 2*PI*divide_scale;
   divide_scale = (double) divide_val;
   divide_trans_count = 0;
  }

//  state = 0:  clk phase between PI*divide and 2*PI*divide
//  state = 1:  clk phase between 0 and PI*divide

if (phase >= 0.0 && phase < PI*divide_scale) /* state = 1 */

  {
    if (clk_state == 1) /* stay in state 1 */
       out = 1.0;
    else  /* transition from state=0 to state=1 */
       out = (phase+prev_phase)/(phase-prev_phase);
    clk_state = 1;
  }
else /* state = 0 */
  {
    if (clk_state == 0) /* stay in state 0 */
       out = -1.0;
    else  /* transition from state=1 to state=0 */
       out = (2*PI*divide_scale-(phase+prev_phase))/(phase-prev_phase);
    clk_state = 0;
  }
prev_phase = phase;
 if ((out < -1.0 || out > 1.0) && out_of_range_flag == 0)
  {
   out_of_range_flag = 1;
   mexPrintf("Warning in Vco.inp:  interpolated output has value beyond -1 to 1 range\n");
   mexPrintf("  in this case, out = %5.3f\n",out);
   mexPrintf("  probable cause:  input or divide value inappropriate\n");
   mexPrintf("  in this case, in = %5.3f, divide value = %d\n",in,divide_val);
   mexPrintf("  also, make sure divide value is not changing more than once\n");
   mexPrintf("  per VCO cycle\n");
  }
return(out);
}


Vco::~Vco()
{
}

void Filter::reset(double value)
{
int i;
double den, num, inv_gain;

out = value;

// figure out DC gain
den = 1.0;
for (i = 0; i < num_a_coeff; i++)
   den -= a_coeff[i];
num = 0.0;
for (i = 0; i < num_b_coeff; i++)
   num += b_coeff[i];

// reset state information and output
if (fabs(num) > 1e-24)
  {
   inv_gain = den/num;
   for (i = num_x_samples-1; i > 0; i--)
      x_samples[i] = inv_gain*out;
   x_samples[0] = inv_gain*out;
  }
else
  {
   for (i = num_x_samples-1; i > 0; i--)
      x_samples[i] = x_samples[0];
  }

for (i = num_y_samples-1; i > 0; i--)
  y_samples[i] = out;

y_samples[0] = out;
}


Filter::Filter()
{
num_a_coeff = 0;
num_b_coeff = 0;
num_x_samples = 0;
num_y_samples = 0;
out = 0.0;
}

Filter::Filter(const char *num, const char *den, const char *vars, double var0, double var1, 
         double var2, double var3, double var4, double var5, double var6, 
         double var7, double var8, double var9)
{
num_a_coeff = 0;
num_b_coeff = 0;
num_x_samples = 0;
num_y_samples = 0;
out = 0.0;

Poly num_poly(num,vars,var0,var1,var2,var3,var4,var5,var6,var7,var8,var9);
Poly den_poly(den,vars,var0,var1,var2,var3,var4,var5,var6,var7,var8,var9);

set_base(&num_poly,&den_poly,num,den);
}

Filter::Filter(const List &num_list, const char *den, const char *vars, double var0, 
         double var1, double var2, double var3, double var4, double var5,
         double var6, double var7, double var8, double var9)
{
num_a_coeff = 0;
num_b_coeff = 0;
num_x_samples = 0;
num_y_samples = 0;
out = 0.0;

Poly num_poly(num_list);
Poly den_poly(den,vars,var0,var1,var2,var3,var4,var5,var6,var7,var8,var9);

set_base(&num_poly,&den_poly,"set by List",den);
}

Filter::Filter(const char *num, const List &den_list, const char *vars, double var0, 
         double var1, double var2, double var3, double var4, double var5, 
         double var6, double var7, double var8, double var9)
{
num_a_coeff = 0;
num_b_coeff = 0;
num_x_samples = 0;
num_y_samples = 0;
out = 0.0;

Poly num_poly(num,vars,var0,var1,var2,var3,var4,var5,var6,var7,var8,var9);
Poly den_poly(den_list);

set_base(&num_poly,&den_poly,num,"set by List");
}

Filter::Filter(const List &num_list, const List &den_list)
{
num_a_coeff = 0;
num_b_coeff = 0;
num_x_samples = 0;
num_y_samples = 0;
out = 0.0;

Poly num_poly(num_list);
Poly den_poly(den_list);

set_base(&num_poly,&den_poly,"set by List","set by List");
}


Filter::Filter(const Vector &num_list, const char *den, const char *vars, double var0, 
         double var1, double var2, double var3, double var4, double var5,
         double var6, double var7, double var8, double var9)
{
num_a_coeff = 0;
num_b_coeff = 0;
num_x_samples = 0;
num_y_samples = 0;
out = 0.0;

Poly num_poly(num_list);
Poly den_poly(den,vars,var0,var1,var2,var3,var4,var5,var6,var7,var8,var9);

set_base(&num_poly,&den_poly,"set by Vector",den);
}

Filter::Filter(const char *num, const Vector &den_list, const char *vars, double var0, 
         double var1, double var2, double var3, double var4, double var5, 
         double var6, double var7, double var8, double var9)
{
num_a_coeff = 0;
num_b_coeff = 0;
num_x_samples = 0;
num_y_samples = 0;
out = 0.0;

Poly num_poly(num,vars,var0,var1,var2,var3,var4,var5,var6,var7,var8,var9);
Poly den_poly(den_list);

set_base(&num_poly,&den_poly,num,"set by Vector");
}

Filter::Filter(const Vector &num_list, const Vector &den_list)
{
num_a_coeff = 0;
num_b_coeff = 0;
num_x_samples = 0;
num_y_samples = 0;
out = 0.0;

Poly num_poly(num_list);
Poly den_poly(den_list);

set_base(&num_poly,&den_poly,"set by Vector","set by Vector");
}

double Filter::inp_old(double in)
{
int i;

out = 0.0;
for (i = 0; i < num_b_coeff; i++)
  {
  if (b_exp[i] == 0)
    out += b_coeff[i]*in;
  else
    out += b_coeff[i]*x_samples[b_exp[i]-1];
  }
for (i = 0; i < num_a_coeff; i++) // a0 coeffient already removed
  out += a_coeff[i]*y_samples[a_exp[i]-1];

// update sample memory
for (i = num_x_samples-1; i > 0; i--)
  x_samples[i] = x_samples[i-1];
x_samples[0] = in;
for (i = num_y_samples-1; i > 0; i--)
  y_samples[i] = y_samples[i-1];

if (out > max_out)
    out = max_out;
if (out < min_out)
    out = min_out;

y_samples[0] = out;
return(out);
}


double Filter::inp(double in)
{
int i, cur_index;


out = 0.0;

for (i = 0; i < num_b_coeff; i++)
  {
  if (b_exp[i] == 0)
    out += b_coeff[i]*in;
  else
    {
    cur_index = x_samples_pointer + b_exp[i] - 1;
    if (cur_index >= num_x_samples)
       cur_index -= num_x_samples;
    out += b_coeff[i]*x_samples[cur_index];
    }
  }

for (i = 0; i < num_a_coeff; i++) // a0 coeffient already removed
  {
  cur_index = y_samples_pointer + a_exp[i] - 1;
  if (cur_index >= num_y_samples)
     cur_index -= num_y_samples;
  out += a_coeff[i]*y_samples[cur_index];
  }

// update sample memory
if (num_x_samples > 0)
  {
   x_samples_pointer--;
   if (x_samples_pointer < 0)
      x_samples_pointer = num_x_samples-1;
   x_samples[x_samples_pointer] = in;
  }
if (num_y_samples > 0)
  {
   y_samples_pointer--;
   if (y_samples_pointer < 0)
      y_samples_pointer = num_y_samples-1;
   y_samples[y_samples_pointer] = out;
  }


if (out > max_out)
    reset(max_out);
if (out < min_out)
    reset(min_out);

return(out);
}

void Filter::set(const char *num, const char *den, const char *vars, double var0, double var1, 
         double var2, double var3, double var4, double var5, double var6, 
         double var7, double var8, double var9)
{
Poly num_poly(num,vars,var0,var1,var2,var3,var4,var5,var6,var7,var8,var9);
Poly den_poly(den,vars,var0,var1,var2,var3,var4,var5,var6,var7,var8,var9);

set_base(&num_poly,&den_poly,num,den);
}

void Filter::set(const List &num_list, const char *den, const char *vars, double var0, 
         double var1, 
         double var2, double var3, double var4, double var5, double var6, 
         double var7, double var8, double var9)
{
Poly num_poly(num_list);
Poly den_poly(den,vars,var0,var1,var2,var3,var4,var5,var6,var7,var8,var9);

set_base(&num_poly,&den_poly,"set by List",den);
}

void Filter::set(const char *num, const List &den_list, const char *vars, double var0, 
         double var1, 
         double var2, double var3, double var4, double var5, double var6, 
         double var7, double var8, double var9)
{
Poly num_poly(num,vars,var0,var1,var2,var3,var4,var5,var6,var7,var8,var9);
Poly den_poly(den_list);

set_base(&num_poly,&den_poly,num,"set by List");
}

void Filter::set(const List &num_list, const List &den_list)
{
Poly num_poly(num_list);
Poly den_poly(den_list);

set_base(&num_poly,&den_poly,"set by List","set by List");
}

void Filter::set_base(Poly *num_poly, Poly *den_poly, const char *num, const char *den)
{
x_samples_pointer = 0;
y_samples_pointer = 0;

max_out = den_poly->max;
min_out = den_poly->min;

//mexPrintf("max_out = %5.3e, min_out = %5.3e\n",max_out,min_out);

if (num_poly->ivar == '!' && den_poly->ivar == '!')
  {
   num_poly->ivar = 'z';
   den_poly->ivar = 'z';
  }

if (num_poly->ivar != den_poly->ivar)
  {
   if (num_poly->ivar != '!' && den_poly->ivar != '!')
     {
      mexPrintf("error in Filter construct:  independent variables for\n");
      mexPrintf("numerator and denominator polynomials are different\n");
      mexPrintf("   in this case, ivar for num = %c, for den = %c\n",
	       num_poly->ivar, den_poly->ivar);
      mexPrintf("  for num: %s\n",num);
      mexPrintf("      den: %s\n",den);
      mexErrMsgTxt("Exiting CppSim Mex object...");
     }   
  }

if (num_poly->ivar == 'z' || den_poly->ivar == 'z')
  {
  ivar = 'z';
  set_dt_filt(num_poly,den_poly,num,den);
  }
else if (num_poly->ivar == 's' || den_poly->ivar == 's')
  {
  ivar = 's';
  set_ct_filt(num_poly,den_poly,num,den);
  }
else
  {
    mexPrintf("error:  filter must have 'z' or 's' for ivar\n");
    mexPrintf("  in this case, ivar = %c\n",num_poly->ivar);
    mexPrintf("  for num: %s\n",num);
    mexPrintf("      den: %s\n",den);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }

}

void Filter::set_ct_filt(Poly *num_poly, Poly *den_poly, const char *num, const char *den)
{
int i,j,k,min,max;
double scale,a0_val;
double *num_coeff, *den_coeff;
int *num_exp, *den_exp;
int number_num_coeff, number_den_coeff;
double *temp_coeff;
double sample_period;

/*
for (i = 0; i < num_poly->num_coeff; i++)
   mexPrintf("num_exp = %5.3f, num_coeff = %5.3e\n",
           num_poly->exp[i],num_poly->coeff[i]);
mexPrintf("\n");
for (i = 0; i < den_poly->num_coeff; i++)
   mexPrintf("den_exp = %5.3f, den_coeff = %5.3e\n",
           den_poly->exp[i],den_poly->coeff[i]);
mexPrintf("-----------------------------------------\n");
*/

number_num_coeff = num_poly->num_coeff;
number_den_coeff = den_poly->num_coeff;

if ((num_coeff = (double *) calloc(number_num_coeff,sizeof(double))) == NULL)
  {
    mexPrintf("Error in 'set_ct_filt': out of memory for calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if ((num_exp = (int *) calloc(number_num_coeff,sizeof(int))) == NULL)
  {
    mexPrintf("Error in 'set_ct_filt': out of memory for calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if ((den_coeff = (double *) calloc(number_den_coeff,sizeof(double))) == NULL)
  {
    mexPrintf("Error in 'set_ct_filt': out of memory for calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if ((den_exp = (int *) calloc(number_den_coeff,sizeof(int))) == NULL)
  {
    mexPrintf("Error in 'set_ct_filt': out of memory for calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }

for (i = 0; i < number_num_coeff; i++)
   {
     num_coeff[i] = num_poly->coeff[i];
     if (fabs(floor(num_poly->exp[i])-num_poly->exp[i]) > .01)
       {
	 mexPrintf("error in 'set_ct_filt':  num_poly->exp has noninteger\n");
	 mexPrintf("values\n");
	 mexPrintf("   in this case, num_poly->exp[%d] = %5.3f\n",i,
		num_poly->exp[i]);
         mexPrintf("  for num: %s\n",num);
         mexPrintf("      den: %s\n",den);
	 mexErrMsgTxt("Exiting CppSim Mex object...");
       }
     num_exp[i] = (int) floor(num_poly->exp[i]);
   }
for (i = 0; i < number_den_coeff; i++)
   {
     den_coeff[i] = den_poly->coeff[i];
// mexPrintf("den_coeff[%d] = %5.3e\n",i,den_coeff[i]);
     if (fabs(floor(den_poly->exp[i])-den_poly->exp[i]) > .01)
       {
	 mexPrintf("error in 'set_ct_filt':  den_poly.exp has noninteger\n");
	 mexPrintf("values\n");
	 mexPrintf("   in this case, den_poly->exp[%d] = %5.3f\n",i,
		den_poly->exp[i]);
         mexPrintf("  for num: %s\n",num);
         mexPrintf("      den: %s\n",den);
	 mexErrMsgTxt("Exiting CppSim Mex object...");
       }
     den_exp[i] = (int) floor(den_poly->exp[i]);
   }

min = num_exp[0];
for (i = 1; i < number_num_coeff; i++)
   if (num_exp[i] < min)
      min = num_exp[i];
for (i = 0; i < number_den_coeff; i++)
   if (den_exp[i] < min)
      min = den_exp[i];

if (min < 0)
  {
   for (i = 0; i < number_num_coeff; i++)
      num_exp[i] -= min;
   for (i = 0; i < number_den_coeff; i++)
      den_exp[i] -= min;
  }

max = num_exp[0];
for (i = 1; i < number_num_coeff; i++)
   if (num_exp[i] > max)
      max = num_exp[i];
for (i = 0; i < number_den_coeff; i++)
   if (den_exp[i] > max)
      max = den_exp[i];

if ((temp_coeff = (double *) calloc(max+1,sizeof(double))) == NULL)
  {
    mexPrintf("Error in 'set_ct_filt': out of memory for calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }

if (num_b_coeff != max+1 || num_a_coeff != max)
  {
  if (num_b_coeff != 0)
    {
    free(b_coeff);
    free(b_exp);
    }
  if (num_a_coeff != 0)
    {
    free(a_coeff);
    free(a_exp);
    }
  num_b_coeff = max+1;
  if ((b_coeff = (double *) calloc(num_b_coeff,sizeof(double))) == NULL)
    {
    mexPrintf("Error in 'set_ct_filt': out of memory for calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }
  if ((b_exp = (int *) calloc(num_b_coeff,sizeof(int))) == NULL)
    {
    mexPrintf("Error in 'set_ct_filt': out of memory for calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }
  num_a_coeff = max+1;
  if ((a_coeff = (double *) calloc(num_a_coeff,sizeof(double))) == NULL)
    {
    mexPrintf("Error in 'set_ct_filt': out of memory for calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }
  if ((a_exp = (int *) calloc(num_a_coeff,sizeof(int))) == NULL)
    {
    mexPrintf("Error in 'set_ct_filt': out of memory for calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }
  if (num_y_samples != 0)
     free(y_samples);
  if (num_x_samples != 0)
     free(x_samples);
  num_y_samples = max;
  num_x_samples = max;
  if ((y_samples = (double *) calloc(num_y_samples,sizeof(double))) == NULL)
    {
    mexPrintf("Error in 'set_dt_filt': out of memory for calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }
  for (i = 0; i < num_y_samples; i++)
    y_samples[i] = 0.0;
  if ((x_samples = (double *) calloc(num_x_samples,sizeof(double))) == NULL)
    {
    mexPrintf("Error in 'set_dt_filt': out of memory for calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }
  for (i = 0; i < num_x_samples; i++)
    x_samples[i] = 0.0;
  }
else
  {
   num_a_coeff = max+1;
  }

for (i = 0; i < max+1; i++)
  {
   b_exp[i] = i;
   b_coeff[i] = 0.0;
   a_exp[i] = i;
   a_coeff[i] = 0.0;
  }

sample_period = num_poly->sample_period;
if (sample_period == NaN)
  {
  mexPrintf("error in 'set_ct_fil':  sample_period has not been set\n");
  mexPrintf("  in variable list, you must specify 'Ts'\n");
  mexPrintf("  for num: %s\n",num);
  mexPrintf("      den: %s\n",den);
  mexErrMsgTxt("Exiting CppSim Mex object...");
  }
else if (sample_period < 1e-30)
{
  mexPrintf("error in 'set_ct_fil':  sample_period can't be < 1e-30\n");
  mexPrintf("  in this case, sample_period = %5.3e\n",sample_period);
  mexPrintf("  for num: %s\n",num);
  mexPrintf("      den: %s\n",den);
  mexErrMsgTxt("Exiting CppSim Mex object...");
}

// numerator conversion

for (i = 0; i < number_num_coeff; i++)
   {
     for (j = 0; j < max+1; j++)
        temp_coeff[j] = 0.0;

     scale = num_coeff[i];
     if (num_exp[i] > 0)
       {
        scale *= pow(2.0/sample_period,(double) num_exp[i]);
        temp_coeff[0] = 1.0;
        temp_coeff[1] = -1.0;
	for (j = 1; j < num_exp[i]; j++) // perform mult. of (1 - z^-1)
	  for (k = max; k > 0; k--)
	    temp_coeff[k] -= temp_coeff[k-1];
	if (num_exp[i] < max) // perform first multiplication of (1 + z^-1)
	  for (k = max; k > 0; k--)
	    temp_coeff[k] += temp_coeff[k-1];
       }
     else if (num_exp[i] < max)
       {
        temp_coeff[0] = 1.0;
        temp_coeff[1] = 1.0;
       }
     for (j = num_exp[i]+1; j < max; j++) // perform mult. of (1 + z^-1)
        for (k = max; k > 0; k--)
	   temp_coeff[k] += temp_coeff[k-1];
     for (j = 0; j < (max+1); j++)
       {
       b_coeff[j] += temp_coeff[j]*scale;
       }
   }

// denominator conversion

for (i = 0; i < number_den_coeff; i++)
   {
// mexPrintf("den_exp[%d] = %d, den_coeff[%d] = %5.3e\n",i,den_exp[i],i,den_coeff[i]);
     for (j = 0; j < max+1; j++)
        temp_coeff[j] = 0.0;

     scale = den_coeff[i];
     if (den_exp[i] > 0)
       {
        scale *= pow(2.0/sample_period,(double) den_exp[i]);
        temp_coeff[0] = 1.0;
        temp_coeff[1] = -1.0;
	for (j = 1; j < den_exp[i]; j++) // perform mult. of (1 - z^-1)
	  for (k = max; k > 0; k--)
	    temp_coeff[k] -= temp_coeff[k-1];
	if (den_exp[i] < max) // perform first multiplication of (1 + z^-1)
	  for (k = max; k > 0; k--)
	    temp_coeff[k] += temp_coeff[k-1];
       }
     else if (den_exp[i] < max)
       {
        temp_coeff[0] = 1.0;
        temp_coeff[1] = 1.0;
       }
     for (j = den_exp[i]+1; j < max; j++) // perform mult. of (1 + z^-1)
        for (k = max; k > 0; k--)
	   temp_coeff[k] += temp_coeff[k-1];
     for (j = 0; j < (max+1); j++)
       {
       a_coeff[j] += temp_coeff[j]*scale;
       }
   }

/*
 for (i = 0; i < max+1; i++)
   mexPrintf("orig num[%d] = %5.9e\n",i,b_coeff[i]);
 mexPrintf("\n");
 for (i = 0; i < max+1; i++)
   mexPrintf("orig den[%d] = %5.9e\n",i,a_coeff[i]);
 mexPrintf("-----------------------------------\n");
*/

if (fabs(a_coeff[0]) < 1e-30)
{
  mexPrintf("error in 'set_ct_fil':  a0 is close to zero\n");
  mexPrintf("  in this case, a0 = %5.3e\n",a_coeff[0]);
  mexPrintf("  must have an ill-posed CT filter for DT converstion\n");
  mexPrintf("  for num: %s\n",num);
  mexPrintf("      den: %s\n",den);
  mexErrMsgTxt("Exiting CppSim Mex object...");
}
for (i = num_b_coeff-1; i >= 0; i--)
    b_coeff[i] = b_coeff[i]/a_coeff[0];
num_a_coeff--;
a0_val = a_coeff[0];
for (i = 0; i < num_a_coeff; i++)
  {
    a_exp[i] = a_exp[i+1];
    a_coeff[i] = -a_coeff[i+1]/a0_val;
  }


/*
 for (i = 0; i < num_b_coeff; i++)
   mexPrintf("num[%d] = %5.3f\n",b_exp[i],b_coeff[i]);
 mexPrintf("\n");
 for (i = 0; i < num_a_coeff; i++)
   mexPrintf("den[%d] = %5.3f\n",a_exp[i],a_coeff[i]);
*/

free(num_coeff);
free(den_coeff);
free(num_exp);
free(den_exp);
free(temp_coeff);
}

void Filter::set_dt_filt(Poly *num_poly, Poly *den_poly, const char *num, const char *den)
{
int i=0,min;
double a0_val;
int new_filt_flag;


if (num_b_coeff != 0)
      {
	free(b_coeff);
	free(b_exp);
      }
if (num_a_coeff != 0)
      {
	free(a_coeff);
	free(a_exp);
      }
num_b_coeff = num_poly->num_coeff;
num_a_coeff = den_poly->num_coeff;

if ((b_coeff = (double *) calloc(num_b_coeff,sizeof(double))) == NULL)
      {
       mexPrintf("Error in 'set_dt_filt': out of memory for calloc\n");
       mexErrMsgTxt("Exiting CppSim Mex object...");
      }
if ((b_exp = (int *) calloc(num_b_coeff,sizeof(int))) == NULL)
      {
       mexPrintf("Error in 'set_dt_filt': out of memory for calloc\n");
       mexErrMsgTxt("Exiting CppSim Mex object...");
      }
if ((a_coeff = (double *) calloc(num_a_coeff,sizeof(double))) == NULL)
      {
       mexPrintf("Error in 'set_dt_filt': out of memory for calloc\n");
       mexErrMsgTxt("Exiting CppSim Mex object...");
      }
if ((a_exp = (int *) calloc(num_a_coeff,sizeof(int))) == NULL)
      {
       mexPrintf("Error in 'set_dt_filt': out of memory for calloc\n");
       mexErrMsgTxt("Exiting CppSim Mex object...");
      }

for (i = 0; i < num_b_coeff; i++)
   {
     b_coeff[i] = num_poly->coeff[i];
     if (fabs(floor(num_poly->exp[i])-num_poly->exp[i]) > .01)
       {
	 mexPrintf("error in 'set_dt_filt':  num_poly->exp has noninteger\n");
	 mexPrintf("values\n");
	 mexPrintf("   in this case, num_poly->exp[%d] = %5.3f\n",i,
		num_poly->exp[i]);
         mexPrintf("  for num: %s\n",num);
         mexPrintf("      den: %s\n",den);
	 mexErrMsgTxt("Exiting CppSim Mex object...");
       }
     b_exp[i] = (int) -floor(num_poly->exp[i]);
   }
for (i = 0; i < num_a_coeff; i++)
   {
     a_coeff[i] = den_poly->coeff[i];
     if (fabs(floor(den_poly->exp[i])-den_poly->exp[i]) > .01)
       {
	 mexPrintf("error in 'set_dt_filt':  den_poly.exp has noninteger\n");
	 mexPrintf("values\n");
	 mexPrintf("   in this case, den_poly->exp[%d] = %5.3f\n",i,
		den_poly->exp[i]);
         mexPrintf("  for num: %s\n",num);
         mexPrintf("      den: %s\n",den);
	 mexErrMsgTxt("Exiting CppSim Mex object...");
       }
     a_exp[i] = (int) -floor(den_poly->exp[i]);
   }

min = a_exp[0];
for (i = 0; i < num_a_coeff; i++)
    if (a_exp[i] < min)
      min = a_exp[i];
if (min != 0)
  {
   for (i = 0; i < num_a_coeff; i++)
      a_exp[i] -= min;
   for (i = 0; i < num_b_coeff; i++)
      b_exp[i] -= min;
  }
min = 0;
for (i = 0; i < num_b_coeff; i++)
    if (b_exp[i] < min)
      min = b_exp[i];
if (min != 0)
   for (i = 0; i < num_b_coeff; i++)
      b_exp[i] -= min;

/*
 for (i = 0; i < num_b_coeff; i++)
     mexPrintf("-- b_exp = %d ,b_coeff = %5.3f\n",b_exp[i],b_coeff[i]);
 for (i = 0; i < num_a_coeff; i++)
     mexPrintf("-- a_exp = %d ,a_coeff = %5.3f\n",a_exp[i],a_coeff[i]);
*/

a0_val = 0.0;
for (i = 0; i < num_a_coeff; i++)
  if (a_exp[i] == 0)
    {
      a0_val = a_coeff[i];
      break;
    }
if (fabs(a0_val) < 1e-30)
{
  mexPrintf("error in 'set_dt_fil':  a0_val is close to zero\n");
  mexPrintf("  in this case, a0_val = %5.3e\n",a0_val);
  mexPrintf("  for num: %s\n",num);
  mexPrintf("      den: %s\n",den);
  mexErrMsgTxt("Exiting CppSim Mex object...");
}
num_a_coeff--;
for (  ; i < num_a_coeff; i++)
  {
    a_exp[i] = a_exp[i+1];
    a_coeff[i] = a_coeff[i+1];
  }

for (i = 0; i < num_a_coeff; i++)
  a_coeff[i] /= -a0_val;  
for (i = 0; i < num_b_coeff; i++)
  b_coeff[i] /= a0_val;  

//////
int num_y_samples_needed = 0;
for (i = 0; i < num_a_coeff; i++)
   if (a_exp[i] > num_y_samples_needed)
      num_y_samples_needed = a_exp[i];

int num_x_samples_needed = 0;
for (i = 0; i < num_b_coeff; i++)
   if (b_exp[i] > num_x_samples_needed)
      num_x_samples_needed = b_exp[i];


if (num_y_samples_needed != num_y_samples || 
    num_x_samples_needed != num_x_samples ||
    (num_x_samples == 0 && num_y_samples == 0))
   new_filt_flag = 1;
else
   new_filt_flag = 0;

if (new_filt_flag == 1)
  {
  if (num_y_samples != 0)
      free(y_samples);
  if (num_x_samples != 0)
      free(x_samples);
  num_y_samples = 0;
  for (i = 0; i < num_a_coeff; i++)
     if (a_exp[i] > num_y_samples)
       num_y_samples = a_exp[i];

  num_x_samples = 0;
  for (i = 0; i < num_b_coeff; i++)
     if (b_exp[i] > num_x_samples)
       num_x_samples = b_exp[i];

  if ((y_samples = (double *) calloc(num_y_samples,sizeof(double))) == NULL)
    {
    mexPrintf("Error in 'set_dt_filt': out of memory for calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }
  for (i = 0; i < num_y_samples; i++)
    y_samples[i] = 0.0;
  if ((x_samples = (double *) calloc(num_x_samples,sizeof(double))) == NULL)
    {
    mexPrintf("Error in 'set_dt_filt': out of memory for calloc\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }
  for (i = 0; i < num_x_samples; i++)
     x_samples[i] = 0.0;
  }

/*
 mexPrintf("num_a_coeff = %d, num_b_coeff = %d, num_x_samples = %d, num_y_samples = %d\n",num_a_coeff,num_b_coeff,num_x_samples,num_y_samples);
for (i = 0; i < num_b_coeff; i++)
     mexPrintf("b_exp = %d ,b_coeff = %5.3f\n",b_exp[i],b_coeff[i]);
 mexPrintf("\n");
for (i = 0; i < num_a_coeff; i++)
     mexPrintf("a_exp = %d ,a_coeff = %5.3f\n",a_exp[i],a_coeff[i]);
*/
}

Filter::~Filter()
{
free(a_coeff);
free(b_coeff);
free(a_exp);
free(b_exp);
free(x_samples);
free(y_samples);
}



void Latch::init(double in)
{
if ((in != 1.0 && in != -1.0) && out_of_range_flag == 0)
  {
   out_of_range_flag = 1;
   mexPrintf("Warning in Latch.init:  input value is not 1.0 or -1.0\n");
   mexPrintf("  in this case, input value = %5.3f\n",in);
  }
outp = in;
out = in;
}

Latch::Latch()
{
out = -1.0;
outp = -1.0;
prev_clk = -1.0;
// the following is used only when reset is used
prev_reset = -1.0;
clk_warning_flag = 0;
out_of_range_flag = 0;
reset_transition_flag = 0;
}

Latch::~Latch()
{
}

double Latch::inp(double in, double clk)
{
out = outp;
inp_base(in,clk);
return(out);
}

double Latch::inp(double in, double clk, double set, double reset)
{
out = outp;
inp_base(in,clk); // updates outp according to in, clk
outp = -outp;
inp_reset(set); // update outp according to set
outp = -outp;
inp_reset(reset); // update outp according to reset
return(out); 
}

void Latch::inp_reset(double reset)
{
if (reset != prev_reset && (prev_reset == 1.0 || prev_reset == -1.0)) 
   //reset transition
   {
    // follow inp_base for reset low (-1.0), set low when reset is high (+1.0)
    // if (prev_reset == 1.0) transition from low - do nothing 
    if (prev_reset == -1.0) // transition to low - time according to reset
       {
       if (outp == 1.0)
	  outp = -reset;
       else
	  outp = -1.0;
       }
   }
else if (reset == -1.0 || reset  == 1.0)
   {
    // follow inp_base for reset low (-1.0), set low when reset is high (+1.0)
    if (reset == 1.0) // set outpput low
	outp = -1.0;
   }
 else if (reset_transition_flag == 0)
  {
    reset_transition_flag = 1;
    mexPrintf("Warning in 'Latch.inp_reset': reset transitions occurred in two consecutive samples\n");
    mexPrintf("  or, reset is outp of its range:  -1.0 <= reset <= 1.0\n");
    mexPrintf("  reset = %5.3f, prev_reset = %5.3f\n",reset,prev_reset);
    mexPrintf("  probable issue:  need to have more samples per clock period\n");
  }

prev_reset = reset;
}

void Latch::inp_base(double in, double clk)
{
if ((in > 1.0 || in < -1.0) && out_of_range_flag == 0)
  {
    out_of_range_flag = 1;
    mexPrintf("Warning in Latch.inp:  in is constrained to be -1.0 <= in <= 1.0\n");
    mexPrintf("  in this case, in = %5.3f\n",in);
  }
if (clk != prev_clk && (prev_clk == 1.0 || prev_clk == -1.0)) //clk transition
   {
    // follow when clk is low (-1.0), hold when clk is high (+1.0)
    if (prev_clk == 1.0) // transition from hold to follow
       outp = (in != outp) ? outp*clk : outp;
    else if (prev_clk == -1.0) // transition from follow to hold
       {
       if (in == 1.0 && outp > 0.0)
	  outp = 1.0;
       else if (in == -1.0 && outp <= 0.0)
	  outp = -1.0;
       else
          {
   	  if (outp <= 0.0) // in is transitioning from -1 to 1
	     outp = (clk < in) ? 1.0 : -1.0;
	  else // in is transitioning from 1 to -1
	     outp = (clk < -in) ? -1.0 : 1.0;
          }
       }
   }
else if (clk == -1.0 || clk  == 1.0)
   {
    // follow when clk is low (-1.0), hold when clk is high (+1.0)
    if (clk == -1.0) // follow
	outp = in;
    else // hold
	outp = (outp > 0.0) ? 1.0 : -1.0;
   }
 else if (clk_warning_flag == 0)
  {
    mexPrintf("Warning:  In 'Latch.inp': clk transitions occurred in two consecutive samples\n");
    mexPrintf("  or, clk is out of its range:  -1.0 <= clk <= 1.0\n");
    mexPrintf("  clk = %5.3f, prev_clk = %5.3f\n",clk,prev_clk);
    mexPrintf("  probable issue:  need to have more samples per clock period\n");
    clk_warning_flag = 1;
  }
prev_clk = clk;
}



void Reg::init(double in)
{
if ((in != 1.0 && in != -1.0) && out_of_range_flag == 0)
  {
   out_of_range_flag = 1;
   mexPrintf("Warning in Reg.init:  input value is not 1.0 or -1.0\n");
   mexPrintf("  in this case, input value = %5.3f\n",in);
  }
lat1.init(in);
lat2.init(in);
out = in;
}

Reg::Reg()
{
out = -1.0;
out_of_range_flag = 0;
}

Reg::~Reg()
{
}

double Reg::inp(double in, double clk)
{
lat1.inp(in,clk);
lat2.inp(lat1.out,-clk);
out = lat2.out;
return(out);
}

double Reg::inp(double in, double clk, double set, double reset)
{
lat1.inp(in,clk,set,reset);
lat2.inp(lat1.out,-clk,set,reset);
out = lat2.out;
return(out);
}


Xor::Xor()
{
out = -1.0;
}

Xor::~Xor()
{
}

double Xor::inp(double in0, double in1)
{
out = -in0*in1;
return(out);
}



double And2::inp(double in0, double in1)
{
if (out == -1.0)
  {
    if (in0 != -1.0 && in1 != -1.0)
      {
	state = 1;
	if (in0 < in1) // in0 transition occurs later than in1
	  out = in0;
        else
          out = in1;
      }
  }
else if (out == 1.0)
  {
    if (in0 != 1.0 || in1 != 1.0)
      {
	state = 0;
	if (-in0 < -in1) // in0 transition occurs later than in1
	  out = in1;
        else
          out = in0;
      }
  }
else
  {
    if (in0 == 1.0 && in1 == 1.0)
      out = 1.0;
    else if (in0 == -1.0 || in1 == -1.0)
      out = -1.0;
    else
      {
      if (state == 0)
	{
         if (in0 != -1.0 && in1 != -1.0)
            {
	    state = 1;
	    if (in0 < in1) // in0 transition occurs later than in1
	       out = in0;
            else
               out = in1;
            }
	}
      else
	{
         if (in0 != 1.0 || in1 != 1.0)
            {
	    state = 0;
	    if (-in0 < -in1) // in0 transition occurs later than in1
	       out = in1;
            else
              out = in0;
            }
	}
      }
  }
return(out);
}

And2::And2()
{
state = 0;
out = -1.0;
}

And2::~And2()
{
}


double And::inp(double in0, double in1)
{
out = and1.inp(in0,in1);
return(out);
}

double And::inp(double in0, double in1, double in2)
{
out = and2.inp(and1.inp(in0,in1),in2);
return(out);
}

double And::inp(double in0, double in1, double in2, double in3)
{
out = and3.inp(and2.inp(and1.inp(in0,in1),in2),in3);
return(out);
}

double And::inp(double in0, double in1, double in2, double in3, double in4)
{
out = and4.inp(and3.inp(and2.inp(and1.inp(in0,in1),in2),in3),in4);
return(out);
}

And::And()
{
out = -1.0;
}

And::~And()
{
}


double Or::inp(double in0, double in1)
{
out = -and1.inp(-in0,-in1);
return(out);
}
double Or::inp(double in0, double in1, double in2)
{
out = -and2.inp(and1.inp(-in0,-in1),-in2);
return(out);
}
double Or::inp(double in0, double in1, double in2, double in3)
{
out = -and3.inp(and2.inp(and1.inp(-in0,-in1),-in2),-in3);
return(out);
}
double Or::inp(double in0, double in1, double in2, double in3, double in4)
{
out = -and4.inp(and3.inp(and2.inp(and1.inp(-in0,-in1),-in2),-in3),-in4);
return(out);
}

Or::Or()
{
out = -1.0;
}
Or::~Or()
{
}

long Rand::idum = -1;

double Rand::inp()
{
if (type_flag == 0)
   out = (double) gasdev(&idum);
else if (type_flag == 1)
   out = (double) ran1(&idum);
else if (type_flag == 2)
   out = bernoulli(p_bern);
else
  {
    mexPrintf("error in Rand out function:  must have 0 <= type_flag <= 2\n");
    mexPrintf("   in this case, type_flag = %d\n",type_flag);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
return(out);
}

Rand::Rand(const char *type)
{
set(type,0.5); // p_bern no important except for Bernoulli case
out = 0.0;
}
Rand::Rand(const char *type, double p)
{
set(type,p);
out = 0.0;
}

Rand::~Rand()
{
}

void Rand::set_seed(int in)
{
idum = (long) in;
}

void Rand::reset()
{
idum = -1;
}

void Rand::set(const char *type, double p)
{
if (strcmp(type,"gauss") == 0)
   type_flag = 0;
else if (strcmp(type,"uniform") == 0)
   type_flag = 1;
else if (strcmp(type,"bernoulli") == 0)
   type_flag = 2;
else
  {
   mexPrintf("error in Rand constructor:  type must be 'gauss',\n");
   mexPrintf("               'uniform', or 'bernoulli'\n");
   mexPrintf("  in this case, type = '%s'\n",type);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
p_bern = p;
}


/* FUNCTION:  bernoulli() -- generates bernoulli random variables */

double Rand::bernoulli(double p)
{
  double pp,out;
  static long idum=-1;

  //idum = 1;

  pp = (p <= 0.5 ? p : 1.0 - p);

  if (ran1(&idum) < pp)
    out  = 1.0;
  else
    out = -1.0;

  if (p != pp)
    out *= -1.0;

  return(out);
}


/* FUNCTION:  gasdev() -- generates gaussian random variables */

float Rand::gasdev(long *idum)   
{   
  static int iset=0;   
  static float gset;   
  float fac,r,v1,v2;   
   
  if (iset == 0)   
    {   
      do {   
	v1 = 2.0*ran1(idum)-1.0;   
	v2 = 2.0*ran1(idum)-1.0;   
	r = v1*v1 + v2*v2;   
      }  while (r >= 1.0);   
      fac = sqrt(-2.0*log(r)/r);   
      gset = v1*fac;   
      iset = 1;   
      return(v2*fac);   
    }   
  else   
    {   
      iset = 0;   
      return(gset);   
    }   
}   
   
/* FUNCTION:  ran1() -- generates uniform (0,1] random variables */

/*
  ran1 is from NR V2.04
  */

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)
 
float Rand::ran1(long *idum)
{
  int j;
  long k;
  static long iy=0;
  static long iv[NTAB];
  float temp;
 
  if (*idum <= 0 || !iy) {
    if (-(*idum) < 1) *idum=1;
    else *idum = -(*idum);
    for (j=NTAB+7;j>=0;j--) {
      k=(*idum)/IQ;
      *idum=IA*(*idum-k*IQ)-IR*k;
      if (*idum < 0) *idum += IM;
      if (j < NTAB) iv[j] = *idum;
    }
    iy=iv[0];
  }
  k=(*idum)/IQ;
  *idum=IA*(*idum-k*IQ)-IR*k;
  if (*idum < 0) *idum += IM;
  j=iy/NDIV;
  iy=iv[j];
  iv[j] = *idum;
  if ((temp=AM*iy) > RNMX) return RNMX;
  else return temp;
}
#undef IA
#undef IM
#undef AM
#undef IQ
#undef IR
#undef NTAB
#undef NDIV
#undef EPS
#undef RNMX


double SdMbitMod::inp(double in)
{
double u;

u = in + H.out;
out = floor(u);
H.inp(out-u);
return(out);
}

SdMbitMod::SdMbitMod(const char *num, const char *vars, 
       double var0, double var1, double var2, double var3, double var4, 
       double var5, double var6, double var7, double var8, double var9)
  : H(num,"1.0",vars,var0,var1,var2,var3,var4,var5,var6,var7,var8,var9)
{
int i,flag;
if (H.ivar != 'z')
  {
   mexPrintf("error in SdMbitMod constructor: ivar for H(z) is not 'z'\n");
   mexPrintf("  in this case, ivar is '%c'\n",H.ivar);
   mexPrintf("  for H(z): %s\n",num);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
flag = 0;
for (i = 0; i < H.num_b_coeff; i++)
   if (H.b_exp[i] == 0)
       if (H.b_coeff[i] == 1.0)
           flag = 1;

if (flag == 0)
  {
   mexPrintf("error in SdMbitMod constructor: must have H(z)\n");
   mexPrintf("  whose z^0 coefficient is exactly 1.0\n");
   mexPrintf("  in this case, H(z) is:  %s\n",num);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
else // want to implement H(z) - 1 and accomodate the delay due to feedback
  {
   for (i = 0; i < H.num_b_coeff; i++)
      if (H.b_exp[i] == 0)
	 {
          H.b_coeff[i] = 0.0;
          H.b_exp[i] += 1;
	 }
   for (i = 0; i < H.num_b_coeff; i++)
      H.b_exp[i] -= 1;
  }
out = 0.0;
}

void SdMbitMod::set(const char *num, const char *vars, 
       double var0, double var1, double var2, double var3, double var4, 
       double var5, double var6, double var7, double var8, double var9)
{
int i,flag;

H.set(num,"1.0",vars,var0,var1,var2,var3,var4,var5,var6,var7,var8,var9);
if (H.ivar != 'z')
  {
   mexPrintf("error in SdMbitMod constructor: ivar for H(z) is not 'z'\n");
   mexPrintf("  in this case, ivar is '%c'\n",H.ivar);
   mexPrintf("  for H(z): %s\n",num);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
flag = 0;
for (i = 0; i < H.num_b_coeff; i++)
   if (H.b_exp[i] == 0)
       if (H.b_coeff[i] == 1.0)
           flag = 1;

if (flag == 0)
  {
   mexPrintf("error in SdMbitMod constructor: must have H(z)\n");
   mexPrintf("  whose z^0 coefficient is exactly 1.0\n");
   mexPrintf("  in this case, H(z) is:  %s\n",num);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
else // want to implement H(z) - 1 and accomodate the delay due to feedback
  {
   for (i = 0; i < H.num_b_coeff; i++)
      if (H.b_exp[i] == 0)
	 {
          H.b_coeff[i] = 0.0;
          H.b_exp[i] += 1;
	 }
   for (i = 0; i < H.num_b_coeff; i++)
      H.b_exp[i] -= 1;
  }
}

SdMbitMod::~SdMbitMod()
{
}


SigGen::SigGen(const char *type, double freq, double Ts)
  :  rand1("bernoulli")
{
clk_state = 0;
prev_phase = 0.0;
phase = 0.0;
out = 0.0;
prev_square = 1.0;
square = 1.0;
data = 1.0;
prev_data = 1.0;
prev_in = 0.0;
first_entry_flag = 1;
end_data_flag = 0;

set(type,freq,Ts);
}

SigGen::SigGen(const char *type, double freq, double Ts, const List &other)
  :  rand1("bernoulli")
{
clk_state = 0;
prev_phase = -(Ts*freq)/2.0;
phase = prev_phase;
out = 0.0;
prev_square = 1.0;
square = 1.0;
data = 1.0;
prev_data = 1.0;
prev_in = 0.0;
first_entry_flag = 1;
end_data_flag = 0;

set(type,freq,Ts,other);
}

SigGen::SigGen(const char *type, double freq, double Ts, const List &other, 
               int start_entry)
  :  rand1("bernoulli")
{
clk_state = 0;
prev_phase = -(Ts*freq)/2.0;
phase = prev_phase;
out = 0.0;
prev_square = 1.0;
square = 1.0;
data = 1.0;
prev_data = 1.0;
first_entry_flag = 1;
end_data_flag = 0;
prev_in = 0.0;

set(type,freq,Ts,other,start_entry);
}

void SigGen::reset()
{
clk_state = 0;
prev_phase = -(sample_period*frequency)/2.0;
phase = prev_phase;
out = 0.0;
prev_square = 1.0;
square = 1.0;
data = 1.0;
prev_data = 1.0;
prev_in = 0.0;
first_entry_flag = 1;
end_data_flag = 0;
data_seq.reset();
phase_filt.reset(0.0);
}

void SigGen::set(const char *type, double freq, double Ts)
{
sample_period = Ts;
frequency = freq;
phase_filt.set("a","1 - (1-a)*z^-1","a",0.5*freq*Ts);

if (sample_period < 1e-30)
  {
    mexPrintf("error in SigGen.set:  Ts < 1e-30\n");
    mexPrintf("   in this case, Ts = %5.3e\n",Ts);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (freq <= 0.0)
  {
    mexPrintf("error in SigGen.set:  freq <= 0.0\n");
    mexPrintf("   in this case, freq = %5.3e\n",freq);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }

if (strcmp(type,"square") == 0)
   type_flag = 0;
else if (strcmp(type,"sine") == 0)
   type_flag = 1;
else if (strcmp(type,"prbs") == 0)
   type_flag = 2;
else if (strcmp(type,"impulse") == 0)
   type_flag = 3;
else
  {
   mexPrintf("error in SigGen.set:  type must be either\n");
   mexPrintf("  'square', 'sine', 'prbs', or 'impulse'\n");
   mexPrintf("  in this case, type = '%s'\n",type);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
}

void SigGen::set(const char *type, double freq, double Ts, const List &other)
{
sample_period = Ts;
frequency = freq;
phase_filt.set("a","1 - (1-a)*z^-1","a",0.5*freq*Ts);

if (sample_period < 1e-30)
  {
    mexPrintf("error in SigGen.set:  Ts < 1e-30\n");
    mexPrintf("   in this case, Ts = %5.3e\n",Ts);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (freq <= 0.0)
  {
    mexPrintf("error in SigGen.set:  freq <= 0.0\n");
    mexPrintf("   in this case, freq = %5.3e\n",freq);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }

if (strcmp(type,"square") == 0)
   type_flag = 0;
else if (strcmp(type,"sine") == 0)
   type_flag = 1;
else if (strcmp(type,"prbs") == 0)
   type_flag = 2;
else if (strcmp(type,"impulse") == 0)
   type_flag = 3;
else
  {
   mexPrintf("error in SigGen.set:  type must be either\n");
   mexPrintf("  'square', 'sine', 'prbs', or 'impulse'\n");
   mexPrintf("  in this case, type = '%s'\n",type);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

end_data_flag = 0;
prev_data = 1.0;
data_seq.copy(other);
data = data_seq.read() > 0.0 ? 1.0 : -1.0;
if (data != -1.0 && data != 1.0)
  {
   mexPrintf("error in SigGen.set:  data list has values that are not 1.0 or -1.0\n");
   mexPrintf("  in this case, the data value is %5.3f\n",data);
  }
if (type_flag == 2)
  {
   reg1.init(data);
   data = data_seq.read() > 0.0 ? 1.0 : -1.0;
   if (data != -1.0 && data != 1.0)
      {
      mexPrintf("error in SigGen.set:  data list has values that are not 1.0 or -1.0\n");
      mexPrintf("  in this case, the data value is %5.3f\n",data);
      }
  }
prev_data = data;
// data_seq.reset();
}

void SigGen::set(const char *type, double freq, double Ts, const List &other,
                 int start_entry)
{
sample_period = Ts;
frequency = freq;
phase_filt.set("a","1 - (1-a)*z^-1","a",0.5*freq*Ts);
int i;

if (start_entry < 1)
   {
     mexPrintf("error in SigGen.set:  starting entry for List input must be >= 1\n");
     mexPrintf("   (note that a value of 1 corresponds to the first entry)\n");
     mexPrintf("   in this case, start_entry = %d\n",start_entry);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if (sample_period < 1e-30)
  {
    mexPrintf("error in SigGen.set:  Ts < 1e-30\n");
    mexPrintf("   in this case, Ts = %5.3e\n",Ts);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (freq <= 0.0)
  {
    mexPrintf("error in SigGen.set:  freq <= 0.0\n");
    mexPrintf("   in this case, freq = %5.3e\n",freq);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }

if (strcmp(type,"square") == 0)
   type_flag = 0;
else if (strcmp(type,"sine") == 0)
   type_flag = 1;
else if (strcmp(type,"prbs") == 0)
   type_flag = 2;
else if (strcmp(type,"impulse") == 0)
   type_flag = 3;
else
  {
   mexPrintf("error in SigGen.set:  type must be either\n");
   mexPrintf("  'square', 'sine', 'prbs', or 'impulse'\n");
   mexPrintf("  in this case, type = '%s'\n",type);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

end_data_flag = 0;
prev_data = 1.0;
data_seq.copy(other);
for (i = 0; i < start_entry; i++)
   data = data_seq.read() > 0.0 ? 1.0 : -1.0;
if (data != -1.0 && data != 1.0)
  {
   mexPrintf("error in SigGen.set:  data list has values that are not 1.0 or -1.0\n");
   mexPrintf("  in this case, the data value is %5.3f\n",data);
  }
if (type_flag == 2)
  {
   reg1.init(data);
   data = data_seq.read() > 0.0 ? 1.0 : -1.0;
   if (data != -1.0 && data != 1.0)
      {
      mexPrintf("error in SigGen.set:  data list has values that are not 1.0 or -1.0\n");
      mexPrintf("  in this case, the data value is %5.3f\n",data);
      }
  }
prev_data = data;
// data_seq.reset();
// for (i = 0; i < start_entry; i++)
//   data = data_seq.read() > 0.0 ? 1.0 : -1.0;
}

SigGen::~SigGen()
{
}

double SigGen::inp(double in)
{
double impulse;

if (first_entry_flag == 1)
   {
     prev_in = 0.0;
     phase_filt.reset(in);
     prev_phase = -1.0; // start one cycle back
     first_entry_flag = 0;
   }

else if (((in-prev_in) < -1.0) || ((in-prev_in) > 1.0))
   {
    mexPrintf("error in SigGen.inp: input phase cannot change\n");
    mexPrintf("     instantaneously by more than a cycle\n");
    mexPrintf("  i.e.,  -1.0 <= delta in <= 1.0\n");
    mexPrintf("  in this case, in = %5.4f, prev_in = %5.4f, delta in = %5.4f\n",
              in,prev_in,in-prev_in);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
phase_filt.inp(in);
phase = prev_phase + sample_period*frequency - (phase_filt.out-prev_in);

if (phase >= 1.0)
  {
   phase -= 1.0;
  }

//  state = 0:  clk phase between 0.5 and 1.0
//  state = 1:  clk phase between 0 and 0.5

if (prev_phase >= phase)
     prev_phase -= 1.0;

if (phase >= 0.0 && phase < 0.5) /* state = 1 */
  {
    if (clk_state == 1) /* stay in state 1 */
       square = 1.0;
    else  /* transition from state=0 to state=1 */
       square = (phase+prev_phase)/(phase-prev_phase);
    clk_state = 1;
    impulse = .5*(square-prev_square);
  }
else /* state = 0 */
  {
    if (clk_state == 0) /* stay in state 0 */
       square = -1.0;
    else /* transition from state=1 to state=0 */
       square = (1.0-(phase+prev_phase))/(phase-prev_phase);
    clk_state = 0;
    impulse = 0.0;
  }

if (square < -1.0 || square > 1.0)
  {
   mexPrintf("error in SigGen.inp:  interpolated output beyond -1 to 1 range\n");
   mexPrintf("  in this case, square out = %5.3f\n",square);
   mexPrintf("  probable cause:  input inappropriate\n");
   mexPrintf("  in this case, in = %5.3f\n",in);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }


if (type_flag == 0) // square
   out = square;
else if (type_flag == 1) // sine
   out = sin(2*PI*phase);
else if (type_flag == 2) // prbs
   out = reg1.out;
else if (type_flag == 3) // impulse
   out = impulse*data;
else
  {
   mexPrintf("error in SigGen.inp:  type_flag not recognized\n");
   mexPrintf("  in this case, type_flag = %d\n",type_flag);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

reg1.inp(data,square);
if (square == 1.0 && prev_square != 1.0) // impulse has finished transitioning
  {
   if (data_seq.length == 0)
      data = rand1.inp();
   else if (end_data_flag == 0)
      data = data_seq.read();
   if (data != -1.0 && data != 1.0)
      {
       data = prev_data;
       end_data_flag = 1;
	/*
       mexPrintf("error in SigGen.set:  data list has values that are not 1.0 or -1.0\n");
       mexPrintf("  in this case, the data value is %5.3f\n",data);
	*/
      }
   else
     {
      data = data > 0.0 ? 1.0 : -1.0;
      prev_data = data;
     }
  }
prev_square = square;
prev_in = phase_filt.out;
prev_phase = phase;

return(out);
}




void create_data(NODE *first_node, FILE *fp, int flush_buffer, float *fbuf,
                 size_t *buffer_length)
{
unsigned int ibuf[4];
NODE *cur_node;
size_t buf_length;

buf_length = *buffer_length;
cur_node = first_node;
if (flush_buffer == 1)
  {
   ibuf[0] = 4;
   ibuf[1] = 128;
   ibuf[2] = 4;
   ibuf[3] = buf_length*4;
   fwrite_int_values(ibuf,4,fp);

   if (fwrite(fbuf, sizeof(float),buf_length,fp) != buf_length)    
      {
       mexPrintf("error using 'create_data': out of disk space\n");
       mexErrMsgTxt("Exiting CppSim Mex object...");
      }
   ibuf[0] = buf_length*4;
   fwrite_int_values(ibuf,1,fp);
   buf_length = 0;
   return;
  }

while(cur_node != NULL)
  {
   if (buf_length >= DATA_BUFFER_LENGTH)
     {
      ibuf[0] = 4;
      ibuf[1] = 128;
      ibuf[2] = 4;
      ibuf[3] = buf_length*4;
      fwrite_int_values(ibuf,4,fp);

      if (fwrite(fbuf, sizeof(float),buf_length,fp) != buf_length)
         {
         mexPrintf("error using 'create_data': out of disk space\n");
         mexErrMsgTxt("Exiting CppSim Mex object...");
         }
      ibuf[0] = buf_length*4;
      fwrite_int_values(ibuf,1,fp);
      buf_length = 0;
     }
   fbuf[buf_length++] = ((float) cur_node->value);
   cur_node = cur_node->next;
  }

*buffer_length = buf_length;
}

void create_header(NODE *first_node, FILE *fp)
{
unsigned int ibuf[5];
char cbuf[CHAR_BUFFER_LENGTH],temp[MAX_NAME_LENGTH];
int i,num_signals,str_len,buf_number,name_length;
NODE *cur_node;

cur_node = first_node;
if (cur_node == NULL)
   {
    mexPrintf("error in 'create_header':  first_node is NULL\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
num_signals = 1;
while(cur_node->next != NULL)
  {
   num_signals++;
   cur_node = cur_node->next;
  }
sprintf(temp,"%4d",num_signals);
for (i = 0; temp[i] == ' '; i++)
  temp[i] = '0';
sprintf(cbuf,"%s0000000000009601    Output Signals from C++ Simulation.                                                                                                                                0%c",temp,'\0');
for (i = 0; cbuf[i] != '\0'; i++);
for (  ; i < 264; i++)
  cbuf[i] = ' ';
cbuf[i] = '\0'; 
str_len = 264;
str_len += 8;  // 8 bytes needed at the end
buf_number = 1;
for (i = 0; i < num_signals; i++)
  {
    if ((str_len + 8) >= (CHAR_BUFFER_LENGTH-1)) 
      {
      buf_number++;
      strcat(cbuf,"        ");

      ibuf[0] = 4;
      ibuf[1] = 112;
      ibuf[2] = 4;
      ibuf[3] = str_len;
      fwrite_int_values(ibuf,4,fp);
      strcat(cbuf,"        ");
      fwrite_char_values(cbuf,str_len,fp);
      ibuf[0] = str_len;
      fwrite_int_values(ibuf,1,fp);

      str_len = 12;
      sprintf(cbuf,"    ");
      }

   strcat(cbuf,"  1     ");
   str_len += 8;
  }
cur_node = first_node;
for (i = 0; i < num_signals; i++)
  {
    strcpy(temp,cur_node->name);
    strcat(temp," ");
    for (name_length = 0; temp[name_length] != '\0';name_length++);
    for (  ; name_length % 16 != 0 ; name_length++)
      {
      temp[name_length] = ' ';
      }
    temp[name_length] = '\0';
    if ((str_len + name_length) >= (CHAR_BUFFER_LENGTH-1)) 
      {
      buf_number++;

      ibuf[0] = 4;
      ibuf[1] = 112;
      ibuf[2] = 4;
      ibuf[3] = str_len;
      fwrite_int_values(ibuf,4,fp);
      strcat(cbuf,"        ");
      fwrite_char_values(cbuf,str_len,fp);
      ibuf[0] = str_len;
      fwrite_int_values(ibuf,1,fp);

      str_len = 12;
      sprintf(cbuf,"    ");
      }

    strcat(cbuf,temp);
    str_len += name_length;

    cur_node = cur_node->next;
  }
strcat(cbuf,"$&%#    ");

ibuf[0] = 4;
ibuf[1] = 112;
ibuf[2] = 4;
ibuf[3] = str_len;
fwrite_int_values(ibuf,4,fp);
fwrite_char_values(cbuf,str_len,fp);
ibuf[0] = str_len;
fwrite_int_values(ibuf,1,fp); 
}

////////////////////////////////////////////////////////////

void create_data64(NODE *first_node, FILE *fp, int flush_buffer, double *dbuf,
                 size_t *buffer_length)
{
unsigned int ibuf[4];
NODE *cur_node;
size_t buf_length;

buf_length = *buffer_length;
cur_node = first_node;
if (flush_buffer == 1)
  {
   ibuf[0] = 8;
   ibuf[1] = 128;
   ibuf[2] = 8;
   ibuf[3] = buf_length*8;
   fwrite_int_values(ibuf,4,fp);

   if (fwrite(dbuf, sizeof(double),buf_length,fp) != buf_length)    
      {
       mexPrintf("error using 'create_data64': out of disk space\n");
       mexErrMsgTxt("Exiting CppSim Mex object...");
      }
   ibuf[0] = buf_length*8;
   fwrite_int_values(ibuf,1,fp);
   buf_length = 0;
   return;
  }

while(cur_node != NULL)
  {
   if (buf_length >= DATA_BUFFER_LENGTH)
     {
      ibuf[0] = 8;
      ibuf[1] = 128;
      ibuf[2] = 8;
      ibuf[3] = buf_length*8;
      fwrite_int_values(ibuf,4,fp);

      if (fwrite(dbuf, sizeof(double),buf_length,fp) != buf_length)
         {
         mexPrintf("error using 'create_data64': out of disk space\n");
         mexErrMsgTxt("Exiting CppSim Mex object...");
         }
      ibuf[0] = buf_length*8;
      fwrite_int_values(ibuf,1,fp);
      buf_length = 0;
     }
   dbuf[buf_length++] = cur_node->value;
   cur_node = cur_node->next;
  }

*buffer_length = buf_length;
}

void create_header64(NODE *first_node, FILE *fp)
{
unsigned int ibuf[5];
char cbuf[CHAR_BUFFER_LENGTH],temp[MAX_NAME_LENGTH];
int i,num_signals,str_len,buf_number,name_length;
NODE *cur_node;

cur_node = first_node;
if (cur_node == NULL)
   {
    mexPrintf("error in 'create_header64':  first_node is NULL\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
num_signals = 1;
while(cur_node->next != NULL)
  {
   num_signals++;
   cur_node = cur_node->next;
  }
sprintf(temp,"%4d",num_signals);
for (i = 0; temp[i] == ' '; i++)
  temp[i] = '0';
sprintf(cbuf,"%s0000000000009602    Output Signals from C++ Simulation.                                                                                                                                0%c",temp,'\0');
for (i = 0; cbuf[i] != '\0'; i++);
for (  ; i < 264; i++)
  cbuf[i] = ' ';
cbuf[i] = '\0'; 
str_len = 264;
str_len += 8;  // 8 bytes needed at the end
buf_number = 1;
for (i = 0; i < num_signals; i++)
  {
    if ((str_len + 8) >= (CHAR_BUFFER_LENGTH-1)) 
      {
      buf_number++;
      strcat(cbuf,"        ");

      ibuf[0] = 4;
      ibuf[1] = 112;
      ibuf[2] = 4;
      ibuf[3] = str_len;
      fwrite_int_values(ibuf,4,fp);
      strcat(cbuf,"        ");
      fwrite_char_values(cbuf,str_len,fp);
      ibuf[0] = str_len;
      fwrite_int_values(ibuf,1,fp);

      str_len = 12;
      sprintf(cbuf,"    ");
      }

   strcat(cbuf,"  1     ");
   str_len += 8;
  }
cur_node = first_node;
for (i = 0; i < num_signals; i++)
  {
    strcpy(temp,cur_node->name);
    strcat(temp," ");
    for (name_length = 0; temp[name_length] != '\0';name_length++);
    for (  ; name_length % 16 != 0 ; name_length++)
      {
      temp[name_length] = ' ';
      }
    temp[name_length] = '\0';
    if ((str_len + name_length) >= (CHAR_BUFFER_LENGTH-1)) 
      {
      buf_number++;

      ibuf[0] = 4;
      ibuf[1] = 112;
      ibuf[2] = 4;
      ibuf[3] = str_len;
      fwrite_int_values(ibuf,4,fp);
      strcat(cbuf,"        ");
      fwrite_char_values(cbuf,str_len,fp);
      ibuf[0] = str_len;
      fwrite_int_values(ibuf,1,fp);

      str_len = 12;
      sprintf(cbuf,"    ");
      }

    strcat(cbuf,temp);
    str_len += name_length;

    cur_node = cur_node->next;
  }
strcat(cbuf,"$&%#    ");

ibuf[0] = 4;
ibuf[1] = 112;
ibuf[2] = 4;
ibuf[3] = str_len;
fwrite_int_values(ibuf,4,fp);
fwrite_char_values(cbuf,str_len,fp);
ibuf[0] = str_len;
fwrite_int_values(ibuf,1,fp); 
}

////////////////////////////////////////////////////////////

/* fix problem with big endian, little endian issue */
void  fwrite_int_values(unsigned int *ibuf, size_t size_of_block, FILE *fp)
  {
   unsigned char *charbuf;
   size_t i;
   int mask0,mask1,mask2,mask3;

   mask0 = (1<<8)-1;
   mask1 = mask0<<8;
   mask2 = mask1<<8;
   mask3 = mask2<<8;

   if ((charbuf = (unsigned char *) calloc(4*size_of_block,sizeof(unsigned char))) == NULL)   
         {   
          mexPrintf("error in 'fwrite_int_values':  mxCalloc call failed - \n");   
          mexPrintf("  not enough memory!!\n");   
          mexErrMsgTxt("Exiting CppSim Mex object...");   
        }   
   for (i = 0; i < size_of_block; i++)
      {
#ifdef WORDS_BIGENDIAN
      charbuf[i*4] = (ibuf[i] & mask3) >> 24;
      charbuf[i*4+1] = (ibuf[i] & mask2) >> 16;
      charbuf[i*4+2] = (ibuf[i] & mask1) >> 8;
      charbuf[i*4+3] = ibuf[i] & mask0;
#else
      charbuf[i*4+3] = (ibuf[i] & mask3) >> 24;
      charbuf[i*4+2] = (ibuf[i] & mask2) >> 16;
      charbuf[i*4+1] = (ibuf[i] & mask1) >> 8;
      charbuf[i*4] = ibuf[i] & mask0;
#endif
      }
   if (fwrite(charbuf, sizeof(unsigned char),4*size_of_block,fp) 
       != 4*size_of_block)
      {
       mexPrintf("error using 'fwrite_int_values': out of disk space\n");
       mexErrMsgTxt("Exiting CppSim Mex object...");
      }
   free(charbuf);
  }


void  fwrite_char_values(char *cbuf, size_t size_of_block, FILE *fp)
  {
    if (fwrite(cbuf,sizeof(char), size_of_block, fp) != size_of_block)
      {
       mexPrintf("error using 'fwrite_char_values': out of disk space\n");
       mexErrMsgTxt("Exiting CppSim Mex object...");
      }
  }



NODE *init_node(const char *name, double value)
{   
NODE *A;   
   
if ((A = (NODE *) malloc(sizeof(NODE))) == NULL)   
   {   
   mexPrintf("error in 'init_node':  malloc call failed\n");   
   mexPrintf("out of memory!\n");   
   mexErrMsgTxt("Exiting CppSim Mex object...");   
  }   
strncpy(A->name,name,MAX_NODE_NAME_LENGTH-1);   
A->value = value;
A->next = NULL;
return(A);   
}


NODE *add_node(NODE *first_node, const char *name, double value)
{
NODE *cur_node;

if (first_node == NULL)
   first_node = init_node(name,value);
else
  {
   cur_node = first_node;
   while(cur_node->next != NULL)
      cur_node = cur_node->next;
   cur_node->next = init_node(name,value);
  }
return(first_node);
}   



PARAM *init_param()
{   
PARAM *A;   
   
if ((A = (PARAM *) malloc(sizeof(PARAM))) == NULL)   
   {   
   mexPrintf("error in 'init_param':  malloc call failed\n");   
   mexPrintf("out of memory!\n");   
   mexErrMsgTxt("Exiting CppSim Mex object...");   
  }   
A->main_par_length = 0;
A->nest_length = 0;
return(A);   
}   


double eval_paren_expr(char *input, PARAM *par_list)
{
int i,j,left_paren_count, right_paren_count;
int left_paren[LENGTH_PAR_LIST];
int curr_right_paren,curr_left_paren,nest_count;
char nest_val_string[3];
double return_value;

/* replace any negative signs following 'e' or 'E' by '!' */
/* this simplifies parsing when dealing with exponential notation */
for (i = 0; input[i] != '\0'; i++)
  {
   if (input[i] == 'e' || input[i] == 'E')
     {
       if (input[i+1] == '-')
           input[i+1] = '!';
       else if (input[i+1] == '+')
	  input[i+1] = '0';
     }
  }
left_paren_count = -1;
right_paren_count = -1;
for (i=0; input[i] != '\0'; i++)
  {
   switch(input[i])
     {
     case '(':
       left_paren_count++;
       if (left_paren_count >= LENGTH_PAR_LIST)
	 {
          mexPrintf("error in 'eval_paren_expr':  parenthesis nested too deep\n");
          mexPrintf("   expression:  %s\n",input);
	  mexPrintf("   overall expression: %s\n",par_list->overall_expr);
          mexErrMsgTxt("Exiting CppSim Mex object...");
	}
       left_paren[left_paren_count] = i;
       break;
     case ')':
       right_paren_count++;
     }
 }
if (right_paren_count != left_paren_count)
  {
    mexPrintf("error in 'eval_paren_expr':  parenthesis mismatch\n");
    mexPrintf("   expression:  %s\n",input);
    mexPrintf("   overall expression: %s\n",par_list->overall_expr);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
par_list->nest_length = 0;

for (nest_count = 0; left_paren_count >= 0; left_paren_count--)
  {
    if (nest_count > 99)
      {
	mexPrintf("error in 'eval_paren_expr':  parenthesis nested too deep\n");
	mexPrintf("   expression:  %s\n",input);
        mexPrintf("   overall expression: %s\n",par_list->overall_expr);
	mexPrintf("  note:  LENGTH_PAR_LIST should not be set above 100!\n");
	mexErrMsgTxt("Exiting CppSim Mex object...");
      }
    sprintf(nest_val_string,"%d",nest_count);
    curr_left_paren = left_paren[left_paren_count];
    for (i = curr_left_paren; input[i] != ')' && input[i] != '\0'; i++);
    if (input[i] == '\0')
      {
	mexPrintf("error in 'eval_paren_expr':  right and left parenthesis mismatch\n");
	mexPrintf("   expression:  %s\n",input);
        mexPrintf("   overall expression: %s\n",par_list->overall_expr);
	mexErrMsgTxt("Exiting CppSim Mex object...");
      }
    curr_right_paren = i;
    input[curr_right_paren] = '\0';
    
    par_list->nest_val[nest_count++] = 
           eval_no_paren_expr(&input[curr_left_paren+1],par_list);
    par_list->nest_length++;
    for (j = 0, i = curr_left_paren+1; j < 2; i++,j++)
      input[i] = nest_val_string[j];
    for (j = 0, i = curr_left_paren + 1; input[i] != '\0' && j < 2; i++,j++);
    for (   ; i <= curr_right_paren; i++)
      input[i] = ' ';
  }

return_value = eval_no_paren_expr(input,par_list);
return(return_value);
}


double eval_no_paren_expr(char *input, PARAM *par_list)
{
int i,j,arg_num,oper_num,arg_flag,found_in_list_flag;
char arg_list[LENGTH_PAR_LIST][LENGTH_PAR];
double arg_value[LENGTH_PAR_LIST];
char oper_list[LENGTH_PAR_LIST];
int error_flag,negative_found_at_begin;
double exp_value;

arg_num = 0;
oper_num = 0;
negative_found_at_begin = 0;
for (i = 0; input[i] == ' '; i++);

if (input[i] == '-')
  {
   negative_found_at_begin = 1;
   for (i++; input[i] == ' '; i++);
 }

/*  parse expression */
while(1)
  {
    if (arg_num > LENGTH_PAR_LIST)
      {
	mexPrintf("error in 'eval_no_paren_expr':  too many strings \n");
        mexPrintf("   expression = %s\n",input);
        mexPrintf("   overall expression: %s\n",par_list->overall_expr);
	mexErrMsgTxt("Exiting CppSim Mex object...");
      }
    arg_flag = 0;
    for (j = 0; input[i] != ' ' && input[i] != '\0' && input[i] != '*' &&
	 input[i] != '+' && input[i] != '\n' && input[i] != '/' &&
         input[i] != '^' && input[i] != '-'; j++,i++)
      {
        arg_flag = 1;
	if (j >= LENGTH_PAR-1)
	  {
	    mexPrintf("error in 'eval_no_paren_expr':  string too long\n");
	    mexPrintf("   expression = %s\n",input);
  	    mexPrintf("   overall expression: %s\n",par_list->overall_expr);
	    mexErrMsgTxt("Exiting CppSim Mex object...");
	  }
	arg_list[arg_num][j] = input[i];
      }
    if (arg_flag == 1)
      {
       arg_list[arg_num][j] = '\0';
       arg_num++;
      }
    if (input[i] == ' ')
       for (   ; input[i] == ' '; i++);
    if (input[i] == '\n' || input[i] == '\0')
        break;
    else if (input[i] == '+' || input[i] == '-' || input[i] == '*' ||
             input[i] == '/' || input[i] == '^')
      {
       if (oper_num > LENGTH_PAR_LIST)
	 {
	   mexPrintf("error in 'eval_no_paren_expr':  too many operators \n");
	   mexPrintf("   expression = %s\n",input);
 	   mexPrintf("   overall expression: %s\n",par_list->overall_expr);
	   mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       oper_list[oper_num] = input[i];
       oper_num++;
       for (i++; input[i] == ' '; i++);
      }
    else
      {
	mexPrintf("error in 'eval_no_paren_expr':  two arguments in a row\n");
	mexPrintf("   expression = %s\n",input);
        mexPrintf("   overall expression: %s\n",par_list->overall_expr);
	mexErrMsgTxt("Exiting CppSim Mex object...");
      }
  }
if (arg_num != oper_num + 1)
  {
    mexPrintf("error in 'eval_no_paren_expr':  operator and argument mismatch \n");
    mexPrintf("   expression = %s\n",input);
    mexPrintf("   note:  if expression is blank, you probably have empty\n");
    mexPrintf("          set of parenthesis somewhere\n");
    mexPrintf("   overall expression: %s\n",par_list->overall_expr);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }


/*  evaluate expression */


/*  first, convert all arguments to numbers */

/* exp_value = convert_string("4582.0",&error_flag);
 mexPrintf("exp_value = %12.8e, error flag = %d \n",exp_value,error_flag); 
  mexPrintf("decimal = %d\n",atoi("9"));
  mexErrMsgTxt("Exiting CppSim Mex object...");
par_list->nest_length = 12;
par_list->main_par_length = 3;
par_list->main_par_val[0] = 14.2;
par_list->main_par_val[1] = 7.45;
par_list->main_par_val[2] = -193e-3;
par_list->nest_val[0] = 23.9;
par_list->nest_val[11] = -67.5;
sprintf(par_list->main_par_list[0],"Weff");
sprintf(par_list->main_par_list[1],"Leff");
sprintf(par_list->main_par_list[2],"error");  */

for (i = 0; i < arg_num; i++)
  {
   exp_value = convert_string(arg_list[i],&error_flag);
   if (error_flag == 0)
      arg_value[i] = exp_value;
   else
     {
       /* check if parameter corresponds to a previously parsed expression
	  within parenthesis  */
      if (arg_list[i][0] == '(')
	{
         j = atoi(&arg_list[i][1]);
         if (j >= par_list->nest_length)
	   {
           mexPrintf("error in 'eval_no_paren_expr':  nested_arg_list exceeded\n");
           mexPrintf("   (this error shouldn't happen:  something wrong\n");
           mexPrintf("   with the adi_to_hspice source code\n");
	   mexPrintf("   expression = %s\n",input);
 	   mexPrintf("   overall expression: %s\n",par_list->overall_expr);
           mexErrMsgTxt("Exiting CppSim Mex object...");
	   }
         arg_value[i] = par_list->nest_val[j];
	}
      /* otherwise:
         parameter must correspond to variables defined within spice netlist */
      else
	{
          found_in_list_flag = 0;
	  for (j = 0; j < par_list->main_par_length; j++)
	    {
             if (strcmp(arg_list[i],par_list->main_par_list[j]) == 0)
	       {
                found_in_list_flag = 1;
                arg_value[i] = par_list->main_par_val[j];
                break;
	      }
	   }
          if (found_in_list_flag == 0)
	    {
             mexPrintf("error in 'eval_no_paren_expr':  argument not defined\n");
             mexPrintf("    arg = `%s'\n",arg_list[i]);
	     mexPrintf("    expression = %s\n",input);
	     mexPrintf("   overall expression: %s\n",par_list->overall_expr);
             mexErrMsgTxt("Exiting CppSim Mex object...");
	   }
	}
    }
 }

/*  now, perform operations and return value of expression */


/*   change sign of any arguments with - operating on them */

if (negative_found_at_begin == 1)
   arg_value[0] = -arg_value[0];
for (i = 0; i < oper_num; i++)
   if (oper_list[i] == '-')
      arg_value[i+1] = -arg_value[i+1];

/*   do multiplication and division from left to right */
for (i = 0; i < oper_num; i++)
  {
    switch(oper_list[i])
      {
      case '^':
	arg_value[i] = pow(arg_value[i],arg_value[i+1]);
        arg_value[i+1] = 1.0;
	oper_list[i] = '*';
      }
  }
for (i = 0; i < oper_num; i++)
  {
    switch(oper_list[i])
      {
      case '*':
	arg_value[i+1] *= arg_value[i];
        arg_value[i] = 0.0;
	break;
      case '/':
	arg_value[i+1] = arg_value[i] / arg_value[i+1];
        arg_value[i] = 0.0;
      }
  }

/*   add up the resulting arguments */
for (i = 0; i < oper_num; i++)
   arg_value[i+1] += arg_value[i];


return(arg_value[oper_num]);
}



double convert_string(char *input, int *error_flag)
{
int i,dec_place=0;
double string_value=0,char_value=0,power_of_ten=0,units_value;

units_value = 1.0;
*error_flag = 0;
for (i = 0; input[i] != '\0'; i++)
  {
   if ((input[i] > '9' || input[i] < '0') && input[i] != '.')
     {
       if (i > 0)
	 {
	   switch(input[i])
	     {
	     case 'E':
             case 'e':
               units_value = convert_exponent(&input[i+1],input);
               break;
	     case 'F':
	     case 'f':
	       units_value = 1.0e-15;
	       break;
	     case 'P':
	     case 'p':
	       units_value = 1.0e-12;
	       break;
	     case 'N':
	     case 'n':
	       units_value = 1.0e-9;
	       break;
	     case 'U':
	     case 'u':
	       units_value = 1.0e-6;
	       break;
	     case 'M':
	     case 'm':
	       if (input[i+1] == 'E' || input[i+1] == 'e')
		 {
		   if (input[i+2] == 'G' || input[i+2] == 'g')
		     units_value = 1.0e6;
		   else
		     {
		       mexPrintf("error in 'convert_string':  units value '%s'\n",
                             &input[i]);
		       mexPrintf("   not recognized\n");
		       mexErrMsgTxt("Exiting CppSim Mex object...");
		     }
		 }
	       else
		 units_value = 1.0e-3;
	       break;
	     case 'K':
	     case 'k':
	       units_value = 1.0e3;
	       break;
	     case 'G':
	     case 'g':
	       units_value = 1.0e9;
	       break;
	     case 'T':
	     case 't':
	       units_value = 1.0e12;
	       break;
	     default:
	       mexPrintf("error in 'convert_string':  units value '%s'\n",&input[i]);
	       mexPrintf("   not recognized\n");
	       mexErrMsgTxt("Exiting CppSim Mex object...");
	     }
	   input[i] = '\0';
	 }
       else
	 {
	   *error_flag = 1;
	   return(0.0);
	 }
       break;
     }
 }
for (dec_place = 0; input[dec_place] != '.' && input[dec_place] != '\0';
         dec_place++);



i = dec_place;
string_value = 0.0;
power_of_ten = 1.0;
while(--i >= 0)
  {
   switch(input[i])
     {
      case '0':
        char_value = 0.0;
        break;
      case '1':
        char_value = 1.0;
        break;
      case '2':
        char_value = 2.0;
        break;
      case '3':
        char_value = 3.0;
        break;
      case '4':
        char_value = 4.0;
        break;
      case '5':
        char_value = 5.0;
        break;
      case '6':
        char_value = 6.0;
        break;
      case '7':
        char_value = 7.0;
        break;
      case '8':
        char_value = 8.0;
        break;
      case '9':
        char_value = 9.0;
      }
     string_value += char_value*power_of_ten;
     power_of_ten *= 10.0;
 }

if (input[dec_place] == '.')
  {
    power_of_ten = 1/10.0;
    for (i = dec_place + 1; input[i] != '\0'; i++)
      {
	switch(input[i])
	  {
	  case '0':
	    char_value = 0.0;
	    break;
	  case '1':
	    char_value = 1.0;
	    break;
	  case '2':
	    char_value = 2.0;
	    break;
	  case '3':
	    char_value = 3.0;
	    break;
	  case '4':
	    char_value = 4.0;
	    break;
	  case '5':
	    char_value = 5.0;
	    break;
	  case '6':
	    char_value = 6.0;
	    break;
	  case '7':
	    char_value = 7.0;
	    break;
	  case '8':
	    char_value = 8.0;
	    break;
	  case '9':
	    char_value = 9.0;
	  }
	string_value += char_value*power_of_ten;
	power_of_ten /= 10.0;
      }
  }
string_value *= units_value;

return(string_value);
}

/* note:  a negative sign in front of exponent is assumed to
          have been replaced with '!' in order to simplify parsing
          in other programs */
double convert_exponent(char *input, char *full_string)
{
int i;
int string_value,char_value=0,power_of_ten,sign;
double output;

sign = 1;
for (i = 0; input[i] != '\0'; i++)
  {
    if (input[i] > '9' || input[i] < '0')
     {
       if (input[i] == '!' && i == 0)
	 sign = -1;
       else
	 {
	 mexPrintf("error in 'convert_exponent':  exponent term '%s'\n",input);
         mexPrintf("  within the string '%s'\n",full_string);
	 mexPrintf("   has unallowed characters\n");
	 mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
     }
  }
if (i == 0)
  {
    mexPrintf("error in 'convert_exponent':  exponent term\n");
    mexPrintf("  within the string '%s'\n",full_string);
    mexPrintf("   has no value\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }


string_value = 0;
power_of_ten = 1;
while(--i >= 0 && input[i] != '!')
  {
   switch(input[i])
     {
      case '0':
        char_value = 0;
        break;
      case '1':
        char_value = 1;
        break;
      case '2':
        char_value = 2;
        break;
      case '3':
        char_value = 3;
        break;
      case '4':
        char_value = 4;
        break;
      case '5':
        char_value = 5;
        break;
      case '6':
        char_value = 6;
        break;
      case '7':
        char_value = 7;
        break;
      case '8':
        char_value = 8;
        break;
      case '9':
        char_value = 9;
      }
     string_value += char_value*power_of_ten;
     power_of_ten *= 10;
 }

output = 1.0;
if (sign > 0)
  {
   for (i = 0; i < string_value; i++)
      output *= 10.0;
  }
else
  {
   for (i = 0; i < string_value; i++)
      output *= 0.1;
  }

return(output);
}




void list_to_vector(const List &in, const Vector &out)
{
int i;
List temp(in);
vector_struct *out_struct;

temp.reset();

out_struct = extract_vector_struct(out);
set_length(temp.length,out_struct);

for (i = 0; i < temp.length; i++)
   out_struct->elem[i] = temp.read();

temp.reset();
}

void list_to_vector(const List &in, Vector &out)
{
int i;
List temp(in);
vector_struct *out_struct;

temp.reset();

out_struct = extract_vector_struct(out);
set_length(temp.length,out_struct);

for (i = 0; i < temp.length; i++)
   out_struct->elem[i] = temp.read();

temp.reset();
}

void list_to_vector(const List &in, vector_struct *out)
{
int i;
List temp(in);

temp.reset();

set_length(temp.length,out);

for (i = 0; i < temp.length; i++)
   out->elem[i] = temp.read();

temp.reset();
}
   

char *bit_print(unsigned long int v)
{
int i, mask = 1 << 31,j;
static char out[40];

for (i=1,j=0; i <=32; ++i)
  {
   out[j++] = ((v & mask) == 0) ? '0' : '1';
   v <<=1;
   if (i%8 == 0 && i != 32)
      out[j++] = ' ';
 }
out[j] = '\0';
return(out);
}

///////////////// Support for PDF Operations ///////////////

   
///////////////// Support for Vector Operations ///////////////
//// Real-valued vectors

Vector::Vector()
{
if ((module_name = (char *) calloc(10,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'Vector' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
sprintf(module_name,"unknown");

if ((name = (char *) calloc(10,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'Vector' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
sprintf(name,"unnamed");

vec = NULL;
init();  // creates vec and sets initialized_flag to 1
}

Vector::Vector(const char *module_name_in, const char *name_in)
{
int i;
for (i = 0; module_name_in[i] != '\0'; i++);

if ((module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'Vector' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  module_name = '%s'\n",module_name_in);
    mexPrintf("  desired module_name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(module_name,module_name_in);

for (i = 0; name_in[i] != '\0'; i++);
if ((name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'Vector' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  name = '%s'\n",name_in);
    mexPrintf("  desired name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(name,name_in);

vec = NULL;
init();  // creates vec and sets initialized_flag to 1
}

Vector::Vector(const char *module_name_in, const char *name_in, const char *null_string)
{
int i;

vec = NULL;
initialized_flag = 0;

if (strcmp(null_string,"NULL") != 0)
   {
     mexPrintf("Error:  you are using the wrong constructor!!!\n");
     mexPrintf("   ---> use either 'Vector vec1;' or 'Vector vec1(\"mod_name\",\"name\");'\n");
     mexPrintf("   in this case, you used 'Vector %s(\"%s\",\"%s\",\"%s\");'\n",
	    name_in, module_name_in,name_in,null_string);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }

for (i = 0; module_name_in[i] != '\0'; i++);
if ((module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'Vector' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  module_name = '%s'\n",module_name_in);
    mexPrintf("  desired module_name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(module_name,module_name_in);

for (i = 0; name_in[i] != '\0'; i++);
if ((name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'Vector' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  name = '%s'\n",name_in);
    mexPrintf("  desired name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(name,name_in);
}


Vector::Vector(const Vector &other)
{
int i,j;

vec = other.vec;
initialized_flag = 0;

for (j = 0; other.name[j] != '\0'; j++);
if ((name = (char *) calloc(j+45,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'Vector' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  name = '%s'\n",other.name);
    mexPrintf("  desired name length = %d\n",j+45);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
sprintf(name,"derived from: %s",other.name);

for (i = 0; other.module_name[i] != '\0'; i++);
if ((module_name = (char *) calloc(i+j+30,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'Vector' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  module_name = '%s'\n",other.module_name);
    mexPrintf("  desired module_name length = %d\n",i+j+30);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
sprintf(module_name,"origin: %s",other.module_name);

}

Vector::~Vector()
{
free(name);
free(module_name);
if (initialized_flag == 1)
   {
    if (vec->elem != NULL)
       free(vec->elem);
    free(vec);
    vec=NULL;
   }
}


void Vector::operator = (const Vector &other) const
{
if (vec != other.vec)
  {
    copy(other);
  }
}

void Vector::operator = (Vector &other) const
{
if (vec != other.vec)
  {
    copy(other);
  }
}

void Vector::replace_vec(vector_struct *in)
{
vec = in;
}

void Vector::copy(vector_struct *in) const
{
int i,length;
double *vec_elem,*in_elem;

length = in->length;
set_length(length);

vec_elem = vec->elem;
in_elem = in->elem;

for (i = 0; i < length; i++)
   vec_elem[i] = in_elem[i];
}

void Vector::copy(const Vector &other) const
{
int i,length;
double *vec_elem,*in_elem;

if (vec != other.vec)
   {
   length = other.vec->length;
   set_length(length);

   vec_elem = vec->elem;
   in_elem = other.vec->elem;

   for (i = 0; i < length; i++)
      vec_elem[i] = in_elem[i];
   }
}


void Vector::copy(Vector &other) const
{
int i,length;
double *vec_elem,*in_elem;

if (vec != other.vec)
   {
   length = other.vec->length;
   set_length(length);

   vec_elem = vec->elem;
   in_elem = other.vec->elem;

   for (i = 0; i < length; i++)
      vec_elem[i] = in_elem[i];
   }
}

void Vector::load(const char *filename) const
{
load_struct(filename,vec);
}

void Vector::save(const char *filename) const
{
save_struct(filename,vec);
}


int Vector::get_length() const
{
return(vec->length);
}

char *Vector::get_name() const
{
return(name);
}

char *Vector::get_module_name() const
{
return(module_name);
}

void Vector::set_length(int length) const
{
int i;

if (length < 0)
   {
    mexPrintf("error in 'Vector::set_length':  length must be >= 0\n");
    mexPrintf("  in this case, length = %d\n",length);
    mexPrintf("  and you ran set_length() on vector '%s'\n",name);
    mexPrintf("     (originating module of vector = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }   

if (vec->length == length)
   return;
else if (vec->freeze_length_flag == 1)
   {
    mexPrintf("error in 'Vector::set_length':  cannot change the length of a\n");
    mexPrintf("   vector declared in the 'outputs:' section of a module in the modules.par file\n");
    mexPrintf("   EXCEPT in the 'init_output_vectors:' section of the modules.par file\n");
    mexPrintf("   in this case, you tried changing the length of  vector '%s'\n",name);
    mexPrintf("     (originating module of vector = '%s')\n",module_name);
    mexPrintf("-> cur vector length = %d, desired length = %d\n",vec->length,
            length);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (vec->length != 0)
  {
   free(vec->elem);
   vec->elem = NULL;
  }

if (length > 0)
   {
   if ((vec->elem = (double *) calloc(length,sizeof(double))) == NULL)   
      {   
      mexPrintf("error in 'Vector::set_length':  calloc call failed - \n");   
      mexPrintf("  not enough memory!!\n");
      mexPrintf("  vector name = '%s', desired length = %d\n",name,length);   
      mexPrintf("     (originating module name of vector = '%s')\n",module_name);
      mexErrMsgTxt("Exiting CppSim Mex object...");   
      }
   for (i = 0; i < length; i++)
       vec->elem[i] = 0.0;
   }
vec->length = length;
}


void Vector::init()
{
if (vec != NULL)
  {
    mexPrintf("error in 'Vector::init()': you cannot initialize a vector\n");
    mexPrintf("  that has already been initialized or assigned from another vector\n");
    mexPrintf("  in this case, you ran init() on vector '%s'\n",name);
    mexPrintf("     (originating in module '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
   
if ((vec = (vector_struct *) malloc(sizeof(vector_struct))) == NULL)   
   {   
   mexPrintf("error in 'Vector::init()':  malloc call failed\n");   
   mexPrintf("out of memory!\n");
   mexPrintf("  vector name = '%s'\n",name);   
   mexPrintf("     (originating module name of vector = '%s')\n",module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");   
  }   

vec->length = 0;
vec->freeze_length_flag = 0;
vec->elem = NULL;
vec->name = name;
vec->module_name = module_name;
initialized_flag = 1;
}   

void Vector::set_elem(int index, double val) const
{
if (index < 0)
   {
    mexPrintf("error in 'Vector::set_elem':  index must be >= 0\n");
    mexPrintf("  in this case, index = %d\n",index);
    mexPrintf("  and you ran set_elem() on vector '%s'\n",name);
    mexPrintf("     (originating module of vector = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (index >= vec->length)
    {
    mexPrintf("error in 'Vector::set_elem':  index must be < length of vector\n");
    mexPrintf("  in this case, index = %d, vector length = %d\n",index,vec->length);
    mexPrintf("  and you ran set_elem() on vector '%s'\n",name);
    mexPrintf("     (originating module of vector = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

vec->elem[index] = val;
}   

void Vector::add_to_elem(int index, double val) const
{
if (index < 0)
   {
    mexPrintf("error in 'Vector::add_to_elem':  index must be >= 0\n");
    mexPrintf("  in this case, index = %d\n",index);
    mexPrintf("  and you ran add_to_elem() on vector '%s'\n",name);
    mexPrintf("     (originating module of vector = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (index >= vec->length)
    {
    mexPrintf("error in 'Vector::add_to_elem':  index must be < length of vector\n");
    mexPrintf("  in this case, index = %d, vector length = %d\n",index,vec->length);
    mexPrintf("  and you ran add_to_elem() on vector '%s'\n",name);
    mexPrintf("     (originating module of vector = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

vec->elem[index] += val;
}   

double Vector::get_elem(int index) const
{
if (index < 0)
   {
    mexPrintf("error in 'Vector::get_elem':  index must be >= 0\n");
    mexPrintf("  in this case, index = %d\n",index);
    mexPrintf("  and you ran get_elem() on vector '%s'\n",name);
    mexPrintf("     (originating module of vector = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (index >= vec->length)
    {
    mexPrintf("error in 'Vector::get_elem':  index must be < length of vector\n");
    mexPrintf("  in this case, index = %d, vector length = %d\n",index,vec->length);
    mexPrintf("  and you ran get_elem() on vector '%s'\n",name);
    mexPrintf("     (originating module of vector = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

return(vec->elem[index]);
}   


//// Integer-valued vectors

IntVector::IntVector()
{
if ((module_name = (char *) calloc(10,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'IntVector' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
sprintf(module_name,"unknown");

if ((name = (char *) calloc(10,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'IntVector' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
sprintf(name,"unnamed");

vec = NULL;
init();  // creates vec and sets initialized_flag to 1
}

IntVector::IntVector(const char *module_name_in, const char *name_in)
{
int i;
for (i = 0; module_name_in[i] != '\0'; i++);

if ((module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'IntVector' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  module_name = '%s'\n",module_name_in);
    mexPrintf("  desired module_name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(module_name,module_name_in);

for (i = 0; name_in[i] != '\0'; i++);
if ((name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'IntVector' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  name = '%s'\n",name_in);
    mexPrintf("  desired name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(name,name_in);

vec = NULL;
init();  // creates vec and sets initialized_flag to 1
}

IntVector::IntVector(const char *module_name_in, const char *name_in, const char *null_string)
{
int i;

vec = NULL;
initialized_flag = 0;

if (strcmp(null_string,"NULL") != 0)
   {
     mexPrintf("Error:  you are using the wrong constructor!!!\n");
     mexPrintf("   ---> use either 'IntVector vec1;' or 'IntVector vec1(\"mod_name\",\"name\");'\n");
     mexPrintf("   in this case, you used 'IntVector %s(\"%s\",\"%s\",\"%s\");'\n",
	    name_in, module_name_in,name_in,null_string);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }

for (i = 0; module_name_in[i] != '\0'; i++);
if ((module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'IntVector' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  module_name = '%s'\n",module_name_in);
    mexPrintf("  desired module_name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(module_name,module_name_in);

for (i = 0; name_in[i] != '\0'; i++);
if ((name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'IntVector' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  name = '%s'\n",name_in);
    mexPrintf("  desired name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(name,name_in);
}


IntVector::IntVector(const IntVector &other)
{
int i,j;

vec = other.vec;
initialized_flag = 0;

for (j = 0; other.name[j] != '\0'; j++);
if ((name = (char *) calloc(j+45,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'IntVector' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  name = '%s'\n",other.name);
    mexPrintf("  desired name length = %d\n",j+45);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
sprintf(name,"derived from: %s",other.name);

for (i = 0; other.module_name[i] != '\0'; i++);
if ((module_name = (char *) calloc(i+j+30,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'IntVector' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  module_name = '%s'\n",other.module_name);
    mexPrintf("  desired module_name length = %d\n",i+j+30);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
sprintf(module_name,"origin: %s",other.module_name);

}

IntVector::~IntVector()
{
free(name);
free(module_name);
if (initialized_flag == 1)
   {
    if (vec->elem != NULL)
       free(vec->elem);
    free(vec);
    vec=NULL;
   }
}


void IntVector::operator = (const IntVector &other) const
{
if (vec != other.vec)
  {
    copy(other);
  }
}

void IntVector::operator = (IntVector &other) const
{
if (vec != other.vec)
  {
    copy(other);
  }
}

void IntVector::replace_vec(int_vector_struct *in)
{
vec = in;
}

void IntVector::set_binary_values() const
{
int i, length;

length = vec->length;
for (i = 0; i < length; i++)
  {
   if (vec->elem[i] <= 0)
      vec->elem[i] = 0;
   else 
      vec->elem[i] = 1;
  }
}

void IntVector::set_decimal_value(int in) const
{
int length, i;

length = vec->length;
for (i = 0; i < length && i < 32; i++)
  vec->elem[i] = (in & 1<<i) == 0 ? 0 : 1;
}

void IntVector::set_decimal_value(int in,int max_index, int min_index) const
{
int length, i, j, max_i, min_i;

if (max_index < min_index)
  {
    max_i = min_index;
    min_i = max_index;
  }
else
  {
    max_i = max_index;
    min_i = min_index;
  }

length = vec->length;
for (i = min_i, j = 0; i < length && i <= max_i && j < 32; i++,j++)
  vec->elem[i] = (in & 1<<j) == 0 ? 0 : 1;
}


int IntVector::get_decimal_value() const
{
int length, i, dec_value;

length = vec->length;
dec_value = 0;
for (i = 0; i < length && i < 32; i++)
   dec_value += (vec->elem[i] == 0) ? 0 : 1<<i;
return(dec_value);
}

int IntVector::get_decimal_value(int max_index, int min_index) const
{
int length, i, j, dec_value, max_i, min_i;

if (max_index < min_index)
  {
    max_i = min_index;
    min_i = max_index;
  }
else
  {
    max_i = max_index;
    min_i = min_index;
  }

length = vec->length;
dec_value = 0;
for (i = min_i, j = 0; i < length && i <= max_i && j < 32; i++,j++)
   dec_value += (vec->elem[i] == 0) ? 0 : 1<<j;
return(dec_value);
}


void IntVector::copy(int_vector_struct *in) const
{
int i,length;
int *vec_elem,*in_elem;

length = in->length;
set_length(length);

vec_elem = vec->elem;
in_elem = in->elem;

for (i = 0; i < length; i++)
   vec_elem[i] = in_elem[i];
}

void IntVector::copy(const IntVector &other) const
{
int i,length;
int *vec_elem,*in_elem;

if (vec != other.vec)
  {
   length = other.vec->length;
   set_length(length);

   vec_elem = vec->elem;
   in_elem = other.vec->elem;

   for (i = 0; i < length; i++)
      vec_elem[i] = in_elem[i];
  }
}

void IntVector::copy(IntVector &other) const
{
int i,length;
int *vec_elem,*in_elem;

if (vec != other.vec)
  {
   length = other.vec->length;
   set_length(length);

   vec_elem = vec->elem;
   in_elem = other.vec->elem;

   for (i = 0; i < length; i++)
      vec_elem[i] = in_elem[i];
  }
}

void IntVector::load(const char *filename) const
{
load_struct(filename,vec);
}

void IntVector::save(const char *filename) const
{
save_struct(filename,vec);
}


int IntVector::get_length() const
{
return(vec->length);
}

char *IntVector::get_name() const
{
return(name);
}

char *IntVector::get_module_name() const
{
return(module_name);
}

void IntVector::set_length(int length) const
{
int i;

if (length < 0)
   {
    mexPrintf("error in 'IntVector::set_length':  length must be >= 0\n");
    mexPrintf("  in this case, length = %d\n",length);
    mexPrintf("  and you ran set_length() on vector '%s'\n",name);
    mexPrintf("     (originating module of vector = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }   

if (vec->length == length)
   return;
else if (vec->freeze_length_flag == 1)
   {
    mexPrintf("error in 'IntVector::set_length':  cannot change the length of a\n");
    mexPrintf("   vector declared in the 'outputs:' section of a module in the modules.par file\n");
    mexPrintf("   EXCEPT in the 'init_output_vectors:' section of the modules.par file\n");
    mexPrintf("   in this case, you tried changing the length of  vector '%s'\n",name);
    mexPrintf("     (originating module of vector = '%s')\n",module_name);
    mexPrintf("-> cur vector length = %d, desired length = %d\n",vec->length,
            length);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (vec->length != 0)
  {
   free(vec->elem);
   vec->elem = NULL;
  }
if (length > 0)
   {
   if ((vec->elem = (int *) calloc(length,sizeof(int))) == NULL)   
      {   
      mexPrintf("error in 'IntVector::set_length':  calloc call failed - \n");   
      mexPrintf("  not enough memory!!\n");
      mexPrintf("  vector name = '%s', desired length = %d\n",name,length);   
      mexPrintf("     (originating module name of vector = '%s')\n",module_name);
      mexErrMsgTxt("Exiting CppSim Mex object...");   
      }
   for (i = 0; i < length; i++)
       vec->elem[i] = 0;
   }
vec->length = length;
}


void IntVector::init()
{
if (vec != NULL)
  {
    mexPrintf("error in 'IntVector::init()': you cannot initialize a vector\n");
    mexPrintf("  that has already been initialized or assigned from another vector\n");
    mexPrintf("  in this case, you ran init() on vector '%s'\n",name);
    mexPrintf("     (originating in module '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
   
if ((vec = (int_vector_struct *) malloc(sizeof(int_vector_struct))) == NULL)   
   {   
   mexPrintf("error in 'IntVector::init()':  malloc call failed\n");   
   mexPrintf("out of memory!\n");
   mexPrintf("  vector name = '%s'\n",name);   
   mexPrintf("     (originating module name of vector = '%s')\n",module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");   
  }   

vec->length = 0;
vec->freeze_length_flag = 0;
vec->elem = NULL;
vec->name = name;
vec->module_name = module_name;
initialized_flag = 1;
}   

void IntVector::set_elem(int index, int val) const
{

if (index < 0)
   {
    mexPrintf("error in 'IntVector::set_elem':  index must be >= 0\n");
    mexPrintf("  in this case, index = %d\n",index);
    mexPrintf("  and you ran set_elem() on vector '%s'\n",name);
    mexPrintf("     (originating module of vector = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (index >= vec->length)
    {
    mexPrintf("error in 'IntVector::set_elem':  index must be < length of vector\n");
    mexPrintf("  in this case, index = %d, vector length = %d\n",index,vec->length);
    mexPrintf("  and you ran set_elem() on vector '%s'\n",name);
    mexPrintf("     (originating module of vector = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

vec->elem[index] = val;
}   

void IntVector::add_to_elem(int index, int val) const
{

if (index < 0)
   {
    mexPrintf("error in 'IntVector::add_to_elem':  index must be >= 0\n");
    mexPrintf("  in this case, index = %d\n",index);
    mexPrintf("  and you ran add_to_elem() on vector '%s'\n",name);
    mexPrintf("     (originating module of vector = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (index >= vec->length)
    {
    mexPrintf("error in 'IntVector::add_to_elem':  index must be < length of vector\n");
    mexPrintf("  in this case, index = %d, vector length = %d\n",index,vec->length);
    mexPrintf("  and you ran add_to_elem() on vector '%s'\n",name);
    mexPrintf("     (originating module of vector = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

vec->elem[index] += val;
}   

int IntVector::get_elem(int index) const
{
if (index < 0)
   {
    mexPrintf("error in 'IntVector::get_elem':  index must be >= 0\n");
    mexPrintf("  in this case, index = %d\n",index);
    mexPrintf("  and you ran get_elem() on vector '%s'\n",name);
    mexPrintf("     (originating module of vector = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (index >= vec->length)
    {
    mexPrintf("error in 'IntVector::get_elem':  index must be < length of vector\n");
    mexPrintf("  in this case, index = %d, vector length = %d\n",index,vec->length);
    mexPrintf("  and you ran get_elem() on vector '%s'\n",name);
    mexPrintf("     (originating module of vector = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

return(vec->elem[index]);
}   



////// Vector Class Functions

void copy(const Vector &from, const Vector &to)
{
vector_struct *x,*y;
x = extract_vector_struct(from);
y = extract_vector_struct(to);

copy(x,y);
}

void copy(const List &from, const Vector &to)
{
int i;
List temp(from);
temp.reset();

vector_struct *y;
y = extract_vector_struct(to);

set_length(temp.length,y);

for (i = 0; i < y->length; i++)
   y->elem[i] = temp.read();
}

void copy(const Vector &from, const List &to)
{
  mexPrintf("error using 'copy':  you cannot use this function to copy\n");
  mexPrintf("   from a vector to a list\n");
  mexPrintf("   -->  use 'list1.copy(vector1);' to accomplish this\n");
  mexPrintf("   this error occurred while trying to copy vector '%s' to a list\n",
	 from.name);
  mexPrintf("       (vector '%s' was created in module '%s')\n",from.name,
	 from.module_name);
  mexErrMsgTxt("Exiting CppSim Mex object...");
}


void copy(const IntVector &from, const IntVector &to)
{
int_vector_struct *x,*y;
x = extract_vector_struct(from);
y = extract_vector_struct(to);

copy(x,y);
}

void copy(const List &from, const IntVector &to)
{
int i;
List temp(from);
temp.reset();

int_vector_struct *y;
y = extract_vector_struct(to);

set_length(temp.length,y);

for (i = 0; i < y->length; i++)
   y->elem[i] = (int) floor(temp.read()+0.5);
}

void copy(const IntVector &from, const List &to)
{
  mexPrintf("error using 'copy':  you cannot use this function to copy\n");
  mexPrintf("   from a vector to a list\n");
  mexPrintf("   -->  use 'list1.copy(vector1);' to accomplish this\n");
  mexPrintf("   this error occurred while trying to copy vector '%s' to a list\n",
	 from.name);
  mexPrintf("       (vector '%s' was created in module '%s')\n",from.name,
	 from.module_name);
  mexErrMsgTxt("Exiting CppSim Mex object...");
}


void print(const Vector &in)
{
vector_struct *x;
x = extract_vector_struct(in);

print(x);
}

void print(const IntVector &in)
{
int_vector_struct *x;
x = extract_vector_struct(in);

print(x);
}

void save(const char *filename, const Vector &in)
{
vector_struct *x;
x = extract_vector_struct(in);

save(filename,x);
}

void save(const char *filename, const IntVector &in)
{
int_vector_struct *x;
x = extract_vector_struct(in);

save(filename,x);
}


void load(const char *filename, const Vector &in)
{
vector_struct *x;
x = extract_vector_struct(in);

load(filename,x);
}


void load(const char *filename, const IntVector &in)
{
int_vector_struct *x;
x = extract_vector_struct(in);

load(filename,x);
}


void ifft(const Vector &DATA_REAL, const Vector &DATA_IMAG, const Vector &FFT_REAL, const Vector &FFT_IMAG)  
{
vector_struct *d_r,*d_i,*f_r,*f_i;
d_r = extract_vector_struct(DATA_REAL);
d_i = extract_vector_struct(DATA_IMAG);
f_r = extract_vector_struct(FFT_REAL);
f_i = extract_vector_struct(FFT_IMAG);

ifft(d_r,d_i,f_r,f_i);
}

void fft(const Vector &DATA_REAL, const Vector &DATA_IMAG, const Vector &FFT_REAL, const Vector &FFT_IMAG)
{
vector_struct *d_r,*d_i,*f_r,*f_i;
d_r = extract_vector_struct(DATA_REAL);
d_i = extract_vector_struct(DATA_IMAG);
f_r = extract_vector_struct(FFT_REAL);
f_i = extract_vector_struct(FFT_IMAG);

fft(d_r,d_i,f_r,f_i);
}


void real_fft(const Vector &DATA, const Vector &REAL, const Vector &IMAG)
{
vector_struct *d,*f_r,*f_i;
d = extract_vector_struct(DATA);
f_r = extract_vector_struct(REAL);
f_i = extract_vector_struct(IMAG);

real_fft(d,f_r,f_i);
}

void real_to_int(const Vector &in, const IntVector &out)
{
vector_struct *x;
int_vector_struct *y;
x = extract_vector_struct(in);
y = extract_vector_struct(out);

real_to_int(x,y);
}

void int_to_real(const IntVector &in, const Vector &out)
{
int_vector_struct *x;
vector_struct *y;
x = extract_vector_struct(in);
y = extract_vector_struct(out);

int_to_real(x,y);
}

void real_fft(const IntVector &data, const Vector &real, const Vector &imag)
{
int_vector_struct *d;
vector_struct *f_r,*f_i;
d = extract_vector_struct(data);
f_r = extract_vector_struct(real);
f_i = extract_vector_struct(imag);

real_fft(d,f_r,f_i);
}

void fft(const IntVector &real, const IntVector &imag, const Vector &fft_real, const Vector &fft_imag)
{
int_vector_struct *d_r,*d_i;
vector_struct *f_r,*f_i;
d_r = extract_vector_struct(real);
d_i = extract_vector_struct(imag);
f_r = extract_vector_struct(fft_real);
f_i = extract_vector_struct(fft_imag);

fft(d_r,d_i,f_r,f_i);
}

void ifft(const IntVector &real, const IntVector &imag, const Vector &out_real, const Vector &out_imag)
{
int_vector_struct *d_r,*d_i;
vector_struct *o_r,*o_i;
d_r = extract_vector_struct(real);
d_i = extract_vector_struct(imag);
o_r = extract_vector_struct(out_real);
o_i = extract_vector_struct(out_imag);

ifft(d_r,d_i,o_r,o_i);
}

void add(const Vector &a, const Vector &b, const Vector &y)
{
vector_struct *x1,*x2,*z;

x1 = extract_vector_struct(a);
x2 = extract_vector_struct(b);
z = extract_vector_struct(y);

add(x1,x2,z);
}

void add(const IntVector &a, const IntVector &b, const IntVector &y)
{
int_vector_struct *x1,*x2,*z;

x1 = extract_vector_struct(a);
x2 = extract_vector_struct(b);
z = extract_vector_struct(y);

add(x1,x2,z);
}

void add(double a, const Vector &b, const Vector &y)
{
vector_struct *x2,*z;

x2 = extract_vector_struct(b);
z = extract_vector_struct(y);

add(a,x2,z);
}

void add(int a, const IntVector &b, const IntVector &y)
{
int_vector_struct *x2,*z;

x2 = extract_vector_struct(b);
z = extract_vector_struct(y);

add(a,x2,z);
}

void add(const Vector &a, double b, const Vector &y)
{
vector_struct *x1,*z;

x1 = extract_vector_struct(a);
z = extract_vector_struct(y);

add(x1,b,z);
}

void add(const IntVector &a, int b, const IntVector &y)
{
int_vector_struct *x1,*z;

x1 = extract_vector_struct(a);
z = extract_vector_struct(y);

add(x1,b,z);
}


void sub(const Vector &a, const Vector &b, const Vector &y)
{
vector_struct *x1,*x2,*z;

x1 = extract_vector_struct(a);
x2 = extract_vector_struct(b);
z = extract_vector_struct(y);

sub(x1,x2,z);
}

void sub(const IntVector &a, const IntVector &b, const IntVector &y)
{
int_vector_struct *x1,*x2,*z;

x1 = extract_vector_struct(a);
x2 = extract_vector_struct(b);
z = extract_vector_struct(y);

sub(x1,x2,z);
}

void sub(double a, const Vector &b, const Vector &y)
{
vector_struct *x2,*z;

x2 = extract_vector_struct(b);
z = extract_vector_struct(y);

sub(a,x2,z);
}

void sub(int a, const IntVector &b, const IntVector &y)
{
int_vector_struct *x2,*z;

x2 = extract_vector_struct(b);
z = extract_vector_struct(y);

sub(a,x2,z);
}

void sub(const Vector &a, double b, const Vector &y)
{
vector_struct *x1,*z;

x1 = extract_vector_struct(a);
z = extract_vector_struct(y);

sub(x1,b,z);
}

void sub(const IntVector &a, int b, const IntVector &y)
{
int_vector_struct *x1,*z;

x1 = extract_vector_struct(a);
z = extract_vector_struct(y);

sub(x1,b,z);
}


void mul_elem(const Vector &a, const Vector &b, const Vector &y)
{
vector_struct *x1,*x2,*z;

x1 = extract_vector_struct(a);
x2 = extract_vector_struct(b);
z = extract_vector_struct(y);

mul_elem(x1,x2,z);
}

void mul_elem(const IntVector &a, const IntVector &b, const IntVector &y)
{
int_vector_struct *x1,*x2,*z;

x1 = extract_vector_struct(a);
x2 = extract_vector_struct(b);
z = extract_vector_struct(y);

mul_elem(x1,x2,z);
}

void mul_elem(double a, const Vector &b, const Vector &y)
{
vector_struct *x2,*z;

x2 = extract_vector_struct(b);
z = extract_vector_struct(y);

mul_elem(a,x2,z);
}

void mul_elem(int a, const IntVector &b, const IntVector &y)
{
int_vector_struct *x2,*z;

x2 = extract_vector_struct(b);
z = extract_vector_struct(y);

mul_elem(a,x2,z);
}

void mul_elem(const Vector &a, double b, const Vector &y)
{
vector_struct *x1,*z;

x1 = extract_vector_struct(a);
z = extract_vector_struct(y);

mul_elem(x1,b,z);
}

void mul_elem(const IntVector &a, int b, const IntVector &y)
{
int_vector_struct *x1,*z;

x1 = extract_vector_struct(a);
z = extract_vector_struct(y);

mul_elem(x1,b,z);
}

void div_elem(const Vector &a, const Vector &b, const Vector &y)
{
vector_struct *x1,*x2,*z;

x1 = extract_vector_struct(a);
x2 = extract_vector_struct(b);
z = extract_vector_struct(y);

div_elem(x1,x2,z);
}

void div_elem(const IntVector &a, const IntVector &b, const IntVector &y)
{
int_vector_struct *x1,*x2,*z;

x1 = extract_vector_struct(a);
x2 = extract_vector_struct(b);
z = extract_vector_struct(y);

div_elem(x1,x2,z);
}

void div_elem(double a, const Vector &b, const Vector &y)
{
vector_struct *x2,*z;

x2 = extract_vector_struct(b);
z = extract_vector_struct(y);

div_elem(a,x2,z);
}

void div_elem(int a, const IntVector &b, const IntVector &y)
{
int_vector_struct *x2,*z;

x2 = extract_vector_struct(b);
z = extract_vector_struct(y);

div_elem(a,x2,z);
}

void div_elem(const Vector &a, double b, const Vector &y)
{
vector_struct *x1,*z;

x1 = extract_vector_struct(a);
z = extract_vector_struct(y);

div_elem(x1,b,z);
}

void div_elem(const IntVector &a, int b, const IntVector &y)
{
int_vector_struct *x1,*z;

x1 = extract_vector_struct(a);
z = extract_vector_struct(y);

div_elem(x1,b,z);
}




void gauss_ran_vector(double std_dev, int length, const Vector &A)
{
vector_struct *y;

y = extract_vector_struct(A);

gauss_ran_vector(std_dev,length,y);
}

void sinc_vector(double step_val, int length, const Vector &A)
{
vector_struct *y;

y = extract_vector_struct(A);

sinc_vector(step_val,length,y);
}


double get_area(const Vector &A)
{
vector_struct *x;

x = extract_vector_struct(A);

return(get_area(x));
}

double get_mean(const Vector &A)
{
vector_struct *x;

x = extract_vector_struct(A);

return(get_mean(x));
}

double get_var(const Vector &A)
{
vector_struct *x;

x = extract_vector_struct(A);

return(get_var(x));
}

double inner_product(const Vector &A, const Vector &B)
{
vector_struct *x,*y;

x = extract_vector_struct(A);
y = extract_vector_struct(B);

return(inner_product(x,y));
}

int inner_product(const IntVector &A, const IntVector &B)
{
int_vector_struct *x,*y;

x = extract_vector_struct(A);
y = extract_vector_struct(B);

return(inner_product(x,y));
}

///////////////// Support for Vector Structure Operations ///////////////

double inner_product(vector_struct *A, vector_struct *B)
{
int k, span;
double *A_elem,*B_elem;
double temp;

if (A->length != B->length)
   {
     mexPrintf("error in 'inner_product(A,B)':  A->length must equal B->length\n");
     mexPrintf("   in this case, A->length = %d, B->length = %d\n",
            A->length,B->length);
     mexPrintf("   vector A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   vector B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }

span = A->length;

A_elem = A->elem;
B_elem = B->elem;

temp = 0.0;
for (k = 0; k < span; k++)
   temp += A_elem[k]*B_elem[k];

return(temp);
}

int inner_product(int_vector_struct *A, int_vector_struct *B)
{
int k, span;
int *A_elem,*B_elem;
int temp;

if (A->length != B->length)
   {
     mexPrintf("error in 'inner_product(A,B)':  A->length must equal B->length\n");
     mexPrintf("   in this case, A->length = %d, B->length = %d\n",
            A->length,B->length);
     mexPrintf("   vector A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   vector B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }

span = A->length;

A_elem = A->elem;
B_elem = B->elem;

temp = 0;
for (k = 0; k < span; k++)
   temp += A_elem[k]*B_elem[k];

return(temp);
}

vector_struct *extract_vector_struct(const Vector &in)
{
if (in.vec == NULL)
    {
      mexPrintf("error in 'extract_vector_struct': vec is NULL!\n");
      mexPrintf("  this shouldn't happen in normal use of vectors\n");
      mexPrintf("  name of vector = '%s'\n",in.name);
      mexPrintf("      (originating module of vector = '%s')\n",in.module_name);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }

// transfer names for error checking

in.vec->name = in.name;
in.vec->module_name = in.module_name;
return(in.vec);
}

int_vector_struct *extract_vector_struct(const IntVector &in)
{
if (in.vec == NULL)
    {
      mexPrintf("error in 'extract_vector_struct': vec is NULL!\n");
      mexPrintf("  this shouldn't happen in normal use of vectors\n");
      mexPrintf("  name of vector = '%s'\n",in.name);
      mexPrintf("      (originating module of vector = '%s')\n",in.module_name);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }


// transfer names for error checking

in.vec->name = in.name;
in.vec->module_name = in.module_name;
return(in.vec);
}


vector_struct *init_vector_struct(const char *module_name, const char *name)   
{   
vector_struct *A;   
   
if ((A = (vector_struct *) malloc(sizeof(vector_struct))) == NULL)   
   {   
   mexPrintf("error in 'init_vector_struct':  malloc call failed\n");   
   mexPrintf("out of memory!\n");
   mexErrMsgTxt("Exiting CppSim Mex object...");   
  }   

int i;
for (i = 0; module_name[i] != '\0'; i++);
   
if ((A->module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'init_vector_struct':  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  module_name = '%s'\n",module_name);
    mexPrintf("  desired module_name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(A->module_name,module_name);

for (i = 0; name[i] != '\0'; i++);
if ((A->name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'init_vector_struct':  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  name = '%s'\n",name);
    mexPrintf("  desired name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(A->name,name);

A->length = 0;
A->freeze_length_flag = 0;
A->elem = NULL;

return(A);   
}   


int_vector_struct *init_int_vector_struct(const char *module_name, const char *name)   
{   
int_vector_struct *A;   
   
if ((A = (int_vector_struct *) malloc(sizeof(int_vector_struct))) == NULL)   
   {   
   mexPrintf("error in 'init_int_vector_struct':  malloc call failed\n");   
   mexPrintf("out of memory!\n");
   mexErrMsgTxt("Exiting CppSim Mex object...");   
  }

int i;
for (i = 0; module_name[i] != '\0'; i++);
   
if ((A->module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'init_int_vector_struct':  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  module_name = '%s'\n",module_name);
    mexPrintf("  desired module_name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(A->module_name,module_name);

for (i = 0; name[i] != '\0'; i++);
if ((A->name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'init_int_vector_struct':  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  name = '%s'\n",name);
    mexPrintf("  desired name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(A->name,name);

A->length = 0;
A->freeze_length_flag = 0;
A->elem = NULL;

return(A);   
}   


void free_vector_struct(vector_struct *x)
{
if (x->elem != NULL)
   free(x->elem);
free(x->name);
free(x->module_name);
free(x);
}

void free_vector_struct(int_vector_struct *x)
{
if (x->elem != NULL)
   free(x->elem);
free(x->name);
free(x->module_name);
free(x);
}

void copy(vector_struct *x, vector_struct *y)
{
int i;

set_length(x->length,y);
for (i = 0; i < y->length; i++)
   y->elem[i] = x->elem[i];
}

void copy(int_vector_struct *x, int_vector_struct *y)
{
int i;

set_length(x->length,y);
for (i = 0; i < y->length; i++)
   y->elem[i] = x->elem[i];
}

void print(vector_struct *x)
{
int i;

mexPrintf("vector name = '%s' (originated in module '%s')\n",
	x->name, x->module_name);

for (i = 0; i < x->length; i++)
   mexPrintf("  %s[%d] = %5.3e\n",x->name,i,x->elem[i]);
}

void print(int_vector_struct *x)
{
int i;

mexPrintf("int_vector name = '%s' (originated in module '%s')\n",
	x->name, x->module_name);

for (i = 0; i < x->length; i++)
   mexPrintf("  %s[%d] = %d\n",x->name,i,x->elem[i]);
}


void add(vector_struct *a, vector_struct *b, vector_struct *y)
{
int i;

if (a->length != b->length)
   {
     mexPrintf("error in 'add':  input vectors have unequal length!\n");
     mexPrintf("  input vector '%s' has length %d (originating module is '%s')\n",a->name,a->length,a->module_name);
     mexPrintf("  input vector '%s' has length %d (originating module is '%s')\n",b->name,b->length,b->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
set_length(a->length,y);

for (i = 0; i < a->length; i++)
   y->elem[i] = a->elem[i] + b->elem[i];
}

void add(int_vector_struct *a, int_vector_struct *b, int_vector_struct *y)
{
int i;

if (a->length != b->length)
   {
     mexPrintf("error in 'add':  input vectors have unequal length!\n");
     mexPrintf("  input vector '%s' has length %d (originating module is '%s')\n",a->name,a->length,a->module_name);
     mexPrintf("  input vector '%s' has length %d (originating module is '%s')\n",b->name,b->length,b->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
set_length(a->length,y);

for (i = 0; i < a->length; i++)
   y->elem[i] = a->elem[i] + b->elem[i];
}

void add(double a, vector_struct *b, vector_struct *y)
{
int i;

set_length(b->length,y);

for (i = 0; i < b->length; i++)
   y->elem[i] = a + b->elem[i];
}

void add(int a, int_vector_struct *b, int_vector_struct *y)
{
int i;

set_length(b->length,y);

for (i = 0; i < b->length; i++)
   y->elem[i] = a + b->elem[i];
}

void add(vector_struct *a, double b, vector_struct *y)
{
int i;

set_length(a->length,y);

for (i = 0; i < a->length; i++)
   y->elem[i] = a->elem[i] + b;
}

void add(int_vector_struct *a, int b, int_vector_struct *y)
{
int i;

set_length(a->length,y);

for (i = 0; i < a->length; i++)
   y->elem[i] = a->elem[i] + b;
}


void sub(vector_struct *a, vector_struct *b, vector_struct *y)
{
int i;

if (a->length != b->length)
   {
     mexPrintf("error in 'sub':  input vectors have unequal length!\n");
     mexPrintf("  input vector '%s' has length %d (originating module is '%s')\n",a->name,a->length,a->module_name);
     mexPrintf("  input vector '%s' has length %d (originating module is '%s')\n",b->name,b->length,b->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
set_length(a->length,y);

for (i = 0; i < a->length; i++)
   y->elem[i] = a->elem[i] - b->elem[i];
}

void sub(int_vector_struct *a, int_vector_struct *b, int_vector_struct *y)
{
int i;

if (a->length != b->length)
   {
     mexPrintf("error in 'sub':  input vectors have unequal length!\n");
     mexPrintf("  input vector '%s' has length %d (originating module is '%s')\n",a->name,a->length,a->module_name);
     mexPrintf("  input vector '%s' has length %d (originating module is '%s')\n",b->name,b->length,b->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
set_length(a->length,y);

for (i = 0; i < a->length; i++)
   y->elem[i] = a->elem[i] - b->elem[i];
}

void sub(double a, vector_struct *b, vector_struct *y)
{
int i;

set_length(b->length,y);

for (i = 0; i < b->length; i++)
   y->elem[i] = a - b->elem[i];
}

void sub(int a, int_vector_struct *b, int_vector_struct *y)
{
int i;

set_length(b->length,y);

for (i = 0; i < b->length; i++)
   y->elem[i] = a - b->elem[i];
}

void sub(vector_struct *a, double b, vector_struct *y)
{
int i;

set_length(a->length,y);

for (i = 0; i < a->length; i++)
   y->elem[i] = a->elem[i] - b;
}

void sub(int_vector_struct *a, int b, int_vector_struct *y)
{
int i;

set_length(a->length,y);

for (i = 0; i < a->length; i++)
   y->elem[i] = a->elem[i] - b;
}



void mul_elem(vector_struct *a, vector_struct *b, vector_struct *y)
{
int i;

if (a->length != b->length)
   {
     mexPrintf("error in 'mul_elem':  input vectors have unequal length!\n");
     mexPrintf("  input vector '%s' has length %d (originating module is '%s')\n",a->name,a->length,a->module_name);
     mexPrintf("  input vector '%s' has length %d (originating module is '%s')\n",b->name,b->length,b->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
set_length(a->length,y);

for (i = 0; i < a->length; i++)
   y->elem[i] = a->elem[i]*b->elem[i];
}

void mul_elem(int_vector_struct *a, int_vector_struct *b, int_vector_struct *y)
{
int i;

if (a->length != b->length)
   {
     mexPrintf("error in 'mul_elem':  input vectors have unequal length!\n");
     mexPrintf("  input vector '%s' has length %d (originating module is '%s')\n",a->name,a->length,a->module_name);
     mexPrintf("  input vector '%s' has length %d (originating module is '%s')\n",b->name,b->length,b->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
set_length(a->length,y);

for (i = 0; i < a->length; i++)
   y->elem[i] = a->elem[i]*b->elem[i];
}

void mul_elem(double a, vector_struct *b, vector_struct *y)
{
int i;

set_length(b->length,y);

for (i = 0; i < b->length; i++)
   y->elem[i] = a*b->elem[i];
}

void mul_elem(int a, int_vector_struct *b, int_vector_struct *y)
{
int i;

set_length(b->length,y);

for (i = 0; i < b->length; i++)
   y->elem[i] = a*b->elem[i];
}

void mul_elem(vector_struct *a, double b, vector_struct *y)
{
int i;

set_length(a->length,y);

for (i = 0; i < a->length; i++)
   y->elem[i] = b*a->elem[i];
}

void mul_elem(int_vector_struct *a, int b, int_vector_struct *y)
{
int i;

set_length(a->length,y);

for (i = 0; i < a->length; i++)
   y->elem[i] = b*a->elem[i];
}

void div_elem(vector_struct *a, vector_struct *b, vector_struct *y)
{
int i;

if (a->length != b->length)
   {
     mexPrintf("error in 'div_elem':  input vectors have unequal length!\n");
     mexPrintf("  input vector '%s' has length %d (originating module is '%s')\n",a->name,a->length,a->module_name);
     mexPrintf("  input vector '%s' has length %d (originating module is '%s')\n",b->name,b->length,b->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
set_length(a->length,y);

for (i = 0; i < a->length; i++)
  {
    //  note 1e30 is close to the limit of floating point precision
    //    (which is the export resolution of the probe statement)
   if (fabs(b->elem[i]) < MIN_DIVIDE_VAL)
      y->elem[i] = MIN_DIVIDE_VAL_RESULT;
   else
      y->elem[i] = a->elem[i]/b->elem[i];
  }
}

void div_elem(int_vector_struct *a, int_vector_struct *b, int_vector_struct *y)
{
int i;

if (a->length != b->length)
   {
     mexPrintf("error in 'div_elem':  input vectors have unequal length!\n");
     mexPrintf("  input vector '%s' has length %d (originating module is '%s')\n",a->name,a->length,a->module_name);
     mexPrintf("  input vector '%s' has length %d (originating module is '%s')\n",b->name,b->length,b->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
set_length(a->length,y);

for (i = 0; i < a->length; i++)
  {
   if (b->elem[i] == 0)
      y->elem[i] = 1<<31;
   else
      y->elem[i] = a->elem[i]/b->elem[i];
  }
}

void div_elem(double a, vector_struct *b, vector_struct *y)
{
int i;

set_length(b->length,y);

for (i = 0; i < b->length; i++)
  {
    //  note 1e30 is close to the limit of floating point precision
    //    (which is the export resolution of the probe statement)
   if (fabs(b->elem[i]) < MIN_DIVIDE_VAL)
      y->elem[i] = MIN_DIVIDE_VAL_RESULT;
   else
      y->elem[i] = a/b->elem[i];
  }
}

void div_elem(int a, int_vector_struct *b, int_vector_struct *y)
{
int i;

set_length(b->length,y);

for (i = 0; i < b->length; i++)
  {
   if (b->elem[i] == 0)
      y->elem[i] = 1<<31;
   else
      y->elem[i] = a/b->elem[i];
  }
}

void div_elem(vector_struct *a, double b, vector_struct *y)
{
int i;

set_length(a->length,y);

for (i = 0; i < a->length; i++)
  {
    //  note 1e30 is close to the limit of floating point precision
    //    (which is the export resolution of the probe statement)
   if (fabs(b) < MIN_DIVIDE_VAL)
      y->elem[i] = MIN_DIVIDE_VAL_RESULT;
   else
      y->elem[i] = a->elem[i]/b;
  }
}

void div_elem(int_vector_struct *a, int b, int_vector_struct *y)
{
int i;

set_length(a->length,y);

for (i = 0; i < a->length; i++)
  {
   if (b == 0)
      y->elem[i] = 1<<31;
   else
      y->elem[i] = a->elem[i]/b;
  }
}


void sinc_vector(double step_val, int length, vector_struct *A)
{   
int i;
double cur_val, dlength;
   
if (length < 1)
  {   
    mexPrintf("error in 'sinc_vector':  length must be > 0\n");
    mexPrintf("  in this case, length = %d\n",length);
    mexPrintf("  for vector '%s' (originating module for vector = '%s')\n",
	   A->name,A->module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }

dlength = (double) length;

set_length(length,A);   

cur_val = -step_val*(dlength-1.0)/2.0;
for (i = 0; i < length; i++)
  {
   if (fabs(cur_val) < 1e-6)
       A->elem[i] = 1.0;
   else
       A->elem[i] = sin(pi*cur_val)/(pi*cur_val);
   cur_val += step_val;
  }
}   


void gauss_ran_vector(double std_dev, int length, vector_struct *A)
{   
int i;
Rand rand1("gauss");   
   
if (length < 1)
  {   
    mexPrintf("error in 'gauss_ran_vector':  length must be > 0\n");
    mexPrintf("  in this case, length = %d\n",length);
    mexPrintf("  for vector '%s' (originating module for vector = '%s')\n",
	   A->name,A->module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }

set_length(length,A);   

for (i = 0; i < length; i++)   
   A->elem[i] = std_dev*rand1.inp();
}   

double get_mean(vector_struct *A)
{
int i,length;
double total;

if (A->length == 0)
  {
   mexPrintf("error in get_mean:  vector has zero length\n");
    mexPrintf("  input vector = '%s' (originating module for vector = '%s'\n",
	   A->name,A->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

total = 0.0;
length = A->length;

for (i = 0; i < length; i++)
   total += A->elem[i];

return(total/((double) length));
}

double get_var(vector_struct *A)
{
int i, length;
double total,val,bias,meanval,rlength;

if (A->length == 0)
  {
   mexPrintf("error in get_var:  vector has zero length\n");
    mexPrintf("  input vector = '%s' (originating module for vector = '%s'\n",
	   A->name,A->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

total = 0.0;
length = A->length;
 
for (i = 0; i < length; i++)
  {
   val = A->elem[i];
   total += val*val;
  }
rlength = (double) length;
if (length == 1)
   bias = 0.0;
else
   bias = rlength/(rlength-1.0);
meanval = get_mean(A);

return(bias*((total/rlength) - meanval*meanval));
}

double get_area(vector_struct *A)
{   
int i,length;
double area;

area = 0.0;
length = A->length;

for (i = 0; i < length; i++)   
   area += A->elem[i];

return(area);
}   


void set_length(int length, vector_struct *A)   
{      

if (length < 0)
   {
    mexPrintf("error in 'set_length':  length must be > 0\n");
    mexPrintf("  in this case, length = %d\n",length);
    mexPrintf("  for vector '%s'\n",A->name);
    mexPrintf("  originating module name of vector = '%s'\n",A->module_name);   
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }   

if (A->length == length)
   return;
else if (A->freeze_length_flag == 1)
   {
    mexPrintf("error in 'set_length()':  cannot change the length of a\n");
    mexPrintf("   vector declared in the 'outputs:' section of a module in the modules.par file\n");
    mexPrintf("   EXCEPT in the 'init_output_vectors:' section of the modules.par file\n");
    mexPrintf("   in this case, you tried changing the length of  vector '%s'\n",A->name);
    mexPrintf("     (originating module of vector = '%s')\n",A->module_name);
    mexPrintf("-> cur vector length = %d, desired length = %d\n",A->length,
            length);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (A->length != 0)
  {
   free(A->elem);
   A->elem = NULL;
  }

if (length > 0)
   {
   if ((A->elem = (double *) calloc(length,sizeof(double))) == NULL)   
      {   
      mexPrintf("error in 'set_length':  calloc call failed - \n");   
      mexPrintf("  not enough memory!!\n");
      mexPrintf("  vector name = '%s', desired length = %d\n",A->name,length);   
      mexPrintf("  originating module name of vector = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");   
      }
   }
A->length = length;
}

void set_length(int length, int_vector_struct *A)   
{

if (length < 0)
   {
    mexPrintf("error in 'set_length':  length must be > 0\n");
    mexPrintf("  in this case, length = %d\n",length);
    mexPrintf("  for vector '%s'\n",A->name);
    mexPrintf("  originating module name of vector = '%s'\n",A->module_name);   
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }   

if (A->length == length)
   return;
else if (A->freeze_length_flag == 1)
   {
    mexPrintf("error in 'set_length()':  cannot change the length of a\n");
    mexPrintf("   vector declared in the 'outputs:' section of a module in the modules.par file\n");
    mexPrintf("   EXCEPT in the 'init_output_vectors:' section of the modules.par file\n");
    mexPrintf("   in this case, you tried changing the length of  vector '%s'\n",A->name);
    mexPrintf("     (originating module of vector = '%s')\n",A->module_name);
    mexPrintf("-> cur vector length = %d, desired length = %d\n",A->length,
            length);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (A->length != 0)
  {
   free(A->elem);
   A->elem = NULL;
  }
if (length > 0)
   {
   if ((A->elem = (int *) calloc(length,sizeof(int))) == NULL)   
      {   
      mexPrintf("error in 'set_length':  calloc call failed - \n");   
      mexPrintf("  not enough memory!!\n");
      mexPrintf("  vector name = '%s', desired length = %d\n",A->name,length);   
      mexPrintf("  originating module name of vector = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");   
      }
   }
A->length = length;
}





double read_elem(int index, vector_struct *A)
{
if (index < 0)
    {
      mexPrintf("error in 'read_elem':  index must be >= 0\n");
      mexPrintf("  in this case, for vector '%s':\n",A->name);
      mexPrintf("  -->  length = %d, index = %d\n",A->length,index);
      mexPrintf("  originating module name of vector = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
if (index >= A->length)
    {
      mexPrintf("error in 'read_elem':  index must be < length of vector_struct\n");
      mexPrintf("  in this case, for vector '%s':\n",A->name);
      mexPrintf("  -->  length = %d, index = %d\n",A->length,index);
      mexPrintf("  originating module name of vector = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }

return(A->elem[index]);
}   


int read_elem(int index, int_vector_struct *A)
{
if (index < 0)
    {
      mexPrintf("error in 'read_elem':  index must be >= 0\n");
      mexPrintf("  in this case, for vector '%s':\n",A->name);
      mexPrintf("  -->  length = %d, index = %d\n",A->length,index);
      mexPrintf("  originating module name of vector = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
if (index >= A->length)
    {
      mexPrintf("error in 'read_elem':  index must be < length of vector_struct\n");
      mexPrintf("  in this case, for vector '%s':\n",A->name);
      mexPrintf("  -->  length = %d, index = %d\n",A->length,index);
      mexPrintf("  originating module name of vector = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }

return(A->elem[index]);
}   



void write_elem(double val, int index, vector_struct *A)
{
if (index < 0)
    {
      mexPrintf("error in 'write_elem':  index must be >= 0\n");
      mexPrintf("  in this case, for vector '%s':\n",A->name);
      mexPrintf("  -->  length = %d, index = %d\n",A->length,index);
      mexPrintf("  originating module name of vector = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
if (index >= A->length)
    {
      mexPrintf("error in 'write_elem':  index must be < length of vector_struct\n");
      mexPrintf("  in this case, for vector '%s':\n",A->name);
      mexPrintf("  -->  length = %d, index = %d\n",A->length,index);
      mexPrintf("  originating module name of vector = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }

A->elem[index] = val;
}   


void write_elem(int val, int index, int_vector_struct *A)
{
if (index < 0)
    {
      mexPrintf("error in 'write_elem':  index must be >= 0\n");
      mexPrintf("  in this case, for vector '%s':\n",A->name);
      mexPrintf("  -->  length = %d, index = %d\n",A->length,index);
      mexPrintf("  originating module name of vector = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
if (index >= A->length)
    {
      mexPrintf("error in 'write_elem':  index must be < length of vector_struct\n");
      mexPrintf("  in this case, for vector '%s':\n",A->name);
      mexPrintf("  -->  length = %d, index = %d\n",A->length,index);
      mexPrintf("  originating module name of vector = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }

A->elem[index] = val;
}   

////////////////////////////////////////////////////////////////////////////

void list_to_vector_struct(const List &in, vector_struct *out)
{
int i;
List temp(in);

temp.reset();
set_length(temp.length,out);

for (i = 0; i < temp.length; i++)
   out->elem[i] = temp.read();

temp.reset();
}

void save(const char *filename, vector_struct *in)
{
save_struct(filename,in);
}

void save(const char *filename, int_vector_struct *in)
{
save_struct(filename,in);
}

void load(const char *filename, vector_struct *in)
{
load_struct(filename,in);
}

void load(const char *filename, int_vector_struct *in)
{
load_struct(filename,in);
}



void save_struct(const char *filename, vector_struct *in)
{
Matrix mat1("save_struct_func","mat1");
matrix_struct *mat1_mat;

mat1_mat = extract_matrix_struct(mat1);

set_size(in->length,1,mat1_mat);

copy(0,"col",in,mat1_mat);

mat1.save(filename);
}

void save_struct(const char *filename, int_vector_struct *in)
{
IntMatrix mat1("save_struct_func","mat1");
int_matrix_struct *mat1_mat;

mat1_mat = extract_matrix_struct(mat1);

set_size(in->length,1,mat1_mat);

copy(0,"col",in,mat1_mat);

mat1.save(filename);
}


void load_struct(const char *filename, vector_struct *in)
{
Matrix mat1("save_struct_func","mat1");
matrix_struct *mat1_mat;

mat1_mat = extract_matrix_struct(mat1);

mat1.load(filename);

copy(0,"col",mat1_mat,in);
}

void load_struct(const char *filename, int_vector_struct *in)
{
IntMatrix mat1("save_struct_func","mat1");
int_matrix_struct *mat1_mat;

mat1_mat = extract_matrix_struct(mat1);

mat1.load(filename);

copy(0,"col",mat1_mat,in);
}


void real_to_int(vector_struct *in, int_vector_struct *out)
{
int i;

set_length(in->length,out);

for (i = 0; i < out->length; i++)
  out->elem[i] = (int) floor(in->elem[i]+0.5);
}

void int_to_real(int_vector_struct *in, vector_struct *out)
{
int i;

set_length(in->length,out);

for (i = 0; i < out->length; i++)
  out->elem[i] = (double) in->elem[i];
}

void real_fft_vec(vector_struct *data, vector_struct *real, 
                  vector_struct *imag)
{
real_fft(data,real,imag);
}

void fft_vec(vector_struct *real, vector_struct *imag, 
         vector_struct *fft_real, vector_struct *fft_imag)
{
fft(real,imag,fft_real,fft_imag);
}

void ifft_vec(vector_struct *real, vector_struct *imag, 
          vector_struct *out_real, vector_struct *out_imag)
{
ifft(real,imag,out_real,out_imag);
}

void real_fft(int_vector_struct *data, vector_struct *real, vector_struct *imag)
{
vector_struct *data_new;

data_new = init_vector_struct("real_fft_function","data_new");

int_to_real(data,data_new);
real_fft(data_new,real,imag);

free_vector_struct(data_new);
}

void fft(int_vector_struct *real, int_vector_struct *imag, vector_struct *fft_real, vector_struct *fft_imag)
{
vector_struct *real_new,*imag_new;

real_new = init_vector_struct("fft_function","real_new");
imag_new = init_vector_struct("fft_function","imag_new");

int_to_real(real,real_new);
int_to_real(imag,imag_new);
fft(real_new,imag_new,fft_real,fft_imag);

free_vector_struct(real_new);
free_vector_struct(imag_new);
}

void ifft(int_vector_struct *real, int_vector_struct *imag, vector_struct *out_real, vector_struct *out_imag)
{
vector_struct *real_new;
vector_struct *imag_new;

real_new = init_vector_struct("fft_function","real_new");
imag_new = init_vector_struct("fft_function","imag_new");

int_to_real(real,real_new);
int_to_real(imag,imag_new);
ifft(real_new,imag_new,out_real,out_imag);

free_vector_struct(real_new);
free_vector_struct(imag_new);
}

void real_fft(vector_struct *DATA, vector_struct *REAL, vector_struct *IMAG)   
{   
	int i1,i2,i3,i4,n2p3,n;   
        int i,j;   
	double h1r,h1i,h2r,h2i;   
	double wr,wi,wpr,wpi,wtemp,wtemp2,theta,test_log2,store_log2;   
        double temp_store;   
        vector_struct *temp;
   
        if (REAL == IMAG)   
	  {   
           mexPrintf("error in 'real_fft':  REAL and IMAG vectors must be distinct\n   in this case, vector '%s' is specified for both vectors REAL and IMAG\n",REAL->name);   
           mexErrMsgTxt("Exiting CppSim Mex object...");   
	 }   
        if (DATA->length == 0)  
	  {  
           mexPrintf("error in 'real_fft':  input vector '%s' has zero length!\n",DATA->name);  
           mexErrMsgTxt("Exiting CppSim Mex object...");  
	 }  
  
        test_log2 = log((double) DATA->length/2)/log(2.0) + 0.0000001;  
        store_log2 = floor(test_log2);   
        test_log2 -= store_log2;   
        if (test_log2 > 0.0000002 )   
	  {   
          temp = init_vector_struct("real_fft_function","temp");
          i1 = (int) (2.0*pow(2.0,store_log2+1.0));
          set_length(i1,temp);
          for (i = 0; i < DATA->length; i++)   
              temp->elem[i] = DATA->elem[i];   
          for ( ; i < temp->length; i++)   
              temp->elem[i] = 0.0;   
          n = temp->length/2;
          four1(temp,REAL);
          REAL->elem--;  
  
          set_length(temp->length,IMAG);   
          free_vector_struct(temp);
	}   
        else
	  {
           n = DATA->length/2;   
	   four1(DATA,REAL);   
           REAL->elem--;  
  
           if (IMAG != DATA)   
              set_length(DATA->length,IMAG);   
	 }

  
	theta = -(ISIGN)*3.141592653589793/(double) n;   
	wtemp=sin(0.5*theta);   
	wpr = -2.0*wtemp*wtemp;   
	wpi=sin(theta);   
	wr=1.0 + wpr;   
	wi=wpi;   
	n2p3=2*n + 3;   
	for (i=2;i<=n/2;i++)    
           {   
	       i4=1+(i3=n2p3-(i2=1+(i1=i+i-1)));  
	        h1r=0.5*(REAL->elem[i1] + REAL->elem[i3]);   
		h1i=0.5*(REAL->elem[i2] - REAL->elem[i4]);   
		h2r = 0.5*(REAL->elem[i2] + REAL->elem[i4]);   
		h2i=0.5*(REAL->elem[i3] - REAL->elem[i1]);   
		REAL->elem[i1] = h1r+(wtemp2 = wr*h2r+wi*h2i);   
		REAL->elem[i2] = h1i+(wtemp = wr*h2i-wi*h2r);   
		REAL->elem[i3] = h1r-wtemp2;   
		REAL->elem[i4] = -h1i+wtemp;   
		wr=(wtemp=wr)*wpr-wi*wpi+wr;   
		wi=wi*wpr+wtemp*wpi+wi;   
	}   
  
        REAL->elem[n+2] *= ISIGN;  
	REAL->elem[1] = (h1r=REAL->elem[1]) + REAL->elem[2];   
	REAL->elem[2] = h1r-REAL->elem[2];   
  
        REAL->elem++;   
   
        IMAG->elem[0] = 0.0;   
        IMAG->elem[n] = 0.0;   
        for (i = 1; i < n; i++)   
           IMAG->elem[i] = REAL->elem[2*i+1];    
  
        temp_store = REAL->elem[1];   
        for (i = 0; i < n; i++)   
           REAL->elem[i] = REAL->elem[2*i];   
        REAL->elem[n] = temp_store;   
  
        for (i = n+1,j = n-1; i < IMAG->length; i++,j--)   
	  {  
           REAL->elem[i] = REAL->elem[j];   
           IMAG->elem[i] = -IMAG->elem[j];   
	 }  
 }   
   
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr   


void four1(vector_struct *DATA, vector_struct *FFT)   
{   
	int n,nn,mmax,m,istep;   
        int i,j;   
	double wtemp,wr,wpr,wpi,wi,theta,test_log2;   
	double tempr,tempi;   
   
        nn = DATA->length/2;   
        if (DATA->length == 0)  
	  {  
           mexPrintf("error in 'four1':  input vector '%s' has zero length!\n",DATA->name);  
           mexErrMsgTxt("Exiting CppSim Mex object...");  
	 }  
        test_log2 = log((double) nn)/log(2.0) + 0.0000001;   
        test_log2 -= floor(test_log2);   
   
        if (test_log2 > 0.0000002)   
	  {   
           mexPrintf("error in 'four1':  length of input vector must be\n");   
           mexPrintf("an integer power of 2\n  in this case, vector '%s' is length %d (times 2)\n",DATA->name,DATA->length/2);   
           mexErrMsgTxt("Exiting CppSim Mex object...");   
	 }   
        if (DATA != FFT)   
          {   
           set_length(DATA->length,FFT);   
           for (i = 0; i < DATA->length; i++)   
              FFT->elem[i] = DATA->elem[i];   
	 }   
        FFT->elem--;   
	n=nn << 1;   
	j=1;   
	for (i=1;i<n;i+=2) {   
		if (j > i) {   
			SWAP(FFT->elem[j],FFT->elem[i]);   
			SWAP(FFT->elem[j+1],FFT->elem[i+1]);   
		}   
		m=n >> 1;   
		while (m >= 2 && j > m) {   
			j -= m;   
			m >>= 1;   
		}   
		j += m;   
	}   
	mmax=2;   
	while (n > mmax) {   
		istep=2*mmax;   
		theta=6.28318530717959/(ISIGN*mmax);   
		wtemp=sin(0.5*theta);   
		wpr = -2.0*wtemp*wtemp;   
		wpi=sin(theta);   
		wr=1.0;   
		wi=0.0;   
		for (m=1;m<mmax;m+=2) {   
			for (i=m;i<=n;i+=istep) {   
				j=i+mmax;   
				tempr=wr*FFT->elem[j]-wi*FFT->elem[j+1];   
				tempi=wr*FFT->elem[j+1]+wi*FFT->elem[j];   
				FFT->elem[j]=FFT->elem[i]-tempr;   
				FFT->elem[j+1]=FFT->elem[i+1]-tempi;   
				FFT->elem[i] += tempr;   
				FFT->elem[i+1] += tempi;   
			}   
			wr=(wtemp=wr)*wpr-wi*wpi+wr;   
			wi=wi*wpr+wtemp*wpi+wi;   
		}   
		mmax=istep;   
	}   
       FFT->elem++;   
}   
   
void fft(vector_struct *DATA_REAL,vector_struct *DATA_IMAG,vector_struct *FFT_REAL,vector_struct *FFT_IMAG)   
{   
	int n,mmax,m,istep,i1,nn,length,power_of_two;   
        int i,j;   
	double wtemp,wr,wpr,wpi,wi,theta;   
        vector_struct *temp;   
	double tempr,tempi;   
   
        if (DATA_REAL == DATA_IMAG)   
	  {   
           mexPrintf("error in 'fft':  DATA_REAL and DATA_IMAG vectors must be specified\ndifferently - in this case, both are specified as vector '%s'\n",DATA_REAL->name);   
           mexErrMsgTxt("Exiting CppSim Mex object...");   
	 }   
        if (FFT_REAL == FFT_IMAG)   
	  {   
           mexPrintf("error in 'fft':  FFT_REAL and FFT_IMAG vectors must be specified\ndifferently - in this case, both are specified as vector '%s'\n",FFT_REAL->name);   
           mexErrMsgTxt("Exiting CppSim Mex object...");   
	 }   
        if (DATA_REAL->length != DATA_IMAG->length)   
	  {   
           mexPrintf("error in 'fft':  DATA_REAL and DATA_IMAG vectors must have\nthe same length\n   in this case, DATA_REAL vector '%s' has length %d\n              DATA_IMAG vector '%s' has length %d\n",DATA_REAL->name,DATA_REAL->length,DATA_IMAG->name,DATA_IMAG->length);   
           mexErrMsgTxt("Exiting CppSim Mex object...");   
	 }   
        if (DATA_REAL->length == 0)  
	  {  
           mexPrintf("error in 'fft':  input vectors '%s' and '%s' have zero length!\n",DATA_REAL->name,DATA_IMAG->name);  
           mexErrMsgTxt("Exiting CppSim Mex object...");  
	 }  
  
        temp = init_vector_struct("fft_function","temp");

        length = DATA_REAL->length;
        power_of_two = 1;
        while (power_of_two < length)
	   power_of_two = power_of_two << 1;

	i1 = 2*power_of_two;
	set_length(i1,temp);
  
        for (i = 0,j = 0; i < DATA_REAL->length && j < i1-1; i++,j += 2)   
	    {   
              temp->elem[j] = DATA_REAL->elem[i];   
              temp->elem[j+1] = DATA_IMAG->elem[i];   
	    }   
  
        for ( ; j < i1; j++)   
            temp->elem[j] = 0.0;

        temp->elem--;   
  
        nn = temp->length/2;   
   
	n=nn << 1;   
	j=1;   
	for (i=1;i<n;i+=2) {   
		if (j > i) {   
			SWAP(temp->elem[j],temp->elem[i]);   
			SWAP(temp->elem[j+1],temp->elem[i+1]);   
		}   
		m=n >> 1;   
		while (m >= 2 && j > m) {   
			j -= m;   
			m >>= 1;   
		}   
		j += m;   
	}   
  
	mmax=2;   
	while (n > mmax) {   
		istep=2*mmax;   
		theta=6.28318530717959/(ISIGN*mmax);   
		wtemp=sin(0.5*theta);   
		wpr = -2.0*wtemp*wtemp;   
		wpi=sin(theta);   
		wr=1.0;   
		wi=0.0;   
		for (m=1;m<mmax;m+=2) {   
			for (i=m;i<=n;i+=istep) {   
				j=i+mmax;   
				tempr=wr*temp->elem[j]-wi*temp->elem[j+1];   
				tempi=wr*temp->elem[j+1]+wi*temp->elem[j];   
				temp->elem[j]=temp->elem[i]-tempr;   
				temp->elem[j+1]=temp->elem[i+1]-tempi;   
				temp->elem[i] += tempr;   
				temp->elem[i+1] += tempi;   
			}   
			wr=(wtemp=wr)*wpr-wi*wpi+wr;   
			wi=wi*wpr+wtemp*wpi+wi;   
		}   
		mmax=istep;   
	}   
       temp->elem++;   
  
       set_length(temp->length/2,FFT_IMAG);
       set_length(temp->length/2,FFT_REAL);

  
       for (i = 0, j = 0; i < FFT_IMAG->length && j < temp->length-1; i++, j += 2)   
	 {   
          FFT_REAL->elem[i] = temp->elem[j];   
          FFT_IMAG->elem[i] = temp->elem[j+1];   
	}  
  
       free_vector_struct(temp);
}   
   
void ifft(vector_struct *DATA_REAL, vector_struct *DATA_IMAG, vector_struct *FFT_REAL, vector_struct *FFT_IMAG)   
{   
	int n,mmax,m,istep,i1,nn,length,power_of_two;   
        int i,j;   
	double wtemp,wr,wpr,wpi,wi,theta;
        vector_struct *temp;
	double tempr,tempi;   
   
/* note:  this program's labels are misleading - output is the real and imag */  
/* components of the inverse fft of the input data */  
  
        if (DATA_REAL == DATA_IMAG)   
	  {   
           mexPrintf("error in 'ifft':  DATA_REAL and DATA_IMAG vectors must be specified\ndifferently - in this case, both are specified as vector '%s'\n",DATA_REAL->name);   
           mexErrMsgTxt("Exiting CppSim Mex object...");   
	 }   
        if (FFT_REAL == FFT_IMAG)   
	  {   
           mexPrintf("error in 'ifft':  FFT_REAL and FFT_IMAG vectors must be specified\ndifferently - in this case, both are specified as vector '%s'\n",FFT_REAL->name);   
           mexErrMsgTxt("Exiting CppSim Mex object...");   
	 }   
        if (DATA_REAL->length != DATA_IMAG->length)   
	  {   
           mexPrintf("error in 'ifft':  DATA_REAL and DATA_IMAG vectors must have\nthe same length\n   in this case, DATA_REAL vector '%s' has length %d\n              DATA_IMAG vector '%s' has length %d\n",DATA_REAL->name,DATA_REAL->length,DATA_IMAG->name,DATA_IMAG->length);   
           mexErrMsgTxt("Exiting CppSim Mex object...");   
	 }   
         if (DATA_REAL->length == 0)  
	  {  
           mexPrintf("error in 'ifft':  input vectors '%s' and '%s' have zero length!\n",DATA_REAL->name,DATA_IMAG->name);  
           mexErrMsgTxt("Exiting CppSim Mex object...");  
	 }  
  
	temp = init_vector_struct("ifft_function","temp");

        length = DATA_REAL->length;
        power_of_two = 1;
        while (power_of_two < length)
	   power_of_two = power_of_two << 1;
   
	i1 = 2*power_of_two;
	set_length(i1,temp);
	     
        for (i = 0,j = 0; i < DATA_REAL->length && j < i1-1; i++,j += 2)   
	    {   
              temp->elem[j] = DATA_REAL->elem[i];   
              temp->elem[j+1] = DATA_IMAG->elem[i];   
	    }   
                 
        for ( ; j < i1; j++)   
              temp->elem[j] = 0.0;
        temp->elem--;   
   
        nn = temp->length/2;   
   
	n=nn << 1;   
	j=1;   
	for (i=1;i<n;i+=2) {   
		if (j > i) {   
			SWAP(temp->elem[j],temp->elem[i]);   
			SWAP(temp->elem[j+1],temp->elem[i+1]);   
		}   
		m=n >> 1;   
		while (m >= 2 && j > m) {   
			j -= m;   
			m >>= 1;   
		}   
		j += m;   
	}   
	mmax=2;   
	while (n > mmax) {   
		istep=2*mmax;   
		theta=6.28318530717959/(-(ISIGN)*mmax);   
		wtemp=sin(0.5*theta);   
		wpr = -2.0*wtemp*wtemp;   
		wpi=sin(theta);   
		wr=1.0;   
		wi=0.0;   
		for (m=1;m<mmax;m+=2) {   
			for (i=m;i<=n;i+=istep) {   
				j=i+mmax;   
				tempr=wr*temp->elem[j]-wi*temp->elem[j+1];   
				tempi=wr*temp->elem[j+1]+wi*temp->elem[j];   
				temp->elem[j]=temp->elem[i]-tempr;   
				temp->elem[j+1]=temp->elem[i+1]-tempi;   
				temp->elem[i] += tempr;   
				temp->elem[i+1] += tempi;   
			}   
			wr=(wtemp=wr)*wpr-wi*wpi+wr;   
			wi=wi*wpr+wtemp*wpi+wi;   
		}   
		mmax=istep;   
	}   
       temp->elem++;   
   
       set_length(temp->length/2,FFT_IMAG);
       set_length(temp->length/2,FFT_REAL);

       for (i = 0, j = 0; i < FFT_IMAG->length && j < temp->length-1; i++, j += 2)   
	 {   
          FFT_REAL->elem[i] = temp->elem[j]/((double) FFT_IMAG->length);   
          FFT_IMAG->elem[i] = temp->elem[j+1]/((double) FFT_IMAG->length);   
	}

       free_vector_struct(temp);   
   }   

///////////////// Support for Matrix Operations ///////////////

//////////////////// Real-valued matrices /////////////////////

Matrix::Matrix()
{
if ((module_name = (char *) calloc(10,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'Matrix' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
sprintf(module_name,"unknown");

if ((name = (char *) calloc(10,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'Matrix' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
sprintf(name,"unnamed");

mat = NULL;
init();  // creates matrix and sets initialized_flag to 1
}

Matrix::Matrix(const char *module_name_in, const char *name_in)
{
int i;
for (i = 0; module_name_in[i] != '\0'; i++);

if ((module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'Matrix' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  module_name = '%s'\n",module_name_in);
    mexPrintf("  desired module_name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(module_name,module_name_in);

for (i = 0; name_in[i] != '\0'; i++);
if ((name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'Matrix' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  name = '%s'\n",name_in);
    mexPrintf("  desired name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(name,name_in);

mat = NULL;
init();  // creates vec and sets initialized_flag to 1
}

Matrix::Matrix(const char *module_name_in, const char *name_in, const char *null_string)
{
int i;

mat = NULL;
initialized_flag = 0;

if (strcmp(null_string,"NULL") != 0)
   {
     mexPrintf("Error:  you are using the wrong constructor!!!\n");
     mexPrintf("   ---> use either 'Matrix mat1;' or 'Matrix mat1(\"mod_name\",\"name\");'\n");
     mexPrintf("   in this case, you used 'Matrix %s(\"%s\",\"%s\",\"%s\");'\n",
	    name_in, module_name_in,name_in,null_string);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }

for (i = 0; module_name_in[i] != '\0'; i++);
if ((module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'Matrix' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  module_name = '%s'\n",module_name_in);
    mexPrintf("  desired module_name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(module_name,module_name_in);

for (i = 0; name_in[i] != '\0'; i++);
if ((name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'Matrix' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  name = '%s'\n",name_in);
    mexPrintf("  desired name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(name,name_in);
}


Matrix::Matrix(const Matrix &other)
{
int i,j;

mat = other.mat;
initialized_flag = 0;

for (j = 0; other.name[j] != '\0'; j++);
if ((name = (char *) calloc(j+45,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'Matrix' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  name = '%s'\n",other.name);
    mexPrintf("  desired name length = %d\n",j+45);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
sprintf(name,"derived from: %s",other.name);

for (i = 0; other.module_name[i] != '\0'; i++);
if ((module_name = (char *) calloc(i+j+30,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'Matrix' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  module_name = '%s'\n",other.module_name);
    mexPrintf("  desired module_name length = %d\n",i+j+30);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
sprintf(module_name,"origin: %s",other.module_name);

}

Matrix::~Matrix()
{
int i;
free(name);
free(module_name);
if (initialized_flag == 1)
   {
    if (mat->elem != NULL)
      {
       for (i = 0; i < mat->rows; i++)   
           free(mat->elem[i]);   
       free(mat->elem);   
      }
    free(mat);
    mat = NULL;
   }
}


void Matrix::operator = (const Matrix &other) const
{
if (mat != other.mat)
  {
    copy(other);
  }
}

void Matrix::operator = (Matrix &other) const
{
if (mat != other.mat)
  {
    copy(other);
  }
}

void Matrix::replace_mat(matrix_struct *in)
{
mat = in;
}

void Matrix::copy(matrix_struct *in) const
{
int i,j,rows,cols;
double **mat_elem,**in_elem;

rows = in->rows;
cols = in->cols;
set_size(rows,cols);

mat_elem = mat->elem;
in_elem = in->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      mat_elem[i][j] = in_elem[i][j];
}

void Matrix::copy(const Matrix &other) const
{
int i,j,rows,cols;
double **mat_elem,**in_elem;

if (mat != other.mat)
   {
   rows = other.mat->rows;
   cols = other.mat->cols;
   set_size(rows,cols);

   mat_elem = mat->elem;
   in_elem = other.mat->elem;

   for (i = 0; i < rows; i++)
      for (j = 0; j < cols; j++)
         mat_elem[i][j] = in_elem[i][j];
   }
}

void Matrix::copy(Matrix &other) const
{
int i,j,rows,cols;
double **mat_elem,**in_elem;

if (mat != other.mat)
   {
   rows = other.mat->rows;
   cols = other.mat->cols;
   set_size(rows,cols);

   mat_elem = mat->elem;
   in_elem = other.mat->elem;

   for (i = 0; i < rows; i++)
      for (j = 0; j < cols; j++)
         mat_elem[i][j] = in_elem[i][j];
   }
}

void Matrix::copy(Matrix *in)
{
int i,j,rows,cols;
double **mat_elem,**in_elem;

if (mat != in->mat)
   {
   rows = in->mat->rows;
   cols = in->mat->cols;
   set_size(rows,cols);

   mat_elem = mat->elem;
   in_elem = in->mat->elem;

   for (i = 0; i < rows; i++)
      for (j = 0; j < cols; j++)
         mat_elem[i][j] = in_elem[i][j];
   }
}

void Matrix::load(const char *filename) const
{
load_struct(filename,mat);
}

void Matrix::save(const char *filename) const
{
save_struct(filename,mat);
}


int Matrix::get_rows() const
{
return(mat->rows);
}

int Matrix::get_cols() const
{
return(mat->cols);
}

char *Matrix::get_name() const
{
return(name);
}

char *Matrix::get_module_name() const
{
return(module_name);
}


void Matrix::set_size(int rows, int cols) const
{
int i,j;

if (mat->rows == rows && mat->cols == cols)
   return;
else if (mat->freeze_length_flag == 1)
   {
    mexPrintf("error in 'Matrix::set_size':  cannot change the size of a\n");
    mexPrintf("   matrix declared in the 'outputs:' section of a module in the modules.par file\n");
    mexPrintf("   EXCEPT in the 'init_output_vectors:' section of the modules.par file\n");
    mexPrintf("   in this case, you tried changing the size of matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexPrintf("-> cur matrix size = %d by %d, desired size = %d by %d\n",
	   mat->rows, mat->cols, rows, cols);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (rows < 0 || cols < 0)
   {
    mexPrintf("error in 'Matrix::set_size':  rows and cols must be >= 0\n");
    mexPrintf("  in this case, rows = %d, cols = %d\n",rows,cols);
    mexPrintf("  and you ran set_size() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }   

if (mat->rows != 0)
  {
    for (i = 0; i < mat->rows; i++)   
        free(mat->elem[i]);   
    free(mat->elem);
    mat->elem = NULL;  
  }

if (rows > 0 && cols > 0)
   {
   if ((mat->elem = (double **) calloc(rows,sizeof(double *))) == NULL)   
      {   
      mexPrintf("error in 'Matrix::set_size':  calloc call failed - \n");   
      mexPrintf("  not enough memory!!\n");
      mexPrintf("  matrix name = '%s', desired rows = %d\n",name,rows);   
      mexPrintf("     (originating module name of matrix = '%s')\n",module_name);
      mexErrMsgTxt("Exiting CppSim Mex object...");   
      }
   for (i = 0; i < rows; i++)   
      if (((mat->elem)[i] = (double *) calloc(cols,sizeof(double))) == NULL)
         {   
         mexPrintf("error in 'Matrix::set_size':  calloc call failed - \n");   
         mexPrintf("  not enough memory!!\n");
         mexPrintf("  matrix name = '%s', desired cols = %d\n",name,cols);
         mexPrintf("   (originating module name of matrix = '%s')\n",module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");   
         }   

   for (i = 0; i < rows; i++)
      for (j = 0; j < cols; j++)
        mat->elem[i][j] = 0.0;
   }
mat->rows = rows;
mat->cols = cols;
}


void Matrix::init()
{
if (mat != NULL)
  {
    mexPrintf("error in 'Matrix::init()': you cannot initialize a matrix\n");
    mexPrintf("  that has already been initialized or assigned from another matrix\n");
    mexPrintf("  in this case, you ran init() on matrix '%s'\n",name);
    mexPrintf("     (originating in module '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
   
if ((mat = (matrix_struct *) malloc(sizeof(matrix_struct))) == NULL)   
   {   
   mexPrintf("error in 'Matrix::init()':  malloc call failed\n");   
   mexPrintf("out of memory!\n");
   mexPrintf("  matrix name = '%s'\n",name);   
   mexPrintf("     (originating module name of matrix = '%s')\n",module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");   
  }   

mat->rows = 0;
mat->cols = 0;
mat->freeze_length_flag = 0;
mat->elem = NULL;
mat->name = name;
mat->module_name = module_name;
initialized_flag = 1;
}   

void Matrix::set_elem(int index_row, int index_col, double val) const
{

if (index_row < 0 || index_col < 0)
   {
    mexPrintf("error in 'Matrix::set_elem':  row and column index must be >= 0\n");
    mexPrintf("  in this case, index_row = %d, index_col = %d\n",index_row,
             index_col);
    mexPrintf("  and you ran set_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (index_row >= mat->rows)
    {
    mexPrintf("error in 'Matrix::set_elem':  index_row must be < rows of matrix\n");
    mexPrintf("  in this case, index_row = %d, matrix is %d by %d\n",index_row,
               mat->rows, mat->cols);
    mexPrintf("  and you ran set_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

if (index_col >= mat->cols)
    {
    mexPrintf("error in 'Matrix::set_elem':  index_col must be < cols of matrix\n");
    mexPrintf("  in this case, index_col = %d, matrix is %d by %d\n",index_col,
               mat->rows, mat->cols);
    mexPrintf("  and you ran set_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

mat->elem[index_row][index_col] = val;
}   


void Matrix::add_to_elem(int index_row, int index_col, double val) const
{

if (index_row < 0 || index_col < 0)
   {
    mexPrintf("error in 'Matrix::add_to_elem':  row and column index must be >= 0\n");
    mexPrintf("  in this case, index_row = %d, index_col = %d\n",index_row,
             index_col);
    mexPrintf("  and you ran add_to_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (index_row >= mat->rows)
    {
    mexPrintf("error in 'Matrix::add_to_elem':  index_row must be < rows of matrix\n");
    mexPrintf("  in this case, index_row = %d, matrix is %d by %d\n",index_row,
               mat->rows, mat->cols);
    mexPrintf("  and you ran add_to_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

if (index_col >= mat->cols)
    {
    mexPrintf("error in 'Matrix::add_to_elem':  index_col must be < cols of matrix\n");
    mexPrintf("  in this case, index_col = %d, matrix is %d by %d\n",index_col,
               mat->rows, mat->cols);
    mexPrintf("  and you ran add_to_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

mat->elem[index_row][index_col] += val;
}   


double Matrix::get_elem(int index_row, int index_col) const
{

if (index_row < 0 || index_col < 0)
   {
    mexPrintf("error in 'Matrix::get_elem':  index for row and column must be >= 0\n");
    mexPrintf("  in this case, index_row = %d, index_col = %d\n",index_row,
              index_col);
    mexPrintf("  and you ran get_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (index_row >= mat->rows)
    {
    mexPrintf("error in 'Matrix::get_elem':  index_row must be < rows of matrix\n");
    mexPrintf("  in this case, index_row = %d, matrix is %d by %d\n",
          index_row, mat->rows, mat->cols);
    mexPrintf("  and you ran get_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

if (index_col >= mat->cols)
    {
    mexPrintf("error in 'Matrix::get_elem':  index_col must be < cols of matrix\n");
    mexPrintf("  in this case, index_col = %d, matrix is %d by %d\n",
          index_col, mat->rows, mat->cols);
    mexPrintf("  and you ran get_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

return(mat->elem[index_row][index_col]);
}   

void copy(const Matrix &from, const Matrix &to)
{
matrix_struct *x,*y;
x = extract_matrix_struct(from);
y = extract_matrix_struct(to);

copy(x,y);
}

void save(const char *filename, const Matrix &in)
{
matrix_struct *x;
x = extract_matrix_struct(in);

save_struct(filename,x);
}

void load(const char *filename, const Matrix &in)
{
matrix_struct *x;
x = extract_matrix_struct(in);

load_struct(filename,x);
}

void mul(const Matrix &A, const Matrix &B, const Matrix &C)
{
matrix_struct *a,*b,*c;

a = extract_matrix_struct(A);
b = extract_matrix_struct(B);
c = extract_matrix_struct(C);

mul(a,b,c);
}

void mul(const Matrix &A, double B, const Matrix &C)
{
matrix_struct *a,*c;

a = extract_matrix_struct(A);
c = extract_matrix_struct(C);

mul(a,B,c);
}


void mul(double A, const Matrix &B, const Matrix &C)
{
matrix_struct *b,*c;

b = extract_matrix_struct(B);
c = extract_matrix_struct(C);

mul(A,b,c);
}

void mul(const Matrix &A, const Vector &B, const Vector &C)
{
matrix_struct *a;
vector_struct *b,*c;

a = extract_matrix_struct(A);
b = extract_vector_struct(B);
c = extract_vector_struct(C);

mul(a,b,c);
}

void mul(const Vector &A, const Matrix &B, const Vector &C)
{
matrix_struct *b;
vector_struct *a,*c;

a = extract_vector_struct(A);
b = extract_matrix_struct(B);
c = extract_vector_struct(C);

mul(a,b,c);
}


void add(const Matrix &A, const Matrix &B, const Matrix &C)
{
matrix_struct *a,*b,*c;

a = extract_matrix_struct(A);
b = extract_matrix_struct(B);
c = extract_matrix_struct(C);

add(a,b,c);
}

void add(const Matrix &A, double B, const Matrix &C)
{
matrix_struct *a,*c;

a = extract_matrix_struct(A);
c = extract_matrix_struct(C);

add(a,B,c);
}

void add(double A, const Matrix &B, const Matrix &C)
{
matrix_struct *b,*c;

b = extract_matrix_struct(B);
c = extract_matrix_struct(C);

add(A,b,c);
}


void sub(const Matrix &A, const Matrix &B, const Matrix &C)
{
matrix_struct *a,*b,*c;

a = extract_matrix_struct(A);
b = extract_matrix_struct(B);
c = extract_matrix_struct(C);

sub(a,b,c);
}

void sub(const Matrix &A, double B, const Matrix &C)
{
matrix_struct *a,*c;

a = extract_matrix_struct(A);
c = extract_matrix_struct(C);

sub(a,B,c);
}

void sub(double A, const Matrix &B, const Matrix &C)
{
matrix_struct *b,*c;

b = extract_matrix_struct(B);
c = extract_matrix_struct(C);

sub(A,b,c);
}

void trans(const Matrix &A, const Matrix &B)
{
matrix_struct *a,*b;

a = extract_matrix_struct(A);
b = extract_matrix_struct(B);

trans(a,b);
}


void svd(const Matrix &A, 
         const Matrix &U, const Matrix &W, const Matrix &V)
{
matrix_struct *a,*u,*w,*v;

a = extract_matrix_struct(A);
u = extract_matrix_struct(U);
w = extract_matrix_struct(W);
v = extract_matrix_struct(V);

svd(a,u,w,v);
}

void inv(const Matrix &A, const Matrix &B)
{
matrix_struct *a,*b;

a = extract_matrix_struct(A);
b = extract_matrix_struct(B);

inv(a,b);
}

void least_sq(const Matrix &A, const Matrix &B, const Matrix &X)
{
matrix_struct *a,*b,*x;

a = extract_matrix_struct(A);
b = extract_matrix_struct(B);
x = extract_matrix_struct(X);

least_sq(a,b,x);
}


void real_to_int(const Matrix &in, const IntMatrix &out)
{
matrix_struct *x;
int_matrix_struct *y;
x = extract_matrix_struct(in);
y = extract_matrix_struct(out);

real_to_int(x,y);
}

void int_to_real(const IntMatrix &in, const Matrix &out)
{
int_matrix_struct *x;
matrix_struct *y;
x = extract_matrix_struct(in);
y = extract_matrix_struct(out);

int_to_real(x,y);
}

void print(const Matrix &in)
{
matrix_struct *x;
x = extract_matrix_struct(in);

print(x);
}



//////////////////// Integer-valued matrices //////////////////

IntMatrix::IntMatrix()
{
if ((module_name = (char *) calloc(10,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'IntMatrix' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
sprintf(module_name,"unknown");

if ((name = (char *) calloc(10,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'IntMatrix' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
sprintf(name,"unnamed");

mat = NULL;
init();  // creates matrix and sets initialized_flag to 1
}

IntMatrix::IntMatrix(const char *module_name_in, const char *name_in)
{
int i;
for (i = 0; module_name_in[i] != '\0'; i++);

if ((module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'IntMatrix' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  module_name = '%s'\n",module_name_in);
    mexPrintf("  desired module_name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(module_name,module_name_in);

for (i = 0; name_in[i] != '\0'; i++);
if ((name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'IntMatrix' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  name = '%s'\n",name_in);
    mexPrintf("  desired name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(name,name_in);

mat = NULL;
init();  // creates vec and sets initialized_flag to 1
}

IntMatrix::IntMatrix(const char *module_name_in, const char *name_in, const char *null_string)
{
int i;

mat = NULL;
initialized_flag = 0;

if (strcmp(null_string,"NULL") != 0)
   {
     mexPrintf("Error:  you are using the wrong constructor!!!\n");
     mexPrintf("   ---> use either 'IntMatrix mat1;' or 'IntMatrix mat1(\"mod_name\",\"name\");'\n");
     mexPrintf("   in this case, you used 'IntMatrix %s(\"%s\",\"%s\",\"%s\");'\n",
	    name_in, module_name_in,name_in,null_string);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }

for (i = 0; module_name_in[i] != '\0'; i++);
if ((module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'IntMatrix' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  module_name = '%s'\n",module_name_in);
    mexPrintf("  desired module_name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(module_name,module_name_in);

for (i = 0; name_in[i] != '\0'; i++);
if ((name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'IntMatrix' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  name = '%s'\n",name_in);
    mexPrintf("  desired name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(name,name_in);
}


IntMatrix::IntMatrix(const IntMatrix &other)
{
int i,j;

mat = other.mat;
initialized_flag = 0;

for (j = 0; other.name[j] != '\0'; j++);
if ((name = (char *) calloc(j+45,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'IntMatrix' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  name = '%s'\n",other.name);
    mexPrintf("  desired name length = %d\n",j+45);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
sprintf(name,"derived from: %s",other.name);

for (i = 0; other.module_name[i] != '\0'; i++);
if ((module_name = (char *) calloc(i+j+30,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'IntMatrix' constructor:  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  module_name = '%s'\n",other.module_name);
    mexPrintf("  desired module_name length = %d\n",i+j+30);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
sprintf(module_name,"origin: %s",other.module_name);

}

IntMatrix::~IntMatrix()
{
int i;
free(name);
free(module_name);
if (initialized_flag == 1)
   {
    if (mat->elem != NULL)
      {
       for (i = 0; i < mat->rows; i++)   
          free(mat->elem[i]);   
       free(mat->elem);   
      }
    free(mat);
    mat = NULL;
   }
}


void IntMatrix::operator = (const IntMatrix &other) const
{
if (mat != other.mat)
  {
    copy(other);
  }
}

void IntMatrix::operator = (IntMatrix &other) const
{
if (mat != other.mat)
  {
    copy(other);
  }
}

void IntMatrix::replace_mat(int_matrix_struct *in)
{
mat = in;
}

void IntMatrix::copy(int_matrix_struct *in) const
{
int i,j,rows,cols;
int **mat_elem,**in_elem;

rows = in->rows;
cols = in->cols;
set_size(rows,cols);

mat_elem = mat->elem;
in_elem = in->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      mat_elem[i][j] = in_elem[i][j];
}

void IntMatrix::copy(const IntMatrix &other) const
{
int i,j,rows,cols;
int **mat_elem,**in_elem;

if (mat != other.mat)
   {
   rows = other.mat->rows;
   cols = other.mat->cols;
   set_size(rows,cols);

   mat_elem = mat->elem;
   in_elem = other.mat->elem;

   for (i = 0; i < rows; i++)
      for (j = 0; j < cols; j++)
         mat_elem[i][j] = in_elem[i][j];
   }
}

void IntMatrix::copy(IntMatrix &other) const
{
int i,j,rows,cols;
int **mat_elem,**in_elem;

if (mat != other.mat)
   {
   rows = other.mat->rows;
   cols = other.mat->cols;
   set_size(rows,cols);

   mat_elem = mat->elem;
   in_elem = other.mat->elem;

   for (i = 0; i < rows; i++)
      for (j = 0; j < cols; j++)
         mat_elem[i][j] = in_elem[i][j];
   }
}


void IntMatrix::load(const char *filename) const
{
load_struct(filename,mat);
}

void IntMatrix::save(const char *filename) const
{
save_struct(filename,mat);
}


int IntMatrix::get_rows() const
{
return(mat->rows);
}

int IntMatrix::get_cols() const
{
return(mat->cols);
}

char *IntMatrix::get_name() const
{
return(name);
}

char *IntMatrix::get_module_name() const
{
return(module_name);
}


void IntMatrix::set_size(int rows, int cols) const
{
int i,j;

if (mat->rows == rows && mat->cols == cols)
   return;
else if (mat->freeze_length_flag == 1)
   {
    mexPrintf("error in 'IntMatrix::set_size':  cannot change the size of a\n");
    mexPrintf("   matrix declared in the 'outputs:' section of a module in the modules.par file\n");
    mexPrintf("   EXCEPT in the 'init_output_vectors:' section of the modules.par file\n");
    mexPrintf("   in this case, you tried changing the size of matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexPrintf("-> cur matrix size = %d by %d, desired size = %d by %d\n",
	   mat->rows, mat->cols, rows, cols);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (rows < 0 || cols < 0)
   {
    mexPrintf("error in 'IntMatrix::set_size':  rows and cols must be >= 0\n");
    mexPrintf("  in this case, rows = %d, cols = %d\n",rows,cols);
    mexPrintf("  and you ran set_size() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }   

if (mat->rows != 0)
  {
    for (i = 0; i < mat->rows; i++)   
        free(mat->elem[i]);   
    free(mat->elem);
    mat->elem = NULL;
  }

if (rows > 0 && cols > 0)
   {
   if ((mat->elem = (int **) calloc(rows,sizeof(int *))) == NULL)   
      {   
      mexPrintf("error in 'IntMatrix::set_size':  calloc call failed - \n");   
      mexPrintf("  not enough memory!!\n");
      mexPrintf("  matrix name = '%s', desired rows = %d\n",name,rows);   
      mexPrintf("     (originating module name of matrix = '%s')\n",module_name);
      mexErrMsgTxt("Exiting CppSim Mex object...");   
      }
   for (i = 0; i < rows; i++)   
      if (((mat->elem)[i] = (int *) calloc(cols,sizeof(int))) == NULL)
         {   
         mexPrintf("error in 'IntMatrix::set_size':  calloc call failed - \n");   
         mexPrintf("  not enough memory!!\n");
         mexPrintf("  matrix name = '%s', desired cols = %d\n",name,cols);
         mexPrintf("   (originating module name of matrix = '%s')\n",module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");   
         }   

   for (i = 0; i < rows; i++)
      for (j = 0; j < cols; j++)
        mat->elem[i][j] = 0;
   }
mat->rows = rows;
mat->cols = cols;
}


void IntMatrix::init()
{
if (mat != NULL)
  {
    mexPrintf("error in 'IntMatrix::init()': you cannot initialize a matrix\n");
    mexPrintf("  that has already been initialized or assigned from another matrix\n");
    mexPrintf("  in this case, you ran init() on matrix '%s'\n",name);
    mexPrintf("     (originating in module '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
  }
   
if ((mat = (int_matrix_struct *) malloc(sizeof(int_matrix_struct))) == NULL)   
   {   
   mexPrintf("error in 'IntMatrix::init()':  malloc call failed\n");   
   mexPrintf("out of memory!\n");
   mexPrintf("  matrix name = '%s'\n",name);   
   mexPrintf("     (originating module name of matrix = '%s')\n",module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");   
  }   

mat->rows = 0;
mat->cols = 0;
mat->freeze_length_flag = 0;
mat->elem = NULL;
mat->name = name;
mat->module_name = module_name;
initialized_flag = 1;
}   

void IntMatrix::set_elem(int index_row, int index_col, int val) const
{

if (index_row < 0 || index_col < 0)
   {
    mexPrintf("error in 'IntMatrix::set_elem':  row and column index must be >= 0\n");
    mexPrintf("  in this case, index_row = %d, index_col = %d\n",index_row,
             index_col);
    mexPrintf("  and you ran set_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (index_row >= mat->rows)
    {
    mexPrintf("error in 'IntMatrix::set_elem':  index_row must be < rows of matrix\n");
    mexPrintf("  in this case, index_row = %d, matrix is %d by %d\n",index_row,
               mat->rows, mat->cols);
    mexPrintf("  and you ran set_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

if (index_col >= mat->cols)
    {
    mexPrintf("error in 'IntMatrix::set_elem':  index_col must be < cols of matrix\n");
    mexPrintf("  in this case, index_col = %d, matrix is %d by %d\n",index_col,
               mat->rows, mat->cols);
    mexPrintf("  and you ran set_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

mat->elem[index_row][index_col] = val;
}   

void IntMatrix::add_to_elem(int index_row, int index_col, int val) const
{

if (index_row < 0 || index_col < 0)
   {
    mexPrintf("error in 'IntMatrix::add_to_elem':  row and column index must be >= 0\n");
    mexPrintf("  in this case, index_row = %d, index_col = %d\n",index_row,
             index_col);
    mexPrintf("  and you ran add_to_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (index_row >= mat->rows)
    {
    mexPrintf("error in 'IntMatrix::add_to_elem':  index_row must be < rows of matrix\n");
    mexPrintf("  in this case, index_row = %d, matrix is %d by %d\n",index_row,
               mat->rows, mat->cols);
    mexPrintf("  and you ran add_to_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

if (index_col >= mat->cols)
    {
    mexPrintf("error in 'IntMatrix::add_to_elem':  index_col must be < cols of matrix\n");
    mexPrintf("  in this case, index_col = %d, matrix is %d by %d\n",index_col,
               mat->rows, mat->cols);
    mexPrintf("  and you ran add_to_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

mat->elem[index_row][index_col] += val;
}   

int IntMatrix::get_elem(int index_row, int index_col) const
{

if (index_row < 0 || index_col < 0)
   {
    mexPrintf("error in 'IntMatrix::get_elem':  index for row and column must be >= 0\n");
    mexPrintf("  in this case, index_row = %d, index_col = %d\n",index_row,
              index_col);
    mexPrintf("  and you ran get_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (index_row >= mat->rows)
    {
    mexPrintf("error in 'IntMatrix::get_elem':  index_row must be < rows of matrix\n");
    mexPrintf("  in this case, index_row = %d, matrix is %d by %d\n",
          index_row, mat->rows, mat->cols);
    mexPrintf("  and you ran get_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

if (index_col >= mat->cols)
    {
    mexPrintf("error in 'IntMatrix::get_elem':  index_col must be < cols of matrix\n");
    mexPrintf("  in this case, index_col = %d, matrix is %d by %d\n",
          index_col, mat->rows, mat->cols);
    mexPrintf("  and you ran get_elem() on matrix '%s'\n",name);
    mexPrintf("     (originating module of matrix = '%s')\n",module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
    }

return(mat->elem[index_row][index_col]);
}   

void copy(const IntMatrix &from, const IntMatrix &to)
{
int_matrix_struct *x,*y;
x = extract_matrix_struct(from);
y = extract_matrix_struct(to);

copy(x,y);
}

void save(const char *filename, const IntMatrix &in)
{
int_matrix_struct *x;
x = extract_matrix_struct(in);

save_struct(filename,x);
}

void load(const char *filename, const IntMatrix &in)
{
int_matrix_struct *x;
x = extract_matrix_struct(in);

load_struct(filename,x);
}

void mul(const IntMatrix &A, const IntMatrix &B, const IntMatrix &C)
{
int_matrix_struct *a,*b,*c;

a = extract_matrix_struct(A);
b = extract_matrix_struct(B);
c = extract_matrix_struct(C);

mul(a,b,c);
}

void mul(const IntMatrix &A, int B, const IntMatrix &C)
{
int_matrix_struct *a,*c;

a = extract_matrix_struct(A);
c = extract_matrix_struct(C);

mul(a,B,c);
}


void mul(int A, const IntMatrix &B, const IntMatrix &C)
{
int_matrix_struct *b,*c;

b = extract_matrix_struct(B);
c = extract_matrix_struct(C);

mul(A,b,c);
}

void mul(const IntMatrix &A, const IntVector &B, const IntVector &C)
{
int_matrix_struct *a;
int_vector_struct *b,*c;

a = extract_matrix_struct(A);
b = extract_vector_struct(B);
c = extract_vector_struct(C);

mul(a,b,c);
}

void mul(const IntVector &A, const IntMatrix &B, const IntVector &C)
{
int_matrix_struct *b;
int_vector_struct *a,*c;

a = extract_vector_struct(A);
b = extract_matrix_struct(B);
c = extract_vector_struct(C);

mul(a,b,c);
}



void add(const IntMatrix &A, const IntMatrix &B, const IntMatrix &C)
{
int_matrix_struct *a,*b,*c;

a = extract_matrix_struct(A);
b = extract_matrix_struct(B);
c = extract_matrix_struct(C);

add(a,b,c);
}

void add(const IntMatrix &A, int B, const IntMatrix &C)
{
int_matrix_struct *a,*c;

a = extract_matrix_struct(A);
c = extract_matrix_struct(C);

add(a,B,c);
}

void add(int A, const IntMatrix &B, const IntMatrix &C)
{
int_matrix_struct *b,*c;

b = extract_matrix_struct(B);
c = extract_matrix_struct(C);

add(A,b,c);
}


void sub(const IntMatrix &A, const IntMatrix &B, const IntMatrix &C)
{
int_matrix_struct *a,*b,*c;

a = extract_matrix_struct(A);
b = extract_matrix_struct(B);
c = extract_matrix_struct(C);

sub(a,b,c);
}

void sub(const IntMatrix &A, int B, const IntMatrix &C)
{
int_matrix_struct *a,*c;

a = extract_matrix_struct(A);
c = extract_matrix_struct(C);

sub(a,B,c);
}

void sub(int A, const IntMatrix &B, const IntMatrix &C)
{
int_matrix_struct *b,*c;

b = extract_matrix_struct(B);
c = extract_matrix_struct(C);

sub(A,b,c);
}

void trans(const IntMatrix &A, const IntMatrix &B)
{
int_matrix_struct *a,*b;

a = extract_matrix_struct(A);
b = extract_matrix_struct(B);

trans(a,b);
}

void print(const IntMatrix &in)
{
int_matrix_struct *x;
x = extract_matrix_struct(in);

print(x);
}

/////////////// Matrix structure routines ////////////////////

void trans(matrix_struct *A, matrix_struct *B)
{
int i,j,rows,cols;
double **A_elem, **B_elem;

if (A == B)
   {
   mexPrintf("error in 'trans(A,B)':  matrix B must be distinct from matrix A\n");
   mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");   
   }

rows = A->rows;
cols = A->cols;

set_size(cols,rows,B);
A_elem = A->elem;
B_elem = B->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      B_elem[j][i] = A_elem[i][j];
}

matrix_struct *init_matrix_struct(const char *module_name, const char *name)   
{   
matrix_struct *A;   
   
if ((A = (matrix_struct *) malloc(sizeof(matrix_struct))) == NULL)   
   {   
   mexPrintf("error in 'init_matrix_struct':  malloc call failed\n");   
   mexPrintf("out of memory!\n");
   mexErrMsgTxt("Exiting CppSim Mex object...");   
  }   

int i;
for (i = 0; module_name[i] != '\0'; i++);
   
if ((A->module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'init_matrix_struct':  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  module_name = '%s'\n",module_name);
    mexPrintf("  desired module_name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(A->module_name,module_name);

for (i = 0; name[i] != '\0'; i++);
if ((A->name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'init_matrix_struct':  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  name = '%s'\n",name);
    mexPrintf("  desired name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(A->name,name);

A->rows = 0;
A->cols = 0;
A->freeze_length_flag = 0;
A->elem = NULL;

return(A);   
}   

void free_matrix_struct(matrix_struct *x)
{
int i;

if (x->elem != NULL)
   {
   for (i = 0; i < x->rows; i++)
      free(x->elem[i]);   
   free(x->elem);
   }
free(x->name);
free(x->module_name);
free(x);
}

double read_elem(int row, int col, matrix_struct *A)
{

if (row < 0 || col < 0)
    {
      mexPrintf("error in 'read_elem':  row and col must be >= 0\n");
      mexPrintf("  in this case, for matrix '%s':\n",A->name);
      mexPrintf("  -->  row = %d, col = %d\n",row,col);
      mexPrintf("  originating module name of matrix = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }

if (row >= A->rows || col >= A->cols)
    {
      mexPrintf("error in 'read_elem':  row must be < A->rows and col must be < A->cols\n");
      mexPrintf("  in this case, for matrix '%s':\n",A->name);
      mexPrintf("  -->  row = %d, A->rows = %d --- col = %d, A->cols = %d\n",
               row, A->rows, col, A->cols);
      mexPrintf("  originating module name of matrix = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }

return(A->elem[row][col]);
}   

void write_elem(double val, int row, int col, matrix_struct *A)
{
if (row < 0 || col < 0)
    {
      mexPrintf("error in 'write_elem':  row and col must be >= 0\n");
      mexPrintf("  in this case, for matrix '%s':\n",A->name);
      mexPrintf("  -->  row = %d, col = %d\n",row,col);
      mexPrintf("  originating module name of matrix = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }

if (row >= A->rows || col >= A->cols)
    {
      mexPrintf("error in 'write_elem':  row must be < A->rows and col must be < A->cols\n");
      mexPrintf("  in this case, for matrix '%s':\n",A->name);
      mexPrintf("  -->  row = %d, A->rows = %d --- col = %d, A->cols = %d\n",
               row, A->rows, col, A->cols);
      mexPrintf("  originating module name of matrix = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }

A->elem[row][col] = val;
}   


void save(const char *filename, matrix_struct *A)
{
save_struct(filename,A);
}

void load(const char *filename, matrix_struct *A)
{
load_struct(filename,A);
}

void copy(matrix_struct *x, matrix_struct *y)
{
int i,j,rows,cols;
double **x_elem,**y_elem;

rows = x->rows;
cols = x->cols;

set_size(rows,cols,y);
x_elem = x->elem;
y_elem = y->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
       y_elem[i][j] = x_elem[i][j];
}


void print(matrix_struct *x)
{
int i,j;

mexPrintf("matrix name = '%s' (originated in module '%s')\n",
	x->name, x->module_name);

for (i = 0; i < x->rows; i++)
  {
   mexPrintf("  %s[%d][0:%d] =",x->name,i,x->cols-1);
   for (j = 0; j < x->cols; j++)
       mexPrintf(" %5.3e",x->elem[i][j]);
   mexPrintf("\n");
  }
}


matrix_struct *extract_matrix_struct(const Matrix &in)
{
if (in.mat == NULL)
    {
      mexPrintf("error in 'extract_matrix_struct': mat is NULL!\n");
      mexPrintf("  this shouldn't happen in normal use of matrices\n");
      mexPrintf("  name of matrix = '%s'\n",in.name);
      mexPrintf("      (originating module of matrix = '%s')\n",in.module_name);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }

// transfer names for error checking

in.mat->name = in.name;
in.mat->module_name = in.module_name;
return(in.mat);
}


void set_size(int rows, int cols, matrix_struct *mat)
{
int i,j;

if (mat->rows == rows && mat->cols == cols)
   return;
else if (mat->freeze_length_flag == 1)
   {
    mexPrintf("error in 'Matrix::set_size':  cannot change the size of a\n");
    mexPrintf("   matrix declared in the 'outputs:' section of a module in the modules.par file\n");
    mexPrintf("   EXCEPT in the 'init_output_vectors:' section of the modules.par file\n");
    mexPrintf("   in this case, you tried changing the size of matrix '%s'\n",
               mat->name);
    mexPrintf("     (originating module of matrix = '%s')\n",mat->module_name);
    mexPrintf("-> cur matrix size = %d by %d, desired size = %d by %d\n",
	   mat->rows, mat->cols, rows, cols);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (rows < 0 || cols < 0)
   {
    mexPrintf("error in 'Matrix::set_size':  rows and cols must be >= 0\n");
    mexPrintf("  in this case, rows = %d, cols = %d\n",rows,cols);
    mexPrintf("  and you ran set_size() on matrix '%s'\n",mat->name);
    mexPrintf("     (originating module of matrix = '%s')\n",mat->module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }   

if (mat->rows != 0)
  {
    for (i = 0; i < mat->rows; i++)   
        free(mat->elem[i]);   
    free(mat->elem);
    mat->elem = NULL;   
  }

if (rows > 0 && cols > 0)
   {
   if ((mat->elem = (double **) calloc(rows,sizeof(double *))) == NULL)   
      {   
      mexPrintf("error in 'Matrix::set_size':  calloc call failed - \n");   
      mexPrintf("  not enough memory!!\n");
      mexPrintf("  matrix name = '%s', desired rows = %d\n",mat->name,rows);   
      mexPrintf("     (originating module name of matrix = '%s')\n",
                mat->module_name);
      mexErrMsgTxt("Exiting CppSim Mex object...");   
      }
   for (i = 0; i < rows; i++)   
      if (((mat->elem)[i] = (double *) calloc(cols,sizeof(double))) == NULL)
         {   
         mexPrintf("error in 'Matrix::set_size':  calloc call failed - \n");   
         mexPrintf("  not enough memory!!\n");
         mexPrintf("  matrix name = '%s', desired cols = %d\n",mat->name,cols);
         mexPrintf("   (originating module name of matrix = '%s')\n",
                mat->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");   
         }   

   for (i = 0; i < rows; i++)
      for (j = 0; j < cols; j++)
        mat->elem[i][j] = 0.0;
   }
mat->rows = rows;
mat->cols = cols;
}



void load_struct(const char *filename,matrix_struct *A)   
{   
FILE *fp;   
int i,j,entry_count,row_count,col_count,char_count,temp_col_count,flag;   
int file_segment_flag,dummy_count,line_count;   
char line[MAX_FILE_CHAR+1],name[MAX_FILE_CHAR+1];   
float dummy_var;

char task[30];
int rows,cols;   
   
if ((fp = fopen(filename,"r")) == NULL)   
  {   
   mexPrintf("\nerror in 'load': file %s can't be opened\n",filename);   
   mexErrMsgTxt("Exiting CppSim Mex object...");   
 }   

/* task: can be a matrix name or "exclusive", "exclusive" meaning that
         only one matrix is contained in the file.
         this parameter was rarely used, and is now always set to "exclusive"
*/
sprintf(task,"exclusive");
/* rows/cols:  can be used to limit the number of data points retrieved
               from the file to what's set by these values.
               these parameters were rarely used in the past, and are now
               always set to 0 so that the all of the points are obtained
               from the file
*/
rows = 0;
cols = 0;

if (strcmp(task,"exclusive") == 0)   
   file_segment_flag = 2;   
else   
   file_segment_flag = 0;   
if (rows <= 0 || cols <= 0)   
  {   
   col_count = row_count = line_count = 0;   
   while(1)   
     {   
      if (fgets(line,MAX_FILE_CHAR+1,fp) != NULL)   
	{   
         line_count++;   
         if (sscanf(line,"%f",&dummy_var) == 0)   
	   {   
            if (sscanf(line,"%s",name) != 0)   
              {   
               if (file_segment_flag == 2)   
                  continue;   
               else if (strcmp(task,name) == 0)   
	         {   
                  file_segment_flag = 1;   
                  continue;   
	         }   
               else   
                   file_segment_flag = 0;   
	     }   
	  }   
         if (file_segment_flag)   
	   {   
            char_count = temp_col_count = flag = 0;   
            while (line[char_count] != '\0' && line[char_count] != '\n' && line[char_count] != '\r')
	      {   
               if (line[char_count] != ' ' && flag == 0)   
	         {   
                  temp_col_count += 1;   
                  flag = 1;   
	        }   
               if (line[char_count] == ' ' && flag == 1)   
                  flag = 0;   
               char_count++;   
	     }   
            if (col_count == 0)   
               col_count = temp_col_count;   
            else if (col_count != temp_col_count && (rows <= 0 || cols <= 0)   
                      && temp_col_count != 0)   
	         {   
                  mexPrintf("error in 'load':\n\n");   
                  mexPrintf("  file '%s':  number of cols needs to be the same in each row\n",filename);   
                  mexPrintf("              in order to fill matrix %s\n",A->name);   
                  mexPrintf("(this condition is violated on line %d of file)\n",   
                          line_count);   
                  mexErrMsgTxt("Exiting CppSim Mex object...");   
	        }   
            if (temp_col_count > 0)   
               row_count++;   
	  }   
       }   
     else   
        break;   
    }   
   rows = row_count;   
   cols = col_count;   
   rewind(fp);   
 }   
      set_size(rows,cols,A);   
      entry_count = 0;   
      if (file_segment_flag != 2)   
         file_segment_flag = 0;   
      for (i = 0; i < A->rows; i++)   
         for (j = 0; j < A->cols; j++)   
           {   
            if (fscanf(fp,"%s",name) == EOF)   
               {   
               mexPrintf("\nerror in 'load':  matrix %s needs %d values,\n",   
                       A->name,rows*cols);    
               mexPrintf("but file %s has only %d data values to fill it with\n",   
                       filename,entry_count);   
               mexErrMsgTxt("Exiting CppSim Mex object...");   
             }   
            if (atof(name) == 0)   
	      {   
               flag = 0;   
               for (dummy_count = 0; dummy_count < MAX_FILE_CHAR &&    
                    name[dummy_count] != '\0' && name[dummy_count] != '\n';   
                    dummy_count++)   
		 {   
                  if (name[dummy_count] == '0')   
		    {   
                     if (dummy_count == 0)   
                         flag = 1;   
                     else if (name[dummy_count-1] == ' ' ||   
                              name[dummy_count-1] == '.' ||   
                              name[dummy_count-1] == '-')   
                         flag = 1;   
                     break;   
		    }   
                  else   
                      continue;   
		}   
               if (flag == 0)   
		 {   
                  j--;   
                  if (file_segment_flag == 2)   
                     continue;   
                  else if (strcmp(task,name) == 0)   
	            {   
                     file_segment_flag = 1;   
                     continue;   
	            }   
                  else   
                    {   
                     file_segment_flag = 0;    
                     continue;   
		   }   
		}   
	     }   
           if (file_segment_flag != 0)   
	     {   
              A->elem[i][j] = (double) atof(name);   
              entry_count++;   
	     }   
           else   
               j--;   
	  }   
   
fclose(fp);   
}   




void save_struct(const char *filename, matrix_struct *A)
{   
FILE *fp;   
int i,j;   

char control[10];
/* control:  parameter that allows the output file to be written
             or appended, and also to attach labels to the matrix
             data stored.  In the past, this parameter was rarely
             used for anything other than "w", and is therefore
             always set to that case now
*/
sprintf(control,"w");

fp = NULL; // initialize so that gcc won't complain
   
if (strcmp(control,"w") == 0 || strcmp(control,"wl") == 0)   
  {   
   if ((fp = fopen(filename,"w")) == NULL)   
     {   
      mexPrintf("error in 'save':  file %s cannot be opened\n",filename);   
      mexErrMsgTxt("Exiting CppSim Mex object...");   
    }   
   if (strcmp(control,"wl") == 0)   
           fprintf(fp,"%s\n",A->name);   
   for (i = 0; i < A->rows; i++)   
     {   
      for (j = 0; j < A->cols; j++)   
          fprintf(fp,"%e ",A->elem[i][j]);   
      fprintf(fp,"\n");   
    }   
 }   
else if (strcmp(control,"a") == 0 || strcmp(control,"al") == 0)   
  {   
   if ((fp = fopen(filename,"a")) == NULL)   
     {   
      mexPrintf("error in 'send_to_file':  file %s cannot be opened\n",filename);   
      mexErrMsgTxt("Exiting CppSim Mex object...");   
    }   
   if (strcmp(control,"al") == 0)   
      fprintf(fp,"%s\n",A->name);   
   for (i = 0; i < A->rows; i++)   
     {   
      for (j = 0; j < A->cols; j++)   
          fprintf(fp,"%e ",A->elem[i][j]);   
      fprintf(fp,"\n");   
    }   
 }   
else   
  {   
   mexPrintf("error in 'save':  control character was specified as %s,\n",   
           control);   
   mexPrintf("which is invalid since it must be specified as either 'w' (for\n");   
   mexPrintf("writing to a file), or 'a' (for appending to a file),\n");   
   mexPrintf("or 'wl' (write and label matrix in file), or 'al'\n");   
   mexPrintf("(append and label matrix to file)\n");   
   mexErrMsgTxt("Exiting CppSim Mex object...");   
 }   
fclose(fp);   
/* if (check_file != NULL)   
   fprintf(check_file,"'save':  write matrix %s (%d by %d) into file %s\n",A->name,A->rows,A->cols,filename); */   

}   


void mul(matrix_struct *A, matrix_struct *B, matrix_struct *C)
{
int i,j,k,rows,cols,span;
double **A_elem,**B_elem,**C_elem;
double temp;

if (A->cols != B->rows)
   {
     mexPrintf("error in 'mul(A,B,C)':  matrix A->cols must equal B->rows\n");
     mexPrintf("   in this case, A->cols = %d, B->rows = %d\n",A->cols,B->rows);
     mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexPrintf("   matrix C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if (C == A || C == B)
   {
     mexPrintf("error in 'mul(A,B,C)':  matrix C must be distinct from matrix A and B\n");
     mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexPrintf("   matrix C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");   
   }

rows = A->rows;
cols = B->cols;
span = A->cols;
set_size(rows,cols,C);

A_elem = A->elem;
B_elem = B->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
  {
  for (j = 0; j < cols; j++)
     {
       temp = 0.0;
       for (k = 0; k < span; k++)
	 {
	  temp += A_elem[i][k]*B_elem[k][j];
	 }
       C_elem[i][j] = temp;
     }
  } 
}

void mul(matrix_struct *A, double B, matrix_struct *C)
{
int i,j,rows,cols;
double **A_elem, **C_elem;

rows = A->rows;
cols = A->cols;
set_size(rows,cols,C);

A_elem = A->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
  for (j = 0; j < cols; j++)
       C_elem[i][j] = B*A_elem[i][j];

}

void mul(double A, matrix_struct *B, matrix_struct *C)
{
int i,j,rows,cols;
double **B_elem, **C_elem;

rows = B->rows;
cols = B->cols;
set_size(rows,cols,C);

B_elem = B->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
  for (j = 0; j < cols; j++)
       C_elem[i][j] = A*B_elem[i][j];
}

void add(matrix_struct *A, matrix_struct *B, matrix_struct *C)
{
int i,j,rows,cols;
double **A_elem,**B_elem,**C_elem;

if (A->rows != B->rows || A->cols != B->cols)
   {
     mexPrintf("error in 'add(A,B,C)':  rows and cols of A and B must be the same\n");
     mexPrintf("   in this case, A->rows = %d, B->rows = %d\n",A->rows,B->rows);
     mexPrintf("   in this case, A->cols = %d, B->cols = %d\n",A->cols,B->cols);
     mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexPrintf("   matrix C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }

rows = A->rows;
cols = A->cols;

set_size(rows,cols,C);

A_elem = A->elem;
B_elem = B->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      C_elem[i][j] = A_elem[i][j] + B_elem[i][j];
}

void add(matrix_struct *A, double B, matrix_struct *C)
{
int i,j,rows,cols;
double **A_elem,**C_elem;

rows = A->rows;
cols = A->cols;

set_size(rows,cols,C);

A_elem = A->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      C_elem[i][j] = A_elem[i][j] + B;
}

void add(double A, matrix_struct *B, matrix_struct *C)
{
int i,j,rows,cols;
double **B_elem,**C_elem;

rows = B->rows;
cols = B->cols;

set_size(rows,cols,C);

B_elem = B->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      C_elem[i][j] = B_elem[i][j] + A;
}


void sub(matrix_struct *A, matrix_struct *B, matrix_struct *C)
{
int i,j,rows,cols;
double **A_elem,**B_elem,**C_elem;

if (A->rows != B->rows || A->cols != B->cols)
   {
     mexPrintf("error in 'add(A,B,C)':  rows and cols of A and B must be the same\n");
     mexPrintf("   in this case, A->rows = %d, B->rows = %d\n",A->rows,B->rows);
     mexPrintf("   in this case, A->cols = %d, B->cols = %d\n",A->cols,B->cols);
     mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexPrintf("   matrix C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }

rows = A->rows;
cols = A->cols;

set_size(rows,cols,C);

A_elem = A->elem;
B_elem = B->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      C_elem[i][j] = A_elem[i][j] - B_elem[i][j];
}

void sub(matrix_struct *A, double B, matrix_struct *C)
{
int i,j,rows,cols;
double **A_elem,**C_elem;

rows = A->rows;
cols = A->cols;

set_size(rows,cols,C);

A_elem = A->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      C_elem[i][j] = A_elem[i][j] - B;
}

void sub(double A, matrix_struct *B, matrix_struct *C)
{
int i,j,rows,cols;
double **B_elem,**C_elem;

rows = B->rows;
cols = B->cols;

set_size(rows,cols,C);

B_elem = B->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      C_elem[i][j] = A - B_elem[i][j];
}


///////////////// Integer Matrix structure routines

void trans(int_matrix_struct *A, int_matrix_struct *B)
{
int i,j,rows,cols;
int **A_elem, **B_elem;

if (A == B)
   {
   mexPrintf("error in 'trans(A,B)':  matrix B must be distinct from matrix A\n");
   mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");   
   }

rows = A->rows;
cols = A->cols;

set_size(cols,rows,B);
A_elem = A->elem;
B_elem = B->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      B_elem[j][i] = A_elem[i][j];
}

int_matrix_struct *init_int_matrix_struct(const char *module_name, const char *name)   
{   
int_matrix_struct *A;   
   
if ((A = (int_matrix_struct *) malloc(sizeof(int_matrix_struct))) == NULL)   
   {   
   mexPrintf("error in 'init_matrix_struct':  malloc call failed\n");   
   mexPrintf("out of memory!\n");
   mexErrMsgTxt("Exiting CppSim Mex object...");   
  }   

int i;
for (i = 0; module_name[i] != '\0'; i++);
   
if ((A->module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'init_matrix_struct':  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  module_name = '%s'\n",module_name);
    mexPrintf("  desired module_name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(A->module_name,module_name);

for (i = 0; name[i] != '\0'; i++);
if ((A->name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    mexPrintf("error in 'init_matrix_struct':  calloc call failed - \n");   
    mexPrintf("  not enough memory!!\n");
    mexPrintf("  name = '%s'\n",name);
    mexPrintf("  desired name length = %d\n",i+1);   
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
strcpy(A->name,name);

A->rows = 0;
A->cols = 0;
A->freeze_length_flag = 0;
A->elem = NULL;

return(A);   
}   

int read_elem(int row, int col, int_matrix_struct *A)
{

if (row < 0 || col < 0)
    {
      mexPrintf("error in 'read_elem':  row and col must be >= 0\n");
      mexPrintf("  in this case, for matrix '%s':\n",A->name);
      mexPrintf("  -->  row = %d, col = %d\n",row,col);
      mexPrintf("  originating module name of matrix = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }

if (row >= A->rows || col >= A->cols)
    {
      mexPrintf("error in 'read_elem':  row must be < A->rows and col must be < A->cols\n");
      mexPrintf("  in this case, for matrix '%s':\n",A->name);
      mexPrintf("  -->  row = %d, A->rows = %d --- col = %d, A->cols = %d\n",
               row, A->rows, col, A->cols);
      mexPrintf("  originating module name of matrix = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }

return(A->elem[row][col]);
}   

void write_elem(int val, int row, int col, int_matrix_struct *A)
{
if (row < 0 || col < 0)
    {
      mexPrintf("error in 'write_elem':  row and col must be >= 0\n");
      mexPrintf("  in this case, for matrix '%s':\n",A->name);
      mexPrintf("  -->  row = %d, col = %d\n",row,col);
      mexPrintf("  originating module name of matrix = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }

if (row >= A->rows || col >= A->cols)
    {
      mexPrintf("error in 'write_elem':  row must be < A->rows and col must be < A->cols\n");
      mexPrintf("  in this case, for matrix '%s':\n",A->name);
      mexPrintf("  -->  row = %d, A->rows = %d --- col = %d, A->cols = %d\n",
               row, A->rows, col, A->cols);
      mexPrintf("  originating module name of matrix = '%s'\n",A->module_name);   
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }

A->elem[row][col] = val;
}   


void free_matrix_struct(int_matrix_struct *x)
{
int i;

if (x->elem != NULL)
  {
   for (i = 0; i < x->rows; i++)
      free(x->elem[i]);   
   free(x->elem);
  }
free(x->name);
free(x->module_name);
free(x);
}


void save(const char *filename, int_matrix_struct *A)
{
save_struct(filename,A);
}

void load(const char *filename, int_matrix_struct *A)
{
load_struct(filename,A);
}

void copy(int_matrix_struct *x, int_matrix_struct *y)
{
int i,j,rows,cols;
int **x_elem,**y_elem;

rows = x->rows;
cols = x->cols;

set_size(rows,cols,y);

x_elem = x->elem;
y_elem = y->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
       y_elem[i][j] = x_elem[i][j];
}


void print(int_matrix_struct *x)
{
int i,j;

mexPrintf("matrix name = '%s' (originated in module '%s')\n",
	x->name, x->module_name);

for (i = 0; i < x->rows; i++)
  {
   mexPrintf("  %s[%d][0:%d] =",x->name,i,x->cols-1);
   for (j = 0; j < x->cols; j++)
       mexPrintf(" %d",x->elem[i][j]);
   mexPrintf("\n");
  }
}


int_matrix_struct *extract_matrix_struct(const IntMatrix &in)
{
if (in.mat == NULL)
    {
      mexPrintf("error in 'extract_matrix_struct': mat is NULL!\n");
      mexPrintf("  this shouldn't happen in normal use of matrices\n");
      mexPrintf("  name of matrix = '%s'\n",in.name);
      mexPrintf("      (originating module of matrix = '%s')\n",in.module_name);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }

// transfer names for error checking

in.mat->name = in.name;
in.mat->module_name = in.module_name;
return(in.mat);
}


void set_size(int rows, int cols, int_matrix_struct *mat)
{
int i,j;

if (mat->rows == rows && mat->cols == cols)
   return;
else if (mat->freeze_length_flag == 1)
   {
    mexPrintf("error in 'Matrix::set_size':  cannot change the size of a\n");
    mexPrintf("   matrix declared in the 'outputs:' section of a module in the modules.par file\n");
    mexPrintf("   EXCEPT in the 'init_output_vectors:' section of the modules.par file\n");
    mexPrintf("   in this case, you tried changing the size of matrix '%s'\n",
               mat->name);
    mexPrintf("     (originating module of matrix = '%s')\n",mat->module_name);
    mexPrintf("-> cur matrix size = %d by %d, desired size = %d by %d\n",
	   mat->rows, mat->cols, rows, cols);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (rows < 0 || cols < 0)
   {
    mexPrintf("error in 'Matrix::set_size':  rows and cols must be >= 0\n");
    mexPrintf("  in this case, rows = %d, cols = %d\n",rows,cols);
    mexPrintf("  and you ran set_size() on matrix '%s'\n",mat->name);
    mexPrintf("     (originating module of matrix = '%s')\n",mat->module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }   

if (mat->rows != 0)
  {
    for (i = 0; i < mat->rows; i++)   
        free(mat->elem[i]);   
    free(mat->elem);
    mat->elem = NULL;   
  }

if (rows > 0 && cols > 0)
   {
   if ((mat->elem = (int **) calloc(rows,sizeof(int *))) == NULL)   
      {   
      mexPrintf("error in 'Matrix::set_size':  calloc call failed - \n");   
      mexPrintf("  not enough memory!!\n");
      mexPrintf("  matrix name = '%s', desired rows = %d\n",mat->name,rows);   
      mexPrintf("     (originating module name of matrix = '%s')\n",
                mat->module_name);
      mexErrMsgTxt("Exiting CppSim Mex object...");   
      }
   for (i = 0; i < rows; i++)   
      if (((mat->elem)[i] = (int *) calloc(cols,sizeof(int))) == NULL)
         {   
         mexPrintf("error in 'Matrix::set_size':  calloc call failed - \n");   
         mexPrintf("  not enough memory!!\n");
         mexPrintf("  matrix name = '%s', desired cols = %d\n",mat->name,cols);
         mexPrintf("   (originating module name of matrix = '%s')\n",
                mat->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");   
         }   

   for (i = 0; i < rows; i++)
      for (j = 0; j < cols; j++)
        mat->elem[i][j] = 0;
   }
mat->rows = rows;
mat->cols = cols;
}


void save_struct(const char *filename, int_matrix_struct *A)   
{   
FILE *fp;   
int i,j;
char control[10];

/* control:  parameter that allows the output file to be written
             or appended, and also to attach labels to the matrix
             data stored.  In the past, this parameter was rarely
             used for anything other than "w", and is therefore
             always set to that case now
*/
sprintf(control,"w");
   
fp = NULL; // initialize so that gcc won't complain

if (strcmp(control,"w") == 0 || strcmp(control,"wl") == 0)   
  {   
   if ((fp = fopen(filename,"w")) == NULL)   
     {   
      mexPrintf("error in 'save_struct':  file %s cannot be opened\n",filename);   
      mexErrMsgTxt("Exiting CppSim Mex object...");   
    }   
   if (strcmp(control,"wl") == 0)   
           fprintf(fp,"%s\n",A->name);   
   for (i = 0; i < A->rows; i++)   
     {   
      for (j = 0; j < A->cols; j++)   
          fprintf(fp,"%d ",A->elem[i][j]);   
      fprintf(fp,"\n");   
    }   
 }   
else if (strcmp(control,"a") == 0 || strcmp(control,"al") == 0)   
  {   
   if ((fp = fopen(filename,"a")) == NULL)   
     {   
      mexPrintf("error in 'save_struct':  file %s cannot be opened\n",filename);   
      mexErrMsgTxt("Exiting CppSim Mex object...");   
    }   
   if (strcmp(control,"al") == 0)   
      fprintf(fp,"%s\n",A->name);   
   for (i = 0; i < A->rows; i++)   
     {   
      for (j = 0; j < A->cols; j++)   
          fprintf(fp,"%d ",A->elem[i][j]);   
      fprintf(fp,"\n");   
    }   
 }   
else   
  {   
   mexPrintf("error in 'save_struct':  control character was specified as %s,\n",   
           control);   
   mexPrintf("which is invalid since it must be specified as either 'w' (for\n");   
   mexPrintf("writing to a file), or 'a' (for appending to a file),\n");   
   mexPrintf("or 'wl' (write and label matrix in file), or 'al'\n");   
   mexPrintf("(append and label matrix to file)\n");   
   mexErrMsgTxt("Exiting CppSim Mex object...");   
 }   
fclose(fp);   
}   


void load_struct(const char *filename,int_matrix_struct *A)   
{   
FILE *fp;   
int i,j,entry_count,row_count,col_count,char_count,temp_col_count,flag;   
int file_segment_flag,dummy_count,line_count;   
char line[MAX_FILE_CHAR+1],name[MAX_FILE_CHAR+1];   
float dummy_var;

char task[30];
int rows,cols;   
   
if ((fp = fopen(filename,"r")) == NULL)   
  {   
   mexPrintf("\nerror in 'load': file %s can't be opened\n",filename);   
   mexErrMsgTxt("Exiting CppSim Mex object...");   
 }   

/* task: can be a matrix name or "exclusive", "exclusive" meaning that
         only one matrix is contained in the file.
         this parameter was rarely used, and is now always set to "exclusive"
*/
sprintf(task,"exclusive");
/* rows/cols:  can be used to limit the number of data points retrieved
               from the file to what's set by these values.
               these parameters were rarely used in the past, and are now
               always set to 0 so that the all of the points are obtained
               from the file
*/
rows = 0;
cols = 0;

if (strcmp(task,"exclusive") == 0)   
   file_segment_flag = 2;   
else   
   file_segment_flag = 0;   
if (rows <= 0 || cols <= 0)   
  {   
   col_count = row_count = line_count = 0;   
   while(1)   
     {   
      if (fgets(line,MAX_FILE_CHAR+1,fp) != NULL)   
	{   
         line_count++;   
         if (sscanf(line,"%f",&dummy_var) == 0)   
	   {   
            if (sscanf(line,"%s",name) != 0)   
              {   
               if (file_segment_flag == 2)   
                  continue;   
               else if (strcmp(task,name) == 0)   
	         {   
                  file_segment_flag = 1;   
                  continue;   
	         }   
               else   
                   file_segment_flag = 0;   
	     }   
	  }   
         if (file_segment_flag)   
	   {   
            char_count = temp_col_count = flag = 0;   
            while (line[char_count] != '\0' && line[char_count] != '\n')   
	      {   
               if (line[char_count] != ' ' && flag == 0)   
	         {   
                  temp_col_count += 1;   
                  flag = 1;   
	        }   
               if (line[char_count] == ' ' && flag == 1)   
                  flag = 0;   
               char_count++;   
	     }   
            if (col_count == 0)   
               col_count = temp_col_count;   
            else if (col_count != temp_col_count && (rows <= 0 || cols <= 0)   
                      && temp_col_count != 0)   
	         {   
                  mexPrintf("error in 'load':\n\n");   
                  mexPrintf("  file '%s':  number of cols needs to be the same in each row\n",filename);   
                  mexPrintf("              in order to fill matrix %s\n",A->name);   
                  mexPrintf("(this condition is violated on line %d of file)\n",   
                          line_count);   
                  mexErrMsgTxt("Exiting CppSim Mex object...");   
	        }   
            if (temp_col_count > 0)   
               row_count++;   
	  }   
       }   
     else   
        break;   
    }   
   rows = row_count;   
   cols = col_count;   
   rewind(fp);   
 }   
      set_size(rows,cols,A);   
      entry_count = 0;   
      if (file_segment_flag != 2)   
         file_segment_flag = 0;   
      for (i = 0; i < A->rows; i++)   
         for (j = 0; j < A->cols; j++)   
           {   
            if (fscanf(fp,"%s",name) == EOF)   
               {   
               mexPrintf("\nerror in 'load':  matrix %s needs %d values,\n",   
                       A->name,rows*cols);    
               mexPrintf("but file %s has only %d data values to fill it with\n",   
                       filename,entry_count);   
               mexErrMsgTxt("Exiting CppSim Mex object...");   
             }   
            if (atof(name) == 0)   
	      {   
               flag = 0;   
               for (dummy_count = 0; dummy_count < MAX_FILE_CHAR &&    
                    name[dummy_count] != '\0' && name[dummy_count] != '\n';   
                    dummy_count++)   
		 {   
                  if (name[dummy_count] == '0')   
		    {   
                     if (dummy_count == 0)   
                         flag = 1;   
                     else if (name[dummy_count-1] == ' ' ||   
                              name[dummy_count-1] == '.' ||   
                              name[dummy_count-1] == '-')   
                         flag = 1;   
                     break;   
		    }   
                  else   
                      continue;   
		}   
               if (flag == 0)   
		 {   
                  j--;   
                  if (file_segment_flag == 2)   
                     continue;   
                  else if (strcmp(task,name) == 0)   
	            {   
                     file_segment_flag = 1;   
                     continue;   
	            }   
                  else   
                    {   
                     file_segment_flag = 0;    
                     continue;   
		   }   
		}   
	     }   
           if (file_segment_flag != 0)   
	     {   
              A->elem[i][j] = (int) floor(atof(name)+0.5);   
              entry_count++;   
	     }   
           else   
               j--;   
	  }   
   
fclose(fp);   
}   

void mul(int_matrix_struct *A, int_matrix_struct *B, int_matrix_struct *C)
{
int i,j,k,rows,cols,span;
int **A_elem,**B_elem,**C_elem;
int temp;

if (A->cols != B->rows)
   {
     mexPrintf("error in 'mul(A,B,C)':  matrix A->cols must equal B->rows\n");
     mexPrintf("   in this case, A->cols = %d, B->rows = %d\n",A->cols,B->rows);
     mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexPrintf("   matrix C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if (C == A || C == B)
   {
     mexPrintf("error in 'mul(A,B,C)':  matrix C must be distinct from matrix A and B\n");
     mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexPrintf("   matrix C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");   
   }

rows = A->rows;
cols = B->cols;
span = A->cols;
set_size(rows,cols,C);

A_elem = A->elem;
B_elem = B->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
  {
  for (j = 0; j < cols; j++)
     {
       temp = 0;
       for (k = 0; k < span; k++)
	 {
	  temp += A_elem[i][k]*B_elem[k][j];
	 }
       C_elem[i][j] = temp;
     }
  } 
}

void mul(int_matrix_struct *A, int B, int_matrix_struct *C)
{
int i,j,rows,cols;
int **A_elem, **C_elem;

rows = A->rows;
cols = A->cols;
set_size(rows,cols,C);

A_elem = A->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
  for (j = 0; j < cols; j++)
       C_elem[i][j] = B*A_elem[i][j];

}

void mul(int A, int_matrix_struct *B, int_matrix_struct *C)
{
int i,j,rows,cols;
int **B_elem, **C_elem;

rows = B->rows;
cols = B->cols;
set_size(rows,cols,C);

B_elem = B->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
  for (j = 0; j < cols; j++)
       C_elem[i][j] = A*B_elem[i][j];

}

void add(int_matrix_struct *A, int_matrix_struct *B, int_matrix_struct *C)
{
int i,j,rows,cols;
int **A_elem,**B_elem,**C_elem;

if (A->rows != B->rows || A->cols != B->cols)
   {
     mexPrintf("error in 'add(A,B,C)':  rows and cols of A and B must be the same\n");
     mexPrintf("   in this case, A->rows = %d, B->rows = %d\n",A->rows,B->rows);
     mexPrintf("   in this case, A->cols = %d, B->cols = %d\n",A->cols,B->cols);
     mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexPrintf("   matrix C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }

rows = A->rows;
cols = A->cols;

set_size(rows,cols,C);

A_elem = A->elem;
B_elem = B->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      C_elem[i][j] = A_elem[i][j] + B_elem[i][j];
}

void add(int_matrix_struct *A, int B, int_matrix_struct *C)
{
int i,j,rows,cols;
int **A_elem,**C_elem;

rows = A->rows;
cols = A->cols;

set_size(rows,cols,C);

A_elem = A->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      C_elem[i][j] = A_elem[i][j] + B;
}

void add(int A, int_matrix_struct *B, int_matrix_struct *C)
{
int i,j,rows,cols;
int **B_elem,**C_elem;

rows = B->rows;
cols = B->cols;

set_size(rows,cols,C);

B_elem = B->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      C_elem[i][j] = B_elem[i][j] + A;
}


void sub(int_matrix_struct *A, int_matrix_struct *B, int_matrix_struct *C)
{
int i,j,rows,cols;
int **A_elem,**B_elem,**C_elem;

if (A->rows != B->rows || A->cols != B->cols)
   {
     mexPrintf("error in 'add(A,B,C)':  rows and cols of A and B must be the same\n");
     mexPrintf("   in this case, A->rows = %d, B->rows = %d\n",A->rows,B->rows);
     mexPrintf("   in this case, A->cols = %d, B->cols = %d\n",A->cols,B->cols);
     mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexPrintf("   matrix C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }

rows = A->rows;
cols = A->cols;

set_size(rows,cols,C);

A_elem = A->elem;
B_elem = B->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      C_elem[i][j] = A_elem[i][j] - B_elem[i][j];
}

void sub(int_matrix_struct *A, int B, int_matrix_struct *C)
{
int i,j,rows,cols;
int **A_elem,**C_elem;

rows = A->rows;
cols = A->cols;

set_size(rows,cols,C);

A_elem = A->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      C_elem[i][j] = A_elem[i][j] - B;
}

void sub(int A, int_matrix_struct *B, int_matrix_struct *C)
{
int i,j,rows,cols;
int **B_elem,**C_elem;

rows = B->rows;
cols = B->cols;

set_size(rows,cols,C);

B_elem = B->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      C_elem[i][j] = A - B_elem[i][j];
}

void real_to_int(matrix_struct *in, int_matrix_struct *out)
{
int i,j,rows,cols;
double **in_elem;
int **out_elem;

rows = in->rows;
cols = in->cols;

set_size(rows,cols,out);
in_elem = in->elem;
out_elem = out->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      out_elem[i][j] = (int) floor(in_elem[i][j] + 0.5);
}

void int_to_real(int_matrix_struct *in, matrix_struct *out)
{
int i,j,rows,cols;
int **in_elem;
double **out_elem;

rows = in->rows;
cols = in->cols;

set_size(rows,cols,out);
in_elem = in->elem;
out_elem = out->elem;

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
      out_elem[i][j] = (double) in_elem[i][j];
}


///////////////////// Vector and Matrix functions /////////////////////

void least_sq(const Matrix &A, const Vector &B, const Vector &X)
{
matrix_struct *a;
vector_struct *b,*x;

a = extract_matrix_struct(A);
b = extract_vector_struct(B);
x = extract_vector_struct(X);

least_sq(a,b,x);
}

void copy(int index, const char *row_or_col, Matrix &from, Vector &to)
{
matrix_struct *from_mat;
vector_struct *to_vec;

from_mat = extract_matrix_struct(from);
to_vec = extract_vector_struct(to);

copy(index, row_or_col, from_mat, to_vec);
}

void copy(int index, const char *row_or_col, matrix_struct *from, vector_struct *to)
{
int i,rows,cols;
double **mat_elem,*vec_elem;


if (strncmp(row_or_col,"col",3) == 0)
   {
    if (index < 0)
       {
	 mexPrintf("error in 'copy(index,\"col\",Matrix,Vector):\n");
         mexPrintf("   index must be >= 0\n");
         mexPrintf("   in this case, index = %d\n",index);
	 mexPrintf("     name of Vector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of Matrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
       }
    if (index >= from->cols)
      {
	 mexPrintf("error in 'copy(index,\"col\",Matrix,Vector):\n");
         mexPrintf("   index must be < Matrix cols\n");
         mexPrintf("   in this case, index = %d, Matrix cols = %d\n",
                     index,from->cols);
	 mexPrintf("     name of Vector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of Matrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
      }

    rows = from->rows;

    if (to->length != rows)
      set_length(rows,to);

    vec_elem = to->elem;
    mat_elem = from->elem;

    for (i = 0; i < rows; i++)
       vec_elem[i] = mat_elem[i][index];
   }
else if (strncmp(row_or_col,"row",3) == 0)
   {
    if (index < 0)
       {
	 mexPrintf("error in 'copy(index,\"row\",Matrix,Vector):\n");
         mexPrintf("   index must be >= 0\n");
         mexPrintf("   in this case, index = %d\n",index);
	 mexPrintf("     name of Vector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of Matrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
       }
    if (index >= from->rows)
      {
	 mexPrintf("error in 'copy(index,\"row\",Matrix,Vector):\n");
         mexPrintf("   index must be < Matrix rows\n");
         mexPrintf("   in this case, index = %d, Matrix rows = %d\n",
                     index,from->rows);
	 mexPrintf("     name of Vector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of Matrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
      }

    cols = from->cols;

    if (to->length != cols)
      set_length(cols,to);

    vec_elem = to->elem;
    mat_elem = from->elem;

    for (i = 0; i < cols; i++)
       vec_elem[i] = mat_elem[index][i];
   }
else
   {
   mexPrintf("error in 'Vector::copy(index,\"row/col\",Matrix):\n");
   mexPrintf("   \"row/col\" must be either \"row\" or \"col\"\n");
   mexPrintf("   in this case, \"row/col\" = \"%s\"\n",row_or_col);
   mexPrintf("     name of Vector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
   mexPrintf("     name of Matrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");
   }
}

void copy(int index, const char *row_or_col, Vector &from, Matrix &to)
{
matrix_struct *to_mat;
vector_struct *from_vec;

to_mat = extract_matrix_struct(to);
from_vec = extract_vector_struct(from);

copy(index, row_or_col, from_vec, to_mat);
}

void copy(int index, const char *row_or_col, vector_struct *from, matrix_struct *to)
{
int i,rows,cols;
double **mat_elem,*vec_elem;


if (strncmp(row_or_col,"col",3) == 0)
   {
    if (index < 0)
       {
	 mexPrintf("error in 'copy(index,\"col\",Vector,Matrix):\n");
         mexPrintf("   index must be >= 0\n");
         mexPrintf("   in this case, index = %d\n",index);
	 mexPrintf("     name of Matrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of Vector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
       }
    if (index >= to->cols)
      {
	 mexPrintf("error in 'copy(index,\"col\",Vector,Matrix):\n");
         mexPrintf("   index must be < Matrix cols\n");
         mexPrintf("   in this case, index = %d, Matrix cols = %d\n",
                     index,to->cols);
	 mexPrintf("     name of Matrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of Vector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
      }
    if (from->length != to->rows)
       {
	 mexPrintf("error in 'copy(index,\"col\",Vector,Matrix):\n");
         mexPrintf("   Vector length must equal Matrix rows\n");
         mexPrintf("   in this case, Vector length = %d, Matrix rows = %d\n",
                  from->length, to->rows);
	 mexPrintf("     name of Matrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of Vector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
       }

    vec_elem = from->elem;
    mat_elem = to->elem;
    rows = to->rows;

    for (i = 0; i < rows; i++)
       mat_elem[i][index] = vec_elem[i];
   }
else if (strncmp(row_or_col,"row",3) == 0)
   {
    if (index < 0)
       {
	 mexPrintf("error in 'copy(index,\"row\",Vector,Matrix):\n");
         mexPrintf("   index must be >= 0\n");
         mexPrintf("   in this case, index = %d\n",index);
	 mexPrintf("     name of Matrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of Vector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
       }
    if (index >= to->rows)
      {
	 mexPrintf("error in 'copy(index,\"row\",Vector,Matrix):\n");
         mexPrintf("   index must be < Matrix rows\n");
         mexPrintf("   in this case, index = %d, Matrix rows = %d\n",
                     index,to->rows);
	 mexPrintf("     name of Matrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of Vector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
      }
    if (from->length != to->cols)
       {
	 mexPrintf("error in 'copy(index,\"row\",Vector,Matrix):\n");
         mexPrintf("   Vector length must equal Matrix cols\n");
         mexPrintf("   in this case, Vector length = %d, Matrix cols = %d\n",
                  from->length, to->cols);
	 mexPrintf("     name of Matrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of Vector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
       }

    vec_elem = from->elem;
    mat_elem = to->elem;
    cols = to->cols;

    for (i = 0; i < cols; i++)
       mat_elem[index][i] = vec_elem[i];
   }
else
   {
   mexPrintf("error in 'copy(index,\"row/col\",Vector,Matrix):\n");
   mexPrintf("   \"row/col\" must be either \"row\" or \"col\"\n");
   mexPrintf("   in this case, \"row/col\" = \"%s\"\n",row_or_col);
   mexPrintf("     name of Matrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
   mexPrintf("     name of Vector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");
   }
}

void mul(matrix_struct *A, vector_struct *B, vector_struct *C)
{
int i,k,rows,span;
double **A_elem,*B_elem,*C_elem;
double temp;

if (A->cols != B->length)
   {
     mexPrintf("error in 'mul(A,B,C)':  matrix A->cols must equal B->length\n");
     mexPrintf("   in this case, A->cols = %d, B->length = %d\n",A->cols,B->length);
     mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   vector B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexPrintf("   vector C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if (C == B)
   {
     mexPrintf("error in 'mul(A,B,C)':  vector C must be distinct from vector B\n");
     mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   vector B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexPrintf("   vector C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");   
   }

rows = A->rows;
span = A->cols;
set_length(rows,C);

A_elem = A->elem;
B_elem = B->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
  {
   temp = 0.0;
   for (k = 0; k < span; k++)
       temp += A_elem[i][k]*B_elem[k];
   C_elem[i] = temp;
  } 
}

void mul(vector_struct *A, matrix_struct *B, vector_struct *C)
{
int j,k,cols,span;
double *A_elem,**B_elem,*C_elem;
double temp;

if (A->length != B->rows)
   {
     mexPrintf("error in 'mul(A,B,C)':  matrix A->length must equal B->rows\n");
     mexPrintf("   in this case, A->length = %d, B->rows = %d\n",A->length,
            B->rows);
     mexPrintf("   vector A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexPrintf("   vector C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if (C == A)
   {
     mexPrintf("error in 'mul(A,B,C)':  vector C must be distinct from vector A\n");
     mexPrintf("   vector A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexPrintf("   vector C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");   
   }

cols = B->cols;
span = B->rows;
set_length(cols,C);

A_elem = A->elem;
B_elem = B->elem;
C_elem = C->elem;

for (j = 0; j < cols; j++)
  {
   temp = 0.0;
   for (k = 0; k < span; k++)
       temp += A_elem[k]*B_elem[k][j];
   C_elem[j] = temp;
  } 
}

//////////////// Integer Matrix/Vector routines //////////////////

void copy(int index, const char *row_or_col, IntVector &from, IntMatrix &to)
{
int_matrix_struct *to_mat;
int_vector_struct *from_vec;

from_vec = extract_vector_struct(from);
to_mat = extract_matrix_struct(to);

copy(index, row_or_col, from_vec, to_mat);
}

void copy(int index, const char *row_or_col, int_vector_struct *from, 
          int_matrix_struct *to)
{
int i,rows,cols;
int **mat_elem,*vec_elem;


if (strncmp(row_or_col,"col",3) == 0)
   {
    if (index < 0)
       {
	 mexPrintf("error in 'copy(index,\"col\",IntVector,IntMatrix):\n");
         mexPrintf("   index must be >= 0\n");
         mexPrintf("   in this case, index = %d\n",index);
	 mexPrintf("     name of IntMatrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of IntVector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
       }
    if (index >= to->cols)
      {
	 mexPrintf("error in 'copy(index,\"col\",IntVector,IntMatrix):\n");
         mexPrintf("   index must be < IntMatrix cols\n");
         mexPrintf("   in this case, index = %d, IntMatrix cols = %d\n",
                     index,to->cols);
	 mexPrintf("     name of IntMatrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of IntVector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
      }
    if (from->length != to->rows)
       {
	 mexPrintf("error in 'copy(index,\"col\",IntVector,IntMatrix):\n");
         mexPrintf("   IntVector length must equal IntMatrix rows\n");
         mexPrintf("   in this case, IntVector length = %d, IntMatrix rows = %d\n",
                  from->length, to->rows);
	 mexPrintf("     name of IntMatrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of IntVector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
       }

    vec_elem = from->elem;
    mat_elem = to->elem;
    rows = to->rows;

    for (i = 0; i < rows; i++)
       mat_elem[i][index] = vec_elem[i];
   }
else if (strncmp(row_or_col,"row",3) == 0)
   {
    if (index < 0)
       {
	 mexPrintf("error in 'copy(index,\"row\",IntVector,IntMatrix):\n");
         mexPrintf("   index must be >= 0\n");
         mexPrintf("   in this case, index = %d\n",index);
	 mexPrintf("     name of IntMatrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of IntVector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
       }
    if (index >= to->rows)
      {
	 mexPrintf("error in 'copy(index,\"row\",IntVector,IntMatrix):\n");
         mexPrintf("   index must be < IntMatrix rows\n");
         mexPrintf("   in this case, index = %d, IntMatrix rows = %d\n",
                     index,to->rows);
	 mexPrintf("     name of IntMatrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of IntVector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
      }
    if (from->length != to->cols)
       {
	 mexPrintf("error in 'copy(index,\"row\",IntVector,IntMatrix):\n");
         mexPrintf("   IntVector length must equal IntMatrix cols\n");
         mexPrintf("   in this case, IntVector length = %d, IntMatrix cols = %d\n",
                  from->length, to->cols);
	 mexPrintf("     name of IntMatrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of IntVector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
       }

    vec_elem = from->elem;
    mat_elem = to->elem;
    cols = to->cols;

    for (i = 0; i < cols; i++)
       mat_elem[index][i] = vec_elem[i];
   }
else
   {
   mexPrintf("error in 'copy(index,\"row/col\",IntVector,IntMatrix):\n");
   mexPrintf("   \"row/col\" must be either \"row\" or \"col\"\n");
   mexPrintf("   in this case, \"row/col\" = \"%s\"\n",row_or_col);
   mexPrintf("     name of IntMatrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
   mexPrintf("     name of IntVector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");
   }
}

void copy(int index, const char *row_or_col, IntMatrix &from, IntVector &to)
{
int_matrix_struct *from_mat;
int_vector_struct *to_vec;

from_mat = extract_matrix_struct(from);
to_vec = extract_vector_struct(to);

copy(index, row_or_col, from_mat, to_vec);
}


void copy(int index, const char *row_or_col, int_matrix_struct *from, 
          int_vector_struct *to)
{
int i,rows,cols;
int **mat_elem,*vec_elem;

if (strncmp(row_or_col,"col",3) == 0)
   {
    if (index < 0)
       {
	 mexPrintf("error in 'copy(index,\"col\",IntMatrix,IntVector):\n");
         mexPrintf("   index must be >= 0\n");
         mexPrintf("   in this case, index = %d\n",index);
	 mexPrintf("     name of IntVector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of IntMatrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
       }
    if (index >= from->cols)
      {
	 mexPrintf("error in 'copy(index,\"col\",IntMatrix,IntVector):\n");
         mexPrintf("   index must be < IntMatrix cols\n");
         mexPrintf("   in this case, index = %d, IntMatrix cols = %d\n",
                     index,from->cols);
	 mexPrintf("     name of IntVector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of IntMatrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
      }

    rows = from->rows;

    if (to->length != rows)
      set_length(rows,to);

    vec_elem = to->elem;
    mat_elem = from->elem;

    for (i = 0; i < rows; i++)
       vec_elem[i] = mat_elem[i][index];
   }
else if (strncmp(row_or_col,"row",3) == 0)
   {
    if (index < 0)
       {
	 mexPrintf("error in 'copy(index,\"row\",IntMatrix,IntVector):\n");
         mexPrintf("   index must be >= 0\n");
         mexPrintf("   in this case, index = %d\n",index);
	 mexPrintf("     name of IntVector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of IntMatrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
       }
    if (index >= from->rows)
      {
	 mexPrintf("error in 'copy(index,\"row\",IntMatrix,IntVector):\n");
         mexPrintf("   index must be < IntMatrix rows\n");
         mexPrintf("   in this case, index = %d, IntMatrix rows = %d\n",
                     index,from->rows);
	 mexPrintf("     name of IntVector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         mexPrintf("     name of IntMatrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         mexErrMsgTxt("Exiting CppSim Mex object...");
      }
    cols = from->cols;

    if (to->length != cols)
      set_length(cols,to);

    vec_elem = to->elem;
    mat_elem = from->elem;

    for (i = 0; i < cols; i++)
       vec_elem[i] = mat_elem[index][i];
   }
else
   {
   mexPrintf("error in 'IntVector::copy(index,\"row/col\",IntMatrix):\n");
   mexPrintf("   \"row/col\" must be either \"row\" or \"col\"\n");
   mexPrintf("   in this case, \"row/col\" = \"%s\"\n",row_or_col);
   mexPrintf("     name of IntVector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
   mexPrintf("     name of IntMatrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");
   }
}

void mul(int_matrix_struct *A, int_vector_struct *B, int_vector_struct *C)
{
int i,k,rows,span;
int **A_elem,*B_elem,*C_elem;
int temp;

if (A->cols != B->length)
   {
     mexPrintf("error in 'mul(A,B,C)':  matrix A->cols must equal B->length\n");
     mexPrintf("   in this case, A->cols = %d, B->length = %d\n",A->cols,B->length);
     mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   vector B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexPrintf("   vector C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if (C == B)
   {
     mexPrintf("error in 'mul(A,B,C)':  vector C must be distinct from vector B\n");
     mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   vector B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexPrintf("   vector C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");   
   }

rows = A->rows;
span = A->cols;
set_length(rows,C);

A_elem = A->elem;
B_elem = B->elem;
C_elem = C->elem;

for (i = 0; i < rows; i++)
  {
   temp = 0;
   for (k = 0; k < span; k++)
       temp += A_elem[i][k]*B_elem[k];
   C_elem[i] = temp;
  } 
}

void mul(int_vector_struct *A, int_matrix_struct *B, int_vector_struct *C)
{
int j,k,cols,span;
int *A_elem,**B_elem,*C_elem;
int temp;

if (A->length != B->rows)
   {
     mexPrintf("error in 'mul(A,B,C)':  matrix A->length must equal B->rows\n");
     mexPrintf("   in this case, A->length = %d, B->rows = %d\n",A->length,
            B->rows);
     mexPrintf("   vector A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexPrintf("   vector C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if (C == A)
   {
     mexPrintf("error in 'mul(A,B,C)':  vector C must be distinct from vector A\n");
     mexPrintf("   vector A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     mexPrintf("   vector C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     mexErrMsgTxt("Exiting CppSim Mex object...");   
   }

cols = B->cols;
span = B->rows;
set_length(cols,C);

A_elem = A->elem;
B_elem = B->elem;
C_elem = C->elem;

for (j = 0; j < cols; j++)
  {
   temp = 0;
   for (k = 0; k < span; k++)
       temp += A_elem[k]*B_elem[k][j];
   C_elem[j] = temp;
  } 
}

///////////// Advanced matrix structure functions ///////////////

void inv(matrix_struct *A,matrix_struct *B)   
{   
matrix_struct *U,*W,*V;   
double inv_rcond,max,min;   
int i;   
   
if (A->rows != A->cols)   
  {   
   mexPrintf("error in 'inv(A,B)':  input matrix A is not square!\n");   
   mexPrintf("   in this case, A->rows = %d, A->cols = %d\n",A->rows,A->cols);
   mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");   
 }

U = init_matrix_struct("inv_function","U_matrix_used_by_inv");   
W = init_matrix_struct("inv_function","W_matrix_used_by_inv");   
V = init_matrix_struct("inv_function","V_matrix_used_by_inv");
   
svd(A,U,W,V);   

max = fabs(W->elem[0][0]);   
min = max;
for (i = 1; i < W->cols; i++)   
   {   
    if (W->elem[i][i] > max)   
         max = fabs(W->elem[i][i]);   
    if (W->elem[i][i] < min)   
         min = fabs(W->elem[i][i]);   
   }   
    
if (min <= INV_DP_SV_L*max)   
   {   
    mexPrintf("error in 'inv(A,B)':  input matrix A is singular!\n",   
               A->name);   
    mexPrintf("  (condition number = %f, minimum singular value = %f\n",   
                  max/min,min);   
    mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
    mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
    mexErrMsgTxt("Exiting CppSim Mex object...");   
   }   
if ((inv_rcond = log10(min)-log10(max)) <= INV_DP_RC_L)   
   {   
    mexPrintf("warning in 'inv(A,B)':  input matrix A is ill-conditioned!\n");   
    mexPrintf("  (matrix %s has condition number: %f)\n",A->name,max/min);   
    mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
    mexPrintf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
   }   

for (i = 0; i < W->cols; i++)   
   W->elem[i][i] = 1.0/W->elem[i][i];   

trans(U,B);
mul(W,B,U);
mul(V,U,B);

free_matrix_struct(U);
free_matrix_struct(W);
free_matrix_struct(V);

}   



/* the following defines and declarations are used by the 'svd' function */  
static double at_pythag,bt_pythag,ct_pythag;  
#define PYTHAG(a,b) ((at_pythag = fabs(a)) > (bt_pythag = fabs(b)) ? (ct_pythag = bt_pythag/at_pythag,at_pythag*sqrt(1.0+ct_pythag*ct_pythag)) : (bt_pythag ? (ct_pythag=at_pythag/bt_pythag,bt_pythag*sqrt(1.0+ct_pythag*ct_pythag)) : 0.0))  
#define MAX(a,b) ((a) > (b) ? (a) : (b))  
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))  
#define MAX_ITER 50 

void svd(matrix_struct *A, 
         matrix_struct *U, matrix_struct *W, matrix_struct *V)
{   
int flag,i,its,j,jj,k,l,nm,m,n;   
double c,f,h,s,x,y,z;   
double anorm = 0.0, g = 0.0, scale = 0.0, *rv1;   

l = 1; // initialize so that gcc won't complain   

if (A->rows < A->cols)   
  {   
   mexPrintf("error in 'svd(A,U,W,V)':  input matrix A needs to have more rows");
   mexPrintf(" than columns\n  (input matrix A is %d by %d)\n",A->rows,A->cols);
   mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   mexPrintf("   matrix U: '%s' (originating module: '%s')\n",
	    U->name,U->module_name);
   mexPrintf("   matrix W: '%s' (originating module: '%s')\n",
	    W->name,W->module_name);
   mexPrintf("   matrix V: '%s' (originating module: '%s')\n",
	    V->name,V->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");   
 }   
if (A == U || A == W || A == V)   
  {   
   mexPrintf("error in 'svd(A,U,W,V)':  input matrix A must be distinct from\n");
   mexPrintf("   matrices U, W, and V\n");
   mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   mexPrintf("   matrix U: '%s' (originating module: '%s')\n",
	    U->name,U->module_name);
   mexPrintf("   matrix W: '%s' (originating module: '%s')\n",
	    W->name,W->module_name);
   mexPrintf("   matrix V: '%s' (originating module: '%s')\n",
	    V->name,V->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");   
 }   
if (U == W || V == W || U == V)   
  {   
   mexPrintf("error in 'svd(A,U,W,V)':  matrices U, W, and V must be\n");
   mexPrintf("   distinct from each other\n");
   mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   mexPrintf("   matrix U: '%s' (originating module: '%s')\n",
	    U->name,U->module_name);
   mexPrintf("   matrix W: '%s' (originating module: '%s')\n",
	    W->name,W->module_name);
   mexPrintf("   matrix V: '%s' (originating module: '%s')\n",
	    V->name,V->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");   
 }   


set_size(A->rows,A->cols,U);
   
for (i = 0; i < A->rows; i++)   
   for (j = 0; j < A->cols; j++)   
      (U->elem)[i][j] = (A->elem)[i][j];   

set_size(A->cols,A->cols,W);   
for (i = 0; i < A->cols; i++)   
   for (j = 0; j < A->cols; j++)   
       (W->elem)[i][j] = 0.0;   
set_size(A->cols,A->cols,V);   
if ((rv1 = (double *) calloc(A->cols,sizeof(double))) == NULL)   
  {   
   mexPrintf("error in 'svd(A,U,W,V)':  calloc call failed\n");   
   mexPrintf(" - not enough memory !\n");   
   mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   mexPrintf("   matrix U: '%s' (originating module: '%s')\n",
	    U->name,U->module_name);
   mexPrintf("   matrix W: '%s' (originating module: '%s')\n",
	    W->name,W->module_name);
   mexPrintf("   matrix V: '%s' (originating module: '%s')\n",
	    V->name,V->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");   
 }   

for (i = 0; i < U->rows; i++)   
   (U->elem)[i]--;   
for (i = 0; i < W->rows; i++)   
   (W->elem)[i]--;   
for (i = 0; i < V->rows; i++)   
   (V->elem)[i]--;   
(U->elem)--;   
(W->elem)--;   
(V->elem)--;   
rv1--;   
        m = A->rows;   
        n = A->cols;   
	for (i=1;i<=n;i++) {   
		l=i+1;   
		rv1[i]=scale*g;   
		g=s=scale=0.0;   
		if (i <= m) {   
			for (k=i;k<=m;k++) scale += fabs(U->elem[k][i]);   
			if (scale) {   
				for (k=i;k<=m;k++) {   
					U->elem[k][i] /= scale;   
					s += U->elem[k][i]*U->elem[k][i];   
				}   
				f=U->elem[i][i];   
				g = -SIGN(sqrt(s),f);   
				h=f*g-s;   
				U->elem[i][i]=f-g;   
				if (i != n) {   
					for (j=l;j<=n;j++) {   
						for (s=0.0,k=i;k<=m;k++) s += U->elem[k][i]*U->elem[k][j];   
						f=s/h;   
						for (k=i;k<=m;k++) U->elem[k][j] += f*U->elem[k][i];   
					}   
				}   
				for (k=i;k<=m;k++) U->elem[k][i] *= scale;   
			}   
		}   
		W->elem[i][i]=scale*g;   
		g=s=scale=0.0;   
		if (i <= m && i != n) {   
			for (k=l;k<=n;k++) scale += fabs(U->elem[i][k]);   
			if (scale) {   
				for (k=l;k<=n;k++) {   
					U->elem[i][k] /= scale;   
					s += U->elem[i][k]*U->elem[i][k];   
				}   
				f=U->elem[i][l];   
				g = -SIGN(sqrt(s),f);   
				h=f*g-s;   
				U->elem[i][l]=f-g;   
				for (k=l;k<=n;k++) rv1[k]=U->elem[i][k]/h;   
				if (i != m) {   
					for (j=l;j<=m;j++) {   
						for (s=0.0,k=l;k<=n;k++) s += U->elem[j][k]*U->elem[i][k];   
						for (k=l;k<=n;k++) U->elem[j][k] += s*rv1[k];   
					}   
				}   
				for (k=l;k<=n;k++) U->elem[i][k] *= scale;   
			}   
		}   
		anorm=MAX(anorm,(fabs(W->elem[i][i])+fabs(rv1[i])));   
	}   
	for (i=n;i>=1;i--) {   
		if (i < n) {   
			if (g) {   
				for (j=l;j<=n;j++)   
					V->elem[j][i]=(U->elem[i][j]/U->elem[i][l])/g;   
				for (j=l;j<=n;j++) {   
					for (s=0.0,k=l;k<=n;k++) s += U->elem[i][k]*V->elem[k][j];   
					for (k=l;k<=n;k++) V->elem[k][j] += s*V->elem[k][i];   
				}   
			}   
			for (j=l;j<=n;j++) V->elem[i][j]=V->elem[j][i]=0.0;   
		}   
		V->elem[i][i]=1.0;   
		g=rv1[i];   
		l=i;   
	}   
	for (i=n;i>=1;i--) {   
		l=i+1;   
		g=W->elem[i][i];   
		if (i < n)   
			for (j=l;j<=n;j++) U->elem[i][j]=0.0;   
		if (g) {   
			g=1.0/g;   
			if (i != n) {   
				for (j=l;j<=n;j++) {   
					for (s=0.0,k=l;k<=m;k++) s += U->elem[k][i]*U->elem[k][j];   
					f=(s/U->elem[i][i])*g;   
					for (k=i;k<=m;k++) U->elem[k][j] += f*U->elem[k][i];   
				}   
			}   
			for (j=i;j<=m;j++) U->elem[j][i] *= g;   
		} else {   
			for (j=i;j<=m;j++) U->elem[j][i]=0.0;   
		}   
		++U->elem[i][i];   
	}   
	for (k=n;k>=1;k--) {   
		for (its=1;its<=MAX_ITER;its++) {   
			flag=1;   
			for (l=k;l>=1;l--) {   
				nm=l-1;   
				if (fabs(rv1[l])+anorm == anorm) {   
					flag=0;   
					break;   
				}   
				if (fabs(W->elem[nm][nm])+anorm == anorm) break;   
			}   
			if (flag) {   
				c=0.0;   
				s=1.0;   
				for (i=l;i<=k;i++) {   
					f=s*rv1[i];   
					if (fabs(f)+anorm != anorm) {   
						g=W->elem[i][i];   
						h=PYTHAG(f,g);   
						W->elem[i][i]=h;   
						h=1.0/h;   
						c=g*h;   
						s=(-f*h);   
						for (j=1;j<=m;j++) {   
							y=U->elem[j][nm];   
							z=U->elem[j][i];   
							U->elem[j][nm]=y*c+z*s;   
							U->elem[j][i]=z*c-y*s;   
						}   
					}   
				}   
			}   
			z=W->elem[k][k];   
			if (l == k) {   
				if (z < 0.0) {   
					W->elem[k][k] = -z;   
					for (j=1;j<=n;j++) V->elem[j][k]=(-V->elem[j][k]);   
				}   
				break;   
			}   
			if (its == MAX_ITER)    
                            {   
                             mexPrintf("error in 'svd':  ");   
                             mexPrintf("no convergence in %d SVDCMP iterations\n",MAX_ITER);   
                             mexErrMsgTxt("Exiting CppSim Mex object...");   
			   }   
			x=W->elem[l][l];   
			nm=k-1;   
			y=W->elem[nm][nm];   
			g=rv1[nm];   
			h=rv1[k];   
			f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);   
			g=PYTHAG(f,1.0);   
			f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;   
			c=s=1.0;   
			for (j=l;j<=nm;j++) {   
				i=j+1;   
				g=rv1[i];   
				y=W->elem[i][i];   
				h=s*g;   
				g=c*g;   
				z=PYTHAG(f,h);   
				rv1[j]=z;   
				c=f/z;   
				s=h/z;   
				f=x*c+g*s;   
				g=g*c-x*s;   
				h=y*s;   
				y=y*c;   
				for (jj=1;jj<=n;jj++) {   
					x=V->elem[jj][j];   
					z=V->elem[jj][i];   
					V->elem[jj][j]=x*c+z*s;   
					V->elem[jj][i]=z*c-x*s;   
				}   
				z=PYTHAG(f,h);   
				W->elem[j][j]=z;   
				if (z) {   
					z=1.0/z;   
					c=f*z;   
					s=h*z;   
				}   
				f=(c*g)+(s*y);   
				x=(c*y)-(s*g);   
				for (jj=1;jj<=m;jj++) {   
					y=U->elem[jj][j];   
					z=U->elem[jj][i];   
					U->elem[jj][j]=y*c+z*s;   
					U->elem[jj][i]=z*c-y*s;   
				}   
			}   
			rv1[l]=0.0;   
			rv1[k]=f;   
			W->elem[k][k]=x;   
		      }   
	      }   
   
(U->elem)++;   
(W->elem)++;   
(V->elem)++;   
for (i = 0; i < U->rows; i++)   
   (U->elem)[i]++;   
for (i = 0; i < W->rows; i++)   
   (W->elem)[i]++;   
for (i = 0; i < V->rows; i++)   
   (V->elem)[i]++;   
rv1++;   
free(rv1);   
/* if (check_file != NULL)   
   fprintf(check_file,"'svd':  A = U*W*V'  A: %s (%d by %d), U: %s (%d by ",B->name,B->rows,B->cols,A->name,A->rows,A->cols); */   
}   

#undef MAX_ITER 


void least_sq(matrix_struct *A, matrix_struct *b, matrix_struct *x)   
{   
int jj,j,i;   
double s;   
double *tmp;   
matrix_struct *U,*W,*V;   
double max;   

if (A->rows <= A->cols)   
  {   
   mexPrintf("error in 'least_sq(A,b,x)':  input matrix A (where A*x = b)\n");   
   mexPrintf("  must have more rows than columns\n");   
   mexPrintf("  in this case, matrix A is %d by %d\n",A->rows, A->cols);
   mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   mexPrintf("   matrix b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
   mexPrintf("   matrix x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");      
  }
if (b->cols != 1)   
  {   
   mexPrintf("error in 'least_sq(A,b,x)':  input matrix b (where A*x = b)\n");   
   mexPrintf("  must be a column vector!\n");   
   mexPrintf("  in this case, matrix b is %d by %d!\n",b->rows,b->cols);   
   mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   mexPrintf("   matrix b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
   mexPrintf("   matrix x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");   
 }   
if (b->rows != A->rows)   
  {   
   mexPrintf("error in 'least_sq(A,b,x)':  input matrix b (where A*x = b)\n");   
   mexPrintf("  must have the same number of rows as A\n");   
   mexPrintf("  in this case, matrix b is %d by %d, matrix A is %d by %d\n",
	    b->rows, b->cols, A->rows, A->cols);
   mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   mexPrintf("   matrix b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
   mexPrintf("   matrix x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");   
 }

U = init_matrix_struct("least_sq_func","matrix_U");   
W = init_matrix_struct("least_sq_func","matrix_W");   
V = init_matrix_struct("least_sq_func","matrix_V");
   
if ((tmp = (double *) calloc(A->cols,sizeof(double))) == NULL)   
  {   
   mexPrintf("error in 'least_sq(A,b,x)':  calloc call failed!\n");   
   mexPrintf("    (not enough memory)\n");   
   mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   mexPrintf("   matrix b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
   mexPrintf("   matrix x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");   
 } 
  
svd(A,U,W,V);   


max = fabs(W->elem[0][0]);   
for (i = 1; i < W->cols; i++)   
    if (max < fabs(W->elem[i][i]))   
         max = fabs(W->elem[i][i]);   
for (j = 0; j < A->cols; j++)   
  {   
   s = 0.0;
   if (fabs(W->elem[j][j]) >= INV_DP_SV_L*max)
     {   
      for (i = 0; i < A->rows; i++)   
         s += U->elem[i][j]*b->elem[i][0];   
      s /= W->elem[j][j];   
     }   
   else 
     { 
      mexPrintf("warning in 'least_sq(A,b,x)':  singular values that are too small\n"); 
      mexPrintf("    have their inverse set to zero as 'A-inverse' is calculated\n"); 
      mexPrintf("   --> given the max sing. value of '%5.3e' in this case\n",max);
      mexPrintf("       the sing. value '%5.3e' was considered too small\n",
                          W->elem[j][j]); 
      mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
      mexPrintf("   matrix b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
      mexPrintf("   matrix x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
      mexPrintf("            calculation continuing....\n"); 
    }     
   tmp[j] = s;   
 }  
set_size(A->cols,1,x);    
for (j = 0; j < V->rows; j++)   
  {   
   s  = 0.0;   
   for (jj = 0; jj < V->cols; jj++)   
      s += V->elem[j][jj]*tmp[jj];   
   x->elem[j][0] = s;   
 }   

free(tmp);
free_matrix_struct(U);
free_matrix_struct(W);
free_matrix_struct(V);

}   


void least_sq(matrix_struct *A, vector_struct *b, vector_struct *x)   
{   
int jj,j,i;   
double s;   
double *tmp;   
matrix_struct *U,*W,*V;   
double max;   

if (A->rows <= A->cols)   
  {   
   mexPrintf("error in 'least_sq(A,b,x)':  input matrix A (where A*x = b)\n");   
   mexPrintf("  must have more rows than columns\n");   
   mexPrintf("  in this case, matrix A is %d by %d\n",A->rows, A->cols);
   mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   mexPrintf("   vector b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
   mexPrintf("   vector x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");      
  }
if (b->length != A->rows)   
  {   
   mexPrintf("error in 'least_sq(A,b,x)':  input vector b (where A*x = b)\n");   
   mexPrintf("  must have the same length as the rows in A\n");   
   mexPrintf("  in this case, vector b has length %d, matrix A is %d by %d\n",
	    b->length, A->rows, A->cols);
   mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   mexPrintf("   vector b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
   mexPrintf("   vector x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");   
 }

U = init_matrix_struct("least_sq_func","matrix_U");   
W = init_matrix_struct("least_sq_func","matrix_W");   
V = init_matrix_struct("least_sq_func","matrix_V");
   
if ((tmp = (double *) calloc(A->cols,sizeof(double))) == NULL)   
  {   
   mexPrintf("error in 'least_sq(A,b,x)':  calloc call failed!\n");   
   mexPrintf("    (not enough memory)\n");   
   mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   mexPrintf("   vector b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
   mexPrintf("   vector x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
   mexErrMsgTxt("Exiting CppSim Mex object...");   
 } 
  
svd(A,U,W,V);   


max = fabs(W->elem[0][0]);   
for (i = 1; i < W->cols; i++)   
    if (max < fabs(W->elem[i][i]))   
         max = fabs(W->elem[i][i]);   
for (j = 0; j < A->cols; j++)   
  {   
   s = 0.0;
   if (fabs(W->elem[j][j]) >= INV_DP_SV_L*max)
     {   
      for (i = 0; i < A->rows; i++)   
         s += U->elem[i][j]*b->elem[i];   
      s /= W->elem[j][j];   
     }   
   else 
     { 
      mexPrintf("warning in 'least_sq(A,b,x)':  singular values that are too small\n"); 
      mexPrintf("    have their inverse set to zero as 'A-inverse' is calculated\n"); 
      mexPrintf("   --> given the max sing. value of '%5.3e' in this case\n",max);
      mexPrintf("       the sing. value '%5.3e' was considered too small\n",
                          W->elem[j][j]); 
      mexPrintf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
      mexPrintf("   vector b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
      mexPrintf("   vector x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
      mexPrintf("            calculation continuing....\n"); 
    }     
   tmp[j] = s;   
 }  
set_length(A->cols,x);    
for (j = 0; j < V->rows; j++)   
  {   
   s  = 0.0;   
   for (jj = 0; jj < V->cols; jj++)   
      s += V->elem[j][jj]*tmp[jj];   
   x->elem[j] = s;   
 }   

free(tmp);
free_matrix_struct(U);
free_matrix_struct(W);
free_matrix_struct(V);

}   



///////////// used for saving signals for mex prototype ////////////
/////////////  -> included here just to match mex version //////////

MexListStream::MexListStream()
{
first_frame = init_mex_list_stream_frame();
cur_frame = first_frame;
length = 0;
}

MexListStream::~MexListStream()
{
delete_frames(first_frame);
first_frame = NULL;
cur_frame = NULL;
length = 0;
}


MexListStreamFrame *MexListStream::init_mex_list_stream_frame()
{
MexListStreamFrame *A; 
   
if ((A = (MexListStreamFrame *) calloc(1,sizeof(MexListStreamFrame))) == NULL)   
   {   
   mexPrintf("error in 'init_mex_list_stream_frame':  calloc call failed\n");   
   mexPrintf("out of memory!\n");   
   mexPrintf("Error: need to reduce length of CppSim mex frame\n");
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
A->length = 0;
A->next = NULL;
return(A);
}

void MexListStream::delete_frames(MexListStreamFrame *first_entry)
{
if (first_entry != NULL)
  {
   if (first_entry->next != NULL)
      delete_frames(first_entry->next);
   free(first_entry);
   first_entry = NULL;
  }
}

void MexListStream::copy_to_mex_array(double *array)
{
int i, cur_frame_position;
MexListStreamFrame *cur_read_frame;

cur_read_frame = first_frame;
if (cur_read_frame == NULL)
   return;

cur_frame_position = 0;
for (i = 0; i < length; i++)
  {
    array[i] = cur_read_frame->buf[cur_frame_position++];
    if (cur_frame_position == MEXLISTSTREAMFRAME_LENGTH)
      {
       cur_read_frame = cur_read_frame->next;
       cur_frame_position = 0;
       if (cur_read_frame == NULL)
	  break;
      }
  }
}


void MexListStream::inp_base(double in)
{
if (cur_frame->length == MEXLISTSTREAMFRAME_LENGTH)
  {
   cur_frame->next = init_mex_list_stream_frame();
   cur_frame = cur_frame->next;
  }
cur_frame->buf[cur_frame->length] = in;
cur_frame->length++;
length++;
}

void MexListStream::inp(double in)
{
inp_base(in);
}

void MexListStream::inp(int in)
{
inp_base((double) in);
}
void MexListStream::inp(const Vector &in)
{
int i;
vector_struct *x;

x = extract_vector_struct(in);

for (i = 0; i < x->length; i++)
  inp_base(x->elem[i]);
}

void MexListStream::inp(Vector &in)
{
int i;
vector_struct *x;

x = extract_vector_struct(in);

for (i = 0; i < x->length; i++)
  inp_base(x->elem[i]);
}

void MexListStream::inp(const IntVector &in)
{
int i;
int_vector_struct *x;

x = extract_vector_struct(in);

for (i = 0; i < x->length; i++)
  inp_base((double) x->elem[i]);
}

void MexListStream::inp(IntVector &in)
{
int i;
int_vector_struct *x;

x = extract_vector_struct(in);

for (i = 0; i < x->length; i++)
  inp_base((double) x->elem[i]);
}


///// 1/f plus thermal noise class


OneOverfPlusWhiteNoise::OneOverfPlusWhiteNoise()
  :  rand1("gauss"),
     filt1("1","1"),
     filt2("1","1"),
     filt3("1","1"),
     filt4("1","1"),
     filt5("1","1"),
     filt6("1","1"),
     filt7("1","1"),
     filt8("1","1"),
     filt9("1","1")
{ 
out = 0.0;
}


OneOverfPlusWhiteNoise::OneOverfPlusWhiteNoise(double f_corner, double a, double r, double Ts)
  :  rand1("gauss"),
     filt1("1","1"),
     filt2("1","1"),
     filt3("1","1"),
     filt4("1","1"),
     filt5("1","1"),
     filt6("1","1"),
     filt7("1","1"),
     filt8("1","1"),
     filt9("1","1")
{ 
if (f_corner < 0.0)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::OneOverfPlusWhiteNoise:  f_corner must be >= 0.0\n");
   mexPrintf("  in this case, f_corner = %5.3e\n",f_corner);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (a <= 0.01)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::OneOverfPlusWhiteNoise:  a must be > 0.01\n");
   mexPrintf("  in this case, a = %5.3e\n",a);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (r <= 0.01)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::OneOverfPlusWhiteNoise:  r must be > 0.01\n");
   mexPrintf("  in this case, r = %5.3e\n",r);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (Ts < 1e-30)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::OneOverfPlusWhiteNoise:  Ts must be >= 1e-30\n");
   mexPrintf("  in this case, Ts = %5.3e\n",Ts);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

set(f_corner, a, r, Ts);
}

OneOverfPlusWhiteNoise::OneOverfPlusWhiteNoise(double f_corner, double Ts)
  :  rand1("gauss"),
     filt1("1","1"),
     filt2("1","1"),
     filt3("1","1"),
     filt4("1","1"),
     filt5("1","1"),
     filt6("1","1"),
     filt7("1","1"),
     filt8("1","1"),
     filt9("1","1")
{ 
double a,r,slope;

slope = -10.0;
a = pow(10.0,-slope/20.0);
r = 10.0;

if (f_corner < 0.0)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::OneOverfPlusWhiteNoise:  f_corner must be >= 0.0\n");
   mexPrintf("  in this case, f_corner = %5.3e\n",f_corner);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (Ts < 1e-30)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::OneOverfPlusWhiteNoise:  Ts must be >= 1e-30\n");
   mexPrintf("  in this case, Ts = %5.3e\n",Ts);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

set(f_corner, a, r, Ts);
}

OneOverfPlusWhiteNoise::OneOverfPlusWhiteNoise(double f_corner, double slope, double Ts)
  :  rand1("gauss"),
     filt1("1","1"),
     filt2("1","1"),
     filt3("1","1"),
     filt4("1","1"),
     filt5("1","1"),
     filt6("1","1"),
     filt7("1","1"),
     filt8("1","1"),
     filt9("1","1")
{ 
double a,r;

if (f_corner < 0.0)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::OneOverfPlusWhiteNoise:  f_corner must be >= 0.0\n");
   mexPrintf("  in this case, f_corner = %5.3e\n",f_corner);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (Ts < 1e-30)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::OneOverfPlusWhiteNoise:  Ts must be >= 1e-30\n");
   mexPrintf("  in this case, Ts = %5.3e\n",Ts);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (slope > -5)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::OneOverfPlusWhiteNoise:  slope must be between -5 and -15\n");
   mexPrintf("  in this case, slope = %5.3g\n",slope);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (slope < -15)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::OneOverfPlusWhiteNoise:  slope must be between -5 and -15\n");
   mexPrintf("  in this case, slope = %5.3g\n",slope);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

a = pow(10.0,-slope/20.0);
r = 10.0;

set(f_corner, a, r, Ts);
}


void OneOverfPlusWhiteNoise::set(double f_corner, double a, double r, double Ts)
{
double k;

ignore_one_over_f_flag = 0; 
 
if (f_corner < 0.0)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::set:  f_corner must be >= 0.0\n");
   mexPrintf("  in this case, f_corner = %5.3e\n",f_corner);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
else if (f_corner < 1e-30)
   ignore_one_over_f_flag = 1;

if (a <= 0.01)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::set:  a must be > 0.01\n");
   mexPrintf("  in this case, a = %5.3e\n",a);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (r <= 0.01)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::set:  r must be > 0.01\n");
   mexPrintf("  in this case, r = %5.3e\n",r);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (Ts < 1e-30)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::set:  Ts must be >= 1e-30\n");
   mexPrintf("  in this case, Ts = %5.3e\n",Ts);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

k = 0.68; // emperical value to properly align 1/f corner

if (ignore_one_over_f_flag == 0)
   {
   filt1.set("gain","1 + 1/wp*s","gain,wp,Ts",k/a,2*pi*f_corner*r*r/a,Ts);
   filt2.set("gain","1 + 1/wp*s","gain,wp,Ts",k,2*pi*f_corner*r/a,Ts);
   filt3.set("gain","1 + 1/wp*s","gain,wp,Ts",k*a,2*pi*f_corner/a,Ts);
   filt4.set("gain","1 + 1/wp*s","gain,wp,Ts",k*a*a,2*pi*f_corner/a/r,Ts);
   filt5.set("gain","1 + 1/wp*s","gain,wp,Ts",k*a*a*a,2*pi*f_corner/a/r/r,Ts);
   filt6.set("gain","1 + 1/wp*s","gain,wp,Ts",k*a*a*a*a,2*pi*f_corner/a/r/r/r,Ts);
   filt7.set("gain","1 + 1/wp*s","gain,wp,Ts",k*a*a*a*a*a,2*pi*f_corner/a/r/r/r/r,Ts);
   filt8.set("gain","1 + 1/wp*s","gain,wp,Ts",k*a*a*a*a*a*a,2*pi*f_corner/a/r/r/r/r/r,Ts);
   filt9.set("gain","1 + 1/wp*s","gain,wp,Ts",k*a*a*a*a*a*a*a,2*pi*f_corner/a/r/r/r/r/r/r,Ts);
   }

out = 0.0;
}

void OneOverfPlusWhiteNoise::set(double f_corner, double Ts)
{ 
double a,r,slope;

if (f_corner < 0.0)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::set:  f_corner must be >= 0.0\n");
   mexPrintf("  in this case, f_corner = %5.3e\n",f_corner);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (Ts < 1e-30)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::set:  Ts must be >= 1e-30\n");
   mexPrintf("  in this case, Ts = %5.3e\n",Ts);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

slope = -10.0;
a = pow(10.0,-slope/20.0);
r = 10.0;

set(f_corner, a, r, Ts);
}

void OneOverfPlusWhiteNoise::set(double f_corner, double slope, double Ts)
{ 
double a,r;

if (f_corner < 0.0)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::set:  f_corner must be >= 0.0\n");
   mexPrintf("  in this case, f_corner = %5.3g\n",f_corner);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (Ts < 1e-30)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::set:  Ts must be >= 1e-30\n");
   mexPrintf("  in this case, Ts = %5.3g\n",Ts);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

if (slope > -5)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::set:  slope must be between -5 and -15\n");
   mexPrintf("  in this case, slope = %5.3g\n",slope);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }
if (slope < -15)
  {
   mexPrintf("Error in OneOverfPlusWhiteNoise::set:  slope must be between -5 and -15\n");
   mexPrintf("  in this case, slope = %5.3g\n",slope);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

a = pow(10.0,-slope/20.0);
r = 10.0;

set(f_corner, a, r, Ts);
}

OneOverfPlusWhiteNoise::~OneOverfPlusWhiteNoise()
{
}

double OneOverfPlusWhiteNoise::inp()
{
double node1;

node1 = rand1.inp();

if (ignore_one_over_f_flag == 0)
  {
   filt1.inp(rand1.inp());
   filt2.inp(rand1.inp());
   filt3.inp(rand1.inp());
   filt4.inp(rand1.inp());
   filt5.inp(rand1.inp());
   filt6.inp(rand1.inp());
   filt7.inp(rand1.inp());
   filt8.inp(rand1.inp());
   filt9.inp(rand1.inp());

   out = node1 + filt1.out + filt2.out + filt3.out + filt4.out + filt5.out + filt6.out + filt7.out + filt8.out + filt9.out;
  }
else
  {
   out = node1;
  }

return(out);
}


////// Neural Node Network

NeuralNode::NeuralNode()
  :  node_filt("1","1")
{
out = 0.0;
Pnode = NULL;
num_nodes = 0;
no_decay_flag = 0;
}

NeuralNode::~NeuralNode()
{
}

void NeuralNode::inp()
{
double sum;

if (no_decay_flag == 0)
   {
   if (Pnode == NULL)
      {
       mexPrintf("error in 'NeuralNode::inp':  Pnode is NULL!\n");
       mexErrMsgTxt("Exiting CppSim Mex object...");
      }

   input_list.reset();
   scale_factor_list.reset();
   sum = 0.0;
   if (input_list.length > 0)
     {
      while(input_list.notdone)
         sum += Pnode[(int) input_list.read()].out*scale_factor_list.read();
     }
   node_filt.inp(sum);
   out = node_filt.out;
   }
}


void NeuralNode::set_node_pointer(NeuralNode *Pnode_in, int num_nodes_in)
{
  if (num_nodes_in < 0)
    {
      mexPrintf("error in 'NeuralNode::set_node_pointer':\n");
      mexPrintf("  num_nodes = %d, but must be > 0\n",num_nodes_in);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
Pnode = Pnode_in;
num_nodes = num_nodes_in;
}


void NeuralNode::set_tau_and_gain(double tau, double gain, double Ts)
{
  no_decay_flag = 0;
  if (tau < 1e-12)
    {
      mexPrintf("error in 'NeuralNode::set_tau_and_gain':\n");
      mexPrintf("   tau must be > 1e-12\n");
      mexPrintf("   in this case, tau = %5.3e\n",tau);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
  node_filt.set("K","1 + tau*s","K,tau,Ts",gain,tau,Ts);
}

void NeuralNode::set_output(double value)
{
no_decay_flag = 1;
input_list.flush();
scale_factor_list.flush();
out = value;
}

void NeuralNode::reset_output(double value)
{
no_decay_flag = 0;
node_filt.reset(value);
out = value;
}

void NeuralNode::flush_inputs()
{
input_list.flush();
scale_factor_list.flush();
}

void NeuralNode::add_input(int node_index, double scale_factor)
{
  no_decay_flag = 0;
  if (node_index < 0 || node_index >= num_nodes)
    {
      mexPrintf("error in 'NeuralNode:add_input':  node_index out of range\n");
      mexPrintf("   in this case, node_index = %d\n", node_index);
      mexPrintf("   but must instead be >= 0 and < %d\n",num_nodes);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
  input_list.inp((double) node_index);
  scale_factor_list.inp(scale_factor);
}


////// Neuron Zone Element

Neuron_Zone_Element::Neuron_Zone_Element()
  :  zone_filt("1","1")
{
double saturation, fp, fz;

fp = 0.5;
fz = 5.0;
saturation = 1.0;

gain1 = 0.0;
gain2 = 0.0;
prev_phase1 = 0.0;
prev_phase2 = 0.0;
phase_count1 = 0.0;
phase_count2 = 0.0;
freq1 = 0.0;
freq2 = 0.0;
out = 0.0;
Ts = 1.0;
}

Neuron_Zone_Element::~Neuron_Zone_Element()
{
}

// Normal transfer function is (1 + s/(2*pi*fz))/(1 + s/(2*pi*fp))
//        with saturation limits of Min = 0.0, Max = saturation
//   if fz < 0.0, then transfer function becomes 1/(1 + s/(2*pi*fp))
void Neuron_Zone_Element::set_filter(double fp, double fz, double saturation, double Ts_in)
{
if (Ts_in < 1e-30)
   {
    mexPrintf("error in 'Neuron_Zone_Element::set_filter':\n");
    mexPrintf("  the value of 'Ts' should be >= 1e-30\n");
    mexPrintf("  -> in this case, Ts = %5.3e\n",Ts_in);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
Ts = Ts_in;

if (fp < 1e-12)
   {
    mexPrintf("error in 'Neuron_Zone_Element::set_filter':\n");
    mexPrintf("  the value of 'fp' should be >= 1e-12\n");
    mexPrintf("  -> in this case, fp = %5.3e\n",fp);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if (fz < 1e-12 && fz >= 0.0)
   {
    mexPrintf("error in 'Neuron_Zone_Element::set_filter':\n");
    mexPrintf("  the value of 'fz' should be >= 1e-12\n");
    mexPrintf("  or less than 0 (indicating zero should be removed)\n");
    mexPrintf("  -> in this case, fz = %5.3e\n",fz);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if (saturation < 0.0)
   {
    mexPrintf("error in 'Neuron_Zone_Element::set_filter':\n");
    mexPrintf("  the value of 'saturation' should be >= 0.0\n");
    mexPrintf("  -> in this case, saturation = %5.3e\n",saturation);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (fz < 0.0)
   zone_filt.set("1.0","1.0 + Tp*s","Max,Min,Tp,Ts",
            saturation,0.0,1.0/(2.0*pi*fp),Ts);
else
   zone_filt.set("1.0 + Tz*s","1.0 + Tp*s","Max,Min,Tz,Tp,Ts",
            saturation,0.0,1.0/(2.0*pi*fz),1.0/(2.0*pi*fp),Ts);

}

void Neuron_Zone_Element::set_gain(double gain1_in, double gain2_in)
{
if (gain1_in < 0.0)
    {
    mexPrintf("error in 'Neuron_Zone_Element::set_gain':\n");
    mexPrintf("  the value of 'gain1' should be >= 0.0\n");
    mexPrintf("  -> in this case, gain1 = %5.3e\n",gain1_in);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if (gain2_in < 0.0)
    {
    mexPrintf("error in 'Neuron_Zone_Element::set_gain':\n");
    mexPrintf("  the value of 'gain2' should be >= 0.0\n");
    mexPrintf("  -> in this case, gain2 = %5.3e\n",gain2_in);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
gain1 = gain1_in;
gain2 = gain2_in;
}

void Neuron_Zone_Element::set_gain(double gain1_in)
{
if (gain1_in < 0.0)
    {
    mexPrintf("error in 'Neuron_Zone_Element::set_gain':\n");
    mexPrintf("  the value of 'gain1' should be >= 0.0\n");
    mexPrintf("  -> in this case, gain1 = %5.3e\n",gain1_in);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
gain1 = gain1_in;
}

void Neuron_Zone_Element::set_auxiliary_gain(double gain2_in)
{
if (gain2_in < 0.0)
    {
    mexPrintf("error in 'Neuron_Zone_Element::set_auxiliary_gain':\n");
    mexPrintf("  the value of 'gain2' should be >= 0.0\n");
    mexPrintf("  -> in this case, gain2 = %5.3e\n",gain2_in);
    mexErrMsgTxt("Exiting CppSim Mex object...");
   }
gain2 = gain2_in;
}

double Neuron_Zone_Element::inp(double phase1)
{
double temp_calc, delta_phase;


if (phase1 < prev_phase1)
    {
      temp_calc = 1.0 + phase1 - prev_phase1;
      if (temp_calc < 1e-12)
	{
	  mexPrintf("error in 'Neuron_Zone_Element::inp':\n");
          mexPrintf("  -> phase1 input is corrupted!\n");
          mexPrintf("     difference between current and prev phase is\n");
          mexPrintf("     to small!\n");
          mexPrintf("     cur_phase1 = %5.3e, prev_phase1 = %5.3e\n",
		 phase1, prev_phase1);  
          mexErrMsgTxt("Exiting CppSim Mex object...");
	}
      delta_phase = (1.0 - prev_phase1)/temp_calc;
      phase_count1 += delta_phase;
      if (phase_count1 < 1.0)
	{
	  mexPrintf("error in 'Neuron_Zone_Element::inp':\n");
          mexPrintf("  -> phase1 input is corrupted!\n");
          mexPrintf("     phase wrap occured in less than one sample!\n");
          mexPrintf("     phase_count1 = %5.3e\n",phase_count1);
          mexErrMsgTxt("Exiting CppSim Mex object...");
	}
      freq1 = gain1/(Ts*phase_count1);
      phase_count1 = 1.0 - delta_phase;
    }
else if (phase1 == prev_phase1)
    {
      freq1 = 0.0;
    }
else
    {
      phase_count1 += 1.0;
    }

zone_filt.inp(freq1);
out = zone_filt.out;

prev_phase1 = phase1;
return(out);
}


double Neuron_Zone_Element::inp(double phase1, double phase2)
{
double temp_calc, delta_phase;

if (phase1 < prev_phase1)
    {
      temp_calc = 1.0 + phase1 - prev_phase1;
      if (temp_calc < 1e-12)
	{
	  mexPrintf("error in 'Neuron_Zone_Element::inp':\n");
          mexPrintf("  -> phase1 input is corrupted!\n");
          mexPrintf("     difference between current and prev phase is\n");
          mexPrintf("     to small!\n");
          mexPrintf("     cur_phase1 = %5.3e, prev_phase1 = %5.3e\n",
		 phase1, prev_phase1);  
          mexErrMsgTxt("Exiting CppSim Mex object...");
	}
      delta_phase = (1.0 - prev_phase1)/temp_calc;
      phase_count1 += delta_phase;
      if (phase_count1 < 1.0)
	{
	  mexPrintf("error in 'Neuron_Zone_Element::inp':\n");
          mexPrintf("  -> phase1 input is corrupted!\n");
          mexPrintf("     phase wrap occured in less than one sample!\n");
          mexPrintf("     phase_count1 = %5.3e\n",phase_count1);
          mexErrMsgTxt("Exiting CppSim Mex object...");
	}
      freq1 = gain1/(Ts*phase_count1);
      phase_count1 = 1.0 - delta_phase;
    }
else if (phase1 == prev_phase1)
    {
      freq1 = 0.0;
    }
else
    {
      phase_count1 += 1.0;
    }

if (phase2 < prev_phase2)
    {
      temp_calc = 1.0 + phase2 - prev_phase2;
      if (temp_calc < 1e-12)
	{
	  mexPrintf("error in 'Neuron_Zone_Element::inp':\n");
          mexPrintf("  -> phase2 input is corrupted!\n");
          mexPrintf("     difference between current and prev phase is\n");
          mexPrintf("     to small!\n");
          mexPrintf("     cur_phase2 = %5.3e, prev_phase2 = %5.3e\n",
		 phase2, prev_phase2);  
          mexErrMsgTxt("Exiting CppSim Mex object...");
	}
      delta_phase = (1.0 - prev_phase2)/temp_calc;
      phase_count2 += delta_phase;
      if (phase_count2 < 1.0)
	{
	  mexPrintf("error in 'Neuron_Zone_Element::inp':\n");
          mexPrintf("  -> phase2 input is corrupted!\n");
          mexPrintf("     phase wrap occured in less than one sample!\n");
          mexPrintf("     phase_count2 = %5.3e\n",phase_count2);
          mexErrMsgTxt("Exiting CppSim Mex object...");
	}
      freq2 = gain2/(Ts*phase_count2);
      phase_count2 = 1.0 - delta_phase;
    }
else if (phase2 == prev_phase2)
    {
      freq2 = 0.0;
    }
else
    {
      phase_count2 += 1.0;
    }
temp_calc = freq1*freq1 + freq2*freq2 
            + 2.0*freq1*freq2*cos(2*pi*(phase1-phase2));
if (temp_calc < 0.0)
   temp_calc = 0.0;

zone_filt.inp(sqrt(temp_calc));
out = zone_filt.out;

prev_phase1 = phase1;
prev_phase2 = phase2;

return(out);
}


////// Neuron Firing Element

Neuron_Firing_Element::Neuron_Firing_Element()
{
nom_freq = -1.0;
Kv = -1.0;
Ts = -1.0;
threshold = 0.0;
out = 0.0;
}

Neuron_Firing_Element::~Neuron_Firing_Element()
{
}

void Neuron_Firing_Element::set_nom_freq_and_kv(double nom_freq_in, 
                                                double Kv_in)
{
if (Ts < 1e-30)
   {
      mexPrintf("error in 'Neuron_Firing_Element::set_nom_freq_and_kv':\n");
      mexPrintf("  must first run 'Neuron_Firing_Element::set_time_step'\n");
      mexPrintf("  to set the value of Ts\n");
      mexErrMsgTxt("Exiting CppSim Mex object...");
   }
nom_freq = nom_freq_in;
if (nom_freq < 1e-12)
    {
      mexPrintf("error in 'Neuron_Firing_Element::set_nom_freq_and_kv':\n");
      mexPrintf("  nom_freq must be >= 1e-12\n");
      mexPrintf("  -> in this case, nom_freq = %5.3e\n",nom_freq);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
Kv = Kv_in;
if (Kv < 0.0)
    {
      mexPrintf("error in 'Neuron_Firing_Element::set_nom_freq_and_kv':\n");
      mexPrintf("  Kv must be >= 0.0\n");
      mexPrintf("  -> in this case, Kv = %5.3e\n",Kv);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
}

void Neuron_Firing_Element::set_phase(double phase)
{
if (Ts < 1e-30)
   {
      mexPrintf("error in 'Neuron_Firing_Element::set_nom_freq_and_kv':\n");
      mexPrintf("  must first run 'Neuron_Firing_Element::set_time_step'\n");
      mexPrintf("  to set the value of Ts\n");
      mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if (phase < 0.0 || phase >= 1.0)
    {
      mexPrintf("error in 'Neuron_Firing_Element::set_phase':\n");
      mexPrintf("  phase must be >= 0.0 and < 1.0\n");
      mexPrintf("  -> in this case, phase = %5.3e\n",phase);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
out = phase;
}

void Neuron_Firing_Element::set_time_step(double Ts_in)
{
Ts = Ts_in;

if (Ts < 1e-30)
    {
      mexPrintf("error in 'Neuron_Firing_Element::set_time_step':\n");
      mexPrintf("  Ts must be >= 1e-30\n");
      mexPrintf("  -> in this case, Ts = %5.3e\n",Ts);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
}

void Neuron_Firing_Element::set_input_threshold(double threshold_in)
{
if (Ts < 1e-30)
   {
      mexPrintf("error in 'Neuron_Firing_Element::set_nom_freq_and_kv':\n");
      mexPrintf("  must first run 'Neuron_Firing_Element::set_time_step'\n");
      mexPrintf("  to set the value of Ts\n");
      mexErrMsgTxt("Exiting CppSim Mex object...");
   }
threshold = threshold_in;
}


double Neuron_Firing_Element::inp(double in)
{
if (nom_freq < 0.0)
    {
      mexPrintf("error in 'Neuron_Firing_Element::inp':\n");
      mexPrintf("  must set 'nom_freq' and 'Kv' by running\n");
      mexPrintf("  method 'set_nom_freq_and_kv(nom_freq,Kv)'\n");
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
if (Ts < 1e-30)
   {
      mexPrintf("error in 'Neuron_Firing_Element::set_nom_freq_and_kv':\n");
      mexPrintf("  must first run 'Neuron_Firing_Element::set_time_step'\n");
      mexPrintf("  to set the value of Ts\n");
      mexErrMsgTxt("Exiting CppSim Mex object...");
   }

if (in >= threshold)
   out += Ts*(nom_freq + Kv*in);

if (out >= 1.0)
  {
   out -= 1.0;
  }
return(out);
}


////// Neuron Element (4 zones)

Neuron_Element4::Neuron_Element4()
  :  neuron_filt("1","1")
{
num_zones = 4;
num_neurons = -1;
num_inputs = -1;
thresh1 = 0.0;
thresh2 = 0.0;
PNeuron = NULL;
input_vec = NULL;
out = 0.0;
out_voltage = 0.0;
Ts = -1.0;

int i;
for (i = 0; i < num_zones; i++)
  {
   coupling_gain_index[i] = -1;
   node_to_node_index[i] = -1;
   input_to_node_index[i] = -1;
  }
}

Neuron_Element4::~Neuron_Element4()
{
}

void Neuron_Element4::store_PNeuron(int num_neurons_in,
                                    Neuron_Element4 *PNeuron_in)
{
num_neurons = num_neurons_in;
PNeuron = PNeuron_in;

if (num_neurons < 1)
    {
      mexPrintf("error in 'Neuron_Element4::store_PNeuron':\n");
      mexPrintf("   num_neurons must be > 0\n");
      mexPrintf("   -> in this case, num_neurons = %d\n",num_neurons);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
if (PNeuron == NULL)
   {
      mexPrintf("error in 'Neuron_Element4::store_PNeuron':\n");
      mexPrintf("   PNeuron is NULL!\n");
      mexErrMsgTxt("Exiting CppSim Mex object...");
   }
}

void Neuron_Element4::store_input_vec(int num_inputs_in,
                                      Vector *input_vec_in)
{
num_inputs = num_inputs_in;
input_vec = input_vec_in;

if (num_inputs < 1)
    {
      mexPrintf("error in 'Neuron_Element4::store_input_vec':\n");
      mexPrintf("   num_inputs must be > 0\n");
      mexPrintf("   -> in this case, num_inputs = %d\n",num_inputs);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
if (input_vec == NULL)
   {
      mexPrintf("error in 'Neuron_Element4::store_input_vec':\n");
      mexPrintf("   input_vec is NULL!\n");
      mexErrMsgTxt("Exiting CppSim Mex object...");
   }
if (num_inputs != input_vec->get_length())
   {
      mexPrintf("error in 'Neuron_Element4::store_input_vec':\n");
      mexPrintf("   num_inputs does not match input vector length!\n");
      mexPrintf("   -> num_inputs = %d, input_vec length = %d\n",
	     num_inputs, input_vec->get_length());
      mexErrMsgTxt("Exiting CppSim Mex object...");
   }
}

void Neuron_Element4::set_zone_gain(int zone_num, double gain_val)
{
if (zone_num >= num_zones || zone_num < 0)
    {
      mexPrintf("error in 'Neuron_Element4::set_zone_gain':\n");
      mexPrintf("   zone_num must be >= 0 and < num_zones\n");
      mexPrintf("   -> in this case: zone_num = %d, num_zones = %d\n",
	     zone_num, num_zones);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
zone[zone_num].set_gain(gain_val); 
}

void Neuron_Element4::set_zone_coupling_gain(int zone_num,
                              int src_zone_num, double gain_val)
{
if (zone_num >= num_zones || zone_num < 0)
    {
      mexPrintf("error in 'Neuron_Element4::set_zone_coupling_gain':\n");
      mexPrintf("   zone_num must be >= 0 and < num_zones\n");
      mexPrintf("   -> in this case: zone_num = %d, num_zones = %d\n",
	     zone_num, num_zones);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
if (src_zone_num >= num_zones)
    {
      mexPrintf("error in 'Neuron_Element4::set_zone_coupling_gain':\n");
      mexPrintf("   src_zone_num must be < num_zones\n");
      mexPrintf("   -> in this case: src_zone_num = %d, num_zones = %d\n",
	     src_zone_num, num_zones);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
if (src_zone_num < 0)
   {
      mexPrintf("error in 'Neuron_Element4::set_zone_coupling_gain':\n");
      mexPrintf("   src_zone_num must be >= 0\n");
      mexPrintf("   -> in this case: src_zone_num = %d\n", src_zone_num);
      mexErrMsgTxt("Exiting CppSim Mex object...");
   }
zone[zone_num].set_auxiliary_gain(gain_val);
coupling_gain_index[zone_num] = src_zone_num;
}


void Neuron_Element4::set_zone_filter(int zone_num, double fp, double fz, 
                       double saturation, double Ts_in)
{
Ts = Ts_in;
if (zone_num >= num_zones || zone_num < 0)
    {
      mexPrintf("error in 'Neuron_Element4::set_zone_filter':\n");
      mexPrintf("   zone_num must be >= 0 and < num_zones\n");
      mexPrintf("   -> in this case: zone_num = %d, num_zones = %d\n",
	     zone_num, num_zones);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
if (Ts < 1e-30)
   {
      mexPrintf("error in 'Neuron_Element4::set_zone_filter':\n");
      mexPrintf("   Ts must be >= 1e-30\n");
      mexPrintf("   -> in this case, Ts = %5.3e\n",Ts);
      mexErrMsgTxt("Exiting CppSim Mex object...");
   }
zone[zone_num].set_filter(fp,fz,saturation,Ts); 
firing_output.set_time_step(Ts);
}

void Neuron_Element4::set_thresh_and_bw(double thresh1_in, 
                      double thresh2_in, double fp)
{
if (fp < 1e-12)
    {
      mexPrintf("error in 'Neuron_Element4::set_thresh_and_bw':\n");
      mexPrintf("  fp must be >= 1e-12\n");
      mexPrintf("  -> in this case, fp = %5.3e\n",fp);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
if (Ts < 1e-30)
   {
      mexPrintf("error in 'Neuron_Element4::set_thresh_and_bw':\n");
      mexPrintf("  must first run 'Neuron_Element4::set_zone_filter'\n");
      mexPrintf("  to set the value of Ts\n");
      mexErrMsgTxt("Exiting CppSim Mex object...");
   }

thresh1 = thresh1_in;
thresh2 = thresh2_in;
neuron_filt.set("wo","wo + s","wo,Ts",2.0*pi*fp,Ts);
}

void Neuron_Element4::set_firing_parameters(double nom_freq,
					    double Kv, double threshold)
{
if (Ts < 1e-30)
   {
      mexPrintf("error in 'Neuron_Element4::set_firing_parameters':\n");
      mexPrintf("  must first run 'Neuron_Element4::set_zone_filter'\n");
      mexPrintf("  to set the value of Ts\n");
      mexErrMsgTxt("Exiting CppSim Mex object...");
   }

firing_output.set_time_step(Ts);
firing_output.set_nom_freq_and_kv(nom_freq,Kv);
firing_output.set_input_threshold(threshold);
} 

void Neuron_Element4::set_node_connection_to_pnode(int zone_num,
					           int neuron_index)
{
if (zone_num >= num_zones || zone_num < 0)
    {
      mexPrintf("error in 'Neuron_Element4::set_node_connection_to_pnode':\n");
      mexPrintf("   zone_num must be >= 0 and < num_zones\n");
      mexPrintf("   -> in this case: zone_num = %d, num_zones = %d\n",
	     zone_num, num_zones);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
if (neuron_index >= num_neurons || neuron_index < 0)
   {
      mexPrintf("error in 'Neuron_Element4::set_node_connection_to_pnode':\n");
      mexPrintf("   neuron_index must be >= 0 and < num_neurons\n");
      mexPrintf("   -> in this case: neuron_index = %d, num_neurons = %d\n",
	     neuron_index, num_neurons);
      mexErrMsgTxt("Exiting CppSim Mex object...");
   }
node_to_node_index[zone_num] = neuron_index;
}

void Neuron_Element4::set_node_connection_to_input(int zone_num,
					           int input_index)
{
if (zone_num >= num_zones || zone_num < 0)
    {
      mexPrintf("error in 'Neuron_Element4::set_node_connection_to_input':\n");
      mexPrintf("   zone_num must be >= 0 and < num_zones\n");
      mexPrintf("   -> in this case: zone_num = %d, num_zones = %d\n",
	     zone_num, num_zones);
      mexErrMsgTxt("Exiting CppSim Mex object...");
    }
if (input_index >= num_inputs || input_index < 0)
   {
      mexPrintf("error in 'Neuron_Element4::set_node_connection_to_input':\n");
      mexPrintf("   input_index must be >= 0 and < num_inputs\n");
      mexPrintf("   -> in this case: input_index = %d, num_inputs = %d\n",
	     input_index, num_inputs);
      mexErrMsgTxt("Exiting CppSim Mex object...");
   }
input_to_node_index[zone_num] = input_index;
}

double Neuron_Element4::inp()
{
int i;
double input_val[4];

if (num_zones > 4)
   {
     mexPrintf("error in 'Neuron_Element4':  only 4 zones assumed\n");
     mexPrintf("   here:   num_zones = %d\n",num_zones);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
for (i = 0; i < num_zones; i++)
   {
     if (input_to_node_index[i] >= 0)
	input_val[i] = input_vec->get_elem(input_to_node_index[i]);
     else if (node_to_node_index[i] >= 0)
	input_val[i] = PNeuron[node_to_node_index[i]].out;
     else
	input_val[i] = 0.0;
   }
for (i = 0; i < num_zones; i++)
   {
     if (coupling_gain_index[i] >= 0)
	 zone[i].inp(input_val[i],input_val[coupling_gain_index[i]]);
     else
	 zone[i].inp(input_val[i]);
   }

/// stuff below is specific for four zone implementation

double mult_inp1 = zone[0].out + zone[1].out - thresh1;
double mult_inp2 = zone[2].out - zone[3].out + thresh2;
neuron_filt.inp(mult_inp1*mult_inp2);
firing_output.inp(neuron_filt.out);
out = firing_output.out;
out_voltage = neuron_filt.out;
return(out);
}



/////////////////////// EvalExpression //////////////////////////

EvalExpression::EvalExpression()
{
first_func = NULL;
first_arg = NULL;
first_var = NULL;
create_func_list();
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


double EvalExpression::eval(const char *expr_in,const char *vars, double var0, double var1, double var2, double var3, double var4, double var5, double var6, double var7, double var8, double var9)
{
char vars2[MAX_CHAR_LENGTH];
char *p;
int i, count;
VarList *cur_arg,*cur_var;


free_arg_and_var();
//free_all();


for (i = 0; i < MAX_CHAR_LENGTH; i++)
   if (expr_in[i] == '\0')
       break;
if (expr_in[i] != '\0')
  {
   mexPrintf("error in EvalExpression.eval:  input expression is too long\n");
   mexPrintf("  string is limited to MAX_CHAR_LENGTH = %d\n",MAX_CHAR_LENGTH);
   mexPrintf("   must increase MAX_CHAR_LENGTH in source code\n");
   mexPrintf("   if you want to use the following input expression:\n");
   mexPrintf("   expr_in = %s\n",expr_in);
   mexErrMsgTxt("Exiting CppSim Mex object...");
  }

// create first_arg list
parse_into_args(expr_in);
//mexPrintf("args:\n");
//print_arg_list();

// create first_var list
determine_variables();
//mexPrintf("args:\n");
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
	mexPrintf("error in EvalExpression.eval:  number of parameters exceeded\n");
	mexPrintf("  (it's limited to 10 parameters");
	mexPrintf("  arg string: %s\n",vars);
	mexPrintf("  associated expression: %s\n",expr_in);
	mexErrMsgTxt("Exiting CppSim Mex object...");
      }
     if (i == 0)
       {
       if (var0 == NaN)
	 {
          mexPrintf("error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       set_var_value(p,var0);
       }
     else if (i == 1)
       {
       if (var1 == NaN)
	 {
          mexPrintf("error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       set_var_value(p,var1);
       }
     else if (i == 2)
       {
       if (var2 == NaN)
	 {
          mexPrintf("error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       set_var_value(p,var2);
       }
     else if (i == 3)
       {
       if (var3 == NaN)
	 {
          mexPrintf("error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       set_var_value(p,var3);
       }
     else if (i == 4)
       {
       if (var4 == NaN)
	 {
          mexPrintf("error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       set_var_value(p,var4);
       }
     else if (i == 5)
       {
       if (var5 == NaN)
	 {
          mexPrintf("error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       set_var_value(p,var5);
       }
     else if (i == 6)
       {
       if (var6 == NaN)
	 {
          mexPrintf("error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       set_var_value(p,var6);
       }
     else if (i == 7)
       {
       if (var7 == NaN)
	 {
          mexPrintf("error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       set_var_value(p,var7);
       }
     else if (i == 8)
       {
       if (var8 == NaN)
	 {
          mexPrintf("error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       set_var_value(p,var8);
       }
     else if (i == 9)
       {
       if (var9 == NaN)
	 {
          mexPrintf("error in EvalExpression.eval:  value of variable '%s' not set\n",
              p);
	  mexPrintf("   overall expression: %s\n",expr_in);
	  mexErrMsgTxt("Exiting CppSim Mex object...");
	 }
       set_var_value(p,var9);
       }
     i++;
     p = strtok(NULL," ,;");
   }

 cur_var = first_var;
 while(cur_var != NULL)
   {
     if (cur_var->is_valid_var_flag == 0)
       {
	 mexPrintf("error in 'EvalExpression.eval':  variable '%s' is undefined\n",cur_var->name);
         mexPrintf("   for expr: '%s'\n",expr_in);
	 mexErrMsgTxt("Exiting CppSim Mex object...");
       }
     cur_var = cur_var->next;
   }
/*
mexPrintf("variables:\n");
print_var_list();
mexPrintf("args:\n");
print_arg_list();
mexErrMsgTxt("Exiting CppSim Mex object...");

*/

set_arg_variable_vals();

/*
mexPrintf("args:\n");
print_arg_list();
mexPrintf("variables:\n");
print_var_list();
*/

for (i = 0; i < 50; i++)
   {
    eval_inner_paren_expr();
    count = count_num_of_args();
    if (count < 2)
      break;
    //    mexPrintf("****** new args:   count = %d ********\n",count);
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
     mexPrintf("error in 'EvalExpression.eval':  expression not evaluated properly!\n");
     mexPrintf("  -> expr = '%s'\n",expr_in);
     mexErrMsgTxt("Exiting CppSim Mex object...");
   }
out = first_arg->value;
return(out);
}


void EvalExpression::set_var_value(const char *name, double val)
{
VarList *cur_var;
int flag;

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
     mexPrintf("error in 'EvalExpression.set_var_value':\n");
     mexPrintf("  can't find variable '%s'\n",name);
     mexErrMsgTxt("Exiting CppSim Mex object...");   
   }
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
mexPrintf("error in 'EvalExpression.get_var_value':\n");
mexPrintf("  can't find variable '%s'\n",name);
mexErrMsgTxt("Exiting CppSim Mex object...");
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
	 mexPrintf("error:  mismatched parenthesis to ')'\n");
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
//	mexPrintf("error: - or + unary operator is misplaced\n");
//	return(NULL);
//       }
//    if (first_paren_arg->next->is_number_flag == 0)
//       {
//	mexPrintf("error: - or + unary operator is misplaced\n");
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
	    mexPrintf("error:  expression for '^' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (cur_arg->next == NULL)
	  {
	    mexPrintf("error:  expression for '^' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (prev_arg->is_number_flag == 0)
	  {
	    mexPrintf("error:  expression for '^' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
        if (cur_arg->next->is_number_flag == 0)
	  {
	    mexPrintf("error:  expression for '^' is invalid\n");
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
	       mexPrintf("error:  expression for unary '+' is invalid\n");
	       first_arg = NULL;
	       return;
	      }
            if (cur_arg->next->is_number_flag == 0)
	      {
	       mexPrintf("error:  expression for unary '+' is invalid\n");
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
	       mexPrintf("error:  expression for unary '-' is invalid\n");
	       first_arg = NULL;
	       return;
	      }
            if (cur_arg->next->is_number_flag == 0)
	      {
	       mexPrintf("error:  expression for unary '-' is invalid\n");
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
	    mexPrintf("error:  expression for '/' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (cur_arg->next == NULL)
	  {
	    mexPrintf("error:  expression for '/' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (prev_arg->is_number_flag == 0)
	  {
	    mexPrintf("error:  expression for '/' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
        if (cur_arg->next->is_number_flag == 0)
	  {
	    mexPrintf("error:  expression for '/' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (fabs(cur_arg->next->value) < 1e-300)
	  {
	    mexPrintf("error:  expression for '/' is invalid (divide by 0)\n");
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
	    mexPrintf("error:  expression for '*' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (cur_arg->next == NULL)
	  {
	    mexPrintf("error:  expression for '*' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (prev_arg->is_number_flag == 0)
	  {
	    mexPrintf("error:  expression for '*' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
        if (cur_arg->next->is_number_flag == 0)
	  {
	    mexPrintf("error:  expression for '*' is invalid\n");
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
	    mexPrintf("error:  expression for '+' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
        if (cur_arg->next->is_number_flag == 0)
	  {
	    mexPrintf("error:  expression for '+' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (prev_arg == NULL)
	  {
	    mexPrintf("error:  expression for '+' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	else if (prev_arg->is_number_flag == 0)
	  {
	    mexPrintf("error:  expression for '+' is invalid\n");
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
	    mexPrintf("error:  expression for '-' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
        if (cur_arg->next->is_number_flag == 0)
	  {
	    mexPrintf("error:  expression for '-' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	if (prev_arg == NULL)
	  {
	    mexPrintf("error:  expression for '-' is invalid\n");
	    first_arg = NULL;
	    return;
	  }
	else if (prev_arg->is_number_flag == 0)
	  {
	    mexPrintf("error:  expression for '-' is invalid\n");
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
     mexPrintf("error:  unknown function '%s'\n",name);
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
        mexPrintf("arg[%d] = '%s' --> val = %17.12e\n",i++,cur_arg->name,cur_arg->value);
     else if (cur_arg->is_function_flag == 1)
        mexPrintf("arg[%d] = '%s' --> ***function***\n",i++,cur_arg->name);
     else
        mexPrintf("arg[%d] = '%s'\n",i++,cur_arg->name);

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
        mexPrintf("arg[%d] = '%s' --> val = %17.12e\n",i++,cur_var->name,cur_var->value);
     else if (cur_var->is_function_flag == 1)
        mexPrintf("arg[%d] = '%s' --> ***function***\n",i++,cur_var->name);
     else
        mexPrintf("arg[%d] = '%s'\n",i++,cur_var->name);

     cur_var = cur_var->next;
   }
}

void EvalExpression::determine_variables()
{
VarList *cur_var, *cur_arg, *cur_func;
char delim[100];
char r[MAX_CHAR_LENGTH], t[MAX_CHAR_LENGTH];

sprintf(delim,"+-/*^()0123456789.");

if (first_var != NULL)
   {
     mexPrintf("error in 'EvalExpression.determine_variables':  first_var is != NULL!\n");
     mexErrMsgTxt("Exiting CppSim Mex object...");
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
}

void EvalExpression::parse_into_args(const char *expr)
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
     mexPrintf("error in 'EvalExpression.parse_into_args': first_arg != NULL!\n");
     mexErrMsgTxt("Exiting CppSim Mex object...");
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
  mexPrintf("error:  parenthesis mismatch\n");
  first_arg = NULL;
  return;
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


void EvalExpression::create_func_list()
{
VarList *cur_func;

if (first_func != NULL)
   {
     mexPrintf("error in 'EvalExpression.create_func_list':  first_func is != NULL!\n");
     mexErrMsgTxt("Exiting CppSim Mex object...");
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


} 



/////////////// CppDigTransitions ////////////////

cppDigTransitions::cppDigTransitions()
{
cur_node_value_list = new List;
prev_node_value_list = new List;
sig_transition = 0;
timeout = 0;
Ts_value = -1.0;
delta_samples = 0;
max_delta_samples = 0;
signal_count = 0;
}

cppDigTransitions::~cppDigTransitions()
{
delete cur_node_value_list;
delete prev_node_value_list;
}

void cppDigTransitions::incr_sample_step(double Ts_val, double max_step_time)
{
if (Ts_value < 0.0)
  {
    if (Ts_val < 1e-30)
      {
	mexPrintf("error in 'cppDigTransitions::incr_sample_step()':  Ts_val < 1e-30!\n");
	mexPrintf("  -> in this case, Ts_val = %5.3e\n",Ts_val);
	mexErrMsgTxt("Exiting CppSim Mex object...");
      }
    Ts_value = Ts_val;
    max_delta_samples = (int) (max_step_time/Ts_value);
    signal_count = 0;
    delta_samples = 0;
    sig_transition = 0;
    timeout = 0;
  }
if (delta_samples >= max_delta_samples)
  {
   delta_samples = 0;
   timeout = 1;
  }
else
  {
   delta_samples++;
   timeout = 0;
  }
sig_transition = 0;
signal_count = 0;
cur_node_value_list->reset();
prev_node_value_list->reset();
}


void cppDigTransitions::signal_value(double cur_value)
{
double prev_value, diff_value;

signal_count++;

if (signal_count > cur_node_value_list->length)
   {
    prev_node_value_list->inp(cur_value);
    cur_node_value_list->inp(cur_value);
    timeout = 1;
    delta_samples = 0;
   }
else
   {
    prev_value = cur_node_value_list->read_without_incrementing();
    cur_node_value_list->write(cur_value);
    prev_node_value_list->write(prev_value);

    diff_value = cur_value - prev_value;
    if (diff_value < -0.5 || diff_value > 0.5)
      {
       sig_transition = 1;
       delta_samples = 0;
      }
   }
}

void cppDigTransitions::signal_value(int cur_value_int)
{
double cur_value;

cur_value = (double) cur_value_int;
signal_value(cur_value);
}

void cppDigTransitions::signal_value_double_interp(double cur_value)
{
double prev_value, diff_value;

signal_count++;

if (signal_count > cur_node_value_list->length)
   {
    prev_node_value_list->inp(cur_value);
    cur_node_value_list->inp(cur_value);
    timeout = 1;
    delta_samples = 0;
   }
else
   {
    prev_value = cur_node_value_list->read_without_incrementing();
    cur_node_value_list->write(cur_value);
    prev_node_value_list->write(prev_value);

    diff_value = cur_value - prev_value;
    if ((cur_value == 1.0 && prev_value != 1.0)
        || (cur_value == -1.0 && prev_value != -1.0)
        || (cur_value != 1.0 && prev_value == 1.0)
        || (cur_value != -1.0 && prev_value == -1.0))
      {
       sig_transition = 1;
       delta_samples = 0;
      }
   }
}
