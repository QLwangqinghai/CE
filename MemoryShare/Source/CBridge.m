//
//  CBridge.m
//  MemoryShare
//
//  Created by vector on 2019/8/10.
//  Copyright © 2019 vector. All rights reserved.
//

#import "CBridge.h"

#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#define BUFSZ 4096

size_t CMSizeAlign(size_t size) {
    assert(size >= 0);
    long pageSize = sysconf(_SC_PAGESIZE);
    size_t t = size + (pageSize - 1);
    assert(t > size);
    
    return t & (~(pageSize - 1));
}



@implementation CBridge

+ (void)a {
    __SIZEOF_INT128__;
    int128
    
}


+ (void)run {
    size_t s = 4096 * 2160 * 4;
    s = CMSizeAlign(s);
    
//    IPC_CREAT
    
    int shmId = shmget(IPC_PRIVATE, s, 0666) ;
    //shm_id=shmget(999,BUFSZ,0666|O_CREAT);
    if (shmId < 0 ) { /*创建共享内存*/
        perror( "shmget" ) ;
        exit ( 1 );
    }
    printf ( "successfully created segment : %d \n", shmId ) ;
    system( "ipcs -m"); /*调用ipcs命令查看IPC*/
}

@end
