//
//  CETask.c
//  CoreEvent
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#include "CETask.h"
#include "CEMemory.h"


void __CEEscapingTaskRelease(CEEscapingTaskRef _Nonnull task);



struct _CETask {
    CETaskBase_t base;
    
    CEParamType_e paramTypes[CETaskParamItemBufferSize];
    CEParamItemValue_u paramItems[CETaskParamItemBufferSize];
    CETaskParamItemRelease_f _Nullable paramReleases[CETaskParamItemBufferSize];
};

typedef struct _CETaskType {
    CETaskExecute_f _Nonnull execute;
} CETaskType_s;






_Bool CENoescapingTaskInit(CENoescapingTaskRef _Nonnull task,
                           CETaskExecute_f _Nonnull execute,
                           CETaskExecuteObserverRef _Nullable observer,
                           uint32_t itemCount) {
    assert(task);
    assert(execute);

    if (itemCount > CETaskParamItemMaxCount) {
        return false;
    }
    CETaskBase_t base = {};
    base.paramItemCount = itemCount;
    base.release = NULL;
    base.execute = execute;
    base.observer = observer;
    base.type = CETaskTypeSync;

    memcpy(&(task->base), &base, sizeof(CETaskBase_t));

    return true;
}



CEEscapingTaskRef _Nonnull CEEscapingTaskAlloc(void) {
    return CEAllocateClear(sizeof(CENoescapingTask_s));
}
void CEEscapingTaskDealloc(CEEscapingTaskRef _Nonnull task) {
    assert(task);
    
    for (uint32_t index=0; index<task->base.paramItemCount; index++) {
        CETaskParamItemRelease_f release = task->paramReleases[index];
        
        if (release) {
            CEParamType_e type = task->paramTypes[index];
            CEParamItemValue_u value = task->paramItems[index];
            release(type, value);
        }
    }
    
    CEDeallocate(task);
}

CEEscapingTaskRef _Nullable CEEscapingTaskInit(CETaskExecute_f _Nonnull execute,
                                               CETaskExecuteObserverRef _Nullable observer,
                                               uint32_t itemCount) {
    assert(execute);
    
    if (itemCount > CETaskParamItemMaxCount) {
        return NULL;
    }
    
    CEEscapingTaskRef task = CEEscapingTaskAlloc();
    CETaskBase_t base = {};
    base.type = CETaskTypeAsync;
    base.paramItemCount = itemCount;
    base.release = __CEEscapingTaskRelease;
    base.execute = execute;
    base.observer = observer;
    
    memcpy(&(task->base), &base, sizeof(CETaskBase_t));
    return task;
}

_Bool CEEscapingTaskSetParamItem(CEEscapingTaskRef _Nonnull task,
                                 uint32_t index,
                                 CEParamType_e type,
                                 CETaskParamItemRelease_f _Nullable release,
                                 void * _Nonnull valuePtr) {
    assert(task);
    assert(valuePtr);
    assert(task->base.type == 1);
    if (type > CEParamTypeMax) {
        return false;
    }
    if (index >= task->base.paramItemCount) {
        return false;
    }
    
    CEParamItemValue_u value = {};
    
    switch (type) {
        case CEParamTypePtr: {
            void ** ptr = valuePtr;
            value.ptrValue = *ptr;
        }
            break;
        case CEParamTypeBool: {
            _Bool * ptr = valuePtr;
            value.boolValue = *ptr;
        }
            break;
        case CEParamTypeSInt8: {
            int8_t * ptr = valuePtr;
            value.sint8Value = *ptr;
        }
            break;
        case CEParamTypeUInt8: {
            uint8_t * ptr = valuePtr;
            value.uint8Value = *ptr;
        }
            break;
        case CEParamTypeSInt16: {
            int16_t * ptr = valuePtr;
            value.sint16Value = *ptr;
        }
            break;
        case CEParamTypeUInt16: {
            uint16_t * ptr = valuePtr;
            value.uint16Value = *ptr;
        }
            break;
        case CEParamTypeSInt32: {
            int32_t * ptr = valuePtr;
            value.sint32Value = *ptr;
        }
            break;
        case CEParamTypeUInt32: {
            uint32_t * ptr = valuePtr;
            value.uint32Value = *ptr;
        }
            break;
        case CEParamTypeSInt64: {
            int64_t * ptr = valuePtr;
            value.sint64Value = *ptr;
        }
            break;
        case CEParamTypeUInt64: {
            uint64_t * ptr = valuePtr;
            value.uint64Value = *ptr;
        }
            break;
        case CEParamTypeFloat: {
            float * ptr = valuePtr;
            value.floatValue = *ptr;
        }
            break;
        case CEParamTypeDouble: {
            double * ptr = valuePtr;
            value.doubleValue = *ptr;
        }
            break;
            //        case CEParamTypeStruct: {
            //
            //        }
            //            break;
        default:
            break;
    }
    
    task->paramTypes[index] = type;
    task->paramItems[index] = value;
    task->paramReleases[index] = release;
    
    return true;
}

_Bool CEEscapingTaskGetParamItem(CEEscapingTaskRef _Nonnull task,
                                 uint32_t index,
                                 CEParamItemValue_u * _Nullable itemRef,
                                 CEParamType_e * _Nullable itemTypeRef) {
    assert(task);
    assert(task->base.type == 1);

    if (index >= task->base.paramItemCount) {
        return false;
    }
    
    if (NULL != itemRef) {
        *itemRef = task->paramItems[index];
    }
    if (NULL != itemTypeRef) {
        *itemTypeRef = task->paramTypes[index];
    }
    
    return true;
}

void __CEEscapingTaskRelease(CEEscapingTaskRef _Nonnull task) {
    assert(task);
    assert(task->base.type == 1);

    CEEscapingTaskDealloc(task);
}

void CEEscapingTaskRelease(CEEscapingTaskRef _Nonnull task) {
    task->base.release(task);
}

