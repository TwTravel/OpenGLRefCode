/* io.c -- io functions
 */


#include "ucl_conf.h"

#if !defined(NO_STDIO_H)

#include <stdio.h>
#include "uclutil.h"

#undef ucl_fread
#undef ucl_fwrite


/***********************************************************************
//
************************************************************************/

UCL_PUBLIC(ucl_uint)
ucl_fread(FILE *f, ucl_voidp s, ucl_uint len)
{
#if 1 && (UCL_UINT_MAX <= SIZE_T_MAX)
    return fread(s,1,len,f);
#else
    ucl_byte *p = (ucl_byte *) s;
    ucl_uint l = 0;
    size_t k;
    unsigned char *b;
    unsigned char buf[512];

    while (l < len)
    {
        k = len - l > sizeof(buf) ? sizeof(buf) : (size_t) (len - l);
        k = fread(buf,1,k,f);
        if (k <= 0)
            break;
        l += k;
        b = buf; do *p++ = *b++; while (--k > 0);
    }
    return l;
#endif
}


/***********************************************************************
//
************************************************************************/

UCL_PUBLIC(ucl_uint)
ucl_fwrite(FILE *f, const ucl_voidp s, ucl_uint len)
{
#if 1 && (UCL_UINT_MAX <= SIZE_T_MAX)
    return fwrite(s,1,len,f);
#else
    const ucl_byte *p = (const ucl_byte *) s;
    ucl_uint l = 0;
    size_t k, n;
    unsigned char *b;
    unsigned char buf[512];

    while (l < len)
    {
        k = len - l > sizeof(buf) ? sizeof(buf) : (size_t) (len - l);
        b = buf; n = k; do *b++ = *p++; while (--n > 0);
        k = fwrite(buf,1,k,f);
        if (k <= 0)
            break;
        l += k;
    }
    return l;
#endif
}


#endif /* !defined(NO_STDIO_H) */


/*
vi:ts=4:et
*/
