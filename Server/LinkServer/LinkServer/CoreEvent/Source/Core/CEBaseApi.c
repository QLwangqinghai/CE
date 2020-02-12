//
//  CEBaseApi.c
//  CoreEvent
//
//  Created by 王青海 on 2018/10/24.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CEBaseApi.h"
#include "CEMemory.h"
#include "CEConfig.h"
#include <poll.h>
#include <sys/socket.h>
#include <sys/fcntl.h>

struct _CEApiState;
typedef struct _CEApiState CEApiState_s;


#if CORE_EVENT_USE_EPOLL

#include <sys/epoll.h>


// typedef union epoll_data {
//    void *ptr;
//     int fd;
//     __uint32_t u32;
//     __uint64_t u64;
// } epoll_data_t;//保存触发事件的某个文件描述符相关的数据
//
// struct epoll_event {
//     __uint32_t events;      /* epoll event */
//     epoll_data_t data;      /* User data variable */
// };
//
//struct kevent {
//    uintptr_t       ident;  /* identifier for this event */
//    int16_t         filter; /* filter for event */
//    uint16_t        flags;  /* general flags */
//    uint32_t        fflags; /* filter-specific flags */
//    intptr_t        data;   /* filter-specific data */
//    void            *udata; /* opaque user data identifier */
//};


typedef struct epoll_event CEApiEvent_s;

#endif

#if CORE_EVENT_USE_KQUEUE

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

typedef struct kevent CEApiEvent_s;

#endif

size_t CEApiGetEventItemSize(void) {
    return sizeof(CEApiEvent_s);
}

struct _CEApiState {
    int fd;
    uint32_t setSize;
    CEApiEvent_s * _Nonnull buffer;
};

#if CORE_EVENT_USE_EPOLL

//typedef union epoll_data {
//    void *ptr;
//    int fd;
//    __uint32_t u32;
//    __uint64_t u64;
//} epoll_data_t;
//struct epoll_event {
//    __uint32_t events; /* Epoll events */
//    epoll_data_t data; /* User data variable */
//};
//int epoll_create(int size);
//int epoll_ctl(int fd, int op, int fd, struct epoll_event *event);
//int epoll_wait(int fd, struct epoll_event *events,int maxevents, int timeout);
//
//#define EPOLL_CTL_ADD 1
//#define EPOLL_CTL_MOD 2
//#define EPOLLIN 1
//#define EPOLLOUT 2
//#define EPOLL_CTL_DEL 2
//#define EPOLLERR 2
//#define EPOLLHUP 2


CCPtr _Nonnull CEApiCreate(uint32_t setSize) {
    uint32_t size = setSize > 1024 ? setSize : 1024;
    int fd = epoll_create(1024); /* 1024 is just a hint for the kernel */
    assert(fd != -1);
    CEApiState_s * state = CEAllocateClear(sizeof(CEApiState_s));
    assert(state);
    state->fd = fd;
    state->setSize = size;
    state->buffer = CEAllocateClear(sizeof(CEApiEvent_s) * size);
    return state;
}

void CEApiAddEvent(void * _Nonnull api, int fd, CEFileEventMask_es oldMask, CEFileEventMask_es mask) {
    assert(api);
    CEApiState_s *state = api;

    struct epoll_event ee = {0}; /* avoid valgrind warning */
    /* If the fd was already monitored for some event, we need a MOD
     * operation. Otherwise we need an ADD operation. */
    
    int op = oldMask == CEFileEventMaskNone ?
    EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    
    ee.events = 0;
    mask |= oldMask; /* Merge old events */
    if (mask & CEFileEventMaskReadable) ee.events |= EPOLLIN;
    if (mask & CEFileEventMaskWritable) ee.events |= EPOLLOUT;
    ee.data.fd = fd;
    if (epoll_ctl(state->fd,op,fd,&ee) == -1) {
        printf("\n");
        abort();
    };
}

void CEApiRemoveEvent(void * _Nonnull api, int fd, CEFileEventMask_es delmask) {
    assert(api);
    CEApiState_s *state = api;

    struct epoll_event ee = {0}; /* avoid valgrind warning */
    CEFileEventMask_es oldMask = state->func(state->context, state, fd);
    CEFileEventMask_es mask = oldMask & (~delmask);

    ee.events = 0;
    if (mask & CEFileEventMaskReadable) ee.events |= EPOLLIN;
    if (mask & CEFileEventMaskWritable) ee.events |= EPOLLOUT;
    ee.data.fd = fd;
    if (mask != CEFileEventMaskNone) {
        epoll_ctl(state->fd,EPOLL_CTL_MOD,fd,&ee);
    } else {
        /* Note, Kernel < 2.6.9 requires a non null event pointer even for
         * EPOLL_CTL_DEL. */
        epoll_ctl(state->fd,EPOLL_CTL_DEL,fd,&ee);
    }
}

int CEApiPoll(void * _Nonnull api, CEMicrosecondTime timeout, CCPtr _Nonnull buffer, size_t bufferSize, void * _Nullable context, const CEApiPollCallback_s * _Nonnull callback) {
    assert(api);
    assert(callback);
    assert(buffer);
    assert(itemSize <= sizeof(CEApiEvent_s));
    int setsize = bufferSize / sizeof(CEApiEvent_s);
    assert(setsize > 0);
    CEApiEvent_s * events = buffer;

    CEApiState_s *state = api;
    int retval, numevents = 0;
    
    uint64_t t = timeout / 1000;
    assert(t <= INT_MAX);
    retval = epoll_wait(state->fd, events, setsize, t);
    if (retval > 0) {
        numevents = retval;
        uint8_t * ptr = buffer;
        for (int j = 0; j < numevents; j++) {
            CEFileEventMask_es mask = CEFileEventMaskNone;
            struct epoll_event *e = state->events+j;
            if (e->ident == state->pread) {
                CEApiPollDoPipe(state, context, callback);
            } else {
                if (e->events & EPOLLIN) mask |= CEFileEventMaskReadable;
                if (e->events & EPOLLOUT) mask |= CEFileEventMaskWritable;
                if (e->events & EPOLLERR) mask |= CEFileEventMaskReadWritable;
                if (e->events & EPOLLHUP) mask |= CEFileEventMaskReadWritable;
                if (mask != CEFileEventMaskNone) {
                    if (callback->filterMapper(context, api, ptr, (int)(e->ident), mask)) {
                        ptr += itemSize;
                    };
                }
            }
        }
    }
    return numevents;
}

char * _Nonnull CEApiName(void) {
    return "epoll";
}

#endif




#if CORE_EVENT_USE_KQUEUE

/*
 int pio[2] = {};
 int result = pipe(pio);
 
 if (0 != result) {
     fprintf(stderr, "CEApiCreate error %s; \n", strerror(errno));
     fflush(stderr);
     abort();
 }
 
 int flags = fcntl(pio[0], F_GETFL, 0);
 int fresult = fcntl(pio[0], F_SETFL, flags | O_NONBLOCK);
 assert(fresult == 0);
 
 int flags1 = fcntl(pio[1], F_GETFL, 0);
 int fresult1 = fcntl(pio[1], F_SETFL, flags1 | O_NONBLOCK);
 assert(fresult1 == 0);

 state->pread = pio[0];
 state->pwrite = pio[1];
 */

CCPtr _Nonnull CEApiCreate(uint32_t setSize) {
    uint32_t size = setSize > 1024 ? setSize : 1024;
    int fd = kqueue();
    assert(fd != -1);
    CEApiState_s *state = CEAllocateClear(sizeof(CEApiState_s));
    assert(state);
    state->fd = fd;
    state->setSize = size;
    state->buffer = CEAllocateClear(sizeof(CEApiEvent_s) * size);
    return state;
}


//ev timer https://blog.csdn.net/Namcodream521/article/details/83032615


void CEApiAddEvent(void * _Nonnull api, int fd, CEFileEventMask_es oldMask, CEFileEventMask_es mask) {
    assert(api);
    CEApiState_s *state = api;
    
    struct kevent ke;
    
    if (mask & CEFileEventMaskReadable) {
        EV_SET(&ke, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        if (kevent(state->fd, &ke, 1, NULL, 0, NULL) == -1) {
            abort();
        }
    }
    if (mask & CEFileEventMaskWritable) {
        EV_SET(&ke, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
        if (kevent(state->fd, &ke, 1, NULL, 0, NULL) == -1) {
            abort();
        }
    }
}

void CEApiRemoveEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask) {
    assert(api);
    CEApiState_s *state = api;
    
    struct kevent ke;
    
    if (mask & CEFileEventMaskReadable) {
        EV_SET(&ke, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        kevent(state->fd, &ke, 1, NULL, 0, NULL);
    }
    if (mask & CEFileEventMaskWritable) {
        EV_SET(&ke, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
        kevent(state->fd, &ke, 1, NULL, 0, NULL);
    }
}

int CEApiPoll(void * _Nonnull api, CEMicrosecondTime timeout, CCPtr _Nonnull buffer, size_t bufferSize, uint32_t itemSize, void * _Nullable context, const CEApiPollCallback_s * _Nonnull callback) {
    
    struct timespec tv = {};
    if (timeout <= 0) {
        tv.tv_nsec = 1;
    } else {
        tv.tv_sec = timeout / 1000000;
        tv.tv_nsec = timeout % 1000000 * 1000;
    }
    
    assert(api);
    assert(callback);
    assert(buffer);
    assert(itemSize <= sizeof(CEApiEvent_s));
    int setsize = bufferSize / sizeof(CEApiEvent_s);
    assert(setsize > 0);
    CEApiEvent_s * events = buffer;
    
    CEApiState_s *state = api;

    int numevents = 0;
    int retval = kevent(state->fd, NULL, 0, events, setsize, &tv);    
    if (retval > 0) {
        numevents = retval;
        uint8_t * ptr = buffer;

        for(int j = 0; j < numevents; j++) {
            CEFileEventMask_es mask = CEFileEventMaskNone;
            struct kevent *e = events + j;
            if (e->ident == state->pread) {
                CEApiPollHandlePipeInput(state, context, callback);
            } else {
                if (e->filter == EVFILT_READ) mask |= CEFileEventMaskReadable;
                if (e->filter == EVFILT_WRITE) mask |= CEFileEventMaskWritable;
                if (mask != CEFileEventMaskNone) {
                    if (callback->filterMapper(context, api, ptr, (int)(e->ident), mask)) {
                        ptr += itemSize;
                    };
                }
            }
        }
    }
    return numevents;
}

char * _Nonnull CEApiName(void) {
    return "kqueue";
}

#endif

void CEApiFree(void * _Nonnull api) {
    assert(api);
    CEApiState_s *state = api;
    close(state->fd);
    CEDeallocate(state->buffer);
    CEDeallocate(state);
}
