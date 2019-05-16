//
//  main.m
//  TGCD
//
//  Created by vector on 2019/5/4.
//  Copyright © 2019 com.wangqinghai. All rights reserved.
//

#import <Foundation/Foundation.h>

struct __CEType;
typedef struct __CEType CETypeBase_s;

struct __CEAlloctor;
typedef struct __CEAlloctor CEAlloctor_s;

typedef uint16_t CETypeMask_t;
static const CETypeMask_t CETypeMaskDefaultRc = 0x8000u;
static const CETypeMask_t CETypeMaskMate = 0x4000u;

#define CETypeMaskVersionBitCount 16
#pragma pack(push)
#pragma pack(1)

struct __CEType {
    CETypeBase_s * _Nonnull type;
    uint16_t version;
    uint16_t masks;
    uint32_t objectSize;//objectSize 为0 时 size 为变长
    size_t (* _Nonnull getSize)(CETypeBase_s * _Nonnull type, void * _Nonnull object);
    
    CETypeBase_s const * _Nullable alloctor;//CMAlloctor_s * description
    void (* _Nonnull init)(void * _Nonnull object);
    void (* _Nonnull deinit)(void * _Nonnull object);
    
    char * _Nonnull name;
    size_t (* _Nonnull descript)(void * _Nonnull object, char * _Nonnull buffer, size_t bufferSize);
    
    void * _Nonnull (* _Nonnull retain)(void * _Nonnull object);
    void * _Nullable (* _Nonnull tryRetain)(void * _Nonnull object);
    void (* _Nonnull release)(void * _Nonnull object);
};

struct __CEAlloctor {
    void * _Nonnull (* _Nonnull allocate)(CETypeBase_s const * _Nonnull type, size_t size);
    void (* _Nonnull deallocate)(CETypeBase_s const * _Nonnull type, void * _Nonnull ptr, size_t size);
    void * _Nullable context;
};

#pragma pack(pop)

static dispatch_queue_t queue;

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        NSLog(@"Hello, World! %ld", sizeof(struct __CEType));
        
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            NSLog(@"11");
        });
        queue = dispatch_queue_create("convert", DISPATCH_QUEUE_CONCURRENT);
        dispatch_async(queue, ^{
            NSLog(@"121");

        });
        
        [[NSRunLoop currentRunLoop] run];
        
    }
    return 0;
}
