//
//  CEInit.c
//  CoreEvent
//
//  Created by vector on 2020/2/2.
//  Copyright © 2020 vector. All rights reserved.
//

#include "CEBase.h"
#include "CETimeEvent.h"

#include <CCFoundation/CCReferenceBase.h>

__attribute__((constructor(102)))
static void __CEInit(void) {
    CERefTypeTimeEvent = CCDomainRefTypeTableRegister("CETimeEvent", _CETimeEventDeinit);
}
