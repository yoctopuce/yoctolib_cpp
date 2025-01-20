/*********************************************************************
 *
 * $Id: ydef_private.h 64144 2025-01-14 08:13:07Z seb $
 *
 * Standard definitions common to all yoctopuce projects
 *
 * - - - - - - - - - License information: - - - - - - - - -
 *
 *  Copyright (C) 2011 and beyond by Yoctopuce Sarl, Switzerland.
 *
 *  Yoctopuce Sarl (hereafter Licensor) grants to you a perpetual
 *  non-exclusive license to use, modify, copy and integrate this
 *  file into your software for the sole purpose of interfacing
 *  with Yoctopuce products.
 *
 *  You may reproduce and distribute copies of this file in
 *  source or object form, as long as the sole purpose of this
 *  code is to interface with Yoctopuce products. You must retain
 *  this notice in the distributed source file.
 *
 *  You should refer to Yoctopuce General Terms and Conditions
 *  for additional information regarding your rights and
 *  obligations.
 *
 *  THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 *  WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING
 *  WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO
 *  EVENT SHALL LICENSOR BE LIABLE FOR ANY INCIDENTAL, SPECIAL,
 *  INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 *  COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR
 *  SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT
 *  LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR
 *  CONTRIBUTION, OR OTHER SIMILAR COSTS, WHETHER ASSERTED ON THE
 *  BASIS OF CONTRACT, TORT (INCLUDING NEGLIGENCE), BREACH OF
 *  WARRANTY, OR OTHERWISE.
 *
 *********************************************************************/

#ifndef  YOCTO_DEF_PRIVATE_H
#define  YOCTO_DEF_PRIVATE_H
#ifdef  __cplusplus
extern "C" {
#endif

#include "ydef.h"

#define OS_IFACE_NAME_MAX_LEN 128
typedef struct {
    u32 flags;
    IPvX_ADDR ip;
    char name[OS_IFACE_NAME_MAX_LEN];
    int ifindex;
} os_ifaces;

#if defined(WINDOWS_API)
    #if defined(__64BITS__)
        typedef u64 BSD_SOCKET;
    #else
        typedef u32 BSD_SOCKET;
    #endif
    #define INVALID_BSD_SOCKET  ((BSD_SOCKET)(~0))
#else
#ifdef TEXAS_API
    typedef int16_t BSD_SOCKET;
    #define INVALID_BSD_SOCKET (-1)
#else
    typedef int BSD_SOCKET;
    #define INVALID_BSD_SOCKET (-1)
#endif
#endif

#if 0
#if defined(WINDOWS_API) && (_MSC_VER)
    #define YBKPT { __debugbreak();}
#else
    #if defined(TEXAS_API)
        #define YBKPT  {__asm__("BKPT");}
    #else
        #define YBKPT  {__asm__("int3");}
    #endif
#endif
#else
#define YBKPT {}
#endif

#if defined(MICROCHIP_API) || defined(TEXAS_API) || defined(VIRTUAL_HUB)
    #define YAPI_IN_YDEVICE
    #define YSTATIC
#else
    #define YSTATIC static
#endif

#if defined(TEXAS_API)
#define YSAFECODE(sectname)         __attribute__((section(".ysafetext." #sectname))) sectname
#define BSS_SECTION(sectname)       __attribute__((section(".bss." #sectname)))
#define DATA_SECTION(sectname)      __attribute__((section(".data." #sectname)))
#define RODATA_SECTION(sectname)    __attribute__((section(".rodata." #sectname)))
#else
#define YSAFECODE(sectname)         sectname
#define BSS_SECTION(sectname)
#define DATA_SECTION(sectname)
#define RODATA_SECTION(sectname)
#endif

#if defined(MICROCHIP_API)
    void ypanic(int line);
    #define YPANIC              panic(__LINE__)
#else
    #ifdef YAPI_IN_YDEVICE
        #ifdef EMBEDDED_API
            void ypanic(u32 origin, u32 irritant);
        #else
            void ypanic(u32 origin, u64 irritant);
        #endif
        #define MK_FILEID(a,b,c)    ((((a)-'A')<<10) | (((b)-'A')<<5) | ((c)-'A'))
        #define MK_ORIGIN(fid,n)    ((((u32)fid) << 16) | (n))
        #define MK_CURPOS           MK_ORIGIN(__FILE_ID__, __LINE__)
        #define YPANIC              do { ypanic(MK_ORIGIN(__FILE_ID__, __LINE__) , 0); } while(0)
        #define YPANIC_IRR(irr)     do { ypanic(MK_ORIGIN(__FILE_ID__, __LINE__) , (u32)(irr)); } while(0)
        #define YASSERT(x,irr)      (void)(!(x) ? ypanic(MK_ORIGIN(__FILE_ID__, __LINE__), (u32)(irr)) : 0)
        #ifdef assert
        #undef assert
        #endif
        #define assert(cond)        YASSERT(cond,0)
    #else
        #define YPANIC              {dbglog("YPANIC:%s:%d\n",__FILENAME__ , __LINE__);}
        #define YPANIC_IRR(irr)     YPANIC
        #define YASSERT(x, irr)     if(!(x)){dbglog("ASSERT FAILED:%s:%d (%" FMTx64 ")\n",__FILENAME__ , __LINE__, (u64) (irr));}
    #endif
#endif

#ifdef  __cplusplus
}
#endif

#endif
