//
//  AppDelegate.m
//  TestC
//
//  Created by vector on 2018/12/7.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#import "AppDelegate.h"
#include <TargetConditionals.h>
#include <stdatomic.h>
#include <limits.h>
#include <stdlib.h>
#include <malloc/malloc.h>
#include <pthread.h>

#include <sys/socket.h>


//#define CO_DECL(name) \
//\
//DISPATCH_EXPORT const struct dispatch_source_type_s \
//_dispatch_source_type_##name
//#endif
//
//
//extern COStringClass_c
//
//
////#define Fast32Size sizeof(_Atomic(uint_fast32_t))
////#define Fast64Size sizeof(_Atomic(uint_fast64_t))
////
////#if Fast32Size != sizeof(uint32_t)
////
////#error "Fast32Size != sizeof(uint32_t)"
////
////#
////#if Fast64Size != sizeof(uint32_t)
////
////#error "Fast64Size != sizeof(uint32_t)"
////
////#
//
//static atomic_uint_fast32_t a = ATOMIC_VAR_INIT(0);

#include <Availability.h>
#include <os/availability.h>
#include <TargetConditionals.h>



@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate


static _Atomic(uint_fast64_t) svalue = ATOMIC_VAR_INIT(0);

static _Atomic(uintptr_t) spvalue = ATOMIC_VAR_INIT((uintptr_t)(NULL));

static _Atomic(uint_fast64_t) allocValue = ATOMIC_VAR_INIT(0);
static _Atomic(uint_fast64_t) freeValue = ATOMIC_VAR_INIT(0);


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application

    void * ptr = malloc(sizeof(uint_fast32_t));
    
    
    size_t size = malloc_size(ptr);
    
    
    NSLog(@"%@", @(size));
    
    ptr = realloc(NULL, 12);
    
//    int so = socket(PF_INET6,SOCK_STREAM, PF_INET6);
    int so = socket(PF_INET, SOCK_STREAM, 0);

    NSLog(@"%d", so);

    struct {
        _Atomic(uint_fast64_t) class;
        _Atomic(uint_fast64_t) active;
    } obj;
//_COMM_PAGE_LOGICAL_CPUS
    
//    valloc(<#size_t#>)
    PAGE_SIZE
    
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        while (1) {
            uintptr_t value = atomic_load(&(spvalue));
            
            uint64_t value64 = atomic_load(&(svalue));
            value64 += 1;
            atomic_store(&(svalue), value64);
            uint64_t * ptr = malloc(sizeof(uint64_t));
            assert(ptr);
            *ptr = value64;
            uintptr_t nValue = (uintptr_t)ptr;

            
            uint64_t allocValue64 = atomic_load(&(allocValue));
            allocValue64 += 1;
            atomic_store(&(allocValue), allocValue64);
            
            
            while (1) {
                value = atomic_load(&(spvalue));
                if ((uintptr_t)NULL != value) {
                    continue;
                }
                if ((uintptr_t)NULL == nValue) {
                    abort();
                }
                if (atomic_compare_exchange_weak(&(spvalue), &value, nValue)) {
                    break;
                }
            }
            

        }
    });
    
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        uint64_t lastValue64 = 0;

        while (1) {
            uintptr_t value = atomic_load(&(spvalue));
            uintptr_t oldValue = value;
            uintptr_t nValue = (uintptr_t)NULL;

            uint64_t loopTimes = 0;
            
            
            while (1) {
                loopTimes += 1;
                value = atomic_load(&(spvalue));
                if ((uintptr_t)NULL == value) {
                    continue;
                }
                if (0 == value) {
                    continue;
                }
                oldValue = value;
                if ((uintptr_t)NULL == oldValue) {
                    abort();
                }
                if (atomic_compare_exchange_weak(&(spvalue), &value, nValue)) {
                    break;
                }
            }
            

            assert(oldValue);
            assert(value);
            uint64_t * ptr = (uint64_t *)value;
            if (lastValue64 > *ptr) {
                abort();
            } else {
                assert(lastValue64 +1 == *ptr);
                lastValue64 = *ptr;
            }
            free(ptr);
            
            uint64_t freeValue64 = atomic_load(&(freeValue));
            freeValue64 += 1;
            atomic_store(&(freeValue), freeValue64);
        }
    });

    
    _PTHREAD_PRIORITY_EVENT_MANAGER_FLAG
    dispatch_source_t timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, <#dispatchQueue#>);
    dispatch_source_set_timer(timer, DISPATCH_TIME_NOW, <#intervalInSeconds#> * NSEC_PER_SEC, <#leewayInSeconds#> * NSEC_PER_SEC);
    dispatch_source_set_event_handler(timer, ^{
        <#code to be executed when timer fires#>
    });
    dispatch_resume(timer);
    dispatch_source_create(<#dispatch_source_type_t  _Nonnull type#>, <#uintptr_t handle#>, <#unsigned long mask#>, <#dispatch_queue_t  _Nullable queue#>)
    dispatch_source_set_event_handler(timer, ^{
        <#code to be executed when timer fires#>
    });
    dispatch_resume(<#dispatch_object_t  _Nonnull object#>)
    
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
