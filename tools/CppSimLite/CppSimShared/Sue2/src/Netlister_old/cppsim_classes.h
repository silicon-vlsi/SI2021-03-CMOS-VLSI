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

#define MAX_NODE_NAME_LENGTH 256
#define CHAR_BUFFER_LENGTH 1000
#define DATA_BUFFER_LENGTH 2048
#define MAX_CHAR_LENGTH 1000

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
void  fwrite_char_values(char *cbuf, size_t size_of_block, FILE *fp);

/* the following dictates whether matrices are single or double precision  */  
/* note:  EL_TYPE_NUMBER = 1 if float specified, 2 if double specified for */  
/* EL_TYPE                                                                 */  
typedef double EL_TYPE;  
#define EL_TYPE_NUMBER 2  
#define ISIGN -1.0

typedef struct  
  {  
   char name[30];  
   int rows,cols;  
   EL_TYPE **elem;
} matrix;  

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
   int inp(matrix *in);
   int length,notdone;
   double mean();
   double var();
   void add(const List &other);
   void add(double in);
   void mul(const List &other);
   void mul(double in);
   void conv(const List &other);
   void cat(const List &other);
   int load(char *filename);
   void save(char *filename);
   void copy(const List &other);
   void print(char *name);
private:
   int temp_flag;
   DOUBLE_LIST *first_entry, *cur_entry, *cur_write_entry;
   DOUBLE_LIST *create_entry();
   void delete_entries(DOUBLE_LIST *first_entry);
};

void list_to_matrix(const List &in, matrix *out);
void neg_index_error(char *function,char *variable,int value,matrix *A);
void free_matrix(matrix *x);
matrix *init(char *name);
matrix *replace_elements(EL_TYPE **new_elements,int rows,int cols,matrix *A);
matrix *conform_matrix(int rows,int cols,matrix *A);
void ifft_mat(matrix *DATA_REAL, matrix *DATA_IMAG, matrix *FFT_REAL, matrix *FFT_IMAG);  
void fft_mat(matrix *DATA_REAL, matrix *DATA_IMAG, matrix *FFT_REAL, matrix *FFT_IMAG);
void four1(matrix *DATA, matrix *FFT);
void real_fft_mat(matrix *DATA, matrix *REAL, matrix *IMAG);

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
   void print(char *name);
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

#define LENGTH_PAR_LIST 50  /* don't make this more than 100 */
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
   Poly(char *expr_in, char *vars=NULL, double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
   Poly(const List &other);
   void set(char *expr_in, char *vars=NULL, double var0=NaN, double var1=NaN, 
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
NODE *add_node(NODE *first_node, char *name, double value);
NODE *init_node(char *name, double value);


class Delay
{
public:
   Delay(double nom_value, double max_value=NaN);
   ~Delay();
   double inp(double in);
   double inp(double in, double delta_delay);
   double out;
private:
   double *buffer;
   double delay_val, prev_delay_val, nom_val;
   int buffer_size, state, transition_flag;
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
  Probe(char *filename,double sample_per=1.0,int subsample=1);
  void inp(double node_value, char *node_name);
  void inp(int int_node_value, char *node_name);
  void inp(double node_value, char *node_name, int sim_step);
  void inp(int int_node_value, char *node_name, int sim_step);
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
   Amp(char *poly_expr, char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
   void set(char *poly_expr, char *vars=NULL, 
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
   Vco(char *poly_expr, char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
   void set(char *poly_expr, char *vars=NULL, 
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
  Filter(char *num, char *den, char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
  Filter(const List &num_list, char *den, char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
  Filter(char *num, const List &den_list, char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
  Filter(const List &num_list, const List &den_list);

  void set_base(Poly *num_poly, Poly *den_poly, char *num, char *den);

  void set(char *num, char *den, char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
  void set(const List &num_list, char *den, char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
  void set(char *num, const List &den_list, char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
  void set(const List &num_list, const List &den_list);
  void set_dt_filt(Poly *num_poly, Poly *den_poly, char *num, char *den);
  void set_ct_filt(Poly *num_poly, Poly *den_poly, char *num, char *den);
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
   Rand(char *type);
   Rand(char *type, double p);
   ~Rand();
   void set(char *type, double p);
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
   SdMbitMod(char *num, char *vars=NULL, 
       double var0=NaN, double var1=NaN, 
       double var2=NaN, double var3=NaN, double var4=NaN, double var5=NaN, 
       double var6=NaN, double var7=NaN, double var8=NaN, double var9=NaN);
   ~SdMbitMod();
   void set(char *num, char *vars=NULL, 
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
   SigGen(char *type, double freq, double Ts);
   SigGen(char *type, double freq, double Ts, const List &other);
   SigGen(char *type, double freq, double Ts, const List &other, int start_entry);
   ~SigGen();
   void set(char *type, double freq, double Ts);
   void set(char *type, double freq, double Ts, const List &other);
   void set(char *type, double freq, double Ts, const List &other, int start_entry);
   void reset();
   double inp(double in);
   double phase, out, square;
private:
   double sample_period, frequency;
   double prev_phase, prev_square, prev_in, data;
   int clk_state, type_flag, first_entry_flag;
   Reg reg1;
   Rand rand1;
   List data_seq;
   Filter phase_filt;
};

