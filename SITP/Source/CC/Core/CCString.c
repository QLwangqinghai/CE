//
//  CCString.c
//  CCollection
//
//  Created by vector on 2019/7/4.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCString.h"


/*
32   [16, 32, 64, 128]
8 2 6
8 2 22
 54
 118
64 [32, 64, 128, 256]
16 2 14
16 2 46
16 2 110
16 2 238
*/

#pragma pack(push,1)

typedef struct __CCString {
    CCStringBase base;
    CCUInt8 content[0];
} CCString;


typedef struct {
#if CCBuild64Bit
    CCUInt hashed: 1;
    CCUInt freePtr: 1;
    CCUInt length: 62;
#else
    CCUInt hashed: 1;
    CCUInt freePtr: 1;
    CCUInt length: 30;
#endif
    const CCU16Char * _Nonnull chars;
} CCStringContent;

#if CCBuild64Bit
typedef struct {
    CCUInt16 hashed: 1;
    CCUInt16 length: 15;
    CCUInt16 content[7];
} CCStringTaggedContent32;
typedef struct {
    CCUInt16 hashed: 1;
    CCUInt16 length: 15;
    CCUInt16 content[23];
} CCStringTaggedContent64;
typedef struct {
    CCUInt16 hashed: 1;
    CCUInt16 length: 15;
    CCUInt16 content[55];
} CCStringTaggedContent128;
#else
typedef struct {
    CCUInt16 hashed: 1;
    CCUInt16 length: 15;
    CCUInt16 content[3];
} CCStringTaggedContent16;
typedef struct {
    CCUInt16 hashed: 1;
    CCUInt16 length: 15;
    CCUInt16 content[11];
} CCStringTaggedContent32;
typedef struct {
    CCUInt16 hashed: 1;
    CCUInt16 length: 15;
    CCUInt16 content[27];
} CCStringTaggedContent64;
#endif

#pragma pack(pop)





#define HashEverythingLimit 96

#define HashNextFourUniChars(accessStart, accessEnd, pointer) \
    {result = result * 67503105 + (((accessStart 0 accessEnd) * 257  + (accessStart 1 accessEnd)) * 257  + (accessStart 2 accessEnd)) * 257 + (accessStart 3 accessEnd); pointer += 4;}

#define HashNextUniChar(accessStart, accessEnd, pointer) \
    {result = result * 257 + (accessStart 0 accessEnd); pointer++;}


/* In this function, actualLen is the length of the original string; but len is the number of characters in buffer. The buffer is expected to contain the parts of the string relevant to hashing.
*/
CCHashCode __CCHashCharacters(const CCU16Char * _Nullable uContents, CCUInt len, CCUInt actualLen) {
    if (actualLen > 0) {
        assert(uContents);
    } else {
        return 0;
    }
    CCHashCode result = actualLen;
    if (len <= HashEverythingLimit) {
        const CCU16Char *end4 = uContents + (len & ~3);
        const CCU16Char *end = uContents + len;
        while (uContents < end4) HashNextFourUniChars(uContents[, ], uContents);     // First count in fours
        while (uContents < end) HashNextUniChar(uContents[, ], uContents);        // Then for the last <4 chars, count in ones...
    } else {
        const CCU16Char *contents, *end;
        contents = uContents;
        end = contents + 32;
        while (contents < end)  HashNextFourUniChars(contents[, ], contents);
        contents = uContents + (len >> 1) - 16;
        end = contents + 32;
        while (contents < end) HashNextFourUniChars(contents[, ], contents);
        end = uContents + len;
        contents = end - 32;
        while (contents < end) HashNextFourUniChars(contents[, ], contents);
    }
    return result + (result << (actualLen & 31));
}

//Boolean __CFStringDecodeByteStream3(const uint8_t *bytes, CFIndex len, CFStringEncoding encoding, Boolean alwaysUnicode, CFVarWidthCharBuffer *buffer, Boolean *useClientsMemoryPtr, UInt32 converterFlags) {
//    CFIndex idx;
//    const uint8_t *chars = (const uint8_t *)bytes;
//    const uint8_t *end = chars + len;
//    Boolean result = TRUE;
//
//    if (useClientsMemoryPtr) *useClientsMemoryPtr = false;
//
//    buffer->isASCII = !alwaysUnicode;
//    buffer->shouldFreeChars = false;
//    buffer->numChars = 0;
//
//    if (0 == len) return true;
//
//    buffer->allocator = (buffer->allocator ? buffer->allocator : __CFGetDefaultAllocator());
//
//    if ((encoding == kCFStringEncodingUTF16) || (encoding == kCFStringEncodingUTF16BE) || (encoding == kCFStringEncodingUTF16LE)) { // UTF-16
//        const UTF16Char *src = (const UTF16Char *)bytes;
//        const UTF16Char *limit = src + (len / sizeof(UTF16Char)); // <rdar://problem/7854378> avoiding odd len issue
//        bool swap = false;
//
//        if (kCFStringEncodingUTF16 == encoding) {
//            UTF16Char bom = ((*src == 0xFFFE) || (*src == 0xFEFF) ? *(src++) : 0);
//
//#if __CF_BIG_ENDIAN__
//            if (bom == 0xFFFE) swap = true;
//#else
//            if (bom != 0xFEFF) swap = true;
//#endif
//            if (bom) useClientsMemoryPtr = NULL;
//        } else {
//#if __CF_BIG_ENDIAN__
//            if (kCFStringEncodingUTF16LE == encoding) swap = true;
//#else
//            if (kCFStringEncodingUTF16BE == encoding) swap = true;
//#endif
//        }
//
//        buffer->numChars = limit - src;
//
//        if (useClientsMemoryPtr && !swap) { // If the caller is ready to deal with no-copy situation, and the situation is possible, indicate it...
//            *useClientsMemoryPtr = true;
//            buffer->chars.unicode = (UniChar *)src;
//            buffer->isASCII = false;
//        } else {
//            if (buffer->isASCII) {    // Let's see if we can reduce the Unicode down to ASCII...
//                const UTF16Char *characters = src;
//                UTF16Char mask = (swap ? 0x80FF : 0xFF80);
//    
//                while (characters < limit) {
//                    if (*(characters++) & mask) {
//                        buffer->isASCII = false;
//                        break;
//                    }
//                }
//            }
//    
//            if (buffer->isASCII) {
//                uint8_t *dst;
//                if (NULL == buffer->chars.ascii) { // we never reallocate when buffer is supplied
//                    if (buffer->numChars > MAX_LOCAL_CHARS) {
//                        buffer->chars.ascii = (UInt8 *)CFAllocatorAllocate(buffer->allocator, (buffer->numChars * sizeof(uint8_t)), 0);
//            if (!buffer->chars.ascii) goto memoryErrorExit;
//                        buffer->shouldFreeChars = true;
//                    } else {
//                        buffer->chars.ascii = (uint8_t *)buffer->localBuffer;
//                    }
//                }
//                dst = buffer->chars.ascii;
//    
//                if (swap) {
//                    while (src < limit) *(dst++) = (*(src++) >> 8);
//                } else {
//                    while (src < limit) *(dst++) = (uint8_t)*(src++);
//                }
//            } else {
//                UTF16Char *dst;
//
//                if (NULL == buffer->chars.unicode) { // we never reallocate when buffer is supplied
//                    if (buffer->numChars > MAX_LOCAL_UNICHARS) {
//                        buffer->chars.unicode = (UniChar *)CFAllocatorAllocate(buffer->allocator, (buffer->numChars * sizeof(UTF16Char)), 0);
//            if (!buffer->chars.unicode) goto memoryErrorExit;
//                        buffer->shouldFreeChars = true;
//                    } else {
//                        buffer->chars.unicode = (UTF16Char *)buffer->localBuffer;
//                    }
//                }
//                dst = buffer->chars.unicode;
//
//                if (swap) {
//                    while (src < limit) *(dst++) = CFSwapInt16(*(src++));
//                } else {
//                    memmove(dst, src, buffer->numChars * sizeof(UTF16Char));
//                }
//            }
//        }
//    } else if ((encoding == kCFStringEncodingUTF32) || (encoding == kCFStringEncodingUTF32BE) || (encoding == kCFStringEncodingUTF32LE)) {
//        const UTF32Char *src = (const UTF32Char *)bytes;
//        const UTF32Char *limit =  src + (len / sizeof(UTF32Char)); // <rdar://problem/7854378> avoiding odd len issue
//        bool swap = false;
//        static bool strictUTF32 = (bool)-1;
//
//        if ((bool)-1 == strictUTF32) strictUTF32 = (1 != 0);
//
//        if (kCFStringEncodingUTF32 == encoding) {
//            UTF32Char bom = ((*src == 0xFFFE0000) || (*src == 0x0000FEFF) ? *(src++) : 0);
//
//#if __CF_BIG_ENDIAN__
//            if (bom == 0xFFFE0000) swap = true;
//#else
//            if (bom != 0x0000FEFF) swap = true;
//#endif
//        } else {
//#if __CF_BIG_ENDIAN__
//            if (kCFStringEncodingUTF32LE == encoding) swap = true;
//#else
//            if (kCFStringEncodingUTF32BE == encoding) swap = true;
//#endif
//        }
//
//        buffer->numChars = limit - src;
//
//        {
//            // Let's see if we have non-ASCII or non-BMP
//            const UTF32Char *characters = src;
//            UTF32Char asciiMask = (swap ? 0x80FFFFFF : 0xFFFFFF80);
//            UTF32Char bmpMask = (swap ? 0x0000FFFF : 0xFFFF0000);
//    
//            while (characters < limit) {
//                if (*characters & asciiMask) {
//                    buffer->isASCII = false;
//                    if (*characters & bmpMask) {
//                        if (strictUTF32 && ((swap ? (UTF32Char)CFSwapInt32(*characters) : *characters) > 0x10FFFF)) return false; // outside of Unicode Scaler Value. Haven't allocated buffer, yet.
//                        ++(buffer->numChars);
//                    }
//                }
//                ++characters;
//            }
//        }
//    
//        if (buffer->isASCII) {
//            uint8_t *dst;
//            if (NULL == buffer->chars.ascii) { // we never reallocate when buffer is supplied
//                if (buffer->numChars > MAX_LOCAL_CHARS) {
//                    buffer->chars.ascii = (UInt8 *)CFAllocatorAllocate(buffer->allocator, (buffer->numChars * sizeof(uint8_t)), 0);
//            if (!buffer->chars.ascii) goto memoryErrorExit;
//                    buffer->shouldFreeChars = true;
//                } else {
//                    buffer->chars.ascii = (uint8_t *)buffer->localBuffer;
//                }
//            }
//            dst = buffer->chars.ascii;
//
//            if (swap) {
//                while (src < limit) *(dst++) = (*(src++) >> 24);
//            } else {
//                while (src < limit) *(dst++) = *(src++);
//            }
//        } else {
//            if (NULL == buffer->chars.unicode) { // we never reallocate when buffer is supplied
//                if (buffer->numChars > MAX_LOCAL_UNICHARS) {
//                    buffer->chars.unicode = (UniChar *)CFAllocatorAllocate(buffer->allocator, (buffer->numChars * sizeof(UTF16Char)), 0);
//            if (!buffer->chars.unicode) goto memoryErrorExit;
//                    buffer->shouldFreeChars = true;
//                } else {
//                    buffer->chars.unicode = (UTF16Char *)buffer->localBuffer;
//                }
//            }
//            result = (CFUniCharFromUTF32(src, limit - src, buffer->chars.unicode, (strictUTF32 ? false : true), __CF_BIG_ENDIAN__ ? !swap : swap) ? TRUE : FALSE);
//        }
//    } else if (kCFStringEncodingUTF8 == encoding) {
//        if ((len >= 3) && (chars[0] == 0xef) && (chars[1] == 0xbb) && (chars[2] == 0xbf)) {    // If UTF8 BOM, skip
//            chars += 3;
//            len -= 3;
//            if (0 == len) return true;
//        }
//        if (buffer->isASCII) {
//            for (idx = 0; idx < len; idx++) {
//                if (128 <= chars[idx]) {
//                    buffer->isASCII = false;
//                    break;
//                }
//            }
//        }
//        if (buffer->isASCII) {
//            buffer->numChars = len;
//            buffer->shouldFreeChars = !buffer->chars.ascii && (len <= MAX_LOCAL_CHARS) ? false : true;
//            buffer->chars.ascii = (buffer->chars.ascii ? buffer->chars.ascii : (len <= MAX_LOCAL_CHARS) ? (uint8_t *)buffer->localBuffer : (UInt8 *)CFAllocatorAllocate(buffer->allocator, len * sizeof(uint8_t), 0));
//        if (!buffer->chars.ascii) goto memoryErrorExit;
//            memmove(buffer->chars.ascii, chars, len * sizeof(uint8_t));
//        } else {
//            CFIndex numDone;
//            static CFStringEncodingToUnicodeProc __CFFromUTF8 = NULL;
//            
//            if (!__CFFromUTF8) {
//                const CFStringEncodingConverter *converter = CFStringEncodingGetConverter(kCFStringEncodingUTF8);
//                __CFFromUTF8 = converter->toUnicode.standard;
//            }
//            
//            buffer->shouldFreeChars = !buffer->chars.unicode && (len <= MAX_LOCAL_UNICHARS) ? false : true;
//            buffer->chars.unicode = (buffer->chars.unicode ? buffer->chars.unicode : (len <= MAX_LOCAL_UNICHARS) ? (UniChar *)buffer->localBuffer : (UniChar *)CFAllocatorAllocate(buffer->allocator, len * sizeof(UniChar), 0));
//        if (!buffer->chars.unicode) goto memoryErrorExit;
//            buffer->numChars = 0;
//            while (chars < end) {
//                numDone = 0;
//                chars += __CFFromUTF8(converterFlags, chars, end - chars, &(buffer->chars.unicode[buffer->numChars]), len - buffer->numChars, &numDone);
//                
//                if (0 == numDone) {
//                    result = FALSE;
//                    break;
//                }
//                buffer->numChars += numDone;
//            }
//        }
//    } else if (kCFStringEncodingNonLossyASCII == encoding) {
//        UTF16Char currentValue = 0;
//        uint8_t character;
//        int8_t mode = __NSNonLossyASCIIMode;
//        
//        buffer->isASCII = false;
//        buffer->shouldFreeChars = !buffer->chars.unicode && (len <= MAX_LOCAL_UNICHARS) ? false : true;
//        buffer->chars.unicode = (buffer->chars.unicode ? buffer->chars.unicode : (len <= MAX_LOCAL_UNICHARS) ? (UniChar *)buffer->localBuffer : (UniChar *)CFAllocatorAllocate(buffer->allocator, len * sizeof(UniChar), 0));
//    if (!buffer->chars.unicode) goto memoryErrorExit;
//        buffer->numChars = 0;
//        
//        while (chars < end) {
//            character = (*chars++);
//            
//            switch (mode) {
//                case __NSNonLossyASCIIMode:
//                    if (character == '\\') {
//                        mode = __NSNonLossyBackslashMode;
//                    } else if (character < 0x80) {
//                        currentValue = character;
//                    } else {
//                        mode = __NSNonLossyErrorMode;
//                    }
//                    break;
//                    
//                    case __NSNonLossyBackslashMode:
//                    if ((character == 'U') || (character == 'u')) {
//                        mode = __NSNonLossyHexInitialMode;
//                        currentValue = 0;
//                    } else if ((character >= '0') && (character <= '9')) {
//                        mode = __NSNonLossyOctalInitialMode;
//                        currentValue = character - '0';
//                    } else if (character == '\\') {
//                        mode = __NSNonLossyASCIIMode;
//                        currentValue = character;
//                    } else {
//                        mode = __NSNonLossyErrorMode;
//                    }
//                    break;
//                    
//                    default:
//                    if (mode < __NSNonLossyHexFinalMode) {
//                        if ((character >= '0') && (character <= '9')) {
//                            currentValue = (currentValue << 4) | (character - '0');
//                            if (++mode == __NSNonLossyHexFinalMode) mode = __NSNonLossyASCIIMode;
//                        } else {
//                            if (character >= 'a') character -= ('a' - 'A');
//                            if ((character >= 'A') && (character <= 'F')) {
//                                currentValue = (currentValue << 4) | ((character - 'A') + 10);
//                                if (++mode == __NSNonLossyHexFinalMode) mode = __NSNonLossyASCIIMode;
//                            } else {
//                                mode = __NSNonLossyErrorMode;
//                            }
//                        }
//                    } else {
//                        if ((character >= '0') && (character <= '9')) {
//                            currentValue = (currentValue << 3) | (character - '0');
//                            if (++mode == __NSNonLossyOctalFinalMode) mode = __NSNonLossyASCIIMode;
//                        } else {
//                            mode = __NSNonLossyErrorMode;
//                        }
//                    }
//                    break;
//            }
//            
//            if (mode == __NSNonLossyASCIIMode) {
//                buffer->chars.unicode[buffer->numChars++] = currentValue;
//            } else if (mode == __NSNonLossyErrorMode) {
//                break;
//            }
//        }
//        result = ((mode == __NSNonLossyASCIIMode) ? YES : NO);
//    } else {
//        const CFStringEncodingConverter *converter = CFStringEncodingGetConverter(encoding);
//        
//        if (!converter) return false;
//        
//        Boolean isASCIISuperset = __CFStringEncodingIsSupersetOfASCII(encoding);
//        
//        if (!isASCIISuperset) buffer->isASCII = false;
//        
//        if (buffer->isASCII) {
//            for (idx = 0; idx < len; idx++) {
//                if (128 <= chars[idx]) {
//                    buffer->isASCII = false;
//                    break;
//                }
//            }
//        }
//        
//        if (converter->encodingClass == kCFStringEncodingConverterCheapEightBit) {
//            if (buffer->isASCII) {
//                buffer->numChars = len;
//                buffer->shouldFreeChars = !buffer->chars.ascii && (len <= MAX_LOCAL_CHARS) ? false : true;
//                buffer->chars.ascii = (buffer->chars.ascii ? buffer->chars.ascii : (len <= MAX_LOCAL_CHARS) ? (uint8_t *)buffer->localBuffer : (UInt8 *)CFAllocatorAllocate(buffer->allocator, len * sizeof(uint8_t), 0));
//        if (!buffer->chars.ascii) goto memoryErrorExit;
//                memmove(buffer->chars.ascii, chars, len * sizeof(uint8_t));
//            } else {
//                buffer->shouldFreeChars = !buffer->chars.unicode && (len <= MAX_LOCAL_UNICHARS) ? false : true;
//                buffer->chars.unicode = (buffer->chars.unicode ? buffer->chars.unicode : (len <= MAX_LOCAL_UNICHARS) ? (UniChar *)buffer->localBuffer : (UniChar *)CFAllocatorAllocate(buffer->allocator, len * sizeof(UniChar), 0));
//        if (!buffer->chars.unicode) goto memoryErrorExit;
//                buffer->numChars = len;
//                if (kCFStringEncodingASCII == encoding || kCFStringEncodingISOLatin1 == encoding) {
//                    for (idx = 0; idx < len; idx++) buffer->chars.unicode[idx] = (UniChar)chars[idx];
//                } else {
//                    for (idx = 0; idx < len; idx++) {
//                        if (chars[idx] < 0x80 && isASCIISuperset) {
//                            buffer->chars.unicode[idx] = (UniChar)chars[idx];
//                        } else if (!converter->toUnicode.cheapEightBit(0, chars[idx], buffer->chars.unicode + idx)) {
//                            result = FALSE;
//                            break;
//                        }
//                    }
//                }
//            }
//        } else {
//            if (buffer->isASCII) {
//                buffer->numChars = len;
//                buffer->shouldFreeChars = !buffer->chars.ascii && (len <= MAX_LOCAL_CHARS) ? false : true;
//                buffer->chars.ascii = (buffer->chars.ascii ? buffer->chars.ascii : (len <= MAX_LOCAL_CHARS) ? (uint8_t *)buffer->localBuffer : (UInt8 *)CFAllocatorAllocate(buffer->allocator, len * sizeof(uint8_t), 0));
//        if (!buffer->chars.ascii) goto memoryErrorExit;
//                memmove(buffer->chars.ascii, chars, len * sizeof(uint8_t));
//            } else {
//                CFIndex guessedLength = CFStringEncodingCharLengthForBytes(encoding, 0, bytes, len);
//                static UInt32 lossyFlag = (UInt32)-1;
//                
//                buffer->shouldFreeChars = !buffer->chars.unicode && (guessedLength <= MAX_LOCAL_UNICHARS) ? false : true;
//                buffer->chars.unicode = (buffer->chars.unicode ? buffer->chars.unicode : (guessedLength <= MAX_LOCAL_UNICHARS) ? (UniChar *)buffer->localBuffer : (UniChar *)CFAllocatorAllocate(buffer->allocator, guessedLength * sizeof(UniChar), 0));
//        if (!buffer->chars.unicode) goto memoryErrorExit;
//                
//                if (lossyFlag == (UInt32)-1) lossyFlag = 0;
//                
//                if (CFStringEncodingBytesToUnicode(encoding, lossyFlag|__CFGetASCIICompatibleFlag(), bytes, len, NULL, buffer->chars.unicode, (guessedLength > MAX_LOCAL_UNICHARS ? guessedLength : MAX_LOCAL_UNICHARS), &(buffer->numChars))) result = FALSE;
//            }
//        }
//    }
//
//    if (FALSE == result) {
//memoryErrorExit:    // Added for <rdar://problem/6581621>, but it's not clear whether an exception would be a better option
//    result = FALSE;    // In case we come here from a goto
//        if (buffer->shouldFreeChars && buffer->chars.unicode) CFAllocatorDeallocate(buffer->allocator, buffer->chars.unicode);
//        buffer->isASCII = !alwaysUnicode;
//        buffer->shouldFreeChars = false;
//        buffer->chars.ascii = NULL;
//        buffer->numChars = 0;
//    }
//    return result;
//}


/*
 32   [16, 32, 64]
 4 2 10
 4 2 26
 
 64 [32, 64, 128]
 8 2 6
 8 2 22
 8 2 54

 
 
 8 + 4(CCHashCode) + 4    2     16
 8 + 4(CCHashCode) + 20   10    32
 8 + 4(CCHashCode) + 52   26    64
 8 + 4(CCHashCode) + 4(Ptr)     16
 
 64
 16 + 8(CCHashCode) + 8    4     32
 16 + 8(CCHashCode) + 40   20    64
 16 + 8(CCHashCode) + 8(Ptr)     32
 */

//CCStringRef CCStringAlloc(CCUInt length) {
//
//}

//http://blog.sina.com.cn/s/blog_63733daa0100qe9n.html

static int CCUInt32LeastSignificant1111(uint8_t c) {
    if (c >= 0xE0) {
        if (c >= 0xF0) {
            if (c == 0xFF) {
                return -1;
            } else {
                return 4;
            }
        } else {
            return 3;
        }
    } else {
        if (c < 0x80) {
            return 1;
        } else {
            if (c >= 0xC0) {
                return 2;
            } else {
                return -1;
            }
        }
    }
}

static const CCU16Char __CCU16CharZero = 0;
static const CCConstantString __CCEmptyString = {
    .base = {
        .base = ATOMIC_VAR_INIT(CCConstantStringRcInfo),
        .hashCode = 0,
    },
    .length = 0,
    .chars = & __CCU16CharZero,
};

CCStringRef _Nonnull CCEmptyString = (CCStringRef)&__CCEmptyString;
