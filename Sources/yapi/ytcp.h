/*********************************************************************
 *
 * $Id: ytcp.h 10311 2013-03-14 13:18:58Z mvuilleu $
 *
 *  Declaration of a client TCP stack
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
#include <winsock2.h>
#include <ws2tcpip.h>
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

#if 0
#define yTCPLOG(txt, ... )    printf(txt"\n",__VA_ARGS__)
#else
#ifdef __BORLANDC__
#define yTCPLOG notcplog
#else
#define yTCPLOG(txt,...)
#endif
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
#ifdef  __cplusplus
}
#endif
#endif
