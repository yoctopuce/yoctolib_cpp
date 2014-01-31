/*********************************************************************
 *
 * $Id: ytcp.c 14711 2014-01-24 14:56:44Z seb $
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
    if(send(wuce->signalsock,&signal,1,SEND_NOSIGPIPE) < 0) {
        return yNetSetErr();
    }
    return YAPI_SUCCESS;
}

int yConsumeWakeUpSocket(WakeUpSocket *wuce, char *errmsg)
{
    u8 signal = 0;
    
    if(recv(wuce->listensock,&signal,1,0) < 0) {
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
        REPORT_ERR("Unable to connect to server");
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

static int yTcpOpen(YSOCKET *newskt, u32 ip, u16 port, char *errmsg)
{
    struct sockaddr_in clientService;
    int iResult;
    u_long flags;
    YSOCKET skt;
#ifdef WINDOWS_API
    char noDelay=1;
#else
    int  noDelay=1;
#ifdef SO_NOSIGPIPE
    int  noSigpipe=1;
#endif    
#endif
    
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
    //----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port of the server to be connected to.
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = ip;
    clientService.sin_port = htons( port );

    //----------------------
    // Connect to server.
    iResult = connect(skt, ( struct sockaddr *) &clientService, sizeof(clientService) );
    YPERF_TCP_LEAVE(TCPOpen_connect);
    if ( iResult == SOCKET_ERROR) {
        REPORT_ERR("Unable to connect to server");
        closesocket (skt);
        return YAPI_IO_ERROR;
    }

    YPERF_TCP_ENTER(TCPOpen_setsockopt_noblock);
    //set socket as non blocking
#ifdef WINDOWS_API
    flags=1;
    ioctlsocket(skt, FIONBIO, &flags);
#else
    flags = fcntl(skt, F_GETFL,0);
    fcntl(skt, F_SETFL, flags | O_NONBLOCK);
#ifdef SO_NOSIGPIPE
    setsockopt(skt, SOL_SOCKET, SO_NOSIGPIPE, (void *)&noSigpipe, sizeof(int));
#endif    
#endif
    YPERF_TCP_LEAVE(TCPOpen_setsockopt_noblock);
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


static int yTcpWrite(YSOCKET skt, const char *buffer, int len,char *errmsg)
{
    int iResult;
    
    // Send an initial buffer
#ifdef WINDOWS_API
retry:
#endif
    iResult = (int) send(skt, buffer, len, SEND_NOSIGPIPE);
    if (iResult == SOCKET_ERROR) {
#ifdef WINDOWS_API
        if(SOCK_ERR == WSAEWOULDBLOCK){
            yApproximateSleep(10);
            goto retry;
        }
#endif
        REPORT_ERR("send failed");
        return YAPI_IO_ERROR;
    }
    return iResult;
}


static int yTcpRead(YSOCKET skt, char *buffer, int len,char *errmsg)
{
    int iResult = (int)recv(skt, buffer, len, 0);
    
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

static void yTcpClose(YSOCKET skt)
{
    // cleanup
    closesocket(skt);
}

void yTcpInitReq(struct _TcpReqSt *req, struct _NetHubSt *hub)
{
    memset(req, 0, sizeof(struct _TcpReqSt));
    req->hub = hub;
    req->skt = INVALID_SOCKET;
    req->replybufsize = 1024;
    req->replybuf = yMalloc(req->replybufsize);
    yInitializeCriticalSection(&req->access);
    yCreateManualEvent(&req->finished,1);
}


#define YDNS_CACHE_SIZE 16
#define YDNS_CACHE_VALIDITY 600000u //10 minutes
typedef struct {
	yUrlRef url;
	u32		ip;
	u64		time;
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



static int yTcpOpenReqEx(struct _TcpReqSt *req, char *errmsg)
{
    char buffer[YOCTO_HOSTNAME_NAME], *p,*last,*end;
    u32  ip;
    u16  port;
    int  res;
    
    YPERF_TCP_ENTER(tmp1);
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
            req->errcode = res;
            req->skt = INVALID_SOCKET;        
            YPERF_TCP_LEAVE(tmp1);
            return res;
    }
    
    req->replypos = -1; // not ready to consume until header found
    req->replysize = 0;
    req->errcode = YAPI_SUCCESS;
    
    YPERF_TCP_ENTER(tmp2);
    res = yTcpOpen(&req->skt, ip, port, errmsg);
    if(YISERR(res)) {
        // yTcpOpen has reset the socket to INVALID
        yTcpClose(req->skt);
        req->errcode = res;
        req->skt = INVALID_SOCKET;        
        YPERF_TCP_LEAVE(tmp2);
        YPERF_TCP_LEAVE(tmp1);
        return res;
    }

    p = req->headerbuf;
    //skip firt line 
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
            end+=len;
        }
        last =p;
    }
    *p++ = '\r'; *p++ = '\n';
    YPERF_TCP_LEAVE(tmp2);
    YPERF_TCP_ENTER(tmp3);
    // insert authorization header in needed
    yEnterCriticalSection(&req->hub->authAccess);
    if(req->hub->user && req->hub->realm) {        
        char *method = req->headerbuf, *uri;
        char *auth = p;
        // null-terminate method and uri for digest computation
        for(uri = method; *uri != ' '; uri++);
        *uri++ = 0;
        for(p = uri; *p != ' '; p++);
        *p = 0;
        yDigestAuthorization(auth, (int)(req->headerbuf+req->headerbufsize-auth), req->hub->user, req->hub->realm, req->hub->ha1, 
                             req->hub->nonce, req->hub->opaque, &req->hub->nc, method, uri);
        // restore space separator after method and uri
        *--uri = ' ';
        *p = ' ';
        // prepare to complete request
        p = auth+strlen(auth);
    }
    yLeaveCriticalSection(&req->hub->authAccess);
    YPERF_TCP_LEAVE(tmp3);
    YPERF_TCP_ENTER(tmp4);
    YSTRCPY(p, (int)(req->headerbuf+req->headerbufsize-p), "Connection: close\r\n\r\n");
    //write header
    res = yTcpWrite(req->skt, req->headerbuf, (int)strlen(req->headerbuf), errmsg);
    if(YISERR(res)) {
        yTcpClose(req->skt);
        req->errcode = res;
        req->skt = INVALID_SOCKET;        
        YPERF_TCP_LEAVE(tmp4);
        YPERF_TCP_LEAVE(tmp1);
        return res;
    }
    if(req->bodysize > 0){
        //write body
        res = yTcpWrite(req->skt, req->bodybuf, req->bodysize, errmsg);
        if(YISERR(res)) {
            yTcpClose(req->skt);
            req->errcode = res;
            req->skt = INVALID_SOCKET;        
            YPERF_TCP_LEAVE(tmp4);
            YPERF_TCP_LEAVE(tmp1);
            return res;
        }
    }
    YPERF_TCP_LEAVE(tmp4);
    YPERF_TCP_LEAVE(tmp1);
    
    return YAPI_SUCCESS;
}

int  yTcpOpenReq(struct _TcpReqSt *req, const char *request, int reqlen, int isAsync, char *errmsg)
{
    int  minlen,i,res;

	YPERF_TCP_ENTER(TCPOpenReq_wait);    
	YPERF_TCP_ENTER(TCPOpenReq);

    
    yWaitForEvent(&req->finished,100);
    yEnterCriticalSection(&req->access);
    while(req->skt != INVALID_SOCKET) {
        // There is an ongoing request to be finished
        yLeaveCriticalSection(&req->access);
        yWaitForEvent(&req->finished,100);
        yEnterCriticalSection(&req->access);
    }
    
	YPERF_TCP_LEAVE(TCPOpenReq_wait);    
    if (request[0]=='G' && request[1]=='E' && request[2]=='T'){
        //for GET request discard all exept the first line
        for(i = 0; i < reqlen; i++) {
            if(request[i] == '\r') {
                reqlen = i;
                break;
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
            req->bodybuf = yMalloc(req->bodybufsize);
        }
        memcpy(req->bodybuf, p, bodylen);
        req->bodysize = bodylen;
    }
    // Build a request buffer with at least a terminal NUL but
    // include space for Connection: close and Authorization: headers
    minlen = reqlen + 400; 
    if(req->headerbufsize < minlen) {
        if(req->headerbuf) yFree(req->headerbuf);
        req->headerbufsize = minlen + (reqlen>>1);
        req->headerbuf = yMalloc(req->headerbufsize);
    }
    memcpy(req->headerbuf, request, reqlen);
    req->headerbuf[reqlen] = 0;

    req->retryCount = 0;
    
    // Really build and send the request
    res = yTcpOpenReqEx(req, errmsg);
    if(res == YAPI_SUCCESS) {
        req->isAsyncIO = isAsync;
        yResetEvent(&req->finished);
    }
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
    if(wuce) {
        FD_SET(wuce->listensock,&fds);
        sktmax = wuce->listensock;
    }
    for (i = 0; i < size; i++) {
        req = reqs[i];
        if(req->skt == INVALID_SOCKET) {
            req->errcode = YAPI_INVALID_ARGUMENT;
        } else {
            FD_SET(req->skt,&fds);
            if(req->skt > sktmax)
                sktmax = req->skt;
        }
    }
    res = select((int)sktmax+1,&fds,NULL,NULL,&timeout);
    if (res<0) {
#ifndef WINDOWS_API
        if(SOCK_ERR ==  EAGAIN){
            return 0;
        } else
#endif
        {
            return yNetSetErr();
        }
    }
    if (res!=0) {
        if (wuce && FD_ISSET(wuce->listensock,&fds)) {
            YPROPERR(yConsumeWakeUpSocket(wuce, errmsg));
        }
        for (i=0; i < size; i++) {
            req = reqs[i];
            if(FD_ISSET(req->skt,&fds)){
                yEnterCriticalSection(&req->access);
                if(req->replysize >= req->replybufsize - 256) {
                    // need to grow receive buffer
                    int  newsize = req->replybufsize << 1;
                    char *newbuf = yMalloc(newsize);
                    memcpy(newbuf, req->replybuf, req->replysize);
                    yFree(req->replybuf);
                    req->replybuf = newbuf;
                    req->replybufsize = newsize;
                }
                res = yTcpRead(req->skt, req->replybuf+req->replysize, req->replybufsize-req->replysize, errmsg);
                if(res < 0) {
                    // any connection closed by peer ends up with YAPI_NO_MORE_DATA
                    yTcpClose(req->skt);
                    req->isAsyncIO = 0;
                    req->skt = INVALID_SOCKET;
                    ySetEvent(&req->finished);
                    req->errcode = res;
                } else if(res > 0) {
                    req->replysize += res;
                    if(req->replypos < 0) {
                        // Need to analyze http headers
                        if(req->replysize >= 4 && !memcmp(req->replybuf, "OK\r\n", 4)) {
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
                                    yTcpClose(req->skt);
                                    req->isAsyncIO = 0;
                                    req->skt = INVALID_SOCKET;
                                    ySetEvent(&req->finished);
                                    req->errcode = YAPI_UNAUTHORIZED;
                                } else if(yParseWWWAuthenticate(req->replybuf, req->replysize, &method, &realm, &qop, &nonce, &opaque) >= 0) {
                                    // Authentication header fully received, we can close the connection
                                    int isAsync = req->isAsyncIO;
                                    yTcpClose(req->skt);
                                    req->skt = INVALID_SOCKET;
                                    req->isAsyncIO = 0;
                                    if(!strcmp(method,"Digest") && !strcmp(qop,"auth")) {
                                        // device requests Digest qop-authentication, good
                                        yEnterCriticalSection(&req->hub->authAccess);
                                        yDupSet(&req->hub->realm, realm);
                                        yDupSet(&req->hub->nonce, nonce);
                                        yDupSet(&req->hub->opaque, opaque);
                                        if(req->hub->user && req->hub->pwd) {
                                            ComputeAuthHA1(req->hub->ha1, req->hub->user, req->hub->pwd, req->hub->realm);
                                        }
                                        req->hub->nc = 0;
                                        yLeaveCriticalSection(&req->hub->authAccess);
                                        // reopen connection with proper auth parameters
                                        res = yTcpOpenReqEx(req, errmsg);
                                        if(res == YAPI_SUCCESS) {
                                            req->isAsyncIO = isAsync;
                                        }
                                    } else {
                                        // unsupported authentication method for devices, give up
                                        req->errcode = YAPI_UNAUTHORIZED;
                                        ySetEvent(&req->finished);
                                    }
                                }
                            }
                        }
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
    if(req->errcode == YAPI_NO_MORE_DATA) {
        return 1;
    } else if(req->errcode == 0) {
        return 0;
    } else if(req->errcode == YAPI_UNAUTHORIZED) {
        return YERRMSG(req->errcode, "Access denied, authorization required");
    }
    return YERRMSG(req->errcode, "Network error during select");
}


int yTcpGetReq(struct _TcpReqSt *req, char **buffer)
{
    int avail;
    
    yEnterCriticalSection(&req->access);
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


int yTcpReadReq(struct _TcpReqSt *req, char *buffer, int len)
{
    int avail;
    
    yEnterCriticalSection(&req->access);
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
    yEnterCriticalSection(&req->access);
    if(req->skt != INVALID_SOCKET) {
        yTcpClose(req->skt);
        req->isAsyncIO = 0;
        req->skt = INVALID_SOCKET;
        ySetEvent(&req->finished);
    }
    req->isAsyncIO = 0;
    yLeaveCriticalSection(&req->access);
}


void yTcpFreeReq(struct _TcpReqSt *req)
{
    if(req->skt != INVALID_SOCKET) {
        yTcpClose(req->skt);
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
	for (i=0; i<YDNS_CACHE_SIZE;i++){
		dnsCache[i].url = INVALID_HASH_IDX;
	}
    return YAPI_SUCCESS;
}

void yTcpShutdown(void)
{

#ifdef PERF_TCP_FUNCTIONS 
    dumpYTcpPerf();
#endif
#ifdef WINDOWS_API
    WSACleanup();
#endif
}



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

	for (i = 0; i<NB_SSDP_CACHE_ENTRY; i++){
        SSDP_CACHE_ENTRY *p = SSDP->SSDPCache[i];
        if(p== NULL)
            break;
        if (YSTRCMP(uuid,p->uuid)==0){
            p->detectedTime = yapiGetTickCount();
            p->maxAge = cacheValidity;

			if (YSTRCMP(url, p->url)){
				if (SSDP->callback) {
					SSDP->callback(p->serial, url, p->url);
				}
				YSTRCPY(p->url, SSDP_URL_LEN, url);
			}
			else {
				if (SSDP->callback){
					SSDP->callback(p->serial, url, NULL);
				}
			}
            return;
        }
    }
	if (i <NB_SSDP_CACHE_ENTRY){
		SSDP_CACHE_ENTRY *p = yMalloc(sizeof(SSDP_CACHE_ENTRY));
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
    
	for (i = 0; i<NB_SSDP_CACHE_ENTRY; i++){
		SSDP_CACHE_ENTRY *p = SSDP->SSDPCache[i];
        if(p== NULL)
            break;
        if ((u64)(now - p->detectedTime) > p->maxAge) { 
            p->maxAge=0;

            if (SSDP->callback){
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
    int         res,received;
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
        FD_SET(SSDP->request_sock,&fds);
        if(SSDP->notify_sock!=INVALID_SOCKET) {
            FD_SET(SSDP->notify_sock,&fds);
            if(SSDP->notify_sock>sktmax){
                sktmax = SSDP->notify_sock;
            }
        }
        sktmax = SSDP->request_sock;
        res = select((int)sktmax+1,&fds,NULL,NULL,&timeout);
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
        if (res!=0) {
            if (FD_ISSET(SSDP->request_sock,&fds)) {
                received = (int)recv(SSDP->request_sock, (char*)buffer, sizeof(buffer)-1, 0);
                if (received>0) {
                    buffer[received]=0;
                    ySSDP_parseSSPDMessage(SSDP, (char*) buffer,received);
                }
            }
            if (FD_ISSET(SSDP->notify_sock,&fds)) {
                received = (int)recv(SSDP->notify_sock, (char *) buffer, sizeof(buffer)-1, 0);
                if (received>0) {
                    buffer[received]=0;
                    ySSDP_parseSSPDMessage(SSDP, (char*) buffer,received);
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
    int sent,len;
    struct sockaddr_in sockaddr_dst;

    memset(&sockaddr_dst, 0, sizeof(struct sockaddr_in));   
    sockaddr_dst.sin_family = AF_INET;
    sockaddr_dst.sin_port = htons(YSSDP_PORT);
    sockaddr_dst.sin_addr.s_addr = inet_addr(YSSDP_MCAST_ADDR_STR);
    len =  (int) strlen(discovery);
    sent = (int) sendto(SSDP->request_sock, discovery, len, 0, (struct sockaddr *)&sockaddr_dst,sizeof(struct sockaddr_in));
    if (sent < 0) {
        return yNetSetErr();
    }
    return YAPI_SUCCESS;
}


int ySSDPStart(SSDPInfos *SSDP, ssdpHubDiscoveryCallback callback, char *errmsg)
{
    u32     optval;
    socklen_t    socksize;
    struct sockaddr_in     sockaddr;
    struct ip_mreq     mcast_membership; 

    if (SSDP->started)
        return YAPI_SUCCESS;

    memset(SSDP, 0, sizeof(SSDPInfos));
	SSDP->callback = callback;


    //create M-search socker
    SSDP->request_sock = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (SSDP->request_sock == INVALID_SOCKET) {
        return yNetSetErr();
    }
   
    optval = 1; 
    setsockopt(SSDP->request_sock,SOL_SOCKET,SO_REUSEADDR,(char *)&optval,sizeof(optval));
#ifdef SO_REUSEPORT
    setsockopt(SSDP->request_sock,SOL_SOCKET,SO_REUSEPORT,(char *)&optval,sizeof(optval));
#endif

    // set port to 0 since we accept any port
    socksize=sizeof(sockaddr);
    memset(&sockaddr,0,socksize);
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr =INADDR_ANY;
    if (bind(SSDP->request_sock ,(struct sockaddr *)&sockaddr,socksize)<0) {
        return yNetSetErr();
    }

    //create NOTIFY socker
    SSDP->notify_sock = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (SSDP->notify_sock == INVALID_SOCKET) {
        return yNetSetErr();
    }
   
    optval = 1; 
    setsockopt(SSDP->notify_sock,SOL_SOCKET,SO_REUSEADDR,(char *)&optval,sizeof(optval));
#ifdef SO_REUSEPORT
    setsockopt(SSDP->notify_sock,SOL_SOCKET,SO_REUSEPORT,(char *)&optval,sizeof(optval));
#endif

    // set port to 0 since we accept any port
    socksize=sizeof(sockaddr);
    memset(&sockaddr,0,socksize);
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(YSSDP_PORT);
    sockaddr.sin_addr.s_addr =INADDR_ANY;
    if (bind(SSDP->notify_sock ,(struct sockaddr *)&sockaddr,socksize)<0) {
        return yNetSetErr();
    }

    mcast_membership.imr_multiaddr.s_addr = inet_addr(YSSDP_MCAST_ADDR_STR);
    mcast_membership.imr_interface.s_addr = INADDR_ANY;    
    if (setsockopt(SSDP->notify_sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,(void*)&mcast_membership,sizeof(mcast_membership))<0){
        dbglog("Unable to add multicat membership for SSDP");
        yNetLogErr();
        closesocket(SSDP->notify_sock);
        SSDP->notify_sock=INVALID_SOCKET;
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
        timeref=yapiGetTickCount();
        while(yThreadIsRunning(&SSDP->thread) && (yapiGetTickCount()-timeref >1000) ) {
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


    if ( SSDP->request_sock != INVALID_SOCKET) {
        closesocket(SSDP->request_sock);
        SSDP->request_sock = INVALID_SOCKET;
    }
    if ( SSDP->notify_sock != INVALID_SOCKET) {
        closesocket(SSDP->notify_sock);
        SSDP->notify_sock = INVALID_SOCKET;
    }
    SSDP->started--;
}
