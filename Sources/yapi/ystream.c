/*********************************************************************
 *
 * $Id: ystream.c 11071 2013-04-15 08:58:27Z mvuilleu $
 *
 * USB multi-interface stream implementation
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

#define __FILE_ID__  "ystream"
#include "yproto.h"
#include "yhash.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#ifndef WINDOWS_API
#include <sys/time.h>
#endif

/*****************************************************************************
  Global variables
 ***************************************************************************/

yContextSt *yContext=NULL;

#ifdef DEBUG_TRACE_FILE
char  ytracefile[512]=DEBUG_TRACE_FILE;
#else
char  ytracefile[512]="";
#endif

/*****************************************************************************
  Error and logs functions
 ***************************************************************************/


YRETCODE ySetErr(YRETCODE code, char *outmsg, char *erreur,const char *file,u32 line)
{
    char *msg;
    if(outmsg==NULL){
        // no output buffer only return code
        return code;
    }
    if(erreur==NULL){
        switch(code){
            case YAPI_SUCCESS:          msg="Success";break;
            case YAPI_NOT_INITIALIZED:  msg="API not initialized";break;
            case YAPI_INVALID_ARGUMENT: msg="Invalid argument";break;
            case YAPI_NOT_SUPPORTED:    msg="Not supported";break;
            case YAPI_DEVICE_NOT_FOUND: msg="Device not found";break;
            case YAPI_VERSION_MISMATCH: msg="Version mismatch"; break;
            case YAPI_DEVICE_BUSY:      msg="Device busy";break;
            case YAPI_TIMEOUT:          msg="Timeout";break;
            case YAPI_IO_ERROR:         msg="I/O error";break;
            case YAPI_NO_MORE_DATA:     msg="No more data";break;
            case YAPI_EXHAUSTED:        msg="Resource exhausted";break;
            default:                    msg="Unknown error";
            break;
        }
    }else{
        msg=erreur;
    }
    YSPRINTF(outmsg,YOCTO_ERRMSG_LEN,"%s (%s:%d)",msg,file,line);
    return code;
}



int FusionErrmsg(int code,char *errmsg,char *generr,char *detailerr)
{
    int len;
    if(YISERR(code) && errmsg){
        YSTRCPY(errmsg,YOCTO_ERRMSG_LEN,generr);
        YSTRCAT(errmsg,YOCTO_ERRMSG_LEN,":");
        len=YSTRLEN(errmsg);
        YSTRNCAT(errmsg,YOCTO_ERRMSG_LEN,detailerr,YOCTO_ERRMSG_LEN-len);
    }
    return code;
}



#ifndef WINDOWS_API

int fopen_s(FILE** f, const char *filename, const char *mode)
{
    *f=fopen(filename,mode);
    if(*f==NULL){
        return -1;
    }
    return 0;
}

#endif



#define DBGLOGOUT(str) yContext->log(str,YSTRLEN(str))

static int dayofs[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };

static void WriteTsamp(FILE *f)
{
	u32  year, month, day, h, m, s;

#ifdef WINCE
	SYSTEMTIME time;
	GetLocalTime( &time);
	year  = time.wYear;
	month = time.wMonth;
	day   = time.wDay;
	h     = time.wHour;
	m     = time.wMinute;
	s     = time.wSecond;
#else

    u32 secs, days,y;
    time_t ltime;

    time(&ltime);
    secs = (u32)(ltime % 86400);
    days = (u32)(ltime / 86400);
    h = secs / 3600;
    m = (secs % 3600) / 60;
    s = secs % 60;
    y = days / 365; /* relative to 1970 */
    if (((y + 1) >> 2) + 365 * (u32)y > days) y--;
    days -= ((y + 1) >> 2) + 365 * (u32)y;
    if ((y & 3) == 2 && days == 59) {
        month = 2;
    } else {
        if((y & 3) == 2 && days >= 59)
        days--;
        for (month = 1; month < 12; (month)++)
            if (dayofs[month] > (u16)days) break;
    }
    year = y + 1970;
    day = days - dayofs[month - 1] + 1;
#endif
    fprintf(f,"[%04d-%02d-%02d/%02d:%02d:%02d]:",year,month,day,h,m,s);
}


int dbglogf(const char *fileid,int line,const char *fmt,...)
{
    char buffer[2048];
    int len;
    int  threadIdx;
    va_list args;

    va_start( args, fmt );
    threadIdx = yThreadIndex();  
    len=YSPRINTF(buffer,2048,"[%d]%s:% 4d: ",threadIdx,fileid,line);
    if(len<0  || YVSPRINTF(buffer+len,2048-len,fmt,args)<0){
        YSTRCPY(buffer,2048,"dbglogf failed\n");
        return -1;
    }
    va_end(args);
    len =YSTRLEN(buffer);

#if 1
    if(yContext && yContext->log)
        yContext->log(buffer,len);
#endif

    if(ytracefile[0]){
        FILE *f;
        if(fopen_s(&f,ytracefile,"a")!=0){
            return -1;
        }
        WriteTsamp(f);
        fwrite(buffer,1,len,f);
        fclose(f);
    }
    return len;
}


#ifdef __BORLANDC__
#pragma argsused
int sprintf_s(char *buffer,size_t sizeOfBuffer,const char *format,...)
{
    va_list args;
    va_start( args, format);
    return vsprintf(buffer,format,args);
}
#pragma argsused
int wcstombs_s(size_t *pReturnValue, char *mbstr, size_t sizeInBytes, const wchar_t *wcstr, size_t count)
{
    *pReturnValue = wcstombs(mbstr, wcstr, count);
    return 0;
}
#endif
//some early declarations
static yPrivDeviceSt *findDevFromSerial(const char *serial);

/*****************************************************************************
 Whitepage and Yellowpage wrapper for USB devices
 ****************************************************************************/

static void wpRegisterUSB(const yDeviceSt *infos)
{
    yStrRef serialref = yHashPutStr(infos->serial);
    yStrRef lnameref = yHashPutStr(infos->logicalname);
    wpRegister(serialref, lnameref, yHashPutStr(infos->productname), infos->deviceid,
               yHashUrlUSB(infos->serial,"", 0,NULL), infos->beacon);
    ypRegister(YSTRREF_MODULE_STRING, serialref, yHashPutStr("module"), lnameref, -1, NULL);

    // Forward high-level notification to API user
    if (yContext->arrivalCallback) {
        yEnterCriticalSection(&yContext->deviceCallbackCS);
        yContext->arrivalCallback(serialref);
        yLeaveCriticalSection(&yContext->deviceCallbackCS);
    }
}


void wpUpdateUSB(const char *serial,const char *logicalname, u8 beacon)
{
    yStrRef serialref = yHashPutStr(serial);
    yStrRef lnameref = yHashPutStr(logicalname);

    // Update white pages
    wpRegister(serialref, lnameref, INVALID_HASH_IDX, 0, INVALID_HASH_IDX, beacon);
    ypRegister(YSTRREF_MODULE_STRING, serialref, yHashPutStr("module"), lnameref, -1, NULL);

    // Forward high-level notification to API user
    if(yContext->changeCallback){
        yEnterCriticalSection(&yContext->deviceCallbackCS);
        yContext->changeCallback(serialref);
        yLeaveCriticalSection(&yContext->deviceCallbackCS);
    }
}

void wpUnregisterUSB(const char *serial)
{
    yStrRef serialref = yHashTestStr(serial);
    if(serialref == INVALID_HASH_IDX) return;
    // Forward high-level notification to API user before deleting data
    wpPreventUnregister();
    if(wpMarkForUnregister(serialref)){
        // Forward high-level notification to API user before deleting data
        if (yContext->removalCallback) {
            yEnterCriticalSection(&yContext->deviceCallbackCS);
            yContext->removalCallback(serialref);
            yLeaveCriticalSection(&yContext->deviceCallbackCS);
        }
    }
    wpAllowUnregister();
}

void ypUpdateUSB(const char *serial, const char *funcid, const char *funcname, int funydx, const char *funcval)
{
    char    categ[YOCTO_FUNCTION_LEN];
    yStrRef serialref, funcidref, funcnameref = INVALID_HASH_IDX;
    u16     i;

    categ[0] = (funcid[0] & ~0x20); // aka to_upper()
    for(i = 1; i < YOCTO_FUNCTION_LEN-1; i++) {
        char c = funcid[i];
        if(!c || (c <= '9' && c >= '0')) break;
        categ[i] = c;
    }
    categ[i] = 0;
    serialref = yHashPutStr(serial);
    funcidref = yHashPutStr(funcid);
    if(funcname) funcnameref = yHashPutStr(funcname);
    if(ypRegister(yHashPutStr(categ), serialref, funcidref, funcnameref, funydx, funcval)){
        // Forward high-level notification to API user
        if (yContext->functionCallback) {
            yEnterCriticalSection(&yContext->functionCallbackCS);
            yContext->functionCallback(((s32)funcidref << 16) | serialref,funcval);
            yLeaveCriticalSection(&yContext->functionCallbackCS);
        }
    }
}

void ypUpdateYdx(int devydx, int funydx, const char *funcval)
{
    YAPI_FUNCTION fundesc;
    
    if(ypRegisterByYdx(devydx, funydx, funcval, &fundesc)){
        // Forward high-level notification to API user
        if (yContext->functionCallback) {
            yEnterCriticalSection(&yContext->functionCallbackCS);
            yContext->functionCallback(fundesc,funcval);
            yLeaveCriticalSection(&yContext->functionCallbackCS);
        }
    }
}

void ypUpdateHybrid(const char *serial, int funydx, const char *funcval)
{
    int     devydx;
    yStrRef serialref;

    serialref = yHashPutStr(serial);
    devydx = wpGetDevYdx(serialref);
    if(devydx >= 0) {
        ypUpdateYdx(devydx, funydx, funcval);
    }
}

/*****************************************************************************
  THEAD / CONCURENCY RELATED FUNCTIONS
 ***************************************************************************/

 static void devInitAccces(yPrivDeviceSt *dev)
 {
     yInitializeCriticalSection(&dev->acces);
  }

 static void devDeleteAccces(yPrivDeviceSt *dev)
 {
     yDeleteCriticalSection(&dev->acces);
 }


 //return 1 if we get the access
 int devGetAcces(yPrivDeviceSt *dev,int waitacces)
 {
     if(waitacces){
         yEnterCriticalSection(&dev->acces);
         return 1;
     }else{
         if(yTryEnterCriticalSection(&dev->acces))
             return 1;
     }
     return 0;
 }

 void devReleaseAcces(yPrivDeviceSt *dev)
 {
    yLeaveCriticalSection(&dev->acces);
 }


#ifdef DEBUG_DEVICE_LOCK
#define devSafeStartIdle(dev,iohdl,errmsg)   devSafeStartIdleEx(__FILE_ID__,__LINE__,dev,iohdl,errmsg)
static int devSafeStartIdleEx(char *file,int line,yPrivDeviceSt *dev, YUSBIO *iohdl,char *errmsg)
#else
static int devSafeStartIdle(yPrivDeviceSt *dev, YUSBIO *iohdl,char *errmsg)
#endif
{
    int res =YAPI_DEVICE_BUSY;
    //get access
    if(devGetAcces(dev,0)==0){
        return YERR(YAPI_DEVICE_BUSY);
    }

    //dbglog("devSafeStartIdle %s\n",dev->infos.serial);
    switch(dev->rstatus){
    case YRUN_STOPED:
    case YRUN_ERROR:
        res= YERRMSG(YAPI_DEVICE_NOT_FOUND,"This device is not available");
        break;
    case YRUN_REQUEST:
    case YRUN_BUSSY:
    case YRUN_IDLE:
#ifdef DEBUG_DEVICE_LOCK
        YASSERT(errmsg);
        YSPRINTF(errmsg,YOCTO_ERRMSG_LEN,"%s:%d:Last Io is not finished (%s:%d)\n",file,line,dev->lastopen.file,dev->lastopen.line);
        res = YAPI_DEVICE_BUSY;
#else
        res = YERR(YAPI_DEVICE_BUSY);
#endif
        break;
    case YRUN_AVAIL:
        // the device is available keep the critical section
        dev->rstatus = YRUN_IDLE;
        yEnterCriticalSection(&yContext->io_cs);
        *iohdl =  dev->iohdl = ++(yContext->io_counter);
        yLeaveCriticalSection(&yContext->io_cs);
#ifdef DEBUG_DEVICE_LOCK
        dev->lastopen.file=file;
        dev->lastopen.line=line;
#endif
        return YAPI_SUCCESS;
    }

    devReleaseAcces(dev);
    return res;
}


static int devSafeStopIdle(yPrivDeviceSt *dev, YUSBIO iohdl,char *errmsg)
{
    
    //dbglog("devSafeStopIdle %s\n",dev->infos.serial);
    if(dev->rstatus != YRUN_IDLE || dev->iohdl != iohdl){
        devReleaseAcces(dev);
        return YERRMSG(YAPI_INVALID_ARGUMENT,"No Idle started");
    }
    dev->rstatus = YRUN_AVAIL;
    devReleaseAcces(dev);
    return YAPI_SUCCESS;
}


#ifdef DEBUG_DEVICE_LOCK
#define devSafeStartIO(dev,iohdl,wait,errmsg)   devSafeStartIOEx(__FILE_ID__,__LINE__,dev,iohdl,wait,errmsg)
static int devSafeStartIOEx(char *file,int line,yPrivDeviceSt *dev, YUSBIO *iohdl,char *errmsg)
#else
static int devSafeStartIO(yPrivDeviceSt *dev, YUSBIO *iohdl,char *errmsg)
#endif
{
    //get access
    if(devGetAcces(dev,1)==0){
        return YERR(YAPI_DEVICE_BUSY);
    }

    //dbglog("devSafeStartIO %s\n",dev->infos.serial);
    switch(dev->rstatus){
    case YRUN_STOPED:
    case YRUN_ERROR:
        devReleaseAcces(dev);
        return YERRMSG(YAPI_DEVICE_NOT_FOUND,"This device is not available");
        break;
    case YRUN_REQUEST:
    case YRUN_BUSSY:
    case YRUN_IDLE:    
        devReleaseAcces(dev);
#ifdef DEBUG_DEVICE_LOCK
        YASSERT(errmsg);
        YSPRINTF(errmsg,YOCTO_ERRMSG_LEN,"%s:%d:Last Io is not finished (%s:%d)\n",file,line,dev->lastopen.file,dev->lastopen.line);
        return YAPI_DEVICE_BUSY;
#else
        return YERR(YAPI_DEVICE_BUSY);
#endif
    case YRUN_AVAIL:
        dev->rstatus = YRUN_BUSSY;
        yEnterCriticalSection(&yContext->io_cs);
        *iohdl =  dev->iohdl = ++(yContext->io_counter);
        yLeaveCriticalSection(&yContext->io_cs);
#ifdef DEBUG_DEVICE_LOCK
        dev->lastopen.file=file;
        dev->lastopen.line=line;
#endif

        break;
    }

    devReleaseAcces(dev);
    return YAPI_SUCCESS;
}



static int devPauseIO(yPrivDeviceSt *dev,char *errmsg)
{
    //get access
    if(devGetAcces(dev,1)==0){
        return YERR(YAPI_DEVICE_BUSY);
    }
    //dbglog("devPauseIO %s\n",dev->infos.serial);
    if(dev->rstatus != YRUN_BUSSY){
        devReleaseAcces(dev);
        return YERRMSG(YAPI_INVALID_ARGUMENT,"No IO started");
    }

    dev->rstatus = YRUN_REQUEST;
    devReleaseAcces(dev);
    return YAPI_SUCCESS;
}



static int devSafeCheckIO(yPrivDeviceSt *dev, YUSBIO iohdl,char *errmsg)
{
    int res;

    //get access
    if(devGetAcces(dev,1)==0){
        return YERR(YAPI_DEVICE_BUSY);
    }

    //dbglog("devSafeCheckIO %s\n",dev->infos.serial);
    if(dev->rstatus != YRUN_REQUEST || dev->iohdl != iohdl){
        if(dev->rstatus == YRUN_BUSSY){
            res=YERRMSG(YAPI_INVALID_ARGUMENT,"Access violation");
        }else{
            res=YERRMSG(YAPI_INVALID_ARGUMENT,"No IO started");
        }
        devReleaseAcces(dev);
        return res;
    }
    dev->rstatus = YRUN_BUSSY;
    devReleaseAcces(dev);
    return YAPI_SUCCESS;
}

static int devSafeStopIO(yPrivDeviceSt *dev, YUSBIO iohdl,char *errmsg)
{
    //get access
    if(devGetAcces(dev,1)==0){
        return YERR(YAPI_DEVICE_BUSY);
    }

    //dbglog("devSafeStopIO %s\n",dev->infos.serial);
    if(dev->rstatus != YRUN_BUSSY || dev->iohdl != iohdl){
        devReleaseAcces(dev);
        return YERRMSG(YAPI_INVALID_ARGUMENT,"No IO started");
    }
    dev->rstatus = YRUN_AVAIL;
    devReleaseAcces(dev);
    return YAPI_SUCCESS;
}



/*****************************************************************************
  Dump functions
 ***************************************************************************/

static yInterfaceSt* Ino2Idx(yPrivDeviceSt *dev,u8 ifaceno)
{
    u8 idx= dev->ifacesMap[ifaceno];
    YASSERT(ifaceno<2);
    return  &dev->ifaces[idx];
}


#ifdef DEBUG_DUMP_PKT

static void dumpAnyStream(char *prefix,int iface,u8 pkt,u8 stream,u8 size,u8 *data,u8 pktno)
{

    USB_Notify_Pkt *notif= (USB_Notify_Pkt*) data;
	int pos, j;
	char buff[17];
	buff[16] = 0;
    dbglog("%s:%d: head: type=%d stream/cmd=%d size=%d (pktno=%d)\n",
           prefix,iface ,pkt ,stream,size,pktno);

    if(pkt== YPKT_CONF){
        switch(stream){
            case USB_CONF_RESET:
                dbglog("%s: USB_CONF_RESET\n",prefix);
                break;
            case USB_CONF_START:
                dbglog("%s: USB_CONF_START\n",prefix);
                break;
            default:
                dbglog("%s: CONF PKT INVALID!\n",prefix);
                break;
        }
    }else if(pkt == YPKT_STREAM){
        switch(stream){
            case YSTREAM_EMPTY:
                dbglog("%s: Stream Empty\n",prefix);
                break;
            case YSTREAM_NOTICE:
                if(notif->firstByte <= NOTIFY_1STBYTE_MAXTINY) {
                    dbglog("%s: TINY FUNCTION VALUE\n",prefix);
                } else if(notif->firstByte >= NOTIFY_1STBYTE_MINSMALL) {
                    dbglog("%s: SMALL FUNCTION VALUE\n",prefix);
                } else switch(notif->head.type){
                    case NOTIFY_PKT_NAME:
                        dbglog("%s: NOTIFY LOGICAL NAME\n",prefix);
                        break;
                    case NOTIFY_PKT_PRODNAME:
                        dbglog("%s: NOTIFY PRODNAME\n",prefix);
                        break;
                    case NOTIFY_PKT_CHILD:
                        dbglog("%s: NOTIFY CHILD\n",prefix);
                        break;
                    case NOTIFY_PKT_FIRMWARE:
                        dbglog("%s: NOTIFY FIRMWARE\n",prefix);
                        break;
                    case NOTIFY_PKT_FUNCNAME:
                        dbglog("%s: NOTIFY FUNCTION NAME\n",prefix);
                        break;
                    case NOTIFY_PKT_FUNCVAL:
                        dbglog("%s: NOTIFY FUNCTION VALUE\n",prefix);
                        break;
                    case NOTIFY_PKT_STREAMREADY:
                        dbglog("%s: NOTIFY DEVICE READY\n",prefix);
                        break;
                    case NOTIFY_PKT_LOG:
                        dbglog("%s: NOTIFY DEVICE LOG\n",prefix);
                        break;
                    default:
                        dbglog("%s: Unknown NOTIFY\n",prefix);
                        break;
                }
                break;
            case YSTREAM_TCP:
                dbglog("%s: Stream tcp\n",prefix);
				for(pos = 0; pos < size; pos += 16) {
					memset(buff, '.', 16);
					for(j = 0; j < 16; j++)
						if(data[pos+j] >= ' ')
							buff[j] = data[pos+j];
					dbglog("   %02x.%02x.%02x.%02x %02x.%02x.%02x.%02x %02x.%02x.%02x.%02x %02x.%02x.%02x.%02x   %s\n",
						   data[pos+0], data[pos+1], data[pos+2], data[pos+3], 
						   data[pos+4], data[pos+5], data[pos+6], data[pos+7], 
						   data[pos+8], data[pos+9], data[pos+10], data[pos+11], 
						   data[pos+12], data[pos+13], data[pos+14], data[pos+15],
						   buff); 
				}
                break;
            case YSTREAM_TCP_CLOSE:
                dbglog("%s: Stream tcp close\n",prefix);
				for(pos = 0; pos < size; pos += 16) {
					memset(buff, '.', 16);
					for(j = 0; j < 16; j++)
						if(data[pos+j] >= ' ')
							buff[j] = data[pos+j];
					dbglog("   %02x.%02x.%02x.%02x %02x.%02x.%02x.%02x %02x.%02x.%02x.%02x %02x.%02x.%02x.%02x   %s\n",
						   data[pos+0], data[pos+1], data[pos+2], data[pos+3], 
						   data[pos+4], data[pos+5], data[pos+6], data[pos+7], 
						   data[pos+8], data[pos+9], data[pos+10], data[pos+11], 
						   data[pos+12], data[pos+13], data[pos+14], data[pos+15],
						   buff); 
				}
                break;
            default:
                dbglog("%s: Stream invalid !\n",prefix);
                break;
        }

    }
}
static u32 dumpAnyStreamHead(char *prefix,int iface,YSTREAM_Head *head)
{
    u8 *data =((u8*)head)+sizeof(YSTREAM_Head);
    dumpAnyStream(prefix,iface,head->pkt , head->stream, head->size, data, head->pktno);
    return head->size;
}

void dumpAnyPacket(char *prefix, int iface,USB_Packet *pkt)
{
    u32 pos=0;
    //dbglog("%s:%d: DUMP FULL PACKET!\n",prefix,iface);
    while(pos < USB_PKT_SIZE-sizeof(YSTREAM_Head)){
        YSTREAM_Head *head =(YSTREAM_Head*) (pkt->data+pos);
        pos+=sizeof(YSTREAM_Head)+dumpAnyStreamHead(prefix,iface,head);
    }

}
#endif


void  yyPushNewPkt(yInterfaceSt *iface,USB_Packet *pkt)
{
    pktItem *newpkt;

    // allocate new buffer
    newpkt= ( pktItem *) yMalloc(sizeof(pktItem));
    memcpy(&newpkt->pkt,pkt,sizeof(USB_Packet));
    newpkt->next = NULL;
    //lock the queue acces    
    yEnterCriticalSection(&iface->rdCS);
    if (iface->rx_ok.first == NULL) {
        //empty queue
        iface->rx_ok.first = newpkt;
        iface->rx_ok.last = newpkt;
    } else {
        iface->rx_ok.last->next = newpkt;
        iface->rx_ok.last = newpkt;
    }
    iface->rx_ok.count++;
    yLeaveCriticalSection(&iface->rdCS);
}

// this function copy the pkt into the interface out queue and send the packet
int yySendPacket( yInterfaceSt *iface,pktItem *pkt,char *errmsg)
{
    YPROPERR(yyyWrite(iface,&pkt->pkt,errmsg));
    return YAPI_SUCCESS;
}


int yyGetTopPkt(yInterfaceSt *iface, pktItem **ptr, int ms_timeout,char *errmsg)
{
    u64  timeout = yapiGetTickCount() + ms_timeout;
    //get any potential packet pending on Os stack
    YPROPERR(yyyReadIdle(iface,errmsg));
    *ptr =NULL;
retry:
    // look if we have received a packet
    yEnterCriticalSection(&iface->rdCS);
    if(iface->rx_ok.count)
        *ptr = iface->rx_ok.first;
    yLeaveCriticalSection(&iface->rdCS);
    if(*ptr ==NULL && (timeout >yapiGetTickCount())){
        YPROPERR(yyyReadIdle(iface,errmsg));
        goto retry;
    }
    return YAPI_SUCCESS;
}

void yyPopPkt(yInterfaceSt *iface, pktItem *ptr)
{
    yEnterCriticalSection(&iface->rdCS);
    YASSERT(ptr ==iface->rx_ok.first);
    if(iface->rx_ok.first == iface->rx_ok.last)
        iface->rx_ok.last=NULL;
    iface->rx_ok.first = iface->rx_ok.first->next;
    iface->rx_ok.count--;
    yLeaveCriticalSection(&iface->rdCS);
}

static int yyPacketRequestRead(yPrivDeviceSt *dev,yInterfaceSt *iface,char *errmsg)
{
    int res;

    //dbglog("yyPacketRequestRead %d\n",iface->ifaceno);
    if((res=yyyRead(iface,errmsg)) <0){
        dev->rstatus = YRUN_ERROR;
        return res;
    }
    return YAPI_SUCCESS;
}

// wait for the next Configuration packet to come on a particular interface
// ALL OTHER PACKET OF THIS INTERFACE ARE DROPED
static int yyWaitOnlyConfPkt(yInterfaceSt *iface, u8 cmdtowait,pktItem **rpkt,u32 s_timeout,char *errmsg)
{
	u64 timeout = yapiGetTickCount() + s_timeout*1000;
    pktItem *tmp;
    u32 dropcount=0;

    *rpkt=NULL;
    do{
        YPROPERR(yyGetTopPkt(iface,&tmp,1000,errmsg));
        if (tmp){
            if(tmp->pkt.confpkt.head.pkt==YPKT_CONF && tmp->pkt.confpkt.head.stream==cmdtowait){
                //conf packet has bee received
                YASSERT(tmp->pkt.confpkt.head.size>=sizeof(USB_Conf_Pkt));
                *rpkt=tmp;
                if(dropcount)
                    dbglog("drop %d pkt on iface %d\n",dropcount,iface->ifaceno);
                yyPopPkt(iface,tmp);
                yTracePtr(tmp);
                return YAPI_SUCCESS;
            }
#ifdef DEBUG_DUMP_PKT
            else{
                dumpAnyPacket("Drop non conf pkt",(u8)iface->ifaceno,&tmp->pkt);
            }
#endif
            dropcount++;
            yyPopPkt(iface,tmp);
            yFree(tmp);
        }
    }while(timeout> yapiGetTickCount());

    return YERR(YAPI_TIMEOUT);
}


static void yyFormatConfPkt(pktItem *pkt, u8 conftype)
{
    pkt->pkt.confpkt.head.pkt    = YPKT_CONF;
    pkt->pkt.confpkt.head.stream = conftype;
    pkt->pkt.confpkt.head.size   = USB_PKT_SIZE - sizeof(pkt->pkt.confpkt.head);
    pkt->pkt.confpkt.head.pktno  = 0;
}


// check procol version compatibility 
// compatiblewithout limitation -> return 1
// compatible with limitations -> return 0;
// incompatible -> return YAPI_IO_ERROR
static int CheckVersionCompatibility(u16 version,const char *serial, char *errmsg)
{

    if((version & 0xff00) != (YPKT_USB_VERSION_BCD & 0xff00)){
        // major version change
        if((version & 0xff00) > (YPKT_USB_VERSION_BCD & 0xff00)){
            dbglog("Yoctopuce library is too old (using 0x%x need 0x%x) to handle device %s, please upgrade your Yoctopuce library\n",YPKT_USB_VERSION_BCD,version,serial);
            return YERRMSG(YAPI_IO_ERROR,"Library is too old to handle this device");
        } else {
            // implement backward compatibility when implementing a new protocol
            YASSERT(0);
            return 1;
        }
    } else if(version != YPKT_USB_VERSION_BCD ){
        if(version > YPKT_USB_VERSION_BCD){
            dbglog("Device %s is using an newer protocol, consider upgrading your Yoctopuce library\n",serial);
        }else{
            dbglog("Device %s is using an older protocol, consider upgrading the device firmware\n",serial);
        }
        return 0;
    }

    return 1;
}


static int yyResetIface( yInterfaceSt  *iface, u16 *ifaceno, u16 *nbifaces,char *errmsg)
{
    pktItem qpkt,*rpkt=NULL;

    int res;
    //first try to send a connection reset
    //get interface no and API check
    if(ifaceno) *ifaceno   = 0;
    if(nbifaces) *nbifaces = 0;
    yyFormatConfPkt(&qpkt,USB_CONF_RESET);
    qpkt.pkt.confpkt.conf.reset.ok = 1;
    TO_SAFE_U16(qpkt.pkt.confpkt.conf.reset.api,YPKT_USB_VERSION_BCD);
    YPROPERR( yySendPacket(iface,&qpkt,errmsg));

    if(YISERR(yyWaitOnlyConfPkt(iface,USB_CONF_RESET,&rpkt,5,errmsg)) || rpkt==NULL){
        return YERRMSG(YAPI_VERSION_MISMATCH,"Device does not respond to reset");
    }
    FROM_SAFE_U16(rpkt->pkt.confpkt.conf.reset.api,iface->pkt_version);
    if (CheckVersionCompatibility(iface->pkt_version,iface->serial, errmsg)<0) {
        res= YAPI_VERSION_MISMATCH;
        yFree(rpkt);
        return res;
    }
    YASSERT(rpkt->pkt.confpkt.conf.reset.ifaceno < NBMAX_INTERFACE_PER_DEV);
    iface->ifaceno = rpkt->pkt.confpkt.conf.reset.ifaceno;
    if(ifaceno) *ifaceno   = rpkt->pkt.confpkt.conf.reset.ifaceno;
    if(nbifaces) *nbifaces = rpkt->pkt.confpkt.conf.reset.nbifaces;
    yFree(rpkt);
    return YAPI_SUCCESS;
}


void yyyInitPktQueue(yInterfaceSt  *iface)
{
    yInitializeCriticalSection(&iface->rdCS);
    memset(&iface->rx_ok,0,sizeof(pktQueue));
    iface->txqueue = &iface->static_txqueue;
    iface->static_txqueue.next=NULL;    
}

void yyyFreePktQueue(yInterfaceSt  *iface)
{
    pktItem *p,*t;
    
    p=iface->rx_ok.first;
    while(p){
        t=p;
        p=p->next;
        yFree(t);
    }
    yDeleteCriticalSection(&iface->rdCS);
}



int yyPacketSetup(yPrivDeviceSt *dev, yInterfaceSt  *iface,int idx,char *errmsg)
{
    u16 ifaceno,nbifaces;
    int res;

    yyyInitPktQueue(iface);
    YPROPERR(yyySetup(iface,errmsg));
    //open for new packet
    res =yyPacketRequestRead(dev,iface,errmsg);
    if(YISERR(res)) goto error_noread;
    res =yyResetIface(iface,&ifaceno,&nbifaces,errmsg);
    if(YISERR(res)){
        goto error;
    }
    dev->ifacesMap[ifaceno] = idx;
    if(dev->infos.nbinbterfaces !=nbifaces){
        dbglog("Missing interface on OS enumeration(%d vs %d)\n",dev->infos.nbinbterfaces, nbifaces);
        res = YERRMSG(YAPI_VERSION_MISMATCH,"Missing interface on OS enumeration");
        goto error;
    }
    return YAPI_SUCCESS;

error:
    yyyReadStop(iface,NULL);
error_noread:
    yyyPacketShutdown(iface);
    return res;
}


static void yyPacketShutdown(yInterfaceSt  *iface)
{
#if 0
    pktItem qpkt;
    // send a connection reset so that the device
    // will stop sending notification to the OS
    yyFormatConfPkt(&qpkt,USB_CONF_RESET);
    qpkt.pkt.confpkt.conf.reset.ok = 1;
    TO_SAFE_U16(qpkt.pkt.confpkt.conf.reset.api,YPKT_USB_VERSION_BCD);
    yySendPacket(iface,&qpkt,NULL);//discard all error message
#endif
    yyyPacketShutdown(iface);
}

/*****************************************************************
 * yPacket API with cycling logic across all interfaces
*****************************************************************/

static int ySendStart(yPrivDeviceSt *dev,char *errmsg)
{
    int nextiface;
    pktItem  qpkt,*rpkt;

    yyFormatConfPkt(&qpkt,USB_CONF_START);
    qpkt.pkt.confpkt.conf.start.nbifaces = (u8)dev->infos.nbinbterfaces;
    YPROPERR(yySendPacket(Ino2Idx(dev,0),&qpkt,errmsg));
    YPROPERR(yyWaitOnlyConfPkt(Ino2Idx(dev,0),USB_CONF_START,&rpkt,5,errmsg));
    nextiface = rpkt->pkt.confpkt.conf.start.nbifaces;
    dev->lastpktno = rpkt->pkt.first_stream.pktno;
    yFree(rpkt);
    if(nextiface!=0 ){
        return YERRMSG(YAPI_VERSION_MISMATCH,"Device has not been started correctly");
    }
    return YAPI_SUCCESS;
}


static int yPacketSetup(yPrivDeviceSt *dev,char *errmsg)
{
    int i,j,res,toclose=0;

    for(i=0; i <dev->infos.nbinbterfaces ; i++){
        res=yyPacketSetup(dev,&dev->ifaces[i],i,errmsg);
        if(YISERR(res)){
            toclose=i;
            dbglog("yyPacketSetup error %d:\"%s\" for %s index=%d\n",res,errmsg,dev->infos.serial,i);
            goto error;
        }
    }
     if(!YISERR(res=ySendStart(dev,errmsg))){
        return YAPI_SUCCESS;
     }
error:
    //shutdown all previously started interfaces;
    for(j=0; j <toclose ; j++){
        char suberr[YOCTO_ERRMSG_LEN];
        dbglog("Closing partial opened device %s index=%d\n",dev->infos.serial,j);
        if(yyyReadStop(&dev->ifaces[j],suberr))
            dbglog("Unable to cancel read for device %s index=%d:%s\n",dev->infos.serial,j,suberr);
        yyPacketShutdown(&dev->ifaces[j]);
    }
    return res;
}


static int yRestartTransfert(yPrivDeviceSt *dev,char *errmsg)
{
    int i;

    for(i=0; i <dev->infos.nbinbterfaces ; i++){
        YPROPERR(yyResetIface(&dev->ifaces[i],NULL,NULL,errmsg));
    }
    YPROPERR(ySendStart(dev,errmsg));
    return YAPI_SUCCESS;
}

// Trigger a non blocking read
static int yPacketReopen(yPrivDeviceSt *dev,char *errmsg)
{

    int i;
    yInterfaceSt *iface;
    for(i=0; i <dev->infos.nbinbterfaces ; i++){
        iface = Ino2Idx(dev,i);
        YPROPERR(yyPacketRequestRead(dev,iface,errmsg));
    }
    return 0;
}
// Trigger a non blocking read
static int yGetNextPkt(yPrivDeviceSt *dev, pktItem **ptr,char *errmsg)
{
    int             i,count, dropcount;
    u8              nextpktno;
    pktItem        *tmp=NULL;
    u8              ifacefull;

    count = 5;
  restart:
    dropcount = 0;
    ifacefull = 0;
    nextpktno = (dev->lastpktno+1)& YPKTNOMSK;
    *ptr = NULL;
    for(i=0; i < dev->infos.nbinbterfaces ; i++){
        yInterfaceSt *iface = Ino2Idx(dev,i);
    again:
        YPROPERR(yyGetTopPkt(iface,&tmp,0,errmsg));
        if(tmp == NULL) continue; // no pkt avail
        ifacefull++;
        if(tmp->pkt.first_stream.pkt == YPKT_CONF){
#ifdef DEBUG_DUMP_PKT
            dumpAnyPacket("Drop Late conf pkt",iface->ifaceno,&tmp->pkt);
#endif
            yyPopPkt(iface,tmp);
            yFree(tmp);
            tmp=NULL;
            dropcount++;
            if(dropcount >100){
                dbglog("Too many packet dropped, disable %s\n",dev->infos.serial);
                dev->rstatus =  YRUN_ERROR;
                return YERRMSG(YAPI_IO_ERROR,"Too many missing packets");
            }
            goto again;
        }

        if(tmp->pkt.first_stream.pktno == nextpktno){
            *ptr = tmp;
            dev->lastpktno = nextpktno;
            yyPopPkt(iface,tmp);
            yTracePtr(tmp);
#ifdef DEBUG_DUMP_PKT
            dumpAnyPacket("POP Pkt",(u8)iface->ifaceno,&tmp->pkt);
#endif
            return YAPI_SUCCESS;
        }
    }

    if(ifacefull == dev->infos.nbinbterfaces){
        dbglog("Missing packet (looking for pkt %d)\n",nextpktno);
        for(i=0; i < dev->infos.nbinbterfaces ; i++){
            yInterfaceSt *iface = Ino2Idx(dev,i);
            YPROPERR(yyGetTopPkt(iface,&tmp,0,errmsg));
            if(tmp == NULL) continue; // no pkt avail
            dbglog("Packet available on interface %d: %d\n", i, tmp->pkt.first_stream.pktno);
        }
        YPROPERR(yRestartTransfert(dev,errmsg));
        if(--count ==0){
            dbglog("Too many missing packets, disable %s\n",dev->infos.serial);
            dev->rstatus =  YRUN_ERROR;
            return YERRMSG(YAPI_IO_ERROR,"Too many missing packets");
        }
        goto restart;
    }
    return YAPI_SUCCESS;
}
// force to flush stream
int ySendPacket(yPrivDeviceSt *dev,pktItem *pkt,char *errmsg)
{
    yInterfaceSt *iface = Ino2Idx(dev,0);
    return yySendPacket(iface,pkt,errmsg);
}
int yPacketStopRead(yPrivDeviceSt *dev,char *errmsg)
{
    int i;
    yInterfaceSt *iface;
    for(i=0; i <dev->infos.nbinbterfaces ; i++){
        iface = Ino2Idx(dev,i);
        YPROPERR(yyyReadStop(iface,errmsg));
    }
    return YAPI_SUCCESS;
}
void yPacketShutdown(yPrivDeviceSt *dev)
{
    int i;
    for(i=0; i <dev->infos.nbinbterfaces ; i++){
        yyPacketShutdown(&dev->ifaces[i]);
    }
}



/*****************************************************************
 * yStream API with cycling logic of the inside of the packet
*****************************************************************/

static int yStreamSetup(yPrivDeviceSt *dev,char *errmsg)
{
    //dbglog("yStreamSetup %x:%x (%d)\n",dev->infos.vendorid,dev->infos.deviceid,dev->infos.nbinbterfaces);
    YPROPERR(yPacketSetup(dev,errmsg));
    // now we have all setup packet sent and received
    dev->currxpkt=NULL;
    dev->curxofs=0xff;
    dev->curtxpkt = &dev->tmptxpkt;
    dev->tmptxpkt.next=NULL;
    dev->curtxofs=0;
    dev->devYdxMap=NULL;

    return YAPI_SUCCESS;
}

int yStreamReOpen(yPrivDeviceSt *dev,char *errmsg)
{
    return yPacketReopen(dev,errmsg);
}
static int yStreamReceived(yPrivDeviceSt *dev,  u8 *stream, u8 **data, u8 *size,char *errmsg)
{
    YSTREAM_Head  *yshead;
    int res;


     //check if we have processed the full usb packet
    if(dev->curxofs >= USB_PKT_SIZE-sizeof(YSTREAM_Head)){
        // look if we have the next packet on a interface
        if(dev->currxpkt){
            yFree(dev->currxpkt);
            dev->currxpkt=NULL;
        }
        res = yGetNextPkt(dev,&dev->currxpkt,errmsg);
        YPROPERR(res);
        if(dev->currxpkt != NULL){
            dev->curxofs=0;
        }else{
            // no pkt avail
            return 0;
        }
    }

    yshead= (YSTREAM_Head*) &dev->currxpkt->pkt.data[dev->curxofs];
    YASSERT( dev->curxofs +sizeof(YSTREAM_Head) + yshead->size <= USB_PKT_SIZE);
    *stream = yshead->stream;
    *size  = yshead->size;
    *data  = &dev->currxpkt->pkt.data[ dev->curxofs + sizeof(YSTREAM_Head) ];

    return 1;
}
static int yStreamReceptionDone(yPrivDeviceSt *dev,char *errmsg)
{
    YSTREAM_Head * yshead= (YSTREAM_Head*) &dev->currxpkt->pkt.data[dev->curxofs];
    dev->curxofs += sizeof(YSTREAM_Head) + yshead->size;
    return YAPI_SUCCESS;
}
// return 0 when no more buff is available
int yStreamGetTxBuff(yPrivDeviceSt *dev, u8 **data, u8 *maxsize, char *errmsg)
{
    u8  avail;
    avail = USB_PKT_SIZE - dev->curtxofs;
    if(avail <= sizeof(YSTREAM_Head)){
        //we may want to allocate an new buffer
        return 0;
    }

    *maxsize = avail - sizeof(YSTREAM_Head);
    *data = &dev->curtxpkt->pkt.data[dev->curtxofs+sizeof(YSTREAM_Head)];
    return 1;
}
int yStreamTransmit(yPrivDeviceSt *dev, u8 proto,u8 size,char *errmsg)
{
    //dbglog("yPacketTransmit\n");

    YSTREAM_Head  *yshead;
    yshead = (YSTREAM_Head*) dev->curtxpkt->pkt.data + dev->curtxofs ;
    yshead->pktno   =0;
    yshead->pkt     = YPKT_STREAM;
    yshead->stream  = proto;
    yshead->size    = size;
    if(!(dev->curtxofs  + sizeof(YSTREAM_Head) + size <=USB_PKT_SIZE)){
        dbglog("Buffer overrun detected on (%s)\n",dev->infos.serial);
        dbglog("unable to append data proto=%d (%d + %d + %d <= %d) (%s)\n",
                proto,dev->curtxofs,sizeof(YSTREAM_Head),size,USB_PKT_SIZE);
        //dumpAnyPacket("ystream buffer",0,&dev->curtxpkt->pkt);
        return YERRMSG(YAPI_IO_ERROR,"buffer overrun");
    }
    dev->curtxofs += sizeof(YSTREAM_Head)+size;
    if( dev->curtxofs >= USB_PKT_SIZE-sizeof(YSTREAM_Head))
        return yStreamFlush(dev,errmsg);
    return YAPI_SUCCESS;
}
int yStreamFlush(yPrivDeviceSt *dev,char *errmsg)
{
    u8 avail;
   //dbglog("   yyPacketTxFlush %d(%d) :\n",iface->ifaceno, idx);
    if(dev->curtxofs==0){
        return YAPI_SUCCESS;
    }

    avail = USB_PKT_SIZE - dev->curtxofs;

    if(avail >=sizeof(YSTREAM_Head) ){
        YSTREAM_Head  *yshead = (YSTREAM_Head*) (dev->curtxpkt->pkt.data + dev->curtxofs);
        yshead->pkt    = YPKT_STREAM;
        yshead->stream = YSTREAM_EMPTY;
        yshead->size   = avail - sizeof(YSTREAM_Head);
        dev->curtxofs  += sizeof(YSTREAM_Head)+yshead->size;
    }

    YPROPERR( ySendPacket(dev,dev->curtxpkt,errmsg));
    dev->curtxofs =0;
    return YAPI_SUCCESS;
}
// stop all pending read operation (needed to be thread safe)
int yStreamClose(yPrivDeviceSt *dev,char *errmsg)
{
    return yPacketStopRead(dev,errmsg);
}
static void yStreamShutdown(yPrivDeviceSt *dev)
{
    if(dev->devYdxMap) {
        yFree(dev->devYdxMap);
        dev->devYdxMap = NULL;
    }
    yPacketShutdown(dev);
}


/// packet dispatcher
static void yDispatchNoctice(yPrivDeviceSt *dev, USB_Notify_Pkt *notify, int pktsize)
{
    yPrivDeviceSt *notDev;
    u16 vendorid,deviceid;

    if(notify->firstByte <= NOTIFY_1STBYTE_MAXTINY || notify->firstByte >= NOTIFY_1STBYTE_MINSMALL) {
        // Tiny or small pubval notification:
        // create a new null-terminated small notification that we can use and forward
        char                buff[sizeof(Notification_small)+YOCTO_PUBVAL_SIZE+2];
        Notification_small *smallnot = (Notification_small *)buff;
        
        memset(smallnot->pubval,0,YOCTO_PUBVAL_SIZE+2);
        if(notify->firstByte <= NOTIFY_1STBYTE_MAXTINY) {
            memcpy(smallnot->pubval,notify->tinypubvalnot.pubval,pktsize - sizeof(Notification_tiny));
            smallnot->funydx = notify->tinypubvalnot.funydx;
            smallnot->devydx = wpGetDevYdx(yHashPutStr(dev->infos.serial));
        } else {
            memcpy(smallnot->pubval,notify->smallpubvalnot.pubval,pktsize - sizeof(Notification_small));
            smallnot->funydx = notify->smallpubvalnot.funydx - NOTIFY_1STBYTE_MINSMALL;
            if(dev->devYdxMap) {
                smallnot->devydx = dev->devYdxMap[notify->smallpubvalnot.devydx];
            } else {
                smallnot->devydx = 255;
            }
        }
#ifdef DEBUG_NOTIFICATION
        dbglog("notifysmall %d %d %s\n",smallnot->devydx,smallnot->funydx,smallnot->pubval);
#endif
        if(smallnot->devydx < 255) {
            ypUpdateYdx(smallnot->devydx,smallnot->funydx,smallnot->pubval);
            smallnot->funydx += NOTIFY_1STBYTE_MINSMALL;
            if(yContext->rawNotificationCb){
                yContext->rawNotificationCb((USB_Notify_Pkt *)smallnot);
            }
        }
        return;
    }
    
    notDev=findDevFromSerial(notify->head.serial);
    if(notDev==NULL){
        dbglog("drop Notification %d for %s received (device missing)\n", notify->head.type,notify->head.serial);
        return;
    }

#ifdef DEBUG_NOTIFICATION
    dbglog("Notification %d received\n", notify->head.type);
#endif
    switch(notify->head.type){
    case NOTIFY_PKT_NAME:
        YSTRCPY(notDev->infos.logicalname,YOCTO_LOGICAL_LEN,notify->namenot.name);
#ifdef DEBUG_NOTIFICATION
        dbglog("new name is \"%s\"\n", notify->namenot.name);
#endif
        notDev->infos.beacon=notify->namenot.beacon;
#ifdef DEBUG_NOTIFICATION
        dbglog("new beacon %x\n",notify->namenot.beacon);
#endif
        wpUpdateUSB(notify->head.serial,notify->namenot.name,notify->namenot.beacon);
        if(yContext->rawNotificationCb){
            yContext->rawNotificationCb(notify);
        }
        break;
    case NOTIFY_PKT_PRODNAME:
        YSTRCPY(notDev->infos.productname,YOCTO_PRODUCTNAME_LEN,notify->productname);
#ifdef DEBUG_NOTIFICATION
        dbglog("Product name update %s\n",notify->productname);
#endif
        break;
    case NOTIFY_PKT_CHILD:
#ifdef DEBUG_NOTIFICATION
        dbglog("child update %s %d %u\n",notify->childserial.childserial,notify->childserial.onoff,notify->childserial.devydx);
#endif
        if(notDev == dev) {
            // build devYdx mapping for immediate child hubs
            if(dev->devYdxMap == NULL) {
                dev->devYdxMap = yMalloc(ALLOC_YDX_PER_HUB);
                memset(dev->devYdxMap, 255, ALLOC_YDX_PER_HUB);
            }
            dev->devYdxMap[notify->childserial.devydx] = wpGetDevYdx(yHashPutStr(notify->childserial.childserial));
        }
        break;
    case NOTIFY_PKT_FIRMWARE:
#ifdef DEBUG_NOTIFICATION
        dbglog("notify firmware %s\n",notify->firmwarenot.firmware);
#endif
        YSTRCPY(notDev->infos.firmware,YOCTO_FIRMWARE_LEN,notify->firmwarenot.firmware);
        FROM_SAFE_U16(notify->firmwarenot.deviceid,deviceid);
        FROM_SAFE_U16(notify->firmwarenot.vendorid,vendorid);
        YASSERT(notDev->infos.deviceid == deviceid);
        YASSERT(notDev->infos.vendorid == vendorid);
        break;
    case NOTIFY_PKT_FUNCNAME:
        notify->funcnameydxnot.funydx = -1;
        // common code below
    case NOTIFY_PKT_FUNCNAMEYDX:
#ifdef DEBUG_NOTIFICATION
        if(notify->funcnameydxnot.funydx >= 0) {
            dbglog("notify functnameydx %s %s %d\n",notify->funcnamenot.funcid, notify->funcnamenot.funcname,notify->funcnameydxnot.funydx);
        } else {
            dbglog("notify functname %s %s\n",notify->funcnamenot.funcid, notify->funcnamenot.funcname);
        }
#endif
        ypUpdateUSB(notDev->infos.serial,notify->funcnamenot.funcid,notify->funcnamenot.funcname,notify->funcnameydxnot.funydx,NULL);
        if(yContext->rawNotificationCb){
            yContext->rawNotificationCb(notify);
        }
        break;
    case NOTIFY_PKT_FUNCVAL:
        {
            char buff[YOCTO_PUBVAL_SIZE+2];
            memset(buff,0,YOCTO_PUBVAL_SIZE+2);
            memcpy(buff,notify->pubvalnot.pubval,YOCTO_PUBVAL_SIZE);
#ifdef DEBUG_NOTIFICATION
            dbglog("notify funcval %s %s\n",notify->pubvalnot.funcid, buff);
#endif
            ypUpdateUSB(notDev->infos.serial,notify->pubvalnot.funcid,NULL,-1,buff);
            if(yContext->rawNotificationCb){
                yContext->rawNotificationCb(notify);
            }
        }
        break;
    case NOTIFY_PKT_STREAMREADY:
#ifdef DEBUG_NOTIFICATION
        dbglog("notify steam ready (%x)\n",notify->raw);
#endif
        notDev->rstatus = YRUN_AVAIL;
        break;
    case NOTIFY_PKT_LOG:
        {
            yStrRef serialref;
#ifdef DEBUG_NOTIFICATION
            dbglog("notify device log (%x)\n",notify->raw);
#endif
            serialref = yHashPutStr(notify->head.serial);
            if (yContext->logDeviceCallback) {
                yEnterCriticalSection(&yContext->functionCallbackCS);
                yContext->logDeviceCallback(serialref);
                yLeaveCriticalSection(&yContext->functionCallbackCS);
            }
            if(yContext->rawNotificationCb){
                yContext->rawNotificationCb(notify);
            }
        }
    default:
        break;
    }
}

int yDispatchReceive(yPrivDeviceSt *dev,char *errmsg)
{
    u8 stream;
    u8 size;
    u8 *data;
#ifdef DEBUG_USB_TRAFIC
    char dump[64];
#endif
    int pktavail;

    pktavail = yStreamReceived(dev, &stream, &data, &size, errmsg);
    YPROPERR(pktavail);
    while (pktavail && yFifoGetFree(&dev->http_fifo) > size) {
        switch (stream) {
            case YSTREAM_TCP_CLOSE:
#ifdef DEBUG_USB_TRAFIC
                dbglog("YSTREAM : TCP_CLOSE %d (%d:%d)\n",size,dev->iohdl,dev->httpstate);
                memcpy(dump,data,size);
                dump[size]=0;
                dbglog("---------------------------\n");
                dbglog("%s\n",dump);
                dbglog("---------------------------\n");
#endif
                if(dev->httpstate == YHTTP_OPENED) {
                    if(dev->isAsyncIO == 0) {
                        //handle new received packet
                        if(size!=yPushFifo(&dev->http_fifo, data, size)){
                            return YERRMSG(YAPI_IO_ERROR,"FIFO overrun");
                        }
                    }
                    dev->httpstate = YHTTP_CLOSE_BY_DEV;
                } else if(dev->httpstate == YHTTP_CLOSE_BY_API) {
                    dev->httpstate = YHTTP_CLOSED;
                    dev->isAsyncIO = 0;
                }
                break;
            case YSTREAM_TCP:
#ifdef DEBUG_USB_TRAFIC
                dbglog("YSTREAM : TCP %d (%d:%d)\n",size,dev->iohdl,dev->httpstate);
                memcpy(dump,data,size);
                dump[size]='\0';
                dbglog("---------------------------\n");
                dbglog("%s\n",dump);
                dbglog("---------------------------\n");
#endif
                if(dev->httpstate==YHTTP_OPENED && dev->isAsyncIO==0){
                    //handle new received packet
                    if(size!=yPushFifo(&dev->http_fifo, data, size)){
                        return YERRMSG(YAPI_IO_ERROR,"FIFO overrun");
                    }
                }
                break;
            case YSTREAM_NOTICE:
                yDispatchNoctice(dev, (USB_Notify_Pkt*)data, size);
                break;
            case YSTREAM_EMPTY:
            default:
                //packet droped
                break;
        }
        //listen again
        YPROPERR(yStreamReceptionDone(dev,errmsg));
        pktavail=yStreamReceived(dev,&stream, &data, &size, errmsg);
        YPROPERR(pktavail);
    }
    return YAPI_SUCCESS;
}





/*****************************************************************************
  ENUMERATION RELATED FUNCTION
  ***************************************************************************/
#ifdef DEBUG_DEV_ENUM
const char *YDEV_STATUS_TXT[] = 
{
    "YDEV_UNPLUGED",              // device has been plugged by the past but is no more
                                  // -> YDEV_ARRIVAL
    "YDEV_ALLREADY_THERE",        // temporary state (used only during enumeration)
                                  // the device is working at the beginning of the enumeration
                                  // ->YDEV_WORKING, YDEV_UNPLUGED
    "YDEV_ARRIVAL",               // temporary stat (used during enumeration) a new plug has been detected
                                  // -> YDEV_WORKING
    "YDEV_WORKING",               // device is plugged and running
                                  // -> YDEV->UNPLUGET, YDEV_ALLREADY_THERE
    "YDEV_NOTRESPONDING"          // device has not repsond to StartDevice and we will never try to speak with it
                                  // -> none
} ;
#endif

void enuResetDStatus(void)
{

    yPrivDeviceSt *p=yContext->devs;

    while(p){
#ifdef DEBUG_DEV_ENUM_VERBOSE
        dbglog("Initial State of %s is %s\n",p->infos.serial,YDEV_STATUS_TXT[p->dstatus]);
#endif
        if(p->dstatus != YDEV_UNPLUGED && p->dstatus != YDEV_NOTRESPONDING){
            p->dstatus=YDEV_ALLREADY_THERE;
        }
        p=p->next;
    }
}


yPrivDeviceSt* enuFindDevSlot(yInterfaceSt *iface)
{
    yPrivDeviceSt  *p;

    for(p=yContext->devs ; p ; p=p->next ){
        if(p->infos.vendorid == iface->vendorid && p->infos.deviceid == iface->deviceid
            && strncmp(p->infos.serial,iface->serial,YOCTO_SERIAL_LEN)==0){
            return p;
        }
    }
    return NULL;
}

//allocate a  new device structure
yPrivDeviceSt* AllocateDevice(void)
{
    yPrivDeviceSt *dev;


    dev  = (yPrivDeviceSt*) yMalloc(sizeof(yPrivDeviceSt));
    yMemset(dev,0,sizeof(yPrivDeviceSt));
    dev->http_raw_buf =  yMalloc(HTTP_RAW_BUFF_SIZE);
    yFifoInit(&dev->http_fifo, dev->http_raw_buf, HTTP_RAW_BUFF_SIZE);
    devInitAccces(dev);
    return dev;
}

void FreeDevice(yPrivDeviceSt *dev)
{
    devDeleteAccces(dev);
    yFree(dev->http_raw_buf);
    yFree(dev);
}



// Start an interface
int StartDevice(yPrivDeviceSt *dev,char *errmsg)
{
    int res;
    unsigned delay=10;
    int nb_try;
    
    for (nb_try=0; nb_try<4 ; nb_try++,yySleep(delay),delay*=4,dbglog("retrying StartDevice...\n")) {
        u64 timeout;
        int streamres =yStreamSetup(dev,errmsg);
        if (YISERR(streamres)){
            continue;
        }
        timeout=yapiGetTickCount()+10000;
        do {
            res = yDispatchReceive(dev,errmsg);
            if(dev->ifaces[0].pkt_version == YPKT_VERSION_ORIGINAL_RELEASE &&  !dev->infos.productname[0]){
                dev->rstatus =YRUN_AVAIL;
            }
            if (yapiGetTickCount()>=timeout){
                yStreamClose(dev,errmsg);
                return YERRMSG(YAPI_TIMEOUT,"Negotiation failed (device did not responded for 10 secs");
            }
        } while(res==YAPI_SUCCESS && dev->rstatus !=YRUN_AVAIL);
        
        streamres = yStreamClose(dev,errmsg);
        if (YISERR(streamres)){
            continue;
        }
        if(dev->rstatus ==YRUN_AVAIL){
            return YAPI_SUCCESS;
        }
    }
    return YERRMSG(YAPI_IO_ERROR,"Negotiation failed");
}


int StopDevice(yPrivDeviceSt *dev,char *errmsg)

{
    yStreamClose(dev,errmsg);
    yStreamShutdown(dev);
    return YAPI_SUCCESS;
}

//thread safe because only modified only by yDetectDevices which is not reentrant
void enuUpdateDStatus(void)
{
    yPrivDeviceSt *p=yContext->devs;
    int workingdev=0;
    char errmsg[YOCTO_ERRMSG_LEN];

#ifdef DEBUG_DEV_ENUM_VERBOSE
    dbglog("enum : update status\n");
#endif
    while(p){
        devGetAcces(p,1);
        switch(p->dstatus){
        case YDEV_ALLREADY_THERE:
#ifdef DEBUG_DEV_ENUM
            dbglog("enum : %s (%d ifaces)\n",p->infos.serial,p->infos.nbinbterfaces);
            dbglog(": YDEV_ALLREADY_THERE -> YDEV_UNPLUGED\n");
#endif
            p->dstatus = YDEV_UNPLUGED;
            if(YISERR(StopDevice(p,errmsg))){
                dbglog("Unable to stop the device %s correctly:(%s)\n",p->infos.serial,errmsg);
            }
            dbglog("Device %s unplugged\n",p->infos.serial);
            wpUnregisterUSB(p->infos.serial);
            break;
        case YDEV_ARRIVAL:
                if( p->next_startup_attempt <= yapiGetTickCount()) {
                    int res;
                    p->rstatus =YRUN_STOPED;
#ifdef DEBUG_DEV_ENUM
                    dbglog("enum : %s (%d ifaces)\n",p->infos.serial,p->infos.nbinbterfaces);
                    dbglog(": YDEV_ARRIVAL -> ???\n");
#endif
                    res = StartDevice(p,errmsg);
                    if(YISERR(res)){
#ifdef DEBUG_DEV_ENUM
                        dbglog(": YDEV_ARRIVAL -> YDEV_NOTRESPONDING\n");
#endif
                        if (res !=YAPI_TIMEOUT && p->nb_startup_retry < NB_MAX_STARTUP_RETRY) {
                            dbglog("Unable to start the device %s correctly retry later : %s\n",p->infos.serial,errmsg);
                            p->dstatus = YDEV_UNPLUGED;
                            p->next_startup_attempt = yapiGetTickCount()+1000;
                            p->nb_startup_retry++;
                        } else {
                            dbglog("Unable to start the device %s correctly: %s\n",p->infos.serial,errmsg);
                            dbglog("Disable device %s\n",p->infos.serial);
                            p->dstatus = YDEV_NOTRESPONDING;
                        }
                    }else{
#ifdef DEBUG_DEV_ENUM
                        dbglog(": YDEV_ARRIVAL -> YDEV_WORKING\n");
#endif
                        p->dstatus = YDEV_WORKING;
                        p->yhdl    = yContext->devhdlcount++;
                        workingdev++;
                        dbglog("Device %s plugged\n",p->infos.serial);
                        wpRegisterUSB(&p->infos);
                    }
                } else {
#ifdef DEBUG_DEV_ENUM_VERBOSE
                    dbglog("enum : %s (%d ifaces) waiting for next attempt\n",p->infos.serial,p->infos.nbinbterfaces);
#endif
                    p->dstatus = YDEV_UNPLUGED;
                }
           break;
        case YDEV_WORKING:
#ifdef DEBUG_DEV_ENUM_VERBOSE
            dbglog(": YDEV_WORKING \n");
#endif
            workingdev++;
            break;

        case YDEV_UNPLUGED:
#ifdef DEBUG_DEV_ENUM_VERBOSE
            dbglog("enum : %s (%d ifaces)\n",p->infos.serial,p->infos.nbinbterfaces);
            dbglog(": YDEV_UNPLUGED \n");
#endif
        case YDEV_NOTRESPONDING:
#ifdef DEBUG_DEV_ENUM_VERBOSE
            dbglog(": YDEV_NOTRESPONDING \n");
#endif

            break;
        }
        devReleaseAcces(p);
        p=p->next;
    }
    yContext->nbdevs=workingdev;

}

void yUSBReleaseAllDevices(void)
{
    yEnterCriticalSection(&yContext->enum_cs);
    enuResetDStatus();
    enuUpdateDStatus();
    yLeaveCriticalSection(&yContext->enum_cs);
    
}

YRETCODE yUSBUpdateDeviceList(char *errmsg)
{
    int             nbifaces=0;
    yInterfaceSt    *iface;
    int             i,j;
    yInterfaceSt    *runifaces=NULL;
    DevEnum         rundevs[NBMAX_USB_DEVICE_CONNECTED];

    int             nbrundev;

    YPROPERR(yUSBGetInterfaces(&runifaces,&nbifaces,errmsg));

    // construct the device list by merging interfaces
    nbrundev=0;
    for(i=0, iface=runifaces ; i < nbifaces ; i++, iface++){
        if(iface->deviceid <=YOCTO_DEVID_BOOTLOADER)
            continue;
        for(j=0  ; j < nbrundev ; j++){
            DevEnum *rdev = rundevs + j;
            if(rdev->ifaces[0]->vendorid == iface->vendorid && rdev->ifaces[0]->deviceid == iface->deviceid
                && strncmp(rdev->ifaces[0]->serial,iface->serial,YOCTO_SERIAL_LEN)==0){
                //add the current iface
                rdev->ifaces[rdev->nbifaces++] = iface;
                break;
            }
        }
        if(j == nbrundev){
            //new device
            rundevs[nbrundev].nbifaces=1;
            rundevs[nbrundev].ifaces[0] = iface;
            nbrundev++;
        }
    }

    yEnterCriticalSection(&yContext->enum_cs);
    enuResetDStatus();

    for(j=0  ; j < nbrundev ; j++){
        yPrivDeviceSt *dev =enuFindDevSlot(rundevs[j].ifaces[0]);
        if(dev){
            //device already allocated
            if(dev->dstatus==YDEV_ALLREADY_THERE){
                if(dev->rstatus!=YRUN_ERROR && yyyOShdlCompare(dev,&rundevs[j])){
#ifdef DEBUG_DEV_ENUM_VERBOSE
                    ENUMLOG("%s was already there set it as working\n",dev->infos.serial);
#endif
                    dev->dstatus =  YDEV_WORKING;
                }else{
                    ENUMLOG("%s was already there but OS handle are no more valid or we get a runtime error\n",dev->infos.serial);
                }
            }else if(dev->dstatus == YDEV_UNPLUGED ||
                (dev->dstatus == YDEV_NOTRESPONDING && !yyyOShdlCompare(dev,&rundevs[j])) ){
                ENUMLOG("%s replug of a previously detected device put it into the arrival state\n",dev->infos.serial);
                dev->dstatus = YDEV_ARRIVAL;
                // to be safe we update infos with fresh data form last enumeration
                dev->infos.nbinbterfaces = rundevs[j].nbifaces;
                for(i = 0 ; i < rundevs[j].nbifaces; i++){
                    memcpy(&dev->ifaces[i],rundevs[j].ifaces[i],sizeof(yInterfaceSt));
                }
            }
        }else{
            ENUMLOG("%s newly plugged device \n",rundevs[j].ifaces[0]->serial);
            //ALLOCATE A  NEW DEVICE STUCTURE
            dev = AllocateDevice();
            dev->dstatus = YDEV_ARRIVAL;
            //mark device a stopped
            dev->rstatus = YRUN_STOPED;
            dev->infos.vendorid = rundevs[j].ifaces[0]->vendorid;
            dev->infos.deviceid = rundevs[j].ifaces[0]->deviceid;
            YSTRNCPY(dev->infos.serial,YOCTO_SERIAL_LEN,rundevs[j].ifaces[0]->serial,YOCTO_SERIAL_LEN-1);
            dev->infos.nbinbterfaces = rundevs[j].nbifaces;
            for(i = 0 ; i < rundevs[j].nbifaces; i++){
                memcpy(&dev->ifaces[i],rundevs[j].ifaces[i],sizeof(yInterfaceSt));
            }
            dev->next = yContext->devs;
            yContext->devs=dev;
        }
    }
    enuUpdateDStatus();
    yLeaveCriticalSection(&yContext->enum_cs);

    // free all tmp ifaces
    if(runifaces){
        yFree(runifaces);
    }
    return YAPI_SUCCESS;
}




/*****************************************************************************
  GENERIC DEVICE LIST FUNCTION
  ***************************************************************************/

// return the YHANDLE from a matching string (serial or name)
yPrivDeviceSt *findDevFromStr(const char *str)
{
    yPrivDeviceSt *p;
    
    for( p=yContext->devs ; p ; p=p->next){
        if(p->dstatus == YDEV_UNPLUGED){
            continue;
        }
        if(strncmp(str,p->infos.serial,YOCTO_SERIAL_LEN)==0){
            return p;
        }
    }

    for( p=yContext->devs ; p ; p=p->next){
        if(p->dstatus == YDEV_UNPLUGED){
            continue;
        }
        if(strncmp(str,p->infos.logicalname,YOCTO_LOGICAL_LEN)==0){
            return p;
        }
    }
    
    return NULL;
}


// return the YHANDLE from a matching string (serial or name)
YUSBDEV findDevHdlFromStr(const char *str)
{
    yPrivDeviceSt *p = findDevFromStr(str);

    if(p != NULL) {
        return p->yhdl;
    }
    return INVALID_YHANDLE;
}


yPrivDeviceSt *findDevFromIOHdl(YUSBIO hdl)
{
    yPrivDeviceSt *p;
    
    for( p=yContext->devs ; p ; p=p->next){
        if(p->iohdl ==hdl)
            return p;
    }
    return NULL;
}

// find a device from his YHANDLE
static yPrivDeviceSt *findDevFromSerial(const char *serial)
{
    yPrivDeviceSt *p;

    for( p=yContext->devs ; p ; p=p->next){
        if(p->dstatus == YDEV_UNPLUGED){
            continue;
        }
        if(strncmp(serial,p->infos.serial,YOCTO_SERIAL_LEN)==0){
            return p;
        }
    }
    return NULL;
}



// find a device from his YHANDLE
static yPrivDeviceSt *findDevFromDevHdl(YUSBDEV hdl)
{
    yPrivDeviceSt *p;

    for( p=yContext->devs ; p ; p=p->next){
        if(p->yhdl ==hdl)
            return p;
    }
    return NULL;
}

void devHdlInfo(YUSBDEV hdl,yDeviceSt *infos)
{
    yPrivDeviceSt *p;

    p=findDevFromDevHdl(hdl);
    if(p!=NULL){
        *infos = p->infos;
    } else {
        memset(infos, 0, sizeof(yDeviceSt));
    }
}



/*****************************************************************************
  USB REQUEST FUNCTIONS
  ***************************************************************************/

int yUsbIdle(void)
{
    yPrivDeviceSt   *p;
    YUSBIO          iohdl;
    char            errmsg[YOCTO_ERRMSG_LEN];

    for( p=yContext->devs ; p ; p=p->next){

        if(p->dstatus == YDEV_UNPLUGED){
            continue;
        }
        memset(&iohdl,0,sizeof(YUSBIO));
        if(!YISERR(devSafeStartIdle(p,&iohdl,errmsg))){
            if(YISERR(yStreamReOpen(p,errmsg))){
                dbglog("yStreamReOpen error:%s\n",errmsg);
            }else{
                if(YISERR(yDispatchReceive(p,errmsg))){
                    dbglog("yPacketDispatchReceive error:%s\n",errmsg);
                }
                // stop all pending read operation (needed to be thread safe)
                if(YISERR(yStreamClose(p,errmsg))){
                    dbglog("yStreamClose error:%s\n",errmsg);
                }
            }
            devSafeStopIdle(p,iohdl,NULL);
        } else if (p->isAsyncIO) {
            // if we have an async IO on this device 
            // simulate read from users but we need to handle
            // potential stream reopen
            if (!YISERR(devSafeCheckIO(p,p->iohdl,errmsg))) {
                if(YISERR(yStreamReOpen(p,errmsg))){
                    dbglog("yStreamReOpen error:%s\n",errmsg);
                }else{
                    if(YISERR(yDispatchReceive(p,errmsg))){
                        dbglog("yPacketDispatchReceive error:%s\n",errmsg);
                    }
                    if(p->httpstate == YHTTP_CLOSE_BY_DEV) {
                        u8  *pktdata;
                        u8  maxpktlen;
                        // send connection reset
                        if(yStreamGetTxBuff(p,&pktdata, &maxpktlen,errmsg)){
                            if(YISERR(yStreamTransmit(p,YSTREAM_TCP_CLOSE,0,errmsg))){
                                dbglog("Unable to send async connection close");
                            } else if(YISERR(yStreamFlush(p,errmsg))) {
                                dbglog("Unable to flush async connection close");
                            }
                            yFifoEmpty(&p->http_fifo);
                            p->httpstate = YHTTP_CLOSED;
                            p->isAsyncIO=0;
                        } 
                    }
                    // stop all pending read operation (needed to be thread safe)
                    if(YISERR(yStreamClose(p,errmsg))){
                        dbglog("yStreamClose error:%s\n",errmsg);
                    }
                }
                if(p->httpstate == YHTTP_CLOSED) {
                    devSafeStopIO(p,p->iohdl,errmsg);
                } else {
                    devPauseIO(p,NULL);
                }
            }
        }
    }
    return YAPI_SUCCESS;
}

int yUsbTrafficPending(void)
{
    yPrivDeviceSt   *p;
    
    for( p=yContext->devs ; p ; p=p->next){
        if(p->dstatus == YDEV_UNPLUGED){
            continue;
        }
        if(p->isAsyncIO) return 1;
    }
    return 0;
}

int yUsbOpen(YIOHDL *ioghdl, const char *device, char *errmsg)
{
    int           res;
    yPrivDeviceSt *p;
    YUSBIO        iohdl;

    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    p=findDevFromStr(device);
    if(p==NULL)
        return YERR(YAPI_DEVICE_NOT_FOUND);
    
    memset(&iohdl,0,sizeof(YUSBIO));
    res = devSafeStartIO(p,&iohdl,errmsg);
    if(YISERR(res)){
        return res;
    }   
    res=yStreamReOpen(p,errmsg);
    if(res<0){
        devSafeStopIO(p,iohdl,NULL);
        return res;
    }
    if(YISERR(res=yDispatchReceive(p,errmsg))){
        devSafeStopIO(p,iohdl,NULL);
        return res;
    }
    p->httpstate = YHTTP_OPENED;
    ioghdl->hdl = iohdl;
    ioghdl->type = YIO_USB;
    
    devPauseIO(p,NULL);
    return YAPI_SUCCESS;
}

int yUsbSetIOAsync(YIOHDL *ioghdl, char *errmsg)
{
    int res =YAPI_SUCCESS;
    yPrivDeviceSt *p;
    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    p=findDevFromIOHdl(ioghdl->hdl);
    if(p==NULL)
        return YERR(YAPI_DEVICE_NOT_FOUND);
    YPROPERR(devSafeCheckIO(p,ioghdl->hdl,errmsg));
    p->isAsyncIO = 1;
    res=yStreamClose(p,errmsg);
    if (!YISERR(res)){
        ioghdl->flags |= YIO_ASYNC;    
    }
    devPauseIO(p,NULL);
    return res;
}



int  yUsbWrite(YIOHDL *ioghdl, const char *buffer, int writelen,char *errmsg)
{
    yPrivDeviceSt *p;
    int totalsend=0;
    u8  *pktdata;
    u8  maxpktlen;
    int res;

    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    p=findDevFromIOHdl(ioghdl->hdl);
    if(p==NULL)
        return YERR(YAPI_DEVICE_NOT_FOUND);
    YPROPERR(devSafeCheckIO(p,ioghdl->hdl,errmsg));
    if(YISERR(res=yDispatchReceive(p,errmsg))){
        goto exit;
    }

    if(p->httpstate != YHTTP_OPENED){
        devPauseIO(p,NULL);
        return YERRMSG(YAPI_IO_ERROR,"Connection closed");
    }

    if(writelen==0){
        goto exitnoerr;
    }
    while (writelen){
        while(writelen && yStreamGetTxBuff(p,&pktdata, &maxpktlen,errmsg)==1) {
            u8 pktlen =(maxpktlen < writelen ? maxpktlen: writelen);
            memcpy(pktdata,buffer,pktlen);
            if(YISERR(res=yStreamTransmit(p,YSTREAM_TCP,pktlen,errmsg))) 
                goto exit;
            buffer    += pktlen;
            writelen  -= pktlen;
            totalsend += pktlen;
        }
        if(YISERR(res=yStreamFlush(p,errmsg))) 
            goto exit;
    }

exitnoerr:
    res=totalsend;
exit:
    devPauseIO(p,NULL);
    return res;
}



int  yUsbReadNonBlock(YIOHDL *ioghdl, char *buffer, int len,char *errmsg)
{
    yPrivDeviceSt *p;
    u16 readed;
    int res;

    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    p=findDevFromIOHdl(ioghdl->hdl);
    if(p==NULL)
        return YERR(YAPI_DEVICE_NOT_FOUND);
    YPROPERR(devSafeCheckIO(p,ioghdl->hdl,errmsg));
    if(YISERR(res=yDispatchReceive(p,errmsg))){
        devPauseIO(p,NULL);
        return res;
    }
    if(len > HTTP_RAW_BUFF_SIZE){
        len = HTTP_RAW_BUFF_SIZE;
    }
    //get all available data
    readed=yPopFifo(&p->http_fifo,(u8*)buffer,(u16)len);
    devPauseIO(p,NULL);
    return readed;
}



int  yUsbReadSelect(YIOHDL *ioghdl, u64 mstimeout,char *errmsg)
{
    yPrivDeviceSt *p;
    int res=0;
    u64 timeout;
    
    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    
    p=findDevFromIOHdl(ioghdl->hdl);
    if(p==NULL)
        return YERR(YAPI_DEVICE_NOT_FOUND);
    YPROPERR(devSafeCheckIO(p,ioghdl->hdl,errmsg));
    
    timeout=yapiGetTickCount()+mstimeout;
    do {
       
        if(YISERR(res=yDispatchReceive(p,errmsg))){
            devPauseIO(p,NULL);
            return res;
        }
        if (yFifoGetUsed(&p->http_fifo)>0)
            res =1;
    } while(res == 0 && yapiGetTickCount()<timeout);
    devPauseIO(p,NULL);
    return res;
}



int  yUsbEOF(YIOHDL *ioghdl,char *errmsg)
{
    yPrivDeviceSt *p;
    int res;
    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);

    p=findDevFromIOHdl(ioghdl->hdl);
    if(p==NULL)
        return YERR(YAPI_DEVICE_NOT_FOUND);
    YPROPERR(devSafeCheckIO(p,ioghdl->hdl,errmsg));
    if(YISERR(res=yDispatchReceive(p,errmsg))){
        devPauseIO(p,NULL);
        return res;
    }
    res=0;
    if(yFifoGetUsed(&p->http_fifo)==0 && p->httpstate == YHTTP_CLOSE_BY_DEV){
	    //dbglog("* yUsbEOF for %d is TRUE\n",p->iohdl);
        res =1;
        ioghdl->flags |= YIO_REMOTE_CLOSE;
    }
    devPauseIO(p,NULL);
    return res;
}



int  yUsbClose(YIOHDL *ioghdl,char *errmsg)
{
    yPrivDeviceSt *p;
    int res;
    u8  *pktdata;
    u8  maxpktlen;
    u16 deviceDead = 0;

    //dbglog("R_Close %d\n",hdl);
    if(!yContext)
        return YERR(YAPI_NOT_INITIALIZED);
    p=findDevFromIOHdl(ioghdl->hdl);
    if(p==NULL)
        return YERR(YAPI_DEVICE_NOT_FOUND);
    //dbglog("* yUsbClose for %d, httpstate=%d\n",p->iohdl,p->httpstate);
    if(p->httpstate == YHTTP_CLOSED || p->httpstate == YHTTP_CLOSE_BY_API) {
        dbglog("yUsb double-close");
        return YAPI_SUCCESS;
    }
    YPROPERR(devSafeCheckIO(p,ioghdl->hdl,errmsg));
    // send connection close
    if(!yStreamGetTxBuff(p,&pktdata, &maxpktlen,errmsg)) {
        if(YISERR(yStreamFlush(p,errmsg))) {
            dbglog("Unable to flush pending data");
            deviceDead = 1;
        }
        yStreamGetTxBuff(p,&pktdata, &maxpktlen,errmsg);
    }
    if(!deviceDead) {
        if(YISERR(yStreamTransmit(p,YSTREAM_TCP_CLOSE,0,errmsg))) {
            dbglog("Unable to send connection close");
            deviceDead = 1;
        } else if(YISERR(yStreamFlush(p,errmsg))) {
            dbglog("Unable to flush connection close");
            deviceDead = 1;
        }
    }
	if(p->httpstate == YHTTP_CLOSE_BY_DEV || deviceDead) {
        p->httpstate = YHTTP_CLOSED;
        if(p->isAsyncIO){
            //dbglog("close Async io");
            p->isAsyncIO=0;
        }
    } else {
        //wait for the device close packet
        u64 timeout = yapiGetTickCount() + 100;
        p->httpstate = YHTTP_CLOSE_BY_API;
        while(!YISERR(yDispatchReceive(p,errmsg))){
            if(p->httpstate == YHTTP_CLOSED) {
                // received close from device
                break;
            }
            if(timeout<yapiGetTickCount()) {
                dbglog("yUSBClose without device ack\n");
                break;
            }
        }
    }

    res=yStreamClose(p,errmsg);
    yFifoEmpty(&p->http_fifo);
    YPROPERR(devSafeStopIO(p,ioghdl->hdl,errmsg));
    return res;
}
