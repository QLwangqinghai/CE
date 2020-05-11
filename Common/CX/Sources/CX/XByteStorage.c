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



/*
 TaggedObject32

 refType: 1, value = 1
 taggedContent: {
    class: 2
    objectContent: 28 {
        len: 4
        content: 24
    }
 }
 flag: 1, value = 1
*/
/*
 TaggedObject64

 refType: 2, value = 1
 taggedContent: 61 {
    class: 2
    objectContent: 59 {
       len: 3
       content: 56
   }
 }
 flag: 1, value = 1
*/

#if BUILD_TARGET_RT_64_BIT
#define X_BUILD_TaggedByteStorageContentMask 0xFFFFFFFFFFFFFFULL
#define X_BUILD_TaggedByteStorageContentLengthMask 0x7ULL
#define X_BUILD_TaggedByteStorageContentLengthShift 57ULL
#define X_BUILD_TaggedByteStorageContentLengthMax 0x7UL
#else
#define X_BUILD_TaggedByteStorageContentMask 0xFFFFFFUL
#define X_BUILD_TaggedByteStorageContentLengthMask 0xFUL
#define X_BUILD_TaggedByteStorageContentLengthShift 25ULL
#define X_BUILD_TaggedByteStorageContentLengthMax 0x3UL
#endif




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
        storageContent->buffer = 0;
        storageContent->buffer = buf;
        atomic_store(&(storageContent->hashCode), XHash32NoneFlag);
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

static _XByteStorage * _Nonnull __XRefAsString(XRef _Nonnull ref, const char * _Nonnull func) {
    XCompressedType compressedType = XCompressedTypeNone;
    
#if BUILD_TARGET_RT_64_BIT
    __unused
#endif
    XClass info = _XRefGetUnpackedType(ref, &compressedType, func);
    
#if BUILD_TARGET_RT_64_BIT
    XAssert(XCompressedTypeString == compressedType, func, "not String instance");
    return (_XByteStorage *)ref;
#else
    const _XType_s * type = (const _XType_s *)info;
    XAssert(type->base.identifier == _XClassTable[X_BUILD_CompressedType_String - 1].base.identifier, func, "not String instance");
    return (_XByteStorage *)ref;
#endif
}

static _XByteStorage * _Nonnull __XRefAsData(XRef _Nonnull ref, const char * _Nonnull func) {
    XCompressedType compressedType = XCompressedTypeNone;
    
#if BUILD_TARGET_RT_64_BIT
    __unused
#endif
    XClass info = _XRefGetUnpackedType(ref, &compressedType, func);
    
#if BUILD_TARGET_RT_64_BIT
    XAssert(XCompressedTypeData == compressedType, func, "not Data instance");
    return (_XByteStorage *)ref;
#else
    const _XType_s * type = (const _XType_s *)info;
    XAssert(type->base.identifier == _XClassTable[X_BUILD_CompressedType_Data - 1].base.identifier, func, "not Data instance");
    return (_XByteStorage *)ref;

#endif
}


//typedef struct {
//    XUInt32 length;
//    XUInt32 hashCode;
//    XUInt32 hasHashCode: 1;
//    XUInt32 __unuse: 31;
//    XUInt8 * _Nonnull buffer;
//} _XByteStorageUnpacked_t;

void _XByteStorageUnpack(XPtr _Nonnull ref, XBool isString, _XByteStorageUnpacked_t * _Nonnull ptr, XBool requireHashCode, const char * _Nonnull func) {
    
#if BUILD_TARGET_RT_64_BIT
    XUInt64 v = (XUInt64)((uintptr_t)ref);
    if ((v & X_BUILD_TaggedMask) == X_BUILD_TaggedObjectFlag) {
        XUInt64 clsId = v & X_BUILD_TaggedObjectClassMask;
        if (isString) {
            XAssert(X_BUILD_TaggedObjectClassString == clsId, func, "not String instance");
        } else {
            XAssert(X_BUILD_TaggedObjectClassData == clsId, func, "not Data instance");
        }
        
        XUInt64 storageContent = (v >> 1) & X_BUILD_TaggedByteStorageContentMask;
        XUInt64 len = (v >> X_BUILD_TaggedByteStorageContentLengthShift) & X_BUILD_TaggedByteStorageContentLengthMask;
        XAssert(len <= X_BUILD_TaggedByteStorageContentLengthMax, func, "");
        
        XUInt64 bytes = storageContent;
        *((XUInt64 *)ptr->tmp) = bytes;

#if BUILD_TARGET_RT_BIG_ENDIAN
        ptr->buffer = ptr->tmp + (sizeof(XUInt64) - len);
#else
        ptr->buffer = ptr->tmp;
#endif
        ptr->length = (XUInt32)len;
        if (requireHashCode) {
            ptr->hashCode = _XELFHashBytes(ptr->buffer, (XUInt32)len) & XHash32Mask;
        } else {
            ptr->hashCode = 0;
        }
        return;
    }
#else
    XUInt32 v = (XUInt32)((uintptr_t)ref);
    if ((v & X_BUILD_TaggedMask) == X_BUILD_TaggedObjectFlag) {
        XUInt32 clsId = v & X_BUILD_TaggedObjectClassMask;
        if (isString) {
            XAssert(X_BUILD_TaggedObjectClassString == clsId, func, "not String instance");
        } else {
            XAssert(X_BUILD_TaggedObjectClassData == clsId, func, "not Data instance");
        }
        
        XUInt32 storageContent = (v >> 1) & X_BUILD_TaggedByteStorageContentMask;
        XUInt32 len = (v >> X_BUILD_TaggedByteStorageContentLengthShift) & X_BUILD_TaggedByteStorageContentLengthMask;
        XAssert(len <= X_BUILD_TaggedByteStorageContentLengthMax, __func__, "");
            
        uint8_t * bytes = (uint8_t *)&storageContent;
        
#if BUILD_TARGET_RT_BIG_ENDIAN
        ptr->buffer = ptr->tmp + (sizeof(XUInt32) - len);
#else
        ptr->buffer = ptr->tmp;
#endif
        ptr->length = (XUInt32)len;
        ptr->buffer = ptr->tmp;
        if (requireHashCode) {
            ptr->hashCode = _XELFHashBytes(ptr->buffer, len) & XHash32Mask;
            ptr->hasHashCode = 1;
        } else {
            ptr->hashCode = 0;
            ptr->hasHashCode = 0;
        }
        return;
    }
#endif
    _XByteStorage * _Nonnull storage = NULL;
    if (isString) {
        storage = __XRefAsString(ref, func);
    } else {
        storage = __XRefAsData(ref, func);
    }
    
    ptr->length = storage->content.length;
    
    if (storage->content.length >= _XByteStorageContentBufferSizeMin) {
        _XByteStorageContentLarge_t * storageContent = (_XByteStorageContentLarge_t *)&(storage->content);
        ptr->buffer = &(storageContent->buffer->content[storageContent->offset]);
        XFastUInt32 hashCode = atomic_load(&(storageContent->hashCode));
        if ((hashCode & XHash32NoneFlag) == XHash32NoneFlag) {
            if (requireHashCode) {
                hashCode = _XELFHashBytes(ptr->buffer, MIN(XHashEverythingLimit, ptr->length)) & XHash32Mask;
                atomic_store(&(storageContent->hashCode), hashCode);
            }
        }
        ptr->hashCode = hashCode;
    } else {
        _XByteStorageContentSmall_t * storageContent = (_XByteStorageContentSmall_t *)&(storage->content);
        ptr->buffer = storageContent->extended;
        if (requireHashCode) {
            ptr->hashCode = _XELFHashBytes(ptr->buffer, MIN(XHashEverythingLimit, ptr->length)) & XHash32Mask;
        } else {
            ptr->hashCode = XHash32NoneFlag;
        }
    }
}

XHashCode XStringHash(XRef _Nonnull ref) {
    _XByteStorageUnpacked_t v = {};
    _XByteStorageUnpack(ref, true, &v, true, __func__);
    return v.hashCode;
}
XHashCode XDataHash(XRef _Nonnull ref) {
    _XByteStorageUnpacked_t v = {};
    _XByteStorageUnpack(ref, false, &v, true, __func__);
    return v.hashCode;
}

XData _Nonnull XDataCreate(XObjectFlag flag, XUInt8 * _Nullable bytes, XUInt32 length) {
    if (length == 0) {
        return XDataEmpty;
    }
    return _XByteStorageCreate(XCompressedTypeData, flag, bytes, length, __func__);
}


typedef XUInt32 XStringEncoding;

static const XStringEncoding XStringEncodingUtf8 = 1;

XData _Nonnull XDataCreateByEncodeString(XObjectFlag flag, XString _Nonnull string, XStringEncoding encode) {
//    if (length == 0) {
//        return XDataEmpty;
//    }
//    return _XByteStorageCreate(XCompressedTypeData, flag, bytes, length, __func__);
    return NULL;
}

XString _Nonnull XStringCreateByDecodeData(XObjectFlag flag, XData _Nonnull data, XStringEncoding encode) {
    XAssert(NULL != data, __func__, "data is NULL");
    XSize size = strlen(cString);
    XAssert(size < XUInt32Max, __func__, "cString len error");
    XUInt32 length = (XUInt32)size;
    if (length == 0) {
        return XDataEmpty;
    }
    return _XByteStorageCreate(XCompressedTypeData, flag, (XUInt8 *)cString, length, __func__);
    return NULL;
}
