/*********************************************************************
 *
 * $Id: ythread.h 9723 2013-02-06 22:33:59Z seb $
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

#ifndef YTHREAD_H
#define YTHREAD_H
#include "ydef.h"

/*********************************************************************
 * EVENT FUNCTION 
 *********************************************************************/

#ifdef WINDOWS_API
#include <windows.h>
typedef HANDLE yEvent;
#else
#include <pthread.h>


typedef struct {
    pthread_cond_t   cond;
    pthread_mutex_t  mtx;
    int              verif;
} yEvent;

#endif

void   yCreateEvent(yEvent *ev);
void   ySetEvent(yEvent* ev);
int    yWaitForEvent(yEvent *ev,int time);
void   yCloseEvent(yEvent *ev);


/*********************************************************************
 * THREAD FUNCTION 
 *********************************************************************/
#ifdef WIN32
typedef HANDLE      osThread;
#else
typedef pthread_t   osThread;
#endif

int yCreateDetachedThread(void* (*fun)(void *), void *arg);

typedef enum {
    YTHREAD_NOT_STARTED=0,
    YTHREAD_RUNNING,
    YTHREAD_MUST_STOP,
    YTHREAD_STOPED
} YTHREAD_STATE;


typedef struct {
	void 			*ctx;
	yEvent 			ev;
	YTHREAD_STATE 	st;
	osThread   		th;
} yThread;

int    yThreadCreate(yThread *yth,void* (*fun)(void *), void *arg);
int    yThreadIsRunning(yThread *yth);
void   yThreadSignalStart(yThread *yth);
void   yThreadSignalEnd(yThread *yth);
void   yThreadRequestEnd(yThread *yth);
int    yThreadMustEnd(yThread *yth);
int    yThreadWaitEnd(yThread *yth);
int    yThreadIndex(void);

#endif


