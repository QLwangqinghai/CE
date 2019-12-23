//
//  CCBase.h
//  CCollection
//
//  Created by vector on 2019/6/26.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCBase.h"
#include <pthread.h>


const CCIndex CCIndexNotFound = CCDefineIndexNotFound;

//count 的最大值
const uint32_t CCCountLimit = 0x20000000uL;
const uint32_t CCElementSizeLimit = CCDefineElementSizeLimit;//2KB
const uint32_t CCCircularBufferElementsSizeLimit = 0x80000000uL;

const CCType CCTypeArray = 0x1;
const CCType CCTypeMutableArray = 0x1 | CCTypeMutableMask;




#pragma mark - thread key-specific

static size_t __CCPageSizes[8] = {};

#if !defined(PAGE_SIZE)
#define PAGE_SIZE 4096
#endif

//void __CCPageSizesOnceBlockFunc(void) {
//    size_t pageSize = 4096;
//#if defined(PAGE_SIZE)
//    if (PAGE_SIZE > pageSize) {
//        pageSize = PAGE_SIZE;
//    }
//#endif
//}
//size_t * _Nonnull CESharedKey(void) {
//    static pthread_once_t token = PTHREAD_ONCE_INIT;
//    pthread_once(&token,&__CCPageSizesOnceBlockFunc);
//    assert(__CCPageSizes[0] > 0);
//    return __CCPageSizes;
//}
//
//CERunLoop_s * _Nullable CEGetCurrentLoop(void) {
//    return (CERunLoop_s *)pthread_getspecific(CESharedKey());
//
//}
//void CESetCurrentLoop(CERunLoop_s * _Nullable loop) {
//    CERunLoop_s * old = CEGetCurrentLoop();
//    if (NULL != old && NULL != loop & old != loop) {
//        fprintf(stderr, "SetCurrentLoop error\n");
//        fflush(stderr);
//        abort();
//    }
//    int result = pthread_setspecific(CESharedKey(), loop);
//    assert(result == 0);
//}

