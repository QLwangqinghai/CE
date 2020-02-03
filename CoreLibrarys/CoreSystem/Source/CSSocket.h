//
//  CSSocket.h
//  CoreSystem
//
//  Created by CaiLianfeng on 2017/8/27.
//  Copyright © 2017年 WangQinghai. All rights reserved.
//

#ifndef CSSocket_h
#define CSSocket_h

#include "CSSocketTypes.h"

CPSocketAddress_t CPSocketAddressFromAddrInfo(struct addrinfo * _Nonnull socketRef);
CPSocketAddress_t CPSocketAddressFromSocket(CPSocketInfo_t * _Nonnull socketRef);


typedef struct _CPSocketMakeInfo {
    CPSocketProtocolFamily_e family;
    CPSocketType_e type;
    CPSocketProtocol_e protocol;
    struct sockaddr * _Nonnull targetAddr;
    socklen_t targetAddrLen;
} CPSocketMakeInfo_t;

typedef struct _CPSocketInfoMakeBuffer {
    int currentIndex;
    int count;
    CPSocketMakeInfo_t * _Nullable infos[10];
}CPSocketMakeInfoBuffer_t;


typedef enum CPSocketMakeInfoBufferMakeSocketInfoError {
    CPSocketMakeInfoBufferMakeSocketInfoErrorNone = 0,
    CPSocketMakeInfoBufferMakeSocketInfoErrorCreateFileDescriptorFailure,
    CPSocketMakeInfoBufferMakeSocketInfoErrorOutRange,
}CPSocketMakeInfoBufferMakeSocketInfoError_e;

CPSocketInfo_t * _Nullable CPSocketMakeInfoBufferMakeClientSocketInfo(const CPSocketMakeInfoBuffer_t * _Nonnull buffer, int index, CPSocketMakeInfoBufferMakeSocketInfoError_e * _Nonnull errorRef);


void CPSocketMakeInfoDealloc(CPSocketMakeInfo_t * _Nonnull makeInfo);

void CPSocketMakeInfoBufferDealloc(CPSocketMakeInfoBuffer_t * _Nonnull buffer);

typedef enum CPSocketInfoTcpClientCreateError {
    CPSocketInfoTcpClientCreateErrorNone = 0,
    CPSocketInfoTcpClientCreateErrorErrorInvalidFlag,
    CPSocketInfoTcpClientCreateErrorErrorInvalidPort,
    CPSocketInfoTcpClientCreateErrorGetAddressError,
    CPSocketInfoTcpClientCreateErrorOther,
}CPSocketInfoTcpClientCreateError_e;

typedef enum CPSocketInfoTcpClientCreateFlag {
    CPSocketInfoTcpClientCreateFlagV4AndV6 = 0,
    CPSocketInfoTcpClientCreateFlagV4Only,
    CPSocketInfoTcpClientCreateFlagV6Only,
}CPSocketInfoTcpClientCreateFlag_e;

CPSocketMakeInfoBuffer_t * _Nullable CPSocketMakeInfoBufferTcpClientCreate(const char * _Nonnull host, int port, CPSocketInfoTcpClientCreateFlag_e flag, CPSocketInfoTcpClientCreateError_e * _Nonnull error);

typedef enum CPSocketInfoTcpServerCreateError {
    CPSocketInfoTcpServerCreateErrorNone = 0,
    CPSocketInfoTcpServerCreateErrorCreateFileDescriptorFailure,
    CPSocketInfoTcpServerCreateErrorInvalidPort,
    CPSocketInfoTcpServerCreateErrorBindError,
    CPSocketInfoTcpServerCreateErrorGetAddress,
}CPSocketInfoTcpServerCreateError_e;



typedef union {
    struct in_addr addr4;
    struct in6_addr addr6;
} CPSocketIpAddress_u;

CPTcpServerSocketInfo_t * _Nullable CPTcpServerSocketInfoCreate(const CPSocketIpAddress_u * _Nullable localAddr, int port, CPIpType_e useIpType, CPSocketInfoTcpServerCreateError_e * _Nonnull error);

//CPSocketInfo_t * CPSocketInfoClientAllocInit(int sockfd, struct addrinfo ai);
CPSocketInfo_t * _Nullable CPSocketInfoAcceptedInetAllocInit(int sockfd, struct sockaddr_in ai);
CPSocketInfo_t * _Nullable CPSocketInfoAcceptedInet6AllocInit(int sockfd, struct sockaddr_in6 ai);
CPSocketInfo_t * _Nullable CPSocketInfoAcceptedUnixAllocInit(int sockfd, struct sockaddr_un ai);

//CPSocketInfo_t * CPSocketInfoServerAllocInit(int sockfd, struct addrinfo ai);

void CPSocketInfoDealloc(CPSocketInfo_t * _Nonnull socket);



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*CPSocket api*/

typedef enum CPSocketFileDescriptorCreateError {
    CPSocketFileDescriptorCreateErrorNone = 0,
    CPSocketFileDescriptorCreateErrorFailure,
}CPSocketFileDescriptorCreateError_e;

int CPSocketFileDescriptorCreate(int32_t familyValue, int32_t typeValue, int32_t protoValue, CPSocketFileDescriptorCreateError_e * _Nonnull error);

void CPSocketPeerReadShutdown(CPSocketInfo_t * _Nonnull socket);
void CPSocketPeerWriteShutdown(CPSocketInfo_t * _Nonnull socket);
void CPSocketShutdown(int sockfd, CPSocketShutdownType_e type);
void CPSocketSetBlocking(int sockfd, CPSocketBlockStatus_e status);
void CPSocketClose(int sockfd);
typedef enum CPSocketListenResult {
    CPSocketListenResultSuccess = 0,
    CPSocketListenResultError,
}CPSocketListenResult_e;

CPSocketListenResult_e CPSocketListen(int sockfd, int32_t maxBacklogSize);


typedef enum CPSocketConnectResult{
    CPSocketConnectResultConnected,
    CPSocketConnectResultConnecting,
    CPSocketConnectResultConnectError
}CPSocketConnectResult_e;

CPSocketConnectResult_e CPAsyncSocketConnect(CPSocketInfo_t * _Nonnull info, int * _Nonnull errorRef);
CPSocketConnectResult_e CPSyncSocketConnect(CPSocketInfo_t * _Nonnull info, int * _Nonnull errorRef);


typedef enum CPSocketAcceptError {
    CPSocketAcceptErrorNone = 0,
    CPSocketAcceptErrorEINTR,//无错误， 也没有accept 到
    CPSocketAcceptErrorCreateCPSocketInfoFailure,
    CPSocketAcceptErrorReadError,
}CPSocketAcceptError_e;

CPSocketInfo_t * _Nullable CPSocketAccept(const CPSocketInfo_t * _Nonnull info, CPSocketAcceptError_e * _Nonnull errorRef);


typedef enum CPSocketReadResult {
    CPSocketReadResultSuccess,
    CPSocketReadResultBufferError,
    CPSocketReadResultWaitRetry,
    CPSocketReadResultError,
    CPSocketReadResultPeerClose,
}CPSocketReadResult_e;
typedef struct _CPSocketReadResult {
    CPSocketReadResult_e status;
    long readLength;
}CPSocketReadResult_t;

//recv
CPSocketReadResult_t CPAsyncSocketRead(CPSocketInfo_t * _Nonnull socket, uint8_t * _Nonnull buffer, int bufferCount, int64_t pendingLength);


typedef enum CPSocketWriteResult {
    CPSocketWriteResultSuccess,
    CPSocketWriteResultBufferError,
    CPSocketWriteResultWaitRetry,
    CPSocketWriteResultError,
    CPSocketWriteResultPeerClose,
}CPSocketWriteResult_e;
typedef struct _CPSocketWriteResult {
    CPSocketWriteResult_e status;
    long writeLength;
}CPSocketWriteResult_t;

//send
CPSocketWriteResult_t CPAsyncSocketWrite(CPSocketInfo_t * _Nonnull socket, const uint8_t * _Nonnull buffer, long count, int64_t pendingLength);


void CPSocketSetWriteBufferSize(int sockfd, int size);
void CPSocketSetReadBufferSize(int sockfd, int size);

int CPSocketGetWriteBufferSize(int sockfd);
int CPSocketGetReadBufferSize(int sockfd);



#endif /* CPSocket_h */
