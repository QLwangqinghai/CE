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

typedef struct epoll_event CEEvent_s;

#endif

#if CORE_EVENT_USE_KQUEUE

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

typedef struct kevent CEEvent_s;

#endif

struct _CEApiState {
    int fd;
    int pread;
    int pwrite;
    int xxxx;
    CEEvent_s * _Nonnull events;
};


static void CEApiPollDoPipe(CEApiState_s * _Nonnull api, void * _Nullable context, const CEApiPoolCallback_s * _Nonnull callback) {
    uint8_t buffer[1024] = {};
    recv(api->pread, buffer, 1024, MSG_DONTWAIT);
    
    callback->pipeCallback(context, api);
}


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


void * _Nullable CEApiCreate(int setsize, int pread) {
    int pio[2] = {};
    int result = pipe(pio);
    if (0 != result) {
        return NULL;
    }
    
    int flags = fcntl(pio[0], F_GETFL, 0);
    int fresult = fcntl(pio[0], F_SETFL, flags | O_NONBLOCK);
    if (fresult != 0) {
        close(pio[0]);
        close(pio[1]);
        return NULL;
    }
    
    int flags1 = fcntl(pio[1], F_GETFL, 0);
    int fresult1 = fcntl(pio[1], F_SETFL, flags1 | O_NONBLOCK);
    if (fresult1 != 0) {
        close(pio[0]);
        close(pio[1]);
        return NULL;
    }
    
    int fd = epoll_create(1024); /* 1024 is just a hint for the kernel */
    if (fd == -1) {
        close(pio[0]);
        close(pio[1]);
        return NULL;
    }

    CEApiState_s *state = CEAllocateClear(sizeof(CEApiState_s));
    state->fd = fd;
    state->pread = pio[0];
    state->pwrite = pio[1];
    state->xxxx = 0;

    if (!state) {
        close(pio[0]);
        close(pio[1]);
        close(fd);
        return NULL;
    };

    state->events = CEAllocateClear(sizeof(CEEvent_s)*setsize);
    if (!state->events) {
        close(pio[0]);
        close(pio[1]);
        close(fd);
        zfree(state);
        return NULL;
    }
    
    //添加 监听
    return state;
}



CEApiResult_t CEApiAddEvent(void * _Nonnull api, int fd, CEFileEventMask_es oldMask, CEFileEventMask_es mask) {
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
        return CEApiResultErrorSystemCall;
    };
    
    
    return CEApiResultSuccess;
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

int CEApiPoll(void * _Nonnull api, struct timeval * _Nullable tvp, int setsize, void * _Nullable context, const CEApiPoolCallback_s * _Nonnull callback) {
    assert(api);
    assert(callback);

    CEApiState_s *state = api;
    int retval, numevents = 0;
    
    retval = epoll_wait(state->fd, state->events, setsize,
                        tvp ? (int)(tvp->tv_sec*1000 + tvp->tv_usec/1000) : -1);
    if (retval > 0) {
        numevents = retval;
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
                    callback->fileEventCallback(context, api, (int)(e->ident), mask);
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


void * _Nullable CEApiCreate(unsigned int setsize) {
    int pio[2] = {};
    int result = pipe(pio);
    if (0 != result) {
        return NULL;
    }
    
    int flags = fcntl(pio[0], F_GETFL, 0);
    int fresult = fcntl(pio[0], F_SETFL, flags | O_NONBLOCK);
    if (fresult != 0) {
        close(pio[0]);
        close(pio[1]);
        return NULL;
    }
    
    int flags1 = fcntl(pio[1], F_GETFL, 0);
    int fresult1 = fcntl(pio[1], F_SETFL, flags1 | O_NONBLOCK);
    if (fresult1 != 0) {
        close(pio[0]);
        close(pio[1]);
        return NULL;
    }
    
    int fd = kqueue(); /* 1024 is just a hint for the kernel */
    if (fd == -1) {
        close(pio[0]);
        close(pio[1]);
        return NULL;
    }
    CEApiState_s *state = CEAllocateClear(sizeof(CEApiState_s));
    
    if (!state) {
        close(fd);
        close(pio[0]);
        close(pio[1]);
        return NULL;
    };
    
    state->events = CEAllocateClear(sizeof(CEEvent_s)*setsize);
    if (!state->events) {
        close(fd);
        close(pio[0]);
        close(pio[1]);
        CEDeallocate(state);
        return NULL;
    }
    state->fd = fd;
    state->pread = pio[0];
    state->pwrite = pio[1];
    state->xxxx = 0;

    
    //添加 监听
    return state;
}


//ev timer https://blog.csdn.net/Namcodream521/article/details/83032615


CEApiResult_t CEApiAddEvent(void * _Nonnull api, int fd, CEFileEventMask_es oldMask, CEFileEventMask_es mask) {
    assert(api);
    CEApiState_s *state = api;
    
    struct kevent ke;
    
    if (mask & CEFileEventMaskReadable) {
        EV_SET(&ke, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        if (kevent(state->fd, &ke, 1, NULL, 0, NULL) == -1) {
            return CEApiResultErrorSystemCall;
        }
    }
    if (mask & CEFileEventMaskWritable) {
        EV_SET(&ke, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
        if (kevent(state->fd, &ke, 1, NULL, 0, NULL) == -1) {
            return CEApiResultErrorSystemCall;
        }
    }
    return CEApiResultSuccess;
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

int CEApiPoll(void * _Nonnull api, struct timeval * _Nullable tvp, int setsize, void * _Nullable context, const CEApiPoolCallback_s * _Nonnull callback) {
    assert(api);
    assert(callback);

    CEApiState_s *state = api;

    int retval, numevents = 0;
    
    if (tvp != NULL) {
        struct timespec timeout;
        timeout.tv_sec = tvp->tv_sec;
        timeout.tv_nsec = tvp->tv_usec * 1000 + 500;
        retval = kevent(state->fd, NULL, 0, state->events, setsize, &timeout);
    } else {
        retval = kevent(state->fd, NULL, 0, state->events, setsize, NULL);
    }
    
    if (retval > 0) {
        numevents = retval;
        for(int j = 0; j < numevents; j++) {
            CEFileEventMask_es mask = CEFileEventMaskNone;
            struct kevent *e = state->events+j;
            if (e->ident == state->pread) {
                CEApiPollDoPipe(state, context, callback);
            } else {
                if (e->filter == EVFILT_READ) mask |= CEFileEventMaskReadable;
                if (e->filter == EVFILT_WRITE) mask |= CEFileEventMaskWritable;
                if (mask != CEFileEventMaskNone) {
                    callback->fileEventCallback(context, api, (int)(e->ident), mask);
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


void CEApiResize(void * _Nonnull api, uint32_t setsize) {
    assert(api);
    CEApiState_s *state = api;
    
    state->events = CEReallocate(state->events, sizeof(CEEvent_s) * setsize);
}


void CEApiFree(void * _Nonnull api) {
    assert(api);
    CEApiState_s *state = api;
    
    close(state->pread);
    close(state->pwrite);
    close(state->fd);
    CEDeallocate(state->events);
    CEDeallocate(state);
}


void _CEApiWakeUp(CEApiState_s * _Nonnull state, uint32_t count) {
    if (count > 1000) {
        abort();
    }

    uintptr_t value = 0;
    ssize_t sendResult = send(state->pwrite, &value, sizeof(uintptr_t), MSG_DONTWAIT);
    if (sendResult <= 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
               //缓冲区满，不做任何事
        } else if (errno == EINTR) {
            //中断
            CEFileEventMask_es mask = CEApiWait(state->pwrite, CEFileEventMaskWritable, 100);
            if (mask == CEFileEventMaskWritable) {
                _CEApiWakeUp(state, count + 1);
            }
        } else if (errno == ECONNRESET) {
            fprintf(stderr, "CEApiWakeUp error 1 %s; \n", strerror(errno));
            fflush(stderr);
            abort();
        } else {
            fprintf(stderr, "CEApiWakeUp error 2 %s; \n", strerror(errno));
            fflush(stderr);
            abort();
        }
    }
}


void CEApiWakeUp(void * _Nonnull api) {
    assert(api);
    CEApiState_s *state = api;

    _CEApiWakeUp(state, 0);
}


CEFileEventMask_es CEApiWait(int fd, CEFileEventMask_es mask, int milliseconds) {
    struct pollfd pfd = {};
    CEFileEventMask_es retmask = 0;
    pfd.fd = fd;
    if (mask & CEFileEventMaskReadable) pfd.events |= POLLIN;
    if (mask & CEFileEventMaskWritable) pfd.events |= POLLOUT;
    
    if (poll(&pfd, 1, milliseconds) == 1) {
        if (pfd.revents & POLLIN) retmask |= CEFileEventMaskReadable;
        if (pfd.revents & POLLOUT) retmask |= CEFileEventMaskWritable;
        if (pfd.revents & POLLERR) retmask |= (CEFileEventMaskReadWritable & mask);
        if (pfd.revents & POLLHUP) retmask |= (CEFileEventMaskReadWritable & mask);
        return retmask;
    } else {
        return CEFileEventMaskNone;
    }
}

