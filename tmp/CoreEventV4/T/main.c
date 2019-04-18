//
//  main.c
//  T
//
//  Created by 王青海 on 2018/11/21.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>



#define CETimeEventLeewayMax 0x1FFFFull


//typedef struct _CETimeEventPtrPage {
//    CETimeEvent_s * _Nonnull ptrs[57344];
//    uint64_t item1[7168];
//    uint64_t item2[896];
//    uint64_t item3[112];
//    uint64_t item4[14];
//    uint64_t unused;
//    uint64_t unusedCount;
//} CETimeEventPtrPage_s;



#define CETimeEventIdMax 0xFFFEul


/* Time event structure */
typedef struct _CETimeEvent {
    uint32_t isStatic: 1;
    uint64_t repeat: 1;
    uint64_t repeatMode: 1;
    uint64_t running: 1;
    uint64_t states: 2;//必须为0
    uint64_t leeway: 16;//偏差 单位 微妙 最大值 0x1FFFF
    uint64_t interval: 42;//间隔时间 单位微妙  最大 1221小时 (50天)
    void * _Nonnull execute;
    void * _Nullable clearContext;
    void * _Nullable context;
    uint64_t when; /* microseconds 微妙 什么时候触发 */
    uint32_t tag;
    uint32_t prev: 16;
    uint32_t next: 16;
} CETimeEvent_s;

static inline uint64_t CETimeEventGetLeeway(CETimeEvent_s * _Nonnull event) {
    return event->leeway * 2;
}

static inline void CETimeEventSetLeeway(CETimeEvent_s * _Nonnull event, uint64_t leeway) {
    if (leeway > CETimeEventLeewayMax) {
        leeway = 0xFFFFull;
    } else if (leeway <= 2) {
        leeway = 1;
    } else {
        leeway = leeway / 2;
    }
    event->leeway = leeway;
}





typedef struct _CETimeEventPage {
    CETimeEvent_s event[4096];
    uint64_t n1[64];
    uint64_t nroot;
    uint64_t unused;
    uint64_t unusedCount;
} CETimeEventPage_s;

typedef struct _CETimeEventPtrPage {
    CETimeEvent_s * _Nonnull ptrs[4096];
    uint64_t n1[64];
    uint64_t nroot;
    uint64_t unusedCount;
} CETimeEventPtrPage_s;


typedef struct _CETimeEventManager {
    CETimeEventPage_s basePage;
    
    CETimeEventPtrPage_s * _Nullable ptrPages[15];
    uint64_t size;
    uint64_t unused;
    uint32_t sequence;

    
} CETimeEventManager_s;

static inline CETimeEvent_s * _Nullable CETimeEventManagerGetEventById(CETimeEventManager_s * _Nonnull manager, uint32_t id) {
    if (id > CETimeEventIdMax) {
        return NULL;
    }
    
    if (id < 4096) {
        uint64_t t = manager->basePage.n1[id >> 6];
        uint64_t mask = 0x8000000000000000ull;
        uint64_t offset = id & 0x3F;
        mask = mask >> offset;
        
        if (t & mask) {
            return &(manager->basePage.event[id]);
        } else {
            return NULL;
        }
    } else {
        uint32_t offset = id - 4096;
        uint32_t ptrPageIndex = offset >> 12;
        CETimeEventPtrPage_s * ptrPage = manager->ptrPages[ptrPageIndex];
        if (NULL == ptrPage) {
            return NULL;
        }
        
        uint32_t index = offset & 0xfff;
        return ptrPage->ptrs[index];
    }
    return NULL;
}


//uintptr_t CETimeEventSourceInit(CERunLoop_s * _Nonnull eventLoop, uint64_t leeway, _Bool repeat, _Bool repeatModeAfter, uint64_t interval, CETimeEventHandler_f _Nonnull execute, void * _Nullable context, CETimeEventClearContextHandler_f _Nullable clearContext, CEResult_t * _Nonnull result) {
//    
//    
//    
//    
//    
//}
//
//


/*
 CETimeEvent_s: 48
 CETimeEventPtrPage_s: 37456
 CETimeEventPage_s: 402576
 CETimeEventManager_s: 402712
 
 
 CETimeEvent_s: 48
 CETimeEventPtrPage_s: 33296
 CETimeEventPage_s: 197144
 CETimeEventManager_s: 197288
 */


int main(int argc, const char * argv[]) {
    // insert code here...
    
    printf("CETimeEvent_s: %llu\n", sizeof(CETimeEvent_s));

    printf("CETimeEventPtrPage_s: %llu\n", sizeof(CETimeEventPtrPage_s));
    printf("CETimeEventPage_s: %llu\n", sizeof(CETimeEventPage_s));
    printf("CETimeEventManager_s: %llu\n", sizeof(CETimeEventManager_s));

    

    uint64_t mask = 0x8000000000000000ull;
    
    for (uint64_t i=0; i<64; i++) {
        uint64_t m = mask >> i;
        printf("m %llX\n", m);
    }
    
//    uint64_t offset = id & 0x3F;
//    mask = mask >> offset;
//
//    if (t & mask) {
//
//        printf("0");
//    } else {
//        printf("1");
//    }

    printf("\n");

    
    pthread_t tid;
    pthread_attr_t attr;
    struct sched_param param;
    //初始化线程属性
    pthread_attr_init(&attr);
    int policy;
    int result = 0;
    
    result = pthread_getschedparam(pthread_self(), &policy, &param);
    if (0 != result) {
        printf("pthread_getschedparam %s\n", strerror(result));
        return result;
    }
    result = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    if (0 != result) {
        printf("pthread_attr_setscope PTHREAD_SCOPE_SYSTEM %s\n", strerror(result));
        return result;
    }
    result = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (0 != result) {
        printf("pthread_attr_setdetachstate PTHREAD_CREATE_DETACHED %s\n", strerror(result));
        return result;
    }
    
    size_t stacksize = 0;
    result = pthread_attr_getstacksize(&attr, &stacksize);
    
    if (0 != result) {
        printf("pthread_attr_getstacksize error %s\n", strerror(result));
        return result;
    }
    
    size_t rStacksize = 1024 * 1024 * 2;

    if (stacksize < rStacksize) {
        result = pthread_attr_setstacksize(&attr, rStacksize);//设置线程堆栈大小
        if (0 != result) {
            printf("pthread_attr_setstacksize error %s\n", strerror(result));
            return result;
        }
    }
    
    return 0;
}


//int main(int argc, const char * argv[]) {
//    // insert code here...
//    printf("Hello, World!\n");
//
//    struct timespec time1 = {0, 0};
//    clock_gettime(CLOCK_REALTIME, &time1);
//    printf("CLOCK_REALTIME: %lu, %lu\n", time1.tv_sec, time1.tv_nsec);
//
//    clock_gettime(CLOCK_MONOTONIC, &time1);
//    printf("CLOCK_MONOTONIC: %lu, %lu\n", time1.tv_sec, time1.tv_nsec);
//
//    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
//    printf("CLOCK_PROCESS_CPUTIME_ID: %lu, %lu\n", time1.tv_sec, time1.tv_nsec);
//
//    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time1);
//    printf("CLOCK_THREAD_CPUTIME_ID: %lu, %lu\n", time1.tv_sec, time1.tv_nsec);
//
//    printf("\n%lu\n", time(NULL));
//    sleep(5);
//
//
//    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
//    printf("CLOCK_PROCESS_CPUTIME_ID: %lu, %lu\n", time1.tv_sec, time1.tv_nsec);
//
//    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time1);
//    printf("CLOCK_THREAD_CPUTIME_ID: %lu, %lu\n", time1.tv_sec, time1.tv_nsec);
//
//
//
//    return 0;
//}
