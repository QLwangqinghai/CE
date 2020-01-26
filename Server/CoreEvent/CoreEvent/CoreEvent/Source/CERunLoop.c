//
//  CELoop.c
//  CoreEvent
//
//  Created by 王青海 on 2018/10/24.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CERunLoop.h"
#include <poll.h>
#include "zmalloc.h"
#include "config.h"
#include "CEBaseApi.h"
#include "CEFileEventHandler.h"


void CELoopEventCallback(void * _Nullable context, void * _Nonnull api, int index, int fd, CEFileEventMask_es mask) {
    assert(context);
    CERunLoop_s * eventLoop = (CERunLoop_s *)context;

    eventLoop->fired[index].fd = fd;
    eventLoop->fired[index].mask = mask;
}


CEFileEventMask_es CELoopGetMask(void * _Nullable context, void * _Nonnull api, int fd) {
    if (NULL == context) {
        return CEFileEventMaskNone;
    }
    
    CERunLoop_s * loop = (CERunLoop_s *)context;
    
    return loop->events[fd].mask;
}


static CERunLoop_s * _Nonnull __CERunLoopShared = NULL;
void CEEventLoopSharedCreate(void) {
    struct rlimit limit;
    if(getrlimit(RLIMIT_NOFILE,&limit) != 0) {
        printf("getrlimit RLIMIT_NOFILE error %s; \n", strerror(errno));
        abort();
    }
//    limit.rlim_cur = limit.rlim_cur > 1024 * 1024 ? limit.rlim_cur : 1024 * 1024;
//    limit.rlim_cur = OPEN_MAX < limit.rlim_max ? OPEN_MAX : limit.rlim_max;

    //https://www.jb51.net/article/132035.htm
    //https://blog.csdn.net/pao98pao/article/details/53322868
    //https://www.baidu.com/s?wd=sysctl+%E8%8E%B7%E5%8F%96%E5%86%85%E6%A0%B8%E5%8F%82%E6%95%B0&tn=84053098_3_dg&ie=utf-8
    //https://blog.csdn.net/whereismatrix/article/details/50582919
    //https://blog.csdn.net/z1134145881/article/details/52573441
    //https://blog.csdn.net/ying0216/article/details/4024143
    //
    
    //参数调优 https://www.cnblogs.com/zengkefu/p/5602473.html

    int mib[2] = {CTL_KERN, KERN_MAXFILESPERPROC};
    int maxFilesPerProc = 0;
    size_t size = sizeof(maxFilesPerProc);

    if (sysctl(mib, 2, &maxFilesPerProc, &size, NULL, 0) != 0) {
        printf("sysctl KERN_MAXFILESPERPROC error %s; \n", strerror(errno));
        abort();
    }
    if (limit.rlim_cur < maxFilesPerProc) {
        limit.rlim_cur = maxFilesPerProc;
        printf("setrlimit rlim_cur %lld; \n", limit.rlim_cur);
        if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
            printf("setrlimit RLIMIT_NOFILE error %s; \n", strerror(errno));
            abort();
        }
    }

    int value = (int)(limit.rlim_cur);
    
    __CERunLoopShared = CERunLoopCreate(value);
    __CERunLoopShared->fdLimit = value;
}
CERunLoop_s * _Nonnull CERunLoopShared(void) {
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, CEEventLoopSharedCreate);
    return __CERunLoopShared;
}

CERunLoop_s * _Nullable CERunLoopCreate(int setsize) {
    if (setsize < 1) {
        return NULL;
    }

    CERunLoop_s *eventLoop;
    int i;
    
    if ((eventLoop = zmalloc(sizeof(*eventLoop))) == NULL) goto err;
    eventLoop->events = zmalloc(sizeof(CEFileEvent_s)*setsize);
    eventLoop->fired = zmalloc(sizeof(CEFiredEvent_s)*setsize);
    if (eventLoop->events == NULL || eventLoop->fired == NULL) goto err;
    eventLoop->setsize = setsize;
    eventLoop->lastTime = time(NULL);
    eventLoop->timeEventHead = NULL;
    eventLoop->timeEventNextId = 0;
    eventLoop->stop = 0;
    eventLoop->maxfd = -1;
    eventLoop->beforesleep = NULL;
    eventLoop->aftersleep = NULL;
    
    
    void * api = CEApiCreate(setsize, eventLoop, CELoopGetMask);
    if (NULL == api) goto err;
    /* Events with mask == CEFileEventMaskNone are not set. So let's initialize the
     * vector with it. */
    for (i = 0; i < setsize; i++)
        eventLoop->events[i].mask = CEFileEventMaskNone;
    return eventLoop;
    
err:
    if (eventLoop) {
        zfree(eventLoop->events);
        zfree(eventLoop->fired);
        zfree(eventLoop);
    }
    return NULL;
}

/* Return the current set size. */
int CEGetSetSize(CERunLoop_s *eventLoop) {
    return eventLoop->setsize;
}

/* Resize the maximum set size of the event loop.
 * If the requested set size is smaller than the current set size, but
 * there is already a file descriptor in use that is >= the requested
 * set size minus one, AE_ERR is returned and the operation is not
 * performed at all.
 *
 * Otherwise AE_OK is returned and the operation is successful. */
int CEResizeSetSize(CERunLoop_s *eventLoop, int setsize) {
    if (setsize < 1) {
        return -1;
    }
    
    int i;
    
    if (setsize == eventLoop->setsize) return AE_OK;
    if (eventLoop->maxfd >= setsize) return AE_ERR;
    if (CEApiResize(eventLoop, setsize) == -1) return AE_ERR;
    
    eventLoop->events = zrealloc(eventLoop->events, sizeof(CEFileEvent_s)*setsize);
    eventLoop->fired = zrealloc(eventLoop->fired, sizeof(CEFiredEvent_s)*setsize);
    eventLoop->setsize = setsize;
    
    /* Make sure that if we created new slots, they are initialized with
     * an CEFileEventMaskNone mask. */
    for (i = eventLoop->maxfd+1; i < setsize; i++)
        eventLoop->events[i].mask = CEFileEventMaskNone;
    return AE_OK;
}

void CERunLoopDeinit(CERunLoop_s *eventLoop) {
    CEApiFree(eventLoop);
    zfree(eventLoop->events);
    zfree(eventLoop->fired);
    zfree(eventLoop);
}

void CERunLoopStop(CERunLoop_s *eventLoop) {
    eventLoop->stop = 1;
}

int CERunLoopAddFileEvent(CERunLoop_s * _Nonnull eventLoop, int fd, CEFileEventMask_es mask, uint32_t handlerId, void * _Nullable clientData) {
    
    CEFileEventHandler_f handler = CEFileEventHandlerGet(handlerId);
    if (NULL == handler) {
        return AE_ERR;
    }
    
    if (fd >= eventLoop->setsize) {
        errno = ERANGE;
        return AE_ERR;
    }
    CEFileEvent_s *fe = &eventLoop->events[fd];
    
    if (CEApiAddEvent(eventLoop, fd, mask) == -1)
        return AE_ERR;
    fe->mask |= mask;
    if (mask & CEFileEventMaskReadable) fe->readHandlerId = handlerId;
    if (mask & CEFileEventMaskWritable) fe->writeHandlerId = handlerId;
    fe->clientData = clientData;
    if (fd > eventLoop->maxfd)
        eventLoop->maxfd = fd;
    return AE_OK;
}

void CERunLoopRemoveFileEvent(CERunLoop_s *eventLoop, int fd, CEFileEventMask_es mask) {
    if (fd >= eventLoop->setsize) return;
    CEFileEvent_s *fe = &eventLoop->events[fd];
    if (fe->mask == CEFileEventMaskNone) return;
    
    /* We want to always remove CEFileEventMaskBarrier if set when CEFileEventMaskWritable
     * is removed. */
    if (mask & CEFileEventMaskWritable) mask |= CEFileEventMaskBarrier;
    
    CEApiRemoveEvent(eventLoop, fd, mask);
    fe->mask = fe->mask & (~mask);
    if (fd == eventLoop->maxfd && fe->mask == CEFileEventMaskNone) {
        /* Update the max fd */
        int j;
        
        for (j = eventLoop->maxfd-1; j >= 0; j--)
            if (eventLoop->events[j].mask != CEFileEventMaskNone) break;
        eventLoop->maxfd = j;
    }
}

CEFileEventMask_es CEGetFileEvents(CERunLoop_s *eventLoop, int fd) {
    if (fd >= eventLoop->setsize) return 0;
    CEFileEvent_s *fe = &eventLoop->events[fd];
    
    return fe->mask;
}

static void CEGetTime(long *seconds, long *milliseconds) {
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    *seconds = tv.tv_sec;
    *milliseconds = tv.tv_usec/1000;
}

static void CEAddMillisecondsToNow(long long milliseconds, long *sec, long *ms) {
    long cur_sec, cur_ms, when_sec, when_ms;
    
    CEGetTime(&cur_sec, &cur_ms);
    when_sec = cur_sec + milliseconds/1000;
    when_ms = cur_ms + milliseconds%1000;
    if (when_ms >= 1000) {
        when_sec ++;
        when_ms -= 1000;
    }
    *sec = when_sec;
    *ms = when_ms;
}

long long CERunLoopAddTimeEvent(CERunLoop_s * _Nonnull eventLoop, long long milliseconds, CETimeEventHandler_f _Nonnull proc, void * _Nullable clientData, CEFinalizerHandler_f _Nullable finalizerProc) {
    long long id = eventLoop->timeEventNextId++;
    CETimeEvent_s *te;
    
    te = zmalloc(sizeof(*te));
    if (te == NULL) return AE_ERR;
    te->id = id;
    CEAddMillisecondsToNow(milliseconds,&te->when_sec,&te->when_ms);
    te->timeProc = proc;
    te->finalizerProc = finalizerProc;
    te->clientData = clientData;
    te->next = eventLoop->timeEventHead;
    eventLoop->timeEventHead = te;
    return id;
}

int CERunLoopRemoveTimeEvent(CERunLoop_s *eventLoop, long long id) {
    CETimeEvent_s *te = eventLoop->timeEventHead;
    while(te) {
        if (te->id == id) {
            te->id = AE_DELETED_EVENT_ID;
            return AE_OK;
        }
        te = te->next;
    }
    return AE_ERR; /* NO event with the specified ID found */
}

/* Search the first timer to fire.
 * This operation is useful to know how many time the select can be
 * put in sleep without to delay any event.
 * If there are no timers NULL is returned.
 *
 * Note that's O(N) since time events are unsorted.
 * Possible optimizations (not needed by Redis so far, but...):
 * 1) Insert the event in order, so that the nearest is just the head.
 *    Much better but still insertion or deletion of timers is O(N).
 * 2) Use a skiplist to have this operation as O(1) and insertion as O(log(N)).
 */
static CETimeEvent_s *aeSearchNearestTimer(CERunLoop_s *eventLoop) {
    CETimeEvent_s *te = eventLoop->timeEventHead;
    CETimeEvent_s *nearest = NULL;
    
    while(te) {
        if (!nearest || te->when_sec < nearest->when_sec ||
            (te->when_sec == nearest->when_sec &&
             te->when_ms < nearest->when_ms))
            nearest = te;
        te = te->next;
    }
    return nearest;
}

/* Process time events */
static int processTimeEvents(CERunLoop_s *eventLoop) {
    int processed = 0;
    CETimeEvent_s *te, *prev;
    long long maxId;
    time_t now = time(NULL);
    
    /* If the system clock is moved to the future, and then set back to the
     * right value, time events may be delayed in a random way. Often this
     * means that scheduled operations will not be performed soon enough.
     *
     * Here we try to detect system clock skews, and force all the time
     * events to be processed ASAP when this happens: the idea is that
     * processing events earlier is less dangerous than delaying them
     * indefinitely, and practice suggests it is. */
    if (now < eventLoop->lastTime) {
        te = eventLoop->timeEventHead;
        while(te) {
            te->when_sec = 0;
            te = te->next;
        }
    }
    eventLoop->lastTime = now;
    
    prev = NULL;
    te = eventLoop->timeEventHead;
    maxId = eventLoop->timeEventNextId-1;
    while(te) {
        long now_sec, now_ms;
        long long id;
        
        /* Remove events scheduled for deletion. */
        if (te->id == AE_DELETED_EVENT_ID) {
            CETimeEvent_s *next = te->next;
            if (prev == NULL)
                eventLoop->timeEventHead = te->next;
            else
                prev->next = te->next;
            if (te->finalizerProc)
                te->finalizerProc(eventLoop, te->clientData);
            zfree(te);
            te = next;
            continue;
        }
        
        /* Make sure we don't process time events created by time events in
         * this iteration. Note that this check is currently useless: we always
         * add new timers on the head, however if we change the implementation
         * detail, this check may be useful again: we keep it here for future
         * defense. */
        if (te->id > maxId) {
            te = te->next;
            continue;
        }
        CEGetTime(&now_sec, &now_ms);
        if (now_sec > te->when_sec ||
            (now_sec == te->when_sec && now_ms >= te->when_ms))
        {
            int retval;
            
            id = te->id;
            retval = te->timeProc(eventLoop, id, te->clientData);
            processed++;
            if (retval != AE_NOMORE) {
                CEAddMillisecondsToNow(retval,&te->when_sec,&te->when_ms);
            } else {
                te->id = AE_DELETED_EVENT_ID;
            }
        }
        prev = te;
        te = te->next;
    }
    return processed;
}

/* Process every pending time event, then every pending file event
 * (that may be registered by time event callbacks just processed).
 * Without special flags the function sleeps until some file event
 * fires, or when the next time event occurs (if any).
 *
 * If flags is 0, the function does nothing and returns.
 * if flags has AE_ALL_EVENTS set, all the kind of events are processed.
 * if flags has AE_FILE_EVENTS set, file events are processed.
 * if flags has AE_TIME_EVENTS set, time events are processed.
 * if flags has AE_DONT_WAIT set the function returns ASAP until all
 * if flags has AE_CALL_AFTER_SLEEP set, the aftersleep callback is called.
 * the events that's possible to process without to wait are processed.
 *
 * The function returns the number of events processed. */
int CERunLoopProcessEvents(CERunLoop_s *eventLoop, int flags) {
    int processed = 0, numevents;
    
    /* Nothing to do? return ASAP */
    if (!(flags & AE_TIME_EVENTS) && !(flags & AE_FILE_EVENTS)) return 0;
    
    /* Note that we want call select() even if there are no
     * file events to process as long as we want to process time
     * events, in order to sleep until the next time event is ready
     * to fire. */
    if (eventLoop->maxfd != -1 ||
        ((flags & AE_TIME_EVENTS) && !(flags & AE_DONT_WAIT))) {
        int j;
        CETimeEvent_s *shortest = NULL;
        struct timeval tv, *tvp;
        
        if (flags & AE_TIME_EVENTS && !(flags & AE_DONT_WAIT))
            shortest = aeSearchNearestTimer(eventLoop);
        if (shortest) {
            long now_sec, now_ms;
            
            CEGetTime(&now_sec, &now_ms);
            tvp = &tv;
            
            /* How many milliseconds we need to wait for the next
             * time event to fire? */
            long long ms =
            (shortest->when_sec - now_sec)*1000 +
            shortest->when_ms - now_ms;
            
            if (ms > 0) {
                tvp->tv_sec = ms/1000;
                tvp->tv_usec = (ms % 1000)*1000;
            } else {
                tvp->tv_sec = 0;
                tvp->tv_usec = 0;
            }
        } else {
            /* If we have to check for events but need to return
             * ASAP because of AE_DONT_WAIT we need to set the timeout
             * to zero */
            if (flags & AE_DONT_WAIT) {
                tv.tv_sec = tv.tv_usec = 0;
                tvp = &tv;
            } else {
                /* Otherwise we can block */
                tvp = NULL; /* wait forever */
            }
        }
        
        /* Call the multiplexing API, will return only on timeout or when
         * some event fires. */
        numevents = CEApiPoll(eventLoop, tvp, eventLoop->maxfd, eventLoop->setsize, eventLoop, CELoopEventCallback);
        
        /* After sleep callback. */
        if (eventLoop->aftersleep != NULL && flags & AE_CALL_AFTER_SLEEP)
            eventLoop->aftersleep(eventLoop);
        
        for (j = 0; j < numevents; j++) {
            CEFileEvent_s *fe = &eventLoop->events[eventLoop->fired[j].fd];
            CEFileEventMask_es mask = eventLoop->fired[j].mask;
            int fd = eventLoop->fired[j].fd;
            int fired = 0; /* Number of events fired for current fd. */
            
            /* Normally we execute the readable event first, and the writable
             * event laster. This is useful as sometimes we may be able
             * to serve the reply of a query immediately after processing the
             * query.
             *
             * However if CEFileEventMaskBarrier is set in the mask, our application is
             * asking us to do the reverse: never fire the writable event
             * after the readable. In such a case, we invert the calls.
             * This is useful when, for instance, we want to do things
             * in the beforeSleep() hook, like fsynching a file to disk,
             * before replying to a client. */
            int invert = fe->mask & CEFileEventMaskBarrier;
            
            CEFileEventHandler_f readHandler = CEFileEventHandlerGet(fe->readHandlerId);
            CEFileEventHandler_f writeHandler = CEFileEventHandlerGet(fe->writeHandlerId);
            
            if (!invert && fe->mask & mask & CEFileEventMaskReadable) {
                readHandler(eventLoop, fd, fe->clientData, mask);
                fired++;
            }
            
            /* Fire the writable event. */
            if (fe->mask & mask & CEFileEventMaskWritable) {
                if (!fired || readHandler != writeHandler) {
                    writeHandler(eventLoop, fd, fe->clientData, mask);
                    fired++;
                }
            }
            
            /* If we have to invert the call, fire the readable event now
             * after the writable one. */
            if (invert && fe->mask & mask & CEFileEventMaskReadable) {
                if (!fired || readHandler != writeHandler) {
                    readHandler(eventLoop, fd, fe->clientData, mask);
                    fired++;
                }
            }

            
            processed++;
        }
    }
    /* Check time events */
    if (flags & AE_TIME_EVENTS)
        processed += processTimeEvents(eventLoop);
    
    return processed; /* return the number of processed file/time events */
}

/* Wait for milliseconds until the given file descriptor becomes
 * writable/readable/exception */
int CEWait(int fd, int mask, int milliseconds) {
    struct pollfd pfd;
    int retmask = 0, retval;
    
    memset(&pfd, 0, sizeof(pfd));
    pfd.fd = fd;
    if (mask & CEFileEventMaskReadable) pfd.events |= POLLIN;
    if (mask & CEFileEventMaskWritable) pfd.events |= POLLOUT;
    
    if ((retval = poll(&pfd, 1, milliseconds))== 1) {
        if (pfd.revents & POLLIN) retmask |= CEFileEventMaskReadable;
        if (pfd.revents & POLLOUT) retmask |= CEFileEventMaskWritable;
        if (pfd.revents & POLLERR) retmask |= CEFileEventMaskWritable;
        if (pfd.revents & POLLHUP) retmask |= CEFileEventMaskWritable;
        return retmask;
    } else {
        return retval;
    }
}

void CERunLoopRun(CERunLoop_s *eventLoop) {
    eventLoop->stop = 0;
    while (!eventLoop->stop) {
        if (eventLoop->beforesleep != NULL) {
            eventLoop->beforesleep(eventLoop);
        }
        CERunLoopProcessEvents(eventLoop, AE_ALL_EVENTS|AE_CALL_AFTER_SLEEP);
    }
}

char *CERunLoopGetApiName(void) {
    return CEApiName();
}

void CESetBeforeSleepProc(CERunLoop_s * _Nonnull eventLoop, CEStateChangeHandler_f _Nullable beforesleep) {
    eventLoop->beforesleep = beforesleep;
}

void CESetAfterSleepProc(CERunLoop_s * _Nonnull eventLoop, CEStateChangeHandler_f _Nullable aftersleep) {
    eventLoop->aftersleep = aftersleep;
}
