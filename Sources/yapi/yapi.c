/*********************************************************************
 *
 * $Id: yapi.c 65642 2025-04-08 09:22:17Z seb $
 *
 * Implementation of public entry points to the low-level API
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
#include "ydef_private.h"
#define __FILE_ID__     MK_FILEID('Y','A','P')
#define __FILENAME__   "yapi"
// ReSharper disable CppClangTidyClangDiagnosticImplicitIntConversion
// ReSharper disable CppClangTidyBugproneNarrowingConversions
// ReSharper disable CommentTypo
// ReSharper disable CppLocalVariableMayBeConst
// ReSharper disable CppParameterMayBeConstPtrOrRef
#include "yapi.h"
#include "yproto.h"
#ifdef YAPI_IN_YDEVICE
#include "privhash.h"
#else
#include "yhash.h"
#endif
#include "yjson.h"
#include "yprog.h"
#include "yssl.h"

#ifdef WINDOWS_API
#include <time.h>
#else
#include <sys/time.h>
#include <dlfcn.h>
#endif

#ifdef LINUX_API
#include <sys/utsname.h>
#include <unistd.h>

int write_text_file(const char *filename, const char *content, int contentlen, char* errmsg)
{
    int res =0;
    FILE *f;

    if (YFOPEN(&f, filename, "w") != 0) {
        int err = errno;
        YSPRINTF(errmsg,YOCTO_ERRMSG_LEN, "Unable to open file %s (%s)", filename, strerror(err));
        if(err == EACCES){
            return YAPI_UNAUTHORIZED;
        } else {
            return YAPI_IO_ERROR;
        }
    }
    if (fwrite(content, 1, contentlen, f) <=0) {
        YSPRINTF(errmsg,YOCTO_ERRMSG_LEN, "Unable to write to file %s", filename);
        res = YAPI_IO_ERROR;
    }
    fclose(f);
    return res;
}

#endif

YRETCODE YAPI_FUNCTION_EXPORT yapiAddUdevRulesForYocto(int force, char *errmsg)
{
#ifdef LINUX_API
#define UDEV_RULE "# udev rules to allow write access to all users for Yoctopuce USB devices\nSUBSYSTEM==\"usb\", ATTR{idVendor}==\"24e0\", MODE=\"0666\"\n"
    const char *filename = "/etc/udev/rules.d/51-yoctopuce.rules";
    if (force == 0 && access(filename, F_OK) == 0) {
        // file already present
        return YAPI_SUCCESS;
    }


    return write_text_file( filename,
                            UDEV_RULE,
                            YSTRLEN(UDEV_RULE),
                            errmsg);
#else
    return YERRMSG(YAPI_NOT_SUPPORTED, "Only available on Linux");
#endif
}


static YRETCODE yapiUpdateDeviceList_internal(u32 forceupdate, char *errmsg);
static void yapiUnregisterHub_internal(const char *url);
static void yapiCleanupDisabledHub(void);
static YRETCODE yapiPreregisterHub_internal(const char *url, char *errmsg);
static YRETCODE yapiHandleEvents_internal(char *errmsg);
static YRETCODE yapiHTTPRequestAsyncEx_internal(int tcpchan, const char *device, const char *request, int len, yapiRequestAsyncCallback callback, void *context, char *errmsg);

//#define DEBUG_YAPI_REQ


#ifdef DEBUG_YAPI_REQ

#include <direct.h>
#include <stdio.h>
#define FILE_NO_BIT_MASK 0XFFF

static yCRITICAL_SECTION   YREQ_CS;
static int global_req_count = 0;

static int write_onfile(int fileno, char *mode, const char *firstline, int firstline_len, const char * buffer, int bufferlen)
{
    char filename[128];
    FILE *f;
    int retry_count = 1;
    YSPRINTF(filename, 128,"req_trace\\req_%03d.bin", fileno);
    // write on file
retry:
    if (YFOPEN(&f, filename, mode) != 0) {
        if (retry_count--){
            _mkdir("req_trace");
            goto retry;
        }
        return -1;
    }
    fwrite(firstline, 1, firstline_len, f);
    if (buffer) {
        fwrite(buffer, 1, bufferlen, f);
    }
    fclose(f);
    return 0;
}





static int YREQ_LOG_START(const char *msg, const char *device, const char * request, int requestsize)
{
    char buffer[2048], buffer2[64];
    struct tm timeinfo;
    time_t rawtime;
    int threadIdx, count, first_len;
    int fileno;

    //we may need to add mutex here
    yEnterCriticalSection(&YREQ_CS);
    count = global_req_count++;
    yLeaveCriticalSection(&YREQ_CS);
    fileno = count & FILE_NO_BIT_MASK;
    // compute time string
    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);
    strftime(buffer2, sizeof(buffer2), "%Y-%m-%d %H:%M:%S", &timeinfo);
    //format first info line
    threadIdx = yThreadIndex();
    first_len = YSPRINTF(buffer, 2048, "[%s/%"FMTu64"](%d):no=%d = %s on %s\n",buffer2, yapiGetTickCount(), threadIdx, count, msg, device);
    write_onfile(fileno, "wb", buffer, first_len, request, requestsize);
    return count;
}

static int YREQ_LOG_APPEND(int count, const char *msg, const char * response, int responsesize, u64 start_tm)
{
    char buffer[2048], buffer2[64];
    struct tm timeinfo;
    time_t rawtime;
    int threadIdx, first_len;

    int fileno = count & FILE_NO_BIT_MASK;
    // compute time string
    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);
    strftime(buffer2, sizeof(buffer2), "%Y-%m-%d %H:%M:%S", &timeinfo);
    //format first info line
    threadIdx = yThreadIndex();
    first_len = YSPRINTF(buffer, 2048, "[%s/%"FMTu64"](%d):no=%d : %s responsesize=%d\n",buffer2, yapiGetTickCount() - start_tm, threadIdx, count, msg, responsesize);
    // write on file
    write_onfile(fileno, "ab", buffer, first_len, response, responsesize);
    return count;
}

static int YREQ_LOG_APPEND_ERR(int count, const char *msg, const char *errmsg, int error, u64 start_tm)
{
    char buffer[2048], buffer2[64];
    struct tm timeinfo;
    time_t rawtime;
    int threadIdx, first_len;

    int fileno = count & FILE_NO_BIT_MASK;
    // compute time string
    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);
    strftime(buffer2, sizeof(buffer2), "%Y-%m-%d %H:%M:%S", &timeinfo);
    //format first info line
    threadIdx = yThreadIndex();
    first_len = YSPRINTF(buffer, 2048, "[%s/%"FMTu64"](%d):no=%d : %s errmsg(%d)=%s\n", buffer2, yapiGetTickCount()- start_tm, threadIdx, count, msg, error, errmsg);
    // write on file
    write_onfile(fileno, "ab", buffer, first_len, NULL, 0);
    return count;
}
#endif


//#define DEBUG_CALLBACK

#ifdef DEBUG_CALLBACK


#include <direct.h>
static int global_callback_count = 0;

static write_cb_onfile(YAPI_FUNCTION fundescr, const char *value)
{
    char filename[128];
    FILE *f;
    int retry_count = 1;
    int fileno = global_callback_count >> 16;
    char *mode ="ab";
    char buffer[2048], buffer2[64];
    struct tm timeinfo;
    time_t rawtime;
    int threadIdx, bufferlen;
    char serial[YOCTO_SERIAL_LEN], funcId[YOCTO_FUNCTION_LEN];

    if ((global_callback_count & 0xffff) == 0) {
        mode = "wb";
    }

    YSPRINTF(filename, 128,"callback_trace\\callback_%d.txt", fileno);
    // compute time string
    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);
    strftime(buffer2, sizeof(buffer2), "%Y-%m-%d %H:%M:%S", &timeinfo);
    //format first info line
    threadIdx = yThreadIndex();
    if (value == NULL){
        value = "(NULL)";
    }
    if(ypGetFunctionInfo(fundescr, serial, funcId, NULL, NULL) < 0){
        bufferlen = YSPRINTF(buffer, 2048, "[%s/%"FMTu64"](%d):no=%d : invalid function ID (%X)\n",buffer2, yapiGetTickCount(), threadIdx, global_callback_count, fundescr);
    }else {
        bufferlen = YSPRINTF(buffer, 2048, "[%s/%"FMTu64"](%d):no=%d : %s.%s (%X)=%s\n",buffer2, yapiGetTickCount(), threadIdx, global_callback_count, serial,funcId, fundescr, value);
    }
    global_callback_count++;


    // write on file
retry:
    if (YFOPEN(&f, filename, mode) != 0) {
        if (retry_count--){
            _mkdir("callback_trace");
            goto retry;
        }
        return -1;
    }
    fwrite(buffer, 1, bufferlen, f);
    fclose(f);
    return 0;
}

static write_timedcb_onfile(YAPI_FUNCTION fundescr, double deviceTime, const u8 *report, u32 len)
{
    char buffer[256];
    YSPRINTF(buffer, 2048, "Timed %f %p:%u",deviceTime, report, len);
    return write_cb_onfile(fundescr, buffer);
}

#endif


void yFunctionUpdate(YAPI_FUNCTION fundescr, const char *value)
{
    if (yContext->functionCallback) {
        yEnterCriticalSection(&yContext->functionCallbackCS);
#ifdef DEBUG_CALLBACK
        write_cb_onfile(fundescr, value);
#endif
        yContext->functionCallback(fundescr, value);
        yLeaveCriticalSection(&yContext->functionCallbackCS);
    }
}

void yFunctionTimedUpdate(YAPI_FUNCTION fundescr, u64 deviceTimeMs, u64 durationMs, const u8 *report, u32 len)
{
    if (yContext->timedReportCallback) {
        double duration = 0;
        if (durationMs > 0) {
            duration = (double)durationMs / 1000.0;
        }
        yEnterCriticalSection(&yContext->functionCallbackCS);
#ifdef DEBUG_CALLBACK
        write_timedcb_onfile(fundescr, deviceTime, report, len);
#endif
        yContext->timedReportCallback(fundescr, (double)deviceTimeMs / 1000.0, report, len, duration);
        yLeaveCriticalSection(&yContext->functionCallbackCS);
    }
}

/*****************************************************************************
 Internal functions for hub enumeration
 ****************************************************************************/

void yFreeParsedURL(HubURLSt *hub)
{
    if (hub->org_url) {
        yFree(hub->org_url);
        hub->org_url = NULL;
    }
    if (hub->user) {
        yFree(hub->user);
        hub->user = NULL;
    }
    if (hub->password) {
        yFree(hub->password);
        hub->password = NULL;
    }
    if (hub->subdomain) {
        yFree(hub->subdomain);
        hub->subdomain = NULL;
    }
    if (hub->host) {
        yFree(hub->host);
        hub->host = NULL;
    }
}

int yParseHubURL(HubURLSt *hub_url, const char *url, char *errmsg)
{
    int len, domlen;
    const char *end, *p, *endv6;
    const char *pos, *posplus;
    char buffer[8];

    hub_url->org_url = YSTRDUP(url);
    // set all hash as invalid
    hub_url->proto = PROTO_LEGACY;
    hub_url->user = NULL;
    hub_url->password = NULL;
    hub_url->host = NULL;
    hub_url->subdomain = YSTRDUP("");
    if (YSTRNCMP(url, "auto://", 7) == 0) {
        url += 7;
        hub_url->proto = PROTO_AUTO;
    } else if (YSTRNCMP(url, "secure://", 9) == 0) {
        url += 9;
        hub_url->proto = PROTO_SECURE;
    } else if (YSTRNCMP(url, "http://", 7) == 0) {
        url += 7;
        hub_url->proto = PROTO_HTTP;
    } else if (YSTRNCMP(url, "ws://", 5) == 0) {
        url += 5;
        hub_url->proto = PROTO_WEBSOCKET;
    } else if (YSTRNCMP(url, "https://", 8) == 0) {
        url += 8;
        hub_url->proto = PROTO_SECURE_HTTP;
    } else if (YSTRNCMP(url, "wss://", 6) == 0) {
        url += 6;
        hub_url->proto = PROTO_SECURE_WEBSOCKET;
    }
    // search for any authentication info
    for (p = url; *p && *p != '@' && *p != '/'; p++);
    if (*p == '@') {
        for (p = url; *p != ':' && *p != '@'; p++);
        if (*p != ':') {
            if (errmsg)
                YSTRCPY(errmsg, YOCTO_ERRMSG_LEN, "missing authentication parameter");
            yFreeParsedURL(hub_url);
            return YAPI_INVALID_ARGUMENT;
        }
        len = (int)(p - url);
        if (len > HASH_BUF_SIZE) {
            if (errmsg)
                YSTRCPY(errmsg, YOCTO_ERRMSG_LEN, "username too long");
            yFreeParsedURL(hub_url);
            return YAPI_INVALID_ARGUMENT;
        }
        hub_url->user = ystrndup_s(url, len);
        //dbglog("user=%s\n", hub->user);
        url = ++p;
        while (*p != '@') p++;
        len = (int)(p - url);
        if (len > HASH_BUF_SIZE) {
            if (errmsg)
                YSTRCPY(errmsg, YOCTO_ERRMSG_LEN, "password too long");
            yFreeParsedURL(hub_url);
            return YAPI_INVALID_ARGUMENT;
        }
        hub_url->password = ystrndup_s(url, len);
        //dbglog("passwd=%s\n", hub->password);
        url = ++p;
    }
    end = url + strlen(url);
    p = strchr(url, '/');
    if (p) {
        len = (int)(end - p);
        if (len > 1) {
            if (*(end - 1) == '/') {
                len--;
            }
            hub_url->subdomain = ystrndup_s(p, len);
            //dbglog("subdomain=%s\n", hub->subdomain);
        }
        end = p;
    }

    endv6 = strchr(url, ']');
    pos = strchr(url, ':');
    if (endv6 && pos && endv6 < end && endv6 > url) {
        // ipv6 URL
        hub_url->host = ystrndup_s(url, (int)(endv6 + 1 - url));
        pos = strchr(endv6, ':');
    }
    posplus = pos + 1;
    if (pos && pos < end) {
        len = (int)(end - posplus);
        if (len > 7) {
            if (errmsg)
                YSTRCPY(errmsg, YOCTO_ERRMSG_LEN, "invalid port");
            yFreeParsedURL(hub_url);
            return YAPI_INVALID_ARGUMENT;
        }
        memcpy(buffer, posplus, len);
        buffer[len] = '\0';
        hub_url->portno = atoi(buffer);
        end = pos;
    } else {
        if (hub_url->proto == PROTO_HTTP && hub_url->subdomain[0]) {
            //http proto + subdomain + no port specified -> Vhub4Web -> Use port 80 instead of 4444
            hub_url->portno = 80;
        } else if (hub_url->proto == PROTO_SECURE_HTTP && hub_url->subdomain[0]) {
            //http proto + subdomain + no port specified -> Vhub4Web -> Use port 443 instead of 4444
            hub_url->portno = 443;
        } else {
            if (hub_url->proto == PROTO_SECURE_HTTP || hub_url->proto == PROTO_SECURE_WEBSOCKET || hub_url->proto == PROTO_SECURE) {
                hub_url->portno = YOCTO_DEFAULT_HTTPS_PORT;
            } else {
                hub_url->portno = YOCTO_DEFAULT_PORT;
            }
        }
    }
    //dbglog("port=%d\n", hub->portno);
    if (hub_url->host == NULL) {
        domlen = (int)(end - url);
        hub_url->host = ystrndup_s(url, domlen);
        //dbglog("domain(host)=%s\n", hub->host);
    }
    return YAPI_SUCCESS;
}


static HubSt* yGetHubFromURL(const char *url)
{
    HubURLSt tmp_url;
    int i, j;
    int res = yParseHubURL(&tmp_url, url, NULL);
    if (res < 0) {
        return NULL;
    }
    for (i = 0; i < NBMAX_NET_HUB; i++) {
        if (yContext->nethub[i]) {
            if (YSTRCMP(tmp_url.org_url, yContext->nethub[i]->url.org_url) == 0) {
                yFreeParsedURL(&tmp_url);
                return yContext->nethub[i];
            }
            if (YSTRCMP(tmp_url.host, yContext->nethub[i]->url.host) == 0 &&
                YSTRCMP(tmp_url.subdomain, yContext->nethub[i]->url.subdomain) == 0 &&
                tmp_url.portno == yContext->nethub[i]->url.portno) {
                yFreeParsedURL(&tmp_url);
                return yContext->nethub[i];
            }
            for (j = 0; yContext->nethub[i]->know_urls[j] && j < MAX_KNOW_URLS_SIZE; j++) {
                if (YSTRCMP(url, yContext->nethub[i]->know_urls[j]) == 0) {
                    yFreeParsedURL(&tmp_url);
                    return yContext->nethub[i];
                }
            }
        }

    }
    yFreeParsedURL(&tmp_url);
    return NULL;
}

typedef enum {
    ENU_HTTP_START,
    ENU_JSON_START,
    ENU_API,
    ENU_MODULE_START,
    ENU_MODULE,
    ENU_MODULE_FIRMWARE,
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
    ENU_YP_BASETYPE,
    ENU_YP_HARDWAREID,
    ENU_YP_LOGICALNAME,
    ENU_YP_PRODUCTNAME,
    ENU_YP_ADVERTISEDVALUE,
    ENU_YP_INDEX,
    ENU_SKIP_FUNCTION_START,
    ENU_SKIP_FUNCTION
} ENU_PARSE_STATE;


#ifdef DEBUG_NET_ENUM

extern const char* yJsonStateStr[];
const char* ENU_PARSE_STATE_STR[] =
{
    "ENU_HTTP_START",
    "ENU_JSON_START",
    "ENU_API",
    "ENU_MODULE_START",
    "ENU_MODULE",
    "ENU_MODULE_FIRMWARE",
    "ENU_NETWORK_START",
    "ENU_NETWORK",
    "ENU_NET_ADMINPWD",
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
    "ENU_YP_BASETYPE",
    "ENU_YP_HARDWAREID",
    "ENU_YP_LOGICALNAME",
    "ENU_YP_PRODUCTNAME",
    "ENU_YP_ADVERTISEDVALUE",
    "ENU_YP_INDEX",
    "ENU_SKIP_FUNCTION_START",
    "ENU_SKIP_FUNCTION"
};
#endif


typedef enum {
    WP_SERIAL,
    WP_LOGICALNAME,
    WP_PRODUCT,
    WP_PROD_ID,
    WP_NET_URL,
    WP_BEACON,
    WP_INDEX,
    WP_DONE
} ENU_WP_STATE;


typedef enum {
    YP_BASETYPE,
    YP_HARDWAREID,
    YP_LOGICALNAME,
    YP_ADV_VAL,
    YP_INDEX,
    YP_DONE
} ENU_YP_STATE;

typedef struct {
    HubSt *hub;
    ENU_PARSE_STATE state;
    yStrRef serial;
    yStrRef logicalName;

    union {
        struct {
            yStrRef productName;
            u16 productId;
            yUrlRef hubref;
            s8 beacon;
            u8 devYdx;
        };

        struct {
            yStrRef ypCateg;
            yStrRef funcId;
            char advertisedValue[YOCTO_PUBVAL_LEN];
            u8 funClass;
            u8 funYdx;
        };
    };

    ENU_YP_STATE yp_state;
    ENU_WP_STATE wp_state;
    int nbKnownDevices;
    yStrRef *knownDevices;
} ENU_CONTEXT;


/*****************************************************************************
 * Generic device information stuff
 ***************************************************************************/

void initDevYdxInfos(int devYdx, yStrRef serial)
{
    yGenericDeviceSt *gen = yContext->generic_infos + devYdx;
    YASSERT(devYdx < ALLOC_YDX_PER_HUB, devYdx);
    yEnterCriticalSection(&yContext->generic_cs);
    memset(gen, 0, sizeof(yGenericDeviceSt));
    gen->serial = serial;
    yLeaveCriticalSection(&yContext->generic_cs);
}

void freeDevYdxInfos(int devYdx)
{
    yGenericDeviceSt *gen = yContext->generic_infos + devYdx;
    YASSERT(devYdx < ALLOC_YDX_PER_HUB, devYdx);
    yEnterCriticalSection(&yContext->generic_cs);
    gen->serial = YSTRREF_EMPTY_STRING;
    yLeaveCriticalSection(&yContext->generic_cs);
}

#define LOG_BUFFER_SIZE 512

static void logResult(void *context, const u8 *result, u32 resultlen, int retcode, const char *errmsg)
{
    char buffer[LOG_BUFFER_SIZE];
    yGenericDeviceSt *gen = (yGenericDeviceSt*)context;
    int poslen, to_cpy;
    const char *p = (char*)result;
    const char *start = (char*)result;

    if (yContext == NULL || yContext->logDeviceCallback == NULL)
        return;

    if (resultlen < 4) {
        return; //invalid packet
    }

    if (result == NULL || start[0] != 'O' || start[1] != 'K') {
        return; // invalid response
    }
    // drop http header
    while (resultlen >= 4) {
        if (p[0] == '\r' && p[1] == '\n' && p[2] == '\r' && p[3] == '\n') {
            resultlen -= 4;
            p += 4;
            break;
        }
        p++;
        resultlen--;
    }
    start = p;
    //look of '@pos'
    p = start + resultlen - 1;
    poslen = 0;
    while (p > start && *p != '@') {
        if (*p < '0' || *p > '9') {
            poslen = 0;
        } else {
            poslen++;
        }
        p--;
        resultlen--;
    }

    if (*p != '@') {
        return;
    }
    to_cpy = poslen > (LOG_BUFFER_SIZE - 1) ? (LOG_BUFFER_SIZE - 1) : poslen;
    memcpy(buffer, p + 1, to_cpy);
    buffer[to_cpy] = '\0';
    //remove empty line before @pos
    if (resultlen == 0)
        return;
    resultlen -= 2;
    p = start;
    yEnterCriticalSection(&yContext->generic_cs);
    gen->deviceLogPos = atoi(buffer);
    yLeaveCriticalSection(&yContext->generic_cs);
    while (resultlen) {
        if (*p == '\n') {
            int linelen = (int)(p - start);
            to_cpy = linelen > (LOG_BUFFER_SIZE - 1) ? (LOG_BUFFER_SIZE - 1) : linelen;
            memcpy(buffer, start, to_cpy);
            buffer[to_cpy] = '\0';
            //dbglog("log:[%s]\n", buffer);
            yContext->logDeviceCallback(gen->serial, buffer);
            start = p + 1;
        }
        p++;
        resultlen--;
    }
    yEnterCriticalSection(&yContext->generic_cs);
    gen->flags &= ~(DEVGEN_LOG_PENDING | DEVGEN_LOG_PULLING);
    yLeaveCriticalSection(&yContext->generic_cs);
}

static int yapiRequestOpenWS(YIOHDL_internal *iohdl, HubSt *hub, YAPI_DEVICE dev, int tcpchan, const char *request, int reqlen, u64 mstimeout, yapiRequestAsyncCallback callback, void *context, RequestProgress progress_cb, void *progress_ctx, char *errmsg);
static int yapiRequestOpenHTTP(YIOHDL_internal *iohdl, HubSt *hub, YAPI_DEVICE dev, const char *request, int reqlen, int wait_for_start, u64 mstimeout, yapiRequestAsyncCallback callback, void *context, char *errmsg);
static int yapiRequestOpenUSB(YIOHDL_internal *iohdl, HubSt *hub, YAPI_DEVICE dev, const char *request, int reqlen, u64 unused_timeout, yapiRequestAsyncCallback callback, void *context, char *errmsg);


YRETCODE yapiPullDeviceLogEx(int devydx)
{
    YRETCODE res;
    int used;
    char rootdevice[YOCTO_SERIAL_LEN];
    char request[512];
    int reqlen;
    char errmsg[YOCTO_ERRMSG_LEN];
    char *p;
    YAPI_DEVICE dev;
    u32 logPos;
    int doPull = 0;
    yGenericDeviceSt *gen;
    yStrRef serialref;
    YIOHDL_internal iohdl;
    HubSt *hub;

    yEnterCriticalSection(&yContext->generic_cs);
    gen = yContext->generic_infos + devydx;
    if ((gen->flags & DEVGEN_LOG_ACTIVATED) &&
        (gen->flags & DEVGEN_LOG_PENDING) &&
        (gen->flags & DEVGEN_LOG_PULLING) == 0) {
        doPull = 1;
        gen->flags |= DEVGEN_LOG_PULLING;
    }
    logPos = gen->deviceLogPos;
    serialref = gen->serial;
    yLeaveCriticalSection(&yContext->generic_cs);
    if (serialref == YSTRREF_EMPTY_STRING || !doPull) {
        return YAPI_SUCCESS;
    }
    dev = wpSearchEx(serialref);
    YSTRCPY(request, 512, "GET ");
    p = request + 4;
    res = yapiGetDevicePath(dev, rootdevice, p, 512 - 5, NULL, errmsg);
    if (YISERR(res)) {
        dbglog(errmsg);
        if (res != YAPI_DEVICE_NOT_FOUND) {
            yEnterCriticalSection(&yContext->generic_cs);
            gen->flags &= ~DEVGEN_LOG_PULLING;
            yLeaveCriticalSection(&yContext->generic_cs);
        }
        return res;
    }
#ifdef TRACE_NET_HUB
    dbglog("TRACE pull device log\n");
#endif
    used = YSTRLEN(request);
    p = request + used;
    YSPRINTF(p, 512 - used, "logs.txt?pos=%d \r\n\r\n", logPos);
    reqlen = YSTRLEN(request);
    memset(&iohdl, 0, sizeof(YIOHDL_internal));
    // compute request timeout
    // dispatch request on correct hub (or pseudo usb HUB)
    errmsg[0] = 0;
    hub = ywpGetDeviceHub(serialref);
    if (hub == FAKE_USB_HUB) {
        res = yapiRequestOpenUSB(&iohdl, NULL, dev, request, reqlen, YIO_10_MINUTES_TCP_TIMEOUT, logResult, (void*)gen, errmsg);
    } else {
        if (hub->url.proto != PROTO_HTTP) {
            res = yapiRequestOpenWS(&iohdl, hub, dev, 0, request, reqlen, YIO_10_MINUTES_TCP_TIMEOUT, logResult, (void*)gen, NULL, NULL, errmsg);
        } else {
            if (hub->url.proto != PROTO_HTTP && hub->url.proto != PROTO_SECURE_HTTP) {
                res = yapiRequestOpenWS(&iohdl, hub, dev, 0, request, reqlen, YIO_10_MINUTES_TCP_TIMEOUT, logResult, (void*)gen, NULL, NULL, errmsg);
            } else {
                res = yapiRequestOpenHTTP(&iohdl, hub, dev, request, reqlen, 0, YIO_10_MINUTES_TCP_TIMEOUT, logResult, (void*)gen, errmsg);
            }
        }
    }

#ifdef TRACE_NET_HUB
    dbglog("TRACE pull returned %d:%s\n", res, errmsg);
#endif
    if (YISERR(res)) {
        yEnterCriticalSection(&yContext->generic_cs);
        gen->flags &= ~DEVGEN_LOG_PULLING;
        yLeaveCriticalSection(&yContext->generic_cs);
        return res;
    }
    return res;
}


YRETCODE yapiPullDeviceLog(const char *serial)
{
    YAPI_DEVICE dev;
    int devydx;
    dev = wpSearch(serial);

    devydx = wpGetDevYdx(dev & 0xffff);
    if (devydx < 0) {
        return YAPI_DEVICE_NOT_FOUND;
    }
    return yapiPullDeviceLogEx(devydx);
}

// no_auth : 0-> full details, 1->passwd obfuscade 2 -> no auth info
int sprintfURL(char *out, int maxlen, HubURLSt *url, int no_auth)
{
    const char *protoStr;
    const char *userStr = "";
    const char *subdomainStr = "";
    const char *sep1 = "";
    const char *sep2 = "";
    const char *passStr = "";
    const char *type = "";

    protoStr = "auto://";
    switch (url->proto) {
    case PROTO_LEGACY:
        break;
    case PROTO_AUTO:
        protoStr = "auto://";
        break;
    case PROTO_SECURE:
        protoStr = "secure://";
        break;
    case PROTO_HTTP:
        protoStr = "http://";
        break;
    case PROTO_WEBSOCKET:
        protoStr = "ws://";
        break;
    case PROTO_SECURE_HTTP:
        protoStr = "https://";
        break;
    case PROTO_SECURE_WEBSOCKET:
        protoStr = "wss://";
        break;
    case PROTO_UNKNOWN:
    default:
        protoStr = "unk://";
        break;
    }
    if (no_auth < 2) {
        if (url->user) {
            sep1 = "@";
            userStr = url->user;
        }
        if (url->password) {
            sep2 = ":";
            if (no_auth == 0) {
                passStr = url->password;
            } else {
                passStr = "XXXX";
            }
        }
    }
    if (url->subdomain) {
        subdomainStr = url->subdomain;
    }
    return YSPRINTF(out, maxlen, "%s%s%s%s%s%s%s:%d%s/", type, protoStr, userStr, sep2, passStr, sep1, url->host, url->portno, subdomainStr);
}


int checkForSameHubAccess(HubSt *hub, yStrRef serial, char *errmsg)
{
    int i;
    int res = YAPI_SUCCESS;
    char buffer[YOCTO_MAX_URL_LEN];

    for (i = 0; i < NBMAX_NET_HUB; i++) {
        if (yContext->nethub[i] == NULL || yContext->nethub[i] == hub || (yContext->nethub[i]->flags & DISABLED_HUB)) {
            continue;
        }
        if (yContext->nethub[i]->serial_hash == serial) {
            int j;
            sprintfURL(buffer, YOCTO_MAX_URL_LEN, &yContext->nethub[i]->url, 1);
            if (errmsg) {
                char serialbuf[32];
                yHashGetStr(serial, serialbuf, sizeof(serialbuf));
                YSPRINTF(errmsg, YOCTO_ERRMSG_LEN, "Hub %s is already registered with URL %s", serialbuf, buffer);
            }
#ifdef TRACE_NET_HUB
            dbglog("Hub %s is already registered with URL %s(%d)\n", yHashGetStrPtr(serial), buffer, i);
#endif
            hub->flags |= DISABLED_HUB;
            for (j = 0; yContext->nethub[i]->know_urls[j] && j < MAX_KNOW_URLS_SIZE; j++) {
                if (YSTRCMP(hub->url.org_url, yContext->nethub[i]->know_urls[j])) {
                    if (yContext->nethub[i]->creation_time < hub->creation_time) {
                        yContext->nethub[i]->mandatory = hub->mandatory;
                    }
                    return YAPI_DOUBLE_ACCES;
                }
            }
            if (j < MAX_KNOW_URLS_SIZE) {
                yContext->nethub[i]->know_urls[j] = YSTRDUP(hub->url.org_url);
                if (yContext->nethub[i]->creation_time < hub->creation_time) {
                    yContext->nethub[i]->mandatory = hub->mandatory;
                }
                return YAPI_DOUBLE_ACCES;
            } else {
                return YAPI_EXHAUSTED;
            }
        }
#if 0
        else {
            const char *serial_str;

            if (hub_serial_ref != INVALID_HASH_IDX) {
                serial_str = yHashGetStrPtr(hub_serial_ref);
            } else {
                serial_str = "unknown";
            }
            sprintfURL(buffer, YOCTO_MAX_URL_LEN, &yContext->nethub[i]->url, 1);
            dbglog("Hub %s is not the same serial as %s %s (%d)\n", yHashGetStrPtr(serial), serial_str, buffer, i);
        }
#endif
    }
    return res;
}


HubSt* ywpGetDeviceHub(yStrRef serialref)
{
    int i, n;

    for (n = 0; n < NBMAX_NET_HUB; n++) {
        if (yContext->nethub[n] == NULL) {
            continue;
        }
        for (i = 0; i < ALLOC_YDX_PER_HUB; i++) {
            if (yContext->nethub[n]->devices[i] == serialref) {
                return yContext->nethub[n];
            }
        }
    }
    for (i = 0; i < ALLOC_YDX_PER_HUB; i++) {
        if (yContext->usbdevices[i] == serialref) {
            return FAKE_USB_HUB;
        }
    }
    return NULL;
}

static int ywpAddDeviceToHub(HubSt *hub, yStrRef serialref)
{
    int i;
    if (hub == FAKE_USB_HUB) {
        for (i = 0; i < ALLOC_YDX_PER_HUB; i++) {
            if (yContext->usbdevices[i] == serialref || yContext->usbdevices[i] == INVALID_HASH_IDX) {
                yContext->usbdevices[i] = serialref;
                return YAPI_SUCCESS;
            }
        }
    } else {
        for (i = 0; i < ALLOC_YDX_PER_HUB; i++) {
            if (hub->devices[i] == serialref || hub->devices[i] == INVALID_HASH_IDX) {
                hub->devices[i] = serialref;
                return YAPI_SUCCESS;
            }
        }
    }
    return YAPI_INVALID_ARGUMENT;
}

static int ywpRemoveDeviceFromHub(HubSt *hub, yStrRef serialref)
{
    int i;
    if (hub == FAKE_USB_HUB) {
        for (i = 0; i < ALLOC_YDX_PER_HUB; i++) {
            if (yContext->usbdevices[i] == serialref) {
                yContext->usbdevices[i] = INVALID_HASH_IDX;
                return YAPI_SUCCESS;
            }
        }
    } else {
        for (i = 0; i < ALLOC_YDX_PER_HUB; i++) {
            if (hub->devices[i] == serialref) {
                hub->devices[i] = INVALID_HASH_IDX;
                return YAPI_SUCCESS;
            }
        }

    }
    return YAPI_INVALID_ARGUMENT;
}

static int ywpGetAllDevUsingNetHub(HubSt *hub, yStrRef *buffer, int sizeInStrRef)
{
    int count = 0;
    int i;

    for (i = 0; i < ALLOC_YDX_PER_HUB; i++) {
        if (hub->devices[i] != INVALID_HASH_IDX) {
            if (sizeInStrRef) {
                *buffer++ = hub->devices[i];
                sizeInStrRef--;
            }
            count++;
        }
    }
    return count;
}

/**
 * Unregister a device from white pages. eventUrl is INVALID_HASH_IDX for USB
 */
void ywpSafeUnregister(HubSt *hub, yStrRef serialref)
{
    HubSt *registeredHub = ywpGetDeviceHub(serialref);
    if (registeredHub && registeredHub != hub) {
        return;
    }

    wpPreventUnregister();
    if (wpMarkForUnregister(serialref)) {
        // Forward high-level notification to API user before deleting data
        if (yContext->removalCallback) {
            yEnterCriticalSection(&yContext->deviceCallbackCS);
            yContext->removalCallback(serialref);
            yLeaveCriticalSection(&yContext->deviceCallbackCS);
        }
    }
    ywpRemoveDeviceFromHub(hub, serialref);
    wpAllowUnregister();
}

// return 1 -> if this we should use devUrl instead of registered URL
static int ywpSafeCheckOverwrite(HubSt *registeredHub, HubSt *hub, yStrRef serial)
{

    if (hub == FAKE_USB_HUB) {
        // no USB device can unregister previous devices
#ifdef  DEBUG_WP
        //dbglog("no USB device can unregister previous devices (%s) \n", hub->host);
#endif
        return 0;
    }
    if (registeredHub == FAKE_USB_HUB) {
#ifdef DEBUG_WP
        //dbglog("unregister same device connected by USB (%s) \n",  hub->host);
#endif
        return 1;
    } else if (registeredHub != hub) {
        if (hub->serial_hash == serial) {
#ifdef DEBUG_WP
            dbglog("unregister same device connected by a VirtualHub (0x%X vs 0x%X) \n", devUrl, hub->host);
#endif
            return 1;
        }

    }
    return 0;
}

/*****************************************************************************
  Function:
    void ywpSafeRegister( HubSt* hub, u8 devYdx, yStrRef serialref,yStrRef lnameref, yStrRef productref, u16 deviceid, s8 beacon)

  Description:
    Register a new device into yapi whites page (and yellow page of the module function). This function
    will call the arrivalCallback. This function will work for usb and TCP
    it will check if this arrival should be dropped (if an hub is connected by usb and ip)

  Parameters:
    HubSt* hub          : HUB used to access the device (for USB this MUSt be FAKE_USB_HUB)
    u8 devYdx           : the devYdy relative to the hub (for usb this MUST be MAX_YDX_PER_HUB)
    yStrRef serialref   : the serial of the device
    yStrRef lnameref    : the logical name of the device
    yStrRef productref  : the product name of the device
    u16 deviceid        : the deviceid of the device
    s8 beacon           : the beacon state
{

 ***************************************************************************/
void ywpSafeRegister(HubSt *hub, u8 devYdx, yStrRef serialref, yStrRef lnameref, yStrRef productref, u16 deviceid, s8 beacon)
{
    HubSt *registeredHub = ywpGetDeviceHub(serialref);
#ifdef DEBUG_WP
    {
        if (hub == FAKE_USB_HUB){
            dbglog("SAFE WP: register %s(0x%X) form USB\n",yHashGetStrPtr(serialref),serialref);
        } else {
            dbglog("SAFE WP: register %s(0x%X) from %s:%u\n",yHashGetStrPtr(serialref),serialref,hub->host, hub->portno);
        }

    }
    dbglog("name    : %s(0x%X)\n",yHashGetStrPtr(lnameref),lnameref);
    dbglog("product : %s(0x%X)\n",yHashGetStrPtr(productref),productref);
    dbglog("device : %x (%d)\n",deviceid,beacon);
#endif

    if (registeredHub && registeredHub != hub) {
        if (ywpSafeCheckOverwrite(registeredHub, hub, serialref)) {
            ywpSafeUnregister(registeredHub, serialref);
        } else {
#ifdef DEBUG_WP
            dbglog("SAFE WP: drop register %s(0x%X)\n", yHashGetStrPtr(serialref), serialref);
#endif
            return;
        }
    }
    ywpAddDeviceToHub(hub, serialref);
    wpRegister(-1, serialref, lnameref, productref, deviceid, INVALID_HASH_IDX, beacon);
    ypRegister(YSTRREF_MODULE_STRING, serialref, YSTRREF_mODULE_STRING, lnameref, YOCTO_AKA_YFUNCTION, -1, NULL);
    if (hub && devYdx < MAX_YDX_PER_HUB) {
        // Update hub-specific devYdx mapping between enus->devYdx and our wp devYdx
        hub->devYdxMap[devYdx] = wpGetDevYdx(serialref);
    }
    // Forward high-level notification to API user
    if (yContext->arrivalCallback) {
        yEnterCriticalSection(&yContext->deviceCallbackCS);
        yContext->arrivalCallback(serialref);
        yLeaveCriticalSection(&yContext->deviceCallbackCS);
    }
}


/*****************************************************************************
  Function:
    void wpSafeUpdate( HubSt* hub, u8 devYdx, yStrRef serialref,yStrRef lnameref, s8 beacon)

  Description:
    Update whites page (and yellow page of the module function ). This function
    will call the changeCallback if needed. This function will work for usb and TCP
    it will check if this update should be dropped (if an hub is connected by usb and ip)

  Parameters:
    For TCP:
        HubSt* hub,         : HUB used to access the device (for USB this MUSB be FAKE_USB_HUB)
        u8 devYdx           : the devYdy relative to the hub (for usb this MUST be MAX_YDX_PER_HUB)
        yStrRef serialref   : the serial of the device
        yStrRef lnameref    : the logical name of the device
        s8 beacon           : the beacon state

 ***************************************************************************/
void ywpSafeUpdate(HubSt *hub, u8 devYdx, yStrRef serialref, yStrRef lnameref, s8 beacon)
{
    int reg;
    HubSt *registeredHub = ywpGetDeviceHub(serialref);
#ifdef DEBUG_WP
    {
        if (hub == FAKE_USB_HUB){
            dbglog("SAFE WP: update %s(0x%X) form USB\n",yHashGetStrPtr(serialref),serialref);
        } else {
            dbglog("SAFE WP: update %s(0x%X) from %s:%u\n", yHashGetStrPtr(serialref), serialref, hub->host, hub->portno);
        }
    }
    dbglog("name    : %s(0x%X)\n",yHashGetStrPtr(lnameref),lnameref);
    dbglog("product : INVALID_HASH_IDX\n");
    dbglog("device : %x (%d)\n",0,beacon);
#endif

    if (registeredHub && ywpSafeCheckOverwrite(registeredHub, hub, serialref)) {
#ifdef DEBUG_WP
        dbglog("SAFE WP: drop update %s(0x%X)\n",yHashGetStrPtr(serialref),serialref);
#endif
        return;
    }
    reg = wpRegister(-1, serialref, lnameref, INVALID_HASH_IDX, 0, INVALID_HASH_IDX, beacon);
    if (reg) {
        if (reg & 1) {
            ypRegister(YSTRREF_MODULE_STRING, serialref, YSTRREF_mODULE_STRING, lnameref, YOCTO_AKA_YFUNCTION, -1, NULL);
            if (hub && devYdx < MAX_YDX_PER_HUB) {
                // Update hub-specific devYdx mapping between enus->devYdx and our wp devYdx
                hub->devYdxMap[devYdx] = wpGetDevYdx(serialref);
            }
            // Forward high-level notification to API user
            if (yContext->changeCallback) {
                yEnterCriticalSection(&yContext->deviceCallbackCS);
                yContext->changeCallback(serialref);
                yLeaveCriticalSection(&yContext->deviceCallbackCS);
            }
        }
        if (reg & 2) {
            if (yContext->beaconCallback) {
                yEnterCriticalSection(&yContext->functionCallbackCS);
                yContext->beaconCallback(serialref, beacon);
                yLeaveCriticalSection(&yContext->functionCallbackCS);
            }
        }
    }
}

int ywpGetDeviceUrl(YAPI_DEVICE devdesc, char *roothubserial, char *request, int requestsize, int *neededsize)
{
    char serial[YOCTO_SERIAL_LEN];
    int fullsize, len;
    HubSt *hub = ywpGetDeviceHub((yStrRef)devdesc);

    if (hub == NULL) {
        return -1;
    }
    // extract root device serial
    if (!request) requestsize = 0;
    if (hub != FAKE_USB_HUB && hub->serial_hash != (yStrRef)devdesc) {
        if (roothubserial) {
            yHashGetStr(hub->serial_hash, roothubserial, YOCTO_SERIAL_LEN);
        }
        if (requestsize > 10) {
            memcpy(request, "/bySerial/", 10);
            request += 10;
            requestsize -= 10;
        }
        fullsize = 11; // null-terminated slash
        yHashGetStr((yStrRef)devdesc, serial, YOCTO_SERIAL_LEN);
        len = (int)strlen(serial) + 1;
        fullsize += len;
        if (requestsize > 0 && requestsize > len) {
            memcpy(request, serial, len - 1);
            request[len - 1] = '/';
            request += len;
            requestsize -= len;
        }
    } else {
        if (roothubserial) {
            yHashGetStr((yStrRef)devdesc, roothubserial, YOCTO_SERIAL_LEN);
        }
        if (requestsize > 1) {
            *request++ = '/';
            requestsize--;
        }
        fullsize = 2; // null-terminated slash
    }
    if (neededsize != NULL) *neededsize = fullsize;
    // null-terminate request
    if (requestsize > 0) *request = 0;

    return 0;
}

static void parseNetWpEntry(ENU_CONTEXT *enus)
{
    int i;


    for (i = 0; i < enus->nbKnownDevices; i++) {
        if (enus->knownDevices[i] != INVALID_HASH_IDX &&
            enus->knownDevices[i] == enus->serial) {
            // mark the device as present (we sweep it later)
#ifdef DEBUG_WP
                dbglog("parseNetWpEntry %X (%s) was present\n",enus->serial,yHashGetStrPtr(enus->serial));
#endif
            enus->knownDevices[i] = INVALID_HASH_IDX;
            break;
        }
    }

    if (i == enus->nbKnownDevices) {
        ywpSafeRegister(enus->hub, enus->devYdx, enus->serial, enus->logicalName, enus->productName, enus->productId, enus->beacon);
    } else {
        ywpSafeUpdate(enus->hub, enus->devYdx, enus->serial, enus->logicalName, enus->beacon);
    }
}

static void unregisterNetDevice(HubSt *hub, yStrRef serialref)
{
    int devydx;

    if (serialref == INVALID_HASH_IDX) return;
    // Free device tcp structure, if needed
    devydx = wpGetDevYdx(serialref);
    if (devydx >= 0 && yContext->tcpreq[devydx]) {
        yReqFree(yContext->tcpreq[devydx]);
        yContext->tcpreq[devydx] = NULL;
    }
    ywpSafeUnregister(hub, serialref);
}

static void ypUpdateNet(ENU_CONTEXT *enus)
{
    if (ypRegister(enus->ypCateg, enus->serial, enus->funcId, enus->logicalName, enus->funClass, enus->funYdx, enus->advertisedValue)) {
        // Forward high-level notification to API user
        yFunctionUpdate(((s32)enus->funcId << 16) | enus->serial, enus->advertisedValue);
    }
}


static void disable_jzon(HubSt *hub)
{
    if ((hub->flags & INCOMPATIBLE_JZON_ENCODING) == 0) {
        dbglog("Disable JZON encoding for hub %s\n", hub->url.host);
    }
    hub->flags |= INCOMPATIBLE_JZON_ENCODING;
}

static void setNotificationConnectionON(HubSt *hub)
{
    hub->notConLastAlive = yapiGetTickCount();
}

static void setNotificationConnectionOFF(HubSt *hub)
{
    hub->notConLastAlive = 0;
}

static int isNotificationConnectionON(HubSt *hub)
{
    if (hub->notConLastAlive && (yapiGetTickCount() - hub->notConLastAlive) < 4000) {
        return 1;
    }
    return hub->state == NET_HUB_ESTABLISHED;
}

static int yEnuJson(ENU_CONTEXT *enus, yJsonStateMachine *j)
{
    char *point;
#ifdef DEBUG_NET_ENUM
    //dbglog("%s: %s %s\n",ENU_PARSE_STATE_STR[enus->state],yJsonStateStr[j->st],j->token);
#endif
    switch (enus->state) {
    case ENU_HTTP_START:
        if (j->st != YJSON_HTTP_READ_CODE || YSTRCMP(j->token, "200")) {
            return YAPI_IO_ERROR;
        }
        enus->state = ENU_JSON_START;
        break;
    case ENU_JSON_START:
        if (j->st == YJSON_HTTP_READ_MSG)
            break;
        if (j->st == YJSON_PARSE_ARRAY)
            return YAPI_IO_ERROR;
        if (j->st == YJSON_PARSE_STRING)
            return YAPI_IO_ERROR;
        enus->state = ENU_API;
        break;
    case ENU_API:
        if (j->st != YJSON_PARSE_MEMBNAME)
            break;
        if (YSTRCMP(j->token, "module") == 0) {
            enus->state = ENU_MODULE_START;
        } else if (YSTRCMP(j->token, "network") == 0) {
            enus->state = ENU_NETWORK_START;
        } else if (YSTRCMP(j->token, "services") == 0) {
            enus->state = ENU_SERVICE;
        } else {
            yJsonSkip(j, 1);
        }
        break;
    case ENU_MODULE_START:
        if (j->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_MODULE;
        }
        break;
    case ENU_MODULE:
        if (j->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_API;
        } else if (j->st == YJSON_PARSE_MEMBNAME) {
            if (YSTRCMP(j->token, "firmwareRelease") == 0) {
                enus->state = ENU_MODULE_FIRMWARE;
            } else {
                yJsonSkip(j, 1);
            }
        }
        break;
    case ENU_MODULE_FIRMWARE:
        NETENUMLOG("firmwareRelease is set to [%s]\n", j->token);
        YSTRCPY(enus->hub->fw_release, YOCTO_FIRMWARE_LEN, j->token);
        enus->state = ENU_MODULE;
        break;
    case ENU_NETWORK_START:
        if (j->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_NETWORK;
        }
        break;
    case ENU_NETWORK:
        if (j->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_API;
        } else if (j->st == YJSON_PARSE_MEMBNAME) {
            if (YSTRCMP(j->token, "adminPassword") == 0) {
                enus->state = ENU_NET_ADMINPWD;
            } else {
                yJsonSkip(j, 1);
            }
        }
        break;
    case ENU_NET_ADMINPWD:
        NETENUMLOG("adminpwd is set to [%s]\n", j->token);
        enus->hub->writeProtected = (j->token[0] != 0);
        enus->state = ENU_NETWORK;
        break;
    case ENU_SERVICE:
        if (j->st != YJSON_PARSE_MEMBNAME)
            break;
        if (YSTRCMP(j->token, "whitePages") == 0) {
            enus->state = ENU_WP_START;
        } else if (YSTRCMP(j->token, "yellowPages") == 0) {
            enus->state = ENU_YP_CONTENT;
        } else {
            yJsonSkip(j, 1);
        }
        break;
    case ENU_WP_START:
        if (j->st == YJSON_PARSE_ARRAY) {
            enus->state = ENU_WP_ARRAY;
        }
        break;
    case ENU_WP_ARRAY:
        if (j->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_WP_ENTRY;
            enus->serial = INVALID_HASH_IDX;
            enus->logicalName = INVALID_HASH_IDX;
            enus->productName = INVALID_HASH_IDX;
            enus->productId = 0;
            enus->hubref = INVALID_HASH_IDX;
            enus->beacon = 0;
            enus->devYdx = -1;
            enus->wp_state = WP_SERIAL;
        } else if (j->st == YJSON_PARSE_ARRAY) {
            enus->state = ENU_SERVICE;
        } else {
            NETENUMLOG("Unknown token %s\n", j->token);
        }
        break;
    case ENU_WP_ENTRY:
        if (j->st == YJSON_PARSE_STRUCT) {
            parseNetWpEntry(enus);
            enus->state = ENU_WP_ARRAY;
        } else if (j->st == YJSON_PARSE_MEMBNAME) {
            if (YSTRCMP(j->token, "serialNumber") == 0) {
                enus->state = ENU_WP_SERIAL;
                if (enus->wp_state != WP_SERIAL) {
                    disable_jzon(enus->hub);
                }
                enus->wp_state = WP_LOGICALNAME;
            } else if (YSTRCMP(j->token, "logicalName") == 0) {
                enus->state = ENU_WP_LOGICALNAME;
                if (enus->wp_state != WP_LOGICALNAME) {
                    disable_jzon(enus->hub);
                }
                enus->wp_state = WP_PRODUCT;
            } else if (YSTRCMP(j->token, "productName") == 0) {
                enus->state = ENU_WP_PRODUCTNAME;
                if (enus->wp_state != WP_PRODUCT) {
                    disable_jzon(enus->hub);
                }
                enus->wp_state = WP_PROD_ID;
            } else if (YSTRCMP(j->token, "productId") == 0) {
                enus->state = ENU_WP_PRODUCTID;
                if (enus->wp_state != WP_PROD_ID) {
                    disable_jzon(enus->hub);
                }
                enus->wp_state = WP_NET_URL;
            } else if (YSTRCMP(j->token, "networkUrl") == 0) {
                enus->state = ENU_WP_DEVURL;
                if (enus->wp_state != WP_NET_URL) {
                    disable_jzon(enus->hub);
                }
                enus->wp_state = WP_BEACON;
            } else if (YSTRCMP(j->token, "beacon") == 0) {
                enus->state = ENU_WP_BEACON;
                if (enus->wp_state != WP_BEACON) {
                    disable_jzon(enus->hub);
                }
                enus->wp_state = WP_INDEX;
            } else if (YSTRCMP(j->token, "index") == 0) {
                enus->state = ENU_WP_INDEX;
                if (enus->wp_state != WP_INDEX) {
                    disable_jzon(enus->hub);
                }
                enus->wp_state = WP_DONE;
            } else {
                if (enus->wp_state != WP_DONE) {
                    disable_jzon(enus->hub);
                }
                yJsonSkip(j, 1);
            }
        }
        break;
    case ENU_WP_SERIAL:
        NETENUMLOG("set serial to %s\n", j->token);
        enus->serial = yHashPutStr(j->token);
        enus->state = ENU_WP_ENTRY;
        break;
    case ENU_WP_LOGICALNAME:
        NETENUMLOG("set Logical name to %s\n", j->token);
        enus->logicalName = yHashPutStr(j->token);
        enus->state = ENU_WP_ENTRY;
        break;
    case ENU_WP_PRODUCTNAME:
        NETENUMLOG("set Product name to %s\n", j->token);
        enus->productName = yHashPutStr(j->token);
        enus->state = ENU_WP_ENTRY;
        break;
    case ENU_WP_PRODUCTID:
        NETENUMLOG("set productid to %s\n", j->token);
        enus->productId = atoi(j->token);
        enus->state = ENU_WP_ENTRY;
        break;
    case ENU_WP_DEVURL:
        NETENUMLOG("set url to %s\n", j->token);
        if (YSTRCMP(j->token, "/api") == 0) {
            if (enus->hub->serial_hash != enus->serial) {
                enus->hub->serial_hash = enus->serial;
                yHashGetStr(enus->serial, enus->hub->info.serial, YOCTO_SERIAL_LEN);
                if (checkForSameHubAccess(enus->hub, enus->serial, NULL) < 0) {
                    // fatal error do not try to reconnect
                    return YAPI_DOUBLE_ACCES;
                }
            }
        }
        enus->hubref = enus->hub->serial_hash;
        enus->state = ENU_WP_ENTRY;
        break;
    case ENU_WP_BEACON:
        NETENUMLOG("set beacon to %s\n", j->token);
        enus->beacon = atoi(j->token);
        enus->state = ENU_WP_ENTRY;
        break;
    case ENU_WP_INDEX:
        NETENUMLOG("set devYdx to %s\n", j->token);
        enus->devYdx = atoi(j->token);
        enus->state = ENU_WP_ENTRY;
        break;
    case ENU_YP_CONTENT:
        if (j->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_YP_TYPE;
            enus->ypCateg = INVALID_HASH_IDX;
        } else {
            NETENUMLOG("Unknown token %s\n", j->token);
        }
        break;
    case ENU_YP_TYPE:
        if (j->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_SERVICE;
        } else if (j->st == YJSON_PARSE_MEMBNAME) {
            enus->ypCateg = yHashPutStr(j->token);
            NETENUMLOG("Categ = %s\n", j->token);
            enus->state = ENU_YP_TYPE_LIST;
        } else {
            NETENUMLOG("Unknown token %s\n", j->token);
        }
        break;
    case ENU_YP_TYPE_LIST:
        if (j->st == YJSON_PARSE_ARRAY) {
            enus->state = ENU_YP_ARRAY;
        } else if (j->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_SERVICE;
        }
        break;
    case ENU_YP_ARRAY:
        if (j->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_YP_ENTRY;
            enus->serial = INVALID_HASH_IDX;
            enus->logicalName = INVALID_HASH_IDX;
            enus->funcId = INVALID_HASH_IDX;
            enus->funClass = YOCTO_AKA_YFUNCTION;
            enus->funYdx = -1;
            memset(enus->advertisedValue, 0, sizeof(enus->advertisedValue));
            enus->yp_state = YP_BASETYPE;
        } else if (j->st == YJSON_PARSE_ARRAY) {
            enus->state = ENU_YP_TYPE;
        } else {
            NETENUMLOG("what is it %s \n", j->token);
        }
        break;
    case ENU_YP_ENTRY:
        if (j->st == YJSON_PARSE_STRUCT) {
            ypUpdateNet(enus);
            enus->state = ENU_YP_ARRAY;
        } else if (j->st == YJSON_PARSE_MEMBNAME) {
            if (YSTRCMP(j->token, "baseType") == 0) {
                enus->state = ENU_YP_BASETYPE;
                if (enus->yp_state != YP_BASETYPE) {
                    disable_jzon(enus->hub);
                }
                enus->yp_state = YP_HARDWAREID;

            } else if (YSTRCMP(j->token, "hardwareId") == 0) {
                enus->state = ENU_YP_HARDWAREID;
                if (enus->yp_state != YP_HARDWAREID) {
                    disable_jzon(enus->hub);
                }
                enus->yp_state = YP_LOGICALNAME;
            } else if (YSTRCMP(j->token, "logicalName") == 0) {
                enus->state = ENU_YP_LOGICALNAME;
                if (enus->yp_state != YP_LOGICALNAME) {
                    disable_jzon(enus->hub);
                }
                enus->yp_state = YP_ADV_VAL;
            } else if (YSTRCMP(j->token, "advertisedValue") == 0) {
                enus->state = ENU_YP_ADVERTISEDVALUE;
                if (enus->yp_state != YP_ADV_VAL) {
                    disable_jzon(enus->hub);
                }
                enus->yp_state = YP_INDEX;
            } else if (YSTRCMP(j->token, "index") == 0) {
                enus->state = ENU_YP_INDEX;
                if (enus->yp_state != YP_INDEX) {
                    disable_jzon(enus->hub);
                }
                enus->yp_state = YP_DONE;
            } else {
                yJsonSkip(j, 1);
                if (enus->yp_state != YP_DONE) {
                    disable_jzon(enus->hub);
                }
            }
        }
        break;

    case ENU_YP_BASETYPE:
        NETENUMLOG("set baseType value to %s\n", j->token);
        enus->funClass = atoi(j->token);
        enus->state = ENU_YP_ENTRY;
        break;
    case ENU_YP_HARDWAREID:
        point = strchr(j->token, '.');
        if (!point) break; //to be safe discard this field if we do not found '.'
        *point++ = '\0';
        NETENUMLOG("set serial  to %s\n", j->token);
        enus->serial = yHashPutStr(j->token);
        NETENUMLOG("set functionid  to %s\n", point);
        enus->funcId = yHashPutStr(point);
        enus->state = ENU_YP_ENTRY;
        break;
    case ENU_YP_LOGICALNAME:
        NETENUMLOG("set function  to %s\n", j->token);
        enus->logicalName = yHashPutStr(j->token);
        enus->state = ENU_YP_ENTRY;
        break;
    case ENU_YP_ADVERTISEDVALUE:
        NETENUMLOG("set advertised value to %s\n", j->token);
        YSTRNCPY(enus->advertisedValue, YOCTO_PUBVAL_LEN, j->token, YOCTO_PUBVAL_LEN);
        enus->state = ENU_YP_ENTRY;
        break;
    case ENU_YP_INDEX:
        NETENUMLOG("set funYdx value to %s\n", j->token);
        enus->funYdx = atoi(j->token);
        enus->state = ENU_YP_ENTRY;
        break;
    default:
        return YAPI_IO_ERROR;
    }
    return YAPI_SUCCESS;
}

static u32 unpackHTTPRequest(u8 *data, u32 datalen)
{
    // check if we need to decode chunks encoding
    int data_ofs = ymemfind(data, datalen, (u8*)"\r\n\r\n", 4);
    if (data_ofs > 0) {
        const u8 *p = data;
        const u8 *d = p + data_ofs;
        char buffer[128];
        char *pt;
        const char *ept = buffer + 128;
        char c = '\0';
        int decode_chunk = 0;
        while (p < d) {
            pt = buffer;
            while (p < d && pt < ept && (c = *p++) != ':' && c != '\r' && c != '\n') {
                if (c != ' ') {
                    *pt++ = c;
                }
            }
            if (p >= d) {
                break;
            }
            *pt = 0;
            if (c == ':') {
                int parse_val = 0;
                p++;
                if (YSTRICMP(buffer, "Transfer-Encoding") == 0) {
                    parse_val = 1;
                }
                pt = buffer;
                while (p < d && pt < ept && (c = *p++) != '\r' && c != '\n') {
                    if (c != ' ') {
                        *pt++ = c;
                    }
                }
                *pt = 0;
                if (parse_val) {
                    if (YSTRICMP(buffer, "chunked") == 0) {
                        decode_chunk = 1;
                        break;
                    }
                }
            }
        }
        if (decode_chunk) {
            u32 newdatalen;
            u8 *newdata = yMalloc(datalen);
            u8 *w = newdata;
            u32 chunklen;
            data_ofs += 4;
            memcpy(w, data, data_ofs);
            w += data_ofs;
            p = data + data_ofs;
            d = data + datalen;
            do {
                int nbdigit = 0;
                pt = buffer;
                while (p < d && pt < ept && (c = *p++) != '\n') {
                    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
                        *pt++ = c;
                        nbdigit++;
                    }
                }
                *pt = 0;
                chunklen = decodeHex(buffer, nbdigit);
                if (chunklen) {
                    memcpy(w, p, chunklen);
                    w += chunklen;
                    p += chunklen + 2;

                }

            } while (chunklen);
            newdatalen = (u32)(w - newdata);
            YASSERT(newdatalen < datalen, newdatalen);
            memcpy(data, newdata, newdatalen);
            yFree(newdata);
            return (int)newdatalen;
        }
    }
    return datalen;
}

static int parseFullAPI(HubSt *hub, ENU_CONTEXT *enus, u8 *data, u32 datalen, char *errmsg)
{
    yJsonStateMachine j;
    yJsonRetCode jstate = YJSON_NEED_INPUT;
    int res;
    // init yjson parser
    memset(&j, 0, sizeof(j));
    j.st = YJSON_HTTP_START;
    enus->state = ENU_HTTP_START;
    j.src = (char*)data;
    j.end = (char*)data + datalen;
    // parse all we can on this buffer
    jstate = yJsonParse(&j);
    while (jstate == YJSON_PARSE_AVAIL) {
        res = yEnuJson(enus, &j);
        if (YISERR(res)) {
            return YERRMSG(res, "Invalid json data");
        }
        jstate = yJsonParse(&j);
    }
    switch (jstate) {
    case YJSON_NEED_INPUT:
        return YERRMSG(YAPI_IO_ERROR, "Invalid json data (missing data)");
    case YJSON_FAILED:
        return YERRMSG(YAPI_IO_ERROR, "Invalid json data");
    default:
        return YAPI_SUCCESS;
    }
}


#define ADVANCE_REF 1
#define ADVANCE_JZON 2
#define ADVANCE_BOTH (ADVANCE_REF + ADVANCE_JZON)
// return a bitfield which indicate which json state machine need to advaNce
static int yEnuJZon(ENU_CONTEXT *enus, yJsonStateMachine *j, yJsonStateMachine *z, char *errmsg)
{
    char *point;
    int res = ADVANCE_BOTH;
#if 0 //def DEBUG_NET_ENUM
    dbglog("REF:%s: %s %s (%p)\n", ENU_PARSE_STATE_STR[enus->state], yJsonStateStr[j->st],j->token, j->src);
    dbglog("ZON:%s: %s %s (%p)\n", ENU_PARSE_STATE_STR[enus->state], yJsonStateStr[z->st], z->token, z->src);
#endif
    switch (enus->state) {
    case ENU_HTTP_START:
        if (j->st != YJSON_HTTP_READ_CODE || YSTRCMP(j->token, "200")) {
            return YAPI_IO_ERROR;
        }
        if (z->st != YJSON_HTTP_READ_CODE || YSTRCMP(z->token, "200")) {
            return YAPI_IO_ERROR;
        }
        enus->state = ENU_JSON_START;
        break;
    case ENU_JSON_START:
        if (j->st == YJSON_HTTP_READ_MSG)
            break;
        if (j->st == YJSON_PARSE_ARRAY)
            return YAPI_IO_ERROR;
        if (j->st == YJSON_PARSE_STRING)
            return YAPI_IO_ERROR;
        if (j->st != YJSON_PARSE_STRUCT || z->st != YJSON_PARSE_ARRAY)
            return YAPI_IO_ERROR;
        enus->state = ENU_API;
        break;
    case ENU_API:
        if (j->st != YJSON_PARSE_MEMBNAME)
            break;
        if (YSTRCMP(j->token, "module") == 0) {
            enus->state = ENU_MODULE_START;
            return ADVANCE_REF;
        } else if (YSTRCMP(j->token, "network") == 0) {
            enus->state = ENU_NETWORK_START;
            return ADVANCE_REF;
        } else if (YSTRCMP(j->token, "services") == 0) {
            enus->state = ENU_SERVICE;
            return ADVANCE_REF;
        } else {
            enus->state = ENU_SKIP_FUNCTION_START;
            return ADVANCE_REF;
        }
    case ENU_SKIP_FUNCTION_START:
        if (j->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_SKIP_FUNCTION;
        }
        break;
    case ENU_SKIP_FUNCTION:
        if (j->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_API;
        } else if (j->st == YJSON_PARSE_MEMBNAME) {
            yJsonSkip(j, 1);
            // do not skip jzon since it's simple value (string or integer)
        }
        break;
    case ENU_MODULE_START:
        if (j->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_MODULE;
        }
        break;
    case ENU_MODULE:
        if (j->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_API;
        } else if (j->st == YJSON_PARSE_MEMBNAME) {
            if (YSTRCMP(j->token, "firmwareRelease") == 0) {
                enus->state = ENU_MODULE_FIRMWARE;
                return ADVANCE_REF;
            } else {
                yJsonSkip(j, 1);
                // do not skip jzon since it's simple value (string or integer)
            }
        }
        break;
    case ENU_MODULE_FIRMWARE:
        NETENUMLOG("firmwareRelease is set to [%s]\n", z->token);
        YSTRCPY(enus->hub->fw_release, YOCTO_FIRMWARE_LEN, z->token);
        enus->state = ENU_MODULE;
        break;
    case ENU_NETWORK_START:
        if (j->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_NETWORK;
        }
        break;
    case ENU_NETWORK:
        if (j->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_API;
        } else if (j->st == YJSON_PARSE_MEMBNAME) {
            if (YSTRCMP(j->token, "adminPassword") == 0) {
                enus->state = ENU_NET_ADMINPWD;
                return ADVANCE_REF;
            } else {
                yJsonSkip(j, 1);
                // do not skip jzon since it's simple value (string or integer)
            }
        }
        break;
    case ENU_NET_ADMINPWD:
        NETENUMLOG("adminpwd is set to [%s]\n", z->token);
        enus->hub->writeProtected = (z->token[0] != 0);
        enus->state = ENU_NETWORK;
        break;
    case ENU_SERVICE:
        if (j->st != YJSON_PARSE_MEMBNAME)
            break;
        if (YSTRCMP(j->token, "whitePages") == 0) {
            enus->state = ENU_WP_ARRAY;
            yJsonSkip(j, 1);
        } else if (YSTRCMP(j->token, "yellowPages") == 0) {
            enus->state = ENU_YP_TYPE;
            enus->ypCateg = INVALID_HASH_IDX;
            yJsonSkip(j, 1);
        } else {
            yJsonSkip(j, 1);
        }
        break;
    case ENU_WP_ARRAY:
        if (z->st == YJSON_PARSE_ARRAY && YSTRCMP(z->token, "[") == 0) {
            enus->state = ENU_WP_ENTRY;
            enus->serial = INVALID_HASH_IDX;
            enus->logicalName = INVALID_HASH_IDX;
            enus->productName = INVALID_HASH_IDX;
            enus->productId = 0;
            enus->hubref = INVALID_HASH_IDX;
            enus->beacon = 0;
            enus->devYdx = -1;
            enus->wp_state = WP_SERIAL;
        } else if (z->st == YJSON_PARSE_ARRAY && YSTRCMP(z->token, "]") == 0) {
            enus->state = ENU_SERVICE;
        } else {
            return YERRMSG(YAPI_IO_ERROR, "Unknown token in white pages");
        }
        return ADVANCE_JZON;
    case ENU_WP_ENTRY:
        if (z->st == YJSON_PARSE_ARRAY && YSTRCMP(z->token, "]") == 0) {
            parseNetWpEntry(enus);
            enus->state = ENU_WP_ARRAY;
        } else if (z->st != YJSON_PARSE_ARRAY) {
            switch (enus->wp_state) {
            case WP_SERIAL:
                NETENUMLOG("set serial to %s\n", z->token);
                enus->serial = yHashPutStr(z->token);
                enus->wp_state = WP_LOGICALNAME;
                break;
            case WP_LOGICALNAME:
                NETENUMLOG("set Logical name to %s\n", z->token);
                enus->logicalName = yHashPutStr(z->token);
                enus->wp_state = WP_PRODUCT;
                break;
            case WP_PRODUCT:
                NETENUMLOG("set Product name to %s\n", z->token);
                enus->productName = yHashPutStr(z->token);
                enus->wp_state = WP_PROD_ID;
                break;
            case WP_PROD_ID:
                NETENUMLOG("set productid to %s\n", z->token);
                enus->productId = atoi(z->token);
                enus->wp_state = WP_NET_URL;
                break;
            case WP_NET_URL:
                NETENUMLOG("set url to %s\n", z->token);
                if (YSTRCMP(z->token, "/api") == 0 && enus->hub->serial_hash != enus->serial) {
                    enus->hub->serial_hash = enus->serial;
                    yHashGetStr(enus->serial, enus->hub->info.serial, YOCTO_SERIAL_LEN);
                    if (checkForSameHubAccess(enus->hub, enus->serial, NULL) < 0) {
                        // fatal error do not try to reconnect
                        return YAPI_DOUBLE_ACCES;
                    }
                }
                enus->hubref = enus->hub->serial_hash;
                enus->wp_state = WP_BEACON;
                break;
            case WP_BEACON:
                NETENUMLOG("set beacon to %s\n", z->token);
                enus->beacon = atoi(z->token);
                enus->wp_state = WP_INDEX;
                break;
            case WP_INDEX:
                NETENUMLOG("set devYdx to %s\n", z->token);
                enus->devYdx = atoi(z->token);
                enus->wp_state = WP_DONE;
                break;
            default:
                break;
            }
        }
        return ADVANCE_JZON;

    case ENU_YP_TYPE:
        if (z->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_SERVICE;
        } else if (z->st == YJSON_PARSE_MEMBNAME) {
            enus->ypCateg = yHashPutStr(z->token);
            NETENUMLOG("Categ = %s\n", z->token);
            enus->state = ENU_YP_TYPE_LIST;
        } else {
            return YERRMSG(YAPI_IO_ERROR, "Unknown token in yellow pages");
        }
        return ADVANCE_JZON;
    case ENU_YP_TYPE_LIST:
        if (z->st == YJSON_PARSE_ARRAY) {
            enus->state = ENU_YP_ARRAY;
        } else if (z->st == YJSON_PARSE_STRUCT) {
            enus->state = ENU_SERVICE;
        }
        return ADVANCE_JZON;
    case ENU_YP_ARRAY:
        if (z->st == YJSON_PARSE_ARRAY && YSTRCMP(z->token, "[") == 0) {
            enus->state = ENU_YP_ENTRY;
            enus->serial = INVALID_HASH_IDX;
            enus->logicalName = INVALID_HASH_IDX;
            enus->funcId = INVALID_HASH_IDX;
            enus->funClass = YOCTO_AKA_YFUNCTION;
            enus->funYdx = -1;
            memset(enus->advertisedValue, 0, sizeof(enus->advertisedValue));
            enus->yp_state = YP_BASETYPE;

        } else if (z->st == YJSON_PARSE_ARRAY && YSTRCMP(z->token, "]") == 0) {
            enus->state = ENU_YP_TYPE;
        } else {
            return YERRMSG(YAPI_IO_ERROR, "Unknown token in yellow pages");
        }
        return ADVANCE_JZON;
    case ENU_YP_ENTRY:
        if (z->st == YJSON_PARSE_ARRAY && YSTRCMP(z->token, "]") == 0) {
            ypUpdateNet(enus);
            enus->state = ENU_YP_ARRAY;
        } else if (z->st != YJSON_PARSE_ARRAY) {
            switch (enus->yp_state) {
            case YP_BASETYPE:
                NETENUMLOG("set baseType value to %s\n", z->token);
                enus->funClass = atoi(z->token);
                enus->yp_state = YP_HARDWAREID;
                break;
            case YP_HARDWAREID:
                point = strchr(z->token, '.');
                if (!point) break; //to be safe discard this field if we do not found '.'
                *point++ = '\0';
                NETENUMLOG("set serial  to %s\n", z->token);
                enus->serial = yHashPutStr(z->token);
                NETENUMLOG("set functionid  to %s\n", point);
                enus->funcId = yHashPutStr(point);
                enus->yp_state = YP_LOGICALNAME;
                break;
            case YP_LOGICALNAME:
                NETENUMLOG("set function  to %s\n", z->token);
                enus->logicalName = yHashPutStr(z->token);
                enus->yp_state = YP_ADV_VAL;
                break;
            case YP_ADV_VAL:
                NETENUMLOG("set advertised value to %s\n", z->token);
                YSTRNCPY(enus->advertisedValue, YOCTO_PUBVAL_LEN, z->token, YOCTO_PUBVAL_LEN);
                enus->yp_state = YP_INDEX;
                break;
            case YP_INDEX:
                NETENUMLOG("set funYdx value to %s\n", z->token);
                enus->funYdx = atoi(z->token);
                enus->yp_state = YP_DONE;
                break;
            default:
                break;
            }
        }
        return ADVANCE_JZON;
    default:
        return YERRMSG(YAPI_IO_ERROR, "Unknown state");
    }
    return res;
}


static int parseJZONAPI(HubSt *hub, ENU_CONTEXT *enus, u8 *data, u32 datalen, char *errmsg)
{
    yJsonStateMachine ref;
    yJsonStateMachine z;
    yJsonRetCode jstate;
    yJsonRetCode zstate;
    // init yjson parser
    memset(&ref, 0, sizeof(ref));
    ref.st = YJSON_HTTP_START;
    enus->state = ENU_HTTP_START;
    ref.src = (char*)hub->ref_api;
    ref.end = (char*)hub->ref_api + hub->ref_api_size;

    memset(&z, 0, sizeof(z));
    z.st = YJSON_HTTP_START;
    enus->state = ENU_HTTP_START;
    z.src = (char*)data;
    z.end = (char*)data + datalen;


    // parse all we can on this buffer
    jstate = yJsonParse(&ref);
    zstate = yJsonParse(&z);
    while (jstate == YJSON_PARSE_AVAIL && jstate == zstate) {
        int subres = yEnuJZon(enus, &ref, &z, errmsg);
        if (YISERR(subres)) {
            return YERRMSG(YAPI_IO_ERROR, "Invalid json data");
        }
        if (subres & ADVANCE_REF) {
            jstate = yJsonParse(&ref);
        }
        if (subres & ADVANCE_JZON) {
            zstate = yJsonParse(&z);
        }
    }
    switch (zstate) {
    case YJSON_NEED_INPUT:
        return YERRMSG(YAPI_IO_ERROR, "Invalid jzon data (missing data)");
    case YJSON_FAILED:
        return YERRMSG(YAPI_IO_ERROR, "Invalid jzon data");
    default:
        return YAPI_SUCCESS;
    }
}


// connect to a network hub and do an enumeration
// this function will do TCP IO and will do a timeout if
// the hub is off line.
// USE NO NOT USE THIS FUNCTION BUT yNetHubEnum INSTEAD
static int yNetHubEnumEx(HubSt *hub, ENU_CONTEXT *enus, char *errmsg)
{
    u8 *buffer;
    u32 buffer_size, avail;
    u32 unpackedBufferSize;
    u32 response_size;
    u8 *p;
    int res;
    const char *base_request = "GET /api.json \r\n\r\n"; // no HTTP/1.1 suffix -> light headers
    const char *request;
    u64 enumTimeout;
    RequestSt *req;
    int use_jzon;
    int enum_done;

retry:

    if (hub->typical_apireq_size < 2048) {
        hub->typical_apireq_size = 2048;
    }
    buffer_size = hub->typical_apireq_size;
    buffer = yMalloc(buffer_size);
    p = buffer;
    response_size = 0;
    avail = buffer_size;

    use_jzon = hub->fw_release[0] && (hub->flags & INCOMPATIBLE_JZON_ENCODING) == 0;

    if (use_jzon) {
        YSPRINTF((char*)buffer, buffer_size, "GET /api.json?fw=%s \r\n\r\n", hub->fw_release);
        request = (char*)buffer;
    } else {
        request = base_request;
    }
#ifdef DEBUG_YAPI_REQ
    int req_count = YREQ_LOG_START("yNetHubEnumEx", hub->host, request, YSTRLEN(request));
    u64 start_tm = yapiGetTickCount();
#endif

    req = yReqAlloc(hub);
    if (YISERR((res = yReqOpen(req, 2 * hub->netTimeout, 0, request, YSTRLEN(request), hub->netTimeout, NULL, NULL, NULL, NULL, errmsg)))) {
        yReqFree(req);
        yFree(buffer);
        return res;
    }
    enumTimeout = yapiGetTickCount() + 10000;

    enum_done = 0;
    while (yapiGetTickCount() < enumTimeout) {
        res = yReqSelect(req, 1000, errmsg);
        if (YISERR(res)) {
            break;
        }
        res = yReqRead(req, p, avail - 4);
        while (res > 0) {
            response_size += res;
            avail -= res;
            if (avail < 256) {
                u8 *nbuf;
                buffer_size *= 2;
                hub->typical_apireq_size = buffer_size;
                nbuf = yMalloc(buffer_size);
                memcpy(nbuf, buffer, response_size);
                yFree(buffer);
                buffer = nbuf;
                p = buffer + response_size;
                avail = buffer_size - response_size;

            } else {
                p += res;
            }
            res = yReqRead(req, p, avail - 4);
        }
        if (res <= 0) {
            res = yReqIsEof(req, errmsg);
            if (res > 0) {
                enum_done = 1;
                break;
            }
            if (YISERR(res)) {
                // any specific error during select
                yReqClose(req);
                yReqFree(req);
                yFree(buffer);
                return res;
            }
        }
    }
    yReqClose(req);
    yReqFree(req);
    if (!enum_done) {
        return YERRMSG(YAPI_TIMEOUT, "Hub enumeration timeout");
    }
    unpackedBufferSize = unpackHTTPRequest(buffer, response_size);
    if (use_jzon) {
        res = parseJZONAPI(hub, enus, buffer, unpackedBufferSize, errmsg);
    } else {
        res = parseFullAPI(hub, enus, buffer, unpackedBufferSize, errmsg);
    }
    if (YISERR(res)) {
        hub->fw_release[0] = 0;
        if (hub->ref_api) {
            yFree(hub->ref_api);
        }
        hub->ref_api = NULL;
        yFree(buffer);
        if (use_jzon) {
            disable_jzon(hub);
            goto retry;
        }
        return res;
    }
    if (!use_jzon) {
        hub->ref_api = buffer;
        hub->ref_api_size = unpackedBufferSize;
    } else {
        yFree(buffer);
    }
    return YAPI_SUCCESS;
}


// helper for yNetHubEnumEx that will trigger TCP connection (and potentially
// timeout) only when it is really needed.
static int yNetHubEnum(HubSt *hub, int forceupdate, char *errmsg)
{
    ENU_CONTEXT enus;
    int i, res;
    yStrRef knownDevices[128];

    //check if the expiration has expired;
    if (!forceupdate && isNotificationConnectionON(hub) && hub->devListExpires > yapiGetTickCount()) {
        return YAPI_SUCCESS;
    }

    // et base url (then entry point)
    memset(&enus, 0, sizeof(enus));
    enus.hub = hub;
    enus.knownDevices = knownDevices;
    enus.nbKnownDevices = ywpGetAllDevUsingNetHub(hub, enus.knownDevices, 128);
    if (enus.nbKnownDevices > 128) {
        return YERRMSG(YAPI_IO_ERROR, "too many device on this Net hub");
    }


    if (hub->mandatory) {
        // if the hub is mandatory we will raise an error
        // and not unregister the connected devices
        if (hub->send_ping && !isNotificationConnectionON(hub)) {
            // the hub send ping notification -> we can rely on helperthread status
            if (errmsg) {
                YSPRINTF(errmsg, YOCTO_ERRMSG_LEN, "hub %s is not reachable", hub->url.host);
            }
            res = YAPI_IO_ERROR;
        } else {
            // the hub does not send ping notification -> we will to a request and potentially
            // get a tcp timeout if the hub is not reachable
            res = yNetHubEnumEx(hub, &enus, errmsg);
        }
    } else {
        // if the hub is optional we will not trigger an error but
        // instead unregister all know device connected on this hub
        if (isNotificationConnectionON(hub)) {
            // the hub send ping notification -> we can rely on helperthread status
            res = yNetHubEnumEx(hub, &enus, errmsg);
            if (YISERR(res)) {
                dbglog("error with hub %s : %s\n", hub->url.host, errmsg);
            }
        }
        res = YAPI_SUCCESS;
    }

    for (i = 0; i < enus.nbKnownDevices; i++) {
        if (enus.knownDevices[i] != INVALID_HASH_IDX) {
            unregisterNetDevice(hub, knownDevices[i]);
        }
    }
    if (isNotificationConnectionON(hub)) {
        hub->devListExpires = yapiGetTickCount() + YctxDeviceListValidityMs;
    } else {
        hub->devListExpires = yapiGetTickCount() + 500;
    }
    return res;
}


static int parseInfoJSon(HubSt *hub, u8 *info_data, int len, char *errmsg)
{
    yJsonStateMachine j;
    int nb_proto = 0;

    // init yjson parser
    memset(&j, 0, sizeof(j));
    j.st = YJSON_HTTP_START;
    j.src = (char*)info_data;
    j.end = (char*)info_data + len;
    // parse all we can on this buffer
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL && j.st != YJSON_HTTP_READ_CODE) {
        return YERRMSG(YAPI_IO_ERROR, "Invalid HTML response");
    }
    if (YSTRCMP("200", j.token)) {
        return YERRMSG(YAPI_NOT_SUPPORTED, "Hub does not implement info.json");
    }
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_HTTP_READ_MSG) {
        return YERRMSG(YAPI_IO_ERROR, "Invalid HTML response");
    }
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Not a JSON struct");
    }
    while (yJsonParse(&j) == YJSON_PARSE_AVAIL) {
        if (j.st == YJSON_PARSE_MEMBNAME) {
            if (YSTRCMP("serialNumber", j.token) == 0) {
                if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRING) {
                    return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid serialNumber in info.json file");
                }
                YSTRCPY(hub->info.serial, YOCTO_SERIAL_LEN, j.token);
            } else if (YSTRCMP("protocol", j.token) == 0) {
                if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRING) {
                    return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid protocol in info.json file");
                }
                if (YSTRCMP("HTTP/1.1", j.token) == 0) {
                    hub->info.use_pure_http = 1;
                }
            } else if (YSTRCMP("port", j.token) == 0) {
#ifdef DEBUG_JSON_PARSE
                dbglog("found %s %s(%d)\n", j.token, yJsonStateStr[j.st], j.st);
#endif
                if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_ARRAY) {
                    return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid info.json file (port should be a array");
                }
                while (yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st != YJSON_PARSE_ARRAY) {
                    char *p;
                    switch (j.st) {
                    case YJSON_PARSE_STRING:
                        if (nb_proto >= NB_PROTO_IN_INFO_JSON) {
                            break;
                        }
                        p = j.token;
                        while (*p && *p != ':') {
                            p++;
                        }
                        if (*p == ':') {
                            *p++ = 0;
                            hub->info.ports[nb_proto].port = atoi(p);
                            if (YSTRCMP("wss", j.token) == 0) {
                                hub->info.ports[nb_proto].proto = PROTO_SECURE_WEBSOCKET;
                            } else if (YSTRCMP("https", j.token) == 0) {
                                hub->info.ports[nb_proto].proto = PROTO_SECURE_HTTP;
                            } else if (YSTRCMP("ws", j.token) == 0) {
                                hub->info.ports[nb_proto].proto = PROTO_WEBSOCKET;
                                hub->info.has_unsecure_open_port = 1;
                            } else if (YSTRCMP("http", j.token) == 0) {
                                hub->info.ports[nb_proto].proto = PROTO_HTTP;
                                hub->info.has_unsecure_open_port = 1;
                            } else {
                                dbglog("Unknown proto found in info.json (%s)\n", j.st);
                                hub->info.ports[nb_proto].proto = PROTO_UNKNOWN;
                            }
                        } else {
                            return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid protocol definition info.json.");
                        }
                        nb_proto++;
                        break;
                    default:
                        break;
                    }
                }
            } else {
#ifdef DEBUG_JSON_PARSE
                dbglog("skip %s %s(%d)\n", j.token, yJsonStateStr[j.st], j.st);
#endif
                yJsonSkip(&j, 1);
            }
        }

    }
    return YAPI_SUCCESS;
}

static const char* protoStr(yHubProto proto)
{
    switch (proto) {
    case PROTO_LEGACY:
        return "Legacy";
    case PROTO_AUTO:
        return "Auto";
    case PROTO_SECURE:
        return "Secure";
    case PROTO_HTTP:
        return "HTTP";
    case PROTO_WEBSOCKET:
        return "WebSocket";
    case PROTO_SECURE_HTTP:
        return "Secure HTTP";
    case PROTO_SECURE_WEBSOCKET:
        return "Secure WebSocket";
    case PROTO_UNKNOWN:
    default:
        return "Unknown";
    }
}


static int yhubUseBestProto(HubSt *hub, char *errmsg)
{
    yHubProto cur_proto = hub->url.proto;
    if (hub->info.serial[0] == 0) {
        //no info.json use legacy change
        switch (cur_proto) {
        case PROTO_LEGACY:
            hub->url.proto = PROTO_WEBSOCKET;
            break;
        case PROTO_SECURE:
            hub->url.proto = PROTO_SECURE_WEBSOCKET;
            break;
        case PROTO_WEBSOCKET:
        case PROTO_SECURE_WEBSOCKET:
        case PROTO_HTTP:
        case PROTO_SECURE_HTTP:
            break;
        default:
            return YERRMSG(YAPI_NOT_SUPPORTED, "Protocol not supported. Upgrade VirtualHub or Hub Firmware.");
        }
    } else {
        if (hub->info.use_pure_http) {
            // For VirtualHub-4web we use the first entry available regardless of the protocol and the port set
            // by the user. In this sernario info.json has the most accurate value. Note: redirection from http to
            // https has already be done by the http redirect mechanism during the download of info.json
            // Note 2 : Websocket are not supported by VirtualHub-4web
            if (cur_proto == PROTO_WEBSOCKET || cur_proto == PROTO_SECURE_WEBSOCKET) {
                return YERRMSG(YAPI_NOT_SUPPORTED, "Websocket protocol is not supported by VirtualHub-4web.");
            }
            int done = 0;
            // if use as specifiec the proto in the URL do not try to guess the best proto and best port.
            while (!done && hub->info.next_port < NB_PROTO_IN_INFO_JSON) {
                yHubProto proto = hub->info.ports[hub->info.next_port].proto;
                int port = hub->info.ports[hub->info.next_port].port;
                hub->info.next_port++;

                switch (proto) {
                case PROTO_HTTP:
                    dbglog("Hub %s will use %s proto on port %d\n", hub->url.host, protoStr(proto), port);
                    hub->url.proto = proto;
                    hub->url.portno = port;
                    done = 1;
                    break;

#ifndef NO_YSSL
                case PROTO_SECURE_HTTP:
                    dbglog("Hub %s will use %s proto on port %d\n", hub->url.host, protoStr(proto), port);
                    hub->url.proto = proto;
                    hub->url.portno = port;
                    done = 1;
                    break;
#endif
                default:
#if 1
                    dbglog("Skip proto %s port %d\n", protoStr(proto), port);
#endif
                    break;
                }
            }
        } else {
            int done = 0;
            if (cur_proto != PROTO_HTTP && cur_proto != PROTO_WEBSOCKET) {
                // if use as specifiec the proto in the URL do not try to guess the best proto and best port.
                while (!done && hub->info.next_port < NB_PROTO_IN_INFO_JSON) {
                    yHubProto proto = hub->info.ports[hub->info.next_port].proto;
                    int port = hub->info.ports[hub->info.next_port].port;
                    hub->info.next_port++;

                    switch (proto) {
                    case PROTO_WEBSOCKET:
                    case PROTO_HTTP:
                        dbglog("Hub %s will use %s proto on port %d\n", hub->url.host, protoStr(proto), port);
                        hub->url.proto = proto;
                        hub->url.portno = port;
                        done = 1;
                        break;

#ifndef NO_YSSL
                    case PROTO_SECURE_WEBSOCKET:
                    case PROTO_SECURE_HTTP:
#endif
                        dbglog("Hub %s will use %s proto on port %d\n", hub->url.host, protoStr(proto), port);
                        hub->url.proto = proto;
                        hub->url.portno = port;
                        done = 1;
                        break;
                    default:
#if 1
                        dbglog("Skip proto %s port %d\n", protoStr(proto), port);
#endif
                        break;
                    }
                }
            }
        }
        if (hub->info.next_port >= NB_PROTO_IN_INFO_JSON) {
            return YERRMSG(YAPI_NOT_SUPPORTED, "No compatible protocol in info.json. Upgrade VirtualHub or Hub Firmware");
        }

    }
    return YAPI_SUCCESS;
}


// return YAPI_NOT_SUPPORTED for very old hub that does not support info.json (HTTP 404)
int LoadInfoJson(HubSt *hub, u32 mstimeout, char *errmsg)
{
    // look for info.json file
    char info_url[128];
    u8 *info_data;
    int res, retry;
    int use_ssl_socket = 0;
    u16 port = hub->url.portno;
    YSPRINTF(info_url, 512, "%s/info.json", hub->url.subdomain);
    if (hub->url.proto == PROTO_SECURE || hub->url.proto == PROTO_SECURE_HTTP || hub->url.proto == PROTO_SECURE_WEBSOCKET) {
        use_ssl_socket = 1;
    }
    do {
        retry = 0;
        //dbglog("Load info.json from %s:%d (ssl=%d) url=%s\n", hub->url.host, port, use_ssl_socket,info_url);
        res = yTcpDownload(hub->url.host, port, use_ssl_socket, info_url, &info_data, mstimeout, errmsg);
        if (res >= 0) {
            res = parseInfoJSon(hub, info_data, res, errmsg);
            if (res < 0) {
                dbglog("Warning: unable to parse info.json (%s)\n", errmsg);
                memset(&hub->info, 0, sizeof(hub->info));
                if (hub->url.proto == PROTO_AUTO) {
                    hub->url.proto = PROTO_LEGACY;
                }
                yFree(info_data);
                return res;
            }
#ifdef TRACE_NET_HUB
            dbglog("HUB: dump of info.json of hub %s\n", hub->info.serial);
            for (int i = 0; i < 4; i++) {
                if (hub->info.ports[i].port==0) {
                    break;
                }
                dbglog("HUB:  - %s:%d\n", protoStr(hub->info.ports[i].proto), hub->info.ports[i].port);
            }
            typedef struct _HubInfoSt {
                char serial[YOCTO_SERIAL_LEN];
                HubInfoPortSt ports[NB_PROTO_IN_INFO_JSON];
                int next_port;
                int use_pure_http;
            } HubInfoSt;
#endif
            yFree(info_data);
        } else if (res == YAPI_SSL_UNK_CERT) {
            return res;
        } else {
            if (hub->url.proto == PROTO_SECURE && port == YOCTO_DEFAULT_HTTPS_PORT) {
                port = 443;
                retry = 1;
            } else if (hub->url.proto == PROTO_AUTO && port == YOCTO_DEFAULT_PORT) {
                port = 80;
                retry = 1;
            }
        }
    } while (retry);
    return res;
}

// initialize NetHubSt structure. no IO in this function
static HubSt* yapiAllocHub(const char *url, u32 mstimeout, int *error_code, char *errmsg)
{
    HubSt *hub;
    int res, i;

    hub = yMalloc(sizeof(HubSt));
    memset(hub, 0, sizeof(HubSt));
    memset(hub->devYdxMap, 255, sizeof(hub->devYdxMap));
    hub->serial_hash = INVALID_HASH_IDX;
    hub->netTimeout = YctxNetworkTimeout;
    hub->creation_time = yapiGetTickCount();
    for (i = 0; i < ALLOC_YDX_PER_HUB; i++) {
        hub->devices[i] = INVALID_HASH_IDX;
    }

    res = yParseHubURL(&hub->url, url, errmsg);
    if (res < 0) {
        if (error_code) {
            *error_code = res;
        }
        return NULL;
    }
#ifdef TRACE_NET_HUB
    {
        char buffer[YOCTO_MAX_URL_LEN];
        sprintfURL(buffer, YOCTO_MAX_URL_LEN, &hub->url, 0);
        dbglog("HUB: phase 1 url = %s \n", buffer);
    }
#endif
    if (hub->url.proto != PROTO_SECURE_WEBSOCKET && hub->url.proto != PROTO_WEBSOCKET) {
        int res = LoadInfoJson(hub, mstimeout, errmsg);
        if (res >= 0) {
            yStrRef serial = yHashPutStr(hub->info.serial);
            if ((res = checkForSameHubAccess(hub, serial, errmsg)) < 0) {
                if (error_code) {
                    *error_code = res;
                }
                yFreeParsedURL(&hub->url);
                yFree(hub);
                return NULL;
            }
        } else if (res == YAPI_SSL_UNK_CERT) {
            if (error_code) {
                *error_code = res;
            }
            yFreeParsedURL(&hub->url);
            yFree(hub);
            return NULL;
        }
    }
    res = yhubUseBestProto(hub, errmsg);
    if (res != YAPI_SUCCESS) {
        if (error_code) {
            *error_code = res;
        }
        yFreeParsedURL(&hub->url);
        yFree(hub);
        return NULL;
    }

    yInitWakeUpSocket(&hub->wuce);
    yFifoInit(&(hub->not_fifo), hub->not_buffer, sizeof(hub->not_buffer));
    yInitializeCriticalSection(&hub->access);

    if (hub->url.proto == PROTO_HTTP || hub->url.proto == PROTO_SECURE_HTTP) {
        hub->http.lastTraffic = yapiGetTickCount();
    } else {
        int tcpchan;
        memset(&hub->ws, 0, sizeof(WSNetHub));
        hub->ws.s_next_async_id = 48;
        hub->ws.fifo_buffer = yMalloc(2048);
        yFifoInit(&hub->ws.mainfifo, hub->ws.fifo_buffer, 2048);
        for (tcpchan = 0; tcpchan < MAX_ASYNC_TCPCHAN; tcpchan++) {
            yInitializeCriticalSection(&hub->ws.chan[tcpchan].access);
        }
        hub->ws.lastTraffic = yapiGetTickCount();
    }
#ifdef TRACE_NET_HUB
    {
        char buffer[YOCTO_MAX_URL_LEN];
        sprintfURL(buffer, YOCTO_MAX_URL_LEN, &hub->url, 0);
        dbglog("HUB: phase 2 url = %s \n", buffer);
    }
    dbglog("HUB: %s allocated (port=%d / %p) \n", hub->url.org_url, hub->url.portno, hub);
#endif
    return hub;

}

static void yapiFreeHub(HubSt *hub)
{
#ifdef TRACE_NET_HUB
    dbglog("HUB: %s Deleted \n", hub->url.org_url);
#endif
    yFreeWakeUpSocket(&hub->wuce);
    if (hub->url.proto == PROTO_HTTP || hub->url.proto == PROTO_SECURE_HTTP) {
        if (hub->http.s_realm)
            yFree(hub->http.s_realm);
        if (hub->http.s_nonce)
            yFree(hub->http.s_nonce);
        if (hub->http.s_opaque)
            yFree(hub->http.s_opaque);
        if (hub->http.notReq) {
            yReqClose(hub->http.notReq);
            yReqFree(hub->http.notReq);
        }
    } else {
        int tcpchan;
        for (tcpchan = 0; tcpchan < MAX_ASYNC_TCPCHAN; tcpchan++) {
            yDeleteCriticalSection(&hub->ws.chan[tcpchan].access);
        }
        ws_cleanup(hub);
        yFree(hub->ws.fifo_buffer);
    }
    yDeleteCriticalSection(&hub->access);
    yFifoCleanup(&hub->not_fifo);
    if (hub->ref_api)
        yFree(hub->ref_api);
    memset(hub, 0, sizeof(HubSt));
    memset(hub->devYdxMap, 255, sizeof(hub->devYdxMap));
    yFreeParsedURL(&hub->url);
    yFree(hub);
}


static void unregisterNetHub(HubSt *refhub)
{
    int i, j;
    u64 timeref;
    int nbKnownDevices;
    yStrRef knownDevices[128];
    char errmsg[YOCTO_ERRMSG_LEN];


#ifdef TRACE_NET_HUB
    dbglog("HUB: unregister %s\n", refhub->url.org_url);
#endif
    timeref = yapiGetTickCount();

    while ((yapiGetTickCount() - timeref < refhub->netTimeout) && yReqHasPending(refhub)) {
        yapiHandleEvents_internal(errmsg);
        yApproximateSleep(50);
    }
    refhub->state = NET_HUB_TOCLOSE;
    yThreadRequestEnd(&refhub->net_thread);
    yDringWakeUpSocket(&refhub->wuce, 0, errmsg);
    // wait for the helper thread to stop monitoring these devices
    //timeref = yapiGetTickCount();
    while (yThreadIsRunning(&refhub->net_thread) && (yapiGetTickCount() - timeref < refhub->netTimeout)) {
        yApproximateSleep(10);
    }
    yThreadKill(&refhub->net_thread);
    nbKnownDevices = ywpGetAllDevUsingNetHub(refhub, knownDevices, 128);
    for (j = 0; j < nbKnownDevices; j++) {
        if (knownDevices[j] != INVALID_HASH_IDX) {
            unregisterNetDevice(refhub, knownDevices[j]);
        }
    }

    for (i = 0; i < NBMAX_NET_HUB; i++) {
        HubSt *hub = yContext->nethub[i];
        if (hub && yContext->nethub[i] == refhub) {
            yContext->nethub[i] = NULL;
            break;
        }
    }
    yFreeParsedURL(&refhub->url);
    yapiFreeHub(refhub);
}


static void ssdpEntryUpdate(const char *serial, const char *urlToRegister, const char *urlToUnregister)
{
    if (!yContext)
        // API not yet initialized -> drop everything
        return;
    if (urlToRegister) {
        // still valid entry
        if (yContext->hubDiscoveryCallback) {
            yEnterCriticalSection(&yContext->deviceCallbackCS);
            yContext->hubDiscoveryCallback(serial, urlToRegister);
            yLeaveCriticalSection(&yContext->deviceCallbackCS);
        }
    }

    if (yContext->detecttype & Y_DETECT_NET) {
        if (urlToRegister) {
            if (urlToUnregister) {
                yapiUnregisterHub_internal(urlToUnregister);
            }
            yapiPreregisterHub_internal(urlToRegister, NULL);
        }
    }
}


static void initializeAllCS(yContextSt *ctx)
{
    //initialize enumeration CS
    yInitializeCriticalSection(&ctx->updateDev_cs);
    yInitializeCriticalSection(&ctx->handleEv_cs);
    yInitializeCriticalSection(&ctx->enum_cs);
    yInitializeCriticalSection(&ctx->io_cs);
    yInitializeCriticalSection(&ctx->deviceCallbackCS);
    yInitializeCriticalSection(&ctx->functionCallbackCS);
    yInitializeCriticalSection(&ctx->generic_cs);
#ifdef DEBUG_YAPI_REQ
    yInitializeCriticalSection(&YREQ_CS);
#endif
}

static void deleteAllCS(yContextSt *ctx)
{
    //initialize enumeration CS
    yDeleteCriticalSection(&ctx->updateDev_cs);
    yDeleteCriticalSection(&ctx->handleEv_cs);
    yDeleteCriticalSection(&ctx->enum_cs);
    yDeleteCriticalSection(&ctx->io_cs);
    yDeleteCriticalSection(&ctx->deviceCallbackCS);
    yDeleteCriticalSection(&ctx->functionCallbackCS);
    yDeleteCriticalSection(&ctx->generic_cs);
}


/*****************************************************************************
  API FUNCTIONS
 ****************************************************************************/
#pragma pack(push,1)
typedef union {
    u32 raw;

    struct {
        u8 a;
        u8 b;
        u8 c;
        u8 d;
    } bytes;
} test_compile;
#pragma pack(pop)

u64 YctxDeviceListValidityMs = DEFAULT_NET_DEVLIST_VALIDITY_MS;
u32 YctxNetworkTimeout = YIO_DEFAULT_TCP_TIMEOUT;

YRETCODE YAPI_FUNCTION_EXPORT yapiSetNetworkSecurityOptions(u32 options, char *errmsg)
{
    if (!yContext) {
        return YERR(YAPI_NOT_INITIALIZED);
    }
    yContext->sslCertOptions = options;
    return YAPI_SUCCESS;
}

static YRETCODE yapiSetSSLCertificateSrv_internal(const char *certificate, const char *privatekey, char *errmsg)
{
    if (!yContext) {
        return YERR(YAPI_NOT_INITIALIZED);
    }

#ifndef NO_YSSL
    int res = yTcpSetSrvCertificateSSL(certificate, privatekey, errmsg);
    if (YISERR(res)) {
        return (YRETCODE)res;
    }
    return YAPI_SUCCESS;
#else
    return YERRMSG(YAPI_NOT_SUPPORTED, "SSL support is not activated.");
#endif
}


static YRETCODE yapiInitAPI_internal(int detect_type, const char *certificate, const char *privatekey, char *errmsg)
{
    test_compile test;
    int i;
    yContextSt *ctx;
    int res;
#ifdef PERF_API_FUNCTIONS
    memset(&yApiPerf,0,sizeof(yApiPerf));
#endif

#ifdef _WIN64
#pragma message("yapi is compiled in 64 bits")
#endif

    if (yContext != NULL)
        return YERRMSG(YAPI_DEVICE_BUSY, "Api already started");
#ifdef __BORLANDC__
#pragma warn - 8066
#pragma warn - 8008
#endif
    if (sizeof(u8) != 1) return YERRMSG(YAPI_INVALID_ARGUMENT, "invalid definition of u8");
    if (sizeof(s8) != 1) return YERRMSG(YAPI_INVALID_ARGUMENT, "invalid definition of s8");
    if (sizeof(u16) != 2) return YERRMSG(YAPI_INVALID_ARGUMENT, "invalid definition of u16");
    if (sizeof(u32) != 4) return YERRMSG(YAPI_INVALID_ARGUMENT, "invalid definition of u32");
    if (sizeof(u64) != 8) return YERRMSG(YAPI_INVALID_ARGUMENT, "invalid definition of u64");
    if (sizeof(s16) != 2) return YERRMSG(YAPI_INVALID_ARGUMENT, "invalid definition of s16");
    if (sizeof(s32) != 4) return YERRMSG(YAPI_INVALID_ARGUMENT, "invalid definition of s32");
    if (sizeof(s64) != 8) return YERRMSG(YAPI_INVALID_ARGUMENT, "invalid definition of s64");
    test.raw = 0xdeadbeef;

    if (test.bytes.a == 0xef && test.bytes.d == 0xde) {
        // little endian
        if (sizeof(test_compile) != 4) return YERRMSG(YAPI_INVALID_ARGUMENT, "pragma pack is not supported");
#ifdef CPU_BIG_ENDIAN
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid endianness. Lib is compiled for big endian but is used on little endian CPU");
#endif
    } else {
        // big endian
        if (sizeof(test_compile) != 4) return YERRMSG(YAPI_INVALID_ARGUMENT, "pragma pack is not supported");
#ifndef CPU_BIG_ENDIAN
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid endianness. Lib is compiled for little endian but is used on big endian CPU");
#endif
    }

#ifdef __BORLANDC__
#pragma warn . 8008
#pragma warn . 8066
#endif

    if (atof("1") != 1.0) {
#if defined(BUILD_ARMHF)
        return YERRMSG(YAPI_INVALID_ARGUMENT,"Invalid arm architecture (try armel binaries)");
#elif defined(BUILD_ARMEL)
        return YERRMSG(YAPI_INVALID_ARGUMENT,"Invalid arm architecture (try armhf binaries)");
#else
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid architecture");
#endif
    }

    //init safe malloc
    ySafeMemoryInit(64*1024);
#ifdef DEBUG_CRITICAL_SECTION
    yInitDebugCS();
#endif

    ctx = (yContextSt*)yMalloc(sizeof(yContextSt));
    yMemset(ctx, 0, sizeof(yContextSt));
    ctx->detecttype = detect_type;

    //initialize enumeration CS
    initializeAllCS(ctx);

    //initialize white/yellow pages support
    yHashInit();
    //initialize ywp list
    for (i = 0; i < ALLOC_YDX_PER_HUB; i++) {
        ctx->usbdevices[i] = INVALID_HASH_IDX;
    }
    ctx->devs = NULL;

    //initialize device pool
    ctx->devs = NULL;
    ctx->devhdlcount = 1;
    if (detect_type & Y_DETECT_USB) {
        int res;
        if (YISERR(res=yUsbInit(ctx,errmsg))) {
            deleteAllCS(ctx);
            yFree(ctx);
            return (YRETCODE)res;
        }
    }

    res = yTcpInitMulti(errmsg);
    if (YISERR(res)) {
        deleteAllCS(ctx);
        yFree(ctx);
        return (YRETCODE)res;
    }

    yCreateEvent(&ctx->yapiSleepWakeUpEvent);

    if (detect_type & Y_DETECT_NET) {
        if (YISERR(ySSDPStart(&ctx->SSDP, ssdpEntryUpdate, errmsg))) {
            yTcpShutdownMulti();
            yCloseEvent(&ctx->yapiSleepWakeUpEvent);
            deleteAllCS(ctx);
            yFree(ctx);
            return YAPI_IO_ERROR;
        }
    }
    ctx->fctx = yMalloc(sizeof(FIRMWARE_CONTEXT));
    ctx->firm_dev = yMalloc(sizeof(BootloaderSt));
    yContext = ctx;
#ifndef YAPI_IN_YDEVICE
    yProgInit(yContext->fctx, yContext->firm_dev);
#endif

#ifdef LINUX_API
    {
        struct utsname buffer;

        errno = 0;
        if (uname(&buffer) == 0) {
            if (strcmp(buffer.nodename,"raspberrypi")==0) {
                ctx->linux_flags |= YCTX_LINUX_ON_RPI;
            }
        }
   }
#endif

#if 0
    int siz = sizeof(yDeviceSt);
    dbglog("memset yDeviceSt struct of %d bytes\n", siz);


    siz = offsetof(yDeviceSt, vendorid);
    dbglog("ofs=%d instead of 0\n",siz);
    siz = offsetof(yDeviceSt, deviceid);
    dbglog("ofs=%d instead of 2\n",siz);
    siz = offsetof(yDeviceSt, devrelease);
    dbglog("ofs=%d instead of 4\n",siz);
    siz = offsetof(yDeviceSt, nbinbterfaces);
    dbglog("ofs=%d instead of 6\n",siz);
    siz = offsetof(yDeviceSt, manufacturer);
    dbglog("ofs=%d instead of 8\n",siz);
    siz = offsetof(yDeviceSt, productname);
    dbglog("ofs=%d instead of 28\n",siz);
    siz = offsetof(yDeviceSt, serial);
    dbglog("ofs=%d instead of 56\n",siz);
    siz = offsetof(yDeviceSt, logicalname);
    dbglog("ofs=%d instead of 76\n",siz);
    siz = offsetof(yDeviceSt, firmware);
    dbglog("ofs=%d instead of 96\n",siz);
    siz = offsetof(yDeviceSt, beacon);
    dbglog("ofs=%d instead of 118\n",siz);
#endif

    if (certificate || privatekey) {
        int res = yapiSetSSLCertificateSrv_internal(certificate, privatekey, errmsg);
        if (res < 0) {
            yapiFreeAPI();
        }
        return res;
    }

    return YAPI_SUCCESS;
}

static YRETCODE yapiAddSSLCertificateCli_internal(const char *cert, int len, char *errmsg)
{
#ifndef NO_YSSL
    int res;
    if (!yContext) {
        YPROPERR(yapiInitAPI_internal(0, NULL, NULL, errmsg));
    }
    res = yTcpAddClientCertificateSSL((u8*)cert, len, errmsg);
    if (YISERR(res)) {
        return (YRETCODE)res;
    }
    return YAPI_SUCCESS;
#else
    return YERRMSG(YAPI_NOT_SUPPORTED, "SSL support is not activated.");
#endif
}

static int yTcpTrafficPending(void);


static void yapiFreeAPI_internal(void)
{
    u64 timeout = yapiGetTickCount() + 1000000;
    char errmsg[YOCTO_ERRMSG_LEN];
    int i;

    if (yContext == NULL)
        return;

#ifdef PERF_API_FUNCTIONS
    dumpYApiPerf();
#endif


    while (yapiGetTickCount() < timeout && (yUsbTrafficPending() || yTcpTrafficPending())) {
        yapiHandleEvents_internal(errmsg);
        yApproximateSleep(50);
    }


#ifndef YAPI_IN_YDEVICE
    yProgFree(yContext->fctx);
    yFree(yContext->fctx);
    yFree(yContext->firm_dev);
#endif
    yEnterCriticalSection(&yContext->updateDev_cs);
    yEnterCriticalSection(&yContext->handleEv_cs);
    yEnterCriticalSection(&yContext->enum_cs);
    if (yContext->detecttype & Y_DETECT_USB) {
        yUsbFree(yContext,NULL);
    }

    ySSDPStop(&yContext->SSDP);
    //unregister all Network hub
    for (i = 0; i < NBMAX_NET_HUB; i++) {
        if (yContext->nethub[i]) {
            unregisterNetHub(yContext->nethub[i]);
        }
    }

    yHashFree();
    yTcpShutdownMulti();
    yCloseEvent(&yContext->yapiSleepWakeUpEvent);

    yLeaveCriticalSection(&yContext->updateDev_cs);
    yLeaveCriticalSection(&yContext->handleEv_cs);
    yLeaveCriticalSection(&yContext->enum_cs);
    deleteAllCS(yContext);
    ySafeMemoryDump(yContext);
    yFree(yContext);
    ySafeMemoryStop();
#ifdef DEBUG_CRITICAL_SECTION
    yFreeDebugCS();
#endif

    yContext = NULL;
}

static int yapiGetRemoteCertificate_internal(const char *url, u64 timeout, char *buffer, int maxsize, int *neededsize, char *errmsg)
{
#ifndef NO_YSSL
    int freeApi = 0;
    HubURLSt urlst;
    int res;
    if (!yContext) {
        YPROPERR(yapiInitAPI_internal(0, NULL, NULL, errmsg));
        freeApi = 1;
    }
    memset(&urlst, 0, sizeof(HubURLSt));
    res = yParseHubURL(&urlst, url, errmsg);
    if (res < 0) {
        return res;
    }
    res = yTcpDownloadSSLCert(urlst.host, urlst.portno, timeout, (u8*)buffer, (u32)maxsize, (u32*)neededsize, errmsg);
    yFreeParsedURL(&urlst);
    if (freeApi) {
        yapiFreeAPI_internal();
    }
    return res;
#else
    return YERRMSG(YAPI_NOT_SUPPORTED, "SSL support is not activated.");
#endif
}

static int yapiSetTrustedCertificatesList_internal(const char *certificatePath, char *errmsg)
{
    return YERRMSG(YAPI_NOT_SUPPORTED, "Only supported on PHP programming library.");
}

static void yapiSetNetworkTimeout_internal(int mstimeout)
{
    YctxNetworkTimeout = mstimeout;
}


static u32 yapiGetNetworkTimeout_internal(void)
{
    u32 res;
    res = YctxNetworkTimeout;
    return res;
}


static void yapiSetNetDevListValidity_internal(u64 value)
{
    YctxDeviceListValidityMs = value;
}


static u64 yapiGetNetDevListValidity_internal(void)
{
    u64 res;
    res = YctxDeviceListValidityMs;
    return res;
}


static void yapiRegisterLogFunction_internal(yapiLogFunction logfun)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    if (!yContext) {
        yapiInitAPI_internal(0, NULL, NULL, errmsg);
    }
    if (yContext) {
        yEnterCriticalSection(&yContext->enum_cs);
        yContext->log = logfun;
        yLeaveCriticalSection(&yContext->enum_cs);
    }
}


static void yapiRegisterDeviceLogCallback_internal(yapiDeviceLogCallback logCallback)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    if (!yContext) {
        yapiInitAPI_internal(0, NULL, NULL, errmsg);
    }
    if (yContext) {
        yEnterCriticalSection(&yContext->enum_cs);
        yContext->logDeviceCallback = logCallback;
        yLeaveCriticalSection(&yContext->enum_cs);
    }
}

static void yapiStartStopDeviceLogCallback_internal(const char *serial, int start)
{
    yStrRef serialref;
    int devydx;
    serialref = yHashPutStr(serial);
    devydx = wpGetDevYdx(serialref);
    if (devydx < 0)
        return;
    yEnterCriticalSection(&yContext->generic_cs);
    if (start) {
        yContext->generic_infos[devydx].flags |= DEVGEN_LOG_ACTIVATED;
    } else {
        yContext->generic_infos[devydx].flags &= ~DEVGEN_LOG_ACTIVATED;
    }
    yLeaveCriticalSection(&yContext->generic_cs);
    yapiPullDeviceLogEx(devydx);
}

static void yapiRegisterDeviceArrivalCallback_internal(yapiDeviceUpdateCallback arrivalCallback)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    if (!yContext) {
        yapiInitAPI_internal(0, NULL, NULL, errmsg);
    }
    if (yContext) {
        yEnterCriticalSection(&yContext->enum_cs);
        yContext->arrivalCallback = arrivalCallback;
        if (arrivalCallback != NULL) {
            // FIXME: WE SHOULD USE THE hash table to list all known devices
#if 0
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
#endif
        }
        yLeaveCriticalSection(&yContext->enum_cs);
    }
}


static void yapiRegisterDeviceRemovalCallback_internal(yapiDeviceUpdateCallback removalCallback)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    if (!yContext) {
        yapiInitAPI_internal(0, NULL, NULL, errmsg);
    }
    if (yContext) {
        yEnterCriticalSection(&yContext->enum_cs);
        yContext->removalCallback = removalCallback;
        yLeaveCriticalSection(&yContext->enum_cs);
    }
}


static void yapiRegisterDeviceChangeCallback_internal(yapiDeviceUpdateCallback changeCallback)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    if (!yContext) {
        yapiInitAPI_internal(0, NULL, NULL, errmsg);
    }
    if (yContext) {
        yContext->changeCallback = changeCallback;
    }
}


static void yapiRegisterBeaconCallback_internal(yapiBeaconCallback beaconCallback)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    if (!yContext) {
        yapiInitAPI_internal(0, NULL, NULL, errmsg);
    }
    if (yContext) {
        yContext->beaconCallback = beaconCallback;
    }
}


static void yapiRegisterDeviceConfigChangeCallback_internal(yapiDeviceUpdateCallback configChangeCallback)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    if (!yContext) {
        yapiInitAPI_internal(0, NULL, NULL, errmsg);
    }
    if (yContext) {
        yContext->confChangeCallback = configChangeCallback;
    }
}


static void yapiRegisterFunctionUpdateCallback_internal(yapiFunctionUpdateCallback updateCallback)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    if (!yContext) {
        yapiInitAPI_internal(0, NULL, NULL, errmsg);
    }
    if (yContext) {
        yContext->functionCallback = updateCallback;
    }
}

static void yapiRegisterTimedReportCallback_internal(yapiTimedReportCallback timedReportCallback)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    if (!yContext) {
        yapiInitAPI_internal(0, NULL, NULL, errmsg);
    }
    if (yContext) {
        yContext->timedReportCallback = timedReportCallback;
    }
}


#ifdef DEBUG_NET_NOTIFICATION
#include <direct.h>
#include <stdio.h>
static void dumpNotif(const char *buffer)
{
    FILE *f;
    printf("%s",buffer);
    YASSERT(YFOPEN(&f,"c:\\tmp\\api_not.txt","ab")==0);
    fwrite(buffer,1,YSTRLEN(buffer),f);
    fclose(f);
}
#endif


static void wpUpdateTCP(HubSt *hub, const char *serial, const char *name, u8 beacon)
{
#define LOCALURL_LEN 64
    int status;
    yStrRef serialref;
    yStrRef lnameref;
    yUrlRef devurl;
    int devydx;

    serialref = yHashPutStr(serial);
    devydx = wpGetDevYdx(serialref);

    if (devydx < 0) {
        // drop notification until we have register the device with
        // a real enumeration
        return;
    }

    if (hub->serial_hash != serialref) {
        // Insert device into white pages
        devurl = hub->serial_hash;
    } else {
        devurl = INVALID_HASH_IDX;
    }
    lnameref = yHashPutStr(name);
    status = wpRegister(-1, serialref, lnameref, INVALID_HASH_IDX, 0, devurl, beacon);
    if (status & 1) {
        ypRegister(YSTRREF_MODULE_STRING, serialref, YSTRREF_mODULE_STRING, lnameref, YOCTO_AKA_YFUNCTION, -1, NULL);
        // Forward high-level notification to API user
        if (yContext->changeCallback) {
            yEnterCriticalSection(&yContext->deviceCallbackCS);
            yContext->changeCallback(serialref);
            yLeaveCriticalSection(&yContext->deviceCallbackCS);
        }
    }
    if (status & 2) {
        // Forward high-level notification to API user
        if (yContext->beaconCallback) {
            yEnterCriticalSection(&yContext->functionCallbackCS);
            yContext->beaconCallback(serialref, beacon);
            yLeaveCriticalSection(&yContext->functionCallbackCS);
        }
    }
}

int handleNetNotification(HubSt *hub)
{
    u16 pos;
    u16 end, size;
    char buffer[128];
    char *p;
    u8 pkttype = 0, devydx, funydx, funclass;
    char *serial = NULL, *name, *funcid, *children;
    char value[YOCTO_PUBVAL_LEN];
    u8 report[18];
    char netstop = NOTIFY_NETPKT_STOP;
    char escapechar = 27;
#ifdef DEBUG_NET_NOTIFICATION
    u32             abspos = hub->notifAbsPos;
    char            Dbuffer[1024];
    u8              throwbuf[1024];
    u16             tmp;
#endif

    // search for start of notification
    size = yFifoGetUsed(&(hub->not_fifo));
    while (size >= NOTIFY_NETPKT_START_LEN) {
        yPeekFifo(&(hub->not_fifo), &pkttype, 1, 0);
        if (pkttype != NOTIFY_NETPKT_STOP) break;
        // drop newline and loop
        yPopFifo(&(hub->not_fifo),NULL, 1);
        // note: keep-alive packets don't count in the notification channel position
        size--;
    }
    if (size < NOTIFY_NETPKT_START_LEN) {
        return 0;
    }
    // make sure we have a full notification
    end = ySeekFifo(&(hub->not_fifo), (u8*)&netstop, 1, 0, 0, 0);
    if (end == 0xffff) {
        if (yFifoGetFree(&(hub->not_fifo)) == 0) {
            dbglog("Too many invalid notifications, clearing buffer\n");
            yFifoEmpty((&(hub->not_fifo)));
            return 1;
        }
        return 0;
    }
    // make sure we have a full notification
    if (0xffff != ySeekFifo(&(hub->not_fifo), (u8*)&escapechar, 1, 0, end, 0)) {
        // drop notification that contain esc char
        yPopFifo(&(hub->not_fifo), NULL, end + 1);
        return 1;
    }
    // handle short funcvalydx notifications
    if (pkttype >= NOTIFY_NETPKT_CONFCHGYDX && pkttype <= NOTIFY_NETPKT_TIMEAVGYDX) {
        memset(value, 0, YOCTO_PUBVAL_LEN);
        if (end + 1 > (u16)sizeof(buffer)) {
            dbglog("Drop invalid short notification (too long :%d)\n", end + 1);
            hub->notifAbsPos += end + 1;
            return 1;
        }
        yPopFifo(&(hub->not_fifo), (u8*)buffer, end + 1);
        hub->notifAbsPos += end + 1;
        p = buffer + 1;
        devydx = (*p++) - 'A';
        funydx = (*p++) - '0';
        if (funydx & 64) {
            // high bit of devydx is on second character
            funydx -= 64;
            devydx += 128;
        }
        pos = 0;
        switch (pkttype) {
        case NOTIFY_NETPKT_FUNCVALYDX:
            while (*p != 0 && *p != NOTIFY_NETPKT_STOP && pos < YOCTO_PUBVAL_LEN - 1) {
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
            if (devydx < MAX_YDX_PER_HUB) {
                Notification_funydx funInfo;
                funInfo.raw = funydx;
                ypUpdateYdx(devydx, funInfo, value);
            }
            break;
        case NOTIFY_NETPKT_DEVLOGYDX:
            // Map hub-specific devydx to our devydx
            devydx = hub->devYdxMap[devydx];
            if (devydx < MAX_YDX_PER_HUB) {
                yEnterCriticalSection(&yContext->generic_cs);
                if (yContext->generic_infos[devydx].flags & DEVGEN_LOG_ACTIVATED) {
                    yContext->generic_infos[devydx].flags |= DEVGEN_LOG_PENDING;
#ifdef DEBUG_NET_NOTIFICATION
                        dbglog("notify device log for devydx %d\n", devydx);
#endif
                }
                yLeaveCriticalSection(&yContext->generic_cs);
            }
            break;
        case NOTIFY_NETPKT_CONFCHGYDX:
            // Map hub-specific devydx to our devydx
            devydx = hub->devYdxMap[devydx];
            if (devydx < MAX_YDX_PER_HUB) {
                // Forward high-level device config change notification to API user
                if (yContext->confChangeCallback) {
                    yStrRef serialref;
                    yEnterCriticalSection(&yContext->generic_cs);
                    serialref = yContext->generic_infos[devydx].serial;
                    yLeaveCriticalSection(&yContext->generic_cs);
                    yEnterCriticalSection(&yContext->deviceCallbackCS);
#ifdef DEBUG_NET_NOTIFICATION
                        dbglog("notify conf change for devydx %d\n", devydx);
#endif
                    yContext->confChangeCallback(serialref);
                    yLeaveCriticalSection(&yContext->deviceCallbackCS);
                }
            }
            break;
        case NOTIFY_NETPKT_TIMEVALYDX:
        case NOTIFY_NETPKT_TIMEAVGYDX:
        case NOTIFY_NETPKT_TIMEV2YDX:
            // Map hub-specific devydx to our devydx
            devydx = hub->devYdxMap[devydx];
            if (devydx >= MAX_YDX_PER_HUB) break;

            report[pos++] = (pkttype == NOTIFY_NETPKT_TIMEVALYDX ? 0 : (pkttype == NOTIFY_NETPKT_TIMEAVGYDX ? 1 : 2));
            while (isxdigit((u8)p[0]) && isxdigit((u8)p[1]) && pos < sizeof(report)) {
                int hi_nib = (p[0] <= '9' ? p[0] - '0' : 10 + (p[0] & 0x4f) - 'A');
                int lo_nib = (p[1] <= '9' ? p[1] - '0' : 10 + (p[1] & 0x4f) - 'A');
                report[pos++] = hi_nib * 16 + lo_nib;
                p += 2;
            }
#ifdef DEBUG_NET_NOTIFICATION
                YSPRINTF(Dbuffer,512,"%s >devYdx=%d funYdx=%d (%d)\n",
                         (pkttype == NOTIFY_NETPKT_TIMEVALYDX ? "TimeValR" : "TimeAvgR"),
                         devydx,funydx,abspos);
                dumpNotif(Dbuffer);
#endif
            if (funydx == 15) {
                u64 t = report[1] + 0x100u * report[2] + 0x10000u * report[3] + 0x1000000u * report[4];
                u32 ms = report[5] << 2;
                u64 freq = 0;
                if (pos >= 8) {
                    ms += report[6] >> 6;
                    freq = report[7];
                    freq += (report[6] & 0xf) * 0x100;
                    if (report[6] & 0x10) {
                        freq *= 1000;
                    }

                }
                yEnterCriticalSection(&yContext->generic_cs);
                yContext->generic_infos[devydx].lastTimeRef = t * 1000 + ms;
                yContext->generic_infos[devydx].lastFreq = freq;
                yLeaveCriticalSection(&yContext->generic_cs);
            } else {
                Notification_funydx funInfo;
                YAPI_FUNCTION fundesc;
                u64 deviceTime;
                u64 freq;
                yEnterCriticalSection(&yContext->generic_cs);
                deviceTime = yContext->generic_infos[devydx].lastTimeRef;
                freq = yContext->generic_infos[devydx].lastFreq;
                yLeaveCriticalSection(&yContext->generic_cs);
                funInfo.raw = funydx;
                ypRegisterByYdx(devydx, funInfo, NULL, &fundesc);
                yFunctionTimedUpdate(fundesc, deviceTime, freq, report, pos);
            }
            break;
        case NOTIFY_NETPKT_FUNCV2YDX:
            while (*p != 0 && *p != NOTIFY_NETPKT_STOP && pos < YOCTO_PUBVAL_LEN - 1) {
                value[pos++] = *p++;
            }
            value[pos] = 0;
        // Map hub-specific devydx to our devydx
            devydx = hub->devYdxMap[devydx];
            if (devydx < MAX_YDX_PER_HUB) {
                Notification_funydx funInfo;
                unsigned char value8bit[YOCTO_PUBVAL_LEN];
                memset(value8bit, 0, YOCTO_PUBVAL_LEN);
                funInfo.raw = funydx;
                if (decodeNetFuncValV2((u8*)value, &funInfo, (char*)value8bit) >= 0) {
#ifdef DEBUG_NET_NOTIFICATION
                        if(!funInfo.v2.typeV2) {
                            YSPRINTF(Dbuffer,512,"FuncV2YDX >devYdx=%d funYdx=%d val=%s (%d)\n",devydx,funydx,value8bit,abspos);
                        } else {
                            YSPRINTF(Dbuffer,512,"FuncV2YDX >devYdx=%d funYdx=%d val=%d:%02x.%02x.%02x.%02x.%02x.%02x (%d)\n",
                                     devydx, funInfo.v2.funydx, funInfo.v2.typeV2,
                                     value8bit[0],value8bit[1],value8bit[2],value8bit[3],value8bit[4],value8bit[5],abspos);
                        }
                        dumpNotif(Dbuffer);
#endif
                    ypUpdateYdx(devydx, funInfo, (char*)value8bit);
                }
            }
            break;
        case NOTIFY_NETPKT_FLUSHV2YDX:
            // To be implemented later
            break;
        default:
            break;
        }
        return 1;
    }

    // make sure packet is a valid notification
    pos = ySeekFifo(&(hub->not_fifo), (u8*)(NOTIFY_NETPKT_START), NOTIFY_NETPKT_START_LEN, 0, end, 0);
    if (pos != 0) {
        // does not start with signature, drop everything until stop marker
#ifdef DEBUG_NET_NOTIFICATION
        memset(throwbuf, 0, sizeof(throwbuf));
        tmp = (end > 50 ? 50 : end);
        yPopFifo(&(hub->not_fifo),throwbuf,tmp);
        yPopFifo(&(hub->not_fifo),NULL,end+1-tmp);
        Dbuffer[1023]=0;
        YSPRINTF(Dbuffer,512,"throw %d / %d [%s]\n",
                 end,pos,throwbuf);
        dumpNotif(Dbuffer);
#else
        yPopFifo(&(hub->not_fifo),NULL, end + 1);
#endif
        hub->notifAbsPos += end + 1;
        return 0;
    }

    // full packet at start of fifo
    size = end - NOTIFY_NETPKT_START_LEN;
    YASSERT(NOTIFY_NETPKT_MAX_LEN > size, size);
    yPopFifo(&(hub->not_fifo),NULL,NOTIFY_NETPKT_START_LEN);
    yPopFifo(&(hub->not_fifo), (u8*)buffer, size + 1);
    buffer[size] = 0;
    pkttype = *buffer;
    p = buffer + 1;
    if (pkttype == NOTIFY_NETPKT_NOT_SYNC) {
        int testPing;
        u64 newpos;
#ifdef DEBUG_NET_NOTIFICATION
        YSPRINTF(Dbuffer,512,"Sync from %d to %s\n",
             hub->notifAbsPos, p);
        dumpNotif(Dbuffer);
#endif
        newpos = atoll(p);
        YASSERT(newpos <= 0xffffffff, newpos);
        hub->notifAbsPos = (u32)newpos;
        //look if we have a \n just after the sync notification
        // if yes this mean that the hub will send some ping notification
        testPing = ySeekFifo(&(hub->not_fifo), (u8*)&netstop, 1, 0, 1, 0);
        if (testPing == 0) {
#ifdef DEBUG_NET_NOTIFICATION
            YSPRINTF(Dbuffer,1024,"HUB: %s will send ping notification\n",hub->url.host);
            dumpNotif(Dbuffer);
#endif
            hub->send_ping = 1;
        }
        return 1;
    }
    hub->notifAbsPos += size + 1 + NOTIFY_NETPKT_START_LEN;
    if (pkttype != NOTIFY_NETPKT_FUNCVALYDX) {
        serial = p;
        p = strchr(serial,NOTIFY_NETPKT_SEP);
        if (p == NULL) {
#ifdef DEBUG_NET_NOTIFICATION
            YSPRINTF(Dbuffer,512,"no serialFOR %s\n",buffer);
            dumpNotif(Dbuffer);
#endif
            return 0;
        }
        *p++ = 0;
    }


    switch (pkttype) {
    case NOTIFY_NETPKT_NAME:
        name = p;
        p = strchr(name,NOTIFY_NETPKT_SEP);
        if (p == NULL) {
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
        wpUpdateTCP(hub, serial, name, (*p == '1' ? 1 : 0));
        break;
    case NOTIFY_NETPKT_FUNCNAME:
        funcid = p;
        p = strchr(funcid,NOTIFY_NETPKT_SEP);
        if (p == NULL) {
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
        ypUpdateTCP(serial, funcid, name, -1, -1,NULL);
        break;
    case NOTIFY_NETPKT_FUNCVAL:
        funcid = p;
        p = strchr(funcid,NOTIFY_NETPKT_SEP);
        if (p == NULL) {
#ifdef DEBUG_NET_NOTIFICATION
                dbglog("drop: invalid funcid (%X)\n",pkttype);
#endif
            return 1;
        }
        *p++ = 0;
        memset(value, 0,YOCTO_PUBVAL_LEN);
        memcpy(value, p,YSTRLEN(p));
#ifdef DEBUG_NET_NOTIFICATION
            //dbglog("NOTIFY_PKT_FUNCVAL %s : funcid is \"%s\" value \"%s\"\n",serial,funcid,value);
            YSPRINTF(Dbuffer,512,"FuncVAL  >%s  funcid=%s val=%s (%d)\n",
                serial,funcid,value,abspos);
            dumpNotif(Dbuffer);
#endif
        ypUpdateTCP(serial, funcid,NULL, -1, -1, value);
        break;
    case NOTIFY_NETPKT_FUNCNAMEYDX:
        funcid = p;
        p = strchr(funcid,NOTIFY_NETPKT_SEP);
        if (p == NULL) {
#ifdef DEBUG_NET_NOTIFICATION
                dbglog("drop: invalid funcid (%X:%s)\n",pkttype,serial);
#endif
            return 1;
        }
        *p++ = 0;
        name = p;
        p = strchr(name,NOTIFY_NETPKT_SEP);
        if (p == NULL) {
#ifdef DEBUG_NET_NOTIFICATION
                dbglog("drop: invalid funcname (%X:%s)\n",pkttype,serial);
#endif
            return 1;
        }
        *p++ = 0;
        funydx = atoi(p);
        p = strchr(p,NOTIFY_NETPKT_SEP);
        if (p == NULL || p[1] < '0') {
            funclass = YOCTO_AKA_YFUNCTION;
        } else {
            funclass = p[1] - '0';
        }
#ifdef DEBUG_NET_NOTIFICATION
            YSPRINTF(Dbuffer,512,"FuncNYDX >%s  funcid=%s funcname=%s funYdx=%d (%d)\n",
                     serial,funcid,name,funydx,abspos);
            dumpNotif(Dbuffer);
#endif
        ypUpdateTCP(serial, funcid, name, funclass, funydx, NULL);
        break;
    case NOTIFY_NETPKT_CHILD:
        children = p;
        p = strchr(children,NOTIFY_NETPKT_SEP);
        if (p == NULL) {
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
        hub->devListExpires = 0;
        if (*p == '0') {
            unregisterNetDevice(hub, yHashPutStr(children));
        }
        break;
    case NOTIFY_NETPKT_LOG:
#ifdef DEBUG_NET_NOTIFICATION
            dbglog("NOTIFY_NETPKT_LOG %s\n", serial);
#endif
    {
        yStrRef serialref = yHashPutStr(serial);
        int devydx = wpGetDevYdx(serialref);
        if (devydx >= 0) {
            yEnterCriticalSection(&yContext->generic_cs);
            if (yContext->generic_infos[devydx].flags & DEVGEN_LOG_ACTIVATED) {
                yContext->generic_infos[devydx].flags |= DEVGEN_LOG_PENDING;
#ifdef DEBUG_NET_NOTIFICATION
                        dbglog("notify device log for %s (%d)\n", serial,devydx);
#endif
            }
            yLeaveCriticalSection(&yContext->generic_cs);
        }
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
    int i;
    HubSt *hub;

    for (i = 0; i < NBMAX_NET_HUB; i++) {
        hub = yContext->nethub[i];
        if (hub == NULL)
            continue;
        if (yReqHasPending(hub)) {
            return 1;
        }
    }
    return 0;
}

void request_pending_logs(HubSt *hub)
{
    int i;
    for (i = 0; i < ALLOC_YDX_PER_HUB; i++) {
        int devydx = hub->devYdxMap[i];
        if (devydx != 0xff) {
            yapiPullDeviceLogEx(devydx);
        }
    }
}

static void* yhelper_thread(void *ctx)
{
    int i, towatch;
    u8 buffer[512];
    yThread *thread = (yThread*)ctx;
    char errmsg[YOCTO_ERRMSG_LEN];
    HubSt *hub = (HubSt*)thread->ctx;
    RequestSt *req, *selectlist[1 + ALLOC_YDX_PER_HUB];
    u32 toread;
    int res;
    int first_notification_connection = 1;
#ifdef DEBUG_NET_NOTIFICATION
    char        Dbuffer[1024];
#endif


    yThreadSignalStart(thread);
    while (!yThreadMustEnd(thread)) {
        // Handle async connections as well in this thread
        request_pending_logs(hub);
        towatch = 0;
        if (hub->state == NET_HUB_ESTABLISHED || hub->state == NET_HUB_TRYING) {
            selectlist[towatch] = hub->http.notReq;
            towatch++;
        } else if (hub->state == NET_HUB_TOCLOSE) {
            yReqClose(hub->http.notReq);
            hub->state = NET_HUB_CLOSED;
        } else if (hub->state == NET_HUB_DISCONNECTED) {
            u64 now;
            if (hub->http.notReq == NULL) {
                hub->http.notReq = yReqAlloc(hub);
            }
            now = yapiGetTickCount();
            if ((u64)(now - hub->lastAttempt) > hub->attemptDelay) {
                char request[256];
                if (!hub->info.serial[0]) {
                    int load_res = LoadInfoJson(hub, hub->netTimeout, errmsg);
                    // YAPI_NOT_SUPPORTED -> old hub that does not support info.json
                    if (load_res < 0 && load_res != YAPI_NOT_SUPPORTED) {
                        hub->attemptDelay = 500 << hub->retryCount;
                        if (hub->attemptDelay > 8000)
                            hub->attemptDelay = 8000;
                        hub->lastAttempt = yapiGetTickCount();
                        hub->retryCount++;
                        yEnterCriticalSection(&hub->access);
                        hub->errcode = ySetErr(load_res, hub->errmsg, errmsg, NULL, 0);
                        yLeaveCriticalSection(&hub->access);
                        continue;
                    }
                }
#ifdef TRACE_NET_HUB
                dbglog("TRACE(%s): try to open notification socket at %d\n", hub->url.org_url, hub->notifAbsPos);
#endif
                // reset fifo
                yFifoEmpty(&(hub->not_fifo));
                if (first_notification_connection) {
                    YSPRINTF(request, 256, "GET /not.byn HTTP/1.1\r\n\r\n");
                } else {
                    YSPRINTF(request, 256, "GET /not.byn?abs=%u HTTP/1.1\r\n\r\n", hub->notifAbsPos);
                }
                res = yReqOpen(hub->http.notReq, 2 * hub->netTimeout, 0, request, YSTRLEN(request), 0, NULL, NULL, NULL, NULL, errmsg);
                if (YISERR(res)) {
                    hub->attemptDelay = 500 << hub->retryCount;
                    if (hub->attemptDelay > 8000)
                        hub->attemptDelay = 8000;
                    hub->lastAttempt = yapiGetTickCount();
                    hub->retryCount++;
                    yEnterCriticalSection(&hub->access);
                    hub->errcode = ySetErr(res, hub->errmsg, errmsg, NULL, 0);
                    yLeaveCriticalSection(&hub->access);

#ifdef TRACE_NET_HUB
                    dbglog("TRACE(%s): unable to open notification socket(%s)\n", hub->url.org_url, errmsg);
                    dbglog("TRACE(%s): retry in %dms (%d retries)\n", hub->url.org_url, hub->attemptDelay, hub->retryCount);
#endif
                    if (res == YAPI_SSL_UNK_CERT || res == YAPI_SSL_ERROR) {
                        // invalid certificate -> no need to retry
                        setNotificationConnectionOFF(hub);
                        hub->state = NET_HUB_TOCLOSE;
                    }
                } else {
#ifdef TRACE_NET_HUB
                    dbglog("TRACE(%s): notification socket open\n", hub->url.org_url);
#endif
#ifdef DEBUG_NET_NOTIFICATION
                    YSPRINTF(Dbuffer,1024,"HUB: %s started\n",hub->url.host);
                    dumpNotif(Dbuffer);
#endif
                    hub->state = NET_HUB_TRYING;
                    hub->retryCount = 0;
                    hub->attemptDelay = 500;
                    hub->http.lastTraffic = yapiGetTickCount();
                    hub->send_ping = 0;
                    selectlist[towatch++] = hub->http.notReq;
                    first_notification_connection = 0;
                }
            }
        }

        // Handle async connections as well in this thread
        for (i = 0; i < ALLOC_YDX_PER_HUB; i++) {
            req = yContext->tcpreq[i];
            if (req == NULL || req->hub != hub) {
                continue;
            }
            if (yReqIsAsync(req)) {
                selectlist[towatch++] = req;
            }
        }

        if (YISERR(yReqMultiSelect(selectlist, towatch, 1000, &hub->wuce, errmsg))) {
            dbglog("yTcpMultiSelectReq failed (%s)\n", errmsg);
            yApproximateSleep(1000);
        } else {
            for (i = 0; i < towatch; i++) {
                req = selectlist[i];
                if (req == hub->http.notReq) {
                    toread = yFifoGetFree(&hub->not_fifo);
                    while (toread > 0) {
                        if (toread >= sizeof(buffer)) toread = sizeof(buffer) - 1;
                        res = yReqRead(req, buffer, toread);
                        if (res > 0) {
                            buffer[res] = 0;
#if 0 //def DEBUG_NET_NOTIFICATION
                            YSPRINTF(Dbuffer,1024,"HUB: %X->%s push %d [\n%s\n]\n",hub->url,hub->name,res,buffer);
                            dumpNotif(Dbuffer);
#endif
                            yPushFifo(&(hub->not_fifo), (u8*)buffer, res);
                            if (hub->state == NET_HUB_TRYING) {
                                int eoh = ySeekFifo(&(hub->not_fifo), (u8*)"\r\n\r\n", 4, 0, 0, 0);
                                if (eoh != 0xffff) {
                                    if (eoh >= 12) {
                                        yPopFifo(&(hub->not_fifo), (u8*)buffer, 12);
                                        yPopFifo(&(hub->not_fifo), NULL, eoh + 4 - 12);
                                        if (!memcmp((u8*)buffer, (u8*)"HTTP/1.1 200", 12)) {
                                            hub->state = NET_HUB_ESTABLISHED;
                                            if (hub->notConLastAlive==0 || (yapiGetTickCount() - hub->notConLastAlive) > 4000){
                                                log_hub_state(&hub->url, "connected", "HTTP");
                                            }
                                            setNotificationConnectionON(hub);
                                            
                                        }
                                    } else if (eoh >= 2) {
                                        yPopFifo(&(hub->not_fifo), (u8*)buffer, 2);
                                        yPopFifo(&(hub->not_fifo), NULL, eoh + 4 - 2);
                                        if (!memcmp((u8*)buffer, (u8*)"OK", 2)) {
                                            hub->state = NET_HUB_ESTABLISHED;
                                            log_hub_state(&hub->url, "connected", "HTTP");
                                            setNotificationConnectionON(hub);
                                        }
                                    }
                                    if (hub->state != NET_HUB_ESTABLISHED) {
                                        // invalid header received, give up
                                        hub->state = NET_HUB_TOCLOSE;
                                        dbglog("Network hub %s cannot provide notifications", hub->url.host);
                                        setNotificationConnectionOFF(hub);
                                    }
                                }
                            }
                            if (hub->state == NET_HUB_ESTABLISHED) {
                                setNotificationConnectionON(hub);
                                while (handleNetNotification(hub));
                            }
                            hub->http.lastTraffic = yapiGetTickCount();
                        } else {
                            if (hub->send_ping && ((u64)(yapiGetTickCount() - hub->http.lastTraffic)) > NET_HUB_NOT_CONNECTION_TIMEOUT) {

                                dbglog("network hub %s didn't respond for too long (%d)\n", hub->url.org_url, res);
                                // hub did not send data for too long. Close the connection and bring it back.
                                yReqClose(req);
                                hub->state = NET_HUB_DISCONNECTED;
                                setNotificationConnectionOFF(hub);
                                if (!hub->info.use_pure_http) {
                                    log_hub_state(&hub->url, "disconnected", "HTTP");
                                }
                            }
                            // nothing more to be read, exit loop
                            break;
                        }
                        toread = yFifoGetFree(&hub->not_fifo);
                    }
                    res = yReqIsEof(req, errmsg);
                    if (res != 0) {
                        // error or remote close
                        yReqClose(req);
                        hub->state = NET_HUB_DISCONNECTED;
                        if (res == 1) {
                            // remote close -> retry a quick as possible without marking the hub as unavailable.
                            // (ex: the api is connecting to the CloudHub)
                            YERRMSG(YAPI_IO_ERROR, "Connection closed by remote host");
                            // dbglog("Disconnected from network hub %s (%s)\n", hub->host, errmsg);
                        } else if (res == YAPI_UNAUTHORIZED) {
                            // invalid login -> no need to retry
                            // dbglog("Disconnected from network hub %s (%s)\n", hub->host, errmsg);
                            setNotificationConnectionOFF(hub);
                            hub->state = NET_HUB_TOCLOSE;
                            log_hub_state(&hub->url, "disconnected", "HTTP");
                        } else {
                            //error -> retry with a delay and invalidate the connextion status
                            setNotificationConnectionOFF(hub);
                            hub->attemptDelay = 500 << hub->retryCount;
                            if (hub->attemptDelay > 8000)
                                hub->attemptDelay = 8000;
                            hub->lastAttempt = yapiGetTickCount();
                            hub->retryCount++;
                            yEnterCriticalSection(&hub->access);
                            hub->errcode = ySetErr(res, hub->errmsg, errmsg, NULL, 0);
                            yLeaveCriticalSection(&hub->access);
                            log_hub_state(&hub->url, "disconnected", "HTTP");
                        }
                        
#ifdef DEBUG_NET_NOTIFICATION
                        YSPRINTF(Dbuffer, 1024, "Network hub %s has closed the connection for notification\n", hub->url.host);
                        dumpNotif(Dbuffer);
#endif
                    }
                } else if (yReqIsAsync(req)) {
                    res = yReqIsEof(req, errmsg);
                    if (res != 0) {
                        yReqClose(req);
                    }
                }
            }
        }
    }

    setNotificationConnectionOFF(hub);
    if (hub->state == NET_HUB_TOCLOSE) {
        yReqClose(hub->http.notReq);
        hub->state = NET_HUB_CLOSED;
    }

    yThreadSignalEnd(thread);
    return NULL;
}


static YRETCODE yapiLockFunctionCallBack_internal(char *errmsg)
{
    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    yEnterCriticalSection(&yContext->functionCallbackCS);
    return YAPI_SUCCESS;
}


static YRETCODE yapiUnlockFunctionCallBack_internal(char *errmsg)
{
    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    yLeaveCriticalSection(&yContext->functionCallbackCS);
    return YAPI_SUCCESS;
}


static YRETCODE yapiLockDeviceCallBack_internal(char *errmsg)
{
    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    yEnterCriticalSection(&yContext->deviceCallbackCS);
    return YAPI_SUCCESS;
}


static YRETCODE yapiUnlockDeviceCallBack_internal(char *errmsg)
{
    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    yLeaveCriticalSection(&yContext->deviceCallbackCS);
    return YAPI_SUCCESS;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiGetDLLPath(char *path, int pathsize, char *errmsg)
{
#ifdef WINDOWS_API
    int res;
    HMODULE module_handle_a = GetModuleHandleA("yapi");
    res = GetModuleFileNameA(module_handle_a, path, pathsize);
    if (res > 0) {
        return res;
    } else {
        dbglog("Unable to get dll path : %d\n", res);
        return YERR(YAPI_IO_ERROR);
    }
#else
#if defined(_GNU_SOURCE) && defined(GENERATE_DYN)
    Dl_info infos;

    if(dladdr((void*)"yapiRegisterHub", &infos) == 0)
    {
        YSPRINTF(errmsg, YOCTO_ERRMSG_LEN, "dladdr failed: %s", dlerror());
        return YAPI_IO_ERROR;
    }

    return YSPRINTF(path, pathsize, infos.dli_fname);
#else
    return YERR(YAPI_NOT_SUPPORTED);
#endif
#endif
}

static YRETCODE yapiRegisterHubEx(const char *url, int checkacces, char *errmsg)
{
    int i;
    int res;

    if (!yContext) {
        YPROPERR(yapiInitAPI_internal(0, NULL, NULL, errmsg));
    }

    if (YSTRICMP(url, "usb") == 0) {
        if (!(yContext->detecttype & Y_DETECT_USB)) {
            yEnterCriticalSection(&yContext->enum_cs);
            res = yUsbInit(yContext, errmsg);
            if (!YISERR(res)) {
                yContext->detecttype |= Y_DETECT_USB;
            }
            yLeaveCriticalSection(&yContext->enum_cs);
            YPROPERR(res);
        }
        if (checkacces) {
            yEnterCriticalSection(&yContext->updateDev_cs);
            res = yUSBUpdateDeviceList(errmsg);
            yLeaveCriticalSection(&yContext->updateDev_cs);
            return res;
        }
    } else if (YSTRICMP(url, "net") == 0) {
        if (!(yContext->detecttype & Y_DETECT_NET)) {
            yEnterCriticalSection(&yContext->enum_cs);
            yContext->detecttype |= Y_DETECT_NET;
            res = ySSDPStart(&yContext->SSDP, ssdpEntryUpdate, errmsg);
            yLeaveCriticalSection(&yContext->enum_cs);
            YPROPERR(res);
        }
        if (checkacces) {
            res = yapiUpdateDeviceList_internal(1, errmsg);
            return res;
        }
    } else {
        HubSt *hubst = NULL;
        char urlbuff[YOCTO_MAX_URL_LEN];
        int firstfree;
        void* (*thead_handler)(void *);
        YSTRCPY(urlbuff, YOCTO_MAX_URL_LEN, url);

        yEnterCriticalSection(&yContext->enum_cs);
        hubst = yGetHubFromURL(url);
        if (hubst) {
            for (i = 0; i < MAX_KNOW_URLS_SIZE; i++) {
                if (hubst->know_urls[i] == NULL || YSTRCMP(hubst->know_urls[i], url) == 0) {
                    break;
                }
            }
            if (i < MAX_KNOW_URLS_SIZE && hubst->know_urls[i] == NULL) {
                hubst->know_urls[i] = YSTRDUP(url);
            }
            hubst->mandatory = checkacces;
#ifdef TRACE_NET_HUB
            dbglog("Hub already registred as %s (new =%s)\n", hubst->url.org_url, url);
#endif
            yLeaveCriticalSection(&yContext->enum_cs);
            return YAPI_SUCCESS;
        }
        yLeaveCriticalSection(&yContext->enum_cs);
    retry:
        hubst = yapiAllocHub(urlbuff, YctxNetworkTimeout, &res, errmsg);
        if (hubst == NULL) {
            if (res == YAPI_DOUBLE_ACCES) {
                return YAPI_SUCCESS;
            }
            return res;
        }
        if (checkacces) {
            hubst->mandatory = 1;
        }
#ifdef NO_YSSL
        if (hubst->url.proto == PROTO_SECURE_HTTP || hubst->url.proto == PROTO_SECURE_WEBSOCKET) {
            yapiFreeHub(hubst);
            return YERRMSG(YAPI_NOT_SUPPORTED, "SSL support is not activated.");
        }
#endif
        //look if we already know this
        yEnterCriticalSection(&yContext->enum_cs);

        firstfree = NBMAX_NET_HUB;
        for (i = 0; i < NBMAX_NET_HUB; i++) {
            if (firstfree == NBMAX_NET_HUB && yContext->nethub[i] == NULL) {
                firstfree = i;
            }
        }
        if (i >= NBMAX_NET_HUB && firstfree < NBMAX_NET_HUB) {
            i = firstfree;
            // save mapping attributed from first access
#ifdef TRACE_NET_HUB
            dbglog("HUB: register as %s \n", hubst->url.org_url);
#endif
            yContext->nethub[i] = hubst;
            if (YISERR(res = yStartWakeUpSocket(&yContext->nethub[i]->wuce, errmsg))) {
                yLeaveCriticalSection(&yContext->enum_cs);
                return (YRETCODE)res;
            }
            if (hubst->url.proto == PROTO_WEBSOCKET || hubst->url.proto == PROTO_SECURE_WEBSOCKET) {
                thead_handler = ws_thread;
            } else if (hubst->url.proto == PROTO_HTTP || hubst->url.proto == PROTO_SECURE_HTTP) {
                thead_handler = yhelper_thread;
            } else {
                yLeaveCriticalSection(&yContext->enum_cs);
                return YERRMSG(YAPI_NOT_SUPPORTED, "Unsupported hub proto");
            }
            //yThreadCreate will not create a new thread if there is already one running
            if (yThreadCreateNamed(&yContext->nethub[i]->net_thread, hubst->url.host, thead_handler, (void*)yContext->nethub[i]) < 0) {
                yLeaveCriticalSection(&yContext->enum_cs);
                return YERRMSG(YAPI_IO_ERROR, "Unable to start helper thread");
            }
            yDringWakeUpSocket(&yContext->nethub[i]->wuce, 1, errmsg);
        }
        yLeaveCriticalSection(&yContext->enum_cs);
        if (i == NBMAX_NET_HUB) {
            yapiFreeHub(hubst);
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Too many network hub registered");
        }

        if (checkacces) {
            // ensure the thread has been able to connect to the hub
            u64 timeout = yapiGetTickCount() + YctxNetworkTimeout;
            while (!isNotificationConnectionON(hubst) && hubst->errcode == YAPI_SUCCESS && timeout > yapiGetTickCount()) {
                yapiSleep(100, errmsg);
            }
            if (!isNotificationConnectionON(hubst)) {
                yEnterCriticalSection(&hubst->access);
                res = YERRMSGSILENT(hubst->errcode, hubst->errmsg);
                yLeaveCriticalSection(&hubst->access);
                if (!YISERR(res)) {
                    return YERRMSG(YAPI_IO_ERROR, "hub not ready");
                }
                unregisterNetHub(hubst);
                if (res == YAPI_DOUBLE_ACCES) {
                    return YAPI_SUCCESS;
                }
                if (res == YAPI_NOT_SUPPORTED) {
                    if (YSTRNCMP(url, "ws://", 5) == 0) {
                        url += 5;
                        YSPRINTF(urlbuff, 256, "http://%s", url);
                        goto retry;
                    }
                }
                return res;
            }
            yEnterCriticalSection(&yContext->updateDev_cs);
            res = yNetHubEnum(hubst, 1, errmsg);
            yLeaveCriticalSection(&yContext->updateDev_cs);
            if (YISERR(res)) {
                yEnterCriticalSection(&yContext->enum_cs);
                unregisterNetHub(hubst);
                yapiCleanupDisabledHub();
                yLeaveCriticalSection(&yContext->enum_cs);
                if (res == YAPI_DOUBLE_ACCES) {
                    return YAPI_SUCCESS;
                }
            } else if (hubst->url.proto == PROTO_HTTP || hubst->url.proto == PROTO_SECURE_HTTP) {
                // for HTTP test admin pass if the hub require it
                if (hubst->writeProtected && hubst->url.user && strcmp(hubst->url.user, "admin") == 0) {
                    YIOHDL iohdl;
                    const char *request = "GET /api/module/serial?serial=&. ";
                    char *reply = NULL;
                    int replysize = 0;
                    int tmpres = yapiHTTPRequestSyncStartEx_internal(&iohdl, 0, hubst->info.serial, request, YSTRLEN(request), &reply, &replysize, NULL, NULL, errmsg);
                    if (tmpres == YAPI_UNAUTHORIZED) {
                        return tmpres;
                    }
                    if (tmpres == YAPI_SUCCESS) {
                        yapiHTTPRequestSyncDone_internal(&iohdl, errmsg);
                        hubst->rw_access = 1;
                    }
                }
            }

            return res;
        }

    }
    return YAPI_SUCCESS;
}
#ifndef YAPI_IN_YDEVICE
static int yapiGetNextHubRef_internal(int last_ref)
{
    int i, use_next = last_ref < 0;

    yEnterCriticalSection(&yContext->enum_cs);
    for (i = 0; i < NBMAX_NET_HUB; i++) {
        if (yContext->nethub[i] && (yContext->nethub[i]->flags & DISABLED_HUB) == 0) {
            if (use_next) {
                return i;
            }
            if (i == last_ref) {
                use_next = 1;
            }
        }
    }
    yLeaveCriticalSection(&yContext->enum_cs);
    if (use_next && last_ref != NBMAX_NET_HUB && (yContext->detecttype & Y_DETECT_USB)) {
        return NBMAX_NET_HUB;
    }
    return -1;
}

static int yapiGetHubStrAttr_internal(int ref, const char *attrname, char *outval, int buffersize, int *fullsize)
{
    if (ref < 0 || ref > NBMAX_NET_HUB || outval == NULL) {
        return YAPI_INVALID_ARGUMENT;
    }
    if (ref == NBMAX_NET_HUB) {
        if (!(yContext->detecttype & Y_DETECT_USB)) {
            return YAPI_INVALID_ARGUMENT;
        }
        if (YSTRCMP(attrname, "registeredUrl") == 0 || YSTRCMP(attrname, "connectionUrl") == 0 || YSTRCMP(attrname, "knownUrls") == 0) {
            YSTRCPY(outval, buffersize, "usb");
            return 3;
        }
        if (YSTRCMP(attrname, "serialNumber") == 0) {
            *outval = 0;
            return 0;
        }
        if (YSTRCMP(attrname, "errorMessage") == 0) {
            YSTRCPY(outval, buffersize, yContext->usb_global_err_msg);
            return YSTRLEN(yContext->usb_global_err_msg);
        }
    } else {
        HubSt *hub = yContext->nethub[ref];
        if (hub == NULL) {
            return YAPI_INVALID_ARGUMENT;
        }
        if (YSTRCMP(attrname, "registeredUrl") == 0) {
            YSTRCPY(outval, buffersize, hub->url.org_url);
            return YSTRLEN(hub->url.org_url);
        }
        if (YSTRCMP(attrname, "connectionUrl") == 0) {
            return sprintfURL(outval, buffersize, &hub->url, 2);
        }
        if (YSTRCMP(attrname, "knownUrls") == 0) {
            int i;
            *fullsize = YSTRLEN(hub->url.org_url);
            for (i = 0; i < MAX_KNOW_URLS_SIZE; i++) {
                if (hub->know_urls[i]) {
                    *fullsize += 1 + YSTRLEN(hub->know_urls[i]);
                }
            }
            if (*fullsize < buffersize) {
                *outval = 0;
                YSTRCAT(outval, buffersize, hub->url.org_url);
                for (i = 0; i < MAX_KNOW_URLS_SIZE; i++) {
                    if (hub->know_urls[i]) {
                        YSTRCAT(outval, buffersize, "?");
                        YSTRCAT(outval, buffersize, hub->know_urls[i]);
                    }
                }
                return *fullsize;
            }
            return 0;
        }
        if (YSTRCMP(attrname, "serialNumber") == 0) {
            if (hub->info.serial[0]) {
                YSTRCPY(outval, buffersize, hub->info.serial);
                return YSTRLEN(hub->info.serial);
            }
            *outval = 0;
            return 0;
        }
        if (YSTRCMP(attrname, "errorMessage") == 0) {
            YSTRCPY(outval, buffersize, hub->errmsg);
            return YSTRLEN(hub->errmsg);
        }
    }
    *outval = 0;
    return YAPI_NOT_SUPPORTED;
}

static int yapiGetHubIntAttr_internal(int ref, const char *attrname)
{
    if (ref < 0 || ref > NBMAX_NET_HUB) {
        return YAPI_INVALID_ARGUMENT;
    }
    if (ref == NBMAX_NET_HUB) {
        if (YSTRCMP(attrname, "isInUse") == 0 || YSTRCMP(attrname, "isOnline") == 0) {
            return yContext->detecttype & Y_DETECT_USB;
        }
        if (!(yContext->detecttype & Y_DETECT_USB)) {
            return YAPI_INVALID_ARGUMENT;
        }
        if (YSTRCMP(attrname, "errorType") == 0) {
            return yContext->usb_global_err;
        }
        if (YSTRCMP(attrname, "networkTimeout") == 0) {
            return YIO_DEFAULT_USB_TIMEOUT;
        }
        if (YSTRCMP(attrname, "isReadOnly") == 0) {
            return 0;
        }
    } else {
        HubSt *hub = yContext->nethub[ref];
        if (YSTRCMP(attrname, "isInUse") == 0) {
            return hub != 0;
        }
        if (hub == NULL) {
            return YAPI_INVALID_ARGUMENT;
        }
        if (YSTRCMP(attrname, "errorType") == 0) {
            return hub->errcode;
        }
        if (YSTRCMP(attrname, "networkTimeout") == 0) {
            return hub->netTimeout;
        }
        if (YSTRCMP(attrname, "isReadOnly") == 0) {
            return hub->writeProtected && !hub->rw_access;
        }
        if (YSTRCMP(attrname, "isOnline") == 0) {
            return isNotificationConnectionON(hub);
        }
    }
    return YAPI_NOT_SUPPORTED;
}

static int yapiSetHubIntAttr_internal(int ref, const char *attrname, int value)
{
    HubSt *hub;
    if (ref < 0 || ref >= NBMAX_NET_HUB) {
        return YAPI_INVALID_ARGUMENT;
    }
    hub = yContext->nethub[ref];
    if (hub == NULL) {
        return YAPI_INVALID_ARGUMENT;
    }
    if (YSTRCMP(attrname, "networkTimeout") == 0) {
        hub->netTimeout = value;
        return YAPI_SUCCESS;
    }
    return YAPI_NOT_SUPPORTED;
}
#endif
static int pingURLOnhub(HubSt *hubst, const char *request, int mstimeout, char *errmsg)
{
    yJsonStateMachine j;
    u8 *buffer;
    int res;
    yJsonRetCode jstate = YJSON_NEED_INPUT;
    u64 globalTimeout;
    RequestSt *req;
    int replysize;

    globalTimeout = yapiGetTickCount() + mstimeout;

    req = yReqAlloc(hubst);
    if (YISERR((res = yReqOpen(req, 2 * hubst->netTimeout, 0, request, YSTRLEN(request), mstimeout, NULL, NULL, NULL, NULL, errmsg)))) {
        yReqFree(req);
        return res;
    }
    res = (YRETCODE)yReqIsEof(req, errmsg);
    while (res == 0 && globalTimeout > yapiGetTickCount()) {
        res = (YRETCODE)yReqSelect(req, globalTimeout - yapiGetTickCount(), errmsg);
        if (YISERR(res)) {
            yReqClose(req);
            return res;
        }
        res = (YRETCODE)yReqIsEof(req, errmsg);
    }
    if (YISERR(res) && res != YAPI_NO_MORE_DATA) {
        yReqClose(req);
        return res;
    }
    replysize = yReqGet(req, (u8**)&buffer);
    replysize = unpackHTTPRequest(buffer, replysize);
    // init yjson parser
    memset(&j, 0, sizeof(j));
    j.st = YJSON_HTTP_START;
    j.src = (char*)buffer;
    j.end = (char*)buffer + replysize;
    // parse all we can on this buffer
    jstate = yJsonParse(&j);
    while (jstate == YJSON_PARSE_AVAIL) {
        jstate = yJsonParse(&j);
    }
    yReqFree(req);
    switch (jstate) {
    case YJSON_NEED_INPUT:
        return YERRMSG(YAPI_IO_ERROR, "Remote host has close the connection");
    case YJSON_PARSE_AVAIL:
    case YJSON_FAILED:
        return YERRMSG(YAPI_IO_ERROR, "Invalid json data");
    case YJSON_SUCCESS:
        break;
    }
    return YAPI_SUCCESS;
}

static YRETCODE yapiTestHub_internal(const char *url, int mstimeout, char *errmsg)
{
    int freeApi = 0;
    int res;
    if (YSTRICMP(url, "net") == 0) {
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid URL");
    }
    if (!yContext) {
        YPROPERR(yapiInitAPI_internal(0, NULL, NULL, errmsg));
        freeApi = 1;
    }

    if (YSTRICMP(url, "usb") == 0) {
        if (freeApi) {
            yapiFreeAPI_internal();
        }
        res = YAPI_SUCCESS;
    } else {
        HubSt *hubst = yapiAllocHub(url, mstimeout, &res, errmsg);
        if (hubst) {
#ifdef TRACE_NET_HUB
            dbglog("HUB: test %s \n", hubst->url.org_url);
            char buffer[YOCTO_MAX_URL_LEN];
            sprintfURL(buffer, YOCTO_MAX_URL_LEN, &hubst->url, 0);
            dbglog("HUB: decoded url = %s \n", buffer);
#endif
            if (hubst->url.proto != PROTO_HTTP && hubst->url.proto != PROTO_SECURE_HTTP) {
                u64 timeout;
                if (YISERR(res = yStartWakeUpSocket(&hubst->wuce, errmsg))) {
                    yapiFreeHub(hubst);
                    return (YRETCODE)res;
                }
                //yThreadCreate will not create a new thread if there is already one running
                if (yThreadCreateNamed(&hubst->net_thread, hubst->url.host, ws_thread, (void*)hubst) < 0) {
                    yapiFreeHub(hubst);
                    return YERRMSG(YAPI_IO_ERROR, "Unable to start helper thread");
                }
                yDringWakeUpSocket(&hubst->wuce, 1, errmsg);

                // ensure the thread has been able to connect to the hub
                timeout = yapiGetTickCount() + mstimeout;
                while (!isNotificationConnectionON(hubst) && hubst->errcode == YAPI_SUCCESS && timeout > yapiGetTickCount()) {
                    yapiSleep(10, errmsg);
                }
                if (!isNotificationConnectionON(hubst)) {
                    yEnterCriticalSection(&hubst->access);
                    res = YERRMSGSILENT(hubst->errcode, hubst->errmsg);
                    yLeaveCriticalSection(&hubst->access);
                    if (!YISERR(res)) {
                        res = YERRMSG(YAPI_IO_ERROR, "hub not ready");
                    }
                }
                if (!YISERR(res)) {
                    res = pingURLOnhub(hubst, "GET /api/module/firmwareRelease.json \r\n\r\n", (int)(timeout - yapiGetTickCount()), errmsg);
                }
                hubst->state = NET_HUB_TOCLOSE;
                yThreadRequestEnd(&hubst->net_thread);
                yDringWakeUpSocket(&hubst->wuce, 0, errmsg);
                // wait for the helper thread to stop monitoring these devices
                while (yThreadIsRunning(&hubst->net_thread) && (yapiGetTickCount() < timeout)) {
                    yApproximateSleep(10);
                }
                yThreadKill(&hubst->net_thread);
            } else {
                res = pingURLOnhub(hubst, "GET /api/module/firmwareRelease.json \r\n\r\n", mstimeout, errmsg);
            }
            yapiFreeHub(hubst);
        } else {
            if (freeApi) {
                yapiFreeAPI_internal();
            }
            return res;
        }
    }
    if (freeApi) {
        yapiFreeAPI_internal();
    }
    return res;
}


static YRETCODE yapiRegisterHub_internal(const char *url, char *errmsg)
{
    YRETCODE res;
    res = yapiRegisterHubEx(url, 1, errmsg);
    return res;
}

static YRETCODE yapiPreregisterHub_internal(const char *url, char *errmsg)
{
    YRETCODE res;
    res = yapiRegisterHubEx(url, 0, errmsg);
    return res;
}

static void yapiCleanupDisabledHub(void)
{
    int i;
    for (i = 0; i < NBMAX_NET_HUB; i++) {
        if (yContext->nethub[i]) {
            if (yContext->nethub[i]->flags & DISABLED_HUB) {
                dbglog("cleanup disabled hub %i\n", i);
                unregisterNetHub(yContext->nethub[i]);
                yContext->nethub[i] = NULL;
            }
        }
    }
}

static void yapiUnregisterHub_internal(const char *url)
{
    if (!yContext) {
        return;
    }
    if (YSTRICMP(url, "usb") == 0) {
        if (yContext->detecttype & Y_DETECT_USB) {
            yUSBReleaseAllDevices();
            yUsbFree(yContext,NULL);
            yContext->detecttype ^= Y_DETECT_USB;
        }
    } else if (YSTRICMP(url, "net") == 0) {
        if (yContext->detecttype & Y_DETECT_NET) {
            yContext->detecttype ^= Y_DETECT_NET;
        }
    } else {
        // compute a hashed url
        HubSt *hub = yGetHubFromURL(url);
        if (hub == NULL) {
            return;
        }
        //look if we already know this
        yEnterCriticalSection(&yContext->enum_cs);
        unregisterNetHub(hub);
        yapiCleanupDisabledHub();
        yLeaveCriticalSection(&yContext->enum_cs);
    }
}


static YRETCODE yapiUpdateDeviceList_internal(u32 forceupdate, char *errmsg)
{
    int i;
    YRETCODE err = YAPI_SUCCESS;
    char suberr[YOCTO_ERRMSG_LEN];

    if (yContext == NULL)
        return YERR(YAPI_NOT_INITIALIZED);

    if (forceupdate) {
        yEnterCriticalSection(&yContext->updateDev_cs);
    } else {
        // if we do not force an update
        if (!yTryEnterCriticalSection(&yContext->updateDev_cs)) {
            return YAPI_SUCCESS;
        }
    }
    yEnterCriticalSection(&yContext->handleEv_cs);

    if (yContext->detecttype & Y_DETECT_USB) {
        err = yUSBUpdateDeviceList(errmsg);
    }
    yEnterCriticalSection(&yContext->enum_cs);
    yapiCleanupDisabledHub();
    yLeaveCriticalSection(&yContext->enum_cs);

    for (i = 0; i < NBMAX_NET_HUB; i++) {
        if (yContext->nethub[i]) {
            int subres;
            if (YISERR(subres = yNetHubEnum(yContext->nethub[i], forceupdate, suberr)) && err == YAPI_SUCCESS) {
                //keep first generated error
                err = (YRETCODE)subres;
                if (errmsg) {
                    YSPRINTF(errmsg,YOCTO_ERRMSG_LEN, "Enumeration failed for %s:%d (%s)", yContext->nethub[i]->url.host, yContext->nethub[i]->url.portno, suberr);
                }
            }
        }
    }
    yLeaveCriticalSection(&yContext->handleEv_cs);
    yLeaveCriticalSection(&yContext->updateDev_cs);

    return err;
}


static YRETCODE yapiHandleEvents_internal(char *errmsg)
{
    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    // we need only one thread to handle the event at a time
    if (yTryEnterCriticalSection(&yContext->handleEv_cs)) {
        YRETCODE res = (YRETCODE)yUsbIdle();
        yLeaveCriticalSection(&yContext->handleEv_cs);
        return res;
    }
    return YAPI_SUCCESS;
}

static YRETCODE yapiRegisterWakeUpCb_internal(yWakeUpCb callback)
{
    if (!yContext)
        return YAPI_NOT_INITIALIZED;
    yContext->wakeUpCallback = callback;
    return YAPI_SUCCESS;
}

void WakeUpAllSleep(void)
{
    yWakeUpCb cb;
    ySetEvent(&yContext->yapiSleepWakeUpEvent);
    cb = yContext->wakeUpCallback;
    if (cb) {
        cb();
    }
}

static YRETCODE yapiSleep_internal(int ms_duration, char *errmsg)
{
    u64 now, timeout;
    YRETCODE err = YAPI_SUCCESS;


    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    timeout = yapiGetTickCount() + ms_duration;
    do {
        if (err == YAPI_SUCCESS) {
            err = yapiHandleEvents_internal(errmsg);
        }
        now = yapiGetTickCount();
        if (now < timeout) {
            yWaitForEvent(&yContext->yapiSleepWakeUpEvent, (int)(timeout - now));
        }
    } while (yapiGetTickCount() < timeout);

    return err;
}

#ifdef WINDOWS_API
static int tickUseHiRes = -1;
static u64 tickOffset = 0;
static LARGE_INTEGER tickFrequency;
static LARGE_INTEGER tickStart;
#endif
u64 YAPI_FUNCTION_EXPORT yapiGetTickCount(void)
{
    u64 res;

#ifdef WINDOWS_API
    LARGE_INTEGER performanceCounter;

    if (tickUseHiRes < 0) {
        if (QueryPerformanceFrequency(&tickFrequency)) {
            tickUseHiRes = 1;
            tickOffset = (u64)time(NULL) * 1000u;
            QueryPerformanceCounter(&tickStart);
        } else {
            tickUseHiRes = 0;
            tickOffset = (u64)time(NULL) * 1000u - GetTickCount();
        }
        // make sure the offset is always > 0
        if ((s64)tickOffset <= 0) tickOffset = 1;
    }
    if (tickUseHiRes > 0) {
        QueryPerformanceCounter(&performanceCounter);
        performanceCounter.QuadPart -= tickStart.QuadPart;
        // we add +1 because it is not nice to start with zero
        res = performanceCounter.QuadPart * 1000 / tickFrequency.QuadPart + tickOffset;
    } else {
        res = GetTickCount() + tickOffset;
    }
#else
    //get the current number of microseconds since January 1st 1970
    struct timeval tim;
    gettimeofday(&tim, NULL);
    res = (u64)tim.tv_sec * 1000 + (tim.tv_usec / 1000);
#endif

    return res;
}

u32 yapiGetCNonce(u32 nc)
{
    HASH_SUM ctx;
    u32 md5[4];

#ifdef WINDOWS_API
    LARGE_INTEGER performanceCounter;

    if (tickUseHiRes > 0) {
        QueryPerformanceCounter(&performanceCounter);
    } else {
        performanceCounter.QuadPart = GetTickCount();
    }
    MD5Initialize(&ctx);
    MD5AddData(&ctx, (u8*)&performanceCounter, sizeof(performanceCounter));
#else
    //get the current number of microseconds since January 1st 1970
    struct timeval tim;

    gettimeofday(&tim, NULL);
    MD5Initialize(&ctx);
    MD5AddData(&ctx, (u8 *)&tim, sizeof(tim));
#endif
    MD5AddData(&ctx, (u8*)&nc, sizeof(nc));
    MD5Calculate(&ctx, (u8*)md5);

    return md5[1];
}

static int yapiCheckLogicalName_internal(const char *name)
{
    char c;

    if (!name) return 0;
    if (!*name) return 1;
    if (strlen(name) > 19) return 0;
    while ((c = *name++) != 0) {
        if (c < '-') return 0;
        if (c > '-' && c < '0') return 0;
        if (c > '9' && c < 'A') return 0;
        if (c > 'Z' && c < '_') return 0;
        if (c > '_' && c < 'a') return 0;
        if (c > 'z') return 0;
    }
    return 1;
}


static u16 yapiGetAPIVersion_internal(const char **version, const char **apidate)
{
    if (version)
        *version = "2.1.6320";
    if (apidate)
        *apidate = YOCTO_API_BUILD_DATE;

    return YOCTO_API_VERSION_BCD;
}

static void yapiSetTraceFile_internal(const char *file)
{
    if (file != NULL) {
        memset(ytracefile, 0,TRACEFILE_NAMELEN);
        YSTRNCPY(ytracefile, TRACEFILE_NAMELEN-1, file, TRACEFILE_NAMELEN-1);
    } else {
        ytracefile[0] = 0;
    }
}


static YAPI_DEVICE yapiGetDevice_internal(const char *device_str, char *errmsg)
{
    YAPI_DEVICE res;

    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    res = wpSearch(device_str);
    if (res == -1) {
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }

    return res;
}


static int yapiGetAllDevices_internal(YAPI_DEVICE *buffer, int maxsize, int *neededsize, char *errmsg)
{
    int maxdev, nbreturned;
    yBlkHdl devhdl;
    YAPI_DEVICE devdescr;

    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    if (buffer == NULL && neededsize == NULL) {
        return YERR(YAPI_INVALID_ARGUMENT);
    }

    nbreturned = 0;
    if (buffer) {
        // This function walks through the white pages without taking the mutex globally
        // it is only reasonably safe because wpGetAttribute properly handles dangerous devhdl.
        // (same principle as used by HTTPSendRec for the REST API)
        maxdev = 0;
        for (devhdl = yWpListHead; devhdl != INVALID_BLK_HDL; devhdl = yBlkListSeek(devhdl, 1)) {
            devdescr = wpGetAttribute(devhdl, Y_WP_SERIALNUMBER);
            if (devdescr < 0) continue;
            maxdev++;
            if (maxsize >= (int)sizeof(YAPI_DEVICE)) {
                maxsize -= sizeof(YAPI_DEVICE);
                *buffer++ = devdescr;
                nbreturned++;
            }
        }
        if (neededsize) *neededsize = sizeof(YAPI_DEVICE) * maxdev;
    } else {
        if (neededsize) *neededsize = sizeof(YAPI_DEVICE) * wpEntryCount();
    }

    return nbreturned;
}


static YRETCODE yapiGetDeviceInfo_internal(YAPI_DEVICE devdesc, yDeviceSt *infos, char *errmsg)
{
    YUSBDEV devhdl;

    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    if (devdesc < 0 || infos == NULL) {
        return YERR(YAPI_INVALID_ARGUMENT);
    }

    yHashGetStr(devdesc & 0xffff, infos->serial, YOCTO_SERIAL_LEN);
    devhdl = findDevHdlFromStr(infos->serial);
    if (devhdl != INVALID_YHANDLE) {
        // local device, get all information straight from the source
        devHdlInfo(devhdl, infos);
    } else {
        // not a local device, get available information from white pages
        infos->vendorid = YOCTO_VENDORID;
        infos->devrelease = 0;
        infos->nbinbterfaces = 1;
        memcpy((u8*)infos->manufacturer, (u8*)"Yoctopuce", 10);
        memset(infos->firmware, 0, YOCTO_FIRMWARE_LEN);
        if (wpGetDeviceInfo(devdesc, &infos->deviceid, infos->productname, infos->serial, infos->logicalname, &infos->beacon) < 0) {
            return YERR(YAPI_DEVICE_NOT_FOUND);
        }
    }

    return YAPI_SUCCESS;
}

static YRETCODE yapiGetDevicePath_internal(YAPI_DEVICE devdesc, char *rootdevice, char *request, int requestsize, int *neededsize, char *errmsg)
{
    YRETCODE res;

    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    if (rootdevice == NULL && request == NULL && neededsize == NULL) {
        return YERR(YAPI_INVALID_ARGUMENT);
    }
    res = (YRETCODE)ywpGetDeviceUrl(devdesc, rootdevice, request, requestsize, neededsize);
    if (neededsize) *neededsize += 4;
    if (res < 0) {
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }
    return res;
}


static YRETCODE yapiGetDevicePathEx_internal(const char *serial, char *rootdevice, char *request, int requestsize, int *neededsize, char *errmsg)
{
    YAPI_DEVICE devdescr;
    char buffer[512];
    HubSt *hub;

    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    if (rootdevice == NULL && request == NULL && neededsize == NULL) {
        return YERR(YAPI_INVALID_ARGUMENT);
    }
    devdescr = wpSearch(serial);
    if (YISERR(devdescr)) {
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }
    hub = ywpGetDeviceHub((yStrRef)devdescr);
    if (hub == FAKE_USB_HUB) {
        if (rootdevice) {
            *rootdevice = 0;
        }
        if (request && requestsize > 4) {
            YSTRCPY(request, requestsize, "usb");
        }
        if (*neededsize) {
            *neededsize = 4;
        }
    } else {
        ywpGetDeviceUrl(devdescr, rootdevice, buffer, 512, neededsize);
        if (request) {
            int len;
            const char *prefix;
            switch (hub->url.proto) {
            case PROTO_HTTP:
                prefix = "http";
                break;
            case PROTO_SECURE_HTTP:
                prefix = "https";
                break;
            case PROTO_SECURE_WEBSOCKET:
                prefix = "wss";
                break;
            default:
                prefix = "ws";
                break;
            }
            len = YSPRINTF(request, requestsize, "%s://%s:%d%s%s", prefix, hub->url.host, hub->url.portno, hub->url.subdomain, buffer);
            *neededsize = len + 1;
        }
        if (rootdevice && YSTRCMP(rootdevice, serial) == 0) {
            *rootdevice = 0;
        }
    }
    return YAPI_SUCCESS;
}


static YAPI_FUNCTION yapiGetFunction_internal(const char *class_str, const char *function_str, char *errmsg)
{
    YAPI_FUNCTION res;

    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    res = ypSearch(class_str, function_str);
    if (res < 0) {
        if (res == -2) {
            return YERRMSG(YAPI_DEVICE_NOT_FOUND, "No function of that class");
        }
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }
    return res;
}

static int yapiGetFunctionsByClass_internal(const char *class_str, YAPI_FUNCTION prevfundesc,
                                            YAPI_FUNCTION *buffer, int maxsize, int *neededsize, char *errmsg)
{
    int res;
    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    if (buffer == NULL && neededsize == NULL) {
        return YERR(YAPI_INVALID_ARGUMENT);
    }

    res = ypGetFunctions(class_str, -1, prevfundesc, buffer, maxsize, neededsize);
    if (res < 0) {
        return YERR(YAPI_DEVICE_NOT_FOUND); // prevfundesc is invalid or has been unplugged, restart enum
    }

    return res;
}

static int yapiGetFunctionsByDevice_internal(YAPI_DEVICE devdesc, YAPI_FUNCTION prevfundesc,
                                             YAPI_FUNCTION *buffer, int maxsize, int *neededsize, char *errmsg)
{
    int res;
    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    if (buffer == NULL && neededsize == NULL) {
        return YERR(YAPI_INVALID_ARGUMENT);
    }
    res = ypGetFunctions(NULL, devdesc, prevfundesc, buffer, maxsize, neededsize);
    if (res < 0) {
        return YERR(YAPI_DEVICE_NOT_FOUND); // prevfundesc is invalid or has been unplugged, restart enum
    }
    return res;
}


static YRETCODE yapiGetFunctionInfoEx_internal(YAPI_FUNCTION fundesc, YAPI_DEVICE *devdesc, char *serial, char *funcId, char *baseType, char *funcName, char *funcVal, char *errmsg)
{
    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    if (ypGetFunctionInfo(fundesc, serial, funcId, baseType, funcName, funcVal) < 0) {
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }
    if (devdesc)
        *devdesc = fundesc & 0xffff;

    return YAPI_SUCCESS;
}


static int yapiRequestOpenUSB(YIOHDL_internal *iohdl, HubSt *hub, YAPI_DEVICE dev, const char *request, int reqlen, u64 unused_timeout, yapiRequestAsyncCallback callback, void *context, char *errmsg)
{
    char buffer[512];
    YRETCODE res;
    int firsttime = 1;
    u64 timeout;

    yHashGetStr(dev & 0xffff, buffer, YOCTO_SERIAL_LEN);
    timeout = yapiGetTickCount() + YAPI_BLOCKING_USBOPEN_REQUEST_TIMEOUT;
    do {
        res = (YRETCODE)yUsbOpen(iohdl, buffer, errmsg);
        if (res != YAPI_DEVICE_BUSY) break;
        yapiHandleEvents_internal(errmsg);
        if (firsttime) {
            //yApproximateSleep(1);
            firsttime = 0;
        }
    } while (yapiGetTickCount() < timeout);

    if (res != YAPI_SUCCESS) {
        return res;
    }
    if (reqlen >= 10 && reqlen <= (int)sizeof(buffer) && request[0] == 'G' && request[1] == 'E' && request[2] == 'T' && request[3] == ' ') {
        int i = 4;
        const char *p;
        char *d;
        memcpy(buffer, request, 4);
        p = request + 4;
        d = buffer + 4;
        while (i < reqlen && *p != ' ' && *p != '\r') {
            *d++ = *p++;
            i++;
        }
        if (*(d - 2) == '&' && *(d - 1) == '.') {
            // remove short sign
            d -= 2;
            i -= 2;
        }
        if (i + 5 < sizeof(buffer)) {
            memcpy(d, " \r\n\r\n", 5);
            i += 5;
        }
        reqlen = i;
        request = buffer;
    }
    res = (YRETCODE)yUsbWrite(iohdl, request, reqlen, errmsg);
    if (YISERR(res)) {
        yUsbClose(iohdl, errmsg);
        return res;
    }
    if (callback) {
        res = (YRETCODE)yUsbSetIOAsync(iohdl, callback, context, errmsg);
        if (YISERR(res)) {
            yUsbClose(iohdl, errmsg);
            return res;
        }
    }
    return res;
}


static int yapiRequestOpenHTTP(YIOHDL_internal *iohdl, HubSt *hub, YAPI_DEVICE dev, const char *request, int reqlen, int wait_for_start, u64 mstimeout, yapiRequestAsyncCallback callback, void *context, char *errmsg)
{
    YRETCODE res;
    int devydx;
    RequestSt *tcpreq;

    devydx = wpGetDevYdx((yStrRef)dev);
    if (devydx < 0) {
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }
    yEnterCriticalSection(&yContext->io_cs);
    tcpreq = yContext->tcpreq[devydx];
    if (tcpreq == NULL) {
        tcpreq = yReqAlloc(hub);
        yContext->tcpreq[devydx] = tcpreq;
    }
    yLeaveCriticalSection(&yContext->io_cs);
    if (callback) {
        if (tcpreq->hub->writeProtected) {
            // no need to take the critical section tcpreq->hub->http.authAccess since we only read user an pass
            if (!tcpreq->hub->url.user || strcmp(tcpreq->hub->url.user, "admin") != 0) {
                return YERRMSG(YAPI_UNAUTHORIZED, "Access denied: admin credentials required");
            }
        }
    }
    if ((tcpreq->hub->send_ping || !tcpreq->hub->mandatory) && !isNotificationConnectionON(hub)) {
        if (errmsg) {
            YSPRINTF(errmsg, YOCTO_ERRMSG_LEN, "hub %s is not reachable", tcpreq->hub->url.host);
        }
        return YAPI_IO_ERROR;
    }

    res = (YRETCODE)yReqOpen(tcpreq, wait_for_start, 0, request, reqlen, mstimeout, callback, context, NULL, NULL, errmsg);
    if (res != YAPI_SUCCESS) {
        return res;
    }

    if (callback) {
        res = (YRETCODE)yDringWakeUpSocket(&tcpreq->hub->wuce, 2, errmsg);
        if (res != YAPI_SUCCESS) {
            return res;
        }
    }
    iohdl->tcpreqidx = devydx;
    iohdl->type = YIO_TCP;
    return YAPI_SUCCESS;
}

static int yapiRequestOpenWS(YIOHDL_internal *iohdl, HubSt *hub, YAPI_DEVICE dev, int tcpchan, const char *request, int reqlen, u64 mstimeout, yapiRequestAsyncCallback callback, void *context, RequestProgress progress_cb, void *progress_ctx, char *errmsg)
{
    YRETCODE res;
    int devydx;
    RequestSt *req;

    devydx = wpGetDevYdx((yStrRef)dev);
    if (devydx < 0) {
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }

    //dbglog("yapiRequestOpenWS on %p %s\n", hub, callback ? "ASYNC": "");
    if (callback) {
        if (hub->writeProtected && !hub->rw_access) {
            return YERRMSG(YAPI_UNAUTHORIZED, "Access denied: admin credentials required");
        }
    }
    req = yReqAlloc(hub);
    if ((req->hub->send_ping || !req->hub->mandatory) && !isNotificationConnectionON(req->hub)) {
        if (errmsg) {
            YSPRINTF(errmsg, YOCTO_ERRMSG_LEN, "hub %s is not reachable", req->hub->url.host);
        }
        return YAPI_IO_ERROR;
    }

    if (!isNotificationConnectionON(req->hub)) {
        if (YISERR(req->hub->errcode)) {
            yEnterCriticalSection(&req->hub->access);
            res = YERRMSG(req->hub->errcode, req->hub->errmsg);
            yLeaveCriticalSection(&req->hub->access);
            return res;
        }
        if (errmsg) {
            YSPRINTF(errmsg, YOCTO_ERRMSG_LEN, "hub %s is not ready", req->hub->url.host);
        }
        return YERRMSG(YAPI_TIMEOUT, "hub is not ready");
    }

    res = (YRETCODE)yReqOpen(req, 2 * hub->netTimeout, tcpchan, request, reqlen, mstimeout, callback, context, progress_cb, progress_ctx, errmsg);
    if (res != YAPI_SUCCESS) {
        return res;
    }

    iohdl->ws = req;
    iohdl->type = YIO_WS;
    return YAPI_SUCCESS;
}


YRETCODE yapiRequestOpen(YIOHDL_internal *iohdl, int tcpchan, const char *device, const char *request, int reqlen, yapiRequestAsyncCallback callback, void *context, yapiRequestProgressCallback progress_cb, void *progress_ctx, char *errmsg)
{
    YAPI_DEVICE dev;
    int len;
    u64 mstimeout;
    HubSt *hub;

    if (!yContext) {
        return YERR(YAPI_NOT_INITIALIZED);
    }

    dev = wpSearch(device);
    if (dev == -1) {
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }
    // dispatch request on correct hub (or pseudo usb HUB)
    hub = ywpGetDeviceHub((yStrRef)dev);
    if (hub == FAKE_USB_HUB) {
        mstimeout = YIO_DEFAULT_USB_TIMEOUT;
    } else {
        mstimeout = hub->netTimeout;
    }
    YASSERT(iohdl != NULL, 0);
    memset(iohdl, 0, sizeof(YIOHDL_internal));
    // compute request timeout
    len = (reqlen < YOCTO_SERIAL_LEN + 32 ? reqlen : YOCTO_SERIAL_LEN + 32);
    if (memcmp(request, "GET ", 4) == 0) {
        if (ymemfind((u8*)request + 4, len, (u8*)"/@YCB+", 6) >= 0) {
            return YERRMSG(YAPI_NOT_SUPPORTED, "Preloading of URL is only supported for HTTP callback.");
        }
        if (ymemfind((u8*)request + 4, len, (u8*)"/testcb.txt", 11) >= 0) {
            mstimeout = YIO_1_MINUTE_TCP_TIMEOUT;
        } else if (ymemfind((u8*)request + 4, len, (u8*)"/logger.json", 12) >= 0) {
            mstimeout = YIO_1_MINUTE_TCP_TIMEOUT;
        } else if (ymemfind((u8*)request + 4, len, (u8*)"/rxmsg.json", 11) >= 0) {
            mstimeout = YIO_1_MINUTE_TCP_TIMEOUT;
        } else if (ymemfind((u8*)request + 4, len, (u8*)"/rxdata.bin", 11) >= 0) {
            mstimeout = YIO_1_MINUTE_TCP_TIMEOUT;
        } else if (ymemfind((u8*)request + 4, len, (u8*)"/at.txt", 7) >= 0) {
            mstimeout = YIO_1_MINUTE_TCP_TIMEOUT;
        } else if (ymemfind((u8*)request + 4, len, (u8*)"/files.json", 11) >= 0) {
            mstimeout = YIO_1_MINUTE_TCP_TIMEOUT;
        } else if (ymemfind((u8*)request + 4, len, (u8*)"/flash.json", 11) >= 0) {
            mstimeout = YIO_10_MINUTES_TCP_TIMEOUT;
        } else if (ymemfind((u8*)request + 4, len, (u8*)"/Yv4wI.js", 9) >= 0) {
            mstimeout = YIO_10_MINUTES_TCP_TIMEOUT;
        }
    } else {
        if (ymemfind((u8*)request + 4, len, (u8*)"/upload.html", 12) >= 0) {
            //fixme: use 1 minute timeout for WS
            mstimeout = YIO_10_MINUTES_TCP_TIMEOUT;
        }
    }
    if (hub == FAKE_USB_HUB) {
        return yapiRequestOpenUSB(iohdl, NULL, dev, request, reqlen, mstimeout, callback, context, errmsg);
    } else {
        if (hub->url.proto == PROTO_WEBSOCKET || hub->url.proto == PROTO_SECURE_WEBSOCKET) {
            return yapiRequestOpenWS(iohdl, hub, dev, tcpchan, request, reqlen, mstimeout, callback, context, progress_cb, progress_ctx, errmsg);
        } else if (hub->url.proto == PROTO_HTTP || hub->url.proto == PROTO_SECURE_HTTP) {
            return yapiRequestOpenHTTP(iohdl, hub, dev, request, reqlen, 2 * hub->netTimeout, mstimeout, callback, context, errmsg);
        } else {
            return YERRMSG(YAPI_NOT_SUPPORTED, "Unsuported hub protocol");
        }
    }
}

static int yapiRequestWaitEndUSB(YIOHDL_internal *iohdl, char **reply, int *replysize, char *errmsg)
{
    u64 timeout;
    yPrivDeviceSt *p;
    int buffpos = 0;
    int res;

    timeout = yapiGetTickCount() + YAPI_BLOCKING_USBREAD_REQUEST_TIMEOUT;
    p = findDevFromIOHdl(iohdl);
    if (p == NULL) {
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }
    if (p->replybuf == NULL) {
        p->replybufsize = 2048;
        p->replybuf = (char*)yMalloc(p->replybufsize);
    }
    while ((res = (YRETCODE)yUsbEOF(iohdl, errmsg)) == 0) {
        if (yapiGetTickCount() > timeout) {
            yUsbClose(iohdl, NULL);
            return YERRMSG(YAPI_TIMEOUT, "Timeout during device request");
        }
        if (buffpos + 256 > p->replybufsize) {
            char *newbuff;
            p->replybufsize <<= 1;
            newbuff = (char*)yMalloc(p->replybufsize);
            memcpy(newbuff, p->replybuf, buffpos);
            yFree(p->replybuf);
            p->replybuf = newbuff;
        }
        res = (YRETCODE)yUsbReadBlock(iohdl, p->replybuf + buffpos, p->replybufsize - buffpos, timeout, errmsg);
        if (YISERR(res)) {
            yUsbClose(iohdl, NULL);
            return res;
        } else if (res > 0) {
            timeout = yapiGetTickCount() + YAPI_BLOCKING_USBREAD_REQUEST_TIMEOUT;
        }
        buffpos += res;
    }
    *reply = p->replybuf;
    *replysize = buffpos;
    return res;
}


static int yapiRequestWaitEndHTTP(YIOHDL_internal *iohdl, char **reply, int *replysize, char *errmsg)
{
    int res;
    RequestSt *tcpreq = yContext->tcpreq[iohdl->tcpreqidx];

    res = (YRETCODE)yReqIsEof(tcpreq, errmsg);
    while (res == 0) {
        res = (YRETCODE)yReqSelect(tcpreq, 1000, errmsg);
        if (YISERR(res)) {
            yReqClose(tcpreq);
            return res;
        }
        res = (YRETCODE)yReqIsEof(tcpreq, errmsg);
    }
    if (YISERR(res) && res != YAPI_NO_MORE_DATA) {
        yReqClose(tcpreq);
        return res;
    }
    *replysize = yReqGet(tcpreq, (u8**)reply);
    return YAPI_SUCCESS;
}


static int yapiRequestWaitEndWS(YIOHDL_internal *iohdl, char **reply, int *replysize, char *errmsg)
{
    int res;
    RequestSt *tcpreq = iohdl->ws;

    res = (YRETCODE)yReqIsEof(tcpreq, errmsg);
    while (res == 0) {
        res = (YRETCODE)yReqSelect(tcpreq, 1000, errmsg);
        if (YISERR(res)) {
            yReqClose(tcpreq);
            return res;
        }
        res = (YRETCODE)yReqIsEof(tcpreq, errmsg);
    }
    if (YISERR(res) && res != YAPI_NO_MORE_DATA) {
        yReqClose(tcpreq);
        return res;
    }
    *replysize = yReqGet(tcpreq, (u8**)reply);
    return YAPI_SUCCESS;
}


static void yapiRequestClose(YIOHDL_internal *arg)
{

    if (arg->type == YIO_USB) {
        yUsbClose(arg, NULL);
    } else if (arg->type == YIO_TCP) {
        RequestSt *tcpreq = yContext->tcpreq[arg->tcpreqidx];
        yReqClose(tcpreq);
    } else {
        yReqClose(arg->ws);
        yReqFree(arg->ws);
    }
}

YRETCODE yapiHTTPRequestSyncStartEx_internal(YIOHDL *iohdl, int tcpchan, const char *device, const char *request, int requestsize, char **reply, int *replysize, yapiRequestProgressCallback progress_cb, void *progress_ctx, char *errmsg)
{
    YRETCODE res;
    YIOHDL_internal *internalio;
#ifdef DEBUG_YAPI_REQ
    int req_count = YREQ_LOG_START("SyncStartEx", device, request, requestsize);
    u64 start_tm = yapiGetTickCount();
#endif


    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);


    *reply = NULL;
    internalio = yMalloc(sizeof(YIOHDL_internal));
    memset((u8*)iohdl, 0, YIOHDL_SIZE);
    if (YISERR(res = yapiRequestOpen(internalio, tcpchan, device, request, requestsize, NULL, NULL, progress_cb, progress_ctx, errmsg))) {
        yFree(internalio);
    } else {

        if (internalio->type == YIO_USB) {
            res = yapiRequestWaitEndUSB(internalio, reply, replysize, errmsg);
        } else if (internalio->type == YIO_TCP) {
            res = yapiRequestWaitEndHTTP(internalio, reply, replysize, errmsg);
            *replysize = unpackHTTPRequest((u8*)(*reply), *replysize);
        } else if (internalio->type == YIO_WS) {
            res = yapiRequestWaitEndWS(internalio, reply, replysize, errmsg);
        } else {
            yFree(internalio);
            return YERR(YAPI_INVALID_ARGUMENT);
        }
        if (res == YAPI_SUCCESS) {
            int pos;
            if (*replysize == 0) {
                yapiRequestClose(internalio);
                yFree(internalio);
                return YERRMSG(YAPI_IO_ERROR, "Returned an empty HTTP response");
            }
            pos = ymemfind((u8*)(*reply), *replysize, (u8*)"\r\n\r\n", 4) < 0;
            if (pos < 0) {
                yapiRequestClose(internalio);
                yFree(internalio);
                return YERRMSG(YAPI_IO_ERROR, "Invalid HTTP header");
            }
        }
        yEnterCriticalSection(&yContext->io_cs);
        *iohdl = internalio;
        internalio->next = yContext->yiohdl_first;
        yContext->yiohdl_first = internalio;
        yLeaveCriticalSection(&yContext->io_cs);
    }
#ifdef DEBUG_YAPI_REQ
    if (res < 0) {
        YREQ_LOG_APPEND_ERR(req_count, "SyncStartEx", errmsg, res, start_tm);
    } else {
        YREQ_LOG_APPEND(req_count, "SyncStartEx", *reply, *replysize, start_tm);
    }
#endif

    return res;
}


YRETCODE yapiHTTPRequestSyncDone_internal(YIOHDL *iohdl, char *errmsg)
{
    YIOHDL_internal *r, *p, *arg = *iohdl;
    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    if (iohdl == NULL)
        return YERR(YAPI_INVALID_ARGUMENT);


    yEnterCriticalSection(&yContext->io_cs);
    r = yContext->yiohdl_first;
    p = NULL;
    while (r && r != arg) {
        p = r;
        r = r->next;
    }
    if (r == NULL || r != arg) {
        yLeaveCriticalSection(&yContext->io_cs);
        return YERR(YAPI_INVALID_ARGUMENT);
    }
    if (p == NULL) {
        yContext->yiohdl_first = r->next;
    } else {
        p->next = r->next;
    }
    yLeaveCriticalSection(&yContext->io_cs);

    yapiRequestClose(arg);
    yFree(arg);
    memset((u8*)iohdl, 0, YIOHDL_SIZE);
    return YAPI_SUCCESS;
}


static void asyncDrop(void *context, const u8 *result, u32 resultlen, int retcode, const char *errmsg)
{
#ifdef DEBUG_YAPI_REQ
    int req_count = (int)(((u8*)context) - ((u8*)NULL));
    YREQ_LOG_APPEND(req_count, "ASync", result, resultlen, 0);
#endif
}


static YRETCODE yapiHTTPRequestAsyncEx_internal(int tcpchan, const char *device, const char *request, int len, yapiRequestAsyncCallback callback, void *context, char *errmsg)
{
    YIOHDL_internal iohdl;
    YRETCODE res;
    int retryCount = 1;
#ifdef DEBUG_YAPI_REQ
    int yreq_count = YREQ_LOG_START("ASync", device, request, len);
#endif

    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    do {
        if (callback == NULL) {
            callback = asyncDrop;
#ifdef DEBUG_YAPI_REQ
            context = ((u8*)NULL) + yreq_count;
#endif
        }
        res = yapiRequestOpen(&iohdl, tcpchan, device, request, len, callback, context, NULL,NULL, errmsg);
        if (YISERR(res)) {
            if (res == YAPI_UNAUTHORIZED) {
                return res;
            }

            if (retryCount) {
                char suberr[YOCTO_ERRMSG_LEN];
                dbglog("ASync request for %s failed. Retrying after yapiUpdateDeviceList\n", device);
                // do not force the update device list. Otherwise this can lead to a deadlock when device reboot or is replugeed.
                // ex : thread 1 UpdateDevicelist -> CB -> any call that use YDevice object
                //      thread 2 Any request that use YDevice obj -> error->  UpdateDeviceList -> DeadLock
                if (YISERR(yapiUpdateDeviceList_internal(0, suberr))) {
                    dbglog("yapiUpdateDeviceList failled too with %s\n", errmsg);
                    return res;
                }
            }
        }
    } while (res != YAPI_SUCCESS && retryCount--);

    return res;
}


static int yapiHTTPRequest_internal(const char *device, const char *request, char *buffer, int buffsize, int *fullsize, char *errmsg)
{
    YIOHDL iohdl;
    char *reply = NULL;
    int replysize = 0;

    if (!buffer || buffsize < 4) {
        return YERR(YAPI_INVALID_ARGUMENT);
    }
    YPROPERR(yapiHTTPRequestSyncStartEx_internal(&iohdl, 0, device, request, YSTRLEN(request), &reply, &replysize, NULL, NULL, errmsg));

    if (fullsize)
        *fullsize = replysize;

    if (replysize > buffsize - 1) {
        replysize = buffsize - 1;
    }
    memcpy(buffer, reply, replysize);
    buffer[replysize] = 0;
    YPROPERR(yapiHTTPRequestSyncDone_internal(&iohdl, errmsg));
    return replysize;
}

static void yapiRegisterHubDiscoveryCallback_internal(yapiHubDiscoveryCallback hubDiscoveryCallback)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    if (!yContext) {
        yapiInitAPI_internal(0, NULL, NULL, errmsg);
    }
    if (yContext) {
        yContext->hubDiscoveryCallback = hubDiscoveryCallback;
    }
}

static YRETCODE yapiTriggerHubDiscovery_internal(char *errmsg)
{
    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    // ensure SSDP thread is started
    YPROPERR(ySSDPStart(&yContext->SSDP,ssdpEntryUpdate,errmsg));
    // triger SSDP discovery
    return (YRETCODE)ySSDPDiscover(&yContext->SSDP, errmsg);
}

// used only by VirtualHub
YRETCODE yapiGetBootloadersDevs(char *serials, unsigned int maxNbSerial, unsigned int *totalBootladers, char *errmsg)
{
    int nbifaces = 0;
    yInterfaceSt *iface;
    yInterfaceSt *runifaces = NULL;
    int i;
    u32 totalBoot, copyedBoot;
    char *s = serials;
    YRETCODE res;

    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    if ((yContext->detecttype & Y_DETECT_USB) == 0) {
        return YERRMSG(YAPI_INVALID_ARGUMENT, "You must init the yAPI with Y_DETECT_USB flag");
    }

    if (YISERR(res = (YRETCODE) yyyUSBGetInterfaces(&runifaces,&nbifaces,errmsg))) {
        return res;
    }

    totalBoot = copyedBoot = 0;

    for (i = 0, iface = runifaces; i < nbifaces; i++, iface++) {
        if (iface->deviceid != YOCTO_DEVID_BOOTLOADER)
            continue;
        if (serials && copyedBoot < maxNbSerial) {
            YSTRCPY(s, YOCTO_SERIAL_LEN*2, iface->serial);
            s += YOCTO_SERIAL_LEN;
            copyedBoot++;
        }
        totalBoot++;
    }
    // free all tmp ifaces
    if (runifaces) {
        yFree(runifaces);
    }
    if (totalBootladers)
        *totalBootladers = totalBoot;

    return (YRETCODE)copyedBoot;
}

//used by API
static YRETCODE yapiGetBootloaders_internal(char *buffer, int buffersize, int *fullsize, char *errmsg)
{
    int i;
    char *p = buffer;
    YRETCODE res;
    int size, total, len;

    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    if (buffer == NULL || buffersize < 1)
        return YERR(YAPI_INVALID_ARGUMENT);

    buffersize--; // reserve space for \0

    size = total = 0;

    if (yContext->detecttype & Y_DETECT_USB) {
        int nbifaces = 0;
        yInterfaceSt *iface;
        yInterfaceSt *runifaces = NULL;

        if (YISERR(res = (YRETCODE)yyyUSBGetInterfaces(&runifaces, &nbifaces, errmsg))) {
            return res;
        }

        for (i = 0, iface = runifaces; i < nbifaces; i++, iface++) {
            if (iface->deviceid != YOCTO_DEVID_BOOTLOADER)
                continue;

            if (buffer && size < buffersize && buffer != p) {
                *p++ = ',';
                size++;
            }

            len = YSTRLEN(iface->serial);
            total += len;
            if (buffer && size + len < buffersize) {
                YSTRCPY(p, buffersize - size, iface->serial);
                p += len;
                size += len;
            }
        }
        // free all tmp ifaces
        if (runifaces) {
            yFree(runifaces);
        }

    }


    for (i = 0; i < NBMAX_NET_HUB; i++) {
        if (yContext->nethub[i]) {
            char bootloaders[4 * YOCTO_SERIAL_LEN];
            int j;
            char *serial;
            res = yNetHubGetBootloaders(yContext->nethub[i]->info.serial, bootloaders, errmsg);
            if (YISERR(res)) {
                return res;
            }
            for (j = 0, serial = bootloaders; j < res; j++, serial += YOCTO_SERIAL_LEN) {
                if (buffer && size < buffersize && buffer != p) {
                    *p++ = ',';
                    size++;
                }

                len = YSTRLEN(serial);
                total += len;
                if (buffer && size + len < buffersize) {
                    YSTRCPY(p, buffersize - size, serial);
                    p += len;
                    size += len;
                }
            }
        }
    }

    //ensure buffer is null terminated;
    *p = 0;
    if (fullsize)
        *fullsize = total;

    return (YRETCODE)size;
}

static int yapiIsModuleWritable_internal(const char *serial, char *errmsg)
{
    YAPI_DEVICE devdescr;
    HubSt *hub;

    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    devdescr = wpSearch(serial);
    if (YISERR(devdescr)) {
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }

    // dispatch request on correct hub (or pseudo usb HUB)
    hub = ywpGetDeviceHub((yStrRef)devdescr);
    if (hub) {
        return 1;
    } else {
        if (hub->writeProtected && !hub->rw_access) {
            return 0;
        } else {
            return 1;
        }
    }
}

#ifndef YAPI_IN_YDEVICE

static int yapiGetSubdevices_internal(const char *serial, char *buffer, int buffersize, int *fullsize, char *errmsg)
{
    int i;
    char *p = buffer;
    int size, total;

    if (!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    if (buffer == NULL || buffersize < 1)
        return YERR(YAPI_INVALID_ARGUMENT);

    buffersize--; // reserve space for \0
    size = total = 0;
    for (i = 0; i < NBMAX_NET_HUB; i++) {
        if (yContext->nethub[i] == NULL)
            continue;
        if (YSTRCMP(serial, yContext->nethub[i]->info.serial) == 0) {
            yStrRef knownDevices[128];
            int j, nbKnownDevices;
            nbKnownDevices = ywpGetAllDevUsingNetHub(yContext->nethub[i], knownDevices, 128);
            total = nbKnownDevices * YOCTO_SERIAL_LEN + nbKnownDevices;
            if (buffersize > total) {
                int isfirst = 1;
                for (j = 0; j < nbKnownDevices; j++) {
                    if (knownDevices[j] == yContext->nethub[i]->serial_hash)
                        continue;
                    if (!isfirst)
                        *p++ = ',';
                    yHashGetStr(knownDevices[j], p, YOCTO_SERIAL_LEN);
                    p += YSTRLEN(p);
                    isfirst = 0;
                }
            }
            break;
        }
    }

    //ensure buffer is null terminated;
    size = (int)(p - buffer);
    *p++ = 0;
    if (fullsize)
        *fullsize = total;

    return size;
}


static const char* yapiJsonValueParseArray(yJsonStateMachine *j, const char *path, int *result, char *errmsg);


static void skipJsonStruct(yJsonStateMachine *j)
{
#ifdef DEBUG_JSON_PARSE
    dbglog("skip  %s(%d):%s\n", yJsonStateStr[j->st], j->st, j->token);
#endif
    yJsonParse(j);
    do {
#ifdef DEBUG_JSON_PARSE
        dbglog("... %s(%d):%s\n", yJsonStateStr[j->st], j->st, j->token);
#endif
        yJsonSkip(j, 1);
    } while (yJsonParse(j) == YJSON_PARSE_AVAIL && j->st != YJSON_PARSE_STRUCT);
}


static void skipJsonArray(yJsonStateMachine *j)
{
    int continue_to_parse;
#ifdef DEBUG_JSON_PARSE
    dbglog("skip  %s(%d):%s depth=%d\n", yJsonStateStr[j->st], j->st, j->token, j->depth);
#endif
    int depth = j->depth;
    int tmp;
    do {
#ifdef DEBUG_JSON_PARSE
        dbglog("... %s(%d):%s\n", yJsonStateStr[j->st], j->st, j->token);
#endif
        yJsonSkip(j, 1);
        tmp = yJsonParse(j);
        continue_to_parse = 0;
        if (tmp == YJSON_PARSE_AVAIL && (j->st != YJSON_PARSE_ARRAY || j->depth > depth)) {
            continue_to_parse = 1;
        }
#ifdef DEBUG_JSON_PARSE
        dbglog("... %s(%d):%s depth=%d\n", yJsonStateStr[j->st], j->st, j->token, j->depth);
#endif
    } while (continue_to_parse);
}


static const char* yapiJsonValueParseStruct(yJsonStateMachine *j, const char *path, int *result, char *errmsg)
{
    const char *p = path;
    char buffer[64];
    char *d = buffer;

    while (*p && *p != '|' && (d - buffer <= 63)) {
        *d++ = *p++;
    }
    *d = 0;

    while (yJsonParse(j) == YJSON_PARSE_AVAIL) {
        if (j->st == YJSON_PARSE_MEMBNAME) {
            if (YSTRCMP(buffer, j->token) == 0) {
                if (*p) {
#ifdef DEBUG_JSON_PARSE
                    dbglog("recurse %s %s(%d):%s\n", j->token, yJsonStateStr[j->st], j->st, j->token);
#endif
                    yJsonParse(j);
                    if (j->st == YJSON_PARSE_STRUCT) {
                        return yapiJsonValueParseStruct(j, ++p, result, errmsg);
                    } else if (j->st == YJSON_PARSE_ARRAY) {
                        return yapiJsonValueParseArray(j, ++p, result, errmsg);
                    } else {
                        *result = YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid JSON struct");
                        return "";
                    }
                } else {
                    const char *start_of_json;
#ifdef DEBUG_JSON_PARSE
                    dbglog("found %s %s(%d):%s\n", j->token, yJsonStateStr[j->st], j->st, j->token);
#endif
                    yJsonParse(j);
                    start_of_json = j->state_start;
                    switch (j->st) {
                    case YJSON_PARSE_STRING:
                        while (j->next == YJSON_PARSE_STRINGCONT) {
                            yJsonParse(j);
                        }
                    case YJSON_PARSE_NUM:
                        *result = (u32)(j->state_end - start_of_json);
                        return start_of_json;
                    case YJSON_PARSE_STRUCT:
                        skipJsonStruct(j);
                        *result = (u32)(j->state_end - start_of_json);
                        return start_of_json;
                    case YJSON_PARSE_ARRAY:
                        skipJsonArray(j);
                        *result = (u32)(j->state_end - start_of_json);
                        return start_of_json;
                    default:
                        *result = YERRMSG(YAPI_INVALID_ARGUMENT, "Only String and numerical target are supported");
                        return "";
                    }
                }
            } else {
#ifdef DEBUG_JSON_PARSE
                dbglog("skip %s %s(%d):%s\n", j->token, yJsonStateStr[j->st], j->st, j->token);
#endif
                yJsonSkip(j, 1);
            }
        }
#ifdef DEBUG_JSON_PARSE
        else {
            dbglog("%s(%d):%s\n", yJsonStateStr[j->st], j->st, j->token);
        }
#endif
    }
    *result = YERRMSG(YAPI_INVALID_ARGUMENT, "Path not found");
    return "";
}


static const char* yapiJsonValueParseArray(yJsonStateMachine *j, const char *path, int *result, char *errmsg)
{
    int len = 0;
    const char *p = path;
    char buffer[16];
    int index, count = 0;
    yJsonState array_type;

    while (*p && *p != '|') {
        p++;
        len++;
    }
    YASSERT(len < 16, len);
    memcpy(buffer, path, len);
    buffer[len] = 0;
    index = atoi(buffer);

    if (yJsonParse(j) != YJSON_PARSE_AVAIL) {
        *result = YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid JSON array");
        return "";
    }

    array_type = j->st;
    if (j->st != YJSON_PARSE_STRUCT) {
#ifdef DEBUG_JSON_PARSE
        dbglog("debug %s %s(%d):%s\n", j->token, yJsonStateStr[j->st], j->st, j->token);
#endif
        *result = YERRMSG(YAPI_NOT_SUPPORTED, "Unsupported JSON array");
        return "";
    }
    do {
        if (index == count) {
            return yapiJsonValueParseStruct(j, ++p, result, errmsg);
        } else {
#ifdef DEBUG_JSON_PARSE
            dbglog("skip  %s(%d):%s\n", yJsonStateStr[j->st], j->st, j->token);
#endif
            yJsonParse(j);
            do {
#ifdef DEBUG_JSON_PARSE
                dbglog("... %s(%d):%s\n", yJsonStateStr[j->st], j->st, j->token);
#endif
                yJsonSkip(j, 1);
            } while (yJsonParse(j) == YJSON_PARSE_AVAIL && j->st != array_type);
        }
        count++;
    } while (yJsonParse(j) == YJSON_PARSE_AVAIL);

    *result = YERRMSG(YAPI_INVALID_ARGUMENT, "Path not found");
    return "";
}


static int yapiJsonDecodeString_internal(const char *json_string, char *output)
{
    yJsonStateMachine j;
    char *p = output;
    int maxsize = YSTRLEN(json_string);

    if (maxsize == 0) {
        *p = 0;
        return 0;
    }
    j.src = json_string;
    j.end = j.src + maxsize;
    j.st = YJSON_START;
    yJsonParse(&j);
    do {
        int len = YSTRLEN(j.token);
        yMemcpy(p, j.token, len);
        p += len;
    } while (j.next == YJSON_PARSE_STRINGCONT && yJsonParse(&j) == YJSON_PARSE_AVAIL);
    *p = 0;

    return (u32)(p - output);
}


int yapiJsonGetPath_internal(const char *path, const char *json_data, int json_size, int withHTTPheader, const char **output, char *errmsg)
{
    yJsonStateMachine j;
    int result;

    j.src = json_data;
    j.end = j.src + json_size;
    if (withHTTPheader) {
        j.st = YJSON_HTTP_START;
        if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_HTTP_READ_CODE) {
            return YERRMSG(YAPI_IO_ERROR, "Failed to parse HTTP header");
        }
        if (YSTRCMP(j.token, "200")) {
            return YERRMSG(YAPI_IO_ERROR, "Unexpected HTTP return code");
        }
        if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_HTTP_READ_MSG) {
            return YERRMSG(YAPI_IO_ERROR, "Unexpected JSON reply format");
        }
    } else {
        j.st = YJSON_START;
    }
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
        *output = "";
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Not a JSON struct");
    }

    *output = yapiJsonValueParseStruct(&j, path, &result, errmsg);
    return result;
}


typedef struct _fullAttrInfo {
    char func[32];
    char attr[32];
    char value[256];
} fullAttrInfo;


static fullAttrInfo* parseSettings(const char *settings, int *count)
{
    yJsonStateMachine j;
    int nbAttr = 0, allocAttr = 0;
    fullAttrInfo *attrBuff = NULL;
    char func[32];
    char attr[32];

    // Parse HTTP header
    j.src = settings;
    j.end = j.src + YSTRLEN(settings);
    j.st = YJSON_START;
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
        nbAttr = -1;
        goto exit;
    }
    while (yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
        YSTRCPY(func, 32, j.token);
        if (YSTRCMP(j.token, "services") == 0) {
            yJsonSkip(&j, 1);
        } else {
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
                nbAttr = -1;
                goto exit;
            }
            while (yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
                YSTRCPY(attr, 32, j.token);
                if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                    nbAttr = -1;
                    goto exit;
                }

                if (j.st != YJSON_PARSE_STRUCT) {
                    if (nbAttr == allocAttr) {
                        //grow the buffer
                        fullAttrInfo *tmp = attrBuff;
                        if (allocAttr) {
                            allocAttr *= 2;
                        } else {
                            allocAttr = 64;
                        }
                        attrBuff = yMalloc(allocAttr * sizeof(fullAttrInfo));
                        if (tmp) {
                            memcpy(attrBuff, tmp, nbAttr * sizeof(fullAttrInfo));
                            yFree(tmp);
                        }
                    }
                    YSTRCPY(attrBuff[nbAttr].func, 32, func);
                    YSTRCPY(attrBuff[nbAttr].attr, 32, attr);
                    YSPRINTF(attrBuff[nbAttr].value, 256, "%s", j.token);
                    while (j.next == YJSON_PARSE_STRINGCONT && yJsonParse(&j) == YJSON_PARSE_AVAIL) {
                        YSTRCAT(attrBuff[nbAttr].value, 256, j.token);
                    }
                    nbAttr++;
                } else {
                    do {
                        yJsonParse(&j);
                    } while (j.st != YJSON_PARSE_STRUCT);
                }
            }
            if (j.st != YJSON_PARSE_STRUCT) {
                nbAttr = -1;
                goto exit;
            }
        }
    }
    if (j.st != YJSON_PARSE_STRUCT) {
        nbAttr = -1;
        goto exit;
    }
exit:
    *count = nbAttr;
    if (nbAttr < 0 && attrBuff) {
        yFree(attrBuff);
        attrBuff = NULL;
    }
    return attrBuff;
}


static YRETCODE yapiGetAllJsonKeys_internal(const char *json_buffer, char *buffer, int buffersize, int *fullsize, char *errmsg)
{
    fullAttrInfo *attrs;
    int attrs_count;
    int j, totalsize = 0;
    int len;
    const char *sep = "";

    attrs = parseSettings(json_buffer, &attrs_count);
    if (!attrs) {
        return YERR(YAPI_IO_ERROR);
    }

    if (buffersize < 16) {
        return YERRMSG(YAPI_INVALID_ARGUMENT, "buffer too small");;
    }

    buffer[0] = '[';
    totalsize++;

    for (j = 0; j < attrs_count; j++) {
        char tmpbuf[1024];
        char *p, *d;
        len = YSPRINTF(tmpbuf, 1024, "%s\"%s/%s=", sep, attrs[j].func, attrs[j].attr);
        if (len < 0) {
            yFree(attrs);
            return YERR(YAPI_IO_ERROR);
        }
        p = attrs[j].value;
        d = tmpbuf + len;
        while (*p && len < 1020) {
            if (*p == '"' || *p == '\\') {
                *d++ = '\\';
                len++;
            }
            *d++ = *p++;
            len++;

        }
        *d = 0;
        YSTRCAT(d, 1024-len, "\"");
        len++;
        YASSERT(len == YSTRLEN(tmpbuf), len);
        sep = ",";
        if (buffersize > totalsize + len) {
            memcpy(buffer + totalsize, tmpbuf, len);
        }
        totalsize += len;
    }

    if (buffersize > totalsize) {
        buffer[totalsize] = ']';
    }
    totalsize++;
    *fullsize = totalsize;
    yFree(attrs);
    return YAPI_SUCCESS;
}

#endif

void yapiRegisterRawNotificationCb(yRawNotificationCb callback)
{
    if (!yContext)
        return;

    yEnterCriticalSection(&yContext->enum_cs);
    yContext->rawNotificationCb = callback;
    yLeaveCriticalSection(&yContext->enum_cs);
}

void yapiRegisterRawReportCb(yRawReportCb callback)
{
    if (!yContext)
        return;

    yEnterCriticalSection(&yContext->enum_cs);
    yContext->rawReportCb = callback;
    yLeaveCriticalSection(&yContext->enum_cs);
}

void yapiRegisterRawReportV2Cb(yRawReportV2Cb callback)
{
    if (!yContext)
        return;

    yEnterCriticalSection(&yContext->enum_cs);
    yContext->rawReportV2Cb = callback;
    yLeaveCriticalSection(&yContext->enum_cs);
}


//#define YDLL_TRACE_FILE "dll_trace.txt"

#ifdef YDLL_TRACE_FILE
#include <direct.h>
#include <stdio.h>


typedef enum {
    trcInitAPI = 0,
    trcInitAPIEx,
    trcSSSrv,
    trcSSLGet,
    trcSSLSet,
    trcSSLAddEx,
    trcFreeAPI,
    trcRegisterLogFunction,
    trcRegisterDeviceLogCallback,
    trcStartStopDeviceLogCallback,
    trcRegisterDeviceArrivalCallback,
    trcRegisterDeviceRemovalCallback,
    trcRegisterDeviceChangeCallback,
    trcRegisterFunctionUpdateCallback,
    trcRegisterTimedReportCallback,
    trcLockFunctionCallBack,
    trcUnlockFunctionCallBack,
    trcLockDeviceCallBack,
    trcUnlockDeviceCallBack,
    trcTestHub,
    trcRegisterHub,
    trcPreregisterHub,
    trcUnregisterHub,
    trcUpdateDeviceList,
    trcHandleEvents,
    trcSleep,
    trcCheckLogicalName,
    trcGetAPIVersion,
    trcSetTraceFile,
    trcGetDevice,
    trcGetAllDevices,
    trcGetDeviceInfo,
    trcGetDevicePath,
    trcGetDevicePathEx,
    trcGetFunction,
    trcGetFunctionsByClass,
    trcGetFunctionsByDevice,
    trcGetFunctionInfo,
    trcGetFunctionInfoEx,
    trcHTTPRequestSyncStartEx,
    trcHTTPRequestSyncStart,
    trcHTTPRequestSyncStartOutOfBand,
    trcHTTPRequestSyncDone,
    trcHTTPRequestAsyncEx,
    trcHTTPRequestAsync,
    trcHTTPRequestAsyncOutOfBand,
    trcHTTPRequest,
    trcRegisterHubDiscoveryCallback,
    trcRegisterWakupCb,
    trcTriggerHubDiscovery,
    trcGetBootloaders,
    trcIsModuleWritable,
    trcJsonDecodeString,
    trcJsonGetPath,
    trcGetAllJsonKeys,
    trcCheckFirmware,
    trcUpdateFirmware,
    trcUpdateFirmwareEx,
    trcGetSubdevices,
    trcGetMem,
    trcFreeMem,
    trcGetSubDevcies,
    trcRegisterDeviceConfigChangeCallback,
    trcGetNetworkTimeout,
    trcSetNetworkTimeout,
    trcGetNetDevListValidity,
    trcSetNetDevListValidity,
    trcGetNextHubRef,
    trGetHubStrAttr,
    trGetHubIntAttr,
    trSetHubIntAttr
} TRC_FUN;


static const char *trc_funname[] =
{
    "initApi",
    "initApiEx",
    "trcSSSrv",
    "trcSSLGet",
    "trcSSLSet",
    "trcSSLAddEx",
    "freeApi",
    "RegLog",
    "RegDeviceLog",
    "StartStopDevLog",
    "RegDeviceArrival",
    "RegDeviceRemoval",
    "RegDeviceChange",
    "RegUpdateCallback",
    "RegTimedCallback",
    "LockFunCback",
    "UnLockFunCback",
    "LockDeviceCback",
    "UnLockDeviceCback",
    "TestHub",
    "RegHub",
    "PreRegHub",
    "UnRegHub",
    "UpDL",
    "HE",
    "Sl",
    "CheckLName",
    "GetAPIVersion",
    "SetTrcFile",
    "GDev",
    "GAllDev",
    "GDevInfo",
    "GDevPath",
    "GDevPathEx",
    "GFun",
    "GFunByClass",
    "GFunByDev",
    "GFunInfo",
    "GFunInfoEx",
    "ReqSyncStartEx",
    "ReqSyncStart",
    "ReqSyncStartOB",
    "ReqSyncDone",
    "ReqAsyncEx",
    "ReqAsync",
    "ReqAsyncOB",
    "Req",
    "RegHubDiscovery",
    "RegWakup",
    "THubDiscov",
    "GBoot",
    "isWr",
    "JsonDecStr",
    "JsonGetPath",
    "GAllJsonK",
    "CkFw",
    "UpFw",
    "UpFwEx",
    "GetSubdev",
    "getmem",
    "freemem",
    "getsubdev",
    "RegDeviceConfChg",
    "GetNetworkTimeout",
    "SetNetworkTimeout",
    "GetNetDevListValidity",
    "SetNetDevListValidity",
    "GetNextHubRef",
    "GetHubStrAttr",
    "GetHubIntAttr",
    "SetHubIntAttr"

};

static const char *dlltracefile = YDLL_TRACE_FILE;

static void write_line(const char *ptr, int len)
{
    FILE *f;
    if (YFOPEN(&f, dlltracefile, "a+") != 0) {
        return;
    }
    fwrite(ptr, 1, len, f);
    fclose(f);

}

static u64 trc_basetime = 0;

static void trace_dll(u64 t, char prefix, TRC_FUN trcfun, const char *action)
{
    char buffer[512];
    if (trc_basetime == 0) {
        trc_basetime = t;
    }
    int len = YSPRINTF(buffer, 512, "%"FMTu64"%c%s%s\n", (t - trc_basetime), prefix, trc_funname[trcfun], action);
    write_line(buffer, len);
}

#define YDLL_CALL_ENTER(funname)    TRC_FUN trcfun = funname;\
                                    u64 dll_start, dll_stop;\
                                    char dbg_msg[128];\
                                    dll_start = yapiGetTickCount();\
                                    trace_dll(dll_start, '>' ,trcfun, "");

#define YDLL_CALL_LEAVEVOID()        dll_stop = yapiGetTickCount();\
                                            YSPRINTF(dbg_msg, 128, ":%"FMTs64, (dll_stop - dll_start)); \
                                            trace_dll(dll_start, '<' ,trcfun, dbg_msg);

#define YDLL_CALL_LEAVE(value)     dll_stop = yapiGetTickCount();\
                                            YSPRINTF(dbg_msg, 128, ":%d=%"FMTx64, (dll_stop - dll_start), (int)(value), (u64)(value)); \
                                            trace_dll(dll_start, '<' ,trcfun, dbg_msg);
#define YDLL_CALL_LEAVEPTR(value)     dll_stop = yapiGetTickCount();\
                                            YSPRINTF(dbg_msg, 128, ":%p", (dll_stop - dll_start), (value)); \
                                            trace_dll(dll_start, '<' ,trcfun, dbg_msg);
#else
#define YDLL_CALL_ENTER(funname)
#define YDLL_CALL_LEAVEVOID()
#define YDLL_CALL_LEAVE(value)
#define YDLL_CALL_LEAVEPTR(value)
#endif


YRETCODE YAPI_FUNCTION_EXPORT yapiInitAPI(int detect_type, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcInitAPI);
    res = yapiInitAPI_internal(detect_type, NULL, NULL, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiInitAPIEx(int detect_type, const char *certfile, const char *keyfile, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcInitAPIEx);
    res = yapiInitAPI_internal(detect_type, certfile, keyfile, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiSetSSLCertificateSrv(const char *certfile, const char *keyfile, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcSSSrv);
    res = yapiSetSSLCertificateSrv_internal(certfile, keyfile, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiGetRemoteCertificate(const char *rooturl, u64 timeout, char *buffer, int maxsize, int *neededsize, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcSSLGet);
    res = yapiGetRemoteCertificate_internal(rooturl, timeout, buffer, maxsize, neededsize, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiSetTrustedCertificatesList(const char *certificatePath, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcSSLSet);
    res = yapiSetTrustedCertificatesList_internal(certificatePath, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiAddSSLCertificateCli(const char *cert, int cert_len, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcSSLAddEx);
    res = yapiAddSSLCertificateCli_internal(cert, cert_len, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

void YAPI_FUNCTION_EXPORT yapiFreeAPI(void)
{
    YDLL_CALL_ENTER(trcFreeAPI);
    yapiFreeAPI_internal();
    YDLL_CALL_LEAVEVOID();
}


void YAPI_FUNCTION_EXPORT yapiSetNetworkTimeout(u32 msTimeout)
{
    YDLL_CALL_ENTER(trcSetNetworkTimeout);
    yapiSetNetworkTimeout_internal(msTimeout);
    YDLL_CALL_LEAVEVOID();
}

u32 YAPI_FUNCTION_EXPORT yapiGetNetworkTimeout(void)
{
    u32 res;
    YDLL_CALL_ENTER(trcGetNetworkTimeout);
    res = yapiGetNetworkTimeout_internal();
    YDLL_CALL_LEAVE(res);
    return res;
}


void YAPI_FUNCTION_EXPORT yapiSetNetDevListValidity(int sValidity)
{
    YDLL_CALL_ENTER(trcSetNetDevListValidity);
    yapiSetNetDevListValidity_internal(sValidity * 1000);
    YDLL_CALL_LEAVEVOID();
}

int YAPI_FUNCTION_EXPORT yapiGetNetDevListValidity(void)
{
    int res;
    YDLL_CALL_ENTER(trcGetNetDevListValidity);
    res = (int)(yapiGetNetDevListValidity_internal() / 1000);
    YDLL_CALL_LEAVE(res);
    return res;
}


void YAPI_FUNCTION_EXPORT yapiRegisterLogFunction(yapiLogFunction logfun)
{
    YDLL_CALL_ENTER(trcRegisterLogFunction);
    yapiRegisterLogFunction_internal(logfun);
    YDLL_CALL_LEAVEVOID();
}

void YAPI_FUNCTION_EXPORT yapiRegisterDeviceLogCallback(yapiDeviceLogCallback logCallback)
{
    YDLL_CALL_ENTER(trcRegisterDeviceLogCallback);
    yapiRegisterDeviceLogCallback_internal(logCallback);
    YDLL_CALL_LEAVEVOID();
}

void YAPI_FUNCTION_EXPORT yapiStartStopDeviceLogCallback(const char *serial, int start)
{
    YDLL_CALL_ENTER(trcStartStopDeviceLogCallback);
    yapiStartStopDeviceLogCallback_internal(serial, start);
    YDLL_CALL_LEAVEVOID();
}

void YAPI_FUNCTION_EXPORT yapiRegisterDeviceArrivalCallback(yapiDeviceUpdateCallback arrivalCallback)
{
    YDLL_CALL_ENTER(trcRegisterDeviceArrivalCallback);
    yapiRegisterDeviceArrivalCallback_internal(arrivalCallback);
    YDLL_CALL_LEAVEVOID();
}

void YAPI_FUNCTION_EXPORT yapiRegisterDeviceRemovalCallback(yapiDeviceUpdateCallback removalCallback)
{
    YDLL_CALL_ENTER(trcRegisterDeviceRemovalCallback);
    yapiRegisterDeviceRemovalCallback_internal(removalCallback);
    YDLL_CALL_LEAVEVOID();
}

void YAPI_FUNCTION_EXPORT yapiRegisterDeviceChangeCallback(yapiDeviceUpdateCallback changeCallback)
{
    YDLL_CALL_ENTER(trcRegisterDeviceChangeCallback);
    yapiRegisterDeviceChangeCallback_internal(changeCallback);
    YDLL_CALL_LEAVEVOID();
}

void YAPI_FUNCTION_EXPORT yapiRegisterBeaconCallback(yapiBeaconCallback beaconCallback)
{
    YDLL_CALL_ENTER(trcRegisterDeviceChangeCallback);
    yapiRegisterBeaconCallback_internal(beaconCallback);
    YDLL_CALL_LEAVEVOID();
}

void YAPI_FUNCTION_EXPORT yapiRegisterDeviceConfigChangeCallback(yapiDeviceUpdateCallback configChangeCallback)
{
    YDLL_CALL_ENTER(trcRegisterDeviceConfigChangeCallback);
    yapiRegisterDeviceConfigChangeCallback_internal(configChangeCallback);
    YDLL_CALL_LEAVEVOID();
}

void YAPI_FUNCTION_EXPORT yapiRegisterFunctionUpdateCallback(yapiFunctionUpdateCallback updateCallback)
{
    YDLL_CALL_ENTER(trcRegisterFunctionUpdateCallback);
    yapiRegisterFunctionUpdateCallback_internal(updateCallback);
    YDLL_CALL_LEAVEVOID();
}

void YAPI_FUNCTION_EXPORT yapiRegisterTimedReportCallback(yapiTimedReportCallback timedReportCallback)
{
    YDLL_CALL_ENTER(trcRegisterTimedReportCallback);
    yapiRegisterTimedReportCallback_internal(timedReportCallback);
    YDLL_CALL_LEAVEVOID();
}

YRETCODE YAPI_FUNCTION_EXPORT yapiLockFunctionCallBack(char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcLockFunctionCallBack);
    res = yapiLockFunctionCallBack_internal(errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiUnlockFunctionCallBack(char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcUnlockFunctionCallBack);
    res = yapiUnlockFunctionCallBack_internal(errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiLockDeviceCallBack(char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcLockDeviceCallBack);
    res = yapiLockDeviceCallBack_internal(errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiUnlockDeviceCallBack(char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcUnlockDeviceCallBack);
    res = yapiUnlockDeviceCallBack_internal(errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiTestHub(const char *url, int mstimeout, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcTestHub);
    res = yapiTestHub_internal(url, mstimeout, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiRegisterHub(const char *url, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcRegisterHub);
    res = yapiRegisterHub_internal(url, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiPreregisterHub(const char *url, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcPreregisterHub);
    res = yapiPreregisterHub_internal(url, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

void YAPI_FUNCTION_EXPORT yapiUnregisterHub(const char *url)
{
    YDLL_CALL_ENTER(trcUnregisterHub);
    yapiUnregisterHub_internal(url);
    YDLL_CALL_LEAVEVOID();
}

YRETCODE YAPI_FUNCTION_EXPORT yapiUpdateDeviceList(u32 forceupdate, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcUpdateDeviceList);
    res = yapiUpdateDeviceList_internal(forceupdate, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiHandleEvents(char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcHandleEvents);
    res = yapiHandleEvents_internal(errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiSleep(int ms_duration, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcSleep);
    res = yapiSleep_internal(ms_duration, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

int YAPI_FUNCTION_EXPORT yapiCheckLogicalName(const char *name)
{
    int res;
    YDLL_CALL_ENTER(trcCheckLogicalName);
    res = yapiCheckLogicalName_internal(name);
    YDLL_CALL_LEAVE(res);
    return res;
}

u16 YAPI_FUNCTION_EXPORT yapiGetAPIVersion(const char **version, const char **apidate)
{
    u16 res;
    YDLL_CALL_ENTER(trcGetAPIVersion);
    res = yapiGetAPIVersion_internal(version, apidate);
    YDLL_CALL_LEAVE(res);
    return res;
}

void YAPI_FUNCTION_EXPORT yapiSetTraceFile(const char *file)
{
    YDLL_CALL_ENTER(trcSetTraceFile);
    yapiSetTraceFile_internal(file);
    YDLL_CALL_LEAVEVOID();
}

YAPI_DEVICE YAPI_FUNCTION_EXPORT yapiGetDevice(const char *device_str, char *errmsg)
{
    YAPI_DEVICE res;
    YDLL_CALL_ENTER(trcGetDevice);
    res = yapiGetDevice_internal(device_str, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

int YAPI_FUNCTION_EXPORT yapiGetAllDevices(YAPI_DEVICE *buffer, int maxsize, int *neededsize, char *errmsg)
{
    int res;
    YDLL_CALL_ENTER(trcGetAllDevices);
    res = yapiGetAllDevices_internal(buffer, maxsize, neededsize, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiGetDeviceInfo(YAPI_DEVICE devdesc, yDeviceSt *infos, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcGetDeviceInfo);
    res = yapiGetDeviceInfo_internal(devdesc, infos, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiGetDevicePath(YAPI_DEVICE devdesc, char *rootdevice, char *request, int requestsize, int *neededsize, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcGetDevicePath);
    res = yapiGetDevicePath_internal(devdesc, rootdevice, request, requestsize, neededsize, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiGetDevicePathEx(const char *serial, char *rootdevice, char *request, int requestsize, int *neededsize, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcGetDevicePathEx);
    res = yapiGetDevicePathEx_internal(serial, rootdevice, request, requestsize, neededsize, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YAPI_FUNCTION YAPI_FUNCTION_EXPORT yapiGetFunction(const char *class_str, const char *function_str, char *errmsg)
{
    YAPI_FUNCTION res;
    YDLL_CALL_ENTER(trcGetFunction);
    res = yapiGetFunction_internal(class_str, function_str, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

int YAPI_FUNCTION_EXPORT yapiGetFunctionsByClass(const char *class_str, YAPI_FUNCTION prevfundesc,
                                                 YAPI_FUNCTION *buffer, int maxsize, int *neededsize, char *errmsg)
{
    int res;
    YDLL_CALL_ENTER(trcGetFunctionsByClass);
    res = yapiGetFunctionsByClass_internal(class_str, prevfundesc, buffer, maxsize, neededsize, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

int YAPI_FUNCTION_EXPORT yapiGetFunctionsByDevice(YAPI_DEVICE devdesc, YAPI_FUNCTION prevfundesc,
                                                  YAPI_FUNCTION *buffer, int maxsize, int *neededsize, char *errmsg)
{
    int res;
    YDLL_CALL_ENTER(trcGetFunctionsByDevice);
    res = yapiGetFunctionsByDevice_internal(devdesc, prevfundesc, buffer, maxsize, neededsize, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiGetFunctionInfo(YAPI_FUNCTION fundesc, YAPI_DEVICE *devdesc, char *serial, char *funcId, char *funcName, char *funcVal, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcGetFunctionInfo);
    res = yapiGetFunctionInfoEx_internal(fundesc, devdesc, serial, funcId, NULL, funcName, funcVal, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}


YRETCODE YAPI_FUNCTION_EXPORT yapiGetFunctionInfoEx(YAPI_FUNCTION fundesc, YAPI_DEVICE *devdesc, char *serial, char *funcId, char *baseType, char *funcName, char *funcVal, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcGetFunctionInfoEx);
    res = yapiGetFunctionInfoEx_internal(fundesc, devdesc, serial, funcId, baseType, funcName, funcVal, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}


YRETCODE YAPI_FUNCTION_EXPORT yapiHTTPRequestSyncStartEx(YIOHDL *iohdl, const char *device, const char *request, int requestsize, char **reply, int *replysize, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcHTTPRequestSyncStartEx);
    res = yapiHTTPRequestSyncStartEx_internal(iohdl, 0, device, request, requestsize, reply, replysize, NULL, NULL, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiHTTPRequestSyncStart(YIOHDL *iohdl, const char *device, const char *request, char **reply, int *replysize, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcHTTPRequestSyncStart);
    res = yapiHTTPRequestSyncStartEx_internal(iohdl, 0, device, request, YSTRLEN(request), reply, replysize, NULL, NULL, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}


YRETCODE YAPI_FUNCTION_EXPORT yapiHTTPRequestSyncStartOutOfBand(YIOHDL *iohdl, int channel, const char *device, const char *request, int requestsize, char **reply, int *replysize, yapiRequestProgressCallback progress_cb, void *progress_ctx, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcHTTPRequestSyncStartOutOfBand);
    res = yapiHTTPRequestSyncStartEx_internal(iohdl, channel, device, request, requestsize, reply, replysize, progress_cb, progress_ctx, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}


YRETCODE YAPI_FUNCTION_EXPORT yapiHTTPRequestSyncDone(YIOHDL *iohdl, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcHTTPRequestSyncDone);
    res = yapiHTTPRequestSyncDone_internal(iohdl, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiHTTPRequestAsyncEx(const char *device, const char *request, int len, yapiRequestAsyncCallback callback, void *context, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcHTTPRequestAsyncEx);
    res = yapiHTTPRequestAsyncEx_internal(0, device, request, len, callback, context, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiHTTPRequestAsync(const char *device, const char *request, yapiRequestAsyncCallback callback, void *context, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcHTTPRequestAsync);
    res = yapiHTTPRequestAsyncEx_internal(0, device, request, YSTRLEN(request), callback, context, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiHTTPRequestAsyncOutOfBand(int channel, const char *device, const char *request, int requestsize, yapiRequestAsyncCallback callback, void *context, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcHTTPRequestAsyncOutOfBand);
    res = yapiHTTPRequestAsyncEx_internal(channel, device, request, YSTRLEN(request), callback, context, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}


int YAPI_FUNCTION_EXPORT yapiHTTPRequest(const char *device, const char *request, char *buffer, int buffsize, int *fullsize, char *errmsg)
{
    int res;
    YDLL_CALL_ENTER(trcHTTPRequest);
    res = yapiHTTPRequest_internal(device, request, buffer, buffsize, fullsize, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

void YAPI_FUNCTION_EXPORT yapiRegisterHubDiscoveryCallback(yapiHubDiscoveryCallback hubDiscoveryCallback)
{
    YDLL_CALL_ENTER(trcRegisterHubDiscoveryCallback);
    yapiRegisterHubDiscoveryCallback_internal(hubDiscoveryCallback);
    YDLL_CALL_LEAVEVOID();
}

YRETCODE yapiRegisterWakeUpCb(yWakeUpCb callback)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcRegisterWakupCb);
    res = yapiRegisterWakeUpCb_internal(callback);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiTriggerHubDiscovery(char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcTriggerHubDiscovery);
    res = yapiTriggerHubDiscovery_internal(errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiGetBootloaders(char *buffer, int buffersize, int *fullsize, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcGetBootloaders);
    res = yapiGetBootloaders_internal(buffer, buffersize, fullsize, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

int YAPI_FUNCTION_EXPORT yapiIsModuleWritable(const char *serial, char *errmsg)
{
    int res;
    YDLL_CALL_ENTER(trcIsModuleWritable);
    res = yapiIsModuleWritable_internal(serial, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

#ifndef YAPI_IN_YDEVICE

int YAPI_FUNCTION_EXPORT yapiJsonDecodeString(const char *json_string, char *output)
{
    int res;
    YDLL_CALL_ENTER(trcJsonDecodeString);
    res = yapiJsonDecodeString_internal(json_string, output);
    YDLL_CALL_LEAVE(res);
    return res;
}

int YAPI_FUNCTION_EXPORT yapiJsonGetPath(const char *path, const char *json_data, int json_size, const char **result, char *errmsg)
{
    int res;
    char *tmp;
    YDLL_CALL_ENTER(trcJsonGetPath);
    res = yapiJsonGetPath_internal(path, json_data, json_size, 0, result, errmsg);
    YDLL_CALL_LEAVE(res);
    if (res > 0) {
        tmp = yMalloc(res);
        memcpy(tmp, *result, res);
        *result = tmp;
    }
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiGetAllJsonKeys(const char *json_buffer, char *buffer, int buffersize, int *fullsize, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcGetAllJsonKeys);
    res = yapiGetAllJsonKeys_internal(json_buffer, buffer, buffersize, fullsize, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YAPI_FUNCTION_EXPORT void* yapiGetMem(int size)
{
    void *res;
    YDLL_CALL_ENTER(trcGetMem);
    res = yMalloc(size);
    YDLL_CALL_LEAVEPTR(res);
    return res;
}

YAPI_FUNCTION_EXPORT void yapiFreeMem(void *ptr)
{
    YDLL_CALL_ENTER(trcFreeMem);
    yFree(ptr);
    YDLL_CALL_LEAVEVOID()
}

YRETCODE YAPI_FUNCTION_EXPORT yapiCheckFirmware(const char *serial, const char *rev, const char *path, char *buffer, int buffersize, int *fullsize, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcCheckFirmware);
    res = yapiCheckFirmware_internal(serial, rev, 0, path, buffer, buffersize, fullsize, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiUpdateFirmware(const char *serial, const char *firmwarePath, const char *settings, int startUpdate, char *msg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcUpdateFirmware);
    res = yapiUpdateFirmware_internal(serial, firmwarePath, settings, 0, startUpdate, msg);
    YDLL_CALL_LEAVE(res);
    return res;
}

YRETCODE YAPI_FUNCTION_EXPORT yapiUpdateFirmwareEx(const char *serial, const char *firmwarePath, const char *settings, int force, int startUpdate, char *msg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcUpdateFirmwareEx);
    res = yapiUpdateFirmware_internal(serial, firmwarePath, settings, force, startUpdate, msg);
    YDLL_CALL_LEAVE(res);
    return res;
}


YRETCODE YAPI_FUNCTION_EXPORT yapiGetSubdevices(const char *serial, char *buffer, int buffersize, int *fullsize, char *errmsg)
{
    YRETCODE res;
    YDLL_CALL_ENTER(trcGetSubdevices);
    res = yapiGetSubdevices_internal(serial, buffer, buffersize, fullsize, errmsg);
    YDLL_CALL_LEAVE(res);
    return res;
}

int YAPI_FUNCTION_EXPORT yapiGetNextHubRef(int ref)
{
    int res;
    YDLL_CALL_ENTER(trcGetNextHubRef);
    res = yapiGetNextHubRef_internal(ref);
    YDLL_CALL_LEAVE(res);
    return res;
}

int YAPI_FUNCTION_EXPORT yapiGetHubStrAttr(int ref, const char *attrname, char *outval, int buffersize, int *fullsize)
{
    int res;
    YDLL_CALL_ENTER(trGetHubStrAttr);
    res = yapiGetHubStrAttr_internal(ref, attrname, outval, buffersize, fullsize);
    YDLL_CALL_LEAVE(res);
    return res;
}

int YAPI_FUNCTION_EXPORT yapiGetHubIntAttr(int ref, const char *attrname)
{
    int res;
    YDLL_CALL_ENTER(trGetHubIntAttr);
    res = yapiGetHubIntAttr_internal(ref, attrname);
    YDLL_CALL_LEAVE(res);
    return res;
}

int YAPI_FUNCTION_EXPORT yapiSetHubIntAttr(int ref, const char *attrname, int value)
{
    int res;
    YDLL_CALL_ENTER(trSetHubIntAttr);
    res = yapiSetHubIntAttr_internal(ref, attrname, value);
    YDLL_CALL_LEAVE(res);
    return res;
}

#endif

/*****************************************************************************
 Same function but defined with stdcall
 ****************************************************************************/
#if defined(WINDOWS_API) && defined(__32BITS__) && defined(_MSC_VER)

//typedef void YAPI_FUNCTION_EXPORT(_stdcall *vb6_yapiLogFunction)(BSTR log, u32 loglen);
typedef void YAPI_FUNCTION_EXPORT (_stdcall *vb6_yapiDeviceUpdateCallback)(YAPI_DEVICE devdescr);
typedef void YAPI_FUNCTION_EXPORT (_stdcall *vb6_yapiFunctionUpdateCallback)(YAPI_FUNCTION fundescr, BSTR value);
typedef void YAPI_FUNCTION_EXPORT (_stdcall *vb6_yapiTimedReportCallback)(YAPI_FUNCTION fundesc, double timestamp, const u8 *bytes, u32 len);
typedef void YAPI_FUNCTION_EXPORT (_stdcall *vb6_yapiHubDiscoveryCallback)(BSTR serial, BSTR url);
typedef void YAPI_FUNCTION_EXPORT (_stdcall *vb6_yapiDeviceLogCallback)(YAPI_DEVICE devdescr, BSTR line);
typedef void YAPI_FUNCTION_EXPORT (_stdcall *vb6_yapiRequestAsyncCallback)(void *context, int retcode, BSTR result, u32 resultlen);
typedef void YAPI_FUNCTION_EXPORT (_stdcall *vb6_yapiLogFunction)(BSTR log, u32 loglen);


typedef struct vb6_callback {
    vb6_yapiRequestAsyncCallback callback;
    void *context;
} vb_callback_cache_entry;

static vb_callback_cache_entry vb_callback_cache[NB_MAX_DEVICES];


void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiStartStopDeviceLogCallback(const char *serial, int start);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiInitAPI(int type, char *errmsg);
void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiFreeAPI(void);
//void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterLogFunction(vb6_yapiLogFunction logfun);
void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterDeviceLogCallback(vb6_yapiDeviceLogCallback logCallback);
void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterDeviceArrivalCallback(vb6_yapiDeviceUpdateCallback arrivalCallback);
void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterDeviceRemovalCallback(vb6_yapiDeviceUpdateCallback removalCallback);
void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterDeviceChangeCallback(vb6_yapiDeviceUpdateCallback changeCallback);
void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterDeviceConfigChangeCallback(vb6_yapiDeviceUpdateCallback configChangeCallback);
void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterFunctionUpdateCallback(vb6_yapiFunctionUpdateCallback updateCallback);
void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterTimedReportCallback(vb6_yapiTimedReportCallback timedReportCallback);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiLockFunctionCallBack(char *errmsg);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiUnlockFunctionCallBack(char *errmsg);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiLockDeviceCallBack(char *errmsg);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiUnlockDeviceCallBack(char *errmsg);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterHub(const char *rooturl, char *errmsg);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiPreregisterHub(const char *rooturl, char *errmsg);
void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiUnregisterHub(const char *url);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiUpdateDeviceList(u32 forceupdate, char *errmsg);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiHandleEvents(char *errmsg);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiSleep(int duration_ms, char *errmsg);
u64 YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetTickCount(void);
int YAPI_FUNCTION_EXPORT __stdcall vb6_yapiCheckLogicalName(const char *name);
u16 YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetAPIVersion(BSTR *version, BSTR *apidate);
u16 YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetAPIVersionEx(char *version, char *apidate);
void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiSetTraceFile(const char *file);
YAPI_DEVICE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetDevice(const char *device_str, char *errmsg);
int YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetAllDevices(YAPI_DEVICE *buffer, int maxsize, int *neededsize, char *errmsg);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetDeviceInfo(YAPI_DEVICE devdesc, yDeviceSt *infos, char *errmsg);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetDevicePath(YAPI_DEVICE devdesc, char *rootdevice, char *path, int pathsize, int *neededsize, char *errmsg);
YAPI_FUNCTION YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetFunction(const char *class_str, const char *function_str, char *errmsg);
int YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetFunctionsByClass(const char *class_str, YAPI_FUNCTION prevfundesc, YAPI_FUNCTION *buffer, int maxsize, int *neededsize, char *errmsg);
int YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetFunctionsByDevice(YAPI_DEVICE devdesc, YAPI_FUNCTION prevfundesc, YAPI_FUNCTION *buffer, int maxsize, int *neededsize, char *errmsg);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetFunctionInfo(YAPI_FUNCTION fundesc, YAPI_DEVICE *devdesc, char *serial, char *funcId, char *funcName, char *funcVal, char *errmsg);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiHTTPRequestSyncStartEx(YIOHDL *iohdl, const char *device, const char *request, int requestsize, char **reply, int *replysize, char *errmsg);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiHTTPRequestSyncStart(YIOHDL *iohdl, const char *device, const char *request, char **reply, int *replysize, char *errmsg);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiHTTPRequestSyncDone(YIOHDL *iohdl, char *errmsg);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiHTTPRequestAsync(const char *device, const char *request, vb6_yapiRequestAsyncCallback callback, void *context, char *errmsg);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiHTTPRequestAsyncEx(const char *device, const char *request, int requestsize, vb6_yapiRequestAsyncCallback callback, void *context, char *errmsg);
int YAPI_FUNCTION_EXPORT __stdcall vb6_yapiHTTPRequest(const char *device, const char *request, char *buffer, int buffsize, int *fullsize, char *errmsg);
void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterHubDiscoveryCallback(vb6_yapiHubDiscoveryCallback hubDiscoveryCallback);
YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiTriggerHubDiscovery(char *errmsg);

static BSTR vb6_version = NULL;
static BSTR vb6_apidate = NULL;


void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiStartStopDeviceLogCallback(const char *serial, int start)
{
    yapiStartStopDeviceLogCallback(serial, start);
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiInitAPI(int type, char *errmsg)
{
    return yapiInitAPI(type, errmsg);
}

void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiFreeAPI(void)
{
    yapiFreeAPI();
    if (vb6_version) {
        SysFreeString(vb6_version);
        vb6_version = NULL;
    }
    if (vb6_apidate) {
        SysFreeString(vb6_apidate);
        vb6_apidate = NULL;
    }
}

void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterLogFunction(vb6_yapiLogFunction logfun);

static BSTR newBSTR(const char *str)
{
    int len = YSTRLEN(str);
    int newlen = MultiByteToWideChar(CP_ACP, 0, str, len, NULL, 0);
    BSTR newstring = SysAllocStringLen(0, newlen);
    MultiByteToWideChar(CP_ACP, 0, str, len, newstring, newlen);
    return newstring;
}

// stdcall implementation of yapiRegisterLogFunction
static vb6_yapiLogFunction vb6_yapiLogFunctionFWD = NULL;

void yapiLogFunctionCdeclToStd(const char *log, u32 loglen)
{
    if (vb6_yapiLogFunctionFWD) {
        BSTR bstrstr = newBSTR(log);
        vb6_yapiLogFunctionFWD(bstrstr, loglen);
        SysFreeString(bstrstr);
    }
}

void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterLogFunction(vb6_yapiLogFunction logfun)
{
    vb6_yapiLogFunctionFWD = logfun;
    if (logfun) {
        yapiRegisterLogFunction(yapiLogFunctionCdeclToStd);
    } else {
        yapiRegisterLogFunction(NULL);
    }
}


static vb6_yapiDeviceLogCallback vb6_yapiRegisterDeviceLogCallbackFWD = NULL;

void yapiRegisterDeviceLogCallbackFWD(YAPI_DEVICE devdescr, const char *line)
{
    if (vb6_yapiRegisterDeviceLogCallbackFWD) {
        BSTR bstrstr = newBSTR(line);
        vb6_yapiRegisterDeviceLogCallbackFWD(devdescr, bstrstr);
        SysFreeString(bstrstr);
    }
}

void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterDeviceLogCallback(vb6_yapiDeviceLogCallback logCallback)
{
    vb6_yapiRegisterDeviceLogCallbackFWD = logCallback;
    if (logCallback) {
        yapiRegisterDeviceLogCallback(yapiRegisterDeviceLogCallbackFWD);
    } else {
        yapiRegisterDeviceLogCallback(NULL);
    }
}

static vb6_yapiDeviceUpdateCallback vb6_yapiRegisterDeviceArrivalCallbackFWD = NULL;

void yapiRegisterDeviceArrivalCallbackFWD(YAPI_DEVICE devdescr)
{
    if (vb6_yapiRegisterDeviceArrivalCallbackFWD)
        vb6_yapiRegisterDeviceArrivalCallbackFWD(devdescr);
}

void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterDeviceArrivalCallback(vb6_yapiDeviceUpdateCallback arrivalCallback)
{
    vb6_yapiRegisterDeviceArrivalCallbackFWD = arrivalCallback;
    if (arrivalCallback) {
        yapiRegisterDeviceArrivalCallback(yapiRegisterDeviceArrivalCallbackFWD);
    } else {
        yapiRegisterDeviceArrivalCallback(NULL);
    }
}

static vb6_yapiDeviceUpdateCallback vb6_yapiRegisterDeviceRemovalCallbackFWD = NULL;

void yapiRegisterDeviceRemovalCallbackFWD(YAPI_DEVICE devdescr)
{
    if (vb6_yapiRegisterDeviceRemovalCallbackFWD)
        vb6_yapiRegisterDeviceRemovalCallbackFWD(devdescr);
}

void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterDeviceRemovalCallback(vb6_yapiDeviceUpdateCallback removalCallback)
{
    vb6_yapiRegisterDeviceRemovalCallbackFWD = removalCallback;
    if (removalCallback) {
        yapiRegisterDeviceRemovalCallback(yapiRegisterDeviceRemovalCallbackFWD);
    } else {
        yapiRegisterDeviceRemovalCallback(NULL);
    }
}

static vb6_yapiDeviceUpdateCallback vb6_yapiRegisterDeviceChangeCallbackFWD = NULL;

void yapiRegisterDeviceChangeCallbackFWD(YAPI_DEVICE devdescr)
{
    if (vb6_yapiRegisterDeviceChangeCallbackFWD)
        vb6_yapiRegisterDeviceChangeCallbackFWD(devdescr);
}

void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterDeviceChangeCallback(vb6_yapiDeviceUpdateCallback changeCallback)
{
    vb6_yapiRegisterDeviceChangeCallbackFWD = changeCallback;
    if (changeCallback) {
        yapiRegisterDeviceChangeCallback(yapiRegisterDeviceChangeCallbackFWD);
    } else {
        yapiRegisterDeviceChangeCallback(NULL);
    }
}

static vb6_yapiDeviceUpdateCallback vb6_yapiRegisterDeviceConfigChangeCallbackFWD = NULL;

void yapiRegisterDeviceConfigChangeCallbackFWD(YAPI_DEVICE devdescr)
{
    if (vb6_yapiRegisterDeviceConfigChangeCallbackFWD)
        vb6_yapiRegisterDeviceConfigChangeCallbackFWD(devdescr);
}

void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterDeviceConfigChangeCallback(vb6_yapiDeviceUpdateCallback configChangeCallback)
{
    vb6_yapiRegisterDeviceConfigChangeCallbackFWD = configChangeCallback;
    if (configChangeCallback) {
        yapiRegisterDeviceConfigChangeCallback(yapiRegisterDeviceConfigChangeCallbackFWD);
    } else {
        yapiRegisterDeviceConfigChangeCallback(NULL);
    }
}

static vb6_yapiFunctionUpdateCallback vb6_yapiRegisterFunctionUpdateCallbackFWD = NULL;

void yapiRegisterFunctionUpdateCallbackFWD(YAPI_FUNCTION fundescr, const char *value)
{
    if (vb6_yapiRegisterFunctionUpdateCallbackFWD && value) {
        BSTR bstrstr = newBSTR(value);
        vb6_yapiRegisterFunctionUpdateCallbackFWD(fundescr, bstrstr);
        SysFreeString(bstrstr);
    }
}

void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterFunctionUpdateCallback(vb6_yapiFunctionUpdateCallback updateCallback)
{
    vb6_yapiRegisterFunctionUpdateCallbackFWD = updateCallback;
    if (updateCallback) {
        yapiRegisterFunctionUpdateCallback(yapiRegisterFunctionUpdateCallbackFWD);
    } else {
        yapiRegisterFunctionUpdateCallback(NULL);
    }
}

static vb6_yapiTimedReportCallback vb6_yapiRegisterTimedReportCallbackFWD = NULL;

void yapiRegisterTimedReportCallbackFWD(YAPI_FUNCTION fundesc, double timestamp, const u8 *bytes, u32 len, double duration)
{
    if (vb6_yapiRegisterTimedReportCallbackFWD)
        vb6_yapiRegisterTimedReportCallbackFWD(fundesc, timestamp, bytes, len);
}

void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterTimedReportCallback(vb6_yapiTimedReportCallback timedReportCallback)
{
    vb6_yapiRegisterTimedReportCallbackFWD = timedReportCallback;
    if (timedReportCallback) {
        yapiRegisterTimedReportCallback(yapiRegisterTimedReportCallbackFWD);
    } else {
        yapiRegisterTimedReportCallback(NULL);
    }
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiLockFunctionCallBack(char *errmsg)
{
    return yapiLockFunctionCallBack(errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiUnlockFunctionCallBack(char *errmsg)
{
    return yapiUnlockFunctionCallBack(errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiLockDeviceCallBack(char *errmsg)
{
    return yapiLockDeviceCallBack(errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiUnlockDeviceCallBack(char *errmsg)
{
    return yapiUnlockDeviceCallBack(errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterHub(const char *rooturl, char *errmsg)
{
    return yapiRegisterHub(rooturl, errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiPreregisterHub(const char *rooturl, char *errmsg)
{
    return yapiPreregisterHub(rooturl, errmsg);
}

void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiUnregisterHub(const char *url)
{
    yapiUnregisterHub(url);
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiUpdateDeviceList(u32 forceupdate, char *errmsg)
{
    return yapiUpdateDeviceList(forceupdate, errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiHandleEvents(char *errmsg)
{
    return yapiHandleEvents(errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiSleep(int duration_ms, char *errmsg)
{
    return yapiSleep(duration_ms, errmsg);
}

u64 YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetTickCount(void)
{
    return yapiGetTickCount();
}

int YAPI_FUNCTION_EXPORT __stdcall vb6_yapiCheckLogicalName(const char *name)
{
    return yapiCheckLogicalName(name);
}

u16 YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetAPIVersion(BSTR *version, BSTR *apidate)
{
    const char *versionA, *apidateA;
    u16 res = yapiGetAPIVersion(&versionA, &apidateA);
    *version = newBSTR(versionA);
    *apidate = newBSTR(apidateA);
    return res;
}

u16 YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetAPIVersionEx(char *version, char *apidate)
{
    const char *versionA, *apidateA;
    u16 res = yapiGetAPIVersion(&versionA, &apidateA);
    YSTRCPY(version, YOCTO_ERRMSG_LEN, versionA);
    YSTRCPY(apidate, YOCTO_ERRMSG_LEN, apidateA);
    return res;
}


void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiSetTraceFile(const char *file)
{
    yapiSetTraceFile(file);
}

YAPI_DEVICE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetDevice(const char *device_str, char *errmsg)
{
    return yapiGetDevice(device_str, errmsg);
}

int YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetAllDevices(YAPI_DEVICE *buffer, int maxsize, int *neededsize, char *errmsg)
{
    return yapiGetAllDevices(buffer, maxsize, neededsize, errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetDeviceInfo(YAPI_DEVICE devdesc, yDeviceSt *infos, char *errmsg)
{
    return yapiGetDeviceInfo(devdesc, infos, errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetDevicePath(YAPI_DEVICE devdesc, char *rootdevice, char *path, int pathsize, int *neededsize, char *errmsg)
{
    return yapiGetDevicePath(devdesc, rootdevice, path, pathsize, neededsize, errmsg);
}

YAPI_FUNCTION YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetFunction(const char *class_str, const char *function_str, char *errmsg)
{
    return yapiGetFunction(class_str, function_str, errmsg);
}

int YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetFunctionsByClass(const char *class_str, YAPI_FUNCTION prevfundesc, YAPI_FUNCTION *buffer, int maxsize, int *neededsize, char *errmsg)
{
    return yapiGetFunctionsByClass(class_str, prevfundesc, buffer, maxsize, neededsize, errmsg);
}

int YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetFunctionsByDevice(YAPI_DEVICE devdesc, YAPI_FUNCTION prevfundesc, YAPI_FUNCTION *buffer, int maxsize, int *neededsize, char *errmsg)
{
    return yapiGetFunctionsByDevice(devdesc, prevfundesc, buffer, maxsize, neededsize, errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiGetFunctionInfo(YAPI_FUNCTION fundesc, YAPI_DEVICE *devdesc, char *serial, char *funcId, char *funcName, char *funcVal, char *errmsg)
{
    return yapiGetFunctionInfo(fundesc, devdesc, serial, funcId, funcName, funcVal, errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiHTTPRequestSyncStartEx(YIOHDL *iohdl, const char *device, const char *request, int requestsize, char **reply, int *replysize, char *errmsg)
{
    return yapiHTTPRequestSyncStartEx(iohdl, device, request, requestsize, reply, replysize, errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiHTTPRequestSyncStart(YIOHDL *iohdl, const char *device, const char *request, char **reply, int *replysize, char *errmsg)
{
    return yapiHTTPRequestSyncStart(iohdl, device, request, reply, replysize, errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiHTTPRequestSyncDone(YIOHDL *iohdl, char *errmsg)
{
    return yapiHTTPRequestSyncDone(iohdl, errmsg);
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiHTTPRequestAsync(const char *device, const char *request, vb6_yapiRequestAsyncCallback callback, void *context, char *errmsg)
{
    return vb6_yapiHTTPRequestAsyncEx(device, request, YSTRLEN(request), callback, context, errmsg);
}

static void vb6_callback_fwd(void *context, const u8 *result, u32 resultlen, int retcode, const char *errmsg)
{
    YAPI_DEVICE devydx = (YAPI_DEVICE)context;
    void *vb6_context = vb_callback_cache[devydx].context;
    BSTR bstrstr = newBSTR((char*)result);
    vb_callback_cache[devydx].callback(vb6_context, retcode, bstrstr, resultlen);
    SysFreeString(bstrstr);
}


YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiHTTPRequestAsyncEx(const char *device, const char *request, int requestsize, vb6_yapiRequestAsyncCallback callback, void *context, char *errmsg)
{
    YAPI_DEVICE devydx = wpSearch(device);
    vb_callback_cache[devydx].callback = callback;
    vb_callback_cache[devydx].context = context;
    return yapiHTTPRequestAsyncEx(device, request, requestsize, vb6_callback_fwd, (void*)devydx, errmsg);
}

int YAPI_FUNCTION_EXPORT __stdcall vb6_yapiHTTPRequest(const char *device, const char *request, char *buffer, int buffsize, int *fullsize, char *errmsg)
{
    return yapiHTTPRequest(device, request, buffer, buffsize, fullsize, errmsg);
}

static vb6_yapiHubDiscoveryCallback vb6_yapiHubDiscoveryCallbackFWD = NULL;

void yapiHubDiscoverCdeclToStdllbackFWD(const char *serial, const char *url)
{
    if (vb6_yapiHubDiscoveryCallbackFWD) {
        BSTR bstrserial = newBSTR(serial);
        BSTR bstrurl = newBSTR(url);
        vb6_yapiHubDiscoveryCallbackFWD(bstrserial, bstrurl);
        SysFreeString(bstrurl);
        SysFreeString(bstrserial);
    }
}

void YAPI_FUNCTION_EXPORT __stdcall vb6_yapiRegisterHubDiscoveryCallback(vb6_yapiHubDiscoveryCallback hubDiscoveryCallback)
{
    vb6_yapiHubDiscoveryCallbackFWD = hubDiscoveryCallback;
    if (hubDiscoveryCallback) {
        yapiRegisterHubDiscoveryCallback(yapiHubDiscoverCdeclToStdllbackFWD);
    } else {
        yapiRegisterHubDiscoveryCallback(NULL);
    }
}

YRETCODE YAPI_FUNCTION_EXPORT __stdcall vb6_yapiTriggerHubDiscovery(char *errmsg)
{
    return yapiTriggerHubDiscovery(errmsg);
}


#endif
