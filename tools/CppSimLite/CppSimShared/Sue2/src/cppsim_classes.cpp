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

int List::inp(matrix *in)
{
int i;

flush();
if (in->rows == 1)
  {
   for (i = 0; i < in->cols; i++)
      inp(in->elem[0][i]);
  }
else if (in->cols == 1)
  {
   for (i = 0; i < in->rows; i++)
      inp(in->elem[i][0]);
  }
else
  {
   printf("error in List.from_matrix:  in matrix must be a vector\n");
   printf("  in this case, in matrix is %s (%d by %d)\n",
          in->name, in->rows, in->cols);
   exit(1);
  }
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
matrix *DATA,*REAL,*IMAG;
DATA = init("DATA");
REAL = init("REAL");
IMAG = init("IMAG");

list_to_matrix(data,DATA);
real_fft_mat(DATA,REAL,IMAG);
real.inp(REAL);
imag.inp(IMAG);

length = real.length;
notdone = real.notdone;
free_matrix(DATA);
free_matrix(REAL);
free_matrix(IMAG);
}

void Clist::fft(const List &data_real, const List &data_imag)
{
matrix *DATA_REAL,*DATA_IMAG,*FFT_REAL,*FFT_IMAG;

DATA_REAL = init("DATA_REAL");
DATA_IMAG = init("DATA_IMAG");
FFT_REAL = init("FFT_REAL");
FFT_IMAG = init("FFT_IMAG");

list_to_matrix(data_real,DATA_REAL);
list_to_matrix(data_imag,DATA_IMAG);
fft_mat(DATA_REAL,DATA_IMAG,FFT_REAL,FFT_IMAG);
real.inp(FFT_REAL);
imag.inp(FFT_IMAG);

length = real.length;
notdone = real.notdone;

free(DATA_REAL);
free(DATA_IMAG);
free(FFT_REAL);
free(FFT_IMAG);
}

void Clist::fft(const Clist &data)
{
matrix *DATA_REAL,*DATA_IMAG,*FFT_REAL,*FFT_IMAG;

DATA_REAL = init("DATA_REAL");
DATA_IMAG = init("DATA_IMAG");
FFT_REAL = init("FFT_REAL");
FFT_IMAG = init("FFT_IMAG");

list_to_matrix(data.real,DATA_REAL);
list_to_matrix(data.imag,DATA_IMAG);
fft_mat(DATA_REAL,DATA_IMAG,FFT_REAL,FFT_IMAG);
real.inp(FFT_REAL);
imag.inp(FFT_IMAG);

length = real.length;
notdone = real.notdone;

free(DATA_REAL);
free(DATA_IMAG);
free(FFT_REAL);
free(FFT_IMAG);
}

void Clist::ifft(const List &fft_real, const List &fft_imag)
{
matrix *DATA_REAL,*DATA_IMAG,*FFT_REAL,*FFT_IMAG;

DATA_REAL = init("DATA_REAL");
DATA_IMAG = init("DATA_IMAG");
FFT_REAL = init("FFT_REAL");
FFT_IMAG = init("FFT_IMAG");

list_to_matrix(fft_real,FFT_REAL);
list_to_matrix(fft_imag,FFT_IMAG);
ifft_mat(FFT_REAL,FFT_IMAG,DATA_REAL,DATA_IMAG);

real.inp(DATA_REAL);
imag.inp(DATA_IMAG);

length = real.length;
notdone = real.notdone;

free(DATA_REAL);
free(DATA_IMAG);
free(FFT_REAL);
free(FFT_IMAG);
}

void Clist::ifft(const Clist &fft_in)
{
matrix *DATA_REAL,*DATA_IMAG,*FFT_REAL,*FFT_IMAG;

DATA_REAL = init("DATA_REAL");
DATA_IMAG = init("DATA_IMAG");
FFT_REAL = init("FFT_REAL");
FFT_IMAG = init("FFT_IMAG");

list_to_matrix(fft_in.real,FFT_REAL);
list_to_matrix(fft_in.imag,FFT_IMAG);
ifft_mat(FFT_REAL,FFT_IMAG,DATA_REAL,DATA_IMAG);

real.inp(DATA_REAL);
imag.inp(DATA_IMAG);

length = real.length;
notdone = real.notdone;

free(DATA_REAL);
free(DATA_IMAG);
free(FFT_REAL);
free(FFT_IMAG);
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

Delay::Delay(double nom_value, double max_value)
{
int i;
if (max_value == NaN)
    buffer_size = ((int) floor(nom_value)) + 2;
else
    buffer_size = ((int) floor(max_value)) + 2;
if ((buffer = (double *) calloc(buffer_size,sizeof(double))) == NULL)
  {
    printf("Error in 'Delay': out of memory for calloc\n");
    exit(1);
  }
for (i = 0; i < buffer_size; i++)
   buffer[i] = -1.0;
delay_val = nom_value;
prev_delay_val = nom_value;
nom_val = nom_value;
out = -1.0;
state = -1;
transition_flag = 0;
}


Delay::~Delay()
{
free(buffer);
}


double Delay::inp(double in, double delta_delay)
{
int i, int_delay;
double buf_val, future_buf_val, frac_delay;

delay_val = nom_val + delta_delay; 
int_delay = (int) floor(delay_val);
frac_delay = 2.0*(delay_val - floor(delay_val))-1.0; 

if (int_delay >= (buffer_size-1) || int_delay < 0)
  {
    printf("error in 'Delay.inp':  delay value exceeds range\n");
    printf("   in this case, delay_val = %5.3e\n",delay_val);
    printf("   integer part must be confined to the range: 0 to %d\n",
            buffer_size-2);
    exit(1);
  }
for (i = buffer_size-1; i > 0; i--)
   buffer[i] = buffer[i-1];    
buffer[0] = in;

buf_val = buffer[int_delay];
if (int_delay > 0)
   future_buf_val = buffer[int_delay-1];
else
   future_buf_val = buf_val;

if (transition_flag == 1)
  {
   transition_flag = 0;
   out = (double) state;
  } 

if (out == -1.0 && state == -1) // looking for rising edge
    {
    if (buf_val != -1.0 && future_buf_val != -1.0)
      {
       state = 1;
       if (buf_val >= frac_delay)
	 {
          out = -1.0 + buf_val - frac_delay;
          buffer[int_delay] = 1.0; // prevent a double trigger
          transition_flag = 1;
	 }       
      }
    }
else if (out == 1.0 && state == 1) // looking for falling edge
    {
    if (buf_val != 1.0 && future_buf_val != 1.0)
      {
       state = -1;
       if (-buf_val >= frac_delay)
	 {
          out = 1.0 + buf_val + frac_delay;
          buffer[int_delay] = -1.0; // prevent a double trigger
          transition_flag = 1;
	 }       
      }
    }

int_delay = (int) floor(prev_delay_val);
frac_delay = 2.0*(prev_delay_val - floor(prev_delay_val))-1.0; 
buf_val = buffer[int_delay+1];
future_buf_val = buffer[int_delay];

if (out == -1.0) // looking for rising edge
    {
    if (buf_val != -1.0 && future_buf_val != -1.0)
      {
       if (buf_val < frac_delay)
	 {
          out = 1.0 - (frac_delay - buf_val);
          transition_flag = 1;
	 }       
      }
    }
else if (out == 1.0) // looking for falling edge
    {
    if (buf_val != 1.0 && future_buf_val != 1.0)
      {
       if (-buf_val < frac_delay)
	 {
          out = -1.0 + (frac_delay + buf_val);
          transition_flag = 1;
	 }       
      }
    }

prev_delay_val = delay_val;
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
if ((fp = fopen(filename,"w")) == NULL)
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

if (num_b_coeff != num_poly->num_coeff || (num_a_coeff+1) != den_poly->num_coeff)
   new_filt_flag = 1;
else
   new_filt_flag = 0;

if (new_filt_flag == 1)
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
  }
else
   num_a_coeff += 1;
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
prev_in = 0.0;
first_entry_flag = 1;

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
prev_in = 0.0;
first_entry_flag = 1;

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
first_entry_flag = 1;
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
prev_in = 0.0;
first_entry_flag = 1;
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
   else
      data = data_seq.read() > 0.0 ? 1.0 : -1.0;
   if (data != -1.0 && data != 1.0)
      {
       printf("error in SigGen.set:  data list has values that are not 1.0 or -1.0\n");
       printf("  in this case, the data value is %5.3f\n",data);
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
       if (input[i+1] == '-')
           input[i+1] = '!';
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


double eval_no_paren_expr(const char *input, PARAM *par_list)
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
double convert_exponent(const char *input, const char *full_string)
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

void free_matrix(matrix *x)
{
conform_matrix(0,0,x);
free(x);
}

matrix *init(const char *name)   
{   
matrix *A;   
   
if ((A = (matrix *) malloc(sizeof(matrix))) == NULL)   
   {   
   printf("error in 'init':  malloc call failed\n");   
   printf("out of memory!\n");   
   exit(1);   
  }   
strncpy(A->name,name,29);   
A->rows = 0;   
A->cols = 0;   
return(A);   
}   

void neg_index_error(const char *function,const char *variable,int value,matrix *A)   
{   
printf("error in '%s':  %s specified as %d for matrix '%s',\n",function,variable,value,A->name);   
printf(" - matrix index parameters must be specified greater than or equal to zero!\n");   
exit(1);   
}   

matrix *replace_elements(EL_TYPE **new_elements,int rows,int cols,matrix *A)   
{   
int i;   
   
if (rows < 0)   
   neg_index_error("replace_elements","rows",rows,A);   
else if (cols < 0)   
   neg_index_error("replace_elements","cols",cols,A);   
if (A->rows > 0 || A->cols > 0)   
  {   
  for (i = 0; i < A->rows; i++)   
     free((A->elem)[i]);   
  free(A->elem);   
  }   
A->elem = new_elements;   
A->rows = rows;   
A->cols = cols;   
return(A);   
}   
   
matrix *conform_matrix(int rows,int cols,matrix *A)   
{   
int i;   
   
if (rows < 0)   
   neg_index_error("conform_matrix","rows",rows,A);   
if (cols < 0)   
   neg_index_error("conform_matrix","cols",cols,A);   
if (A->rows != rows || A->cols != cols)
  {   
   if (A->rows > 0 || A->cols > 0)   
     {   
     for (i = 0; i < A->rows; i++)   
        free((A->elem)[i]);   
     free(A->elem);   
     }   
   if (rows > 0 && cols > 0)   
     {   
     if ((A->elem = (EL_TYPE **) calloc(rows,sizeof(EL_TYPE *))) == NULL)   
        {   
         printf("error in 'conform_matrix':  calloc call failed - \n");   
         printf("  not enough memory!!\n");   
         exit(1);   
       }   
     for (i = 0; i < rows; i++)   
        if (((A->elem)[i] = (EL_TYPE *) calloc(cols,sizeof(EL_TYPE))) == NULL)   
           {   
            printf("error in 'conform_matrix':  calloc call failed - \n");   
            printf("  not enough memory!!\n");   
            exit(1);   
          }   
    }   
 }   
A->rows = rows;   
A->cols = cols;   
return(A);   
}   


void list_to_matrix(const List &in, matrix *out)
{
int i;
List temp(in);

temp.reset();
conform_matrix(temp.length,1,out);

for (i = 0; i < temp.length; i++)
   out->elem[i][0] = temp.read();

temp.reset();
}
   
void real_fft_mat(matrix *DATA, matrix *REAL, matrix *IMAG)   
{   
	int i1,i2,i3,i4,n2p3,n;   
        register int i,j;   
	float h1r,h1i,h2r,h2i;   
	double wr,wi,wpr,wpi,wtemp,wtemp2,theta,test_log2,store_log2;   
        EL_TYPE temp_store;   
        matrix *temp;
   
        if (REAL == IMAG)   
	  {   
           printf("error in 'real_fft':  REAL and IMAG matrices must be distinct\n   in this case, matrix '%s' is specified for both matrix REAL and IMAG\n",REAL->name);   
           exit(1);   
	 }   
        if (DATA->cols != 1)   
	  {   
           printf("error in 'real_fft':  input matrix ('%s' in this case) should have one column\n",DATA->name);   
           printf("   in this case, matrix '%s' is %d by %d\n",DATA->name,DATA->rows,DATA->cols);   
           exit(1);   
	 }   
        if (DATA->rows == 0)  
	  {  
           printf("error in 'real_fft':  input matrix '%s' has zero rows of data!\n",DATA->name);  
           exit(1);  
	 }  
  
        test_log2 = log((double) DATA->rows/2)/log(2.0) + 0.0000001;  
        store_log2 = floor(test_log2);   
        test_log2 -= store_log2;   
        if (test_log2 > 0.0000002 )   
	  {   
          temp = init("temp");
          i1 = (int) (2.0*pow(2.0,store_log2+1.0));
          conform_matrix(i1,1,temp);
          for (i = 0; i < DATA->rows; i++)   
              temp->elem[i][0] = DATA->elem[i][0];   
          for ( ; i < temp->rows; i++)   
              temp->elem[i][0] = 0.0;   
          n = temp->rows/2;
          four1(temp,REAL);
          REAL->elem--;  
  
          conform_matrix(temp->rows,1,IMAG);   
          conform_matrix(0,0,temp);
          free(temp);
	}   
        else
	  {
           n = DATA->rows/2;   
	   four1(DATA,REAL);   
           REAL->elem--;  
  
           if (IMAG != DATA)   
              conform_matrix(DATA->rows,1,IMAG);   
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
	        h1r=0.5*(REAL->elem[i1][0] + REAL->elem[i3][0]);   
		h1i=0.5*(REAL->elem[i2][0] - REAL->elem[i4][0]);   
		h2r = 0.5*(REAL->elem[i2][0] + REAL->elem[i4][0]);   
		h2i=0.5*(REAL->elem[i3][0] - REAL->elem[i1][0]);   
		REAL->elem[i1][0] = h1r+(wtemp2 = wr*h2r+wi*h2i);   
		REAL->elem[i2][0] = h1i+(wtemp = wr*h2i-wi*h2r);   
		REAL->elem[i3][0] = h1r-wtemp2;   
		REAL->elem[i4][0] = -h1i+wtemp;   
		wr=(wtemp=wr)*wpr-wi*wpi+wr;   
		wi=wi*wpr+wtemp*wpi+wi;   
	}   
  
        REAL->elem[n+2][0] *= ISIGN;  
	REAL->elem[1][0] = (h1r=REAL->elem[1][0]) + REAL->elem[2][0];   
	REAL->elem[2][0] = h1r-REAL->elem[2][0];   
  
        REAL->elem++;   
   
        IMAG->elem[0][0] = 0.0;   
        IMAG->elem[n][0] = 0.0;   
        for (i = 1; i < n; i++)   
           IMAG->elem[i][0] = REAL->elem[2*i+1][0];    
  
        temp_store = REAL->elem[1][0];   
        for (i = 0; i < n; i++)   
           REAL->elem[i][0] = REAL->elem[2*i][0];   
        REAL->elem[n][0] = temp_store;   
  
        for (i = n+1,j = n-1; i < IMAG->rows; i++,j--)   
	  {  
           REAL->elem[i][0] = REAL->elem[j][0];   
           IMAG->elem[i][0] = -IMAG->elem[j][0];   
	 }  
  
  
}   
   
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr   


void four1(matrix *DATA, matrix *FFT)   
{   
	int n,nn,mmax,m,istep;   
        register int i,j;   
	double wtemp,wr,wpr,wpi,wi,theta,test_log2;   
	float tempr,tempi;   
   
        nn = DATA->rows/2;   
        if (DATA->cols != 1)   
	  {   
           printf("error in 'four1':  input matrix must have 1 column\n  in this case, input matrix '%s' is %d by %d\n",DATA->name,DATA->rows,DATA->cols);   
           exit(1);   
	 }   
        if (DATA->rows == 0)  
	  {  
           printf("error in 'four1':  input matrix '%s' has zero rows of data!\n",DATA->name);  
           exit(1);  
	 }  
        test_log2 = log((double) nn)/log(2.0) + 0.0000001;   
        test_log2 -= floor(test_log2);   
   
        if (test_log2 > 0.0000002)   
	  {   
           printf("error in 'four1':  number of rows in input matrix must be\n");   
           printf("an integer power of 2\n  in this case, matrix '%s' is %d (times 2) by %d\n",DATA->name,DATA->rows/2,DATA->cols);   
           exit(1);   
	 }   
        if (DATA != FFT)   
          {   
           conform_matrix(DATA->rows,1,FFT);   
           for (i = 0; i < DATA->rows; i++)   
              FFT->elem[i][0] = DATA->elem[i][0];   
	 }   
        FFT->elem--;   
	n=nn << 1;   
	j=1;   
	for (i=1;i<n;i+=2) {   
		if (j > i) {   
			SWAP(FFT->elem[j][0],FFT->elem[i][0]);   
			SWAP(FFT->elem[j+1][0],FFT->elem[i+1][0]);   
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
				tempr=wr*FFT->elem[j][0]-wi*FFT->elem[j+1][0];   
				tempi=wr*FFT->elem[j+1][0]+wi*FFT->elem[j][0];   
				FFT->elem[j][0]=FFT->elem[i][0]-tempr;   
				FFT->elem[j+1][0]=FFT->elem[i+1][0]-tempi;   
				FFT->elem[i][0] += tempr;   
				FFT->elem[i+1][0] += tempi;   
			}   
			wr=(wtemp=wr)*wpr-wi*wpi+wr;   
			wi=wi*wpr+wtemp*wpi+wi;   
		}   
		mmax=istep;   
	}   
       FFT->elem++;   
}   
   
void fft_mat(matrix *DATA_REAL, matrix *DATA_IMAG, matrix *FFT_REAL, matrix *FFT_IMAG)   
{   
	int n,mmax,m,istep,i1,nn;   
        register int i,j;   
	double wtemp,wr,wpr,wpi,wi,theta,test_log2,store_log2;   
        EL_TYPE **temp;   
	float tempr,tempi;   
   
        if (DATA_REAL == DATA_IMAG)   
	  {   
           printf("error in 'fft':  DATA_REAL and DATA_IMAG matrices must be specified\ndifferently - in this case, both are specified as matrix '%s'\n",DATA_REAL->name);   
           exit(1);   
	 }   
        if (FFT_REAL == FFT_IMAG)   
	  {   
           printf("error in 'fft':  FFT_REAL and FFT_IMAG matrices must be specified\ndifferently - in this case, both are specified as matrix '%s'\n",FFT_REAL->name);   
           exit(1);   
	 }   
        if (DATA_REAL->cols != 1 || DATA_IMAG->cols != 1)   
	  {   
           printf("error in 'fft':  DATA_REAL and DATA_IMAG matrices must have 1 column\n  in this case, DATA_REAL matrix '%s' is %d by %d\n              DATA_IMAG matrix '%s' is %d by %d\n",DATA_REAL->name,DATA_REAL->rows,DATA_REAL->cols,DATA_IMAG->name,DATA_IMAG->rows,DATA_IMAG->cols);   
           exit(1);   
	 }   
        if (DATA_REAL->rows != DATA_IMAG->rows)   
	  {   
           printf("error in 'fft':  DATA_REAL and DATA_IMAG matrices must have\nthe same number of rows\n   in this case, DATA_REAL matrix '%s' is %d by %d\n              DATA_IMAG matrix '%s' is %d by %d\n",DATA_REAL->name,DATA_REAL->rows,DATA_REAL->cols,DATA_IMAG->name,DATA_IMAG->rows,DATA_IMAG->cols);   
           exit(1);   
	 }   
        if (DATA_REAL->rows == 0)  
	  {  
           printf("error in 'fft':  input matrices '%s' and '%s' have zero rows of data!\n",DATA_REAL->name,DATA_IMAG->name);  
           exit(1);  
	 }  
  
        test_log2 = log((double) DATA_REAL->rows)/log(2.0) + 0.0000001;   
        store_log2 = floor(test_log2);   
        test_log2 -= store_log2;   
   
        if (test_log2 > 0.0000002 || FFT_REAL == DATA_REAL || FFT_REAL == DATA_IMAG)   
	  {   
          if (test_log2 > 0.0000002)   
	    {  
             if ((temp = (EL_TYPE **) calloc(i1 = (int) (2.0*pow(2.0,store_log2+1.0)),sizeof(EL_TYPE *))) == NULL)   
	       {   
                printf("error in 'fft':  calloc call failed!\n");   
                printf(" (not enough memory)\n");   
                exit(1);   
              }   
	   }  
          else  
            {   
             if ((temp = (EL_TYPE **) calloc(i1 = 2*DATA_REAL->rows,sizeof(EL_TYPE *))) == NULL)   
	       {   
                printf("error in 'fft':  calloc call failed!\n");   
                printf(" (not enough memory)\n");   
                exit(1);   
              }  
	   }  
  
          for (i = 0; i < i1; i++)   
             if ((temp[i] = (EL_TYPE *) calloc(1,sizeof(EL_TYPE))) == NULL)   
	       {   
                printf("error in 'fft':  calloc call failed!\n");   
                printf(" (not enough memory)\n");   
                exit(1);   
              }        
          for (i = 0,j = 0; i < DATA_REAL->rows && j < i1-1; i++,j += 2)   
	    {   
              temp[j][0] = DATA_REAL->elem[i][0];   
              temp[j+1][0] = DATA_IMAG->elem[i][0];   
	    }   
  
          for ( ; j < i1; j++)   
              temp[j][0] = 0.0;   
          replace_elements(temp,i1,1,FFT_REAL);              
	}  
        else   
	  {   
           conform_matrix(2*DATA_REAL->rows,1,FFT_REAL);   
           for (i = 0, j = 0; i < DATA_REAL->rows && j < FFT_REAL->rows-1; i++,j += 2)   
	     {   
              FFT_REAL->elem[j][0] = DATA_REAL->elem[i][0];   
              FFT_REAL->elem[j+1][0] = DATA_IMAG->elem[i][0];   
	    }   
	 }   
        FFT_REAL->elem--;   
  
        nn = FFT_REAL->rows/2;   
   
	n=nn << 1;   
	j=1;   
	for (i=1;i<n;i+=2) {   
		if (j > i) {   
			SWAP(FFT_REAL->elem[j][0],FFT_REAL->elem[i][0]);   
			SWAP(FFT_REAL->elem[j+1][0],FFT_REAL->elem[i+1][0]);   
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
				tempr=wr*FFT_REAL->elem[j][0]-wi*FFT_REAL->elem[j+1][0];   
				tempi=wr*FFT_REAL->elem[j+1][0]+wi*FFT_REAL->elem[j][0];   
				FFT_REAL->elem[j][0]=FFT_REAL->elem[i][0]-tempr;   
				FFT_REAL->elem[j+1][0]=FFT_REAL->elem[i+1][0]-tempi;   
				FFT_REAL->elem[i][0] += tempr;   
				FFT_REAL->elem[i+1][0] += tempi;   
			}   
			wr=(wtemp=wr)*wpr-wi*wpi+wr;   
			wi=wi*wpr+wtemp*wpi+wi;   
		}   
		mmax=istep;   
	}   
       FFT_REAL->elem++;   
  
       conform_matrix(FFT_REAL->rows/2,1,FFT_IMAG);   
       if ((temp = (EL_TYPE **) calloc(FFT_IMAG->rows,sizeof(EL_TYPE *))) == NULL)   
	    {   
             printf("error in 'fft':  calloc call failed!\n");   
             printf(" (not enough memory)\n");   
             exit(1);   
            }   
       for (i = 0; i < FFT_IMAG->rows; i++)   
             if ((temp[i] = (EL_TYPE *) calloc(1,sizeof(EL_TYPE))) == NULL)   
	       {   
                printf("error in 'fft':  calloc call failed!\n");   
                printf(" (not enough memory)\n");   
                exit(1);   
              }        
  
       for (i = 0, j = 0; i < FFT_IMAG->rows && j < FFT_REAL->rows-1; i++, j += 2)   
	 {   
          temp[i][0] = FFT_REAL->elem[j][0];   
          FFT_IMAG->elem[i][0] = FFT_REAL->elem[j+1][0];   
	}  
  
       replace_elements(temp,FFT_IMAG->rows,1,FFT_REAL);  
   
}   
   
void ifft_mat(matrix *DATA_REAL, matrix *DATA_IMAG, matrix *FFT_REAL, matrix *FFT_IMAG)   
{   
	int n,mmax,m,istep,i1,nn;   
        register int i,j;   
	double wtemp,wr,wpr,wpi,wi,theta,test_log2,store_log2;   
        EL_TYPE **temp;   
	float tempr,tempi;   
   
/* note:  this program's labels are misleading - output is the real and imag */  
/* components of the inverse fft of the input data */  
  
        if (DATA_REAL == DATA_IMAG)   
	  {   
           printf("error in 'ifft':  DATA_REAL and DATA_IMAG matrices must be specified\ndifferently - in this case, both are specified as matrix '%s'\n",DATA_REAL->name);   
           exit(1);   
	 }   
        if (FFT_REAL == FFT_IMAG)   
	  {   
           printf("error in 'ifft':  FFT_REAL and FFT_IMAG matrices must be specified\ndifferently - in this case, both are specified as matrix '%s'\n",FFT_REAL->name);   
           exit(1);   
	 }   
        if (DATA_REAL->cols != 1 || DATA_IMAG->cols != 1)   
	  {   
           printf("error in 'ifft':  DATA_REAL and DATA_IMAG matrices must have 1 column\n  in this case, DATA_REAL matrix '%s' is %d by %d\n              DATA_IMAG matrix '%s' is %d by %d\n",DATA_REAL->name,DATA_REAL->rows,DATA_REAL->cols,DATA_IMAG->name,DATA_IMAG->rows,DATA_IMAG->cols);   
           exit(1);   
	 }   
        if (DATA_REAL->rows != DATA_IMAG->rows)   
	  {   
           printf("error in 'ifft':  DATA_REAL and DATA_IMAG matrices must have\nthe same number of rows\n   in this case, DATA_REAL matrix '%s' is %d by %d\n              DATA_IMAG matrix '%s' is %d by %d\n",DATA_REAL->name,DATA_REAL->rows,DATA_REAL->cols,DATA_IMAG->name,DATA_IMAG->rows,DATA_IMAG->cols);   
           exit(1);   
	 }   
         if (DATA_REAL->rows == 0)  
	  {  
           printf("error in 'ifft':  input matrices '%s' and '%s' have zero rows of data!\n",DATA_REAL->name,DATA_IMAG->name);  
           exit(1);  
	 }  
  
        test_log2 = log((double) DATA_REAL->rows)/log(2.0) + 0.0000001;   
        store_log2 = floor(test_log2);   
        test_log2 -= store_log2;   
   
        if (test_log2 > 0.0000002 || FFT_REAL == DATA_REAL || FFT_REAL == DATA_IMAG)   
	  {   
          if (test_log2 > 0.0000002)   
	    {  
             if ((temp = (EL_TYPE **) calloc(i1 = (int) (2.0*pow(2.0,store_log2+1.0)),sizeof(EL_TYPE *))) == NULL)   
	       {   
                printf("error in 'ifft':  calloc call failed!\n");   
                printf(" (not enough memory)\n");   
                exit(1);   
              }   
	   }  
          else   
	    {  
             if ((temp = (EL_TYPE **) calloc(i1 = 2*DATA_REAL->rows,sizeof(EL_TYPE *))) == NULL)   
	       {   
                printf("error in 'ifft':  calloc call failed!\n");   
                printf(" (not enough memory)\n");   
                exit(1);   
              }   
	   }  
          for (i = 0; i < i1; i++)   
             if ((temp[i] = (EL_TYPE *) calloc(1,sizeof(EL_TYPE))) == NULL)   
	       {   
                printf("error in 'ifft':  calloc call failed!\n");   
                printf(" (not enough memory)\n");   
                exit(1);   
              }        
          for (i = 0,j = 0; i < DATA_REAL->rows && j < i1-1; i++,j += 2)   
	    {   
              temp[j][0] = DATA_REAL->elem[i][0];   
              temp[j+1][0] = DATA_IMAG->elem[i][0];   
	    }   
                 
          for ( ; j < i1; j++)   
              temp[j][0] = 0.0;   
          replace_elements(temp,i1,1,FFT_REAL);              
	}   
        else   
	  {   
           conform_matrix(2*DATA_REAL->rows,1,FFT_REAL);   
           for (i = 0, j = 0; i < DATA_REAL->rows && j < FFT_REAL->rows-1; i++,j += 2)   
	     {   
              FFT_REAL->elem[j][0] = DATA_REAL->elem[i][0];   
              FFT_REAL->elem[j+1][0] = DATA_IMAG->elem[i][0];   
	    }   
	 }   
        FFT_REAL->elem--;   
   
        nn = FFT_REAL->rows/2;   
   
	n=nn << 1;   
	j=1;   
	for (i=1;i<n;i+=2) {   
		if (j > i) {   
			SWAP(FFT_REAL->elem[j][0],FFT_REAL->elem[i][0]);   
			SWAP(FFT_REAL->elem[j+1][0],FFT_REAL->elem[i+1][0]);   
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
				tempr=wr*FFT_REAL->elem[j][0]-wi*FFT_REAL->elem[j+1][0];   
				tempi=wr*FFT_REAL->elem[j+1][0]+wi*FFT_REAL->elem[j][0];   
				FFT_REAL->elem[j][0]=FFT_REAL->elem[i][0]-tempr;   
				FFT_REAL->elem[j+1][0]=FFT_REAL->elem[i+1][0]-tempi;   
				FFT_REAL->elem[i][0] += tempr;   
				FFT_REAL->elem[i+1][0] += tempi;   
			}   
			wr=(wtemp=wr)*wpr-wi*wpi+wr;   
			wi=wi*wpr+wtemp*wpi+wi;   
		}   
		mmax=istep;   
	}   
       FFT_REAL->elem++;   
   
       conform_matrix(FFT_REAL->rows/2,1,FFT_IMAG);   
       if ((temp = (EL_TYPE **) calloc(FFT_IMAG->rows,sizeof(EL_TYPE *))) == NULL)   
	    {   
             printf("error in 'ifft':  calloc call failed!\n");   
             printf(" (not enough memory)\n");   
             exit(1);   
            }   
       for (i = 0; i < FFT_IMAG->rows; i++)   
             if ((temp[i] = (EL_TYPE *) calloc(1,sizeof(EL_TYPE))) == NULL)   
	       {   
                printf("error in 'ifft':  calloc call failed!\n");   
                printf(" (not enough memory)\n");   
                exit(1);   
              }        
       for (i = 0, j = 0; i < FFT_IMAG->rows && j < FFT_REAL->rows-1; i++, j += 2)   
	 {   
          temp[i][0] = FFT_REAL->elem[j][0]/(double) FFT_IMAG->rows;   
          FFT_IMAG->elem[i][0] = FFT_REAL->elem[j+1][0]/(double) FFT_IMAG->rows;   
	}   
       replace_elements(temp,FFT_IMAG->rows,1,FFT_REAL);    
   
}   
