//
//  CCReferenceBase.c
//  SITP
//
//  Created by vector on 2020/1/20.
//  Copyright © 2020 angfung. All rights reserved.
//

#include "CCReferenceBase.h"
#include "CCClosure.h"

const uint32_t CCDomain = 0;

void _CCDomainRefDeinitCallback(CCPtr _Nonnull context, CCRef _Nonnull ref);

typedef struct {
    const char * name;
    CCRefDeinit_f _Nonnull deinit;
} CCDomainRefType_s;

typedef struct {
    CCDomainRefType_s types[0x1000];
    CCUInt count;
} CCDomainRefTypeTable_s;




static CCDomainRefTypeTable_s _CCDomainRefTypeTable = {
    .types = {
        {
            .name = "CCClosure",
            .deinit = _CCClosureDeinit,
        }
        
    },
    .count = 1,
};

static CCDomainRefTypeTable_s * _Nonnull _CCDomainRefTypeTablePtr = &_CCDomainRefTypeTable;


uint32_t CCDomainRefTypeTableRegister(const char * _Nonnull name, CCRefDeinit_f _Nonnull deinit) {
    assert(name);
    assert(deinit);
    CCDomainRefType_s type = {
        .name = name,
        .deinit = deinit,
    };
    uint32_t id = (uint32_t)(_CCDomainRefTypeTablePtr->count);
    _CCDomainRefTypeTablePtr->types[id] = type;
    _CCDomainRefTypeTablePtr->count += 1;
    return id;
}

void _CCDomainRefDeinitCallback(CCPtr _Nonnull context, CCRef _Nonnull ref) {
    CCDomainRefTypeTable_s * table = (CCDomainRefTypeTable_s *)context;
    
    _Atomic(uintptr_t) * typePtr = (_Atomic(uintptr_t) *)ref;
    uintptr_t type = atomic_load(typePtr);
    assert((type & 1) == 1);
    CCRefType typeId = (type >> 1) & 0xfff;
    (table->types[typeId]).deinit(ref);
}

typedef struct {
    CCPtr _Nonnull context;
    CCRefDomainDeinitCallback_f _Nonnull deinit;
} CCRefDomain_s;

typedef struct {
    CCUInt count;
    CCRefDomain_s domains[128];
} CCRefDomainTable_s;

static CCRefDomainTable_s _CCRefDomainTable = {
    .domains = {
        {
            .context = &_CCDomainRefTypeTable,
            .deinit = _CCDomainRefDeinitCallback,
        }
        
    },
    .count = 1,
};
CCRefDomain_s * _Nonnull _CCRefDomainTableGetItem(CCUIntFast id) {
    return &((&_CCRefDomainTable)->domains[id]);
}




typedef struct {
    CCUInt counter;
    CCUInt hash;
    uintptr_t value;
} CCRefWeakContainer_t;

void CCRefWeakTableLock(uintptr_t ref);
void CCRefWeakTableUnlock(uintptr_t ref);

void CCRefWeakTableLockByHash(CCUInt hash);
void CCRefWeakTableUnlockByHash(CCUInt hash);
void _CCRefWeakTableRemoveRef(uintptr_t ref);



const CCRefType CCTypeClosure = 0;


typedef struct {
    _Atomic(CCUIntFast) info;
} CCRefDecreasedBase;


//32:  flag:1b type(4k):12b weak:1 count:18
//64   flag:1b type(4k):12b weak:1 count:50

//32:  life:7b weak:1 count:24
//64   life:7b weak:1 count:56


typedef CCUInt CCRefCounterType;


#pragma pack(push, 1)

typedef struct {
    uintptr_t flag: 1;
    uintptr_t type: 12;
    uintptr_t hasWeak: 1;
#if CCBuild64Bit
    uintptr_t count: 50;
#else
    uintptr_t count: 18;
#endif
} CCRefDecreasedBaseInfo;


typedef struct {
#if CCBuild64Bit
    CCUInt count: 56;
#else
    CCUInt count: 24;
#endif
    CCUInt hasWeak: 1;
    CCUInt domain: 7;
} CCRefCounter;


typedef struct {
    _Atomic(uintptr_t) type;
    _Atomic(CCUIntFast) counter;
} CCRefBase;

#pragma pack(pop)


static inline CCBool __CCRefIsDecreasedType(_Atomic(uintptr_t) * _Nonnull typePtr) {
    uintptr_t type = atomic_load(typePtr);
    return ((type & 1) == 1);
}

#define CCRefDecreasedRcOne 0x4000
#define CCRefDecreasedRcTwo 0x8000
#define CCRefDecreasedRcThree 0xC000


CCPtr _Nonnull CCRefGetContentPtr(CCRef _Nonnull ref) {
    assert(ref);
    _Atomic(uintptr_t) * typePtr = (_Atomic(uintptr_t) *)ref;
    if (__CCRefIsDecreasedType(typePtr)) {
        return typePtr + 1;
    } else {
        CCRefBase * base = (CCRefBase *)ref;
        return base + 1;
    }
}

/*
 count == 1 标识 正在deallocing/dealloced
 count == 2 标识 随时可以dealloc
  */


#define CCRefDecreasedTypeAndFlagMask 0x1fff
#define CCRefDecreasedRcOne 0x4000
#define CCRefDecreasedRcTwo 0x8000
#define CCRefDecreasedRcThree 0xC000

static const uintptr_t __CCRefDecreasedRcMask = ~((uintptr_t)(0x3FFF));
static const uintptr_t __CCRefDecreasedRcHasWeakMask = 0x2000;


#if CCBuild64Bit

static const CCUIntFast __CCRefNormalDomainMask = 0xFE00000000000000ULL;
static const CCUIntFast __CCRefNormalRcMask = 0xFFFFFFFFFFFFFFULL;
static const CCUIntFast __CCRefNormalHasWeakMask = 0x100000000000000ULL;
static const CCUIntFast __CCRefNormalDomainShift = 57;

#else

static const CCUIntFast __CCRefNormalDomainMask = 0xFE000000UL;
static const CCUIntFast __CCRefNormalRcMask = 0xFFFFFFUL;
static const CCUIntFast __CCRefNormalHasWeakMask = 0x1000000UL;
static const CCUIntFast __CCRefNormalDomainShift = 25;

#endif



static inline _Atomic(CCUIntFast) * _Nonnull CCRefNormalGetCounterPtr(CCRef _Nonnull ref) {
    return &(((CCRefBase *)ref)->counter);
}

static inline CCBool __CCRefNormalRetain(CCRef _Nonnull ref) {
    CCUIntFast rcInfo = 0;
    CCUIntFast newRcInfo = 0;
    _Atomic(CCUIntFast) * rcInfoPtr = CCRefNormalGetCounterPtr(ref);
    
    do {
        rcInfo = atomic_load(rcInfoPtr);
        if ((rcInfo & __CCRefNormalRcMask) <= 1) {
            CCLogError("CCRefNormalRetain ref is dealloced\n");
            abort();
        }
        if ((rcInfo & __CCRefNormalRcMask) == __CCRefNormalRcMask) {
            return true;
        }
        newRcInfo = rcInfo + 1;
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    return true;
}

static inline CCBool __CCRefDecreasedRetain(_Atomic(uintptr_t) * _Nonnull rcInfoPtr) {
    uintptr_t rcInfo = 0;
    uintptr_t newRcInfo = 0;
    do {
        rcInfo = atomic_load(rcInfoPtr);
        if ((rcInfo & __CCRefDecreasedRcMask) <= CCRefDecreasedRcOne) {
            CCLogError("CCRefNormalRetain ref is dealloced\n");
            abort();
        }
        if ((rcInfo & __CCRefDecreasedRcMask) == __CCRefDecreasedRcMask) {
            return true;
        }
        newRcInfo = rcInfo + CCRefDecreasedRcOne;
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    return true;
}

static inline CCBool __CCRefRetain(CCRef _Nonnull ref) {
    _Atomic(uintptr_t) * typePtr = (_Atomic(uintptr_t) *)ref;
    if (__CCRefIsDecreasedType(typePtr)) {
        return __CCRefDecreasedRetain(typePtr);
    } else {
        return __CCRefNormalRetain(ref);
    }
}


/*
理论依据：
CCRefWeakContainerCreate 会先对对象retain， 对象一定是个有效对象， counter >= 3
如果存在弱引用，则加锁
weak标记只能在加锁中 修改
引用计数充当了自旋锁作用
*/
static inline CCBool __CCRefNormalSetDeallocing(CCRef _Nonnull ref) {
    CCUIntFast rcInfo = 0;
    CCUIntFast newRcInfo = 0;
    _Atomic(CCUIntFast) * rcInfoPtr = CCRefNormalGetCounterPtr(ref);

    uintptr_t address = (uintptr_t)ref;
    while (1) {
        rcInfo = atomic_load(rcInfoPtr);
        if ((rcInfo & __CCRefNormalRcMask) < 2) {
            CCLogError("__CCRefNormalSetDeallocing error\n");
            abort();
        } else if ((rcInfo & __CCRefNormalRcMask) > 2) {
            return false;
        }
        newRcInfo = (rcInfo & __CCRefNormalDomainMask) + 1;
        if ((rcInfo & __CCRefNormalHasWeakMask) == __CCRefNormalHasWeakMask) {
            CCRefWeakTableLock(address);
            CCBool result = (atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
            if (result) {
                _CCRefWeakTableRemoveRef(address);
            }
            CCRefWeakTableUnlock(address);
            if (result) {
                return true;
            }
        } else {
            CCBool result = (atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
            if (result) {
                return true;
            }
        }
    }
}

static inline CCBool __CCRefDecreasedSetDeallocing(CCRef _Nonnull ref) {
    _Atomic(uintptr_t) * rcInfoPtr = (_Atomic(uintptr_t) *)ref;
    uintptr_t rcInfo = atomic_load(rcInfoPtr);
    uintptr_t newRcInfo = (rcInfo & CCRefDecreasedTypeAndFlagMask) + CCRefDecreasedRcOne;
    uintptr_t address = (uintptr_t)ref;

    while (1) {
        rcInfo = atomic_load(rcInfoPtr);
        if ((rcInfo & __CCRefDecreasedRcMask) < CCRefDecreasedRcTwo) {
            CCLogError("__CCRefDecreasedSetDeallocing error\n");
            abort();
        } else if ((rcInfo & __CCRefDecreasedRcMask) > CCRefDecreasedRcThree) {
            return false;
        }
        if ((rcInfo & __CCRefDecreasedRcHasWeakMask) == __CCRefDecreasedRcHasWeakMask) {
            CCRefWeakTableLock(address);
            CCBool result = (atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
            if (result) {
                _CCRefWeakTableRemoveRef(address);
            }
            CCRefWeakTableUnlock(address);
            if (result) {
                return true;
            }
        } else {
            CCBool result = (atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
            if (result) {
                return true;
            }
        }
    }
}



static inline void __CCRefDecreasedRelease(CCRef _Nonnull ref) {
    _Atomic(uintptr_t) * rcInfoPtr = (_Atomic(uintptr_t) *)ref;

    uintptr_t rcInfo = 0;
    uintptr_t newRcInfo = 0;
    do {
        rcInfo = atomic_load(rcInfoPtr);
        if ((rcInfo & __CCRefDecreasedRcMask) < CCRefDecreasedRcThree) {
            CCLogError("CCRefRelease error\n");
            abort();
        }
        if ((rcInfo & __CCRefDecreasedRcMask) == __CCRefDecreasedRcMask) {
            return;
        }
        newRcInfo = rcInfo - CCRefDecreasedRcOne;
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    if ((newRcInfo & __CCRefDecreasedRcMask) == CCRefDecreasedRcTwo) {
        if (__CCRefDecreasedSetDeallocing(ref)) {
            CCRefType typeId = (rcInfo >> 1) & 0xfff;
            CCRefDomain_s * domain = _CCRefDomainTableGetItem(typeId);
            domain->deinit(domain->context, ref);
            CCDeallocate(ref);
        }
    }
}

static inline void __CCRefNormalRelease(CCRef _Nonnull ref) {
    CCUIntFast rcInfo = 0;
    CCUIntFast newRcInfo = 0;
    
    _Atomic(CCUIntFast) * rcInfoPtr = CCRefNormalGetCounterPtr(ref);
    do {
        rcInfo = atomic_load(rcInfoPtr);
        if ((rcInfo & __CCRefNormalRcMask) < 3) {
            CCLogError("CCRefRelease error\n");
            abort();
        }
        if ((rcInfo & __CCRefNormalRcMask) == __CCRefNormalRcMask) {
            return;
        }
        newRcInfo = rcInfo - 1;
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    if ((newRcInfo & __CCRefNormalRcMask) == 2) {
        if (__CCRefNormalSetDeallocing(ref)) {
            CCRefDomain_s * domain = _CCRefDomainTableGetItem(newRcInfo >> __CCRefNormalDomainShift);
            domain->deinit(domain->context, ref);
            CCDeallocate(ref);
        }
    }
}


void CCRefRelease(CCRef _Nonnull ref) {
    if (NULL == ref) {
        CCLogError("CCRefRelease ref is NULL");
        abort();
    }
    _Atomic(uintptr_t) * typePtr = (_Atomic(uintptr_t) *)ref;
    uintptr_t type = atomic_load(typePtr);
    if (((type & 1) == 1)) {
        __CCRefDecreasedRelease(ref);
    } else {
        __CCRefNormalRelease(ref);
    }
}


static inline void __CCRefNormalInit(CCRefBase * _Nonnull ref, uint32_t domain, CCRefType type) {
    ref->type = type;

    CCUIntFast rcInfo = domain;
    rcInfo = rcInfo << __CCRefNormalDomainShift;
    rcInfo += 3;
    _Atomic(CCUIntFast) * rcInfoPtr = CCRefNormalGetCounterPtr(ref);
    atomic_store(rcInfoPtr, rcInfo);
}

CCRef _Nonnull CCRefAllocate(uint32_t domain, CCRefType type, size_t contentSize) {
    assert(contentSize >= 0);
        
    assert(domain < _CCRefDomainTable.count);
    
    if (0 == domain) {
        assert(type <= _CCDomainRefTypeTable.count);
        
        size_t size = sizeof(_Atomic(uintptr_t)) + contentSize;
        CCPtr ptr = CCMalloc(size);
        if (NULL == ptr) {
            CCLogError("CCRefAllocate ptr is NULL\n");
            abort();
        }
        _Atomic(uintptr_t) * _Nonnull baseRef = ptr;
        uintptr_t rcInfo = CCRefDecreasedRcThree + type;
        atomic_store(baseRef, rcInfo);
        return ptr;
    } else {
        assert(type);
        assert((type & 1) == 0);

        size_t size = sizeof(CCRefBase) + contentSize;
        CCPtr ptr = CCMalloc(size);
        if (NULL == ptr) {
            CCLogError("CCRefAllocate ptr is NULL\n");
            abort();
        }
        CCRefBase * _Nonnull baseRef = ptr;
        __CCRefNormalInit(baseRef, domain, type);
        return ptr;
    }
}

void CCRefRetain(CCRef _Nonnull ref) {
    if (NULL == ref) {
        CCLogError("CCRefRetain NULL");
        abort();
    };
    if (!__CCRefRetain(ref)) {
        CCLogError("CCRefRetain error, retain a deallocing object\n");
        abort();
    }
}

CCRef _Nullable CCRefTryRetain(CCRef _Nonnull ref) {
    if (NULL == ref) {
        return ref;
    };
    if (__CCRefRetain(ref)) {
        return ref;
    } else {
        return NULL;
    }
}


static inline CCBool __CCRefNormalSetHasWeak(CCRef _Nonnull ref, CCBool hasWeak) {
    CCUIntFast rcInfo = 0;
    CCUIntFast newRcInfo = 1;
    _Atomic(CCUIntFast) * rcInfoPtr = CCRefNormalGetCounterPtr(ref);

    do {
        rcInfo = atomic_load(rcInfoPtr);
        if (((rcInfo & __CCRefNormalHasWeakMask) == __CCRefNormalHasWeakMask) == hasWeak) {
            return false;
        }
        if ((rcInfo & __CCRefNormalRcMask) <= 2) {
            return false;
        }
        newRcInfo = rcInfo;
        if (hasWeak) {
            newRcInfo = newRcInfo | __CCRefNormalHasWeakMask;
        } else {
            newRcInfo = newRcInfo & (~__CCRefNormalHasWeakMask);
        }
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    return true;
}

static inline CCBool __CCRefDecreasedSetHasWeak(CCRef _Nonnull ref, CCBool hasWeak) {
    _Atomic(uintptr_t) * rcInfoPtr = (_Atomic(uintptr_t) *)ref;

    uintptr_t rcInfo = 0;
    uintptr_t newRcInfo = 9;
    
    do {
        rcInfo = atomic_load(rcInfoPtr);
        if (((rcInfo & __CCRefDecreasedRcHasWeakMask) == __CCRefDecreasedRcHasWeakMask) == hasWeak) {
            return false;
        }
        if ((rcInfo & __CCRefDecreasedRcMask) <= CCRefDecreasedRcTwo) {
            return false;
        }
        newRcInfo = rcInfo;
        if (hasWeak) {
            newRcInfo = newRcInfo | __CCRefDecreasedRcHasWeakMask;
        } else {
            newRcInfo = newRcInfo & (~__CCRefDecreasedRcHasWeakMask);
        }
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    return true;
}

CCBool _CCRefSetHasWeak(CCRef _Nonnull ref, CCBool hasWeak) {
    assert(ref);
    _Atomic(uintptr_t) * typePtr = (_Atomic(uintptr_t) *)ref;
    uintptr_t type = atomic_load(typePtr);
    if ((type & 1) == 1) {
        return __CCRefNormalSetHasWeak(ref, hasWeak);
    } else {
        return __CCRefDecreasedSetHasWeak(ref, hasWeak);
    }
}




void CCRefWeakTableLockByHash(CCUInt hash) {
    
}
void CCRefWeakTableUnlockByHash(CCUInt hash) {
    
}
void CCRefWeakTableLock(uintptr_t ref) {

};
void CCRefWeakTableUnlock(uintptr_t ref) {

};


CCRefWeakContainer_t * _Nullable _CCRefWeakTableGetContainer(uintptr_t ref) {
    
    return NULL;
}
void _CCRefWeakTableSetContainer(CCRefWeakContainer_t * _Nonnull container) {
    
}
void _CCRefWeakTableRemoveContainer(CCRefWeakContainer_t * _Nonnull container) {
    if ((uintptr_t)NULL != container->value) {
        CCRefWeakContainer_t * current = _CCRefWeakTableGetContainer(container->value);
        if (current == container) {
            //移除
            _CCRefWeakTableRemoveRef(container->value);
        }
    }
}
void _CCRefWeakTableRemoveRef(uintptr_t ref) {
    assert(ref != (uintptr_t)NULL);
    CCRefWeakContainer_t * container = _CCRefWeakTableGetContainer(ref);
    container->value = (uintptr_t)NULL;
    assert(_CCRefSetHasWeak((CCRef)ref, false));
}

//CCWeakContainerRef CCWeakContainerCreate() {};
CCRefWeakContainerPtr _Nonnull CCRefWeakContainerCreate(CCRef _Nonnull ref) {
    assert(ref);
    CCRefRetain(ref);
    uintptr_t address = (uintptr_t)ref;
    CCRefWeakTableLock(address);
    CCRefWeakContainer_t * container = _CCRefWeakTableGetContainer(address);//从map中获取
    if (container) {
        container->counter += 1;
    } else {
        container = NULL;//初始化一个
        
        //把container存储到map中
        _CCRefWeakTableSetContainer(container);
        
        
        //添加weak 标记 失败就assert
        assert(_CCRefSetHasWeak(ref, true));
    }
    CCRefWeakTableUnlock(address);
    CCRefRelease(ref);
    return container;
};
void CCRefWeakContainerDestroy(CCRefWeakContainerPtr _Nonnull container) {
    CCRefWeakContainer_t * ptr = container;
    CCUInt hash = ptr->hash;
    CCRefWeakTableLockByHash(hash);
    assert(ptr->counter > 0);
    ptr->counter -= 1;
    if (ptr->counter == 0) {
        //释放对象， 从map中移除
        _CCRefWeakTableRemoveContainer(ptr);
    }
    CCRefWeakTableUnlockByHash(hash);
}




//static inline CCBool CCRefIsTagged(CCRef _Nonnull ref) {
//    assert(ref);
//    uintptr_t v = (uintptr_t)ref;
//    return (v & 0x1) ? (CCUInt8)((v >> 1) & 0x7) : 255;
//}
//
//static inline CCUInt8 CCRefGetTaggedTypeCode(CCRef _Nonnull ref) {
//    uintptr_t v = (uintptr_t)ref;
//    assert(v & 0x1);
//
//    return (CCUInt8)((v >> 1) & 0x7);
//}
//
//static inline CCUInt8 CCRefGetTaggedSpecific(CCRef _Nonnull ref) {
//    uintptr_t v = (uintptr_t)ref;
//    assert(v & 0x1);
//    return (v >> 4) & 0x7;
//}
