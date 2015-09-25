/*********************************************************************
 *
 * $Id: ypkt_lin.c 21565 2015-09-18 13:22:27Z seb $
 *
 * OS-specific USB packet layer, Linux version
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

#define __FILE_ID__  "ypkt_lin"
#include "yapi.h"
#ifdef LINUX_API
#include "yproto.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define yLinSetErr(intro, err,errmsg)  yLinSetErrEx(__LINE__,intro, err,errmsg)

static int yLinSetErrEx(u32 line,char *intro, int err,char *errmsg)
{
    const char *msg;
    if(errmsg==NULL)
        return YAPI_IO_ERROR;
     switch(err){
        case LIBUSB_SUCCESS:            msg="Success (no error)";break;
        case LIBUSB_ERROR_IO:           msg="Input/output error"; break;
        case LIBUSB_ERROR_INVALID_PARAM:msg="Invalid parameter"; break;
        case LIBUSB_ERROR_ACCESS:       msg="Access denied (insufficient permissions)"; break;
        case LIBUSB_ERROR_NO_DEVICE:    msg="No such device (it may have been disconnected)"; break;
        case LIBUSB_ERROR_NOT_FOUND:    msg="Entity not found"; break;
        case LIBUSB_ERROR_BUSY:         msg="Resource busy"; break;
        case LIBUSB_ERROR_TIMEOUT:      msg="Operation timed out"; break;
        case LIBUSB_ERROR_OVERFLOW:     msg="Overflow"; break;
        case LIBUSB_ERROR_PIPE:         msg="Pipe error"; break;
        case LIBUSB_ERROR_INTERRUPTED:  msg="System call interrupted (perhaps due to signal)"; break;
        case LIBUSB_ERROR_NO_MEM:       msg="Insufficient memory"; break;
        case LIBUSB_ERROR_NOT_SUPPORTED:msg="Operation not supported or unimplemented on this platform"; break;
        default:
        case LIBUSB_ERROR_OTHER:        msg="Other error"; break;
    }
    if (intro){
        YSPRINTF(errmsg,YOCTO_ERRMSG_LEN,"%s:%s",intro,msg);
    } else{
        YSPRINTF(errmsg,YOCTO_ERRMSG_LEN,"LIN(%d):%s",line,msg);
    }
    HALLOG("LIN(%d):%s\n",line,msg);
    return YAPI_IO_ERROR;
};


 /*****************************************************************
 * USB ENUMERATION
*****************************************************************/


#define YOCTO_LOCK_PIPE "/tmp/.yoctolock"

// return 1 if we can reserve access to the device 0 if the device
// is already reserved
static int yReserveGlobalAccess(yContextSt *ctx, char *errmsg)
{
    int fd;
    int chk_val, mypid, usedpid = 0;
    size_t res;
    mode_t mode=0666;
    mode_t oldmode = umask(0000);
    char msg[YOCTO_ERRMSG_LEN];

    HALLOG("old mode (%#o)\n",oldmode);
    HALLOG("create fifo with (%#o)\n",mode);
    if(mkfifo(YOCTO_LOCK_PIPE,mode)<0) {
        HALLOG("unable to create lock fifo (%d:%s)\n",errno,strerror(errno));
    }
    umask(oldmode);
    fd = open(YOCTO_LOCK_PIPE,O_RDWR|O_NONBLOCK);
    if(fd<0){
        HALLOG("unable to open lock fifo (%d)\n",errno);
        if(errno==EACCES) {
            return YERRMSG(YAPI_DOUBLE_ACCES, "we do not have acces to lock fifo");
        }else{
            // we cannot open lock file so we cannot realy
            // check double instance so we asume that we are
            // alone
            return YAPI_SUCCESS;
        }
    }
    chk_val = 0;
    mypid = (int) getpid();
    res = read(fd, &chk_val, sizeof(chk_val));
    if (res == sizeof(chk_val)) {
        //there is allready someone
        usedpid = chk_val;
    } else{
        // nobody there -> store my PID
        chk_val = mypid;
    }
    res = write(fd, &chk_val, sizeof(chk_val));
    if(res != sizeof(chk_val)) {
        YSPRINTF(msg, YOCTO_ERRMSG_LEN, "Write to lock fifo failed (%d)", res);
        return YERRMSG(YAPI_DOUBLE_ACCES, msg);
    }
    if (usedpid != 0) {
        if (usedpid == 1) {
            // locked by api that not store the pid
            return YERRMSG(YAPI_DOUBLE_ACCES, "Another process is already using yAPI");
        } else {
            YSPRINTF(msg, YOCTO_ERRMSG_LEN, "Another process (pid %d) is already using yAPI", (u32) usedpid);
            return YERRMSG(YAPI_DOUBLE_ACCES, msg);
        }
    }
    return YAPI_SUCCESS;
}


size_t dropwarning;

static void yReleaseGlobalAccess(yContextSt *ctx)
{
    int chk_val;
    int fd = open(YOCTO_LOCK_PIPE,O_RDWR|O_NONBLOCK);
    if(fd>=0){
        dropwarning = read(fd,&chk_val,sizeof(chk_val));
      }
}

typedef struct {
    libusb_device *dev;
    int desc_index;
    int len;
    char *string;
    u64 expiration;
} stringCacheSt;

#define STRING_CACHE_SIZE 16
#define STRING_CACHE_EXPIRATION 60000 //1 minutes
static stringCacheSt stringCache[STRING_CACHE_SIZE];


// on success data point to a null terminated string of max length-1 characters
static int getUsbStringASCII(libusb_device_handle *hdl, libusb_device *dev, u8 desc_index, char *data, u32 length)
{
    u8  buffer[512];
    u32 l,len;
    int res,i;
    stringCacheSt *c = stringCache;
    stringCacheSt *f = NULL;
    u64 now = yapiGetTickCount();

    yEnterCriticalSection(&yContext->string_cache_cs);

    for (i = 0; i < STRING_CACHE_SIZE; i++, c++) {
        if (c->expiration > now) {
            if(c->dev == dev && c->desc_index == desc_index) {
                if (c->len > 0 && c->string) {
                    len = c->len;
                    if (c->len >= length)
                        len = length-1;
                    memcpy(data, c->string,  len);
                    data[len] = 0;
                    HALLOG("return string from cache (%p:%d->%s)\n",dev,desc_index,c->string);
                    yLeaveCriticalSection(&yContext->string_cache_cs);
                    return c->len;
                } else {
                    f = c;
                    break;
                }
            }
        } else {
            if (c->string) {
                yFree(c->string);
                c->string =NULL;
            }
            if (f == NULL) {
                f = c;
            }
        }
    }

    res=libusb_control_transfer(hdl, LIBUSB_ENDPOINT_IN,
        LIBUSB_REQUEST_GET_DESCRIPTOR, (LIBUSB_DT_STRING << 8) | desc_index,
        0, buffer, 512, 10000);
    if(res<0) return res;

    len=(buffer[0]-2)/2;
    if (len >= length) {
        len = length - 1;
    }

    for (l = 0; l < len; l++){
        data[l] = (char) buffer[2+(l*2)];
    }
    data[len] = 0;

    if (f != NULL) {
        f->dev = dev;
        f->desc_index = desc_index;
        f->string = yMalloc(len+1);
        memcpy(f->string, data, len+1);
        f->len  = len;
        f->expiration = yapiGetTickCount() + STRING_CACHE_EXPIRATION;
        HALLOG("add string to cache (%p:%d->%s)\n",dev,desc_index,f->string);
    }
    yLeaveCriticalSection(&yContext->string_cache_cs);

    return len;
}


static void *event_thread(void *param)
{
    yContextSt *ctx = (yContextSt*)param;
    char            errmsg[YOCTO_ERRMSG_LEN];
    ctx->usb_thread_state = USB_THREAD_RUNNING;
    /* Non-blocking. See if the OS has any reports to give. */
    HALLOG("Start event_thread run loop\n");
    while (ctx->usb_thread_state != USB_THREAD_MUST_STOP) {
        int res;
        struct timeval tv;
        memset(&tv,0,sizeof(tv));
        tv.tv_sec = 1;
        res = libusb_handle_events_timeout(ctx->libusb, &tv);
        if (res < 0) {
            yLinSetErr("libusb_handle_events_timeout", res,errmsg);
            break;
        }
    }
    HALLOG("event_thread run loop stoped\n");
    ctx->usb_thread_state = USB_THREAD_STOPED;
    return NULL;
}


int yyyUSB_init(yContextSt *ctx,char *errmsg)
{
    int res;

    YPROPERR(yReserveGlobalAccess(ctx, errmsg));
    memset(stringCache, 0, sizeof(stringCache));
    yInitializeCriticalSection(&ctx->string_cache_cs);
    res = libusb_init(&ctx->libusb);
    if(res !=0){
        return yLinSetErr("Unable to start lib USB", res,errmsg);
    }

    ctx->usb_thread_state = USB_THREAD_NOT_STARTED;
    pthread_create(&ctx->usb_thread, NULL, event_thread, ctx);
    //wait thead start
    while(ctx->usb_thread_state != USB_THREAD_RUNNING){
        usleep(50000);
    }

    return YAPI_SUCCESS;
}


int yyyUSB_stop(yContextSt *ctx,char *errmsg)
{
    int i;
    stringCacheSt *c = stringCache;

    if(ctx->usb_thread_state == USB_THREAD_RUNNING){
        ctx->usb_thread_state = USB_THREAD_MUST_STOP;
        pthread_join(ctx->usb_thread,NULL);
    }
    YASSERT(ctx->usb_thread_state == USB_THREAD_STOPED);

    libusb_exit(ctx->libusb);
    yReleaseGlobalAccess(ctx);
    for (i = 0; i < STRING_CACHE_SIZE; i++, c++) {
        if(c->string) {
            yFree(c->string);
        }
    }
    yDeleteCriticalSection(&ctx->string_cache_cs);

    return YAPI_SUCCESS;
}



static int getDevConfig(libusb_device *dev, struct libusb_config_descriptor **config)
{

   int res = libusb_get_active_config_descriptor(dev,  config);
    if(res==LIBUSB_ERROR_NOT_FOUND){
        HALLOG("not yet configured\n");
        if(libusb_get_config_descriptor(dev, 0, config)!=0){
            return -1;
        }
    }else if(res!=0){
        HALLOG("unable to get active configuration %d\n",res);
        return -1;
    }
    return 0;

}


int yyyUSBGetInterfaces(yInterfaceSt **ifaces,int *nbifaceDetect,char *errmsg)
{
    libusb_device   **list;
    ssize_t         nbdev;
    int             returnval=YAPI_SUCCESS;
    int             i,j;
    int             nbifaceAlloc;
    yInterfaceSt    *iface;

    nbdev=libusb_get_device_list(yContext->libusb,&list);
    if(nbdev<0)
        return yLinSetErr("Unable to get device list",nbdev,errmsg);
    HALLOG("%d devices found\n",nbdev);

     // allocate buffer for detected interfaces
    *nbifaceDetect = 0;
    nbifaceAlloc  = nbdev*2;
    *ifaces = (yInterfaceSt*) yMalloc(nbifaceAlloc * sizeof(yInterfaceSt));
    memset(*ifaces,0,nbifaceAlloc * sizeof(yInterfaceSt));


    for(i=0; i < nbdev; i++){
        int  res;
        struct libusb_device_descriptor desc;
        struct libusb_config_descriptor *config;
        libusb_device_handle *hdl;

        libusb_device  *dev=list[i];
        if((res=libusb_get_device_descriptor(dev,&desc))!=0){
            returnval = yLinSetErr("Unable to get device descriptor",res,errmsg);
            goto exit;
        }
        if(desc.idVendor!=YOCTO_VENDORID ){
            continue;
        }
        HALLOG("open device %x:%x\n",desc.idVendor,desc.idProduct);


        if(getDevConfig(dev,&config)<0)
            continue;

        for(j=0 ; j < config->bNumInterfaces; j++){
            //ensure the buffer of detected interface is big enough
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
            iface->vendorid = (u16)desc.idVendor;
            iface->deviceid = (u16)desc.idProduct;
            iface->ifaceno  = (u16)j;
            iface->devref   = libusb_ref_device(dev);

            res = libusb_open(dev,&hdl);
            if(res==LIBUSB_ERROR_ACCESS){
                returnval =YERRMSG(YAPI_IO_ERROR,"the user has insufficient permissions to access USB devices");
                goto exit;
            }
            if(res!=0){
                HALLOG("unable to access device %x:%x\n",desc.idVendor,desc.idProduct);
                continue;
            }
            HALLOG("try to get serial for %x:%x:%x (%p)\n",desc.idVendor,desc.idProduct,desc.iSerialNumber,dev);

            res = getUsbStringASCII(hdl, dev, desc.iSerialNumber, iface->serial, YOCTO_SERIAL_LEN);
            if(res<0){
                HALLOG("unable to get serial for device %x:%x\n",desc.idVendor,desc.idProduct);
            }
            libusb_close(hdl);
            (*nbifaceDetect)++;
            HALLOG("----Running Dev %x:%x:%d:%s ---\n",iface->vendorid,iface->deviceid,iface->ifaceno,iface->serial);
        }
        libusb_free_config_descriptor(config);
    }


exit:
    libusb_free_device_list(list,1);
    return returnval;

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



static void read_callback(struct libusb_transfer *transfer)
{
    int res;
    linRdTr      *lintr = (linRdTr*)transfer->user_data;
    yInterfaceSt *iface = lintr->iface;
    switch(transfer->status){
    case LIBUSB_TRANSFER_COMPLETED:
        //HALLOG("%s:%d pkt_arrived (len=%d)\n",iface->serial,iface->ifaceno,transfer->actual_length);
        yPktQueuePushD2H(iface,&lintr->tmppkt,NULL);
        if (iface->flags.yyySetupDone) {
            res=libusb_submit_transfer(lintr->tr);
            if(res<0){
                HALLOG("%s:%d libusb_submit_transfer errror %X\n",iface->serial,iface->ifaceno,res);
            }
        }
        return;
    case LIBUSB_TRANSFER_ERROR:
        HALLOG("%s:%d pkt error\n",iface->serial,iface->ifaceno);
        res=libusb_submit_transfer(lintr->tr);
        if(res<0){
            HALLOG("%s:%d libusb_submit_transfer errror %X\n",iface->serial,iface->ifaceno,res);
        }
        break;
    case LIBUSB_TRANSFER_TIMED_OUT :
        HALLOG("%s:%d pkt timeout\n",iface->serial,iface->ifaceno);
        break;
    case LIBUSB_TRANSFER_CANCELLED:
        HALLOG("%s:%d pkt_cancelled (len=%d) \n",iface->serial,iface->ifaceno,transfer->actual_length);
        if(transfer->actual_length==64){
            yPktQueuePushD2H(iface,&lintr->tmppkt,NULL);
        }
        break;
    case LIBUSB_TRANSFER_STALL:
        HALLOG("%s:%d pkt stall\n",iface->serial,iface->ifaceno);
        res=libusb_submit_transfer(lintr->tr);
        if(res<0){
            HALLOG("%s:%d libusb_submit_transfer errror %X\n",iface->serial,iface->ifaceno,res);
        }
        break;
    case LIBUSB_TRANSFER_NO_DEVICE:
        HALLOG("%s:%d pkt_cancelled (len=%d)\n",iface->serial,iface->ifaceno,transfer->actual_length);
        return;
    case LIBUSB_TRANSFER_OVERFLOW:
        HALLOG("%s:%d pkt_overflow (len=%d)\n",iface->serial,iface->ifaceno,transfer->actual_length);
        break;
    default:
        HALLOG("%s:%d unknown state %X\n",iface->serial,iface->ifaceno,transfer->status);
        break;
    }
}



int yyySetup(yInterfaceSt *iface,char *errmsg)
{
    int res,j;
    int error;
    struct libusb_config_descriptor *config;
    const struct libusb_interface_descriptor* ifd;



    HALLOG("%s:%d yyySetup %X:%X\n",iface->serial,iface->ifaceno,iface->vendorid,iface->deviceid);
    if(iface->devref==NULL){
        return YERR(YAPI_DEVICE_NOT_FOUND);
    }
    if((res=libusb_open(iface->devref,&iface->hdl))!=0){
        return yLinSetErr("libusb_open", res,errmsg);
    }

    if((res=libusb_kernel_driver_active(iface->hdl,iface->ifaceno))<0){
        error= yLinSetErr("libusb_kernel_driver_active",res,errmsg);
        goto error;
    }
    if(res){
        HALLOG("%s:%d need to detach kernel driver\n",iface->serial,iface->ifaceno);
        if((res = libusb_detach_kernel_driver(iface->hdl,iface->ifaceno))<0){
            error= yLinSetErr("libusb_detach_kernel_driver",res,errmsg);
            goto error;
        }
    }

    HALLOG("%s:%d Claim interface\n",iface->serial,iface->ifaceno);
    if((res = libusb_claim_interface(iface->hdl,iface->ifaceno))<0){
        error= yLinSetErr("libusb_claim_interface", res,errmsg);
        goto error;
    }


    res=getDevConfig(iface->devref,&config);
    if(res<0){
        error=YERRMSG(YAPI_IO_ERROR,"unable to get configuration descriptor");
        goto error;
    }


    ifd = &config->interface[iface->ifaceno].altsetting[0];
    for (j = 0; j < ifd->bNumEndpoints; j++) {
        //HALLOG("endpoint %X size=%d \n",ifd->endpoint[j].bEndpointAddress,ifd->endpoint[j].wMaxPacketSize);
        if((ifd->endpoint[j].bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_IN){
            iface->rdendp = ifd->endpoint[j].bEndpointAddress;
        }else{
            iface->wrendp = ifd->endpoint[j].bEndpointAddress;
        }
    }

    yPktQueueInit(&iface->rxQueue);
    yPktQueueInit(&iface->txQueue);
    iface->flags.yyySetupDone = 1;

    for(j=0;j< NB_LINUX_USB_TR ; j++){
        iface->rdTr[j].iface = iface;
        iface->rdTr[j].tr=libusb_alloc_transfer(0);
        YASSERT(iface->rdTr[j].tr);
        libusb_fill_interrupt_transfer( iface->rdTr[j].tr,
                                        iface->hdl,
                                        iface->rdendp,
                                        (u8*)&iface->rdTr[j].tmppkt,
                                        sizeof(USB_Packet),
                                        read_callback,
                                        &iface->rdTr[j],
                                        0/*5 sec*/);

        HALLOG("%s:%d libusb_TR filled (%d)\n",iface->serial,iface->ifaceno,j);

    }

    //HALLOG("%s:%d yyyRead\n",iface->serial ,iface->ifaceno);
    for(j=0;j< NB_LINUX_USB_TR ; j++){
        //HALLOG("%s:%d libusb_TR transmit (%d)\n",iface->serial,iface->ifaceno,j);
        res=libusb_submit_transfer(iface->rdTr[j].tr);
        if(res<0){
            return yLinSetErr("libusb_submit_transfer",res,errmsg);
        }
    }
    HALLOG("%s:%d yyySetup done\n",iface->serial,iface->ifaceno);

    return YAPI_SUCCESS;
error:
    libusb_close(iface->hdl);
    return error;
}




int yyySignalOutPkt(yInterfaceSt *iface)
{
    pktItem *pktitem;
    yPktQueuePopH2D(iface, &pktitem);
    while (pktitem!=NULL){
        int transfered,res;
        res = libusb_interrupt_transfer(iface->hdl,
                                    iface->wrendp,
                                    (u8*)&pktitem->pkt,
                                    sizeof(USB_Packet),
                                    &transfered,
                                    5000/*5 sec*/);
        yFree(pktitem);
        if(res<0){
            return YAPI_IO_ERROR;
        }
        yPktQueuePopH2D(iface, &pktitem);
    }

    return YAPI_SUCCESS;
}



void yyyPacketShutdown(yInterfaceSt  *iface)
{
    int res,j;
    iface->flags.yyySetupDone = 0;
    HALLOG("%s:%d cancel all transfer\n",iface->serial,iface->ifaceno);
    for(j=0;j< NB_LINUX_USB_TR ; j++){
        int count=10;
        int res =libusb_cancel_transfer(iface->rdTr[j].tr);
        if(res==0){
            while(count && iface->rdTr[j].tr->status != LIBUSB_TRANSFER_CANCELLED){
                usleep(1000);
                count--;
            }
        }
    }

    for(j=0;j< NB_LINUX_USB_TR ; j++){
        if (iface->rdTr[j].tr){
            HALLOG("%s:%d libusb_TR free %d\n",iface->serial,iface->ifaceno,j);
            libusb_free_transfer(iface->rdTr[j].tr);
            iface->rdTr[j].tr=NULL;
        }
    }
    HALLOG("%s:%d libusb relase iface\n",iface->serial,iface->ifaceno);
    res = libusb_release_interface(iface->hdl,iface->ifaceno);
    if(res != 0 && res!=LIBUSB_ERROR_NOT_FOUND && res!=LIBUSB_ERROR_NO_DEVICE){
        HALLOG("%s:%dlibusb_release_interface error\n",iface->serial,iface->ifaceno);
    }

    res = libusb_attach_kernel_driver(iface->hdl,iface->ifaceno);
    if(res<0 && res!=LIBUSB_ERROR_NO_DEVICE){
        HALLOG("%s:%d libusb_attach_kernel_driver error\n",iface->serial,iface->ifaceno);
    }
    libusb_close(iface->hdl);
    yPktQueueFree(&iface->rxQueue);
    yPktQueueFree(&iface->txQueue);
}

#endif
