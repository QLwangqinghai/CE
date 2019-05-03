//
//  CEFileEvent.c
//  CoreEvent
//
//  Created by vector on 2018/11/26.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CEFileEvent.h"
#include "CEMemory.h"
#include "CEConfig.h"


void CEFileEventRemoveFromReadTimer(CERunLoop_s * _Nonnull eventLoop, CEFileEvent_s * _Nonnull fileEvent);
void CEFileEventRemoveFromWriteTimer(CERunLoop_s * _Nonnull eventLoop, CEFileEvent_s * _Nonnull fileEvent);
void CEFileEventAddToReadTimer(CERunLoop_s * _Nonnull eventLoop, int feid, uint32_t timerId);
void CEFileEventAddToWriteTimer(CERunLoop_s * _Nonnull eventLoop, int feid, uint32_t timerId);
void CEFileEventUpdateReadTimer(CERunLoop_s * _Nonnull eventLoop, int feid, uint32_t timerId);
void CEFileEventUpdateWriteTimer(CERunLoop_s * _Nonnull eventLoop, int feid, uint32_t timerId);



CEFileEvent_s * _Nullable CERunLoopGetFileEventByIndex(CERunLoop_s * _Nonnull eventLoop, int index) {
    if (index >= eventLoop->setsize || index < 0) {
        return NULL;
    }
    Source
    uint32_t offset = (uint32_t)index;
    return &(eventLoop->fileEventsPages[offset >> 16][offset & 0xffff]);
}

CEFileEvent_s * _Nullable CERunLoopGetFileEvent(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd) {
    if (fd.fd >= eventLoop->setsize) {
        return NULL;
    }
    CEFileEvent_s *fe = CERunLoopGetFileEventByIndex(eventLoop, fd.fd);
    if (NULL == fe) {
        return NULL;
    }
    if (NULL == fe->handler) {
        return NULL;
    }
    if (fe->fdTag != fd.tag) {
        return NULL;
    }
    return fe;
}


static inline uint32_t CEFileEventTimeout(CERunLoop_s * _Nonnull eventLoop, uint32_t interval) {
    uint64_t nowMicroseconds = eventLoop->microsecondsTime;
    uint64_t nowSeconds8 = nowMicroseconds / 125000;

    uint32_t timeout = (uint32_t)(nowSeconds8 + (uint64_t)interval - eventLoop->fileTimerSeconds8);
    
    if (timeout >= 0x4000) {
        return 0x4000;
    } else {
        return timeout;
    }
}


//TODO: check
static inline uint32_t CEFileEventTimeId(CERunLoop_s * _Nonnull eventLoop, uint32_t interval) {
    return (eventLoop->timerFiredIndex + interval) & 0x1fff;
}

CEResult_t CEFileEventSourceSetReadTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, uint32_t initialTimeout, uint32_t timeout) {
    CEFileEvent_s * fe = CERunLoopGetFileEvent(eventLoop, fd);
    if (NULL == fe) {
        return CEResultErrorFileDescription;
    }
    if (0 == timeout || timeout >= CEFileEventTimeoutMillisecondMax) {
        return CEResultErrorParams;
    }
    if (0 == initialTimeout || initialTimeout >= CEFileEventTimeoutMillisecondMax) {
        return CEResultErrorParams;
    }
    
    timeout = timeout / 125;
    if (0 == timeout) {
        timeout = 1;
    }
    initialTimeout = initialTimeout / 125;
    
    
    uint32_t timeOffset = CEFileEventTimeout(eventLoop, initialTimeout);
    uint32_t tid = CEFileEventTimeId(eventLoop, timeOffset);
    CEFileEventUpdateReadTimer(eventLoop, fd.fd, tid);
    fe->readTimeoutInterval = timeout;
    return CEResultSuccess;
}

CEResult_t CEFileEventSourceSetWriteTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, uint32_t initialTimeout, uint32_t timeout) {
    CEFileEvent_s * fe = CERunLoopGetFileEvent(eventLoop, fd);
    if (NULL == fe) {
        return CEResultErrorFileDescription;
    }
    if (0 == timeout || timeout >= CEFileEventTimeoutMillisecondMax) {
        return CEResultErrorParams;
    }
    if (0 == initialTimeout || initialTimeout >= CEFileEventTimeoutMillisecondMax) {
        return CEResultErrorParams;
    }
    
    timeout = timeout / 125;
    if (0 == timeout) {
        timeout = 1;
    }
    initialTimeout = initialTimeout / 125;
    
    
    uint32_t timeOffset = CEFileEventTimeout(eventLoop, initialTimeout);
    uint32_t tid = CEFileEventTimeId(eventLoop, timeOffset);
    CEFileEventUpdateWriteTimer(eventLoop, fd.fd, tid);
    fe->writeTimeoutInterval = timeout;
    return CEResultSuccess;
}

CEResult_t CEFileEventSourceCancelReadTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd) {
    CEFileEvent_s * fe = CERunLoopGetFileEvent(eventLoop, fd);
    if (NULL == fe) {
        return CEResultErrorFileDescription;
    }
    if (0 != fe->readTimeoutInterval) {
        CEFileEventRemoveFromReadTimer(eventLoop, fe);
        fe->readTimeoutInterval = 0;
    }
    return CEResultSuccess;
}
CEResult_t CEFileEventSourceCancelWriteTimeout(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd) {
    CEFileEvent_s * fe = CERunLoopGetFileEvent(eventLoop, fd);
    if (NULL == fe) {
        return CEResultErrorFileDescription;
    }
    if (0 != fe->writeTimeoutInterval) {
        CEFileEventRemoveFromWriteTimer(eventLoop, fe);
        fe->writeTimeoutInterval = 0;
    }
    return CEResultSuccess;
}

CEResult_t CEFileEventSourceUpdateReadTime(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd) {
    CEFileEvent_s * fe = CERunLoopGetFileEvent(eventLoop, fd);
    if (NULL == fe) {
        return CEResultErrorFileDescription;
    }
    if (0 == fe->readTimeoutInterval) {
        return CEResultSuccess;
    }
    
    uint32_t timeOffset = CEFileEventTimeout(eventLoop, fe->readTimeoutInterval);
    uint32_t tid = CEFileEventTimeId(eventLoop, timeOffset);
    CEFileEventUpdateReadTimer(eventLoop, fd.fd, tid);
    return CEResultSuccess;
}

CEResult_t CEFileEventSourceUpdateWriteTime(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd) {
    CEFileEvent_s * fe = CERunLoopGetFileEvent(eventLoop, fd);
    if (NULL == fe) {
        return CEResultErrorFileDescription;
    }
    if (0 == fe->writeTimeoutInterval) {
        return CEResultSuccess;
    }
    
    uint32_t timeOffset = CEFileEventTimeout(eventLoop, fe->writeTimeoutInterval);
    uint32_t tid = CEFileEventTimeId(eventLoop, timeOffset);
    CEFileEventUpdateWriteTimer(eventLoop, fd.fd, tid);
    return CEResultSuccess;
}


CEFileEvent_s * _Nullable CEFileEventSourceInit(CERunLoop_s * _Nonnull eventLoop, CEFileDescription_s fd, CEFileEventHandler_f _Nonnull handler, void * _Nullable clientData) {
    assert(fd.fd != INT_MAX);
    assert(NULL != handler);
    assert(fd.fd < eventLoop->setsize);
    
    CEFileEvent_s * fe = CERunLoopGetFileEventByIndex(eventLoop, fd.fd);
    if (NULL != fe->handler) {
        return NULL;
    }
    fe->fdTag = fd.tag;
    fe->handler = handler;
    fe->clientData = clientData;
    fe->readTimer.prev = CEFileDescriptionInvalid;
    fe->readTimer.next = CEFileDescriptionInvalid;
    fe->writeTimer.prev = CEFileDescriptionInvalid;
    fe->writeTimer.next = CEFileDescriptionInvalid;

    return fe;
}
CEResult_t CEFileEventSourceDeinit(CERunLoop_s * _Nonnull eventLoop, CEFileEvent_s * _Nonnull fe) {

    if (fe->mask != CEFileEventMaskNone) {
        return CEResultErrorShouldRemoveSource;
    }
    
    //TODO: remove timer
    
    memset(fe, 0, sizeof(CEFileEvent_s));

    return CEResultSuccess;
}


static inline _Bool CEFileEventTimerListIsHead(int fd, uint32_t * _Nonnull indexRef) {
    if (fd <= -0x4000 && fd > -0x8000) {
        uint32_t index = fd * -1;
        index = index & 0x3fff;
        *indexRef = index;
        return 1;
    } else {
        return 0;
    }
}

static inline int CEFileEventTimerIdToFd(uint32_t id) {
    assert(id < 0x4000);
    int result = id;
    result = result * -1 - 0x4000;
    return result;
}

void CEFileEventRemoveFromReadTimer(CERunLoop_s * _Nonnull eventLoop, CEFileEvent_s * _Nonnull fileEvent) {
    CEFileEvent_s * fe = fileEvent;
    int prev = fe->readTimer.prev;
    int next = fe->readTimer.next;
    fe->readTimer.prev = CEFileDescriptionInvalid;
    fe->readTimer.next = CEFileDescriptionInvalid;
    if (prev < 0) {
        uint32_t index = 0;
        if (CEFileEventTimerListIsHead(prev, &index)) {
            CEFileEvent_s * nextFe = CERunLoopGetFileEventByIndex(eventLoop, next);
            if (nextFe) {
                nextFe->readTimer.prev = prev;
            }
            eventLoop->readTimerPages[index] = next;
        } else if (CEFileDescriptionInvalid == prev) {//不在readtimer 中
        } else {
            fprintf(stderr, "CEFileEventRemoveFromReadTimer prev error, prev=%d\n", prev);
            fflush(stderr);
            abort();
        }
    } else {
        CEFileEvent_s * prevFe = CERunLoopGetFileEventByIndex(eventLoop, prev);
        if (NULL == prevFe) {
            fprintf(stderr, "CEFileEventRemoveFromReadTimer prev get error, prev=%d\n", prev);
            fflush(stderr);
            abort();
        }
        prevFe->readTimer.next = next;
        
        CEFileEvent_s * nextFe = CERunLoopGetFileEventByIndex(eventLoop, next);
        if (nextFe) {
            nextFe->readTimer.prev = prev;
        }
    }
}

void CEFileEventRemoveFromWriteTimer(CERunLoop_s * _Nonnull eventLoop, CEFileEvent_s * _Nonnull fileEvent) {
    CEFileEvent_s * fe = fileEvent;
    int prev = fe->writeTimer.prev;
    int next = fe->writeTimer.next;
    fe->writeTimer.prev = CEFileDescriptionInvalid;
    fe->writeTimer.next = CEFileDescriptionInvalid;
    if (prev < 0) {
        uint32_t index = 0;
        if (CEFileEventTimerListIsHead(prev, &index)) {
            CEFileEvent_s * nextFe = CERunLoopGetFileEventByIndex(eventLoop, next);
            if (nextFe) {
                nextFe->writeTimer.prev = prev;
            }
            eventLoop->writeTimerPages[index] = next;
        } else if (CEFileDescriptionInvalid == prev) {//不在readtimer 中
        } else {
            fprintf(stderr, "CEFileEventRemoveFromWriteTimer prev error, prev=%d\n", prev);
            fflush(stderr);
            abort();
        }
    } else {
        CEFileEvent_s * prevFe = CERunLoopGetFileEventByIndex(eventLoop, prev);
        if (NULL == prevFe) {
            fprintf(stderr, "CEFileEventRemoveFromWriteTimer prev get error, prev=%d\n", prev);
            fflush(stderr);
            abort();
        }
        prevFe->writeTimer.next = next;
        
        CEFileEvent_s * nextFe = CERunLoopGetFileEventByIndex(eventLoop, next);
        if (nextFe) {
            nextFe->writeTimer.prev = prev;
        }
    }
}

void CEFileEventAddToReadTimer(CERunLoop_s * _Nonnull eventLoop, int feid, uint32_t timerId) {
    CEFileEvent_s * fe = CERunLoopGetFileEventByIndex(eventLoop, feid);

    assert(fe);
    assert(fe->readTimer.prev == CEFileDescriptionInvalid);
    assert(fe->readTimer.next == CEFileDescriptionInvalid);
    assert(timerId < 0x4000);
    int old = eventLoop->readTimerPages[timerId];
    eventLoop->readTimerPages[timerId] = feid;
    
    fe->readTimer.prev = CEFileEventTimerIdToFd(timerId);
    fe->readTimer.next = old;

    CEFileEvent_s * oldFe = CERunLoopGetFileEventByIndex(eventLoop, old);
    if (oldFe) {
        oldFe->readTimer.prev = feid;
    }
}

void CEFileEventAddToWriteTimer(CERunLoop_s * _Nonnull eventLoop, int feid, uint32_t timerId) {
    CEFileEvent_s * fe = CERunLoopGetFileEventByIndex(eventLoop, feid);
    
    assert(fe);
    assert(fe->writeTimer.prev == CEFileDescriptionInvalid);
    assert(fe->writeTimer.next == CEFileDescriptionInvalid);
    assert(timerId < 0x4000);
    int old = eventLoop->writeTimerPages[timerId];
    eventLoop->writeTimerPages[timerId] = feid;
    fe->writeTimer.prev = CEFileEventTimerIdToFd(timerId);
    fe->writeTimer.next = old;

    CEFileEvent_s * oldFe = CERunLoopGetFileEventByIndex(eventLoop, old);
    if (oldFe) {
        oldFe->readTimer.prev = feid;
    }
}

void CEFileEventUpdateReadTimer(CERunLoop_s * _Nonnull eventLoop, int feid, uint32_t timerId) {
    CEFileEvent_s * fe = CERunLoopGetFileEventByIndex(eventLoop, feid);
    assert(fe);

    CEFileEventRemoveFromReadTimer(eventLoop, fe);
    CEFileEventAddToReadTimer(eventLoop, feid, timerId);
}

void CEFileEventUpdateWriteTimer(CERunLoop_s * _Nonnull eventLoop, int feid, uint32_t timerId) {
    CEFileEvent_s * fe = CERunLoopGetFileEventByIndex(eventLoop, feid);
    assert(fe);

    CEFileEventRemoveFromWriteTimer(eventLoop, fe);
    CEFileEventAddToWriteTimer(eventLoop, feid, timerId);
}









