/*********************************************************************
 *
 * $Id: yssl.h 53508 2023-03-10 09:48:02Z seb $
 *
 *  Declaration of a client TCP stack
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

#ifndef YSSL_H
#define YSSL_H


#ifdef  __cplusplus
extern "C" {
#endif

#include "ydef_private.h"

//#define DEBUG_SSL

#ifdef DEBUG_SSL
#define SSLLOG  dbglog
#else
#if defined(_MSC_VER)
#if (_MSC_VER > MSC_VS2003)
#define SSLLOG(fmt,...)
#else
__forceinline void __SSLLOG(fmt,...){}
#define SSLLOG __SSLLOG
#endif
#else
#define SSLLOG(fmt,args...)
#endif
#endif

#ifndef NO_YSSL
#include "yfifo.h"

#define YSSL_BUFFER_SIZE  1024
#define YSSL_TCP_SOCK_CLOSED  1
#define YSSL_TCP_SERVER_MODE  2

typedef struct {
    YSOCKET tcpskt; // tcp socket
    u32 flags;
    void* ssl; // ssl context
    void* ssl_conf; // ssl config (used by ssl context)
} yssl_socket_st;

#define YSSL_SOCKET yssl_socket_st*

int yTcpInitSSL(char* errmsg);
int yTcpSetSrvCertificateSSL(const char* certfile, const char* keyfile, char* errmsg);
int yTcpAddClientCertificateSSL(const u8* cert, u32 cert_len, char* errmsg);
void yTcpShutdownSSL(void);
int yTcpOpenSSL(YSSL_SOCKET* newskt, const char *host, u16 port, int skip_cert_validation, u64 mstimeout, char* errmsg);
int yTcpAcceptSSL(YSSL_SOCKET* newskt, YSOCKET sock, char* errmsg);
void yTcpCloseSSL(YSSL_SOCKET skt);
YSOCKET yTcpFdSetSSL(YSSL_SOCKET yssl, void* set, YSOCKET sktmax);
int yTcpFdIsSetSSL(YSSL_SOCKET yssl, void* set);
int yTcpCheckSocketStillValidSSL(YSSL_SOCKET skt, char* errmsg);
int yTcpWriteSSL(YSSL_SOCKET skt, const u8* buffer, int len, char* errmsg);
int yTcpReadSSL(YSSL_SOCKET skt, u8* buffer, int len, char* errmsg);
u32 yTcpGetRcvBufSizeSSL(YSSL_SOCKET skt);
int yTcpDownloadSSLCert(const char* host, int port, u64 mstimeout, u8* buffer, u32 maxsize, u32* neededsize, char* errmsg);


/*
 * some helper function that need to be called after yTcpInitSSL()
 */
int yssl_generate_private_key(const char* keyfile, u32 nbits, char* errmsg);
int yssl_write_certificate(void* crt, const char* certfilename, char* errmsg);
int yssl_generate_certificate(const char* keyfile, const char* certfile,
                              const char* country, const char* state,
                              const char* organisation, const char* common_name,
                              time_t expiration, char* errmsg);

#else
typedef u32 YSSL_SOCKET;
#endif
#ifdef  __cplusplus
}
#endif

#endif
