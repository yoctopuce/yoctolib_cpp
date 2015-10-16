/*********************************************************************
 *
 * $Id: ytcp.c 21762 2015-10-15 11:55:52Z seb $
 *
 * Implementation of a client TCP stack
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

#define __FILE_ID__  "ytcp"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "ydef.h"
#if defined(WINDOWS_API) && !defined(_MSC_VER)
#define _WIN32_WINNT 0x501
#endif
#ifdef WINDOWS_API
typedef int socklen_t;
#if defined(__BORLANDC__)
#pragma warn -8004
#pragma warn -8019
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma warn +8004
#pragma warn +8019
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#endif
#include "ytcp.h"
#include "yproto.h"
#include "yhash.h"

#ifdef WIN32
    #ifndef WINCE
        #include <iphlpapi.h>
        #if defined(_MSC_VER) || defined (__BORLANDC__)
            #pragma comment(lib, "Ws2_32.lib")
        #endif
    #else
        #pragma comment(lib, "Ws2.lib")
    #endif
#else
    #include <unistd.h>
    #include <fcntl.h>
    #include <netdb.h>
#endif


//#define DEBUG_SLOW_TCP
//#define TRACE_TCP_REQ

//#define PERF_TCP_FUNCTIONS
#ifdef PERF_TCP_FUNCTIONS


typedef struct {
    yPerfMon  TCPOpen_socket;
    yPerfMon  TCPOpen_connect;
    yPerfMon  TCPOpen_setsockopt_noblock;
    yPerfMon  TCPOpen_setsockopt_nodelay;
    yPerfMon  TCPOpenReq_wait;
    yPerfMon  TCPOpenReq;
    yPerfMon  tmp1;
    yPerfMon  tmp2;
    yPerfMon  tmp3;
    yPerfMon  tmp4;
} yTcpPerfMonSt;

yTcpPerfMonSt yTcpPerf;


#define YPERF_TCP_ENTER(NAME) {yTcpPerf.NAME.count++;yTcpPerf.NAME.tmp=yapiGetTickCount();}
#define YPERF_TCP_LEAVE(NAME) {yTcpPerf.NAME.leave++;yTcpPerf.NAME.totaltime += yapiGetTickCount()- yTcpPerf.NAME.tmp;}


void dumpYTcpPerf(void)
{
    dumpYPerfEntry(&yTcpPerf.TCPOpen_socket,"TCPOpen:socket");
    dumpYPerfEntry(&yTcpPerf.TCPOpen_connect,"TCPOpen:connect");
    dumpYPerfEntry(&yTcpPerf.TCPOpen_setsockopt_noblock,"TCPOpen:sockopt_noblock");
    dumpYPerfEntry(&yTcpPerf.TCPOpen_setsockopt_nodelay,"TCPOpen:sockopt_nodelay");
    dumpYPerfEntry(&yTcpPerf.TCPOpenReq_wait,"TCPOpenReq:wait");
    dumpYPerfEntry(&yTcpPerf.TCPOpenReq,"TCPOpenReq");
    dumpYPerfEntry(&yTcpPerf.tmp1,"TCP:tmp1");
    dumpYPerfEntry(&yTcpPerf.tmp2,"TCP:tmp2");
    dumpYPerfEntry(&yTcpPerf.tmp3,"TCP:tmp3");
    dumpYPerfEntry(&yTcpPerf.tmp4,"TCP:tmp4");
}
#else
#define YPERF_TCP_ENTER(NAME)
#define YPERF_TCP_LEAVE(NAME)
#endif



void  yDupSet(char **storage, const char *val)
{
    int  len = (val ? (int)strlen(val)+1 : 1);

    if(*storage) yFree(*storage);
    *storage = (char*) yMalloc(len);
    if(val) {
        memcpy(*storage, val, len);
    } else {
        **storage = 0;
    }
}

int yNetSetErrEx(u32 line,unsigned err,char *errmsg)
{
    int len;
    if(errmsg==NULL)
        return YAPI_IO_ERROR;
    YSPRINTF(errmsg,YOCTO_ERRMSG_LEN,"%s:%d:tcp(%d):",__FILE_ID__,line,err);
#if defined(WINDOWS_API) && !defined(WINCE)
    len=(int)strlen(errmsg);
    FormatMessageA (
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR) (errmsg+len),
                    YOCTO_ERRMSG_LEN-len, NULL );
#else
    len=YSTRLEN(errmsg);
    strcpy(errmsg+len, strerror((int)err));
#endif
    return YAPI_IO_ERROR;
}
#if 1
#define yNetLogErr()  yNetLogErrEx(__LINE__,SOCK_ERR)
static int yNetLogErrEx(u32 line,unsigned err)
{
    int retval;
    char errmsg[YOCTO_ERRMSG_LEN];
    retval = yNetSetErrEx(line,err,errmsg);
    dbglog("%s",errmsg);
    return retval;
}
#endif

void yInitWakeUpSocket(WakeUpSocket *wuce)
{
    wuce->listensock = INVALID_SOCKET;
    wuce->signalsock = INVALID_SOCKET;
}


int yStartWakeUpSocket(WakeUpSocket *wuce, char *errmsg)
{
    u32     optval;
    socklen_t     localh_size;
    struct  sockaddr_in     localh;

    if(wuce->listensock != INVALID_SOCKET || wuce->signalsock != INVALID_SOCKET) {
        return YERRMSG(YAPI_INVALID_ARGUMENT,"WakeUpSocket allready Started");
    }
    //create socket
    wuce->listensock = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (wuce->listensock == INVALID_SOCKET) {
        return yNetSetErr();
    }
    optval = 1;
    setsockopt(wuce->listensock,SOL_SOCKET,SO_REUSEADDR,(char *)&optval,sizeof(optval));

    localh_size=sizeof(localh);
    // set port to 0 since we accept any port
    memset(&localh,0,localh_size);
    localh.sin_family = AF_INET;
    localh.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (bind(wuce->listensock,(struct sockaddr *)&localh,localh_size)<0) {
        return yNetSetErr();
    }
    if (getsockname(wuce->listensock,(struct sockaddr *)&localh,&localh_size)<0) {
        return yNetSetErr();
    }
    wuce->signalsock = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (wuce->signalsock == INVALID_SOCKET) {
        return yNetSetErr();
    }
    if (connect(wuce->signalsock,(struct sockaddr *)&localh,localh_size)<0) {
        return yNetSetErr();
    }
    return YAPI_SUCCESS;
}

int yDringWakeUpSocket(WakeUpSocket *wuce, u8 signal, char *errmsg)
{
    if(send(wuce->signalsock,(char*)&signal,1,SEND_NOSIGPIPE) < 0) {
        return yNetSetErr();
    }
    return YAPI_SUCCESS;
}

int yConsumeWakeUpSocket(WakeUpSocket *wuce, char *errmsg)
{
    u8 signal = 0;

    if(recv(wuce->listensock,(char*)&signal,1,0) < 0) {
        return yNetSetErr();
    }
    return signal;
}

void yFreeWakeUpSocket(WakeUpSocket *wuce)
{
    if ( wuce->listensock != INVALID_SOCKET) {
        closesocket(wuce->listensock);
        wuce->listensock = INVALID_SOCKET;
    }
    if ( wuce->signalsock != INVALID_SOCKET) {
        closesocket(wuce->signalsock);
        wuce->signalsock = INVALID_SOCKET;
    }
}




u32 yResolveDNS(const char *name,char *errmsg)
{
    u32 ipv4=0;

    struct addrinfo *infos,*p;
    if(getaddrinfo(name,NULL,NULL,&infos)!=0){
        REPORT_ERR("Unable to resolve hostname");
        return 0;
    }

    // Retrieve each address and print out the hex bytes
    for(p=infos; p != NULL ; p=p->ai_next) {
        if (p->ai_family == AF_INET){
            ipv4 = ((struct sockaddr_in *) p->ai_addr)->sin_addr.s_addr;
            break;
        }
    }
    freeaddrinfo(infos);
    return ipv4;
}

static int yTcpOpen(YSOCKET *newskt, u32 ip, u16 port, u64 mstimeout, char *errmsg)
{
    struct sockaddr_in clientService;
    int iResult;
    u_long flags;
    YSOCKET skt;
    fd_set      readfds, writefds, exceptfds;
    struct timeval timeout;
#ifdef WINDOWS_API
    char noDelay=1;
#else
    int  noDelay=1;
#ifdef SO_NOSIGPIPE
    int  noSigpipe=1;
#endif
#endif

    TCPLOG("yTcpOpenReqEx %p [dst=%x:%d %dms]\n", newskt, ip, port, mstimeout);


    YPERF_TCP_ENTER(TCPOpen_socket);
    *newskt = INVALID_SOCKET;
    skt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    YPERF_TCP_LEAVE(TCPOpen_socket);
    if (skt == INVALID_SOCKET) {
        REPORT_ERR("Error at socket()");
        return YAPI_IO_ERROR;
    }
    //dbglog("ytcpOpen %X:%x: skt= %x\n",ip,port,skt);
    YPERF_TCP_ENTER(TCPOpen_connect);
    memset(&clientService, 0, sizeof(clientService));
    //----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port of the server to be connected to.
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = ip;
    clientService.sin_port = htons( port );

    //----------------------
    // Connect to server.
    YPERF_TCP_ENTER(TCPOpen_setsockopt_noblock);
    //set socket as non blocking
#ifdef WINDOWS_API
    flags = 1;
    ioctlsocket(skt, FIONBIO, &flags);
#else
    flags = fcntl(skt, F_GETFL, 0);
    fcntl(skt, F_SETFL, flags | O_NONBLOCK);
#ifdef SO_NOSIGPIPE
    setsockopt(skt, SOL_SOCKET, SO_NOSIGPIPE, (void *)&noSigpipe, sizeof(int));
#endif
#endif
    YPERF_TCP_LEAVE(TCPOpen_setsockopt_noblock);
    iResult = connect(skt, ( struct sockaddr *) &clientService, sizeof(clientService) );

    // wait for the connection with a select
    memset(&timeout, 0, sizeof(timeout));
    if (mstimeout != 0) {
        u64 nbsec = mstimeout / 1000;
        timeout.tv_sec = (long)nbsec;
        timeout.tv_usec = ((int) (mstimeout - (nbsec * 1000))) * 1000;
    } else {
        timeout.tv_sec = 20;
    }
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    FD_SET(skt, &readfds);
    FD_SET(skt, &writefds);
    FD_SET(skt, &exceptfds);
    iResult = select((int)skt + 1, &readfds, &writefds, &exceptfds, &timeout);
    if (iResult < 0) {
        REPORT_ERR("Unable to connect to server");
        closesocket(skt);
        return YAPI_IO_ERROR;
    }
    if (FD_ISSET(skt, &exceptfds)) {
        closesocket(skt);
        return YERRMSG(YAPI_IO_ERROR, "Unable to connect to server");
    }
    if (!FD_ISSET(skt, &writefds)) {
        closesocket(skt);
        return YERRMSG(YAPI_IO_ERROR, "Unable to connect to server");
    }
    YPERF_TCP_LEAVE(TCPOpen_connect);
    if ( iResult == SOCKET_ERROR) {
        REPORT_ERR("Unable to connect to server");
        closesocket (skt);
        return YAPI_IO_ERROR;
    }
    YPERF_TCP_ENTER(TCPOpen_setsockopt_nodelay);
    if(setsockopt(skt, IPPROTO_TCP, TCP_NODELAY, &noDelay, sizeof(noDelay)) < 0) {
#if 0
        switch(errno) {
            case EBADF:
                dbglog("The argument sockfd is not a valid descriptor.\n");
                break;
            case EFAULT:
                dbglog("The address pointed to by optval is not in a valid part of the process address space. For getsockopt(), "
                        "this error may also be returned if optlen is not in a valid part of the process address space.\n");
                break;
            case EINVAL:
                dbglog("optlen invalid in setsockopt(). In some cases this error can also occur for an invalid value in optval "
                       "(e.g., for the IP_ADD_MEMBERSHIP option described in ip(7)).\n");
                break;
            case ENOPROTOOPT:
                dbglog("The option is unknown at the level indicated.\n");
                break;
            case ENOTSOCK:
                dbglog("The argument sockfd is a file, not a socket.\n");
                break;
        }
#endif
        dbglog("SetSockOpt TCP_NODELAY failed %d\n",errno);
    }
    YPERF_TCP_LEAVE(TCPOpen_setsockopt_nodelay);
    *newskt = skt;
    return YAPI_SUCCESS;
}

static void yTcpClose(YSOCKET skt)
{
    // cleanup
    closesocket(skt);
}


// check it a socket is still valid and empty (ie: nothing to read and writable)
// return 1 if the socket is valid or a error code
static int yTcpCheckSocketStillValid(YSOCKET skt, char * errmsg)
{
    int iResult, res;
    fd_set      readfds,writefds,exceptfds;
    struct timeval timeout;

    // Send an initial buffer
#ifndef WINDOWS_API
retry:
#endif
    memset(&timeout,0,sizeof(timeout));
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    FD_SET(skt,&readfds);
    FD_SET(skt,&writefds);
    FD_SET(skt,&exceptfds);
    res = select((int)skt+1,&readfds,&writefds,&exceptfds,&timeout);
    if (res<0) {
#ifndef WINDOWS_API
        if(SOCK_ERR ==  EAGAIN){
            goto retry;
        } else
#endif
        {
            res = yNetSetErr();
            yTcpClose(skt);
            return res;
        }
    }
    if (FD_ISSET(skt,&exceptfds)) {
        yTcpClose(skt);
        return YERRMSG(YAPI_IO_ERROR, "Exception on socket");
    }
    if (!FD_ISSET(skt,&writefds)) {
        yTcpClose(skt);
        return YERRMSG(YAPI_IO_ERROR, "Socket not ready for write");
    }

    if (FD_ISSET(skt,&readfds)) {
        char buffer[128];
        iResult = (int)recv(skt, buffer, sizeof(buffer), 0);
        if (iResult == 0) {
            yTcpClose(skt);
            return YERR(YAPI_NO_MORE_DATA);
        } if ( iResult < 0 ){
            yTcpClose(skt);
            return YERR(YAPI_IO_ERROR);
        } else {
            yTcpClose(skt);
            return YERR(YAPI_DOUBLE_ACCES);
        }
    }
    return 1;
}


static int yTcpWrite(YSOCKET skt, const char *buffer, int len,char *errmsg)
{
    int res;
    int tosend = len;
    const char * p = buffer;

    while (tosend>0) {
        res = (int) send(skt, p, tosend, SEND_NOSIGPIPE);
        if (res == SOCKET_ERROR) {
#ifdef WINDOWS_API
            if(SOCK_ERR != WSAEWOULDBLOCK)
#else
            if(SOCK_ERR != EAGAIN)

#endif
            {
                return yNetSetErr();
            }

        } else {
            tosend -= res;
            p += res;
            // unable to send all data
            // wait a bit with a select
            if (tosend != res) {
                struct timeval timeout;
                fd_set      fds;
                memset(&timeout,0,sizeof(timeout));
                // Upload of large files (external firmware updates) may need
                // a long time to process (on OSX: seen more than 40 seconds !)
                timeout.tv_sec = 60;
                FD_ZERO(&fds);
                FD_SET(skt,&fds);
                res = select((int)skt+1,NULL,&fds,NULL,&timeout);
                if (res<0) {
#ifndef WINDOWS_API
                    if(SOCK_ERR ==  EAGAIN){
                        continue;
                    } else
#endif
                    {
                        return yNetSetErr();
                    }
                } else if (res == 0) {
                    return YERRMSG(YAPI_TIMEOUT, "Timeout during TCP write");
                }
            }
        }
    }
    return len;
}


static int yTcpRead(YSOCKET skt, u8 *buffer, int len,char *errmsg)
{
    int iResult = (int)recv(skt, (char*)buffer, len, 0);

    if (iResult == 0){
        return YAPI_NO_MORE_DATA;
    }else if ( iResult < 0 ){
#ifdef WINDOWS_API
        if(SOCK_ERR == WSAEWOULDBLOCK){
            return 0;
        }
#else
        if(SOCK_ERR == EAGAIN){
            return 0;
        }
#endif
        REPORT_ERR("read failed");
        return YAPI_IO_ERROR;
    }
    return iResult;
}


int yTcpDownload(const char *host, const char *url, u8 **out_buffer, u32 mstimeout, char *errmsg)
{
    YSOCKET skt;
    u32 ip;
    int res,len,readed;
    char request[512];
    u8      *replybuf = yMalloc(512);
    int     replybufsize = 512;
    int     replysize = 0;
    fd_set      fds;
    u64 expiration;

    ip = yResolveDNS(host, errmsg);
    if (ip==0) {
        yFree(replybuf);
        return YAPI_IO_ERROR;
    }
    expiration = yapiGetTickCount() + mstimeout;
    if (yTcpOpen(&skt, ip, 80, mstimeout, errmsg)<0) {
        yTcpClose(skt);
        yFree(replybuf);
        return YAPI_IO_ERROR;
    }
    len = YSPRINTF(request,512,"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n"
        "Accept-Encoding:\r\nUser-Agent: Yoctopuce\r\n\r\n",url,host);
    //write header
    res = yTcpWrite(skt, request, len, errmsg);
    if(YISERR(res)) {
        goto exit;
    }
    while(expiration - yapiGetTickCount() > 0) {
        struct timeval timeout;
        u64 ms = expiration - yapiGetTickCount();
        memset(&timeout,0,sizeof(timeout));
        timeout.tv_sec = (long)ms/1000;
        timeout.tv_usec = (int)(ms % 1000) *1000;
        /* wait for data */
        FD_ZERO(&fds);
        FD_SET(skt,&fds);
        res = select((int)skt+1,&fds,NULL,NULL,&timeout);
        if (res<0) {
    #ifndef WINDOWS_API
            if(SOCK_ERR ==  EAGAIN){
                continue;
            } else
    #endif
            {
                res = yNetSetErr();
                goto exit;
            }
        }
        if(replysize + 256 >= replybufsize) {
            // need to grow receive buffer
            int  newsize = replybufsize << 1;
            u8 *newbuf = (u8*) yMalloc(newsize);
            if (replybuf) {
                memcpy(newbuf, replybuf, replysize);
                yFree(replybuf);
            }
            replybuf = newbuf;
            replybufsize = newsize;
        }
        readed = yTcpRead(skt, replybuf + replysize, replybufsize - replysize, errmsg);
        if(readed < 0) {
            // any connection closed by peer ends up with YAPI_NO_MORE_DATA
            if (readed == YAPI_NO_MORE_DATA) {
                res = replysize;
            } else {
                res = readed;
            }
            goto exit;
        } else {
            replysize += readed;
        }
    }
    res = YERR(YAPI_TIMEOUT);

exit:
    yTcpClose(skt);
    if (res < 0) {
        yFree(replybuf);
    } else {
        *out_buffer = replybuf;
    }
    return res;
}

#ifdef TRACE_TCP_REQ

static void dumpTCPReq(const char *fileid, int lineno, struct _TcpReqSt *req)
{

    int w;
    dbglog("dump TCPReq %p from %s:%d\n", req, fileid, lineno);
    if (req->hub){
        dbglog("Hub: %s\n", req->hub->name);
    } else{
        dbglog("Hub: null\n");
    }

    dbglog("state retcode=%d (retrycount=%d) errmsg=%s\n", req->errcode, req->retryCount, req->errmsg);
    dbglog("socket=%x (reuse=%x) flags=%x\n", req->skt, req->reuseskt, req->flags);
    dbglog("time open=%"FMTx64" last read=%"FMTx64"  timeout=%"FMTx64"\n", req->open_tm, req->read_tm, req->timeout_tm);
    dbglog("readed=%d (readpos=%d)\n", req->replysize, req->replysize);
    dbglog("callback=%p context=%p\n", req->callback, req->context);
    if (req->headerbuf){
        dbglog("req[%s]\n", req->headerbuf);
    } else {
        dbglog("null\n");
    }
    w = yWaitForEvent(&req->finished, 1);
    dbglog("finished=%d\n", w);
}
#endif
void yTcpInitReq(struct _TcpReqSt *req, struct _NetHubSt *hub)
{
    memset(req, 0, sizeof(struct _TcpReqSt));
    TCPLOG("yTcpInitReq %p[%x]\n",req,req->skt);
    req->reuseskt = INVALID_SOCKET;
    req->replybufsize = 1500;
    req->replybuf = (u8*) yMalloc(req->replybufsize);
    yInitializeCriticalSection(&req->access);
    yCreateManualEvent(&req->finished,1);
    req->hub = hub;
    req->skt = INVALID_SOCKET;
 }


#define YDNS_CACHE_SIZE 16
#define YDNS_CACHE_VALIDITY 600000u //10 minutes
typedef struct {
    yUrlRef url;
    u32     ip;
    u64     time;
} DnsCache;

DnsCache dnsCache[YDNS_CACHE_SIZE];


static u32 resolveDNSCache(yUrlRef url, char *errmsg)
{
    int i,firstFree=-1;
    char buffer[YOCTO_HOSTNAME_NAME];
    u16  port;
    u32  ip;

    for (i=0;i<YDNS_CACHE_SIZE;i++) {
        if (dnsCache[i].url ==url) {
            break;
        }
        if(firstFree <0 && dnsCache[i].url == INVALID_HASH_IDX){
            firstFree = i;
        }
    }
    if (i< YDNS_CACHE_SIZE) {
        if ( (u64) (yapiGetTickCount() - dnsCache[i].time) <= YDNS_CACHE_VALIDITY ){
            return dnsCache[i].ip;
        }
        firstFree=i;
    }
    yHashGetUrlPort(url, buffer,&port);
    ip =yResolveDNS(buffer,errmsg);
    if (ip != 0 && firstFree < YDNS_CACHE_SIZE) {
        dnsCache[firstFree].url  = url;
        dnsCache[firstFree].ip   = ip;
        dnsCache[firstFree].time = yapiGetTickCount();
    }
    return ip;
}


static int yTcpCheckReqTimeout(struct _TcpReqSt *req, char *errmsg)
{
    if (req->timeout_tm != 0) {
        u64 now = yapiGetTickCount();
        u64 idle_durration = now - req->read_tm;
        u64 duration = now - req->open_tm;
        if (idle_durration < YIO_IDLE_TCP_TIMEOUT) {
            return  YAPI_SUCCESS;
        }
#ifdef DEBUG_SLOW_TCP
        else {
            dbglog("Long Idle TCP request %p = %dms (total = %dms)\n", req, (u32)idle_durration, duration);
        }
#endif
        if (duration > req->timeout_tm) {
            req->errcode = YAPI_TIMEOUT;
            YSPRINTF(req->errmsg, YOCTO_ERRMSG_LEN , "TCP request took too long (%dms)",duration);
            return YERRMSG(YAPI_TIMEOUT, req->errmsg);
        }
#ifdef DEBUG_SLOW_TCP
        else {
            if (duration > (req->timeout_tm - (req->timeout_tm / 4))) {
                dbglog("Slow TCP request %p = %dms\n",req,duration);
                dbglog("req = %s\n",req->headerbuf);
            }
        }
#endif
    }
    return YAPI_SUCCESS;
}
// access mutex taken by caller
static int yTcpOpenReqEx(struct _TcpReqSt *req, u64 mstimout, char *errmsg)
{
    char buffer[YOCTO_HOSTNAME_NAME], *p,*last,*end;
    u32  ip;
    u16  port;
    int  res;
    u64  start_process = yapiGetTickCount();

    switch(yHashGetUrlPort(req->hub->url, buffer,&port)){
        case NAME_URL:
            ip = resolveDNSCache(req->hub->url,errmsg);
            if(ip==0){
                YPERF_TCP_LEAVE(tmp1);
                return YAPI_IO_ERROR;
            }
            break;
        case IP_URL:
            ip = inet_addr(buffer);
            break;
        default:
            res = YERRMSG(YAPI_IO_ERROR,"not an IP hub");
            req->skt = INVALID_SOCKET;
            TCPLOG("yTcpOpenReqEx error%p[%x]\n",req,req->skt);
            return res;
    }
    TCPLOG("yTcpOpenReqEx %p [%x:%x %d]\n", req, req->skt, req->reuseskt, mstimout);
    req->replypos = -1; // not ready to consume until header found
    req->replysize = 0;
    memset(req->replybuf,0,req->replybufsize);
    req->errcode = YAPI_SUCCESS;

    if(req->reuseskt != INVALID_SOCKET && (res = yTcpCheckSocketStillValid(req->reuseskt,NULL)) ==1) {
        req->skt = req->reuseskt;
        req->reuseskt = INVALID_SOCKET;
    } else {
        req->reuseskt = INVALID_SOCKET;
        res = yTcpOpen(&req->skt, ip, port, mstimout, errmsg);
        if(YISERR(res)) {
            // yTcpOpen has reset the socket to INVALID
            yTcpClose(req->skt);
            req->skt = INVALID_SOCKET;
            TCPLOG("yTcpOpenReqEx error %p [%x]\n",req, req->skt);
            return res;
        }
    }
    req->open_tm = start_process;
    req->timeout_tm = mstimout;
    req->read_tm = yapiGetTickCount();
    p = req->headerbuf;
    //skip first line
    while(*p && *p != '\r') p++;
    end=p;
    last=p;

    while(*p == '\r' && *(p+1)=='\n' && *(p+2)!='\r') {
        p+=2;
        while(*p && *p != '\r') p++;
        if (YSTRNCMP(last,"\r\nContent-Type",strlen("\r\nContent-Type"))==0){
            unsigned len = (unsigned)(p -  last);
            if(last != end){
                memcpy(end,last,len);
            }
            end += len;
        }
        last = p;
    }
    *end++ = '\r'; *end++ = '\n';
    // insert authorization header in needed
    yEnterCriticalSection(&req->hub->authAccess);
    if(req->hub->user && req->hub->realm) {
        char *method = req->headerbuf, *uri;
        char *auth = end;
        // null-terminate method and uri for digest computation
        for(uri = method; *uri != ' '; uri++);
        *uri++ = 0;
        for(p = uri; *p != ' '; p++);
        *p = 0;
        yDigestAuthorization(auth, (int)(req->headerbuf + req->headerbufsize - auth), req->hub->user, req->hub->realm, req->hub->ha1,
                             req->hub->nonce, req->hub->opaque, &req->hub->nc, method, uri);
        // restore space separator after method and uri
        *--uri = ' ';
        *p = ' ';
        // prepare to complete request
        end = auth+strlen(auth);
    }
    yLeaveCriticalSection(&req->hub->authAccess);
    if(req->flags & TCPREQ_KEEPALIVE) {
        YSTRCPY(end, (int)(req->headerbuf + req->headerbufsize - end), "\r\n");
    } else {
        YSTRCPY(end, (int)(req->headerbuf + req->headerbufsize - end), "Connection: close\r\n\r\n");
    }
    //write header
    res = yTcpWrite(req->skt, req->headerbuf, (int)strlen(req->headerbuf), errmsg);
    if(YISERR(res)) {
        yTcpClose(req->skt);
        req->skt = INVALID_SOCKET;
        return res;
    }
    if(req->bodysize > 0){
        //write body
        res = yTcpWrite(req->skt, req->bodybuf, req->bodysize, errmsg);
        if(YISERR(res)) {
            yTcpClose(req->skt);
            req->skt = INVALID_SOCKET;
            TCPLOG("yTcpOpenReqEx write failed for Req %p[%x]\n",req,req->skt);
            return res;
        }
    }
    req->read_tm = yapiGetTickCount();

    return YAPI_SUCCESS;
}


// mutex already taken by caller

static void yTcpCloseReqEx(struct _TcpReqSt *req, int canReuseSocket)
{
    TCPLOG("yTcpCloseReqEx %p[%x]\n",req,req->skt);

    req->flags &= ~TCPREQ_KEEPALIVE;
    if (req->callback) {
        u32 len = req->replysize - req->replypos;
        u8  *ptr = req->replybuf + req->replypos;
        if (req->errcode == YAPI_NO_MORE_DATA) {
            req->callback(req->context, ptr, len, YAPI_SUCCESS, "");
        } else {
            req->callback(req->context, ptr, len, req->errcode, req->errmsg);
        }
        req->callback = NULL;
        // ASYNC Request are automaticaly released
        req->flags &= ~TCPREQ_IN_USE;
    }

    if(req->skt != INVALID_SOCKET) {
        if (canReuseSocket) {
            req->reuseskt = req->skt;
        } else {
            yTcpClose(req->skt);
        }
        req->skt = INVALID_SOCKET;
    }
    ySetEvent(&req->finished);
}


int  yTcpOpenReq(struct _TcpReqSt *req, const char *request, int reqlen, u64 mstimeout, yapiRequestAsyncCallback callback, void *context, char *errmsg)
{
    int  minlen,i,res;
    u64  startwait;

    YPERF_TCP_ENTER(TCPOpenReq_wait);
    YPERF_TCP_ENTER(TCPOpenReq);


    yWaitForEvent(&req->finished,100);
    yEnterCriticalSection(&req->access);
    //TCPLOG("yTcpOpenReq %p[%x]\n",req,req->skt);
    startwait = yapiGetTickCount();
    while(req->skt != INVALID_SOCKET || (req->flags & TCPREQ_IN_USE) ) {
        // There is an ongoing request to be finished
        yLeaveCriticalSection(&req->access);
        yWaitForEvent(&req->finished,100);
        if ((yapiGetTickCount() - startwait) > 2 * YIO_DEFAULT_TCP_TIMEOUT) {
#ifdef TRACE_TCP_REQ
            dumpTCPReq(__FILE_ID__, __LINE__, req);
#endif
            return YERRMSG(YAPI_TIMEOUT,"last TCP request is not finished");
        }
        yEnterCriticalSection(&req->access);
    }

    YPERF_TCP_LEAVE(TCPOpenReq_wait);

    req->flags = 0;
    if (request[0]=='G' && request[1]=='E' && request[2]=='T'){
        //for GET request discard all exept the first line
        for(i = 0; i < reqlen; i++) {
            if(request[i] == '\r') {
                reqlen = i;
                break;
            }
        }
        if(i > 3) {
            if(request[i-3] == '&' && request[i-2] == '.' && request[i-1] == ' ') {
                req->flags |= TCPREQ_KEEPALIVE;
            }
        }
        req->bodysize = 0;
    } else {
        const char *p = request;
        int bodylen = reqlen - 4;

        while(bodylen > 0 && (p[0] != '\r' || p[1] != '\n' ||
                              p[2] != '\r' || p[3] != '\n')) {
            p++, bodylen--;
        }
        p += 4;
        reqlen = (int)(p-request);
        // Build a request body buffer
        if(req->bodybufsize < bodylen) {
            if(req->bodybuf) yFree(req->bodybuf);
            req->bodybufsize = bodylen + (bodylen>>1);
            req->bodybuf = (char*) yMalloc(req->bodybufsize);
        }
        memcpy(req->bodybuf, p, bodylen);
        req->bodysize = bodylen;
    }
    // Build a request buffer with at least a terminal NUL but
    // include space for Connection: close and Authorization: headers
    minlen = reqlen + 400;
    if (req->headerbufsize < minlen) {
        if (req->headerbuf) yFree(req->headerbuf);
        req->headerbufsize = minlen + (reqlen>>1);
        req->headerbuf = (char*) yMalloc(req->headerbufsize);
    }
    memcpy(req->headerbuf, request, reqlen);
    req->headerbuf[reqlen] = 0;
    req->retryCount = 0;
    req->callback = callback;
    req->context = context;
    // Really build and send the request
    res = yTcpOpenReqEx(req, mstimeout, errmsg);
    if(res == YAPI_SUCCESS) {
        req->errmsg[0] = '\0';
        req->flags |= TCPREQ_IN_USE;
        yResetEvent(&req->finished);
    }
    TCPLOG("yTcpOpenReq %p[%x]\n",req,req->skt);

    yLeaveCriticalSection(&req->access);

    YPERF_TCP_LEAVE(TCPOpenReq);
    return res;
}

int yTcpSelectReq(struct _TcpReqSt **reqs, int size, u64 ms, WakeUpSocket *wuce, char *errmsg)
{
    fd_set      fds;
    struct timeval timeout;
    int         res,i;
    YSOCKET     sktmax=0;
    TcpReqSt    *req;

    memset(&timeout,0,sizeof(timeout));
    timeout.tv_sec = (long)ms/1000;
    timeout.tv_usec = (int)(ms % 1000) *1000;
    /* wait for data */
    FD_ZERO(&fds);
    if (wuce) {
        FD_SET(wuce->listensock,&fds);
        sktmax = wuce->listensock;
    }
    for (i = 0; i < size; i++) {
        req = reqs[i];
        if(req->skt == INVALID_SOCKET) {
            return YERR(YAPI_INVALID_ARGUMENT);
        } else {
            FD_SET(req->skt,&fds);
            if(req->skt > sktmax)
                sktmax = req->skt;
        }
    }
    if (sktmax == 0) {
        return YAPI_SUCCESS;
    }
    res = select((int)sktmax+1,&fds,NULL,NULL,&timeout);
    if (res<0) {
#ifndef WINDOWS_API
        if(SOCK_ERR ==  EAGAIN){
            return 0;
        } else
#endif
        {
            res = yNetSetErr();
            for (i = 0; i < size; i++) {
                TCPLOG("yTcpSelectReq %p[%X] (%s)\n",reqs[i],req[i].skt,errmsg);
            }
            return res;
        }
    }
    if (res != 0) {
        if (wuce && FD_ISSET(wuce->listensock,&fds)) {
            YPROPERR(yConsumeWakeUpSocket(wuce, errmsg));
        }
        for (i=0; i < size; i++) {
            req = reqs[i];
            if (FD_ISSET(req->skt, &fds)) {
                yEnterCriticalSection(&req->access);
                if (req->replysize >= req->replybufsize - 256) {
                    // need to grow receive buffer
                    int  newsize = req->replybufsize << 1;
                    u8 *newbuf = (u8*) yMalloc(newsize);
                    memcpy(newbuf, req->replybuf, req->replysize);
                    yFree(req->replybuf);
                    req->replybuf = newbuf;
                    req->replybufsize = newsize;
                }
                res = yTcpRead(req->skt, req->replybuf+req->replysize, req->replybufsize-req->replysize, errmsg);
                req->read_tm = yapiGetTickCount();
                if (res < 0) {
                    // any connection closed by peer ends up with YAPI_NO_MORE_DATA
                    req->replypos = 0;
                    req->errcode = YERRTO((YRETCODE) res,req->errmsg);
                    TCPLOG("yTcpSelectReq %p[%x] connection closed by peer\n",req,req->skt);
                    yTcpCloseReqEx(req, 0);
                } else if (res > 0) {
                    req->replysize += res;
                    if(req->replypos < 0) {
                        // Need to analyze http headers
                        if(req->replysize == 8 && !memcmp(req->replybuf, "0K\r\n\r\n\r\n", 8)) {
                            TCPLOG("yTcpSelectReq %p[%x] untrashort reply\n",req,req->skt);
                            // successful abbreviated reply (keepalive)
                            req->replypos = 0;
                            req->replybuf[0] = 'O';
                            req->errcode = YERRTO(YAPI_NO_MORE_DATA, req->errmsg);
                            yTcpCloseReqEx(req, 1);
                        } else if(req->replysize >= 4 && !memcmp(req->replybuf, "OK\r\n", 4)) {
                            // successful short reply, let it go through
                            req->replypos = 0;
                        } else if(req->replysize >= 12) {
                            if(memcmp(req->replybuf, "HTTP/1.1 401", 12) != 0) {
                                // no authentication required, let it go through
                                req->replypos = 0;
                            } else {
                                // authentication required, process authentication headers
                                char *method = NULL, *realm = NULL, *qop = NULL, *nonce = NULL, *opaque = NULL;

                                if(!req->hub->user || req->retryCount++ > 3) {
                                    // No credential provided, give up immediately
                                    req->replypos = 0;
                                    req->replysize = 0;
                                    req->errcode = YERRTO(YAPI_UNAUTHORIZED, req->errmsg);
                                    yTcpCloseReqEx(req, 0);
                                } else if(yParseWWWAuthenticate((char*)req->replybuf, req->replysize, &method, &realm, &qop, &nonce, &opaque) >= 0) {
                                    // Authentication header fully received, we can close the connection
                                    if (!strcmp(method, "Digest") && !strcmp(qop, "auth")) {
                                        // partial close to reopen with authentication settings
                                        yTcpClose(req->skt);
                                        req->skt = INVALID_SOCKET;
                                        // device requests Digest qop-authentication, good
                                        yEnterCriticalSection(&req->hub->authAccess);
                                        yDupSet(&req->hub->realm, realm);
                                        yDupSet(&req->hub->nonce, nonce);
                                        yDupSet(&req->hub->opaque, opaque);
                                        if (req->hub->user && req->hub->pwd) {
                                            ComputeAuthHA1(req->hub->ha1, req->hub->user, req->hub->pwd, req->hub->realm);
                                        }
                                        req->hub->nc = 0;
                                        yLeaveCriticalSection(&req->hub->authAccess);
                                        // reopen connection with proper auth parameters
                                        // callback and context parameters are preserved
                                        req->errcode = yTcpOpenReqEx(req, req->timeout_tm, req->errmsg);
                                        if (YISERR(req->errcode)) {
                                            yTcpCloseReqEx(req, 0);
                                        }
                                    } else {
                                        // unsupported authentication method for devices, give up
                                        req->replypos = 0;
                                        req->errcode = YERRTO(YAPI_UNAUTHORIZED, req->errmsg);
                                        yTcpCloseReqEx(req, 0);
                                    }
                                }
                            }
                        }
                    }
                    if (req->errcode == YAPI_SUCCESS) {
                        req->errcode = yTcpCheckReqTimeout(req, req->errmsg);
                    }
                }
                yLeaveCriticalSection(&req->access);
            }
        }
    }
    return YAPI_SUCCESS;
}


int yTcpEofReq(struct _TcpReqSt *req, char *errmsg)
{
    int res;
    yEnterCriticalSection(&req->access);
    if(req->errcode == YAPI_NO_MORE_DATA) {
        res = 1;
    } else if(req->errcode == 0) {
        res = req->errcode = yTcpCheckReqTimeout(req, req->errmsg);
    } else if(req->errcode == YAPI_UNAUTHORIZED) {
        res = YERRMSG((YRETCODE) req->errcode, "Access denied, authorization required");
    } else {
        res = YERRMSG((YRETCODE) req->errcode, req->errmsg);
    }
    yLeaveCriticalSection(&req->access);
    return res;
}


int yTcpGetReq(struct _TcpReqSt *req, u8 **buffer)
{
    int avail;

    yEnterCriticalSection(&req->access);
    yTcpCheckReqTimeout(req, req->errmsg);
    if(req->replypos < 0) {
        // data is not yet ready to consume (still processing header)
        avail = 0;
    } else {
        avail = req->replysize - req->replypos;
        if(buffer) {
            *buffer = req->replybuf + req->replypos;
        }
    }
    yLeaveCriticalSection(&req->access);

    return avail;
}


int yTcpReadReq(struct _TcpReqSt *req, u8 *buffer, int len)
{
    int avail;

    yEnterCriticalSection(&req->access);
    yTcpCheckReqTimeout(req, req->errmsg);
    if(req->replypos < 0) {
        // data is not yet ready to consume (still processing header)
        len = 0;
    } else {
        avail = req->replysize - req->replypos;
        if(len > avail) {
            len = avail;
        }
        if(buffer) {
            memcpy(buffer, req->replybuf+req->replypos, len);
        }
        if(req->replypos + len == req->replysize) {
            req->replypos = 0;
            req->replysize = 0;
        } else {
            req->replypos += len;
        }
    }
    yLeaveCriticalSection(&req->access);

    return len;
}


void yTcpCloseReq(struct _TcpReqSt *req)
{
    TCPLOG("yTcpCloseReq %p[%x]\n",req,req->skt);
    yEnterCriticalSection(&req->access);
    yTcpCloseReqEx(req, 0);
    req->flags &= ~TCPREQ_IN_USE;
    yLeaveCriticalSection(&req->access);
}


int yTcpIsAsyncReq(struct _TcpReqSt *req)
{
    int res;
    yEnterCriticalSection(&req->access);
    res = (req->skt != INVALID_SOCKET) && (req->callback != NULL);
    yLeaveCriticalSection(&req->access);
    return res;
}


void yTcpFreeReq(struct _TcpReqSt *req)
{
    TCPLOG("yTcpFreeReq %p[%x]\n",req,req->skt);
    if(req->skt != INVALID_SOCKET) {
        yTcpClose(req->skt);
    }
    if(req->reuseskt != INVALID_SOCKET) {
        yTcpClose(req->reuseskt);
    }
    if(req->headerbuf)  yFree(req->headerbuf);
    if(req->bodybuf)    yFree(req->bodybuf);
    if(req->replybuf)   yFree(req->replybuf);
    yCloseEvent(&req->finished);
    yDeleteCriticalSection(&req->access);
    memset(req, 0, sizeof(struct _TcpReqSt));
}


int yTcpInit(char *errmsg)
{
    int i;
#ifdef WINDOWS_API
    // Initialize Winsock 2.2
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0){
        return YERRMSG(YAPI_IO_ERROR,"Unable to start Windows Socket");
    }
#endif
    TCPLOG("yTcpInit\n");
    for (i=0; i<YDNS_CACHE_SIZE;i++){
        dnsCache[i].url = INVALID_HASH_IDX;
    }
    return YAPI_SUCCESS;
}

void yTcpShutdown(void)
{

    TCPLOG("yTcpShutdown\n");
#ifdef PERF_TCP_FUNCTIONS
    dumpYTcpPerf();
#endif
#ifdef WINDOWS_API
    WSACleanup();
#endif
}


/********************************************************************************
 * UDP funtions
 *******************************************************************************/

//#define DEBUG_NET_DETECTION

os_ifaces detectedIfaces[NB_OS_IFACES];
int nbDetectedIfaces = 0;


#ifdef WINDOWS_API
YSTATIC int yDetectNetworkInterfaces(u32 only_ip)
{
    INTERFACE_INFO winIfaces[NB_OS_IFACES];
    DWORD          returnedSize, nbifaces, i;
    SOCKET         sock;

    nbDetectedIfaces = 0;
    memset(detectedIfaces, 0, sizeof(detectedIfaces));
    sock = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
    if (sock == SOCKET_ERROR){
        yNetLogErr();
        return -1;
    }
    if (WSAIoctl(sock, SIO_GET_INTERFACE_LIST, NULL, 0, winIfaces, sizeof(winIfaces), &returnedSize, NULL, NULL)<0){
        yNetLogErr();
        return -1;
    }

    nbifaces = returnedSize / sizeof(INTERFACE_INFO);
    for (i = 0; i<nbifaces; i++){
        if (winIfaces[i].iiFlags & IFF_LOOPBACK)
            continue;
        if (winIfaces[i].iiFlags & IFF_UP){
            if (winIfaces[i].iiFlags & IFF_MULTICAST)
                detectedIfaces[nbDetectedIfaces].flags |= OS_IFACE_CAN_MCAST;
            if (only_ip != 0 && only_ip != winIfaces[i].iiAddress.AddressIn.sin_addr.S_un.S_addr){
                continue;
            }
            detectedIfaces[nbDetectedIfaces].ip = winIfaces[i].iiAddress.AddressIn.sin_addr.S_un.S_addr;
            detectedIfaces[nbDetectedIfaces].netmask = winIfaces[i].iiNetmask.AddressIn.sin_addr.S_un.S_addr;
            nbDetectedIfaces++;
        }
    }
    return nbDetectedIfaces;

}
#else

#include <net/if.h>
#include <ifaddrs.h>
YSTATIC int yDetectNetworkInterfaces(u32 only_ip)
{
    struct ifaddrs *if_addrs = NULL;
    struct ifaddrs *p = NULL;
#if 1
    nbDetectedIfaces = 0;
    memset(detectedIfaces, 0, sizeof(detectedIfaces));
    if (getifaddrs(&if_addrs) != 0){
        yNetLogErr();
        return -1;
    }
    p = if_addrs;
    while (p) {
        if (p->ifa_addr && p->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *tmp;
            u32 ip, netmask;
            tmp = (struct sockaddr_in*)p->ifa_addr;
            ip = tmp->sin_addr.s_addr;
            if (only_ip != 0 && only_ip != ip){
                p = p->ifa_next;
                continue;
            }
            tmp = (struct sockaddr_in*)p->ifa_netmask;
            netmask = tmp->sin_addr.s_addr;
            if ((p->ifa_flags & IFF_LOOPBACK) == 0){
                if (p->ifa_flags & IFF_UP && p->ifa_flags & IFF_RUNNING){
#ifdef DEBUG_NET_DETECTION
                    ylogf("%s : ", p->ifa_name);
                    ylogIP(ip);
                    ylogf("/");
                    ylogIP(netmask);
                    ylogf(" (%X)\n", p->ifa_flags);
#endif
                    if (p->ifa_flags & IFF_MULTICAST){
                        detectedIfaces[nbDetectedIfaces].flags |= OS_IFACE_CAN_MCAST;
                    }
                    detectedIfaces[nbDetectedIfaces].ip = ip;
                    detectedIfaces[nbDetectedIfaces].netmask = netmask;
                    nbDetectedIfaces++;
                }
            }
#ifdef DEBUG_NET_DETECTION
            else {
                ylogf("drop %s : ", p->ifa_name);
                ylogIP(ip);
                ylogf("/");
                ylogIP(netmask);
                ylogf(" (%X)\n", p->ifa_flags);
            }
#endif
        }
        p = p->ifa_next;
    }

#else
    nbDetectedIfaces = 1;
    memset(detectedIfaces, 0, sizeof(detectedIfaces));
    detectedIfaces[0].flags |= OS_IFACE_CAN_MCAST;
    detectedIfaces[0].ip = INADDR_ANY;
#endif
    return nbDetectedIfaces;
}

#endif




static const char *discovery =
                "M-SEARCH * HTTP/1.1\r\n"
                "HOST:" YSSDP_MCAST_ADDR_STR ":" TOSTRING(YSSDP_PORT) "\r\n"
                "MAN:\"ssdp:discover\"\r\n"
                "MX:5\r\n"
                "ST:" YSSDP_URN_YOCTOPUCE"\r\n"
                "\r\n";


#define SSDP_NOTIFY "NOTIFY * HTTP/1.1\r\n"
#define SSDP_M_SEARCH "M-SEARCH * HTTP/1.1\r\n"
#define SSDP_HTTP "HTTP/1.1 200 OK\r\n"
#define SSDP_LINE_MAX_LEN 80u

#define UDP_IN_FIFO yFifoBuf


static char  hexatochar(char hi_c, char lo_c)
{
    u8 hi, lo;
    hi = ((u8)(hi_c)& 0x1f) ^ 0x10;
    lo = ((u8)(lo_c) & 0x1f) ^ 0x10;
    if (hi & 0x10) hi -= 7;
    if (lo & 0x10) lo -= 7;
    return (hi << 4) + lo;
}

static void uuidToSerial(const char * uuid, char *serial)
{
    int i;
    int len;
    char *s = serial;
    const char *u = uuid;

    for (i = 0, u = uuid; i< 4; i++, u += 2){
        *s++ = hexatochar(*u, *(u + 1));
    }
    u++;
    for (; i< 6; i++, u += 2){
        *s++ = hexatochar(*u, *(u + 1));
    }
    u++;
    for (; i< 8; i++, u += 2){
        *s++ = hexatochar(*u, *(u + 1));
    }
    *s++ ='-';
    u = strstr(uuid, "-COFF-EE");
    u += 8;
    while (*u == '0') u++;
    len = YSTRLEN(u);
    for (i = len; i<5; i++){
        *s++ = '0';
    }
    *s = 0;
    YSTRCAT(serial, YOCTO_SERIAL_LEN, u);
}


static void ySSDPUpdateCache(SSDPInfos *SSDP, const char *uuid, const char * url, int cacheValidity)
{
    int i;

    if(cacheValidity<=0)
        cacheValidity = 1800;
    cacheValidity*=1000;

    for (i = 0; i < NB_SSDP_CACHE_ENTRY; i++){
        SSDP_CACHE_ENTRY *p = SSDP->SSDPCache[i];
        if (p == NULL)
            break;
        if (YSTRCMP(uuid,p->uuid) == 0) {
            p->detectedTime = yapiGetTickCount();
            p->maxAge = cacheValidity;

            if (YSTRCMP(url, p->url)){
                if (SSDP->callback) {
                    SSDP->callback(p->serial, url, p->url);
                }
                YSTRCPY(p->url, SSDP_URL_LEN, url);
            } else {
                if (SSDP->callback){
                    SSDP->callback(p->serial, url, NULL);
                }
            }
            return;
        }
    }
    if (i < NB_SSDP_CACHE_ENTRY){
        SSDP_CACHE_ENTRY *p = (SSDP_CACHE_ENTRY*) yMalloc(sizeof(SSDP_CACHE_ENTRY));
        YSTRCPY(p->uuid,SSDP_URL_LEN,uuid);
        uuidToSerial(p->uuid, p->serial);
        YSTRCPY(p->url,SSDP_URL_LEN,url);
        p->detectedTime = yapiGetTickCount();
        p->maxAge = cacheValidity;
        SSDP->SSDPCache[i] = p;
        if (SSDP->callback){
            SSDP->callback(p->serial, p->url, NULL);
        }
    }
}

static void ySSDPCheckExpiration(SSDPInfos *SSDP)
{
    int i;
    u64  now =yapiGetTickCount();

    for (i = 0; i<NB_SSDP_CACHE_ENTRY; i++) {
        SSDP_CACHE_ENTRY *p = SSDP->SSDPCache[i];
        if (p == NULL)
            break;
        if ((u64) (now - p->detectedTime) > p->maxAge) {
            p->maxAge = 0;
            if (SSDP->callback) {
                SSDP->callback(p->serial, NULL, p->url);
            }
        }
    }
}




static void ySSDP_parseSSPDMessage(SSDPInfos *SSDP, char *message,int msg_len)
{
    int len =0;
    char *p,*start,*lastsep;
    char *location=NULL;
    char *usn=NULL;
    char *cache=NULL;

    if(len>=msg_len){
        return;
    }

    if (memcmp(message,SSDP_HTTP,YSTRLEN(SSDP_HTTP))==0) {
        len=YSTRLEN(SSDP_HTTP);
    } else if (memcmp(message,SSDP_NOTIFY,YSTRLEN(SSDP_NOTIFY))==0) {
        len=YSTRLEN(SSDP_NOTIFY);
    }
    if (len){
        //dbglog("SSDP Message:\n%s\n",message);
        start = p = lastsep= message +len;
        msg_len-=len;
        while( msg_len && *p ){
            switch(*p) {
            case ':':
                if (lastsep == start){
                    lastsep = p;
                }
                break;
            case '\r':
                if (p==start){
                    // \r\n\r\n ->end
                    if(msg_len>1) msg_len=1;
                    break;
                }

                if (lastsep == start){
                    //no : on the line -> drop this message
                    return;
                }
                *lastsep++=0;
                if (*lastsep==' ') lastsep++;
                *p=0;
                if (strcmp(start,"LOCATION")==0){
                    location=lastsep;
                }else if (strcmp(start,"USN")==0){
                    usn=lastsep;
                }else if (strcmp(start,"CACHE-CONTROL")==0){
                    cache=lastsep;
                }
                break;
            case '\n':
                start =lastsep= p+1;
                break;
            }
            p++;
            msg_len--;
        }
        if(location && usn && cache){
            const char *uuid,*urn;
            int         cacheVal;
            //dbglog("SSDP: location: %s %s %s\n\n",location,usn,cache);
            // parse USN
            p=usn;
            while (*p && *p++!=':');
            if (!*p) return;
            uuid=p;
            while (*p && *p++!=':');
            if (*p != ':') return;
            *(p++-1)=0;
            if (!*p) return;
            urn=p;
            // parse Location
            if(YSTRNCMP(location,"http://",7)==0){
                location += 7;
            }
            p=location;
            while (*p && *p != '/') p++;
            if(*p=='/') *p=0;
            p=cache;
            while (*p && *p++!='=');
            if(!*p) return;
            cacheVal = atoi(p);
            if (YSTRCMP(urn,YSSDP_URN_YOCTOPUCE)==0){
                ySSDPUpdateCache(SSDP, uuid, location, cacheVal);
            }
        }
    }
#if 0
    else {
        dbglog("SSDP drop invalid message:\n%s\n",message);
    }
#endif
}



static void* ySSDP_thread(void* ctx)
{
    yThread     *thread=(yThread*)ctx;
    SSDPInfos *SSDP = (SSDPInfos*)thread->ctx;
    fd_set      fds;
    u8          buffer[1536];
    struct timeval timeout;
    int         res, received, i;
    YSOCKET     sktmax=0;
    yFifoBuf    inFifo;


    yThreadSignalStart(thread);
    yFifoInit(&inFifo,buffer,sizeof(buffer));

    while (!yThreadMustEnd(thread)) {
        memset(&timeout,0,sizeof(timeout));
        timeout.tv_sec = (long)1;
        timeout.tv_usec = (int)0;
        /* wait for data */
        FD_ZERO(&fds);
        sktmax = 0;
        for (i = 0; i < nbDetectedIfaces; i++) {
            FD_SET(SSDP->request_sock[i], &fds);
            if (SSDP->request_sock[i] > sktmax) {
                sktmax = SSDP->request_sock[i];
            }
            if(SSDP->notify_sock[i] != INVALID_SOCKET) {
                FD_SET(SSDP->notify_sock[i], &fds);
                if (SSDP->notify_sock[i] > sktmax) {
                    sktmax = SSDP->notify_sock[i];
                }
            }
        }
        res = select((int)sktmax + 1, &fds, NULL, NULL, &timeout);
        if (res<0) {
    #ifndef WINDOWS_API
            if(SOCK_ERR ==  EAGAIN){
                continue;
            } else
    #endif
            {
                yNetLogErr();
                break;
            }
        }

        if(!yContext) continue;
        ySSDPCheckExpiration(SSDP);
        if (res != 0) {
            for (i = 0; i < nbDetectedIfaces; i++) {
                if (FD_ISSET(SSDP->request_sock[i], &fds)) {
                    received = (int)recv(SSDP->request_sock[i], (char*)buffer, sizeof(buffer)-1, 0);
                    if (received>0) {
                        buffer[received] = 0;
                        ySSDP_parseSSPDMessage(SSDP, (char*)buffer, received);
                    }
                }
                if (FD_ISSET(SSDP->notify_sock[i], &fds)) {
                    received = (int)recv(SSDP->notify_sock[i], (char *)buffer, sizeof(buffer)-1, 0);
                    if (received > 0) {
                        buffer[received] = 0;
                        ySSDP_parseSSPDMessage(SSDP, (char*)buffer, received);
                    }
                }
            }
        }
    }
    yFifoCleanup(&inFifo);
    yThreadSignalEnd(thread);
    return NULL;
}


int ySSDPDiscover(SSDPInfos *SSDP, char *errmsg)
{
    int sent, len, i;
    struct sockaddr_in sockaddr_dst;

    for (i = 0; i < nbDetectedIfaces; i++) {
        memset(&sockaddr_dst, 0, sizeof(struct sockaddr_in));
        sockaddr_dst.sin_family = AF_INET;
        sockaddr_dst.sin_port = htons(YSSDP_PORT);
        sockaddr_dst.sin_addr.s_addr = inet_addr(YSSDP_MCAST_ADDR_STR);
        len = (int)strlen(discovery);
        sent = (int)sendto(SSDP->request_sock[i], discovery, len, 0, (struct sockaddr *)&sockaddr_dst, sizeof(struct sockaddr_in));
        if (sent < 0) {
            return yNetSetErr();
        }
    }
    return YAPI_SUCCESS;
}


int ySSDPStart(SSDPInfos *SSDP, ssdpHubDiscoveryCallback callback, char *errmsg)
{
    u32     optval;
    int     i;
    socklen_t    socksize;
    struct sockaddr_in     sockaddr;
    struct ip_mreq     mcast_membership;

    if (SSDP->started)
        return YAPI_SUCCESS;

    memset(SSDP, 0, sizeof(SSDPInfos));
    SSDP->callback = callback;
    yDetectNetworkInterfaces(0);

    for (i = 0; i < nbDetectedIfaces; i++) {
        //create M-search socker
        SSDP->request_sock[i] = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (SSDP->request_sock[i] == INVALID_SOCKET) {
            return yNetSetErr();
        }

        optval = 1;
        setsockopt(SSDP->request_sock[i], SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
#ifdef SO_REUSEPORT
        setsockopt(SSDP->request_sock[i], SOL_SOCKET, SO_REUSEPORT, (char *)&optval, sizeof(optval));
#endif

        // set port to 0 since we accept any port
        socksize = sizeof(sockaddr);
        memset(&sockaddr, 0, socksize);
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_addr.s_addr = detectedIfaces[i].ip;
        if (bind(SSDP->request_sock[i], (struct sockaddr*) &sockaddr, socksize) < 0) {
            return yNetSetErr();
        }
        //create NOTIFY socker
        SSDP->notify_sock[i] = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (SSDP->notify_sock[i] == INVALID_SOCKET) {
            return yNetSetErr();
        }

        optval = 1;
        setsockopt(SSDP->notify_sock[i], SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
#ifdef SO_REUSEPORT
        setsockopt(SSDP->notify_sock[i], SOL_SOCKET, SO_REUSEPORT, (char *)&optval, sizeof(optval));
#endif

        // set port to 0 since we accept any port
        socksize = sizeof(sockaddr);
        memset(&sockaddr, 0, socksize);
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_port = htons(YSSDP_PORT);
        sockaddr.sin_addr.s_addr = INADDR_ANY;
        if (bind(SSDP->notify_sock[i], (struct sockaddr *)&sockaddr, socksize) < 0) {
            return yNetSetErr();
        }

        mcast_membership.imr_multiaddr.s_addr = inet_addr(YSSDP_MCAST_ADDR_STR);
        mcast_membership.imr_interface.s_addr = INADDR_ANY;
        if (setsockopt(SSDP->notify_sock[i], IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&mcast_membership, sizeof(mcast_membership)) < 0){
            dbglog("Unable to add multicat membership for SSDP");
            yNetLogErr();
            closesocket(SSDP->notify_sock[i]);
            SSDP->notify_sock[i] = INVALID_SOCKET;
        }
    }
    //yThreadCreate will not create a new thread if there is already one running
    if(yThreadCreate(&SSDP->thread,ySSDP_thread,SSDP)<0){
        return YERRMSG(YAPI_IO_ERROR,"Unable to start helper thread");
    }
    SSDP->started++;
    return ySSDPDiscover(SSDP,errmsg);
    //return YAPI_SUCCESS;
}


void  ySSDPStop(SSDPInfos *SSDP)
{
    int i;

    if(yThreadIsRunning(&SSDP->thread)) {
        u64 timeref;
        yThreadRequestEnd(&SSDP->thread);
        timeref = yapiGetTickCount();
        while(yThreadIsRunning(&SSDP->thread) && (yapiGetTickCount() - timeref < 1000) ) {
            yApproximateSleep(10);
        }
        yThreadKill(&SSDP->thread);
    }

    //unregister all detected hubs
    for (i = 0; i<NB_SSDP_CACHE_ENTRY; i++){
        SSDP_CACHE_ENTRY *p = SSDP->SSDPCache[i];
        if(p== NULL)
            continue;
        if (p->maxAge) {
            yapiUnregisterHub(p->url);
            p->maxAge=0;
            if (SSDP->callback)
                SSDP->callback(p->serial, NULL, p->url);
        }
        yFree(p);
    }

    for (i = 0; i < nbDetectedIfaces; i++) {

        if (SSDP->request_sock[i] != INVALID_SOCKET) {
            closesocket(SSDP->request_sock[i]);
            SSDP->request_sock[i] = INVALID_SOCKET;
        }
        if (SSDP->notify_sock[i] != INVALID_SOCKET) {
            closesocket(SSDP->notify_sock[i]);
            SSDP->notify_sock[i] = INVALID_SOCKET;
        }
    }
    SSDP->started--;
}
