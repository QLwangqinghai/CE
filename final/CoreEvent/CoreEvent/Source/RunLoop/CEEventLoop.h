//
//  CEEventLoop.h
//  CoreEvent
//
//  Created by vector on 2019/6/23.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CEEventLoop_h
#define CEEventLoop_h

#include <CoreEvent/CEThreadBase.h>


typedef uint64_t CEIdentfier_t;


/* File event structure */
typedef struct _CEFileEvent11 {
    _Atomic(uint_fast32_t) sequence;
    
    uint32_t writeOpened: 1;
    uint32_t readOpened: 1;
    uint32_t closed: 1;

    
    uint32_t writeMask: 1;
    uint32_t fireWriting: 1;
    uint32_t poolWrite: 1;

    uint32_t readMask: 1;
    uint32_t fireReading: 1;
    uint32_t poolRead: 1;
    
    uint32_t refcount: 10;
    
    uint32_t readQueueId: 16;
    uint32_t writeQueueId: 16;
    uint32_t poolId: 16;
    void * _Nullable clientData;
} CEFileEvent11_s;

/* File event structure */
typedef struct _CEFileEvent {
    int sockfd;
    _Atomic(uint_fast32_t) sequence;
    _Atomic(uint_fast32_t) m;

    uint32_t mask: 4;
    uint32_t readHandlerId: 10;
    uint32_t writeHandlerId: 10;
    uint32_t xxx: 8;
    void * _Nullable clientData;
} CEFileEvent_s;

static inline CEIdentfier_t CEFileEventGetIdentifier(CEFileEvent_s * _Nonnull event) {
    return 0;
}





#endif /* CEEventLoop_h */
