/*********************************************************************
 *
 * $Id: ypkt_win.c 10249 2013-03-11 13:38:37Z mvuilleu $
 *
 * OS-specific USB packet layer, Windows version
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

#define __FILE_ID__  "ypkt_win"
#include "yapi.h"
#if defined(WINDOWS_API) && !defined(WINCE)
#include "yproto.h"

#ifndef _MSC_VER
#define SPDRP_INSTALL_STATE (0x00000022)
#define _TRUNCATE ((size_t)-1)
#endif

#define yWinSetErr(errmsg)  yWinSetErrEx(__LINE__,GetLastError(),errmsg)

static int yWinSetErrEx(u32 line,DWORD err,char *errmsg)
{
    int len;
    if(errmsg==NULL)
        return YAPI_IO_ERROR;
    YSPRINTF(errmsg,YOCTO_ERRMSG_LEN,"%s:%d: WIN(%d):",__FILE_ID__,line,(u32)err);
    len=YSTRLEN(errmsg);
    FormatMessageA (
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR) (errmsg+len),
        YOCTO_ERRMSG_LEN-len, NULL );

    return YAPI_IO_ERROR;
}





static u32 decodeHex(const char *p,int nbdigit)
{
    u32 ret=0;
    int i;
    for(i=nbdigit-1 ; i>=0 ; i--,p++ ){
        u32 digit;
        if(*p >= 'a' && *p <='f'){
            digit= 10 + *p - 'a';
        }else if(*p >= 'A' && *p <='F'){
            digit= 10 + *p - 'A';
        }else if(*p >='0' && *p <='9'){
            digit=*p-'0';
        }else{
            return 0;
        }
        ret += digit << (4*i);
    }
    return ret;
}



#define FIRST_OF        1
#define FIRST_NOT_OF    2

static char* findDelim(char *str,const char *delimiters,const int nbdelim,const int mode)
{
    int d;

    while(*str){
        for( d=0 ; d < nbdelim ; d++ ){
            if( *str == delimiters[d]){
                break;
            }
        }
        if(mode==FIRST_OF){
            if(d < nbdelim){
                return str;
            }
        }else{
            if(d == nbdelim){
                return str;
            }
        }
        str++;
    }
    return str;
}


void DecodeHardwareid(char *str, u32 *vendorid,u32 *deviceid,u32 *release, u32 *iface)
{
    const char *delim="\\&?";
    char *token_start, *token_stop;
    token_start = findDelim(str,delim,4,FIRST_NOT_OF);
    token_stop = findDelim(token_start,delim,4,FIRST_OF);
    *vendorid = *deviceid = *release = *iface = 0;
    while(token_start!=token_stop){
        if(YSTRNICMP(token_start,"VID_",4)==0){
            *vendorid = decodeHex(token_start+4,4);
        }else if(YSTRNICMP(token_start,"PID_",4)==0){
            *deviceid = decodeHex(token_start+4,4);
        }else if(YSTRNICMP(token_start,"REV_",4)==0){
            *release = decodeHex(token_start+4,4);
        }else if(YSTRNICMP(token_start,"MI_",3)==0){
            *iface = decodeHex(token_start+3,2);
        }
        token_start = findDelim(token_stop,delim,2,FIRST_NOT_OF);
        token_stop = findDelim(token_start,delim,2,FIRST_OF);
    }
}


// return 1 if we can reserve access to the device 0 if the device
// is already reserved
static int yReserveGlobalAccess(yContextSt *ctx)
{
    ctx->apiLock = CreateMailslotA("\\\\.\\mailslot\\yoctopuce_yapi",8,0,NULL);
    if (ctx->apiLock == INVALID_HANDLE_VALUE) {
        // unable to create lock -> another instance is already using the device
        return 0;
    }
    return 1;
}

static void yReleaseGlobalAccess(yContextSt *ctx)
{
    CloseHandle(ctx->apiLock);
    ctx->apiLock=INVALID_HANDLE_VALUE;
}



static void* yUSBHelperThread(void* arg)
{
    yThread     *thread=(yThread*)arg;

    yThreadSignalStart(thread);
    //dbglog("start helper thread\n");
    while(!yThreadMustEnd(thread)){
        if(yContext!=NULL){
            yapiHandleEvents(NULL);
        }
        yySleep(10);//sleep 10 ms
    }
    yThreadSignalEnd(thread);
    return NULL;
}



int yUSB_init(yContextSt *ctx,char *errmsg)
{
    if(!yReserveGlobalAccess(ctx)){
        return YERRMSG(YAPI_DOUBLE_ACCES,"Another process is already using yAPI");
    }
    ctx->hid.hHID = LoadLibraryA("HID.DLL");
    if( ctx->hid.hHID == NULL){
          return yWinSetErr(errmsg);
    }
    //Update the pointers:
    ctx->hid.GetHidGuid       = (PHidD_GetHidGuid)    GetProcAddress( ctx->hid.hHID, "HidD_GetHidGuid");
    ctx->hid.GetAttributes    = (PHidD_GetAttributes) GetProcAddress( ctx->hid.hHID, "HidD_GetAttributes");
    ctx->hid.GetManufacturerString    = (PHidD_GetManufacturerString)     GetProcAddress( ctx->hid.hHID, "HidD_GetManufacturerString");
    ctx->hid.GetProductString         = (PHidD_GetProductString)          GetProcAddress( ctx->hid.hHID, "HidD_GetProductString");
    ctx->hid.GetSerialNumberString    = (PHidD_GetSerialNumberString)     GetProcAddress( ctx->hid.hHID, "HidD_GetSerialNumberString");
 
    if(yThreadCreate(&ctx->usb_thread,yUSBHelperThread,(void*)yContext)<0){
        return YERRMSG(YAPI_IO_ERROR,"Unable to start USB thread");
    }

    return YAPI_SUCCESS;
}


int yUSB_stop(yContextSt *ctx,char *errmsg)
{
    yThreadRequestEnd(&ctx->usb_thread);
    yThreadWaitEnd(&ctx->usb_thread);
	if(ctx->prevEnum) {
        yFree(ctx->prevEnum);
        ctx->prevEnum;
    }
    yReleaseGlobalAccess(ctx);
    return YAPI_SUCCESS;
}
 /*****************************************************************
 * yPacket API without cycling logic
*****************************************************************/


// no check on reentrance or initializations since we are only called
// by the yUpdateDeviceList witch take care of all this stuff
// the caller is responsible of freeing the ifaces buffer (if not set to NULL)
int yUSBGetInterfaces(yInterfaceSt **ifaces,int *nbifaceDetect,char *errmsg)
{
    PSP_DEVICE_INTERFACE_DETAIL_DATA_A    pDetailedInterfaceData;
    int         index=0;
    GUID        InterfaceClassGuid;
    DWORD       needsize;
    HDEVINFO    DeviceInfoTable = INVALID_HANDLE_VALUE;
    int         nbifaceAlloc;
    char        buffer[WIN_DEVICE_PATH_LEN];//buffer forp DetailedInterfaceData

    *ifaces=NULL;
    //setup some windows stuff
    yContext->hid.GetHidGuid(&InterfaceClassGuid);
    DeviceInfoTable = SetupDiGetClassDevs(&InterfaceClassGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if(DeviceInfoTable == INVALID_HANDLE_VALUE) {
        return yWinSetErr(errmsg);
    }
    pDetailedInterfaceData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_A) buffer;
    // allocate buffer for detected interfaces
    *nbifaceDetect = 0;
    nbifaceAlloc   = yContext->prevEnumCnt + 8;
    *ifaces = (yInterfaceSt*) yMalloc(nbifaceAlloc * sizeof(yInterfaceSt));
    memset(*ifaces,0,nbifaceAlloc * sizeof(yInterfaceSt));

    //Now look through the list we just populated.  We are trying to see if any of them match our device.
    for(;;index++){
        u32             vendorid,deviceid,release,ifaceno,inst_state;
        SP_DEVINFO_DATA DevInfoData;

        DevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        if(!SetupDiEnumDeviceInfo(DeviceInfoTable, index, &DevInfoData)){
            //no more device
            break;
        }
        // get device hardwareid string
        SetupDiGetDeviceRegistryPropertyA(DeviceInfoTable, &DevInfoData, SPDRP_HARDWAREID, NULL, (PBYTE)buffer, 512, NULL);
        // skip 4 first char ("HID/")
        DecodeHardwareid(buffer+4, &vendorid,&deviceid,&release, &ifaceno);
        // get installation status see:http://msdn.microsoft.com/en-us/library/ff543130
        SetupDiGetDeviceRegistryProperty(DeviceInfoTable, &DevInfoData, SPDRP_INSTALL_STATE, NULL, (PBYTE) &inst_state, sizeof(inst_state), &needsize);
        if(vendorid==YOCTO_VENDORID && inst_state==0){
            SP_DEVICE_INTERFACE_DATA    InterfaceData;
            size_t          len;
            yInterfaceSt    *iface;
			int             find,retry = 16;

			//ensure the buffer of detected interface is big enought
			if(*nbifaceDetect == nbifaceAlloc){
				yInterfaceSt    *tmp;
				u32 newsize = nbifaceAlloc*2 * sizeof(yInterfaceSt);
				tmp = (yInterfaceSt*) yMalloc(newsize);
				memset(tmp,0,newsize);
				yMemcpy(tmp,*ifaces, nbifaceAlloc * sizeof(yInterfaceSt) );
				yFree(*ifaces);
				*ifaces = tmp;
				nbifaceAlloc    *=2;
			}
			iface = *ifaces + *nbifaceDetect;
			iface->vendorid = (u16)vendorid;
			iface->deviceid = (u16)deviceid;
			iface->ifaceno  = (u16)ifaceno;

			InterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
            if(!SetupDiEnumDeviceInterfaces(DeviceInfoTable, &DevInfoData, &InterfaceClassGuid, 0, &InterfaceData)){
				dbglog("Fail to retrieve DeviceInterfaces");
                continue;
            }
            SetupDiGetDeviceInterfaceDetailA(DeviceInfoTable, &InterfaceData, NULL, 0, &needsize, NULL);
            if(WIN_DEVICE_PATH_LEN < needsize){
                dbglog("buffer too small");
                continue;
            }
            pDetailedInterfaceData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);
            SetupDiGetDeviceInterfaceDetailA(DeviceInfoTable, &InterfaceData, pDetailedInterfaceData, needsize, NULL, NULL);
			YSTRNCPY(iface->devicePath,WIN_DEVICE_PATH_LEN,pDetailedInterfaceData->DevicePath,WIN_DEVICE_PATH_LEN);
			for(find = 0; find < yContext->prevEnumCnt; find++) {
				if(YSTRCMP(iface->devicePath,yContext->prevEnum[find].devicePath)==0) break;
			}
			if(find < yContext->prevEnumCnt) {
				yMemcpy(iface->serial, yContext->prevEnum[find].serial, YOCTO_SERIAL_LEN*2);
			} else {
				HANDLE          control;
				HALLOG("Get serial for %s\n",pDetailedInterfaceData->DevicePath);
				control = CreateFileA(pDetailedInterfaceData->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
				if(control==INVALID_HANDLE_VALUE){
					dbglog("Unable to open device %s to get serial\n",pDetailedInterfaceData->DevicePath);
					continue;
				}
				while(!yContext->hid.GetSerialNumberString(control,buffer,WIN_DEVICE_PATH_LEN) && --retry >= 0) {
					dbglog("Unable to get serial for %s (%d), retrying (%d)\n",pDetailedInterfaceData->DevicePath,GetLastError(),retry);
					Sleep(17);
				}
				if(retry < 0) {
					dbglog("Unable to get serial for %s (%d), giving up\n",pDetailedInterfaceData->DevicePath, GetLastError());
					CloseHandle(control);
					continue;
				}
	#ifdef _MSC_VER
				wcstombs_s(&len,iface->serial,YOCTO_SERIAL_LEN*2,(wchar_t*)buffer,_TRUNCATE);
	#else
				len = wcstombs(iface->serial,(wchar_t*)buffer,YOCTO_SERIAL_LEN*2);
	#endif
	            CloseHandle(control);
			}
            (*nbifaceDetect)++;
            if(deviceid>YOCTO_DEVID_BOOTLOADER){
                HALLOG("----Running Dev %x:%x:%d:%s ---\n",vendorid,deviceid,ifaceno,iface->serial);
            }else{
                HALLOG("----Running Firm %x:%x:%d:%s ---\n",vendorid,deviceid,ifaceno,iface->serial);
            }
        } else {
			HALLOG("Drop device vendorid=%x inst_state=%x\n", vendorid, inst_state);
		}
    }
    // unallocate Device infos
    if(!SetupDiDestroyDeviceInfoList(DeviceInfoTable)){
        HALLOG("Unable to unallocated Device Info Table  (%d)",GetLastError());
    }
	// save enumeration result to prevent later USB packets to redetect serial
	if(yContext->prevEnum) yFree(yContext->prevEnum);
	yContext->prevEnumCnt = *nbifaceDetect;
	if(*nbifaceDetect>0){
		yContext->prevEnum = (yInterfaceSt*) yMalloc(*nbifaceDetect * sizeof(yInterfaceSt));
		yMemcpy(yContext->prevEnum, *ifaces, *nbifaceDetect * sizeof(yInterfaceSt));
	}else{
		yContext->prevEnum = NULL;
	}

    return YAPI_SUCCESS;
}


// return 1 if OS hdl are identicals
//        0 if any of the interface has changed
int yyyOShdlCompare( yPrivDeviceSt *dev, DevEnum *newdev)
{
    int i,j, nbifaces;
    if(dev->infos.nbinbterfaces != newdev->nbifaces){
        HALLOG("bad number of inteface for %s (%d:%d)\n",dev->infos.serial,dev->infos.nbinbterfaces, newdev->nbifaces);
        return 0;
    }
    nbifaces=newdev->nbifaces;

    for (i =0 ; i < nbifaces ;i++) {
        for (j =0 ; j < nbifaces ;j++) {
            if(YSTRCMP(dev->ifaces[i].devicePath,newdev->ifaces[j]->devicePath)==0){
                break;
            }
        }
        if(j==nbifaces)
            break;
    }
    if( i < nbifaces ) {
        HALLOG("devref %d has changed for %s (%s)\n",i,dev->infos.serial,dev->ifaces[i].devicePath);
        return 0;
    }

    return 1;
}



 /*****************************************************************
 * Window implementation of yyypacket functions
*****************************************************************/


int yyySetup(yInterfaceSt *iface,char *errmsg)
{
    yyyInitPktQueue(iface);
    iface->wrHDL = INVALID_HANDLE_VALUE;
    iface->rdEV = NULL;
    iface->rdHDL = INVALID_HANDLE_VALUE;
    iface->wrHDL = CreateFileA(iface->devicePath, GENERIC_WRITE |GENERIC_READ,
                                        FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
    if(iface->wrHDL == INVALID_HANDLE_VALUE){
        HALLOG("Iface hdl error %x %s\n",iface->ifaceno,iface->serial);
        return yWinSetErr(errmsg);
    }
    iface->rdHDL = CreateFileA(iface->devicePath, GENERIC_WRITE |GENERIC_READ,
                                        FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
    if(iface->rdHDL == INVALID_HANDLE_VALUE){
        HALLOG("Iface hdl error %x %s\n",iface->ifaceno,iface->serial);
        return yWinSetErr(errmsg);
    }
    iface->rdEV = CreateEvent(NULL, TRUE, TRUE, NULL);
    if(iface->rdEV == NULL){
        HALLOG("Iface event creation error %x %s\n",iface->ifaceno,iface->serial);
        return yWinSetErr(errmsg);
    }
    iface->rdpending=0;
    HALLOG("yyySetup I%x wr=%x rd=%x se=%s\n",iface->ifaceno,iface->wrHDL, iface->rdHDL,iface->serial);
    return YAPI_SUCCESS;
}





int yyyRead(yInterfaceSt *iface,char *errmsg)
{
    DWORD readed;
 retry:
    //HALLOG("   yyyRead %d \n",iface->ifaceno);
    memset(&iface->rdOL,0,sizeof(iface->rdOL));
    if(!SetEvent(iface->rdEV)) return yWinSetErr(errmsg);
    iface->rdOL.hEvent = iface->rdEV;
    YASSERT(iface->rdpending==0);
    iface->rdpending++;
    if(!ReadFile(iface->rdHDL, &iface->tmpd2hpkt, sizeof(OS_USB_Packet), &readed, &iface->rdOL)){
        u32 error= GetLastError();
        if (error != ERROR_IO_PENDING){
            iface->rdpending--;
            HALLOG("packet triger error\n");
            return yWinSetErrEx(__LINE__,error,errmsg);
        }
    }else{
         //HALLOG("blocking read %d\n",iface->ifaceno);
         iface->rdpending--;
         //HALLOG("   yyyReadResult %d\n",iface->ifaceno);
         yyPushNewPkt(iface,&iface->tmpd2hpkt.pkt);
         goto retry;
    }
    return YAPI_SUCCESS;
}





//Look if we have new packet arrived
int yyyReadIdle(yInterfaceSt *iface,char *errmsg)
{
    DWORD           readed;

    YASSERT(iface->rdpending ==1);
    if(GetOverlappedResult(iface->rdHDL,&iface->rdOL,&readed,0)){
        //finished
        iface->rdpending--;
        //HALLOG("   yyyReadResult %d\n",iface->ifaceno);
        if(readed!=sizeof(OS_USB_Packet)){
            HALLOG("invalid packet size read %d  %d\n",iface->ifaceno,readed);
        }
        yyPushNewPkt(iface,&iface->tmpd2hpkt.pkt);
        memset(&iface->tmpd2hpkt.pkt,0xff,sizeof(iface->tmpd2hpkt.pkt));
        return yyyRead(iface,errmsg);

    }else{
        u32 error=GetLastError();
        if (error != ERROR_IO_INCOMPLETE){
            iface->rdpending--;
            HALLOG("   yyyReadERROR %d\n",iface->ifaceno);
            return yWinSetErrEx(__LINE__,error,errmsg);
        }
        return YAPI_SUCCESS;
    }
}


int yyyReadStop(yInterfaceSt *iface,char *errmsg)
{
    DWORD readed;

    //HALLOG("   yyyReadStop %d\n",iface->ifaceno);
    if(iface->rdpending){
        if(CancelIo(iface->rdHDL)==0){
            HALLOG("CancelIo failed with %d\n",GetLastError());
        }
        if(GetOverlappedResult(iface->rdHDL,&iface->rdOL,&readed,TRUE)){
            //finished
            if(readed!=sizeof(OS_USB_Packet)){
                HALLOG("invalid packet size read %d  %d\n",iface->ifaceno,readed);
            }
            yyPushNewPkt(iface,&iface->tmpd2hpkt.pkt);
        }else{
            u32 error=GetLastError();
            if (error != ERROR_OPERATION_ABORTED){
                return yWinSetErrEx(__LINE__,error,errmsg);
            }
        }
        iface->rdpending--;
        //HALLOG("   yyyReadStoped %d\n",iface->ifaceno);
        memset(&iface->tmpd2hpkt.pkt,0xff,sizeof(iface->tmpd2hpkt.pkt));
    }
    YASSERT(iface->rdpending==0);
    return YAPI_SUCCESS;
}



int yyyWrite(yInterfaceSt *iface,USB_Packet *pkt,char *errmsg)
{
    DWORD           written;
    OS_USB_Packet   winpkt;

    //HALLOG(" - write on iface %d(%d) (flush)\n",iface->ifaceno, idx);
    winpkt.dummy=0;
    memcpy(&winpkt.pkt,pkt,sizeof(USB_Packet));
    if(!WriteFile(iface->wrHDL,&winpkt,sizeof(OS_USB_Packet),&written,NULL)){
        HALLOG(" - write on iface failed %d\n",iface->ifaceno);
        return yWinSetErr(errmsg);
    }
    return YAPI_SUCCESS;
}

void yyyPacketShutdown(yInterfaceSt *iface)
{
    //HALLOG("yyyPacketShutdown\n");
    if(iface->wrHDL != INVALID_HANDLE_VALUE){
        CloseHandle(iface->wrHDL);
        iface->wrHDL = INVALID_HANDLE_VALUE;
    }
    if(iface->rdEV != NULL){
        CloseHandle(iface->rdEV);
        iface->rdEV = NULL;
    }
    if(iface->rdHDL != INVALID_HANDLE_VALUE){
        CloseHandle(iface->rdHDL);
        iface->rdHDL = INVALID_HANDLE_VALUE;
    }
    yyyFreePktQueue(iface);
}

#endif


#ifdef WINCE
#include "yproto.h"


int yUSB_init(yContextSt *ctx,char *errmsg)
{
    return YAPI_SUCCESS;
}


int yUSB_stop(yContextSt *ctx,char *errmsg)
{
    return 0;
}



int yUSBGetInterfaces(yInterfaceSt **ifaces,int *nbifaceDetect,char *errmsg)
{
    *nbifaceDetect = 0;
    return 0;
}



int yyyTestOShdl( yPrivDeviceSt *dev, DevEnum *newdev)
{
    return 1;
}



int yyySetup(yInterfaceSt *iface,char *errmsg)
{
    return YERR(YAPI_NOT_SUPPORTED);
}

int yyyRead(yInterfaceSt *iface,char *errmsg)
{
    return YERR(YAPI_NOT_SUPPORTED);
}

int yyyReadIdle(yInterfaceSt *iface,char *errmsg)
{
    return YERR(YAPI_NOT_SUPPORTED);
}

int yyyReadStop(yInterfaceSt *iface,char *errmsg)
{
    return YERR(YAPI_NOT_SUPPORTED);
}

int yyyWrite(yInterfaceSt *iface,USB_Packet *pkt,char *errmsg)
{
    return YERR(YAPI_NOT_SUPPORTED);
}

void yyyPacketShutdown(yInterfaceSt  *iface)
{}

#endif
