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
    **    IN     my_classes_and_functions.h               **
    **  SUGGESTION FOR NEW NAME:  CLASSNAME_YOURINITIALS  **
*********************************************************/

/*************** Configuration for Different Platforms ****************/
#define WORDS_BIGENDIAN 1 // one should never need to change this
                          // with the newest version of loadsig
/**********************************************************************/

#define MAX_NAME_LENGTH 1000
#define MAX_HEADER_BLOCKS 100
#define TIME 1
#define FREQUENCY 4
#define VOLTAGE 2
#define CURRENT 3
#define UNKNOWN 0
#define NaN 1.2345e307
#define MIN_DIVIDE_VAL 1e-30
#define MIN_DIVIDE_VAL_RESULT 1e30

#define MAX_NODE_NAME_LENGTH 2000
#define CHAR_BUFFER_LENGTH 1000
#define DATA_BUFFER_LENGTH 2048
#define MAX_CHAR_LENGTH 1000

/* the following defines the maximum number of characters read in per line */  
/* for a file (used by the 'load_mat_struct' function) */  
#define MAX_FILE_CHAR 3000  

//#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <math.h>


#ifndef M_PI /* usually defined in math.h */
  #define M_PI            3.14159265358979323846  
#endif
#ifndef PI
  #define PI M_PI
#endif
#ifndef pi
  #define pi M_PI
#endif
#define MAX_FILE_WORD 255

void  fwrite_int_values(unsigned int *ibuf, size_t size_of_block, FILE *fp);
void  fwrite_char_values(const char *cbuf, size_t size_of_block, FILE *fp);

/* the following dictates whether matrices are single or double precision  */  
/* note:  EL_TYPE_NUMBER = 1 if float specified, 2 if double specified for */  
/* EL_TYPE                                                                 */  
typedef double EL_TYPE;  
#define EL_TYPE_NUMBER 2  
#define ISIGN -1.0

/* 'inv(A,B)':  double precision limit on singular values to */  
/*                    determine if matrix is singular            */  
/* note:  this limit is scaled by the maximum singular value */  
#define INV_DP_SV_L 1E-15  
/*                    determining if matrix is ill-conditioned          */  
#define INV_DP_RC_L -3.0 


///////////////// Support for Vector Operations ///////////////

typedef struct  
  {  
   int length, freeze_length_flag;
   double *elem;
   char *name, *module_name;
} vector_struct;  

class Vector
{
public:
   Vector();
   Vector(const char *module_name, const char *name);
   Vector(const char *module_name, const char *name, const char *null_string);
   Vector(const Vector &other);
   ~Vector();
   Vector& operator = (const Vector &other) const;
   Vector& operator = (Vector &other) const;
   void replace_vec(vector_struct *in);
   void copy(vector_struct *in) const;
   void copy(const Vector &other) const;
   void copy(Vector &other) const;
   void load(const char *filename) const;
   void save(const char *filename) const;
   int get_length() const;
   char *get_name() const;
   char *get_module_name() const;
   void set_length(int length) const;
   double get_elem(int index) const;
   void set_elem(int index, double val) const; 
   vector_struct *vec;
   char *name;
   char *module_name;
   int initialized_flag;
private:
   void init();
};

typedef struct  
  {  
   int length, freeze_length_flag;
   int *elem;
   char *name, *module_name;
} int_vector_struct;  

class IntVector
{
public:
   IntVector();
   IntVector(const char *module_name, const char *name);
   IntVector(const char *module_name, const char *name, const char *null_string);
   IntVector(const IntVector &other);
   ~IntVector();
   IntVector& operator = (const IntVector &other) const;
   IntVector& operator = (IntVector &other) const;
   void replace_vec(int_vector_struct *in);
   int get_length() const;
   void copy(int_vector_struct *in) const;
   void copy(const IntVector &other) const;
   void copy(IntVector &other) const;
   void load(const char *filename) const;
   void save(const char *filename) const;
   char *get_name() const;
   char *get_module_name() const;
   void set_length(int length) const;
   int get_elem(int index) const;
   void set_elem(int index, int val) const; 
   int_vector_struct *vec;
   char *name;
   char *module_name;
   int initialized_flag;
private:
   void init();
};



///////////////// Support for Matrix Operations ///////////////

typedef struct  
  {  
   int rows, cols, freeze_length_flag;
   double **elem;
   char *name, *module_name;
} matrix_struct;  

class Matrix
{
public:
   Matrix();
   Matrix(const char *module_name, const char *name);
   Matrix(const Matrix &other);
   Matrix(const char *module_name_in, const char *name_in, const char *null_string);
   ~Matrix();
   void replace_mat(matrix_struct *in);
   Matrix& operator = (const Matrix &other) const;
   Matrix& operator = (Matrix &other) const;
   void copy(matrix_struct *in) const;
   void copy(const Matrix &other) const;
   void copy(Matrix &other) const;
   void load(const char *filename) const;
   void save(const char *filename) const;
   int get_rows() const;
   int get_cols() const;
   char *get_name() const;
   char *get_module_name() const;
   void set_size(int rows,int cols) const;
   double get_elem(int index_row, int index_col) const;
   void set_elem(int index_row, int index_col, double val) const; 
   matrix_struct *mat;
   char *name;
   char *module_name;
   int initialized_flag;
private:
   void init();
};

typedef struct  
  {  
   int rows, cols, freeze_length_flag;
   int **elem;
   char *name, *module_name;
} int_matrix_struct;  

class IntMatrix
{
public:
   IntMatrix();
   IntMatrix(const char *module_name, const char *name);
   IntMatrix(const IntMatrix &other);
   IntMatrix(const char *module_name_in, const char *name_in, const char *null_string);
   void replace_mat(int_matrix_struct *in);
   ~IntMatrix();
   IntMatrix& operator = (const IntMatrix &other) const;
   IntMatrix& operator = (IntMatrix &other) const;
   void copy(int_matrix_struct *in) const;
   void copy(const IntMatrix &other) const;
   void copy(IntMatrix &other) const;
   void load(const char *filename) const;
   void save(const char *filename) const;
   int get_rows() const;
   int get_cols() const;
   char *get_name() const;
   char *get_module_name() const;
   void set_size(int rows,int cols) const;
   int get_elem(int index_row, int index_col) const;
   void set_elem(int index_row, int index_col, int val) const; 
   int_matrix_struct *mat;
   char *name;
   char *module_name;
   int initialized_flag;
private:
   void init();
};

//////////////// Vector functions ////////////////////

void copy(const Vector &from, const Vector &to);
void copy(const IntVector &from, const IntVector &to);

void print(const Vector &x);
void print(const IntVector &x);

void save(const char *filename, const Vector &in);
void save(const char *filename, const IntVector &in);
void load(const char *filename, const Vector &in);
void load(const char *filename, const IntVector &in);


void ifft(const Vector &DATA_REAL, const Vector &DATA_IMAG, const Vector &FFT_REAL, const Vector &FFT_IMAG);  
void fft(const Vector &DATA_REAL, const Vector &DATA_IMAG, const Vector &FFT_REAL, const Vector &FFT_IMAG);
void real_fft(const Vector &DATA, const Vector &REAL, const Vector &IMAG);

void real_to_int(const Vector &in, const IntVector &out);
void int_to_real(const IntVector &in, const Vector &out);
void real_fft(const IntVector &data, const Vector &real, const Vector &imag);
void fft(const IntVector &real, const IntVector &imag, const Vector &fft_real, const Vector &fft_imag);
void ifft(const IntVector &real, const IntVector &imag, const Vector &out_real, const Vector &out_imag);

void add(const Vector &a, const Vector &b, const Vector &y);
void add(const IntVector &a, const IntVector &b, const IntVector &y);
void add(double a, const Vector &b, const Vector &y);
void add(int a, const IntVector &b, const IntVector &y);
void add(const Vector &a, double b, const Vector &y);
void add(const IntVector &a, int b, const IntVector &y);

void sub(const Vector &a, const Vector &b, const Vector &y);
void sub(const IntVector &a, const IntVector &b, const IntVector &y);
void sub(double a, const Vector &b, const Vector &y);
void sub(int a, const IntVector &b, const IntVector &y);
void sub(const Vector &a, double b, const Vector &y);
void sub(const IntVector &a, int b, const IntVector &y);

void mul_elem(const Vector &a, const Vector &b, const Vector &y);
void mul_elem(const IntVector &a, const IntVector &b, const IntVector &y);
void mul_elem(double a, const Vector &b, const Vector &y);
void mul_elem(int a, const IntVector &b, const IntVector &y);
void mul_elem(const Vector &a, double b, const Vector &y);
void mul_elem(const IntVector &a, int b, const IntVector &y);

void div_elem(const Vector &a, const Vector &b, const Vector &y);
void div_elem(const IntVector &a, const IntVector &b, const IntVector &y);
void div_elem(double a, const Vector &b, const Vector &y);
void div_elem(int a, const IntVector &b, const IntVector &y);
void div_elem(const Vector &a, double b, const Vector &y);
void div_elem(const IntVector &a, int b, const IntVector &y);

void sinc_vector(double step_val, int length, const Vector &A);
void gauss_ran_vector(double std_dev, int length, const Vector &A);
double get_area(const Vector &A);
double get_mean(const Vector &A);
double get_var(const Vector &A);

double inner_product(const Vector &A, const Vector &B);
int inner_product(const IntVector &A, const IntVector &B);

///////////////// Support for Vector Structures ///////////////

double inner_product(vector_struct *A, vector_struct *B);
int inner_product(int_vector_struct *A, int_vector_struct *B);

vector_struct *extract_vector_struct(const Vector &in);
int_vector_struct *extract_vector_struct(const IntVector &in);

///////// The following functions are used for lower level routines
///////// and should not be used by most users
vector_struct *init_vector_struct(const char *module_name, const char *name);
int_vector_struct *init_int_vector_struct(const char *module_name, const char *name);

void free_vector_struct(vector_struct *x);
void free_vector_struct(int_vector_struct *x);

void set_length(int length, vector_struct *A);
void set_length(int length, int_vector_struct *A);

double read_elem(int index, vector_struct *A);
int read_elem(int index, int_vector_struct *A);

void write_elem(double val, int index, vector_struct *A);
void write_elem(int val, int index, int_vector_struct *A);

//////// the following are leveraged in the Vector class functions

void copy(vector_struct *x, vector_struct *y);
void copy(int_vector_struct *x, int_vector_struct *y);

void print(vector_struct *x);
void print(int_vector_struct *x);

void save(const char *filename, vector_struct *in);
void save(const char *filename, int_vector_struct *in);
void load(const char *filename, vector_struct *in);
void load(const char *filename, int_vector_struct *in);

void save_struct(const char *filename, vector_struct *in);
void save_struct(const char *filename, int_vector_struct *in);
void load_struct(const char *filename, vector_struct *in);
void load_struct(const char *filename, int_vector_struct *in);

void real_fft_vec(vector_struct *data, vector_struct *real, 
                  vector_struct *imag);
void fft_vec(vector_struct *real, vector_struct *imag, 
         vector_struct *fft_real, vector_struct *fft_imag);
void ifft_vec(vector_struct *real, vector_struct *imag, 
          vector_struct *out_real, vector_struct *out_imag);
void ifft(vector_struct *DATA_REAL, vector_struct *DATA_IMAG, vector_struct *FFT_REAL, vector_struct *FFT_IMAG);  
void fft(vector_struct *DATA_REAL, vector_struct *DATA_IMAG, vector_struct *FFT_REAL, vector_struct *FFT_IMAG);
void four1(vector_struct *DATA, vector_struct *FFT);
void real_fft(vector_struct *DATA, vector_struct *REAL, vector_struct *IMAG);

void real_to_int(vector_struct *in, int_vector_struct *out);
void int_to_real(int_vector_struct *in, vector_struct *out);
void real_fft(int_vector_struct *data, vector_struct *real, vector_struct *imag);
void fft(int_vector_struct *real, int_vector_struct *imag, vector_struct *fft_real, vector_struct *fft_imag);
void ifft(int_vector_struct *real, int_vector_struct *imag, vector_struct *out_real, vector_struct *out_imag);

void add(vector_struct *a, vector_struct *b, vector_struct *y);
void add(int_vector_struct *a, int_vector_struct *b, int_vector_struct *y);
void add(double a, vector_struct *b, vector_struct *y);
void add(int a, int_vector_struct *b, int_vector_struct *y);
void add(vector_struct *a, double b, vector_struct *y);
void add(int_vector_struct *a, int b, int_vector_struct *y);

void sub(vector_struct *a, vector_struct *b, vector_struct *y);
void sub(int_vector_struct *a, int_vector_struct *b, int_vector_struct *y);
void sub(double a, vector_struct *b, vector_struct *y);
void sub(int a, int_vector_struct *b, int_vector_struct *y);
void sub(vector_struct *a, double b, vector_struct *y);
void sub(int_vector_struct *a, int b, int_vector_struct *y);

void mul_elem(vector_struct *a, vector_struct *b, vector_struct *y);
void mul_elem(int_vector_struct *a, int_vector_struct *b, int_vector_struct *y);
void mul_elem(double a, vector_struct *b, vector_struct *y);
void mul_elem(int a, int_vector_struct *b, int_vector_struct *y);
void mul_elem(vector_struct *a, double b, vector_struct *y);
void mul_elem(int_vector_struct *a, int b, int_vector_struct *y);


void div_elem(vector_struct *a, vector_struct *b, vector_struct *y);
void div_elem(int_vector_struct *a, int_vector_struct *b, int_vector_struct *y);
void div_elem(double a, vector_struct *b, vector_struct *y);
void div_elem(int a, int_vector_struct *b, int_vector_struct *y);
void div_elem(vector_struct *a, double b, vector_struct *y);
void div_elem(int_vector_struct *a, int b, int_vector_struct *y);

void sinc_vector(double step_val, int length, vector_struct *A);
void gauss_ran_vector(double std_dev, int length, vector_struct *A);
double get_mean(vector_struct *A);
double get_var(vector_struct *A);
double get_area(vector_struct *A);


struct DOUBLE_LIST
{
double value;
struct DOUBLE_LIST *next;
};
typedef struct DOUBLE_LIST DOUBLE_LIST;

class List
{
public:
   List();
   List(const List &other);
   void set(const List &other);
   ~List();
   double read();
   void reset();
   void flush();
   double out;
   int inp(double in);
   int inp(const Vector &in);
   int inp(Vector &in);
   int inp(vector_struct *in);
   int length,notdone;
   double mean();
   double var();
   void add(const List &other);
   void add(double in);
   void mul(const List &other);
   void mul(double in);
   void conv(const List &other);
   void cat(const List &other);
   int load(const char *filename);
   void save(const char *filename);
   void copy(const List &other);
   void copy(const Vector &other);
   void copy(const IntVector &other);
   void print(const char *name);
   // new functions as of 6/5/04 - written by Michael Perrott
   double read_without_incrementing();
   void write(double in);
   void write_without_incrementing(double in);
   void remove_first_entry();
private:
   int temp_flag;
   DOUBLE_LIST *first_entry, *cur_entry, *cur_write_entry;
   DOUBLE_LIST *create_entry();
   void delete_entries(DOUBLE_LIST *first_entry);
};

void copy(const List &from, const Vector &to);
void copy(const List &from, const IntVector &to);
void copy(const IntVector &from, const List &to);
void copy(const IntVector &from, const List &to);

void list_to_vector(const List &in, const Vector &out);
void list_to_vector(const List &in, Vector &out);
void list_to_vector(const List &in, vector_struct *out);

class Clist
{
public:
   Clist();
   ~Clist();
   Clist(const Clist &other);
   void cat(const Clist &other);
   List real,imag;
   void ifft(const List &fft_real, const List &fft_imag);
   void ifft(const Clist &fft_in);
   void fft(const List &data_real, const List &data_imag);
   void fft(const Clist &data);
   void fft(const List &data);
   void print(const char *name);
   void read();
   void reset();
   void flush();
   double outr,outi;
   int inp(double rin, double iin);
   int inp(const List &rin, const List &iin);
   int length,notdone;
   void add(const Clist &other);
   void add(double rin, double iin);
   void mul(const Clist &other);
   void mul(double rin, double iin);
   void conv(const Clist &other);
   void conv(const List &other);
   void copy(const Clist &other);
private:
};

#define LENGTH_PAR_LIST 99  /* don't make this more than 100 */
#define LENGTH_PAR 200

typedef struct  
  {  
   int main_par_length,nest_length;

   char main_par_list[LENGTH_PAR_LIST][LENGTH_PAR];
   double main_par_val[LENGTH_PAR_LIST];

   double nest_val[LENGTH_PAR_LIST];
   char overall_expr[MAX_CHAR_LENGTH];
} PARAM;

PARAM *init_param();
double eval_paren_expr(char *input, PARAM *par_list);
double eval_no_paren_expr(char *input, PARAM *par_list);
double convert_string(char *input, int *error_flag);
double convert_exponent(char *input, char *full_string);


class Poly
{
 public:
   Poly(const char *expr_in, const char *vars=NULL, double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
   Poly(const List &other);
   void set(const char *expr_in, const char *vars=NULL, double var0=NaN, double var1=NaN, 
	    double var2=NaN, double var3=NaN, double var4=NaN, 
            double var5=NaN, double var6=NaN, double var7=NaN, 
            double var8=NaN, double var9=NaN);
   void set(const List &other);
   ~Poly();
   int num_coeff;
   double *coeff;
   double *exp;
   double max,min,sample_period;
   char ivar;
 private:
};


struct NODE
  {
  char name[MAX_NODE_NAME_LENGTH];
  double value;
  struct NODE *next;
};
typedef struct NODE NODE;

void create_header(NODE *first_node, FILE *fp);
void create_data(NODE *first_node, FILE *fp, int flush_buffer, float *fbuf,
                 size_t *buffer_length);
NODE *add_node(NODE *first_node, const char *name, double value);
NODE *init_node(const char *name, double value);


class Delay
{
public:
   Delay(double nom_value);
   ~Delay();
   double inp(double in);
   double inp(double in, double delta_delay);
   double out;
private:
   List edges;
   double nom_delay, prev_in;
   int input_state, output_state, warning_flag;
};


class SampleAndHold
{
public:
   SampleAndHold();
   ~SampleAndHold();
   double inp(double in, double clk);
   double out;
private:
   double prev_in, prev_clk;
};

class EdgeDetect
{
public:
   EdgeDetect();
   ~EdgeDetect();
   int inp(double in);
   int inp_interp(double in);
   int inp(int in);
   int out;
private:
   double prev_in;
   int prev_slope;
   int out_of_range_flag;
};

class EdgeInterpolate
{
public:
   EdgeInterpolate();
   ~EdgeInterpolate();
   double inp(double in);
   void update(double clk, double in);
   double out;
private:
   double prev_in, clk_in;
   int out_of_range_flag;
   int initial_timer;
};

class EdgeMeasure
{
public:
   EdgeMeasure();
   ~EdgeMeasure();
   double inp(double in);
   double out;
private:
   double prev_in;
   double time_val;
   int out_of_range_flag;
};

class Quantizer
{
public:
   Quantizer(int levels, double step_size, double in_center, double out_min,
             double out_max);
   ~Quantizer();
   double inp(double in);
   double inp(double in, double clk);
   double out;
private:
   int ilevels,odd_levels_flag;
   double istep_size, iin_center, iout_max, iout_min, prev_clk;
   double out_center, out_scale;
   int clk_out_of_range_flag;
};

class Probe
{
public:
  Probe(const char *filename,double sample_per=1.0,int subsample=1);
  void inp(const Vector &vec_node_value, const char *node_name);
  void inp(const IntVector &vec_node_value, const char *node_name);
  void inp(const Vector &vec_node_value, const char *node_name, int sim_step);
  void inp(const IntVector &vec_node_value, const char *node_name, int sim_step);
  void inp(double node_value, const char *node_name);
  void inp(int int_node_value, const char *node_name);
  void inp(double node_value, const char *node_name, int sim_step);
  void inp(int int_node_value, const char *node_name, int sim_step);
  ~Probe();
private:
  float fbuf[DATA_BUFFER_LENGTH];
  size_t buf_length;
  NODE *first_node,*cur_node,*nodes_with_time;
  FILE *fp;
  int header_flag,num_signals,cur_signal;
  int subsample_count, cur_sample;
  double sample_period;
};



class Divider
{
public:
  Divider();
  double inp(double in, int divide_value);
  double inp(double in, double divide_value);
  double inp_base(double in, int divide_value);
  ~Divider();
  double out;
private:
  int cycle_count, state;
  double prev_in;
  int high_count, low_count;
  int divide_trans_count, prev_divide_val;
  int trans_warning_flag;
  int divide_val_flag;
};

class Amp
{
public:
   Amp(const char *poly_expr, const char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
   void set(const char *poly_expr, const char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
   double inp(double in);
   ~Amp();
   double out;
   Poly poly_gain;
private:
};

class Vco
{
public:
   Vco(const char *poly_expr, const char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
   void set(const char *poly_expr, const char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
  double inp(double in);
  double inp(double in, int divide_val);
  double inp(double in, double divide_val);
  double inp_div(double in, int divide_val);
  ~Vco();
  double out,phase;
private:
  double prev_phase,divide_scale,sample_period;
  int clk_state;
  int prev_divide_val, divide_trans_count;
  int trans_warning_flag,divide_val_warning_flag;
  int out_of_range_flag;
  Amp Kv_poly;
};

class Filter
{
public:
  Filter();
  Filter(const char *num, const char *den, const char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
  Filter(const List &num_list, const char *den, const char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
  Filter(const char *num, const List &den_list, const char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
  Filter(const List &num_list, const List &den_list);

  void set_base(Poly *num_poly, Poly *den_poly, const char *num, const char *den);

  void set(const char *num, const char *den, const char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
  void set(const List &num_list, const char *den, const char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
  void set(const char *num, const List &den_list, const char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
  void set(const List &num_list, const List &den_list);
  void set_dt_filt(Poly *num_poly, Poly *den_poly, const char *num, const char *den);
  void set_ct_filt(Poly *num_poly, Poly *den_poly, const char *num, const char *den);
  double inp(double in);
  void reset(double value);
  ~Filter();
  double out;
  char ivar;
  double *a_coeff, *b_coeff;
  int num_a_coeff, num_b_coeff;
  int *a_exp, *b_exp;
private:
  double max_out, min_out;
  double *x_samples, *y_samples;
  int num_x_samples, num_y_samples;
};



class Latch
{
public:
  Latch();
  void inp_base(double in, double clk);
  void inp_reset(double reset);
  double inp(double in, double clk);
  double inp(double in, double clk, double set, double reset);
  void init(double in);
  ~Latch();
  double out;
private:
  double prev_clk, prev_reset, outp;
  int clk_warning_flag;
  int out_of_range_flag;
  int reset_transition_flag;
};


class Reg
{
public:
  Reg();
  double inp(double in, double clk);
  double inp(double in, double clk, double set, double reset);
  void init(double in);
  ~Reg();
  Latch lat1,lat2;
  double out;
private:
  int out_of_range_flag;
};

class Xor
{
public:
   Xor();
   ~Xor();
   double inp(double in0, double in1);
   double out;
private:
};

class And2
{
public:
    And2();
    ~And2();
    double inp(double in0, double in1);
    double out;
private:
    int state;
};

class And
{
public:
  And();
  ~And();
  double inp(double in0, double in1);
  double inp(double in0, double in1, double in2);
  double inp(double in0, double in1, double in2, double in3);
  double inp(double in0, double in1, double in2, double in3, double in4);
  double out;
private:
  And2 and1,and2,and3,and4;
};

class Or
{
public:
  Or();
  ~Or();
  double inp(double in0, double in1);
  double inp(double in0, double in1, double in2);
  double inp(double in0, double in1, double in2, double in3);
  double inp(double in0, double in1, double in2, double in3, double in4);
  double out;
private:
  And2 and1,and2,and3,and4;
};

class Rand
{
public:
   Rand(const char *type);
   Rand(const char *type, double p);
   ~Rand();
   void set(const char *type, double p);
   void reset();
   void set_seed(int in);
   double inp();
   double out;
private:
   double bernoulli(double p);
   float gasdev(long *idum_in);
   float ran1(long *idum_in);
   static long idum;
   int type_flag; // type_flag=0: Gaussian, 1: uniform, 2:bernoulli
   double p_bern;
};

class SdMbitMod
{
public:
   SdMbitMod(const char *num, const char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
   ~SdMbitMod();
   void set(const char *num, const char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
   double inp(double in);
   double out;
private:
   Filter H;
};


class SigGen
{
public:
   SigGen(const char *type, double freq, double Ts);
   SigGen(const char *type, double freq, double Ts, const List &other);
   SigGen(const char *type, double freq, double Ts, const List &other, int start_entry);
   ~SigGen();
   void set(const char *type, double freq, double Ts);
   void set(const char *type, double freq, double Ts, const List &other);
   void set(const char *type, double freq, double Ts, const List &other, int start_entry);
   void reset();
   double inp(double in);
   double phase, out, square;
private:
   double sample_period, frequency;
   double prev_phase, prev_square, prev_in, data, prev_data;
   int clk_state, type_flag, first_entry_flag, end_data_flag;
   Reg reg1;
   Rand rand1;
   List data_seq;
   Filter phase_filt;
};

char *bit_print(unsigned long int v);




//////////////// Matrix functions ////////////////////

void real_to_int(const Matrix &in, const IntMatrix &out);
void int_to_real(const IntMatrix &in, const Matrix &out);

void print(const Matrix &in);
void print(const IntMatrix &in);

void save(const char *filename, const Matrix &in);
void save(const char *filename, const IntMatrix &in);

void load(const char *filename, const Matrix &in);
void load(const char *filename, const IntMatrix &in);

void copy(const Matrix &from, const Matrix &to);
void copy(const IntMatrix &from, const IntMatrix &to);

void mul(const Matrix &A, const Matrix &B, const Matrix &C);
void mul(const IntMatrix &A, const IntMatrix &B, const IntMatrix &C);
void mul(const Matrix &A, double B, const Matrix &C);
void mul(const IntMatrix &A, int B, const IntMatrix &C);
void mul(double A, const Matrix &B, const Matrix &C);
void mul(int A, const IntMatrix &B, const IntMatrix &C);

void add(const Matrix &A, const Matrix &B, const Matrix &C);
void add(const IntMatrix &A, const IntMatrix &B, const IntMatrix &C);
void add(const Matrix &A, double B, const Matrix &C);
void add(double A, const Matrix &B, const Matrix &C);
void add(const IntMatrix &A, int B, const IntMatrix &C);
void add(int A, const IntMatrix &B, const IntMatrix &C);

void sub(const Matrix &A, const Matrix &B, const Matrix &C);
void sub(const IntMatrix &A, const IntMatrix &B, const IntMatrix &C);
void sub(const Matrix &A, double B, const Matrix &C);
void sub(double A, const Matrix &B, const Matrix &C);
void sub(const IntMatrix &A, int B, const IntMatrix &C);
void sub(int A, const IntMatrix &B, const IntMatrix &C);

void trans(const Matrix &A, const Matrix &B);
void trans(const IntMatrix &A, const IntMatrix &B);

void svd(const Matrix &A, 
         const Matrix &U, const Matrix &W, const Matrix &V);
void inv(const Matrix &A, const Matrix &B);
void least_sq(const Matrix &A, const Matrix &B, const Matrix &X);

//////////////// Matrix and Vector functions ///////////////////

void copy(int index, const char *row_or_col, Vector &from, Matrix &to);
void copy(int index, const char *row_or_col, IntVector &from, IntMatrix &to);

void copy(int index, const char *row_or_col, Matrix &from, Vector &to);
void copy(int index, const char *row_or_col, IntMatrix &from, IntVector &to);

void mul(const Matrix &A, const Vector &B, const Vector &C);
void mul(const IntMatrix &A, const IntVector &B, const IntVector &C);
void mul(const Vector &A, const Matrix &B, const Vector &C);
void mul(const IntVector &A, const IntMatrix &B, const IntVector &C);


void least_sq(const Matrix &A, const Vector &B, const Vector &X);

/// supporting structure functions
void mul(matrix_struct *A, vector_struct *B, vector_struct *C);
void mul(vector_struct *A, matrix_struct *B, vector_struct *C);
void mul(int_matrix_struct *A, int_vector_struct *B, int_vector_struct *C);
void mul(int_vector_struct *A, int_matrix_struct *B, int_vector_struct *C);

void copy(int index, const char *row_or_col, matrix_struct *from, vector_struct *to);
void copy(int index, const char *row_or_col, vector_struct *from, matrix_struct *to);
void copy(int index, const char *row_or_col, int_vector_struct *from, 
          int_matrix_struct *to);
void copy(int index, const char *row_or_col, int_matrix_struct *from, 
          int_vector_struct *to);

//////////////// Matrix struct functions ////////////////////

///////// The following functions are used for lower level routines
///////// and should not be used by most users

matrix_struct *init_matrix_struct(const char *module_name, const char *name);
int_matrix_struct *init_int_matrix_struct(const char *module_name, const char *name);

void free_matrix_struct(matrix_struct *x);
void free_matrix_struct(int_matrix_struct *x);

void set_size(int rows, int cols, matrix_struct *mat);
void set_size(int rows, int cols, int_matrix_struct *mat);

double read_elem(int row, int col, matrix_struct *A);
int read_elem(int row, int col, int_matrix_struct *A);

void write_elem(double val, int row, int col, matrix_struct *A);
void write_elem(int val, int row, int col, int_matrix_struct *A);

/////////////// other struct functions //////////////////////////

void real_to_int(matrix_struct *in, int_matrix_struct *out);
void int_to_real(int_matrix_struct *in, matrix_struct *out);

void copy(matrix_struct *x, matrix_struct *y);
void copy(int_matrix_struct *x, int_matrix_struct *y);
void print(matrix_struct *x);
void print(int_matrix_struct *x);

matrix_struct *extract_matrix_struct(const Matrix &in);
int_matrix_struct *extract_matrix_struct(const IntMatrix &in);

void trans(matrix_struct *A, matrix_struct *B);
void trans(int_matrix_struct *A, int_matrix_struct *B);

void save(const char *filename, matrix_struct *A);
void load(const char *filename, matrix_struct *A);
void save(const char *filename, int_matrix_struct *A);
void load(const char *filename, int_matrix_struct *A);
void save_struct(const char *filename, matrix_struct *A);
void load_struct(const char *filename, matrix_struct *A);
void save_struct(const char *filename, int_matrix_struct *A);
void load_struct(const char *filename,int_matrix_struct *A);

void mul(matrix_struct *A, matrix_struct *B, matrix_struct *C);
void mul(int_matrix_struct *A, int_matrix_struct *B, int_matrix_struct *C);
void mul(matrix_struct *A, double B, matrix_struct *C);
void mul(double A, matrix_struct *B, matrix_struct *C);
void mul(int_matrix_struct *A, int B, int_matrix_struct *C);
void mul(int A, int_matrix_struct *B, int_matrix_struct *C);

void add(matrix_struct *A, matrix_struct *B, matrix_struct *C);
void add(int_matrix_struct *A, int_matrix_struct *B, int_matrix_struct *C);
void add(matrix_struct *A, double B, matrix_struct *C);
void add(double A, matrix_struct *B, matrix_struct *C);
void add(int_matrix_struct *A, int B, int_matrix_struct *C);
void add(int A, int_matrix_struct *B, int_matrix_struct *C);

void sub(matrix_struct *A, matrix_struct *B, matrix_struct *C);
void sub(int_matrix_struct *A, int_matrix_struct *B, int_matrix_struct *C);
void sub(matrix_struct *A, double B, matrix_struct *C);
void sub(double A, matrix_struct *B, matrix_struct *C);
void sub(int_matrix_struct *A, int B, int_matrix_struct *C);
void sub(int A, int_matrix_struct *B, int_matrix_struct *C);

/////////////// advanced matrix structure functions ////////////////
void svd(matrix_struct *A, 
         matrix_struct *U, matrix_struct *W, matrix_struct *V);
void inv(matrix_struct *A,matrix_struct *B);
void least_sq(matrix_struct *A, matrix_struct *b, matrix_struct *x);
void least_sq(matrix_struct *A, vector_struct *b, vector_struct *x);
