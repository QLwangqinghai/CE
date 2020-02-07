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

LSManager_s * _Nonnull LSManagerCreate(uint32_t domain, uint32_t handlerCount) {
    
    LSManager_s * manager = CCAllocate(sizeof(LSManager_s) + sizeof(LSFileHandler_s) * handlerCount);
    CEPollPtr poll = CEPollShared();

    for (uint32_t index=0; index<handlerCount; index++) {
        LSFileHandler_s * handler = LSManagerGetHandler(manager, index);
        uint32_t handlerId = CEPollRegisterHandler(poll, handler, _LSFileHandlerFileEventCallback);
        assert(handlerId != CEFileHandlerInvalid);
        LSFileHandlerInit(handler, handlerId, manager);
    }

    return manager;
}







#pragma mark - shared

static LSManager_s * _Nonnull __LSManagerShared = NULL;
void LSManagerSharedCreate(void) {
    uint32_t domain = 0;
    uint32_t handlerCount = 1;
    __LSManagerShared = LSManagerCreate(domain, handlerCount);
}
LSManager_s * _Nonnull LSManagerShared(void) {
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, LSManagerSharedCreate);
    return __LSManagerShared;
}
