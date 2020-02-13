//
//  LSConnnectionGroup.c
//  LinkServer
//
//  Created by vector on 2020/2/13.
//  Copyright © 2020 vector. All rights reserved.
//

#include "LSConnnectionGroup.h"


void LSConnnectionGroupInit(LSConnectionGroup_s * _Nonnull group, uint32_t groupId) {
    assert(group);
    group->groupId = groupId;
    group->activeCount = 0;
    group->storage = CCAlignedAllocate(sizeof(LSConnnectionGroupStorage_s), 128);
}
