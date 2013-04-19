/*********************************************************************
 *
 * $Id: yprog.c 10218 2013-03-08 08:46:40Z seb $
 *
 * Implementation of firmware upgrade functions
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

#define __FILE_ID__ "yprog"
#include "yprog.h"
#ifdef MICROCHIP_API
#include <Yocto/yapi_ext.h>
#else
#include "yproto.h"
#endif

//#define DEBUG_FIRMWARE

#if defined(YHUB) || !defined(MICROCHIP_API)

const char* prog_GetCPUName(BootloaderSt *dev)
{
    switch(dev->devid_family){
    case FAMILY_PIC24FJ256DA210:
        switch(dev->devid_model){
#ifndef MICROCHIP_API
            case PIC24FJ128DA206 :
                return "PIC24FJ128DA206";
            case PIC24FJ128DA106 :
                return "PIC24FJ128DA106";
            case PIC24FJ128DA210 :
                return "PIC24FJ128DA210";
            case PIC24FJ128DA110 :
                return "PIC24FJ128DA110";
            case PIC24FJ256DA206 :
                return "PIC24FJ256DA206";
            case PIC24FJ256DA106 :
                return "PIC24FJ256DA106";
            case PIC24FJ256DA210 :
                return "PIC24FJ256DA210";
            case PIC24FJ256DA110 :
                return "PIC24FJ256DA110";
            default:
               return "Unknown CPU model(family PIC24FJ256DA210)";
#else
            case PIC24FJ256DA206 :
                return "PIC24FJ256DA206";
            default: ;
#endif
        }
        break;
    case FAMILY_PIC24FJ64GB004:
        switch(dev->devid_model){
#ifndef MICROCHIP_API
            case PIC24FJ32GB002 :
                return "PIC24FJ32GB002";
            case PIC24FJ64GB002 :
                return "PIC24FJ64GB002";
            case PIC24FJ32GB004 :
                return "PIC24FJ32GB004";
            case PIC24FJ64GB004 :
                return "PIC24FJ64GB004";
            default:
                return "Unknown CPU model(family PIC24FJ64GB004)";
#else
            case PIC24FJ64GB002 :
                return "PIC24FJ64GB002";
            default: ;
#endif
        }
        break;
    }
    return "";
}

int  checkHardwareCompat(BootloaderSt *dev,const char *pictype)
{
    const char *cpuname=prog_GetCPUName(dev);
    if(YSTRICMP(cpuname,pictype)!=0){
        return 0;
    }
    return 1;
}

#endif

int ValidateBynCompat(const byn_head_multi *head, u32 size, const char *serial, BootloaderSt *dev, char *errmsg)
{    
    YPROPERR(IsValidBynHead(head,size, errmsg));
    if(YSTRNCMP(head->h.serial,serial,YOCTO_BASE_SERIAL_LEN)!=0){
        return YERRMSG(YAPI_INVALID_ARGUMENT, "This BYN file is not designed for your device");
    }
    if(dev && !checkHardwareCompat(dev,head->h.pictype)){
        return YERRMSG(YAPI_INVALID_ARGUMENT, "This BYN file is not designed for your device");
    }
    return 0;
}

#ifdef MICROCHIP_API

int IsValidBynHead(const byn_head_multi *head, u32 size, char *errmsg)
{
    if(head->h.sign != BYN_SIGN){
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Not a firmware file");
    }    
    if(YSTRLEN(head->h.serial) >= YOCTO_SERIAL_LEN){
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Bad serial");
    }
    if(YSTRLEN(head->h.product) >= YOCTO_PRODUCTNAME_LEN){
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Bad product name");
    }
    if(YSTRLEN(head->h.firmware) >= YOCTO_FIRMWARE_LEN){
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Bad firmware revision");
    }    
    switch(head->h.rev) {
        case BYN_REV_V4:            
            if( head->v4.nbzones > MAX_ROM_ZONES_PER_FILES){
                return YERRMSG(YAPI_INVALID_ARGUMENT,"Too many zones");
            }            
            if(head->v4.datasize != size -(sizeof(byn_head_sign)+sizeof(byn_head_v4))){
                return YERRMSG(YAPI_INVALID_ARGUMENT, "Incorrect file size");
            }
            return YAPI_SUCCESS;
        case BYN_REV_V5:            
#ifndef YBUILD_PATCH_WITH_BUILD
            if(head->v5.prog_version[0]){
                int byn = atoi(head->v5.prog_version);
                int tools=atoi(YOCTO_API_BUILD_NO);
                if(byn>tools){
                    return YERRMSG(YAPI_VERSION_MISMATCH, "Please upgrade the hub device first");
                }
            }
#endif            
            if( head->v5.nbzones > MAX_ROM_ZONES_PER_FILES){
                return YERRMSG(YAPI_INVALID_ARGUMENT,"Too many zones");
            }            
            if(head->v5.datasize != size -(sizeof(byn_head_sign)+sizeof(byn_head_v5))){
                return YERRMSG(YAPI_INVALID_ARGUMENT, "Incorrect file size");
            }
            return YAPI_SUCCESS;
        case BYN_REV_V6:            
#ifndef YBUILD_PATCH_WITH_BUILD
            if(head->v6.prog_version[0]){
                int byn = atoi(head->v6.prog_version);
                int tools=atoi(YOCTO_API_BUILD_NO);
                if(byn>tools){
                    return YERRMSG(YAPI_VERSION_MISMATCH, "Please upgrade the hub device first");
                }
            }
#endif            
            if( head->v6.ROM_nb_zone > MAX_ROM_ZONES_PER_FILES){
                return YERRMSG(YAPI_INVALID_ARGUMENT,"Too many ROM zones");
            }
            if( head->v6.FLA_nb_zone > MAX_FLASH_ZONES_PER_FILES){
                return YERRMSG(YAPI_INVALID_ARGUMENT,"Too many FLASH zones");
            }            
            return YAPI_SUCCESS;
        default:
            break;            
    }
    return YERRMSG(YAPI_INVALID_ARGUMENT, "Please upgrade the hub device first");    
}

#else

int IsValidBynHead(const byn_head_multi *head, u32 size, char *errmsg)
{
    if(head->h.sign != BYN_SIGN){
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Not a valid .byn file");
    }
    if(YSTRLEN(head->h.serial) >= YOCTO_SERIAL_LEN){
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid serial");
    }    
    if(YSTRLEN(head->h.product) >= YOCTO_PRODUCTNAME_LEN){
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid product name");
    }
    if(YSTRLEN(head->h.firmware) >= YOCTO_FIRMWARE_LEN){
        return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid firmware revision");
    }

    switch(head->h.rev) {
        case BYN_REV_V4:
            if( head->v4.nbzones > MAX_ROM_ZONES_PER_FILES){
                return YERRMSG(YAPI_INVALID_ARGUMENT,"Too many zones in .byn file");
            }
            if(head->v4.datasize != size -(sizeof(byn_head_sign)+sizeof(byn_head_v4))){
               return YERRMSG(YAPI_INVALID_ARGUMENT, "Incorrect file size or corrupt file");
            }
            return YAPI_SUCCESS;
        case BYN_REV_V5:
            if(YSTRLEN(head->v5.prog_version) >= YOCTO_SERIAL_LEN){
                return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid programming tools revision or corrupt file");
            }
#ifndef YBUILD_PATCH_WITH_BUILD
            if(head->v5.prog_version[0]){
                 int byn = atoi(head->v5.prog_version);
                 int tools=atoi(YOCTO_API_BUILD_NO);
                 if(byn>tools){
                     return YERRMSG(YAPI_VERSION_MISMATCH, "This firmware is too recent, please upgrade your VirtualHub or Yoctopuce library");
                 }
            }
#endif
            if( head->v5.nbzones > MAX_ROM_ZONES_PER_FILES){
                return YERRMSG(YAPI_INVALID_ARGUMENT,"Too many zones in .byn file");
            }
            if(head->v5.datasize != size -(sizeof(byn_head_sign)+sizeof(byn_head_v5))){
               return YERRMSG(YAPI_INVALID_ARGUMENT, "Incorrect file size or corrupt file");
            }
            return YAPI_SUCCESS;
        case BYN_REV_V6:            
            if(YSTRLEN(head->v6.prog_version) >= YOCTO_SERIAL_LEN){
                return YERRMSG(YAPI_INVALID_ARGUMENT, "Invalid programming tools revision or corrupt file");
            }
#ifndef YBUILD_PATCH_WITH_BUILD
            if(head->v6.prog_version[0]){
                int byn = atoi(head->v6.prog_version);
                int tools=atoi(YOCTO_API_BUILD_NO);
                if(byn>tools){
                    return YERRMSG(YAPI_VERSION_MISMATCH, "This firmware is too recent, please upgrade your VirtualHub or Yoctopuce library");
                }
            }
#endif            
            if( head->v6.ROM_nb_zone > MAX_ROM_ZONES_PER_FILES){
                return YERRMSG(YAPI_INVALID_ARGUMENT,"Too many ROM zones in .byn file");
            }
            if( head->v6.FLA_nb_zone > MAX_FLASH_ZONES_PER_FILES){
                return YERRMSG(YAPI_INVALID_ARGUMENT,"Too many FLASH zones in .byn file");
            }                      
            return YAPI_SUCCESS;
        default:
            break;        
    }
    return YERRMSG(YAPI_INVALID_ARGUMENT, "Unsupported file format, please upgrade your VirtualHub or Yoctopuce library");
}

int IsValidBynFile(const byn_head_multi  *head, u32 size, char *errmsg)
{
    HASH_SUM ctx;
    u8       md5res[16];
    int      res;
    
    res = IsValidBynHead(head, size, errmsg);
    if(res == YAPI_SUCCESS && head->h.rev == BYN_REV_V6) {
        // compute MD5
        MD5Initialize(&ctx);
        MD5AddData(&ctx, ((u8*)head)+BYN_MD5_OFS_V6, size-BYN_MD5_OFS_V6);
        MD5Calculate(&ctx, md5res);
        if(memcmp(md5res, head->v6.md5chk, 16)) {
            return YERRMSG(YAPI_INVALID_ARGUMENT,"Invalid checksum");
        }
    }
    return res;
}

// Return 1 if the communication channel to the device is busy
// Return 0 if there is no ongoing transaction with the device
int ypIsSendBootloaderBusy(BootloaderSt *dev)
{
    return 0;
}


// Return 0 if there command was successfully queued for sending
// Return -1 if the output channel is busy and the command could not be sent
int ypSendBootloaderCmd(BootloaderSt *dev, const USB_Packet *pkt,char *errmsg)
{
   return yyyWrite(&dev->iface,(USB_Packet*)pkt,errmsg);
}

// Return 0 if a reply packet was available and returned
// Return -1 if there was no reply available
int ypGetBootloaderReply(BootloaderSt *dev, USB_Packet *pkt,char *errmsg)
{
    pktItem *ptr=NULL;
    YPROPERR(yyGetTopPkt(&dev->iface,&ptr,0,errmsg));
    if(ptr){
        yyPopPkt(&dev->iface,ptr);
        yTracePtr(ptr);
        memcpy(pkt,&ptr->pkt,sizeof(USB_Packet));
        yFree(ptr);
        return 0;
    }
    return -1;
}

//pool a packet form usb for a specific device
static int BlockingRead(BootloaderSt *dev,USB_Packet *pkt,char *errmsg)
{
    //fimxe: change to use ypGetBootloaderReply
    pktItem *ptr=NULL;
    YPROPERR(yyyRead(&dev->iface,errmsg));
    do{
        YPROPERR(yyGetTopPkt(&dev->iface,&ptr,10000,errmsg));
    }while(ptr==NULL);
    yyPopPkt(&dev->iface,ptr);
    yTracePtr(ptr);
    memcpy(pkt,&ptr->pkt,sizeof(USB_Packet));
    yFree(ptr);
    YPROPERR(yyyReadStop(&dev->iface,errmsg));
    return YAPI_SUCCESS;
}

static int SendDataPacket( BootloaderSt *dev,int program, u32 address, u8 *data,int nbinstr,char *errmsg)
{

    USB_Packet  pkt;
    //USB_Prog_Packet *pkt = &dev->iface.txqueue->pkt.prog;
    memset(&pkt.prog,0,sizeof(USB_Prog_Packet));
    if(program){
        pkt.prog.pkt.type = PROG_PROG;
    }else{
        pkt.prog.pkt.type = PROG_VERIF;
    }
    pkt.prog.pkt.adress_low = address &0xffff;
    pkt.prog.pkt.addres_high = (address>>16)&0xff;
    if(nbinstr > MAX_INSTR_IN_PACKET){
        nbinstr = MAX_INSTR_IN_PACKET;
    }
    if(nbinstr){
        memcpy(pkt.prog.pkt.data,data,nbinstr*3);
        pkt.prog.pkt.size= nbinstr;
    }

    YPROPERR(ypSendBootloaderCmd(dev,&pkt,errmsg));
    return nbinstr;
}




int prog_RebootDevice(BootloaderSt *dev,u16 signature, char *errmsg)
{
    int res;
    char    suberr[YOCTO_ERRMSG_LEN];
    USB_Packet pkt;
    memset(&pkt,0,sizeof(USB_Packet));
    pkt.prog.pkt_ext.type = PROG_REBOOT;
    pkt.prog.pkt_ext.opt.btsign = signature;
    res = ypSendBootloaderCmd(dev,&pkt,suberr);
	if (YISERR(res)){
		dbglog("Reboot cmd has generated an error:%s",suberr);
	}
	return YAPI_SUCCESS;
    
}

int prog_BlankDevice(BootloaderSt *dev,char *errmsg)
{
    int     res;
    char    suberr[YOCTO_ERRMSG_LEN];
    USB_Packet pkt;
    memset(&pkt,0,sizeof(USB_Prog_Packet));
    if(dev->ext_total_pages){
        pkt.prog.pkt_ext.type = PROG_ERASE;
        pkt.prog.pkt_ext.pageno = dev->first_code_page;
        pkt.prog.pkt_ext.opt.npages = dev->ext_total_pages - dev->first_code_page;
        res= ypSendBootloaderCmd(dev,&pkt,suberr);
    }else{
        pkt.prog.pkt.type = PROG_ERASE;
        res= ypSendBootloaderCmd(dev,&pkt,suberr);
    }
    return FusionErrmsg(res,errmsg,"Unable to blank the device",suberr);
}

static int prog_FlashVerifBlock(BootloaderSt *dev,int flash,u32 startAddr, u8 *data,u32 size,char *errmsg)
{
    u32 nb_instr;
    u32 instr_no;
    USB_Packet respkt;
   
   
    instr_no = startAddr/2;
    if((startAddr % (dev->pr_blk_size*2)) !=0){
        return YERRMSG(YAPI_INVALID_ARGUMENT,"Prog block not on a boundary");
    }
    
    nb_instr = size/3;
    if(nb_instr < (u32)dev->pr_blk_size){
        return YERRMSG(YAPI_INVALID_ARGUMENT,"Prog block too small");
    }
    if(nb_instr > (dev->settings_addr-startAddr)/2){
        nb_instr = (dev->settings_addr-startAddr)/2;
    }
    while(nb_instr){
        u32 ofs          = instr_no % dev->pr_blk_size;
        u32 block_number = instr_no / dev->pr_blk_size;
        u32 block_addr   = block_number * dev->pr_blk_size * 2;
        
        while(nb_instr &&  ofs < dev->pr_blk_size ){
            int sent;
            int size = nb_instr < 20 ? (int)nb_instr : 20;          
            sent = SendDataPacket(dev,flash,block_addr,data,size,errmsg);
            if(sent<0){ 
                return YERRMSG(YAPI_IO_ERROR,"Transmit failed");
            }
            ofs      += sent;
            instr_no += sent;
            nb_instr -= sent;
            data     += (sent*3);
        }
        YPROPERR( BlockingRead(dev,&respkt, errmsg) );
        if(respkt.prog.pktinfo.type != PROG_PROG){
            return YERRMSG(YAPI_INVALID_ARGUMENT,"Block verification failed");
        }
    }

    return YAPI_SUCCESS;
}


int prog_FlashBlock(BootloaderSt *dev,u32 startAddr, u8 *data,int size,char *errmsg)
{
    int     res;
    char    suberr[YOCTO_ERRMSG_LEN];
    
    res = prog_FlashVerifBlock(dev,1,startAddr,data,size,suberr);
    if(YISERR(res)){
        int len;
        if(errmsg){
            YSTRCPY(errmsg,YOCTO_ERRMSG_LEN,"Flash failed (try to blank device before) : ");
            len=YSTRLEN(errmsg);
            YSTRNCAT(errmsg,YOCTO_ERRMSG_LEN,suberr,YOCTO_ERRMSG_LEN-len);
        }
    }
    return res;
}


static int prog_VerifyBlock(BootloaderSt *dev,u32 startAddr, u8 *data,int size,char *errmsg)
{
    int     res;
    char    suberr[YOCTO_ERRMSG_LEN];
  
    res = prog_FlashVerifBlock(dev,0,startAddr,data,size,suberr);
    if(YISERR(res)){
        int len;
        if(errmsg){
            YSTRCPY(errmsg,YOCTO_ERRMSG_LEN,"Verification failed: ");
            len=YSTRLEN(errmsg);
            YSTRNCAT(errmsg,YOCTO_ERRMSG_LEN,suberr,YOCTO_ERRMSG_LEN-len);
        }
    }
    return res;
}


static int prog_FlashFlash(yFlashArg *arg,int *step,BootloaderSt *dev, newmemzones *zones,char *errmsg)
{
    u32 currzone;
    u32 page;
    u32 len;
    USB_Packet pkt;
    char suberr[YOCTO_ERRMSG_LEN];
    int res;
    
    YASSERT(dev->first_yfs3_page !=0xffff && dev->first_code_page!=0xffff);
    
    for(currzone=0; currzone < zones->nbrom + zones->nbflash ;currzone++) {
        u8  *data,*verif_data;
        u32 stepB;
        u32 addr, datasize;
        if(arg->callback) arg->callback(*step,10,arg->context);
        if(*step<8)
			(*step)++;

        if(currzone < zones->nbrom) {
            page      = (u32)dev->first_code_page * dev->ext_page_size + 3*zones->rom[currzone].addr/2;
            len       = zones->rom[currzone].len;
            data      = zones->rom[currzone].ptr;
        } else {
            page      = (u32)dev->first_yfs3_page * dev->ext_page_size + zones->flash[currzone-zones->nbrom].page;
            len       = zones->flash[currzone-zones->nbrom].len;
            data      = zones->flash[currzone-zones->nbrom].ptr;
        }
        verif_data = data;
#ifdef DEBUG_FIRMWARE
        dbglog("Flash zone %d : %x(%x)\n",currzone,page,len);
#endif
        if((page & 1) != 0 || (len & 1) != 0) {
            dbglog("Prog block not on a word boundary (%d+%d)\n", page, len);
            YSTRCPY(errmsg,YOCTO_ERRMSG_LEN,"Prog block not on a word boundary");
            return -1;
        }
        
        while (len>0) {
            if(currzone < zones->nbrom && page >= (u32)dev->first_yfs3_page * dev->ext_page_size) {
                // skip end of ROM image past reserved flash zone
#ifdef DEBUG_FIRMWARE   
                ylogf("Drop ROM data past firmware boundary (zone %d at offset %x)\n", fctx.currzone, fctx.zOfs);
#endif
                data += len;
                len = 0;
                break;
            }
            stepB =0;
            do{
                addr = page + stepB;
                memset(&pkt,0,sizeof(USB_Packet));
                pkt.prog.pkt_ext.dwordpos_lo = (addr >> 2) & 0xff;
                pkt.prog.pkt_ext.dwordpos_hi = (addr >> 10) & 3;
                pkt.prog.pkt_ext.pageno = (addr / dev->ext_page_size) & 0xffff;
                datasize = dev->ext_page_size - (addr & (dev->ext_page_size-1));
                if(datasize > MAX_BYTE_IN_PACKET) {
                    datasize = MAX_BYTE_IN_PACKET;
                }
                if(stepB + datasize > len) {
                    datasize = len - stepB;
                }
                YASSERT((datasize & 1) == 0);
                pkt.prog.pkt_ext.size = (u8)(datasize / 2);
                pkt.prog.pkt_ext.type = PROG_PROG;
#ifdef DEBUG_FIRMWARE
                dbglog("Flash at %x:%x (%x bytes) found at %x (%x more in zone)\n",pkt.prog.pkt_ext.pageno,
                  ((u32)pkt.prog.pkt_ext.dwordpos_hi*1024)+(u32)pkt.prog.pkt_ext.dwordpos_lo*4,
                  2*pkt.prog.pkt_ext.size, data, len);
#endif
                memcpy( pkt.prog.pkt_ext.opt.data,data, datasize);
                YPROPERR(ypSendBootloaderCmd(dev,&pkt,errmsg));
                data  += datasize;
                stepB += datasize;
            } while ( (addr & (dev->ext_page_size-1)) + datasize < dev->ext_page_size && stepB <len);

            // pageno is already set properly
            addr = page;
            pkt.prog.pkt_ext.dwordpos_lo = (addr >> 2) & 0xff;
            pkt.prog.pkt_ext.dwordpos_hi = (addr >> 10) & 3;
            pkt.prog.pkt_ext.pageno = (u16)(addr / dev->ext_page_size);
            pkt.prog.pkt.type = PROG_VERIF;
            if((res=ypSendBootloaderCmd(dev,&pkt,suberr))<0){
                dbglog("Unable to send verif pkt\n");
                return FusionErrmsg(res,errmsg,"Unable to blank the device",suberr);
            }
            do{
                // clear the buffer
                pkt.prog.pkt.type = PROG_NOP;
                res = BlockingRead(dev,&pkt,suberr);
                if(YISERR(res)){ return FusionErrmsg(res,errmsg,"Unable to get verification packet",suberr);}

                if(pkt.prog.pkt.type != PROG_VERIF) {
                    dbglog("Invalid verif pkt\n");
                    return YERRMSG(YAPI_IO_ERROR,"Block verification failed");
                }
#ifdef DEBUG_FIRMWARE
                dbglog("Verif at %x:%x (up to %x bytes)\n",pkt.prog.pkt_ext.pageno,
                  ((u32)pkt.prog.pkt_ext.dwordpos_hi*1024)+(u32)pkt.prog.pkt_ext.dwordpos_lo*4,
                  2*pkt.prog.pkt_ext.size);
#endif
                addr = (u32)pkt.prog.pkt_ext.pageno * dev->ext_page_size +
                    ((u32)pkt.prog.pkt_ext.dwordpos_hi*1024)+(u32)pkt.prog.pkt_ext.dwordpos_lo*4;
                YASSERT(addr >= page);
                if(addr < page + stepB) {
                    // packet is in verification range
                    datasize = 2 * pkt.prog.pkt_ext.size;
                    if(addr + datasize >= page + stepB) {
                        datasize = page + stepB - addr;
                    }
                    if(memcmp(verif_data+(addr-page), pkt.prog.pkt_ext.opt.data, datasize) != 0) {
                        dbglog("Flash verification failed\n");
                        return YERRMSG(YAPI_IO_ERROR,"Flash verification failed");
                    }
                } else {
#ifdef DEBUG_FIRMWARE
                    dbglog("Skip verification for block at %x (block ends at %x)\n", addr, page + stepB);
#endif
                }
            }while((addr & (dev->ext_page_size-1)) + 2 * (u32)pkt.prog.pkt_ext.size < (u32)dev->ext_page_size);
            verif_data += stepB;
            page += stepB;
            len -= stepB;
            if(len > 0 && currzone < zones->nbrom && page >= (u32)dev->first_yfs3_page * dev->ext_page_size) {
                // skip end of ROM image past reserved flash zone
#ifdef DEBUG_FIRMWARE
                dbglog("Drop ROM data past firmware boundary (zone %d at offset %x)\n",currzone, data);
#endif
                data += len;
                len = 0;
            }
        }
    }
    return 0;
}

//#define DEBUG_FIRMWARE

int  prog_FlashDevice(yFlashArg *arg, int realyflash, char *errmsg)
{
    BootloaderSt dev;
    char suberrmsg[YOCTO_ERRMSG_LEN];
    int res;
	u32 z;
    newmemzones zones;
    int step=1;

#ifdef DEBUG_FIRMWARE
    dbglog("Upgrade firmware(%s,%d)\n",arg->serial,realyflash);
#endif
    if(arg->callback) arg->callback(step++,10,arg->context);
    if(arg->OSDeviceName!= NULL){
        res = yUSBGetBooloader(NULL,arg->OSDeviceName,&dev.iface,suberrmsg);
        if(YISERR(res)){
            return FusionErrmsg(res,errmsg,"Unable to open the device by name",suberrmsg);
        }
    }else{
        res=yUSBGetBooloader(arg->serial,NULL,&dev.iface,suberrmsg);
        if(YISERR(res)){
            return FusionErrmsg(res,errmsg,"Unable to open the device by serial",suberrmsg);
        }
    }
    yyyInitPktQueue(&dev.iface);
    YPROPERR(yyySetup(&dev.iface,errmsg));
#ifdef DEBUG_FIRMWARE
    dbglog("get infos\n");
#endif

    res=prog_GetDeviceInfo(&dev,errmsg);
    if(YISERR(res)){
        yyyPacketShutdown(&dev.iface);
        return res;
    }
#ifdef DEBUG_FIRMWARE
    dbglog("decode byn\n");
#endif
    if(arg->callback) arg->callback(step++,10,arg->context);
    res = DecodeBynFile(arg->firmwarePtr,arg->firmwareLen,&zones,dev.iface.serial,&dev,errmsg);
    if(res<0)
        return res;
#ifdef DEBUG_FIRMWARE
    dbglog("blank dev\n");
#endif
    if(arg->callback) arg->callback(step++,10,arg->context);
    if(realyflash){
        res = prog_BlankDevice(&dev,errmsg);
        if(YISERR(res)){
            FreeZones(&zones);
            yyyPacketShutdown(&dev.iface);
            return res;
        }
    }
    if(dev.ext_total_pages ==0){
        for (z=0 ; z < zones.nbrom  ; z++){
#ifdef DEBUG_FIRMWARE
    dbglog("prog zone %d\n",z);
#endif

            if(arg->callback) arg->callback(step,10,arg->context);
            if(step<8)
                step++;
            if(realyflash){
                res = prog_FlashBlock(&dev, zones.rom[z].addr,zones.rom[z].ptr,zones.rom[z].len,errmsg);
            }else{
                res = prog_VerifyBlock(&dev, zones.rom[z].addr,zones.rom[z].ptr,zones.rom[z].len,errmsg);
            }
            if(YISERR(res)){
                FreeZones(&zones);
                yyyPacketShutdown(&dev.iface);  
                return res;
            }
        }
        if(arg->callback) arg->callback(9,10,arg->context);
        if(realyflash){
            res = prog_RebootDevice(&dev,START_APPLICATION_SIGN,errmsg);
        }else{
            res = YAPI_SUCCESS;
        }
    }else{
        res = prog_FlashFlash(arg,&step,&dev, &zones, errmsg);
        if(YISERR(res)){
            FreeZones(&zones);
            yyyPacketShutdown(&dev.iface);
            return res;
        }
        if(arg->callback) arg->callback(9,10,arg->context);
        res = prog_RebootDevice(&dev,START_AUTOFLASHER_SIGN,errmsg);
        if(YISERR(res)){
            FreeZones(&zones);
            yyyPacketShutdown(&dev.iface);
            return res;
        }        
    }
    
    FreeZones(&zones);
    yyyPacketShutdown(&dev.iface);
    if(YISERR(res)){
        return res;
    }
        
    if(arg->callback) arg->callback(10,10,arg->context);
    
    return YAPI_SUCCESS;
}

// retrun 0 for legacy bootloader and 1 for flash bootloader
int prog_GetDeviceInfo(BootloaderSt *dev,char *errmsg)
{
    int     res;
    char    suberr[YOCTO_ERRMSG_LEN];
    char    *nicemsg="Unable to get device infos";
    USB_Packet pkt;
    memset(&pkt,0,sizeof(USB_Prog_Packet));
    pkt.prog.pkt.type = PROG_INFO;
    res = ypSendBootloaderCmd(dev,&pkt,suberr);
    if(YISERR(res)){ return FusionErrmsg(res,errmsg,nicemsg,suberr);}
    pkt.prog.pkt.type = PROG_NOP;
    res = BlockingRead(dev,&pkt,suberr);
    if(YISERR(res)){ return FusionErrmsg(res,errmsg,nicemsg,suberr);}
    
    switch(pkt.prog.pktinfo.type){
        case PROG_INFO:
            dev->er_blk_size   = pkt.prog.pktinfo.er_blk_size;
            dev->pr_blk_size   = pkt.prog.pktinfo.pr_blk_size;
            dev->last_addr     = pkt.prog.pktinfo.last_addr;
            dev->settings_addr = pkt.prog.pktinfo.settings_addr;
            dev->devid_family  = pkt.prog.pktinfo.devidl>>8;
            dev->devid_model   = pkt.prog.pktinfo.devidl & 0xff;
            dev->devid_rev     = pkt.prog.pktinfo.devidh;
            dev->startconfig   = pkt.prog.pktinfo.config_start;
            dev->endofconfig   = pkt.prog.pktinfo.config_stop;
            dev->ext_jedec_id  = 0xffff;
            dev->ext_page_size = 0xffff;
            dev->ext_total_pages = 0;
            dev->first_code_page = 0xffff;
            dev->first_yfs3_page = 0xffff;
            return 0;
        case PROG_INFO_EXT:
            dev->er_blk_size   = pkt.prog.pktinfo_ext.er_blk_size;
            dev->pr_blk_size   = pkt.prog.pktinfo_ext.pr_blk_size;
            dev->last_addr     = pkt.prog.pktinfo_ext.last_addr;
            dev->settings_addr = pkt.prog.pktinfo_ext.settings_addr;
            dev->devid_family  = pkt.prog.pktinfo_ext.devidl>>8;
            dev->devid_model   = pkt.prog.pktinfo_ext.devidl & 0xff;
            dev->devid_rev     = pkt.prog.pktinfo_ext.devidh;
            dev->startconfig   = pkt.prog.pktinfo_ext.config_start;
            dev->endofconfig   = pkt.prog.pktinfo_ext.config_stop;
            dev->ext_jedec_id  = pkt.prog.pktinfo_ext.ext_jedec_id;
            dev->ext_page_size = pkt.prog.pktinfo_ext.ext_page_size;
            dev->ext_total_pages = pkt.prog.pktinfo_ext.ext_total_pages;
            dev->first_code_page = pkt.prog.pktinfo_ext.first_code_page;
            dev->first_yfs3_page = pkt.prog.pktinfo_ext.first_yfs3_page;
            return 1;
        default:
            return FusionErrmsg(YAPI_IO_ERROR,errmsg,nicemsg,"Invalid Prog packet");
    }
    
    return YAPI_SUCCESS;
}




int DecodeBynFile(const u8 *buffer,u32 size,newmemzones *zones,const char *serial, BootloaderSt *dev,char *errmsg)
{
    const byn_head_multi  *head=(byn_head_multi*)buffer;
    const u8 *rom=NULL;
    const u8 *fla=NULL;
    u32  i;
    int  res=ValidateBynCompat(head,size,serial,dev,errmsg);
    YPROPERR(res);
    memset(zones, 0,sizeof(newmemzones));
    switch(head->h.rev) {
        case BYN_REV_V4:
            rom = buffer+BYN_HEAD_SIZE_V4;
            zones->nbrom =  head->v4.nbzones;
            break;
        case BYN_REV_V5:
            rom = buffer+BYN_HEAD_SIZE_V5;
            zones->nbrom =  head->v5.nbzones;
            break;
        case BYN_REV_V6:
            rom = buffer+BYN_HEAD_SIZE_V6;
            zones->nbrom =  head->v6.ROM_nb_zone;
            fla = rom+head->v6.ROM_total_size;
            zones->nbflash =  head->v6.FLA_nb_zone;
            break;
        default:
            return YERRMSG(YAPI_INVALID_ARGUMENT, "Unsupported file format (upgrade your tools)");
    }

    //decode all rom zones
    for(i=0 ; i<zones->nbrom && i < MAX_ROM_ZONES_PER_FILES ; i++){
        byn_zone *zone=(byn_zone*)rom;
        zones->rom[i].addr = zone->addr_page;
        zones->rom[i].len  = zone->len;
        zones->rom[i].ptr = (u8*)yMalloc(zone->len);
        memcpy(zones->rom[i].ptr,rom+sizeof(byn_zone),zone->len);
        zones->rom[i].nbinstr = zone->len/3;
        zones->rom[i].nbblock = zones->rom[i].nbinstr/PROGRAM_BLOCK_SIZE_INSTR;
        rom += sizeof(byn_zone)+zone->len;
    }
    //decode all Flash zones
    for(i=0 ; i<zones->nbflash && i < MAX_FLASH_ZONES_PER_FILES ; i++){
        byn_zone *zone=(byn_zone*)fla;
        zones->flash[i].page = zone->addr_page;
        zones->flash[i].len  = zone->len;
        zones->flash[i].ptr = (u8*)yMalloc(zone->len);
        memcpy(zones->flash[i].ptr,rom+sizeof(byn_zone),zone->len);
        fla += sizeof(byn_zone)+zone->len;
    }
    
    return 0;
}

void FreeZones(newmemzones *zones)
{
    u32 i;
    for(i=0 ; i<zones->nbrom && i<MAX_ROM_ZONES_PER_FILES ; i++){
        if(zones->rom[i].ptr){
            yFree(zones->rom[i].ptr);
        }
    }
    for(i=0 ; i < zones->nbflash && i <MAX_FLASH_ZONES_PER_FILES  ; i++){
        if(zones->flash[i].ptr){
            yFree(zones->flash[i].ptr);
        }
    }
    memset(zones,0,sizeof(newmemzones));
}



int yUSBGetBooloader(const char *serial, const char * name,  yInterfaceSt *iface,char *errmsg)
{

    int             nbifaces=0;
    yInterfaceSt    *curif;
    yInterfaceSt    *runifaces=NULL;
    int             i;    
    
    YPROPERR(yUSBGetInterfaces(&runifaces,&nbifaces,errmsg));
    //inspect all interfaces
    for(i=0, curif = runifaces ; i < nbifaces ; i++, curif++){
        // skip real devices
        if(curif->deviceid >YOCTO_DEVID_BOOTLOADER)
            continue;
#ifdef WINDOWS_API        
        if(name !=NULL && YSTRICMP(curif->devicePath,name)==0){
            memcpy(iface,curif,sizeof(yInterfaceSt));
            yFree(runifaces);
            return YAPI_SUCCESS; 
        }else 
#endif            
        if(serial!=NULL && YSTRCMP(curif->serial,serial)==0){
            memcpy(iface,curif,sizeof(yInterfaceSt));
            yFree(runifaces);
            return YAPI_SUCCESS;
        }            
    }
    // free all tmp ifaces
    if(runifaces){
        yFree(runifaces);
    } 
    return YERR(YAPI_DEVICE_NOT_FOUND);
}



#endif

