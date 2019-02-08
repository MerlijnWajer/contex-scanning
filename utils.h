#ifndef _UTILS_H_INCLUDED_
#define _UTILS_H_INCLUDED_ 1

#include "windows.h"

#define INCHDIV1200_TO_MM(x) \
      ((int)(x*25.4/1200))

#define MM_TO_INCHDIV1200(x) \
      ((int)(x*1200/25.4))

#define GET_BYTE(p,i) \
    ((unsigned char) p[i])

#define GET_CHAR(p,i) \
    ((char) p[i])

#define GET_WORD(p,i) \
    (((unsigned int) p[i] << 8) + p[i+1])

#define GET_DWORD(p,i) \
    (((unsigned long) p[i]   << 24) + \
     ((unsigned long) p[i+1] << 16) + \
     ((unsigned long) p[i+2] << 8)  + \
     p[i+3])

#define PUT_WORD(p,i,v) \
   p[i]   = (unsigned char) (((v) >> 8) & 0xff); \
   p[i+1] = (unsigned char) ((v)        & 0xff)

template <typename T> T MySwap(T src)
{
   BYTE b[sizeof(T)];
   BYTE *pb=(BYTE *)&src;
   for(int i=0;i<sizeof(T);i++)
      b[i]=pb[sizeof(T)-1-i];
   
   return *(T *)b;
   
}


#endif
