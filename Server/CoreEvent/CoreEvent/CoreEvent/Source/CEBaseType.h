//
//  CEBaseType.h
//  CoreEvent
//
//  Created by CaiLianfeng on 2018/10/23.
//  Copyright © 2018年 com.wangqinghai. All rights reserved.
//

#ifndef CEBaseType_h
#define CEBaseType_h

#include <CCFoundation/CCFoundation.h>
#include <pthread.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/sysctl.h>
#include <fcntl.h>

typedef uint32_t CEFileEventMask_es;

extern const CEFileEventMask_es CEFileEventMaskNone;
extern const CEFileEventMask_es CEFileEventMaskReadable;
extern const CEFileEventMask_es CEFileEventMaskWritable;
extern const CEFileEventMask_es CEFileEventMaskBarrier;


typedef CEFileEventMask_es (*CEApiPoolGetEventMask_f)(void * _Nullable context, void * _Nonnull api, int fd);
typedef void (*CEApiPoolCallback_f)(void * _Nullable context, void * _Nonnull api, int index, int fd, CEFileEventMask_es mask);




struct _CERunLoop;
typedef struct _CERunLoop CERunLoop_s;


typedef void (*CEFileEventHandler_f)(CERunLoop_s * _Nonnull eventLoop, int fd, void * _Nullable clientData, int mask);
typedef int (*CETimeEventHandler_f)(CERunLoop_s * _Nonnull eventLoop, long long id, void * _Nullable clientData);
typedef void (*CEFinalizerHandler_f)(CERunLoop_s * _Nonnull eventLoop, void * _Nullable clientData);
typedef void (*CEStateChangeHandler_f)(CERunLoop_s * _Nonnull eventLoop);


/* File event structure */
typedef struct _CEFileEvent {
    uint32_t mask: 4;
    uint32_t readHandlerId: 10;
    uint32_t writeHandlerId: 10;
    uint32_t xxx: 8;
    void * _Nullable clientData;
} CEFileEvent_s;

/* Time event structure */
typedef struct _CETimeEvent {
    long long id; /* time event identifier. */
    long when_sec; /* seconds */
    long when_ms; /* milliseconds */
    CETimeEventHandler_f _Nonnull timeProc;
    CEFinalizerHandler_f _Nullable finalizerProc;
    void * _Nullable clientData;
    struct _CETimeEvent * _Nullable next;
} CETimeEvent_s;

/* A fired event */
typedef struct _CEFiredEvent {
    int fd;
    int mask;
} CEFiredEvent_s;



/* State of an event based program */
struct _CERunLoop {
    int fdLimit;
    int maxfd;   /* highest file descriptor currently registered */
    int setsize; /* max number of file descriptors tracked */
    long long timeEventNextId;
    time_t lastTime;     /* Used to detect system clock skew */
    CEFileEvent_s * _Nonnull events; /* Registered events */
    CEFiredEvent_s * _Nonnull fired; /* Fired events */
    CETimeEvent_s * _Nullable timeEventHead;
    int stop;
    void * _Nullable api; /* This is used for polling API specific data */
    CEStateChangeHandler_f _Nullable beforesleep;
    CEStateChangeHandler_f _Nullable aftersleep;
};


#endif /* CEBaseType_h */
