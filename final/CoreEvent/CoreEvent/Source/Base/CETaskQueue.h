//
//  CETaskQueue.h
//  CoreEvent
//
//  Created by vector on 2019/5/6.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#ifndef CETaskQueue_h
#define CETaskQueue_h

#include "CETask.h"

struct _CETaskQueue;
typedef struct _CETaskQueue CETaskQueue_s;
typedef CETaskQueue_s * CETaskQueueRef;

typedef struct _CETaskQueuePage {
    _Atomic(uintptr_t) buffer[1024];
} CETaskQueuePage_s;

/*
 

 
 */

/*
 api
 
 resize
 
 
 */

#if CEBuild64Bit



struct _CETaskQueue {
    _Atomic(uint_fast64_t) refCount;//引用计数器
    
};
#else
struct _CETaskQueue {
    _Atomic(uint_fast32_t) refCount;//引用计数器
};
#endif




#endif /* CETaskQueue_h */
