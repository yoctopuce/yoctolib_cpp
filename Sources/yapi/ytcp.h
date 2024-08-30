/*********************************************************************
 *
 * $Id: ytcp.h 62146 2024-08-13 07:52:22Z seb $
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


#ifndef YTCP_H
#define YTCP_H


#ifdef  __cplusplus
extern "C" {
#endif

#include "ydef_private.h"


#ifdef WINDOWS_API
/**************************************************************
 *
 *  WINDOWS IMPLEMENTATION
 *
 **************************************************************/

//SOCKET RELATED DEFIITIONS AND INCLUDE
#else
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define closesocket(s) close(s)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#endif

#ifdef MSG_NOSIGNAL
#define SEND_NOSIGPIPE MSG_NOSIGNAL
#else
#define SEND_NOSIGPIPE 0
#endif

#ifdef WINDOWS_API
#define SOCK_ERR    (WSAGetLastError())
#else
#define SOCK_ERR    (errno)
#endif
#define REPORT_ERR(msg) if(errmsg){ YSPRINTF(errmsg,YOCTO_ERRMSG_LEN,"%s (%s:%d / errno=%d)",(msg), __FILENAME__, __LINE__, SOCK_ERR);errmsg[YOCTO_ERRMSG_LEN-1]='\0';}

#define yNetSetErr()  yNetSetErrEx(__FILENAME__, __LINE__,SOCK_ERR,errmsg)

int yNetSetErrEx(const char* fileid, u32 line, unsigned err, char* errmsg);

#define DEFAULT_HTTPS_PORT 443
#define DEFAULT_HTTP_PORT 80

#define YTCP_REMOTE_CLOSE 1

struct _HubSt;
struct _RequestSt;

typedef struct {
    YSOCKET listensock;
    YSOCKET signalsock;
} WakeUpSocket;

void yDupSet(char** storage, const char* val);
void yInitWakeUpSocket(WakeUpSocket* wuce);
int yStartWakeUpSocket(WakeUpSocket* wuce, char* errmsg);
int yDringWakeUpSocket(WakeUpSocket* wuce, u8 signal, char* errmsg);
int yConsumeWakeUpSocket(WakeUpSocket* wuce, char* errmsg);
void yFreeWakeUpSocket(WakeUpSocket* wuce);
int yTcpDownload(const char* host, int port, int usessl, const char* url, u8** out_buffer, u32 mstimeout, char* errmsg);

int yResolveDNS(const char* name, IPvX_ADDR* addr, char* errmsg);

int yTcpOpenBasic(YSOCKET* newskt, const char* host, u16 port, u64 mstimeout, char* errmsg);
int yTcpOpenBasicEx(YSOCKET* newskt, const IPvX_ADDR* ip, u16 port, u64 mstimeout, char* errmsg);
void yTcpCloseBasic(YSOCKET skt);
int yTcpReadBasic(YSOCKET skt, u8* buffer, int len, char* errmsg);
u32 yTcpGetRcvBufSizeBasic(YSOCKET skt);
int yTcpWriteBasic(YSOCKET skt, const u8* buffer, int len, char* errmsg);
int yTcpCheckSocketStillValidBasic(YSOCKET skt, char* errmsg);

#include "yssl.h"
//#define DUMP_YSOCKET_MULTI_TRAFFIC


#define YSOCKFLAG_secure 1
#define YSOCKFLAG_IPV6 2
#define YSOCKFLAG_SO_BROADCAST 4
#define YSOCKFLAG_SO_REUSEPORT 5
typedef struct {
    u16 secure_socket:1;
    u16 ipv6:1;
    union {
        YSOCKET basic;
        YSSL_SOCKET secure;
    };
#ifdef DUMP_YSOCKET_MULTI_TRAFFIC
    u64 creation_tm; 
#endif
} YSOCKET_MULTI_ST, *YSOCKET_MULTI;

#define INVALID_SOCKET_MULTI NULL


int yTcpInitMulti(char* errmsg);
int yTcpOpenMulti(YSOCKET_MULTI* newskt, const char* host, u16 port, int useSSL, u64 mstimeout, char* errmsg);
int yTcpAcceptMulti(YSOCKET_MULTI* newskt, YSOCKET sock, int useSSL, char* errmsg);
void yTcpCloseMulti(YSOCKET_MULTI skt);
YSOCKET yTcpFdSetMulti(YSOCKET_MULTI skt, void* set, YSOCKET sktmax);
int yTcpFdIsSetMulti(YSOCKET_MULTI skt, void* set);
int yTcpCheckSocketStillValidMulti(YSOCKET_MULTI skt, char* errmsg);
int yTcpReadMulti(YSOCKET_MULTI skt, u8* buffer, int len, char* errmsg);
u32 yTcpGetRcvBufSizeMulti(YSOCKET_MULTI sock);
int yTcpWriteMulti(YSOCKET_MULTI skt, const u8* buffer, int len, char* errmsg);
void yTcpShutdownMulti(void);

int yUdpOpenMulti(YSOCKET_MULTI *newskt, IPvX_ADDR *local_ip, int sin6_scope_id, u16 port, u16 sockFlags, char *errmsg);
int yUdpWriteMulti(YSOCKET_MULTI skt, IPvX_ADDR* dest_ip, u16 dest_port, const u8* buffer, int len, char* errmsg);
int yUdpReadMulti(YSOCKET_MULTI skt, u8* buffer, int len, IPvX_ADDR* dest_ip, u16* dest_port, char* errmsg);
int yUdpRegisterMCAST(YSOCKET_MULTI skt,  IPvX_ADDR *mcastAddr, int interfaceNo);

struct _RequestSt* yReqAlloc(struct _HubSt* hub);
int yReqOpen(struct _RequestSt* tcpreq, int wait_for_start, int tcpchan, const char* request, int reqlen, u64 mstimeout, yapiRequestAsyncCallback callback, void* context, yapiRequestProgressCallback progress_cb, void* progress_ctx, char* errmsg);
int yReqIsAsync(struct _RequestSt* req);
int yReqSelect(struct _RequestSt* tcpreq, u64 ms, char* errmsg);
int yReqMultiSelect(struct _RequestSt** tcpreq, int size, u64 ms, WakeUpSocket* wuce, char* errmsg);
int yReqIsEof(struct _RequestSt* tcpreq, char* errmsg);
int yReqGet(struct _RequestSt* tcpreq, u8** buffer);
int yReqRead(struct _RequestSt* rcoreq, u8* buffer, int len);
void yReqClose(struct _RequestSt* tcpreq);
void yReqFree(struct _RequestSt* tcpreq);
int yReqHasPending(struct _HubSt* hub);


void* ws_thread(void* ctx);
void ws_cleanup(struct _HubSt* basehub);

#include "ythread.h"

#define OS_IFACE_CAN_MCAST 1

#ifdef YAPI_IN_YDEVICE
int yDetectNetworkInterfaces(IPvX_ADDR *only_ip, os_ifaces *interfaces, int max_nb_interfaces);
#endif

#define SSDP_UUID_LEN   48
#define SSDP_URL_LEN    48

typedef struct {
    char serial[YOCTO_SERIAL_LEN];
    char uuid[SSDP_UUID_LEN];
    char url[SSDP_URL_LEN];
    u64 detectedTime;
    u64 maxAge;
} SSDP_CACHE_ENTRY;


// prototype of the ssdp hub discovery callback
// will be called on discover, refresh, and expiration
typedef void (*ssdpHubDiscoveryCallback)(const char* serial, const char* urlToRegister, const char* urlToUnregister);

#define NB_SSDP_CACHE_ENTRY 128
#define NB_OS_IFACES 8


typedef struct {
    int started;
    ssdpHubDiscoveryCallback callback;
    YSOCKET request_sock[NB_OS_IFACES];
    YSOCKET notify_sock[NB_OS_IFACES];
    yThread thread;
    SSDP_CACHE_ENTRY* SSDPCache[NB_SSDP_CACHE_ENTRY];
} SSDPInfos;

int ySSDPStart(SSDPInfos* SSDP, ssdpHubDiscoveryCallback callback, char* errmsg);
int ySSDPDiscover(SSDPInfos* SSDP, char* errmsg);
void ySSDPStop(SSDPInfos* SSDP);

#ifdef  __cplusplus
}
#endif
#endif
