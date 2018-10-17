/* ucl_str.c -- string functions for the the UCL library
 */


#include "ucl_conf.h"


/***********************************************************************
// slow but portable <string.h> stuff, only used in assertions
************************************************************************/

UCL_PUBLIC(int)
ucl_memcmp(const ucl_voidp s1, const ucl_voidp s2, ucl_uint len)
{
#if (UCL_UINT_MAX <= SIZE_T_MAX) && defined(HAVE_MEMCMP)
    return memcmp(s1,s2,len);
#else
    const ucl_byte *p1 = (const ucl_byte *) s1;
    const ucl_byte *p2 = (const ucl_byte *) s2;
    int d;

    if (len > 0) do
    {
        d = *p1 - *p2;
        if (d != 0)
            return d;
        p1++;
        p2++;
    }
    while (--len > 0);
    return 0;
#endif
}


UCL_PUBLIC(ucl_voidp)
ucl_memcpy(ucl_voidp dest, const ucl_voidp src, ucl_uint len)
{
#if (UCL_UINT_MAX <= SIZE_T_MAX) && defined(HAVE_MEMCPY)
    return memcpy(dest,src,len);
#else
    ucl_byte *p1 = (ucl_byte *) dest;
    const ucl_byte *p2 = (const ucl_byte *) src;

    if (len <= 0 || p1 == p2)
        return dest;
    do
        *p1++ = *p2++;
    while (--len > 0);
    return dest;
#endif
}


UCL_PUBLIC(ucl_voidp)
ucl_memmove(ucl_voidp dest, const ucl_voidp src, ucl_uint len)
{
#if (UCL_UINT_MAX <= SIZE_T_MAX) && defined(HAVE_MEMMOVE)
    return memmove(dest,src,len);
#else
    ucl_byte *p1 = (ucl_byte *) dest;
    const ucl_byte *p2 = (const ucl_byte *) src;

    if (len <= 0 || p1 == p2)
        return dest;

    if (p1 < p2)
    {
        do
            *p1++ = *p2++;
        while (--len > 0);
    }
    else
    {
        p1 += len;
        p2 += len;
        do
            *--p1 = *--p2;
        while (--len > 0);
    }
    return dest;
#endif
}


UCL_PUBLIC(ucl_voidp)
ucl_memset(ucl_voidp s, int c, ucl_uint len)
{
#if (UCL_UINT_MAX <= SIZE_T_MAX) && defined(HAVE_MEMSET)
    return memset(s,c,len);
#else
    ucl_byte *p = (ucl_byte *) s;

    if (len > 0) do
        *p++ = UCL_BYTE(c);
    while (--len > 0);
    return s;
#endif
}
