/* $Id$ */

/*********************************************************
      CppSim Classes for Behavioral Simulation of Systems
  written by Michael H. Perrott (http://www-mtl.mit.edu/~perrott)
            Copyright 2002 by Michael H. Perrott
                 All rights reserved
    This software comes with no warranty or support

NOTE:  PLEASE DO NOT MODIFY THIS CODE - DOING SO WILL
       PREVENT YOU FROM SHARING YOUR SIMULATION CODE
       WITH OTHERS, AND WILL MAKE IT MUCH HARDER FOR
       YOU TO BENEFIT FROM FUTURE UPDATES
    **    IF YOU WANT TO CHANGE A CLASS, THEN COPY THE    **
    **    CODE AND SAVE IT UNDER A DIFFERENT CLASS NAME   **
    **    IN     my_classes_and_functions.cpp             **
    **  SUGGESTION FOR NEW NAME:  CLASSNAME_YOURINITIALS  **
*********************************************************/

// #include "config.h"
#include "cppsim_classes.h"


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
   printf("%s[%d] = %5.3e\n",name,i++,read());
printf("\n");
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
   printf("\nerror in List.load: file %s can't be opened\n",filename);   
   exit(1);   
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
   printf("\nerror in List.save: file %s can't be opened\n",filename);   
   exit(1);   
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
   printf("error in List.inp:  cur_write_entry is NULL\n");
   printf(" odds are, you are trying to write to a passed list\n");
   printf(" which is not permitted\n");
   exit(1);
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
      printf("error in List.conv:  length of self list is zero\n");
   else
      printf("error in List.conv:  length of other list is zero\n");
   exit(1);
  }
lengthr = length+temp.length-1;

if ((array1 = (double *) calloc(length,sizeof(double))) == NULL)
   {
    printf("Error in List.conv: out of memory for coeff calloc\n");
    exit(1);
   }
if ((array2 = (double *) calloc(temp.length,sizeof(double))) == NULL)
   {
    printf("Error in List.conv: out of memory for coeff calloc\n");
    exit(1);
   }
if ((result = (double *) calloc(lengthr,sizeof(double))) == NULL)
   {
    printf("Error in List.conv: out of memory for coeff calloc\n");
    exit(1);
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
   printf("error in List.mean:  list has zero length\n");
   exit(1);
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
   printf("error in List.var:  list has zero length\n");
   exit(1);
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
   printf("error in List.add:  list sequences are of different length\n");
   printf("  in this case, self list has length %d\n",length);
   printf("                other list has length %d\n",temp.length);
   exit(1);
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
   printf("error in List.mul:  list sequences are of different length\n");
   printf("  in this case, self list has length %d\n",length);
   printf("                other list has length %d\n",temp.length);
   exit(1);
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
    printf("error in List.read:  cur_entry is NULL\n");
    exit(1);
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
    printf("error in List.read_without_incrementing:  cur_entry is NULL\n");
    exit(1);
  }
out = cur_entry->value;

return(out);
}

void List::write(double in)
{
if (cur_entry == NULL)
  {
    printf("error in List.write:  cur_entry is NULL\n");
    exit(1);
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
    printf("error in List.write_without_incrementing:  cur_entry is NULL\n");
    exit(1);
  }
cur_entry->value = in;

out = in;
}

void List::remove_first_entry()
{
DOUBLE_LIST *temp;

if (cur_write_entry == NULL)
  {
   printf("error in List.remove_first_entry:  cur_write_entry is NULL\n");
   printf(" odds are, you are trying to write to a passed list\n");
   printf(" which is not permitted\n");
   exit(1);
  }

if (length > 1)
  {
   temp = first_entry;
   first_entry = first_entry->next;
   if (first_entry == NULL)
     {
      printf("error in List.remove_first_entry:  first_entry->next is NULL\n");
      exit(1);
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
   printf("error in 'create_entry':  malloc call failed\n");
   printf("  out of memory!\n");
   exit(1);
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
   printf("error in Clist.inp:  length mismatch in input lists\n");
   printf("   list rin has length %d, while list iin has length %d\n",
          rin.length, iin.length);
   exit(1);
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
   printf("error in Clist.add:  length mismatch in Clists\n");
   printf("   self Clist has length %d, while other Clist has length %d\n",
          real.length, other.real.length);
   exit(1);
  }
else if (imag.length != other.imag.length)
  {
   printf("error in Clist.add:  length mismatch in Clists\n");
   printf("   self Clist has length %d, while other Clist has length %d\n",
          imag.length, other.imag.length);
   exit(1);
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
   printf("error in Clist.mul:  length mismatch in Clists\n");
   printf("   self Clist has real length %d, and imag length %d\n",
          real.length, imag.length);
   exit(1);
  }
else if (other.real.length != other.imag.length)
  {
   printf("error in Clist.mul:  length mismatch in Clists\n");
   printf("   other Clist has real length %d, and imag length %d\n",
          other.real.length, other.imag.length);
   exit(1);
  }
else if (real.length != other.real.length)
  {
   printf("error in Clist.mul:  length mismatch in Clists\n");
   printf("   self Clist has length %d, while other Clist has length %d\n",
          real.length, other.real.length);
   exit(1);
  }
else if (imag.length != other.imag.length)
  {
   printf("error in Clist.mul:  length mismatch in Clists\n");
   printf("   self Clist has length %d, while other Clist has length %d\n",
          imag.length, other.imag.length);
   exit(1);
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
   printf("error in Clist.mul:  length mismatch in Clist\n");
   printf("   self Clist has real length %d, and imag length %d\n",
          real.length, imag.length);
   exit(1);
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
   printf("error in Clist.conv:  length mismatch in Clists\n");
   printf("   self Clist has real length %d, and imag length %d\n",
          real.length, imag.length);
   exit(1);
  }
else if (other.real.length != other.imag.length)
  {
   printf("error in Clist.conv:  length mismatch in Clists\n");
   printf("   other Clist has real length %d, and imag length %d\n",
          other.real.length, other.imag.length);
   exit(1);
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
   printf("error in Clist.conv:  length mismatch in Clists\n");
   printf("   self Clist has real length %d, and imag length %d\n",
          real.length, imag.length);
   exit(1);
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
   printf("error in Clist.print:  real.length != imag.length\n");
   printf("   in this case, real.length = %d, imag.length = %d\n",
     real.length, imag.length);
   exit(1);
  }
for (i = 0; i < real.length; i++)
   printf("%s[%d]: real = %5.3e, imag = %5.3e\n",name,i+1,
              real.read(),imag.read());
printf("\n");
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

void Poly::set(const List &other)
{
int i;
List temp(other);


if (temp.length < 1)
  {
   printf("error in Poly.set (using List):  list length is zero\n");
   exit(1);
  }
if (num_coeff != 0)
   {
     free(coeff);
     free(exp);
   }

ivar = 'z';
num_coeff = temp.length;
if ((coeff = (double *) calloc(temp.length,sizeof(double))) == NULL)
   {
    printf("Error in Poly.set (using List): out of memory for coeff calloc\n");
    exit(1);
   }
if ((exp = (double *) calloc(temp.length,sizeof(double))) == NULL)
   {
    printf("Error in Poly.set (using List): out of memory for exponent calloc\n");
    exit(1);
   }
temp.reset();
i = 0;
while(temp.notdone)
   {
    coeff[i] = temp.read();
    exp[i] = -((double) i);
    i++;
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
   printf("error in Poly.set:  input expression is too long\n");
   printf("  string is limited to MAX_CHAR_LENGTH = %d\n",MAX_CHAR_LENGTH);
   printf("   must increase MAX_CHAR_LENGTH in source code\n");
   printf("   if you want to use the following input expression:\n");
   printf("   expr_in = %s\n",expr_in);
   exit(1);
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
	printf("error in Poly construct:  number of parameters exceeded\n");
	printf("  (it's limited to 10 parameters");
	printf("  arg string: %s\n",vars);
	printf("  associated expression: %s\n",expr_in);
	exit(1);
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
          printf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  printf("   overall expression: %s\n",expr_in);
	  exit(1);
	 }
       par_list->main_par_val[i] = var0;
       }
     else if (i == 1)
       {
       if (var1 == NaN)
	 {
          printf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  printf("   overall expression: %s\n",expr_in);
	  exit(1);
	 }
       par_list->main_par_val[i] = var1;
       }
     else if (i == 2)
       {
       if (var2 == NaN)
	 {
          printf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  printf("   overall expression: %s\n",expr_in);
	  exit(1);
	 }
       par_list->main_par_val[i] = var2;
       }
     else if (i == 3)
       {
       if (var3 == NaN)
	 {
          printf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  printf("   overall expression: %s\n",expr_in);
	  exit(1);
	 }
       par_list->main_par_val[i] = var3;
       }
     else if (i == 4)
       {
       if (var4 == NaN)
	 {
          printf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  printf("   overall expression: %s\n",expr_in);
	  exit(1);
	 }
       par_list->main_par_val[i] = var4;
       }
     else if (i == 5)
       {
       if (var5 == NaN)
	 {
          printf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  printf("   overall expression: %s\n",expr_in);
	  exit(1);
	 }
       par_list->main_par_val[i] = var5;
       }
     else if (i == 6)
       {
       if (var6 == NaN)
	 {
          printf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  printf("   overall expression: %s\n",expr_in);
	  exit(1);
	 }
       par_list->main_par_val[i] = var6;
       }
     else if (i == 7)
       {
       if (var7 == NaN)
	 {
          printf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  printf("   overall expression: %s\n",expr_in);
	  exit(1);
	 }
       par_list->main_par_val[i] = var7;
       }
     else if (i == 8)
       {
       if (var8 == NaN)
	 {
          printf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  printf("   overall expression: %s\n",expr_in);
	  exit(1);
	 }
       par_list->main_par_val[i] = var8;
       }
     else if (i == 9)
       {
       if (var9 == NaN)
	 {
          printf("error in Poly construct:  value of variable '%s' not set\n",
              par_list->main_par_list[i]);
	  printf("   overall expression: %s\n",expr_in);
	  exit(1);
	 }
       par_list->main_par_val[i] = var9;
       }
   }

if (count >= LENGTH_PAR_LIST)
    {
     printf("error in Poly construct:  number of parameters exceeded\n");
     exit(1);
    }
strncpy(par_list->main_par_list[count],"pi",LENGTH_PAR-1);
par_list->main_par_val[count++] = PI;
par_list->main_par_length = count;

/*
 for (i = 0; i < count; i++)
   printf("par = '%s', %5.3f\n",par_list->main_par_list[i],par_list->main_par_val[i]);
*/
// printf("expr_in = %s\n",expr_in);


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
//printf("ivar = %c\n",ivar);
/*
 if (ivar == '!')
   {
     printf("error in Poly construct:  couldn't determine the independent variable\n");
     exit(1);
   }
*/

count = 0;
for (i = 0,j = 0; expr_in[i] != '\0'; i++)
  {
   if (j >= (MAX_CHAR_LENGTH-1))
      {
	printf("error in Poly construct:  max char length exceeded\n");
        printf("  to fix, change MAX_CHAR_LENGTH in source code\n");
	exit(1);
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
//printf("expr2 = %s\n",expr2);

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
// printf("expr2 = %s\n",expr2);

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
    printf("Error in Poly construct: out of memory for coeff calloc\n");
    exit(1);
  }
if ((exp = (double *) calloc(count,sizeof(double))) == NULL)
  {
    printf("Error in Poly construct: out of memory for exponent calloc\n");
    exit(1);
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
	       printf("error in Poly construct:  bad polynomial expression\n");
	       printf("   expression must be of form a0*x + a1*x^2 ...\n");
	       printf("   NOT   .... x*a0 .... or .... x/a0 ....\n");
	       printf("    or   .... a0*(x) .... or .... (x/a1)^2 ....\n");
	       printf("   overall expression in question: %s\n",expr_in);
	       exit(1);
	     }
	   for (k = i+1; p[k] == ' '; k++);
	   if (p[k] == '/' || p[k] == '*' || p[k] == '(' || p[k] == ')')
	     {
	       printf("error in Poly construct:  bad polynomial expression\n");
	       printf("   expression must be of form a0*x + a1*x^2 ...\n");
	       printf("   NOT   .... x*a0 .... or .... x/a0 ....\n");
	       printf("    or   .... a0*(x) .... or .... (x/a1)^2 ....\n");
	       printf("   overall expression in question: %s\n",expr_in);
	       exit(1);
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
       printf("error in Poly construct:  cannot use '/' before independent\n");
       printf("  variable in polynomial expression\n");
       printf("     offending portion:  %ss^%s\n",p2,&p2[i+1]);
       printf("     overall expr: %s\n",expr_in);
       exit(1);
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
   printf("error in Poly constructor:  max is set <= min\n");
   printf("  in this case, max = %5.3e, min = %5.3e\n",max,min);
   exit(1);
  }
if (sample_period <= 0.0)
  {
   printf("error in Poly constructor:  sample_period is set <= 0.0\n");
   printf("  in this case, Ts = %5.3e\n",sample_period);
   exit(1);
  }

/*
for (i = 0; i < act_count; i++)
   printf("coeff = %5.3f, exponent = %5.3f\n",coeff[i],exp[i]);
*/
// printf("max = %5.3e, min = %5.3e\n",max,min);

num_coeff = act_count;
 free(par_list);
}

Poly::~Poly()
{
  free(coeff);
  free(exp);
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
     printf("warning in 'delay.inp':  nonvalid input!\n");
     printf("     in must -1.0, 1.0, or some value inbetween\n");
     printf("     in this case, in = %5.3e\n",in);
     printf("  **** in short, input is not conforming to the double_interp protocol ****\n");
    }

cur_delay_val = nom_delay + delta_delay;

if (cur_delay_val < 0.0)
   {
     printf("error in 'delay.inp':  delay value must be >= 0.0!\n");
     printf("  in this case, delay_val = %5.3e time samples\n",cur_delay_val);
     printf("  --> nom_delay = %5.3e time samples, delta_delay = %5.3e time samples\n",
            nom_delay,delta_delay);
     exit(1);
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
    printf("error in 'delay.inp':  limit of 200 rising/falling edges has been exceeded!\n");
           printf("     in edge buffer\n");
           printf("     -> this is due to having a delay that is too long\n");
           printf("        relative to the edge frequency of the signal coming in\n");
           printf("  **** you should reduce the delay time requested of this delay cell ****\n");
           printf("       nominal_delay = %5.3e time samples, delta_delay = %5.3e time samples\n",
		  nom_delay, delta_delay);
           printf("       overall delay = %5.3e time samples\n",cur_delay_val);
           exit(1);
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
   printf("error in SampleAndHold.inp:   clk is < -1.0 or > 1.0\n");
   printf("  in this case, clk = %5.3f\n",clk);
   exit(1);
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
   printf("Warning in EdgeDetect.inp:   in is < -1.0 or > 1.0\n");
   printf("  in this case, in = %5.3f\n",in);
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
   printf("Warning in EdgeDetect.inp:   in is < -1.0 or > 1.0\n");
   printf("  in this case, in = %5.3f\n",in);
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
}

EdgeInterpolate::~EdgeInterpolate()
{
}

double EdgeInterpolate::inp(double in)
{

if ((clk_in < -1.0 || clk_in > 1.0) && out_of_range_flag == 0)
  {
   out_of_range_flag = 1;
   printf("Warning in EdgeInterpolate.inp:   clk_in is < -1.0 or > 1.0\n");
   printf("  in this case, clk_in = %5.3f\n",in);
  }
out = (prev_in*(clk_in+1.0) + in*(-clk_in+1.0))/2.0;

return(out);
}

void EdgeInterpolate::update(double clk, double in)
{
prev_in = in;
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
   printf("Warning in EdgeMeasure.inp:   in is < -1.0 or > 1.0\n");
   printf("  in this case, in = %5.3f\n",in);
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
    printf("error in 'Quantizer': step_size is less than 1e-30\n");
    printf("   in this case, step_size = %5.3e\n",step_size);
    exit(1);
  }
istep_size = step_size;
if (levels < 2)
  {
    printf("error in 'Quantizer': levels must be >= 2\n");
    printf("  in this case, levels = %d\n",levels);
    exit(1);
  }
ilevels = levels;
if (levels % 2 == 1)
   odd_levels_flag = 1;
else
   odd_levels_flag = 0;

if (out_max <= out_min)
   {
     printf("error in 'Quantizer':  out_max must be > out_min\n");
     printf("  in this case, out_max = %5.3e, out_min = %5.3e\n",
            out_max, out_min);
     exit(1);
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
   printf("Warning in Quantizer.inp:   clk is < -1.0 or > 1.0\n");
   printf("  in this case, clk = %5.3f\n",clk);
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
    printf("Error in Probe Construct:  file '%s' can't be opened\n",filename);
    exit(1);
   }
if (subsample < 1)
   {
    printf("Error in Probe Construct:  subsample must be > 0\n");
    printf("  in this case, subsample = %d\n",subsample);
    exit(1);
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
	   printf("error in Probe.inp - filename has not been specified!\n");
	   exit(1);
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
       printf("error in Probe.inp - the signal probed does not match\n");
       printf("   with the signal that should be specified\n");
       printf("      the node that should be probed is: %s\n", cur_node->name);
       printf("      the node that is being probed is: %s\n", node_name);
       printf("   this error may be caused by using the same probe object\n");
       printf("   in two different loops\n");
       exit(1);
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
          printf("error:  this should never happen\n");
	  exit(1);
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
    printf("error in Divider.inp:  divide value is far from integer\n");
    printf(" in this case, divide value = %5.3f\n",divide_value);
    exit(1);
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
   printf("warning: in Divider.inp  divide value transitioned more than once\n");
   printf("  during one cycle of divider output\n");
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
             printf("Warning in Divider.inp:  divide_value must be > 1\n");
             printf("  in this case, divide_value = %d\n",divide_value);
	     printf("  setting divide value to 2 whenever it drops below 2\n");
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
divide_scale = 1.0;
sample_period = Kv_poly.poly_gain.sample_period;
prev_divide_val = 0;
divide_trans_count = -1;
trans_warning_flag = 0;
divide_val_warning_flag = 0;
out_of_range_flag = 0;

if (sample_period == NaN)
  {
   printf("Error in Vco Constructor:  sample_period has not been set\n");
   printf("   in variable list, you must specify 'Ts'\n");
   exit(1);
  }
else if (sample_period < 1e-30)
{
  printf("error in Vco Constructor:  sample_period can't be < 1e-30\n");
  printf("  in this case, sample_period = %5.3e\n",sample_period);
  exit(1);
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
   printf("Error in Vco Constructor:  sample_period has not been set\n");
   printf("   in variable list, you must specify 'Ts'\n");
   exit(1);
  }
else if (sample_period < 1e-30)
{
  printf("error in Vco Constructor:  sample_period can't be < 1e-30\n");
  printf("  in this case, sample_period = %5.3e\n",sample_period);
  exit(1);
}
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
   printf("Warning in Vco.inp:  interpolated output has value beyond -1 to 1 range\n");
   printf("  in this case, out = %5.3f\n",out);
   printf("  probable cause:  input inappropriate\n");
   printf("  in this case, in = %5.3f\n",in);
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
    printf("error in Vco.inp:  divide value is far from integer\n");
    printf(" in this case, divide value = %5.3f\n",divide_val);
    exit(1);
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
   printf("warning in Vco.inp:  divide value transitioned more than once\n");
   printf("  during one cycle of Vco output\n");
  }

if (sample_period*Kv_poly.inp(in) > divide_scale/2.0)
  {
   if (divide_val_warning_flag == 0)
     {
     divide_val_warning_flag = 1;
     printf("Warning in Vco.inp:  divide_val is too small for the given sample rate!\n");
     printf("  in this case, divide_val = '%d', and should be >= '%d'\n",
          divide_val,(int) (4*sample_period*Kv_poly.inp(in)));
     printf("  -> setting divide val to '%d' whenever it is too small\n",
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
   printf("Warning in Vco.inp:  interpolated output has value beyond -1 to 1 range\n");
   printf("  in this case, out = %5.3f\n",out);
   printf("  probable cause:  input or divide value inappropriate\n");
   printf("  in this case, in = %5.3f, divide value = %d\n",in,divide_val);
   printf("  also, make sure divide value is not changing more than once\n");
   printf("  per VCO cycle\n");
  }
return(out);
}


Vco::~Vco()
{
}


void Filter::reset(double value)
{
int i;

out = value;

// zero out sample memory
for (i = num_x_samples-1; i > 0; i--)
  x_samples[i] = value;
x_samples[0] = value;
for (i = num_y_samples-1; i > 0; i--)
  y_samples[i] = value;

y_samples[0] = out;
}

Filter::Filter()
{
num_a_coeff = 0;
num_b_coeff = 0;
num_x_samples = 0;
num_y_samples = 0;
}

Filter::Filter(const char *num, const char *den, const char *vars, double var0, double var1, 
         double var2, double var3, double var4, double var5, double var6, 
         double var7, double var8, double var9)
{
num_a_coeff = 0;
num_b_coeff = 0;
num_x_samples = 0;
num_y_samples = 0;

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

Poly num_poly(num_list);
Poly den_poly(den_list);

set_base(&num_poly,&den_poly,"set by List","set by List");
}

double Filter::inp(double in)
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

max_out = den_poly->max;
min_out = den_poly->min;

//printf("max_out = %5.3e, min_out = %5.3e\n",max_out,min_out);

if (num_poly->ivar == '!' && den_poly->ivar == '!')
  {
   num_poly->ivar = 'z';
   den_poly->ivar = 'z';
  }

if (num_poly->ivar != den_poly->ivar)
  {
   if (num_poly->ivar != '!' && den_poly->ivar != '!')
     {
      printf("error in Filter construct:  independent variables for\n");
      printf("numerator and denominator polynomials are different\n");
      printf("   in this case, ivar for num = %c, for den = %c\n",
	       num_poly->ivar, den_poly->ivar);
      printf("  for num: %s\n",num);
      printf("      den: %s\n",den);
      exit(1);
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
    printf("error:  filter must have 'z' or 's' for ivar\n");
    printf("  in this case, ivar = %c\n",num_poly->ivar);
    printf("  for num: %s\n",num);
    printf("      den: %s\n",den);
    exit(1);
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
   printf("num_exp = %5.3f, num_coeff = %5.3e\n",
           num_poly->exp[i],num_poly->coeff[i]);
printf("\n");
for (i = 0; i < den_poly->num_coeff; i++)
   printf("den_exp = %5.3f, den_coeff = %5.3e\n",
           den_poly->exp[i],den_poly->coeff[i]);
printf("-----------------------------------------\n");
*/

number_num_coeff = num_poly->num_coeff;
number_den_coeff = den_poly->num_coeff;

if ((num_coeff = (double *) calloc(number_num_coeff,sizeof(double))) == NULL)
  {
    printf("Error in 'set_ct_filt': out of memory for calloc\n");
    exit(1);
  }
if ((num_exp = (int *) calloc(number_num_coeff,sizeof(int))) == NULL)
  {
    printf("Error in 'set_ct_filt': out of memory for calloc\n");
    exit(1);
  }
if ((den_coeff = (double *) calloc(number_den_coeff,sizeof(double))) == NULL)
  {
    printf("Error in 'set_ct_filt': out of memory for calloc\n");
    exit(1);
  }
if ((den_exp = (int *) calloc(number_den_coeff,sizeof(int))) == NULL)
  {
    printf("Error in 'set_ct_filt': out of memory for calloc\n");
    exit(1);
  }

for (i = 0; i < number_num_coeff; i++)
   {
     num_coeff[i] = num_poly->coeff[i];
     if (fabs(floor(num_poly->exp[i])-num_poly->exp[i]) > .01)
       {
	 printf("error in 'set_ct_filt':  num_poly->exp has noninteger\n");
	 printf("values\n");
	 printf("   in this case, num_poly->exp[%d] = %5.3f\n",i,
		num_poly->exp[i]);
         printf("  for num: %s\n",num);
         printf("      den: %s\n",den);
	 exit(1);
       }
     num_exp[i] = (int) floor(num_poly->exp[i]);
   }
for (i = 0; i < number_den_coeff; i++)
   {
     den_coeff[i] = den_poly->coeff[i];
// printf("den_coeff[%d] = %5.3e\n",i,den_coeff[i]);
     if (fabs(floor(den_poly->exp[i])-den_poly->exp[i]) > .01)
       {
	 printf("error in 'set_ct_filt':  den_poly.exp has noninteger\n");
	 printf("values\n");
	 printf("   in this case, den_poly->exp[%d] = %5.3f\n",i,
		den_poly->exp[i]);
         printf("  for num: %s\n",num);
         printf("      den: %s\n",den);
	 exit(1);
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
    printf("Error in 'set_ct_filt': out of memory for calloc\n");
    exit(1);
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
    printf("Error in 'set_ct_filt': out of memory for calloc\n");
    exit(1);
    }
  if ((b_exp = (int *) calloc(num_b_coeff,sizeof(int))) == NULL)
    {
    printf("Error in 'set_ct_filt': out of memory for calloc\n");
    exit(1);
    }
  num_a_coeff = max+1;
  if ((a_coeff = (double *) calloc(num_a_coeff,sizeof(double))) == NULL)
    {
    printf("Error in 'set_ct_filt': out of memory for calloc\n");
    exit(1);
    }
  if ((a_exp = (int *) calloc(num_a_coeff,sizeof(int))) == NULL)
    {
    printf("Error in 'set_ct_filt': out of memory for calloc\n");
    exit(1);
    }
  if (num_y_samples != 0)
     free(y_samples);
  if (num_x_samples != 0)
     free(x_samples);
  num_y_samples = max;
  num_x_samples = max;
  if ((y_samples = (double *) calloc(num_y_samples,sizeof(double))) == NULL)
    {
    printf("Error in 'set_dt_filt': out of memory for calloc\n");
    exit(1);
    }
  for (i = 0; i < num_y_samples; i++)
    y_samples[i] = 0.0;
  if ((x_samples = (double *) calloc(num_x_samples,sizeof(double))) == NULL)
    {
    printf("Error in 'set_dt_filt': out of memory for calloc\n");
    exit(1);
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
  printf("error in 'set_ct_fil':  sample_period has not been set\n");
  printf("  in variable list, you must specify 'Ts'\n");
  printf("  for num: %s\n",num);
  printf("      den: %s\n",den);
  exit(1);
  }
else if (sample_period < 1e-30)
{
  printf("error in 'set_ct_fil':  sample_period can't be < 1e-30\n");
  printf("  in this case, sample_period = %5.3e\n",sample_period);
  printf("  for num: %s\n",num);
  printf("      den: %s\n",den);
  exit(1);
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
// printf("den_exp[%d] = %d, den_coeff[%d] = %5.3e\n",i,den_exp[i],i,den_coeff[i]);
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
   printf("orig num[%d] = %5.9e\n",i,b_coeff[i]);
 printf("\n");
 for (i = 0; i < max+1; i++)
   printf("orig den[%d] = %5.9e\n",i,a_coeff[i]);
 printf("-----------------------------------\n");
*/

if (fabs(a_coeff[0]) < 1e-30)
{
  printf("error in 'set_ct_fil':  a0 is close to zero\n");
  printf("  in this case, a0 = %5.3e\n",a_coeff[0]);
  printf("  must have an ill-posed CT filter for DT converstion\n");
  printf("  for num: %s\n",num);
  printf("      den: %s\n",den);
  exit(1);
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
   printf("num[%d] = %5.3f\n",b_exp[i],b_coeff[i]);
 printf("\n");
 for (i = 0; i < num_a_coeff; i++)
   printf("den[%d] = %5.3f\n",a_exp[i],a_coeff[i]);
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
       printf("Error in 'set_dt_filt': out of memory for calloc\n");
       exit(1);
      }
if ((b_exp = (int *) calloc(num_b_coeff,sizeof(int))) == NULL)
      {
       printf("Error in 'set_dt_filt': out of memory for calloc\n");
       exit(1);
      }
if ((a_coeff = (double *) calloc(num_a_coeff,sizeof(double))) == NULL)
      {
       printf("Error in 'set_dt_filt': out of memory for calloc\n");
       exit(1);
      }
if ((a_exp = (int *) calloc(num_a_coeff,sizeof(int))) == NULL)
      {
       printf("Error in 'set_dt_filt': out of memory for calloc\n");
       exit(1);
      }

for (i = 0; i < num_b_coeff; i++)
   {
     b_coeff[i] = num_poly->coeff[i];
     if (fabs(floor(num_poly->exp[i])-num_poly->exp[i]) > .01)
       {
	 printf("error in 'set_dt_filt':  num_poly->exp has noninteger\n");
	 printf("values\n");
	 printf("   in this case, num_poly->exp[%d] = %5.3f\n",i,
		num_poly->exp[i]);
         printf("  for num: %s\n",num);
         printf("      den: %s\n",den);
	 exit(1);
       }
     b_exp[i] = (int) -floor(num_poly->exp[i]);
   }
for (i = 0; i < num_a_coeff; i++)
   {
     a_coeff[i] = den_poly->coeff[i];
     if (fabs(floor(den_poly->exp[i])-den_poly->exp[i]) > .01)
       {
	 printf("error in 'set_dt_filt':  den_poly.exp has noninteger\n");
	 printf("values\n");
	 printf("   in this case, den_poly->exp[%d] = %5.3f\n",i,
		den_poly->exp[i]);
         printf("  for num: %s\n",num);
         printf("      den: %s\n",den);
	 exit(1);
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
     printf("-- b_exp = %d ,b_coeff = %5.3f\n",b_exp[i],b_coeff[i]);
 for (i = 0; i < num_a_coeff; i++)
     printf("-- a_exp = %d ,a_coeff = %5.3f\n",a_exp[i],a_coeff[i]);
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
  printf("error in 'set_dt_fil':  a0_val is close to zero\n");
  printf("  in this case, a0_val = %5.3e\n",a0_val);
  printf("  for num: %s\n",num);
  printf("      den: %s\n",den);
  exit(1);
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
    printf("Error in 'set_dt_filt': out of memory for calloc\n");
    exit(1);
    }
  for (i = 0; i < num_y_samples; i++)
    y_samples[i] = 0.0;
  if ((x_samples = (double *) calloc(num_x_samples,sizeof(double))) == NULL)
    {
    printf("Error in 'set_dt_filt': out of memory for calloc\n");
    exit(1);
    }
  for (i = 0; i < num_x_samples; i++)
     x_samples[i] = 0.0;
  }

/*
 printf("num_a_coeff = %d, num_b_coeff = %d, num_x_samples = %d, num_y_samples = %d\n",num_a_coeff,num_b_coeff,num_x_samples,num_y_samples);
for (i = 0; i < num_b_coeff; i++)
     printf("b_exp = %d ,b_coeff = %5.3f\n",b_exp[i],b_coeff[i]);
 printf("\n");
for (i = 0; i < num_a_coeff; i++)
     printf("a_exp = %d ,a_coeff = %5.3f\n",a_exp[i],a_coeff[i]);
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
   printf("Warning in Latch.init:  input value is not 1.0 or -1.0\n");
   printf("  in this case, input value = %5.3f\n",in);
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
    printf("Warning in 'Latch.inp_reset': reset transitions occurred in two consecutive samples\n");
    printf("  or, reset is outp of its range:  -1.0 <= reset <= 1.0\n");
    printf("  reset = %5.3f, prev_reset = %5.3f\n",reset,prev_reset);
    printf("  probable issue:  need to have more samples per clock period\n");
  }

prev_reset = reset;
}

void Latch::inp_base(double in, double clk)
{
if ((in > 1.0 || in < -1.0) && out_of_range_flag == 0)
  {
    out_of_range_flag = 1;
    printf("Warning in Latch.inp:  in is constrained to be -1.0 <= in <= 1.0\n");
    printf("  in this case, in = %5.3f\n",in);
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
    printf("Warning:  In 'Latch.inp': clk transitions occurred in two consecutive samples\n");
    printf("  or, clk is out of its range:  -1.0 <= clk <= 1.0\n");
    printf("  clk = %5.3f, prev_clk = %5.3f\n",clk,prev_clk);
    printf("  probable issue:  need to have more samples per clock period\n");
    clk_warning_flag = 1;
  }
prev_clk = clk;
}



void Reg::init(double in)
{
if ((in != 1.0 && in != -1.0) && out_of_range_flag == 0)
  {
   out_of_range_flag = 1;
   printf("Warning in Reg.init:  input value is not 1.0 or -1.0\n");
   printf("  in this case, input value = %5.3f\n",in);
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
    printf("error in Rand out function:  must have 0 <= type_flag <= 2\n");
    printf("   in this case, type_flag = %d\n",type_flag);
    exit(1);
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
   printf("error in Rand constructor:  type must be 'gauss',\n");
   printf("               'uniform', or 'bernoulli'\n");
   printf("  in this case, type = '%s'\n",type);
   exit(1);
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
   printf("error in SdMbitMod constructor: ivar for H(z) is not 'z'\n");
   printf("  in this case, ivar is '%c'\n",H.ivar);
   printf("  for H(z): %s\n",num);
   exit(1);
  }
flag = 0;
for (i = 0; i < H.num_b_coeff; i++)
   if (H.b_exp[i] == 0)
       if (H.b_coeff[i] == 1.0)
           flag = 1;

if (flag == 0)
  {
   printf("error in SdMbitMod constructor: must have H(z)\n");
   printf("  whose z^0 coefficient is exactly 1.0\n");
   printf("  in this case, H(z) is:  %s\n",num);
   exit(1);
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
   printf("error in SdMbitMod constructor: ivar for H(z) is not 'z'\n");
   printf("  in this case, ivar is '%c'\n",H.ivar);
   printf("  for H(z): %s\n",num);
   exit(1);
  }
flag = 0;
for (i = 0; i < H.num_b_coeff; i++)
   if (H.b_exp[i] == 0)
       if (H.b_coeff[i] == 1.0)
           flag = 1;

if (flag == 0)
  {
   printf("error in SdMbitMod constructor: must have H(z)\n");
   printf("  whose z^0 coefficient is exactly 1.0\n");
   printf("  in this case, H(z) is:  %s\n",num);
   exit(1);
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
    printf("error in SigGen.set:  Ts < 1e-30\n");
    printf("   in this case, Ts = %5.3e\n",Ts);
    exit(1);
  }
if (freq <= 0.0)
  {
    printf("error in SigGen.set:  freq <= 0.0\n");
    printf("   in this case, freq = %5.3e\n",freq);
    exit(1);
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
   printf("error in SigGen.set:  type must be either\n");
   printf("  'square', 'sine', 'prbs', or 'impulse'\n");
   printf("  in this case, type = '%s'\n",type);
   exit(1);
  }
}

void SigGen::set(const char *type, double freq, double Ts, const List &other)
{
sample_period = Ts;
frequency = freq;
phase_filt.set("a","1 - (1-a)*z^-1","a",0.5*freq*Ts);

if (sample_period < 1e-30)
  {
    printf("error in SigGen.set:  Ts < 1e-30\n");
    printf("   in this case, Ts = %5.3e\n",Ts);
    exit(1);
  }
if (freq <= 0.0)
  {
    printf("error in SigGen.set:  freq <= 0.0\n");
    printf("   in this case, freq = %5.3e\n",freq);
    exit(1);
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
   printf("error in SigGen.set:  type must be either\n");
   printf("  'square', 'sine', 'prbs', or 'impulse'\n");
   printf("  in this case, type = '%s'\n",type);
   exit(1);
  }

end_data_flag = 0;
prev_data = 1.0;
data_seq.copy(other);
data = data_seq.read() > 0.0 ? 1.0 : -1.0;
if (data != -1.0 && data != 1.0)
  {
   printf("error in SigGen.set:  data list has values that are not 1.0 or -1.0\n");
   printf("  in this case, the data value is %5.3f\n",data);
  }
if (type_flag == 2)
  {
   reg1.init(data);
   data = data_seq.read() > 0.0 ? 1.0 : -1.0;
   if (data != -1.0 && data != 1.0)
      {
      printf("error in SigGen.set:  data list has values that are not 1.0 or -1.0\n");
      printf("  in this case, the data value is %5.3f\n",data);
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
     printf("error in SigGen.set:  starting entry for List input must be >= 1\n");
     printf("   (note that a value of 1 corresponds to the first entry)\n");
     printf("   in this case, start_entry = %d\n",start_entry);
     exit(1);
   }
if (sample_period < 1e-30)
  {
    printf("error in SigGen.set:  Ts < 1e-30\n");
    printf("   in this case, Ts = %5.3e\n",Ts);
    exit(1);
  }
if (freq <= 0.0)
  {
    printf("error in SigGen.set:  freq <= 0.0\n");
    printf("   in this case, freq = %5.3e\n",freq);
    exit(1);
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
   printf("error in SigGen.set:  type must be either\n");
   printf("  'square', 'sine', 'prbs', or 'impulse'\n");
   printf("  in this case, type = '%s'\n",type);
   exit(1);
  }

end_data_flag = 0;
prev_data = 1.0;
data_seq.copy(other);
for (i = 0; i < start_entry; i++)
   data = data_seq.read() > 0.0 ? 1.0 : -1.0;
if (data != -1.0 && data != 1.0)
  {
   printf("error in SigGen.set:  data list has values that are not 1.0 or -1.0\n");
   printf("  in this case, the data value is %5.3f\n",data);
  }
if (type_flag == 2)
  {
   reg1.init(data);
   data = data_seq.read() > 0.0 ? 1.0 : -1.0;
   if (data != -1.0 && data != 1.0)
      {
      printf("error in SigGen.set:  data list has values that are not 1.0 or -1.0\n");
      printf("  in this case, the data value is %5.3f\n",data);
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
    printf("error in SigGen.inp: input phase cannot change\n");
    printf("     instantaneously by more than a cycle\n");
    printf("  i.e.,  -1.0 <= delta in <= 1.0\n");
    printf("  in this case, in = %5.4f, prev_in = %5.4f, delta in = %5.4f\n",
              in,prev_in,in-prev_in);
    exit(1);
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
   printf("error in SigGen.inp:  interpolated output beyond -1 to 1 range\n");
   printf("  in this case, square out = %5.3f\n",square);
   printf("  probable cause:  input inappropriate\n");
   printf("  in this case, in = %5.3f\n",in);
   exit(1);
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
   printf("error in SigGen.inp:  type_flag not recognized\n");
   printf("  in this case, type_flag = %d\n",type_flag);
   exit(1);
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
       printf("error in SigGen.set:  data list has values that are not 1.0 or -1.0\n");
       printf("  in this case, the data value is %5.3f\n",data);
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
       printf("error using 'create_data': out of disk space\n");
       exit(1);
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
         printf("error using 'create_data': out of disk space\n");
         exit(1);
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
    printf("error in 'create_header':  first_node is NULL\n");
    exit(1);
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
          printf("error in 'fwrite_int_values':  mxCalloc call failed - \n");   
          printf("  not enough memory!!\n");   
          exit(1);   
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
       printf("error using 'fwrite_int_values': out of disk space\n");
       exit(1);
      }
   free(charbuf);
  }


void  fwrite_char_values(const char *cbuf, size_t size_of_block, FILE *fp)
  {
    if (fwrite(cbuf,sizeof(char), size_of_block, fp) != size_of_block)
      {
       printf("error using 'fwrite_char_values': out of disk space\n");
       exit(1);
      }
  }



NODE *init_node(const char *name, double value)
{   
NODE *A;   
   
if ((A = (NODE *) malloc(sizeof(NODE))) == NULL)   
   {   
   printf("error in 'init_node':  malloc call failed\n");   
   printf("out of memory!\n");   
   exit(1);   
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
   printf("error in 'init_param':  malloc call failed\n");   
   printf("out of memory!\n");   
   exit(1);   
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
          printf("error in 'eval_paren_expr':  parenthesis nested too deep\n");
          printf("   expression:  %s\n",input);
	  printf("   overall expression: %s\n",par_list->overall_expr);
          exit(1);
	}
       left_paren[left_paren_count] = i;
       break;
     case ')':
       right_paren_count++;
     }
 }
if (right_paren_count != left_paren_count)
  {
    printf("error in 'eval_paren_expr':  parenthesis mismatch\n");
    printf("   expression:  %s\n",input);
    printf("   overall expression: %s\n",par_list->overall_expr);
    exit(1);
  }
par_list->nest_length = 0;

for (nest_count = 0; left_paren_count >= 0; left_paren_count--)
  {
    if (nest_count > 99)
      {
	printf("error in 'eval_paren_expr':  parenthesis nested too deep\n");
	printf("   expression:  %s\n",input);
        printf("   overall expression: %s\n",par_list->overall_expr);
	printf("  note:  LENGTH_PAR_LIST should not be set above 100!\n");
	exit(1);
      }
    sprintf(nest_val_string,"%d",nest_count);
    curr_left_paren = left_paren[left_paren_count];
    for (i = curr_left_paren; input[i] != ')' && input[i] != '\0'; i++);
    if (input[i] == '\0')
      {
	printf("error in 'eval_paren_expr':  right and left parenthesis mismatch\n");
	printf("   expression:  %s\n",input);
        printf("   overall expression: %s\n",par_list->overall_expr);
	exit(1);
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
	printf("error in 'eval_no_paren_expr':  too many strings \n");
        printf("   expression = %s\n",input);
        printf("   overall expression: %s\n",par_list->overall_expr);
	exit(1);
      }
    arg_flag = 0;
    for (j = 0; input[i] != ' ' && input[i] != '\0' && input[i] != '*' &&
	 input[i] != '+' && input[i] != '\n' && input[i] != '/' &&
         input[i] != '^' && input[i] != '-'; j++,i++)
      {
        arg_flag = 1;
	if (j >= LENGTH_PAR-1)
	  {
	    printf("error in 'eval_no_paren_expr':  string too long\n");
	    printf("   expression = %s\n",input);
  	    printf("   overall expression: %s\n",par_list->overall_expr);
	    exit(1);
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
	   printf("error in 'eval_no_paren_expr':  too many operators \n");
	   printf("   expression = %s\n",input);
 	   printf("   overall expression: %s\n",par_list->overall_expr);
	   exit(1);
	 }
       oper_list[oper_num] = input[i];
       oper_num++;
       for (i++; input[i] == ' '; i++);
      }
    else
      {
	printf("error in 'eval_no_paren_expr':  two arguments in a row\n");
	printf("   expression = %s\n",input);
        printf("   overall expression: %s\n",par_list->overall_expr);
	exit(1);
      }
  }
if (arg_num != oper_num + 1)
  {
    printf("error in 'eval_no_paren_expr':  operator and argument mismatch \n");
    printf("   expression = %s\n",input);
    printf("   note:  if expression is blank, you probably have empty\n");
    printf("          set of parenthesis somewhere\n");
    printf("   overall expression: %s\n",par_list->overall_expr);
    exit(1);
  }


/*  evaluate expression */


/*  first, convert all arguments to numbers */

/* exp_value = convert_string("4582.0",&error_flag);
 printf("exp_value = %12.8e, error flag = %d \n",exp_value,error_flag); 
  printf("decimal = %d\n",atoi("9"));
  exit(1);
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
           printf("error in 'eval_no_paren_expr':  nested_arg_list exceeded\n");
           printf("   (this error shouldn't happen:  something wrong\n");
           printf("   with the adi_to_hspice source code\n");
	   printf("   expression = %s\n",input);
 	   printf("   overall expression: %s\n",par_list->overall_expr);
           exit(1);
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
             printf("error in 'eval_no_paren_expr':  argument not defined\n");
             printf("    arg = `%s'\n",arg_list[i]);
	     printf("    expression = %s\n",input);
	     printf("   overall expression: %s\n",par_list->overall_expr);
             exit(1);
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
		       printf("error in 'convert_string':  units value '%s'\n",
                             &input[i]);
		       printf("   not recognized\n");
		       exit(1);
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
	       printf("error in 'convert_string':  units value '%s'\n",&input[i]);
	       printf("   not recognized\n");
	       exit(1);
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
	 printf("error in 'convert_exponent':  exponent term '%s'\n",input);
         printf("  within the string '%s'\n",full_string);
	 printf("   has unallowed characters\n");
	 exit(1);
	 }
     }
  }
if (i == 0)
  {
    printf("error in 'convert_exponent':  exponent term\n");
    printf("  within the string '%s'\n",full_string);
    printf("   has no value\n");
    exit(1);
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
    printf("error in 'Vector' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    exit(1);   
   }
sprintf(module_name,"unknown");

if ((name = (char *) calloc(10,sizeof(char))) == NULL)   
   {   
    printf("error in 'Vector' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    exit(1);   
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
    printf("error in 'Vector' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  module_name = '%s'\n",module_name_in);
    printf("  desired module_name length = %d\n",i+1);   
    exit(1);   
   }
strcpy(module_name,module_name_in);

for (i = 0; name_in[i] != '\0'; i++);
if ((name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'Vector' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  name = '%s'\n",name_in);
    printf("  desired name length = %d\n",i+1);   
    exit(1);   
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
     printf("Error:  you are using the wrong constructor!!!\n");
     printf("   ---> use either 'Vector vec1;' or 'Vector vec1(\"mod_name\",\"name\");'\n");
     printf("   in this case, you used 'Vector %s(\"%s\",\"%s\",\"%s\");'\n",
	    name_in, module_name_in,name_in,null_string);
     exit(1);
   }

for (i = 0; module_name_in[i] != '\0'; i++);
if ((module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'Vector' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  module_name = '%s'\n",module_name_in);
    printf("  desired module_name length = %d\n",i+1);   
    exit(1);   
   }
strcpy(module_name,module_name_in);

for (i = 0; name_in[i] != '\0'; i++);
if ((name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'Vector' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  name = '%s'\n",name_in);
    printf("  desired name length = %d\n",i+1);   
    exit(1);   
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
    printf("error in 'Vector' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  name = '%s'\n",other.name);
    printf("  desired name length = %d\n",j+45);   
    exit(1);   
   }
sprintf(name,"derived from: %s",other.name);

for (i = 0; other.module_name[i] != '\0'; i++);
if ((module_name = (char *) calloc(i+j+30,sizeof(char))) == NULL)   
   {   
    printf("error in 'Vector' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  module_name = '%s'\n",other.module_name);
    printf("  desired module_name length = %d\n",i+j+30);   
    exit(1);   
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


Vector& Vector::operator = (const Vector &other) const
{
if (vec != other.vec)
  {
    copy(other);
  }
}

Vector& Vector::operator = (Vector &other) const
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
    printf("error in 'Vector::set_length':  length must be >= 0\n");
    printf("  in this case, length = %d\n",length);
    printf("  and you ran set_length() on vector '%s'\n",name);
    printf("     (originating module of vector = '%s')\n",module_name);
    exit(1);
   }   

if (vec->length == length)
   return;
else if (vec->freeze_length_flag == 1)
   {
    printf("error in 'Vector::set_length':  cannot change the length of a\n");
    printf("   vector declared in the 'outputs:' section of a module in the modules.par file\n");
    printf("   EXCEPT in the 'init_output_vectors:' section of the modules.par file\n");
    printf("   in this case, you tried changing the length of  vector '%s'\n",name);
    printf("     (originating module of vector = '%s')\n",module_name);
    printf("-> cur vector length = %d, desired length = %d\n",vec->length,
            length);
    exit(1);
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
      printf("error in 'Vector::set_length':  calloc call failed - \n");   
      printf("  not enough memory!!\n");
      printf("  vector name = '%s', desired length = %d\n",name,length);   
      printf("     (originating module name of vector = '%s')\n",module_name);
      exit(1);   
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
    printf("error in 'Vector::init()': you cannot initialize a vector\n");
    printf("  that has already been initialized or assigned from another vector\n");
    printf("  in this case, you ran init() on vector '%s'\n",name);
    printf("     (originating in module '%s')\n",module_name);
    exit(1);
  }
   
if ((vec = (vector_struct *) malloc(sizeof(vector_struct))) == NULL)   
   {   
   printf("error in 'Vector::init()':  malloc call failed\n");   
   printf("out of memory!\n");
   printf("  vector name = '%s'\n",name);   
   printf("     (originating module name of vector = '%s')\n",module_name);
   exit(1);   
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
    printf("error in 'Vector::set_elem':  index must be >= 0\n");
    printf("  in this case, index = %d\n",index);
    printf("  and you ran set_elem() on vector '%s'\n",name);
    printf("     (originating module of vector = '%s')\n",module_name);
    exit(1);
   }

if (index >= vec->length)
    {
    printf("error in 'Vector::set_elem':  index must be < length of vector\n");
    printf("  in this case, index = %d, vector length = %d\n",index,vec->length);
    printf("  and you ran set_elem() on vector '%s'\n",name);
    printf("     (originating module of vector = '%s')\n",module_name);
    exit(1);
    }

vec->elem[index] = val;
}   

double Vector::get_elem(int index) const
{
if (index < 0)
   {
    printf("error in 'Vector::get_elem':  index must be >= 0\n");
    printf("  in this case, index = %d\n",index);
    printf("  and you ran get_elem() on vector '%s'\n",name);
    printf("     (originating module of vector = '%s')\n",module_name);
    exit(1);
   }

if (index >= vec->length)
    {
    printf("error in 'Vector::get_elem':  index must be < length of vector\n");
    printf("  in this case, index = %d, vector length = %d\n",index,vec->length);
    printf("  and you ran get_elem() on vector '%s'\n",name);
    printf("     (originating module of vector = '%s')\n",module_name);
    exit(1);
    }

return(vec->elem[index]);
}   


//// Integer-valued vectors

IntVector::IntVector()
{
if ((module_name = (char *) calloc(10,sizeof(char))) == NULL)   
   {   
    printf("error in 'IntVector' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    exit(1);   
   }
sprintf(module_name,"unknown");

if ((name = (char *) calloc(10,sizeof(char))) == NULL)   
   {   
    printf("error in 'IntVector' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    exit(1);   
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
    printf("error in 'IntVector' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  module_name = '%s'\n",module_name_in);
    printf("  desired module_name length = %d\n",i+1);   
    exit(1);   
   }
strcpy(module_name,module_name_in);

for (i = 0; name_in[i] != '\0'; i++);
if ((name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'IntVector' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  name = '%s'\n",name_in);
    printf("  desired name length = %d\n",i+1);   
    exit(1);   
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
     printf("Error:  you are using the wrong constructor!!!\n");
     printf("   ---> use either 'IntVector vec1;' or 'IntVector vec1(\"mod_name\",\"name\");'\n");
     printf("   in this case, you used 'IntVector %s(\"%s\",\"%s\",\"%s\");'\n",
	    name_in, module_name_in,name_in,null_string);
     exit(1);
   }

for (i = 0; module_name_in[i] != '\0'; i++);
if ((module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'IntVector' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  module_name = '%s'\n",module_name_in);
    printf("  desired module_name length = %d\n",i+1);   
    exit(1);   
   }
strcpy(module_name,module_name_in);

for (i = 0; name_in[i] != '\0'; i++);
if ((name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'IntVector' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  name = '%s'\n",name_in);
    printf("  desired name length = %d\n",i+1);   
    exit(1);   
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
    printf("error in 'IntVector' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  name = '%s'\n",other.name);
    printf("  desired name length = %d\n",j+45);   
    exit(1);   
   }
sprintf(name,"derived from: %s",other.name);

for (i = 0; other.module_name[i] != '\0'; i++);
if ((module_name = (char *) calloc(i+j+30,sizeof(char))) == NULL)   
   {   
    printf("error in 'IntVector' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  module_name = '%s'\n",other.module_name);
    printf("  desired module_name length = %d\n",i+j+30);   
    exit(1);   
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


IntVector& IntVector::operator = (const IntVector &other) const
{
if (vec != other.vec)
  {
    copy(other);
  }
}

IntVector& IntVector::operator = (IntVector &other) const
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
    printf("error in 'IntVector::set_length':  length must be >= 0\n");
    printf("  in this case, length = %d\n",length);
    printf("  and you ran set_length() on vector '%s'\n",name);
    printf("     (originating module of vector = '%s')\n",module_name);
    exit(1);
   }   

if (vec->length == length)
   return;
else if (vec->freeze_length_flag == 1)
   {
    printf("error in 'IntVector::set_length':  cannot change the length of a\n");
    printf("   vector declared in the 'outputs:' section of a module in the modules.par file\n");
    printf("   EXCEPT in the 'init_output_vectors:' section of the modules.par file\n");
    printf("   in this case, you tried changing the length of  vector '%s'\n",name);
    printf("     (originating module of vector = '%s')\n",module_name);
    printf("-> cur vector length = %d, desired length = %d\n",vec->length,
            length);
    exit(1);
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
      printf("error in 'IntVector::set_length':  calloc call failed - \n");   
      printf("  not enough memory!!\n");
      printf("  vector name = '%s', desired length = %d\n",name,length);   
      printf("     (originating module name of vector = '%s')\n",module_name);
      exit(1);   
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
    printf("error in 'IntVector::init()': you cannot initialize a vector\n");
    printf("  that has already been initialized or assigned from another vector\n");
    printf("  in this case, you ran init() on vector '%s'\n",name);
    printf("     (originating in module '%s')\n",module_name);
    exit(1);
  }
   
if ((vec = (int_vector_struct *) malloc(sizeof(int_vector_struct))) == NULL)   
   {   
   printf("error in 'IntVector::init()':  malloc call failed\n");   
   printf("out of memory!\n");
   printf("  vector name = '%s'\n",name);   
   printf("     (originating module name of vector = '%s')\n",module_name);
   exit(1);   
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
    printf("error in 'IntVector::set_elem':  index must be >= 0\n");
    printf("  in this case, index = %d\n",index);
    printf("  and you ran set_elem() on vector '%s'\n",name);
    printf("     (originating module of vector = '%s')\n",module_name);
    exit(1);
   }

if (index >= vec->length)
    {
    printf("error in 'IntVector::set_elem':  index must be < length of vector\n");
    printf("  in this case, index = %d, vector length = %d\n",index,vec->length);
    printf("  and you ran set_elem() on vector '%s'\n",name);
    printf("     (originating module of vector = '%s')\n",module_name);
    exit(1);
    }

vec->elem[index] = val;
}   

int IntVector::get_elem(int index) const
{
if (index < 0)
   {
    printf("error in 'IntVector::get_elem':  index must be >= 0\n");
    printf("  in this case, index = %d\n",index);
    printf("  and you ran get_elem() on vector '%s'\n",name);
    printf("     (originating module of vector = '%s')\n",module_name);
    exit(1);
   }

if (index >= vec->length)
    {
    printf("error in 'IntVector::get_elem':  index must be < length of vector\n");
    printf("  in this case, index = %d, vector length = %d\n",index,vec->length);
    printf("  and you ran get_elem() on vector '%s'\n",name);
    printf("     (originating module of vector = '%s')\n",module_name);
    exit(1);
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
  printf("error using 'copy':  you cannot use this function to copy\n");
  printf("   from a vector to a list\n");
  printf("   -->  use 'list1.copy(vector1);' to accomplish this\n");
  printf("   this error occurred while trying to copy vector '%s' to a list\n",
	 from.name);
  printf("       (vector '%s' was created in module '%s')\n",from.name,
	 from.module_name);
  exit(1);
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
  printf("error using 'copy':  you cannot use this function to copy\n");
  printf("   from a vector to a list\n");
  printf("   -->  use 'list1.copy(vector1);' to accomplish this\n");
  printf("   this error occurred while trying to copy vector '%s' to a list\n",
	 from.name);
  printf("       (vector '%s' was created in module '%s')\n",from.name,
	 from.module_name);
  exit(1);
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
     printf("error in 'inner_product(A,B)':  A->length must equal B->length\n");
     printf("   in this case, A->length = %d, B->length = %d\n",
            A->length,B->length);
     printf("   vector A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   vector B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     exit(1);
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
     printf("error in 'inner_product(A,B)':  A->length must equal B->length\n");
     printf("   in this case, A->length = %d, B->length = %d\n",
            A->length,B->length);
     printf("   vector A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   vector B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     exit(1);
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
      printf("error in 'extract_vector_struct': vec is NULL!\n");
      printf("  this shouldn't happen in normal use of vectors\n");
      printf("  name of vector = '%s'\n",in.name);
      printf("      (originating module of vector = '%s')\n",in.module_name);
      exit(1);
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
      printf("error in 'extract_vector_struct': vec is NULL!\n");
      printf("  this shouldn't happen in normal use of vectors\n");
      printf("  name of vector = '%s'\n",in.name);
      printf("      (originating module of vector = '%s')\n",in.module_name);
      exit(1);
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
   printf("error in 'init_vector_struct':  malloc call failed\n");   
   printf("out of memory!\n");
   exit(1);   
  }   

int i;
for (i = 0; module_name[i] != '\0'; i++);
   
if ((A->module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'init_vector_struct':  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  module_name = '%s'\n",module_name);
    printf("  desired module_name length = %d\n",i+1);   
    exit(1);   
   }
strcpy(A->module_name,module_name);

for (i = 0; name[i] != '\0'; i++);
if ((A->name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'init_vector_struct':  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  name = '%s'\n",name);
    printf("  desired name length = %d\n",i+1);   
    exit(1);   
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
   printf("error in 'init_int_vector_struct':  malloc call failed\n");   
   printf("out of memory!\n");
   exit(1);   
  }

int i;
for (i = 0; module_name[i] != '\0'; i++);
   
if ((A->module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'init_int_vector_struct':  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  module_name = '%s'\n",module_name);
    printf("  desired module_name length = %d\n",i+1);   
    exit(1);   
   }
strcpy(A->module_name,module_name);

for (i = 0; name[i] != '\0'; i++);
if ((A->name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'init_int_vector_struct':  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  name = '%s'\n",name);
    printf("  desired name length = %d\n",i+1);   
    exit(1);   
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

printf("vector name = '%s' (originated in module '%s')\n",
	x->name, x->module_name);

for (i = 0; i < x->length; i++)
   printf("  %s[%d] = %5.3e\n",x->name,i,x->elem[i]);
}

void print(int_vector_struct *x)
{
int i;

printf("int_vector name = '%s' (originated in module '%s')\n",
	x->name, x->module_name);

for (i = 0; i < x->length; i++)
   printf("  %s[%d] = %d\n",x->name,i,x->elem[i]);
}


void add(vector_struct *a, vector_struct *b, vector_struct *y)
{
int i;

if (a->length != b->length)
   {
     printf("error in 'add':  input vectors have unequal length!\n");
     printf("  input vector '%s' has length %d (originating module is '%s')\n",a->name,a->length,a->module_name);
     printf("  input vector '%s' has length %d (originating module is '%s')\n",b->name,b->length,b->module_name);
     exit(1);
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
     printf("error in 'add':  input vectors have unequal length!\n");
     printf("  input vector '%s' has length %d (originating module is '%s')\n",a->name,a->length,a->module_name);
     printf("  input vector '%s' has length %d (originating module is '%s')\n",b->name,b->length,b->module_name);
     exit(1);
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
     printf("error in 'sub':  input vectors have unequal length!\n");
     printf("  input vector '%s' has length %d (originating module is '%s')\n",a->name,a->length,a->module_name);
     printf("  input vector '%s' has length %d (originating module is '%s')\n",b->name,b->length,b->module_name);
     exit(1);
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
     printf("error in 'sub':  input vectors have unequal length!\n");
     printf("  input vector '%s' has length %d (originating module is '%s')\n",a->name,a->length,a->module_name);
     printf("  input vector '%s' has length %d (originating module is '%s')\n",b->name,b->length,b->module_name);
     exit(1);
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
     printf("error in 'mul_elem':  input vectors have unequal length!\n");
     printf("  input vector '%s' has length %d (originating module is '%s')\n",a->name,a->length,a->module_name);
     printf("  input vector '%s' has length %d (originating module is '%s')\n",b->name,b->length,b->module_name);
     exit(1);
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
     printf("error in 'mul_elem':  input vectors have unequal length!\n");
     printf("  input vector '%s' has length %d (originating module is '%s')\n",a->name,a->length,a->module_name);
     printf("  input vector '%s' has length %d (originating module is '%s')\n",b->name,b->length,b->module_name);
     exit(1);
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
     printf("error in 'div_elem':  input vectors have unequal length!\n");
     printf("  input vector '%s' has length %d (originating module is '%s')\n",a->name,a->length,a->module_name);
     printf("  input vector '%s' has length %d (originating module is '%s')\n",b->name,b->length,b->module_name);
     exit(1);
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
     printf("error in 'div_elem':  input vectors have unequal length!\n");
     printf("  input vector '%s' has length %d (originating module is '%s')\n",a->name,a->length,a->module_name);
     printf("  input vector '%s' has length %d (originating module is '%s')\n",b->name,b->length,b->module_name);
     exit(1);
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
    printf("error in 'sinc_vector':  length must be > 0\n");
    printf("  in this case, length = %d\n",length);
    printf("  for vector '%s' (originating module for vector = '%s')\n",
	   A->name,A->module_name);
    exit(1);
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
    printf("error in 'gauss_ran_vector':  length must be > 0\n");
    printf("  in this case, length = %d\n",length);
    printf("  for vector '%s' (originating module for vector = '%s')\n",
	   A->name,A->module_name);
    exit(1);
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
   printf("error in get_mean:  vector has zero length\n");
    printf("  input vector = '%s' (originating module for vector = '%s'\n",
	   A->name,A->module_name);
   exit(1);
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
   printf("error in get_var:  vector has zero length\n");
    printf("  input vector = '%s' (originating module for vector = '%s'\n",
	   A->name,A->module_name);
   exit(1);
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
    printf("error in 'set_length':  length must be > 0\n");
    printf("  in this case, length = %d\n",length);
    printf("  for vector '%s'\n",A->name);
    printf("  originating module name of vector = '%s'\n",A->module_name);   
    exit(1);
   }   

if (A->length == length)
   return;
else if (A->freeze_length_flag == 1)
   {
    printf("error in 'set_length()':  cannot change the length of a\n");
    printf("   vector declared in the 'outputs:' section of a module in the modules.par file\n");
    printf("   EXCEPT in the 'init_output_vectors:' section of the modules.par file\n");
    printf("   in this case, you tried changing the length of  vector '%s'\n",A->name);
    printf("     (originating module of vector = '%s')\n",A->module_name);
    printf("-> cur vector length = %d, desired length = %d\n",A->length,
            length);
    exit(1);
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
      printf("error in 'set_length':  calloc call failed - \n");   
      printf("  not enough memory!!\n");
      printf("  vector name = '%s', desired length = %d\n",A->name,length);   
      printf("  originating module name of vector = '%s'\n",A->module_name);   
      exit(1);   
      }
   }
A->length = length;
}

void set_length(int length, int_vector_struct *A)   
{

if (length < 0)
   {
    printf("error in 'set_length':  length must be > 0\n");
    printf("  in this case, length = %d\n",length);
    printf("  for vector '%s'\n",A->name);
    printf("  originating module name of vector = '%s'\n",A->module_name);   
    exit(1);
   }   

if (A->length == length)
   return;
else if (A->freeze_length_flag == 1)
   {
    printf("error in 'set_length()':  cannot change the length of a\n");
    printf("   vector declared in the 'outputs:' section of a module in the modules.par file\n");
    printf("   EXCEPT in the 'init_output_vectors:' section of the modules.par file\n");
    printf("   in this case, you tried changing the length of  vector '%s'\n",A->name);
    printf("     (originating module of vector = '%s')\n",A->module_name);
    printf("-> cur vector length = %d, desired length = %d\n",A->length,
            length);
    exit(1);
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
      printf("error in 'set_length':  calloc call failed - \n");   
      printf("  not enough memory!!\n");
      printf("  vector name = '%s', desired length = %d\n",A->name,length);   
      printf("  originating module name of vector = '%s'\n",A->module_name);   
      exit(1);   
      }
   }
A->length = length;
}





double read_elem(int index, vector_struct *A)
{
if (index < 0)
    {
      printf("error in 'read_elem':  index must be >= 0\n");
      printf("  in this case, for vector '%s':\n",A->name);
      printf("  -->  length = %d, index = %d\n",A->length,index);
      printf("  originating module name of vector = '%s'\n",A->module_name);   
      exit(1);
    }
if (index >= A->length)
    {
      printf("error in 'read_elem':  index must be < length of vector_struct\n");
      printf("  in this case, for vector '%s':\n",A->name);
      printf("  -->  length = %d, index = %d\n",A->length,index);
      printf("  originating module name of vector = '%s'\n",A->module_name);   
      exit(1);
    }

return(A->elem[index]);
}   


int read_elem(int index, int_vector_struct *A)
{
if (index < 0)
    {
      printf("error in 'read_elem':  index must be >= 0\n");
      printf("  in this case, for vector '%s':\n",A->name);
      printf("  -->  length = %d, index = %d\n",A->length,index);
      printf("  originating module name of vector = '%s'\n",A->module_name);   
      exit(1);
    }
if (index >= A->length)
    {
      printf("error in 'read_elem':  index must be < length of vector_struct\n");
      printf("  in this case, for vector '%s':\n",A->name);
      printf("  -->  length = %d, index = %d\n",A->length,index);
      printf("  originating module name of vector = '%s'\n",A->module_name);   
      exit(1);
    }

return(A->elem[index]);
}   



void write_elem(double val, int index, vector_struct *A)
{
if (index < 0)
    {
      printf("error in 'write_elem':  index must be >= 0\n");
      printf("  in this case, for vector '%s':\n",A->name);
      printf("  -->  length = %d, index = %d\n",A->length,index);
      printf("  originating module name of vector = '%s'\n",A->module_name);   
      exit(1);
    }
if (index >= A->length)
    {
      printf("error in 'write_elem':  index must be < length of vector_struct\n");
      printf("  in this case, for vector '%s':\n",A->name);
      printf("  -->  length = %d, index = %d\n",A->length,index);
      printf("  originating module name of vector = '%s'\n",A->module_name);   
      exit(1);
    }

A->elem[index] = val;
}   


void write_elem(int val, int index, int_vector_struct *A)
{
if (index < 0)
    {
      printf("error in 'write_elem':  index must be >= 0\n");
      printf("  in this case, for vector '%s':\n",A->name);
      printf("  -->  length = %d, index = %d\n",A->length,index);
      printf("  originating module name of vector = '%s'\n",A->module_name);   
      exit(1);
    }
if (index >= A->length)
    {
      printf("error in 'write_elem':  index must be < length of vector_struct\n");
      printf("  in this case, for vector '%s':\n",A->name);
      printf("  -->  length = %d, index = %d\n",A->length,index);
      printf("  originating module name of vector = '%s'\n",A->module_name);   
      exit(1);
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
        register int i,j;   
	double h1r,h1i,h2r,h2i;   
	double wr,wi,wpr,wpi,wtemp,wtemp2,theta,test_log2,store_log2;   
        double temp_store;   
        vector_struct *temp;
   
        if (REAL == IMAG)   
	  {   
           printf("error in 'real_fft':  REAL and IMAG vectors must be distinct\n   in this case, vector '%s' is specified for both vectors REAL and IMAG\n",REAL->name);   
           exit(1);   
	 }   
        if (DATA->length == 0)  
	  {  
           printf("error in 'real_fft':  input vector '%s' has zero length!\n",DATA->name);  
           exit(1);  
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
        register int i,j;   
	double wtemp,wr,wpr,wpi,wi,theta,test_log2;   
	double tempr,tempi;   
   
        nn = DATA->length/2;   
        if (DATA->length == 0)  
	  {  
           printf("error in 'four1':  input vector '%s' has zero length!\n",DATA->name);  
           exit(1);  
	 }  
        test_log2 = log((double) nn)/log(2.0) + 0.0000001;   
        test_log2 -= floor(test_log2);   
   
        if (test_log2 > 0.0000002)   
	  {   
           printf("error in 'four1':  length of input vector must be\n");   
           printf("an integer power of 2\n  in this case, vector '%s' is length %d (times 2)\n",DATA->name,DATA->length/2);   
           exit(1);   
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
        register int i,j;   
	double wtemp,wr,wpr,wpi,wi,theta;   
        vector_struct *temp;   
	double tempr,tempi;   
   
        if (DATA_REAL == DATA_IMAG)   
	  {   
           printf("error in 'fft':  DATA_REAL and DATA_IMAG vectors must be specified\ndifferently - in this case, both are specified as vector '%s'\n",DATA_REAL->name);   
           exit(1);   
	 }   
        if (FFT_REAL == FFT_IMAG)   
	  {   
           printf("error in 'fft':  FFT_REAL and FFT_IMAG vectors must be specified\ndifferently - in this case, both are specified as vector '%s'\n",FFT_REAL->name);   
           exit(1);   
	 }   
        if (DATA_REAL->length != DATA_IMAG->length)   
	  {   
           printf("error in 'fft':  DATA_REAL and DATA_IMAG vectors must have\nthe same length\n   in this case, DATA_REAL vector '%s' has length %d\n              DATA_IMAG vector '%s' has length %d\n",DATA_REAL->name,DATA_REAL->length,DATA_IMAG->name,DATA_IMAG->length);   
           exit(1);   
	 }   
        if (DATA_REAL->length == 0)  
	  {  
           printf("error in 'fft':  input vectors '%s' and '%s' have zero length!\n",DATA_REAL->name,DATA_IMAG->name);  
           exit(1);  
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
        register int i,j;   
	double wtemp,wr,wpr,wpi,wi,theta;
        vector_struct *temp;
	double tempr,tempi;   
   
/* note:  this program's labels are misleading - output is the real and imag */  
/* components of the inverse fft of the input data */  
  
        if (DATA_REAL == DATA_IMAG)   
	  {   
           printf("error in 'ifft':  DATA_REAL and DATA_IMAG vectors must be specified\ndifferently - in this case, both are specified as vector '%s'\n",DATA_REAL->name);   
           exit(1);   
	 }   
        if (FFT_REAL == FFT_IMAG)   
	  {   
           printf("error in 'ifft':  FFT_REAL and FFT_IMAG vectors must be specified\ndifferently - in this case, both are specified as vector '%s'\n",FFT_REAL->name);   
           exit(1);   
	 }   
        if (DATA_REAL->length != DATA_IMAG->length)   
	  {   
           printf("error in 'ifft':  DATA_REAL and DATA_IMAG vectors must have\nthe same length\n   in this case, DATA_REAL vector '%s' has length %d\n              DATA_IMAG vector '%s' has length %d\n",DATA_REAL->name,DATA_REAL->length,DATA_IMAG->name,DATA_IMAG->length);   
           exit(1);   
	 }   
         if (DATA_REAL->length == 0)  
	  {  
           printf("error in 'ifft':  input vectors '%s' and '%s' have zero length!\n",DATA_REAL->name,DATA_IMAG->name);  
           exit(1);  
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
    printf("error in 'Matrix' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    exit(1);   
   }
sprintf(module_name,"unknown");

if ((name = (char *) calloc(10,sizeof(char))) == NULL)   
   {   
    printf("error in 'Matrix' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    exit(1);   
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
    printf("error in 'Matrix' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  module_name = '%s'\n",module_name_in);
    printf("  desired module_name length = %d\n",i+1);   
    exit(1);   
   }
strcpy(module_name,module_name_in);

for (i = 0; name_in[i] != '\0'; i++);
if ((name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'Matrix' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  name = '%s'\n",name_in);
    printf("  desired name length = %d\n",i+1);   
    exit(1);   
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
     printf("Error:  you are using the wrong constructor!!!\n");
     printf("   ---> use either 'Matrix mat1;' or 'Matrix mat1(\"mod_name\",\"name\");'\n");
     printf("   in this case, you used 'Matrix %s(\"%s\",\"%s\",\"%s\");'\n",
	    name_in, module_name_in,name_in,null_string);
     exit(1);
   }

for (i = 0; module_name_in[i] != '\0'; i++);
if ((module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'Matrix' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  module_name = '%s'\n",module_name_in);
    printf("  desired module_name length = %d\n",i+1);   
    exit(1);   
   }
strcpy(module_name,module_name_in);

for (i = 0; name_in[i] != '\0'; i++);
if ((name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'Matrix' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  name = '%s'\n",name_in);
    printf("  desired name length = %d\n",i+1);   
    exit(1);   
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
    printf("error in 'Matrix' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  name = '%s'\n",other.name);
    printf("  desired name length = %d\n",j+45);   
    exit(1);   
   }
sprintf(name,"derived from: %s",other.name);

for (i = 0; other.module_name[i] != '\0'; i++);
if ((module_name = (char *) calloc(i+j+30,sizeof(char))) == NULL)   
   {   
    printf("error in 'Matrix' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  module_name = '%s'\n",other.module_name);
    printf("  desired module_name length = %d\n",i+j+30);   
    exit(1);   
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


Matrix& Matrix::operator = (const Matrix &other) const
{
if (mat != other.mat)
  {
    copy(other);
  }
}

Matrix& Matrix::operator = (Matrix &other) const
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
    printf("error in 'Matrix::set_size':  cannot change the size of a\n");
    printf("   matrix declared in the 'outputs:' section of a module in the modules.par file\n");
    printf("   EXCEPT in the 'init_output_vectors:' section of the modules.par file\n");
    printf("   in this case, you tried changing the size of matrix '%s'\n",name);
    printf("     (originating module of matrix = '%s')\n",module_name);
    printf("-> cur matrix size = %d by %d, desired size = %d by %d\n",
	   mat->rows, mat->cols, rows, cols);
    exit(1);
   }

if (rows < 0 || cols < 0)
   {
    printf("error in 'Matrix::set_size':  rows and cols must be >= 0\n");
    printf("  in this case, rows = %d, cols = %d\n",rows,cols);
    printf("  and you ran set_size() on matrix '%s'\n",name);
    printf("     (originating module of matrix = '%s')\n",module_name);
    exit(1);
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
      printf("error in 'Matrix::set_size':  calloc call failed - \n");   
      printf("  not enough memory!!\n");
      printf("  matrix name = '%s', desired rows = %d\n",name,rows);   
      printf("     (originating module name of matrix = '%s')\n",module_name);
      exit(1);   
      }
   for (i = 0; i < rows; i++)   
      if (((mat->elem)[i] = (double *) calloc(cols,sizeof(double))) == NULL)
         {   
         printf("error in 'Matrix::set_size':  calloc call failed - \n");   
         printf("  not enough memory!!\n");
         printf("  matrix name = '%s', desired cols = %d\n",name,cols);
         printf("   (originating module name of matrix = '%s')\n",module_name);
         exit(1);   
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
    printf("error in 'Matrix::init()': you cannot initialize a matrix\n");
    printf("  that has already been initialized or assigned from another matrix\n");
    printf("  in this case, you ran init() on matrix '%s'\n",name);
    printf("     (originating in module '%s')\n",module_name);
    exit(1);
  }
   
if ((mat = (matrix_struct *) malloc(sizeof(matrix_struct))) == NULL)   
   {   
   printf("error in 'Matrix::init()':  malloc call failed\n");   
   printf("out of memory!\n");
   printf("  matrix name = '%s'\n",name);   
   printf("     (originating module name of matrix = '%s')\n",module_name);
   exit(1);   
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
    printf("error in 'Matrix::set_elem':  row and column index must be >= 0\n");
    printf("  in this case, index_row = %d, index_col = %d\n",index_row,
             index_col);
    printf("  and you ran set_elem() on matrix '%s'\n",name);
    printf("     (originating module of matrix = '%s')\n",module_name);
    exit(1);
   }

if (index_row >= mat->rows)
    {
    printf("error in 'Matrix::set_elem':  index_row must be < rows of matrix\n");
    printf("  in this case, index_row = %d, matrix is %d by %d\n",index_row,
               mat->rows, mat->cols);
    printf("  and you ran set_elem() on matrix '%s'\n",name);
    printf("     (originating module of matrix = '%s')\n",module_name);
    exit(1);
    }

if (index_col >= mat->cols)
    {
    printf("error in 'Matrix::set_elem':  index_col must be < cols of matrix\n");
    printf("  in this case, index_col = %d, matrix is %d by %d\n",index_col,
               mat->rows, mat->cols);
    printf("  and you ran set_elem() on matrix '%s'\n",name);
    printf("     (originating module of matrix = '%s')\n",module_name);
    exit(1);
    }

mat->elem[index_row][index_col] = val;
}   

double Matrix::get_elem(int index_row, int index_col) const
{

if (index_row < 0 || index_col < 0)
   {
    printf("error in 'Matrix::get_elem':  index for row and column must be >= 0\n");
    printf("  in this case, index_row = %d, index_col = %d\n",index_row,
              index_col);
    printf("  and you ran get_elem() on matrix '%s'\n",name);
    printf("     (originating module of matrix = '%s')\n",module_name);
    exit(1);
   }

if (index_row >= mat->rows)
    {
    printf("error in 'Matrix::get_elem':  index_row must be < rows of matrix\n");
    printf("  in this case, index_row = %d, matrix is %d by %d\n",
          index_row, mat->rows, mat->cols);
    printf("  and you ran get_elem() on matrix '%s'\n",name);
    printf("     (originating module of matrix = '%s')\n",module_name);
    exit(1);
    }

if (index_col >= mat->cols)
    {
    printf("error in 'Matrix::get_elem':  index_col must be < cols of matrix\n");
    printf("  in this case, index_col = %d, matrix is %d by %d\n",
          index_col, mat->rows, mat->cols);
    printf("  and you ran get_elem() on matrix '%s'\n",name);
    printf("     (originating module of matrix = '%s')\n",module_name);
    exit(1);
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
    printf("error in 'IntMatrix' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    exit(1);   
   }
sprintf(module_name,"unknown");

if ((name = (char *) calloc(10,sizeof(char))) == NULL)   
   {   
    printf("error in 'IntMatrix' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    exit(1);   
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
    printf("error in 'IntMatrix' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  module_name = '%s'\n",module_name_in);
    printf("  desired module_name length = %d\n",i+1);   
    exit(1);   
   }
strcpy(module_name,module_name_in);

for (i = 0; name_in[i] != '\0'; i++);
if ((name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'IntMatrix' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  name = '%s'\n",name_in);
    printf("  desired name length = %d\n",i+1);   
    exit(1);   
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
     printf("Error:  you are using the wrong constructor!!!\n");
     printf("   ---> use either 'IntMatrix mat1;' or 'IntMatrix mat1(\"mod_name\",\"name\");'\n");
     printf("   in this case, you used 'IntMatrix %s(\"%s\",\"%s\",\"%s\");'\n",
	    name_in, module_name_in,name_in,null_string);
     exit(1);
   }

for (i = 0; module_name_in[i] != '\0'; i++);
if ((module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'IntMatrix' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  module_name = '%s'\n",module_name_in);
    printf("  desired module_name length = %d\n",i+1);   
    exit(1);   
   }
strcpy(module_name,module_name_in);

for (i = 0; name_in[i] != '\0'; i++);
if ((name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'IntMatrix' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  name = '%s'\n",name_in);
    printf("  desired name length = %d\n",i+1);   
    exit(1);   
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
    printf("error in 'IntMatrix' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  name = '%s'\n",other.name);
    printf("  desired name length = %d\n",j+45);   
    exit(1);   
   }
sprintf(name,"derived from: %s",other.name);

for (i = 0; other.module_name[i] != '\0'; i++);
if ((module_name = (char *) calloc(i+j+30,sizeof(char))) == NULL)   
   {   
    printf("error in 'IntMatrix' constructor:  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  module_name = '%s'\n",other.module_name);
    printf("  desired module_name length = %d\n",i+j+30);   
    exit(1);   
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


IntMatrix& IntMatrix::operator = (const IntMatrix &other) const
{
if (mat != other.mat)
  {
    copy(other);
  }
}

IntMatrix& IntMatrix::operator = (IntMatrix &other) const
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
    printf("error in 'IntMatrix::set_size':  cannot change the size of a\n");
    printf("   matrix declared in the 'outputs:' section of a module in the modules.par file\n");
    printf("   EXCEPT in the 'init_output_vectors:' section of the modules.par file\n");
    printf("   in this case, you tried changing the size of matrix '%s'\n",name);
    printf("     (originating module of matrix = '%s')\n",module_name);
    printf("-> cur matrix size = %d by %d, desired size = %d by %d\n",
	   mat->rows, mat->cols, rows, cols);
    exit(1);
   }

if (rows < 0 || cols < 0)
   {
    printf("error in 'IntMatrix::set_size':  rows and cols must be >= 0\n");
    printf("  in this case, rows = %d, cols = %d\n",rows,cols);
    printf("  and you ran set_size() on matrix '%s'\n",name);
    printf("     (originating module of matrix = '%s')\n",module_name);
    exit(1);
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
      printf("error in 'IntMatrix::set_size':  calloc call failed - \n");   
      printf("  not enough memory!!\n");
      printf("  matrix name = '%s', desired rows = %d\n",name,rows);   
      printf("     (originating module name of matrix = '%s')\n",module_name);
      exit(1);   
      }
   for (i = 0; i < rows; i++)   
      if (((mat->elem)[i] = (int *) calloc(cols,sizeof(int))) == NULL)
         {   
         printf("error in 'IntMatrix::set_size':  calloc call failed - \n");   
         printf("  not enough memory!!\n");
         printf("  matrix name = '%s', desired cols = %d\n",name,cols);
         printf("   (originating module name of matrix = '%s')\n",module_name);
         exit(1);   
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
    printf("error in 'IntMatrix::init()': you cannot initialize a matrix\n");
    printf("  that has already been initialized or assigned from another matrix\n");
    printf("  in this case, you ran init() on matrix '%s'\n",name);
    printf("     (originating in module '%s')\n",module_name);
    exit(1);
  }
   
if ((mat = (int_matrix_struct *) malloc(sizeof(int_matrix_struct))) == NULL)   
   {   
   printf("error in 'IntMatrix::init()':  malloc call failed\n");   
   printf("out of memory!\n");
   printf("  matrix name = '%s'\n",name);   
   printf("     (originating module name of matrix = '%s')\n",module_name);
   exit(1);   
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
    printf("error in 'IntMatrix::set_elem':  row and column index must be >= 0\n");
    printf("  in this case, index_row = %d, index_col = %d\n",index_row,
             index_col);
    printf("  and you ran set_elem() on matrix '%s'\n",name);
    printf("     (originating module of matrix = '%s')\n",module_name);
    exit(1);
   }

if (index_row >= mat->rows)
    {
    printf("error in 'IntMatrix::set_elem':  index_row must be < rows of matrix\n");
    printf("  in this case, index_row = %d, matrix is %d by %d\n",index_row,
               mat->rows, mat->cols);
    printf("  and you ran set_elem() on matrix '%s'\n",name);
    printf("     (originating module of matrix = '%s')\n",module_name);
    exit(1);
    }

if (index_col >= mat->cols)
    {
    printf("error in 'IntMatrix::set_elem':  index_col must be < cols of matrix\n");
    printf("  in this case, index_col = %d, matrix is %d by %d\n",index_col,
               mat->rows, mat->cols);
    printf("  and you ran set_elem() on matrix '%s'\n",name);
    printf("     (originating module of matrix = '%s')\n",module_name);
    exit(1);
    }

mat->elem[index_row][index_col] = val;
}   

int IntMatrix::get_elem(int index_row, int index_col) const
{

if (index_row < 0 || index_col < 0)
   {
    printf("error in 'IntMatrix::get_elem':  index for row and column must be >= 0\n");
    printf("  in this case, index_row = %d, index_col = %d\n",index_row,
              index_col);
    printf("  and you ran get_elem() on matrix '%s'\n",name);
    printf("     (originating module of matrix = '%s')\n",module_name);
    exit(1);
   }

if (index_row >= mat->rows)
    {
    printf("error in 'IntMatrix::get_elem':  index_row must be < rows of matrix\n");
    printf("  in this case, index_row = %d, matrix is %d by %d\n",
          index_row, mat->rows, mat->cols);
    printf("  and you ran get_elem() on matrix '%s'\n",name);
    printf("     (originating module of matrix = '%s')\n",module_name);
    exit(1);
    }

if (index_col >= mat->cols)
    {
    printf("error in 'IntMatrix::get_elem':  index_col must be < cols of matrix\n");
    printf("  in this case, index_col = %d, matrix is %d by %d\n",
          index_col, mat->rows, mat->cols);
    printf("  and you ran get_elem() on matrix '%s'\n",name);
    printf("     (originating module of matrix = '%s')\n",module_name);
    exit(1);
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
   printf("error in 'trans(A,B)':  matrix B must be distinct from matrix A\n");
   printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   printf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
   exit(1);   
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
   printf("error in 'init_matrix_struct':  malloc call failed\n");   
   printf("out of memory!\n");
   exit(1);   
  }   

int i;
for (i = 0; module_name[i] != '\0'; i++);
   
if ((A->module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'init_matrix_struct':  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  module_name = '%s'\n",module_name);
    printf("  desired module_name length = %d\n",i+1);   
    exit(1);   
   }
strcpy(A->module_name,module_name);

for (i = 0; name[i] != '\0'; i++);
if ((A->name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'init_matrix_struct':  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  name = '%s'\n",name);
    printf("  desired name length = %d\n",i+1);   
    exit(1);   
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
      printf("error in 'read_elem':  row and col must be >= 0\n");
      printf("  in this case, for matrix '%s':\n",A->name);
      printf("  -->  row = %d, col = %d\n",row,col);
      printf("  originating module name of matrix = '%s'\n",A->module_name);   
      exit(1);
    }

if (row >= A->rows || col >= A->cols)
    {
      printf("error in 'read_elem':  row must be < A->rows and col must be < A->cols\n");
      printf("  in this case, for matrix '%s':\n",A->name);
      printf("  -->  row = %d, A->rows = %d --- col = %d, A->cols = %d\n",
               row, A->rows, col, A->cols);
      printf("  originating module name of matrix = '%s'\n",A->module_name);   
      exit(1);
    }

return(A->elem[row][col]);
}   

void write_elem(double val, int row, int col, matrix_struct *A)
{
if (row < 0 || col < 0)
    {
      printf("error in 'write_elem':  row and col must be >= 0\n");
      printf("  in this case, for matrix '%s':\n",A->name);
      printf("  -->  row = %d, col = %d\n",row,col);
      printf("  originating module name of matrix = '%s'\n",A->module_name);   
      exit(1);
    }

if (row >= A->rows || col >= A->cols)
    {
      printf("error in 'write_elem':  row must be < A->rows and col must be < A->cols\n");
      printf("  in this case, for matrix '%s':\n",A->name);
      printf("  -->  row = %d, A->rows = %d --- col = %d, A->cols = %d\n",
               row, A->rows, col, A->cols);
      printf("  originating module name of matrix = '%s'\n",A->module_name);   
      exit(1);
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

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
       y_elem[i][j] = x_elem[i][j];
}


void print(matrix_struct *x)
{
int i,j;

printf("matrix name = '%s' (originated in module '%s')\n",
	x->name, x->module_name);

for (i = 0; i < x->rows; i++)
  {
   printf("  %s[%d][0:%d] =",x->name,i,x->cols-1);
   for (j = 0; j < x->cols; j++)
       printf(" %5.3e",x->elem[i][j]);
   printf("\n");
  }
}


matrix_struct *extract_matrix_struct(const Matrix &in)
{
if (in.mat == NULL)
    {
      printf("error in 'extract_matrix_struct': mat is NULL!\n");
      printf("  this shouldn't happen in normal use of matrices\n");
      printf("  name of matrix = '%s'\n",in.name);
      printf("      (originating module of matrix = '%s')\n",in.module_name);
      exit(1);
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
    printf("error in 'Matrix::set_size':  cannot change the size of a\n");
    printf("   matrix declared in the 'outputs:' section of a module in the modules.par file\n");
    printf("   EXCEPT in the 'init_output_vectors:' section of the modules.par file\n");
    printf("   in this case, you tried changing the size of matrix '%s'\n",
               mat->name);
    printf("     (originating module of matrix = '%s')\n",mat->module_name);
    printf("-> cur matrix size = %d by %d, desired size = %d by %d\n",
	   mat->rows, mat->cols, rows, cols);
    exit(1);
   }

if (rows < 0 || cols < 0)
   {
    printf("error in 'Matrix::set_size':  rows and cols must be >= 0\n");
    printf("  in this case, rows = %d, cols = %d\n",rows,cols);
    printf("  and you ran set_size() on matrix '%s'\n",mat->name);
    printf("     (originating module of matrix = '%s')\n",mat->module_name);
    exit(1);
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
      printf("error in 'Matrix::set_size':  calloc call failed - \n");   
      printf("  not enough memory!!\n");
      printf("  matrix name = '%s', desired rows = %d\n",mat->name,rows);   
      printf("     (originating module name of matrix = '%s')\n",
                mat->module_name);
      exit(1);   
      }
   for (i = 0; i < rows; i++)   
      if (((mat->elem)[i] = (double *) calloc(cols,sizeof(double))) == NULL)
         {   
         printf("error in 'Matrix::set_size':  calloc call failed - \n");   
         printf("  not enough memory!!\n");
         printf("  matrix name = '%s', desired cols = %d\n",mat->name,cols);
         printf("   (originating module name of matrix = '%s')\n",
                mat->module_name);
         exit(1);   
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
   printf("\nerror in 'load': file %s can't be opened\n",filename);   
   exit(1);   
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
                  printf("error in 'load':\n\n");   
                  printf("  file '%s':  number of cols needs to be the same in each row\n",filename);   
                  printf("              in order to fill matrix %s\n",A->name);   
                  printf("(this condition is violated on line %d of file)\n",   
                          line_count);   
                  exit(1);   
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
               printf("\nerror in 'load':  matrix %s needs %d values,\n",   
                       A->name,rows*cols);    
               printf("but file %s has only %d data values to fill it with\n",   
                       filename,entry_count);   
               exit(1);   
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
   
if (strcmp(control,"w") == 0 || strcmp(control,"wl") == 0)   
  {   
   if ((fp = fopen(filename,"w")) == NULL)   
     {   
      printf("error in 'save':  file %s cannot be opened\n",filename);   
      exit(1);   
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
      printf("error in 'send_to_file':  file %s cannot be opened\n",filename);   
      exit(1);   
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
   printf("error in 'save':  control character was specified as %s,\n",   
           control);   
   printf("which is invalid since it must be specified as either 'w' (for\n");   
   printf("writing to a file), or 'a' (for appending to a file),\n");   
   printf("or 'wl' (write and label matrix in file), or 'al'\n");   
   printf("(append and label matrix to file)\n");   
   exit(1);   
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
     printf("error in 'mul(A,B,C)':  matrix A->cols must equal B->rows\n");
     printf("   in this case, A->cols = %d, B->rows = %d\n",A->cols,B->rows);
     printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     printf("   matrix C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     exit(1);
   }
if (C == A || C == B)
   {
     printf("error in 'mul(A,B,C)':  matrix C must be distinct from matrix A and B\n");
     printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     printf("   matrix C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     exit(1);   
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
     printf("error in 'add(A,B,C)':  rows and cols of A and B must be the same\n");
     printf("   in this case, A->rows = %d, B->rows = %d\n",A->rows,B->rows);
     printf("   in this case, A->cols = %d, B->cols = %d\n",A->cols,B->cols);
     printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     printf("   matrix C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     exit(1);
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
     printf("error in 'add(A,B,C)':  rows and cols of A and B must be the same\n");
     printf("   in this case, A->rows = %d, B->rows = %d\n",A->rows,B->rows);
     printf("   in this case, A->cols = %d, B->cols = %d\n",A->cols,B->cols);
     printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     printf("   matrix C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     exit(1);
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
   printf("error in 'trans(A,B)':  matrix B must be distinct from matrix A\n");
   printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   printf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
   exit(1);   
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
   printf("error in 'init_matrix_struct':  malloc call failed\n");   
   printf("out of memory!\n");
   exit(1);   
  }   

int i;
for (i = 0; module_name[i] != '\0'; i++);
   
if ((A->module_name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'init_matrix_struct':  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  module_name = '%s'\n",module_name);
    printf("  desired module_name length = %d\n",i+1);   
    exit(1);   
   }
strcpy(A->module_name,module_name);

for (i = 0; name[i] != '\0'; i++);
if ((A->name = (char *) calloc(i+1,sizeof(char))) == NULL)   
   {   
    printf("error in 'init_matrix_struct':  calloc call failed - \n");   
    printf("  not enough memory!!\n");
    printf("  name = '%s'\n",name);
    printf("  desired name length = %d\n",i+1);   
    exit(1);   
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
      printf("error in 'read_elem':  row and col must be >= 0\n");
      printf("  in this case, for matrix '%s':\n",A->name);
      printf("  -->  row = %d, col = %d\n",row,col);
      printf("  originating module name of matrix = '%s'\n",A->module_name);   
      exit(1);
    }

if (row >= A->rows || col >= A->cols)
    {
      printf("error in 'read_elem':  row must be < A->rows and col must be < A->cols\n");
      printf("  in this case, for matrix '%s':\n",A->name);
      printf("  -->  row = %d, A->rows = %d --- col = %d, A->cols = %d\n",
               row, A->rows, col, A->cols);
      printf("  originating module name of matrix = '%s'\n",A->module_name);   
      exit(1);
    }

return(A->elem[row][col]);
}   

void write_elem(int val, int row, int col, int_matrix_struct *A)
{
if (row < 0 || col < 0)
    {
      printf("error in 'write_elem':  row and col must be >= 0\n");
      printf("  in this case, for matrix '%s':\n",A->name);
      printf("  -->  row = %d, col = %d\n",row,col);
      printf("  originating module name of matrix = '%s'\n",A->module_name);   
      exit(1);
    }

if (row >= A->rows || col >= A->cols)
    {
      printf("error in 'write_elem':  row must be < A->rows and col must be < A->cols\n");
      printf("  in this case, for matrix '%s':\n",A->name);
      printf("  -->  row = %d, A->rows = %d --- col = %d, A->cols = %d\n",
               row, A->rows, col, A->cols);
      printf("  originating module name of matrix = '%s'\n",A->module_name);   
      exit(1);
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

for (i = 0; i < rows; i++)
   for (j = 0; j < cols; j++)
       y_elem[i][j] = x_elem[i][j];
}


void print(int_matrix_struct *x)
{
int i,j;

printf("matrix name = '%s' (originated in module '%s')\n",
	x->name, x->module_name);

for (i = 0; i < x->rows; i++)
  {
   printf("  %s[%d][0:%d] =",x->name,i,x->cols-1);
   for (j = 0; j < x->cols; j++)
       printf(" %d",x->elem[i][j]);
   printf("\n");
  }
}


int_matrix_struct *extract_matrix_struct(const IntMatrix &in)
{
if (in.mat == NULL)
    {
      printf("error in 'extract_matrix_struct': mat is NULL!\n");
      printf("  this shouldn't happen in normal use of matrices\n");
      printf("  name of matrix = '%s'\n",in.name);
      printf("      (originating module of matrix = '%s')\n",in.module_name);
      exit(1);
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
    printf("error in 'Matrix::set_size':  cannot change the size of a\n");
    printf("   matrix declared in the 'outputs:' section of a module in the modules.par file\n");
    printf("   EXCEPT in the 'init_output_vectors:' section of the modules.par file\n");
    printf("   in this case, you tried changing the size of matrix '%s'\n",
               mat->name);
    printf("     (originating module of matrix = '%s')\n",mat->module_name);
    printf("-> cur matrix size = %d by %d, desired size = %d by %d\n",
	   mat->rows, mat->cols, rows, cols);
    exit(1);
   }

if (rows < 0 || cols < 0)
   {
    printf("error in 'Matrix::set_size':  rows and cols must be >= 0\n");
    printf("  in this case, rows = %d, cols = %d\n",rows,cols);
    printf("  and you ran set_size() on matrix '%s'\n",mat->name);
    printf("     (originating module of matrix = '%s')\n",mat->module_name);
    exit(1);
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
      printf("error in 'Matrix::set_size':  calloc call failed - \n");   
      printf("  not enough memory!!\n");
      printf("  matrix name = '%s', desired rows = %d\n",mat->name,rows);   
      printf("     (originating module name of matrix = '%s')\n",
                mat->module_name);
      exit(1);   
      }
   for (i = 0; i < rows; i++)   
      if (((mat->elem)[i] = (int *) calloc(cols,sizeof(int))) == NULL)
         {   
         printf("error in 'Matrix::set_size':  calloc call failed - \n");   
         printf("  not enough memory!!\n");
         printf("  matrix name = '%s', desired cols = %d\n",mat->name,cols);
         printf("   (originating module name of matrix = '%s')\n",
                mat->module_name);
         exit(1);   
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
   
if (strcmp(control,"w") == 0 || strcmp(control,"wl") == 0)   
  {   
   if ((fp = fopen(filename,"w")) == NULL)   
     {   
      printf("error in 'save_struct':  file %s cannot be opened\n",filename);   
      exit(1);   
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
      printf("error in 'save_struct':  file %s cannot be opened\n",filename);   
      exit(1);   
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
   printf("error in 'save_struct':  control character was specified as %s,\n",   
           control);   
   printf("which is invalid since it must be specified as either 'w' (for\n");   
   printf("writing to a file), or 'a' (for appending to a file),\n");   
   printf("or 'wl' (write and label matrix in file), or 'al'\n");   
   printf("(append and label matrix to file)\n");   
   exit(1);   
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
   printf("\nerror in 'load': file %s can't be opened\n",filename);   
   exit(1);   
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
                  printf("error in 'load':\n\n");   
                  printf("  file '%s':  number of cols needs to be the same in each row\n",filename);   
                  printf("              in order to fill matrix %s\n",A->name);   
                  printf("(this condition is violated on line %d of file)\n",   
                          line_count);   
                  exit(1);   
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
               printf("\nerror in 'load':  matrix %s needs %d values,\n",   
                       A->name,rows*cols);    
               printf("but file %s has only %d data values to fill it with\n",   
                       filename,entry_count);   
               exit(1);   
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
     printf("error in 'mul(A,B,C)':  matrix A->cols must equal B->rows\n");
     printf("   in this case, A->cols = %d, B->rows = %d\n",A->cols,B->rows);
     printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     printf("   matrix C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     exit(1);
   }
if (C == A || C == B)
   {
     printf("error in 'mul(A,B,C)':  matrix C must be distinct from matrix A and B\n");
     printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     printf("   matrix C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     exit(1);   
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
     printf("error in 'add(A,B,C)':  rows and cols of A and B must be the same\n");
     printf("   in this case, A->rows = %d, B->rows = %d\n",A->rows,B->rows);
     printf("   in this case, A->cols = %d, B->cols = %d\n",A->cols,B->cols);
     printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     printf("   matrix C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     exit(1);
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
     printf("error in 'add(A,B,C)':  rows and cols of A and B must be the same\n");
     printf("   in this case, A->rows = %d, B->rows = %d\n",A->rows,B->rows);
     printf("   in this case, A->cols = %d, B->cols = %d\n",A->cols,B->cols);
     printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     printf("   matrix C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     exit(1);
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
	 printf("error in 'copy(index,\"col\",Matrix,Vector):\n");
         printf("   index must be >= 0\n");
         printf("   in this case, index = %d\n",index);
	 printf("     name of Vector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of Matrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
       }
    if (index >= from->cols)
      {
	 printf("error in 'copy(index,\"col\",Matrix,Vector):\n");
         printf("   index must be < Matrix cols\n");
         printf("   in this case, index = %d, Matrix cols = %d\n",
                     index,from->cols);
	 printf("     name of Vector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of Matrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
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
	 printf("error in 'copy(index,\"row\",Matrix,Vector):\n");
         printf("   index must be >= 0\n");
         printf("   in this case, index = %d\n",index);
	 printf("     name of Vector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of Matrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
       }
    if (index >= from->rows)
      {
	 printf("error in 'copy(index,\"row\",Matrix,Vector):\n");
         printf("   index must be < Matrix rows\n");
         printf("   in this case, index = %d, Matrix rows = %d\n",
                     index,from->rows);
	 printf("     name of Vector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of Matrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
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
   printf("error in 'Vector::copy(index,\"row/col\",Matrix):\n");
   printf("   \"row/col\" must be either \"row\" or \"col\"\n");
   printf("   in this case, \"row/col\" = \"%s\"\n",row_or_col);
   printf("     name of Vector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
   printf("     name of Matrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
   exit(1);
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
	 printf("error in 'copy(index,\"col\",Vector,Matrix):\n");
         printf("   index must be >= 0\n");
         printf("   in this case, index = %d\n",index);
	 printf("     name of Matrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of Vector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
       }
    if (index >= to->cols)
      {
	 printf("error in 'copy(index,\"col\",Vector,Matrix):\n");
         printf("   index must be < Matrix cols\n");
         printf("   in this case, index = %d, Matrix cols = %d\n",
                     index,to->cols);
	 printf("     name of Matrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of Vector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
      }
    if (from->length != to->rows)
       {
	 printf("error in 'copy(index,\"col\",Vector,Matrix):\n");
         printf("   Vector length must equal Matrix rows\n");
         printf("   in this case, Vector length = %d, Matrix rows = %d\n",
                  from->length, to->rows);
	 printf("     name of Matrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of Vector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
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
	 printf("error in 'copy(index,\"row\",Vector,Matrix):\n");
         printf("   index must be >= 0\n");
         printf("   in this case, index = %d\n",index);
	 printf("     name of Matrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of Vector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
       }
    if (index >= to->rows)
      {
	 printf("error in 'copy(index,\"row\",Vector,Matrix):\n");
         printf("   index must be < Matrix rows\n");
         printf("   in this case, index = %d, Matrix rows = %d\n",
                     index,to->rows);
	 printf("     name of Matrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of Vector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
      }
    if (from->length != to->cols)
       {
	 printf("error in 'copy(index,\"row\",Vector,Matrix):\n");
         printf("   Vector length must equal Matrix cols\n");
         printf("   in this case, Vector length = %d, Matrix cols = %d\n",
                  from->length, to->cols);
	 printf("     name of Matrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of Vector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
       }

    vec_elem = from->elem;
    mat_elem = to->elem;
    cols = to->cols;

    for (i = 0; i < cols; i++)
       mat_elem[index][i] = vec_elem[i];
   }
else
   {
   printf("error in 'copy(index,\"row/col\",Vector,Matrix):\n");
   printf("   \"row/col\" must be either \"row\" or \"col\"\n");
   printf("   in this case, \"row/col\" = \"%s\"\n",row_or_col);
   printf("     name of Matrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
   printf("     name of Vector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
   exit(1);
   }
}

void mul(matrix_struct *A, vector_struct *B, vector_struct *C)
{
int i,k,rows,span;
double **A_elem,*B_elem,*C_elem;
double temp;

if (A->cols != B->length)
   {
     printf("error in 'mul(A,B,C)':  matrix A->cols must equal B->length\n");
     printf("   in this case, A->cols = %d, B->length = %d\n",A->cols,B->length);
     printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   vector B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     printf("   vector C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     exit(1);
   }
if (C == B)
   {
     printf("error in 'mul(A,B,C)':  vector C must be distinct from vector B\n");
     printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   vector B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     printf("   vector C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     exit(1);   
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
     printf("error in 'mul(A,B,C)':  matrix A->length must equal B->rows\n");
     printf("   in this case, A->length = %d, B->rows = %d\n",A->length,
            B->rows);
     printf("   vector A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     printf("   vector C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     exit(1);
   }
if (C == A)
   {
     printf("error in 'mul(A,B,C)':  vector C must be distinct from vector A\n");
     printf("   vector A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     printf("   vector C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     exit(1);   
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
	 printf("error in 'copy(index,\"col\",IntVector,IntMatrix):\n");
         printf("   index must be >= 0\n");
         printf("   in this case, index = %d\n",index);
	 printf("     name of IntMatrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of IntVector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
       }
    if (index >= to->cols)
      {
	 printf("error in 'copy(index,\"col\",IntVector,IntMatrix):\n");
         printf("   index must be < IntMatrix cols\n");
         printf("   in this case, index = %d, IntMatrix cols = %d\n",
                     index,to->cols);
	 printf("     name of IntMatrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of IntVector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
      }
    if (from->length != to->rows)
       {
	 printf("error in 'copy(index,\"col\",IntVector,IntMatrix):\n");
         printf("   IntVector length must equal IntMatrix rows\n");
         printf("   in this case, IntVector length = %d, IntMatrix rows = %d\n",
                  from->length, to->rows);
	 printf("     name of IntMatrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of IntVector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
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
	 printf("error in 'copy(index,\"row\",IntVector,IntMatrix):\n");
         printf("   index must be >= 0\n");
         printf("   in this case, index = %d\n",index);
	 printf("     name of IntMatrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of IntVector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
       }
    if (index >= to->rows)
      {
	 printf("error in 'copy(index,\"row\",IntVector,IntMatrix):\n");
         printf("   index must be < IntMatrix rows\n");
         printf("   in this case, index = %d, IntMatrix rows = %d\n",
                     index,to->rows);
	 printf("     name of IntMatrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of IntVector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
      }
    if (from->length != to->cols)
       {
	 printf("error in 'copy(index,\"row\",IntVector,IntMatrix):\n");
         printf("   IntVector length must equal IntMatrix cols\n");
         printf("   in this case, IntVector length = %d, IntMatrix cols = %d\n",
                  from->length, to->cols);
	 printf("     name of IntMatrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of IntVector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
       }

    vec_elem = from->elem;
    mat_elem = to->elem;
    cols = to->cols;

    for (i = 0; i < cols; i++)
       mat_elem[index][i] = vec_elem[i];
   }
else
   {
   printf("error in 'copy(index,\"row/col\",IntVector,IntMatrix):\n");
   printf("   \"row/col\" must be either \"row\" or \"col\"\n");
   printf("   in this case, \"row/col\" = \"%s\"\n",row_or_col);
   printf("     name of IntMatrix:  '%s', originating module: %s\n",
                   to->name,to->module_name);
   printf("     name of IntVector:  '%s', originating module: %s\n",
                   from->name,from->module_name);
   exit(1);
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
	 printf("error in 'copy(index,\"col\",IntMatrix,IntVector):\n");
         printf("   index must be >= 0\n");
         printf("   in this case, index = %d\n",index);
	 printf("     name of IntVector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of IntMatrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
       }
    if (index >= from->cols)
      {
	 printf("error in 'copy(index,\"col\",IntMatrix,IntVector):\n");
         printf("   index must be < IntMatrix cols\n");
         printf("   in this case, index = %d, IntMatrix cols = %d\n",
                     index,from->cols);
	 printf("     name of IntVector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of IntMatrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
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
	 printf("error in 'copy(index,\"row\",IntMatrix,IntVector):\n");
         printf("   index must be >= 0\n");
         printf("   in this case, index = %d\n",index);
	 printf("     name of IntVector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of IntMatrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
       }
    if (index >= from->rows)
      {
	 printf("error in 'copy(index,\"row\",IntMatrix,IntVector):\n");
         printf("   index must be < IntMatrix rows\n");
         printf("   in this case, index = %d, IntMatrix rows = %d\n",
                     index,from->rows);
	 printf("     name of IntVector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
         printf("     name of IntMatrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
         exit(1);
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
   printf("error in 'IntVector::copy(index,\"row/col\",IntMatrix):\n");
   printf("   \"row/col\" must be either \"row\" or \"col\"\n");
   printf("   in this case, \"row/col\" = \"%s\"\n",row_or_col);
   printf("     name of IntVector:  '%s', originating module: %s\n",
                   to->name,to->module_name);
   printf("     name of IntMatrix:  '%s', originating module: %s\n",
                   from->name,from->module_name);
   exit(1);
   }
}

void mul(int_matrix_struct *A, int_vector_struct *B, int_vector_struct *C)
{
int i,k,rows,span;
int **A_elem,*B_elem,*C_elem;
int temp;

if (A->cols != B->length)
   {
     printf("error in 'mul(A,B,C)':  matrix A->cols must equal B->length\n");
     printf("   in this case, A->cols = %d, B->length = %d\n",A->cols,B->length);
     printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   vector B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     printf("   vector C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     exit(1);
   }
if (C == B)
   {
     printf("error in 'mul(A,B,C)':  vector C must be distinct from vector B\n");
     printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   vector B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     printf("   vector C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     exit(1);   
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
     printf("error in 'mul(A,B,C)':  matrix A->length must equal B->rows\n");
     printf("   in this case, A->length = %d, B->rows = %d\n",A->length,
            B->rows);
     printf("   vector A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     printf("   vector C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     exit(1);
   }
if (C == A)
   {
     printf("error in 'mul(A,B,C)':  vector C must be distinct from vector A\n");
     printf("   vector A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
     printf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
     printf("   vector C: '%s' (originating module: '%s')\n",
	    C->name,C->module_name);
     exit(1);   
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
   printf("error in 'inv(A,B)':  input matrix A is not square!\n");   
   printf("   in this case, A->rows = %d, A->cols = %d\n",A->rows,A->cols);
   printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   printf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
   exit(1);   
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
    printf("error in 'inv(A,B)':  input matrix A is singular!\n",   
               A->name);   
    printf("  (condition number = %f, minimum singular value = %f\n",   
                  max/min,min);   
    printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
    printf("   matrix B: '%s' (originating module: '%s')\n",
	    B->name,B->module_name);
    exit(1);   
   }   
if ((inv_rcond = log10(min)-log10(max)) <= INV_DP_RC_L)   
   {   
    printf("warning in 'inv(A,B)':  input matrix A is ill-conditioned!\n");   
    printf("  (matrix %s has condition number: %f)\n",A->name,max/min);   
    printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
    printf("   matrix B: '%s' (originating module: '%s')\n",
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
   
if (A->rows < A->cols)   
  {   
   printf("error in 'svd(A,U,W,V)':  input matrix A needs to have more rows");
   printf(" than columns\n  (input matrix A is %d by %d)\n",A->rows,A->cols);
   printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   printf("   matrix U: '%s' (originating module: '%s')\n",
	    U->name,U->module_name);
   printf("   matrix W: '%s' (originating module: '%s')\n",
	    W->name,W->module_name);
   printf("   matrix V: '%s' (originating module: '%s')\n",
	    V->name,V->module_name);
   exit(1);   
 }   
if (A == U || A == W || A == V)   
  {   
   printf("error in 'svd(A,U,W,V)':  input matrix A must be distinct from\n");
   printf("   matrices U, W, and V\n");
   printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   printf("   matrix U: '%s' (originating module: '%s')\n",
	    U->name,U->module_name);
   printf("   matrix W: '%s' (originating module: '%s')\n",
	    W->name,W->module_name);
   printf("   matrix V: '%s' (originating module: '%s')\n",
	    V->name,V->module_name);
   exit(1);   
 }   
if (U == W || V == W || U == V)   
  {   
   printf("error in 'svd(A,U,W,V)':  matrices U, W, and V must be\n");
   printf("   distinct from each other\n");
   printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   printf("   matrix U: '%s' (originating module: '%s')\n",
	    U->name,U->module_name);
   printf("   matrix W: '%s' (originating module: '%s')\n",
	    W->name,W->module_name);
   printf("   matrix V: '%s' (originating module: '%s')\n",
	    V->name,V->module_name);
   exit(1);   
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
   printf("error in 'svd(A,U,W,V)':  calloc call failed\n");   
   printf(" - not enough memory !\n");   
   printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   printf("   matrix U: '%s' (originating module: '%s')\n",
	    U->name,U->module_name);
   printf("   matrix W: '%s' (originating module: '%s')\n",
	    W->name,W->module_name);
   printf("   matrix V: '%s' (originating module: '%s')\n",
	    V->name,V->module_name);
   exit(1);   
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
                             printf("error in 'svd':  ");   
                             printf("no convergence in %d SVDCMP iterations\n",MAX_ITER);   
                             exit(1);   
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
double *tmp,*b_temp;   
matrix_struct *U,*W,*V;   
double max;   

if (A->rows <= A->cols)   
  {   
   printf("error in 'least_sq(A,b,x)':  input matrix A (where A*x = b)\n");   
   printf("  must have more rows than columns\n");   
   printf("  in this case, matrix A is %d by %d\n",A->rows, A->cols);
   printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   printf("   matrix b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
   printf("   matrix x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
   exit(1);      
  }
if (b->cols != 1)   
  {   
   printf("error in 'least_sq(A,b,x)':  input matrix b (where A*x = b)\n");   
   printf("  must be a column vector!\n");   
   printf("  in this case, matrix b is %d by %d!\n",b->rows,b->cols);   
   printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   printf("   matrix b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
   printf("   matrix x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
   exit(1);   
 }   
if (b->rows != A->rows)   
  {   
   printf("error in 'least_sq(A,b,x)':  input matrix b (where A*x = b)\n");   
   printf("  must have the same number of rows as A\n");   
   printf("  in this case, matrix b is %d by %d, matrix A is %d by %d\n",
	    b->rows, b->cols, A->rows, A->cols);
   printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   printf("   matrix b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
   printf("   matrix x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
   exit(1);   
 }

U = init_matrix_struct("least_sq_func","matrix_U");   
W = init_matrix_struct("least_sq_func","matrix_W");   
V = init_matrix_struct("least_sq_func","matrix_V");
   
if ((tmp = (double *) calloc(A->cols,sizeof(double))) == NULL)   
  {   
   printf("error in 'least_sq(A,b,x)':  calloc call failed!\n");   
   printf("    (not enough memory)\n");   
   printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   printf("   matrix b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
   printf("   matrix x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
   exit(1);   
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
      printf("warning in 'least_sq(A,b,x)':  singular values that are too small\n"); 
      printf("    have their inverse set to zero as 'A-inverse' is calculated\n"); 
      printf("   --> given the max sing. value of '%5.3e' in this case\n",max);
      printf("       the sing. value '%5.3e' was considered too small\n",
                          W->elem[j][j]); 
      printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
      printf("   matrix b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
      printf("   matrix x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
      printf("            calculation continuing....\n"); 
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
double *tmp,*b_temp;   
matrix_struct *U,*W,*V;   
double max;   

if (A->rows <= A->cols)   
  {   
   printf("error in 'least_sq(A,b,x)':  input matrix A (where A*x = b)\n");   
   printf("  must have more rows than columns\n");   
   printf("  in this case, matrix A is %d by %d\n",A->rows, A->cols);
   printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   printf("   vector b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
   printf("   vector x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
   exit(1);      
  }
if (b->length != A->rows)   
  {   
   printf("error in 'least_sq(A,b,x)':  input vector b (where A*x = b)\n");   
   printf("  must have the same length as the rows in A\n");   
   printf("  in this case, vector b has length %d, matrix A is %d by %d\n",
	    b->length, A->rows, A->cols);
   printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   printf("   vector b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
   printf("   vector x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
   exit(1);   
 }

U = init_matrix_struct("least_sq_func","matrix_U");   
W = init_matrix_struct("least_sq_func","matrix_W");   
V = init_matrix_struct("least_sq_func","matrix_V");
   
if ((tmp = (double *) calloc(A->cols,sizeof(double))) == NULL)   
  {   
   printf("error in 'least_sq(A,b,x)':  calloc call failed!\n");   
   printf("    (not enough memory)\n");   
   printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
   printf("   vector b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
   printf("   vector x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
   exit(1);   
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
      printf("warning in 'least_sq(A,b,x)':  singular values that are too small\n"); 
      printf("    have their inverse set to zero as 'A-inverse' is calculated\n"); 
      printf("   --> given the max sing. value of '%5.3e' in this case\n",max);
      printf("       the sing. value '%5.3e' was considered too small\n",
                          W->elem[j][j]); 
      printf("   matrix A: '%s' (originating module: '%s')\n",
	    A->name,A->module_name);
      printf("   vector b: '%s' (originating module: '%s')\n",
	    b->name,b->module_name);
      printf("   vector x: '%s' (originating module: '%s')\n",
	    x->name,x->module_name);
      printf("            calculation continuing....\n"); 
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
