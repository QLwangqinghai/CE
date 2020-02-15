//
//  LSConnnectionGroup.h
//  LinkServer
//
//  Created by vector on 2020/2/13.
//  Copyright © 2020 vector. All rights reserved.
//

#ifndef LSConnnectionGroup_h
#define LSConnnectionGroup_h

#include "LSTypes.h"

void LSConnnectionGroupInit(LSConnectionGroup_s * _Nonnull group, uint32_t groupId);

void _LSConnnectionGroupClearEvents(LSConnectionGroup_s * _Nonnull group);
void _LSConnnectionGroupSaveEvent(LSConnectionGroup_s * _Nonnull group, LSEvent_s event);
void _LSConnnectionGroupPerformEvents(LSConnectionGroup_s * _Nonnull group, uint64_t time);

void _LSConnnectionGroupCheckTimeout(LSConnectionGroup_s * _Nonnull group, uint64_t currentTime);


#endif /* LSConnnectionGroup_h */
