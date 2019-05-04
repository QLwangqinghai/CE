//
//  CEBase.h
//  CoreEvent
//
//  Created by wangqinghai on 2018/10/23.
//  Copyright © 2018年 com.wangqinghai. All rights reserved.
//

#ifndef CEBase_h
#define CEBase_h

//c
#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <errno.h>

#include <pthread.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/sysctl.h>
#include <fcntl.h>

#if __APPLE__
#import <os/lock.h>
#endif

//custom
#include "CETime.h"
#include "CELog.h"


static inline uint64_t CEGetMicrosecondsTime(void) {
    return CESystemBootMicroseconds();
}

typedef int CEResult_t;

typedef uint32_t CEFileEventMask_es;
static const CEFileEventMask_es CEFileEventMaskNone = 0;
static const CEFileEventMask_es CEFileEventMaskReadable = 1;
static const CEFileEventMask_es CEFileEventMaskWritable = 2;
static const CEFileEventMask_es CEFileEventMaskReadWritable = 3;

//单位: 微妙
typedef uint64_t CETimeInterval;

static int CEFileDescriptionInvalid = INT_MIN;


//当前模式下会在上次timer 执行的时间基础上 添加 间隔时间 作为下次触发的时间， 比如  触发时间为 10000， 间隔时间 为 1000， 当前执行的时间 为 10234， 则下次触发 在 11234 之后
#define CETimeEventRepeatModeAfter 1

//当前模式下会在上次timer 应该触发的时间基础上 添加 间隔时间 作为下次触发的时间， 比如  触发时间为 10000， 间隔时间 为 1000， 当前执行的时间 为 10234， 则下次触发 在 11000 之后
#define CETimeEventRepeatModeNone 0




#define CETimeEventStatesWait 0u
#define CETimeEventStatesExecuting 0x1u
#define CETimeEventStatesCancel 0x2u
#define CETimeEventStatesFinished 0x3u

#define CETimeEventLeewayMax 0x1FFFFull
#define CETimeEventIntervalMax 0x3FFFFFFFFFFull



static const uint32_t CETimeEventQueueIndexMax = 0xFFFEul;
static const uint32_t CETimeEventQueueIndexInvalid = 0xFFFFul;

static const uint32_t CETimeEventIdMax = 0xFFFEul;
static const uint32_t CETimeEventIdInvalid = 0xFFFFul;

static const uint64_t CETimeEventIdAllUsed = UINT64_MAX;


#if __LLP64__ || __LP64__
#define CEAtomicMemoryBlockSizeUse64 1
#else
#define CEAtomicMemoryBlockSizeUse64 0
#endif

#if CEAtomicMemoryBlockSizeUse64

typedef uint64_t CEMemoryBlock_t;
typedef _Atomic(uint_fast64_t) CEAtomicMemoryBlock_t;
typedef uint64_t CEAtomicMemoryBlockBridgeType_t;
#define CEMemoryBlockSize (8)
#define CEAtomicMemoryBlockSize (8)
#define CEAtomicMemoryBlockSizeShift (3)

#else

typedef uint32_t CEMemoryBlock_t;
typedef _Atomic(uint_fast32_t) CEAtomicMemoryBlock_t;
typedef uint32_t CEAtomicMemoryBlockBridgeType_t;
#define CEMemoryBlockSize (4)
#define CEAtomicMemoryBlockSize (4)
#define CEAtomicMemoryBlockSizeShift 2

#endif


typedef uint32_t CERunLoopProgress_t;

static const CERunLoopProgress_t CERunLoopProgressExit = 0;
static const CERunLoopProgress_t CERunLoopProgressDoSource = 1;
static const CERunLoopProgress_t CERunLoopProgressDoTimer = 2;
static const CERunLoopProgress_t CERunLoopProgressWaiting = 3;

typedef uint32_t CEThreadStatus_t;

static const CEThreadStatus_t CEThreadStatusStarting = 0;
static const CEThreadStatus_t CEThreadStatusExecuting = 1;
static const CEThreadStatus_t CEThreadStatusFinished = 2;


typedef uint32_t CERunLoopObserverMask_t;

static const CERunLoopObserverMask_t ERunLoopObserverMaskFrameBegin = 0x80000000;
static const CERunLoopObserverMask_t ERunLoopObserverMaskFrameFinish = 0x40000000;

static const CERunLoopObserverMask_t ERunLoopObserverMaskBeforeDoBlock0 = 0x1;
static const CERunLoopObserverMask_t ERunLoopObserverMaskAfterDoBlock0 = 0x2;
static const CERunLoopObserverMask_t ERunLoopObserverMaskBeforeDoTimer0 = 0x4;
static const CERunLoopObserverMask_t ERunLoopObserverMaskAfterDoTimer0 = 0x8;
static const CERunLoopObserverMask_t ERunLoopObserverMaskBeforeDoCheckSource = 0x10;
static const CERunLoopObserverMask_t ERunLoopObserverMaskAfterDoCheckSource = 0x20;
static const CERunLoopObserverMask_t ERunLoopObserverMaskBeforeDoBlock1 = 0x40;
static const CERunLoopObserverMask_t ERunLoopObserverMaskAfterDoBlock1 = 0x80;
static const CERunLoopObserverMask_t ERunLoopObserverMaskBeforeDoTimer1 = 0x100;
static const CERunLoopObserverMask_t ERunLoopObserverMaskAfterDoTimer1 = 0x200;
static const CERunLoopObserverMask_t ERunLoopObserverMaskBeforeDoSource = 0x400;
static const CERunLoopObserverMask_t ERunLoopObserverMaskAfterDoSource = 0x800;
static const CERunLoopObserverMask_t ERunLoopObserverMaskBeforeCheckSourceTimeout = 0x1000;
static const CERunLoopObserverMask_t ERunLoopObserverMaskAfterCheckSourceTimeout = 0x2000;
static const CERunLoopObserverMask_t ERunLoopObserverMaskBeforeDoSourceTimeout = 0x4000;
static const CERunLoopObserverMask_t ERunLoopObserverMaskAfterDoSourceTimeout = 0x8000;


static const CEResult_t CEResultSuccess = 0;
static const CEResult_t CEResultErrorParams = 1;
static const CEResult_t CEResultErrorSystemCall = 2;
static const CEResult_t CEResultErrorFileDescription = 3;
static const CEResult_t CEResultErrorSourceReinit = 4;
static const CEResult_t CEResultErrorShouldRemoveSource = 5;
static const CEResult_t CEResultFailureResize = 6;
static const CEResult_t CEResultErrorFileDescriptionInvalid = 7;
static const CEResult_t CEResultErrorTimeDescriptionInvalid = 8;
static const CEResult_t CEResultErrorTimeDescriptionNotEnough = 9;

#endif /* CEBaseType_h */
