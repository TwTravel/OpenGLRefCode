/* getbit.h -- bit-buffer access
 */


/***********************************************************************
//
************************************************************************/

#if 1
#define getbit_8(bb, src, ilen) \
    (bb*=2,bb&0xff ? (bb>>8)&1 : ((bb=src[ilen++]*2+1)>>8)&1)
#else
#define getbit_8(bb, src, ilen) \
    (((bb*=2, bb&0xff ? bb : bb = src[ilen++]*2+1) >> 8) & 1)
#endif


#define getbit_le16(bb, src, ilen) \
    (bb*=2,bb&0xffff ? (bb>>16)&1 : (ilen+=2,((bb=(src[ilen-2]+src[ilen-1]*256U)*2+1)>>16)&1))


#if 1 && defined(UCL_UNALIGNED_OK_4) && (UCL_BYTE_ORDER == UCL_LITTLE_ENDIAN)
#define getbit_le32(bb, bc, src, ilen) \
    (bc > 0 ? ((bb>>--bc)&1) : (bc=31,\
    bb=*(const ucl_uint32p)((src)+ilen),ilen+=4,(bb>>31)&1))
#else
#define getbit_le32(bb, bc, src, ilen) \
    (bc > 0 ? ((bb>>--bc)&1) : (bc=31,\
    bb=src[ilen]+src[ilen+1]*0x100+src[ilen+2]*0x10000+src[ilen+3]*0x1000000,\
    ilen+=4,(bb>>31)&1))
#endif


/*
vi:ts=4:et
*/

