#if !defined(_ZINF_SWAB_H)
#define _ZINF_SWAB_H

#include <stdint.h>

#define SWAB16(a) ( (((uint16_t)(a) << 8) & 0xFF00) | (((uint16_t)(a) >> 8) & 0x00FF) )
#define SWAB32(a) ( (SWAB16(((a) >> 16) & 0xFFFF) << 0) | (SWAB16((a) & 0xFFFF) << 16) )

#endif

/* arch-tag: 30ffdf61-86df-4c0a-8b60-2f1588421f74
   (do not change this comment) */
