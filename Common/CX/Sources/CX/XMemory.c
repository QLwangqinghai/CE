//
//  File.c
//  
//
//  Created by vector on 2020/4/4.
//

#include "XMemory.h"

//返回相等的字节数
static inline XSize _XMemoryCompare8(const XPtr _Nonnull lhs, const XPtr _Nonnull rhs, XSize memorySize) {
    
    const XUInt64 * left = lhs;
    const XUInt64 * right = rhs;
    XSize size = 0;
    while (memorySize - size >= sizeof(XUInt64)) {
        if (*left != *right) {
            size += sizeof(XUInt64);
            left ++;
            right ++;
        } else {
            return size;
        }
    }
    return size;
}
static inline XSize _XMemoryCompare4(const XPtr _Nonnull lhs, const XPtr _Nonnull rhs, XSize memorySize) {
    const XUInt32 * left = lhs;
    const XUInt32 * right = rhs;
    XSize size = 0;
    while (memorySize - size >= sizeof(XUInt32)) {
        if (*left != *right) {
            size += sizeof(XUInt32);
            left ++;
            right ++;
        } else {
            return size;
        }
    }
    return size;
}
static inline XComparisonResult _XMemoryCompare2(const XPtr _Nonnull lhs, const XPtr _Nonnull rhs, XSize memorySize) {
    const XUInt16 * left = lhs;
    const XUInt16 * right = rhs;
    XSize size = 0;
    while (memorySize - size >= sizeof(XUInt16)) {
        if (*left != *right) {
            size += sizeof(XUInt16);
            left ++;
            right ++;
        } else {
            return size;
        }
    }
    return size;
}

static inline XComparisonResult _XMemoryCompare(const XPtr _Nonnull lhs, const XPtr _Nonnull rhs, XSize size) {
    if (0 == size) {
        return XCompareEqualTo;
    }
    int code = memcmp(lhs, rhs, size);
    if (code == 0) {
        return XCompareEqualTo;
    } else if (code < 0) {
        return XCompareLessThan;
    } else {
        return XCompareGreaterThan;
    }
}


XComparisonResult XMemoryCompare(const XPtr _Nonnull lhs, const XPtr _Nonnull rhs, XSize size) {
    XAssert(size >= 0, __func__, "size error");
    if (size == 0) {
        return XCompareEqualTo;
    } else {//size > 0
        XAssert(NULL != lhs, __func__, "lhs NULL error");
        XAssert(NULL != rhs, __func__, "rhs NULL error");
        uintptr_t lhsV = (uintptr_t)lhs;
        uintptr_t rhsV = (uintptr_t)rhs;
        const XUInt8 * left = lhs;
        const XUInt8 * right = rhs;
        if (size < 64) {
            return _XMemoryCompare(lhs, rhs, size);
        } else {
            XSize len = 0;
            XComparisonResult result = XCompareEqualTo;
            #if BUILD_TARGET_RT_64_BIT
                if ((lhsV & X_BUILD_UInt(0x7)) == (rhsV & X_BUILD_UInt(0x7))) {
                    XSize s = lhsV & X_BUILD_UInt(0x7);
                    result = _XMemoryCompare((const XPtr)left, (const XPtr)right, s);
                    if (XCompareEqualTo != result) {
                        return result;
                    } else {
                        len -= s;
                    }
                    
                    len += _XMemoryCompare8((const XPtr)(left + len), (const XPtr)(right + len), size - len);
                    if (size == len) {
                        return XCompareEqualTo;
                    }
                }
            #endif
            if ((lhsV & X_BUILD_UInt(0x3)) == (rhsV & X_BUILD_UInt(0x3))) {
                XSize s = lhsV & X_BUILD_UInt(0x3);
                result = _XMemoryCompare((const XPtr)left, (const XPtr)right, s);
                if (XCompareEqualTo != result) {
                    return result;
                } else {
                    len -= s;
                    left += s;
                    right += s;
                }
                len -= _XMemoryCompare4((const XPtr)left, (const XPtr)right, len);
                if (0 == len) {
                    return XCompareEqualTo;
                }
                return _XMemoryCompare((const XPtr)left, (const XPtr)right, len);
            }
            if ((lhsV & X_BUILD_UInt(0x1)) == (rhsV & X_BUILD_UInt(0x1))) {
                XSize s = lhsV & X_BUILD_UInt(0x1);
                result = _XMemoryCompare((const XPtr)left, (const XPtr)right, s);
                if (XCompareEqualTo != result) {
                    return result;
                } else {
                    len -= s;
                    left += s;
                    right += s;
                }
                len -= _XMemoryCompare2((const XPtr)left, (const XPtr)right, len);
                if (0 == len) {
                    return XCompareEqualTo;
                }
                return _XMemoryCompare((const XPtr)left, (const XPtr)right, len);
            }
        }
    }

    
}


