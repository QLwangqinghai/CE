//
//  AppDelegate.m
//  Demo
//
//  Created by 王青海 on 2018/10/23.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#import "AppDelegate.h"
#import <CoreEvent/CoreEvent.h>
#import <CoreEvent/CETimeEvent.h>

typedef void (^CERunLoopBlock_t)(CERunLoop_s * _Nonnull eventLoop);
typedef _Bool (^CERunLoopTimerBlock_t)(CERunLoop_s * _Nonnull eventLoop, uintptr_t id, void * _Nullable clientData);

void CERunLoopBlockExecute(CERunLoop_s * _Nonnull eventLoop, void * _Nullable context) {
    assert(context);
    CERunLoopBlock_t block = (__bridge CERunLoopBlock_t)context;
    block(eventLoop);
}
_Bool CERunLoopTimerBlockExecute(CERunLoop_s * _Nonnull eventLoop, uintptr_t id, void * _Nullable clientData) {
    assert(clientData);
    CERunLoopTimerBlock_t block = (__bridge CERunLoopTimerBlock_t)clientData;
    return block(eventLoop, id, clientData);
}
    
void CERunLoopBlockDealloc(CERunLoop_s * _Nonnull eventLoop, void * _Nullable context) {
    if (context) {
        Block_release(context);
    }
}

void * _Nonnull CERunLoopTimerBlockCreate(CERunLoopTimerBlock_t _Nonnull block) {
    assert(block);
    
    void * b = (__bridge void *)(block);
    return Block_copy(b);
}
void CERunLoopTimerBlockDealloc(CERunLoop_s * _Nonnull eventLoop, uintptr_t id, void * _Nullable context) {
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
    CERunLoopAsync(eventLoop, CERunLoopBlockExecute, CERunLoopBlockDealloc, context);
}


uintptr_t CETimeEventInitWithBlock(CERunLoop_s * _Nonnull eventLoop, uint64_t leeway, _Bool repeat, _Bool repeatModeAfter, uint64_t interval, CERunLoopTimerBlock_t _Nonnull timerBlock, CEResult_t * _Nonnull result) {

    void * context = CERunLoopTimerBlockCreate(timerBlock);

    uintptr_t v = CETimeEventInit(eventLoop, leeway, repeat, repeatModeAfter, interval, CERunLoopTimerBlockExecute, context, CERunLoopTimerBlockDealloc, result);
    if (*result != CEResultSuccess) {
        CERunLoopBlockDealloc(eventLoop, context);
    }
    
    return v;
}

uintptr_t CETimeEventInitRepeatTimerWithBlock(CERunLoop_s * _Nonnull eventLoop, uint64_t leeway, _Bool repeatModeAfter, uint64_t interval, CERunLoopTimerBlock_t _Nonnull timerBlock, CEResult_t * _Nonnull result) {
    return CETimeEventInitWithBlock(eventLoop, leeway, 1, repeatModeAfter, interval, timerBlock, result);
}

uintptr_t CETimeEventInitAfterTimerWithBlock(CERunLoop_s * _Nonnull eventLoop, uint64_t leeway, CERunLoopTimerBlock_t _Nonnull timerBlock, CEResult_t * _Nonnull result) {
    return CETimeEventInitWithBlock(eventLoop, leeway, 0, 0, 0, timerBlock, result);
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

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application

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
        
        __block uint64_t firedId = 0;
        
        CEResult_t result = 0;
        uintptr_t id = CETimeEventInitRepeatTimerWithBlock(eventLoop, 1000000 / 600, false, 1000000 / 60, ^bool(CERunLoop_s * _Nonnull eventLoop, uintptr_t id, void * _Nullable clientData) {
            firedId += 1;
            NSLog(@"repeat timer-  %llu\n", firedId);
            

            
            if (firedId == 500) {
                NSLog(@"b: %p", b);

                return false;
            }
            
            return true;
        }, &result);
        
        if (CEResultSuccess != result) {
            
        } else {
            CERunLoopAddTimeEvent(eventLoop, id, 5 * 1000000);
        }
        
        
    });
    
    
    NSLog(@"CEFileEvent_s size: %@", @(sizeof(CEFileEvent_s)));
    NSLog(@"CERunLoop_s size: %@", @(sizeof(CERunLoop_s)));

    
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

//    CELogVerboseLevel = 127;
    CELogVerboseLevel = 128;

    
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
