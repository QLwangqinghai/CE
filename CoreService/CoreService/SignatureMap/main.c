//
//  main.c
//  SignatureMap
//
//  Created by vector on 2020/1/26.
//  Copyright © 2020 vector. All rights reserved.
//

#include "CCBase.h"
#include <dispatch/dispatch.h>


#pragma pack(push,4)
typedef struct {
    CCUInt32 next;
    CCUInt32 prev;
} LinkInfo;

typedef struct {
    CCUInt32 isUsing: 1;
    CCUInt32 next0: 31;
    CCUInt32 isInvalid: 1;//标记非法请求(+签名错误请求)
    CCUInt32 next1: 31;
    LinkInfo link;
    CCUInt32 time;
    CCByte16 signature;
} SignatureItem;


typedef struct {
    CCUInt32 isUsing: 1;
    CCUInt32 next: 31;
    CCUInt32 invalidTime;//失效日期
    CCByte16 ip;
} ConnectItem;
#pragma pack(pop)


//0x200000
typedef struct {
    SignatureItem * _Nonnull items;
    CCUInt32 * _Nonnull table0;
    CCUInt32 * _Nonnull table1;
    CCUInt32 hashSeed0;
    CCUInt32 hashSeed1;

    CCUInt32 size;
    CCUInt32 tableSize;
    CCUInt32 tableIndexMask;

    CCUInt32 time;
    CCUInt32 qps;
    
    CCUInt32 cacheHead;
    CCUInt32 queueHead;
    CCUInt32 queueFoot;
} SignatureMap;



typedef struct {
    ConnectItem * _Nonnull items;
    uint32_t * _Nonnull table;

    uint32_t hashSeed;
    uint32_t size;
    uint32_t tableSize;
    uint32_t tableIndexMask;

    uint32_t updateSequence;

    uint32_t cacheHead;
} BlackIpMap;

/*
 非登录态 签名去重
 按照QPS 2000计算

 uuid 剃重 6分钟， 向前3分钟 向后3分钟， 360s  720,000
 最大容量 0x40000 - 1


 需要一个好点的hash算法

 需要的内存块 0x40000 * 48 字节  0x1200000 18MB
 索引结构 0x200000 字节 2MB

 
 */
















int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    dispatch_async(dispatch_get_main_queue(), ^{
        
    });
    
    return 0;
}
