/* alloc.c -- memory allocation
 */


#include "ucl_conf.h"

#if defined(HAVE_MALLOC_H)
#  include <malloc.h>
#endif
#if defined(__palmos__)
#  include <System/MemoryMgr.h>
#endif


#undef ucl_alloc_hook
#undef ucl_free_hook
#undef ucl_alloc
#undef ucl_malloc
#undef ucl_free


/***********************************************************************
// implementation
************************************************************************/

UCL_PRIVATE(ucl_voidp)
ucl_alloc_internal(ucl_uint nelems, ucl_uint size)
{
    ucl_voidp p = NULL;
    unsigned long s = (unsigned long) nelems * size;

    if (nelems <= 0 || size <= 0 || s < nelems || s < size)
        return NULL;

#if defined(__palmos__)
    p = (ucl_voidp) MemPtrNew(s);
#elif (UCL_UINT_MAX <= SIZE_T_MAX)
    if (s < SIZE_T_MAX)
        p = (ucl_voidp) malloc((size_t)s);
#elif defined(HAVE_HALLOC)
    if (size < SIZE_T_MAX)
        p = (ucl_voidp) halloc(nelems,(size_t)size);
#else
    if (s < SIZE_T_MAX)
        p = (ucl_voidp) malloc((size_t)s);
#endif

    return p;
}


UCL_PRIVATE(void)
ucl_free_internal(ucl_voidp p)
{
    if (!p)
        return;

#if defined(__palmos__)
    MemPtrFree(p);
#elif (UCL_UINT_MAX <= SIZE_T_MAX)
    free(p);
#elif defined(HAVE_HALLOC)
    hfree(p);
#else
    free(p);
#endif
}


/***********************************************************************
// public interface using the global hooks
************************************************************************/

/* global allocator hooks */
ucl_alloc_hook_t ucl_alloc_hook = ucl_alloc_internal;
ucl_free_hook_t ucl_free_hook = ucl_free_internal;


UCL_PUBLIC(ucl_voidp)
ucl_alloc(ucl_uint nelems, ucl_uint size)
{
    if (!ucl_alloc_hook)
        return NULL;

    return ucl_alloc_hook(nelems,size);
}


UCL_PUBLIC(ucl_voidp)
ucl_malloc(ucl_uint size)
{
    if (!ucl_alloc_hook)
        return NULL;

#if defined(__palmos__)
    return ucl_alloc_hook(size,1);
#elif (UCL_UINT_MAX <= SIZE_T_MAX)
    return ucl_alloc_hook(size,1);
#elif defined(HAVE_HALLOC)
    /* use segment granularity by default */
    if (size + 15 > size)   /* avoid overflow */
        return ucl_alloc_hook((size+15)/16,16);
    return ucl_alloc_hook(size,1);
#else
    return ucl_alloc_hook(size,1);
#endif
}


UCL_PUBLIC(void)
ucl_free(ucl_voidp p)
{
    if (!ucl_free_hook)
        return;

    ucl_free_hook(p);
}


/*
vi:ts=4:et
*/
