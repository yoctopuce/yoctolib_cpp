/*********************************************************************
 *
 * $Id: yapi.c 11086 2013-04-15 15:38:10Z seb $
 *
 * Implementation of public entry points to the low-level API
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
#define __FILE_ID__  "yapi"
// do not use microsoft secure string 
#define _CRT_SECURE_NO_DEPRECATE
#include "yapi.h"
#include "yproto.h"
#include "yhash.h"
#include "yjson.h"
#include "yprog.h"
#ifndef WINDOWS_API
#include <sys/time.h>
#endif

//#define DEBUG_NET_NOTIFICATION

#ifdef DEBUG_PERF

void ystat_exit(stat_func *stat,u64 tick)
{
    u64 delta = tick - stat->tmp;
    if(delta > stat->maxtime)
        stat->maxtime =  delta;
    if(delta < stat->mintime)
        stat->mintime =delta;
    stat->count++;
    stat->totaltime += delta;
}
void ystat_clear(stat_func *stat)
{
    stat->count = 0;
    stat->totaltime = 0;
    stat->maxtime = 0;
    stat->mintime = 0xffffffffffffffff;
}

void ystat_dump(stat_func *stat)
{
    dbglog("STAT (%s) : %d call in %d ms (min:%d  max:%d)\n",
        stat->name,stat->count,stat->totaltime,stat->mintime,stat->maxtime);
}



#endif

/*****************************************************************************
 Internal functions for hub enumeration
 ****************************************************************************/

typedef enum
{
    ENU_HTTP_START,
    ENU_API,
    ENU_NETWORK_START,
    ENU_NETWORK,
    ENU_NET_ADMINPWD,
    ENU_SERVICE,
    ENU_WP_START,
    ENU_WP_ARRAY,
    ENU_WP_ENTRY,
    ENU_WP_SERIAL,
    ENU_WP_LOGICALNAME,
    ENU_WP_PRODUCTNAME,
    ENU_WP_PRODUCTID,
    ENU_WP_DEVURL,
    ENU_WP_BEACON,
    ENU_WP_INDEX,
    ENU_YP_CONTENT,
    ENU_YP_TYPE,
    ENU_YP_TYPE_LIST,
    ENU_YP_ARRAY,
    ENU_YP_ENTRY,
    ENU_YP_HARDWAREID,
    ENU_YP_LOGICALNAME,
    ENU_YP_PRODUCTNAME,
    ENU_YP_ADVERTISEDVALUE,
    ENU_YP_INDEX
} ENU_PARSE_STATE;


#ifdef DEBUG_NET_ENUM
const char * ENU_PARSE_STATE_STR[]=
{
    "ENU_HTTP_START",
    "ENU_SERVICE",
    "ENU_WP_START",
    "ENU_WP_ARRAY",
    "ENU_WP_ENTRY",
    "ENU_WP_SERIAL",
    "ENU_WP_LOGICALNAME",
    "ENU_WP_PRODUCTNAME",
    "ENU_WP_PRODUCTID",
    "ENU_WP_DEVURL",
    "ENU_WP_BEACON",
    "ENU_WP_INDEX",
    "ENU_YP_CONTENT",
    "ENU_YP_TYPE",
    "ENU_YP_TYPE_LIST",
    "ENU_YP_ARRAY",
    "ENU_YP_ENTRY",
    "ENU_YP_HARDWAREID",
    "ENU_YP_LOGICALNAME",
    "ENU_YP_PRODUCTNAME",
    "ENU_YP_ADVERTISEDVALUE",
    "ENU_YP_INDEX"
};
#endif

typedef struct {
    NetHubSt *hub;
    yUrlRef baseurl;
    ENU_PARSE_STATE state;
    yStrRef serial;
    yStrRef logicalName;
    union {
        struct {
            yStrRef productName;
            u16     productId;
            yUrlRef hubref;
            s8      beacon;
            u8      devYdx;
        };
        struct{
            yStrRef ypCateg;
            yStrRef funcId;
            char    advertisedValue[YOCTO_PUBVAL_LEN];
            u8      funYdx;
        };
    };
    int     nbKnownDevices;
    yStrRef *knownDevices;
}ENU_CONTEXT;



static void wpRegisterNet(ENU_CONTEXT *enus)
{
    int i;

    for(i=0; i<enus->nbKnownDevices ; i++){
        if(enus->knownDevices[i] != INVALID_HASH_IDX &&
            enus->knownDevices[i] == enus->serial){
                // mark the device as present (we sweep it later)
                //dbglog("wpRegisterNet %X was present\n",enus->serial);
                enus->knownDevices[i] = INVALID_HASH_IDX;
                break;
        }
    }
    if(i==enus->nbKnownDevices){
        wpRegister(enus->serial, enus->logicalName , enus->productName,
                   enus->productId, enus->hubref, enus->beacon);
        ypRegister(YSTRREF_MODULE_STRING, enus->serial, yHashPutStr("module"), enus->logicalName, -1, NULL);
        if(enus->devYdx < MAX_YDX_PER_HUB) {
            // Update hub-specific devYdx mapping between enus->devYdx and our wp devYdx
            enus->hub->devYdxMap[enus->devYdx] = wpGetDevYdx(enus->serial);
        }
        // Forward high-level notification to API user
        if(yContext->arrivalCallback)
            yContext->arrivalCallback(enus->serial);
        //dbglog("wpRegisterNet %X on %X\n",enus->serial,enus->hubref);
    } else{
        // Update white pages
        if(wpRegister(enus->serial, enus->logicalName, INVALID_HASH_IDX, 0, INVALID_HASH_IDX, enus->beacon)){
            ypRegister(YSTRREF_MODULE_STRING, enus->serial, yHashPutStr("module"), enus->logicalName, -1, NULL);
            if(enus->devYdx < MAX_YDX_PER_HUB) {
                // Update hub-specific devYdx mapping between enus->devYdx and our wp devYdx
                enus->hub->devYdxMap[enus->devYdx] = wpGetDevYdx(enus->serial);
            }
            // Forward high-level notification to API user
            if(yContext->changeCallback)
                yContext->changeCallback(enus->serial);
        }
    }
}

static void wpUnregisterNet(yStrRef serialref)
{
    int devydx;
    
    //dbglog("wpUnregisterNet %X:\n",serialref);
    if(serialref == INVALID_HASH_IDX) return;
    wpPreventUnregister();
    if(wpMarkForUnregister(serialref)){
        // Forward high-level notification to API user before deleting data
        if(yContext->removalCallback) {
            yEnterCriticalSection(&yContext->deviceCallbackCS);
            yContext->removalCallback(serialref);
            yLeaveCriticalSection(&yContext->deviceCallbackCS);
        }
        // Free device tcp structure, if needed
        devydx = wpGetDevYdx(serialref);
        if(devydx >= 0) {
            yTcpFreeReq(&yContext->tcpreq[devydx]);
        }        
    }
    wpAllowUnregister();
}

static void ypUpdateNet(ENU_CONTEXT *enus)
{
    if(ypRegister(enus->ypCateg, enus->serial, enus->funcId, enus->logicalName, enus->funYdx, enus->advertisedValue)){
        // Forward high-level notification to API user
        if(yContext->functionCallback) {
            yEnterCriticalSection(&yContext->functionCallbackCS);
            yContext->functionCallback(((s32)enus->funcId << 16) | enus->serial,enus->advertisedValue);
            yLeaveCriticalSection(&yContext->functionCallbackCS);
        }
    }
}


static int yEnuJson(ENU_CONTEXT *enus, yJsonStateMachine *j)
{
    char *point;
#ifdef DEBUG_NET_ENUM
    //dbglog("%s: %s %s\n",ENU_PARSE_STATE_STR[enus->state],yJsonStateStr[j->st],j->token);
#endif
    switch(enus->state){
        case ENU_HTTP_START:
            if(j->st != YJSON_HTTP_READ_CODE || YSTRCMP(j->token,"200")){
                return YAPI_IO_ERROR;
            }
            enus->state = ENU_API;
            break;
        case ENU_API:
            if(j->st !=YJSON_PARSE_MEMBNAME)
                break;
            if(YSTRCMP(j->token,"network")==0){
                enus->state = ENU_NETWORK_START;
            } else if(YSTRCMP(j->token,"services")==0){
                enus->state = ENU_SERVICE;
            } else{
                yJsonSkip(j,1);
            }
            break;
        case ENU_NETWORK_START:
            if(j->st == YJSON_PARSE_STRUCT){
				enus->state = ENU_NETWORK;
            }
            break;
        case ENU_NETWORK:
            if(j->st == YJSON_PARSE_STRUCT){
				enus->state = ENU_API;
            } else if(j->st == YJSON_PARSE_MEMBNAME){
                if(YSTRCMP(j->token,"adminPassword")==0){
                    enus->state = ENU_NET_ADMINPWD;
                } else{
                    yJsonSkip(j,1);
                }
            }
            break;
        case ENU_NET_ADMINPWD:
            NETENUMLOG("adminpwd is set to [%s]\n",j->token);
            enus->hub->writeProtected = (j->token[0] != 0);
            enus->state = ENU_NETWORK;
            break;
        case ENU_SERVICE:
            if(j->st !=YJSON_PARSE_MEMBNAME)
                break;
            if(YSTRCMP(j->token,"whitePages")==0){
                enus->state = ENU_WP_START;
            } else if(YSTRCMP(j->token,"yellowPages")==0){
                enus->state = ENU_YP_CONTENT;
            } else{
                yJsonSkip(j,1);
            }
            break;
        case ENU_WP_START:
            if(j->st ==YJSON_PARSE_ARRAY){
                enus->state = ENU_WP_ARRAY;
            }
            break;
        case ENU_WP_ARRAY:
            if(j->st == YJSON_PARSE_STRUCT){
                enus->state     = ENU_WP_ENTRY;
                enus->serial    = INVALID_HASH_IDX;
                enus->logicalName = INVALID_HASH_IDX;
                enus->productName = INVALID_HASH_IDX;
                enus->productId   = 0;
                enus->hubref      = INVALID_HASH_IDX;
                enus->beacon      = 0;
                enus->devYdx      = -1;
            }else  if(j->st ==YJSON_PARSE_ARRAY){
                enus->state=ENU_SERVICE;
            }else{
                NETENUMLOG("Unknown token %s\n",j->token);
            }
            break;
        case ENU_WP_ENTRY:
            if(j->st ==YJSON_PARSE_STRUCT){
                wpRegisterNet(enus);
				enus->state     = ENU_WP_ARRAY;
            }else if(j->st ==YJSON_PARSE_MEMBNAME){
                if(YSTRCMP(j->token,"serialNumber")==0){
                    enus->state = ENU_WP_SERIAL;
                } else if(YSTRCMP(j->token,"logicalName")==0){
                    enus->state = ENU_WP_LOGICALNAME;
                } else if(YSTRCMP(j->token,"productName")==0){
                    enus->state = ENU_WP_PRODUCTNAME;
                } else if(YSTRCMP(j->token,"productId")==0){
                    enus->state = ENU_WP_PRODUCTID;
                } else  if(YSTRCMP(j->token,"networkUrl")==0){
                    enus->state = ENU_WP_DEVURL;
                } else if(YSTRCMP(j->token,"beacon")==0){
                    enus->state = ENU_WP_BEACON;
                } else if(YSTRCMP(j->token,"index")==0){
                    enus->state = ENU_WP_INDEX;
                } else{
                    yJsonSkip(j,1);
                }
            }
            break;
        case ENU_WP_SERIAL:
            NETENUMLOG("set serial to %s\n",j->token);
            enus->serial = yHashPutStr(j->token);
            enus->state  = ENU_WP_ENTRY;
            break;
        case ENU_WP_LOGICALNAME:
            NETENUMLOG("set Logical name to %s\n",j->token);
            enus->logicalName = yHashPutStr(j->token);
            enus->state  = ENU_WP_ENTRY;
            break;
        case ENU_WP_PRODUCTNAME:
            NETENUMLOG("set Product name to %s\n",j->token);
            enus->productName = yHashPutStr(j->token);
            enus->state  = ENU_WP_ENTRY;
            break;
        case ENU_WP_PRODUCTID:
            NETENUMLOG("set productid to %s\n",j->token);
            enus->productId = atoi(j->token);
            enus->state  = ENU_WP_ENTRY;
            break;
        case ENU_WP_DEVURL:
            NETENUMLOG("set url to %s\n",j->token);
            enus->hubref = yHashUrlFromRef(enus->baseurl, j->token, 0,NULL);
            enus->state  = ENU_WP_ENTRY;
            break;
        case ENU_WP_BEACON:
            NETENUMLOG("set beacon to %s\n",j->token);
            enus->beacon = atoi(j->token);
            enus->state  = ENU_WP_ENTRY;
            break;
        case ENU_WP_INDEX:
            NETENUMLOG("set devYdx to %s\n",j->token);
            enus->devYdx = atoi(j->token);
            enus->state  = ENU_WP_ENTRY;
            break;
        case ENU_YP_CONTENT:
            if(j->st == YJSON_PARSE_STRUCT){
                enus->state     = ENU_YP_TYPE;
                enus->ypCateg   = INVALID_HASH_IDX;
            }else{
                NETENUMLOG("Unknown token %s\n",j->token);
            }
            break;
        case ENU_YP_TYPE:
            if(j->st ==YJSON_PARSE_STRUCT){
                enus->state     = ENU_SERVICE;
            }else if(j->st ==YJSON_PARSE_MEMBNAME){
                enus->ypCateg = yHashPutStr(j->token);
                NETENUMLOG("Categ = %s\n",j->token);
                enus->state = ENU_YP_TYPE_LIST;
            }else{
                NETENUMLOG("Unknown token %s\n",j->token);
            }
            break;
        case ENU_YP_TYPE_LIST:
            if(j->st ==YJSON_PARSE_ARRAY){
                enus->state = ENU_YP_ARRAY;
            } else if(j->st ==YJSON_PARSE_STRUCT){
                enus->state = ENU_SERVICE;
            }
            break;
        case ENU_YP_ARRAY:
            if(j->st == YJSON_PARSE_STRUCT){
                enus->state     = ENU_YP_ENTRY;
                enus->serial    = INVALID_HASH_IDX;
                enus->logicalName = INVALID_HASH_IDX;
                enus->funcId    = INVALID_HASH_IDX;
                enus->funYdx    = -1;
                memset(enus->advertisedValue,0, sizeof(enus->advertisedValue));
            }else  if(j->st ==YJSON_PARSE_ARRAY){
                enus->state = ENU_YP_TYPE;
            }else{
                NETENUMLOG("what is it %s \n",j->token);
            }
            break;
        case ENU_YP_ENTRY:
            if(j->st ==YJSON_PARSE_STRUCT){
                ypUpdateNet(enus);
                enus->state     = ENU_YP_ARRAY;
            }else if(j->st ==YJSON_PARSE_MEMBNAME){
                if(YSTRCMP(j->token,"logicalName")==0){
                    enus->state = ENU_YP_LOGICALNAME;
                } else if(YSTRCMP(j->token,"hardwareId")==0){
                    enus->state = ENU_YP_HARDWAREID;
                } else if(YSTRCMP(j->token,"advertisedValue")==0){
                    enus->state = ENU_YP_ADVERTISEDVALUE;
                }else{
                    yJsonSkip(j,1);
                }
            }
            break;

        case ENU_YP_HARDWAREID:
            point = strchr(j->token,'.');
            if(!point) break; //to be safe discart this field if we do not found '.'
            *point++ = '\0';
            NETENUMLOG("set serial  to %s\n",j->token);
            enus->serial = yHashPutStr(j->token);
            NETENUMLOG("set functionid  to %s\n",point);
            enus->funcId = yHashPutStr(point);
            enus->state  = ENU_YP_ENTRY;
            break;
        case ENU_YP_LOGICALNAME:
            NETENUMLOG("set function  to %s\n",j->token);
            enus->logicalName = yHashPutStr(j->token);
            enus->state  = ENU_YP_ENTRY;
            break;
        case ENU_YP_ADVERTISEDVALUE:
            NETENUMLOG("set advertised value to %s\n",j->token);
            YSTRNCPY(enus->advertisedValue,YOCTO_PUBVAL_LEN,j->token,YOCTO_PUBVAL_SIZE);
            enus->state  = ENU_YP_ENTRY;
            break;
        case ENU_YP_INDEX:
            NETENUMLOG("set funYdx value to %s\n",j->token);
            enus->funYdx = atoi(j->token);
            enus->state  = ENU_YP_ENTRY;
            break;
        default:
            return YAPI_IO_ERROR;
    }
    return YAPI_SUCCESS;
}


static int yNetHubEnum(NetHubSt *hub,int forceupdate,char *errmsg)
{
    yJsonStateMachine j;
    char            buffer[512];
    int             i, res;
    const char      *request = "GET /api.json \r\n\r\n";// no HTTP/1.1 suffix -> light headers
    yJsonRetCode    jstate = YJSON_NEED_INPUT;
    u64             enumTimeout;
    ENU_CONTEXT     enus;
    yStrRef         knownDevices[128];
    TcpReqSt        req, *reqptr = &req;

    //check if the expiration has expired;
    if(!forceupdate && hub->devListExpires > yapiGetTickCount()) {
        return YAPI_SUCCESS;
    }

    // et base url (then entry point)
    memset(&enus,0,sizeof(enus));
    enus.hub = hub;
    enus.baseurl = hub->url;
    enus.knownDevices = knownDevices;
    enus.nbKnownDevices = wpGetAllDevUsingHubUrl( hub->url,enus.knownDevices,128);
    if(enus.nbKnownDevices >128){
        return YERRMSG(YAPI_IO_ERROR,"too many device on this Net hub");
    }

    // setup connection structure
    yTcpInitReq(&req, hub);
    if(YISERR((res = yTcpOpenReq(&req, request, YSTRLEN(request), 0,errmsg)))) {
        yTcpFreeReq(&req);
        return res;
    }
    // init yjson parrser
    memset(&j,0,sizeof(j));
    j.st = YJSON_HTTP_START;
    enus.state = ENU_HTTP_START;
    enumTimeout = yapiGetTickCount() + 10000;
    while(jstate == YJSON_NEED_INPUT){
        res = yTcpSelectReq(&reqptr, 1, 1000, NULL, errmsg);
        if(YISERR(res)){
            yTcpFreeReq(&req);
            return res;
        }
        res = yTcpReadReq(&req, buffer, sizeof(buffer));
        while(res > 0) {
            j.src = buffer;
            j.end = buffer + res;

            // parse all we can on this buffer
            jstate=yJsonParse(&j);
            while(jstate == YJSON_PARSE_AVAIL){
                if(YISERR(yEnuJson(&enus,&j))){
                    jstate = YJSON_FAILED;
                    break;
                }
                jstate=yJsonParse(&j);
            }
            res = yTcpReadReq(&req, buffer, sizeof(buffer));
        }
        if(res <= 0) {
            res = yTcpEofReq(&req, errmsg);
            if(YISERR(res)){
                // any specific error during select
                yTcpFreeReq(&req);
                return res;
            }
            if(res == 1) {
                // connection close before end of result
                yTcpFreeReq(&req);
                return YERR(YAPI_IO_ERROR);
            }
            if (yapiGetTickCount()>= enumTimeout){
                // timeout
                yTcpFreeReq(&req);
                return YERR(YAPI_TIMEOUT);
            }
        }
    }
    yTcpFreeReq(&req);
    for(i=0; i < enus.nbKnownDevices ;  i++){
        if (enus.knownDevices[i]!=INVALID_HASH_IDX){
            wpUnregisterNet(knownDevices[i]);
        }
    }

    switch(jstate){
        case YJSON_NEED_INPUT:
            return YERRMSG(YAPI_IO_ERROR,"Remote host has close the connection");
        case YJSON_PARSE_AVAIL:
        case YJSON_FAILED:
            return YERRMSG(YAPI_IO_ERROR,"Invalid json data");
        case YJSON_SUCCESS:
            break;
    }
    // reset the
    if(hub->state == NET_HUB_ESTABLISHED){
        hub->devListExpires = yapiGetTickCount()+10000; // 10s validity when notification are working properly
    } else {
        hub->devListExpires = yapiGetTickCount()+500;
    }
    return YAPI_SUCCESS;
}





/*****************************************************************************
  API FUNCTIONS
 ****************************************************************************/


YRETCODE YAPI_FUNCTION_EXPORT yapiInitAPI(int detect_type,char *errmsg)
{
    int i;
    yContextSt *ctx;

    if(yContext!=NULL)
        return YERRMSG(YAPI_DEVICE_BUSY,"Api already started");

    YASSERT(sizeof(u8)==1);
    YASSERT(sizeof(s8)==1);
    YASSERT(sizeof(u16)==2);
    YASSERT(sizeof(u32)==4);
    YASSERT(sizeof(u64)==8);
    YASSERT(sizeof(s16)==2);
    YASSERT(sizeof(s32)==4);
    YASSERT(sizeof(s64)==8);
    YASSERT(sizeof(YSTREAM_Head)==2);
    //init safe malloc
    ySafeMemoryInit(64*1024);

    ctx=(yContextSt*)yMalloc(sizeof(yContextSt));
    yMemset(ctx,0,sizeof(yContextSt));
    ctx->detecttype=detect_type;

    //initialize enumeration CS
    yInitializeCriticalSection(&ctx->updateDev_cs);
    yInitializeCriticalSection(&ctx->handleEv_cs);
    yInitializeCriticalSection(&ctx->enum_cs);
    yInitializeCriticalSection(&ctx->io_cs);
    yInitializeCriticalSection(&ctx->deviceCallbackCS);
    yInitializeCriticalSection(&ctx->functionCallbackCS);


    //initialize device pool
    ctx->devs = NULL;
    ctx->devhdlcount = 1;
    if(detect_type & Y_DETECT_USB) {
        int res;
        if(YISERR(res=yUSB_init(ctx,errmsg))){
            yDeleteCriticalSection(&ctx->updateDev_cs);
            yDeleteCriticalSection(&ctx->handleEv_cs);
            yDeleteCriticalSection(&ctx->enum_cs);
            yDeleteCriticalSection(&ctx->io_cs);
            yDeleteCriticalSection(&ctx->deviceCallbackCS);
            yDeleteCriticalSection(&ctx->functionCallbackCS);
            yFree(ctx);
            return res;
        }
    }


    //initialize white/yellow pages support
    yHashInit();

    yInitWakeUpSocket(&ctx->wuce);
    if (YISERR(yTcpInit(errmsg))){
        yDeleteCriticalSection(&ctx->updateDev_cs);
        yDeleteCriticalSection(&ctx->handleEv_cs);
        yDeleteCriticalSection(&ctx->enum_cs);
        yDeleteCriticalSection(&ctx->io_cs);
        yDeleteCriticalSection(&ctx->deviceCallbackCS);
        yDeleteCriticalSection(&ctx->functionCallbackCS);
        yFree(ctx);
        return YAPI_IO_ERROR;
    }
    // initialise network hub list
    for(i=0; i < NBMAX_NET_HUB; i++) {
        ctx->nethub[i].url=INVALID_HASH_IDX;
    }
    yContext=ctx;

    if(detect_type & Y_DETECT_NET) {
        return YERRMSG(YAPI_NOT_SUPPORTED, "Detection of network hubs is not yet supported in this version of the API");
    }
    return YAPI_SUCCESS;

    
}

static int yTcpTrafficPending(void);

void YAPI_FUNCTION_EXPORT yapiFreeAPI(void)
{
    yPrivDeviceSt  *p;
    yPrivDeviceSt  *next;
    u64             timeout = yapiGetTickCount() + 1000;
    char            errmsg[YOCTO_ERRMSG_LEN];

    if(yContext==NULL)
        return;

    while(yapiGetTickCount() < timeout && (yUsbTrafficPending() || yTcpTrafficPending())) {
        yapiHandleEvents(errmsg);
        yySleep(1);
    }
    yEnterCriticalSection(&yContext->updateDev_cs);
    yEnterCriticalSection(&yContext->handleEv_cs);
    yEnterCriticalSection(&yContext->enum_cs);
    p=yContext->devs;
    yContext->devs=NULL;
    while(p) {
        if(p->dstatus ==YDEV_WORKING){
            StopDevice(p,NULL);
        }
        if(p->replybuf) {
            yFree(p->replybuf);
            p->replybuf=NULL;
        }
        next=p->next;
        FreeDevice(p);
        p=next;
    }
    if(yContext->detecttype & Y_DETECT_USB) {
        yUSB_stop(yContext,NULL);
    }

    if(yThreadIsRunning(&yContext->net_thread)) {     
        yThreadRequestEnd(&yContext->net_thread);
        yThreadWaitEnd(&yContext->net_thread);
    }
    yHashFree();
    yFreeWakeUpSocket(&yContext->wuce);
    yTcpShutdown();

    yLeaveCriticalSection(&yContext->updateDev_cs);
    yLeaveCriticalSection(&yContext->handleEv_cs);
    yLeaveCriticalSection(&yContext->enum_cs);
    yDeleteCriticalSection(&yContext->updateDev_cs);
    yDeleteCriticalSection(&yContext->handleEv_cs);
    yDeleteCriticalSection(&yContext->enum_cs);
    yDeleteCriticalSection(&yContext->io_cs);
    yDeleteCriticalSection(&yContext->deviceCallbackCS);
    yDeleteCriticalSection(&yContext->functionCallbackCS);

    ySafeMemoryDump(yContext);
    yFree(yContext);
    ySafeMemoryStop();
    yContext=NULL;
}


void YAPI_FUNCTION_EXPORT yapiRegisterLogFunction(yapiLogFunction logfun)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    if(!yContext) {
        yapiInitAPI(0,errmsg);
    }
    if(yContext) {
        yEnterCriticalSection(&yContext->enum_cs);
        yContext->log = logfun;
        yLeaveCriticalSection(&yContext->enum_cs);
    }
}

void YAPI_FUNCTION_EXPORT yapiRegisterDeviceLogCallback(yapiDeviceUpdateCallback logCallback)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    if(!yContext) {
        yapiInitAPI(0,errmsg);
    }
    if(yContext) {
        yEnterCriticalSection(&yContext->enum_cs);
        yContext->logDeviceCallback = logCallback;
        yLeaveCriticalSection(&yContext->enum_cs);
    }
}



void YAPI_FUNCTION_EXPORT yapiRegisterDeviceArrivalCallback(yapiDeviceUpdateCallback arrivalCallback)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    if(!yContext) {
        yapiInitAPI(0,errmsg);
    }
    if(yContext) {
        yEnterCriticalSection(&yContext->enum_cs);
        yContext->arrivalCallback = arrivalCallback;
        if(arrivalCallback != NULL){

            // call callback with already detected devices
            yPrivDeviceSt *p=yContext->devs;
            while(p){
                devGetAcces(p,1);
                if(p->dstatus == YDEV_WORKING){
                    yStrRef serialref = yHashPutStr(p->infos.serial);
                    yContext->arrivalCallback(serialref);
                }
                devReleaseAcces(p);
                p=p->next;
            }
        }
        yLeaveCriticalSection(&yContext->enum_cs);
    }
}


void YAPI_FUNCTION_EXPORT yapiRegisterDeviceRemovalCallback(yapiDeviceUpdateCallback removalCallback)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    if(!yContext) {
        yapiInitAPI(0,errmsg);
    }
    if(yContext) {
        yEnterCriticalSection(&yContext->enum_cs);
        yContext->removalCallback = removalCallback;
        yLeaveCriticalSection(&yContext->enum_cs);
    }
}


void YAPI_FUNCTION_EXPORT yapiRegisterDeviceChangeCallback(yapiDeviceUpdateCallback changeCallback)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    if(!yContext) {
        yapiInitAPI(0,errmsg);
    }
    if(yContext) {
        yContext->changeCallback = changeCallback;
    }
}


void YAPI_FUNCTION_EXPORT yapiRegisterFunctionUpdateCallback(yapiFunctionUpdateCallback updateCallback)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    if(!yContext) {
        yapiInitAPI(0,errmsg);
    }
    if(yContext) {
        yContext->functionCallback = updateCallback;
    }
}


#ifdef DEBUG_NET_NOTIFICATION
static void dumpNotif(const char *buffer)
{
    FILE *f; 
    YASSERT(fopen_s(&f,"c:\\tmp\\api_not.txt","ab")==0);
    fwrite(buffer,1,YSTRLEN(buffer),f);
    fclose(f);
}
#endif


static void wpUpdateTCP(yUrlRef huburl,const char *serial,const char *name, u8 beacon)
{
    #define LOCALURL_LEN 64
    int status;
    char devUrl[LOCALURL_LEN];
    yStrRef serialref;
    yStrRef lnameref;
    yUrlRef devurl;

    // Insert device into white pages
    YSTRCPY(devUrl,LOCALURL_LEN, "/bySerial/");
    YSTRCAT(devUrl,LOCALURL_LEN, serial);
    YSTRCAT(devUrl,LOCALURL_LEN, "/api");

    serialref   = yHashPutStr(serial);
    lnameref    = yHashPutStr(name);
    devurl      = yHashUrlFromRef(huburl,devUrl, 0,NULL);
    status = wpRegister(serialref, lnameref, INVALID_HASH_IDX, 0, devurl, beacon);
    if(status==0)
        return; // no change

    ypRegister(YSTRREF_MODULE_STRING, serialref, yHashPutStr("module"), lnameref, -1, NULL);

    // Forward high-level notification to API user
    if(status==2){
        if (yContext->arrivalCallback) {
            yEnterCriticalSection(&yContext->deviceCallbackCS);
            yContext->arrivalCallback(serialref);
            yLeaveCriticalSection(&yContext->deviceCallbackCS);
        }
    }else {
        if(yContext->changeCallback){
            yEnterCriticalSection(&yContext->deviceCallbackCS);
            yContext->changeCallback(serialref);
            yLeaveCriticalSection(&yContext->deviceCallbackCS);
        }
    }
}

static int handleNetNotification(NetHubSt *hub)
{
    u16             pos;
    u16             end,size;
    char            buffer[128];
    char            *p;
    u8              pkttype,devydx,funydx;
    char            *serial,*name,*funcid,*children;
    char            value[YOCTO_PUBVAL_LEN];
    char            netstop=NOTIFY_NETPKT_STOP;
#ifdef DEBUG_NET_NOTIFICATION
    u32             abspos = hub->notifAbsPos;
    char            Dbuffer[1024];
    u8              throw[1024];
    u16             tmp;
#endif    
    
    // search for start of notification
    size = yFifoGetUsed(&(hub->fifo));
    while(size >= NOTIFY_NETPKT_START_LEN) {
        yPeekFifo(&(hub->fifo), &pkttype, 1, 0);
        if(pkttype != NOTIFY_NETPKT_STOP) break;
        // drop newline and loop
        yPopFifo(&(hub->fifo),NULL,1);
        hub->notifAbsPos++;
        size--;
    }
    if(size < NOTIFY_NETPKT_START_LEN) {
        return 0;
    }
    // make sure we have a full notification
    end = ySeekFifo(&(hub->fifo), (u8*) &netstop, 1, 0, 0, 0);
    if(end == 0xffff){
        return 0;
    }
    
    // handle short funcvalydx notifications
    if(pkttype == NOTIFY_NETPKT_FUNCVALYDX) {
        yPopFifo(&(hub->fifo),(u8*) buffer,end+1);
        hub->notifAbsPos += end+1;
        p = buffer+1;
        devydx = (*p++) - 'A';
        funydx = (*p++) - '0';
        if(funydx & 64) { // high bit of devydx is on second character
            funydx -= 64;
            devydx += 128;
        }
        pos = 0;
        while(*p != 0 && *p != NOTIFY_NETPKT_STOP) {
            value[pos++] = *p++;
        }
        value[pos] = 0;
#ifdef DEBUG_NET_NOTIFICATION
        YSPRINTF(Dbuffer,512,"FuncVYDX >devYdx=%d funYdx=%d val=%s (%d)\n",
                 devydx,funydx,value,abspos);
        dumpNotif(Dbuffer);    
#endif
        // Map hub-specific devydx to our devydx
        devydx = hub->devYdxMap[devydx];
        if(devydx < MAX_YDX_PER_HUB) {
            ypUpdateYdx(devydx,funydx,value);
        }
        return 1;
    }
    
    // make sure packet is a valid notification
    pos = ySeekFifo(&(hub->fifo), (u8*) (NOTIFY_NETPKT_START), NOTIFY_NETPKT_START_LEN, 0, end, 0);
    if(pos != 0) {
        // does not start with signature, drop everything until stop marker
#ifdef DEBUG_NET_NOTIFICATION
        memset(throw, 0, sizeof(throw));
        tmp = (end > 50 ? 50 : end);
        yPopFifo(&(hub->fifo),throw,tmp);
        yPopFifo(&(hub->fifo),NULL,end+1-tmp);            
        Dbuffer[1023]=0;
        YSPRINTF(Dbuffer,512,"throw %d / %d [%s]\n",
                 end,pos,throw);
        dumpNotif(Dbuffer);    
#else            
        yPopFifo(&(hub->fifo),NULL,end+1);            
#endif   
        hub->notifAbsPos += end+1;
        return 0;
    }
    
    // full packet at start of fifo
    size = end - NOTIFY_NETPKT_START_LEN;
    YASSERT(NOTIFY_NETPKT_MAX_LEN > size);
    yPopFifo(&(hub->fifo),NULL,NOTIFY_NETPKT_START_LEN);
    yPopFifo(&(hub->fifo),(u8*) buffer,size+1);
    buffer[size]=0;
    pkttype = *buffer;
    p = buffer+1;
    if(pkttype == NOTIFY_NETPKT_NOT_SYNC) {
#ifdef DEBUG_NET_NOTIFICATION
        YSPRINTF(Dbuffer,512,"Sync from %d to %s\n",
             hub->notifAbsPos, p);
        dumpNotif(Dbuffer);    
#endif
        hub->notifAbsPos = atoi(p);
        return 1;
    }
    hub->notifAbsPos += size+1+NOTIFY_NETPKT_START_LEN;
    if(pkttype != NOTIFY_NETPKT_FUNCVALYDX) {
        serial = p;
        p = strchr(serial,NOTIFY_NETPKT_SEP);
        if(p==NULL) {
#ifdef DEBUG_NET_NOTIFICATION
            YSPRINTF(Dbuffer,512,"no serialFOR %s\n",buffer);
            dumpNotif(Dbuffer);    
#endif        
            return 0;
        }
        *p++ = 0;
    }


    switch(pkttype){
        case NOTIFY_NETPKT_NAME:
            name = p;
            p = strchr(name,NOTIFY_NETPKT_SEP);
            if(p==NULL){
#ifdef DEBUG_NET_NOTIFICATION
                dbglog("drop: invalid new name (%X)\n",pkttype);
#endif
                return 1;
            }
            *p++ = 0;
#ifdef DEBUG_NET_NOTIFICATION
            dbglog("NOTIFY_PKT_NAME %s : new name is \"%s\" beacon %x\n",serial,name ,*p);
            YSPRINTF(Dbuffer,512,"NAME     >%s  name=%s beacon=%c (%d)\n",serial,name,*p,abspos);
            dumpNotif(Dbuffer);    
#endif
            wpUpdateTCP(hub->url,serial,name,(*p== '1' ? 1:0));
            break;

            
        case NOTIFY_NETPKT_FUNCNAME:
            funcid = p;
            p = strchr(funcid,NOTIFY_NETPKT_SEP);
            if(p==NULL){
#ifdef DEBUG_NET_NOTIFICATION
                dbglog("drop: invalid funcid (%X:%s)\n",pkttype,serial);
#endif
                return 1;
            }
            *p++ = 0;
            name = p;
#ifdef DEBUG_NET_NOTIFICATION
            dbglog("NOTIFY_PKT_FUNCNAME %s : funcid is \"%s\" name \"%s\"\n",serial,funcid,name);
            YSPRINTF(Dbuffer,512,"FuncNAME >%s  funcid=%s funcname=%s (%d)\n",
                     serial,funcid,name,abspos);
            dumpNotif(Dbuffer);    
#endif
            ypUpdateUSB(serial,funcid,name,-1,NULL);
            break;

        
        
        case NOTIFY_NETPKT_FUNCVAL:
            funcid = p;
            p = strchr(funcid,NOTIFY_NETPKT_SEP);
            if(p==NULL){
#ifdef DEBUG_NET_NOTIFICATION
                dbglog("drop: invalid funcid (%X)\n",pkttype);
#endif
                return 1;
            }
            *p++ = 0;
            memset(value,0,YOCTO_PUBVAL_LEN);
            memcpy(value,p,YSTRLEN(p));
#ifdef DEBUG_NET_NOTIFICATION
            //dbglog("NOTIFY_PKT_FUNCVAL %s : funcid is \"%s\" value \"%s\"\n",serial,funcid,value);
            YSPRINTF(Dbuffer,512,"FuncVAL  >%s  funcid=%s val=%s (%d)\n",
                serial,funcid,value,abspos);
            dumpNotif(Dbuffer);    
#endif
            ypUpdateUSB(serial,funcid,NULL,-1,value);
            break;

        
        case NOTIFY_NETPKT_FUNCNAMEYDX:
            funcid = p;
            p = strchr(funcid,NOTIFY_NETPKT_SEP);
            if(p==NULL){
#ifdef DEBUG_NET_NOTIFICATION
                dbglog("drop: invalid funcid (%X:%s)\n",pkttype,serial);
#endif
                return 1;
            }
            *p++ = 0;
            name = p;
            p = strchr(name,NOTIFY_NETPKT_SEP);
            if(p==NULL){
#ifdef DEBUG_NET_NOTIFICATION
                dbglog("drop: invalid funcname (%X:%s)\n",pkttype,serial);
#endif
                return 1;
            }
            *p++ = 0;
            funydx = atoi(p);
#ifdef DEBUG_NET_NOTIFICATION
            YSPRINTF(Dbuffer,512,"FuncNYDX >%s  funcid=%s funcname=%s funYdx=%d (%d)\n",
                     serial,funcid,name,funydx,abspos);
            dumpNotif(Dbuffer);    
#endif
            ypUpdateUSB(serial,funcid,name,funydx,NULL);
            break;
            
            
        case NOTIFY_NETPKT_CHILD:
            children = p;
            p = strchr(children,NOTIFY_NETPKT_SEP);
            if(p==NULL){
#ifdef DEBUG_NET_NOTIFICATION
                dbglog("drop: invalid children notification (%X)\n",pkttype);
#endif
                return 1;
            }
            *p++ = 0;
#ifdef DEBUG_NET_NOTIFICATION
            dbglog("NOTIFY_PKT_CHILDREN %s : new children is \"%s\" plug %x\n",serial,children ,*p);
            YSPRINTF(Dbuffer,512,"CHILD    >%s  childserial=%s on-off=%c (%d)\n",
                     serial, children,*p,abspos);
            dumpNotif(Dbuffer);    
#endif
            if ( *p == '0') {
                wpUnregisterNet(yHashPutStr(children));
            }
            break;
            
        default:
#ifdef DEBUG_NET_NOTIFICATION
            dbglog("drop: invalid pkttype (%X)\n",pkttype);
            dumpNotif("drop: invalid pkttype\n");    
#endif
            break;

    }
    return 1;
}

static int yTcpTrafficPending(void)
{
    int         i;
    NetHubSt    *hub;
    TcpReqSt    *req;

    for (i = 0; i < NBMAX_NET_HUB; i++) {
        hub = &yContext->nethub[i];            
        if(hub->url == INVALID_HASH_IDX)
            continue;
        for (i=0; i < ALLOC_YDX_PER_HUB; i++) {
            req = &yContext->tcpreq[i];
            if(req->isAsyncIO && req->skt != INVALID_SOCKET) {
                return 1;
            }
        }
    }
    return 0;
}

static void* yhelper_thread(void* ctx)
{
    int         i,towatch;
    char        buffer[512];
    yThread     *thread=(yThread*)ctx;
    char        errmsg[YOCTO_ERRMSG_LEN];
    NetHubSt    *hub;
    TcpReqSt    *req, *selectlist[NBMAX_NET_HUB+ALLOC_YDX_PER_HUB];    
    int         toread,res;
#ifdef DEBUG_NET_NOTIFICATION
    char        Dbuffer[1024];
#endif    

    yThreadSignalStart(thread);
    while(!yThreadMustEnd(thread)){
        towatch=0;
        for (i = 0; i < NBMAX_NET_HUB; i++) {
            hub = &yContext->nethub[i];            
            if(hub->url == INVALID_HASH_IDX)
                continue;
            if(hub->state == NET_HUB_ESTABLISHED || hub->state == NET_HUB_TRYING){
                selectlist[towatch] = hub->notReq;
                towatch++;
            } else if (hub->state == NET_HUB_DISCONNECTED) {
                if(hub->notReq == NULL) {
                    hub->notReq = yMalloc(sizeof(TcpReqSt));
                    yTcpInitReq(hub->notReq, hub);
                }
                if (yapiGetTickCount() >= hub->retryWait) {
                    yFifoInit(&(hub->fifo), hub->buffer,sizeof(hub->buffer));                    
                    YSPRINTF(buffer, 256,"GET /not.byn?abs=%d HTTP/1.1\r\n\r\n" , hub->notifAbsPos);
                    res = yTcpOpenReq(hub->notReq, buffer, YSTRLEN(buffer), 0, errmsg);
                    if (YISERR(res)) {
                        u32 delay = 500 << hub->retryCount;
                        if(delay > 8000)
                            delay = 8000;
                        hub->retryWait =  yapiGetTickCount() + delay;
                        hub->retryCount++;
                    } else {
#ifdef DEBUG_NET_NOTIFICATION
                        YSPRINTF(Dbuffer,1024,"nethub %X started\n",hub->url);
                        dumpNotif(Dbuffer);
#endif
                        hub->state = NET_HUB_TRYING;
                        hub->retryCount=0;
                        selectlist[towatch++] = hub->notReq;
                    }
                }
            }
        }
        // Handle async connections as well in this thread
        for (i=0; i < ALLOC_YDX_PER_HUB; i++) {
            req = &yContext->tcpreq[i];
            if(req->isAsyncIO && req->skt != INVALID_SOCKET) {
                selectlist[towatch++] = req;
            }
        }

        if(YISERR(yTcpSelectReq(selectlist,towatch, 1000, &yContext->wuce, errmsg))){
            dbglog("yTcpSelectReq failed (%s)\n",errmsg);
            yySleep(1000);
        } else {
            for (i = 0; i < towatch; i++) {
                req = selectlist[i];
                hub = req->hub;
                if(req == hub->notReq) {
                    toread = yFifoGetFree(&hub->fifo);
                    while(toread > 0) {
                        if(toread >= sizeof(buffer)) toread = sizeof(buffer)-1;
                        res = yTcpReadReq(req, buffer, toread);
                        if(res > 0) {
                            buffer[res]=0;
#ifdef DEBUG_NET_NOTIFICATION
                            YSPRINTF(Dbuffer,1024,"Network hub %X push %d [\n%s\n]\n",hub->url,res,buffer);
                            dumpNotif(Dbuffer);
#endif
                            yPushFifo(&(hub->fifo), (u8*)buffer, res);
                            if(hub->state == NET_HUB_TRYING) {
                                int eoh = ySeekFifo(&(hub->fifo), (u8 *)"\r\n\r\n", 4, 0, 0, 0);
                                if(eoh != 0xffff) {
                                    if(eoh >= 12) {
                                        yPopFifo(&(hub->fifo), (u8 *)buffer, 12);
                                        yPopFifo(&(hub->fifo), NULL, eoh+2-12);
                                        if(!memcmp((u8 *)buffer, (u8 *)"HTTP/1.1 200", 12)) {
                                            hub->state = NET_HUB_ESTABLISHED;
                                        }
                                    }
                                    if(hub->state != NET_HUB_ESTABLISHED) {
                                        // invalid header received, give up
                                        char hubname[YOCTO_HOSTNAME_NAME]="";
                                        u16  port;
                                        hub->state = NET_HUB_FAILED;
                                        yHashGetUrlPort(hub->url, hubname, &port);
                                        dbglog("Network hub %s cannot provide notifications", hubname);
                                    }
                                }
                            }
                            if(hub->state == NET_HUB_ESTABLISHED) {
                                while(handleNetNotification(hub));
                            }
                        } else {
                            res = yTcpEofReq(req, errmsg);
                            if (res != 0) {
                                char hubname[YOCTO_HOSTNAME_NAME]="";
                                u16  port;
                                // error or remote close
                                yTcpCloseReq(req);
                                hub->state = NET_HUB_DISCONNECTED;
                                yHashGetUrlPort(hub->url, hubname, &port);
                                if(res == 1) YERRMSG(YAPI_IO_ERROR, "Connection closed by remote host");
                                dbglog("Disconnected from network hub %s: %s\n", hubname, errmsg);
#ifdef DEBUG_NET_NOTIFICATION
                                YSPRINTF(Dbuffer,1024,"Network hub %X has closed the connection for notification\n",hub->url);
                                dumpNotif(Dbuffer);
#endif
                            } else {
                                // nothing more to be read, exit loop
                                break;
                            }
                        }
                        toread = yFifoGetFree(&hub->fifo);
                    }
                } else if(req->isAsyncIO) { // normally always true
                    res = yTcpEofReq(req, errmsg);
                    if(res != 0) {
                        yTcpCloseReq(req);
                    }
                }
            }
        }
    }

    yThreadSignalEnd(thread);
    return NULL;
}


YRETCODE YAPI_FUNCTION_EXPORT yapiLockFunctionCallBack( char *errmsg)
{
    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    yEnterCriticalSection(&yContext->functionCallbackCS);
    return YAPI_SUCCESS;
}



YRETCODE YAPI_FUNCTION_EXPORT yapiUnlockFunctionCallBack(char *errmsg)
{
    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    yLeaveCriticalSection(&yContext->functionCallbackCS);
    return YAPI_SUCCESS;
}


YRETCODE YAPI_FUNCTION_EXPORT yapiLockDeviceCallBack( char *errmsg)
{
    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    yEnterCriticalSection(&yContext->deviceCallbackCS);
    return YAPI_SUCCESS;
}



YRETCODE YAPI_FUNCTION_EXPORT yapiUnlockDeviceCallBack(char *errmsg)
{
    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    yLeaveCriticalSection(&yContext->deviceCallbackCS);
    return YAPI_SUCCESS;
}

static void yapiInitHub(NetHubSt *hub, yUrlRef huburl)
{
    memset(hub,0,sizeof(NetHubSt));
    memset(hub->devYdxMap, 255, sizeof(hub->devYdxMap));
    yInitializeCriticalSection(&hub->authAccess);
    hub->url = huburl;
}

static void yapiDeleteHub(NetHubSt *hub)
{
    yDeleteCriticalSection(&hub->authAccess);
    if(hub->user)   yFree(hub->user);
    if(hub->realm)  yFree(hub->realm);
    if(hub->pwd)    yFree(hub->pwd);
    if(hub->nonce)  yFree(hub->nonce);
    if(hub->opaque) yFree(hub->opaque);
    memset(hub, 0, sizeof(NetHubSt));
    memset(hub->devYdxMap, 255, sizeof(hub->devYdxMap));
    hub->url = INVALID_HASH_IDX;
}

static YRETCODE yapiRegisterHubEx(const char *url, int checkacces, char *errmsg)
{
    yUrlRef  huburl;
    int i;
    int res;

    if(!yContext) {
        YPROPERR(yapiInitAPI(0,errmsg));
    }
    if(YSTRICMP(url,"usb")==0){
        if(!(yContext->detecttype & Y_DETECT_USB)){
            yEnterCriticalSection(&yContext->enum_cs);
            yContext->detecttype |=Y_DETECT_USB;
            res = yUSB_init(yContext,errmsg);
            yLeaveCriticalSection(&yContext->enum_cs);
            YPROPERR(res);
        }
    }else{
        NetHubSt hubst;
        char     suberr[YOCTO_ERRMSG_LEN];
        int      len, firstfree;        
        char     *user = NULL, *pwd = NULL;
        const char *p;
        
        // skip http prefix
        if(YSTRNCMP(url,"http://",7)==0){
            url += 7;
        }
        // search for any authentication info
        for(p = url; *p && *p != '@' && *p != '/'; p++);
        if(*p == '@') {
            for(p = url; *p != ':' && *p != '@'; p++);
            if(*p != ':') {
                return YERRMSG(YAPI_INVALID_ARGUMENT, "Missing authentication parameter");
            }
            len = (int)(p-url);
            user = yMalloc(len+1);
            memcpy(user, url, len);
            user[len] = 0;
            url = ++p;
            while(*p != '@') p++;
            len = (int)(p-url);
            pwd = yMalloc(len+1);
            memcpy(pwd, url, len);
            pwd[len] = 0;
            url = p+1;
        }
        // compute an hashed url
        huburl = yHashUrl(url,"",0,errmsg);
        if (checkacces) {
            yapiInitHub(&hubst, huburl);
            hubst.user = user;
            hubst.pwd  = pwd;
            res = yNetHubEnum(&hubst,1,suberr);
            if(YISERR(res)){
                if (errmsg)
                    YSPRINTF(errmsg,YOCTO_ERRMSG_LEN,"Enumeration failed for %s (%s)\n",url,suberr);
                yapiDeleteHub(&hubst);
                return res;
            }
        }
        //look if we allready know this
        yEnterCriticalSection(&yContext->enum_cs);
        firstfree = NBMAX_NET_HUB;
        for(i=0; i < NBMAX_NET_HUB; i++){
            if(yContext->nethub[i].url == huburl)
                break;
            if(firstfree == NBMAX_NET_HUB && yContext->nethub[i].url == INVALID_HASH_IDX) {
                firstfree = i;
            }
        }
        if (i>=NBMAX_NET_HUB && firstfree < NBMAX_NET_HUB) {
            i = firstfree;
            if(checkacces) {
                // save mapping attributed from first access
                memcpy(&yContext->nethub[i], &hubst, sizeof(NetHubSt));
            } else {
                yapiInitHub(&yContext->nethub[i], huburl);
                yContext->nethub[i].user = user;
                yContext->nethub[i].pwd  = pwd;
            }

            if(!yThreadIsRunning(&yContext->net_thread)) {
                //yThreadCreate will not create a new thread if there is already one running
                if(YISERR(res = yStartWakeUpSocket(&yContext->wuce, errmsg))){
                    yLeaveCriticalSection(&yContext->enum_cs);
                    return res;
                }

                //yThreadCreate will not create a new thread if there is already one running
                if(yThreadCreate(&yContext->net_thread,yhelper_thread,(void*)yContext)<0){
                    yLeaveCriticalSection(&yContext->enum_cs);
                    return YERRMSG(YAPI_IO_ERROR,"Unable to start helper thread");
                }
            }
            yDringWakeUpSocket(&yContext->wuce, 1, errmsg);
        }
        if(user) yFree(user);
        if(pwd) yFree(pwd);
        yLeaveCriticalSection(&yContext->enum_cs);

        if (i==NBMAX_NET_HUB) {
            yapiDeleteHub(&hubst);
            return YERRMSG(YAPI_INVALID_ARGUMENT,"Too many network hub registered");
        }

    }
    if(checkacces)
        return yapiUpdateDeviceList(1,errmsg);
    return YAPI_SUCCESS;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiRegisterHub(const char *url, char *errmsg)
{
    return yapiRegisterHubEx(url,1, errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT yapiPreregisterHub(const char *url, char *errmsg)
{
    return yapiRegisterHubEx(url,0, errmsg);
}

void YAPI_FUNCTION_EXPORT yapiUnregisterHub(const char *url)
{
    yUrlRef  huburl;
    char     errmsg[YOCTO_ERRMSG_LEN];
    
    if(!yContext) {
        return;
    }
    if(YSTRICMP(url,"usb")==0){
        if(yContext->detecttype & Y_DETECT_USB) {
            yUSBReleaseAllDevices();
            yUSB_stop(yContext,NULL);
            yContext->detecttype ^= Y_DETECT_USB;
        }
    }else{
        int      i;
        int      nbKnownDevices;
        yStrRef  knownDevices[128];
        u64      timeout;
        const char *p;

        // skip http prefix
        if(YSTRNCMP(url,"http://",7)==0){
            url += 7;
        }
        // search for any authentication info
        for(p = url; *p && *p != '@' && *p != '/'; p++);
        if(*p == '@') {
            url = p+1;
        }
        // compute an hashed url
        huburl = yHashUrl(url,"",1,errmsg);
        if(huburl == INVALID_HASH_IDX){
            return;
        }
        //look if we allready know this
        yEnterCriticalSection(&yContext->enum_cs);
        for(i=0; i < NBMAX_NET_HUB; i++){
            NetHubSt *hub = &yContext->nethub[i];
            if(hub->url == huburl) {
                if(hub->state == NET_HUB_ESTABLISHED || hub->state == NET_HUB_TRYING || hub->state == NET_HUB_DISCONNECTED) {
                    hub->state = NET_HUB_TOCLOSE;
                    yDringWakeUpSocket(&yContext->wuce, 0, errmsg);
                    // wait for the helper thread to stop monitoring these devices
                    timeout = yapiGetTickCount() + 5000;
                    while(hub->state != NET_HUB_CLOSED && yapiGetTickCount() < timeout) {
                        yySleep(5);
                    }
                }
                yapiDeleteHub(hub);
                break;
            }
        }
        
        nbKnownDevices = wpGetAllDevUsingHubUrl(huburl,knownDevices,128);
        for(i = 0 ; i < nbKnownDevices; i++) {
            if (knownDevices[i]!=INVALID_HASH_IDX){
                wpUnregisterNet(knownDevices[i]);
            }
        }
        yLeaveCriticalSection(&yContext->enum_cs);
    }
}


YRETCODE YAPI_FUNCTION_EXPORT yapiUpdateDeviceList(u32 forceupdate, char *errmsg)
{
    int i;
    YRETCODE err =YAPI_SUCCESS;
    char suberr[YOCTO_ERRMSG_LEN];

    if(yContext==NULL)
        return YERR(YAPI_NOT_INITIALIZED);

    if(forceupdate) {
        yEnterCriticalSection(&yContext->updateDev_cs);
    } else {
        // if we do not force an update
        if(!yTryEnterCriticalSection(&yContext->updateDev_cs))
            return YAPI_SUCCESS;
    }
    if(yContext->detecttype & Y_DETECT_USB){
        err = yUSBUpdateDeviceList(errmsg);
    }

    for(i=0; i < NBMAX_NET_HUB; i++){
       if(yContext->nethub[i].url!= INVALID_HASH_IDX){
            int subres;
            if(YISERR(subres = yNetHubEnum(&yContext->nethub[i],forceupdate,suberr)) && err==YAPI_SUCCESS){
                //keep first generated error
                char buffer[YOCTO_HOSTNAME_NAME]="";
                u16  port;
                err = subres;
                yHashGetUrlPort(yContext->nethub[i].url, buffer,&port);
                if(errmsg) {
                    dbglog("Enumeration failed for %s:%d (%s)\n",buffer,port,suberr);
                    YSPRINTF(errmsg,YOCTO_ERRMSG_LEN,"Enumeration failed for %s:%d (%s)\n",buffer,port,suberr);
                }
            }
        }
    }
    yLeaveCriticalSection(&yContext->updateDev_cs);

    return err;
}


YRETCODE YAPI_FUNCTION_EXPORT yapiHandleEvents(char *errmsg)
{
    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    // we need only one thread to handle the event at a time
    if(yTryEnterCriticalSection(&yContext->handleEv_cs)){
        YRETCODE res =yUsbIdle();
        yLeaveCriticalSection(&yContext->handleEv_cs);
        return res;
    }
    return YAPI_SUCCESS;
}


YRETCODE YAPI_FUNCTION_EXPORT yapiSleep(int ms_duration, char *errmsg)
{
    u64      timeout = yapiGetTickCount() + ms_duration;
    YRETCODE err = YAPI_SUCCESS;

    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    do {
        if(err == YAPI_SUCCESS) {
            err = yapiHandleEvents(errmsg);
        }
        if(yapiGetTickCount() >= timeout) break;
        yySleep(1);
    } while(yapiGetTickCount() < timeout);

    return err;
}

#ifdef WINDOWS_API
static int                 tickUseHiRes = -1;
static LARGE_INTEGER       tickFrequency;
static LARGE_INTEGER       tickStart;
#endif
u64 YAPI_FUNCTION_EXPORT yapiGetTickCount(void)
{
    u64 res;

#ifdef WINDOWS_API
    LARGE_INTEGER performanceCounter;

    if(tickUseHiRes<0){
        if (QueryPerformanceFrequency(&tickFrequency)){
            tickUseHiRes=1;
            QueryPerformanceCounter(&tickStart);
        } else {
            tickUseHiRes=0;
        }
    }
    if(tickUseHiRes>0) {
        QueryPerformanceCounter(&performanceCounter);
        performanceCounter.QuadPart -= tickStart.QuadPart;
		// we add +1 because it is not nice to start with zero
        res = performanceCounter.QuadPart * 1000 / tickFrequency.QuadPart + 1;
    } else {
        res = GetTickCount();
    }
#else
    //get the current number of microseconds since January 1st 1970
    struct timeval tim;
    gettimeofday(&tim, NULL);
    res= tim.tv_sec * 1000 + (tim.tv_usec / 1000);
#endif
    
    return res;
}

u32 YAPI_FUNCTION_EXPORT yapiGetCNonce(u32 nc)
{
    HASH_SUM ctx;
    u32      md5[4];

#ifdef WINDOWS_API
    LARGE_INTEGER performanceCounter;
    
    if(tickUseHiRes>0) {
        QueryPerformanceCounter(&performanceCounter);
    } else {
        performanceCounter.QuadPart = GetTickCount();
    }
    MD5Initialize(&ctx);
    MD5AddData(&ctx, (u8 *)&performanceCounter, sizeof(performanceCounter));
#else
    //get the current number of microseconds since January 1st 1970
    struct timeval tim;

    gettimeofday(&tim, NULL);
    MD5Initialize(&ctx);
    MD5AddData(&ctx, (u8 *)&tim, sizeof(tim));
#endif
    MD5AddData(&ctx, (u8 *)&nc, sizeof(nc));
    MD5Calculate(&ctx, (u8 *)md5);
    
    return md5[1];
}

int YAPI_FUNCTION_EXPORT yapiCheckLogicalName(const char *name)
{
    char c;

    if(!name) return 0;
    if(!*name) return 1;
    if(strlen(name) > 19) return 0;
    while((c = *name++) != 0) {
        if(c < '-') return 0;
        if(c > '-' && c < '0') return 0;
        if(c > '9' && c < 'A') return 0;
        if(c > 'Z' && c < '_') return 0;
        if(c > '_' && c < 'a') return 0;
        if(c > 'z') return 0;
    }
    return 1;
}


u16 YAPI_FUNCTION_EXPORT yapiGetAPIVersion(const char **version,const char **apidate)
{
    if(version)
        *version = YOCTO_API_VERSION_STR"."YOCTO_API_BUILD_NO;
    if(apidate)
        *apidate = YOCTO_API_BUILD_DATE;

    return YOCTO_API_VERSION_BCD;
}

void YAPI_FUNCTION_EXPORT yapiSetTraceFile(const char *file)
{
    if(file!=NULL){
        memset(ytracefile,0,TRACEFILE_NAMELEN);
        YSTRNCPY(ytracefile,TRACEFILE_NAMELEN-1,file,TRACEFILE_NAMELEN-1);
    }else{
        ytracefile[0]=0;
    }
}


YAPI_DEVICE YAPI_FUNCTION_EXPORT yapiGetDevice(const char *device_str,char *errmsg)
{
    char    hostname[HASH_BUF_SIZE], c;
    int     i;
    YAPI_DEVICE res;

    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    if(!strncmp(device_str, "http://", 7)) {
        for(i = 0; i < HASH_BUF_SIZE-1; i++) {
            c = device_str[7+i];
            if(!c || c == '/') break;
            hostname[i] = c;
        }
        res = wpSearchByUrl(hostname, device_str+7+i);
    } else {
        res = wpSearch(device_str);
    }
    if(res == -1) return YERR(YAPI_DEVICE_NOT_FOUND);

    return res;
}


int YAPI_FUNCTION_EXPORT yapiGetAllDevices(YAPI_DEVICE *buffer,int maxsize,int *neededsize,char *errmsg)
{
    int     maxdev,nbreturned;
    yBlkHdl devhdl;
    YAPI_DEVICE devdescr;

    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    if(buffer==NULL && neededsize==NULL)
        return YERR(YAPI_INVALID_ARGUMENT);

    nbreturned = 0;
    if(buffer) {
        // This function walks through the white pages without taking the mutex globally
        // it is only reasonably safe because wpGetAttribute properly handles dangerous devhdl.
        // (same principle as used by HTTPSendRec for the REST API)
        maxdev = 0;
        for(devhdl = yWpListHead; devhdl != INVALID_BLK_HDL; devhdl = yBlkListSeek(devhdl, 1)) {
            devdescr = wpGetAttribute(devhdl, Y_WP_SERIALNUMBER);
            if(devdescr < 0) continue;
            maxdev++;
            if(maxsize >= sizeof(YAPI_DEVICE)) {
                maxsize -= sizeof(YAPI_DEVICE);
                *buffer++ = devdescr;
                nbreturned++;
            }
        }
        if(neededsize) *neededsize = sizeof(YAPI_DEVICE) * maxdev;
    } else {
        if(neededsize) *neededsize = sizeof(YAPI_DEVICE) * wpEntryCount();
    }

    return nbreturned;
}


YRETCODE YAPI_FUNCTION_EXPORT yapiGetDeviceInfo(YAPI_DEVICE devdesc,yDeviceSt *infos,char *errmsg)
{
    YUSBDEV devhdl;

    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    if(devdesc < 0 || infos == NULL)
        return YERR(YAPI_INVALID_ARGUMENT);

    yHashGetStr(devdesc & 0xffff, infos->serial, YOCTO_SERIAL_LEN);
    devhdl = findDevHdlFromStr(infos->serial);
    if(devhdl != INVALID_YHANDLE) {
        // local device, get all information straight from the source
        devHdlInfo(devhdl, infos);
    } else {
        // not a local device, get available information from white pages
        infos->vendorid = 0x24e0;
        infos->devrelease = 0;
        infos->nbinbterfaces = 0;
        memcpy((u8 *)infos->manufacturer, (u8 *)"Yoctopuce", 10);
        memset(infos->firmware, 0, YOCTO_FIRMWARE_LEN);
        if(wpGetDeviceInfo(devdesc, &infos->deviceid, infos->productname, infos->serial, infos->logicalname, &infos->beacon) < 0)
            return YERR(YAPI_DEVICE_NOT_FOUND);
    }

    return YAPI_SUCCESS;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiGetDevicePath(YAPI_DEVICE devdesc, char *rootdevice, char *request, int requestsize, int *neededsize, char *errmsg)
{
    int res;

    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    if(rootdevice==NULL && request==NULL && neededsize==NULL)
        return YERR(YAPI_INVALID_ARGUMENT);

    res = wpGetDeviceUrl(devdesc, rootdevice, request, requestsize, neededsize);
    if(neededsize) *neededsize += 4;
    if(res < 0) return YERR(YAPI_DEVICE_NOT_FOUND);
    return res;
}

YAPI_FUNCTION YAPI_FUNCTION_EXPORT yapiGetFunction(const char *class_str, const char *function_str,char *errmsg)
{
    YAPI_FUNCTION res;

    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    res = ypSearch(class_str, function_str);
    if(res < 0) {
        if(res == -2) return YERRMSG(YAPI_DEVICE_NOT_FOUND, "No function of that class");
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }
    return res;
}

int YAPI_FUNCTION_EXPORT yapiGetFunctionsByClass(const char *class_str, YAPI_FUNCTION prevfundesc,
                                          YAPI_FUNCTION *buffer,int maxsize,int *neededsize,char *errmsg)
{
    int res;
    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    if(buffer==NULL && neededsize==NULL)
        return YERR(YAPI_INVALID_ARGUMENT);

    res = ypGetFunctions(class_str, -1, prevfundesc, buffer, maxsize, neededsize);
    if(res < 0) {
        return YERR(YAPI_DEVICE_NOT_FOUND); // prevfundesc is invalid or has been unplugged, restart enum
    }
    return res;
}

int YAPI_FUNCTION_EXPORT yapiGetFunctionsByDevice(YAPI_DEVICE devdesc, YAPI_FUNCTION prevfundesc,
                                           YAPI_FUNCTION *buffer,int maxsize,int *neededsize,char *errmsg)
{
    int res;
    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    if(buffer==NULL && neededsize==NULL)
        return YERR(YAPI_INVALID_ARGUMENT);

    res = ypGetFunctions(NULL, devdesc, prevfundesc, buffer, maxsize, neededsize);
    if(res < 0) {
        return YERR(YAPI_DEVICE_NOT_FOUND); // prevfundesc is invalid or has been unplugged, restart enum
    }
    return res;
}


YRETCODE YAPI_FUNCTION_EXPORT yapiGetFunctionInfo(YAPI_FUNCTION fundesc,YAPI_DEVICE *devdesc,char *serial,char *funcId,char *funcName,char *funcVal,char *errmsg)
{
    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    if(ypGetFunctionInfo(fundesc, serial, funcId, funcName, funcVal) < 0)
        return YERR(YAPI_DEVICE_NOT_FOUND);
    if(devdesc)
        *devdesc = fundesc & 0xffff;

    return YAPI_SUCCESS;
}


// when iohdl is set to NULL, the request is opened in Async mode and will close automatically
static YRETCODE yapiRequestOpen(YIOHDL *iohdl, const char *device, const char *request, int reqlen, int isAsync, char *errmsg)
{
    YAPI_DEVICE dev;
    char        buffer[256];
    YRETCODE    res;
    int         i, firsttime = 1;
    YIOHDL      localhdl;
    int         devydx;
    TcpReqSt    *tcpreq;
    u64         timeout;

    if(!yContext) {
        return YERR(YAPI_NOT_INITIALIZED);
    }

    dev = wpSearch(device);
    if(dev == -1) {
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }
    if(iohdl==NULL){
        iohdl = &localhdl;
    }
    memset(iohdl,0,sizeof(YIOHDL));
    iohdl->url = wpGetDeviceUrlRef(dev);

    switch(yHashGetUrlPort(iohdl->url, buffer, NULL)) {
    case USB_URL:
        yHashGetStr(dev & 0xffff, buffer, YOCTO_SERIAL_LEN);
        timeout = yapiGetTickCount() + YAPI_BLOCKING_USBOPEN_REQUEST_TIMEOUT;
        do {
            res = yUsbOpen(iohdl,buffer,errmsg);
            if(res != YAPI_DEVICE_BUSY) break;
            yapiHandleEvents(errmsg);
            if(firsttime) {
                yySleep(1);
                firsttime = 0;
            }
        } while(yapiGetTickCount() < timeout);
        if(res == YAPI_SUCCESS) {
            res = yUsbWrite(iohdl,request,reqlen,errmsg);
            if(YISERR(res)) {
                yUsbClose(iohdl,errmsg);
            }
            if(isAsync){
                res = yUsbSetIOAsync(iohdl,errmsg);
                if(YISERR(res)) {
                    yUsbClose(iohdl,errmsg);
                }
            }
        }
        return res;

    default:
        devydx = wpGetDevYdx((yStrRef)dev);
        if(devydx < 0) break;
        tcpreq = &yContext->tcpreq[devydx];
        if(!tcpreq->hub) {
            for(i=0; i < NBMAX_NET_HUB; i++){
                if(yContext->nethub[i].url == iohdl->url)
                    break;
            }
            if(i >= NBMAX_NET_HUB) break;
            yTcpInitReq(tcpreq, &yContext->nethub[i]);
        }
        if(isAsync) {
            if(tcpreq->hub->writeProtected) {
                yEnterCriticalSection(&tcpreq->hub->authAccess);
                if(!tcpreq->hub->user || strcmp(tcpreq->hub->user,"admin")!=0) {
                    yLeaveCriticalSection(&tcpreq->hub->authAccess);
                    return YERRMSG(YAPI_UNAUTHORIZED, "Access denied: admin credentials required");
                }
                yLeaveCriticalSection(&tcpreq->hub->authAccess);
            }
        }
        res = yTcpOpenReq(tcpreq,request,reqlen,isAsync,errmsg);
        if(res != YAPI_SUCCESS) return res;
        iohdl->tcpreqidx = devydx;
        iohdl->type = YIO_TCP;
        if(isAsync) iohdl->flags |= YIO_ASYNC;
        return YAPI_SUCCESS;
    }
    return YERR(YAPI_DEVICE_NOT_FOUND);
}

YRETCODE YAPI_FUNCTION_EXPORT yapiHTTPRequestSyncStartEx(YIOHDL *iohdl, const char *device, const char *request, int requestsize, char **reply, int *replysize, char *errmsg)
{
    u64     timeout;
    int     retcode;

    YREQLOG("SyncStart for %s \n%s\n",device,request);
    memset((u8 *)iohdl, 0, sizeof(YIOHDL));
    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
	 *reply =NULL;
    YPROPERR(yapiRequestOpen(iohdl,device,request,requestsize,0,errmsg));
    if(iohdl->type == YIO_USB){
        yPrivDeviceSt *p;
        int buffpos = 0;
        
        timeout = yapiGetTickCount() + YAPI_BLOCKING_USBREAD_REQUEST_TIMEOUT;
        p = findDevFromIOHdl(iohdl->hdl);
        if(p==NULL) {
            return YERR(YAPI_DEVICE_NOT_FOUND);
        }
        if(p->replybuf == NULL) {
            p->replybufsize = 2048;
            p->replybuf = yMalloc(p->replybufsize);
        }
        while(yUsbEOF(iohdl,errmsg)==0){
            if(iohdl->flags & YIO_REMOTE_CLOSE){
                return YERRMSG(YAPI_IO_ERROR,"Device has closed the connection");
            }
            if(yapiGetTickCount() > timeout) {
                yUsbClose(iohdl,NULL);
                return YERRMSG(YAPI_TIMEOUT,"Timeout during device request");
            }
            if(buffpos + 256 > p->replybufsize) {                
                char *newbuff;
                p->replybufsize <<= 1;
                newbuff = yMalloc(p->replybufsize);
                memcpy(newbuff, p->replybuf, buffpos);
                yFree(p->replybuf);
                p->replybuf = newbuff;
            }
            retcode = yUsbReadNonBlock(iohdl, p->replybuf+buffpos, p->replybufsize-buffpos, errmsg);
            if(YISERR(retcode)){
                yUsbClose(iohdl,NULL);
                return retcode;
            }
            buffpos += retcode;
            // FIXME: seb, use a blocking read instead
            if(retcode == 0) yySleep(3);
        }
        *reply = p->replybuf;
        *replysize = buffpos;
        return ALLOC_YDX_PER_HUB;
    } else if(iohdl->type == YIO_TCP) {
        int retcode;
        TcpReqSt *tcpreq = &yContext->tcpreq[iohdl->tcpreqidx];
        timeout = yapiGetTickCount() + YAPI_BLOCKING_NET_REQUEST_TIMEOUT;
        do {
            if(yapiGetTickCount() > timeout) {
                yTcpCloseReq(tcpreq);
                return YERRMSG(YAPI_TIMEOUT,"Timeout during device request");
            }
            retcode = yTcpSelectReq(&tcpreq, 1, 1000, NULL, errmsg);
            if(YISERR(retcode)){
                yTcpCloseReq(tcpreq);
                return retcode;
            }
            retcode = yTcpEofReq(tcpreq, errmsg);
        } while(retcode == 0);
        if(YISERR(retcode) && retcode != YAPI_NO_MORE_DATA) {
            yTcpCloseReq(tcpreq);
            return retcode;
        }
        *replysize = yTcpGetReq(tcpreq, reply);
    } else {
        return YERR(YAPI_INVALID_ARGUMENT);
    }
    YREQLOG("SyncStart for %s done (size=%d iodhl=%x)\n",device,replysize?*replysize:-1,iohdl);

    return YAPI_SUCCESS;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiHTTPRequestSyncStart(YIOHDL *iohdl, const char *device, const char *request, char **reply, int *replysize, char *errmsg)
{
    return yapiHTTPRequestSyncStartEx(iohdl, device, request, YSTRLEN(request), reply, replysize, errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT yapiHTTPRequestSyncDone(YIOHDL *iohdl, char *errmsg)
{

    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    YREQLOG("SyncDone for %x \n",iohdl);
    if(iohdl->type == YIO_USB) {
        yUsbClose(iohdl, errmsg);
    } else if(iohdl->type == YIO_TCP) {
        TcpReqSt *tcpreq = &yContext->tcpreq[iohdl->tcpreqidx];
        
        yTcpCloseReq(tcpreq);
    } else {
        return YERR(YAPI_INVALID_ARGUMENT);
    }
    memset((u8 *)iohdl, 0, sizeof(YIOHDL));
    
    return YAPI_SUCCESS;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiHTTPRequestAsyncEx(const char *device, const char *request, int len, yapiRequestAsyncCallback callback, void *context, char *errmsg)
{
    YIOHDL  iohdl;
    int     res;
    
    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    
    YREQLOG("ASync for %s \n%s\n",device,request);

    res = yapiRequestOpen(&iohdl,device,request,len,1,errmsg);
    if(!YISERR(res) && iohdl.type == YIO_TCP) {
        res = yDringWakeUpSocket(&yContext->wuce, 2, errmsg);
    }

    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiHTTPRequestAsync(const char *device, const char *request, yapiRequestAsyncCallback callback, void *context, char *errmsg)
{
    return yapiHTTPRequestAsyncEx(device, request, YSTRLEN(request), callback,context, errmsg);
}



int YAPI_FUNCTION_EXPORT yapiHTTPRequest(const char *device, const char *request, char* buffer,int buffsize, int *fullsize,char *errmsg)
{
    YIOHDL  iohdl;
    char    *reply=NULL;
    int     replysize=0;
    
    YREQLOG("yapiHTTPRequest for %s \n%s\n",device,request);
    if(!buffer || buffsize < 4)
        return YERR(YAPI_INVALID_ARGUMENT);
    YPROPERR(yapiHTTPRequestSyncStart(&iohdl, device, request, &reply, &replysize, errmsg));

    if(replysize > buffsize-1) {
        replysize = buffsize-1;
    }
    memcpy(buffer, reply, replysize);
    buffer[replysize] = 0;
    YPROPERR(yapiHTTPRequestSyncDone(&iohdl, errmsg));
    
    return replysize;
}

YRETCODE  YAPI_FUNCTION_EXPORT  yapiFlashDevice(yFlashArg *arg, char *errmsg)
{
    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    if((yContext->detecttype & Y_DETECT_USB) ==0) {
        return YERRMSG(YAPI_INVALID_ARGUMENT,"You must init the yAPI with Y_DETECT_USB flag");
    }


    return prog_FlashDevice(arg, 1, errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT yapiVerifyDevice(yFlashArg *arg, char *errmsg)
{
    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    if((yContext->detecttype & Y_DETECT_USB) ==0) {
        return YERRMSG(YAPI_INVALID_ARGUMENT,"You must init the yAPI with Y_DETECT_USB flag");
    }

    return prog_FlashDevice(arg, 0, errmsg);
}


YRETCODE YAPI_FUNCTION_EXPORT yapiGetBootloadersDevs(char *serials,unsigned int maxNbSerial, unsigned int *totalBootladers, char *errmsg)
{
    int             nbifaces=0;
    yInterfaceSt    *iface;
    yInterfaceSt    *runifaces=NULL;
    int             i;
    u32             totalBoot,copyedBoot;
    char            *s=serials;

    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    if((yContext->detecttype & Y_DETECT_USB) ==0) {
        return YERRMSG(YAPI_INVALID_ARGUMENT,"You must init the yAPI with Y_DETECT_USB flag");
    }

    YPROPERR(yUSBGetInterfaces(&runifaces,&nbifaces,errmsg));

    totalBoot=copyedBoot=0;

    for(i=0, iface=runifaces ; i < nbifaces ; i++, iface++){
        if(iface->deviceid != YOCTO_DEVID_BOOTLOADER)
            continue;
        if(serials && copyedBoot < maxNbSerial){
            YSTRCPY(s,YOCTO_SERIAL_LEN*2,iface->serial);
            s+=YOCTO_SERIAL_LEN;
            copyedBoot++;
        }
        totalBoot++;
    }
    // free all tmp ifaces
    if(runifaces){
        yFree(runifaces);
    }
    if(totalBootladers)
        *totalBootladers=totalBoot;

    return copyedBoot;

}

void yapiRegisterRawNotificationCb(yRawNotificationCb callback)
{
    if(!yContext)
        return;

    yEnterCriticalSection(&yContext->enum_cs);
    yContext->rawNotificationCb = callback;
    yLeaveCriticalSection(&yContext->enum_cs);
}




// return the min of strlen and maxlen
static unsigned ystrnlen(const char *src,unsigned maxlen)
{
    unsigned len;
    for (len=0 ; *src && len < maxlen ;len++,src++);
    return len;
}

YRETCODE ystrcpy_s(char *dst, unsigned dstsize,const char *src)
{
    
    return ystrncpy_s(dst,dstsize,src,dstsize);
}


YRETCODE ystrcat_s(char *dst, unsigned dstsize,const char *src)
{
    return ystrncat_s(dst, dstsize, src, dstsize);
}

int ysprintf_s(char *dst, unsigned dstsize,const char *fmt ,...)
{
    int len;
    va_list args;
    va_start( args, fmt );
    len = yvsprintf_s(dst,dstsize,fmt,args);
    va_end(args);
    return len;
}



YRETCODE ystrncpy_s(char *dst,unsigned dstsize,const char *src,unsigned arglen)
{
    unsigned len;
    
    if (dst==NULL){
        YPANNIC;
        return YAPI_INVALID_ARGUMENT;
    }
    if (src==NULL){
        YPANNIC;
        return YAPI_INVALID_ARGUMENT;
    }
    if(dstsize ==0){
        YPANNIC;
        return YAPI_INVALID_ARGUMENT;
    }
    len = ystrnlen(src,arglen);
    if(len+1 > dstsize){
        YPANNIC;
        dst[0]=0;
        return YAPI_INVALID_ARGUMENT;
    }else{
        memcpy(dst,src,len);
        dst[len]=0;
    }
    return 0;
    
}


YRETCODE ystrncat_s(char *dst, unsigned dstsize,const char *src,unsigned len)
{
    unsigned dstlen;
    if (dst==NULL){
        YPANNIC;
        return YAPI_INVALID_ARGUMENT;
    }
    if (src==NULL){
        YPANNIC;
        return YAPI_INVALID_ARGUMENT;
    }
    dstlen = ystrnlen(dst, dstsize);
    if(dstlen+1 >= dstsize){
        YPANNIC;
        return YAPI_INVALID_ARGUMENT;
    }
    return ystrncpy_s(dst+dstlen, dstsize-dstlen, src, len);
}


int yvsprintf_s (char *dst, unsigned dstsize, const char * fmt, va_list arg )
{
    int len;
    
	len = vsnprintf(dst,dstsize,fmt,arg);
    if(len <0 || len >=(long)dstsize){
        YPANNIC;
		dst[dstsize-1]=0;
        return YAPI_INVALID_ARGUMENT;
    }
    return len;
}

