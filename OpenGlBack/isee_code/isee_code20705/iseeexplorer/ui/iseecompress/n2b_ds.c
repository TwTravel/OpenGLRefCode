/* n2b_ds.c -- implementation of the NRV2B decompression algorithm
 */


#define SAFE
#define ucl_nrv2b_decompress_8      ucl_nrv2b_decompress_safe_8
#define ucl_nrv2b_decompress_le16   ucl_nrv2b_decompress_safe_le16
#define ucl_nrv2b_decompress_le32   ucl_nrv2b_decompress_safe_le32
#include "n2b_d.c"
#undef SAFE
