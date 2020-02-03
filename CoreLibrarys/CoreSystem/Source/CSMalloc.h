//
//  CSMalloc.h
//  CoreSystem
//
//  Created by vector on 2018/12/7.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CSMalloc_h
#define CSMalloc_h

#include <CoreSystem/CSBase.h>

CS_EXTERN_C_BEGIN

void * _Nullable CSMalloc(size_t size);

void * _Nullable CSRealloc(void * _Nullable ptr, size_t size);

size_t CSMallocSize(const void * _Nonnull ptr);

void CSFree(void * _Nullable ptr);


CS_EXTERN_C_END

#endif /* CSMalloc_h */
