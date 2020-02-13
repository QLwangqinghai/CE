//
//  LSEventLoop.c
//  LinkServer
//
//  Created by vector on 2020/2/12.
//  Copyright © 2020 vector. All rights reserved.
//

#include "LSEventLoop.h"
#include "LSConnnectionGroup.h"



LSEventLoop_s * _Nonnull LSEventLoopCreate(uint32_t id, uint32_t groupCount) {
    assert(groupCount > 0);
    uint32_t connectionCapacity = groupCount * 4096;
    LSEventLoop_s * loop = CCAllocate(sizeof(LSEventLoop_s) + sizeof(LSConnectionGroup_s) * groupCount);
    loop->connectionCapacity = connectionCapacity;
    loop->groupCount = groupCount;
    loop->id = id;
    for (uint32_t i=0; i<groupCount; i++) {
        LSConnnectionGroupInit(&(loop->groups[i]), id);
    }
    return loop;
}
