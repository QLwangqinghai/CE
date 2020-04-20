//
//  CCBase.h
//  CCollection
//
//  Created by vector on 2019/6/26.
//  Copyright © 2019 angfung. All rights reserved.
//

#include "CCBase.h"
#include <pthread.h>


static inline void _CCMemoryQuickCopy(uint64_t * _Nonnull dst, const uint64_t * _Nonnull src, size_t count) {
    static size_t countQuickMask = ~((size_t)(0x7));

    if ((dst + count) <= src || (src + count) <= dst) {//没有重叠区域
        uint64_t * to = dst;
        const uint64_t * from = src;
        uint64_t * end = to + (count & countQuickMask);

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
        end = (uint64_t *)dst + count;
        while (to < end) {
            *to = *from;
            to ++;
            from ++;
        }
    } else {
        uint64_t * end = dst;
        end = end + (count & countQuickMask);
        uint64_t * to = end + count;
        const uint64_t * from = src;
        from = from + count;
        
        end = (uint64_t *)dst + count;
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
    if (offset == (to & 0x7) && BUILD_TARGET_RT_64_BIT) {
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
            _CCMemoryQuickCopy((uint64_t *)dst8, (const uint64_t *)src8, count);
        }
        remain = remain & 0x7;
        if (remain > 0) {
            memcpy(dst8 + (count * 8), src8 + (count * 8), remain);
        }
    } else {
        memcpy(dst, src, size);
    }
}

void _CCMemorySetUInt16(void * _Nonnull dst, uint16_t value, size_t valueCount) {
    
}
void _CCMemorySetUInt32(void * _Nonnull dst, uint32_t value, size_t valueCount) {
    
}

void CCMemorySetUInt8(void * _Nonnull dst, uint8_t value, size_t valueCount) {
    size_t count = valueCount;
    uint8_t * ptr = dst;
    if (count > 0) {
        uintptr_t tmp = (uintptr_t)ptr;
        if ((tmp & 0x1) != 0) {
            ptr[0] = value;
            ptr ++;
            count --;
        }
    }
    if (count % 2 != 0) {
        ptr[count - 1] = value;
    }
    uint16_t v = value;
    v = (v << 8) + value;
    _CCMemorySetUInt16(ptr, v, value / 2);
}
void CCMemorySetUInt16(void * _Nonnull dst, uint16_t value, size_t valueCount) {
    
}
void CCMemorySetUInt32(void * _Nonnull dst, uint32_t value, size_t valueCount) {
    
}



void CCMemorySet(void * _Nonnull dst, uint32_t value, size_t count) {
    if (0 == count) {
        return;
    }
    assert(dst);
#if BUILD_TARGET_RT_64_BIT
    uint64_t v = value;
    v = (v << 32) + value;

    uintptr_t tmp = (uintptr_t)dst;
    size_t offset = (tmp & 0x7);

    if (offset == 0) {
        size_t r = count;
        uint64_t * to = dst;
        while (r > 1) {
            *to = v;
            to ++;
            r -= 2;
        }
        if (r == 1) {
            uint32_t * to32 = (uint32_t *)to;
            *to32 = value;
            r --;
        }
    } else if (offset == 4) {
        size_t r = count;
        uint32_t * to32 = dst;
        *to32 = value;
        to32 ++;
        r --;
        uint64_t * to = (uint64_t *)to32;
        while (r > 1) {
            *to = v;
            to ++;
            r -= 2;
        }
        if (r == 1) {
            uint32_t * to32 = (uint32_t *)to;
            *to32 = value;
            r --;
        }
    } else
#endif
    {
        size_t r = count;
        uint32_t * to = dst;
        while (r > 0) {
            *to = value;
            to ++;
            r --;
        }
    }
}

//const CCType CCTypeArray = 0x1;
//const CCType CCTypeMutableArray = 0x1 | CCTypeMutableMask;




#pragma mark - thread key-specific

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


#if __APPLE__

#include <mach-o/dyld.h>
int CCGetExecutablePath(char * _Nonnull buf, uint32_t * _Nonnull bufsize) {
    return _NSGetExecutablePath(buf, bufsize);
}

#else

#include <unistd.h>
int CCGetExecutablePath(char* buf, uint32_t * bufsize) {
    if (NULL == buf || NULL == bufsize) {
        return -1;
    }
    ssize_t ret = readlink("/proc/self/exe", path, (size_t)(*bufsize));
    if (ret <= 0) {
        return -1;
    } else {
        *bufsize = (uint32_t)ret;
    }
}
#endif
#if defined(__APPLE__)

#include <sys/sysctl.h>
size_t CCGetCachelineSize(void) {
    
    int name[] = {CTL_HW,HW_CACHELINE};
    size_t lineSize = 0;
    size_t sizeOfLineSize = sizeof(lineSize);

    int code = sysctl(name, 2, &lineSize, &sizeOfLineSize, NULL, 0); // getting size of answer
    if (0 == code) {
        return (int32_t)lineSize;
    } else {
        return 64;
    }
}
#elif defined(_WIN32)

#include <stdlib.h>
#include <windows.h>
size_t CCGetCachelineSize() {
    size_t line_size = 0;
    DWORD buffer_size = 0;
    DWORD i = 0;
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION * buffer = 0;

    GetLogicalProcessorInformation(0, &buffer_size);
    buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *)malloc(buffer_size);
    GetLogicalProcessorInformation(&buffer[0], &buffer_size);

    for (i = 0; i != buffer_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
        if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1) {
            line_size = buffer[i].Cache.LineSize;
            break;
        }
    }

    free(buffer);
    return line_size;
}

#elif defined(linux)

#include <stdio.h>
size_t CCGetCachelineSize() {
    FILE * p = 0;
    p = fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r");
    unsigned int i = 0;
    if (p) {
        fscanf(p, "%d", &i);
        fclose(p);
    }
    return i;
}

#else
    #error Unrecognized platform
#endif


//uint32_t CCCpuCount(void) {
//
//    #include <sys/sysinfo.h>
//
//    int main(int argc, char* argv[])
//    {
//        printf("cpu total: %d\n", get_nprocs_conf());
//        printf("cpu num: %d\n", get_nprocs());
//        return 0;
//
//    }
//}


uint32_t CCActiveProcessorCount() {
    int32_t pcnt;
#if BUILD_TARGET_OS_WINDOWS
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    DWORD_PTR activeProcessorMask = sysInfo.dwActiveProcessorMask;
    // assumes sizeof(DWORD_PTR) is 64 bits or less
    uint64_t v = activeProcessorMask;
    v = v - ((v >> 1) & 0x5555555555555555ULL);
    v = (v & 0x3333333333333333ULL) + ((v >> 2) & 0x3333333333333333ULL);
    v = (v + (v >> 4)) & 0xf0f0f0f0f0f0f0fULL;
    pcnt = (v * 0x0101010101010101ULL) >> ((sizeof(v) - 1) * 8);
#elif BUILD_TARGET_OS_DARWIN
    int32_t mib[] = {CTL_HW, HW_AVAILCPU};
    size_t len = sizeof(pcnt);
    int32_t result = sysctl(mib, sizeof(mib) / sizeof(int32_t), &pcnt, &len, NULL, 0);
    if (result != 0) {
        pcnt = 0;
    }
#elif BUILD_TARGET_OS_LINUX
    pcnt = sysconf(_SC_NPROCESSORS_ONLN);
#else
    // Assume the worst
    pcnt = 1;
#endif
    return pcnt;
}
