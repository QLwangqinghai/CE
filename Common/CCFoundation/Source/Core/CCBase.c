//
//  CCBase.h
//  CCollection
//
//  Created by vector on 2019/6/26.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCBase.h"
#include <pthread.h>


static inline void _CCMemoryQuickCopy(CCUInt64 * _Nonnull dst, const CCUInt64 * _Nonnull src, size_t count) {
    static size_t countQuickMask = ~((size_t)(0x7));

    if ((dst + count) <= src || (src + count) <= dst) {//没有重叠区域
        CCUInt64 * to = dst;
        const CCUInt64 * from = src;
        CCUInt64 * end = to + (count & countQuickMask);

        while (to < end) {
            *to = *from;
            to ++;
            from ++;
            *to = *from;
            to ++;
            from ++;
            *to = *from;
            to ++;
            from ++;
            *to = *from;
            to ++;
            from ++;
            *to = *from;
            to ++;
            from ++;
            *to = *from;
            to ++;
            from ++;
            *to = *from;
            to ++;
            from ++;
            *to = *from;
            to ++;
            from ++;
        }
        end = (CCUInt64 *)dst + count;
        while (to < end) {
            *to = *from;
            to ++;
            from ++;
        }
    } else {
        CCUInt64 * end = dst;
        end = end + (count & countQuickMask);
        CCUInt64 * to = end + count;
        const CCUInt64 * from = src;
        from = from + count;
        
        end = (CCUInt64 *)dst + count;
        while (to > end) {
            to --;
            from --;
            *to = *from;
        }
        
        end = dst;
        while (to > end) {
            to --;
            from --;
            *to = *from;
            to --;
            from --;
            *to = *from;
            to --;
            from --;
            *to = *from;
            to --;
            from --;
            *to = *from;
            to --;
            from --;
            *to = *from;
            to --;
            from --;
            *to = *from;
            to --;
            from --;
            *to = *from;
            to --;
            from --;
            *to = *from;
        }
    }
}

void CCMemoryCopy(void * _Nonnull dst, const void * _Nonnull src, size_t size) {
    assert(dst);
    assert(src);
    assert(size >= 0);
    uintptr_t from = (uintptr_t)dst;
    uintptr_t to = (uintptr_t)src;

    size_t offset = (from & 0x7);
    if (offset == (to & 0x7) && CCBuild64Bit) {
        uint8_t * dst8 = dst;
        const uint8_t * src8 = src;
        if (offset > 0) {
            memcpy(dst, src, offset);
            dst8 += offset;
            src8 += offset;
        }
        size_t remain = (size - offset);
        size_t count = remain / 8;
        if (count > 0) {
            _CCMemoryQuickCopy((CCUInt64 *)dst8, (const CCUInt64 *)src8, count);
        }
        remain = remain & 0x7;
        if (remain > 0) {
            memcpy(dst8 + (count * 8), src8 + (count * 8), remain);
        }
    } else {
        memcpy(dst, src, size);
    }
}



//const CCType CCTypeArray = 0x1;
//const CCType CCTypeMutableArray = 0x1 | CCTypeMutableMask;




#pragma mark - thread key-specific

static size_t __CCPageSizes[8] = {};

#if !defined(PAGE_SIZE)
#define PAGE_SIZE 4096
#endif

//void __CCPageSizesOnceBlockFunc(void) {
//    size_t pageSize = 4096;
//#if defined(PAGE_SIZE)
//    if (PAGE_SIZE > pageSize) {
//        pageSize = PAGE_SIZE;
//    }
//#endif
//}
//size_t * _Nonnull CESharedKey(void) {
//    static pthread_once_t token = PTHREAD_ONCE_INIT;
//    pthread_once(&token,&__CCPageSizesOnceBlockFunc);
//    assert(__CCPageSizes[0] > 0);
//    return __CCPageSizes;
//}
//
//CERunLoop_s * _Nullable CEGetCurrentLoop(void) {
//    return (CERunLoop_s *)pthread_getspecific(CESharedKey());
//
//}
//void CESetCurrentLoop(CERunLoop_s * _Nullable loop) {
//    CERunLoop_s * old = CEGetCurrentLoop();
//    if (NULL != old && NULL != loop & old != loop) {
//        fprintf(stderr, "SetCurrentLoop error\n");
//        fflush(stderr);
//        abort();
//    }
//    int result = pthread_setspecific(CESharedKey(), loop);
//    assert(result == 0);
//}

