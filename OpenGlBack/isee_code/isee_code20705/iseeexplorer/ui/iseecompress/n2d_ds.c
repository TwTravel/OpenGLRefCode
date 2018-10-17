/* n2d_ds.c -- implementation of the NRV2D decompression algorithm
 */


#define SAFE
#define ucl_nrv2d_decompress_8      ucl_nrv2d_decompress_safe_8
#define ucl_nrv2d_decompress_le16   ucl_nrv2d_decompress_safe_le16
#define ucl_nrv2d_decompress_le32   ucl_nrv2d_decompress_safe_le32
#include "n2d_d.c"
#undef SAFE
