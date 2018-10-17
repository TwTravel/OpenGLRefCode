/* uclutil.h -- utilities for the UCL real-time data compression library
 */


#ifndef __UCLUTIL_H
#define __UCLUTIL_H

#ifndef __UCLCONF_H
#include <ucl/uclconf.h>
#endif

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************************
//
************************************************************************/

UCL_EXTERN(ucl_uint)
ucl_fread(FILE *f, ucl_voidp buf, ucl_uint size);
UCL_EXTERN(ucl_uint)
ucl_fwrite(FILE *f, const ucl_voidp buf, ucl_uint size);


#if (UCL_UINT_MAX <= UINT_MAX)
   /* avoid problems with Win32 DLLs */
#  define ucl_fread(f,b,s)      (fread(b,1,s,f))
#  define ucl_fwrite(f,b,s)     (fwrite(b,1,s,f))
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* already included */

