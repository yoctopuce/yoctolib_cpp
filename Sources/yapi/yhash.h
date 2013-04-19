/*********************************************************************
 *
 * $Id: yhash.h 9109 2012-12-16 00:17:29Z mvuilleu $
 *
 * Simple hash tables and device/function information store
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

#ifndef YHASH_H
#define YHASH_H

#ifdef  __cplusplus
extern "C" {
#endif
    
#include "ydef.h"

//#define DEBUG_WP_LOCK

#define HASH_BUF_SIZE 28
#define HASH_BUF_POW   5 /* HASH_BUF_POW = log_2(HASH_BUF_SIZE+2+2) */
#ifdef C30
#ifdef YHUB
#define NB_MAX_HASH_ENTRIES 1023     /* keep hash table size <32KB on Yocto-Hub */
#else
#define NB_MAX_HASH_ENTRIES 320      /* keep hash table size <10KB on non-hub devices */
#endif
#else
#define NB_MAX_HASH_ENTRIES 8192
#endif
#define NB_MAX_DEVICES       256

#define YSTRREF_EMPTY_STRING  0x00ff /* yStrRef value for the empty string    */
#define YSTRREF_MODULE_STRING 0x0020 /* yStrRef value for the string 'Module' */

typedef enum {
    Y_WP_SERIALNUMBER,
    Y_WP_LOGICALNAME,
    Y_WP_PRODUCTNAME,
    Y_WP_PRODUCTID,
    Y_WP_NETWORKURL,
    Y_WP_BEACON,
    Y_WP_INDEX
} yWPAttribute;

#define YBLKID_WPENTRY    0xf0
#define YBLKID_YPENTRY    0xf1
#define YBLKID_YPCATEG    0xf2
#define YBLKID_YPARRAY    0xf3

typedef struct {
    u8          devYdx;
    u8          blkId;
    yBlkHdl     nextPtr;
    yStrRef     serial;
    yStrRef     name;
    yStrRef     product;
    u16         devid;
    yUrlRef     url;
    u16         flags;
} yWhitePageEntry;

// WP entry flags
#define YWP_MARK_FOR_UNREGISTER 0x02
#define YWP_BEACON_ON           0x01
    
typedef struct {
    u8          posYdx;
    u8          blkId;
    yBlkHdl     nextPtr;
    yBlkHdl     entries[6];
} yYellowPageArray;

typedef struct {
    u8          funYdx;
    u8          blkId;
    yBlkHdl     nextPtr;
    union {
      struct {
        yStrRef serialNum;
        yStrRef funcId;
      };
      YAPI_FUNCTION hwId;
    };
    yStrRef     funcName;
    union {
        char    funcVal[YOCTO_PUBVAL_SIZE];
        u16     funcValWords[YOCTO_PUBVAL_SIZE/2];
    };
} yYellowPageEntry;

typedef struct {
    u8          catYdx;
    u8          blkId;
    yBlkHdl     nextPtr;
    yStrRef     name;
    yBlkHdl     entries;
} yYellowPageCateg;

typedef union{
    struct {
        u16     hash;
        yHash   next;
        u8      buff[HASH_BUF_SIZE];
    };
    union {
        struct {
            u8      ydx;
            u8      blkId;
            yBlkHdl nextPtr;
        };
        yWhitePageEntry     wpEntry;
        yYellowPageEntry    ypEntry;
        yYellowPageCateg    ypCateg;
        yYellowPageArray    ypArray;
    } blk[2];
} YHashSlot;

extern char SerialNumberStr[YOCTO_SERIAL_LEN];
extern yStrRef SerialRef;
extern yBlkHdl yWpListHead;
extern yBlkHdl yYpListHead;

#define YMAX_HUB_URL_DEEP           11
#define YOCTO_HOSTNAME_NAME         (HASH_BUF_SIZE*2+2)

typedef struct{
    union{
        struct{
            yStrRef   host;
            yStrRef   domaine;
            u16       port;
        }byname;
        struct{
            yStrRef   ip;
            yHash     invalid;
            u16       port;
        }byip;
        struct{
            yHash     invalid1;
            yHash     invalid2;
            yStrRef   serial;
        }byusb;
    };
    yStrRef path[YMAX_HUB_URL_DEEP];
} yAbsUrl;

typedef enum {
    USB_URL,
    IP_URL,
    NAME_URL
} yAsbUrlType;

void  yHashInit(void);
void  yHashFree(void);
yHash yHashPutBuf(const u8 *buf, u16 len);
yHash yHashPutStr(const char *str);
yHash yHashTestBuf(const u8 *buf, u16 len);
yHash yHashTestStr(const char *str);
void  yHashGetBuf(yHash yhash, u8 *destbuf, u16 bufsize);
void  yHashGetStr(yHash yhash, char *destbuf, u16 bufsize);
u16   yHashGetStrLen(yHash yhash);
char  *yHashGetStrPtr(yHash yhash);
yUrlRef yHashUrlFromRef(yUrlRef urlref, const char *rootUrl, u8 testonly,char *errmsg);
yUrlRef yHashUrlUSB(const char *serial,const char *rootUrl, u8 testonly,char *errmsg);
yUrlRef yHashUrl(const char *host, const char *rootUrl, u8 testonly,char *errmsg);
yAsbUrlType  yHashGetUrlPort(yUrlRef urlref, char *url,u16 *port);
yBlkHdl yBlkAlloc(void);
void    yBlkFree(yBlkHdl hdl);
u16     yBlkListLength(yBlkHdl hdl);
yBlkHdl yBlkListSeek(yBlkHdl hdl, u16 pos);
int     wpRegister(yStrRef serial, yStrRef logicalName, yStrRef productName, u16 productId, yUrlRef devUrl, s8 beacon);
yStrRef wpGetAttribute(yBlkHdl hdl, yWPAttribute attridx);
#ifdef DEBUG_WP_LOCK
void    wpPreventUnregisterDbg(const char *file, u32 line);    
void    wpAllowUnregisterDbg(const char *file, u32 line);
#define wpPreventUnregister()   wpPreventUnregisterDbg(__FILE__,__LINE__)
#define wpAllowUnregister()     wpAllowUnregisterDbg(__FILE__,__LINE__)
#else
void    wpPreventUnregisterEx(void);    
void    wpAllowUnregisterEx(void);
#define wpPreventUnregister()   wpPreventUnregisterEx()
#define wpAllowUnregister()     wpAllowUnregisterEx()
#endif
int     wpMarkForUnregister(yStrRef serial);
u16     wpEntryCount(void);
int     wpGetDevYdx(yStrRef serial);
YAPI_DEVICE wpSearch(const char *device_str);
YAPI_DEVICE wpSearchByUrl(const char *host, const char *rootUrl);
int     wpGetAllDevUsingHubUrl( yUrlRef hubUrl, yStrRef *buffer,int sizeInStrRef);
int     wpGetDeviceInfo(YAPI_DEVICE devdesc, u16 *deviceid, char *productname, char *serial, char *logicalname, u8 *beacon);
yUrlRef wpGetDeviceUrlRef(YAPI_DEVICE devdesc);
int     wpGetDeviceUrl(YAPI_DEVICE devdesc, char *roothubserial, char *request, int requestsize, int *neededsize);
int     ypRegister(yStrRef categ, yStrRef serial, yStrRef funcId, yStrRef funcName, int funYdx, const char *funcVal);
int     ypRegisterByYdx(u8 devYdx, u8 funYdx, const char *funcVal, YAPI_FUNCTION *fundesc);
void    ypGetCategory(yBlkHdl hdl, char *name, yBlkHdl *entries);
int     ypGetAttributes(yBlkHdl hdl, yStrRef *serial, yStrRef *funcId, yStrRef *funcName, char *funcVal);
YAPI_FUNCTION ypSearch(const char *class_str, const char *func_str);
int     ypGetFunctions(const char *class_str, YAPI_DEVICE devdesc, YAPI_FUNCTION prevfundesc,
                       YAPI_FUNCTION *buffer,int maxsize,int *neededsize);
int     ypGetFunctionInfo(YAPI_FUNCTION fundesc, char *serial, char *funcId, char *funcName, char *funcVal);

#ifdef  __cplusplus
}
#endif
#endif


