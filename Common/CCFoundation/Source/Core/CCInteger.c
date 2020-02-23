//
//  CCInteger.c
//  CCollection
//
//  Created by vector on 2019/6/25.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCInteger.h"
#include <memory.h>
#include <assert.h>

static const int8_t CByteMostSignificant[256] = {-1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};

static const int8_t CByteLeastSignificant[256] = {-1, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};


static const uint8_t CByteSignificantCount[256] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};


CCInt CCUInt64MostSignificantBit(CCUInt64 n) {
    if (0 == n) {
        return -1;
    }
    if (n & 0xFFFFFFFF00000000ull) {
        if (n & 0xFFFF000000000000ull) {
            if (n & 0xFF00000000000000ull) {
                uint64_t v = n >> 56;
                return 56 + CByteMostSignificant[v];
            } else {
                uint64_t v = n >> 48;
                return 48 + CByteMostSignificant[v];
            }
        } else {
            if (n & 0x0000FF0000000000ull) {
                uint64_t v = n >> 40;
                return 40 + CByteMostSignificant[v];
            } else {
                uint64_t v = n >> 32;
                return 32 + CByteMostSignificant[v];
            }
        }
    } else {
        if (n & 0xFFFF0000ull) {
            if (n & 0x00FF0000ull) {
                uint64_t v = n >> 24;
                return 24 + CByteMostSignificant[v];
            } else {
                uint64_t v = n >> 16;
                return 16 + CByteMostSignificant[v];
            }
        } else {
            if (n & 0xFF00ull) {
                uint64_t v = n >> 8;
                return 8 + CByteMostSignificant[v];
            } else {
                return CByteMostSignificant[n];
            }
        }
    }
}
CCInt CCUInt32MostSignificantBit(CCUInt32 n) {
    if (0 == n) {
        return -1;
    }
    
    if (n & 0xFFFF0000ull) {
        if (n & 0x00FF0000ull) {
            uint32_t v = n >> 24;
            return 24 + CByteMostSignificant[v];
        } else {
            uint32_t v = n >> 16;
            return 16 + CByteMostSignificant[v];
        }
    } else {
        if (n & 0xFF00ull) {
            uint32_t v = n >> 8;
            return 8 + CByteMostSignificant[v];
        } else {
            return CByteMostSignificant[n];
        }
    }
}

CCInt CCUInt64LeastSignificantBit(CCUInt64 n) {
    if (0 == n) {
        return -1;
    }
    if (n & 0xFFFFFFFFull) {
        if (n & 0xFFFFull) {
            if (n & 0xFFull) {
                uint64_t v = n & 0xFF;
                return CByteLeastSignificant[v];
            } else {
                uint64_t v = (n >> 8) & 0xFF;
                return 8 + CByteLeastSignificant[v];
            }
        } else {
            if (n & 0xFF0000ull) {
                uint64_t v = (n >> 16) & 0xFF;
                return 16 + CByteLeastSignificant[v];
            } else {
                uint64_t v = (n >> 24) & 0xFF;
                return 24 + CByteLeastSignificant[v];
            }
        }
    } else {
        if (n & 0xFFFF00000000ull) {
            if (n & 0xFF00000000ull) {
                uint64_t v = (n >> 32) & 0xFF;
                return 32 + CByteLeastSignificant[v];
            } else {
                uint64_t v = (n >> 40) & 0xFF;
                return 40 + CByteLeastSignificant[v];
            }
        } else {
            if (n & 0xFF000000000000ull) {
                uint64_t v = (n >> 48) & 0xFF;
                return 48 + CByteLeastSignificant[v];
            } else {
                uint64_t v = (n >> 56) & 0xFF;
                return 56 + CByteLeastSignificant[v];
            }
        }
    }
}

CCInt CCUInt32LeastSignificantBit(CCUInt32 n) {
    if (0 == n) {
        return -1;
    }
    
    if (n & 0xFFFF00000000ull) {
        if (n & 0xFF00000000ull) {
            uint32_t v = n & 0xFF;
            return CByteLeastSignificant[v];
        } else {
            uint64_t v = (n >> 8) & 0xFF;
            return 8 + CByteLeastSignificant[v];
        }
    } else {
        if (n & 0xFF000000000000ull) {
            uint32_t v = (n >> 16) & 0xFF;
            return 16 + CByteLeastSignificant[v];
        } else {
            uint32_t v = (n >> 24) & 0xFF;
            return 24 + CByteLeastSignificant[v];
        }
    }
}

static inline CCInt __CCSInt64Encode(CCUInt64 n, CCIntegerEncoding_e encoding, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength) {
    switch (encoding) {
        case CCIntegerEncodingCompress7BitsPerComponent: {
            CCUInt8 buffer[10] = {};
            CCUInt64 mask = 0x7F;
            CCBool compress0 = ((n & 0x8000000000000000ULL) == 0);
            if (compress0) {
                buffer[0] = 0;
            } else {
                buffer[0] = 0xFF;
            }
            buffer[1] = (n >> 56) & mask;
            buffer[2] = (n >> 49) & mask;
            buffer[3] = (n >> 42) & mask;
            buffer[4] = (n >> 35) & mask;
            buffer[5] = (n >> 28) & mask;
            buffer[6] = (n >> 21) & mask;
            buffer[7] = (n >> 14) & mask;
            buffer[8] = (n >> 7) & mask;
            buffer[9] = n & mask;
            CCInt offset = -1;
            if (compress0) {
                for (CCInt i=0; i<9; i++) {
                    if (!(buffer[i] == 0 && buffer[i+1] < 0x40)) {
                        offset = i;
                        break;
                    }
                }
            } else {
                for (CCInt i=0; i<9; i++) {
                    if (!(buffer[i] == 0x7F && buffer[i+1] >= 0x40)) {
                        offset = i;
                        break;
                    }
                }
            }
            if (offset < 0) {
                offset = 9;
            }
            CCInt requireLength = 10 - offset;
            if (bufferLength < requireLength) {
                return -2;
            }
            
            CCInt outputIndex = 0;
            for (CCInt i=offset; i<9; i++) {
                outputBuffer[outputIndex] = (buffer[i] | 0x80);
                outputIndex += 1;
            }
            outputBuffer[outputIndex] = buffer[9];
            return requireLength;
        }
            break;
        case CCIntegerEncodingCompressNoFlag: {
            CCUInt8 buffer[8] = {};
#if BUILD_TARGET_RT_LITTLE_ENDIAN
            const uint8_t * bytes = (const uint8_t *)(&n);
            buffer[7] = bytes[0];
            buffer[6] = bytes[1];
            buffer[5] = bytes[2];
            buffer[4] = bytes[3];
            buffer[3] = bytes[4];
            buffer[2] = bytes[5];
            buffer[1] = bytes[6];
            buffer[0] = bytes[7];
#else
            ((CCUInt64 *)buffer)[0] = n;
#endif
            
            CCBool compress0 = ((n & 0x8000000000000000ULL) == 0);
            CCInt offset = -1;
            if (compress0) {
                for (CCInt i=0; i<7; i++) {
                    if (!(buffer[i] == 0 && buffer[i+1] < 0x80)) {
                        offset = i;
                        break;
                    }
                }
            } else {
                for (CCInt i=0; i<7; i++) {
                    if (!(buffer[i] == 0xFF && buffer[i+1] >= 0x80)) {
                        offset = i;
                        break;
                    }
                }
            }
            if (offset < 0) {
                offset = 7;
            }
            CCInt requireLength = 8 - offset;
            if (bufferLength < requireLength) {
                return -2;
            }
            memcpy(outputBuffer, &(buffer[offset]), requireLength);
            return requireLength;
        }
            break;
        default: {
            return -1;
        }
            break;
    }
}
static inline CCInt __CCSInt32Encode(CCUInt32 n, CCIntegerEncoding_e encoding, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength) {
    switch (encoding) {
        case CCIntegerEncodingCompress7BitsPerComponent: {
            CCUInt8 buffer[5] = {};
            CCUInt32 mask = 0x7F;
            CCBool compress0 = ((n & 0x80000000UL) == 0);
            if (compress0) {
                buffer[0] = ((n >> 28) & mask);
            } else {
                buffer[0] = (((n >> 28) & mask) | 0x70);
            }
            buffer[1] = (n >> 21) & mask;
            buffer[2] = (n >> 14) & mask;
            buffer[3] = (n >> 7) & mask;
            buffer[4] = n & mask;
            CCInt offset = -1;
            if (compress0) {
                for (CCInt i=0; i<4; i++) {
                    if (!(buffer[i] == 0 && buffer[i+1] < 0x40)) {
                        offset = i;
                        break;
                    }
                }
            } else {
                for (CCInt i=0; i<4; i++) {
                    if (!(buffer[i] == 0x7F && buffer[i+1] >= 0x40)) {
                        offset = i;
                        break;
                    }
                }
            }
            if (offset < 0) {
                offset = 4;
            }
            CCInt requireLength = 5 - offset;
            if (bufferLength < requireLength) {
                return -2;
            }
            CCInt outputIndex = 0;
            for (CCInt i=offset; i<4; i++) {
                outputBuffer[outputIndex] = (buffer[i] | 0x80);
                outputIndex += 1;
            }
            outputBuffer[outputIndex] = buffer[4];
            return requireLength;
        }
            break;
        case CCIntegerEncodingCompressNoFlag: {
            CCUInt8 buffer[4] = {};
#if BUILD_TARGET_RT_LITTLE_ENDIAN
            const uint8_t * bytes = (const uint8_t *)(&n);
            buffer[3] = bytes[0];
            buffer[2] = bytes[1];
            buffer[1] = bytes[2];
            buffer[0] = bytes[3];
#else
            ((CCUInt32 *)buffer)[0] = n;
#endif
            
            CCBool compress0 = ((n & 0x80000000UL) == 0);
            CCInt offset = -1;
            if (compress0) {
                for (CCInt i=0; i<3; i++) {
                    if (!(buffer[i] == 0 && buffer[i+1] < 0x80)) {
                        offset = i;
                        break;
                    }
                }
            } else {
                for (CCInt i=0; i<3; i++) {
                    if (!(buffer[i] == 0xFF && buffer[i+1] >= 0x80)) {
                        offset = i;
                        break;
                    }
                }
            }
            if (offset < 0) {
                offset = 3;
            }
            CCInt requireLength = 4 - offset;
            if (bufferLength < requireLength) {
                return -2;
            }
            memcpy(outputBuffer, &(buffer[offset]), requireLength);
            return requireLength;
        }
            break;
        default: {
            return -1;
        }
            break;
    }
}


static inline CCInt __CCUInt64Encode(CCUInt64 n, CCIntegerEncoding_e encoding, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength) {
    switch (encoding) {
        case CCIntegerEncodingCompress7BitsPerComponent: {
            CCUInt8 buffer[10] = {};
            CCUInt64 mask = 0x7F;
            buffer[0] = (n >> 63) & mask;
            buffer[1] = (n >> 56) & mask;
            buffer[2] = (n >> 49) & mask;
            buffer[3] = (n >> 42) & mask;
            buffer[4] = (n >> 35) & mask;
            buffer[5] = (n >> 28) & mask;
            buffer[6] = (n >> 21) & mask;
            buffer[7] = (n >> 14) & mask;
            buffer[8] = (n >> 7) & mask;
            buffer[9] = n & mask;
            CCInt offset = -1;
            for (CCInt i=0; i<9; i++) {
                if (buffer[i] != 0) {
                    offset = i;
                    break;
                }
            }

            if (offset < 0) {
                offset = 9;
            }
            CCInt requireLength = 10 - offset;
            if (bufferLength < requireLength) {
                return -2;
            }
            CCInt outputIndex = 0;
            for (CCInt i=offset; i<9; i++) {
                outputBuffer[outputIndex] = (buffer[i] | 0x80);
                outputIndex += 1;
            }
            outputBuffer[outputIndex] = buffer[9];
            return requireLength;
        }
            break;
        case CCIntegerEncodingCompressNoFlag: {
            CCUInt8 buffer[8] = {};
#if BUILD_TARGET_RT_LITTLE_ENDIAN
            const uint8_t * bytes = (const uint8_t *)(&n);
            buffer[7] = bytes[0];
            buffer[6] = bytes[1];
            buffer[5] = bytes[2];
            buffer[4] = bytes[3];
            buffer[3] = bytes[4];
            buffer[2] = bytes[5];
            buffer[1] = bytes[6];
            buffer[0] = bytes[7];
#else
            ((CCUInt64 *)buffer)[0] = n;
#endif
            
            CCInt offset = -1;
            for (CCInt i=0; i<7; i++) {
                if (buffer[i] != 0) {
                    offset = i;
                    break;
                }
            }
            if (offset < 0) {
                offset = 7;
            }
            CCInt requireLength = 8 - offset;
            if (bufferLength < requireLength) {
                return -2;
            }
            memcpy(outputBuffer, &(buffer[offset]), requireLength);
            return requireLength;
        }
            break;
        default: {
            return -1;
        }
            break;
    }
}
static inline CCInt __CCUInt32Encode(CCUInt32 n, CCIntegerEncoding_e encoding, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength) {
    switch (encoding) {
        case CCIntegerEncodingCompress7BitsPerComponent: {
            CCUInt8 buffer[5] = {};
            CCUInt32 mask = 0x7F;
            buffer[0] = ((n >> 28) & mask);
            buffer[1] = (n >> 21) & mask;
            buffer[2] = (n >> 14) & mask;
            buffer[3] = (n >> 7) & mask;
            buffer[4] = n & mask;
            CCInt offset = -1;
            for (CCInt i=0; i<4; i++) {
                if (buffer[i] != 0) {
                    offset = i;
                    break;
                }
            }

            if (offset < 0) {
                offset = 4;
            }
            CCInt requireLength = 5 - offset;
            if (bufferLength < requireLength) {
                return -2;
            }
            CCInt outputIndex = 0;
            for (CCInt i=offset; i<4; i++) {
                outputBuffer[outputIndex] = (buffer[i] | 0x80);
                outputIndex += 1;
            }
            outputBuffer[outputIndex] = buffer[4];
            return requireLength;
        }
            break;
        case CCIntegerEncodingCompressNoFlag: {
            CCUInt8 buffer[4] = {};
#if BUILD_TARGET_RT_LITTLE_ENDIAN
            const uint8_t * bytes = (const uint8_t *)(&n);
            buffer[3] = bytes[0];
            buffer[2] = bytes[1];
            buffer[1] = bytes[2];
            buffer[0] = bytes[3];
#else
            ((CCUInt32 *)buffer)[0] = n;
#endif
            
            CCInt offset = -1;
            for (CCInt i=0; i<3; i++) {
                if (buffer[i] != 0) {
                    offset = i;
                    break;
                }
            }
            if (offset < 0) {
                offset = 3;
            }
            CCInt requireLength = 4 - offset;
            if (bufferLength < requireLength) {
                return -2;
            }
            memcpy(outputBuffer, &(buffer[offset]), requireLength);
            return requireLength;
        }
            break;
        default: {
            return -1;
        }
            break;
    }
}




CCInt CCUInt64Encode(CCUInt64 n, CCIntegerEncoding_e encoding, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength) {
    return __CCUInt64Encode(n, encoding, outputBuffer, bufferLength);
}
CCInt CCUInt32Encode(CCUInt32 n, CCIntegerEncoding_e encoding, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength) {
    return __CCUInt32Encode(n, encoding, outputBuffer, bufferLength);
}

CCInt CCSInt64Encode(CCSInt64 n, CCIntegerEncoding_e encoding, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength) {
    return __CCSInt64Encode(n, encoding, outputBuffer, bufferLength);
}
CCInt CCSInt32Encode(CCSInt32 n, CCIntegerEncoding_e encoding, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength) {
    return __CCSInt32Encode(n, encoding, outputBuffer, bufferLength);
}



CCInt CCUInt64Decode(CCUInt64 * _Nonnull n, CCIntegerEncoding_e encoding, const CCUInt8 * _Nonnull bytes, CCInt bytesLength) {
    if (bytesLength <= 0) {
        return -1;
    }
    assert(n);
    assert(bytes);
    CCUInt64 result = 0;
    switch (encoding) {
        case CCIntegerEncodingCompress7BitsPerComponent: {
            CCInt upper = MIN(bytesLength, 10);
            for (CCInt idx=0; idx<upper; idx++) {
                CCUInt64 v = bytes[idx];
                if (v >= 0x80) {
                    result = (result << 7) | (v & 0x7F);
                } else {
                    result = (result << 7) | v;
                    if (9 == idx && 0 != ((bytes[0] & 0x7E))) {
                        return -2;
                    }
                    *n = result;
                    return idx + 1;
                }
            }
            return -2;
        }
            break;
        case CCIntegerEncodingCompressNoFlag: {
            if (bytesLength > 8) {
                return -2;
            }
            for (CCInt idx=0; idx<bytesLength; idx++) {
                CCUInt64 v = bytes[idx];
                result = (result << 8) | v;
            }
            *n = result;
            return bytesLength;
        }
            break;
        default: {
            return -1;
        }
            break;
    }
}
CCInt CCSInt64Decode(CCSInt64 * _Nonnull n, CCIntegerEncoding_e encoding, const CCUInt8 * _Nonnull bytes, CCInt bytesLength) {
    if (bytesLength <= 0) {
        return -1;
    }
    assert(n);
    assert(bytes);

    CCUInt64 result = 0;
    
    switch (encoding) {
        case CCIntegerEncodingCompress7BitsPerComponent: {
            CCBool compress0 = ((bytes[0] & 0x40) == 0);
            if (!compress0) {
                result = ~0ULL;
            }

            CCInt upper = MIN(bytesLength, 10);
            for (CCInt idx=0; idx<upper; idx++) {
                CCUInt64 v = bytes[idx];
                if (v >= 0x80) {
                    result = (result << 7) | (v & 0x7F);
                } else {
                    result = (result << 7) | v;
                    if (9 == idx && (0x80 != bytes[0] || 0xFF != bytes[0])) {
                        return -2;
                    }
                    *n = (CCSInt64)result;
                    return idx + 1;
                }
            }
            return -2;
        }
            break;
        case CCIntegerEncodingCompressNoFlag: {
            if (bytesLength > 8) {
                return -2;
            }
            CCBool compress0 = ((bytes[0] & 0x80) == 0);
            if (!compress0) {
                result = ~0ULL;
            }

            for (CCInt idx=0; idx<bytesLength; idx++) {
                CCUInt64 v = bytes[idx];
                result = (result << 8) | v;
            }
            *n = (CCSInt64)result;
            return bytesLength;
        }
            break;
        default: {
            return -1;
        }
            break;
    }
}



static inline CCInt __CCIntegerToBytes(void * _Nonnull n, CCInt len, CCBool swap, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength) {
    if (bufferLength < len) {
        return -1;
    }
    assert(outputBuffer);
    if (len > 1 && swap) {
        CCUInt8 * v = (CCUInt8 *)n;
        CCInt upper = len / 2;
        for (CCInt i=0; i<upper; i++) {
            CCUInt8 tmp = v[i];
            v[i] = v[len-1-i];
            v[len-1-i] = tmp;
        }
    }
    memcpy(outputBuffer, n, len);
    return len;
}

static inline CCBool __CCIntegerBytesSwap(CCBool bigEndianEncode) {
#if BUILD_TARGET_RT_LITTLE_ENDIAN
    return bigEndianEncode;
#else
    return !bigEndianEncode;
#endif
}


CCInt CCUInt64ToBytes(CCUInt64 n, CCBool bigEndianEncode, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength) {
    return __CCIntegerToBytes(&n, 8, __CCIntegerBytesSwap(bigEndianEncode), outputBuffer, bufferLength);
}
CCInt CCUInt32ToBytes(CCUInt32 n, CCBool bigEndianEncode, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength) {
    return __CCIntegerToBytes(&n, 4, __CCIntegerBytesSwap(bigEndianEncode), outputBuffer, bufferLength);
}
CCInt CCUInt16ToBytes(CCUInt16 n, CCBool bigEndianEncode, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength) {
    return __CCIntegerToBytes(&n, 2, __CCIntegerBytesSwap(bigEndianEncode), outputBuffer, bufferLength);
}

CCInt CCSInt64ToBytes(CCSInt64 n, CCBool bigEndianEncode, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength) {
    return __CCIntegerToBytes(&n, 8, __CCIntegerBytesSwap(bigEndianEncode), outputBuffer, bufferLength);
}
CCInt CCSInt32ToBytes(CCSInt32 n, CCBool bigEndianEncode, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength) {
    return __CCIntegerToBytes(&n, 4, __CCIntegerBytesSwap(bigEndianEncode), outputBuffer, bufferLength);
}
CCInt CCSInt16ToBytes(CCSInt16 n, CCBool bigEndianEncode, CCUInt8 * _Nonnull outputBuffer, CCInt bufferLength) {
    return __CCIntegerToBytes(&n, 2, __CCIntegerBytesSwap(bigEndianEncode), outputBuffer, bufferLength);
}


static inline CCInt __CCBytesToInteger(const CCUInt8 * _Nonnull bytes, CCInt bytesLength, void * _Nonnull n, CCInt len, CCBool swap) {
    if (bytesLength < len) {
        return -1;
    }
    assert(n);
    assert(bytes);
    memcpy(n, bytes, len);
    if (len > 1 && swap) {
        CCUInt8 * v = (CCUInt8 *)n;
        CCInt upper = len / 2;
        for (CCInt i=0; i<upper; i++) {
            CCUInt8 tmp = v[i];
            v[i] = v[len-1-i];
            v[len-1-i] = tmp;
        }
    }
    return len;
}


CCInt CCBytesToUInt64(const CCUInt8 * _Nonnull bytes, CCInt length, CCBool bigEndianEncode, CCUInt64 * _Nonnull n) {
    return __CCBytesToInteger(bytes, length, n, 8, __CCIntegerBytesSwap(bigEndianEncode));
}
CCInt CCBytesToUInt32(const CCUInt8 * _Nonnull bytes, CCInt length, CCBool bigEndianEncode, CCUInt32 * _Nonnull n) {
    return __CCBytesToInteger(bytes, length, n, 4, __CCIntegerBytesSwap(bigEndianEncode));
}
CCInt CCBytesToUInt16(const CCUInt8 * _Nonnull bytes, CCInt length, CCBool bigEndianEncode, CCUInt16 * _Nonnull n) {
    return __CCBytesToInteger(bytes, length, n, 2, __CCIntegerBytesSwap(bigEndianEncode));
}

CCInt CCBytesToSInt64(const CCUInt8 * _Nonnull bytes, CCInt length, CCBool bigEndianEncode, CCSInt64 * _Nonnull n) {
    return __CCBytesToInteger(bytes, length, n, 8, __CCIntegerBytesSwap(bigEndianEncode));
}
CCInt CCBytesToSInt32(const CCUInt8 * _Nonnull bytes, CCInt length, CCBool bigEndianEncode, CCSInt32 * _Nonnull n) {
    return __CCBytesToInteger(bytes, length, n, 4, __CCIntegerBytesSwap(bigEndianEncode));
}
CCInt CCBytesToSInt16(const CCUInt8 * _Nonnull bytes, CCInt length, CCBool bigEndianEncode, CCSInt16 * _Nonnull n) {
    return __CCBytesToInteger(bytes, length, n, 2, __CCIntegerBytesSwap(bigEndianEncode));
}
