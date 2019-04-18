//
//  CTWaiter.c
//  CoreThread
//
//  Created by vector on 2018/12/4.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include "CTWaiter.h"
#include "CEMemory.h"

static const uint32_t CTIndexInvalid = 0xFFFFFFFFul;
static const uint32_t CTIndexMax = 4096;


static pthread_mutex_t __CTMutex = PTHREAD_MUTEX_INITIALIZER;

static uint32_t __CTSequence = 0;
static uint64_t __CTIndexSet[64] = {};
static uint64_t __CTIndexSetRoot = 0;
static CTSpecific_s * _Nullable __CTSpecificGlobal[4096];

void CTMutexLock(void) {
    pthread_mutex_lock(&__CTMutex);
}
void CTMutexUnlock(void) {
    pthread_mutex_unlock(&__CTMutex);
}


static inline uint32_t CTGetUnusedIndex64(uint64_t n) {
    if (UINT64_MAX == n) {
        return UINT32_MAX;
    }
    
    uint32_t i = 0;
    uint32_t j = 63;
    uint32_t n1 = (uint32_t)(n >> 32);
    if (n1 == UINT32_MAX) {
        i += 32;
        n1 = (uint32_t)(n & UINT32_MAX);
    } else {
        j -= 32;
    }
    uint16_t n2 = (uint16_t)(n1 >> 16);
    if (n2 == UINT16_MAX) {
        i += 16;
        n2 = (uint16_t)(n1 & UINT16_MAX);
    } else {
        j -= 16;
    }
    uint8_t n3 = (uint8_t)(n2 >> 8);
    if (n3 == UINT8_MAX) {
        i += 8;
        n3 = (uint8_t)(n2 & UINT8_MAX);
    } else {
        j -= 8;
    }
    uint8_t n4 = (n3 >> 4);
    if (n4 == 0xf) {
        i += 4;
        n4 = (n3 & 0xf);
    } else {
        j -= 4;
    }
    uint8_t n5 = (n4 >> 2);
    if (n5 == 0x3) {
        i += 2;
        n5 = (n4 & 0x3);
    } else {
        j -= 2;
    }
    uint8_t n6 = (n5 >> 1);
    if (n6 == 0x1) {
        return j;
    } else {
        return i;
    }
}

static inline uint32_t __CTIndexMake(void) {
    uint64_t mask = 0x8000000000000000ull;
    
    uint32_t index = CTIndexInvalid;
    
    uint64_t n0 = __CTIndexSetRoot;
    uint32_t offset0 = CTGetUnusedIndex64(n0);
    if (offset0 > 63) {
        return index;
    }
    
    uint32_t index1 = offset0;
    uint64_t n1 = __CTIndexSet[index1];
    uint32_t offset1 = CTGetUnusedIndex64(n1);
    if (offset1 > 63) {
        abort();
    }
    
    n1 = n1 | (mask >> offset1);
    __CTIndexSet[index1] = n1;
    
    if (UINT64_MAX == n1) {
        n0 = n0 | (mask >> offset0);
        __CTIndexSetRoot = n0;
    }
    index = (uint32_t)((index1 << 6) + offset1);
    return index;
}

static inline void __CTIndexFree(uint32_t index) {
    assert(index <= 4096);
    uint64_t mask = 0x8000000000000000ull;
    
    uint64_t n0 = __CTIndexSetRoot;
    uint32_t offset0 = index >> 6;
    
    uint32_t index1 = offset0;
    uint32_t offset1 = index & 0x3F;
    uint64_t n1 = __CTIndexSet[index1];
    
    
    n1 = n1 & (~(mask >> offset1));
    __CTIndexSet[index1] = n1;
    
    n0 = n0 & (~(mask >> offset0));
    __CTIndexSetRoot = n0;
}
static inline uint32_t __CTNextSequence() {
    uint32_t sequence = __CTSequence;
    __CTSequence += 1;
    return sequence;
}



CTSpecific_s * _Nullable CTSpecificInit(void);
void CTSpecificDeinit(CTSpecific_s * _Nonnull specific);


void CTSyncWaiterInit(CTSyncWaiter_s * _Nonnull waiter, uint64_t id, uint32_t index, uint32_t sequence);
void CTSyncWaiterDeinit(CTSyncWaiter_s * _Nonnull waiter);

CTSpecific_s * _Nonnull CTSpecificInit(void) {
    CTSpecific_s * specific = NULL;
    
    CTMutexLock();
    uint32_t index = __CTIndexMake();
    if (index < CTIndexMax) {
        uint32_t sequence = __CTNextSequence();
        uint64_t id = sequence;
        id = id << 32;
        id += index;
        
        specific = (CTSpecific_s *)malloc(sizeof(CTSpecific_s));
        assert(specific);
        memset(specific, 0, sizeof(CTSpecific_s));
        
        specific->owner.thread = pthread_self();
        CTSyncWaiterInit(&(specific->waiter), id, index, sequence);
        
        
        __CTSpecificGlobal[index] = specific;
    }
    CTMutexUnlock();

    return specific;
}
void CTSpecificDeinit(CTSpecific_s * _Nonnull specific) {
    CTMutexLock();

    uint64_t id = specific->waiter.id;
    uint32_t index = (uint32_t)id;
    assert(specific == __CTSpecificGlobal[index]);
    __CTSpecificGlobal[index] = NULL;
    CTSyncWaiterDeinit(&(specific->waiter));

    __CTIndexFree(index);

    
    CTMutexUnlock();
}

void CTSyncWaiterInit(CTSyncWaiter_s * _Nonnull waiter, uint64_t id, uint32_t index, uint32_t sequence) {

#if __APPLE__
    snprintf(waiter->name, 64, "/%x.%x.%x.0", getpid(), index, sequence);
    sem_t * sem = sem_open(waiter->name, O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (SEM_FAILED == sem) {
        fprintf(stderr, "CTSyncWaiterInit sem_open error %s; \n", strerror(errno));
        fflush(stderr);
        abort();
    }
    waiter->lock = sem;
#else
    if (0 != sem_init(&(waiter->lockValue), 0, 0)) {
        fprintf(stderr, "CTSyncWaiterInit sem_init error %s; \n", strerror(errno));
        fflush(stderr);
        abort();
    }
    waiter->lock = &(waiter->lockValue);
#endif
    atomic_store(&(waiter->whoWakeUp), (uintptr_t)(NULL));
    waiter->id = id;
}

void CTSyncWaiterDeinit(CTSyncWaiter_s * _Nonnull waiter) {
#if __APPLE__
    sem_close(waiter->lock);
    sem_unlink(waiter->name);
#else
    sem_destroy(waiter->lock);
#endif
}


void CTSyncWaiterWait(CTSyncWaiter_s * _Nonnull waiter) {
    if (0 != sem_wait(waiter->lock)) {
        fprintf(stderr, "CEThreadWaiterWait sem_wait error %s; \n", strerror(errno));
        fflush(stderr);
        abort();
    }
}
void CTSyncWaiterWakeUp(CTSyncWaiter_s * _Nonnull waiter) {
    if (0 != sem_post(waiter->lock)) {
        fprintf(stderr, "CEThreadWaiterWakeUp sem_post error %s; \n", strerror(errno));
        fflush(stderr);
        abort();
    }
}


pthread_key_t CTKeyShared(void);
static inline CTSpecific_s * _Nullable CTSpecificGetCurrentIfCreated(void) {
    CTSpecific_s * specific = (CTSpecific_s *)pthread_getspecific(CTKeyShared());
    return specific;
}

CTSpecific_s * _Nonnull CTSpecificGetCurrent(void) {
    CTSpecific_s * specific = CTSpecificGetCurrentIfCreated();
    if (NULL == specific) {
        specific = CTSpecificInit();
        pthread_setspecific(CTKeyShared(), specific);
    }
    return specific;
}
CTSyncWaiter_s * _Nonnull CTSyncWaiterGetCurrent(void) {
    CTSpecific_s * specific = CTSpecificGetCurrent();
    return &(specific->waiter);
}



static pthread_key_t __CTKeyShared = 0;
pthread_key_t CTKeyShared(void) {
    CTShare();
    return __CTKeyShared;
}

void __CTKeySharedValueDealloc(void * _Nullable value) {
    CTSpecific_s * specific = CTSpecificGetCurrentIfCreated();
    if (NULL != specific) {
        CTSpecificDeinit(specific);
        pthread_setspecific(CTKeyShared(), NULL);
    }
}
void __CTShareOnceBlockFunc(void) {
    int result = pthread_key_create(&__CTKeyShared, __CTKeySharedValueDealloc);
    assert(result == 0);
}
void CTShare(void) {
    static pthread_once_t token = PTHREAD_ONCE_INIT;
    pthread_once(&token,&__CTShareOnceBlockFunc);
}
