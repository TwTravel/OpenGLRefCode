/* n2b_to.c -- implementation of the NRV2B test overlap algorithm
 */



/***********************************************************************
// entries for the different bit-buffer sizes
************************************************************************/

#include "ucl.h"
#include "ucl_conf.h"
#include "getbit.h"

#define SAFE
#define TEST_OVERLAP


UCL_PUBLIC(int)
ucl_nrv2b_test_overlap_8        ( const ucl_byte *src, ucl_uint src_off,
                                        ucl_uint  src_len, ucl_uint *dst_len,
                                        ucl_voidp wrkmem )
{
#define getbit(bb)      getbit_8(bb,src,ilen)
#include "n2b_d.c"
#undef getbit
}


UCL_PUBLIC(int)
ucl_nrv2b_test_overlap_le16     ( const ucl_byte *src, ucl_uint src_off,
                                        ucl_uint  src_len, ucl_uint *dst_len,
                                        ucl_voidp wrkmem )
{
#define getbit(bb)      getbit_le16(bb,src,ilen)
#include "n2b_d.c"
#undef getbit
}


UCL_PUBLIC(int)
ucl_nrv2b_test_overlap_le32     ( const ucl_byte *src, ucl_uint src_off,
                                        ucl_uint  src_len, ucl_uint *dst_len,
                                        ucl_voidp wrkmem )
{
    unsigned bc = 0;
#define getbit(bb)      getbit_le32(bb,bc,src,ilen)
#include "n2b_d.c"
#undef getbit
}
