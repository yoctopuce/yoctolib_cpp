/*********************************************************************
 *
 * $Id: ykey.h 61107 2024-05-24 07:59:31Z mvuilleu $
 *
 * Declaration of standard key computations
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

#ifndef YKEY_H
#define YKEY_H
#include "ydef.h"

void bin2str(char* to, const u8* p, u16 len, u8 addnull);

// HTTP digest authentication support

#define HTTP_AUTH_MD5_SIZE      16
#define HTTP_AUTH_MD5_STRLEN    (HTTP_AUTH_MD5_SIZE*2)

void ComputeAuthHA1(u8* buf, const char* user, const char* pass, const char* realm);
void ComputeAuthHA2(u8* buf, const char* method, const char* url);
void ComputeAuthResponse(char* buf, const u8* ha1, const char* nonce, const char* nc, const char* cnonce, const u8* ha2);
int CheckWSAuth(u32 nonce, const u8* ha1, const u8* to_verify, u8* out);

// Parse a request header, return 0 if a valid WWW-Authenticate header and set args to corresponding fields
// - Request is patched in place to null-terminate each field.
// - If return value is 0, at least method,realm and qop are set to non-NULL value
// - qop is set to an empty string if not specified in thq authenticate header
int yParseWWWAuthenticate(char* replybuf, int replysize, char** method, char** realm, char** qop, char** nonce, char** opaque);

// Fill in buf with a proper digest authorization header
int yDigestAuthorization(char* buf, u16 bufsize, const char* user, const char* realm, const u8* ha1,
                          const char* nonce, const char* opaque, u32* nc, const char* method, const char* uri);

// Note: This API is designed for cooperative multitasking
//       It is not multithread-safe
void yInitPsk(const char* pass, const char* ssid);
int yIterPsk(u8* res, const char* ssid);
u8* ySHA1(const char* text);

// MD5 hash structures
#ifndef MICROCHIP_API
typedef struct {
    u32 buf[4];
    u32 bits[2];

    union {
        u8 in[64];
        u32 in32[16];
    };
} HASH_SUM;

void MD5Initialize(HASH_SUM* theSum);
void MD5AddData(HASH_SUM* theSum, const u8* data, u32 len);
void MD5Calculate(HASH_SUM* theSum, u8* result);
#endif

#define swaps(w)                    ((((w)&0xff00u)>>8u) + (((w)&0xffu)<<8u))
#define swapl(dw)                   (swaps(((dw)&0xffff0000u)>>16u) + (swaps((dw)&0xffffu)<<16u))
#define NETMASK_LOW(len)            swapl(((u32)1 << (32-len))-1)
#define NETMASK_HIGH(len)           swapl(0-((u32)1 << (32-len)))

#ifndef IPV4_ONLY
void setIPv6Mask(IPvX_ADDR* addr, u16 nbits);
void setIPv4Mask(IPvX_ADDR* addr, u16 nbits);
void setIPv4Val(IPvX_ADDR* addr, u32 ipval);
u32 getIPv4Val(const IPvX_ADDR* addr);
int isIPEmpty(const IPvX_ADDR* addr);
int isIPv4(const IPvX_ADDR* addr);
u16 IPvXAddrLen(const IPvX_ADDR* addr);
#else
#define setIPv4Mask(ipaddr,nbits)   (ipaddr)->v4.addr.Val = NETMASK_HIGH((nbits))
#define setIPv4Val(ipaddr,ipval)    (ipaddr)->v4.addr.Val = (ipval)
#define getIPv4Val(ipaddr)          ((ipaddr)->v4.addr.Val)
#define isIPEmpty(ipaddr)           ((ipaddr)->v4.addr.Val == 0)
#define isIPv4(addr)                1
#endif

char* u16toa(u16 val, char* buff);
char* iptoa(const IPvX_ADDR* addr, char* buff); /* sizeof(buff) should be IPSTR_SIZE */

#endif
