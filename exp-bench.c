#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>

#define NITERS 1000000

float finps[NITERS], foutps[NITERS];
double dinps[NITERS], doutps[NITERS];

int main ()
{
  struct rusage start_float_ru, start_double_ru, end_ru;
  int i;

  for (i = 0; i < NITERS; i++)
    finps[i] = (float) (float) (drand48() + 1e-6);
  for (i = 0; i < NITERS; i++)
    dinps[i] = (double) (finps[i]);

  getrusage (RUSAGE_SELF, &start_float_ru);
  for (i = 0; i < NITERS; i++)
    foutps[i] = expf (finps[i]);

  getrusage (RUSAGE_SELF, &start_double_ru);
  for (i = 0; i < NITERS; i++)
    doutps[i] = exp (dinps[i]);

  getrusage (RUSAGE_SELF, &end_ru);

  double fsum = 0;
  for (i = 0; i < NITERS; i++)
    fsum += foutps[i];

  double dsum = 0;
  for (i = 0; i < NITERS; i++)
    dsum += doutps[i];

  for (i = 0; i < NITERS; i++)
    {
      double d = doutps[i];
      float f = foutps[i];
      float d_to_f = (float)d;
      if (d_to_f != f)
	{
	  union { float f; unsigned int u; } du, fu, deltau;
	  du.f = d_to_f;
	  fu.f = f;
	  deltau.f = d_to_f - f;
	  if ((int)du.u - (int)fu.u > 1 || (int)fu.u - (int)du.u > 1)
	    printf ("%d:\n"
		    "  (float)exp(%a) = %a [%08x]\n"
		    "  extf(%a) == %a [%08x]\n"
		    "  delta = %a [%08x]\n"
		    ,
		    i,
		    dinps[i], d_to_f, du.u,
		    finps[i], f, fu.u,
		    d_to_f - f, deltau.u
		    );
	}
    }

  printf ("fsum = %g, dsum = %g\n", fsum, dsum);
  printf ("float user time: %g sec\n",
	  ((double)(start_double_ru.ru_utime.tv_sec
		    - start_float_ru.ru_utime.tv_sec)
	   +(double)(start_double_ru.ru_utime.tv_usec
		     - start_float_ru.ru_utime.tv_usec) / 1.e6));
  printf ("double user time: %g sec\n",
	  ((double)(end_ru.ru_utime.tv_sec
		    - start_double_ru.ru_utime.tv_sec)
	   +(double)(end_ru.ru_utime.tv_usec
		     - start_double_ru.ru_utime.tv_usec) / 1.e6));

  return 0;
}
