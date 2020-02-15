//
//  LSManager.c
//  LinkServer
//
//  Created by vector on 2020/2/6.
//  Copyright © 2020 vector. All rights reserved.
//

#include "LSManager.h"
#include "LSFileHandler.h"
#include "LSEventLoop.h"
#include <pthread.h>
#include <semaphore.h>

LSEventLoop_s * _Nonnull LSManagerGetEventLoop(LSManager_s * _Nonnull manager, uint32_t index) {
    assert(index<manager->loopCount);
    return manager->loops[index];
}
LSManager_s * _Nonnull LSManagerCreate(uint32_t domain, uint32_t loopCount, uint32_t maxConnectionCount) {
    assert(loopCount <= 128);
    assert(loopCount > 0);
    LSManager_s * manager = CCAllocate(sizeof(LSManager_s) + sizeof(LSEventLoop_s *) * loopCount);
    manager->loopCount = loopCount;
    struct rlimit limit;
    if(getrlimit(RLIMIT_NOFILE, &limit) != 0) {
        CCLogError("getrlimit RLIMIT_NOFILE error %s; \n", strerror(errno));
        abort();
    }
    printf("getrlimit RLIMIT_NOFILE max: %llu, current: %llu\n", limit.rlim_max, limit.rlim_cur);
    uint32_t fileTableSize = limit.rlim_max < UINT32_MAX ? (uint32_t)(limit.rlim_max) : UINT32_MAX;

    manager->fileTable = CCAllocate(sizeof(LSFile_s) * fileTableSize);
    manager->fileTableSize = fileTableSize;
    
    uint32_t p = (8192 * loopCount);
    uint32_t groupCount = maxConnectionCount / p;
    if (maxConnectionCount % p != 0) {
        groupCount += 1;
    }
    for (uint32_t index=0; index<loopCount; index++) {
        manager->loops[index] = LSEventLoopCreate(index, groupCount);
    }
    return manager;
}

#pragma mark - shared

static LSManager_s * _Nonnull __LSManagerShared = NULL;
void LSManagerSharedCreate(void) {
    uint32_t domain = 0;
    uint32_t loopCount = CCActiveProcessorCount();
    uint32_t maxCount = 123;
    __LSManagerShared = LSManagerCreate(domain, loopCount, maxCount);
}
LSManager_s * _Nonnull LSManagerShared(void) {
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, LSManagerSharedCreate);
    return __LSManagerShared;
}
