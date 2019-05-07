#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>

#ifdef __x86_64__
extern float glibc_x86_64_expf (float);
#endif
extern float glibc_generic_expf (float);
extern double glibc_generic_exp (double);

#define NUM_ITERS 10000000
#define NUM_INNER_ITERS 500

float finps[NUM_INNER_ITERS + 1];
double dinps[NUM_INNER_ITERS + 1];

inline float nopf (float x) { return x; }
inline float addf (float x, float y) { return x + y; }
inline float mulf (float x, float y) { return x * y; }
inline float divf (float x, float y) { return x / y; }

inline double nop (double x) { return x; }
inline double add (double x, double y) { return x + y; }
inline double mul (double x, double y) { return x * y; }
inline double divd (double x, double y) { return x / y; }

extern float exp_series_f (float);
extern double exp_series_d (double);

enum OP { NOP, ADD, MUL, DIV, SQRT, SIN, ATAN2, EXP, EXP_SERIES, X86_64_EXP, GENERIC_EXP, NUM_OPS };

double time_diff (struct rusage *start_ru, struct rusage *end_ru)
{
  return ((double)(end_ru->ru_utime.tv_sec
		   - start_ru->ru_utime.tv_sec)
	  + ((double)(end_ru->ru_utime.tv_usec
		     - start_ru->ru_utime.tv_usec)
	     / 1.e6));
}

int main (int argc, const char **argv)
{
  struct rusage start_float_ru, end_float_ru;
  struct rusage start_double_ru, end_double_ru;
  int i, j;
  int num_iters = NUM_ITERS, num_outer_iters;
  double float_nop_time, double_nop_time;
  float final_fosum = 0;
  double final_dosum = 0;

  if (*++argv && atoi (*argv))
    num_iters = atoi (*argv++);

  for (i = 0; i < NUM_INNER_ITERS + 1; i++)
    dinps[i] = drand48() + 1e-6;
  for (i = 0; i < NUM_INNER_ITERS + 1; i++)
    finps[i] = (float) (dinps[i]);

#define FLOAT_TEST_LOOP(op)			\
  do {						\
    float_fun_name = #op;  			\
    for (i = 0; i < num_outer_iters; i++)	\
      for (j = 0; j < NUM_INNER_ITERS; j++)	\
	fosum += op (finps[j]);			\
  } while (0);
#define FLOAT_TEST_LOOP2(op)			\
  do {						\
    float_fun_name = #op;  			\
    for (i = 0; i < num_outer_iters; i++)	\
      for (j = 0; j < NUM_INNER_ITERS; j++)	\
        fosum += op (finps[j], finps[j+1]);	\
  } while (0);

#define DOUBLE_TEST_LOOP(op)			\
  do {						\
    double_fun_name = #op;  			\
    for (i = 0; i < num_outer_iters; i++)	\
      for (j = 0; j < NUM_INNER_ITERS; j++)	\
        dosum += op (dinps[j]);			\
  } while (0);
#define DOUBLE_TEST_LOOP2(op)			\
  do {						\
    double_fun_name = #op;  			\
    for (i = 0; i < num_outer_iters; i++)	\
      for (j = 0; j < NUM_INNER_ITERS; j++)	\
        dosum += op (dinps[j], dinps[j+1]);	\
  } while (0);

  num_outer_iters = num_iters / NUM_INNER_ITERS;
  num_iters = NUM_INNER_ITERS * num_outer_iters;

  double fisum = 0;
  for (i = 0; i < NUM_INNER_ITERS; i++)
    fisum += finps[i];

  double disum = 0;
  for (i = 0; i < NUM_INNER_ITERS; i++)
    disum += dinps[i];

  printf ("%d iterations\n", num_iters);

  {
    float fosum = 0;
    double dosum = 0;
    const char *float_fun_name = "?", *double_fun_name = "?";
    
    getrusage (RUSAGE_SELF, &start_float_ru);
    FLOAT_TEST_LOOP (nopf);
    getrusage (RUSAGE_SELF, &end_float_ru);

    getrusage (RUSAGE_SELF, &start_double_ru);
    DOUBLE_TEST_LOOP (nop);
    getrusage (RUSAGE_SELF, &end_double_ru);

    float_nop_time = time_diff (&start_float_ru, &end_float_ru);
    double_nop_time = time_diff (&start_double_ru, &end_double_ru);

    final_fosum += fosum;
    final_dosum += dosum;
  }

  printf ("loop overhead: float: %g sec, double: %g sec\n",
	  float_nop_time, double_nop_time);

  while (*argv)
    {
      const char *op_name = *argv++;
      const char *float_fun_name = "?", *double_fun_name = "?";

      enum OP op;
      float fosum = 0;
      double dosum = 0;

      if (strcmp (op_name, "nop") == 0)
	op = NOP;
      else if (strcmp (op_name, "add") == 0)
	op = ADD;
      else if (strcmp (op_name, "mul") == 0)
	op = MUL;
      else if (strcmp (op_name, "div") == 0)
	op = DIV;
      else if (strcmp (op_name, "sqrt") == 0)
	op = SQRT;
      else if (strcmp (op_name, "sin") == 0)
	op = SIN;
      else if (strcmp (op_name, "atan2") == 0)
	op = ATAN2;
      else if (strcmp (op_name, "exp") == 0)
	op = EXP;
      else if (strcmp (op_name, "exp-series") == 0)
	op = EXP_SERIES;
#ifdef __x86_64__
      else if (strcmp (op_name, "x86_64_exp") == 0)
	op = X86_64_EXP;
#endif
      else if (strcmp (op_name, "generic_exp") == 0)
	op = GENERIC_EXP;
      else
	{
	  fprintf (stderr, "Unknown operation \"%s\"\n", op_name);
	  exit (1);
	}

      getrusage (RUSAGE_SELF, &start_float_ru);
      switch (op)
	{
	case NOP:  FLOAT_TEST_LOOP (nopf); break;
	case ADD:  FLOAT_TEST_LOOP2 (addf); break;
	case MUL:  FLOAT_TEST_LOOP2 (mulf); break;
	case DIV:  FLOAT_TEST_LOOP2 (divf); break;
	case SQRT: FLOAT_TEST_LOOP (sqrtf); break;
	case SIN:  FLOAT_TEST_LOOP (sinf); break;
	case ATAN2: FLOAT_TEST_LOOP2 (atan2f); break;
	case EXP:  FLOAT_TEST_LOOP (expf); break;
	case EXP_SERIES:  FLOAT_TEST_LOOP (exp_series_f); break;
#ifdef __x86_64__
	case X86_64_EXP:  FLOAT_TEST_LOOP (glibc_x86_64_expf); break;
#endif
	case GENERIC_EXP:  FLOAT_TEST_LOOP (glibc_generic_expf); break;
	}
      getrusage (RUSAGE_SELF, &end_float_ru);

      getrusage (RUSAGE_SELF, &start_double_ru);
      switch (op)
	{
	case NOP:  DOUBLE_TEST_LOOP (nop); break;
	case ADD:  DOUBLE_TEST_LOOP2 (add); break;
	case MUL:  DOUBLE_TEST_LOOP2 (mul); break;
	case DIV:  DOUBLE_TEST_LOOP2 (divd); break;
	case SQRT: DOUBLE_TEST_LOOP (sqrt); break;
	case SIN:  DOUBLE_TEST_LOOP (sin); break;
	case ATAN2: DOUBLE_TEST_LOOP2 (atan2); break;
	case EXP:  DOUBLE_TEST_LOOP (exp); break;
	case EXP_SERIES:  DOUBLE_TEST_LOOP (exp_series_d); break;
#ifdef __x86_64__
	case X86_64_EXP:  DOUBLE_TEST_LOOP (exp); break;
#endif
	case GENERIC_EXP:  DOUBLE_TEST_LOOP (glibc_generic_exp); break;
	}
      getrusage (RUSAGE_SELF, &end_double_ru);

      printf ("%s:\n", op_name);

      printf ("  fosum = %g, dosum = %g\n", fosum, dosum);
      final_fosum += fosum;
      final_dosum += dosum;

      printf ("  float (%s) user time: %g sec\n",
	      float_fun_name,
	      time_diff (&start_float_ru, &end_float_ru) - float_nop_time);
      printf ("  double (%s) user time: %g sec\n",
	      double_fun_name,
	      time_diff (&start_double_ru, &end_double_ru) - double_nop_time);
    }

  printf ("final fosum = %g, dosum = %g\n", final_fosum, final_dosum);

  return 0;
}
