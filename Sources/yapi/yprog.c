/*********************************************************************
 *
 * $Id: yprog.c 68921 2025-09-10 07:47:29Z seb $
 *
 * Implementation of firmware upgrade functions
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
#define __FILE_ID__     MK_FILEID('P','R','G')
#define __FILENAME__   "yprog"

#ifdef YAPI_IN_YDEVICE
#include "yocto.h"
#endif
#ifdef EMBEDDED_API
#include "subdevices.h"
#define YERRMSG(code,message)   (code)
#define YPROPERR(call)          {int tmpres=(call); if(YISERR(tmpres)) {return tmpres;}}
#else
#include "yproto.h"
#ifndef WINDOWS_API
#include <dirent.h>
#include <sys/stat.h>
#endif
#endif
#ifdef YAPI_IN_YDEVICE
#include "privhash.h"
#else
#include "yhash.h"
#endif
#include "yjson.h"
#include "yprog.h"
#include <stdio.h>
//#define DEBUG_FIRMWARE

#ifndef YAPI_IN_YDEVICE


#ifdef __BORLANDC__
#pragma warn - 8066
#pragma warn - 8008
#pragma warn - 8065
#endif

void yProgInit(FIRMWARE_CONTEXT *fctx, BootloaderSt *firm_dev)
{
    // BYN header must have an even number of bytes
    YASSERT((sizeof(byn_head_multi)& 1) == 0, 0);

    memset(fctx, 0, sizeof(FIRMWARE_CONTEXT));
    fctx->stepA = FLASH_DONE;
    memset(firm_dev, 0, sizeof(BootloaderSt));
    yContext->fuCtx.global_progress = 100;
    yInitializeCriticalSection(&fctx->cs);
}

void yProgFree(FIRMWARE_CONTEXT *fctx)
{
    int fuPending;
    do {

        yEnterCriticalSection(&fctx->cs);
        if (yContext->fuCtx.global_progress < 0 || yContext->fuCtx.global_progress >= 100) {
            fuPending = 0;
        } else {
            fuPending = 1;
        }
        yLeaveCriticalSection(&fctx->cs);
        if (fuPending) {
            yApproximateSleep(0);
        }
    } while (fuPending);

    if (yContext->fuCtx.serial)
        yFree(yContext->fuCtx.serial);
    if (yContext->fuCtx.firmwarePath)
        yFree(yContext->fuCtx.firmwarePath);
    if (yContext->fuCtx.settings)
        yFree(yContext->fuCtx.settings);
    yDeleteCriticalSection(&fctx->cs);
    memset(fctx, 0, sizeof(FIRMWARE_CONTEXT));
}

#endif

#ifdef EMBEDDED_API

int IsValidBynHead(const byn_head_multi *head, u32 size, u16 flags, char *errmsg)
{
    if (head->h.sign != BYN_SIGN) {
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Not a firmware file");
    }
    if (YSTRLEN(head->h.serial) >= YOCTO_SERIAL_LEN) {
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Bad serial");
    }
    if (YSTRLEN(head->h.product) >= YOCTO_PRODUCTNAME_LEN) {
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Bad product name");
    }
    if (YSTRLEN(head->h.firmware) >= YOCTO_FIRMWARE_LEN) {
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Bad firmware revision");
    }
    switch (head->h.rev) {
    case BYN_REV_V4:
        if (head->v4.nbzones > MAX_ROM_ZONES_PER_FILES) {
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Too many zones");
        }
        if (head->v4.datasize != size - (sizeof(byn_head_sign) + sizeof(byn_head_v4))) {
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Incorrect file size");
        }
        return YAPI_SUCCESS;
    case BYN_REV_V5:
        //we do not check prog_version on YoctoHubs on purpose
        if (head->v5.nbzones > MAX_ROM_ZONES_PER_FILES) {
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Too many zones");
        }
        if (head->v5.datasize != size - (sizeof(byn_head_sign) + sizeof(byn_head_v5))) {
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Incorrect file size");
        }
        return YAPI_SUCCESS;
    case BYN_REV_V6:
        //we do not check prog_version on YoctoHubs on purpose
        if (head->v6.ROM_nb_zone > MAX_ROM_ZONES_PER_FILES) {
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Too many ROM zones");
        }
        if (head->v6.FLA_nb_zone > MAX_FLASH_ZONES_PER_FILES) {
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Too many FLASH zones");
        }
        return YAPI_SUCCESS;
    default:
        break;
    }
    return YERRMSG(YAPI_INVALID_ARGUMENT, "Please upgrade the hub device first");
}

#else

const char* prog_GetCPUName(BootloaderSt *dev)
{
    const char *res = "Unknown CPU model";

    switch (dev->devid_family) {
    case FAMILY_PIC24FJ256DA210:
        if (dev->devid_model == PIC24FJ256DA206) {
            res = "PIC24FJ256DA206";
        }
        break;
    case FAMILY_PIC24FJ64GB004:
        if (dev->devid_model == PIC24FJ64GB002) {
            return "PIC24FJ64GB002";
        }
        break;
    case FAMILY_TM4C123:
        if (dev->devid_model == YCPU_TM4C123GH6PM) {
            return "TM4C123GH6PM";
        }
        break;
    case FAMILY_MSP432E4:
        if (dev->devid_model == YCPU_MSP432E401Y) {
            return "MSP432E401Y";
        }
        break;
    }
    return res;
}


int IsValidBynHead(const byn_head_multi *head, u32 size, u16 flags, char *errmsg)
{
    if (head->h.sign != BYN_SIGN) {
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Not a valid .byn file");
    }
    if (YSTRLEN(head->h.serial) >= YOCTO_SERIAL_LEN) {
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid serial");
    }
    if (YSTRLEN(head->h.product) >= YOCTO_PRODUCTNAME_LEN) {
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid product name");
    }
    if (YSTRLEN(head->h.firmware) >= YOCTO_FIRMWARE_LEN) {
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid firmware revision");
    }

    switch (head->h.rev) {
    case BYN_REV_V4:
        if (head->v4.nbzones > MAX_ROM_ZONES_PER_FILES) {
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Too many zones in .byn file");
        }
        if (head->v4.datasize != size - (sizeof(byn_head_sign) + sizeof(byn_head_v4))) {
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Incorrect file size or corrupt file");
        }
        return YAPI_SUCCESS;
    case BYN_REV_V5:
        if (YSTRLEN(head->v5.prog_version) >= YOCTO_SERIAL_LEN) {
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid programming tools revision or corrupt file");
        }
#ifndef YBUILD_PATCH_WITH_BUILD
        if ((flags & YPROG_FORCE_FW_UPDATE) == 0 && head->v5.prog_version[0]) {
            int byn = atoi(head->v5.prog_version);
            int tools = atoi(YOCTO_API_BUILD_NO);
            if (byn > tools) {
                return YERRMSG(YAPI_VERSION_MISMATCH, "This firmware is too recent, please upgrade your VirtualHub or Yoctopuce library");
            }
        }
#endif
        if (head->v5.nbzones > MAX_ROM_ZONES_PER_FILES) {
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Too many zones in .byn file");
        }
        if (head->v5.datasize != size - (sizeof(byn_head_sign) + sizeof(byn_head_v5))) {
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Incorrect file size or corrupt file");
        }
        return YAPI_SUCCESS;
    case BYN_REV_V6:
        if (YSTRLEN(head->v6.prog_version) >= YOCTO_SERIAL_LEN) {
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid programming tools revision or corrupt file");
        }
#ifndef YBUILD_PATCH_WITH_BUILD
        if ((flags & YPROG_FORCE_FW_UPDATE) == 0 && head->v6.prog_version[0]) {
            int byn = atoi(head->v6.prog_version);
            int tools = atoi(YOCTO_API_BUILD_NO);
            if (byn > tools) {
                return YERRMSG(YAPI_VERSION_MISMATCH, "This firmware is too recent, please upgrade your VirtualHub or Yoctopuce library");
            }
        }
#endif
        if (head->v6.ROM_nb_zone > MAX_ROM_ZONES_PER_FILES) {
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Too many ROM zones in .byn file");
        }
        if (head->v6.FLA_nb_zone > MAX_FLASH_ZONES_PER_FILES) {
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Too many FLASH zones in .byn file");
        }
        return YAPI_SUCCESS;
    default:
        break;
    }
    return YERRMSG(YAPI_INVALID_ARGUMENT, "Unsupported file format, please upgrade your VirtualHub or Yoctopuce library");
}
#endif

int ValidateBynCompat(const byn_head_multi *head, u32 size, const char *serial, u16 flags, BootloaderSt *dev, char *errmsg)
{
    YPROPERR(IsValidBynHead(head, size, flags, errmsg));
    if (serial && YSTRNCMP(head->h.serial, serial, YOCTO_BASE_SERIAL_LEN) != 0) {
        return YERRMSG(YAPI_INVALID_ARGUMENT, "This BYN file is not designed for your device");
    }
#ifndef EMBEDDED_API
    if (dev) {
        const char *cpuname = prog_GetCPUName(dev);
        if (YSTRICMP(cpuname, head->h.pictype) != 0) {
            return YERRMSG(YAPI_INVALID_ARGUMENT, "This BYN file is not designed for your device");
        }
    }
#endif
    return 0;
}

#ifndef EMBEDDED_API

int IsValidBynFile(const byn_head_multi *head, u32 size, const char *serial, u16 flags, char *errmsg)
{
    HASH_SUM ctx;
    u8 md5res[16];
    int res;

    res = ValidateBynCompat(head, size, serial, flags, NULL, errmsg);
    if (res == YAPI_SUCCESS && head->h.rev == BYN_REV_V6) {
        // compute MD5
        MD5Initialize(&ctx);
        MD5AddData(&ctx, ((u8*)head) + BYN_MD5_OFS_V6, size - BYN_MD5_OFS_V6);
        MD5Calculate(&ctx, md5res);
        if (memcmp(md5res, head->v6.md5chk, 16)) {
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid checksum");
        }
    }
    return res;
}
#endif

#ifdef CPU_BIG_ENDIAN

#define BSWAP_U16(NUM)  (((NUM )>> 8) | ((NUM) << 8))
#define BSWAP_U32(NUM) ((((NUM) >> 24) & 0xff) | (((NUM) << 8) & 0xff0000) | (((NUM) >> 8) & 0xff00) | (((NUM) << 24) & 0xff000000 ))

void decode_byn_head_multi(byn_head_multi *head)
{
    head->h.sign = BSWAP_U32(head->h.sign);
    head->h.rev = BSWAP_U16(head->h.rev);
    switch (head->h.rev) {
    case BYN_REV_V4:
        head->v4.nbzones = BSWAP_U32(head->v4.nbzones);
        head->v4.datasize = BSWAP_U32(head->v4.datasize);
        break;
    case BYN_REV_V5:
        head->v5.pad = BSWAP_U16(head->v5.pad);
        head->v5.nbzones = BSWAP_U32(head->v5.nbzones);
        head->v5.datasize = BSWAP_U32(head->v5.datasize);
        break;
    case BYN_REV_V6:
        head->v6.ROM_total_size = BSWAP_U32(head->v6.ROM_total_size);
        head->v6.FLA_total_size = BSWAP_U32(head->v6.FLA_total_size);
        break;
    default:
        break;
    }
}

void decode_byn_zone(byn_zone *zone)
{
    zone->addr_page = BSWAP_U32(zone->addr_page);
    zone->len = BSWAP_U32(zone->len);
}

#endif


#if !defined(EMBEDDED_API)
// Return 1 if the communication channel to the device is busy
// Return 0 if there is no ongoing transaction with the device
int ypIsSendBootloaderBusy(BootloaderSt *dev)
{
    return 0;
}


// Return 0 if there command was successfully queued for sending
// Return -1 if the output channel is busy and the command could not be sent
int ypSendBootloaderCmd(BootloaderSt *dev, char *errmsg)
{
    return yyySendPacket(&dev->iface, &dev->pkt, errmsg);
}

// Return 0 if a reply packet was available and returned
// Return -1 if there was no reply available or on error
int ypGetBootloaderReply(BootloaderSt *dev, char *errmsg)
{
    pktItem *ptr;
    // clear the dest buffer to avoid any misinterpretation
    memset(&dev->pkt.prog.raw, 0, sizeof(USB_Packet));
    YPROPERR(yPktQueueWaitAndPopD2H(&dev->iface,&ptr,10,errmsg));
    if (ptr) {
        yTracePtr(ptr);
        memcpy(&dev->pkt, &ptr->pkt, sizeof(USB_Packet));
        yFree(ptr);
        return 0;
    }
    return YAPI_TIMEOUT; // not a fatal error, handled by caller
}
#endif


#if !defined(EMBEDDED_API)
//pool a packet form usb for a specific device
int BlockingRead(BootloaderSt *dev, int maxwait, char *errmsg)
{
    pktItem *ptr;
    YPROPERR(yPktQueueWaitAndPopD2H(&dev->iface,&ptr,maxwait,errmsg));
    if (ptr) {
        yTracePtr(ptr);
        memcpy(&dev->pkt, &ptr->pkt, sizeof(USB_Packet));
        yFree(ptr);
        return YAPI_SUCCESS;
    }
    return YERR(YAPI_TIMEOUT);
}

int SendDataPacket(BootloaderSt *dev, int program, u32 address, u8 *data, int size, char *errmsg)
{
    memset(&dev->pkt.prog, 0, sizeof(USB_Prog_Packet));
    if (program) {
        dev->pkt.prog.pkt.type = PROG_PROG;
    } else {
        dev->pkt.prog.pkt.type = PROG_VERIF;
    }
    dev->pkt.prog.pkt.adress_low = address & 0xffff;
    dev->pkt.prog.pkt.addres_high = (address >> 16) & 0xff;
    if (IS_TEXAS_FAMILY(dev->devid_family)) {
        if (size > MSP432E_MAX_INSTR_IN_PACKET) {
            size = MSP432E_MAX_INSTR_IN_PACKET;
        }
        if (size) {
            memcpy(dev->pkt.prog.pkt.data, data, size * MSP432E_INSTR_LEN);
        }
    } else {
        if (size > MAX_INSTR_IN_PACKET) {
            size = MAX_INSTR_IN_PACKET;
        }
        if (size) {
            memcpy(dev->pkt.prog.pkt.data, data, size * 3);
        }
    }
    // size is in in instruction
    dev->pkt.prog.pkt.size = size;
    YPROPERR(ypSendBootloaderCmd(dev,errmsg));
    return size;
}


int yUSBGetBooloader(const char *serial, const char *name, yInterfaceSt *iface, char *errmsg)
{
    int nbifaces = 0;
    yInterfaceSt *curif;
    yInterfaceSt *runifaces = NULL;
    int i;

    YPROPERR(yyyUSBGetInterfaces(&runifaces,&nbifaces,errmsg));
    //inspect all interfaces
    for (i = 0, curif = runifaces; i < nbifaces; i++, curif++) {
        // skip real devices
        if (curif->deviceid > YOCTO_DEVID_BOOTLOADER)
            continue;
#ifdef WINDOWS_API
        if (name != NULL && YSTRICMP(curif->devicePath, name) == 0) {
            if (iface)
                memcpy(iface, curif, sizeof(yInterfaceSt));
            yFree(runifaces);
            return YAPI_SUCCESS;
        } else
#endif
            if (serial != NULL && YSTRCMP(curif->serial, serial) == 0) {
                if (iface)
                    memcpy(iface, curif, sizeof(yInterfaceSt));
                yFree(runifaces);
                return YAPI_SUCCESS;
            }
    }
    // free all tmp ifaces
    if (runifaces) {
        yFree(runifaces);
    }
    return YERR(YAPI_DEVICE_NOT_FOUND);
}

#endif

#ifndef YAPI_IN_YDEVICE
static int yLoadFirmwareFile(const char *filename, u8 **buffer, char *errmsg)
{
    FILE *f = NULL;
    int size;
    int nread;
    u8 *ptr;

    *buffer = NULL;
    if (YFOPEN(&f, filename, "rb") != 0) {
        return YERRMSG(YAPI_IO_ERROR, "unable to access file");
    }
    fseek(f, 0, SEEK_END);
    size = (int)ftell(f);
    if (size > MAX_FIRMWARE_LEN || size <= 0) {
        fclose(f);
        return YERR(YAPI_IO_ERROR);
    }
    ptr = yMalloc(size);
    if (ptr == NULL) {
        fclose(f);
        return YERR(YAPI_IO_ERROR);
    }
    fseek(f, 0, SEEK_SET);
    nread = (int)fread(ptr, 1, size, f);
    fclose(f);
    if (nread != size) {
        yFree(ptr);
        return YERRMSG(YAPI_IO_ERROR, "short read");
    }
    *buffer = ptr;
    return size;
}

static void yGetFirmware(FIRMWARE_CONTEXT *fctx, u32 ofs, u8 *dst, u16 size)
{
    YASSERT(fctx->firmware, fctx->firmware - (u8*)NULL);
    YASSERT(ofs + size <= fctx->len, fctx->len);
    memcpy(dst, fctx->firmware + ofs, size);
}

#endif


#ifdef YAPI_IN_YDEVICE
#define ulog ylog
#define ulogU16 ylogU16
#define ulogChar ylogChar
#define uLogProgress(ctx, msg) yProgLogProgress(ctx, msg)


// report progress for devices and vhub
static void yProgLogProgress(FIRMWARE_CONTEXT *fctx, const char *msg)
{



#ifndef EMBEDDED_API
yEnterCriticalSection(&fctx->cs);
#endif
YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, msg);
hProgLog(msg);
#ifndef EMBEDDED_API
yLeaveCriticalSection(&fctx->cs);
#endif
}
#else
#define ytime() ((u32) yapiGetTickCount())
#define Flash_ready()  1
#define ulog(str) dbglog("%s",str)
#define ulogU16(val) dbglog("%x",val)
#define ulogChar(val) dbglog("%c",val)

#define setOsGlobalProgress(ctx, prog, msg) osProgLogProgressEx(ctx,__FILENAME__,__LINE__, prog, msg)
#define uLogProgress(ctx, msg) yProgLogProgress(ctx,msg)


// report progress for devices and vhub
static void yProgLogProgress(FIRMWARE_CONTEXT *fctx, const char *msg)
{
    yEnterCriticalSection(&fctx->cs);
    YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, msg);
    yLeaveCriticalSection(&fctx->cs);
}


static void osProgLogProgressEx(FIRMWARE_CONTEXT *fctx, const char *fileid, int line, int prog, const char *msg)
{
    yEnterCriticalSection(&fctx->cs);
    if (prog != 0) {
        yContext->fuCtx.global_progress = prog;
    }
    if (msg != NULL && *msg != 0) {
#ifdef DEBUG_FIRMWARE
        printf("%s:%d:(%d%%) %s\n", fileid, line, prog, msg);
#endif
        YSTRCPY(yContext->fuCtx.global_message, YOCTO_ERRMSG_LEN, msg);
    }
    yLeaveCriticalSection(&fctx->cs);
}

#endif

#ifdef EMBEDDED_API
#define uGetBootloader(serial,ifaceptr)   yGetBootloaderPort(serial,ifaceptr)
#else
#define uGetBootloader(serial,ifaceptr)   yUSBGetBooloader(serial, NULL, ifaceptr,NULL)
#endif


//-1 = error 0= retry 1= ok (the global fctx.stepA is already updated)
static int uGetDeviceInfo(FIRMWARE_CONTEXT *fctx, BootloaderSt *firm_dev)
{
    switch (fctx->stepB) {
    case 0:
        fctx->stepB++;
        fctx->timeout = ytime() + PROG_GET_INFO_TIMEOUT;
    // no break on purpose;
    case 1:
        memset(&firm_dev->pkt, 0, sizeof(USB_Prog_Packet));
        firm_dev->pkt.prog.pkt.type = PROG_INFO;
        if (ypSendBootloaderCmd(firm_dev, NULL) < 0) {
            if ((s32)(fctx->timeout - ytime()) < 0) {
#ifdef DEBUG_FIRMWARE
                ulog("Cannot send GetInfo pkt\n");
#endif
                YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, "Cannot send GetInfo");
                return -1;
            }
            return 0;
        }
        fctx->stepB++;
        fctx->timeout = ytime() + PROG_GET_INFO_TIMEOUT;
    // no break on purpose;
    case 2:
        if (ypGetBootloaderReply(firm_dev, NULL) < 0) {
            if ((s32)(fctx->timeout - ytime()) < 0) {
#ifdef DEBUG_FIRMWARE
                ulog("Bootloader did not respond to GetInfo pkt\n");
#endif
                YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, "Cannot recv GetInfo");
                return -1;
            }
            return 0;
        }
        fctx->stepB++;
    // no break on purpose;
    case 3:
        if (firm_dev->pkt.prog.pkt.type == PROG_INFO) {
#ifdef DEBUG_FIRMWARE
            ulog("PROG_INFO received\n");
#endif
            firm_dev->er_blk_size = DECODE_U16(firm_dev->pkt.prog.pktinfo.er_blk_size);
            firm_dev->pr_blk_size = DECODE_U16(firm_dev->pkt.prog.pktinfo.pr_blk_size);
            firm_dev->last_addr = DECODE_U32(firm_dev->pkt.prog.pktinfo.last_addr);
            firm_dev->settings_addr = DECODE_U32(firm_dev->pkt.prog.pktinfo.settings_addr);
            firm_dev->devid_family = DECODE_U16(firm_dev->pkt.prog.pktinfo.devidl) >> 8;
            firm_dev->devid_model = DECODE_U16(firm_dev->pkt.prog.pktinfo.devidl) & 0xff;
            firm_dev->devid_rev = DECODE_U16(firm_dev->pkt.prog.pktinfo.devidh);
            firm_dev->startconfig = DECODE_U32(firm_dev->pkt.prog.pktinfo.config_start);
            firm_dev->endofconfig = DECODE_U32(firm_dev->pkt.prog.pktinfo.config_stop);
#ifndef EMBEDDED_API
            firm_dev->ext_jedec_id = 0xffff;
            firm_dev->ext_page_size = 0xffff;
            firm_dev->ext_total_pages = 0;
            firm_dev->first_code_page = 0xffff;
            firm_dev->first_yfs3_page = 0xffff;
#endif
            uLogProgress(fctx, "Device info retrieved");
            fctx->stepB = 0;
            fctx->stepA = FLASH_VALIDATE_BYN;
#ifndef EMBEDDED_API
        } else if (firm_dev->pkt.prog.pkt.type == PROG_INFO_EXT) {
#ifdef DEBUG_FIRMWARE
            ulog("PROG_INFO_EXT received\n");
#endif
            firm_dev->er_blk_size = DECODE_U16(firm_dev->pkt.prog.pktinfo_ext.er_blk_size);
            firm_dev->pr_blk_size = DECODE_U16(firm_dev->pkt.prog.pktinfo_ext.pr_blk_size);
            firm_dev->last_addr = DECODE_U32(firm_dev->pkt.prog.pktinfo_ext.last_addr);
            firm_dev->settings_addr = DECODE_U32(firm_dev->pkt.prog.pktinfo_ext.settings_addr);
            firm_dev->devid_family = DECODE_U16(firm_dev->pkt.prog.pktinfo_ext.devidl) >> 8;
            firm_dev->devid_model = DECODE_U16(firm_dev->pkt.prog.pktinfo_ext.devidl) & 0xff;
            firm_dev->devid_rev = DECODE_U16(firm_dev->pkt.prog.pktinfo_ext.devidh);
            firm_dev->startconfig = DECODE_U32(firm_dev->pkt.prog.pktinfo_ext.config_start);
            firm_dev->endofconfig = DECODE_U32(firm_dev->pkt.prog.pktinfo_ext.config_stop);
            firm_dev->ext_jedec_id = DECODE_U16(firm_dev->pkt.prog.pktinfo_ext.ext_jedec_id);
            firm_dev->ext_page_size = DECODE_U16(firm_dev->pkt.prog.pktinfo_ext.ext_page_size);
            firm_dev->ext_total_pages = DECODE_U16(firm_dev->pkt.prog.pktinfo_ext.ext_total_pages);
            firm_dev->first_code_page = DECODE_U16(firm_dev->pkt.prog.pktinfo_ext.first_code_page);
            firm_dev->first_yfs3_page = DECODE_U16(firm_dev->pkt.prog.pktinfo_ext.first_yfs3_page);
            uLogProgress(fctx, "Device info retrieved");
            fctx->stepB = 0;
            fctx->stepA = FLASH_VALIDATE_BYN;
#endif
        } else {
#ifdef DEBUG_FIRMWARE
            ulog("Not a PROG_INFO pkt\n");
#endif
            YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, "Invalid prog pkt");
            return -1;
        }
        return 1;
#ifdef DEBUG_FIRMWARE
    default:
        ulog("invalid step in uGetDeviceInfo\n");
        break;
#endif
    }
    return 0;
}


static int uSendCmd(BootloaderSt *firm_dev, u8 cmd)
{
    if (ypIsSendBootloaderBusy(firm_dev)) {
        return 0;
    }
    memset(&firm_dev->pkt, 0, sizeof(USB_Packet));
    firm_dev->pkt.prog.pkt.type = cmd;
    if (ypSendBootloaderCmd(firm_dev, NULL) < 0) {
        return -1;
    }
    return 1;
}

#ifndef EMBEDDED_API
#define FULL_ERRMSG fullmsg
#define SET_FLASH_ERRMSG(dst, len, short_msg, long_msg) YSPRINTF(dst, len, "%s (%s)",  (short_msg), (long_msg))
#else
#define FULL_ERRMSG NULL
#define SET_FLASH_ERRMSG(dst, len, short_msg, long_msg) YSTRCPY(dst, len, (short_msg))
#endif

//#define DEBUG_FW_UPDATE_STEPS

#ifdef DEBUG_FW_UPDATE_STEPS
const char *FLASH_DEVICE_STATE_STR[] = {
    "FLASH_FIND_DEV", 
#ifndef EMBEDDED_API
"FLASH_CONNECT",
#endif
"FLASH_GET_INFO",
    "FLASH_VALIDATE_BYN",
    "FLASH_ERASE",
    "FLASH_WAIT_ERASE",
    "FLASH_DOFLASH",
    "FLASH_GET_INFO_BFOR_REBOOT",
    "FLASH_REBOOT",
    "FLASH_REBOOT_VALIDATE",
#ifndef EMBEDDED_API
"FLASH_AUTOFLASH",
#endif
"FLASH_SUCCEEDED",
    "FLASH_DISCONNECT",
    "FLASH_DONE"
};

#endif

static int uFlashZone(FIRMWARE_CONTEXT *fctx, BootloaderSt *firm_dev)
{
    u16 datasize;
    char msg[FLASH_ERRMSG_LEN];
#ifndef EMBEDDED_API
    char fullmsg[YOCTO_ERRMSG_LEN];
#endif
    switch (fctx->zst) {
    case FLASH_ZONE_START:
        if (fctx->currzone == fctx->bynHead.v6.ROM_nb_zone + fctx->bynHead.v6.FLA_nb_zone) {
            fctx->stepA = FLASH_GET_INFO_BFOR_REBOOT;
            fctx->stepB = 0;
            fctx->zOfs = FLASH_NB_REBOOT_RETRY;
            return 0;
        }
        uGetFirmwareBynZone(fctx, fctx->zOfs, &fctx->bz);
        YSTRCPY(msg, FLASH_ERRMSG_LEN, "Flash zone");
#if defined(DEBUG_FIRMWARE)
#ifdef EMBEDDED_API
            {
                char *p = msg + 10;
                *p++ = ' ';
                u16toa(fctx->currzone, p);
                p += ystrlen(p);
                *p++ = ':';
                u32toa(fctx->zOfs, p);
                p += ystrlen(p);
            }
#else
        YSPRINTF(msg, FLASH_ERRMSG_LEN, "Flash zone %d:%d : %x(%x)", fctx->currzone, fctx->zOfs, fctx->bz.addr_page, fctx->bz.len);
        dbglog("Flash zone %d:%x : %x(%x + %x)\n", fctx->currzone, fctx->zOfs, fctx->bz.addr_page, fctx->bz.len , sizeof(byn_zone));
#endif
#endif
        uLogProgress(fctx, msg);
        if ((fctx->bz.addr_page % (firm_dev->pr_blk_size * 2)) != 0) {
            YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, "ProgAlign");
            return -1;
        }
        fctx->zOfs += sizeof(byn_zone);
        fctx->zNbInstr = fctx->bz.len / 3;
        if(fctx->zNbInstr > (firm_dev->settings_addr-fctx->bz.addr_page)/2){
            fctx->zNbInstr = (firm_dev->settings_addr-fctx->bz.addr_page)/2;
        }
        fctx->stepB = 0;
        if (fctx->zNbInstr < (u32)firm_dev->pr_blk_size) {
            YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, "ProgSmall");
            return -1;
        }
        fctx->zst = FLASH_ZONE_PROG;
    //no break on purpose
    case FLASH_ZONE_PROG:
        if (ypIsSendBootloaderBusy(firm_dev)) {
            return 0;
        }
        memset(&firm_dev->pkt, 0, sizeof(USB_Packet));
        if (fctx->flags & YPROG_ONLY_VALIDATE) {
            firm_dev->pkt.prog.pkt.type = PROG_VERIF;
        }else{
            firm_dev->pkt.prog.pkt.type = PROG_PROG;
        }
        firm_dev->pkt.prog.pkt.adress_low = DECODE_U16(fctx->bz.addr_page & 0xffff);
        firm_dev->pkt.prog.pkt.addres_high = (fctx->bz.addr_page >> 16) & 0xff;
        firm_dev->pkt.prog.pkt.size = (u8)(fctx->zNbInstr < MAX_INSTR_IN_PACKET ? fctx->zNbInstr : MAX_INSTR_IN_PACKET);

        datasize = firm_dev->pkt.prog.pkt.size * 3;
        uGetFirmware(fctx, fctx->zOfs, firm_dev->pkt.prog.pkt.data, datasize);
        //dbglog("Flash zone %d:0x%x  0x%x(%d /%d)\n", fctx->currzone, fctx->zOfs, fctx->bz.addr_page, fctx->stepB, firm_dev->pr_blk_size);
        if (ypSendBootloaderCmd(firm_dev, FULL_ERRMSG) < 0) {
            SET_FLASH_ERRMSG(fctx->errmsg, FLASH_ERRMSG_LEN, "ProgPkt", FULL_ERRMSG);
            return -1;
        }

        fctx->zOfs += datasize;
        fctx->bz.len -= datasize;
        fctx->zNbInstr -= firm_dev->pkt.prog.pkt.size;
        fctx->stepB += firm_dev->pkt.prog.pkt.size;
        fctx->progress = (s16)(PROGRESS_FLASH_ERASE + (PROGRESS_FLASH_DOFLASH - PROGRESS_FLASH_ERASE) * fctx->zOfs / fctx->len);

        if (fctx->stepB >= firm_dev->pr_blk_size) {
            //look for confirmation
            fctx->timeout = ytime() + BLOCK_FLASH_TIMEOUT;
            fctx->zst = FLASH_ZONE_RECV_OK;
        }
        break;
    case FLASH_ZONE_RECV_OK:
        if (ypGetBootloaderReply(firm_dev, FULL_ERRMSG) < 0) {
            if ((s32)(fctx->timeout - ytime()) < 0) {
#if defined(DEBUG_FIRMWARE) && !defined(EMBEDDED_API)
                dbglog("Bootloader did not send confirmation for Zone %x Block %x\n", fctx->currzone, fctx->bz.addr_page);
#endif
                SET_FLASH_ERRMSG(fctx->errmsg, FLASH_ERRMSG_LEN, "ProgPkt", FULL_ERRMSG);
                return -1;
            }
            return 0;
        }
        if (firm_dev->pkt.prog.pkt.type != PROG_PROG) {
            YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, "ProgReply");
            return -1;
        } else {
            u32 newblock = ((u32)firm_dev->pkt.prog.pkt.addres_high << 16) | DECODE_U16(firm_dev->pkt.prog.pkt.adress_low);
            //uLogProgress("Block %x to %x is done",fctx.zStartAddr,newblock);
            fctx->bz.addr_page = newblock;
        }
        fctx->stepB = fctx->stepB - firm_dev->pr_blk_size;
        if (fctx->zNbInstr == 0) {
            fctx->zst = FLASH_ZONE_START;
            fctx->zOfs +=  fctx->bz.len;
            fctx->currzone++;
        } else {
            fctx->zst = FLASH_ZONE_PROG;
        }
        break;
    default:
        YASSERT(0, fctx->zst);
    }

    return 0;
}


#ifndef EMBEDDED_API

int uSendReboot(BootloaderSt *firm_dev, u16 signature)
{
    if (ypIsSendBootloaderBusy(firm_dev))
        return 0;
    memset(&firm_dev->pkt, 0, sizeof(USB_Packet));
    firm_dev->pkt.prog.pkt_ext.type = PROG_REBOOT;
    firm_dev->pkt.prog.pkt_ext.opt.btsign = DECODE_U16(signature);
    // do not check reboot packet on purpose (most of the time
    // the os generate an error because the device rebooted too quickly)
    ypSendBootloaderCmd(firm_dev, NULL);
    return 1;
}

static int uSendErase(BootloaderSt *firm_dev, u16 firstPage, u16 nPages)
{
    if (ypIsSendBootloaderBusy(firm_dev))
        return 0;
    memset(&firm_dev->pkt, 0, sizeof(USB_Packet));
    firm_dev->pkt.prog.pkt_ext.type = PROG_ERASE;
    SET_PROG_POS_PAGENO(firm_dev->pkt.prog.pkt_ext, firstPage, 0);
    firm_dev->pkt.prog.pkt_ext.opt.npages = DECODE_U16(nPages);
    if (ypSendBootloaderCmd(firm_dev, NULL) < 0) {
        return -1;
    }
    return 1;
}

static int uFlashFlash(FIRMWARE_CONTEXT *fctx, BootloaderSt *firm_dev)
{
    u32 addr, datasize;
    u8 buff[MAX_BYTE_IN_PACKET];
    char msg[FLASH_ERRMSG_LEN];
    u32 pos, pageno;

    switch (fctx->zst) {
    case FLASH_ZONE_START:
        if (fctx->currzone == fctx->bynHead.v6.ROM_nb_zone + fctx->bynHead.v6.FLA_nb_zone) {
            fctx->stepA = FLASH_AUTOFLASH;
            return 0;
        }
        uGetFirmwareBynZone(fctx, fctx->zOfs, &fctx->bz);
        if (fctx->currzone < fctx->bynHead.v6.ROM_nb_zone) {
            if (IS_TEXAS_FAMILY(firm_dev->devid_family)) {
                fctx->bz.addr_page = (u32)firm_dev->first_code_page * firm_dev->ext_page_size + fctx->bz.addr_page;
            } else {
                fctx->bz.addr_page = (u32)firm_dev->first_code_page * firm_dev->ext_page_size + 3 * fctx->bz.addr_page / 2;
            }
        } else {
            fctx->bz.addr_page = (u32)firm_dev->first_yfs3_page * firm_dev->ext_page_size + fctx->bz.addr_page;
        }
#ifdef DEBUG_FIRMWARE
        dbglog("Flash zone %d:%x : %x(%x)\n", fctx->currzone, fctx->zOfs, fctx->bz.addr_page, fctx->bz.len);
#endif
        YSPRINTF(msg, FLASH_ERRMSG_LEN, "Flash zone %d:%x : %x(%x)", fctx->currzone, fctx->zOfs, fctx->bz.addr_page, fctx->bz.len);
        uLogProgress(fctx, msg);

        if ((fctx->bz.addr_page & 1) != 0 || (fctx->bz.len & 1) != 0) {
            dbglog("Prog block not on a word boundary (%d+%d)\n", fctx->bz.addr_page, fctx->bz.len);
            YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, "Prog block not on a word boundary");
            return -1;
        }
        fctx->zOfs += sizeof(byn_zone);
        fctx->stepB = 0;
        fctx->zst = FLASH_ZONE_PROG;
    //no break on purpose
    case FLASH_ZONE_PROG:
        if (fctx->bz.len > 0 && fctx->currzone < fctx->bynHead.v6.ROM_nb_zone &&
            fctx->bz.addr_page >= (u32)firm_dev->first_yfs3_page * firm_dev->ext_page_size) {
            // skip end of ROM image past reserved flash zone
#ifdef DEBUG_FIRMWARE
            dbglog("Drop ROM data past firmware boundary (zone %d at offset %x)\n", fctx->currzone, fctx->zOfs);
#endif
            fctx->zOfs += fctx->bz.len;
            fctx->bz.len = 0;
            fctx->zst = FLASH_ZONE_START;
            fctx->currzone++;
            return 0;
        }
        addr = fctx->bz.addr_page + fctx->stepB;
        memset(&firm_dev->pkt, 0, sizeof(USB_Packet));
        SET_PROG_POS_PAGENO(firm_dev->pkt.prog.pkt_ext, addr / firm_dev->ext_page_size, addr >> 2);
        datasize = firm_dev->ext_page_size - (addr & (firm_dev->ext_page_size - 1));
        if (datasize > MAX_BYTE_IN_PACKET) {
            datasize = MAX_BYTE_IN_PACKET;
        }
        if (fctx->stepB + datasize > fctx->bz.len) {
            datasize = fctx->bz.len - fctx->stepB;
        }
        YASSERT((datasize & 1) == 0, datasize);
        firm_dev->pkt.prog.pkt_ext.size = (u8)(datasize / 2);
        if (fctx->flags & YPROG_ONLY_VALIDATE) {
            firm_dev->pkt.prog.pkt.type = PROG_VERIF;
        } else {
            firm_dev->pkt.prog.pkt.type = PROG_PROG;
        }
#ifdef DEBUG_FIRMWARE
        {
            u32 page, pos;
            GET_PROG_POS_PAGENO(firm_dev->pkt.prog.pkt_ext, page,  pos);
            pos *=4;
            dbglog("Flash at 0x%x:0x%04x (0x%x bytes) found at 0x%x (0x%x more in fw zone)\n",page, pos,
              2*firm_dev->pkt.prog.pkt_ext.size, fctx->zOfs, fctx->bz.len);
         }
#endif
        uGetFirmware(fctx, fctx->zOfs, firm_dev->pkt.prog.pkt_ext.opt.data, 2*firm_dev->pkt.prog.pkt_ext.size);
        if (ypSendBootloaderCmd(firm_dev, NULL) < 0) {
            dbglog("Unable to send prog pkt\n");
            YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, "Unable to send prog pkt");
            return -1;
        }
        fctx->zOfs += datasize;
        fctx->stepB += datasize;

        // verify each time we finish a page or a zone
        if ((u16)((addr & (firm_dev->ext_page_size - 1)) + datasize) >= firm_dev->ext_page_size || fctx->stepB >= fctx->bz.len) {
            fctx->zOfs -= fctx->stepB; // rewind to check
            fctx->zst = FLASH_ZONE_READ;
        }
        break;
    case FLASH_ZONE_READ:
        // pageno is already set properly
        addr = fctx->bz.addr_page;
        SET_PROG_POS_PAGENO(firm_dev->pkt.prog.pkt_ext, addr / firm_dev->ext_page_size, addr >> 2);
        firm_dev->pkt.prog.pkt.type = PROG_VERIF;
        if (ypSendBootloaderCmd(firm_dev, NULL) < 0) {
            dbglog("Unable to send verif pkt\n");
            YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, "Unable to send verif pkt");
            return -1;
        }
        fctx->zst = FLASH_ZONE_RECV_OK;
        fctx->timeout = ytime() + ZONE_VERIF_TIMEOUT;
    //no break on purpose
    case FLASH_ZONE_RECV_OK:
        if (ypGetBootloaderReply(firm_dev, NULL) < 0) {
            if ((s32)(fctx->timeout - ytime()) < 0) {
#ifdef DEBUG_FIRMWARE
                dbglog("Bootloader did not send confirmation for Zone %x Block %x\n", fctx->currzone, fctx->bz.addr_page);
#endif
                YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, "Device did not respond to verif pkt");
                return -1;
            }
            return 0;
        }
        if (firm_dev->pkt.prog.pkt.type != PROG_VERIF) {
            dbglog("Invalid verif pkt\n");
            YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, "Invalid verif pkt");
            return -1;
        }
        GET_PROG_POS_PAGENO(firm_dev->pkt.prog.pkt_ext, pageno, pos);
#ifdef DEBUG_FIRMWARE
        dbglog("Verif at 0x%x:0x%x (up to 0x%x bytes)\n", pageno,
               pos <<2,
               2*firm_dev->pkt.prog.pkt_ext.size);
#endif
        addr = pageno * firm_dev->ext_page_size + (pos << 2);
        YASSERT(addr >= fctx->bz.addr_page, addr);
        if (addr < fctx->bz.addr_page + fctx->stepB) {
            // packet is in verification range
            datasize = 2 * firm_dev->pkt.prog.pkt_ext.size;
            if (addr + datasize >= fctx->bz.addr_page + fctx->stepB) {
                datasize = fctx->bz.addr_page + fctx->stepB - addr;
            }
            uGetFirmware(fctx, fctx->zOfs + (addr-fctx->bz.addr_page), buff, (u16)datasize);
            if (memcmp(buff, firm_dev->pkt.prog.pkt_ext.opt.data, datasize) != 0) {
                dbglog("Flash verification failed at %x (%x:%x)\n", addr, pageno, addr);
                YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, "Flash verification failed");
                return -1;
            }
#ifdef DEBUG_FIRMWARE
        } else {
            dbglog("Skip verification for block at addr 0x%x (block ends at %x)\n", addr, fctx->bz.addr_page + fctx->stepB);
#endif
        }
        if ((addr & (firm_dev->ext_page_size - 1)) + 2 * (u32)firm_dev->pkt.prog.pkt_ext.size < (u32)firm_dev->ext_page_size) {
            // more packets expected (device will dump a whole flash page)
            return 0;
        }
        fctx->zOfs += fctx->stepB;
        fctx->progress = (s16)(PROGRESS_FLASH_ERASE + (PROGRESS_FLASH_DOFLASH - PROGRESS_FLASH_ERASE) * fctx->zOfs / (BYN_HEAD_SIZE_V6 + fctx->bynHead.v6.ROM_total_size + fctx->bynHead.v6.FLA_total_size));
        fctx->bz.addr_page += fctx->stepB;
        fctx->bz.len -= fctx->stepB;
        if (fctx->bz.len > 0 && fctx->currzone < fctx->bynHead.v6.ROM_nb_zone &&
            fctx->bz.addr_page >= (u32)firm_dev->first_yfs3_page * firm_dev->ext_page_size) {
            // skip end of ROM image past reserved flash zone
#ifdef DEBUG_FIRMWARE
            dbglog("Drop ROM data past firmware boundary (zone %d at offset %x)\n", fctx->currzone, fctx->zOfs);
#endif
            fctx->zOfs += fctx->bz.len;
            fctx->bz.len = 0;
        }
        if (fctx->bz.len == 0) {
            fctx->zst = FLASH_ZONE_START;
            fctx->currzone++;
#ifdef DEBUG_FIRMWARE
            dbglog("Switch to next zone (zone %d at offset %x)\n", fctx->currzone, fctx->zOfs);
#endif
        } else {
            fctx->zst = FLASH_ZONE_PROG;
            fctx->stepB = 0;
#ifdef DEBUG_FIRMWARE
            dbglog("Continue zone %d at offset %x for %x more bytes\n", fctx->currzone, fctx->zOfs, fctx->bz.len);
#endif
        }
    }

    return 0;
}
#endif


YPROG_RESULT uFlashDevice(FIRMWARE_CONTEXT *fctx, BootloaderSt *firm_dev)
{
    byn_head_multi head;
    int res;
#ifdef DEBUG_FW_UPDATE_STEPS
    FLASH_DEVICE_STATE org_step;
#endif
    if (fctx->stepA != FLASH_FIND_DEV && fctx->stepA != FLASH_DONE) {
        if (ypIsSendBootloaderBusy(firm_dev)) {
            return YPROG_WAITING;
        }
        // ReSharper disable once CppUnreachableCode
        if (!Flash_ready()) {
            return YPROG_WAITING;
        }
    }
#ifdef DEBUG_FW_UPDATE_STEPS
    org_step = fctx->stepA;
#endif
    switch (fctx->stepA) {
    case FLASH_FIND_DEV:
        uLogProgress(fctx, "Wait for device");
        if (uGetBootloader(fctx->bynHead.h.serial, &firm_dev->iface) < 0) {
#ifndef EMBEDDED_API
            if ((s32)(fctx->timeout - ytime()) >= 0) {
                return YPROG_WAITING;
            }
#endif
            YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, "device not present");
#ifdef DEBUG_FIRMWARE
            ulog("device not present\n");
#endif
            return YPROG_DONE;
        }
        fctx->progress = PROGRESS_FLASH_FIND_DEV;
        uLogProgress(fctx, "Device detected");

#if defined(DEBUG_FIRMWARE) && defined(EMBEDDED_API)
        ulog("Bootloader ");
        ulog(fctx->bynHead.h.serial);
        ulog(" on port ");
#ifdef EMBEDDED_API
        ulogU16(firm_dev->iface);
#else
        ulogU16(firm_dev->iface.deviceid);
#endif
        ulog("\n");
#endif
#ifndef EMBEDDED_API
        fctx->stepA = FLASH_CONNECT;
    // no break on purpose
    case FLASH_CONNECT:
        if (YISERR(yyySetup(&firm_dev->iface,NULL))) {
            YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, "Unable to open connection to the device");
            return YPROG_DONE;
        }
        uLogProgress(fctx, "Device connected");
#endif
        fctx->stepA = FLASH_GET_INFO;
        fctx->stepB = 0;
        break;
    case FLASH_GET_INFO:
        if (uGetDeviceInfo(fctx, firm_dev) < 0) {
#ifdef DEBUG_FIRMWARE
            ulog("uGetDeviceInfo failed\n");
#endif
            fctx->stepA = FLASH_DISCONNECT;
        }
        fctx->progress = 2;
        break;
    case FLASH_VALIDATE_BYN:
#ifdef DEBUG_FIRMWARE
        ulog("PICDev ");
        ulogU16(firm_dev->devid_model);
        ulog(" detected\n");
#endif
        uGetFirmwareBynHead(fctx, &head);
        if (ValidateBynCompat(&head, fctx->len, fctx->bynHead.h.serial, fctx->flags, firm_dev, fctx->errmsg) < 0) {
#ifdef DEBUG_FIRMWARE
            ulog("ValidateBynCompat failed:");
            ulog(fctx->errmsg);
            ulog("\n");
#endif
            fctx->stepA = FLASH_DISCONNECT;
            return YPROG_DONE;
        }
        memcpy(&fctx->bynHead, &head, sizeof(fctx->bynHead));
        fctx->currzone = 0;
        switch (head.h.rev) {
        case BYN_REV_V4:
            fctx->bynHead.v6.ROM_nb_zone = (u8)head.v4.nbzones;
            fctx->bynHead.v6.FLA_nb_zone = 0;
            fctx->zOfs = BYN_HEAD_SIZE_V4;
            break;
        case BYN_REV_V5:
            fctx->bynHead.v6.ROM_nb_zone = (u8)head.v5.nbzones;
            fctx->bynHead.v6.FLA_nb_zone = 0;
            fctx->zOfs = BYN_HEAD_SIZE_V5;
            break;
        case BYN_REV_V6:
            fctx->zOfs = BYN_HEAD_SIZE_V6;
            break;
        default:
#ifdef DEBUG_FIRMWARE
            ulog("Unsupported file format (upgrade our VirtualHub)\n");
#endif
            fctx->stepA = FLASH_DISCONNECT;
            break;
        }
        fctx->progress = PROGRESS_FLASH_VALIDATE_BYN;
        if (fctx->flags & YPROG_ONLY_VALIDATE) {
            // skip erase
            fctx->stepA = FLASH_DOFLASH;
            fctx->stepB = 0;
        }else{
            fctx->stepA = FLASH_ERASE;
        }
#ifndef EMBEDDED_API
        if (firm_dev->ext_total_pages) {
            fctx->flashPage = firm_dev->first_code_page;
        }
#endif
#ifdef DEBUG_FIRMWARE
        ulogU16(fctx->bynHead.v6.ROM_nb_zone);
        ulog(" ROM zones to flash\n");
#endif
        break;
    case FLASH_ERASE:
        fctx->zst = FLASH_ZONE_START;
        fctx->stepB = 0;
#ifdef EMBEDDED_API
        res = uSendCmd(firm_dev,PROG_ERASE);
#else
        if (firm_dev->ext_total_pages) {
            int npages = firm_dev->ext_total_pages - fctx->flashPage;
            int maxpages = (firm_dev->ext_jedec_id == JEDEC_SPANSION_4MB || firm_dev->ext_jedec_id == JEDEC_SPANSION_8MB ? 16 : 128);
#if 0 //def DEBUG_FIRMWARE
            ulogU16(npages);
            ulog(" pages still to erase\n");
#endif
            if (npages > maxpages) npages = maxpages;
            res = uSendErase(firm_dev,fctx->flashPage, npages);
            if (res >0){
                fctx->flashPage += npages;
            }
        } else {
            res = uSendCmd(firm_dev, PROG_ERASE);
        }
#endif
        if (res > 0) {
            fctx->stepA = FLASH_WAIT_ERASE;
        } else if (res < 0) {
#ifdef DEBUG_FIRMWARE
            ulog("FlashErase failed\n");
#endif
            YSTRCPY(fctx->errmsg, sizeof(fctx->errmsg), "Unable to blank flash");
            fctx->stepA = FLASH_DISCONNECT;
        }
        break;
    case FLASH_WAIT_ERASE:
        if (fctx->stepB == 0) {
#ifndef EMBEDDED_API
            if (firm_dev->ext_total_pages) {
                if (ypIsSendBootloaderBusy(firm_dev)) {
                    return YPROG_WAITING;
                }
                memset(&firm_dev->pkt, 0, sizeof(USB_Prog_Packet));
                firm_dev->pkt.prog.pkt.type = PROG_INFO;
                if (ypSendBootloaderCmd(firm_dev, NULL) < 0) {
                    return YPROG_WAITING;
                }
            }
#endif
            fctx->stepB = ytime();
        } else {
#ifndef EMBEDDED_API
            if (firm_dev->ext_total_pages) {
                if (ypGetBootloaderReply(firm_dev, NULL) < 0) {
                    if ((u32)(ytime() - fctx->stepB) < 2000u) {
                        return YPROG_WAITING;
                    }
#ifdef DEBUG_FIRMWARE
                    ulog("FlashErase failed\n");
#endif
                    YSTRCPY(fctx->errmsg, sizeof(fctx->errmsg), "Timeout blanking flash");
                    fctx->stepA = FLASH_DISCONNECT;
                } else {
#if 0 //def DEBUG_FIRMWARE
                    ulog("clear time: ");
                    ulogU16((u16)(ytime() - pfctx->stepB));
                    ulog("\n");
#endif
                    fctx->progress = PROGRESS_FLASH_VALIDATE_BYN + ((PROGRESS_FLASH_ERASE - PROGRESS_FLASH_VALIDATE_BYN) * fctx->flashPage / firm_dev->ext_total_pages);
                    uLogProgress(fctx, "Erasing flash");
                    if (fctx->flashPage < firm_dev->ext_total_pages) {
                        fctx->stepA = FLASH_ERASE;
                        break;
                    }
                }
            } else
#endif
            {
                u32 delay = 1000 + (firm_dev->last_addr >> 5);
                if ((u32)(ytime() - fctx->stepB) < delay) {
                    return YPROG_WAITING;
                }
            }
            fctx->stepA = FLASH_DOFLASH;
            fctx->stepB = 0;
        }
        break;
    case FLASH_DOFLASH:
#ifdef EMBEDDED_API
        res = uFlashZone(fctx, firm_dev);
#else
        if (firm_dev->ext_total_pages) {
            res = uFlashFlash(fctx, firm_dev);
        } else {
            res = uFlashZone(fctx, firm_dev);
        }
#endif
        if (res < 0) {
#ifdef DEBUG_FIRMWARE
            ulog("Flash failed\n");
            ulog("errmsg=");
            ulog(fctx->errmsg);
            ulogChar('\n');
#endif
            fctx->stepA = FLASH_DISCONNECT;
        }
        break;

    case FLASH_GET_INFO_BFOR_REBOOT:
        res = uGetDeviceInfo(fctx, firm_dev);
        if (res < 0) {
#ifdef DEBUG_FIRMWARE
            ulog("uGetDeviceInfo failed\n");
#endif
            YSTRCPY(fctx->errmsg, FLASH_ERRMSG_LEN, "Last communication before reboot failed");
            fctx->stepA = FLASH_DISCONNECT;
        } else if (res == 1) {
            fctx->stepA = FLASH_REBOOT;
        }
        break;

    case FLASH_REBOOT:
        fctx->progress = PROGRESS_FLASH_REBOOT;
#ifdef DEBUG_FIRMWARE
        ulog("Send reboot\n");
#endif

#ifdef EMBEDDED_API
        res = ypBootloaderShutdown(firm_dev);
        if (res < 0) {
#ifdef DEBUG_FIRMWARE
            ulog("reboot failed\n");
#endif
            YSTRCPY(fctx->errmsg,sizeof(fctx->errmsg),"Unable to reboot bootloader");
            fctx->stepA = FLASH_DISCONNECT;
        }
#else
        uSendCmd(firm_dev, PROG_REBOOT);
        // do not check reboot packet on purpose (most of the time
        // the os generate an error because the device rebooted too quickly)
#endif
        fctx->stepA = FLASH_REBOOT_VALIDATE;
        fctx->timeout = ytime() + YPROG_BOOTLOADER_TIMEOUT;
        break;
    case FLASH_REBOOT_VALIDATE:
        if (uGetBootloader(fctx->bynHead.h.serial, NULL) < 0) {
            fctx->progress = PROGRESS_FLASH_REBOOT_VALIDATE;
#ifdef DEBUG_FIRMWARE
            ulog("device not present\n");
#endif
            fctx->stepA = FLASH_SUCCEEDED;
            break;
        } else {
            if ((s32)(fctx->timeout - ytime()) >= 0) {
                return YPROG_WAITING;
            }
#if defined(DEBUG_FIRMWARE) && defined(EMBEDDED_API)
            ulog("Bootloader ");
            ulog(fctx->bynHead.h.serial);
            ulog(" on port ");
#ifdef EMBEDDED_API
            ulogU16(firm_dev->iface);
#else
            ulogU16(firm_dev.iface.deviceid);
#endif
            ulog("\n");
#endif
            if (fctx->zOfs == 0) {
                uLogProgress(fctx, "reboot failed try again...");
                fctx->stepA = FLASH_GET_INFO_BFOR_REBOOT;
                break;
            }
            uLogProgress(fctx, "Device still in bootloader");
            fctx->zOfs--;
            uLogProgress(fctx, "Device still in bootloader");
            // FIXME: could try to add a retry
            fctx->stepA = FLASH_DISCONNECT;
        }
        break;
#ifndef EMBEDDED_API
    case FLASH_AUTOFLASH:
        fctx->progress = PROGRESS_FLASH_AUTOFLASH;
        if (uSendReboot(firm_dev,START_AUTOFLASHER_SIGN) > 0) {
            fctx->stepA = FLASH_SUCCEEDED;            
        }
        break;
#endif
    case FLASH_SUCCEEDED:
#ifdef DEBUG_FIRMWARE
        ulog("Flash succeeded\n");
#endif
        YSTRCPY(fctx->errmsg, sizeof(fctx->errmsg), "Flash succeeded");
        fctx->progress = 100;
        fctx->stepA = FLASH_DISCONNECT;
    // intentionally no break
    case FLASH_DISCONNECT:
#ifdef DEBUG_FIRMWARE
        ulog("Flash disconnect\n");
#endif
#ifndef EMBEDDED_API
        yyyPacketShutdown(&firm_dev->iface);
#endif
        fctx->stepA = FLASH_DONE;
    // intentionally no break
    case FLASH_DONE:
        return YPROG_DONE;
    }
#ifdef DEBUG_FW_UPDATE_STEPS
    if (org_step != fctx->stepA) {
        dbglog("state %s ->%s\n", FLASH_DEVICE_STATE_STR[org_step], FLASH_DEVICE_STATE_STR[fctx->stepA]);
    }
#endif
    return YPROG_WAITING;
}


#ifndef EMBEDDED_API

typedef int (*yprogTcpReqCb)(void *ctx, const char *buffer, u32 len, char *errmsg);

static int getTCPBootloaders(void *ctx, const char *buffer, u32 len, char *errmsg)
{
    int res = 0;
    yJsonStateMachine j;
    char *p = ctx;
    memset(p, 0, YOCTO_SERIAL_LEN * 4);

    // Parse HTTP header
    j.src = buffer;
    j.end = j.src + len;
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
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
        return YERRMSG(YAPI_IO_ERROR, "Unexpected JSON reply format");
    }
    while (yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
        if (!strcmp(j.token, "list")) {
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_ARRAY) {
                return YERRMSG(YAPI_IO_ERROR, "Unexpected JSON reply format");
            }

            while (yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st != YJSON_PARSE_ARRAY) {
                if (res < 4) {
                    YSTRCPY(p + res * YOCTO_SERIAL_LEN, YOCTO_SERIAL_LEN, j.token);
                }
                res++;
            }
        }
        yJsonSkip(&j, 1);
    }
    return res;
}


// return the list of bootloaders in a specific hub
// buffer must be an pointer to a buffer of min 4 * YOCTO_SERIAL_LEN
// return the number of bootloader copied to buffer
int yNetHubGetBootloaders(const char *hubserial, char *buffer, char *errmsg)
{
    const char *req = "GET /flash.json?a=list \r\n\r\n";
    YIOHDL iohdl;
    YRETCODE res, subres;
    int replysize;
    char *reply;

    res = yapiHTTPRequestSyncStartEx_internal(&iohdl, 0, hubserial, req, YSTRLEN(req), &reply, &replysize, NULL, NULL, errmsg);
    if (YISERR(res)) {
        return res;
    }
    res = getTCPBootloaders(buffer, reply, replysize, errmsg);
    subres = yapiHTTPRequestSyncDone_internal(&iohdl, NULL);
    YASSERT(!YISERR(subres), subres);
    return res;
}


#endif


#ifndef YAPI_IN_YDEVICE

static int getBootloaderInfos(const char *devserial, char *out_hubserial, char *errmsg)
{
    int i, res;


    if (yContext->detecttype & Y_DETECT_USB) {
        int nbifaces = 0;
        yInterfaceSt *iface;
        yInterfaceSt *runifaces = NULL;

        if (YISERR(res = (YRETCODE)yyyUSBGetInterfaces(&runifaces, &nbifaces, errmsg))) {
            return res;
        }

        for (i = 0, iface = runifaces; i < nbifaces; i++, iface++) {
            if (iface->deviceid == YOCTO_DEVID_BOOTLOADER && YSTRCMP(devserial, iface->serial) == 0) {
                YSTRCPY(out_hubserial, YOCTO_SERIAL_LEN, "usb");
                return 1;
            }
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
                if (YSTRCMP(devserial, serial) == 0) {
                    YSTRCPY(out_hubserial, YOCTO_SERIAL_LEN, yContext->nethub[i]->info.serial);
                    return 1;
                }
            }
        }
    }
    return 0;
}

typedef enum {
    FLASH_HUB_AVAIL = 0u,
    FLASH_HUB_STATE,
    FLASH_HUB_FLASH,
    FLASH_HUB_NOT_BUSY,
    // CHECK there is on pending fwupdate (cmd=state-> !uploading && !flashing)
    FLASH_HUB_NONE
} FLASH_HUB_CMD;


typedef struct {
    FLASH_HUB_CMD cmd;
    const char *devserial;
} ckReqHeadCtx;

static int checkRequestHeader(FIRMWARE_CONTEXT *fctx, void *ctx_ptr, const char *buffer, u32 len, char *errmsg)
{
    ckReqHeadCtx *ctx = ctx_ptr;
    yJsonStateMachine j;
    char lastmsg[YOCTO_ERRMSG_LEN] = "invalid";
    int count = 0, return_code = 0;;

    // Parse HTTP header
    j.src = buffer;
    j.end = j.src + len;
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
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
        return YERRMSG(YAPI_IO_ERROR, "Unexpected JSON reply format");
    }
    while (yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
        switch (ctx->cmd) {
        case FLASH_HUB_STATE:
            if (!strcmp(j.token, "state")) {
                if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                    return YERRMSG(YAPI_IO_ERROR, "Unexpected JSON reply format");
                }
                if (YSTRCMP(j.token, "valid")) {
                    YSTRCPY(lastmsg, YOCTO_ERRMSG_LEN, "Invalid firmware");
                    return_code = YAPI_IO_ERROR;
                } else {
                    count++;
                }
            } else if (!strcmp(j.token, "firmware")) {
                if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                    return YERRMSG(YAPI_IO_ERROR, "Unexpected JSON reply format");
                }
                if (YSTRNCMP(j.token, ctx->devserial, YOCTO_BASE_SERIAL_LEN)) {
                    YSTRCPY(lastmsg, YOCTO_ERRMSG_LEN, "Firmware not designed for this module");
                    return_code = YAPI_IO_ERROR;
                } else {
                    count++;
                }
            } else if (!strcmp(j.token, "message")) {
                if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                    return YERRMSG(YAPI_IO_ERROR, "Unexpected JSON reply format");
                }
                YSTRCPY(lastmsg, YOCTO_ERRMSG_LEN, j.token);
            } else {
                yJsonSkip(&j, 1);
            }
            break;
        case FLASH_HUB_NOT_BUSY:
            if (!strcmp(j.token, "state")) {
                if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                    return YERRMSG(YAPI_IO_ERROR, "Unexpected JSON reply format");
                }
                if (YSTRCMP(j.token, "uploading") == 0 || YSTRCMP(j.token, "flashing") == 0) {
                    YSTRCPY(lastmsg, YOCTO_ERRMSG_LEN, "Cannot start firmware update: busy (");
                    YSTRCAT(lastmsg, YOCTO_ERRMSG_LEN, j.token);
                    YSTRCAT(lastmsg, YOCTO_ERRMSG_LEN, ")");
                    return_code = YAPI_IO_ERROR;
                } else {
                    count++;
                }
            } else {
                yJsonSkip(&j, 1);
            }
            break;
        case FLASH_HUB_AVAIL:
            yJsonSkip(&j, 1);
            break;
        case FLASH_HUB_FLASH:
            if (!strcmp(j.token, "logs")) {
                if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_ARRAY) {
                    return YERRMSG(YAPI_IO_ERROR, "Unexpected JSON reply format");
                }
                while (yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st != YJSON_PARSE_ARRAY) {
                    YSTRCPY(lastmsg, YOCTO_ERRMSG_LEN, j.token);
                    while (j.next == YJSON_PARSE_STRINGCONT) {
                        yJsonParse(&j);
                        YSTRCAT(lastmsg, YOCTO_ERRMSG_LEN, j.token);
                    }
                    setOsGlobalProgress(fctx, 0, lastmsg);
                }
            } else if (!strcmp(j.token, "progress")) {
                int progress;
                if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                    return YERRMSG(YAPI_IO_ERROR, "Unexpected JSON reply format");
                }
                progress = atoi(j.token);
                if (progress < 100) {
                    return_code = YAPI_IO_ERROR;
                }
            } else {
                yJsonSkip(&j, 1);
            }
            break;
        default:
            yJsonSkip(&j, 1);
            break;
        }
    }

    if (return_code < 0) {
        YSTRCPY(errmsg, YOCTO_ERRMSG_LEN, lastmsg);
        return return_code;
    }
    return count;
}

static int checkHTTPHeader(void *ctx, const char *buffer, u32 len, char *errmsg)
{
    yJsonStateMachine j;
    // Parse HTTP header
    j.src = buffer;
    j.end = j.src + len;
    j.st = YJSON_HTTP_START;
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_HTTP_READ_CODE) {
        return YERRMSG(YAPI_IO_ERROR, "Failed to parse HTTP header");
    }
    if (YSTRCMP(j.token, "200")) {
        return YERRMSG(YAPI_IO_ERROR, "Unexpected HTTP return code");
    }

    return 0;
}


// Method used to upload a file to the device
static int upload(const char *hubserial, const char *subpath, const char *filename, u8 *data, u32 data_len, char *errmsg)
{
    char *p;
    int buffer_size = 1024 + data_len;
    char *buffer = yMalloc(buffer_size);
    char boundary[32];
    int res;
    YIOHDL iohdl;
    char *reply = NULL;
    int replysize = 0;

    do {
        YSPRINTF(boundary, 32, "Zz%06xzZ", rand() & 0xffffff);
    } while (ymemfind(data, data_len, (u8*)boundary, YSTRLEN(boundary)) >= 0);

    YSTRCPY(buffer, buffer_size, "POST ");
    YSTRCAT(buffer, buffer_size, subpath);
    YSTRCAT(buffer, buffer_size, "upload.html HTTP/1.1\r\nContent-Type: multipart/form-data; boundary=");
    YSTRCAT(buffer, buffer_size, boundary);
    YSTRCAT(buffer, buffer_size,
            "\r\n\r\n"
            "--");
    YSTRCAT(buffer, buffer_size, boundary);
    YSTRCAT(buffer, buffer_size, "\r\nContent-Disposition: form-data; name=\"");
    YSTRCAT(buffer, buffer_size, filename);
    YSTRCAT(buffer, buffer_size, "\"; filename=\"api\"\r\n"
            "Content-Type: application/octet-stream\r\n"
            "Content-Transfer-Encoding: binary\r\n\r\n");
    p = buffer + YSTRLEN(buffer);
    memcpy(p, data, data_len);
    p += data_len;
    YASSERT(p - buffer < buffer_size, p-buffer);
    buffer_size -= (int)(p - buffer);
    YSTRCPY(p, buffer_size, "\r\n--");
    YSTRCAT(p, buffer_size, boundary);
    YSTRCAT(p, buffer_size, "--\r\n");
    buffer_size = (int)(p - buffer) + YSTRLEN(p);
    //todo: chose wisely callback and tpchan
    res = yapiHTTPRequestSyncStartEx_internal(&iohdl, 0, hubserial, buffer, buffer_size, &reply, &replysize, NULL, NULL, errmsg);
    if (res >= 0) {
        res = checkHTTPHeader(NULL, reply, replysize, errmsg);
        yapiHTTPRequestSyncDone_internal(&iohdl, errmsg);
    }
    yFree(buffer);
    return res;
}


typedef enum {
    FLASH_USB = 0u,
    FLASH_NET_SELF,
    FLASH_NET_SUBDEV,
} FLASH_TYPE;


static int sendHubFlashCmd(FIRMWARE_CONTEXT *fctx, const char *hubserial, const char *subpath, const char *devserial, FLASH_HUB_CMD cmd, const char *args, char *errmsg)
{
    char buffer[512];
    const char *cmd_str;
    ckReqHeadCtx ctx;
    int res;
    YIOHDL iohdl;
    YRETCODE subres;
    int replysize;
    char *reply;

    switch (cmd) {
    case FLASH_HUB_AVAIL:
    case FLASH_HUB_STATE:
    case FLASH_HUB_NOT_BUSY:
        cmd_str = "state";
        break;
    case FLASH_HUB_FLASH:
        cmd_str = "flash";
        break;
    default:
        return YERR(YAPI_INVALID_ARGUMENT);
    }
    YSPRINTF(buffer, 512, "GET %sflash.json?a=%s%s \r\n\r\n", subpath, cmd_str, args);
    ctx.cmd = cmd;
    ctx.devserial = devserial;
    res = yapiHTTPRequestSyncStartEx_internal(&iohdl, 0, hubserial, buffer, YSTRLEN(buffer), &reply, &replysize, NULL, NULL, errmsg);
    if (YISERR(res)) {
        return res;
    }
    res = checkRequestHeader(fctx, &ctx, reply, replysize, errmsg);
    subres = yapiHTTPRequestSyncDone_internal(&iohdl, NULL);
    YASSERT(!YISERR(subres), subres);
    return res;
}

static int isWebPath(const char *path)
{
    if (YSTRNCMP(path, "http://", 7) == 0) {
        return 7;
    } else if (YSTRNCMP(path, "https://", 8) == 0) {
        return 8;
    } else if (YSTRNCMP(path, "www.yoctopuce.com", 17) == 0) {
        return 0;
    }
    return -1;
}

static int yDownloadFirmware(const char *fullurl, int host_ofs, u8 **out_buffer, char *errmsg)
{
    char host[256];
    u8 *buffer;
    int res, len, ofs, i, port, usessl;
    const char *url = fullurl + host_ofs;
    const char *http_ok = "HTTP/1.1 200 OK";

    for (i = 0; i < 255 && i < YSTRLEN(url) && url[i] != '/'; i++) {
        host[i] = url[i];
    }

    if (url[i] != '/') {
        return YERRMSG(YAPI_INVALID_ARGUMENT, "invalid url");
    }
    host[i] = 0;

    if (host_ofs == 8) {
        // url start with https://
        port = DEFAULT_HTTPS_PORT;
        usessl = 1;
        if (ystrncmp(fullurl, "https://www.yoctopuce.com", 25) == 0) {
            // skip certificate validation for official certificate
            usessl = 2;
        }
    } else {
        port = DEFAULT_HTTP_PORT;
        usessl = 0;
    }
    res = yTcpDownload(host, port, usessl, url + i, &buffer, 10000, errmsg);
    if (res < 0) {
        return res;
    }
    if (YSTRNCMP((char*)buffer, http_ok, YSTRLEN(http_ok))) {
        yFree(buffer);
        return YERRMSG(YAPI_IO_ERROR, "Unexpected HTTP return code");
    }

    ofs = ymemfind(buffer, res, (u8*)"\r\n\r\n", 4);
    if (ofs < 0) {
        yFree(buffer);
        return YERRMSG(YAPI_IO_ERROR, "Invalid HTTP header");

    }
    ofs += 4;
    len = res - ofs;
    *out_buffer = yMalloc(len);
    memcpy(*out_buffer, buffer + ofs, len);
    yFree(buffer);
    return len;
}


static void* yFirmwareUpdate_thread(void *ctx)
{
    yThread *thread = (yThread*)ctx;
    YAPI_DEVICE dev;
    int res;
    char errmsg[YOCTO_ERRMSG_LEN];
    char buffer[256];
    char subpath[256];
    char bootloaders[YOCTO_SERIAL_LEN * 4];
    char *p;
    char replybuf[512];
    const char *reboot_req = "GET %sapi/module/rebootCountdown?rebootCountdown=-3 \r\n\r\n";
    const char *reboot_hub = "GET %sapi/module/rebootCountdown?rebootCountdown=-1003 \r\n\r\n";
    const char *get_api_fmt = "GET %sapi.json \r\n\r\n";
    char hubserial[YOCTO_SERIAL_LEN];
    char *reply = NULL;
    int replysize = 0;
    int ofs, i;
    u64 timeout;
    FLASH_TYPE type = FLASH_USB;
    int online, found;
    YPROG_RESULT u_flash_res;
    int device_on_vhub4web = 0;


    yThreadSignalStart(thread);

    //1% -> 5%
    setOsGlobalProgress(yContext->fctx, 1, "Loading firmware");
    ofs = isWebPath(yContext->fuCtx.firmwarePath);
    if (ofs < 0) {
        res = yLoadFirmwareFile(yContext->fuCtx.firmwarePath, &yContext->fctx->firmware, errmsg);
    } else {
        res = yDownloadFirmware(yContext->fuCtx.firmwarePath, ofs, &yContext->fctx->firmware, errmsg);
    }
    if (YISERR(res)) {
        setOsGlobalProgress(yContext->fctx, res, errmsg);
        goto exitthread;
    }
    yContext->fctx->len = res;
    //copy firmware header into context variable (to have same behavior as a device)
    memcpy(&yContext->fctx->bynHead, yContext->fctx->firmware, sizeof(yContext->fctx->bynHead));
    YSTRCPY(yContext->fctx->bynHead.h.serial, YOCTO_SERIAL_LEN, yContext->fuCtx.serial);


    res = IsValidBynFile((const byn_head_multi*)yContext->fctx->firmware, yContext->fctx->len, yContext->fuCtx.serial, yContext->fctx->flags, errmsg);
    if (YISERR(res)) {
        setOsGlobalProgress(yContext->fctx, res, errmsg);
        goto exit_and_free;
    }

    //5% -> 10%
    setOsGlobalProgress(yContext->fctx, 5, "Enter firmware update mode");
    dev = wpSearch(yContext->fuCtx.serial);
    if (dev != -1) {
        HubSt *hub;
        int urlres = ywpGetDeviceUrl(dev, hubserial, subpath, 256, NULL);
        if (urlres < 0) {
            setOsGlobalProgress(yContext->fctx, YAPI_IO_ERROR, NULL);
            goto exit_and_free;
        }
        hub = ywpGetDeviceHub((yStrRef)dev);
        if (hub == FAKE_USB_HUB) {
            // USB connected device -> reboot it in bootloader
            type = FLASH_USB;
            YSPRINTF(buffer, sizeof(buffer), reboot_req, subpath);
            res = yapiHTTPRequest(hubserial, buffer, replybuf, sizeof(replybuf), NULL, errmsg);
            if (res < 0) {
                setOsGlobalProgress(yContext->fctx, res, errmsg);
                goto exit_and_free;
            }
        } else {
            if (YSTRNCMP(hubserial, "VHUB4WEB", 8) == 0) {
                device_on_vhub4web = 1;
            }
            res = sendHubFlashCmd(yContext->fctx, hubserial, subpath, yContext->fuCtx.serial, FLASH_HUB_AVAIL, "", NULL);
            if (res < 0 || YSTRNCMP(hubserial, "VIRTHUB", 7) == 0) {
                int is_shield = YSTRNCMP(yContext->fuCtx.serial, "YHUBSHL1", YOCTO_BASE_SERIAL_LEN) == 0;
                res = yNetHubGetBootloaders(hubserial, bootloaders, errmsg);
                if (res < 0) {
                    setOsGlobalProgress(yContext->fctx, res, errmsg);
                    goto exit_and_free;
                }
                for (i = 0; i < res; i++) {
                    p = bootloaders + YOCTO_SERIAL_LEN * i;
                    if (YSTRCMP(yContext->fuCtx.serial, p) == 0) {
                        break;
                    }
                }
                if (i == res) {
                    // not in bootloader list...
                    //...check if list is already full..
                    if (res == 4) {
                        setOsGlobalProgress(yContext->fctx, YAPI_IO_ERROR, "Too many devices in update mode");
                        goto exit_and_free;
                    }
                    if (is_shield) {
                        //...and that we do not already have a shield in bootloader..
                        for (i = 0; i < res; i++) {
                            p = bootloaders + YOCTO_SERIAL_LEN * i;
                            if (YSTRNCMP(p, "YHUBSHL1", YOCTO_BASE_SERIAL_LEN) == 0) {
                                setOsGlobalProgress(yContext->fctx, YAPI_IO_ERROR, "Only one YoctoHub-Shield is allowed in update mode");
                                goto exit_and_free;
                            }
                        }
                    }

                    // ...must reboot in programing
                    setOsGlobalProgress(yContext->fctx, 8, "Reboot to firmware update mode");
                    YSPRINTF(buffer, sizeof(buffer), reboot_req, subpath);
                    res = yapiHTTPRequest(hubserial, buffer, replybuf, sizeof(replybuf), NULL, errmsg);
                    if (res < 0) {
                        setOsGlobalProgress(yContext->fctx, res, errmsg);
                        goto exit_and_free;
                    }
                    if (replybuf[0] != 'O' || replybuf[1] != 'K') {
                        dbglog("Reboot to firmware update mode:\n%s\n", replybuf);
                    }
                }
                type = FLASH_NET_SUBDEV;
            } else {
                type = FLASH_NET_SELF;
            }
        }
    } else {
        //no known device -> check if device is in bootloader
        res = getBootloaderInfos(yContext->fuCtx.serial, hubserial, errmsg);
        if (res < 0) {
            setOsGlobalProgress(yContext->fctx, res, errmsg);
            goto exit_and_free;
        }
        if (res == 0) {
            setOsGlobalProgress(yContext->fctx, YAPI_DEVICE_NOT_FOUND, "Bootloader not found");
            goto exit_and_free;
        }
        if (YSTRCMP(hubserial, "usb") == 0) {
            type = FLASH_USB;
        } else {
            type = FLASH_NET_SUBDEV;
        }
    }

    //10% -> 40%
    setOsGlobalProgress(yContext->fctx, 10, "Send new firmware");
    if (type != FLASH_USB) {
        // ensure flash engine is not busy
        res = sendHubFlashCmd(yContext->fctx, hubserial, type == FLASH_NET_SELF ? subpath : "/", yContext->fuCtx.serial, FLASH_HUB_NOT_BUSY, "", errmsg);
        if (res < 1) {
            setOsGlobalProgress(yContext->fctx, res, errmsg);
            goto exit_and_free;
        }
        // start firmware upload
        // IP connected device -> upload the firmware to the Hub
        res = upload(hubserial, type == FLASH_NET_SELF ? subpath : "/", "firmware", yContext->fctx->firmware, yContext->fctx->len, errmsg);
        if (res < 0) {
            setOsGlobalProgress(yContext->fctx, res, errmsg);
            goto exit_and_free;
        }
        // verify that firmware is correctly uploaded
        res = sendHubFlashCmd(yContext->fctx, hubserial, type == FLASH_NET_SELF ? subpath : "/", yContext->fuCtx.serial, FLASH_HUB_STATE, "", errmsg);
        if (res < 2) {
            setOsGlobalProgress(yContext->fctx, res, errmsg);
            goto exit_and_free;
        }

        if (type == FLASH_NET_SELF) {
            const char *settingsOnly, *services;
            u8 *startupconf_data;
            int settings_len = yapiJsonGetPath_internal("api", (char*)yContext->fuCtx.settings, yContext->fuCtx.settings_len, 0, &settingsOnly, errmsg);
            int service_len = yapiJsonGetPath_internal("services", settingsOnly, settings_len, 0, &services, errmsg);
            int startupconf_data_len;
            if (service_len > 0) {
                int first_len = (services - settingsOnly) & 0xffffffff;
                int sec_len = ((settingsOnly + settings_len) - (services + service_len)) & 0xffffffff;
                startupconf_data = yMalloc(settings_len - service_len + 2);
                memcpy(startupconf_data, settingsOnly, first_len);
                startupconf_data[first_len] = '{';
                startupconf_data[first_len + 1] = '}';
                memcpy(startupconf_data + first_len + 2, services + service_len, sec_len);
                startupconf_data_len = first_len + sec_len;
            } else {
                startupconf_data_len = settings_len;
                startupconf_data = yMalloc(settings_len);
                memcpy(startupconf_data, settingsOnly, settings_len);
            }
            setOsGlobalProgress(yContext->fctx, 20, "Save startupConf.json");
            // save settings
            res = upload(hubserial, subpath, "startupConf.json", startupconf_data, startupconf_data_len, errmsg);
            if (res < 0) {
                yFree(startupconf_data);
                setOsGlobalProgress(yContext->fctx, res, errmsg);
                goto exit_and_free;
            }
            setOsGlobalProgress(yContext->fctx, 30, "Save firmwareConf");
            res = upload(hubserial, subpath, "firmwareConf", startupconf_data, startupconf_data_len, errmsg);
            yFree(startupconf_data);
            if (res < 0) {
                setOsGlobalProgress(yContext->fctx, res, errmsg);
                goto exit_and_free;
            }
        }
    }

    //40%-> 80%
    yContext->fctx->progress = 0;
    switch (type) {
    case FLASH_USB:
        setOsGlobalProgress(yContext->fctx, 40, "Flash firmware");
        yContext->fctx->timeout = ytime() + YPROG_BOOTLOADER_TIMEOUT;
        do {
            u_flash_res = uFlashDevice(yContext->fctx, yContext->firm_dev);
            if (u_flash_res != YPROG_DONE) {
                setOsGlobalProgress(yContext->fctx, 40 + yContext->fctx->progress/2, yContext->fctx->errmsg);
                yApproximateSleep(0);
            }
        } while (u_flash_res != YPROG_DONE);
        if (yContext->fctx->progress < 100) {
            setOsGlobalProgress(yContext->fctx, YAPI_IO_ERROR, yContext->fctx->errmsg);
            goto exit_and_free;
        }
        break;
    case FLASH_NET_SELF:
        setOsGlobalProgress(yContext->fctx, 40, "Flash firmware");
        // the hub itself -> reboot in autoflash mode
        YSPRINTF(buffer, sizeof(buffer), reboot_hub, subpath);
        res = yapiHTTPRequest(hubserial, buffer, replybuf, sizeof(replybuf), NULL, errmsg);
        if (res < 0) {
            setOsGlobalProgress(yContext->fctx, res, errmsg);
            goto exit_and_free;
        }
        for (i = 0; i < 8; i++) {
            setOsGlobalProgress(yContext->fctx, 50 + i*5, "Flash firmware");
            yApproximateSleep(1000);
        }
        break;
    case FLASH_NET_SUBDEV:
        // verify that the device is in bootloader
        setOsGlobalProgress(yContext->fctx, 40, "Verify that the device is in update mode");
        timeout = yapiGetTickCount() + YPROG_BOOTLOADER_TIMEOUT;
        found = 0;
        while (!found && yapiGetTickCount() < timeout) {
            res = yNetHubGetBootloaders(hubserial, bootloaders, errmsg);
            if (res < 0) {
                setOsGlobalProgress(yContext->fctx, res, errmsg);
                goto exit_and_free;
            } else if (res > 0) {
                for (i = 0; i < res; i++) {
                    p = bootloaders + YOCTO_SERIAL_LEN * i;
                    if (YSTRCMP(yContext->fuCtx.serial, p) == 0) {
                        found = 1;
                        break;
                    }
                }
            }
            // device still rebooting
            yApproximateSleep(100);
        }
        if (!found) {
            setOsGlobalProgress(yContext->fctx, YAPI_IO_ERROR, "Hub did not detect bootloader");
            goto exit_and_free;
        }
        //start flash
        setOsGlobalProgress(yContext->fctx, 50, "Flash firmware");
        YSPRINTF(buffer, sizeof(buffer), "&s=%s", yContext->fuCtx.serial);
        res = sendHubFlashCmd(yContext->fctx, hubserial, "/", yContext->fuCtx.serial, FLASH_HUB_FLASH, buffer, errmsg);
        if (res < 0) {
            setOsGlobalProgress(yContext->fctx, res, errmsg);
            goto exit_and_free;
        }
        break;
    }


    if (device_on_vhub4web) {
        // this device is hosted on VirtualHub for Web
        setOsGlobalProgress(yContext->fctx, 101, "Firmware update scheduled successfully");
    } else {
        //90%-> 98%
        setOsGlobalProgress(yContext->fctx, 90, "Wait for the device to restart");
        online = 0;
        timeout = yapiGetTickCount() + 60000;
        do {
            YIOHDL iohdl;
            char tmp_errmsg[YOCTO_ERRMSG_LEN];
            res = yapiUpdateDeviceList(1, errmsg);
            if (res < 0 && type != FLASH_NET_SELF) {
                setOsGlobalProgress(yContext->fctx, res, errmsg);
                goto exit_and_free;
            }
            dev = wpSearch(yContext->fuCtx.serial);
            if (dev != -1) {
                ywpGetDeviceUrl(dev, hubserial, subpath, 256, NULL);
                YSPRINTF(buffer, sizeof(buffer), get_api_fmt, subpath);
                res = yapiHTTPRequestSyncStartEx_internal(&iohdl, 0, hubserial, buffer, YSTRLEN(buffer), &reply, &replysize, NULL, NULL, tmp_errmsg);
                if (res >= 0) {
                    if (checkHTTPHeader(NULL, reply, replysize, tmp_errmsg) >= 0) {
                        const char *real_fw;
                        int fw_len;
                        fw_len = yapiJsonGetPath_internal("module|firmwareRelease", (char*)reply, replysize, 1, &real_fw, errmsg);
                        online = 1;
                        if (fw_len > 2) {
                            const char *p = ((const byn_head_multi*)yContext->fctx->firmware)->h.firmware;
                            //remove quote
                            real_fw++;
                            fw_len -= 2;
                            if (YSTRNCMP(real_fw, p, fw_len) == 0) {
                                online = 2;
                            }
                        }
                        yapiHTTPRequestSyncDone_internal(&iohdl, tmp_errmsg);
                        break;
                    }
                    yapiHTTPRequestSyncDone_internal(&iohdl, tmp_errmsg);
                }
            }
            // idle a bit
            yApproximateSleep(100);
        } while (!online && yapiGetTickCount() < timeout);

        if (online) {
            if (online == 2) {
                setOsGlobalProgress(yContext->fctx, 100, "Firmware updated");
            } else {
                setOsGlobalProgress(yContext->fctx, YAPI_VERSION_MISMATCH, "Unable to update firmware");
            }
        } else {
            setOsGlobalProgress(yContext->fctx, YAPI_DEVICE_NOT_FOUND, "Device did not reboot correctly");
        }
    }
exit_and_free:

    if (yContext->fctx->firmware) {
        yFree(yContext->fctx->firmware);
        yContext->fctx->firmware = NULL;
    }

exitthread:
    yThreadSignalEnd(thread);
    return NULL;
}


static int yStartFirmwareUpdate(const char *serial, const char *firmwarePath, const char *settings, u16 flags, char *msg)
{
    if (yContext->fuCtx.serial)
        yFree(yContext->fuCtx.serial);
    yContext->fuCtx.serial = YSTRDUP(serial);
    if (yContext->fuCtx.firmwarePath)
        yFree(yContext->fuCtx.firmwarePath);
    if (yContext->fuCtx.settings)
        yFree(yContext->fuCtx.settings);
    yContext->fuCtx.firmwarePath = YSTRDUP(firmwarePath);
    yContext->fuCtx.settings = (u8*)YSTRDUP(settings);
    yContext->fuCtx.settings_len = YSTRLEN(settings);
    yContext->fctx->firmware = NULL;
    yContext->fctx->len = 0;
    yContext->fctx->flags = flags;
    yContext->fctx->stepA = FLASH_FIND_DEV;
    YSTRNCPY(yContext->fctx->bynHead.h.serial, YOCTO_SERIAL_LEN, serial, YOCTO_SERIAL_LEN - 1);
    yContext->fuCtx.global_progress = 0;
    YSTRCPY(msg, FLASH_ERRMSG_LEN, "Firmware update started");
    memset(&yContext->fuCtx.thread, 0, sizeof(yThread));
    //yThreadCreate will not create a new thread if there is already one running
    if (yThreadCreate(&yContext->fuCtx.thread, yFirmwareUpdate_thread, NULL) < 0) {
        yContext->fuCtx.serial = NULL;
        YSTRCPY(msg, FLASH_ERRMSG_LEN, "Unable to start helper thread");
        return YAPI_IO_ERROR;
    }
    return 0;
}


static YRETCODE yapiCheckFirmwareFile(const char *serial, int current_rev, u16 flags, const char *path, char *buffer, int buffersize, int *fullsize, char *errmsg)
{
    byn_head_multi *head;
    int size, res, file_rev;
    u8 *p;

    size = yLoadFirmwareFile(path, &p, errmsg);
    if (YISERR(size) || p == NULL) {
        return YAPI_IO_ERROR;
    }
    head = (byn_head_multi*)p;
    res = IsValidBynFile(head, size, serial, flags, errmsg);
    if (YISERR(res)) {
        yFree(p);
        return res;
    }


    file_rev = atoi(head->h.firmware);
    if (file_rev > current_rev) {
        int pathsize = YSTRLEN(path) + 1;
        if (fullsize)
            *fullsize = YSTRLEN(path);
        if (pathsize <= buffersize) {
            YSTRCPY(buffer, buffersize, path);
        }
    } else {
        file_rev = 0;
    }
    yFree(p);
    return file_rev;
}


/***************************************************************************
 * new firmware upgrade API
 **************************************************************************/

static YRETCODE yapiCheckFirmware_r(const char *serial, int current_rev, u16 flags, const char *path, char *buffer, int buffersize, int *fullsize, char *errmsg)
{
    int best_rev = current_rev;
    int pathlen = YSTRLEN(path);
    char abspath[1024];
#ifdef WINDOWS_API
    WIN32_FIND_DATAA ffd;
    HANDLE hFind;
#else
    struct dirent *pDirent;
    DIR *pDir;
#endif

#ifdef WINDOWS_API
#else

    pDir = opendir(path);
    if (pDir == NULL) {
        return yapiCheckFirmwareFile(serial, current_rev, flags, path, buffer, buffersize, fullsize, errmsg);
    }
#endif

    if (pathlen == 0 || pathlen >= 1024 - 32) {
        return YERRMSG(YAPI_INVALID_ARGUMENT, "path too long");
    }

    YSTRCPY(abspath, 1024, path);
    if (abspath[pathlen - 1] != '/' && abspath[pathlen - 1] != '\\') {
#ifdef WINDOWS_API
        abspath[pathlen] = '\\';
#else
        abspath[pathlen] = '/';
#endif
        abspath[++pathlen] = 0;
    }


#ifdef WINDOWS_API
    // Find the first file in the directory.
    YSTRCAT(abspath, 1024, "*");
    hFind = FindFirstFileA(abspath, &ffd);
    if (INVALID_HANDLE_VALUE == hFind) {
        return yapiCheckFirmwareFile(serial, current_rev, flags, path, buffer, buffersize, fullsize, errmsg);
    }
    do {
        char *name = ffd.cFileName;
#else
        while ((pDirent = readdir(pDir)) != NULL) {
            char *name = pDirent->d_name;
            struct stat buf;
#endif
        int isdir;
        int frev = 0;

        if (*name == '.')
            continue;
        abspath[pathlen] = 0;
        YSTRCAT(abspath, 1024, name);
#ifdef WINDOWS_API
        isdir = ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
#else
        stat(abspath, &buf);
        isdir = S_ISDIR(buf.st_mode);
#endif
        if (isdir) {
            frev = yapiCheckFirmware_r(serial, best_rev, flags, abspath, buffer, buffersize, fullsize, errmsg);
        } else {
            int len = YSTRLEN(name);
            if (len < 32 && 'b' == name[len - 3] && 'y' == name[len - 2] && 'n' == name[len - 1]) {
                frev = yapiCheckFirmwareFile(serial, best_rev, flags, abspath, buffer, buffersize, fullsize, errmsg);
            }
        }
        if (frev > 0) {
            best_rev = frev;
        }

#ifdef WINDOWS_API
    } while (FindNextFileA(hFind, &ffd) != 0);
#else
    }
    closedir(pDir);
#endif
    return best_rev;
}


static int checkFirmwareFromWeb(const char *serial, char *out_url, int url_max_len, int *fullsize, char *errmsg)
{
    char request[256];
    u8 *buffer;
    int res, len;
    yJsonStateMachine j;

    YSPRINTF(request, 256, "/FR/common/getLastFirmwareLink.php?serial=%s", serial);
#ifndef NO_YSSL
    res = yTcpDownload("www.yoctopuce.com", DEFAULT_HTTPS_PORT, 2, request, &buffer, 10000, errmsg);
#else
    res = yTcpDownload("www.yoctopuce.com", DEFAULT_HTTP_PORT, 0, request, &buffer, 10000, errmsg);
#endif
    if (res < 0) {
        return res;
    }
    // Parse HTTP header
    j.src = (char*)buffer;
    j.end = j.src + res;
    j.st = YJSON_HTTP_START;
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_HTTP_READ_CODE) {
        yFree(buffer);
        return YERRMSG(YAPI_IO_ERROR, "Unexpected HTTP return code");
    }
    if (YSTRCMP(j.token, "200")) {
        yFree(buffer);
        return YERRMSG(YAPI_IO_ERROR, "Unexpected HTTP return code");
    }
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_HTTP_READ_MSG) {
        yFree(buffer);
        return YERRMSG(YAPI_IO_ERROR, "Unexpected JSON reply format");
    }
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
        yFree(buffer);
        return YERRMSG(YAPI_IO_ERROR, "Unexpected JSON reply format");
    }
    res = 0;
    while (yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
        if (!strcmp(j.token, "link")) {
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                res = YERRMSG(YAPI_IO_ERROR, "Unexpected JSON reply format");
                break;
            }
            len = YSTRLEN(j.token);
            if (fullsize) {
                *fullsize = len;
            }
            if (url_max_len < len + 1) {
                res = YERRMSG(YAPI_INVALID_ARGUMENT, "buffer too small");
                break;
            }
            if (out_url) {
                YSTRCPY(out_url, url_max_len, j.token);
            }
        } else if (!strcmp(j.token, "version")) {
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                res = YERRMSG(YAPI_IO_ERROR, "Unexpected JSON reply format");
                break;
            }
            res = atoi(j.token);
        } else {
            yJsonSkip(&j, 1);
        }
    }

    yFree(buffer);
    return res;
}

YRETCODE yapiCheckFirmware_internal(const char *serial, const char *rev, u32 flags, const char *path, char *buffer, int buffersize, int *fullsize, char *errmsg)
{
    int current_rev = 0;
    int best_rev;

    *buffer = 0;
    if (fullsize)
        *fullsize = 0;
    if (*rev != 0)
        current_rev = atoi(rev);

    if (isWebPath(path) >= 0) {
        best_rev = checkFirmwareFromWeb(serial, buffer, buffersize, fullsize, errmsg);
    } else {
        best_rev = yapiCheckFirmware_r(serial, current_rev, (u16)flags, path, buffer, buffersize, fullsize, errmsg);
    }
    if (best_rev < 0) {
        return best_rev;
    }
    if (best_rev <= current_rev) {
        buffer[0] = 0;
        if (fullsize) {
            *fullsize = 0;
        }
        return 0;
    }
    return best_rev;
}

YRETCODE yapiUpdateFirmware_internal(const char *serial, const char *firmwarePath, const char *settings, int force, int startUpdate, char *msg)
{
    YRETCODE res;
    yEnterCriticalSection(&yContext->fctx->cs);
    if (startUpdate) {
        if (yContext->fuCtx.serial == NULL || yContext->fuCtx.firmwarePath == NULL) {
            res = yStartFirmwareUpdate(serial, firmwarePath, settings, force ? YPROG_FORCE_FW_UPDATE : 0, msg);
        } else if (yContext->fuCtx.global_progress < 0 || yContext->fuCtx.global_progress >= 100) {
            res = yStartFirmwareUpdate(serial, firmwarePath, settings, force ? YPROG_FORCE_FW_UPDATE : 0, msg);
        } else {
            YSTRCPY(msg, FLASH_ERRMSG_LEN, "Last firmware update is not finished");
            res = 0;
        }
    } else {
        if (yContext->fuCtx.serial == NULL || yContext->fuCtx.firmwarePath == NULL) {
            YSTRCPY(msg, FLASH_ERRMSG_LEN, "No firmware update pending");
            res = YAPI_INVALID_ARGUMENT;
        } else if (YSTRCMP(serial, yContext->fuCtx.serial) || YSTRCMP(firmwarePath, yContext->fuCtx.firmwarePath)) {
            YSTRCPY(msg, FLASH_ERRMSG_LEN, "Last firmware update is not finished");
            res = YAPI_INVALID_ARGUMENT;
        } else {
            YSTRCPY(msg, FLASH_ERRMSG_LEN, yContext->fuCtx.global_message);
            res = yContext->fuCtx.global_progress;
        }
    }
    yLeaveCriticalSection(&yContext->fctx->cs);
    return res;
}

#endif
