//
//  CEBaseApi.c
//  CoreEvent
//
//  Created by 王青海 on 2018/10/24.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CEBaseApi.h"
#include "CEMemory.h"
#include <poll.h>
#include <sys/socket.h>
#include <sys/fcntl.h>

#include "CEBaseType.h"

const CEFileEventMask_es CEFileEventMaskNone = 0;
const CEFileEventMask_es CEFileEventMaskReadable = 1;
const CEFileEventMask_es CEFileEventMaskWritable = 2;

const CEFileEventMask_es CEFileEventMaskReadWritable = 3;



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
#if CORE_EVENT_USE_EPOLL
    int eventfd;
#endif
    int setSize;
    CEApiEvent_s * _Nonnull events;
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


void * _Nonnull CEApiCreate(int setSize) {
    int size = setSize > 256 ? setSize : 256;
    int fd = epoll_create(1024); /* 1024 is just a hint for the kernel */
    assert(fd != -1);
    
    int eventfd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    assert(eventfd != -1);

    CEApiState_s * state = CEAllocateClear(sizeof(CEApiState_s));
    assert(state);
    state->fd = fd;
    state->eventfd = eventfd;
    state->setSize = size;
    state->events = CEAllocateClear(sizeof(CEApiEvent_s) * size);
    return state;
}


void CEApiAddEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask) {
    assert(api);
    CEApiState_s *state = api;

    CEApiEvent_s ee = {0}; /* avoid valgrind warning */
    /* If the fd was already monitored for some event, we need a MOD
     * operation. Otherwise we need an ADD operation. */
    
    int op = EPOLL_CTL_ADD;
    ee.events = EPOLLERR | EPOLLHUP;
    if (mask & CEFileEventMaskReadable) ee.events |= EPOLLIN;
    if (mask & CEFileEventMaskWritable) ee.events |= EPOLLOUT;
    ee.data.fd = fd;
    if (epoll_ctl(state->fd,op,fd,&ee) == -1) {
        printf("\n");
        abort();
    };
}
void CEApiUpdateEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask) {
    assert(api);
    CEApiState_s *state = api;

    CEApiEvent_s ee = {0}; /* avoid valgrind warning */
    /* If the fd was already monitored for some event, we need a MOD
     * operation. Otherwise we need an ADD operation. */
    
    int op = EPOLL_CTL_MOD;
    ee.events = EPOLLERR | EPOLLHUP;
    if (mask & CEFileEventMaskReadable) ee.events |= EPOLLIN;
    if (mask & CEFileEventMaskWritable) ee.events |= EPOLLOUT;
    ee.data.fd = fd;
    if (epoll_ctl(state->fd,op,fd,&ee) == -1) {
        printf("\n");
        abort();
    };
}
void CEApiRemoveEvent(void * _Nonnull api, int fd) {
    assert(api);
    CEApiState_s *state = api;

    CEApiEvent_s ee = {0}; /* avoid valgrind warning */
    /* If the fd was already monitored for some event, we need a MOD
     * operation. Otherwise we need an ADD operation. */
    int op = EPOLL_CTL_DEL;
    ee.data.fd = fd;
    if (epoll_ctl(state->fd,op,fd,&ee) == -1) {
        printf("\n");
        abort();
    };
}


void CEApiPoll(void * _Nonnull api, uint64_t timeout, void * _Nonnull context, CEApiPollCallback_f _Nonnull callback) {
    assert(api);
    assert(callback);
    assert(buffer);
    assert(itemSize <= sizeof(CEApiEvent_s));
    CEApiState_s *state = api;
    
    int retval, numevents = 0;
    uint64_t t = timeout / 1000;
    assert(t <= INT_MAX);
    retval = epoll_wait(state->fd, state->events, state->setSize, t);
    if (retval > 0) {
        numevents = retval;
        uint8_t * ptr = buffer;
        for (int j = 0; j < numevents; j++) {
            CEFileEventMask_es mask = CEFileEventMaskNone;
            CEApiEvent_s * e = state->events+j;
            if (e->ident == state->eventfd) {
                eventfd_t value = 0;
                eventfd_read(state->eventfd, &value);
            } else {
                if (e->events & EPOLLIN) mask |= CEFileEventMaskReadable;
                if (e->events & EPOLLOUT) mask |= CEFileEventMaskWritable;
                if (e->events & EPOLLERR) mask |= CEFileEventMaskReadWritable;
                if (e->events & EPOLLHUP) mask |= CEFileEventMaskReadWritable;
                if (mask != CEFileEventMaskNone) {
                    callback(api, context, (int)(e->ident), mask);
                }
            }
        }
    }
}

void CEApiWakeUp(void * _Nonnull api) {
    assert(api);
    CEApiState_s *state = api;
    eventfd_t value = 1;
    eventfd_write(state->eventfd, value);
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

void * _Nonnull CEApiCreate(int setSize) {
    int size = setSize > 256 ? setSize : 256;
    int fd = kqueue();
    assert(fd != -1);
    CEApiState_s *state = CEAllocateClear(sizeof(CEApiState_s));
    assert(state);
    state->fd = fd;
    state->setSize = size;
    state->events = CEAllocateClear(sizeof(CEApiEvent_s) * size);
    
    struct kevent event = {};
    event.ident = 0;
    event.filter = EVFILT_USER;
    event.fflags = NOTE_FFNOP;
    event.data = 0;
    event.udata = NULL;
    event.flags = EV_ADD | EV_ENABLE | EV_CLEAR;
    if (kevent(state->fd, &event, 1, NULL, 0, NULL) == -1) {
        abort();
    }
    return state;
}

//ev timer https://blog.csdn.net/Namcodream521/article/details/83032615

void CEApiAddReadEvent(void * _Nonnull api, int fd) {
    assert(api);
    CEApiState_s *state = api;
    
    struct kevent ke;
    EV_SET(&ke, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(state->fd, &ke, 1, NULL, 0, NULL) == -1) {
        abort();
    }
}
void CEApiRemoveReadEvent(void * _Nonnull api, int fd) {
    assert(api);
    CEApiState_s *state = api;
    
    struct kevent ke;
    EV_SET(&ke, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    if (kevent(state->fd, &ke, 1, NULL, 0, NULL) == -1) {
        abort();
    }
}
void CEApiAddWriteEvent(void * _Nonnull api, int fd) {
    assert(api);
    CEApiState_s *state = api;
    
    struct kevent ke;
    EV_SET(&ke, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    if (kevent(state->fd, &ke, 1, NULL, 0, NULL) == -1) {
        abort();
    }
}
void CEApiRemoveWriteEvent(void * _Nonnull api, int fd) {
    assert(api);
    CEApiState_s *state = api;
    
    struct kevent ke;
    EV_SET(&ke, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    if (kevent(state->fd, &ke, 1, NULL, 0, NULL) == -1) {
        abort();
    }
}

void CEApiPoll(void * _Nonnull api, uint64_t timeout, void * _Nonnull context, CEApiPollCallback_f _Nonnull callback) {
    struct timespec tv = {};
    if (timeout <= 0) {
        tv.tv_nsec = 1;
    } else {
        tv.tv_sec = timeout / 1000000;
        tv.tv_nsec = timeout % 1000000 * 1000;
    }
    
    assert(api);
    assert(callback);
    CEApiState_s * state = api;
    CEApiEvent_s * events = state->events;

    int numevents = 0;
    int retval = kevent(state->fd, NULL, 0, events, state->setSize, &tv);
    if (retval > 0) {
        numevents = retval;
        for(int j = 0; j < numevents; j++) {
            struct kevent *e = events + j;
            if (e->ident == 0 && e->filter == EVFILT_USER) {
                //do noting
            } else {
                if (e->filter == EVFILT_READ) {
                    callback(api, context, (int)(e->ident), CEFileEventMaskReadable);
                } else if (e->filter == EVFILT_WRITE) {
                    callback(api, context, (int)(e->ident), CEFileEventMaskWritable);
                }
            }
        }
    }
}

void CEApiWakeUp(void * _Nonnull api) {
    assert(api);
    CEApiState_s *state = api;

    struct kevent event = {};
    
    event.ident = 0;
    event.filter = EVFILT_USER;
    event.fflags = NOTE_TRIGGER | NOTE_FFNOP;
    event.data = 0;
    event.udata = NULL;
    event.flags = 0;
    if (kevent(state->fd, &event, 1, NULL, 0, NULL) == -1) {
        abort();
    }
}

char * _Nonnull CEApiName(void) {
    return "kqueue";
}


#endif

void CEApiFree(void * _Nonnull api) {
    assert(api);
    CEApiState_s *state = api;
    close(state->fd);
    CEDeallocate(state->events);
    CEDeallocate(state);
}
