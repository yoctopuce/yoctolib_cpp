/*********************************************************************
 *
 * $Id: yfifo.h 9683 2013-02-05 09:02:21Z seb $
 *
  * Declaration of a generic fifo queue 
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
#ifndef YFIFO_H
#define YFIFO_H
#include "ydef.h"

#ifndef MICROCHIP_API
#define YFIFO_USE_MUTEX
#endif

//#define DEBUG_FIFO


#if defined(WINDOWS_API) && defined(YFIFO_USE_MUTEX)
#include <Windows.h>
#endif

typedef struct {
    u16 buffsize;
    u16 datasize;
    u8 *buff;
    u8 *head;
    u8 *tail;
#ifdef YFIFO_USE_MUTEX
	yCRITICAL_SECTION cs;
#endif
#ifdef DEBUG_FIFO
	const char* fileid;
	int 		line;
    u32 		totalPushed;
    u32 		totalPopded;
#endif
} yFifoBuf;

#define YFIFOEND(fifobuf)   ((fifobuf)->buff + (fifobuf)->buffsize)


#ifdef DEBUG_FIFO
void 	yFifoInitEx(const char* fileid,int line, yFifoBuf *buf, u8 *buffer, u16 bufflen);
#define yFifoInit(fifo,buffer,len)  yFifoInitEx(__FILE_ID__,__LINE__,fifo,buffer,len);
#else
void 	yFifoInitEx(yFifoBuf *buf, u8 *buffer, u16 bufflen);
#define yFifoInit(fifo,buffer,len)  yFifoInitEx(fifo,buffer,len);
#endif

#ifndef MICROCHIP_API
void yFifoCleanup(yFifoBuf *buf);
#endif

// Ex version do not have muxtex
void yFifoEmptyEx(yFifoBuf *buf);
u16  yPushFifoEx(yFifoBuf *buf, const u8 *data, u16 datalen);
u16  yForceFifoEx(yFifoBuf *buf, const u8 *data, u16 datalen);
u16  yPopFifoEx(yFifoBuf *buf, u8 *data, u16 datalen);
u16  yPeekFifoEx(yFifoBuf *buf, u8 *data, u16 datalen, u16 startofs);
u16  yPeekContinuousFifoEx(yFifoBuf *buf, u8 **ptr,u16 *len,u16 startofs);
u16  ySeekFifoEx(yFifoBuf *buf, const u8* pattern, u16 patlen,  u16 startofs, u16 searchlen, u8 bTextCompare);
u16  yFifoGetUsedEx(yFifoBuf *buf);
u16  yFifoGetFreeEx(yFifoBuf *buf);

#ifdef YFIFO_USE_MUTEX
// mutex non-Ex function call yFifoEnterCs and yFifoLeaveCs
// before calling Ex version

void yFifoEnterCS(yFifoBuf *buf);
void yFifoLeaveCS(yFifoBuf *buf);
void yFifoEmpty(yFifoBuf *buf);
u16  yPushFifo(yFifoBuf *buf, const u8 *data, u16 datalen);
u16  yForceFifo(yFifoBuf *buf, const u8 *data, u16 datalen);
u16  yPopFifo(yFifoBuf *buf, u8 *data, u16 datalen);
u16  yPeekFifo(yFifoBuf *buf, u8 *data, u16 datalen, u16 startofs);
u16  yPeekContinuousFifo(yFifoBuf *buf, u8 **ptr,u16 *len,u16 startofs);
u16  ySeekFifo(yFifoBuf *buf, const u8* pattern, u16 patlen,  u16 startofs, u16 searchlen, u8 bTextCompare);
u16  yFifoGetUsed(yFifoBuf *buf);
u16  yFifoGetFree(yFifoBuf *buf);

#else
// no mutex -> map function to Ex version
#define yFifoEnterCS(buf)
#define yFifoLeaveCS(buf)
#define yFifoEmpty(buf)							yFifoEmptyEx(buf)
#define yPushFifo(buf,data, datalen)			yPushFifoEx(buf,data,datalen)
#define yForceFifo(buf, data, datalen)			yForceFifoEx(buf,data,datalen)
#define yPopFifo(buf, data, datalen)			yPopFifoEx(buf,data,datalen)
#define yPeekFifo(buf, data, datalen, startofs)	yPeekFifoEx(buf,data,datalen,startofs)
#define yPeekContinuousFifo(buf,ptr,len,stofs)	yPeekContinuousFifoEx(buf,ptr,len,stofs)
#define ySeekFifo(buf, pattern, patlen,  startofs, searchlen, bTextCompare)	ySeekFifoEx(buf,pattern,patlen,startofs,searchlen,bTextCompare)
#define yFifoGetUsed(buf)						yFifoGetUsedEx(buf)
#define yFifoGetFree(buf)						yFifoGetFreeEx(buf)
#endif
//misc functions
void yxtoa(u32 x, char *buf, u16 len);


#endif
