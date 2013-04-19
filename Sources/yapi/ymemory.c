/*********************************************************************
 *
 * $Id: ymemory.c 9983 2013-02-22 15:14:56Z mvuilleu $
 *
 * Basic memory check function to prevent memory leak
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
#define __FILE_ID__  "ymemory"
#define YMEMORY_ALLOW_MALLOC
#include "yproto.h"
#ifdef YSAFE_MEMORY
typedef  enum{
    YMEM_NOT_USED=0,
    YMEM_MALLOCED,
    YMEM_FREED
} YMEM_STATE;

typedef struct{
    YMEM_STATE  state;
    const char *malloc_file;
    u32         malloc_line;
    u32         malloc_size;
    const char *free_file;
    u32         free_line;
    void        *ptr;
} YMEM_ENTRY;

YMEM_ENTRY          *yMap    = NULL;
u32                 yMapSize = 0;
u32                 yMapUsed = 0;
yCRITICAL_SECTION   yMapCS;



static void ymemdumpentry(YMEM_ENTRY *entry,const char* prefix)
{
    dbglog("%s %lX: alloc %s:%d of %db, free %s:%d\n",prefix,entry->ptr,entry->malloc_file,entry->malloc_line,entry->malloc_size,entry->free_file,entry->free_line);
}


static void  ymemdump(void)
{
    u32 i;
    YMEM_ENTRY *entry;
    u32 total,count;

    dbglog("ySafeMemoryDump: %d/%d entry\n\n",yMapUsed,yMapSize);
    dbglog("Malloc:\n");
    total=count=0;
    for(i=0, entry=yMap; i< yMapUsed ; i++,entry++){
        if(entry->state == YMEM_MALLOCED){
            ymemdumpentry(entry,"");
            dbglog("%s : %d of %db (0x%x)\n",entry->malloc_file,entry->malloc_line,entry->malloc_size,entry->ptr);            
            total+= entry->malloc_size;
            count++;
        }
    }
    dbglog("total: %db (%d Kb) on %d entry\n\n",total,(int)(total/1024),count);
#if 0    
    dbglog("Free:\n");
    for(i=0, entry=yMap; i< yMapUsed ; i++,entry++){
        if(entry->state == YMEM_FREED){
            dbglog("alloc %s:%d of %db, free %s:%d\n",entry->malloc_file,entry->malloc_line,entry->malloc_size,entry->free_file,entry->free_line);
        }
    }
#endif
}



void ySafeMemoryInit(u32 nbentry)
{
    YASSERT(yMap==NULL && yMapSize==0);
    yInitializeCriticalSection(&yMapCS);
    yEnterCriticalSection(&yMapCS);
    yMap = malloc(nbentry *sizeof(YMEM_ENTRY));
    if(yMap){
        yMapSize = nbentry;
        memset(yMap,0,nbentry *sizeof(YMEM_ENTRY));
        yMapUsed=0;
    }    
    yLeaveCriticalSection(&yMapCS);
}

void *ySafeMalloc(const char *file,u32 line,u32 size)
{
    u32 i;
    YMEM_ENTRY *entry;
    void *ptr;

    yEnterCriticalSection(&yMapCS);
    if(yMapUsed < yMapSize){
        //use a new one
        entry=yMap+yMapUsed;
    }else{
        // find a freed entry
        for(i=0; i< yMapSize;i++){
            if(yMap[i].state == YMEM_FREED)
                break;                
        }
        if(i==yMapSize){
            dbglog("No more entry available for ySafeMalloc\n\n");
            ymemdump();
            yLeaveCriticalSection(&yMapCS);
            return NULL;
        }
        entry = yMap+i;
    }

    ptr=malloc(size);
    if(!ptr){
        dbglog("No more memory available (unable to allocate %d bytes)\n\n",size);
        ymemdump();
        yLeaveCriticalSection(&yMapCS);
        return NULL;
    }

    memset(entry,0,sizeof(YMEM_ENTRY));
    entry->state = YMEM_MALLOCED;
    entry->malloc_file = file;
    entry->malloc_line = line;
    entry->ptr  = ptr;
    entry->malloc_size = size;
    if(yMapUsed < yMapSize)
        yMapUsed++;
    yLeaveCriticalSection(&yMapCS);

    return ptr;    
}

void  ySafeFree(const char *file,u32 line,void *ptr)
{
    u32 i;
    YMEM_ENTRY *entry;

    yEnterCriticalSection(&yMapCS);
    for(i=0, entry=yMap; i< yMapUsed ; i++,entry++){
        YASSERT(entry->state != YMEM_NOT_USED);
        if(entry->ptr == ptr)
            break;                
    }
    if(i == yMapUsed){
        dbglog("Free of unallocated pointer 0x%x at %s:%d\n\n",ptr,file,line);
        ymemdump();
        YASSERT(0);
    }
    if(entry->state == YMEM_FREED){
        dbglog("Free of allready freed pointer (0x%x) at %s:%d\n",ptr,file,line);
        dbglog("was allocated at %s:%d size =%d freed at %s:%d\n\n",
            entry->malloc_file, entry->malloc_line, entry->malloc_size, entry->free_file,entry->free_line);
        ymemdump();
        YASSERT(0);        
    }
    entry->free_file = file;
    entry->free_line = line;
    entry->state = YMEM_FREED;
    free(entry->ptr);
    entry->ptr=NULL;

    yLeaveCriticalSection(&yMapCS);
}

void  ySafeTrace(const char *file,u32 line,void *ptr)
{
    u32 i;
    YMEM_ENTRY *entry;
    
    yEnterCriticalSection(&yMapCS);
    for(i=0, entry=yMap; i< yMapUsed ; i++,entry++){
        YASSERT(entry->state != YMEM_NOT_USED);
        if(entry->ptr == ptr)
            break;
    }
    if(i == yMapUsed){
        dbglog("Update trace of unallocated pointer 0x%x at %s:%d\n\n",ptr,file,line);
        ymemdump();
        YASSERT(0);
    }
    if(entry->state == YMEM_FREED){
        dbglog("Update trace of allready freed pointer (0x%x) at %s:%d\n",ptr,file,line);
        dbglog("was allocated at %s:%d size =%d freed at %s:%d\n\n",
               entry->malloc_file, entry->malloc_line, entry->malloc_size, entry->free_file,entry->free_line);
        ymemdump();
        YASSERT(0);
    }
    ymemdumpentry(entry,"trace");
    entry->malloc_file = file;
    entry->malloc_line = line;    
    yLeaveCriticalSection(&yMapCS);
}

void  ySafeMemoryDump(void *discard)
{
    u32 i;
    YMEM_ENTRY *entry;

    yEnterCriticalSection(&yMapCS);
    for(i=0, entry=yMap; i< yMapUsed ; i++,entry++){
        if(entry->state == YMEM_MALLOCED && entry->ptr!=discard){
            break;
        }
    }
    if(i< yMapUsed){
        ymemdump();
    }
    yLeaveCriticalSection(&yMapCS);
}


void  ySafeMemoryStop(void)
{
    yDeleteCriticalSection(&yMapCS);
    free(yMap);
    yMap=NULL;
    yMapSize = yMapUsed = 0;
}

#endif
