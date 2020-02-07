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

struct _LSSocketSource;
typedef struct _LSSocketSource LSSocketSource_s;

typedef LSSocketSource_s * _Nonnull (*LSTimerQueueGetSource_f)(LSFileHandler_s * _Nonnull handler, uint32_t id);

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

struct _LSSocketSource {
    //到期时间
    uint64_t deadline: 48;
    
    //在timerQueue中的index
    uint64_t index: 16;
};

#define __LSSocketSourceDeadlineMax (UINT64_MAX >> 16)
#define LSSocketSourceDeadlineForever __LSSocketSourceDeadlineMax

typedef struct {
    int fd;
    uint32_t xxx;
    uint64_t sequence;
    LSSocketSource_s writeSource;
    LSSocketSource_s readSource;

    CCByte16 deviceToken;
    CCByte16 writeVi;
    CCByte16 readVi;
} LSConnect_s;


typedef struct {
    LSFileHandler_s * _Nonnull owner;
    LSTimerQueueGetSource_f _Nonnull getSource;
    uint16_t table[0x10000];
} LSTimerQueue_s;

struct _LSFileHandler {
    uint32_t handlerId;
    uint32_t count;
    LSManager_s * _Nonnull manager;
    LSConnect_s * _Nonnull connectTable;
    LSTimerQueue_s * _Nonnull readTimerQueue;
    LSTimerQueue_s * _Nonnull writeTimerQueue;
};

#pragma pack(pop)

struct _LSManager {
    uint32_t domain;
    uint32_t handlerCount;
    uint64_t sequence;
    LSFileHandler_s handlers[0];
};


#if defined(__cplusplus)
}  // extern C
#endif


#endif /* LSTypes_h */
