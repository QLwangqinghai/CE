//
//  COBaseType.h
//  CoreObject
//
//  Created by wangqinghai on 2018/10/23.
//  Copyright © 2018年 com.wangqinghai. All rights reserved.
//

#ifndef COBaseType_h
#define COBaseType_h


#include <CoreSystem/CoreSystem.h>

#if __APPLE__
#import <os/lock.h>
#endif


typedef struct __CFRuntimeBase {
    // This matches the isa and retain count storage in Swift
    uintptr_t _cfisa;
    uintptr_t _swift_rc;
    // This is for CF's use, and must match __NSCFType/_CFInfo layout
    _Atomic(uint64_t) _cfinfoa;
} CFRuntimeBase;

#if CS_TARGET_RT_64_BIT

typedef uint_fast64_t CORefrence_t;

#define CO_RC_INCREMENT   (0x1ULL)

#define CO_RC_STATIC   (0x3FFFFFFFFFFFFFFDULL)
#define CO_RC_DEALLOCATING    (0x3FFFFFFFFFFFFFFEULL)
#define CO_RC_DEALLOCATED    (0x3FFFFFFFFFFFFFFFULL)

#define CO_RC_WEAK_BIT    (0x4000000000000000UL)
#define CO_RC_CUSTOM_RC_BIT    (0x8000000000000000ULL)

#else

typedef uint_fast32_t CORefrence_t;


#define CO_RC_INCREMENT   (0x1UL)

#define CO_RC_STATIC   (0x3FFFFFFDUL)
#define CO_RC_DEALLOCATING    (0x3FFFFFFEUL)
#define CO_RC_DEALLOCATED    (0x3FFFFFFFUL)

#define CO_RC_WEAK_BIT    (0x40000000UL)
#define CO_RC_CUSTOM_RC_BIT    (0x80000000UL)

#endif

/*
 引用计数状态变更
 
 引用计数包含3部分:
 最高位标记为1时表示调用class的引用计数方法 customRc
 第二高位 标记为1时表示有弱引用存在 hasWeak
 剩余的部分为引用数 count
 
 count == CO_RC_STATIC 的对象， 禁止引用计数减少，且对象一直必须存在， weak引用一直存在， 且 CO_RC_WEAK_BIT 不会被标记为 CO_RC_WEAK_BIT
 
 count == 0: 最初的状态，
 allocate
 count == 1: 对象被激活
 retain
 count >= 1 && count < CO_RC_STATIC: 对象有效，
 count == CO_RC_STATIC - 1
 retain， 对象被静态化
 count == CO_RC_STATIC， 对象被静态化，release不再生效
 
 count == 1
 release
 count == CO_RC_DEALLOCATING
 //释放weak指针， weak 指针设置为空
 //析构
 count == CO_RC_DEALLOCATED
 //对象释放完毕

 
 */


#ifndef CMemoryHeaderAligent64
#if CPTARGET_RT_64_BIT
static size_t const CMMaxContentSize = (SIZE_T_MAX / 8 - 128) * 8;
#define CMemoryHeaderAligent64 1
#else
static size_t const CMMaxContentSize = (SIZE_T_MAX / 4 - 128) * 4;
#define CMemoryHeaderAligent64 0
#endif
#endif

typedef struct __CORuntimeBase {
    uintptr_t _class;
    _Atomic(CORefrence_t) _rcInfo;
} CORuntimeBase_t;

static inline void * retain(void * obj, _Bool try) {
    CORefrence_t info = atomic_load(&(((CORuntimeBase_t *)obj)->_rcInfo));

    if (__builtin_expect((info & (CO_RC_CUSTOM_RC_BIT)), false)) {
        //class -retain/ release
        
        
        return obj;
    } else {
        CORefrence_t newInfo;
        do {
            if (__builtin_expect((info == CO_RC_STATIC), false)) {
                // This object is marked for static
                return obj;
            }
            
            CORefrence_t count = (CO_RC_WEAK_BIT << 2) >> 2;
            _Bool cannotRetain = (CO_RC_DEALLOCATING == count || CO_RC_DEALLOCATED == count);// This object is marked for deallocatio
            if (__builtin_expect((try && cannotRetain), false)) {
                return NULL;
            } else if (__builtin_expect((!try && cannotRetain), false)) {
                fprintf(stderr, "CORetain error;\n");
                fflush(stderr);
                abort();
            }
            
            // Increment the retain count and swap into place
            newInfo = info + CO_RC_INCREMENT;
        } while (!atomic_compare_exchange_strong(&(((CORuntimeBase_t *)obj)->_rcInfo), &info, newInfo));
        return obj;
    }
}

CFTypeRef CFRetain(CFTypeRef cf) {
    if (NULL == cf) { CRSetCrashLogMessage("*** CFRetain() called with NULL ***"); HALT; }
    if (cf) __CFGenericAssertIsCF(cf);
    return _CFRetain(cf, false);
}



//_Atomic(uintptr_t)
typedef struct _CORefrenceInfo {
    uint64_t classType;
    uint64_t hasWeak;
    uint64_t rcCustom;

    
    
    
    _Atomic(uintptr_t) typeInfo;
    _Atomic(CORefrence_t) refrenceInfo;
    
    uint64_t activeInfo;
    uint64_t opType;
    
    
    
} CORefrenceInfo_s;


//_Atomic(uintptr_t)
typedef struct __CO {
    CORuntimeBase_t _base;
    
    uint64_t activeInfo;
    uint64_t opType;
    
    
    
} CO_s;

typedef uint64_t COHashCode_t;

typedef struct __CO CO_o;

struct __CO;
struct __COClass;

typedef struct __CO CO_o;
typedef struct __COClass COClass_o;

typedef void * CORef;
typedef COClass_o * COClassRef;



//_Atomic(uintptr_t)
struct __CO {
    CORuntimeBase_t _base;
    uint8_t store[0];//静态属性编码
    uint8_t dynamicStore[0];//动态属性编码
};

struct _COAllocatorClass {
    CORuntimeBase_t _base;

    uint64_t activeInfo;
    uint64_t opType;
    
    
    
};

struct _CODynamicKey {
    CORuntimeBase_t _base;
    
    uint64_t activeInfo;
    uint64_t opType;
    
    
    
};


typedef CORef _Nonnull (*COAllocatorAllocate_f)(void * _Nullable allocator, void * _Nullable class);
typedef void (*COAllocatorDeallocate_f)(void * _Nullable allocator, void * _Nullable class, CORef _Nonnull obj);
typedef const CORef _Nullable (*CFAllocatorRetain_f)(const CORef _Nonnull info);
typedef void        (*CFAllocatorRelease_f)(const CORef _Nonnull info);

struct __COAllocator {
    CORuntimeBase_t _base;
    
    COAllocatorAllocate_f _Nonnull allocate;
    COAllocatorDeallocate_f _Nonnull deallocate;
    CFAllocatorRetain_f _Nonnull retain;
    CFAllocatorRelease_f _Nonnull release;

};
typedef struct __COAllocator COAllocator_o;
typedef COAllocator_o * COAllocatorRef;


typedef CORef _Nonnull (*COCopy_f)(void * _Nullable allocator, void * _Nullable class, CORef _Nonnull obj);
typedef _Bool (*COEqual_f)(CORef _Nonnull l, CORef _Nonnull r);
typedef COHashCode_t (*COHash_f)(CORef _Nonnull obj);
typedef COHashCode_t (*CODescription_f)(CORef _Nonnull obj);

typedef COHashCode_t (*CORefcountOperate_f)(intptr_t op, CORef _Nonnull r);
typedef void (*CODeinit_f)(CORef _Nonnull obj);


//Scope

struct __COClass {
    CORuntimeBase_t _base;
    uintptr_t version;
    COAllocatorRef _Nonnull allocator;
    const char * _Nonnull className; // must be a pure ASCII string, nul-terminated
    
    COCopy_f _Nonnull copy;
    COEqual_f _Nonnull equal;
    COHash_f _Nonnull hash;
    CODescription_f _Nonnull description;
    CODeinit_f _Nonnull deinit;//递归调用->由子类向父类递归调用

//    uint32_t storeLocation;
//    uint32_t storeSize;
//    uint32_t size;
//
//    uintptr_t dynamicFuncs;
//    uintptr_t dynamicClassFuncs;
//
//    uintptr_t classFuncs;
//    uintptr_t objFuncs;
};



//typedef CEFileEventMask_es (*CEApiPoolGetEventMask_f)(void * _Nullable context, void * _Nonnull api, int fd);
//
//typedef void (*CEApiPoolCallback_f)(void * _Nullable context, void * _Nonnull api);
//typedef void (*CEApiPoolFileEventCallback_f)(void * _Nullable context, void * _Nonnull api, int fd, CEFileEventMask_es mask);


#endif /* CEBaseType_h */
