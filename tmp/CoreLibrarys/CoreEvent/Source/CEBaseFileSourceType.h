//
//  CEBaseFileSourceType.h
//  CoreEvent
//
//  Created by vector on 2018/12/18.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEBaseFileSourceType_h
#define CEBaseFileSourceType_h


#include <CoreEvent/CEBaseType.h>


typedef uint32_t CEFileSourceMask_es;

static const CEFileSourceMask_es CEFileSourceMaskNone = 0;
static const CEFileSourceMask_es CEFileSourceMaskReadable = 1;
static const CEFileSourceMask_es CEFileSourceMaskReadTimeout = 2;
static const CEFileSourceMask_es CEFileSourceMaskWritable = 4;
static const CEFileSourceMask_es CEFileSourceMaskWriteTimeout = 8;




struct __CEFileSourceGroup;
typedef struct __CEFileSourceGroup CEFileSourceGroup_t;
typedef CEFileSourceGroup_t * CEFileSourceGroupRef;


struct __CEFileSource;
typedef struct __CEFileSource CEFileSource_t;
typedef CEFileSource_t * CEFileSourceRef;


struct __CEFileEventHandle;
typedef struct __CEFileEventHandle CEFileEventHandle_t;
typedef CEFileEventHandle_t * CEFileEventHandleRef;


/* File event structure 32b-32B 64b-40B */
struct __CEFileEventHandle {
    uint32_t canceled: 1;//suspend
    const uint32_t sourcable: 1;
    const uint32_t timeout: 1;
    uint32_t timeoutInterval;
    
};

typedef void (*CEFileSourceHandler_f)(CEFileSourceHandlerRef _Nonnull handler, CEFileDescription_s fd, CEFileEventInfo_s * _Nonnull clientData, CEFileEventInfo_s eventInfo);
typedef void (*CEFileEventCallback_f)(CEFileDescription_s fd, CEFileEventInfo_s * _Nonnull context, void * _Nullable clientData, CEFileEventHandleRef _Nullable readHandle, CEFileEventHandleRef _Nullable writeHandle);

static struct {
    CEFileSourceGroupRef _Nullable groups[2048];
    pthread_mutex_t mutex;
    size_t count;
} CEFileSourceGroups = {};

void ttt() {
}



struct __CEFileSourceGroup {
    uint32_t id: 16;
    uint32_t mode: 1;//sync async
    
    uint32_t sourceCount;

    CEFileSourceGroupRef _Nullable prev;
    CEFileSourceGroupRef _Nullable next;
    CEFileSourceHandler_f _Nonnull func;
    void * _Nullable context;
    size_t count;
    uint64_t firedTime;
    
    
    
    
    
};

struct __CEFileSourceFiredContext {
    uint32_t count;
    uint32_t count1;

    
};


struct __CEFileSourceGroupHandleResult {
    uint32_t id;
    uint32_t mode: 1;//sync async
    
    
    CEFileSourceGroupRef _Nullable prev;
    CEFileSourceGroupRef _Nullable next;
    CEFileSourceHandler_f _Nonnull func;
    void * _Nullable context;
    size_t count;
    uint64_t firedTime;
    
    
    
    
    
};



/* File event structure 8B*/
typedef struct __CEFileSourceTimer {
    int prev;
    int next;
} CEFileSourceTimer_s;

/* File event structure 8B*/
typedef union __CEFileSourceTimeStorage {
    CEFileSourceTimer_s timer;
    uint64_t time;
} CEFileSourceTimeStorage_u;


/* File event structure 32b-32B 64b-40B */
struct __CEFileSource {
//    _Atomic(uint32_fast_t) ref;
    unsigned int sequence;
//    CEFileDescription_s fd;

    //fired events
    uint32_t valid:  1;
    uint32_t apiRegistered: 1;
    uint32_t firedMask: 4;
    uint32_t mask: 4;
    uint32_t apiMask: 4;//当前mask
    uint32_t readTimerSequence: 11;
    uint32_t writeTimerSequence: 11;

    
    CEFileSourceTimeStorage_u readTimer;//添加到timer list， 需要通过time字段计算timer的序列，添加到timer 之后time失效， 只能用timer
    CEFileSourceTimeStorage_u writeTimer;
    
    CEFileSourceGroupRef _Nonnull group;
    CEFileEventCallback_f _Nonnull callback;
    void * _Nullable clientData;
};

/* File event structure 32b-32B 64b-40B */
struct __CEFile {
    _Atomic(uint_fast32_t) ref;


    uint32_t firedMask: 4;
    uint32_t mask: 4;
    uint32_t apiRegistered: 1;
    uint32_t apiMask: 4;//当前mask
    uint32_t readTimerSequence: 11;
    uint32_t writeTimerSequence: 11;
    
    int fd;
    unsigned int sequence;
    
    CEFileSourceGroupRef _Nonnull readGroup;
    CEFileSourceGroupRef _Nonnull writeGroup;
    
    CEFileEventCallback_f _Nonnull callback;
    void * _Nullable clientData;
};



/* File event structure 32b-32B 64b-40B */
struct __CEFiredFileSource {
    CEFileDescription_s fd;

    
    //fired events
    //    uint32_t valid:  1;
    uint32_t firedMask: 4;
    uint32_t mask: 4;
    uint32_t apiRegistered: 1;
    uint32_t apiMask: 4;//当前mask
    uint32_t readTimerSequence: 11;
    uint32_t writeTimerSequence: 11;
    
    CEFileSourceGroupRef _Nonnull readGroup;
    CEFileSourceGroupRef _Nonnull writeGroup;
    
    CEFileEventCallback_f _Nonnull callback;

    CEFileEventHandler_f _Nonnull readHandle;
    CEFileEventHandler_f _Nonnull writeHandle;

    void * _Nullable clientData;
};








/*
 512
 512
 1024
 2048
 4096
 8192
 16384
 
 
 */
struct __CEFileSourceManager {
    CEFileSourceRef _Nullable fileEventsPages[32768]; /* Registered events, 0x10000 count peer page */
    CEFiredEvent_s * _Nullable firedPages[32768]; /* Fired events, 0x10000 count peer page  */
    
    int readTimerPages[CERunLoopFileTimerPageSize];
    int writeTimerPages[CERunLoopFileTimerPageSize];
    
    
    
    
    
};



struct __CEFileSourceGroupMap {
    CEFileSourceGroupRef _Nonnull bucket[1024];
    
    CEFileSourceHandler_f _Nonnull func;
    void * _Nullable context;
    
    
    
    
    
    
};




#endif /* CEBaseFileSourceType_h */
