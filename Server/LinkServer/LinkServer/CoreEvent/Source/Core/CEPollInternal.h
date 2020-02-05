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
    uint64_t isFired: 1;
    uint64_t isWaitingTimeout: 1;
    uint64_t isTimeout: 1;
    uint64_t hashOfTimer: 13;
    uint64_t prev: 24;
    uint64_t next: 24;
} CEFileSource_s;

extern const CEFileSource_s CEFileSourceInit;

typedef struct {
    uint64_t index: 24;
    uint64_t sequence: 40;
} CEFileInternalId;

//32B
typedef struct {
    uint64_t sequence: 40;
    uint64_t handler: 22;
    uint64_t mask: 2;
    CEFileSource_s read;
    CEFileSource_s write;
    uint64_t context;
} CEFile_t;

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
    CEFile_t * _Nonnull fileTable;

//    uint32_t observerBufferSize;
//    uint32_t observerBufferCount;
//
//    CERunLoopObserver_s * _Nullable * _Nonnull observers;
    CEFileEventHandler_s fileEventHandlers[CEPollFileHandlerTableSize];
    CEFileTimerMap_s readTimerMap;
    CEFileTimerMap_s writeTimerMap;
} CEPoll_s;
#pragma pack(pop)

#define CEFileHandlerInvalid 0x1FFFFF
#define CEFileIndexInvalid 0x7FFFFF
#define CEFileIndexMax 0x7FFFFFULL
#define CEFileIndexShift 41

#define CEFileIdSequenceMask 0x1FFFFFFFFFFULL


static inline CEFile_t * _Nonnull __CEPollGetFileWithIndex(CEPoll_s * _Nonnull p, uint64_t index) {
    CEFile_t * file = p->fileTable;
    file += index;
    return file;
}
static inline CEFileId __CEFileIdMake(uint64_t index, uint64_t seq) {
    assert(index <= CEFileIndexMax);
    uint64_t result = index;
    result = result << CEFileIndexShift;
    result |= (seq & CEFileIdSequenceMask);
    return result;
}

static inline CEFileId CEFileInternalIdToFileId(CEFileInternalId fid) {
    uint64_t result = fid.index;
    result = result << CEFileIndexShift;
    result |= fid.sequence;
    return result;
}
static inline CEFileInternalId CEFileIdToFileInternalId(CEFileId fid) {
    CEFileInternalId result = {
        .index = (fid >> CEFileIndexShift),
        .sequence = (fid & CEFileIdSequenceMask),
    };
    return result;
}

static inline CEFile_t * _Nullable _CEPollGetFile(CEPoll_s * _Nonnull p, CEFileInternalId id) {
    if (id.index >= p->fileTableSize) {
        return NULL;
    };
    CEFile_t * file = __CEPollGetFileWithIndex(p, id.index);
    if (file->sequence == id.sequence) {
        if (file->handler >= p->maxHandlerId) {
            return NULL;
        }
        return file;
    } else {
        return NULL;
    }
}

#endif /* CEPollInternal_h */
