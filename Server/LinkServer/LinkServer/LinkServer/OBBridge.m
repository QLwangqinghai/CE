//
//  OBBridge.m
//  LinkServer
//
//  Created by vector on 2020/1/30.
//  Copyright © 2020 vector. All rights reserved.
//

#import "OBBridge.h"
#import <CoreEvent/CoreEvent.h>
#import <CoreEvent/CCNet.h>


#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void CEHandleSocketTimeout(void * _Nullable context, CEFileId * _Nonnull ids, uint32_t count) {
    CEPollPtr poll = CEPollShared();

//    uint8_t buffer[4096] = {};
//    for (uint32_t i=0; i<count; i++) {
//        CEFileId * id = ids + i;
//
//        CEFileEventMask_es status = CEPollGetFileStatus(poll, *id);
//        if (status & CEFileEventMaskReadable) {
//            int len = recv(id->fd, buffer, 2096, 0);
//            printf("readLen: %ld\n", len);
//        } else {
//            int len = send(id->fd, buffer, 2096, 0);
//            printf("sendLen: %ld\n", len);
//        }
//    }
}
void CEHandleSocket(void * _Nullable context, CEFileId * _Nonnull ids, uint32_t count) {
    CEPollPtr poll = CEPollShared();
    
    struct sockaddr_un aa;
//    if_nametoindex("eth0")
    
    uint8_t buffer[4096] = {};
    for (uint32_t i=0; i<count; i++) {
        CEFileId * id = ids + i;
        
        CEFileEventMask_es status = CEPollGetFileStatus(poll, *id);
        if (status & CEFileEventMaskReadable) {
            int len = recv(id->fd, buffer, 2096, 0);
            printf("readLen: %ld\n", len);
        } else {
            int len = send(id->fd, buffer, 2096, 0);
            printf("sendLen: %ld\n", len);
        }
    }
}


void CEAddSocket(uintptr_t context, const CCPtr _Nullable input, CCPtr _Nullable output) {
    uint16_t sfd = context & 0xffff;
    uint16_t hid = (context & 0xffff0000) >> 16;
    CEPollPtr poll = CEPollShared();
    CEFileId fd = {};
    CEPollAddFile(poll, hid, sfd, &fd);
    CEPollAddFileEventMask(poll, fd, CEFileEventMaskReadable);
}

typedef void (*CCClosureClearContext_f)(uintptr_t context);



@implementation OBBridge


+ (void)test {
    CEPollPtr poll = CEPollShared();
    
    uint32_t handlerId = CEPollRegisterHandler(poll, NULL, CEHandleSocket, CEHandleSocketTimeout);

    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        struct sockaddr_in6 clientAddress = {};
        struct sockaddr_in server_addr = {};
        socklen_t len6 = sizeof(struct sockaddr_in6);
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8000);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        
        int struct_len = sizeof(struct sockaddr_in);
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        assert(fd >= 0);
        assert(bind(fd, (struct sockaddr *)&server_addr, struct_len) == 0);
        assert(listen(fd, 1024) == 0);

        int newFd = accept(fd, (struct sockaddr *)&clientAddress, &len6);
        
        struct sockaddr_in serverAddr2 = {};
        
        socklen_t currLen = sizeof(struct sockaddr_in);
        int result = getsockname(newFd, &serverAddr2, &currLen);
getpeername(<#int#>, <#struct sockaddr *restrict#>, <#socklen_t *restrict#>)
        if (-1 == newFd) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                //no connections没有新连接请求；
            } else {
                //其他错误
            }
        } else {
            //正常的
            
        }
        

        
        
        assert(newFd >= 0);
        assert(newFd < 0x10000);
        
        uint32_t h = handlerId << 16;
        uint32_t context = h + newFd;
        CCClosureRef closure = CCClosureCreate(CEAddSocket, NULL, context);
        CEPollAsync(poll, closure);
        CCClosureRelease(closure);
        
        
    });
    
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        [self testIpv6];
    });
    
    
}

+ (void)testIpv6 {
    CEPollPtr poll = CEPollShared();
    uint32_t handlerId = CEPollRegisterHandler(poll, NULL, CEHandleSocket, CEHandleSocketTimeout);

    struct sockaddr_in6 clientAddress = {};
    struct sockaddr_in6 server_addr = {};
    socklen_t len6 = sizeof(struct sockaddr_in6);
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_port = htons(8000);
    struct in6_addr anyaddr = IN6ADDR_ANY_INIT;
    server_addr.sin6_addr = anyaddr;
    int fd = socket(AF_INET6, SOCK_STREAM, 0);
    assert(fd >= 0);
    assert(bind(fd, (struct sockaddr *)&server_addr, len6) == 0);
    assert(listen(fd, 1024) == 0);

    int new_fd = accept(fd, (struct sockaddr *)&clientAddress, &len6);
    assert(new_fd >= 0);
    assert(new_fd < 0x10000);
    
    uint32_t h = handlerId << 16;
    uint32_t context = h + new_fd;
    CCClosureRef closure = CCClosureCreate(CEAddSocket, NULL, context);
    CEPollAsync(poll, closure);
    CCClosureRelease(closure);
    
    
}


+ (void)test2 {
    
}

@end
