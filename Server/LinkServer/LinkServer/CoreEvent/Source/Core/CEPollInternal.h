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
#define CEPollFileHandlerTableSize 0x1000

#pragma pack(push, 2)

typedef struct {
    uint16_t isFired: 1;
    uint16_t isWaitingTimeout: 1;
    uint16_t isTimeout: 1;
    uint16_t hashOfTimer: 15;
    uint32_t prev: 23;
    uint32_t next: 23;
} CEFileSource_s;

extern const CEFileSource_s CEFileSourceInit;

typedef struct {
    uint64_t sequence: 41;
    uint64_t handler: 21;
    uint64_t mask: 2;
    CEFileSource_s read;
    CEFileSource_s write;
} CEFile_s;

typedef struct {
    CEFileId id;
    uint32_t handler;
} CEFileFiredInfo;


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
    uint32_t handlerId;
    uint32_t count;
    CEFileId items[1023];
} CEFileEventBuffer_s;

typedef struct {
    uint32_t table[0x10000];
} CEFileTimerMap_s;


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
    uint32_t timerTableOffset;//source timer 的 游标
    uint32_t blockEvent;
    uint32_t tmpBufferSize;
    CEBlockQueue_s blockQueue;
    CCPtr _Nonnull tmpBuffer;
    CETimeEventQueue_s timerQueue;
    CCMicrosecondTime currentTime;//单位 微秒
    uint32_t fileTableSize;
    uint32_t maxHandlerId;
    CEFile_s * _Nonnull fileTable;

//    uint32_t observerBufferSize;
//    uint32_t observerBufferCount;
//
//    CERunLoopObserver_s * _Nullable * _Nonnull observers;
    CEFileEventHandler_s fileEventHandlers[CEPollFileHandlerTableSize];
    CEFileTimerMap_s readTimerMap;
    CEFileTimerMap_s writeTimerMap;
} CEPoll_s;
#pragma pack(pop)

#define CEFileIndexInvalid 0xffff


static inline CEFile_s * _Nonnull _CEPollGetFile(CEPoll_s * _Nonnull p, uint32_t index) {
    CEFile_s * file = p->fileTable;
    file += index;
    return file;
}
static inline CEFileId CEFileIdMake(uint64_t fd, uint64_t seq) {
    CEFileId f = {
        .fd = fd,
        .sequence = seq,
    };
    return f;
}


#endif /* CEPollInternal_h */
