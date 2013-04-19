/*********************************************************************
 *
 * $Id: ypkt_osx.c 10877 2013-04-04 14:30:47Z mvuilleu $
 *
 * OS-specific USB packet layer, Mac OS X version
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

#define __FILE_ID__ "ypkt_osx"
#include "yapi.h"
#ifdef OSX_API
#include "yproto.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 /*****************************************************************
 * USB ENUMERATION
*****************************************************************/


#define YOCTO_LOCK_PIPE "/tmp/.yoctolock"

// return 1 if we can reserve access to the device 0 if the device
// is already reserved
static int yReserveGlobalAccess(yContextSt *ctx)
{
    int fd;
    int chk_val;
    size_t res;

    mkfifo(YOCTO_LOCK_PIPE,0600);
    fd = open(YOCTO_LOCK_PIPE,O_RDWR|O_NONBLOCK);
    if(fd<0){
        // we cannot open lock file so we cannot realy
        // check double instance so we asume that we are
        // alone
        return 1;
    }
    chk_val=0;
    res = read(fd,&chk_val,sizeof(chk_val));
    if(res==sizeof(chk_val)){
        //there is allready someone
        chk_val=1;
    }
    write(fd,&chk_val,sizeof(chk_val));
    if(chk_val==1)
        return 0;
    return 1;
}



static void yReleaseGlobalAccess(yContextSt *ctx)
{
    int chk_val;
    int fd = open(YOCTO_LOCK_PIPE,O_RDWR|O_NONBLOCK);
    if(fd>=0){
        read(fd,&chk_val,sizeof(chk_val));
    }
}



static void hid_device_removal_callback(void *context, IOReturn result,    void *sender, IOHIDDeviceRef dev_ref)
{
    yContextSt *ctx =(yContextSt*)context;
    yPrivDeviceSt *p=ctx->devs;
    //dbglog("handle device removal correctly\n");
    while(p){
        int i;
        for(i =0 ; i< p->infos.nbinbterfaces ;i++){
            yInterfaceSt *iface = &p->ifaces[i];
            if(iface->devref == dev_ref){
                if(iface->flags.yyySetupDone) {
                    yEnterCriticalSection(&iface->yyyCS);
                }
                HALLOG("unplug of %s:%d detected\n",iface->serial,iface->ifaceno);
                iface->devref=NULL;
                if(iface->flags.yyySetupDone) {
                    yLeaveCriticalSection(&iface->yyyCS);
                }
                break;
            }
        }
        p=p->next;
    }
}




static void *event_thread(void *param)
{   
    yContextSt  *ctx=param;
    
    ctx->usb_run_loop     = CFRunLoopGetCurrent();
    ctx->usb_thread_state = USB_THREAD_RUNNING;
    /* Non-blocking. See if the OS has any reports to give. */
    HALLOG("Start event_thread run loop\n");
    while (ctx->usb_thread_state != USB_THREAD_MUST_STOP) {
        CFRunLoopRunInMode( kCFRunLoopDefaultMode, 10, FALSE);
    }

    HALLOG("event_thread run loop stoped\n");
    ctx->usb_thread_state = USB_THREAD_STOPED;
    return NULL;
}





int yUSB_init(yContextSt *ctx,char *errmsg)
{
    int             c_vendorid = YOCTO_VENDORID;
    IOReturn        tIOReturn;
    CFMutableDictionaryRef dictionary;
    CFNumberRef     Vendorid;

    if(!yReserveGlobalAccess(ctx)){
        return YERRMSG(YAPI_DOUBLE_ACCES,"Another process is already using yAPI");
    }
    ctx->usb_thread_state = USB_THREAD_NOT_STARTED;
    pthread_create(&ctx->usb_thread, NULL, event_thread, ctx);
    


    yInitializeCriticalSection(&ctx->hidMCS);
    // Initialize HID Manager
    ctx->hidM = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    // create dictionary to match Yocto devices
    dictionary = CFDictionaryCreateMutable(kCFAllocatorDefault,1,&kCFTypeDictionaryKeyCallBacks,&kCFTypeDictionaryValueCallBacks);
    Vendorid = CFNumberCreate( kCFAllocatorDefault, kCFNumberIntType, &c_vendorid );
    CFDictionarySetValue( dictionary, CFSTR( kIOHIDVendorIDKey ), Vendorid );
    // register the dictionary
    IOHIDManagerSetDeviceMatching( ctx->hidM, dictionary );
    // now we can release the dictionary
    CFRelease(dictionary);
    IOHIDManagerRegisterDeviceRemovalCallback(ctx->hidM,hid_device_removal_callback,ctx);
      
    //register hid into read_thead's RunnLoop
    while(ctx->usb_thread_state != USB_THREAD_RUNNING){
        usleep(50000);
    }

    IOHIDManagerScheduleWithRunLoop(ctx->hidM, ctx->usb_run_loop, kCFRunLoopDefaultMode);
    // Now open the IO HID Manager reference
    tIOReturn = IOHIDManagerOpen( ctx->hidM, kIOHIDOptionsTypeNone );
    CFRelease(Vendorid);
    if(kIOReturnSuccess != tIOReturn ||CFGetTypeID(ctx->hidM) != IOHIDManagerGetTypeID()){
        HALLOG("Unable to Open HID Manager");
        return YERRMSG(YAPI_NOT_SUPPORTED,"Unable to Open HID Manager");        
    }
    
    return YAPI_SUCCESS;
}


int yUSB_stop(yContextSt *ctx,char *errmsg)
{

    if ( ctx->hidM ) {
        IOHIDManagerUnscheduleFromRunLoop(ctx->hidM, ctx->usb_run_loop, kCFRunLoopDefaultMode);
        CFRelease( ctx->hidM );
        ctx->hidM=NULL;
    }
    
    
    if(ctx->usb_thread_state == USB_THREAD_RUNNING){
        ctx->usb_thread_state = USB_THREAD_MUST_STOP;
        CFRunLoopStop(ctx->usb_run_loop);       
    }
    pthread_join(ctx->usb_thread,NULL); 
    YASSERT(ctx->usb_thread_state == USB_THREAD_STOPED);

    yDeleteCriticalSection(&ctx->hidMCS);
    yReleaseGlobalAccess(ctx);

    return 0;
}




static u32 get_int_property(IOHIDDeviceRef device, CFStringRef key)
{
    CFTypeRef ref;
    u32 value;
    
    ref = IOHIDDeviceGetProperty(device, key);
    if (ref) {
        if (CFGetTypeID(ref) == CFNumberGetTypeID() && CFNumberGetValue((CFNumberRef) ref, kCFNumberSInt32Type, &value)) {
            return value;
        }
    }
    return 0;
}


static void get_txt_property(IOHIDDeviceRef device,char *buffer,u32 maxlen, CFStringRef key)
{
    CFTypeRef ref;
    size_t  len;
    
    ref = IOHIDDeviceGetProperty(device, key);
    if (ref) {
        if (CFGetTypeID(ref) == CFStringGetTypeID()) {
            const char *str;
            CFStringEncoding encodingMethod;
            encodingMethod = CFStringGetSystemEncoding();
            // 1st try for English system
            str = CFStringGetCStringPtr(ref, encodingMethod);
            //str = NULL;
            if ( str == NULL ) {
                // 2nd try 
                encodingMethod = kCFStringEncodingUTF8;
                str = CFStringGetCStringPtr(ref, encodingMethod);
            }
            if( str == NULL ) {
                //3rd try
                CFIndex cflength = CFStringGetLength(ref)*2+2;
                char *tmp_str = yMalloc( (u32)cflength);
                if (!CFStringGetCString(ref, tmp_str, cflength, kCFStringEncodingUTF8 )) {
                    yFree( tmp_str );
                    *buffer=0;
                    return;
                }
                if(cflength>maxlen-1){
                    cflength=maxlen-1;
                }
                memcpy(buffer,tmp_str,cflength);
                buffer[cflength]=0;
                yFree( tmp_str );
                return;
            }
            len=strlen(str);
            if(len>maxlen-1){
                len=maxlen-1;
            }
            memcpy(buffer,str,len);
            buffer[len]=0;
            return;
        }
    }
    *buffer=0;
}





int yUSBGetInterfaces(yInterfaceSt **ifaces,int *nbifaceDetect,char *errmsg)
{
    int             nbifaceAlloc;
    int             deviceIndex;
   
    CFSetRef        deviceCFSetRef;
    CFIndex         deviceCount;
    IOHIDDeviceRef  *dev_refs;
    
    // allocate buffer for detected interfaces
    *nbifaceDetect = 0;
    nbifaceAlloc  = 8;
    *ifaces =yMalloc(nbifaceAlloc * sizeof(yInterfaceSt));
    memset(*ifaces, 0 ,nbifaceAlloc * sizeof(yInterfaceSt));
            
    
    yEnterCriticalSection(&yContext->hidMCS);
    deviceCFSetRef = IOHIDManagerCopyDevices( yContext->hidM );
    yLeaveCriticalSection(&yContext->hidMCS);
    if(deviceCFSetRef== NULL){
        //no device found
        return 0;
    }
    
    // how many devices in the set?
    deviceCount = CFSetGetCount( deviceCFSetRef ); 
    HALLOG("%ld usb interfaces detected\n",deviceCount);
    
    dev_refs = yMalloc( sizeof(IOHIDDeviceRef) * (u32)deviceCount );
    
    // now extract the device ref's from the set
    CFSetGetValues( deviceCFSetRef, (const void **) dev_refs );
    
    for(deviceIndex=0 ; deviceIndex < deviceCount ;deviceIndex++){
        u16 vendorid;
        u16 deviceid;
        IOHIDDeviceRef dev = dev_refs[deviceIndex];
        yInterfaceSt    *iface;
        vendorid = get_int_property(dev,CFSTR(kIOHIDVendorIDKey));
        deviceid = get_int_property(dev,CFSTR(kIOHIDProductIDKey));
        //ensure the buffer of detected interface is big enought
        if(*nbifaceDetect == nbifaceAlloc){
            yInterfaceSt    *tmp;
            tmp = (yInterfaceSt*) yMalloc(nbifaceAlloc*2 * sizeof(yInterfaceSt));
            memset(tmp,0,nbifaceAlloc*2 * sizeof(yInterfaceSt));
            yMemcpy(tmp,*ifaces, nbifaceAlloc * sizeof(yInterfaceSt) );
            yFree(*ifaces);
            *ifaces = tmp;
            nbifaceAlloc    *=2;
        }
        iface = *ifaces + *nbifaceDetect;
        iface->devref   = dev;
        iface->vendorid = vendorid;
        iface->deviceid = deviceid;
        get_txt_property(dev,iface->serial,YOCTO_SERIAL_LEN*2, CFSTR(kIOHIDSerialNumberKey));
        HALLOG("work on interface %d (%x:%x:%s)\n",deviceIndex,vendorid,deviceid,iface->serial);
        (*nbifaceDetect)++;
    }
    CFRelease(deviceCFSetRef);
    yFree(dev_refs);
    return YAPI_SUCCESS;
}


 /*****************************************************************
 * OSX implementation of yyypacket functions
*****************************************************************/




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
            if(dev->ifaces[i].devref == newdev->ifaces[j]->devref){
                break;
            }
        }
        if(j==nbifaces)
            break;
    }
    if( i < nbifaces ) {
        HALLOG("devref %d has changed for %s (%X)\n",i,dev->infos.serial,dev->ifaces[i].devref);
        return 0;
    }

    return 1;
}




static void Handle_IOHIDDeviceIOHIDReportCallback(
                void *          inContext,          // context from IOHIDDeviceRegisterInputReportCallback
                IOReturn        inResult,           // completion result for the input report operation
                void *          inSender,           // IOHIDDeviceRef of the device this report is from
                IOHIDReportType inType,             // the report type
                uint32_t        inReportID,         // the report ID
                uint8_t *       inReport,           // pointer to the report data
                CFIndex         InReportLength)     // the actual size of the input report
{
    yInterfaceSt *iface= (yInterfaceSt*) inContext;
    yEnterCriticalSection(&iface->yyyCS);
    yyPushNewPkt(iface,&iface->tmprxpkt);
    memset(&iface->tmprxpkt,0xff,sizeof(USB_Packet));
    yLeaveCriticalSection(&iface->yyyCS);
}

int yyySetup(yInterfaceSt *iface,char *errmsg)
{  
    char str[32];

    yyyInitPktQueue(iface);
    yInitializeCriticalSection(&iface->yyyCS);
    yEnterCriticalSection(&iface->yyyCS);
    iface->flags.yyySetupDone = 1;
    if(iface->devref==NULL){
        // device has disappeared in between, too bad
        iface->flags.yyySetupDone = 0;
        yLeaveCriticalSection(&iface->yyyCS);
        yDeleteCriticalSection(&iface->yyyCS);
        yyyFreePktQueue(iface);
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }

    IOReturn ret = IOHIDDeviceOpen(iface->devref, kIOHIDOptionsTypeNone);
    if (ret != kIOReturnSuccess) {
        iface->flags.yyySetupDone = 0;
        yLeaveCriticalSection(&iface->yyyCS);
        yDeleteCriticalSection(&iface->yyyCS);
        yyyFreePktQueue(iface);
        YSPRINTF(str,32,"Unable to open device (0x%x)",ret);
        return YERRMSG(YAPI_IO_ERROR,str);
    }
    
    /* Create the Run Loop Mode for this device. printing the reference seems to work. */
    sprintf(str, "yocto_%p", iface->devref);
    iface->run_loop_mode = CFStringCreateWithCString(NULL, str, kCFStringEncodingASCII);

    /* Attach the device to a Run Loop */
    IOHIDDeviceScheduleWithRunLoop(iface->devref, yContext->usb_run_loop, iface->run_loop_mode);
    IOHIDDeviceRegisterInputReportCallback( iface->devref,              // IOHIDDeviceRef for the HID device
                                            (u8*) &iface->tmprxpkt,     // pointer to the report data 
                                            USB_PKT_SIZE,               // number of bytes in the report (CFIndex)
                                            &Handle_IOHIDDeviceIOHIDReportCallback,   // the callback routine
                                            iface);                     // context passed to callback
    yLeaveCriticalSection(&iface->yyyCS);

    return 0;
}


int yyyRead(yInterfaceSt *iface,char *errmsg)
{
    yEnterCriticalSection(&iface->yyyCS);
    if(iface->devref==NULL){
        yLeaveCriticalSection(&iface->yyyCS);
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }
    yLeaveCriticalSection(&iface->yyyCS);
    return 0;
}





//return 0 if a read is still pending
int yyyReadIdle(yInterfaceSt *iface,char *errmsg)
{
    yEnterCriticalSection(&iface->yyyCS);
    if(iface->devref==NULL){
        yLeaveCriticalSection(&iface->yyyCS);
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }
    yLeaveCriticalSection(&iface->yyyCS);
    return  YAPI_SUCCESS;
}




int yyyReadStop(yInterfaceSt *iface,char *errmsg)
{
   return 0;
}


int yyyWrite(yInterfaceSt *iface,USB_Packet *pkt,char *errmsg)
{
    IOReturn res;
    yEnterCriticalSection(&iface->yyyCS);
    if(iface->devref==NULL){
        yLeaveCriticalSection(&iface->yyyCS);
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }
    res = IOHIDDeviceSetReport(iface->devref,
                   kIOHIDReportTypeOutput,
                   0, /* Report ID*/
                   (u8*)pkt, sizeof(USB_Packet));
    yLeaveCriticalSection(&iface->yyyCS);
    if (res != kIOReturnSuccess) {
        dbglog("IOHIDDeviceSetReport failed with 0x%x\n", res);
        return YERR(YAPI_IO_ERROR);
    }
    return 0;
}


void yyyPacketShutdown(yInterfaceSt  *iface)
{
    yEnterCriticalSection(&iface->yyyCS);
    if(iface->devref!=NULL){
        IOHIDDeviceRegisterInputReportCallback( iface->devref,              // IOHIDDeviceRef for the HID device
                                            (u8*) &iface->tmprxpkt,   // pointer to the report data (uint8_t's)
                                            USB_PKT_SIZE,              // number of bytes in the report (CFIndex)
                                            NULL,   // the callback routine
                                            iface);                      // context passed to callback
        IOHIDDeviceClose(iface->devref, kIOHIDOptionsTypeNone);
        iface->devref=NULL;
    }
    iface->flags.yyySetupDone = 0;
    yLeaveCriticalSection(&iface->yyyCS);
    yDeleteCriticalSection(&iface->yyyCS);
    yyyFreePktQueue(iface);

}

#endif

#ifdef IOS_API
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



int yyyOShdlCompare( yPrivDeviceSt *dev, DevEnum *newdev)
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
