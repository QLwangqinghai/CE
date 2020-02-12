//
//  LSManager.c
//  LinkServer
//
//  Created by vector on 2020/2/6.
//  Copyright © 2020 vector. All rights reserved.
//

#include "LSManager.h"
#include "LSFileHandler.h"
#import <CoreEvent/CoreEvent.h>

extern void _LSFileHandlerFileEventCallback(CCPtr _Nullable context, CEFileFiredInfo * _Nonnull infos, uint32_t count);

LSFileHandler_s * _Nonnull LSManagerGetHandler(LSManager_s * _Nonnull manager, uint32_t index) {
    return &(manager->handlers[index]);
}

LSEventLoop_s * _Nonnull LSManagerGetEventLoop(LSManager_s * _Nonnull manager, uint32_t index) {
    struct rlimit limit;
    if(getrlimit(RLIMIT_NOFILE, &limit) != 0) {
        CELogError("getrlimit RLIMIT_NOFILE error %s; \n", strerror(errno))
        abort();
    }
    CELogVerbose("getrlimit RLIMIT_NOFILE max: %llu, current: %llu\n", limit.rlim_max, limit.rlim_cur)
    uint32_t fileTableSize = limit.rlim_max < UINT32_MAX ? limit.rlim_max : UINT32_MAX;
    
    LSManager_s * manager = CCAllocate(sizeof(LSManager_s));
    manager->handlers = CCAllocate(sizeof(LSFileHandler_s) * handlerCount);
    manager->handlerCount = handlerCount;
    
    manager->fileTable = CCAllocate(sizeof(LSFile_s) * fileTableSize);
    manager->fileTableSize = fileTableSize;
    
    CEPollPtr poll = CEPollShared();

    for (uint32_t index=0; index<handlerCount; index++) {
        LSFileHandler_s * handler = LSManagerGetHandler(manager, index);
        uint32_t handlerId = CEPollRegisterHandler(poll, handler, _LSFileHandlerFileEventCallback);
        assert(handlerId != CEFileHandlerInvalid);
        LSFileHandlerInit(handler, handlerId, manager);
    }

    return manager;
}

LSManager_s * _Nonnull LSManagerCreate(uint32_t domain, uint32_t loopCount, uint32_t maxConnectionCount) {
    LSManager_s * manager = CCAllocate(sizeof(LSManager_s));
    manager->loops = CCAllocate(sizeof(LSEventLoop_s) * loopCount);
    manager->loopCount = loopCount;
    
    struct rlimit limit;
    if(getrlimit(RLIMIT_NOFILE, &limit) != 0) {
        CELogError("getrlimit RLIMIT_NOFILE error %s; \n", strerror(errno))
        abort();
    }
    CELogVerbose("getrlimit RLIMIT_NOFILE max: %llu, current: %llu\n", limit.rlim_max, limit.rlim_cur)
    uint32_t fileTableSize = limit.rlim_max < UINT32_MAX ? limit.rlim_max : UINT32_MAX;

    manager->fileTable = CCAllocate(sizeof(LSFile_s) * fileTableSize);
    manager->fileTableSize = fileTableSize;
    
    
    
}

#pragma mark - shared

static LSManager_s * _Nonnull __LSManagerShared = NULL;
void LSManagerSharedCreate(void) {
    uint32_t domain = 0;
    uint32_t eventLoopCount = CCActiveProcessorCount();
    uint32_t maxCount = 123;
    __LSManagerShared = LSManagerCreate(domain, handlerCount, maxCount);
}
LSManager_s * _Nonnull LSManagerShared(void) {
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, LSManagerSharedCreate);
    return __LSManagerShared;
}
