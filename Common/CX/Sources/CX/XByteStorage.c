//
//  XByteStorage.c
//  
//
//  Created by vector on 2020/4/3.
//

#include "XType.h"
#include "internal/XRuntimeInternal.h"
#include "internal/XAllocator.h"
#include "XMemory.h"

#define _XBufferClearWhenDeallocFlag X_BUILD_UInt(1)
#define _XBufferRcOne X_BUILD_UInt(2)
#define _XBufferRcMax (XFastUIntMax-X_BUILD_UInt(1))

_XBuffer * _Nonnull _XBufferAllocate(XPtr _Nonnull content, XUInt size, XBool clearWhenDealloc) {
    XAssert(size <= (XUIntMax - X_BUILD_UInt(0xf)), __func__, "size to large");
    XUInt bufferSize = (size + 15) & (~X_BUILD_UInt(0xf));
    _XBuffer * buffer = XAlignedAllocate(bufferSize, 16);
    buffer->size = bufferSize;
    _Atomic(XFastUInt) * rcPtr = &(buffer->_refCount);
    XFastUInt rc = _XBufferRcOne;
    if (clearWhenDealloc) {
        rc |= _XBufferClearWhenDeallocFlag;
    }
    atomic_store(rcPtr, rc);
    memcpy(&(buffer->content[0]), content, size);
    
    if (bufferSize > size) {
        memset(&(buffer->content[size]), 0, bufferSize - size);
    }
    return buffer;
}

void __XBufferDeallocate(_XBuffer * _Nonnull buffer) {
    XDeallocate(buffer);
}

XPtr _Nonnull _XBufferGetContent(_XBuffer * _Nonnull buffer) {
    assert(buffer);
    return &(buffer->content);
}


void _XBufferSetClearWhenDealloc(_XBuffer * _Nonnull buffer) {
    assert(buffer);
    _Atomic(XFastUInt) * rcInfoPtr = &(buffer->_refCount);

    XFastUInt rcInfo = 0;
    XFastUInt newRcInfo = 0;
    
    do {
        rcInfo = atomic_load(rcInfoPtr);
        if ((rcInfo & _XBufferClearWhenDeallocFlag) == _XBufferClearWhenDeallocFlag) {
            return;
        }
        assert(rcInfo >= _XBufferRcOne);
        newRcInfo = rcInfo | _XBufferClearWhenDeallocFlag;
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    return;
}


_XBuffer * _Nonnull _XBufferRetain(_XBuffer * _Nonnull buffer) {
    assert(buffer);
    _Atomic(XFastUInt) * rcInfoPtr = &(buffer->_refCount);

    XFastUInt rcInfo = 0;
    XFastUInt newRcInfo = 0;
    
    do {
        rcInfo = atomic_load(rcInfoPtr);
        if (rcInfo >= _XBufferRcMax) {
            return buffer;
        }
        assert(rcInfo >= _XBufferRcOne);
        newRcInfo = rcInfo + _XBufferRcOne;
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
        if (rcInfo >= _XBufferRcMax) {
            return;
        }
        assert(rcInfo >= _XBufferRcOne);
        newRcInfo = rcInfo - _XBufferRcOne;
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    if (newRcInfo == 0) {
        __XBufferDeallocate(buffer);
    }
}

static void _XByteStorageDistory(_XByteStorage * _Nonnull storage) {
    XUInt32 length = storage->content.length;
    if (length < _XByteStorageContentBufferSizeMin) {
        size_t size = sizeof(_XByteStorageContent_t);
        if (length > 4) {
            XUInt bufferSize = (length - 1) & (~X_BUILD_UInt(0x3));
            size += bufferSize;
        }
        memset(&(storage->content), 0, size);
    } else {
        _XByteStorageContentLarge_t * content = (_XByteStorageContentLarge_t *)&(storage->content);
        _XBufferRelease(content->buffer);
        memset(&(storage->content), 0, sizeof(_XByteStorageContentLarge_t));
    }
}

_Static_assert(sizeof(_XByteStorageContent_t) == 8, "error");

static _XByteStorage * _Nonnull _XByteStorageCreate(XCompressedType type, XObjectFlag flag, XUInt8 * _Nullable buffer, XUInt32 length, const char * _Nonnull func) {
    if (length > 0) {
        XAssert(NULL != buffer, func, "length > 0, buffer cannot NULL");
    }
    const _XType_s * cls = NULL;
    if (XCompressedTypeString == type) {
        cls = _XClassString;
    } else if (XCompressedTypeData == type) {
        cls = _XClassData;
    } else {
        XAssert(false, func, "unknown error");
    }
    const _XAllocator_s * allocator = (const _XAllocator_s *)(cls->base.allocator);
    XSize size = sizeof(_XByteStorageContent_t);
    
    XObjectRcFlag rcFlag = 0;
    if ((flag & XObjectFlagStatic) == XObjectFlagStatic) {
        rcFlag |= XObjectRcFlagStatic;
    }
    
    if (length >= _XByteStorageContentBufferSizeMin) {
        size += sizeof(_XByteStorageContentLarge_t) - sizeof(_XByteStorageContent_t);
        XRef ref = allocator->allocateRef(allocator, (XClass)cls, size, rcFlag);
        _XByteStorage * storage = (_XByteStorage *)ref;
        _XByteStorageContentLarge_t * storageContent = (_XByteStorageContentLarge_t *)&(storage->content);
        _XBuffer * buf = _XBufferAllocate(buffer, length, ((flag & XObjectFlagClearWhenDealloc) == XObjectFlagClearWhenDealloc));
        storageContent->length = length;
        storageContent->offset = 0;
        storageContent->hashCode = 0;
        storageContent->hasHashCode = 0;
        storageContent->buffer = 0;
        storageContent->buffer = buf;
        return ref;
    } else {
        XUInt bufferSize = 4;
        if (length > 4) {
            bufferSize = (length + 3) & (~X_BUILD_UInt(0x3));
            size += bufferSize - 4;
        }
        XRef ref = allocator->allocateRef(allocator, (XClass)cls, size, rcFlag);
        _XByteStorage * storage = (_XByteStorage *)ref;
        
        _XByteStorageContentSmall_t * storageContent = (_XByteStorageContentSmall_t *)&(storage->content);
        storageContent->length = length;
        if (length > 0) {
            memcpy(&(storageContent->extended[0]), buffer, length);
        }

        if (bufferSize > length) {
            memset(&(storageContent->extended[length]), 0, bufferSize - length);
        }
        return ref;
    }
}

static _XByteStorage * _Nonnull _XByteStorageCreateWithBuffer(XCompressedType type, XObjectFlag flag, _XBuffer * _Nonnull xbuffer, XUInt32 offset, XUInt32 length, const char * _Nonnull func) {
    XAssert(NULL != xbuffer, func, "length > 0, buffer cannot NULL");
    XAssert(length <= xbuffer->size, func, "range error");
    XAssert(offset <= xbuffer->size - length, func, "range error");
    
    const _XType_s * cls = NULL;
    if (XCompressedTypeString == type) {
        cls = _XClassString;
    } else if (XCompressedTypeData == type) {
        cls = _XClassData;
    } else {
        XAssert(false, func, "unknown error");
    }
    const _XAllocator_s * allocator = (const _XAllocator_s *)(cls->base.allocator);
    XSize size = sizeof(_XByteStorageContent_t);
    
    XObjectRcFlag rcFlag = 0;
    if ((flag & XObjectFlagStatic) == XObjectFlagStatic) {
        rcFlag |= XObjectRcFlagStatic;
    }
    
    if (length >= _XByteStorageContentBufferSizeMin) {
        size += sizeof(_XByteStorageContentLarge_t) - sizeof(_XByteStorageContent_t);
        XRef ref = allocator->allocateRef(allocator, (XClass)cls, size, rcFlag);
        _XByteStorage * storage = (_XByteStorage *)ref;
        _XByteStorageContentLarge_t * storageContent = (_XByteStorageContentLarge_t *)&(storage->content);

        _XBuffer * buf = _XBufferRetain(xbuffer);
        if ((flag & XObjectFlagClearWhenDealloc) == XObjectFlagClearWhenDealloc) {
            _XBufferSetClearWhenDealloc(buf);
        }
        storageContent->buffer = buf;
        storageContent->offset = offset;
        storageContent->length = length;
        return ref;
    } else {
        XUInt bufferSize = 4;
        if (length > 4) {
            bufferSize = (length + 3) & (~X_BUILD_UInt(0x3));
            size += bufferSize - 4;
        }
        
        XUInt8 * buffer = &(xbuffer->content[offset]);
        XRef ref = allocator->allocateRef(allocator, (XClass)cls, size, rcFlag);
        _XByteStorage * storage = (_XByteStorage *)ref;
        _XByteStorageContentSmall_t * storageContent = (_XByteStorageContentSmall_t *)&(storage->content);
        storage->content.length = length;
        if (length > 0) {
            memcpy(&(storageContent->extended[0]), buffer, length);
        }
        if (bufferSize > length) {
            memset(&(storageContent->extended[length]), 0, bufferSize - length);
        }
        return ref;
    }
}



