#include <cmath>
#include <type_traits>

template<typename F, typename D>
F exp_series_1 (F val)
{
  F num = 1;
  D div = 1;
  D step = 0;

  F sum = 0;

  for (;;)
    {
      F prev_sum = sum * step;
      sum = prev_sum + num;

      if (sum == prev_sum)
	       break;

      step += 1;

      num *= val;
      div *= step;
    }

    return sum / div;
}

template<typename F>
typename std::enable_if<std::is_floating_point<typename F::value_type>::value, F>::type
exp_series (F val) { return exp_series_1<F, typename F::value_type> (val); }

template<typename F>
typename std::enable_if<std::is_floating_point<F>::value, F>::type
exp_series (F val) { return exp_series_1<F, F> (val); }

extern "C"
{
  float exp_series_f (float val) { return exp_series (val); }
  double exp_series_d (double val) { return exp_series (val); }
};
