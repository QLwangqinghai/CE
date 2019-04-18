//
//  CEBaseApi.c
//  CoreEvent
//
//  Created by 王青海 on 2018/10/24.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CEBaseApi.h"
#include "zmalloc.h"
#include "config.h"



struct _CEApiState;
typedef struct _CEApiState CEApiState_s;




//#undef CORE_EVENT_USE_EVPORT
//#undef CORE_EVENT_USE_EPOLL
//#undef CORE_EVENT_USE_KQUEUE
//#undef CORE_EVENT_USE_SELECT
//
//#define CORE_EVENT_USE_EVPORT 0
//#define CORE_EVENT_USE_EPOLL 0
//#define CORE_EVENT_USE_KQUEUE 1
//#define CORE_EVENT_USE_SELECT 0



#if CORE_EVENT_USE_SELECT


#include <sys/select.h>
#include <string.h>

struct _CEApiState {
    void * _Nonnull context;
    CEApiPoolGetEventMask_f _Nonnull getEventMask;
    fd_set rfds, wfds;
    /* We need to have a copy of the fd sets as it's not safe to reuse
     * FD sets after select(). */
    fd_set _rfds, _wfds;
};

void * _Nullable CEApiCreate(int setsize, void * _Nonnull context, CEApiPoolGetEventMask_f _Nonnull func) {
    if (NULL == func || NULL == context) {
        return NULL;
    }
    
    CEApiState_s *state = zmalloc(sizeof(CEApiState_s));
    
    if (!state) return NULL;
    
    state->context = context;
    state->getEventMask = context;
    FD_ZERO(&state->rfds);
    FD_ZERO(&state->wfds);
    return state;
}

int CEApiResize(void * _Nonnull api, int setsize) {
    /* Just ensure we have enough room in the fd_set type. */
    assert(api);
    if (setsize >= FD_SETSIZE) return -1;
    return 0;
}

void CEApiFree(void * _Nonnull api) {
    assert(api);
    zfree(api);
}

int CEApiAddEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask) {
    assert(api);

    CEApiState_s *state = api;
    
    if (mask & CEFileEventMaskReadable) FD_SET(fd,&state->rfds);
    if (mask & CEFileEventMaskWritable) FD_SET(fd,&state->wfds);
    return 0;
}

void CEApiRemoveEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask) {
    assert(api);

    CEApiState_s *state = api;
    
    if (mask & CEFileEventMaskReadable) FD_CLR(fd,&state->rfds);
    if (mask & CEFileEventMaskWritable) FD_CLR(fd,&state->wfds);
}

int CEApiPoll(void * _Nonnull api, struct timeval * _Nullable tvp, int maxfd, int setsize, void * _Nullable context, CEApiPoolCallback_f _Nonnull callback) {
    assert(api);
    assert(callback);

    CEApiState_s *state = api;
    int retval, j, numevents = 0;
    
    memcpy(&state->_rfds,&state->rfds,sizeof(fd_set));
    memcpy(&state->_wfds,&state->wfds,sizeof(fd_set));
    retval = select(maxfd+1, &state->_rfds, &state->_wfds, NULL, tvp);
    
    
    if (retval > 0) {
        for (j = 0; j <= maxfd; j++) {
            CEFileEventMask_es mask = 0;
            
            CEFileEventMask_es oldMask = state->func(state->context, state, j);
            
            if (oldMask == CEFileEventMaskNone) continue;
            if (oldMask & CEFileEventMaskReadable && FD_ISSET(j,&state->_rfds))
                mask |= CEFileEventMaskReadable;
            if (oldMask & CEFileEventMaskWritable && FD_ISSET(j,&state->_wfds))
                mask |= CEFileEventMaskWritable;
            
            callback(context, api, numevents, j, mask);
            
            numevents++;
        }
    }
    return numevents;
}

char * _Nonnull CEApiName(void) {
    return "select";
}

#endif





#if CORE_EVENT_USE_EPOLL

#include <sys/epoll.h>

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
//int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
//int epoll_wait(int epfd, struct epoll_event *events,int maxevents, int timeout);
//
//#define EPOLL_CTL_ADD 1
//#define EPOLL_CTL_MOD 2
//#define EPOLLIN 1
//#define EPOLLOUT 2
//#define EPOLL_CTL_DEL 2
//#define EPOLLERR 2
//#define EPOLLHUP 2


struct _CEApiState {
    void * _Nonnull context;
    CEApiPoolGetEventMask_f _Nonnull getEventMask;
    int epfd;
    struct epoll_event *events;
};

void * _Nullable CEApiCreate(int setsize, void * _Nonnull context, CEApiPoolGetEventMask_f _Nonnull func) {
    if (NULL == func || NULL == context) {
        return NULL;
    }
    CEApiState_s *state = zmalloc(sizeof(CEApiState_s));
    
    if (!state) return NULL;
    
    state->context = context;
    state->getEventMask = context;

    state->events = zmalloc(sizeof(struct epoll_event)*setsize);
    if (!state->events) {
        zfree(state);
        return NULL;
    }
    state->epfd = epoll_create(1024); /* 1024 is just a hint for the kernel */
    if (state->epfd == -1) {
        zfree(state->events);
        zfree(state);
        return NULL;
    }
    return state;
}

int CEApiResize(void * _Nonnull api, int setsize) {
    assert(api);
    CEApiState_s *state = api;
    
    state->events = zrealloc(state->events, sizeof(struct epoll_event)*setsize);
    return 0;
}

void CEApiFree(void * _Nonnull api) {
    assert(api);
    CEApiState_s *state = api;

    close(state->epfd);
    zfree(state->events);
    zfree(state);
}

int CEApiAddEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask) {
    assert(api);
    CEApiState_s *state = api;

    struct epoll_event ee = {0}; /* avoid valgrind warning */
    /* If the fd was already monitored for some event, we need a MOD
     * operation. Otherwise we need an ADD operation. */
    
    CEFileEventMask_es oldMask = state->func(state->context, state, fd);
    int op = oldMask == CEFileEventMaskNone ?
    EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    
    ee.events = 0;
    mask |= oldMask; /* Merge old events */
    if (mask & CEFileEventMaskReadable) ee.events |= EPOLLIN;
    if (mask & CEFileEventMaskWritable) ee.events |= EPOLLOUT;
    ee.data.fd = fd;
    if (epoll_ctl(state->epfd,op,fd,&ee) == -1) return -1;
    return 0;
}

void CEApiRemoveEvent(void * _Nonnull api, int fd, CEFileEventMask_es delmask) {
    assert(api);
    CEApiState_s *state = api;

    struct epoll_event ee = {0}; /* avoid valgrind warning */
    CEFileEventMask_es oldMask = state->func(state->context, state, fd);
    int mask = oldMask & (~delmask);

    ee.events = 0;
    if (mask & CEFileEventMaskReadable) ee.events |= EPOLLIN;
    if (mask & CEFileEventMaskWritable) ee.events |= EPOLLOUT;
    ee.data.fd = fd;
    if (mask != CEFileEventMaskNone) {
        epoll_ctl(state->epfd,EPOLL_CTL_MOD,fd,&ee);
    } else {
        /* Note, Kernel < 2.6.9 requires a non null event pointer even for
         * EPOLL_CTL_DEL. */
        epoll_ctl(state->epfd,EPOLL_CTL_DEL,fd,&ee);
    }
}

int CEApiPoll(void * _Nonnull api, struct timeval * _Nullable tvp, int maxfd, int setsize, void * _Nullable context, CEApiPoolCallback_f _Nonnull callback) {
    assert(api);
    CEApiState_s *state = api;
    int retval, numevents = 0;
    
    retval = epoll_wait(state->epfd, state->events, setsize,
                        tvp ? (int)(tvp->tv_sec*1000 + tvp->tv_usec/1000) : -1);
    if (retval > 0) {
        int j;
        
        numevents = retval;
        for (j = 0; j < numevents; j++) {
            int mask = 0;
            struct epoll_event *e = state->events+j;
            
            if (e->events & EPOLLIN) mask |= CEFileEventMaskReadable;
            if (e->events & EPOLLOUT) mask |= CEFileEventMaskWritable;
            if (e->events & EPOLLERR) mask |= CEFileEventMaskWritable;
            if (e->events & EPOLLHUP) mask |= CEFileEventMaskWritable;
            callback(context, api, j, e->data.fd, mask);
        }
    }
    return numevents;
}

char * _Nonnull CEApiName(void) {
    return "epoll";
}

#endif




#if CORE_EVENT_USE_EVPORT

#include <assert.h>
#include <errno.h>
#include <port.h>
#include <poll.h>

#include <sys/types.h>
#include <sys/time.h>

#include <stdio.h>

int evport_debug = 0;

/*
 * This file implements the ae API using event ports, present on Solaris-based
 * systems since Solaris 10.  Using the event port interface, we associate file
 * descriptors with the port.  Each association also includes the set of poll(2)
 * events that the consumer is interested in (e.g., POLLIN and POLLOUT).
 *
 * There's one tricky piece to this implementation: when we return events via
 * CEApiPoll, the corresponding file descriptors become dissociated from the
 * port.  This is necessary because poll events are level-triggered, so if the
 * fd didn't become dissociated, it would immediately fire another event since
 * the underlying state hasn't changed yet.  We must re-associate the file
 * descriptor, but only after we know that our caller has actually read from it.
 * The ae API does not tell us exactly when that happens, but we do know that
 * it must happen by the time CEApiPoll is called again.  Our solution is to
 * keep track of the last fds returned by CEApiPoll and re-associate them next
 * time CEApiPoll is invoked.
 *
 * To summarize, in this module, each fd association is EITHER (a) represented
 * only via the in-kernel association OR (b) represented by pending_fds and
 * pending_masks.  (b) is only true for the last fds we returned from CEApiPoll,
 * and only until we enter CEApiPoll again (at which point we restore the
 * in-kernel association).
 */
#define MAX_EVENT_BATCHSZ 512

struct _CEApiState {
    void * _Nonnull context;
    CEApiPoolGetEventMask_f _Nonnull getEventMask;

    int     portfd;                             /* event port */
    int     npending;                           /* # of pending fds */
    int     pending_fds[MAX_EVENT_BATCHSZ];     /* pending fds */
    CEFileEventMask_es     pending_masks[MAX_EVENT_BATCHSZ];   /* pending fds' masks */
};

void * _Nullable CEApiCreate(int setsize, void * _Nonnull context, CEApiPoolGetEventMask_f _Nonnull func) {
    if (NULL == func || NULL == context) {
        return NULL;
    }
    
    int i;
    CEApiState_s *state = zmalloc(sizeof(CEApiState_s));
    if (!state) return NULL;
    
    state->portfd = port_create();
    if (state->portfd == -1) {
        zfree(state);
        return NULL;
    }
    state->context = context;
    state->getEventMask = context;
    state->npending = 0;
    
    for (i = 0; i < MAX_EVENT_BATCHSZ; i++) {
        state->pending_fds[i] = -1;
        state->pending_masks[i] = CEFileEventMaskNone;
    }
    return state;
}

int CEApiResize(void * _Nonnull api, int setsize) {
    /* Nothing to resize here. */
    return 0;
}

void CEApiFree(void * _Nonnull api) {
    assert(api);
    CEApiState_s *state = api;
    
    close(state->portfd);
    zfree(state);
}

int CEApiEvportLookupPending(CEApiState_s *state, int fd) {
    int i;
    
    for (i = 0; i < state->npending; i++) {
        if (state->pending_fds[i] == fd)
            return (i);
    }
    
    return (-1);
}

/*
 * Helper function to invoke port_associate for the given fd and mask.
 */
int CEApiEvportaeAssociate(const char *where, int portfd, int fd, int mask) {
    int events = 0;
    int rv, err;
    
    if (mask & CEFileEventMaskReadable)
        events |= POLLIN;
    if (mask & CEFileEventMaskWritable)
        events |= POLLOUT;
    
    if (evport_debug)
        fprintf(stderr, "%s: port_associate(%d, 0x%x) = ", where, fd, events);
    
    rv = port_associate(portfd, PORT_SOURCE_FD, fd, events,
                        (void *)(uintptr_t)mask);
    err = errno;
    
    if (evport_debug)
        fprintf(stderr, "%d (%s)\n", rv, rv == 0 ? "no error" : strerror(err));
    
    if (rv == -1) {
        fprintf(stderr, "%s: port_associate: %s\n", where, strerror(err));
        
        if (err == EAGAIN)
            fprintf(stderr, "CEApiEvportaeAssociate: event port limit exceeded.");
    }
    
    return rv;
}

int CEApiAddEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask) {
    assert(api);
    CEApiState_s *state = api;
    CEFileEventMask_es fullmask;
    int pfd;
    
    if (evport_debug)
        fprintf(stderr, "CEApiAddEvent: fd %d mask 0x%x\n", fd, mask);
    
    /*
     * Since port_associate's "events" argument replaces any existing events, we
     * must be sure to include whatever events are already associated when
     * we call port_associate() again.
     */
    fullmask = mask | eventLoop->events[fd].mask;
    pfd = CEApiEvportLookupPending(state, fd);
    
    if (pfd != -1) {
        /*
         * This fd was recently returned from CEApiPoll.  It should be safe to
         * assume that the consumer has processed that poll event, but we play
         * it safer by simply updating pending_mask.  The fd will be
         * re-associated as usual when CEApiPoll is called again.
         */
        if (evport_debug)
            fprintf(stderr, "CEApiAddEvent: adding to pending fd %d\n", fd);
        state->pending_masks[pfd] |= fullmask;
        return 0;
    }
    
    return (CEApiEvportaeAssociate("CEApiAddEvent", state->portfd, fd, fullmask));
}

void CEApiRemoveEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask) {
    assert(api);
    CEApiState_s *state = api;

    int fullmask, pfd;
    
    if (evport_debug)
        fprintf(stderr, "del fd %d mask 0x%x\n", fd, mask);
    
    pfd = CEApiEvportLookupPending(state, fd);
    
    if (pfd != -1) {
        if (evport_debug)
            fprintf(stderr, "deleting event from pending fd %d\n", fd);
        
        /*
         * This fd was just returned from CEApiPoll, so it's not currently
         * associated with the port.  All we need to do is update
         * pending_mask appropriately.
         */
        state->pending_masks[pfd] &= ~mask;
        
        if (state->pending_masks[pfd] == CEFileEventMaskNone)
            state->pending_fds[pfd] = -1;
        
        return;
    }
    
    /*
     * The fd is currently associated with the port.  Like with the add case
     * above, we must look at the full mask for the file descriptor before
     * updating that association.  We don't have a good way of knowing what the
     * events are without looking into the eventLoop state directly.  We rely on
     * the fact that our caller has already updated the mask in the eventLoop.
     */
    
    
    fullmask = state->func(state->context, state, fd);
    if (fullmask == CEFileEventMaskNone) {
        /*
         * We're removing *all* events, so use port_dissociate to remove the
         * association completely.  Failure here indicates a bug.
         */
        if (evport_debug)
            fprintf(stderr, "CEApiRemoveEvent: port_dissociate(%d)\n", fd);
        
        if (port_dissociate(state->portfd, PORT_SOURCE_FD, fd) != 0) {
            perror("CEApiRemoveEvent: port_dissociate");
            abort(); /* will not return */
        }
    } else if (CEApiEvportaeAssociate("CEApiRemoveEvent", state->portfd, fd,
                              fullmask) != 0) {
        /*
         * ENOMEM is a potentially transient condition, but the kernel won't
         * generally return it unless things are really bad.  EAGAIN indicates
         * we've reached an resource limit, for which it doesn't make sense to
         * retry (counter-intuitively).  All other errors indicate a bug.  In any
         * of these cases, the best we can do is to abort.
         */
        abort(); /* will not return */
    }
}

int CEApiPoll(void * _Nonnull api, struct timeval * _Nullable tvp, int maxfd, int setsize, void * _Nullable context, CEApiPoolCallback_f _Nonnull callback) {
    assert(api);
    CEApiState_s *state = api;

    struct timespec timeout, *tsp;
    int mask, i;
    uint_t nevents;
    port_event_t event[MAX_EVENT_BATCHSZ];
    
    /*
     * If we've returned fd events before, we must re-associate them with the
     * port now, before calling port_get().  See the block comment at the top of
     * this file for an explanation of why.
     */
    for (i = 0; i < state->npending; i++) {
        if (state->pending_fds[i] == -1)
        /* This fd has since been deleted. */
            continue;
        
        if (CEApiEvportaeAssociate("CEApiPoll", state->portfd,
                           state->pending_fds[i], state->pending_masks[i]) != 0) {
            /* See CEApiRemoveEvent for why this case is fatal. */
            abort();
        }
        
        state->pending_masks[i] = CEFileEventMaskNone;
        state->pending_fds[i] = -1;
    }
    
    state->npending = 0;
    
    if (tvp != NULL) {
        timeout.tv_sec = tvp->tv_sec;
        timeout.tv_nsec = tvp->tv_usec * 1000;
        tsp = &timeout;
    } else {
        tsp = NULL;
    }
    
    /*
     * port_getn can return with errno == ETIME having returned some events (!).
     * So if we get ETIME, we check nevents, too.
     */
    nevents = 1;
    if (port_getn(state->portfd, event, MAX_EVENT_BATCHSZ, &nevents,
                  tsp) == -1 && (errno != ETIME || nevents == 0)) {
        if (errno == ETIME || errno == EINTR)
            return 0;
        
        /* Any other error indicates a bug. */
        perror("CEApiPoll: port_get");
        abort();
    }
    
    state->npending = nevents;
    
    for (i = 0; i < nevents; i++) {
        mask = 0;
        if (event[i].portev_events & POLLIN)
            mask |= CEFileEventMaskReadable;
        if (event[i].portev_events & POLLOUT)
            mask |= CEFileEventMaskWritable;
        
        callback(context, api, i, event[i].portev_object, mask);

        if (evport_debug)
            fprintf(stderr, "CEApiPoll: fd %d mask 0x%x\n",
                    (int)event[i].portev_object, mask);
        
        state->pending_fds[i] = event[i].portev_object;
        state->pending_masks[i] = (uintptr_t)event[i].portev_user;
    }
    
    return nevents;
}

char * _Nonnull CEApiName(void) {
    return "evport";
}


#endif



#if CORE_EVENT_USE_KQUEUE

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

struct _CEApiState {
    void * _Nonnull context;
    CEApiPoolGetEventMask_f _Nonnull getEventMask;
    int kqfd;
    struct kevent *events;
};

void * _Nullable CEApiCreate(int setsize, void * _Nonnull context, CEApiPoolGetEventMask_f _Nonnull func) {
    if (NULL == func || NULL == context) {
        return NULL;
    }
    
    CEApiState_s *state = zmalloc(sizeof(CEApiState_s));
    
    if (!state) return NULL;
    state->events = zmalloc(sizeof(struct kevent)*setsize);
    if (!state->events) {
        zfree(state);
        return NULL;
    }
    state->context = context;
    state->getEventMask = context;
    state->kqfd = kqueue();
    if (state->kqfd == -1) {
        zfree(state->events);
        zfree(state);
        return NULL;
    }
    return state;
}

int CEApiResize(void * _Nonnull api, int setsize) {
    assert(api);
    CEApiState_s *state = api;
    
    state->events = zrealloc(state->events, sizeof(struct kevent)*setsize);
    return 0;
}

void CEApiFree(void * _Nonnull api) {
    assert(api);
    CEApiState_s *state = api;

    close(state->kqfd);
    zfree(state->events);
    zfree(state);
}

int CEApiAddEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask) {
    assert(api);
    CEApiState_s *state = api;
    
    struct kevent ke;
    
    if (mask & CEFileEventMaskReadable) {
        EV_SET(&ke, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        if (kevent(state->kqfd, &ke, 1, NULL, 0, NULL) == -1) return -1;
    }
    if (mask & CEFileEventMaskWritable) {
        EV_SET(&ke, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
        if (kevent(state->kqfd, &ke, 1, NULL, 0, NULL) == -1) return -1;
    }
    return 0;
}

void CEApiRemoveEvent(void * _Nonnull api, int fd, CEFileEventMask_es mask) {
    assert(api);
    CEApiState_s *state = api;
    
    struct kevent ke;
    
    if (mask & CEFileEventMaskReadable) {
        EV_SET(&ke, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        kevent(state->kqfd, &ke, 1, NULL, 0, NULL);
    }
    if (mask & CEFileEventMaskWritable) {
        EV_SET(&ke, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
        kevent(state->kqfd, &ke, 1, NULL, 0, NULL);
    }
}

int CEApiPoll(void * _Nonnull api, struct timeval * _Nullable tvp, int maxfd, int setsize, void * _Nullable context, CEApiPoolCallback_f _Nonnull callback) {
    assert(api);
    assert(callback);
    CEApiState_s *state = api;

    int retval, numevents = 0;
    
    if (tvp != NULL) {
        struct timespec timeout;
        timeout.tv_sec = tvp->tv_sec;
        timeout.tv_nsec = tvp->tv_usec * 1000;
        retval = kevent(state->kqfd, NULL, 0, state->events, setsize,
                        &timeout);
    } else {
        retval = kevent(state->kqfd, NULL, 0, state->events, setsize,
                        NULL);
    }
    
    if (retval > 0) {
        int j;
        
        numevents = retval;
        for(j = 0; j < numevents; j++) {
            CEFileEventMask_es mask = 0;
            struct kevent *e = state->events+j;
            
            if (e->filter == EVFILT_READ) mask |= CEFileEventMaskReadable;
            if (e->filter == EVFILT_WRITE) mask |= CEFileEventMaskWritable;
            
            callback(context, api, j, (int)(e->ident), mask);
        }
    }
    return numevents;
}

char * _Nonnull CEApiName(void) {
    return "kqueue";
}

#endif



