/*********************************************************************
 *
 * $Id: ytcp.c 10311 2013-03-14 13:18:58Z mvuilleu $
 *
 *  Implementation of a client TCP stack
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

#define __FILE_ID__  "ytcp"
#include "ydef.h"
#if defined(WINDOWS_API) && !defined(_MSC_VER)
#define _WIN32_WINNT 0x501
#endif
#include "ytcp.h"
#include "yproto.h"
#include "yhash.h"


#ifdef WIN32
#ifndef WINCE
#include <iphlpapi.h>
#ifdef _MSC_VER
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

#ifdef __BORLANDC__
typedef struct addrinfo {
    int                 ai_flags;       // AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST
    int                 ai_family;      // PF_xxx
    int                 ai_socktype;    // SOCK_xxx
    int                 ai_protocol;    // 0 or IPPROTO_xxx for IPv4 and IPv6
    size_t              ai_addrlen;     // Length of ai_addr
    char *              ai_canonname;   // Canonical name for nodename
    struct sockaddr *   ai_addr;        // Binary address
    struct addrinfo *   ai_next;        // Next structure in linked list
} ADDRINFOA, *PADDRINFOA;
int WSAAPI getaddrinfo(PCSTR pNodeName,PCSTR pServiceName,const ADDRINFOA *pHints,PADDRINFOA *ppResult);
void freeaddrinfo(struct addrinfo *ai);
#endif


void  yDupSet(char **storage, const char *val)
{
    int  len = (val ? (int)strlen(val)+1 : 1);
    
    if(*storage) yFree(*storage);
    *storage = yMalloc(len);
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


void yInitWakeUpSocket(WakeUpSocket *wuce)
{
    wuce->listensock = INVALID_SOCKET;
    wuce->signalsock = INVALID_SOCKET;
}


int yStartWakeUpSocket(WakeUpSocket *wuce, char *errmsg)
{
    u32     optval,localh_size;
    struct  sockaddr_in     localh;
    
    if(wuce->listensock != INVALID_SOCKET || wuce->signalsock != INVALID_SOCKET) {
        return YERRMSG(YAPI_INVALID_ARGUMENT,"WakeUpSocket allready Started");
    }
    //create socket
    wuce->listensock = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (wuce->listensock<0) {
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
    if (wuce->signalsock<0) {
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




int yTcpInit(char *errmsg)
{
#ifdef WINDOWS_API
    // Initialize Winsock 2.2
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0){
        return YERRMSG(YAPI_IO_ERROR,"Unable to start Windows Socket");
    }
#endif
    return YAPI_SUCCESS;
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
    
    *newskt = INVALID_SOCKET;
    skt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (skt == INVALID_SOCKET) {
        REPORT_ERR("Error at socket()");
        return YAPI_IO_ERROR;
    }
    //dbglog("ytcpOpen %X:%x: skt= %x\n",ip,port,skt);
    //----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port of the server to be connected to.
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = ip;
    clientService.sin_port = htons( port );

    //----------------------
    // Connect to server.
    iResult = connect(skt, ( struct sockaddr *) &clientService, sizeof(clientService) );
    if ( iResult == SOCKET_ERROR) {
        REPORT_ERR("Unable to connect to server");
        closesocket (skt);
        return YAPI_IO_ERROR;
    }

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
            yySleep(10);
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
}


static int yTcpOpenReqEx(struct _TcpReqSt *req, char *errmsg)
{
    char buffer[YOCTO_HOSTNAME_NAME], *p,*last,*end;
    u32  ip;
    u16  port;
    int  res;
    
    switch(yHashGetUrlPort(req->hub->url, buffer,&port)){
        case NAME_URL:
            ip = yResolveDNS(buffer,errmsg);
            if(ip==0){ return YAPI_IO_ERROR;}
            break;
        case IP_URL:
            ip = inet_addr(buffer);
            break;
        default:
            res = YERRMSG(YAPI_IO_ERROR,"not an IP hub");
            req->errcode = res;
            req->skt = INVALID_SOCKET;        
            return res;
    }
    
    req->replypos = -1; // not ready to consume until header found
    req->replysize = 0;
    req->errcode = YAPI_SUCCESS;
    
    res = yTcpOpen(&req->skt, ip, port, errmsg);
    if(YISERR(res)) {
        // yTcpOpen has reset the socket to INVALID
        yTcpClose(req->skt);
        req->errcode = res;
        req->skt = INVALID_SOCKET;        
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
    YSTRCPY(p, (int)(req->headerbuf+req->headerbufsize-p), "Connection: close\r\n\r\n");
    //write header
    res = yTcpWrite(req->skt, req->headerbuf, (int)strlen(req->headerbuf), errmsg);
    if(YISERR(res)) {
        yTcpClose(req->skt);
        req->errcode = res;
        req->skt = INVALID_SOCKET;        
        return res;
    }
    if(req->bodysize > 0){
        //write body
        res = yTcpWrite(req->skt, req->bodybuf, req->bodysize, errmsg);
        if(YISERR(res)) {
            yTcpClose(req->skt);
            req->errcode = res;
            req->skt = INVALID_SOCKET;        
            return res;
        }
    }
            
    
    return YAPI_SUCCESS;
}

int  yTcpOpenReq(struct _TcpReqSt *req, const char *request, int reqlen, int isAsync, char *errmsg)
{
    int  minlen,i,res;
    
    yEnterCriticalSection(&req->access);
    while(req->skt != INVALID_SOCKET) {
        // There is an ongoing request to be finished
        yLeaveCriticalSection(&req->access);
        yySleep(2);
        yEnterCriticalSection(&req->access);
    }
    
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
    }
    yLeaveCriticalSection(&req->access);    
    
    return res;
}

int yTcpSelectReq(struct _TcpReqSt **reqs, int size, u64 ms, WakeUpSocket *wuce, char *errmsg)
{
    fd_set      fds;
    struct timeval timeout;
    int         res,i;
    YSOCKET     sktmax=0;
    TcpReqSt    *req;
    
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
    yDeleteCriticalSection(&req->access);    
    memset(req, 0, sizeof(struct _TcpReqSt));
}

void yTcpShutdown(void)
{
#ifdef WINDOWS_API
    WSACleanup();
#endif
}

