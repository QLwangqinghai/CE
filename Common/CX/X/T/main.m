//
//  main.m
//  T
//
//  Created by vector on 2020/4/22.
//  Copyright © 2020 haoqi. All rights reserved.
//

#import <Foundation/Foundation.h>

#define BITSFORDOUBLEPOSINF32     ((uint32_t)0x7f800000UL)
#define BITSFORDOUBLENAN32   ((uint32_t)0x7fc00000UL)
#define BITSFORDOUBLENEGINF32    ((uint32_t)0xff800000UL)

#define BITSFORDOUBLENAN    ((uint64_t)0x7ff8000000000000ULL)
#define BITSFORDOUBLEPOSINF    ((uint64_t)0x7ff0000000000000ULL)
#define BITSFORDOUBLENEGINF    ((uint64_t)0xfff0000000000000ULL)

typedef uint8_t XUInt8;
typedef uint16_t XUInt16;
typedef uint32_t XUInt32;
typedef uint64_t XUInt64;

typedef int8_t XSInt8;
typedef int16_t XSInt16;
typedef int32_t XSInt32;
typedef int64_t XSInt64;

typedef float XFloat32;
typedef double XFloat64;


typedef union {
    XSInt32 s;
    XUInt32 u;
    XFloat32 f;
} _XNumberBits32_u;

typedef union {
    XSInt64 s;
    XUInt64 u;
    XFloat64 f;
} _XNumberBits64_u;

static const _XNumberBits32_u _XNumberBits32PositiveInfinity = {.u = BITSFORDOUBLEPOSINF32};
static const _XNumberBits32_u _XNumberBits32Nan = {.u = BITSFORDOUBLENAN32};
static const _XNumberBits32_u _XNumberBits32NegativeInfinity = {.u = BITSFORDOUBLENEGINF32};


static const _XNumberBits64_u _XNumberBits64PositiveInfinity = {.u = BITSFORDOUBLEPOSINF};
static const _XNumberBits64_u _XNumberBits64Nan = {.u = BITSFORDOUBLENAN};
static const _XNumberBits64_u _XNumberBits64NegativeInfinity = {.u = BITSFORDOUBLENEGINF};


typedef int XComparisonResult;
const XComparisonResult XCompareLessThan = -1;
const XComparisonResult XCompareEqualTo = 0;
const XComparisonResult XCompareGreaterThan = 1;


typedef struct {
    int64_t high;
    uint64_t low;
    double f;
} _XRealNumber;

static XComparisonResult _XRealNumberCompare(const _XRealNumber *in1, const _XRealNumber *in2) {
    if (in1->high < in2->high) return XCompareLessThan;
    if (in1->high > in2->high) return XCompareGreaterThan;
    if (in1->low < in2->low) return XCompareLessThan;
    if (in1->low > in2->low) return XCompareGreaterThan;
    if (in1->f < in2->f) return XCompareLessThan;
    if (in1->f > in2->f) return XCompareGreaterThan;
    return XCompareEqualTo;
}

#define FLOAT_POSITIVE_2_TO_THE_127    170141183460469231731687303715884105728.0


int main(int argc, const char * argv[]) {
    @autoreleasepool {
        int64_t v = -0x10000000000000LL;
        NSLog(@"v %@", @(v));
        
        
        float nanf = _XNumberBits32Nan.f;
        double nanf64 = (double)nanf;
        NSLog(@"nan equal %ld", nanf == nanf64);
        
        
        // insert code here...
        NSLog(@"Hello, World!");
        
        NSLog(@"%ld", sizeof(long double));
        
        int32_t a = INT32_MIN;
        NSLog(@"%d", a);
        uint32_t c = (uint32_t)a;
        NSLog(@"%u", c);

        a *= -1;
        
        uint32_t b = (uint32_t)a;
        NSLog(@"%u", b);
        NSLog(@"%d", a);

         //nan < 0
        NSNumber * number = (__bridge NSNumber *)kCFNumberNaN;
        NSNumber * n1 = @(-0.00001);
        NSComparisonResult r = [number compare:n1];
        NSComparisonResult r1 = [n1 compare:number];

        
        int64_t ac = number.longLongValue;
        NSLog(@"%d", r);
        NSLog(@"%d", r1);
        
        uint64_t nanInt = 0x7ff8000000000000ULL;
        double nan = *(double *)(&nanInt);
        
        if (nan > 0) {
            NSLog(@"nan > 0");

        }
        if (nan < 0) {
            NSLog(@"nan < 0");
        }
        
        isnan(nan);
        
        float pif = _XNumberBits32PositiveInfinity.f;
        float nif = _XNumberBits32NegativeInfinity.f;
        
        _XNumberBits64_u nand = {.f = (double)nanf};
        _XNumberBits64_u pid = {.f = (double)pif};
        _XNumberBits64_u nid = {.f = (double)nif};

        NSLog(@"%d", _XNumberBits64PositiveInfinity.u == pid.u);
        NSLog(@"%d", _XNumberBits64Nan.u == nand.u);
        NSLog(@"%d", _XNumberBits64NegativeInfinity.u == nid.u);
        uint64_t maxInt = UINT64_MAX;
        NSLog(@"%llu", maxInt);

        double maxD = maxInt;
        NSLog(@"%lf", maxD);
        long double maxDl = maxInt;
        NSLog(@"%Lf", maxDl);

        uint64_t maxInt2 = maxD;
        NSLog(@"%d", maxInt == maxInt2);
        
        double ddd = 0.1;
        
        double ddd1 = floor(ddd);
        
        
        double ddd2 = -0.1;
        
        double ddd3 = floor(ddd2);

        double aaa = 3.4123413;
        double aaaI = 0;
        double aaas = modf(aaa, &aaaI);

        
        double faaa = -3.4123413;
        double faaaI = 0;
        double faaas = modf(faaa, &faaaI);
        
        long double ll = 170141183460469231731687303715884105728.0;
        double ll2 = ll;
        long double ll3 = ll2;

        NSLog(@"%d", ll == ll3);
        
        int csfaf = 1;
        
        long double ddfsfl = 170141183460469231731687303715884105728.0L;

        double ddfsf = 170141183460469231731687303715884105728.0L;
        
        double offset = 1;
        //double 1 11 52    128-52
        
        double d127 = 1.0;
        for (int i=0; i<127; i++) {
            d127 *= 2;
        }
        NSLog(@"127: %lf", d127);

        for (int i=0; i<1024; i++) {
            double tmp = ddfsf - offset;
            if (tmp != ddfsf) {
                NSLog(@"%lf", ddfsf);
                NSLog(@"%lf", tmp);
                break;
            }
            offset = offset * 2;
        }

  
        double d127_74 = -170141183460469212842221372237303250944.0L;
        
        double d127_64 = 18446744073709551616.0L;

        double t = floor(d127_74 / d127_64);
        NSLog(@"t: %lf", t);

        int64_t high = (int64_t)t;
        uint64_t low = (uint64_t)(d127_74 - t * d127_64);
        
        
        NSLog(@"%d", ddfsfl == (long double)ddfsf);

        
        double d64 = -18446744073709551616.0L - 16384L;
        
        double t1 = floor(d64 / d127_64);
        NSLog(@"t1: %lf", t1);

        int64_t high1 = (int64_t)t1;
        uint64_t low1 = (uint64_t)(d64 - t1 * d127_64);
        
        
        NSLog(@"%d", ddfsfl == (long double)ddfsf);
        
    }
    return 0;
}
