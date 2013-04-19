/*********************************************************************
 *
 * $Id: yproto.h 10877 2013-04-04 14:30:47Z mvuilleu $
 *
 * Definitions and prototype common to all supported OS
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

#ifndef  YPROTO_H
#define  YPROTO_H

#include "ydef.h"
#include "yapi.h"
#include "ytcp.h"
#ifdef WINDOWS_API
/**************************************************************
* WINDOWS SPECIFIC HEADER
***************************************************************/
#include <Windows.h>
#include <dbt.h>

#include <SetupAPI.h>
#ifdef _MSC_VER
#pragma comment(lib, "SetupApi.lib")
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
#define yFree(ptr)                      ySafeFree(__FILE_ID__,__LINE__,ptr)
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

#define YSTRCPY(dst,dstsize,src)            ystrcpy_s(dst,dstsize,src)
#define YSTRCAT(dst,dstsize,src)            ystrcat_s(dst,dstsize,src)
#define YSTRNCAT(dst,dstsize,src,len)       ystrncat_s(dst,dstsize,src,len)
#define YSTRNCPY(dst,dstsize,src,len)       ystrncpy_s(dst,dstsize,src,len)
#define YSPRINTF                            ysprintf_s
#define YVSPRINTF                           yvsprintf_s
YRETCODE ystrcpy_s(char *dst, unsigned dstsize,const char *src);
YRETCODE ystrncpy_s(char *dst,unsigned dstsize,const char *src,unsigned len);
YRETCODE ystrcat_s(char *dst, unsigned dstsize,const char *src);
YRETCODE ystrncat_s(char *dst, unsigned dstsize,const char *src,unsigned len);
int ysprintf_s(char *dst, unsigned dstsize,const char *fmt ,...);
int yvsprintf_s (char *dst, unsigned dstsize, const char * fmt, va_list arg );



//#define DEBUG_YAPI_REQ
//#define DEBUG_HAL
//#define DEBUG_DEV_ENUM
//#define DEBUG_DEV_ENUM_VERBOSE
//#define DEBUG_NET_ENUM
//#define DEBUG_NOTIFICATION
//#define DEBUG_NET_NOTIFICATION
//#define DEBUG_DUMP_PKT
//#define DEBUG_USB_TRAFIC
//#define DEBUG_DEVICE_LOCK

#ifdef DEBUG_YAPI_REQ
#define YREQLOG  dbglog
#else
#define YREQLOG(fmt,...)
#endif


#ifdef DEBUG_HAL
#define HALLOG  dbglog
#else
#define HALLOG(fmt,...)
#endif

#ifdef DEBUG_HAL
#define HALLOG  dbglog
#else
#define HALLOG(fmt,...)
#endif

#ifdef DEBUG_DEV_ENUM
#define ENUMLOG  dbglog
#else
#define ENUMLOG(fmt,...)
#endif

#ifdef DEBUG_NET_ENUM
#define NETENUMLOG  dbglog
#else
#define NETENUMLOG(fmt,...)
#endif

int dbglogf(const char *fileid,int line,const char *fmt,...);
#ifdef WINDOWS_API
#if defined(_MSC_VER)
#define dbglog(fmt,...)      dbglogf(__FILE_ID__,__LINE__,fmt, __VA_ARGS__)
#else
#define dbglog(fmt,args...)  dbglogf(__FILE_ID__,__LINE__,fmt, ## args)
#endif
#else
#define dbglog(fmt,args...)  dbglogf(__FILE_ID__,__LINE__,fmt, ## args)
#endif


#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#if 0
#if defined(WINDOWS_API) && (_MSC_VER)
#define YDEBUG_BREAK { _asm {int 3}}
#else
#define YDEBUG_BREAK  {__asm__("int3");}
#endif
#else
#define YDEBUG_BREAK {}
#endif

#define YPANNIC                 {dbglog("YPANIC:%s:%d\n",__FILE__ , __LINE__);YDEBUG_BREAK}
#define YASSERT(x)              if(!(x)){dbglog("ASSERT FAILED:%s:%d\n",__FILE__ , __LINE__);YDEBUG_BREAK}
#define YPROPERR(call)          {int tmpres=(call); if(YISERR(tmpres)) {return tmpres;}}
#define YERR(code)              ySetErr(code,errmsg,NULL,__FILE_ID__,__LINE__)
#define YERRMSG(code,message)   ySetErr(code,errmsg,message,__FILE_ID__,__LINE__)
YRETCODE ySetErr(YRETCODE code, char *outmsg, char *erreur,const char *file,u32 line);
int FusionErrmsg(int code,char *errmsg,char *generr,char *detailerr);
void dumpAnyPacket(char *prefix,int ifaceno,USB_Packet *pkt);
/*****************************************************************************
 MISC DEFINITION
 ****************************************************************************/
#ifdef WINDOWS_API

#define yySleep(ms)          Sleep(ms)
#if defined(WINCE) || !defined(_MSC_VER)
#define fopen_s(f,name,mode) ((*(f)=fopen((name),(mode))) == NULL ? -1 : 0)
#endif

#else

#include <unistd.h>
#include <stdio.h>
#define yySleep(ms)          usleep(ms*1000)
int fopen_s(FILE** f, const char *filename, const char *mode);

#endif



/*****************************************************************************
 OLD version fo protocol that are supported
****************************************************************************/

#define YPKT_VERSION_ORIGINAL_RELEASE    0x0202

/*****************************************************************************
 INTERNAL STRUCTURES and DEFINITIONS
****************************************************************************/

#ifdef DEBUG_PERF

typedef struct _stat_func{
    u32 count;
    u64 totaltime;
    u64 maxtime;
    u64 mintime;
    u64 tmp;
    const char *name;
}stat_func;

extern stat_func* allstats[];
#define YSTAT_COUNT (sizeof(stat_func)/sizeof(stat_func))
#define YSTAT_NAME(name)        (name ## _command)
#define YSTAT_REGISTER(name)    (&YSTAT_NAME(name) )

#define YSTAT_CREATE(name)      stat_func YSTAT_NAME(name) = {0,0,0,0xffffffffffffffff,0,TOSTRING(name)};
#ifdef  __cplusplus
#define YSTAT_EXTERN(name)      extern "C" {extern stat_func YSTAT_NAME(name);}
#else
#define YSTAT_EXTERN(name)      extern stat_func YSTAT_NAME(name);
#endif
#define YSTAT_ENTER(name)       {YSTAT_NAME(name).tmp = yapiGetTickCount();}
#define YSTAT_EXIT(name)        {ystat_exit(&YSTAT_NAME(name),yapiGetTickCount());}
#define YSTAT_CLEAR(name)       {ystat_clear(&YSTAT_NAME(name));}
#define YSTAT_DUMP(name)        {ystat_dump(&YSTAT_NAME(name));}
void ystat_exit(stat_func *stat,u64 tick);
void ystat_clear(stat_func *stat);
void ystat_dump(stat_func *stat);
#else
#define YSTAT_CREATE(name)
#define YSTAT_EXTERN(name)
#define YSTAT_ENTER(name)
#define YSTAT_EXIT(name)
#define YSTAT_CLEAR(name)
#define YSTAT_DUMP(name)
#endif



// ON WINDOWS PACKET NEED ONE BYTE FOR THE REQUEST ID
#pragma pack(push,1)
typedef struct{
    u8         dummy;
    USB_Packet pkt;
} OS_USB_Packet;
#pragma pack(pop)


#define NBMAX_NET_HUB               32
#define NBMAX_USB_DEVICE_CONNECTED  256
#define WIN_DEVICE_PATH_LEN         512
#define HTTP_RAW_BUFF_SIZE          (8*1024)

#define NB_LINUX_USB_TR             1



typedef struct _pktItem{
    USB_Packet      pkt;
    struct _pktItem   *next;
} pktItem;


typedef struct {
    pktItem     *first;
    pktItem     *last;
    int         count;
} pktQueue;

#if defined(LINUX_API)
typedef struct {
    struct _yInterfaceSt    *iface;
    struct libusb_transfer  *tr;
    USB_Packet              tmppkt;
} linRdTr;
#endif

typedef struct _yInterfaceSt {
    u16             vendorid;
    u16             deviceid;
    u16             ifaceno;
    u16             pkt_version;
    char            serial[YOCTO_SERIAL_LEN*2];

    struct {
        int         yyySetupDone:1;
    } flags;
    pktQueue        rx_ok;
    pktItem         *txqueue;
    pktItem         static_txqueue;
    yCRITICAL_SECTION   rdCS;
#if defined(WINDOWS_API)
    HANDLE          wrHDL;
    OVERLAPPED      rdOL;
    HANDLE          rdHDL;
    HANDLE          rdEV;
    u8              rdpending;
    OS_USB_Packet   tmpd2hpkt;
    OS_USB_Packet   tmph2dpkt;
    char            devicePath[WIN_DEVICE_PATH_LEN];
#elif defined(OSX_API)
    yCRITICAL_SECTION   yyyCS;
    IOHIDDeviceRef      devref;
    CFStringRef         run_loop_mode;
    USB_Packet          tmprxpkt;
#elif defined(LINUX_API)
    libusb_device           *devref;
    libusb_device_handle    *hdl;
    u8                      rdendp;
    u8                      wrendp;
    linRdTr                 rdTr[NB_LINUX_USB_TR];
#endif

} yInterfaceSt;


#define NBMAX_INTERFACE_PER_DEV     2
typedef enum
{
    YDEV_UNPLUGED,              // device has been plugged by the past but is no more
                                // -> YDEV_ARRIVAL
    YDEV_ALLREADY_THERE,        // temporary state (used only during enumeration)
                                // the device is working at the beginning of the enumeration
                                // ->YDEV_WORKING, YDEV_UNPLUGED
    YDEV_ARRIVAL,               // temporary stat (used during enumeration) a new plug has been detected
                                // -> YDEV_WORKING
    YDEV_WORKING,               // device is plugged and running
                                // -> YDEV->UNPLUGET, YDEV_ALLREADY_THERE
    YDEV_NOTRESPONDING          // device has not repsond to StartDevice and we will never try to speak with it
                                // -> none
} YDEV_STATUS;

typedef enum
{
    YRUN_STOPED,
    YRUN_AVAIL,                 // device is available for a request
    YRUN_REQUEST,               // device has be opened for a request
    YRUN_BUSSY,                 // device is doing IO for the request
    YRUN_IDLE,                 // device is doing IO for the idle thread
    YRUN_ERROR,                 // device has been stopped because an IO error
} YRUN_STATUS;

typedef enum
{
    YHTTP_CLOSED,
    YHTTP_OPENED,
    YHTTP_CLOSE_BY_DEV,
    YHTTP_CLOSE_BY_API
} YHTTP_STATUS;

#ifdef DEBUG_DEVICE_LOCK
typedef struct{
    char    *file;
    int     line;
} Flock;
#endif


#define NB_MAX_STARTUP_RETRY   5


// structure that contain all information about a device
typedef struct  _yPrivDeviceSt{
    yCRITICAL_SECTION   acces;
    YUSBDEV             yhdl;       // unique YHANDLE to identify device during execution
    YDEV_STATUS         dstatus;    // detection status
    YRUN_STATUS         rstatus;    // running status of the device (valid only on working dev)
#ifdef DEBUG_DEVICE_LOCK
    Flock               lastopen;
#endif
    unsigned int        nb_startup_retry;
    u64                 next_startup_attempt;
    YUSBIO              iohdl;      // handle open/read/write/close access (valid only if rstatus== bussy)
    u8                  isAsyncIO;  // when set to true, TCP content can be safely discarded
    YHTTP_STATUS        httpstate;
    yDeviceSt           infos;      // device infos

    pktItem             *currxpkt;
    u8                  curxofs;
    pktItem             *curtxpkt;
    u8                  curtxofs;
    pktItem             tmptxpkt;
    u8                  lastpktno;

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

typedef struct _NetHubSt {
    yUrlRef             url;            // hub base URL, or INVALID_HASH_IDX if unused
    // the following fields are for the notification helper thread only
    NET_HUB_STATE       state;
    int                 retryCount;
    struct _TcpReqSt    *notReq;
    u32                 notifAbsPos;
    u64                 retryWait;
    yFifoBuf            fifo;           // notification fifo
    u8                  buffer[1024];   // buffer for the fifo
    // the following fields are used by hub net enum and notification helper thread
    u64                 devListExpires;
    u8                  devYdxMap[ALLOC_YDX_PER_HUB];   // maps hub's internal devYdx to our WP devYdx
    int                 writeProtected; // admin password detected
    // the following fields are used for authentication to the hub, and require mutex access
    yCRITICAL_SECTION   authAccess;
    char                *user;
    char                *realm;
    char                *pwd;
    char                *nonce;
    char                *opaque;
    u8                  ha1[16];        // computed when realm is received if pwd is not NULL
    u32                 nc;             // reset each time a new nonce is received
} NetHubSt;

typedef struct _TcpReqSt {
    NetHubSt            *hub;           // pointer to the NetHubSt handling the device
    yCRITICAL_SECTION   access;
    int                 isAsyncIO;      // when set to true, TCP content is handled by hub thread
    YSOCKET             skt;            // socket used to talk to the device
    char                *headerbuf;     // Used to store all lines of the HTTP header (with the double \r\n)
    int                 headerbufsize;  // allocated size of requestbuf
    char                *bodybuf;       // Used to store the body of the POST request
    int                 bodybufsize;    // allocated size of the body of the POST request
    int                 bodysize;       // effective size of the body of the POST request
    char                *replybuf;      // Used to buffer request result
    int                 replybufsize;   // allocated size of replybuf
    int                 replysize;      // write pointer within replybuf
    int                 replypos;       // read pointer within replybuf; -1 when not ready to start reading
    int                 retryCount;     // number of authorization attempts
    int                 errcode;        // in case an error occured
} TcpReqSt;

// structure that contain information about the API
typedef struct{
    //yapi CS
    yCRITICAL_SECTION   updateDev_cs;
    yCRITICAL_SECTION   handleEv_cs;
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
    WakeUpSocket        wuce;
    yThread             net_thread;
    yRawNotificationCb  rawNotificationCb;
    yCRITICAL_SECTION   deviceCallbackCS;
    yCRITICAL_SECTION   functionCallbackCS;
    // Public callbacks
    yapiLogFunction             log;
    yapiDeviceUpdateCallback    logDeviceCallback;
    yapiDeviceUpdateCallback    arrivalCallback;
    yapiDeviceUpdateCallback    changeCallback;
    yapiDeviceUpdateCallback    removalCallback;
    yapiFunctionUpdateCallback  functionCallback;
    // OS specifics variables
#if defined(WINDOWS_API)
    win_hid_api         hid;
    HANDLE              apiLock;
    yThread             usb_thread;
	int					prevEnumCnt;
	yInterfaceSt		*prevEnum;
#elif defined(OSX_API)
    IOHIDManagerRef     hidM;
    yCRITICAL_SECTION   hidMCS;
    CFRunLoopRef        usb_run_loop;
    pthread_t           usb_thread;
    USB_THREAD_STATE    usb_thread_state;
#elif defined(LINUX_API)
    libusb_context      *libusb;
    pthread_t           usb_thread;
    USB_THREAD_STATE    usb_thread_state;
#endif
 } yContextSt;

#define TRACEFILE_NAMELEN  512

extern char  ytracefile[];
extern yContextSt  *yContext;

/*****************************************************************
 * PLATFORM SPECIFIC USB code
*****************************************************************/

#ifdef WINDOWS_API
int yWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif


// for devices detection
int  yUSB_init(yContextSt *ctx,char *errmsg);
int  yUSB_stop(yContextSt *ctx,char *errmsg);
int  yUSBGetInterfaces(yInterfaceSt **ifaces,int *nbifaceDetect,char *errmsg);
int  yyyOShdlCompare( yPrivDeviceSt *dev, DevEnum *newdev);
// setup all stuff butt does not listen for new packet
void yyyInitPktQueue(yInterfaceSt  *iface);
void yyyFreePktQueue(yInterfaceSt  *iface);
int  yyySetup(yInterfaceSt *iface,char *errmsg);
// listen for packet
int  yyyRead(yInterfaceSt *iface,char *errmsg);
// check if we have received a new packet
int  yyyReadIdle(yInterfaceSt *iface,char *errmsg);
// cancel listen packet
int  yyyReadStop(yInterfaceSt *iface,char *errmsg);
// send a packet
int  yyyWrite(yInterfaceSt *iface,USB_Packet *pkt, char *errmsg);
// close all stuff of setup
void yyyPacketShutdown(yInterfaceSt *iface);



/*****************************************************************************
  ENUMERATION RELATED FUNCTION
  ***************************************************************************/

int devGetAcces(yPrivDeviceSt *dev,int waitacces);
void devReleaseAcces(yPrivDeviceSt *dev);
void enuResetDStatus(void);
yPrivDeviceSt* enuFindDevSlot(yInterfaceSt *iface);
//thread safe because only modified only by yDetectDevcies which is not reentrant
void enuUpdateDStatus(void);

yPrivDeviceSt* AllocateDevice(void);
void FreeDevice(yPrivDeviceSt *dev);
int  StartDevice(yPrivDeviceSt *dev,char *errmsg);
int  StopDevice(yPrivDeviceSt *dev,char *errmsg);


//some early declarations
//yPrivDeviceSt *findDevFromSerial(const char *serial);
void wpUpdateUSB(const char *serial,const char *logicalname, u8 beacon);
void wpUnregisterUSB(const char *serial);
void ypUpdateUSB(const char *serial, const char *funcid, const char *funcname, int funydx, const char *funcval);
void ypUpdateYdx(int devydx, int funydx, const char *funcval);
void ypUpdateHybrid(const char *serial, int funydx, const char *funcval);

/*****************************************************************
 * yStream API with cycling logic
*****************************************************************/


//yypacket
void  yyPushNewPkt(yInterfaceSt *iface,USB_Packet *pkt);
// this function copy the pkt into the iterface out queue and send the packet
int yySendPacket( yInterfaceSt *iface,pktItem *qpkt,char *errmsg);
int yyGetTopPkt(yInterfaceSt *iface, pktItem **ptr, int blocking,char *errmsg);
void yyPopPkt(yInterfaceSt *iface, pktItem *ptr);
int yyPacketSetup(yPrivDeviceSt *dev, yInterfaceSt  *iface,int idx,char *errmsg);

//ypacket
int ySendPacket(yPrivDeviceSt *dev,pktItem *qpkt,char *errmsg);
int yPacketStopRead(yPrivDeviceSt *dev,char *errmsg);
void yPacketShutdown(yPrivDeviceSt *dev);

//ystream
int yStreamReOpen(yPrivDeviceSt *dev,char *errmsg);
int yStreamGetTxBuff(yPrivDeviceSt *dev, u8 **data, u8 *maxsize, char *errmsg);
int yStreamTransmit(yPrivDeviceSt *dev, u8 proto,u8 size,char *errmsg);
int yStreamFlush(yPrivDeviceSt *dev,char *errmsg);
int yStreamClose(yPrivDeviceSt *dev,char *errmsg);
int yDispatchReceive(yPrivDeviceSt *dev,char *errmsg);

/**********************************************************************
  GENERIC DEVICE LIST FUNCTION
 **********************************************************************/


// return the yDeviceSt *from a matching string (serial or name)
yPrivDeviceSt *findDevFromStr(const char *str);

// return the YHANDLE from a matching string (serial or name)
YUSBDEV findDevHdlFromStr(const char *str);
yPrivDeviceSt *findDevFromIOHdl(YUSBIO hdl);
void devHdlInfo(YUSBDEV hdl,yDeviceSt *infos);

YRETCODE yUSBUpdateDeviceList(char *errmsg);
void     yUSBReleaseAllDevices(void);

/*****************************************************************************
  USB REQUEST FUNCTIONS
  ***************************************************************************/
int  yUsbIdle(void);
int  yUsbTrafficPending(void);
int  yUsbOpen(YIOHDL *ioghdl, const char *device, char *errmsg);
int  yUsbSetIOAsync(YIOHDL *ioghdl, char *errmsg);
int  yUsbWrite(YIOHDL *ioghdl, const char *buffer, int writelen,char *errmsg);
int  yUsbReadNonBlock(YIOHDL *ioghdl, char *buffer, int len,char *errmsg);
int  yUsbReadSelect(YIOHDL *ioghdl, u64 mstimeout,char *errmsg);
int  yUsbEOF(YIOHDL *ioghdl,char *errmsg);
int  yUsbClose(YIOHDL *ioghdl,char *errmsg);
int  yUSBGetBooloader(const char *serial, const char * name,  yInterfaceSt *iface,char *errmsg);

#endif
