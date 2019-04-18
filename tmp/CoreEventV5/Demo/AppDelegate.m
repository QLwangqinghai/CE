//
//  AppDelegate.m
//  Demo
//
//  Created by 王青海 on 2018/10/23.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#import "AppDelegate.h"
#import <CoreEvent/CoreEvent.h>


typedef void (^CERunLoopBlock_t)(CERunLoop_s * _Nonnull eventLoop);

void CERunLoopBlockExecuteTTT(CERunLoop_s * _Nonnull eventLoop, void * _Nullable params, size_t paramsSize, CEAtomicMemoryBlock_t * _Nullable resultReceiver, size_t resultSize) {
    assert(params);
    assert(paramsSize == sizeof(void *));
    void * * tmp = (void * *)params;
    CERunLoopBlock_t block = (__bridge CERunLoopBlock_t)(*tmp);
    block(eventLoop);
    uint8_t buffer[1024];
    for (uint32_t i=0; i<1024; i++) {
        buffer[i] = (uint8_t)i;
    }
    CEAtomicMemoryStore(resultReceiver, buffer, resultSize);
}


typedef void (^CERunLoopBlock_t)(CERunLoop_s * _Nonnull eventLoop);
typedef _Bool (^CERunLoopTimerBlock_t)(CERunLoop_s * _Nonnull eventLoop, CETimeDescription_s td, void * _Nullable context);


void CERunLoopBlockExecute(CERunLoop_s * _Nonnull eventLoop, void * _Nullable params, size_t paramsSize, CEAtomicMemoryBlock_t * _Nullable resultReceiver, size_t resultSize) {
    assert(params);
    assert(paramsSize == sizeof(void *));
    void * * tmp = (void * *)params;
    CERunLoopBlock_t block = (__bridge CERunLoopBlock_t)(*tmp);
    block(eventLoop);
}
_Bool CERunLoopTimerBlockExecute(CERunLoop_s * _Nonnull eventLoop, CETimeDescription_s td, void * _Nullable context) {
    assert(context);
    CERunLoopTimerBlock_t block = (__bridge CERunLoopTimerBlock_t)context;
    return block(eventLoop, td, context);
}

void CERunLoopBlockDeallocParams(CERunLoop_s * _Nonnull eventLoop, void * _Nullable params, size_t paramsSize) {
    assert(params);
    assert(paramsSize == sizeof(void *));
    void * * tmp = (void * *)params;
    if (tmp) {
        Block_release(*tmp);
    }
}

void * _Nonnull CERunLoopTimerBlockCreate(CERunLoopTimerBlock_t _Nonnull block) {
    assert(block);
    
    void * b = (__bridge void *)(block);
    return Block_copy(b);
}
void CERunLoopTimerBlockDealloc(CERunLoop_s * _Nonnull eventLoop, CETimeDescription_s td, void * _Nullable context) {
    if (context) {
        Block_release(context);
    }
}

void * _Nonnull CERunLoopBlockCreate(CERunLoopBlock_t _Nonnull block) {
    assert(block);

    void * b = (__bridge void *)(block);
    return Block_copy(b);
}

void CERunLoopAsyncBlock(CERunLoop_s * _Nonnull eventLoop, CERunLoopBlock_t block) {
    assert(eventLoop);
    assert(block);
    void * context = CERunLoopBlockCreate(block);
    CERunLoopAsync(eventLoop, CERunLoopBlockExecute, CERunLoopBlockDeallocParams, &context, sizeof(void *));
}

void CERunLoopSyncBlock(CERunLoop_s * _Nonnull eventLoop, CERunLoopBlock_t block) {
    assert(eventLoop);
    assert(block);
    void * context = CERunLoopBlockCreate(block);
    uint8_t buffer[1024] = {};
    size_t size = arc4random() % 1024 + 1;
    CELogInfo("size: %zu", size);
    CERunLoopSync(eventLoop, CERunLoopBlockExecuteTTT, CERunLoopBlockDeallocParams, &context, sizeof(void *), buffer, size);
    for (int i=0; i<size; i++) {
        printf("%d:%d, ", i, buffer[i]);
    }
    printf("\n");
}

extern void CERunLoopUpdateApiTimer(CERunLoop_s * _Nonnull eventLoop);
extern CEResult_t CERunLoopCreateTimeEvent(CERunLoop_s * _Nonnull eventLoop,
                                           CETimeEvent_s * _Nullable * _Nonnull eventPtr,
                                           uint64_t delay,
                                           uint64_t leeway,
                                           _Bool repeat,
                                           _Bool repeatModeAfter,
                                           uint64_t interval,
                                           CETimeEventHandler_f _Nonnull execute,
                                           void * _Nullable context,
                                           CETimeEventClearContextHandler_f _Nullable clearContext);

CEResult_t CERunLoopAddTimeEventBlock(CERunLoop_s * _Nonnull eventLoop,
                                      CETimeDescription_s * _Nullable tdPtr,
                                      uint64_t delay,
                                      uint64_t leeway,
                                      _Bool repeat,
                                      _Bool repeatModeAfter,
                                      uint64_t interval,
                                      CERunLoopTimerBlock_t _Nonnull timerBlock) {
    CETimeEvent_s * te = NULL;
    
    CEResult_t result = CERunLoopCreateTimeEvent(eventLoop, &te, delay, leeway, repeat, repeatModeAfter, interval, CERunLoopTimerBlockExecute, NULL, CERunLoopTimerBlockDealloc);
    if (CEResultSuccess != result) {
        return result;
    }
    
    void * context = CERunLoopTimerBlockCreate(timerBlock);
    te->context = context;
    return result;
}

CEResult_t CERunLoopAddRepeatTimerBlock(CERunLoop_s * _Nonnull eventLoop,
                                   CETimeDescription_s * _Nullable tdPtr,
                                   uint64_t delay,
                                   uint64_t leeway,
                                   _Bool repeatModeAfter,
                                   uint64_t interval,
                                   CERunLoopTimerBlock_t _Nonnull timerBlock) {
    return CERunLoopAddTimeEventBlock(eventLoop, tdPtr, delay, leeway, 1, repeatModeAfter, interval, timerBlock);
}

CEResult_t CERunLoopAddDelayTimerBlock(CERunLoop_s * _Nonnull eventLoop,
                                       CETimeDescription_s * _Nullable tdPtr,
                                       uint64_t delay,
                                       uint64_t leeway,
                                       CERunLoopTimerBlock_t _Nonnull timerBlock) {
    return CERunLoopAddTimeEventBlock(eventLoop, tdPtr, delay, leeway, 0, 0, 0, timerBlock);

}




@interface Anchor : NSObject

@property (nonatomic, copy) NSString * tag;

@end
@implementation Anchor

- (instancetype)init {
    return [self initWithTag:nil];
}

- (instancetype)initWithTag: (NSString *)tag {
    self = [super init];
    if (self) {
        self.tag = tag;
        NSLog(@"Anchor create: %p, tag: %@", self, self.tag != nil ? self.tag : @"");
    }
    return self;
}

- (void)dealloc {
    NSLog(@"Anchor dealloc: %p, tag: %@", self, self.tag != nil ? self.tag : @"");
}

@end



@interface AppDelegate ()
@end
@implementation AppDelegate


- (void)doTest {
    
    CERunLoopRunDefault();
    CERunLoop_s * eventLoop = CERunLoopShared();
    
    Anchor * a = [[Anchor alloc] init];
    Anchor * b = [[Anchor alloc] init];
    
    CERunLoopAsyncBlock(eventLoop, ^(CERunLoop_s * _Nonnull eventLoop) {
        NSLog(@"a: %p", a);
    });
    
    CERunLoopAsyncBlock(eventLoop, ^(CERunLoop_s * _Nonnull eventLoop) {
        //        CERunLoopAddAfterTimeEventBlock(eventLoop, 15 * 1000000, 1000000, ^bool(CERunLoop_s * _Nonnull eventLoop, uint64_t id, void * _Nullable clientData) {
        //
        //            printf("after timer-\n");
        //            return true;
        //        });
        
        
        //        CERunLoopAddRepeatTimeEventBlock(eventLoop, 6 * 1000000, 1000000, true, 2.5 * 1000000, ^bool(CERunLoop_s * _Nonnull eventLoop, uint64_t id, void * _Nullable clientData) {
        //
        //            NSLog(@"repeat timer-  \n");
        //            return true;
        //        });
        
        
        
        for (int i=0; i<70000; i++) {
            if (i < CETimeEventIdMax) {
                //                __block uint64_t firedId = 0;
                //                CETimeDescription_s td = {};
                //                CEResult_t result = CERunLoopAddRepeatTimerBlock(eventLoop, &td, 5000000, 1000000 / 600, false, 1000000 / 60, ^bool(CERunLoop_s * _Nonnull eventLoop, CETimeDescription_s td, void * _Nullable context) {
                //
                //                    firedId += 1;
                //                    NSLog(@"repeat timer-  %llu\n", firedId);
                //
                //
                //
                //                    if (firedId == 500) {
                //                        NSLog(@"b: %p", b);
                //
                //                        return false;
                //                    }
                //
                //                    return true;
                //                });
                
                CETimeDescription_s td = {};
                CEResult_t result = CERunLoopAddDelayTimerBlock(eventLoop, &td, 12000000ull + i * 1000ull, 1000000, ^bool(CERunLoop_s * _Nonnull eventLoop, CETimeDescription_s td, void * _Nullable context) {
                    
                    
                    CELogInfo("r %d\n", i);
                    return true;
                });
                
                if (CEResultSuccess == result) {
                    
                } else {
                    assert(0);
                }
            } else if (i == CETimeEventIdMax) {
                
                CETimeDescription_s td = {};
                CEResult_t result = CERunLoopAddDelayTimerBlock(eventLoop, &td, 12000000ull + i * 1000ull, 1000000, ^bool(CERunLoop_s * _Nonnull eventLoop, CETimeDescription_s td, void * _Nullable context) {
                    [self doTest];

                    return true;
                });
                
                if (CEResultSuccess == result) {
                    
                } else {
                    assert(0);
                }
            } else {
                CETimeDescription_s td = {};
                CEResult_t result = CERunLoopAddDelayTimerBlock(eventLoop, &td, 12000000 + i * 1000000, 1000000, ^bool(CERunLoop_s * _Nonnull eventLoop, CETimeDescription_s td, void * _Nullable context) {
                    
                    
                    CELogInfo("r %d\n", i);
                    return true;
                });
                
                if (CEResultSuccess == result) {
                    assert(0);
                } else {
                    
                }
            }
            
            
            
        }
        
        
        
        
    });
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(NSEC_PER_SEC / 10000)), dispatch_get_global_queue(0, 0), ^{
        CERunLoopSyncBlock(eventLoop, ^(CERunLoop_s * _Nonnull eventLoop) {
            NSLog(@"sync");
        });

    });
    
    NSLog(@"CEFileEvent_s size: %@", @(sizeof(CEFileEvent_s)));
    NSLog(@"CERunLoop_s size: %@", @(sizeof(CERunLoop_s)));
}


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
//    char name[1024] = {};
//    char * n = "/aw3er3ffasf3";
//    NSString * str = [NSString stringWithFormat:@"%@/Documents/123123131", NSHomeDirectory()];
//    NSData * data = [str dataUsingEncoding:NSUTF8StringEncoding];
//    [data writeToFile:str atomically:true];
////    sem_t * sem = sem_open([str cStringUsingEncoding:NSUTF8StringEncoding], O_CREAT, S_IRUSR | S_IWUSR, 0);
//    sem_t * sem = sem_open(n, O_CREAT, S_IRUSR | S_IWUSR, 10);
////    sem_t * sem = sem_open(n, O_CREAT, 0, 0);
//
//    if (SEM_FAILED == sem) {
//        fprintf(stderr, "CEThreadWaiterInit sem_open error %s; \n", strerror(errno));
//        fflush(stderr);
//        abort();
//    }
//    int rr = sem_wait(sem);
//    if (0 != rr) {
//        fprintf(stderr, "CEThreadWaiterWait sem_wait error %s; \n", strerror(errno));
//        fflush(stderr);
//        abort();
//    }
//
//    
    [self doTest];
    
    struct rlimit limit;
    if(getrlimit(RLIMIT_NOFILE,&limit) != 0) {
        fprintf(stderr, "getrlimit RLIMIT_NOFILE error %s; \n", strerror(errno));
        fflush(stderr);
        abort();
    }
    
    int mib[2] = {CTL_KERN, KERN_MAXFILESPERPROC};
    int maxFilesPerProc = 0;
    size_t size = sizeof(maxFilesPerProc);
    
    if (sysctl(mib, 2, &maxFilesPerProc, &size, NULL, 0) != 0) {
        fprintf(stderr, "sysctl KERN_MAXFILESPERPROC error %s; \n", strerror(errno));
        fflush(stderr);
        abort();
    }
    if (limit.rlim_cur < maxFilesPerProc) {
        limit.rlim_cur = maxFilesPerProc;
        printf("setrlimit rlim_cur %lld; \n", limit.rlim_cur);
        if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
            fprintf(stderr, "setrlimit RLIMIT_NOFILE error %s; \n", strerror(errno));
            fflush(stderr);
            abort();
        }
    }
    
    int value = (int)(limit.rlim_cur);

//    CELogVerboseLevel = 63;
    CELogVerboseLevel = 127;
//    CELogVerboseLevel = 0;
    
    
//    size_t size11 = 1024 * 1024;
//    int as = 1;
//    for (int i=0; i<32; i++) {
//        void * ptr = malloc(size11 * as);
//
//        if (ptr) {
//            NSLog(@"%d %lld malloc success %p", i, (uint64_t)as, ptr);
//        } else {
//            NSLog(@"%d %lld malloc failure", i, (uint64_t)as);
//        }
//
//        as *= 2;
//    }
    
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
