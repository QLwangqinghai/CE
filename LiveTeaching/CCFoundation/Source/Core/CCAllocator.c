//
//  CCAllocator.c
//  SITP
//
//  Created by vector on 2019/12/24.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCAllocator.h"

//mac

#include <sys/sysctl.h>
size_t CCGetCachelineSize(void) {
    int name[] = {CTL_HW,HW_CACHELINE};
    size_t lineSize = 0;
    size_t sizeOfLineSize = sizeof(lineSize);

    int code = sysctl(name, 2, &lineSize, &sizeOfLineSize, NULL, 0); // getting size of answer
    if (0 == code) {
        return (int32_t)lineSize;
    } else {
        return 64;
    }
}
