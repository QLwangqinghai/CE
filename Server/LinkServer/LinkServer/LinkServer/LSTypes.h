//
//  LSTypes.h
//  LinkServer
//
//  Created by vector on 2020/2/6.
//  Copyright © 2020 vector. All rights reserved.
//

#ifndef LSTypes_h
#define LSTypes_h

#if defined(__cplusplus)
extern "C" {
#endif

#import <CCFoundation/CCFoundation.h>
#import <CoreEventShim/CoreEventShim.h>

// AES_MAXNR is the maximum number of AES rounds.   128b:10 192:12 256:14
#define AES_MAXNR 14

#define AES_BLOCK_SIZE 16

// aes_key_st should be an opaque type, but EVP requires that the size be
// known.

struct aes_key_st {
  uint32_t rd_key[4 * (AES_MAXNR + 1)];
  uint32_t rounds;
};


typedef struct {
    struct aes_key_st readKey;
    struct aes_key_st writeKey;
} ConnectAesKey_s;


#define LSConnectIndexInvalid 0xffffffff


struct _LSManager;
typedef struct _LSManager LSManager_s;

struct _LSFileHandler;
typedef struct _LSFileHandler LSFileHandler_s;

struct _LSConnectionTimeSource;
typedef struct _LSConnectionTimeSource LSConnectionTimeSource_s;

struct _LSFileGroup;
typedef struct _LSFileGroup LSFileGroup_s;

struct _LSConnnectionGroup;
typedef struct _LSConnnectionGroup LSConnectionGroup_s;

struct _LSEventLoop;
typedef struct _LSEventLoop LSEventLoop_s;

#pragma pack(push, 2)
//handlerId14 context:16

typedef struct {
    int fd;
    uint32_t indexInGroup: 13;//LSFile_s
    uint32_t groupId: 10;
    uint32_t loopId: 6;
    uint32_t error: 1;
    uint32_t status: 2;//CEFileEventMask_es
} LSEvent_s;


typedef struct {
    uint32_t domain;
    uint16_t handler;
    uint16_t context;
    uint64_t sequence;
} LSConnectSessionId_s;

typedef struct {
    int fd;
    uint64_t sequence;
    CCByte16 deviceToken;
    CCByte16 readVi;
    CCByte16 writeVi;
} Connect_s;

typedef struct {
    uint16_t domain;
    uint16_t service;
    uint16_t time;
} PackageHeader_s;

struct _LSConnectionTimeSource {
    //到期时间
    uint64_t deadline: 51;

    uint64_t connectionIndex: 13;//对应的connection在group中的index
};

#define __LSSocketSourceDeadlineMax (UINT64_MAX >> 12)
#define LSSocketSourceDeadlineForever __LSSocketSourceDeadlineMax


//    CCByte16 deviceToken; fd4 + sequence8 + 
typedef struct {
    int fd;
    uint32_t mask: 2;
    uint32_t status: 2;
    uint32_t hasError: 1;
    uint32_t readTimerIndex: 13;
    uint32_t writeTimerIndex: 13;
    uint32_t isValid: 1;
    uint64_t sequence;
    CCByte16 deviceToken;
    CCByte16 writeVi;
    CCByte16 readVi;
    CCByte64 writeKey;
    CCByte64 readKey;
} LSConnection_s;



typedef LSConnectionTimeSource_s * _Nonnull (*LSTimerQueueSetSourceIndex_f)(LSConnectionGroup_s * _Nonnull group, uint32_t connectionIndex, uint32_t timerIndex);

typedef struct {
    LSConnectionGroup_s * _Nonnull group;
    LSConnectionTimeSource_s * _Nonnull sources;
    LSTimerQueueSetSourceIndex_f _Nonnull setSourceIndex;
} LSTimerQueue_s;

//每4kconnection为一组
typedef struct {
    uint16_t trigger[8192];
    LSConnection_s table[8192];
    LSConnectionTimeSource_s writeSourceQueue[8192];
    LSConnectionTimeSource_s readSourceQueue[8192];
} LSConnnectionGroupStorage_s;

struct _LSConnnectionGroup {
    uint32_t groupId;
    uint32_t triggerCount;
    uint32_t activeCount;
    LSConnnectionGroupStorage_s * _Nonnull storage;
};

struct _LSEventLoop {
#if (defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined (__NetBSD__))
    pthread_t _Nonnull thread;
#else
    pthread_t thread;
#endif
    uint64_t time;
    uint32_t id;
    uint32_t groupCount;
    uint32_t connectionCapacity;
    uint32_t connectCount;
    void * _Nonnull api;
    LSConnectionGroup_s groups[0];
};

static inline LSConnectionGroup_s * _Nonnull LSEventLoopGetGroup(LSEventLoop_s * _Nonnull loop, uint32_t index) {
    assert(loop);
    assert(index<loop->groupCount);
    return &(loop->groups[index]);
}

typedef struct {
    uint32_t index: 23;
    uint32_t loopId: 6;
    uint32_t mask: 2;
    uint32_t isValid: 1;
} LSFile_s;

#pragma pack(pop)

struct _LSManager {
    uint32_t domain;
    uint32_t loopCount;
    uint32_t maxConnectionCount;
    uint32_t fileTableSize;//当前进程能打开的最大文件描述符大小
    uint64_t sequence;
    LSFile_s * _Nonnull fileTable;//index 是 fd， 值是 File在fileTable中的index
    int * clientBuffer;
    LSEventLoop_s * _Nonnull loops[0];
};

static inline LSFile_s * _Nonnull LSManagerGetFile(LSManager_s * _Nonnull manager, uint32_t index) {
    assert(index < manager->fileTableSize);
    return manager->fileTable + index;
}


#if defined(__cplusplus)
}  // extern C
#endif


#endif /* LSTypes_h */
