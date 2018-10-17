/* ucl_ptr.c -- low-level pointer constructs
 */


#include "ucl_conf.h"


/***********************************************************************
//
************************************************************************/

UCL_PUBLIC(ucl_ptr_t)
__ucl_ptr_linear(const ucl_voidp ptr)
{
    ucl_ptr_t p;

#if defined(__UCL_DOS16) || defined(__UCL_WIN16)
    p = (((ucl_ptr_t)(_FP_SEG(ptr))) << (16 - __UCL_HShift)) + (_FP_OFF(ptr));
#else
    p = PTR_LINEAR(ptr);
#endif

    return p;
}


/***********************************************************************
//
************************************************************************/

UCL_PUBLIC(unsigned)
__ucl_align_gap(const ucl_voidp ptr, ucl_uint size)
{
    ucl_ptr_t p, s, n;

    assert(size > 0);

    p = __ucl_ptr_linear(ptr);
    s = (ucl_ptr_t) (size - 1);
#if 0
    assert((size & (size - 1)) == 0);
    n = ((p + s) & ~s) - p;
#else
    n = (((p + s) / size) * size) - p;
#endif

    assert((long)n >= 0);
    assert(n <= s);

    return (unsigned)n;
}
