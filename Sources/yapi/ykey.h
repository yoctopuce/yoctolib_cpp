/*********************************************************************
 *
 * $Id: yfifo.h 6846 2012-07-09 17:41:26Z seb $
 *
  * Declaration of standard key computations
 *
 * - - - - - - - - - License information: - - - - - - - - - 
 *
 * Copyright (C) 2011 and beyond by Yoctopuce Sarl, Switzerland.
 *
 * 1) If you have obtained this file from www.yoctopuce.com,
 *    Yoctopuce Sarl licenses to you (hereafter Licensee) the
 *    right to use, modify, copy, and integrate this source file
 *    into your own solution for the sole purpose of interfacing
 *    a Yoctopuce product with Licensee's solution.
 *
 *    The use of this file and all relationship between Yoctopuce 
 *    and Licensee are governed by Yoctopuce General Terms and 
 *    Conditions.
 *
 *    THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 *    WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING 
 *    WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS 
 *    FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO
 *    EVENT SHALL LICENSOR BE LIABLE FOR ANY INCIDENTAL, SPECIAL,
 *    INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, 
 *    COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR 
 *    SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT 
 *    LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR
 *    CONTRIBUTION, OR OTHER SIMILAR COSTS, WHETHER ASSERTED ON THE
 *    BASIS OF CONTRACT, TORT (INCLUDING NEGLIGENCE), BREACH OF
 *    WARRANTY, OR OTHERWISE.
 *
 * 2) If your intent is not to interface with Yoctopuce products,
 *    you are not entitled to use, read or create any derived 
 *    material from this source file.
 *
 *********************************************************************/
#ifndef YKEY_H
#define YKEY_H
#include "ydef.h"

void bin2str(char *to, const u8 *p, u16 len,u8 addnull);

// HTTP digest authentication support

#define HTTP_AUTH_MD5_SIZE      16
#define HTTP_AUTH_MD5_STRLEN    (HTTP_AUTH_MD5_SIZE*2)

void ComputeAuthHA1(u8 *buf, const char *user, const char *pass, const char *realm);
void ComputeAuthHA2(u8 *buf, const char *method, const char *url) ;
void ComputeAuthResponse(char *buf,  const u8 * ha1,  const char *nonce, const char *nc,  const char *cnonce,  const u8 *ha2);

// Parse a request header, return 0 if a valid WWW-Authenticate header and set args to corresponding fields
// - Request is patched in place to null-terminate each field.
// - If return value is 0, at least method,realm and qop are set to non-NULL value
// - qop is set to an empty string if not specified in thq authenticate header
int yParseWWWAuthenticate(char *replybuf, int replysize, char **method, char **realm, char **qop, char **nonce, char **opaque);

// Fill in buf with a proper digest authorization header
void yDigestAuthorization(char *buf, int bufsize, const char *user, const char *realm, const u8 *ha1, 
                          const char *nonce, const char *opaque, u32 *nc, const char *method, const char *uri);

// Note: This API is designed for cooperative multitasking
//       It is not multithread-safe
void yInitPsk(const char *pass, const char *ssid);
int  yIterPsk(u8 *res, const char *ssid);

// MD5 hash structures
#ifndef MICROCHIP_API
typedef struct {
    u32 buf[4];
    u32 bits[2];
    u8  in[64];
} HASH_SUM;

void MD5Initialize(HASH_SUM *theSum);
void MD5AddData(HASH_SUM *theSum, const u8* data, u32 len);
void MD5Calculate(HASH_SUM *theSum, u8* result);
#endif

#endif
