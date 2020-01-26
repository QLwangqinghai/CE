//
//  CELoop.h
//  CoreEvent
//
//  Created by 王青海 on 2018/10/24.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CERunLoop_h
#define CERunLoop_h

#include "CEBaseType.h"

#define AE_OK 0
#define AE_ERR -1


#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_DONT_WAIT 4
#define AE_CALL_AFTER_SLEEP 8

#define AE_NOMORE -1
#define AE_DELETED_EVENT_ID -1

/* Macros */
#define AE_NOTUSED(V) ((void) V)


/* Prototypes */
CERunLoop_s * _Nonnull CERunLoopShared(void);
CERunLoop_s * _Nullable CERunLoopCreate(int setsize);
void CERunLoopDeinit(CERunLoop_s * _Nonnull eventLoop);
void CERunLoopStop(CERunLoop_s * _Nonnull eventLoop);

int CERunLoopAddFileEvent(CERunLoop_s * _Nonnull eventLoop, int fd, CEFileEventMask_es mask, uint32_t handlerId, void * _Nullable clientData);
void CERunLoopRemoveFileEvent(CERunLoop_s * _Nonnull eventLoop, int fd, CEFileEventMask_es mask);
CEFileEventMask_es CEGetFileEvents(CERunLoop_s * _Nonnull eventLoop, int fd);
long long CERunLoopAddTimeEvent(CERunLoop_s * _Nonnull eventLoop, long long milliseconds, CETimeEventHandler_f _Nonnull proc, void * _Nullable clientData, CEFinalizerHandler_f _Nullable finalizerProc);

int CERunLoopRemoveTimeEvent(CERunLoop_s * _Nonnull eventLoop, long long id);

/* return the number of processed file/time events */
int CERunLoopProcessEvents(CERunLoop_s * _Nonnull eventLoop, int flags);
int CEWait(int fd, int mask, int milliseconds);
void CERunLoopRun(CERunLoop_s * _Nonnull eventLoop);
char * _Nonnull CERunLoopGetApiName(void);
void CESetBeforeSleepProc(CERunLoop_s * _Nonnull eventLoop, CEStateChangeHandler_f _Nullable beforesleep);
void CESetAfterSleepProc(CERunLoop_s * _Nonnull eventLoop, CEStateChangeHandler_f _Nullable aftersleep);
int CEGetSetSize(CERunLoop_s * _Nonnull eventLoop);
int CEResizeSetSize(CERunLoop_s * _Nonnull eventLoop, int setsize);


#endif /* CELoop_h */
