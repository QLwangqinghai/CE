//
//  CETaskStack.c
//  CoreEvent
//
//  Created by vector on 2019/5/24.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CETaskStack.h"
#include "CEMemory.h"


struct _CETaskStackPage;
typedef struct _CETaskStackPage CETaskStackPage_s;
typedef CETaskStackPage_s * CETaskStackPagePtr;


typedef uint16_t CETaskStackItemType_e;
static CETaskStackItemType_e CETaskStackItemTypeFlag = 0;
static CETaskStackItemType_e CETaskStackItemTypeStackResult = 1;
static CETaskStackItemType_e CETaskStackItemTypeStackParam = 2;
static CETaskStackItemType_e CETaskStackItemTypeTaskContext = 3;
static CETaskStackItemType_e CETaskStackItemTypeTask = 4;



typedef struct _CETaskStackItem {
    uint16_t type;
    uint16_t length;
    uint32_t location;
    //... load
} CETaskStackItem_s;
typedef CETaskStackItem_s * CETaskStackItemPtr;

struct _CETaskStackPage {
    CETaskStackPagePtr _Nonnull prev;
    uint32_t loadSize;
    uint32_t itemCount;
    CETaskStackItemPtr _Nonnull items[2048];
    //... load
};

struct _CETaskStack {
    CEThreadRef _Nonnull thread;
    CETaskStackPagePtr _Nonnull top;
};


CETaskStackPagePtr _Nonnull CETaskStackPageCreate(void) {
    size_t size = 0x8000;
    CETaskStackPagePtr result = CEAllocate(size);
    result->prev = NULL;
    result->loadSize = (uint32_t)(size - sizeof(CETaskStackPage_s));
    result->itemCount = 0;
    return result;
}

void CETaskStackPageDestroy(CETaskStackPagePtr _Nonnull pagePtr) {
    CEDeallocate(pagePtr);
}

CETaskStackPtr _Nonnull CETaskStackCreate(void) {
    CETaskStackPtr result = CEAllocate(sizeof(CETaskStack_s));
 
    return result;
}

void CETaskStackDestroy(CETaskStackPtr _Nonnull pagePtr) {
    CEDeallocate(pagePtr);
}

CETaskStackPtr _Nonnull CETaskStackGetCurrent(void) {
    CEThreadSpecificRef specific = CEThreadSpecificGetCurrent();
    CETaskStackPtr result = specific->taskStack;
    if (NULL == result) {
        result = CETaskStackCreate();
        specific->taskStack = result;
    }
    return result;
}

void CETaskStackPush(CETaskStackPtr _Nonnull stackPtr) {
    assert(stackPtr);

}

void CETaskStackPop(CETaskStackPtr _Nonnull stackPtr) {
    assert(stackPtr);
    
}


/*
 param
 result
 task
 context
 */


CEStackParamRef _Nullable CETaskStackPopParam(CETaskStackPtr _Nonnull stackPtr, uint32_t capacity, size_t bufferItemsTotalSize) {
    assert(stackPtr);
    
//    assert(bufferItemsTotalSize <= CEParamBufferItemsTotalSizeMax);
//
//    if (0 == bufferItemsTotalSize) {
//        bufferItemsTotalSize = 64;
//    }
//
//    size_t size = sizeof(CEHeapParam_s) + (sizeof(CEParamItem_s) + sizeof(void *) + sizeof(double)) * capacity + bufferItemsTotalSize;
//    size = (size + sizeof(void *) - 1) / sizeof(void *) * sizeof(void *);
//
//    CEHeapParamRef ptr = CETypeHeapParam->alloctor->allocate(CETypeHeapParam, size);
//
//    CEHeapParam_s * result = ptr;
//    result->base.capacity = capacity;
//
//    size_t contentSize = size - sizeof(CEStackParam_s);
//    if (contentSize > 0xFFFFFFu) {
//        result->base.contentSize = 0xFFFFFFu;
//    } else {
//        result->base.contentSize = (uint32_t)contentSize;
//    }
//    result->base.contentUsedSize = capacity * sizeof(CEParamItem_s);
    return NULL;
    
}
CEStackParamRef _Nullable CEStackParamInit(void * _Nonnull ptr, size_t size, uint32_t capacity);
CEHeapParamRef _Nonnull CEHeapParamCreate(uint32_t capacity, size_t bufferItemsTotalSize);





uint32_t CEParamGetCount(CEParamRef _Nonnull param);
_Bool CEParamGetItemType(CEParamRef _Nonnull param, uint32_t index, CEParamType_e * _Nonnull itemType);






