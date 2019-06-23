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


typedef uint32_t CEFileEventMask_es;

extern const CEFileEventMask_es CEFileEventMaskNone;
extern const CEFileEventMask_es CEFileEventMaskRead;
extern const CEFileEventMask_es CEFileEventMaskWrite;
extern const CEFileEventMask_es CEFileEventMaskAll;



/* File event structure */
typedef struct _CEFileEvent11 {
    _Atomic(uint_fast32_t) sequence;
    uint32_t writeMask: 1;
    uint32_t fireWriting: 1;
    uint32_t poolWrite: 1;
    uint32_t readQueueId: 13;

    uint32_t readMask: 1;
    uint32_t fireReading: 1;
    uint32_t poolRead: 1;
    
    uint32_t writeQueueId: 13;
    void * _Nullable clientData;
} CEFileEvent11_s;

/* File event structure */
typedef struct _CEFileEvent {
    uint32_t mask: 4;
    uint32_t readHandlerId: 10;
    uint32_t writeHandlerId: 10;
    uint32_t xxx: 8;
    void * _Nullable clientData;
} CEFileEvent_s;






#endif /* CEEventLoop_h */
