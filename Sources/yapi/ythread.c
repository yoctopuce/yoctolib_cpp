/*********************************************************************
 *
 * $Id: ythread.c 10664 2013-03-25 11:12:04Z seb $
 *
 * Generic fifo queue (should be moved into a file that is common on
 * all platform
 *
 * - - - - - - - - - License information: - - - - - - - - - 
 *
 * Copyright (C) 2011 and beyond by Yoctopuce Sarl, Switzerland.
 *
 * 1) If you have obtained this file from www.yoctopuce.com using
 *    a valid customer account established in your proper name,
 *    Yoctopuce Sarl (hereafter Licensor) licenses to you (hereafter 
 *    Licensee) the right to use, modify, copy, and integrate this 
 *    source file into your own solution for the sole purpose of 
 *    interfacing a Yoctopuce product integrated into Licensee's
 *    solution.
 *
 *    You should refer to the license agreement accompanying this
 *    Software for additional information regarding your rights
 *    and obligations.
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
 * 2) If you have obtained this file from any other source, you
 *    are not entitled to use it, read it or create any derived 
 *    material. You should delete this file immediately.
 *
 *********************************************************************/

#include "ythread.h"
#define __FILE_ID__  "ythread"


#ifdef WINDOWS_API

static DWORD yTlsBucket = TLS_OUT_OF_INDEXES;
static DWORD yNextThreadIdx = 1;

void yCreateEvent(yEvent *event)
{
	*event= CreateEvent(0,0,0,0);
}

void ySetEvent(yEvent *ev)
{
	SetEvent(*ev);
}

int    yWaitForEvent(yEvent *ev,int time)
{
	DWORD usec;
	if(time<=0){
		usec=INFINITE;
	}else{
		usec=time;
	}
	WaitForSingleObject(*ev,usec);
	return 1;
}

void   yCloseEvent(yEvent *ev)
{
	CloseHandle(*ev);
}


static int    yCreateDetachedThreadEx(osThread *th_hdl,void* (*fun)(void *), void *arg)
{
    *th_hdl = CreateThread( 
                    NULL,                   // default security attibutes
                    0,                      // use default stack size  
                    (LPTHREAD_START_ROUTINE)fun,   // thread function name
                    arg,                    // argument to thread function 
                    0,                      // use default creation flags 
                    NULL);
    if (*th_hdl==NULL) {
        return -1;
    }
    return 0;
}


static void    yReleaseDetachedThreadEx(osThread *th_hdl)
{
    CloseHandle(*th_hdl);
}


static int    yWaitEndThread(osThread *th)
{
    int result = WaitForSingleObject(*th, INFINITE);
    return result==WAIT_FAILED?-1:0;
}

int    yThreadIndex(void)
{
    DWORD res;
    
    if(yTlsBucket == TLS_OUT_OF_INDEXES) {
        // Only happens the very first time, from main thread
        yTlsBucket = TlsAlloc();
    }
    res = (DWORD)TlsGetValue(yTlsBucket);
    if(res == 0) {
        // tiny risk of race condition, but thread idx is only
        // used for debug log purposes and is not sensitive
        res = yNextThreadIdx++;
        TlsSetValue(yTlsBucket, (void *)res);
    }
    return res;
}

#else
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>

static pthread_once_t yInitKeyOnce = PTHREAD_ONCE_INIT;
static pthread_key_t yTsdKey;
static unsigned yNextThreadIdx = 1;

static void initTsdKey()
{
    pthread_key_create(&yTsdKey, NULL);
}

void yCreateEvent(yEvent *ev)
{
    pthread_cond_init(&ev->cond, NULL);
    pthread_mutex_init(&ev->mtx, NULL);
    ev->verif=0;
}

void    ySetEvent(yEvent *ev)
{
    pthread_mutex_lock(&ev->mtx);
    ev->verif=1;
    // set verif to 1 because pthread condition seems
    // to allow conditional wait to exit event if nobody
    // has set the alarm (see google or linux books of seb)
    pthread_cond_signal(&ev->cond);
    pthread_mutex_unlock(&ev->mtx);

}

int   yWaitForEvent(yEvent *ev,int time)
{
    int retval;
    pthread_mutex_lock(&ev->mtx);
    if(!ev->verif){
        if(time>0){
            struct timeval now;
            struct timespec later;
            gettimeofday(&now, NULL);          
            later.tv_sec=now.tv_sec+ time/1000;
            later.tv_sec=now.tv_usec*1000 +(time%1000)*1000000;
            pthread_cond_timedwait(&ev->cond, &ev->mtx, &later);
        }else{
            pthread_cond_wait(&ev->cond,&ev->mtx);    
        }
    }
    retval=ev->verif;
    ev->verif=0;
    pthread_mutex_unlock(&ev->mtx);
    return retval;
	
}
void   yCloseEvent(yEvent *ev)
{
    pthread_cond_destroy(&ev->cond);
    pthread_mutex_destroy(&ev->mtx);
}

static int    yCreateDetachedThreadEx(osThread *th,void* (*fun)(void *), void *arg)
{
    pthread_attr_t attr;
    int result;
    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    if(pthread_create(th, &attr ,fun,arg)!=0){
        result=-1;
    }else{
        result=0;
    }
    pthread_attr_destroy(&attr);
    
	return result;
}

static void    yReleaseDetachedThreadEx(osThread *th_hdl)
{
}



static int yWaitEndThread(osThread *th)
{
    return pthread_join(*th,NULL);
}

int    yThreadIndex(void)
{
    int res;
    
    pthread_once(&yInitKeyOnce, initTsdKey);
    res = (int)((u8 *)pthread_getspecific(yTsdKey)-(u8 *)NULL);
    if(!res) {
        // tiny risk of race condition, but thread idx is only
        // used for debug log purposes and is not sensitive
        res = yNextThreadIdx++;
        pthread_setspecific(yTsdKey, (void*)((u8 *)NULL + res));
    }
    return res;
}

#endif




int    yCreateDetachedThread(void* (*fun)(void *), void *arg)
{
	osThread th_hdl;
	if(yCreateDetachedThreadEx(&th_hdl,fun,arg)<0){
		return -1;
	}
	yReleaseDetachedThreadEx(&th_hdl);
	return 0;
}





int    yThreadCreate(yThread *yth,void* (*fun)(void *), void *arg)
{
    if(yth->st == YTHREAD_RUNNING)
        return 0; // allready started nothing to do
    if(yth->st == YTHREAD_NOT_STARTED){
        yth->ctx = arg;
        yCreateEvent(&yth->ev);
        if(yCreateDetachedThreadEx(&yth->th,fun,yth)<0){
            yCloseEvent(&yth->ev);
            return-1;
        }
        yWaitForEvent(&yth->ev,0);
        yCloseEvent(&yth->ev);        
        return 1;
    }
    return -1;
}

int yThreadIsRunning(yThread *yth)
{
    return (yth->st == YTHREAD_RUNNING);
}

void   yThreadSignalStart(yThread *yth)
{
    //send ok to parent thread
    yth->st = YTHREAD_RUNNING;
    ySetEvent(&yth->ev);
}


void   yThreadSignalEnd(yThread *yth)
{
     yth->st = YTHREAD_STOPED;
}

void   yThreadRequestEnd(yThread *yth)
{
    if( yth->st == YTHREAD_RUNNING){
        yth->st = YTHREAD_MUST_STOP;
    }
}

int    yThreadMustEnd(yThread *yth)
{
    return yth->st != YTHREAD_RUNNING;
}

int    yThreadWaitEnd(yThread *yth)
{
	int res = yWaitEndThread(&yth->th);   
	yReleaseDetachedThreadEx(&yth->th);
	return res;
}


#ifdef DEBUG_CRITICAL_SECTION

//#include "yproto.h"
/* printf example */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DB_CS 128



typedef struct {
    int         thread;
    const char* fileid;
    int         lineno;
} yCRITICAL_SECTION_LOC;

typedef enum {
    CS_FREE=0, CS_LOCKED=1,CS_RELEASED=2
} CS_STATE;

static const char*  CS_STATE_STR[]= {
    "CS_FREE", "CS_LOCKED","CS_RELEASED"
};


typedef struct {
    int                 no;
    CS_STATE            state;
#ifdef  MICROCHIP_API
    u8                  cs;
#elif defined(WINDOWS_API)
    CRITICAL_SECTION    cs;
#else
    pthread_mutex_t     cs;
#endif
    yCRITICAL_SECTION_LOC init;
    yCRITICAL_SECTION_LOC enter;
    yCRITICAL_SECTION_LOC leave;
    yCRITICAL_SECTION_LOC release;
} yDbgCRITICAL_SECTION;

int nbycs=0;
yDbgCRITICAL_SECTION allycs[MAX_DB_CS];


static void dump_YCS( yCRITICAL_SECTION *csno)
{
    int i,s;
    yDbgCRITICAL_SECTION *ycs = & allycs[csno->no];
    const char* st=CS_STATE_STR[ycs->state];
    printf(
        "%lx:%02x:%s(%d:%s:%d -> %d:%s:%d) %s\n",(long)((void*)csno-NULL),csno->no,st,
        ycs->init.thread,ycs->init.fileid,ycs->init.lineno,
        ycs->release.thread,ycs->release.fileid,ycs->release.lineno,
        "");
    printf(
        "last enter %d:%s:%d  last Leave: %d:%s:%d\n",ycs->enter.thread,ycs->enter.fileid,ycs->enter.lineno,
        ycs->leave.thread,ycs->leave.fileid,ycs->leave.lineno);



    for(s=0 ; s<=CS_RELEASED; s++){
        for (i=0;i<nbycs;i++){
            ycs = & allycs[i];
            st=CS_STATE_STR[ycs->state];
            if(ycs->state!=s)
                continue;
            printf(
                "%02x:%s(%d:%s:%d -> %d:%s:%d) %s\n",i,st,
                ycs->init.thread,ycs->init.fileid,ycs->init.lineno,
                ycs->release.thread,ycs->release.fileid,ycs->release.lineno,
                i==csno->no?"<----":"");
        }
    }
}


#define CS_ASSERT(x)   if(!(x)){printf("ASSERT FAILED:%s:%d (%s:%d)\n",__FILE__ , __LINE__,fileid,lineno);dump_YCS(csno);while(1);}

#define CS_ON_STACK


void yDbgInitializeCriticalSection(const char* fileid, int lineno, yCRITICAL_SECTION *csno)
{
    int res;
    yDbgCRITICAL_SECTION *ycs;

    memset(csno,0xCA,sizeof(yCRITICAL_SECTION));

    CS_ASSERT(nbycs<MAX_DB_CS);
    csno->no = nbycs++;
    csno->dummy_cs = malloc(sizeof(pthread_mutex_t));
    ycs = allycs + csno->no;

    memset(ycs,0,sizeof(yCRITICAL_SECTION));
    ycs->state=CS_FREE;
    ycs->init.thread = yThreadIndex();
    ycs->init.fileid=fileid;
    ycs->init.lineno=lineno;
    memset(&(ycs->cs),0xFE,sizeof(ycs->cs));
    
#if MICROCHIP_API
    ycs->cs=0;
    res =0;
#elif defined(WINDOWS_API)
    res =InitializeCriticalSection(&(ycs->cs);
#else
    res = pthread_mutex_init(&(ycs->cs),NULL);

#endif
    CS_ASSERT(res==0);
#ifdef CS_ON_STACK
    res = pthread_mutex_init(csno->dummy_cs,NULL);
    CS_ASSERT(res==0);
#endif
}

void yDbgEnterCriticalSection(const char* fileid, int lineno, yCRITICAL_SECTION *csno)
{
    int res;
    yDbgCRITICAL_SECTION *ycs;

    CS_ASSERT(csno->no < nbycs);
    ycs = allycs + csno->no;
    CS_ASSERT(ycs->state == CS_FREE || ycs->state==CS_LOCKED );


#if MICROCHIP_API
    ycs->cs=1;
#elif defined(WINDOWS_API)
    res =EnterCriticalSection(&(ycs->cs);
#else
    res = pthread_mutex_lock(&(ycs->cs));
#endif
    CS_ASSERT(res==0);
    ycs->enter.thread = yThreadIndex();
    ycs->enter.fileid=fileid;
    ycs->enter.lineno=lineno;
    ycs->state = CS_LOCKED;
#ifdef CS_ON_STACK
    res = pthread_mutex_lock(csno->dummy_cs);
    CS_ASSERT(res==0);
#endif
}
int yDbgTryEnterCriticalSection(const char* fileid, int lineno, yCRITICAL_SECTION *csno)
{
    int res;
    yDbgCRITICAL_SECTION *ycs;

    CS_ASSERT(csno->no < nbycs);
    ycs = allycs + csno->no;

    CS_ASSERT(ycs->state == CS_FREE || ycs->state==CS_LOCKED );

#if MICROCHIP_API
    if(ycs->cs)
        return 0;
    ycs->cs=1;
#elif defined(WINDOWS_API)
    res = TryEnterCriticalSection(&(ycs->cs);
    if(res==0)
        return 0
    CS_ASSERT(res==1);
#else
    res = pthread_mutex_trylock(&(ycs->cs));
    if(res ==EBUSY)
        return 0;
    CS_ASSERT(res==0);
#endif
    CS_ASSERT(ycs->state == CS_FREE);
    ycs->enter.thread = yThreadIndex();
    ycs->enter.fileid=fileid;
    ycs->enter.lineno=lineno;
    ycs->state = CS_LOCKED;
#ifdef CS_ON_STACK
    res = pthread_mutex_trylock(csno->dummy_cs);
    CS_ASSERT(res==0);
#endif

    return res;

}
void yDbgLeaveCriticalSection(const char* fileid, int lineno, yCRITICAL_SECTION *csno)
{
    int res;
    yDbgCRITICAL_SECTION *ycs;

    CS_ASSERT(csno->no < nbycs);
    ycs = allycs + csno->no;
    CS_ASSERT(ycs->state == CS_LOCKED);

    ycs->leave.thread = yThreadIndex();
    ycs->leave.fileid=fileid;
    ycs->leave.lineno=lineno;
    ycs->state = CS_FREE;

#ifdef CS_ON_STACK
    res = pthread_mutex_unlock(csno->dummy_cs);
    CS_ASSERT(res==0);
#endif


#if MICROCHIP_API
    ycs->cs=0;
    res =0;
#elif defined(WINDOWS_API)
    res = LeaveCriticalSection(&(ycs->cs);
#else
    res = pthread_mutex_unlock(&(ycs->cs));
#endif
    CS_ASSERT(res==0);
}
void yDbgDeleteCriticalSection(const char* fileid, int lineno, yCRITICAL_SECTION *csno)
{
    int res;
    yDbgCRITICAL_SECTION *ycs;

    CS_ASSERT(csno->no < nbycs);
    ycs = allycs + csno->no;
    CS_ASSERT(ycs->state == CS_FREE);

#if MICROCHIP_API
    ycs->cs=0xCA;
    res = 0;
#elif defined(WINDOWS_API)
    res = DeleteCriticalSection(&(ycs->cs);
#else
    res = pthread_mutex_destroy(&(ycs->cs));
#endif
    CS_ASSERT(res==0);
    ycs->release.thread = yThreadIndex();
    ycs->release.fileid=fileid;
    ycs->release.lineno=lineno;
    ycs->state = CS_RELEASED;


#ifdef CS_ON_STACK
    res = pthread_mutex_destroy(csno->dummy_cs);
    CS_ASSERT(res==0);
    free(csno->dummy_cs);

#endif
}

#elif !defined(MICROCHIP_API) && !defined(WINDOWS_API)

#include <stdio.h>
#include <stdlib.h>

void yInitializeCriticalSection(yCRITICAL_SECTION *cs)
{
    cs->mutex_ptr = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(cs->mutex_ptr,NULL);
}

void yEnterCriticalSection(yCRITICAL_SECTION *cs)
{
    pthread_mutex_lock(cs->mutex_ptr);
}

int yTryEnterCriticalSection(yCRITICAL_SECTION *cs)
{
    int res;
    res = pthread_mutex_trylock(cs->mutex_ptr);
    if(res ==EBUSY)
        return 0;
    return 1;

}

void yLeaveCriticalSection(yCRITICAL_SECTION *cs)
{
    pthread_mutex_unlock(cs->mutex_ptr);
}

void yDeleteCriticalSection(yCRITICAL_SECTION *cs)
{
    pthread_mutex_destroy(cs->mutex_ptr);
    free(cs->mutex_ptr);
    cs->mutex_ptr=NULL;
}

#endif


