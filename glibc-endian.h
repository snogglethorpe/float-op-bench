#include <endian.h>

# if __FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__ || (!__FLOAT_WORD_ORDER__ && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
#  define BIG_ENDI 1
#  undef LITTLE_ENDI
#  define HIGH_HALF 0
#  define  LOW_HALF 1
# else
#  if __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__ || (!__FLOAT_WORD_ORDER__ && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))
#   undef BIG_ENDI
#   define LITTLE_ENDI 1
#   define HIGH_HALF 1
#   define  LOW_HALF 0
#  endif
# endif
