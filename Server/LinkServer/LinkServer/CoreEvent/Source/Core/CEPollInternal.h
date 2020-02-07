//
//  CEPollInternal.h
//  CoreEvent
//
//  Created by vector on 2020/1/27.
//  Copyright © 2020 vector. All rights reserved.
//

#ifndef CEPollInternal_h
#define CEPollInternal_h

#include "CEPoll.h"


#define CEPollFileTimerHashShift 2
#define CEPollFileHandlerTableSize 0x4000
#define CEPollFileContextMax 0x1ffff

#pragma pack(push, 2)

//4B
typedef struct {
    uint32_t context: 16;
    uint32_t handler: 14;
    uint32_t mask: 2;
} CEFile_t;

typedef struct {
    CEPollFileEventCallback_f _Nonnull handleFileEvent;
    CCPtr _Nullable context;
} CEFileEventHandler_s;


/*
 none
 checking
 timeout
 */
//typedef uint32_t CEFileTimerStatus_t;
//const CEFileTimerStatus_t CEFileTimerStatusNone = 0;
//const CEFileTimerStatus_t CEFileTimerStatusNormal = 1;
//const CEFileTimerStatus_t CEFileTimerStatusTimeout = 2;

/* File event structure 16B*/

//typedef struct {
//    CCRange32 range;
//    CEFileEventHandler_s handler;
//} CEFileHandler_s;


typedef struct {
    pthread_t _Nullable thread;
    void * _Nullable api; /* This is used for polling API specific data */

#if __APPLE__
    os_unfair_lock blockQueueLock;
#else
    pthread_spinlock_t blockQueueLock;
#endif
    uint64_t fdSequence;
    uint32_t progress;//
    uint32_t blockEvent;
    size_t tmpBufferSize;
    CEBlockQueue_s blockQueue;
    CCPtr _Nonnull tmpBuffer;
    CETimeEventQueue_s timerQueue;
    CCMicrosecondTime currentTime;//单位 微秒
    uint32_t fileTableSize;
    uint32_t handlerCount;
    CEFile_t * _Nonnull fileTable;

//    uint32_t observerBufferSize;
//    uint32_t observerBufferCount;
//
//    CERunLoopObserver_s * _Nullable * _Nonnull observers;
    CEFileEventHandler_s fileEventHandlers[CEPollFileHandlerTableSize];
} CEPoll_s;
#pragma pack(pop)

static inline CEFile_t * _Nonnull __CEPollGetFileWithIndex(CEPoll_s * _Nonnull p, int index) {
    CEFile_t * file = p->fileTable;
    file += index;
    return file;
}

static inline CEFile_t * _Nullable _CEPollGetFile(CEPoll_s * _Nonnull p, int fd) {
    if (fd >= p->fileTableSize) {
        return NULL;
    };
    CEFile_t * file = __CEPollGetFileWithIndex(p, fd);
    if (file->handler >= p->handlerCount) {
        return NULL;
    }
    return file;
}

#endif /* CEPollInternal_h */
