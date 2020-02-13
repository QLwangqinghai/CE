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

#import <CoreEvent/CoreEvent.h>

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
    uint64_t deadline: 52;

    uint64_t connectionIndex: 12;//对应的connection在group中的index
};

#define __LSSocketSourceDeadlineMax (UINT64_MAX >> 12)
#define LSSocketSourceDeadlineForever __LSSocketSourceDeadlineMax

typedef struct {
    int fd;
    uint32_t mask: 2;
    uint32_t status: 2;
//    uint32_t isReadable: 1;
//    uint32_t isWritable: 1;
    uint32_t readTimerIndex: 12;
    uint32_t writeTimerIndex: 12;
    uint32_t xxx: 4;
    uint64_t sequence;
    CCByte16 deviceToken;
    CCByte16 writeVi;
    CCByte16 readVi;
} LSConnection_s;



typedef LSConnectionTimeSource_s * _Nonnull (*LSTimerQueueSetSourceIndex_f)(LSConnectionGroup_s * _Nonnull group, uint32_t connectionIndex, uint32_t timerIndex);

typedef struct {
    LSConnectionGroup_s * _Nonnull group;
    LSConnectionTimeSource_s * _Nonnull sources;
    LSTimerQueueSetSourceIndex_f _Nonnull setSourceIndex;
} LSTimerQueue_s;

//每4kconnection为一组
typedef struct {
    LSConnection_s table[4096];
    LSConnectionTimeSource_s writeSourceQueue[4096];
    LSConnectionTimeSource_s readSourceQueue[4096];
} LSConnnectionGroupStorage_s;

struct _LSConnnectionGroup {
    uint32_t groupId;
    uint32_t activeCount;
    LSConnnectionGroupStorage_s * _Nonnull storage;
};

struct _LSEventLoop {
    uint32_t id;
    uint32_t groupCount;
    uint32_t connectionCapacity;
    uint32_t connectCount;
    void * _Nonnull api;
    LSConnectionGroup_s groups[0];
};

typedef struct {
    uint32_t index: 24;
    uint32_t loopId: 7;
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
    LSEventLoop_s * _Nonnull loops[0];
};


#if defined(__cplusplus)
}  // extern C
#endif


#endif /* LSTypes_h */
