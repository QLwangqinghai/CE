//
//  main.c
//  CSTimeTest
//
//  Created by vector on 2018/12/6.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include <stdio.h>


#include <stdlib.h>
#include <mach/mach_time.h>
#include <sys/time.h>
#include <assert.h>
#include <pthread.h>

static inline struct timespec CSMachTimeToTime(uint64_t t, mach_timebase_info_data_t timeBaseInfo) {
    struct timespec time = {};
    if (1 == timeBaseInfo.numer) {
        uint64_t mt = t;
        uint64_t nsec = mt / timeBaseInfo.denom;
        time.tv_sec = nsec / NSEC_PER_SEC;
        time.tv_nsec = nsec % NSEC_PER_SEC;
    } else {
        uint64_t h = t >> 32;
        uint64_t l = (t << 32) >> 32;
        
        h = h * timeBaseInfo.numer;
        l = l * timeBaseInfo.numer;
        
        uint64_t h1 = h / timeBaseInfo.denom;
        uint64_t rh = h % timeBaseInfo.denom;
        
        if (rh != 0) {
            uint64_t need = (timeBaseInfo.denom - rh) << 32;
            if (l >= need) {
                l -= need;
                h1 += 1;
            } else {
                l += rh << 32;
            }
        }
        
        uint64_t l1 = l / timeBaseInfo.denom;
        
        if (UINT64_MAX - l1 <= (h1 << 32)) {
            uint64_t nsec = (h1 << 32) + l1;
            time.tv_sec = nsec / NSEC_PER_SEC;
            time.tv_nsec = nsec % NSEC_PER_SEC;
        } else {
            uint64_t h2 = h1 / NSEC_PER_SEC;
            uint64_t rh1 = h1 % NSEC_PER_SEC;
            l1 += (rh1 << 32);
            time.tv_sec = (h2 << 32) + l1 / NSEC_PER_SEC;
            time.tv_nsec = l1 % NSEC_PER_SEC;
        }
    }
    return time;
}


struct timespec CSBootInterval(void) {
    
    mach_timebase_info_data_t timeBaseInfo = {};
    assert(0 == mach_timebase_info(&timeBaseInfo));
    uint64_t t = mach_continuous_time();
    
    return CSMachTimeToTime(t, timeBaseInfo);
}

void t0() {
    mach_timebase_info_data_t data = {};
    data.denom = 75497536;
    data.numer = 75497536;
    uint64_t t = 18446730488727992106ull;
    struct timespec time = CSMachTimeToTime(t, data);
    
    printf("s: %ld, ns: %ld\n", time.tv_sec, time.tv_nsec);
}

void t1() {
    mach_timebase_info_data_t data = {};
    data.denom = 4294967231;
    data.numer = 4294967231;
    uint64_t t = 18446730488727992106ull;
    struct timespec time = CSMachTimeToTime(t, data);
    
    printf("s: %ld, ns: %ld\n", time.tv_sec, time.tv_nsec);
}


void t2() {
    mach_timebase_info_data_t data = {};
    data.denom = 4294967231;
    data.numer = 1;
    uint64_t t = 18446730488727992106ull;
    struct timespec time = CSMachTimeToTime(t, data);
    
    printf("s: %ld, ns: %ld\n", time.tv_sec, time.tv_nsec);
}
void t3() {
    mach_timebase_info_data_t data = {};
    data.denom = 4294967231;
    data.numer = 4294967222;
    uint64_t t = 18446730488727992106ull;
    struct timespec time = CSMachTimeToTime(t, data);
    
    printf("s: %ld, ns: %ld\n", time.tv_sec, time.tv_nsec);
}
void t4() {
    mach_timebase_info_data_t data = {};
    data.denom = 1;
    data.numer = 10000000;
    uint64_t t = 488727992106ull;
    struct timespec time = CSMachTimeToTime(t, data);
    
    printf("s: %ld, ns: %ld\n", time.tv_sec, time.tv_nsec);
}
void t5() {
    mach_timebase_info_data_t data = {};
    data.denom = 3;
    data.numer = 67;
    uint64_t t = 485372548906ull;//- 485328358208 = 44190698
    struct timespec time = CSMachTimeToTime(t, data);
    
    printf("s: %ld, ns: %ld\n", time.tv_sec, time.tv_nsec);
}
int main(int argc, const char * argv[]) {

//    t0();
//    t1();
//    t2();

    t3();
    t4();
    t5();
    printf("long double size: %d\n", sizeof(long double));
    
    
    
    
    // insert code here...
    printf("Hello, World!\n");
    return 0;
}
