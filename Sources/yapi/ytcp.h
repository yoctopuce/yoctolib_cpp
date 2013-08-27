/*********************************************************************
 *
 * $Id: ytcp.h 12461 2013-08-22 08:58:05Z seb $
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
    
#include "ydef.h"


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
#define REPORT_ERR(msg) if(errmsg){ YSPRINTF(errmsg,YOCTO_ERRMSG_LEN,"%s:errno=%d",(msg),SOCK_ERR);errmsg[YOCTO_ERRMSG_LEN-1]='\0';}
    
#define yNetSetErr()  yNetSetErrEx(__LINE__,SOCK_ERR,errmsg)
    
int yNetSetErrEx(u32 line,unsigned err,char *errmsg);
    

// A TCP_SOCKET is stored as a single BYTE
#define YTCP_REMOTE_CLOSE 1

struct _NetHubSt;    
struct _TcpReqSt;

typedef struct {
    YSOCKET listensock;
    YSOCKET signalsock;
} WakeUpSocket;

void yDupSet(char **storage, const char *val);
void yInitWakeUpSocket(WakeUpSocket *wuce);
int  yStartWakeUpSocket(WakeUpSocket *wuce, char *errmsg);
int  yDringWakeUpSocket(WakeUpSocket *wuce, u8 signal, char *errmsg);
int  yConsumeWakeUpSocket(WakeUpSocket *wuce, char *errmsg);
void yFreeWakeUpSocket(WakeUpSocket *wuce);
    
int  yTcpInit(char *errmsg);
u32  yResolveDNS(const char *name,char *errmsg);
void yTcpInitReq(struct _TcpReqSt *tcpreq, struct _NetHubSt *hub);
int  yTcpOpenReq(struct _TcpReqSt *tcpreq, const char *request, int reqlen, int isAsync, char *errmsg);
int  yTcpSelectReq(struct _TcpReqSt **tcpreq, int size, u64 ms, WakeUpSocket *wuce, char *errmsg);
int  yTcpEofReq(struct _TcpReqSt *tcpreq, char *errmsg);
int  yTcpGetReq(struct _TcpReqSt *tcpreq, char **buffer);
int  yTcpReadReq(struct _TcpReqSt *rcoreq, char *buffer, int len);
void yTcpCloseReq(struct _TcpReqSt *tcpreq);
void yTcpFreeReq(struct _TcpReqSt *tcpreq);
void yTcpShutdown(void);

//#define Y_UPNP_DETECT
#ifdef Y_UPNP_DETECT
#include "ythread.h"

#define UPNP_PORT  1900
#define UPNP_MCAST_ADDR "239.255.255.250"
#define SSDP_URN_YOCTOPUCE "urn:yoctopuce-com:device:hub:1"


typedef struct {
    YSOCKET request_sock;
    YSOCKET notify_sock;
    yThread thread;
} UPNPSocket;

int		yUPNPStart(UPNPSocket *upnp, char *errmsg);
int		yUPNPDiscover(UPNPSocket *upnp, char *errmsg);
void	yUPNPStop(UPNPSocket *upnp);

#endif
#ifdef  __cplusplus
}
#endif
#endif
