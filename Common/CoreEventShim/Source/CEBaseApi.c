//
//  CEBaseApi.c
//  CoreEvent
//
//  Created by 王青海 on 2018/10/24.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CEBaseApi.h"

#include <poll.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/sysctl.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <wchar.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>


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


void CELogError(void) {
    printf("%s\n", strerror(errno));
}


size_t CEApiGetEventItemSize(void) {
    return sizeof(CEApiEvent_s);
}

struct _CEApiState {
    int fd;
#if CORE_EVENT_USE_EPOLL
    int eventfd;
#endif
    int size;
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

_Bool _CEApiInit(CEApiState_s * _Nonnull state) {
    int fd = epoll_create(1024); /* 1024 is just a hint for the kernel */
    if (fd == -1) {
        return false;
    }
    int eventfd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (eventfd == -1) {
        close(fd);
        return false;
    }

    CEApiEvent_s * ev = state->events+j;
    ev.events = EPOLLIN;
    ev.data.fd = eventfd
    
    if (epoll_ctl(fd, EPOLL_CTL_ADD, eventfd, &ev) == -1) {
        close(fd);
        close(eventfd);
        return false;
    };
    state->fd = fd;
    state->eventfd = eventfd;
    return true;
}

void _CEApiDeinit(CEApiState_s * _Nonnull state) {
    close(state->fd);
    close(state->eventfd);
}

void CEApiAddEvent(void * _Nonnull api, int fd) {
    assert(api);
    CEApiState_s *state = api;

    CEApiEvent_s ee = {0}; /* avoid valgrind warning */
    /* If the fd was already monitored for some event, we need a MOD
     * operation. Otherwise we need an ADD operation. */
    
    int op = EPOLL_CTL_ADD;
    ee.events = EPOLLERR | EPOLLHUP | EPOLLRDHUP;
    ee.data.fd = fd;
    if (epoll_ctl(state->fd,op,fd,&ee) == -1) {
        CELogError();
        abort();
    };
}

void CEApiRemoveExceptEvent(void * _Nonnull api, int fd) {
    assert(api);
    CEApiState_s *state = api;

    CEApiEvent_s ee = {0}; /* avoid valgrind warning */
    /* If the fd was already monitored for some event, we need a MOD
     * operation. Otherwise we need an ADD operation. */
    int op = EPOLL_CTL_DEL;
    ee.data.fd = fd;
    if (epoll_ctl(state->fd,op,fd,&ee) == -1) {
        CELogError();
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
    ee.events = EPOLLERR | EPOLLHUP | EPOLLRDHUP;
    if (mask & CEFileEventMaskReadable) ee.events |= EPOLLIN;
    if (mask & CEFileEventMaskWritable) ee.events |= EPOLLOUT;
    ee.data.fd = fd;
    if (epoll_ctl(state->fd,op,fd,&ee) == -1) {
        CELogError();
        abort();
    };
}

void CEApiPoll(void * _Nonnull api, uint64_t timeout, void * _Nonnull context, CEBaseApiPollCallback_f _Nonnull callback) {
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
                _Bool error = false;
                if (e->events & EPOLLIN) {
                    mask |= CEFileEventMaskReadable;
                }
                if (e->events & EPOLLOUT) {
                    mask |= CEFileEventMaskWritable
                };
                if (e->events & EPOLLERR) {
                    error = true;
                    mask |= CEFileEventMaskReadWritable
                };
                if (e->events & EPOLLHUP) {
                    error = true;
                    mask |= CEFileEventMaskReadWritable
                };
                if (e->events & EPOLLRDHUP) {
                    error = true;
                    mask |= CEFileEventMaskReadWritable
                };
                if (mask != CEFileEventMaskNone) {
                    callback(api, context, CEEventMake(e->data.fd, 0, mask));
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

int _CESetSize(int size) {
    return size;
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

_Bool _CEApiInit(CEApiState_s * _Nonnull state) {
    int fd = kqueue();
    if (fd == -1) {
        return false;
    }
    struct kevent event = {};
    event.ident = 0;
    event.filter = EVFILT_USER;
    event.fflags = NOTE_FFNOP;
    event.data = 0;
    event.udata = NULL;
    event.flags = EV_ADD | EV_ENABLE | EV_CLEAR;
    if (kevent(state->fd, &event, 1, NULL, 0, NULL) == -1) {
        CELogError();
        close(fd);
        return false;
    }
    state->fd = fd;
    return true;
}

void _CEApiDeinit(CEApiState_s * _Nonnull state) {
    close(state->fd);
}

//ev timer https://blog.csdn.net/Namcodream521/article/details/83032615


void CEApiAddEvent(void * _Nonnull api, int fd) {
    assert(api);
    CEApiState_s *state = api;
    
    uint16_t op = EV_ADD;

    struct kevent ke;
    EV_SET(&ke, fd, EVFILT_EXCEPT, op, 0, 0, NULL);
    if (kevent(state->fd, &ke, 1, NULL, 0, NULL) == -1) {
        if (op == EV_DELETE && errno == ENOENT) {
            
        } else {
            CELogError();
            abort();
        }
    }
}

void CEApiRemoveEvent(void * _Nonnull api, int fd) {
    assert(api);
    CEApiState_s *state = api;
    uint16_t op = EV_DELETE;
    struct kevent ke;
    EV_SET(&ke, fd, EVFILT_EXCEPT, op, 0, 0, NULL);
    if (kevent(state->fd, &ke, 1, NULL, 0, NULL) == -1) {
        if (op == EV_DELETE && errno == ENOENT) {
            
        } else {
            CELogError();
            abort();
        }
    }
    EV_SET(&ke, fd, EVFILT_READ, op, 0, 0, NULL);
    if (kevent(state->fd, &ke, 1, NULL, 0, NULL) == -1) {
        if (op == EV_DELETE && errno == ENOENT) {
            
        } else {
            CELogError();
            abort();
        }
    }
    EV_SET(&ke, fd, EVFILT_WRITE, op, 0, 0, NULL);
    if (kevent(state->fd, &ke, 1, NULL, 0, NULL) == -1) {
        if (op == EV_DELETE && errno == ENOENT) {
            
        } else {
            CELogError();
            abort();
        }
    }
}


void CEApiUpdateReadEvent(void * _Nonnull api, int fd, _Bool enable) {
    assert(api);
    CEApiState_s *state = api;
    
    uint16_t op = enable ? EV_ADD : EV_DELETE;

    struct kevent ke;
    EV_SET(&ke, fd, EVFILT_READ, op, 0, 0, NULL);
    if (kevent(state->fd, &ke, 1, NULL, 0, NULL) == -1) {
        if (op == EV_DELETE && errno == ENOENT) {
            
        } else {
            CELogError();
            abort();
        }
    }
}
void CEApiUpdateWriteEvent(void * _Nonnull api, int fd, _Bool enable) {
    assert(api);
    CEApiState_s *state = api;
    uint16_t op = enable ? EV_ADD : EV_DELETE;
    struct kevent ke;
    EV_SET(&ke, fd, EVFILT_WRITE, op, 0, 0, NULL);
    if (kevent(state->fd, &ke, 1, NULL, 0, NULL) == -1) {
        if (op == EV_DELETE && errno == ENOENT) {
            
        } else {
            CELogError();
            abort();
        }
    }
}

void CEApiPoll(void * _Nonnull api, uint64_t timeout, void * _Nonnull context, CEBaseApiPollCallback_f _Nonnull callback) {
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
                    callback(api, context, CEEventMake((int)(e->ident), 0, CEFileEventMaskReadable));
                } else if (e->filter == EVFILT_WRITE) {
                    callback(api, context, CEEventMake((int)(e->ident), 0, CEFileEventMaskWritable));
                } else {
                    callback(api, context, CEEventMake((int)(e->ident), 1, CEFileEventMaskReadable));
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
        CELogError();
        abort();
    }
}

char * _Nonnull CEApiName(void) {
    return "kqueue";
}
int _CESetSize(int size) {
    int64_t value = size;
    value = value * 2;
    if (value > INT32_MAX) {
        value = INT32_MAX;
    }
    return (int)value;
}
#endif

void * _Nullable CEApiCreate(int size) {
    if (size < 256) {
        size = 256;
    }
    int setSize = _CESetSize(size);
    CEApiState_s *state = malloc(sizeof(CEApiState_s));
    if (NULL == state) {
        return NULL;
    }
    CEApiEvent_s * events = malloc(sizeof(CEApiEvent_s) * setSize);
    if (NULL == events) {
        free(state);
        return NULL;
    }
    if (!_CEApiInit(state)) {
        free(state);
        free(events);
        return NULL;
    }
    state->setSize = setSize;
    state->size = size;
    state->events = events;
    return state;
}

void CEApiFree(void * _Nonnull api) {
    assert(api);
    CEApiState_s *state = api;
    _CEApiDeinit(state);
    free(state->events);
    free(state);
}
