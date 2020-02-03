//
//  CEMemory.h
//  CoreEvent
//
//  Created by vector on 2018/11/23.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CEMemory_h
#define CEMemory_h

#include <stdio.h>



void * _Nonnull CEAllocate(size_t size);
void * _Nonnull CEAllocateClear(size_t size);
void * _Nonnull CEReallocate(void * _Nonnull ptr, size_t size);
void CEDeallocate(void * _Nonnull ptr);


void CEMemoryManagerUpdateAllocate(void * _Nullable (* _Nonnull func)(size_t size));
void CEMemoryManagerUpdateAllocateClear(void * _Nullable (* _Nonnull func)(size_t size));
void CEMemoryManagerUpdateReallocate(void * _Nullable (* _Nonnull func)(void * _Nonnull ptr, size_t size));
void CEMemoryManagerUpdateDeallocate(void (* _Nonnull func)(void * _Nonnull ptr));






#endif /* CEMemory_h */
