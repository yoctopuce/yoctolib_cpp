/*********************************************************************
 *
 * $Id: yproto.h 21533 2015-09-16 16:25:07Z seb $
 *
 * Definitions and prototype common to all supported OS
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

#ifndef  YPROTO_H
#define  YPROTO_H

#include "ydef.h"
#ifdef WINDOWS_API
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x400
#endif
#endif
#include "yapi.h"
#include "ytcp.h"
#ifdef WINDOWS_API
/**************************************************************
* WINDOWS SPECIFIC HEADER
***************************************************************/
#if defined(__BORLANDC__)
#pragma warn -8019
#include <windows.h>
#pragma warn +8019
#else
#include <windows.h>
#endif
#include <dbt.h>

#include <SetupAPI.h>
#ifdef _MSC_VER
#pragma comment(lib, "SetupApi.lib")
#pragma comment(lib, "OleAut32.lib")

#endif
/**************************************************************
* WINDOWS HID ACCES FOR WINODWS
***************************************************************/

typedef struct _HIDD_ATTRIBUTES {
    ULONG Size;
    USHORT VendorID;
    USHORT ProductID;
    USHORT VersionNumber;
} HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;

//Pointers to a HID function  used
typedef VOID    (__stdcall *PHidD_GetHidGuid)(LPGUID);
typedef BOOLEAN (__stdcall *PHidD_GetAttributes)(HANDLE, PHIDD_ATTRIBUTES);
typedef BOOLEAN (__stdcall *PHidD_GetManufacturerString) (HANDLE,PVOID,ULONG);
typedef BOOLEAN (__stdcall *PHidD_GetProductString) (HANDLE,PVOID,ULONG);
typedef BOOLEAN (__stdcall *PHidD_GetSerialNumberString) (HANDLE,PVOID,ULONG);


typedef struct{
    HINSTANCE                   hHID;
    PHidD_GetHidGuid            GetHidGuid;
    PHidD_GetAttributes         GetAttributes;
    PHidD_GetManufacturerString GetManufacturerString;
    PHidD_GetProductString      GetProductString;
    PHidD_GetSerialNumberString GetSerialNumberString;
}win_hid_api;


//Pointers to a registry function  used

typedef LONG (__stdcall *PYRegCreateKeyEx)( HKEY hKey,
                                            char *                lpSubKey,
                                            DWORD                 Reserved,
                                            LPTSTR                lpClass,
                                            DWORD                 dwOptions,
                                            REGSAM                samDesired,
                                            LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                                            PHKEY                 phkResult,
                                            LPDWORD               lpdwDisposition);

typedef LONG (__stdcall *PYRegSetValueEx)(  HKEY    hKey,
                                            char *  lpValueName,
                                            DWORD   Reserved,
                                            DWORD   dwType,
                                            const BYTE    *lpData,
                                            DWORD   cbData);

typedef LONG (__stdcall *PYRegQueryValueEx)(    HKEY    hKey,
                                                char *  lpValueName,
                                                LPDWORD lpReserved,
                                                LPDWORD lpType,
                                                LPBYTE  lpData,
                                                LPDWORD lpcbData);
typedef LONG (__stdcall *PYRegCloseKey)(HKEY hKey);

typedef struct{
    HINSTANCE                  hREG;
    PYRegCreateKeyEx           yRegCreateKeyEx;
    PYRegSetValueEx            yRegSetValueEx;
    PYRegQueryValueEx          yRegQueryValueEx;
    PYRegCloseKey              yRegCloseKey;
}win_reg_api;



#elif defined(OSX_API)
/*****************************************************************************
  OSX SPECIFIC HEADER
 ****************************************************************************/
#include <IOKit/hid/IOHIDLib.h>


#elif defined(LINUX_API)
/*****************************************************************************
  LINUX SPECIFIC HEADER
 ****************************************************************************/
#include <libusb-1.0/libusb.h>
#endif

/*****************************************************************************
  MISC GLOBAL INCLUDES:
 ****************************************************************************/

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "yfifo.h"
#include "ythread.h"
#include "ykey.h"

/*****************************************************************************
  MEMORY MANAGEMENT FUNCTION:
 ****************************************************************************/
//#define YSAFE_MEMORY

#include "ymemory.h"
#ifdef YSAFE_MEMORY
#define yMalloc(size)                   ySafeMalloc(__FILE_ID__,__LINE__,size)
#define yFree(ptr)                      {ySafeFree(__FILE_ID__,__LINE__,ptr);ptr=NULL;}
#define yTracePtr(ptr)                  ySafeTrace(__FILE_ID__,__LINE__,ptr)
#ifndef YMEMORY_ALLOW_MALLOC
#undef malloc
#undef free
#define malloc(size)                    yForbiden_malloc(size)
#define free(ptr)                       yForbiden_free(ptr)
#endif
#else
#define yMalloc(size)                   malloc(size)
#define yFree(ptr)                      free(ptr)
#define yTracePtr(ptr)
#endif

#define yMemset(dst,val,size)           memset(dst,val,size)
#define yMemcpy(dst,src,size)           memcpy(dst,src,size)
#define yMemmove(dst,src,size)          memmove(dst,src,size)

#if defined(WINDOWS_API) && defined(_MSC_VER) && !defined(WINCE)
    #define YSTRCMP(A,B)                        strcmp(A,B)
    #define YSTRNCMP(A,B,len)                   strncmp(A,B,len)
    #define YSTRICMP(A,B)                       _stricmp(A,B)
    #define YSTRNICMP(A,B,len)                  _strnicmp(A,B,len)
    #define YSTRLEN(str)                        ((int)strlen(str))
#elif defined(WINDOWS_API) && defined(__BORLANDC__)
    #define YSTRCMP(A,B)                        strcmp(A,B)
    #define YSTRNCMP(A,B,len)                   strncmp(A,B,len)
    #define YSTRICMP(A,B)                       strcmpi(A,B)
    #define YSTRNICMP(A,B,len)                  strncmpi(A,B,len)
    #define YSTRLEN(str)                        ((int)strlen(str))
#elif defined(WINCE)
    #define YSTRCMP(A,B)                        strcmp(A,B)
    #define YSTRNCMP(A,B,len)                   strncmp(A,B,len)
    #define YSTRICMP(A,B)                       _stricmp(A,B)
    #define YSTRNICMP(A,B,len)                  _strnicmp(A,B,len)
    #define YSTRLEN(str)                        ((int)strlen(str))
#else
    #define YSTRCMP(A,B)                        strcmp(A,B)
    #define YSTRNCMP(A,B,len)                   strncmp(A,B,len)
    #define YSTRICMP(A,B)                       strcasecmp(A,B)
    #define YSTRNICMP(A,B,len)                  strncasecmp(A,B,len)
    #define YSTRLEN(str)                        ((int)strlen(str))
#endif

#define YSTRDUP(src)                        ystrdup_s(src)
#define YSTRCPY(dst,dstsize,src)            ystrcpy_s(dst,dstsize,src)
#define YSTRCAT(dst,dstsize,src)            ystrcat_s(dst,dstsize,src)
#define YSTRNCAT(dst,dstsize,src,len)       ystrncat_s(dst,dstsize,src,len)
#define YSTRNCPY(dst,dstsize,src,len)       ystrncpy_s(dst,dstsize,src,len)
#define YSPRINTF                            ysprintf_s
#define YVSPRINTF                           yvsprintf_s
char *ystrdup_s(const char *src);
YRETCODE ystrcpy_s(char *dst, unsigned dstsize, const char *src);
YRETCODE ystrncpy_s(char *dst,unsigned dstsize,const char *src,unsigned len);
YRETCODE ystrcat_s(char *dst, unsigned dstsize,const char *src);
YRETCODE ystrncat_s(char *dst, unsigned dstsize,const char *src,unsigned len);
int ysprintf_s(char *dst, unsigned dstsize,const char *fmt ,...);
int yvsprintf_s (char *dst, unsigned dstsize, const char * fmt, va_list arg );
int ymemfind(const u8 *haystack, u32 haystack_len, const u8 *needle, u32 needle_len);


//#define DEBUG_YAPI_REQ
//#define DEBUG_HAL
//#define DEBUG_DEV_ENUM
//#define DEBUG_DEV_ENUM_VERBOSE
//#define DEBUG_NET_ENUM
//#define DEBUG_NOTIFICATION
//#define DEBUG_NET_NOTIFICATION
//#define DEBUG_DUMP_PKT
//#define DEBUG_USB_TRAFIC
//#define TRACE_NET_HUB
//#define DEBUG_TRACE_FILE "c:\\tmp\\tracefile.txt"
//#define DEBUG_TCP
//#define DEBUG_MISSING_PACKET

#define MSC_VS2003 1310

#ifdef DEBUG_YAPI_REQ
#define YREQLOG  dbglog
#else
#if defined(_MSC_VER)
#if (_MSC_VER > MSC_VS2003)
#define YREQLOG(fmt,...)
#else
__forceinline void __YREQLOG(fmt,...){}
#define YREQLOG __YREQLOG
#endif
#else
#define YREQLOG(fmt,args...)
#endif
#endif

#ifdef DEBUG_HAL
#define HALLOG  dbglog
#else
#if defined(_MSC_VER)
#if (_MSC_VER > MSC_VS2003)
#define HALLOG(fmt,...)
#else
__forceinline void __HALLOG(fmt,...){}
#define HALLOG __HALLOG
#endif
#else
#define HALLOG(fmt,args...)
#endif
#endif

#ifdef DEBUG_TCP
#define TCPLOG  dbglog
#else
#if defined(_MSC_VER)
#if (_MSC_VER > MSC_VS2003)
#define TCPLOG(fmt,...)
#else
__forceinline void __TCPLOG(fmt,...){}
#define TCPLOG __TCPLOG
#endif
#else
#define TCPLOG(fmt,args...)
#endif
#endif

#ifdef DEBUG_DEV_ENUM
#define ENUMLOG  dbglog
#else
#if defined(_MSC_VER)
#if (_MSC_VER > MSC_VS2003)
#define ENUMLOG(fmt,...)
#else
__forceinline void __ENUMLOG(fmt,...){}
#define ENUMLOG __ENUMLOG
#endif
#else
#define ENUMLOG(fmt,args...)
#endif
#endif

#ifdef DEBUG_NET_ENUM
#define NETENUMLOG  dbglog
#else
#if defined(_MSC_VER)
#if (_MSC_VER > MSC_VS2003)
#define NETENUMLOG(fmt,...)
#else
__forceinline void __NETENUMLOG(fmt,...){}
#define NETENUMLOG __NETENUMLOG
#endif
#else
#define NETENUMLOG(fmt,args...)
#endif
#endif

int vdbglogf(const char *fileid,int line,const char *fmt,va_list args);
int dbglogf(const char *fileid,int line,const char *fmt,...);
#if defined(_MSC_VER)
#if (_MSC_VER > MSC_VS2003)
#define dbglog(...)      dbglogf(__FILE_ID__,__LINE__, __VA_ARGS__)
#else
__forceinline int __dbglog(const char* fmt,...) {
    int len;
    va_list args;

    va_start( args, fmt );
    len = vdbglogf("vs2003",__LINE__,fmt,args);
    va_end(args);
    return len;
}
#define dbglog __dbglog
#endif
#else
#define dbglog(args...)  dbglogf(__FILE_ID__,__LINE__, ## args)
#endif


#ifdef DEBUG_DUMP_PKT
void dumpAnyPacket(char *prefix,int ifaceno,USB_Packet *pkt);
#endif


#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

/*****************************************************************************
 MISC DEFINITION
 ****************************************************************************/
#ifdef WINDOWS_API
#define yApproximateSleep(ms)  Sleep(ms)
#else
#include <unistd.h>
#include <stdio.h>
#define yApproximateSleep(ms)          usleep(ms*1000)
#endif
//secure fopen
#if defined(_MSC_VER) &&  (_MSC_VER > MSC_VS2003)
#define YFOPEN(f,filename,mode) fopen_s(f,filename,mode)
#else
int YFOPEN(FILE** f, const char *filename, const char *mode);
#endif

#if 0
#if defined(WINDOWS_API) && (_MSC_VER)
#define YDEBUG_BREAK { __debugbreak();}
#else
#define YDEBUG_BREAK  {__asm__("int3");}
#endif
#else
#define YDEBUG_BREAK {}
#endif

#define YPANIC                  {dbglog("YPANIC:%s:%d\n",__FILE_ID__ , __LINE__);YDEBUG_BREAK}
#define YASSERT(x)              if(!(x)){dbglog("ASSERT FAILED:%s:%d\n",__FILE_ID__ , __LINE__);YDEBUG_BREAK}
#define YPROPERR(call)          {int tmpres=(call); if(YISERR(tmpres)) {return (YRETCODE)tmpres;}}
#define YERR(code)              ySetErr(code,errmsg,NULL,__FILE_ID__,__LINE__)
#define YERRTO(code,buffer)     ySetErr(code,buffer,NULL,__FILE_ID__,__LINE__)
#define YERRMSG(code,message)   ySetErr(code,errmsg,message,__FILE_ID__,__LINE__)
#define YERRMSGTO(code,message,buffer)   ySetErr(code,buffer,message,__FILE_ID__,__LINE__)
YRETCODE ySetErr(YRETCODE code, char *outmsg, const char *erreur, const char *file, u32 line);
int FusionErrmsg(int code,char *errmsg, const char *generr, const char *detailerr);


/*****************************************************************************
 OLD version fo protocol that are supported
****************************************************************************/

#define YPKT_VERSION_ORIGINAL_RELEASE    0x0202

/*****************************************************************************
 PERFORMANCE TEST DEFINITIONS (very old)
****************************************************************************/
typedef struct
{
    u64 totaltime;
    u64 count;
    u64 leave;
    u64 tmp;
} yPerfMon;

void  dumpYPerfEntry(yPerfMon *entry,const char *name);


/*****************************************************************************
 INTERNAL STRUCTURES and DEFINITIONS
****************************************************************************/

// MISC packet definitions
#pragma pack(push,1)
typedef struct{
    u8         dummy;
    USB_Packet pkt;
} OS_USB_Packet;
#pragma pack(pop)


#if defined(LINUX_API)
typedef struct {
    struct _yInterfaceSt    *iface;
    struct libusb_transfer  *tr;
    USB_Packet              tmppkt;
} linRdTr;
#endif

// packet queue stuff
typedef struct _pktItem{
    USB_Packet          pkt;
#ifdef DEBUG_PKT_TIMING
    u64                 time;
    u64                 ospktno;
#endif
    struct _pktItem     *next;
} pktItem;


typedef struct {
    pktItem             *first;
    pktItem             *last;
    int                 count;
    u64                 totalPush;
    u64                 totalPop;
    YRETCODE            status;
    char                errmsg[YOCTO_ERRMSG_LEN];
    yCRITICAL_SECTION   cs;
    yEvent              notEmptyEvent;
    yEvent              emptyEvent;
} pktQueue;

//pktQueue Helpers
void yPktQueueInit(pktQueue  *q);
void yPktQueueFree(pktQueue  *q);
void yPktQueueSetError(pktQueue  *q,YRETCODE code, const char * msg);

#ifdef OSX_API

typedef struct {
    IOHIDManagerRef     manager;
    yCRITICAL_SECTION   hidMCS;
} OSX_HID_REF;

#endif


#define NBMAX_NET_HUB               32
#define NBMAX_USB_DEVICE_CONNECTED  256
#define WIN_DEVICE_PATH_LEN         512
#define HTTP_RAW_BUFF_SIZE          (8*1024)
#define NB_LINUX_USB_TR             1

#define YWIN_EVENT_READ     0
#define YWIN_EVENT_INTERRUPT 1

typedef struct _yInterfaceSt {
    u16             vendorid;
    u16             deviceid;
    u16             ifaceno;
    u16             pkt_version;
    char            serial[YOCTO_SERIAL_LEN*2];
    struct {
        u32         yyySetupDone:1;
    } flags;
    pktQueue        rxQueue;
    pktQueue        txQueue;
#if defined(WINDOWS_API)
    char            devicePath[WIN_DEVICE_PATH_LEN];
    yThread         io_thread;
    HANDLE          wrHDL;
    OVERLAPPED      rdOL;
    HANDLE          rdHDL;
    HANDLE          EV[2];
    u32             rdpending;
    OS_USB_Packet   tmpd2hpkt;
    OS_USB_Packet   tmph2dpkt;
#elif defined(OSX_API)
    OSX_HID_REF         hid;
    CFStringRef         run_loop_mode;
    IOHIDDeviceRef      devref;
    USB_Packet          tmprxpkt;
#elif defined(LINUX_API)
    libusb_device           *devref;
    libusb_device_handle    *hdl;
    u8                      rdendp;
    u8                      wrendp;
    linRdTr                 rdTr[NB_LINUX_USB_TR];
#endif
} yInterfaceSt;


YRETCODE    yPktQueuePushD2H(yInterfaceSt *iface,const USB_Packet *pkt, char * errmsg);
YRETCODE    yPktQueueWaitAndPopD2H(yInterfaceSt *iface,pktItem **pkt,int ms,char * errmsg);
YRETCODE    yPktQueuePushH2D(yInterfaceSt *iface,const USB_Packet *pkt, char * errmsg);
YRETCODE    yPktQueuePeekH2D(yInterfaceSt *iface,pktItem **pkt);
YRETCODE    yPktQueuePopH2D(yInterfaceSt *iface,pktItem **pkt);

#define NBMAX_INTERFACE_PER_DEV     2
typedef enum
{
    YDEV_UNPLUGGED=0,           // device has been plugged by the past but is no more
                                // -> YDEV_WORKING  or YDEV_NOTRESPONDING
    YDEV_WORKING,               // device is plugged and running
                                // -> YDEV_UNPLUGGED
    YDEV_NOTRESPONDING          // device has not repsond to StartDevice and we will never try to speak with it
                                // -> none
} YDEV_STATUS;


typedef enum
{
    YENU_NONE,
    YENU_START,
    YENU_STOP,
    YENU_RESTART
} YENU_ACTION;

typedef enum
{
    YRUN_STOPED,
    YRUN_AVAIL,                 // device is available for a request
    YRUN_REQUEST,               // device has be reserved for a request
    YRUN_BUSY,                  // device is doing IO for the request
    YRUN_IDLE,                  // device is doing IO for the idle thread
    YRUN_ERROR,                 // device has been stopped because an IO error
} YRUN_STATUS;

typedef enum
{
    YHTTP_CLOSED,
    YHTTP_OPENED,
    YHTTP_CLOSE_BY_DEV,
    YHTTP_CLOSE_BY_API
} YHTTP_STATUS;


// structure that contain generic device information (usb and netowrk)
#define DEVGEN_LOG_ACTIVATED     1u
#define DEVGEN_LOG_PENDING       2u
#define DEVGEN_LOG_PULLING       4u
typedef struct  _yGenericDeviceSt {
    yStrRef             serial; // set only once at init -> no need to use the mutex
    u32                 flags;
    u32                 deviceLogPos;
    yFifoBuf            logFifo;
    u8*                 logBuffer;
    double              deviceTime;
} yGenericDeviceSt;

void initDevYdxInfos(int devYdxy, yStrRef serial);
void freeDevYdxInfos(int devYdx);


#define YIO_REMOTE_CLOSE 1u

typedef struct{
    u8      flags;
    u64     timeout;
    YUSBIO  hdl;
    yapiRequestAsyncCallback callback;
    void *context;
} USB_HDL;

#define NB_MAX_STARTUP_RETRY   5u

#define NEXT_YPKT_NO(current) ((current+1)& YPKTNOMSK)
#define NEXT_IFACE_NO(current,total) (current+1<total?current+1:0)

// structure that contain all information about a device
typedef struct  _yPrivDeviceSt{
    yCRITICAL_SECTION   acces_state;
    YUSBDEV             yhdl;       // unique YHANDLE to identify device during execution
    YDEV_STATUS         dStatus;    // detection status
    YENU_ACTION         enumAction; // action to triger at end of enumeration
    YRUN_STATUS         rstatus;    // running status of the device (valid only on working dev)
    char                errmsg[YOCTO_ERRMSG_LEN];
    unsigned int        nb_startup_retry;
    u64                 next_startup_attempt;
    USB_HDL             pendingIO;
    YHTTP_STATUS        httpstate;
    yDeviceSt           infos;      // device infos
    u32                 lastUtcUpdate;
    pktItem             *currxpkt;
    u8                  curxofs;
    pktItem             *curtxpkt;
    u8                  curtxofs;
    pktItem             tmptxpkt;
    u8                  lastpktno;
    int                 pktAckDelay;
    int                 currentIfaceNo;
    u8                  ifacesMap[NBMAX_INTERFACE_PER_DEV];
    yInterfaceSt        ifaces[NBMAX_INTERFACE_PER_DEV];
    char                *replybuf;      // Used to buffer request result
    int                 replybufsize;   // allocated size of replybuf
    yFifoBuf            http_fifo;
    u8                  *http_raw_buf;
    u8                  *devYdxMap;
    struct              _yPrivDeviceSt   *next;
} yPrivDeviceSt;

typedef struct  _DevEnum{
    int             nbifaces;
    yInterfaceSt    *ifaces[NBMAX_INTERFACE_PER_DEV];
} DevEnum;

typedef void (*yDevInfoCallback)(const yDeviceSt *infos);
typedef void (*yNotificCallback)(const char *serial, const char *funcid, const char *funcname, const char *funcval);

typedef enum {
    USB_THREAD_NOT_STARTED,
    USB_THREAD_RUNNING,
    USB_THREAD_MUST_STOP,
    USB_THREAD_STOPED
} USB_THREAD_STATE;

typedef enum {
    NET_HUB_DISCONNECTED=0,
    NET_HUB_TRYING,
    NET_HUB_ESTABLISHED,
    NET_HUB_FAILED,
    NET_HUB_TOCLOSE,
    NET_HUB_CLOSED
} NET_HUB_STATE;

// If made bigger than 255, change plenty of u8 into u16 and pray
#define MAX_YDX_PER_HUB 255
#define ALLOC_YDX_PER_HUB 256
// NetHubSt flags
#define NETH_F_MANDATORY                1
#define NETH_F_SEND_PING_NOTIFICATION   2

#define NET_HUB_NOT_CONNECTION_TIMEOUT   (6*1024)


typedef struct _NetHubSt {
    yUrlRef             url;            // hub base URL, or INVALID_HASH_IDX if unused
    u32                 flags;
    yStrRef             serial;
    WakeUpSocket        wuce;
    yThread             net_thread;
    // the following fields are for the notification helper thread only
    NET_HUB_STATE       state;
    int                 retryCount;
    struct _TcpReqSt    *notReq;
    u32                 notifAbsPos;
    u64                 lastAttempt;    // time of the last connection attempt (in ms)
    u64                 attemptDelay;   // delay until next attemps (in ms)
    yFifoBuf            fifo;           // notification fifo
    u8                  buffer[1024];   // buffer for the fifo
    // the following fields are used by hub net enum and notification helper thread
    u64                 devListExpires;
    u8                  devYdxMap[ALLOC_YDX_PER_HUB];   // maps hub's internal devYdx to our WP devYdx
    int                 writeProtected; // admin password detected
    u64                 lastTraffic;    // time of the last data received on the notification socket (in ms)
    // the following fields are used for authentication to the hub, and require mutex access
    yCRITICAL_SECTION   authAccess;
    char                *name;
    char                *user;
    char                *realm;
    char                *pwd;
    char                *nonce;
    char                *opaque;
    u8                  ha1[16];        // computed when realm is received if pwd is not NULL
    u32                 nc;             // reset each time a new nonce is received
} NetHubSt;


#define TCPREQ_KEEPALIVE       1
#define TCPREQ_IN_USE          2

typedef struct _TcpReqSt {
    NetHubSt            *hub;           // pointer to the NetHubSt handling the device
    yCRITICAL_SECTION   access;
    yEvent              finished;       // event seted when this request can be reused
    YSOCKET             skt;            // socket used to talk to the device
    char                *headerbuf;     // Used to store all lines of the HTTP header (with the double \r\n)
    int                 headerbufsize;  // allocated size of requestbuf
    char                *bodybuf;       // Used to store the body of the POST request
    int                 bodybufsize;    // allocated size of the body of the POST request
    int                 bodysize;       // effective size of the body of the POST request
    u8                  *replybuf;      // Used to buffer request result
    int                 replybufsize;   // allocated size of replybuf
    int                 replysize;      // write pointer within replybuf
    int                 replypos;       // read pointer within replybuf; -1 when not ready to start reading
    int                 retryCount;     // number of authorization attempts
    int                 errcode;        // in case an error occured
    char                errmsg[YOCTO_ERRMSG_LEN];
    u64                 open_tm;        // timestamp of the start of a connection used to detect timout of the device
                                        // (must be reset if we reuse the socket)
    u64                 read_tm;        // timestamp of the last received packet (must be reset if we reuse the socket)
    u64                 timeout_tm;     // the maximum time to live of this connection
    u32                 flags;          // flags for keepalive and no expiration
    YSOCKET             reuseskt;       // socket to reuse for next query, when keepalive is true
    yapiRequestAsyncCallback callback;
    void                *context;
} TcpReqSt;

#define SETUPED_IFACE_CACHE_SIZE 128


typedef struct {
    char        *serial;
    char        *firmwarePath;
    u8          *settings;
    int         settings_len;
    yThread     thread;
    int         global_progress; //-1:error 0-99:working 100:success
    char        global_message[YOCTO_ERRMSG_LEN]; // the last message or the error
    const char* fileid;
    int         line;
} FUpdateContext;




#define YCTX_OSX_MULTIPLES_HID 1
// structure that contain information about the API
typedef struct{
    //yapi CS
    yCRITICAL_SECTION   updateDev_cs;
    yCRITICAL_SECTION   handleEv_cs;
    yEvent              exitSleepEvent;
    // global inforation on all devices
    yCRITICAL_SECTION   generic_cs;
    yGenericDeviceSt    generic_infos[ALLOC_YDX_PER_HUB];
    // usb stuff
    yCRITICAL_SECTION   enum_cs;
    int                 detecttype;
    YUSBDEV             devhdlcount;
    yPrivDeviceSt       *devs;
    int                 nbdevs;
    int                 devs_capacity;
    yCRITICAL_SECTION   io_cs;
    u32                 io_counter;
    // network discovery info
    NetHubSt            nethub[NBMAX_NET_HUB];
    TcpReqSt            tcpreq[ALLOC_YDX_PER_HUB];  // indexed by our own DevYdx
    yRawNotificationCb  rawNotificationCb;
    yRawReportCb        rawReportCb;
    yRawReportV2Cb      rawReportV2Cb;
    yCRITICAL_SECTION   deviceCallbackCS;
    yCRITICAL_SECTION   functionCallbackCS;
    // SSDP stuff
    SSDPInfos           SSDP;            // socket used to talk to the device
    // Public callbacks
    yapiLogFunction             log;
    yapiDeviceLogCallback       logDeviceCallback;
    yapiDeviceUpdateCallback    arrivalCallback;
    yapiDeviceUpdateCallback    changeCallback;
    yapiDeviceUpdateCallback    removalCallback;
    yapiFunctionUpdateCallback  functionCallback;
    yapiTimedReportCallback     timedReportCallback;
    yapiHubDiscoveryCallback    hubDiscoveryCallback;
    // Programing api
    FUpdateContext      fuCtx;
    // OS specifics variables
    yInterfaceSt*       setupedIfaceCache[SETUPED_IFACE_CACHE_SIZE];
#if defined(WINDOWS_API)
    win_hid_api         hid;
    HANDLE              apiLock;
    yCRITICAL_SECTION   prevEnum_cs;
    int                 prevEnumCnt;
    yInterfaceSt        *prevEnum;
    win_reg_api         registry;
#elif defined(OSX_API)
    u32                 osx_flags;
    OSX_HID_REF         hid;
    CFRunLoopRef        usb_run_loop;
    pthread_t           usb_thread;
    USB_THREAD_STATE    usb_thread_state;
#elif defined(LINUX_API)
    yCRITICAL_SECTION   string_cache_cs;
    libusb_context      *libusb;
    pthread_t           usb_thread;
    USB_THREAD_STATE    usb_thread_state;
#endif
 } yContextSt;

#define TRACEFILE_NAMELEN  512

extern char  ytracefile[];
extern yContextSt  *yContext;

YRETCODE yapiPullDeviceLogEx(int devydx);
YRETCODE yapiPullDeviceLog(const char *serial);
YRETCODE yapiRequestOpen(YIOHDL *iohdl, const char *device, const char *request, int reqlen, yapiRequestAsyncCallback callback, void *context, char *errmsg);

/*****************************************************************
 * PLATFORM SPECIFIC USB code
*****************************************************************/


// for devices detection
int  yyyUSB_init(yContextSt *ctx,char *errmsg);
int  yyyUSB_stop(yContextSt *ctx,char *errmsg);
int  yyyUSBGetInterfaces(yInterfaceSt **ifaces,int *nbifaceDetect,char *errmsg);
int  yyyOShdlCompare( yPrivDeviceSt *dev, DevEnum *newdev);
int  yyySetup(yInterfaceSt *iface,char *errmsg);
YRETCODE  yyySendPacket( yInterfaceSt *iface,const USB_Packet *pkt,char *errmsg);
int  yyySignalOutPkt(yInterfaceSt *iface);
// close all stuff of setup
void yyyPacketShutdown(yInterfaceSt *iface);


/*****************************************************************************
  ENUMERATION RELATED FUNCTION
******************************************************************************/

//some early declarations
void wpSafeRegister( NetHubSt *hub, u8 devYdx, yStrRef serialref,yStrRef lnameref, yStrRef productref, u16 deviceid, yUrlRef devUrl,s8 beacon);
void wpSafeUpdate( NetHubSt *hub, u8 devYdx, yStrRef serialref,yStrRef lnameref, yUrlRef devUrl, s8 beacon);
void wpSafeUnregister(yStrRef serialref);

void ypUpdateUSB(const char *serial, const char *funcid, const char *funcname, int funclass, int funydx, const char *funcval);
void ypUpdateYdx(int devydx, Notification_funydx funInfo, const char *funcval);
void ypUpdateHybrid(const char *serial, Notification_funydx funInfo, const char *funcval);

/*****************************************************************
 * yStream API with cycling logic and yyPacket API
*****************************************************************/

/**********************************************************************
  GENERIC DEVICE LIST FUNCTION
 **********************************************************************/

// return the yDeviceSt *from a matching string (serial or name)
#define FIND_FROM_SERIAL 1
#define FIND_FROM_NAME   2
#define FIND_FROM_ANY    (FIND_FROM_SERIAL|FIND_FROM_NAME)
yPrivDeviceSt *findDev(const char *str,u32 flags);


// return the YHANDLE from a matching string (serial or name)
YUSBDEV findDevHdlFromStr(const char *str);
yPrivDeviceSt *findDevFromIOHdl(YIOHDL *hdl);
void devHdlInfo(YUSBDEV hdl,yDeviceSt *infos);

YRETCODE yUSBUpdateDeviceList(char *errmsg);
void     yUSBReleaseAllDevices(void);

/*****************************************************************************
  USB REQUEST FUNCTIONS
  ***************************************************************************/

int  yUsbInit(yContextSt *ctx,char *errmsg);
int  yUsbFree(yContextSt *ctx,char *errmsg);
int  yUsbIdle(void);
int  yUsbTrafficPending(void);
yGenericDeviceSt* yUSBGetGenericInfo(yStrRef devdescr);

int  yUsbOpenDevDescr(YIOHDL *ioghdl, yStrRef devdescr, char *errmsg);
int  yUsbOpen(YIOHDL *ioghdl, const char *device, char *errmsg);
int  yUsbSetIOAsync(YIOHDL *ioghdl, yapiRequestAsyncCallback callback, void *context, char *errmsg);
int  yUsbWrite(YIOHDL *ioghdl, const char *buffer, int writelen,char *errmsg);
int  yUsbReadNonBlock(YIOHDL *ioghdl, char *buffer, int len,char *errmsg);
int  yUsbReadBlock(YIOHDL *ioghdl, char *buffer, int len,u64 blockUntil,char *errmsg);
int  yUsbEOF(YIOHDL *ioghdl,char *errmsg);
int  yUsbClose(YIOHDL *ioghdl,char *errmsg);

int  yUSBGetBooloader(const char *serial, const char * name,  yInterfaceSt *iface,char *errmsg);

// Misc helper
u32 yapiGetCNonce(u32 nc);
YRETCODE  yapiHTTPRequestSyncStartEx_internal(YIOHDL *iohdl, const char *device, const char *request, int requestsize, char **reply, int *replysize, char *errmsg);
YRETCODE  yapiHTTPRequestSyncDone_internal(YIOHDL *iohdl, char *errmsg);
void yFunctionUpdate(YAPI_FUNCTION fundescr, const char *value);
void yFunctionTimedUpdate(YAPI_FUNCTION fundescr, double deviceTime, const u8 *report, u32 len);
#endif
