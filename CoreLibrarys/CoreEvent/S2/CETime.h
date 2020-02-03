//
//  CETime.h
//  CoreEvent
//
//  Created by vector on 2018/12/14.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CETime_h
#define CETime_h

#include <CoreSystem/CoreSystem.h>

static inline uint64_t CESystemBootInterval(void) {
    struct timespec ts = CSBootInterval();
    return ts.tv_nsec / 1000ull + ts.tv_sec * 1000000ull;
}


static inline uint64_t CEGetMicrosecondsTime(void) {
    return CESystemBootInterval();
}


#endif /* CETime_h */
