/* ucl_util.c -- utilities for the UCL library
 */


#include "ucl_conf.h"
#include "ucl_util.h"


/***********************************************************************
//
************************************************************************/

UCL_PUBLIC(ucl_bool)
ucl_assert(int expr)
{
    return (expr) ? 1 : 0;
}


/***********************************************************************
// adler32 checksum
// adapted from free code by Mark Adler <madler@alumni.caltech.edu>
// see http://www.cdrom.com/pub/infozip/zlib/
************************************************************************/

#define UCL_BASE 65521u /* largest prime smaller than 65536 */
#define UCL_NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define UCL_DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define UCL_DO2(buf,i)  UCL_DO1(buf,i); UCL_DO1(buf,i+1);
#define UCL_DO4(buf,i)  UCL_DO2(buf,i); UCL_DO2(buf,i+2);
#define UCL_DO8(buf,i)  UCL_DO4(buf,i); UCL_DO4(buf,i+4);
#define UCL_DO16(buf,i) UCL_DO8(buf,i); UCL_DO8(buf,i+8);

UCL_PUBLIC(ucl_uint32)
ucl_adler32(ucl_uint32 adler, const ucl_byte *buf, ucl_uint len)
{
    ucl_uint32 s1 = adler & 0xffff;
    ucl_uint32 s2 = (adler >> 16) & 0xffff;
    int k;

    if (buf == NULL)
        return 1;

    while (len > 0)
    {
        k = len < UCL_NMAX ? (int) len : UCL_NMAX;
        len -= k;
        if (k >= 16) do
        {
            UCL_DO16(buf,0);
            buf += 16;
            k -= 16;
        } while (k >= 16);
        if (k != 0) do
        {
            s1 += *buf++;
            s2 += s1;
        } while (--k > 0);
        s1 %= UCL_BASE;
        s2 %= UCL_BASE;
    }
    return (s2 << 16) | s1;
}
