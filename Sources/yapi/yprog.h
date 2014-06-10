/*********************************************************************
 *
 * $Id: yprog.h 16260 2014-05-20 19:14:30Z mvuilleu $
 *
 * Declaration of firmware upgrade functions
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

#ifndef YPROG_H
#define YPROG_H
#include "ydef.h"

#ifdef MICROCHIP_API
typedef int ProgIface;
#else
#include "yproto.h"
typedef yInterfaceSt ProgIface;
#endif

#define MAX_ROM_ZONES_PER_FILES     16
#define MAX_FLASH_ZONES_PER_FILES   4

#define  BYN_SIGN  ((u32) ('B'| ((u16)'Y'<<8) | ((u32)'N'<<16) ))
#define  BYN_REV_V4 4
#define  BYN_REV_V5 5
#define  BYN_REV_V6 6

#ifndef C30
#pragma pack(push,1)
#endif


#define USE_V6_BYN_FILE


typedef struct{
    union {
        u32 sign;
        struct {
            char a;
            char b;
            char c;
            char d;
        } signchars;
    };
    u16 rev;
    char serial[YOCTO_SERIAL_LEN];
    char pictype[20];
    char product[YOCTO_PRODUCTNAME_LEN];
    char firmware[YOCTO_FIRMWARE_LEN];
}byn_head_sign;

typedef struct{
    u32 nbzones;
    u32 datasize;
}byn_head_v4;

typedef struct{
    char prog_version[YOCTO_FIRMWARE_LEN];
    u16 pad;
    u32 nbzones;
    u32 datasize;
}byn_head_v5;

typedef struct{
    u8  md5chk[16];
    char prog_version[YOCTO_FIRMWARE_LEN]; // 22 bytes
    u8  ROM_nb_zone;
    u8  FLA_nb_zone;
    u32 ROM_total_size;
    u32 FLA_total_size;
}byn_head_v6;

typedef struct{
    byn_head_sign h;
    union {
        byn_head_v6 v6;
        byn_head_v5 v5;
        byn_head_v4 v4;
     };
}byn_head_multi;

#define BYN_HEAD_SIZE_V4    (sizeof(byn_head_sign)+sizeof(byn_head_v4))
#define BYN_HEAD_SIZE_V5    (sizeof(byn_head_sign)+sizeof(byn_head_v5))
#define BYN_HEAD_SIZE_V6    (sizeof(byn_head_sign)+sizeof(byn_head_v6))
#define BYN_MD5_OFS_V6      (sizeof(byn_head_sign)+16)

typedef struct{
    u32 addr_page;
    u32 len;
}byn_zone;

typedef struct{
    u32 addr;
    u32 nbinstr;
    u32 nbblock;
    u8  *ptr;
    u32 len;    
}romzone;

typedef struct{
    u32 page;
    u8  *ptr;
    u32 len;
}flashzone;

typedef struct {
    u32         nbrom;
    u32         nbflash;
    romzone     rom[MAX_ROM_ZONES_PER_FILES];
    flashzone   flash[MAX_FLASH_ZONES_PER_FILES];
}newmemzones;

typedef struct{
    ProgIface   iface;
    u32         pr_blk_size;
    u32         er_blk_size;
    u32         last_addr;
    u32         settings_addr;
    u8          devid_family;
    u8          devid_model;
    u16         devid_rev;
    u32         startconfig;
    u32         endofconfig;
#ifndef MICROCHIP_API    
    u16         ext_jedec_id;
    u16         ext_page_size;
    u16         ext_total_pages;
    u16         first_code_page;
    u16         first_yfs3_page;
#endif    
}BootloaderSt;

// from yfirmupd.c
extern BootloaderSt firm_dev;
extern USB_Packet   firm_pkt;

#ifndef C30
#pragma pack(pop)
#endif

// Return 1 if the communication channel to the device is busy
// Return 0 if there is no ongoing transaction with the device
int ypIsSendBootloaderBusy(BootloaderSt *dev);

// Return 0 if there command was successfully queued for sending
// Return -1 if the output channel is busy and the command could not be sent
int ypSendBootloaderCmd(BootloaderSt *dev, const USB_Packet *pkt,char *errmsg);
// Return 0 if a reply packet was available and returned
// Return -1 if there was no reply available
int ypGetBootloaderReply(BootloaderSt *dev, USB_Packet *pkt,char *errmsg);
// Power cycle the device
void ypBootloaderShutdown(BootloaderSt *dev);

const char* prog_GetCPUName(BootloaderSt *dev);
int prog_RebootDevice(BootloaderSt *dev,u16 bootmode,char *errmsg);
int prog_BlankDevice(BootloaderSt *dev,char *errmsg);
int prog_FlashBlock(BootloaderSt *dev,u32 startAddr, u8 *data,int size,char *errmsg);
int prog_GetDeviceInfo(BootloaderSt *dev,char *errmsg);
int checkHardwareCompat(BootloaderSt *dev,const char *pictype);
int IsValidBynHead(const byn_head_multi *head, u32 size, char *errmsg);
int ValidateBynCompat(const byn_head_multi *head,u32 size,const char *serial, BootloaderSt *dev,char *errmsg);

#ifndef MICROCHIP_API
int IsValidBynFile(const byn_head_multi *head, u32 size,char *errmsg);
int prog_FlashDevice(yFlashArg *arg, int realyflash, char *errmsg);
int DecodeBynFile(const u8 *buffer,u32 size,newmemzones *zones,const char *serial, BootloaderSt *dev,char *errmsg);
int yUSBGetBooloader(const char *serial, const char * name,  yInterfaceSt *iface,char *errmsg);
void FreeZones(newmemzones *zones);
#endif
#endif
