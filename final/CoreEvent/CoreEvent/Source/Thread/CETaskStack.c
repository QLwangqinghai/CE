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








