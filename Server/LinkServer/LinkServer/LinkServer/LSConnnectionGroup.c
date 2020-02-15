//
//  LSConnnectionGroup.c
//  LinkServer
//
//  Created by vector on 2020/2/13.
//  Copyright © 2020 vector. All rights reserved.
//

#include "LSConnnectionGroup.h"

/*
 struct _LSConnnectionGroup {
     uint32_t groupId;
     uint32_t triggerCount;
     uint32_t activeCount;
     LSConnnectionGroupStorage_s * _Nonnull storage;
 };
 typedef struct {
     uint32_t trigger[4096];
     LSConnection_s table[4096];
     LSConnectionTimeSource_s writeSourceQueue[4096];
     LSConnectionTimeSource_s readSourceQueue[4096];
 } LSConnnectionGroupStorage_s;
 */

void LSConnnectionGroupInit(LSConnectionGroup_s * _Nonnull group, uint32_t groupId) {
    assert(group);
    group->groupId = groupId;
    group->activeCount = 0;
    group->triggerCount = 0;
    group->storage = CCAlignedAllocate(sizeof(LSConnnectionGroupStorage_s), 128);
}

void _LSConnnectionGroupSaveEvent(LSConnectionGroup_s * _Nonnull group, LSEvent_s event) {
    LSConnection_s * connection = &(group->storage->table[event.indexInGroup]);
    if (connection->hasError != 0 && connection->status == CEFileEventMaskNone) {
        group->storage->trigger[group->triggerCount] = event.indexInGroup;
        group->triggerCount += 1;
    }
    connection->hasError = connection->hasError | event.error;
    connection->status = connection->status | event.status;
    assert(group->triggerCount < 0x1000);
}
void _LSConnnectionGroupClearEvents(LSConnectionGroup_s * _Nonnull group) {
    group->triggerCount = 0;
}
void _LSConnnectionGroupPerformEvents(LSConnectionGroup_s * _Nonnull group, uint64_t time) {
    uint32_t count = group->triggerCount;
    for (uint32_t index=0; index<count; index++) {
        LSConnection_s * connection = &(group->storage->table[index]);
        if (0 != connection->hasError) {
            //read and write, check status
        } else {
            
            
        }
    }
}

void _LSConnnectionGroupCheckTimeout(LSConnectionGroup_s * _Nonnull group, uint64_t currentTime) {

}
