//
//  XObject.c
//  
//
//  Created by vector on 2020/4/4.
//

#include "internal/XRuntimeInternal.h"
#include "XAtomic.h"
#include "internal/XAllocator.h"
#include "XMemory.h"

typedef XPtr _XDescriptionBuffer;

typedef void (*XObjectDeinit_f)(XObject _Nonnull obj);
typedef void (*XObjectDescribe_f)(XObject _Nonnull obj, _XDescriptionBuffer buffer);
typedef XObject _Nonnull (*XObjectCopy_f)(XObject _Nonnull obj);


#pragma pack(push, 1)



#pragma pack(pop)



//typedef struct {
//    CCDomainRefType_s types[0x1000];
//    UInteger count;
//} CCDomainRefTypeTable_s;
//
//static CCDomainRefTypeTable_s _CCDomainRefTypeTable = {
//    .types = {
//        {
//            .name = "CCClosure",
//            .deinit = _CCClosureDeinit,
//        }
//        
//    },
//    .count = 1,
//};

_XBuffer * _Nonnull _XBufferAllocate(XUInt size) {
    assert(size <= (XUIntMax >> 1));
    XSize s = size;
    if (s < 8) {
        s = 8;
    }
    _XBuffer * buffer = XAlignedAllocate(size, 8);
    buffer->size = size;
    _Atomic(XFastUInt) * rcPtr = &(buffer->_refCount);
    atomic_store(rcPtr, 1);
    return buffer;
}

void __XBufferDeallocate(_XBuffer * _Nonnull buffer) {
    XDeallocate(buffer);
}

XPtr _Nonnull _XBufferGetContent(_XBuffer * _Nonnull buffer) {
    assert(buffer);
    return &(buffer->content);
}

_XBuffer * _Nonnull _XBufferRetain(_XBuffer * _Nonnull buffer) {
    assert(buffer);
    _Atomic(XFastUInt) * rcInfoPtr = &(buffer->_refCount);

    XFastUInt rcInfo = 0;
    XFastUInt newRcInfo = 0;
    
    do {
        rcInfo = atomic_load(rcInfoPtr);
        if (rcInfo == XFastUIntMax) {
            return buffer;
        }
        assert(rcInfo > 0);
        newRcInfo = rcInfo + 1;
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));

    return buffer;
}

void _XBufferRelease(_XBuffer * _Nonnull buffer) {
    assert(buffer);
    _Atomic(XFastUInt) * rcInfoPtr = &(buffer->_refCount);

    XFastUInt rcInfo = 0;
    XFastUInt newRcInfo = 0;

    do {
        rcInfo = atomic_load(rcInfoPtr);
        if (rcInfo == XFastUIntMax) {
            return;
        }
        assert(rcInfo > 0);
        newRcInfo = rcInfo - 1;
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    if (newRcInfo == 0) {
        __XBufferDeallocate(buffer);
    }
}



#define ELF_STEP(B) T1 = (H << 4) + B; T2 = T1 & 0xF0000000; if (T2) T1 ^= (T2 >> 24); T1 &= (~T2); H = T1;

XUInt32 _XELFHashBytes(XUInt8 * _Nullable bytes, XUInt32 length) {
    if (length > 0) {
        assert(bytes);
    }
    XUInt32 H = 0, T1, T2;
    XUInt32 rem = length;
    while (3 < rem) {
    ELF_STEP(bytes[length - rem]);
    ELF_STEP(bytes[length - rem + 1]);
    ELF_STEP(bytes[length - rem + 2]);
    ELF_STEP(bytes[length - rem + 3]);
    rem -= 4;
    }
    switch (rem) {
    case 3:  ELF_STEP(bytes[length - 3]);
    case 2:  ELF_STEP(bytes[length - 2]);
    case 1:  ELF_STEP(bytes[length - 1]);
    case 0:  ;
    }
    return H;
}

#undef ELF_STEP






XBool XObjectEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {
    assert(XRefKindInstance == XRefGetKind(lhs));
    assert(XRefKindInstance == XRefGetKind(rhs));
    return lhs == rhs;
};

XHashCode XObjectHash(XRef _Nonnull obj) {
    assert(XRefKindInstance == XRefGetKind(obj));
    return (XHashCode)(((uintptr_t)obj) >> 4);
};

