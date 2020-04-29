//
//  XNumber.c
//  
//
//  Created by vector on 2020/4/13.
//

#include "XRuntime.h"
#include "XRef.h"
#include "internal/XRuntimeInternal.h"

#include <math.h>
#include <float.h>


/*
 TaggedNumber32

 refType: 1, value = 1
 taggedContent: {
    class: 2
    objectContent: 28 {
        numberType: 4
        numberContent: 24
    }
 }
 flag: 1, value = 1
*/

/*
 TaggedNumber64

 refType: 2, value = 1
 taggedContent: 61 {
    class: 2
    objectContent: 59 {
        numberType: 4
        numberContent: 55
    }
 }
 flag: 1, value = 1
*/




#if BUILD_TARGET_RT_64_BIT

#define X_BUILD_TaggedNumberNumberTypeMask 0xF00000000000000ULL
#define X_BUILD_TaggedNumberNumberTypeShift 56ULL

#define X_BUILD_TaggedNumberSIntMax  0x3FFFFFFFFFFFFFULL
#define X_BUILD_TaggedNumberSIntMin -0x40000000000000ULL

#define X_BUILD_TaggedNumberUIntMax  0x7FFFFFFFFFFFFFULL
#define X_BUILD_TaggedNumberContentMask  0x7FFFFFFFFFFFFFULL
#define X_BUILD_TaggedNumberContentSignBit  0x40000000000000ULL
#define X_BUILD_TaggedNumberContentSignHigh  0xFF80000000000000ULL

#elif BUILD_TARGET_RT_32_BIT

#define X_BUILD_TaggedNumberNumberTypeMask 0x1E000000UL
#define X_BUILD_TaggedNumberNumberTypeShift 25UL
#define X_BUILD_TaggedNumberContentSignBit  0x400000ULL
#define X_BUILD_TaggedNumberContentSignHigh  0xFF800000UL

#define X_BUILD_TaggedNumberSIntMax  0x7FFFFFL
#define X_BUILD_TaggedNumberSIntMin -0x800000L

#define X_BUILD_TaggedNumberUIntMax  0xFFFFFFUL
#define X_BUILD_TaggedNumberContentMask  0xFFFFFFUL

#else
    #error unknown rt
#endif



const XNumberType XNumberTypeSInt8 = 1;
const XNumberType XNumberTypeUInt8 = 2;

const XNumberType XNumberTypeSInt16 = 3;
const XNumberType XNumberTypeUInt16 = 4;

const XNumberType XNumberTypeSInt32 = 5;
const XNumberType XNumberTypeUInt32 = 6;
const XNumberType XNumberTypeFloat32 = 7;

const XNumberType XNumberTypeSInt64 = 8;
const XNumberType XNumberTypeUInt64 = 9;
const XNumberType XNumberTypeFloat64 = 10;

static inline XBool XNumberTypeIsValid(XNumberType type) {
    return (0 > type && type <= XNumberTypeFloat64);
}


typedef union {
    XSInt64 s;
    XUInt64 u;
    XFloat64 f;
} _XNumberBits_u;

typedef struct {
    XNumberType type;
    XUInt32 xx;
    _XNumberBits_u bits;
} _XNumberStruct;


#if BUILD_TARGET_OS_WINDOWS
#define copysign(A, B) _copysign(A, B)
#endif


/* The IEEE bit patterns... Also have:
0x7f800000        float +Inf
0x7fc00000        float NaN
0xff800000        float -Inf
*/
#define BITSFORDOUBLEPOSINF32     ((uint32_t)0x7f800000UL)
#define BITSFORDOUBLENAN32   ((uint32_t)0x7fc00000UL)
#define BITSFORDOUBLENEGINF32    ((uint32_t)0xff800000UL)

#define BITSFORDOUBLENAN    ((uint64_t)0x7ff8000000000000ULL)
#define BITSFORDOUBLEPOSINF    ((uint64_t)0x7ff0000000000000ULL)
#define BITSFORDOUBLENEGINF    ((uint64_t)0xfff0000000000000ULL)

// The union dance is to avoid needing hard coded values for different floating point ABIs, technically would be safe to hardcode values, but this way is future proofed
typedef union {
    XFloat32 floatValue;
    int32_t bits;
} XFloat32Bits;

typedef union {
    XFloat64 floatValue;
    int64_t bits;
} XFloat64Bits;


//正无穷
static const _XNumberBits32_u _XNumberBits32PositiveInfinity = {.u = BITSFORDOUBLEPOSINF32};
static const _XNumberBits32_u _XNumberBits32Nan = {.u = BITSFORDOUBLENAN32};
//负无穷
static const _XNumberBits32_u _XNumberBits32NegativeInfinity = {.u = BITSFORDOUBLENEGINF32};


static const _XNumberBits64_u _XNumberBits64PositiveInfinity = {.u = BITSFORDOUBLEPOSINF};
static const _XNumberBits64_u _XNumberBits64Nan = {.u = BITSFORDOUBLENAN};
static const _XNumberBits64_u _XNumberBits64NegativeInfinity = {.u = BITSFORDOUBLENEGINF};


#define BITSFORFLOATZERO floatZeroBits.bits
#define BITSFORFLOATONE  floatOneBits.bits

#define BITSFORDOUBLEZERO doubleZeroBits.bits
#define BITSFORDOUBLEONE  doubleOneBits.bits

#if BUILD_TARGET_OS_DARWIN || BUILD_TARGET_OS_LINUX || BUILD_TARGET_OS_ANDROID || BUILD_TARGET_OS_BSD
#define FLOAT_POSITIVE_2_TO_THE_64    0x1.0p+64L
#define FLOAT_NEGATIVE_2_TO_THE_127    -0x1.0p+127L
#define FLOAT_POSITIVE_2_TO_THE_127    0x1.0p+127L
#elif BUILD_TARGET_OS_WINDOWS
#define FLOAT_POSITIVE_2_TO_THE_64    18446744073709551616.0
#define FLOAT_NEGATIVE_2_TO_THE_127    -170141183460469231731687303715884105728.0
#define FLOAT_POSITIVE_2_TO_THE_127    170141183460469231731687303715884105728.0
#else
#error Unknown or unspecified DEPLOYMENT_TARGET
#endif

typedef struct {    // NOTE WELL: these two fields may switch position someday, do not use '= {high, low}' -style initialization
    int64_t high;
    uint64_t low;
} XSInt128Struct;

static XBool isNeg128(const XSInt128Struct *in) {
    return in->high < 0;
}

static XComparisonResult cmp128(const XSInt128Struct *in1, const XSInt128Struct *in2) {
    if (in1->high < in2->high) return XCompareLessThan;
    if (in1->high > in2->high) return XCompareGreaterThan;
    if (in1->low < in2->low) return XCompareLessThan;
    if (in1->low > in2->low) return XCompareGreaterThan;
    return XCompareEqualTo;
}

// allows out to be the same as in1 or in2
static void add128(XSInt128Struct *out, XSInt128Struct *in1, XSInt128Struct *in2) {
    XSInt128Struct tmp;
    tmp.low = in1->low + in2->low;
    tmp.high = in1->high + in2->high;
    if (UINT64_MAX - in1->low < in2->low) {
        tmp.high++;
    }
    *out = tmp;
}

// allows out to be the same as in
static void neg128(XSInt128Struct *out, XSInt128Struct *in) {
    uint64_t tmplow = ~in->low;
    out->low = tmplow + 1;
    out->high = ~in->high;
    if (UINT64_MAX == tmplow) {
    out->high++;
    }
}

static const XSInt128Struct powersOf10[] = {
    { 0x4B3B4CA85A86C47ALL, 0x098A224000000000ULL },
    { 0x0785EE10D5DA46D9LL, 0x00F436A000000000ULL },
    { 0x00C097CE7BC90715LL, 0xB34B9F1000000000ULL },
    { 0x0013426172C74D82LL, 0x2B878FE800000000ULL },
    { 0x0001ED09BEAD87C0LL, 0x378D8E6400000000ULL },
    { 0x0000314DC6448D93LL, 0x38C15B0A00000000ULL },
    { 0x000004EE2D6D415BLL, 0x85ACEF8100000000ULL },
    { 0x0000007E37BE2022LL, 0xC0914B2680000000ULL },
    { 0x0000000C9F2C9CD0LL, 0x4674EDEA40000000ULL },
    { 0x00000001431E0FAELL, 0x6D7217CAA0000000ULL },
    { 0x00000000204FCE5ELL, 0x3E25026110000000ULL },
    { 0x00000000033B2E3CLL, 0x9FD0803CE8000000ULL },
    { 0x000000000052B7D2LL, 0xDCC80CD2E4000000ULL },
    { 0x0000000000084595LL, 0x161401484A000000ULL },
    { 0x000000000000D3C2LL, 0x1BCECCEDA1000000ULL },
    { 0x000000000000152DLL, 0x02C7E14AF6800000ULL },
    { 0x000000000000021ELL, 0x19E0C9BAB2400000ULL },
    { 0x0000000000000036LL, 0x35C9ADC5DEA00000ULL },
    { 0x0000000000000005LL, 0x6BC75E2D63100000ULL },
    { 0x0000000000000000LL, 0x8AC7230489E80000ULL },
    { 0x0000000000000000LL, 0x0DE0B6B3A7640000ULL },
    { 0x0000000000000000LL, 0x016345785D8A0000ULL },
    { 0x0000000000000000LL, 0x002386F26FC10000ULL },
    { 0x0000000000000000LL, 0x00038D7EA4C68000ULL },
    { 0x0000000000000000LL, 0x00005AF3107A4000ULL },
    { 0x0000000000000000LL, 0x000009184E72A000ULL },
    { 0x0000000000000000LL, 0x000000E8D4A51000ULL },
    { 0x0000000000000000LL, 0x000000174876E800ULL },
    { 0x0000000000000000LL, 0x00000002540BE400ULL },
    { 0x0000000000000000LL, 0x000000003B9ACA00ULL },
    { 0x0000000000000000LL, 0x0000000005F5E100ULL },
    { 0x0000000000000000LL, 0x0000000000989680ULL },
    { 0x0000000000000000LL, 0x00000000000F4240ULL },
    { 0x0000000000000000LL, 0x00000000000186A0ULL },
    { 0x0000000000000000LL, 0x0000000000002710ULL },
    { 0x0000000000000000LL, 0x00000000000003E8ULL },
    { 0x0000000000000000LL, 0x0000000000000064ULL },
    { 0x0000000000000000LL, 0x000000000000000AULL },
    { 0x0000000000000000LL, 0x0000000000000001ULL },
};

static const XSInt128Struct neg_powersOf10[] = {
    { 0xB4C4B357A5793B85LL, 0xF675DDC000000000ULL },
    { 0xF87A11EF2A25B926LL, 0xFF0BC96000000000ULL },
    { 0xFF3F68318436F8EALL, 0x4CB460F000000000ULL },
    { 0xFFECBD9E8D38B27DLL, 0xD478701800000000ULL },
    { 0xFFFE12F64152783FLL, 0xC872719C00000000ULL },
    { 0xFFFFCEB239BB726CLL, 0xC73EA4F600000000ULL },
    { 0xFFFFFB11D292BEA4LL, 0x7A53107F00000000ULL },
    { 0xFFFFFF81C841DFDDLL, 0x3F6EB4D980000000ULL },
    { 0xFFFFFFF360D3632FLL, 0xB98B1215C0000000ULL },
    { 0xFFFFFFFEBCE1F051LL, 0x928DE83560000000ULL },
    { 0xFFFFFFFFDFB031A1LL, 0xC1DAFD9EF0000000ULL },
    { 0xFFFFFFFFFCC4D1C3LL, 0x602F7FC318000000ULL },
    { 0xFFFFFFFFFFAD482DLL, 0x2337F32D1C000000ULL },
    { 0xFFFFFFFFFFF7BA6ALL, 0xE9EBFEB7B6000000ULL },
    { 0xFFFFFFFFFFFF2C3DLL, 0xE43133125F000000ULL },
    { 0xFFFFFFFFFFFFEAD2LL, 0xFD381EB509800000ULL },
    { 0xFFFFFFFFFFFFFDE1LL, 0xE61F36454DC00000ULL },
    { 0xFFFFFFFFFFFFFFC9LL, 0xCA36523A21600000ULL },
    { 0xFFFFFFFFFFFFFFFALL, 0x9438A1D29CF00000ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0x7538DCFB76180000ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xF21F494C589C0000ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFE9CBA87A2760000ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFDC790D903F0000ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFFC72815B398000ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFFFA50CEF85C000ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFFFF6E7B18D6000ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFF172B5AF000ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFE8B7891800ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFDABF41C00ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFC4653600ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFA0A1F00ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFF676980ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFF0BDC0ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFE7960ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFD8F0ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFC18ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFF9CULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFF6ULL },
    { 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFULL },
};

static void emit128(char *buffer, const XSInt128Struct *in, XBool forcePlus) {
    XSInt128Struct tmp = *in;
    if (isNeg128(&tmp)) {
    neg128(&tmp, &tmp);
    *buffer++ = '-';
    } else if (forcePlus) {
    *buffer++ = '+';
    }
    XBool doneOne = false;
    int idx;
    for (idx = 0; idx < sizeof(powersOf10) / sizeof(powersOf10[0]); idx++) {
        int count = 0;
        while (cmp128(&powersOf10[idx], &tmp) <= 0) {
            add128(&tmp, &tmp, (XSInt128Struct *)&neg_powersOf10[idx]);
            count++;
        }
        if (0 != count || doneOne) {
            *buffer++ = '0' + count;
            doneOne = true;
        }
    }
    if (!doneOne) {
    *buffer++ = '0';
    }
    *buffer = '\0';
}


static void cvtUInt64ToSInt128(XSInt128Struct *out, XUInt64 v) {
    XSInt128Struct i;
    i.high = 0;
    i.low = (XUInt64)v;
    *out = i;
}
static void cvtSInt64ToSInt128(XSInt128Struct *out, XSInt64 v) {
    XSInt128Struct i;

    if (v < 0) {
        i.high = 0x8000000000000000LL;
        i.low = (XUInt64)(v * -1);
    } else {
        i.high = 0;
        i.low = (XUInt64)v;
    }
    *out = i;
}


static void cvtSInt128ToFloat64(XFloat64 *out, const XSInt128Struct *in) {
    // switching to a positive number results in better accuracy
    // for negative numbers close to zero, because the multiply
    // of -1 by 2^64 (scaling the Float64 high) is avoided
    XBool wasNeg = false;
    XSInt128Struct tmp = *in;
    if (isNeg128(&tmp)) {
    neg128(&tmp, &tmp);
    wasNeg = true;
    }
    XFloat64 d = (XFloat64)tmp.high * FLOAT_POSITIVE_2_TO_THE_64 + (XFloat64)tmp.low;
    if (wasNeg) d = -d;
    *out = d;
}

static void cvtFloat64ToSInt128(XSInt128Struct *out, const XFloat64 *in) {
    XSInt128Struct i;
    XFloat64 d = *in;
    if (isnan(d)) {
        out->high = 0;
        out->low = 0;
        return;
    }
    
    //i 的取值范围 是 [FLOAT_NEGATIVE_2_TO_THE_127, (FLOAT_POSITIVE_2_TO_THE_127-1)]
    if (d < FLOAT_NEGATIVE_2_TO_THE_127) {
    i.high = 0x8000000000000000LL;
    i.low = 0x0000000000000000ULL;
    *out = i;
    return;
    }
    if (FLOAT_POSITIVE_2_TO_THE_127<= d) {
    i.high = 0x7fffffffffffffffLL;
    i.low = 0xffffffffffffffffULL;
    *out = i;
    return;
    }
    
    //比FLOAT_POSITIVE_2_TO_THE_127小的double 是170141183460469212842221372237303250944 (2^127 - 2^74)， 也就是说 只要比FLOAT_POSITIVE_2_TO_THE_127 小的double 一定能用XSInt128Struct 保存整数部分
    
    XFloat64 t = floor(d / FLOAT_POSITIVE_2_TO_THE_64);
    i.high = (int64_t)t;
    i.low = (uint64_t)(d - t * FLOAT_POSITIVE_2_TO_THE_64);
    *out = i;
}


//static _CF_CONSTANT_OBJECT_BACKING struct __CFNumber __kCFNumberFloat32Zero = {
//    INIT_CFRUNTIME_BASE_WITH_CLASS_AND_FLAGS(__NSCFNumber, _kCFRuntimeIDCFNumber, kCFNumberFloat32typeIndex),
//    { ._32.floatValue = 0.0f }
//};
//static const CFNumberRef kCFNumberFloat32Zero = &__kCFNumberFloat32Zero;
//
//static _CF_CONSTANT_OBJECT_BACKING struct __CFNumber __kCFNumberFloat32One = {
//    INIT_CFRUNTIME_BASE_WITH_CLASS_AND_FLAGS(__NSCFNumber, _kCFRuntimeIDCFNumber, kCFNumberFloat32typeIndex),
//    { ._32.floatValue = 1.0f }
//};
//static const CFNumberRef kCFNumberFloat32One = &__kCFNumberFloat32One;
//
//static _CF_CONSTANT_OBJECT_BACKING struct __CFNumber __kCFNumberFloat64Zero = {
//    INIT_CFRUNTIME_BASE_WITH_CLASS_AND_FLAGS(__NSCFNumber, _kCFRuntimeIDCFNumber, kCFNumberFloat64typeIndex),
//    { ._64.floatValue = 0.0f }
//};
//static const CFNumberRef kCFNumberFloat64Zero = &__kCFNumberFloat64Zero;
//
//static _CF_CONSTANT_OBJECT_BACKING struct __CFNumber __kCFNumberFloat64One = {
//    INIT_CFRUNTIME_BASE_WITH_CLASS_AND_FLAGS(__NSCFNumber, _kCFRuntimeIDCFNumber, kCFNumberFloat64typeIndex),
//    { ._64.floatValue = 1.0f }
//};
//static const CFNumberRef kCFNumberFloat64One = &__kCFNumberFloat64One;


static inline XNumber _Nullable ___XNumberMakeSInt(XNumberType type, XSInt64 value, const char * _Nonnull func) {
    if (X_BUILD_TaggedNumberSIntMin <= value && value <= X_BUILD_TaggedNumberSIntMax) {
    #if BUILD_TARGET_RT_64_BIT
        XSInt64 v = value;
        XUInt64 tmp = *(XUInt64 *)(&v);
        XUInt64 content = (tmp & X_BUILD_TaggedNumberContentMask) << 1;
        XUInt64 typeFlag = type;
        typeFlag = typeFlag << X_BUILD_TaggedNumberNumberTypeShift;
        XUInt64 result = X_BUILD_TaggedObjectFlag | X_BUILD_TaggedObjectClassNumber | typeFlag | content;
        XNumber ref = (XNumber)((uintptr_t)result);
        XAssert((XUInt64)((uintptr_t)ref) == result, func, "unknown error");
        return ref;
    #elif BUILD_TARGET_RT_32_BIT
        XSInt32 v = (XSInt32)value;
        XUInt32 tmp = *(XUInt32 *)(&v);
        XUInt32 content = (tmp & X_BUILD_TaggedNumberContentMask) << 1;
        XUInt32 typeFlag = type;
        typeFlag = typeFlag << X_BUILD_TaggedNumberNumberTypeShift;
        XUInt32 result = X_BUILD_TaggedObjectFlag | X_BUILD_TaggedObjectClassNumber | typeFlag | content;
        XNumber ref = (XNumber)((uintptr_t)result);
        XAssert((XUInt32)((uintptr_t)ref) == result, func, "unknown error");
        return ref;
    #else
        #error unknown rt
    #endif
    } else {
        return NULL;
    }
}

static inline  XNumber _Nullable ___XNumberMakeUInt(XNumberType type, XUInt64 value, const char * _Nonnull func) {
    if (value <= X_BUILD_TaggedNumberUIntMax) {
    #if BUILD_TARGET_RT_64_BIT
        XUInt64 content = value << 1ULL;
        XUInt64 typeFlag = type;
        typeFlag = typeFlag << X_BUILD_TaggedNumberNumberTypeShift;
        XUInt64 result = X_BUILD_TaggedObjectFlag | X_BUILD_TaggedObjectClassNumber | typeFlag | content;
        XNumber ref = (XNumber)((uintptr_t)result);
        XAssert((XUInt64)((uintptr_t)ref) == result, func, "unknown error");
        return ref;
    #elif BUILD_TARGET_RT_32_BIT
        XUInt32 content = (XUInt32)(value << 1);
        XUInt32 typeFlag = type;
        typeFlag = typeFlag << X_BUILD_TaggedNumberNumberTypeShift;
        XUInt32 result = X_BUILD_TaggedObjectFlag | X_BUILD_TaggedObjectClassNumber | typeFlag | content;
        XNumber ref = (XNumber)((uintptr_t)result);
        XAssert((XUInt32)((uintptr_t)ref) == result, func, "unknown error");
        return ref;
    #else
        #error unknown rt
    #endif
    } else {
        return NULL;
    }
}

XNumber _Nonnull __XNumberCreate32(XNumberType type, _XNumberBits32_u value, const char * _Nonnull func) {
    
    
    return NULL;
}
XNumber _Nonnull __XNumberCreate64(XNumberType type, _XNumberBits64_u value, const char * _Nonnull func) {
    
    
    return NULL;
}

static XNumber _Nonnull __XNumberCreateSInt32(XNumberType type, XSInt32 value, const char * _Nonnull func) {
    XNumber result = ___XNumberMakeSInt(type, (XSInt64)value, func);
    if (NULL == result) {
        _XNumberBits32_u bits = {};
        bits.s = value;
        result = __XNumberCreate32(type, bits, func);
    }
    return result;
}
static XNumber _Nonnull __XNumberCreateUInt32(XNumberType type, XUInt32 value, const char * _Nonnull func) {
    XNumber result = ___XNumberMakeUInt(type, (XUInt64)value, func);
    if (NULL == result) {
        _XNumberBits32_u bits = {};
        bits.u = value;
        result = __XNumberCreate32(type, bits, func);
    }
    return result;
}
static XNumber _Nonnull __XNumberCreateSInt64(XNumberType type, XSInt64 value, const char * _Nonnull func) {
    XNumber result = ___XNumberMakeSInt(type, value, func);
    if (NULL == result) {
        _XNumberBits64_u bits = {};
        bits.s = value;
        result = __XNumberCreate64(type, bits, func);
    }
    return result;
}
static XNumber _Nonnull __XNumberCreateUInt64(XNumberType type, XUInt64 value, const char * _Nonnull func) {
    XNumber result = ___XNumberMakeUInt(type, value, func);
    if (NULL == result) {
        _XNumberBits64_u bits = {};
        bits.u = value;
        result = __XNumberCreate64(type, bits, func);
    }
    return result;
}

#if BUILD_TARGET_RT_64_BIT
static inline XNumber _Nonnull ___XNumberMakeFloat32(XNumberType type, XFloat32 value, const char * _Nonnull func) {
    XUInt32 tmp = *(XUInt32 *)(&value);
    XUInt64 content = tmp;
    XUInt64 typeFlag = type;
    typeFlag = typeFlag << 58;
    XUInt64 result = 0xE000000000000001ULL | (content << 1) | typeFlag;
    XNumber ref = (XNumber)((uintptr_t)result);
    XAssert((XUInt64)((uintptr_t)ref) == result, func, "unknown error");
    return ref;
}
#endif

XNumber _Nonnull __XNumberCreateFloat32(XNumberType type, XFloat32 value, const char * _Nonnull func) {
#if BUILD_TARGET_RT_64_BIT
    return ___XNumberMakeFloat32(type, value, func);
#elif BUILD_TARGET_RT_32_BIT
    _XNumberBits32_u bits = {};
    bits.f = value;
    return __XNumberCreate32(type, bits, func);
#else
#error unknown rt
#endif
}

XNumber _Nonnull __XNumberCreateFloat64(XNumberType type, XFloat64 value, const char * _Nonnull func) {
#if BUILD_TARGET_RT_64_BIT
    if (isnan(value)) {
        XFloat32 f32 = _XNumberBits32Nan.f;
        return ___XNumberMakeFloat32(type, f32, func);
    } else {
        XFloat32 f32 = (XFloat32)value;
        XFloat64 f64 = (XFloat64)f32;
        if (value == f64) {
            return ___XNumberMakeFloat32(type, f32, func);
        }
    }
#endif
    _XNumberBits64_u bits = {};
    bits.f = value;
    return __XNumberCreate64(type, bits, func);
}



XNumber _Nonnull XNumberCreate(XNumberType theType, const void * _Nonnull valuePtr) {
    XAssert(NULL != valuePtr, __func__, "valuePtr connot be NULL");
    XAssert(XNumberTypeIsValid(theType), __func__, "number type error");
    switch (theType) {
        case XNumberTypeSInt8: {
            XSInt8 s8 = 0;
            memcpy(&s8, valuePtr, sizeof(XSInt8));
            XSInt32 s32 = (XSInt32)s8;
            return __XNumberCreateSInt32(theType, s32, __func__);
        }
            break;
        case XNumberTypeSInt16: {
            XSInt16 s16 = 0;
            memcpy(&s16, valuePtr, sizeof(XSInt16));
            XSInt32 s32 = (XSInt32)s16;
            return __XNumberCreateSInt32(theType, s32, __func__);
       }
           break;
        case XNumberTypeSInt32: {
            XSInt32 s32 = 0;
            memcpy(&s32, valuePtr, sizeof(XSInt32));
            return __XNumberCreateSInt32(theType, s32, __func__);
        }
            break;
        case XNumberTypeSInt64: {
            XSInt64 s64 = 0;
            memcpy(&s64, valuePtr, sizeof(XSInt64));
            return __XNumberCreateSInt64(theType, s64, __func__);
        }
            break;
        case XNumberTypeUInt8: {
            XUInt8 u8 = 0;
            memcpy(&u8, valuePtr, sizeof(XUInt8));
            XSInt32 u32 = (XUInt32)u8;
            return __XNumberCreateUInt32(theType, u32, __func__);
        }
            break;
        case XNumberTypeUInt16: {
            XUInt16 u16 = 0;
            memcpy(&u16, valuePtr, sizeof(XUInt16));
            XSInt32 u32 = (XUInt32)u16;
            return __XNumberCreateUInt32(theType, u32, __func__);
        }
           break;
        case XNumberTypeUInt32: {
            XUInt32 u32 = 0;
            memcpy(&u32, valuePtr, sizeof(XUInt32));
            return __XNumberCreateUInt32(theType, u32, __func__);
        }
            break;
        case XNumberTypeUInt64: {
            XUInt64 u64 = 0;
            memcpy(&u64, valuePtr, sizeof(XUInt64));
            return __XNumberCreateUInt64(theType, u64, __func__);
        }
            break;
        case XNumberTypeFloat32: {
            XFloat32 f32 = 0;
            memcpy(&f32, valuePtr, sizeof(XFloat32));
            return __XNumberCreateFloat32(theType, f32, __func__);
        }
            break;
        case XNumberTypeFloat64: {
            XFloat64 f64 = 0;
            memcpy(&f64, valuePtr, sizeof(XFloat64));
            return __XNumberCreateFloat64(theType, f64, __func__);
        }
            break;
        default: {
            XAssert(false, __func__, "number type error");
        }
            break;
    }
}
 
static _XNumber * _Nonnull __XRefAsNumber(XNumber _Nonnull ref, const char * _Nonnull func) {
    XCompressedType compressedType = XCompressedTypeNone;
    
#if BUILD_TARGET_RT_64_BIT
    __unused
#endif
    XClass info = _XRefGetUnpackedType(ref, &compressedType, func);
    
#if BUILD_TARGET_RT_64_BIT
    XAssert(XCompressedTypeNumber == compressedType, func, "not Number instance");
    return (_XNumber *)ref;
#elif BUILD_TARGET_RT_32_BIT
    const _XType_s * type = (const _XType_s *)info;
    XAssert(type->base.identifier == _XClassTable[X_BUILD_CompressedType_Number - 1].base.identifier, func, "not Number instance");
    return (_XNumber *)ref;
#else
    #error unknown rt
#endif
}


void __XNumberUnpack(XNumber _Nonnull ref, _XNumberStruct * _Nonnull ptr, const char * _Nonnull func) {
    _XNumberStruct content = {};
#if BUILD_TARGET_RT_64_BIT
    XUInt64 v = (XUInt64)((uintptr_t)ref);
    if ((v & X_BUILD_TaggedMask) == X_BUILD_TaggedObjectFlag) {
        XUInt64 clsId = v & X_BUILD_TaggedObjectClassMask;
        XAssert(X_BUILD_TaggedObjectClassNumber == clsId, func, "not Number instance");
        content.type = (XNumberType)((v >> X_BUILD_TaggedNumberNumberTypeShift) & 0xf);
        switch (content.type) {
            case XNumberTypeSInt8:
            case XNumberTypeSInt16:
            case XNumberTypeSInt32:
            case XNumberTypeSInt64: {
                XUInt64 tmp = (v >> 1) & X_BUILD_TaggedNumberContentMask;
                XUInt64 bits = 0;
                if (tmp & X_BUILD_TaggedNumberContentSignBit) {
                    bits = X_BUILD_TaggedNumberContentSignHigh | tmp;
                } else {
                    bits = tmp;
                }
                XSInt64 number = *(XSInt64 *)(&bits);
                content.bits.s = number;
            }
                break;
            case XNumberTypeUInt8:
            case XNumberTypeUInt16:
            case XNumberTypeUInt32:
            case XNumberTypeUInt64: {
                XUInt64 number = (v >> 1) & X_BUILD_TaggedNumberContentMask;
                content.bits.u = number;
            }
                break;
            case XNumberTypeFloat32:
            case XNumberTypeFloat64: {
                XUInt32 tmp = (XUInt32)((v >> 1) & 0xFFFFFFFFU);
                XFloat32 f = *(XFloat32 *)(&tmp);
                XFloat64 number = (XFloat64)f;
                content.bits.f = number;
            }
                break;
            default: {
                XAssert(false, func, "number type error");
            }
                break;
        }
    } else
#elif BUILD_TARGET_RT_32_BIT
    if ((v & X_BUILD_TaggedMask) == X_BUILD_TaggedObjectFlag) {
        XUInt32 clsId = v & X_BUILD_TaggedObjectClassMask;
        XAssert(X_BUILD_TaggedObjectClassNumber == clsId, func, "not Number instance");
        content.type = (XNumberType)((v >> X_BUILD_TaggedNumberNumberTypeShift) & 0xf);
        switch (contentOfLhs.type) {
            case XNumberTypeSInt8:
            case XNumberTypeSInt16:
            case XNumberTypeSInt32:
            case XNumberTypeSInt64: {
                XUInt32 tmp = (v >> 1) & X_BUILD_TaggedNumberContentMask;
                XUInt32 bits = 0;
                if (tmp & X_BUILD_TaggedNumberContentSignBit) {
                    bits = X_BUILD_TaggedNumberContentSignHigh | tmp;
                } else {
                    bits = tmp;
                }
                XSInt32 number = *(XSInt32 *)(&bits);
                content.bits.s = (XSInt64)number;
            }
                break;
            case XNumberTypeUInt8:
            case XNumberTypeUInt16:
            case XNumberTypeUInt32:
            case XNumberTypeUInt64: {
                XUInt32 tmp = (v >> 1) & X_BUILD_TaggedNumberContentMask;
                XUInt64 number = tmp;
                content.bits.u = number;
            }
                break;
            case XNumberTypeFloat32:
            case XNumberTypeFloat64:
            default: {
                XAssert(false, func, );
            }
                break;
        }
    } else
#else
    #error unknown rt
#endif
    {
        _XNumber * obj = __XRefAsNumber(ref, func);
        _XNumberContent_t * ncontent = &(obj->content);
        content.type = ncontent->type;
        switch (content.type) {
            case XNumberTypeSInt8:
            case XNumberTypeSInt16:
            case XNumberTypeSInt32: {
                XSInt32 s32 = ncontent->bits32.s;
                XSInt64 s64 = (XSInt64)s32;
                content.bits.s = s64;
            }
                break;
            case XNumberTypeSInt64: {
                _XNumberBits64_u * bits64 = (_XNumberBits64_u *)(&(ncontent->extended[0]));
                content.bits.s = bits64->s;
            }
                break;
            case XNumberTypeUInt8:
            case XNumberTypeUInt16:
            case XNumberTypeUInt32: {
                XUInt32 u32 = ncontent->bits32.u;
                XUInt64 u64 = (XUInt64)u32;
                content.bits.u = u64;
            }
                break;
            case XNumberTypeUInt64: {
                _XNumberBits64_u * bits64 = (_XNumberBits64_u *)(&(ncontent->extended[0]));
                content.bits.u = bits64->u;
            }
                break;
            case XNumberTypeFloat32: {
                XFloat32 f32 = ncontent->bits32.f;
                XFloat64 f64 = (XFloat64)f32;
               content.bits.f = f64;
           }
               break;
            case XNumberTypeFloat64: {
                _XNumberBits64_u * bits64 = (_XNumberBits64_u *)(&(ncontent->extended[0]));
                 XFloat64 f64 = bits64->f;
                content.bits.f = f64;
            }
                break;
            default: {
                XAssert(false, func, "number type error");
            }
                break;
        }
    }
    memcpy(ptr, &content, sizeof(_XNumberStruct));
}



#define CVT2(SRC_TYPE, DST_TYPE, DST_MIN, DST_MAX) do { \
SRC_TYPE sv = value; \
DST_TYPE dv = (sv < DST_MIN) ? (DST_TYPE)DST_MIN : (DST_TYPE)(((DST_MAX < sv) ? DST_MAX : sv)); \
memcpy(valuePtr, &dv, sizeof(DST_TYPE)); \
SRC_TYPE vv = (SRC_TYPE)dv; return (vv == sv); \
} while (0)


#define CVT(SRC_TYPE, DST_TYPE, DST_MIN, DST_MAX) do { \
    SRC_TYPE sv; memmove(&sv, data, sizeof(SRC_TYPE)); \
    DST_TYPE dv = (sv < DST_MIN) ? (DST_TYPE)DST_MIN : (DST_TYPE)(((DST_MAX < sv) ? DST_MAX : sv)); \
    memmove(valuePtr, &dv, sizeof(DST_TYPE)); \
    SRC_TYPE vv = (SRC_TYPE)dv; return (vv == sv); \
    } while (0)

#define CVT128ToInt(SRC_TYPE, DST_TYPE, DST_MIN, DST_MAX) do { \
        SRC_TYPE sv; memmove(&sv, data, sizeof(SRC_TYPE)); \
    DST_TYPE dv; Boolean noLoss = false; \
    if (0 < sv.high || (0 == sv.high && (int64_t)DST_MAX < sv.low)) { \
        dv = DST_MAX; \
    } else if (sv.high < -1 || (-1 == sv.high && sv.low < (int64_t)DST_MIN)) { \
        dv = DST_MIN; \
    } else { \
        dv = (DST_TYPE)sv.low; \
        noLoss = true; \
    } \
        memmove(valuePtr, &dv, sizeof(DST_TYPE)); \
        return noLoss; \
        } while (0)

//返回true 表示完全相等、否则是个约数
static XBool __XNumberGetSInt128StructValue(_XNumberStruct * _Nonnull number, XSInt128Struct * _Nonnull valuePtr) {
    XNumberType type = number->type;
    switch (type) {
        case XNumberTypeSInt8:
        case XNumberTypeSInt16:
        case XNumberTypeSInt32:
        case XNumberTypeSInt64: {
            XSInt64 value = number->bits.s;
            cvtSInt64ToSInt128(valuePtr, value);
            return true;
        }
            break;
        case XNumberTypeUInt8:
        case XNumberTypeUInt16:
        case XNumberTypeUInt32:
        case XNumberTypeUInt64: {
            XUInt64 value = number->bits.u;
            cvtUInt64ToSInt128(valuePtr, value);
            return true;
        }
            break;
        case XNumberTypeFloat64: {
            XSInt128Struct i;
            XFloat64 d = number->bits.f;
            cvtFloat64ToSInt128(&i, &d);
            memmove(valuePtr, &i, 16);
            XFloat64 d2;
            cvtSInt128ToFloat64(&d2, &i);
            return (d2 == d);
        }
            break;
        default:
            break;
    }
    return false;
}

#define XCVTSInt64ToS(DST_TYPE, DST_MIN, DST_MAX) do { \
XSInt64 sv = value; \
DST_TYPE dv = (sv < DST_MIN) ? (DST_TYPE)DST_MIN : (DST_TYPE)(((DST_MAX < sv) ? DST_MAX : sv)); \
memcpy(valuePtr, &dv, sizeof(DST_TYPE)); \
XSInt64 vv = (XSInt64)dv; return (vv == sv); \
} while (0)

#define XCVTSInt64ToU(DST_TYPE, DST_MAX) do { \
XSInt64 sv = value; \
DST_TYPE dv = (sv < 0) ? 0 : (DST_TYPE)(((DST_MAX < sv) ? DST_MAX : sv)); \
memcpy(valuePtr, &dv, sizeof(DST_TYPE)); \
XSInt64 vv = (XSInt64)dv; return (vv == sv); \
} while (0)

static inline XBool __XNumberConvertSInt64(XSInt64 value, XNumberType type, void * valuePtr) {
    switch (type) {
        case XNumberTypeSInt8: {
            XCVTSInt64ToS(int8_t, INT8_MIN, INT8_MAX);
        }
            break;
        case XNumberTypeSInt16: {
            XCVTSInt64ToS(int16_t, INT16_MIN, INT16_MAX);
        }
            break;
        case XNumberTypeSInt32: {
            XCVTSInt64ToS(int32_t, INT32_MIN, INT32_MAX);
        }
            break;
        case XNumberTypeSInt64: {
            memcpy(valuePtr, &value, sizeof(XSInt64));
            return true;
        }
            break;
        case XNumberTypeUInt8: {
            XCVTSInt64ToU(uint8_t, UINT8_MAX);
        }
            break;
        case XNumberTypeUInt16: {
            XCVTSInt64ToU(uint16_t, UINT16_MAX);
        }
            break;
        case XNumberTypeUInt32: {
            XCVTSInt64ToU(uint32_t, UINT32_MAX);
        }
            break;
        case XNumberTypeUInt64: {
            XCVTSInt64ToU(uint64_t, UINT64_MAX);
        }
            break;
        case XNumberTypeFloat32: {
            XFloat32 dv = (value < -FLT_MAX) ? -FLT_MAX : (((FLT_MAX < value) ? FLT_MAX : (XFloat32)value)); ;
            memcpy(valuePtr, &dv, sizeof(XFloat32));
            XSInt64 vv = (XSInt64)dv;
            return (vv == value);
        }
            break;
        case XNumberTypeFloat64: {
            XFloat64 dv = (value < -DBL_MAX) ? -DBL_MAX : (((DBL_MAX < value) ? DBL_MAX : (XFloat64)value)); ;
            memcpy(valuePtr, &dv, sizeof(XFloat64));
            XSInt64 vv = (XSInt64)dv;
            return (vv == value);
        }
            break;
    }
    return true;
}


#define XCVTUInt64To(DST_TYPE, DST_MAX) do { \
XUInt64 sv = value; \
DST_TYPE dv = (DST_TYPE)(((DST_MAX < sv) ? DST_MAX : sv)); \
memcpy(valuePtr, &dv, sizeof(DST_TYPE)); \
XUInt64 vv = (XUInt64)dv; return (vv == sv); \
} while (0)

static inline XBool __XNumberConvertUInt64(XUInt64 value, XNumberType type, void * valuePtr) {
    switch (type) {
        case XNumberTypeSInt8: {
            XCVTUInt64To(int8_t, INT8_MAX);
        }
            break;
        case XNumberTypeSInt16: {
            XCVTUInt64To(int16_t, INT16_MAX);
        }
            break;
        case XNumberTypeSInt32: {
            XCVTUInt64To(int32_t, INT32_MAX);
        }
            break;
        case XNumberTypeSInt64: {
            XCVTUInt64To(int32_t, INT64_MAX);
            return true;
        }
            break;
        case XNumberTypeUInt8: {
            XCVTUInt64To(uint8_t, UINT8_MAX);
        }
            break;
        case XNumberTypeUInt16: {
            XCVTUInt64To(uint16_t, UINT16_MAX);
        }
            break;
        case XNumberTypeUInt32: {
            XCVTUInt64To(uint32_t, UINT32_MAX);
        }
            break;
        case XNumberTypeUInt64: {
            memcpy(valuePtr, &value, sizeof(XUInt64));
            return true;
        }
            break;
        case XNumberTypeFloat32: {
            XFloat32 dv = (((FLT_MAX < value) ? FLT_MAX : (XFloat32)value)); ;
            memcpy(valuePtr, &dv, sizeof(XFloat32));
            XUInt64 vv = (XUInt64)dv;
            return (vv == value);
        }
            break;
        case XNumberTypeFloat64: {
            XFloat64 dv = (((DBL_MAX < value) ? DBL_MAX : (XFloat64)value)); ;
            memcpy(valuePtr, &dv, sizeof(XFloat64));
            XUInt64 vv = (XUInt64)dv;
            return (vv == value);
        }
            break;
    }
    return true;
}

#define XCVTFloat64ToS(DST_TYPE, DST_MIN, DST_MAX) do { \
XSInt64 sv = value; \
DST_TYPE dv = (sv < DST_MIN) ? (DST_TYPE)DST_MIN : (DST_TYPE)(((DST_MAX < sv) ? DST_MAX : sv)); \
memcpy(valuePtr, &dv, sizeof(DST_TYPE)); \
XSInt64 vv = (XSInt64)dv; return (vv == sv); \
} while (0)

#define XCVTFloat64ToU(DST_TYPE, DST_MAX) do { \
XSInt64 sv = value; \
DST_TYPE dv = (sv < 0.0) ? 0 : (DST_TYPE)(((DST_MAX < sv) ? DST_MAX : sv)); \
memcpy(valuePtr, &dv, sizeof(DST_TYPE)); \
XSInt64 vv = (XSInt64)dv; return (vv == sv); \
} while (0)
static inline XBool __XNumberConvertFloat64(XFloat64 value, XNumberType type, void * valuePtr) {
    switch (type) {
        case XNumberTypeSInt8: {
            XCVTFloat64ToS(int8_t, INT8_MIN, INT8_MAX);
        }
            break;
        case XNumberTypeSInt16: {
            XCVTFloat64ToS(int16_t, INT16_MIN, INT16_MAX);
        }
            break;
        case XNumberTypeSInt32: {
            XCVTFloat64ToS(int32_t, INT32_MIN, INT32_MAX);
        }
            break;
        case XNumberTypeSInt64: {
            XCVTFloat64ToS(int64_t, INT64_MIN, INT64_MAX);
        }
            break;
        case XNumberTypeUInt8: {
            XCVTFloat64ToU(uint8_t, UINT8_MAX);
        }
            break;
        case XNumberTypeUInt16: {
            XCVTFloat64ToU(uint16_t, UINT16_MAX);
        }
            break;
        case XNumberTypeUInt32: {
            XCVTFloat64ToU(uint32_t, UINT32_MAX);
        }
            break;
        case XNumberTypeUInt64: {
            XCVTFloat64ToU(uint64_t, UINT64_MAX);
        }
            break;
        case XNumberTypeFloat32: {
            XFloat32 dv = 0;
            if (isnan(value)) {
                dv = _XNumberBits32Nan.f;
            } else if (isinf(value)) {
                if (value < 0.0) {
                    dv = _XNumberBits32NegativeInfinity.f;
                } else {
                    dv = _XNumberBits32PositiveInfinity.f;
                }
            } else {
                dv = (value < -FLT_MAX) ? -FLT_MAX : (((FLT_MAX < value) ? FLT_MAX : (XFloat32)value)); ;
            }
            memcpy(valuePtr, &dv, sizeof(XFloat32));
            XFloat64 vv = (XFloat64)dv;
            return (vv == value);
        }
            break;
        case XNumberTypeFloat64: {
            memcpy(valuePtr, &value, sizeof(XFloat64));
            return true;
        }
            break;
    }
    return true;
}


XNumberType __XNumberGetType(XNumber _Nonnull ref, const char * _Nonnull func) {

#if BUILD_TARGET_RT_64_BIT
    XUInt64 v = (XUInt64)((uintptr_t)ref);
    if ((v & 0xE000000000000001ULL) == 0xE000000000000001ULL) {
        //53bits
        XUInt64 id = (v >> 58) & 0x7;
        XAssert(XCompressedTypeNumber == id, func, "not Number instance");

        XNumberType type = (XNumberType)((id >> 54) & 0xf);
        
        XAssert(XNumberTypeIsValid(type), func, "number type error");
        return type;
    } else
#elif BUILD_TARGET_RT_32_BIT
    if ((v & 0xE0000001UL) == v & 0xE0000001UL) {
        //21 bits
        XUInt32 id = (v >> 26) & 0x7;
        //Number String Data Date
        
        XAssert(XCompressedTypeNumber == id, func, "not Number instance");
        XNumberType type = (XNumberType)((id >> 22) & 0xf);
        XAssert(XNumberTypeIsValid(type), func, "number type error");
        return type;
    } else
#else
    #error unknown rt
#endif
    {
        _XNumber * obj = __XRefAsNumber(ref, func);
        _XNumberContent_t * ncontent = &(obj->content);
        XNumberType type = ncontent->type;
        XAssert(XNumberTypeIsValid(type), func, "number type error");
        return type;
    }
}

XNumberType XNumberGetType(XNumber _Nonnull ref) {
    XAssert(NULL == ref, __func__, "ref is NULL");
    return __XNumberGetType(ref, __func__);
}

XBool XNumberIsFloatType(XNumber _Nonnull ref) {
    XAssert(NULL == ref, __func__, "ref is NULL");
    XNumberType type = __XNumberGetType(ref, __func__);
    return (XNumberTypeFloat32 == type || XNumberTypeFloat64 == type);
}
XBool XNumberIsSignedType(XNumber _Nonnull ref) {
    XAssert(NULL == ref, __func__, "ref is NULL");
    XNumberType type = __XNumberGetType(ref, __func__);
    return (XNumberTypeFloat32 == type || XNumberTypeFloat64 == type || XNumberTypeSInt8 == type || XNumberTypeSInt16 == type || XNumberTypeSInt32 == type || XNumberTypeSInt64 == type);
}


// returns false if the output value is not the same as the number's value, which
// can occur due to accuracy loss and the value not being within the target range
static XBool __XNumberGetValue(_XNumberStruct * _Nonnull number, XNumberType type, void * valuePtr) {
    XNumberType ntype = number->type;
    switch (ntype) {
        case XNumberTypeSInt8:
        case XNumberTypeSInt16:
        case XNumberTypeSInt32:
        case XNumberTypeSInt64: {
            XSInt64 value = number->bits.s;
            return __XNumberConvertSInt64(value, type, valuePtr);
        }
            break;
        case XNumberTypeUInt8:
        case XNumberTypeUInt16:
        case XNumberTypeUInt32:
        case XNumberTypeUInt64: {
            XUInt64 value = number->bits.u;
            return __XNumberConvertUInt64(value, type, valuePtr);
        }
            break;
        case XNumberTypeFloat32:
        case XNumberTypeFloat64: {
            XFloat64 value = number->bits.f;
            return __XNumberConvertFloat64(value, type, valuePtr);
        }
            break;
        default: {
            abort();
        }
            break;
    }
    return false;
}



extern XBool XNumberGetValue(XNumber _Nonnull number, XNumberType theType, void * _Nonnull valuePtr) {
    XAssert(NULL != number, __func__, "number connot be NULL");
    XAssert(NULL != valuePtr, __func__, "valuePtr connot be NULL");
    _XNumberStruct content = {};
    __XNumberUnpack(number, &content, __func__);
    
    return __XNumberGetValue(&content, theType, valuePtr);
}


#define CVT_COMPAT(SRC_TYPE, DST_TYPE, FT) do { \
    SRC_TYPE sv; memmove(&sv, data, sizeof(SRC_TYPE)); \
    DST_TYPE dv = (DST_TYPE)(sv); \
    memmove(valuePtr, &dv, sizeof(DST_TYPE)); \
    SRC_TYPE vv = (SRC_TYPE)dv; return (FT) || (vv == sv); \
    } while (0)

#define CVT128ToInt_COMPAT(SRC_TYPE, DST_TYPE) do { \
        SRC_TYPE sv; memmove(&sv, data, sizeof(SRC_TYPE)); \
    DST_TYPE dv; dv = (DST_TYPE)sv.low; \
        memmove(valuePtr, &dv, sizeof(DST_TYPE)); \
    uint64_t vv = (uint64_t)dv; return (vv == sv.low); \
        } while (0)


//static CFStringRef __CFNumberCopyDescription(CFTypeRef cf) {
//    CFNumberRef number = (CFNumberRef)cf;
//    XNumberType type = __XNumberGetType(number);
//    CFMutableStringRef mstr = CFStringCreateMutable(kCFAllocatorSystemDefault, 0);
//    CFStringAppendFormat(mstr, NULL, CFSTR("<CFNumber %p [%p]>{value = "), cf, CFGetAllocator(cf));
//    if (_XNumberTypeInfoTable[type].floatBit) {
//    Float64 d;
//        __XNumberGetValue(number, kCFNumberFloat64Type, &d);
//    if (isnan(d)) {
//        CFStringAppend(mstr, CFSTR("nan"));
//    } else if (isinf(d)) {
//        CFStringAppend(mstr, (0.0 < d) ? CFSTR("+infinity") : CFSTR("-infinity"));
//    } else if (0.0 == d) {
//        CFStringAppend(mstr, (copysign(1.0, d) < 0.0) ? CFSTR("-0.0") : CFSTR("+0.0"));
//    } else {
//        CFStringAppendFormat(mstr, NULL, CFSTR("%+.*f"), (_XNumberTypeInfoTable[type].storageBit ? 20 : 10), d);
//    }
//    const char *typeName = "unknown float";
//    switch (type) {
//    case kCFNumberFloat32Type: typeName = "kCFNumberFloat32Type"; break;
//    case kCFNumberFloat64Type: typeName = "kCFNumberFloat64Type"; break;
//    }
//    CFStringAppendFormat(mstr, NULL, CFSTR(", type = %s}"), typeName);
//    } else {
//    XSInt128Struct i;
//    __XNumberGetValue(number, kCFNumberSInt128Type, &i);
//    char buffer[128];
//    emit128(buffer, &i, true);
//    const char *typeName = "unknown integer";
//    switch (type) {
//    case kCFNumberSInt8Type: typeName = "kCFNumberSInt8Type"; break;
//    case kCFNumberSInt16Type: typeName = "kCFNumberSInt16Type"; break;
//    case kCFNumberSInt32Type: typeName = "kCFNumberSInt32Type"; break;
//    case kCFNumberSInt64Type: typeName = "kCFNumberSInt64Type"; break;
//    case kCFNumberSInt128Type: typeName = "kCFNumberSInt128Type"; break;
//    }
//    CFStringAppendFormat(mstr, NULL, CFSTR("%s, type = %s}"), buffer, typeName);
//    }
//    return mstr;
//}

// This function separated out from __CFNumberCopyFormattingDescription() so the plist creation can use it as well.

//static CFStringRef __CFNumberCreateFormattingDescriptionAsFloat64(CFAllocatorRef allocator, CFTypeRef cf) {
//    Float64 d;
//    CFNumberGetValue((CFNumberRef)cf, kCFNumberFloat64Type, &d);
//    if (isnan(d)) {
//    return (CFStringRef)CFRetain(CFSTR("nan"));
//    }
//    if (isinf(d)) {
//    return (CFStringRef)CFRetain((0.0 < d) ? CFSTR("+infinity") : CFSTR("-infinity"));
//    }
//    if (0.0 == d) {
//    return (CFStringRef)CFRetain(CFSTR("0.0"));
//    }
//    // if %g is used here, need to use DBL_DIG + 2 on Mac OS X, but %f needs +1
//    return CFStringCreateWithFormat(allocator, NULL, CFSTR("%.*g"), DBL_DIG + 2, d);
//}

//CF_PRIVATE CFStringRef __CFNumberCopyFormattingDescriptionAsFloat64(CFTypeRef cf) {
//    CFStringRef result = __CFNumberCreateFormattingDescriptionAsFloat64(kCFAllocatorSystemDefault, cf);
//    return result;
//}

//CF_PRIVATE CFStringRef __CFNumberCreateFormattingDescription(CFAllocatorRef allocator, CFTypeRef cf, CFDictionaryRef formatOptions) {
//    CFNumberRef number = (CFNumberRef)cf;
//    XNumberType type = __XNumberGetType(number);
//    if (_XNumberTypeInfoTable[type].floatBit) {
//        return __CFNumberCreateFormattingDescriptionAsFloat64(allocator, number);
//    }
//    XSInt128Struct i;
//    __XNumberGetValue(number, kCFNumberSInt128Type, &i);
//    char buffer[128];
//    emit128(buffer, &i, false);
//    return CFStringCreateWithFormat(allocator, NULL, CFSTR("%s"), buffer);
//}

//static CFStringRef __CFNumberCopyFormattingDescription_new(CFTypeRef cf, CFDictionaryRef formatOptions) {
//    CFNumberRef number = (CFNumberRef)cf;
//    XNumberType type = __XNumberGetType(number);
//    if (_XNumberTypeInfoTable[type].floatBit) {
//        return __CFNumberCopyFormattingDescriptionAsFloat64(number);
//    }
//    XSInt128Struct i;
//    __XNumberGetValue(number, kCFNumberSInt128Type, &i);
//    char buffer[128];
//    emit128(buffer, &i, false);
//    return CFStringCreateWithFormat(kCFAllocatorSystemDefault, NULL, CFSTR("%s"), buffer);
//}

//CF_PRIVATE CFStringRef __CFNumberCopyFormattingDescription(CFTypeRef cf, CFDictionaryRef formatOptions) {
//    CFStringRef result = __CFNumberCopyFormattingDescription_new(cf, formatOptions);
//    return result;
//}






//static inline void __CFNumberInit(CFNumberRef result, XNumberType type, const void *valuePtr) {
//    __XAssertIsValidNumberType(type);
//
//    uint64_t value;
//    switch (_XNumberTypeInfoTable[type].type) {
//        case kCFNumberSInt8Type:   value = (uint64_t)(int64_t)*(int8_t *)valuePtr; goto smallVal;
//        case kCFNumberSInt16Type:  value = (uint64_t)(int64_t)*(int16_t *)valuePtr; goto smallVal;
//        case kCFNumberSInt32Type:  value = (uint64_t)(int64_t)*(int32_t *)valuePtr; goto smallVal;
//        smallVal: memmove((void *)&result->_bits._64.bits, &value, 8); break;
//        case kCFNumberSInt64Type:  memmove((void *)&result->_bits._64.bits, valuePtr, 8); break;
//        case kCFNumberSInt128Type: memmove((void *)&result->_bits._64.bits, valuePtr, 16); break;
//        case kCFNumberFloat32Type: memmove((void *)&result->_bits._64.bits, valuePtr, 4); break;
//        case kCFNumberFloat64Type: memmove((void *)&result->_bits._64.bits, valuePtr, 8); break;
//    }
//}
//
//static inline void _CFNumberInit(CFNumberRef result, XNumberType type, const void *valuePtr) {
//    __CFNumberInit(result, type, valuePtr);
//}
//
//void _CFNumberInitBool(CFNumberRef result, Boolean value) {
//    _CFNumberInit(result, kCFNumberCharType, &value);
//}
//
//void _CFNumberInitInt8(CFNumberRef result, int8_t value) {
//    _CFNumberInit(result, kCFNumberCharType, &value);
//}
//
//void _CFNumberInitUInt8(CFNumberRef result, uint8_t value) {
//    _CFNumberInit(result, kCFNumberCharType, &value);
//}
//
//void _CFNumberInitInt16(CFNumberRef result, int16_t value) {
//    _CFNumberInit(result, kCFNumberShortType, &value);
//}
//
//void _CFNumberInitUInt16(CFNumberRef result, uint16_t value) {
//    _CFNumberInit(result, kCFNumberShortType, &value);
//}
//
//void _CFNumberInitInt32(CFNumberRef result, int32_t value) {
//    _CFNumberInit(result, kCFNumberIntType, &value);
//}
//
//void _CFNumberInitUInt32(CFNumberRef result, uint32_t value) {
//    _CFNumberInit(result, kCFNumberIntType, &value);
//}
//
//void _CFNumberInitInt(CFNumberRef result, long value) {
//    _CFNumberInit(result, kCFNumberLongType, &value);
//}
//
//void _CFNumberInitUInt(CFNumberRef result, unsigned long value) {
//    _CFNumberInit(result, kCFNumberLongType, &value);
//}
//
//void _CFNumberInitInt64(CFNumberRef result, int64_t value) {
//    _CFNumberInit(result, kCFNumberLongLongType, &value);
//}
//
//void _CFNumberInitUInt64(CFNumberRef result, uint64_t value) {
//    _CFNumberInit(result, kCFNumberLongLongType, &value);
//}
//
//void _CFNumberInitFloat(CFNumberRef result, float value) {
//    _CFNumberInit(result, kCFNumberFloatType, &value);
//}
//
//void _CFNumberInitDouble(CFNumberRef result, double value) {
//    _CFNumberInit(result, kCFNumberDoubleType, &value);
//}

//static CFNumberRef _CFNumberCreate(CFAllocatorRef allocator, XNumberType type, const void *valuePtr) {
//    __XAssertIsValidNumberType(type);
////printf("+ [%p] CFNumberCreate(%p, %d, %p)\n", pthread_self(), allocator, type, valuePtr);
//
//    if (!allocator) allocator = __CFGetDefaultAllocator();
//
//
//    // Look for cases where we can return a cached instance.
//    // We only use cached objects if the allocator is the system
//    // default allocator, except for the special floating point
//    // constant objects, where we return the cached object
//    // regardless of allocator, since that is what has always
//    // been done (and now must for compatibility).
//    int64_t valToBeCached = NotToBeCached;
//    if (_XNumberTypeInfoTable[type].floatBit) {
//        CFNumberRef cached = NULL;
//        if (0 == _XNumberTypeInfoTable[type].storageBit) {
//            XFloat32Bits f = *(XFloat32Bits *)valuePtr;
//            if (f.bits == BITSFORFLOATZERO) cached = kCFNumberFloat32Zero;
//            else if (f.bits == BITSFORFLOATONE) cached = kCFNumberFloat32One;
//            else if (isnan(f.floatValue)) cached = kCFNumberNaN;
//            else if (isinf(f.floatValue)) cached = (f.floatValue < 0.0) ? kCFNumberNegativeInfinity : kCFNumberPositiveInfinity;
//
//
//        } else {
//            XFloat64Bits d = *(XFloat64Bits *)valuePtr;
//            if (d.bits == BITSFORDOUBLEZERO) cached = kCFNumberFloat64Zero;
//            else if (d.bits == BITSFORDOUBLEONE) cached = kCFNumberFloat64One;
//            else if (isnan(d.floatValue)) cached = kCFNumberNaN;
//            else if (isinf(d.floatValue)) cached = (d.floatValue < 0.0) ? kCFNumberNegativeInfinity : kCFNumberPositiveInfinity;
//        }
//        if (cached) return (CFNumberRef)CFRetain(cached);
//    } else if (_CFAllocatorIsSystemDefault(allocator) && (__CFNumberCaching == kCFNumberCachingEnabled)) {
//        switch (_XNumberTypeInfoTable[type].type) {
//        case kCFNumberSInt8Type:   {int8_t  val = *(int8_t *)valuePtr;  if (MinCachedInt <= val && val <= MaxCachedInt) valToBeCached = (int64_t)val; break;}
//        case kCFNumberSInt16Type:  {int16_t val = *(int16_t *)valuePtr; if (MinCachedInt <= val && val <= MaxCachedInt) valToBeCached = (int64_t)val; break;}
//        case kCFNumberSInt32Type:  {int32_t val = *(int32_t *)valuePtr; if (MinCachedInt <= val && val <= MaxCachedInt) valToBeCached = (int64_t)val; break;}
//        case kCFNumberSInt64Type:  {int64_t val = *(int64_t *)valuePtr; if (MinCachedInt <= val && val <= MaxCachedInt) valToBeCached = (int64_t)val; break;}
//        }
//        if (NotToBeCached != valToBeCached) {
//            CFNumberRef cached = __CFNumberCache[valToBeCached - MinCachedInt];        // Atomic to access the value in the cache
//            if (NULL != cached) return (CFNumberRef)CFRetain(cached);
//        }
//    }
//
//    CFIndex size = 8 + ((!_XNumberTypeInfoTable[type].floatBit && _XNumberTypeInfoTable[type].storageBit) ? 8 : 0);
//    CFNumberRef result = (CFNumberRef)_CFRuntimeCreateInstance(allocator, CFNumberGetTypeID(), size, NULL);
//    if (NULL == result) {
//    return NULL;
//    }
//
//    __CFRuntimeSetNumberType(result, (uint8_t)_XNumberTypeInfoTable[type].type);
//
//
//    // should be initialized BEFORE ever caching it!
//    __CFNumberInit(result, type, valuePtr);
//
//    // for a value to be cached, we already have the value handy
//    if (NotToBeCached != valToBeCached) {
//    memmove((void *)&result->_bits._64.bits, &valToBeCached, 8);
//    // Put this in the cache unless the cache is already filled (by another thread).  If we do put it in the cache, retain it an extra time for the cache.
//    // Note that we don't bother freeing this result and returning the cached value if the cache was filled, since cached CFNumbers are not guaranteed unique.
//    // Barrier assures that the number that is placed in the cache is properly formed.
//    XNumberType origType = __XNumberGetType(result);
//    // Force all cached numbers to have the same type, so that the type does not
//    // depend on the order and original type in/with which the numbers are created.
//    // Forcing the type AFTER it was cached would cause a race condition with other
//    // threads pulling the number object out of the cache and using it.
//        __CFRuntimeSetNumberType(result, (uint8_t)kCFNumberSInt32Type);
//    if (OSAtomicCompareAndSwapPtrBarrier(NULL, (void *)result, (void *volatile *)&__CFNumberCache[valToBeCached - MinCachedInt])) {
//        CFRetain(result);
//    } else {
//        // Did not cache the number object, put original type back.
//            __CFRuntimeSetNumberType(result, origType);
//    }
//    return result;
//    }
//
//    return result;
//}
//
//CFNumberRef CFNumberCreate(CFAllocatorRef allocator, XNumberType type, const void *valuePtr) {
//    return _CFNumberCreate(allocator, type, valuePtr);
//}
//
//XNumberType CFNumberGetType(CFNumberRef number) {
//    CF_OBJC_FUNCDISPATCHV(CFNumberGetTypeID(), XNumberType, (NSNumber *)number, _XNumberType);
//    CF_SWIFT_FUNCDISPATCHV(CFNumberGetTypeID(), XNumberType, (CFSwiftRef)number, NSNumber._cfNumberGetType);
//    __XAssertIsNumber(number);
//    XNumberType type = __XNumberGetType(number);
//    if (kCFNumberSInt128Type == type) type = kCFNumberSInt64Type; // must hide this type, since it is not public
//    return type;
//}
//
//CF_EXPORT XNumberType _CFNumberGetType2(CFNumberRef number) {
//    CF_OBJC_FUNCDISPATCHV(CFNumberGetTypeID(), XNumberType, (NSNumber *)number, _XNumberType);
//    __XAssertIsNumber(number);
//    return __XNumberGetType(number);
//}
//
//CFIndex CFNumberGetByteSize(CFNumberRef number) {
//    __XAssertIsNumber(number);
//    CFIndex r = 1 << _XNumberTypeInfoTable[CFNumberGetType(number)].lgByteSize;
//    return r;
//}
//
//Boolean CFNumberIsFloatType(CFNumberRef number) {
//    __XAssertIsNumber(number);
//    Boolean r = _XNumberTypeInfoTable[CFNumberGetType(number)].floatBit;
//    return r;
//}

//XBool CFNumberGetValue(CFNumberRef number, XNumberType type, void *valuePtr) {
////printf("+ [%p] CFNumberGetValue(%p, %d, %p)\n", pthread_self(), number, type, valuePtr);
//
//    CF_OBJC_FUNCDISPATCHV(CFNumberGetTypeID(), Boolean, (NSNumber *)number, _getValue:(void *)valuePtr forType:(XNumberType)_XNumberTypeInfoTable[type].type);
//    CF_SWIFT_FUNCDISPATCHV(CFNumberGetTypeID(), Boolean, (CFSwiftRef)number, NSNumber._getValue, valuePtr, (XNumberType)_XNumberTypeInfoTable[type].type);
//    __XAssertIsNumber(number);
//    __XAssertIsValidNumberType(type);
//    uint8_t localMemory[128];
//    Boolean r = __XNumberGetValueCompat(number, type, valuePtr ? valuePtr : localMemory);
//    return r;
//}

XComparisonResult __XNumberCompare(_XNumberStruct * _Nonnull number1, _XNumberStruct * _Nonnull number2) {
    //限定： -0.000...001 < NaN < 0, NaN 大于任何double能表示的负数， 小于0

    XNumberType type1 = number1->type;
    XNumberType type2 = number2->type;
    XBool isFloat1 = (XNumberTypeFloat32 == type1 || XNumberTypeFloat64 == type1);
    XBool isFloat2 = (XNumberTypeFloat32 == type2 || XNumberTypeFloat64 == type2);

    // Both numbers are integers
    if (!isFloat1 && !isFloat2) {
        XSInt128Struct i1, i2;
        __XNumberGetSInt128StructValue(number1, &i1);
        __XNumberGetSInt128StructValue(number2, &i2);
        return cmp128(&i1, &i2);
    }
    // Both numbers are floats
    if (isFloat1 && isFloat2) {
        XFloat64 d1 = number1->bits.f;
        XFloat64 d2 = number2->bits.f;
        double s1 = copysign(1.0, d1);
        double s2 = copysign(1.0, d2);
        if (isnan(d1) && isnan(d2)) return XCompareEqualTo;
        if (isnan(d1)) return (s2 < 0.0) ? XCompareGreaterThan : XCompareLessThan;
        if (isnan(d2)) return (s1 < 0.0) ? XCompareLessThan : XCompareGreaterThan;
        // at this point, we know we don't have any NaNs
        if (s1 < s2) return XCompareLessThan;
        if (s2 < s1) return XCompareGreaterThan;
        // at this point, we know the signs are the same; do not combine these tests
        if (d1 < d2) return XCompareLessThan;
        if (d2 < d1) return XCompareGreaterThan;
        return XCompareEqualTo;
    }
    // One float, one integer; swap if necessary so number1 is the float
    XBool swapResult = false;
    if (isFloat2) {
        _XNumberStruct * tmp = number1;
        number1 = number2;
        number2 = tmp;
        swapResult = true;
    }
    // At large integer values, the precision of double is quite low
    // e.g. all values roughly 2^127 +- 2^73 are represented by 1 double, 2^127.
    // If we just used double compare, that would make the 2^73 largest 128-bit
    // integers look equal, so we have to use integer comparison when possible.
    XFloat64 d1, d2;
    __XNumberGetValue(number1, XNumberTypeFloat64, &d1);
    // if the double value is really big, cannot be equal to integer
    // nan d1 will not compare true here
    XSInt128Struct i1, i2;
    if (isnan(d1)) {
        __XNumberGetSInt128StructValue(number2, &i2);
        if (isNeg128(&i2)) {
            return !swapResult ? XCompareGreaterThan : XCompareLessThan;
        }
        // nan compares less than positive 0 too
        return !swapResult ? XCompareLessThan : XCompareGreaterThan;
    } else {
        if (d1 < FLOAT_NEGATIVE_2_TO_THE_127) {//d1 小于 XSInt128Struct 的最小值
            return !swapResult ? XCompareLessThan : XCompareGreaterThan;
        } else if (FLOAT_POSITIVE_2_TO_THE_127 <= d1) {//d1 大于等于 XSInt128Struct 的最大值+1
            return !swapResult ? XCompareGreaterThan : XCompareLessThan;
        } else {
            __XNumberGetSInt128StructValue(number1, &i1);
            __XNumberGetSInt128StructValue(number2, &i2);
            XComparisonResult res = cmp128(&i1, &i2);
            if (XCompareEqualTo != res) {
                return !swapResult ? res : -res;
            }
            //整数部分、符号部分完全相同
            
            double s1 = copysign(1.0, d1);
            double s2 = isNeg128(&i2) ? -1.0 : 1.0;
            if (s1 < s2) return !swapResult ? XCompareLessThan : XCompareGreaterThan;
            if (s2 < s1) return !swapResult ? XCompareGreaterThan : XCompareLessThan;
            // at this point, we know the signs are the same; do not combine these tests
            __XNumberGetValue(number2, XNumberTypeFloat64, &d2);
            if (d1 < d2) return !swapResult ? XCompareLessThan : XCompareGreaterThan;
            if (d2 < d1) return !swapResult ? XCompareGreaterThan : XCompareLessThan;
            return XCompareEqualTo;
        }
    }
}





XComparisonResult XNumberCompare(XNumber _Nonnull lhs, XNumber _Nonnull rhs) {
    _XNumberStruct contentOfLhs = {};
    _XNumberStruct contentOfRhs = {};
    __XNumberUnpack(lhs, &contentOfLhs, __func__);
    __XNumberUnpack(rhs, &contentOfRhs, __func__);
    return __XNumberCompare(&contentOfLhs, &contentOfRhs);
}


XBool XNumberEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {
    _XNumberStruct contentOfLhs = {};
    _XNumberStruct contentOfRhs = {};
    __XNumberUnpack(lhs, &contentOfLhs, __func__);
    __XNumberUnpack(rhs, &contentOfRhs, __func__);
    XBool b = __XNumberCompare(&contentOfLhs, &contentOfRhs) == XCompareEqualTo;
    return b;
}

XHashCode XNumberHash(XRef _Nonnull ref) {
    XAssert(NULL != ref, __func__, "ref is NULL");
    _XNumberStruct content = {};
    __XNumberUnpack(ref, &content, __func__);
    switch (content.type) {
        case XNumberTypeSInt8:
        case XNumberTypeSInt16:
        case XNumberTypeSInt32:
        case XNumberTypeSInt64: {
            XSInt64 s = content.bits.s;
            return _XHashSInt64(s);
        }
            break;
        case XNumberTypeUInt8:
        case XNumberTypeUInt16:
        case XNumberTypeUInt32:
        case XNumberTypeUInt64: {
            XUInt64 u = content.bits.u;
            return _XHashUInt64(u);
        }
            break;
        case XNumberTypeFloat32:
        case XNumberTypeFloat64: {
            XFloat64 f = content.bits.f;
            return _XHashFloat64(f);
        }
            break;
        default: {
            abort();
        }
            break;
    }
}

#undef BITSFORDOUBLENAN
#undef BITSFORDOUBLEPOSINF
#undef BITSFORDOUBLENEGINF
