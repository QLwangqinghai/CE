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


SInteger CCUInt64MostSignificantBit(uint64_t n) {
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
SInteger CCUInt32MostSignificantBit(uint32_t n) {
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

SInteger CCUInt64LeastSignificantBit(uint64_t n) {
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

SInteger CCUInt32LeastSignificantBit(uint32_t n) {
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

static inline SInteger __CCSInt64Encode(uint64_t n, CCIntegerEncoding_e encoding, uint8_t * _Nonnull outputBuffer, SInteger bufferLength) {
    switch (encoding) {
        case CCIntegerEncodingCompress7BitsPerComponent: {
            uint8_t buffer[10] = {};
            uint64_t mask = 0x7F;
            CBool compress0 = ((n & 0x8000000000000000ULL) == 0);
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
            SInteger offset = -1;
            if (compress0) {
                for (SInteger i=0; i<9; i++) {
                    if (!(buffer[i] == 0 && buffer[i+1] < 0x40)) {
                        offset = i;
                        break;
                    }
                }
            } else {
                for (SInteger i=0; i<9; i++) {
                    if (!(buffer[i] == 0x7F && buffer[i+1] >= 0x40)) {
                        offset = i;
                        break;
                    }
                }
            }
            if (offset < 0) {
                offset = 9;
            }
            SInteger requireLength = 10 - offset;
            if (bufferLength < requireLength) {
                return -2;
            }
            
            SInteger outputIndex = 0;
            for (SInteger i=offset; i<9; i++) {
                outputBuffer[outputIndex] = (buffer[i] | 0x80);
                outputIndex += 1;
            }
            outputBuffer[outputIndex] = buffer[9];
            return requireLength;
        }
            break;
        case CCIntegerEncodingCompressNoFlag: {
            uint8_t buffer[8] = {};
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
            
            CBool compress0 = ((n & 0x8000000000000000ULL) == 0);
            SInteger offset = -1;
            if (compress0) {
                for (SInteger i=0; i<7; i++) {
                    if (!(buffer[i] == 0 && buffer[i+1] < 0x80)) {
                        offset = i;
                        break;
                    }
                }
            } else {
                for (SInteger i=0; i<7; i++) {
                    if (!(buffer[i] == 0xFF && buffer[i+1] >= 0x80)) {
                        offset = i;
                        break;
                    }
                }
            }
            if (offset < 0) {
                offset = 7;
            }
            SInteger requireLength = 8 - offset;
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
static inline SInteger __CCSInt32Encode(uint32_t n, CCIntegerEncoding_e encoding, uint8_t * _Nonnull outputBuffer, SInteger bufferLength) {
    switch (encoding) {
        case CCIntegerEncodingCompress7BitsPerComponent: {
            uint8_t buffer[5] = {};
            uint32_t mask = 0x7F;
            CBool compress0 = ((n & 0x80000000UL) == 0);
            if (compress0) {
                buffer[0] = ((n >> 28) & mask);
            } else {
                buffer[0] = (((n >> 28) & mask) | 0x70);
            }
            buffer[1] = (n >> 21) & mask;
            buffer[2] = (n >> 14) & mask;
            buffer[3] = (n >> 7) & mask;
            buffer[4] = n & mask;
            SInteger offset = -1;
            if (compress0) {
                for (SInteger i=0; i<4; i++) {
                    if (!(buffer[i] == 0 && buffer[i+1] < 0x40)) {
                        offset = i;
                        break;
                    }
                }
            } else {
                for (SInteger i=0; i<4; i++) {
                    if (!(buffer[i] == 0x7F && buffer[i+1] >= 0x40)) {
                        offset = i;
                        break;
                    }
                }
            }
            if (offset < 0) {
                offset = 4;
            }
            SInteger requireLength = 5 - offset;
            if (bufferLength < requireLength) {
                return -2;
            }
            SInteger outputIndex = 0;
            for (SInteger i=offset; i<4; i++) {
                outputBuffer[outputIndex] = (buffer[i] | 0x80);
                outputIndex += 1;
            }
            outputBuffer[outputIndex] = buffer[4];
            return requireLength;
        }
            break;
        case CCIntegerEncodingCompressNoFlag: {
            uint8_t buffer[4] = {};
#if BUILD_TARGET_RT_LITTLE_ENDIAN
            const uint8_t * bytes = (const uint8_t *)(&n);
            buffer[3] = bytes[0];
            buffer[2] = bytes[1];
            buffer[1] = bytes[2];
            buffer[0] = bytes[3];
#else
            ((CCUInt32 *)buffer)[0] = n;
#endif
            
            CBool compress0 = ((n & 0x80000000UL) == 0);
            SInteger offset = -1;
            if (compress0) {
                for (SInteger i=0; i<3; i++) {
                    if (!(buffer[i] == 0 && buffer[i+1] < 0x80)) {
                        offset = i;
                        break;
                    }
                }
            } else {
                for (SInteger i=0; i<3; i++) {
                    if (!(buffer[i] == 0xFF && buffer[i+1] >= 0x80)) {
                        offset = i;
                        break;
                    }
                }
            }
            if (offset < 0) {
                offset = 3;
            }
            SInteger requireLength = 4 - offset;
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


static inline SInteger __CCUInt64Encode(uint64_t n, CCIntegerEncoding_e encoding, uint8_t * _Nonnull outputBuffer, SInteger bufferLength) {
    switch (encoding) {
        case CCIntegerEncodingCompress7BitsPerComponent: {
            uint8_t buffer[10] = {};
            uint64_t mask = 0x7F;
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
            SInteger offset = -1;
            for (SInteger i=0; i<9; i++) {
                if (buffer[i] != 0) {
                    offset = i;
                    break;
                }
            }

            if (offset < 0) {
                offset = 9;
            }
            SInteger requireLength = 10 - offset;
            if (bufferLength < requireLength) {
                return -2;
            }
            SInteger outputIndex = 0;
            for (SInteger i=offset; i<9; i++) {
                outputBuffer[outputIndex] = (buffer[i] | 0x80);
                outputIndex += 1;
            }
            outputBuffer[outputIndex] = buffer[9];
            return requireLength;
        }
            break;
        case CCIntegerEncodingCompressNoFlag: {
            uint8_t buffer[8] = {};
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
            
            SInteger offset = -1;
            for (SInteger i=0; i<7; i++) {
                if (buffer[i] != 0) {
                    offset = i;
                    break;
                }
            }
            if (offset < 0) {
                offset = 7;
            }
            SInteger requireLength = 8 - offset;
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
static inline SInteger __CCUInt32Encode(uint32_t n, CCIntegerEncoding_e encoding, uint8_t * _Nonnull outputBuffer, SInteger bufferLength) {
    switch (encoding) {
        case CCIntegerEncodingCompress7BitsPerComponent: {
            uint8_t buffer[5] = {};
            uint32_t mask = 0x7F;
            buffer[0] = ((n >> 28) & mask);
            buffer[1] = (n >> 21) & mask;
            buffer[2] = (n >> 14) & mask;
            buffer[3] = (n >> 7) & mask;
            buffer[4] = n & mask;
            SInteger offset = -1;
            for (SInteger i=0; i<4; i++) {
                if (buffer[i] != 0) {
                    offset = i;
                    break;
                }
            }

            if (offset < 0) {
                offset = 4;
            }
            SInteger requireLength = 5 - offset;
            if (bufferLength < requireLength) {
                return -2;
            }
            SInteger outputIndex = 0;
            for (SInteger i=offset; i<4; i++) {
                outputBuffer[outputIndex] = (buffer[i] | 0x80);
                outputIndex += 1;
            }
            outputBuffer[outputIndex] = buffer[4];
            return requireLength;
        }
            break;
        case CCIntegerEncodingCompressNoFlag: {
            uint8_t buffer[4] = {};
#if BUILD_TARGET_RT_LITTLE_ENDIAN
            const uint8_t * bytes = (const uint8_t *)(&n);
            buffer[3] = bytes[0];
            buffer[2] = bytes[1];
            buffer[1] = bytes[2];
            buffer[0] = bytes[3];
#else
            ((CCUInt32 *)buffer)[0] = n;
#endif
            
            SInteger offset = -1;
            for (SInteger i=0; i<3; i++) {
                if (buffer[i] != 0) {
                    offset = i;
                    break;
                }
            }
            if (offset < 0) {
                offset = 3;
            }
            SInteger requireLength = 4 - offset;
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




SInteger CCUInt64Encode(uint64_t n, CCIntegerEncoding_e encoding, uint8_t * _Nonnull outputBuffer, SInteger bufferLength) {
    return __CCUInt64Encode(n, encoding, outputBuffer, bufferLength);
}
SInteger CCUInt32Encode(uint32_t n, CCIntegerEncoding_e encoding, uint8_t * _Nonnull outputBuffer, SInteger bufferLength) {
    return __CCUInt32Encode(n, encoding, outputBuffer, bufferLength);
}

SInteger CCSInt64Encode(int64_t n, CCIntegerEncoding_e encoding, uint8_t * _Nonnull outputBuffer, SInteger bufferLength) {
    return __CCSInt64Encode(n, encoding, outputBuffer, bufferLength);
}
SInteger CCSInt32Encode(int32_t n, CCIntegerEncoding_e encoding, uint8_t * _Nonnull outputBuffer, SInteger bufferLength) {
    return __CCSInt32Encode(n, encoding, outputBuffer, bufferLength);
}



SInteger CCUInt64Decode(uint64_t * _Nonnull n, CCIntegerEncoding_e encoding, const uint8_t * _Nonnull bytes, SInteger bytesLength) {
    if (bytesLength <= 0) {
        return -1;
    }
    assert(n);
    assert(bytes);
    uint64_t result = 0;
    switch (encoding) {
        case CCIntegerEncodingCompress7BitsPerComponent: {
            SInteger upper = MIN(bytesLength, 10);
            for (SInteger idx=0; idx<upper; idx++) {
                uint64_t v = bytes[idx];
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
            for (SInteger idx=0; idx<bytesLength; idx++) {
                uint64_t v = bytes[idx];
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
SInteger CCSInt64Decode(int64_t * _Nonnull n, CCIntegerEncoding_e encoding, const uint8_t * _Nonnull bytes, SInteger bytesLength) {
    if (bytesLength <= 0) {
        return -1;
    }
    assert(n);
    assert(bytes);

    uint64_t result = 0;
    
    switch (encoding) {
        case CCIntegerEncodingCompress7BitsPerComponent: {
            CBool compress0 = ((bytes[0] & 0x40) == 0);
            if (!compress0) {
                result = ~0ULL;
            }

            SInteger upper = MIN(bytesLength, 10);
            for (SInteger idx=0; idx<upper; idx++) {
                uint64_t v = bytes[idx];
                if (v >= 0x80) {
                    result = (result << 7) | (v & 0x7F);
                } else {
                    result = (result << 7) | v;
                    if (9 == idx && (0x80 != bytes[0] || 0xFF != bytes[0])) {
                        return -2;
                    }
                    *n = (int64_t)result;
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
            CBool compress0 = ((bytes[0] & 0x80) == 0);
            if (!compress0) {
                result = ~0ULL;
            }

            for (SInteger idx=0; idx<bytesLength; idx++) {
                uint64_t v = bytes[idx];
                result = (result << 8) | v;
            }
            *n = (int64_t)result;
            return bytesLength;
        }
            break;
        default: {
            return -1;
        }
            break;
    }
}



static inline SInteger __CCIntegerToBytes(void * _Nonnull n, SInteger len, CBool swap, uint8_t * _Nonnull outputBuffer, SInteger bufferLength) {
    if (bufferLength < len) {
        return -1;
    }
    assert(outputBuffer);
    if (len > 1 && swap) {
        uint8_t * v = (uint8_t *)n;
        SInteger upper = len / 2;
        for (SInteger i=0; i<upper; i++) {
            uint8_t tmp = v[i];
            v[i] = v[len-1-i];
            v[len-1-i] = tmp;
        }
    }
    memcpy(outputBuffer, n, len);
    return len;
}

static inline CBool __CCIntegerBytesSwap(CBool bigEndianEncode) {
#if BUILD_TARGET_RT_LITTLE_ENDIAN
    return bigEndianEncode;
#else
    return !bigEndianEncode;
#endif
}


SInteger CCUInt64ToBytes(uint64_t n, CBool bigEndianEncode, uint8_t * _Nonnull outputBuffer, SInteger bufferLength) {
    return __CCIntegerToBytes(&n, 8, __CCIntegerBytesSwap(bigEndianEncode), outputBuffer, bufferLength);
}
SInteger CCUInt32ToBytes(uint32_t n, CBool bigEndianEncode, uint8_t * _Nonnull outputBuffer, SInteger bufferLength) {
    return __CCIntegerToBytes(&n, 4, __CCIntegerBytesSwap(bigEndianEncode), outputBuffer, bufferLength);
}
SInteger CCUInt16ToBytes(uint16_t n, CBool bigEndianEncode, uint8_t * _Nonnull outputBuffer, SInteger bufferLength) {
    return __CCIntegerToBytes(&n, 2, __CCIntegerBytesSwap(bigEndianEncode), outputBuffer, bufferLength);
}

SInteger CCSInt64ToBytes(int64_t n, CBool bigEndianEncode, uint8_t * _Nonnull outputBuffer, SInteger bufferLength) {
    return __CCIntegerToBytes(&n, 8, __CCIntegerBytesSwap(bigEndianEncode), outputBuffer, bufferLength);
}
SInteger CCSInt32ToBytes(int32_t n, CBool bigEndianEncode, uint8_t * _Nonnull outputBuffer, SInteger bufferLength) {
    return __CCIntegerToBytes(&n, 4, __CCIntegerBytesSwap(bigEndianEncode), outputBuffer, bufferLength);
}
SInteger CCSInt16ToBytes(int16_t n, CBool bigEndianEncode, uint8_t * _Nonnull outputBuffer, SInteger bufferLength) {
    return __CCIntegerToBytes(&n, 2, __CCIntegerBytesSwap(bigEndianEncode), outputBuffer, bufferLength);
}


static inline SInteger __CCBytesToInteger(const uint8_t * _Nonnull bytes, SInteger bytesLength, void * _Nonnull n, SInteger len, CBool swap) {
    if (bytesLength < len) {
        return -1;
    }
    assert(n);
    assert(bytes);
    memcpy(n, bytes, len);
    if (len > 1 && swap) {
        uint8_t * v = (uint8_t *)n;
        SInteger upper = len / 2;
        for (SInteger i=0; i<upper; i++) {
            uint8_t tmp = v[i];
            v[i] = v[len-1-i];
            v[len-1-i] = tmp;
        }
    }
    return len;
}


SInteger CCBytesToUInt64(const uint8_t * _Nonnull bytes, SInteger length, CBool bigEndianEncode, uint64_t * _Nonnull n) {
    return __CCBytesToInteger(bytes, length, n, 8, __CCIntegerBytesSwap(bigEndianEncode));
}
SInteger CCBytesToUInt32(const uint8_t * _Nonnull bytes, SInteger length, CBool bigEndianEncode, uint32_t * _Nonnull n) {
    return __CCBytesToInteger(bytes, length, n, 4, __CCIntegerBytesSwap(bigEndianEncode));
}
SInteger CCBytesToUInt16(const uint8_t * _Nonnull bytes, SInteger length, CBool bigEndianEncode, uint16_t * _Nonnull n) {
    return __CCBytesToInteger(bytes, length, n, 2, __CCIntegerBytesSwap(bigEndianEncode));
}

SInteger CCBytesToSInt64(const uint8_t * _Nonnull bytes, SInteger length, CBool bigEndianEncode, int64_t * _Nonnull n) {
    return __CCBytesToInteger(bytes, length, n, 8, __CCIntegerBytesSwap(bigEndianEncode));
}
SInteger CCBytesToSInt32(const uint8_t * _Nonnull bytes, SInteger length, CBool bigEndianEncode, int32_t * _Nonnull n) {
    return __CCBytesToInteger(bytes, length, n, 4, __CCIntegerBytesSwap(bigEndianEncode));
}
SInteger CCBytesToSInt16(const uint8_t * _Nonnull bytes, SInteger length, CBool bigEndianEncode, int16_t * _Nonnull n) {
    return __CCBytesToInteger(bytes, length, n, 2, __CCIntegerBytesSwap(bigEndianEncode));
}
