//
//  CEPool.m
//  CoreEvent
//
//  Created by vector on 2020/1/27.
//  Copyright © 2020 vector. All rights reserved.
//

#import "CEPool.h"

typedef uint32_t CETimerTimeInterval;

#define CEPoolFileTimerTableSize 0x800

#pragma pack(push, 4)
typedef struct {
    uint32_t prev: 16;
    uint32_t next: 16;
} CEFileLink_s;

typedef struct {
    int fd;
    uint32_t sequence: 22;
    uint32_t handler: 10;
} CEFileId_s;

/* File event structure 16B*/
typedef struct {
    uint32_t sequence: 22;
    uint32_t handler: 10;

    uint32_t isValid: 1;
    uint32_t xx: 1;

    uint32_t checkReadTimeout: 1;
    uint32_t checkWriteTimout: 1;

    uint32_t readable: 1;
    uint32_t writable: 1;
    uint32_t mask: 2;
    
    uint32_t isReadTimeout: 1;
    uint32_t isWriteTimeout: 1;

    uint32_t indexOfReadTimeout: 11;//单位为(1/8)秒
    uint32_t indexOfWriteTimeout: 11;//单位为(1/)秒
    
    CEFileLink_s readTimer;
    CEFileLink_s writeTimer;
} CEFile_s;

/*
 我测试过 getsockopt(m_socket, SOL_SOCKET, SO_ERROR,(char*)&optval, &optlen);
 和 getsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE,(char*)&optval, &optlen);
 3. getsockopt

 int optval, optlen = sizeof(int);

 getsockopt(sd, SOL_SOCKET, SO_ERROR,(char*) &optval, &optlen);

 switch(optval){

       case 0:

         //“处于连接状态“

         break;

       case ECONNREFUSED:

       case EHOSTNUMREACH:

 }
 
 4. 项目中使用的情况

 // 判断是否连接

 int optval, optlen = sizeof(int);

 int r = getsockopt(g_sockfd, SOL_SOCKET, SO_ERROR,(char*) &optval, &optlen);

 // send HELLO 包

 if ((optval == 0) && (r == 0) && (send(g_sockfd,(char*)&g_DAUCommand,sizeof(struct _DAUCommand),0) != SOCKET_ERROR ))

 {

 ServiceLog.write_log("[threadOperationWaitTrasferAgain]: try to recv from dau...");

          length = recv(g_sockfd,(char *)&command,sizeof(struct _DAUCommand),0);

 }

 else

 {

          if (connect(g_sockfd,(struct sockaddr*)&g_serv_addr, sizeof(g_serv_addr) ) == 0)

          {

                   ServiceLog.write_log("[threadOperationWaitTrasferAgain]:connect to DAU  success.");

                    ServiceLog.write_log("[threadOperationWaitTrasferAgain]: try to recv from dau...");

                    length = recv(g_sockfd,(char *)&command,sizeof(struct _DAUCommand),0);

          }

          else

          {

                    ServiceLog.write_log("[threadOperationWaitTransferAgain]:can't connect to DAU, try again ");

                    length = 0;

                    continue;

          }

 }

 */





struct _CEPool {
    _Atomic(uintptr_t) runInThread;
    _Atomic(uint_fast32_t) runningStates;// 0 is stopped , 1 running, 2 is will stopped
    CERunLoopProgress_t progress;//
    CEThread_s thread;

    int maxfd;   /* highest file descriptor currently registered */
    int maxIndex;
    unsigned int setsize; /* max number of file descriptors tracked */
    int firedCount;

#if __APPLE__
    os_unfair_lock blockQueueLock;
#else
    pthread_spinlock_t blockQueueLock;
#endif
    uint32_t blockEvent;
    uint32_t timerFiredIndex;//source timer 的 游标
    uint32_t fdTagSequence;
    uint32_t xxxx;
    CEBlockQueue_s blockQueue;
    
    CETimeEventManager_s timeEventManager;
    
    CCMicrosecondTime currentTime;//单位 微秒
    uint64_t fileTimerSeconds8;//单位为(1/8)秒
    
    uint32_t fileTableSize;
    CEFile_s * _Nonnull fileTable;
    CEFiredEvent_s * _Nullable firedPages[32768]; /* Fired events, 0x10000 count peer page  */

    uint16_t readTimerTable[CEPoolFileTimerTableSize];
    uint16_t writeTimerTable[CEPoolFileTimerTableSize];
    
    void * _Nullable api; /* This is used for polling API specific data */
    uint32_t observerBufferSize;
    uint32_t observerBufferCount;

    CERunLoopObserver_s * _Nullable * _Nonnull observers;
};




#pragma pack(pop)




