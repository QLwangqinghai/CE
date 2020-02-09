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
extern CCRefType CERefTypeTimeEvent;
extern void _CETimeEventDeinit(CETimeEventRef _Nonnull ref);


#define CEPollFileTimerHashShift 2
#define CEPollFileHandlerTableSize 0x4000
#define CEPollFileContextMax 0x1ffff

#pragma pack(push, 2)

//8B
typedef struct {
    uint32_t context: 30;
    uint32_t mask: 2;
    uint32_t handler: 14;
    uint32_t xx: 18;
} CEFile_t;

typedef struct {
    CEPollFileEventCallback_f _Nonnull handleFileEvent;
    CCPtr _Nullable context;
} CEFileEventHandler_s;


typedef struct {
    pthread_t _Nullable thread;
    uint32_t progress;//
    uint32_t blockEvent;
    size_t tmpBufferSize;
    CEBlockQueue_s blockQueue;
    CCPtr _Nonnull tmpBuffer;
    CETimerQueue_s * _Nonnull timerQueue;
    CCMicrosecondTime currentTime;//单位 微秒
    uint32_t fileTableSize;
    uint32_t fileStatusTableSize;
    uint32_t handlerCount;
    CEFile_t * _Nonnull fileTable;
    CCUInt8 * _Nonnull fileStatusTable;

    void * _Nullable api; /* This is used for polling API specific data */

//    uint32_t observerBufferSize;
//    uint32_t observerBufferCount;
//
//    CERunLoopObserver_s * _Nullable * _Nonnull observers;
    CEFileEventHandler_s fileEventHandlers[CEPollFileHandlerTableSize];
} CEPoll_s;
#pragma pack(pop)


static inline void __CEPollSetStatus(CEPoll_s * _Nonnull p, uint32_t index, uint32_t status) {
    CCUInt8 offsets[4] = {6, 4, 2, 0};
    CCUInt8 mask[4] = {0x3F, 0xCF, 0xF3, 0x3};
    uint32_t offset = index % 4;
    CCUInt8 * ptr = p->fileStatusTable + (index >> 2);
    CCUInt8 v = (status & 0x3) << (offsets[offset]);
    *ptr = ((*ptr) & mask[offset]) | v;
}

static inline uint32_t __CEPollGetStatus(CEPoll_s * _Nonnull p, uint32_t index) {
    CCUInt8 offsets[4] = {6, 4, 2, 0};
    CCUInt8 mask[4] = {0x3F, 0xCF, 0xF3, 0x3};
    uint32_t offset = index % 4;
    CCUInt8 v = p->fileStatusTable[(index >> 2)];
    CCUInt8 status = (v & mask[offset]) >> (offsets[offset]);
    return status;
}





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
